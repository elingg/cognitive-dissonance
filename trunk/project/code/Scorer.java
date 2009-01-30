/* CS221 Fall 2006. */
/* 
 *  Scoring function.  Takes in two xml files, a truth file and a
 *  prediction file. 
 *
 *  Written by: Konstantin Davydov, with some code taken from Fall 2006
 *  CS145 project
 */


import java.io.*;
import java.text.*;
import java.util.*;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.FactoryConfigurationError;
import javax.xml.parsers.ParserConfigurationException;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.Element;
import org.w3c.dom.Text;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.xml.sax.ErrorHandler;


class Scorer {
    
    static final String columnSeparator = "|*|";
    static DocumentBuilder builder;
    static boolean verbose = false;    
    
    static final String[] labels = {
	"mug",
	"clock",
	"stapler",
	"scissors",
	"keyboard",
    };


    static final String[] typeName = {
	"none",
	"Element",
	"Attr",
	"Text",
	"CDATA",
	"EntityRef",
	"Entity",
	"ProcInstr",
	"Comment",
	"Document",
	"DocType",
	"DocFragment",
	"Notation",
    };
    
    /* Non-recursive (NR) version of Node.getElementsByTagName(...) */
    static Element[] getElementsByTagNameNR(Element e, String tagName) {
	Vector< Element > elements = new Vector< Element >();
	Node child = e.getFirstChild();
	while (child != null) {
	    if (child instanceof Element && child.getNodeName().equals(tagName))
		{
		    elements.add( (Element)child );
		}
	    child = child.getNextSibling();
	}
	Element[] result = new Element[elements.size()];
	elements.copyInto(result);
	return result;
    }
    
    /* Returns the first subelement of e matching the given tagName, or
     * null if one does not exist. */
    static Element getElementByTagNameNR(Element e, String tagName) {
	Node child = e.getFirstChild();
	while (child != null) {
	    if (child instanceof Element && child.getNodeName().equals(tagName))
		return (Element) child;
	    child = child.getNextSibling();
	}
	return null;
    }
    
    /* Returns the text associated with the given element (which must have
     * type #PCDATA) as child, or "" if it contains no text. */
    static String getElementText(Element e) {
	if (e.getChildNodes().getLength() == 1) {
	    Text elementText = (Text) e.getFirstChild();
	    return elementText.getNodeValue();
	}
	else
	    return "";
    }
    
    /* Returns the text (#PCDATA) associated with the first subelement X
     * of e with the given tagName. If no such X exists or X contains no
     * text, "" is returned. */
    static String getElementTextByTagNameNR(Element e, String tagName) {
	Element elem = getElementByTagNameNR(e, tagName);
	if (elem != null)
	    return getElementText(elem);
	else
	    return "";
    }
    
    /* Process files. */
    static void processFiles(File predFile, File truthFile) {
	Document predDoc = null;
	Document truthDoc = null;
	
	try {
	    predDoc = builder.parse(predFile);
	    truthDoc = builder.parse(truthFile);
	}
	catch (IOException e) {
	    e.printStackTrace();
	    System.exit(3);
	}
	catch (SAXException e) {
	    e.printStackTrace();
	    System.exit(3);
	}
	
	
	Element predRoot = (Element)predDoc.getDocumentElement();
	Element truthRoot = (Element)truthDoc.getDocumentElement();
	
	Node currPredFrame = predRoot.getFirstChild();
	Node currTruthFrame = truthRoot.getFirstChild();
	
	double score = 0.0;
	double totalScore = 0.0;
	int frameCounter = 0;
	
	
	while(currPredFrame != null && currTruthFrame != null) {
	    
	    if(currPredFrame.getNodeName().equals("frame") &&
	       currTruthFrame.getNodeName().equals("frame")) {


		//if(frameCounter == 194)
		score = calcScore(currPredFrame, currTruthFrame);
		
		System.out.println("The score for frame " + frameCounter + " is "
				   + score);
		
		frameCounter++;
		totalScore += score;

		if(verbose && frameCounter > 20)
		    break;
	    }
	    currPredFrame = currPredFrame.getNextSibling();
	    currTruthFrame = currTruthFrame.getNextSibling();
	}
	System.out.println("*********************************");
	System.out.println("The Total Score is: " + totalScore/frameCounter);
    }
    
    public static double calcScore(Node predFrame, Node truthFrame) {
	
	Element[] predictions = getElementsByTagNameNR((Element)predFrame, "object");
	Element[] truth = getElementsByTagNameNR((Element)truthFrame, "object");

	double[] scores = new double[labels.length];
	double sum = 0;
	ArrayList<Element> tempPredictions;
	ArrayList<Element> tempTruth;

	for(int i = 0; i < labels.length; i++) {
	    tempPredictions = new ArrayList<Element>();
	    tempTruth = new ArrayList<Element>();
	
	    for(int j = 0; j < predictions.length; j++) 
		if(predictions[j].getAttribute("label").equals(labels[i])) 
		    tempPredictions.add(predictions[j]);

	    for(int j = 0; j < truth.length; j++) 
		if(truth[j].getAttribute("label").equals(labels[i])) 
		    tempTruth.add(truth[j]);

	    scores[i] = calcScoreHelper(tempPredictions, tempTruth);
	    sum += scores[i];
	}

	return sum/labels.length;

    }
    private static double calcScoreHelper(ArrayList<Element> predictions, ArrayList<Element> truth) {
	double totalOverlap = 0;
	double total = 0;
	
	if(predictions.size() == 0 && truth.size() > 0)
	    return 0;
		

	if(truth.size() == 0) 
	    return 1.0/(double)(1 + predictions.size());
	
	// calculate the overlap
	for(int i = 0; i < predictions.size(); i++) {
	    totalOverlap += getMaxOverlapScore(predictions.get(i), truth);
	    
		if(verbose) {
		    System.out.println(" curoverlap " +
				       getMaxOverlapScore(predictions.get(i), truth));
		}
	}
	
	return totalOverlap/predictions.size();
    }
    
    /*
     * Finds the overlap between the rectangle and each of the rectanges
     * in the nodes list, and returns the largest overlap.
     *
     */
    private static double getMaxOverlapScore(Element rect, ArrayList<Element> rects) {
	double maxOverlapScore = 0;
	double overlapScore;

	for(int i = 0; i < rects.size(); i++) {
	    overlapScore = getOverlapScore(rect, rects.get(i));

	    if(overlapScore > maxOverlapScore)
		maxOverlapScore = overlapScore;
	}

	return maxOverlapScore;
    }

    /*
     * Calculates the overlap between the two rectangles
     *
     */ 
    private static double getOverlapScore(Element rect1, Element rect2) {
	double x1 = Double.parseDouble(rect1.getAttribute("x"));
	double y1 = Double.parseDouble(rect1.getAttribute("y"));
	double w1 = Double.parseDouble(rect1.getAttribute("w"));
	double h1 = Double.parseDouble(rect1.getAttribute("h"));
	double x2 = Double.parseDouble(rect2.getAttribute("x"));
	double y2 = Double.parseDouble(rect2.getAttribute("y"));
	double w2 = Double.parseDouble(rect2.getAttribute("w"));
	double h2 = Double.parseDouble(rect2.getAttribute("h"));

	double width, height;

	if(x1 <= x2) {
	    width = Math.max(0, w1 - (x2 - x1));
	    width = Math.min(width, w2);
	} else {
	    width = Math.max(0, w2 - (x1 - x2));
	    width = Math.min(width, w1);
	}

	if(y1 <= y2) {
	    height = Math.max(0, h1 - (y2 - y1));
	    height = Math.min(height, h2);
	} else {
	    height = Math.max(0, h2 - (y1 - y2));
	    height = Math.min(height, h1);
	}

	double overlap = height * width;
	double totalArea = getArea(rect1) + getArea(rect2);

	if(totalArea == 0) {
	    return 1;
	}

	return overlap/(totalArea - overlap);
    }

    /*
     * Calculates the area of the given rectangle
     */
    private static double getArea(Element rect) {
	double h = Double.parseDouble(rect.getAttribute("h"));
	double w = Double.parseDouble(rect.getAttribute("w"));

	return h*w;
    }

    public static void main (String[] args) {
	if (args.length == 0 || args.length == 1) {
	    System.out.println("Usage: java Scorer [predictionFile] [truthFile] ");
	    System.exit(1);
	}
	
	/* Initialize parser. */
	try {
	    DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
	    factory.setValidating(true);
	    factory.setIgnoringElementContentWhitespace(true);      
	    builder = factory.newDocumentBuilder();
	    //builder.setErrorHandler(new MyErrorHandler());
	}
	catch (FactoryConfigurationError e) {
	    System.out.println("unable to get a document builder factory");
	    System.exit(2);
	} 
	catch (ParserConfigurationException e) {
	    System.out.println("parser was unable to be configured");
	    System.exit(2);
	}
	
	/* Process all files listed on command line. */
	File predFile, truthFile;
	
	predFile = new File(args[0]);
	truthFile = new File(args[1]);
	
	processFiles(predFile, truthFile);
	
    }
}
