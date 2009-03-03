#ifndef _MAPS_H
#define _MAPS_H

/* CS221 Students */
/* -------------- */
/* This file contains useful information about the belief map class.
 * The belief map class inherits from the cell map, so be sure to look
 * inside of there too (you'll be able to use methods for cell map
 * with the belief map).
 * 
 * Useful classes: MapCoord, CellMap, BeliefMap (which inherits from 
 * CellMap)
 */

#include "PpmUtil.h"
#include <vector>

using namespace std;

// forward declarations
class CellMap;
class BeliefMap;
class PPMMap;

// enumeration useful for iterating through
// the laser rangefinder stuff...
enum
  {
    FIRST_DIRECTION = 0,
    EAST = 0,
    NORTH,
    WEST,
    SOUTH,
    NUM_DIRECTIONS    
  };

// Each cell can be of unkown type, free, or occupied.
// CS221 students: Pay no attention to this.  You do 
// not need to use any of this to do the assignment
typedef enum
  {
    UNKNOWN,
    FREE,
    OCCUPIED
  } CellType;

/* Point and MapCoord */
/* ------------------ */
/* These are basically the same classes.  Point bundles
 * together two doubles, MapCoord bundles together two 
 * ints.  
 * CS221 students: You'll only be using MapCoord.
 */

class Point
{
public:
  double x, y;
  Point operator+ (Point param) 
  { 
    Point result; 
    result.x = x + param.x; 
    result.y = y + param.y;
    return result;
  }
  Point operator- (Point param) 
  { 
    Point result; 
    result.x = x - param.x; 
    result.y = y - param.y;
    return result;
  }
};

class MapCoord
{
public:
  int x,y;
  MapCoord operator+ (MapCoord param) 
  { 
    MapCoord result; 
    result.x = x + param.x; 
    result.y = y + param.y;
    return result;
  }
  MapCoord operator- (MapCoord param) 
  { 
    MapCoord result; 
    result.x = x - param.x; 
    result.y = y - param.y;
    return result;
  }
};

/* Cell */
/* ---- */
/* All maps store large array of cells.  
 * CS221 Students: you will find the belief and buffer, and maybe
 * the coord fields of use in the assignment.  Nothing else though...
 *
 * note: when you're implementing a new sensor model, you do not want
 * to use the values stored in closestWall directly because these
 * values are pixels in the original map.  Use
 * BeliefMap::GetDistanceToWall to get the distance in meters (which
 * is what you'll need when editing the SensorModel.
 */

typedef struct
{
  MapCoord coord; // The coordinate that this cell corresponds to
                  // in the map that contains it.
  double belief;  // For use in the belief map. 
  double buffer;  // For use in your calculations of the belief...

  CellType type;  // What type of cell is this?  
  int closestWall[NUM_DIRECTIONS];
  double value;   // RGB value that was used to generate this cell
                  // only relevent for the PpmMap
} Cell;

/* CellMap */
/* ------- */
/* The CellMap is the parent class of the BeliefMap.  It basically stores
 * a large array of cells which represent a rectangular grid of cells. 
 *
 * CS221 Students: The methods GetCell, GetWidth, GetHeight, and
 * IsCoordInMap will be will be useful for you.  Really you don't need
 * anything else in this class.  
 *
 * Note: Be sure to use IsCoordInMap before calling GetCell!!!
 */

class CellMap
{
public:
  Cell *cells;
  int width, height;

  double mPerCell;

  CellMap();
  CellMap(int w, int h, double mpcell);
  ~CellMap();

  // Is the given coordinates in the map?
  bool IsCoordInMap(MapCoord coord);
  bool IsCoordInMap(int x, int y);

  // Get the cell corresponding to the given coordinates.
  // Will return null if you give it invalid coordinates.
  Cell *GetCell(int x, int y);
  Cell *GetCell(MapCoord coord);

  // Gets the width and height of the map in cells.
  int GetWidth() const { return width; }
  int GetHeight() const { return height; }

  void InitializeCell(int x, int y);

  MapCoord GetMapCoordFromMeters(double x, double y) const;
  MapCoord GetMapCoordFromMeters(Point pt) const;

  bool Reset(int w, int h, double mpcell);
  void SetMetersPerCell(double mpcell) { mPerCell = mpcell; }
  double GetMetersPerCell() const { return mPerCell; }
};

/* BeliefMap */
/* --------- */
/* Really, the belief map is just a CellMap a wee bit of extra
 * functionality, tailored specifically for the CS221 assignment.
 * It has the additional ability to dump itself into a ppm file
 * which is useful to see what is going on.
 *
 * CS221 Students: The only item you might want to use in the
 * BeliefMap is the vector allCells.  If you don't like vectors, you
 * can also just access all cells via their coordinates.  There are
 * some pretty useful methods, however, defined in its parent class,
 * CellMap.
 *
 * Ignore the rest of this...
 */

class BeliefMap : public CellMap
{
public:
  vector<Cell*> allCells;  // A vector of all the cells in the map. 
  vector<Cell*> freeCells;  // A vector of all the free cells in the
			    // map. (CS221 students, you prob won't
			    // need this)

  int pixelsPerCell;
  double metersPerWallUnit;
  BeliefMap(PPMMap *ppmMap, int numCells);
  void InitializeCellFromPpmMap(int x, int y, PPMMap *ppmMap, int numCells);
  PpmImg *DrawSmallMap(int pixPerCell);
  void DrawBeliefsWithTruth(PpmImg *mapImg, MapCoord realLoc, int pixPerCell);
  void DrawBeliefsWithTruthAndDR(PpmImg *mapImg, MapCoord realLoc, 
				 MapCoord drLoc, int pixPerCell);
  void DrawBeliefs(PpmImg *mapImg, int pixPerCell);
  void DumpBeliefsWithTruthToPpm(const PpmImg *mapImg, const char *newFilename, 
				 MapCoord realLoc, int pixPerCell);
  void DumpBeliefsWithTruthAndDRToPpm(const PpmImg *mapImg, const char *newFilename, 
				      MapCoord realLoc, MapCoord drLoc, int pixPerCell);
  double GetDistanceToWall(MapCoord mc, int direction);
};

/* PPMMap */
/* ------ */
/* The PPMMap inherits from the CellMap.  It basically is a map generated
 * directly form a .ppm file.  Each pixel in the .ppm corresponds exactly to
 * one pixel.  We use this as an intermediate step in discretizing the map
 * to eventually form a BeliefMap.
 */

class PPMMap : public CellMap
{
public:
  PpmImg *mapImg;

public:
  PPMMap();
  bool ReadFromPPM(const char *filename);
  BeliefMap *Discretize(int numCells);

  // Called by BeliefMap.  Used to initialize belif map cell from the corresponding 
  // block of cells that the belief map cell represents.
  void SetDiscreteCellFromCellBlock(Cell *cellDisc, int x, int y, int numCells);

private:
  void SetCellTypes();
  void SetCellDistances();
};

CellType GetCellType(RGB rgb);

#endif
