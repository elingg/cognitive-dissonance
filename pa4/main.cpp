#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>

#include <ctype.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "PpmUtil.h"
#include "Maps.h"
#include "Robot.h"
#include "SensorModel.h"
#include "ActionModel.h"

using namespace std;

// Enums
// used to see what sensor model the user chose
enum 
{
  USE_BUMP_SM,
  USE_LASERS_SM,
  USE_CUSTOM_SM,
};

// used to see what action model the user chose
enum
{
  USE_RANDOM_AM,
  USE_SIMPLE_AM,
};
/* ================ */
/* Global variables */
/* ================ */

/* All of these are used for getting input from the command line
 * arguments (which are all set in the function ReadArguments.
 *
 * CS221 Students: you will only need to use the first four of these
 * global variables.
 */

int v_useSM;       // Check enum's above to see what values these can take
int v_useAM;       // Check enum's above to see what values these can take
bool v_kidnapped;
bool v_noHistory;

bool v_dumpInfo;             // should we dump info out
bool v_dumpError;            // should we dump the error out

bool v_dumpImages;           // should we dump images out
int v_nFramesPerImgDumped;   // number of frames to skip before dumping an image
int v_pixPerCellDump;        // number of pixels to use to represent one cell (as in length of a square)
PpmImg *v_ppmImg;            // used to dump out an image file.  
PpmImg *v_ppmResultImg;      // used to dump out the final image file.  

char *v_mapFilename;  // map file name
char *v_logFilename;  // log file name

double v_totalError;

/* =================== */
/* Function Prototypes */
/* =================== */

// Reads the command line arguments and sets our ugly global variables
void ReadArguments(int argc, char **argv);
// Prints out the error message including the proper usage of the function
void ErrorIntro(int argc, char **argv);

// This is used to write out the final localization results.
void DumpResultImage(BeliefMap *bmap, RobotState *robotState);

// Debug functions accessed via the -i, -e, and -d flags.
void DumpInfo(BeliefMap *bmap, RobotState *robotState);
void DumpError(BeliefMap *bmap, RobotState *robotState);
void DumpImage(int i, BeliefMap *bmap, RobotState *robotState);


// Prototypes of the functions you will have to edit.
void RunExperiment(SimpleActionModel *am, RandomActionModel *ram, RobotSimulation *rsim, BeliefMap *bmap);
void InitializeBeliefs(BeliefMap *bmap, RobotSimulation *rsim, bool kidnapped);
void UpdateLocationBeliefs(SensorModel *sm, ActionModel *am, RobotState *robotState, BeliefMap *bmap);


/* ==================== */
/* Function Definitions */
/* ==================== */

/* main */
/* ---- */
/* reads the command line, sets the global variables, runs the experiment.
 */

int main(int argc, char** argv) {
  PPMMap ppmMap;
  BeliefMap *bmap;
  RobotSimulation robotsim;
  SimpleActionModel sam;
  RandomActionModel ram;

  // Reads all the arguments and sets up the global variables.  hooray!
  ReadArguments(argc, argv);
  
  // We've now successfully read in the command line arguments.

  // Setup the maps
  if(!ppmMap.ReadFromPPM(v_mapFilename))
    exit(1);
  ppmMap.SetMetersPerCell(0.1);
  bmap = ppmMap.Discretize(5);

  v_ppmResultImg = ppmMap.mapImg;

  if(v_pixPerCellDump == 5){
    // if we're using same size cells... might as well just
    // use the original map file image :)
    v_ppmImg = ppmMap.mapImg;
  }else{
    // otherwise we draw ourselves a new one...
    v_ppmImg = bmap->DrawSmallMap(v_pixPerCellDump);
  }
  
  // Setup the logfiles
  if(!robotsim.ReadLogFile(v_logFilename))
    exit(1);
  robotsim.GenerateStates(6, bmap);

  // Setup Action Model
  if(!sam.ReadFromFile("simpleaction.mdl"))
    exit(1);

  cout << "Starting Experiment" << endl;
  RunExperiment(&sam, &ram, &robotsim, bmap);
  cout << "Experiment Completed" << endl;

  return(0);
}

/* ReadArguments */
/* ------------- */
/* Just reads in the commandline arguments and sets global vars.
 * Makes sure arguments are legit.  If they aren't then we print out
 * the proper usage.
 */

void ReadArguments(int argc, char **argv)
{
  char **arguments = argv + 1;
  int numArgs = argc - 1;

  // Preinitialize all of the global variables first.
  v_kidnapped = false;
  v_noHistory = false;
  v_useSM = USE_BUMP_SM;
  v_useAM = USE_RANDOM_AM;

  v_dumpInfo = false;
  v_dumpError = false;
  v_dumpImages = false;
  v_pixPerCellDump = 1;
  v_nFramesPerImgDumped = 0;

  v_mapFilename = NULL;
  v_logFilename = NULL;

  v_totalError = 0.0;

  // Check for image flag.
  if(numArgs >= 3) {
    if (strcmp(arguments[0], "-d") == 0) {
      //   printf("caught the -d\n");
      v_dumpImages = true;
      numArgs--;
      arguments++;

      v_pixPerCellDump = atoi(arguments[0]);
      //   cout << v_pixPerCellDump << endl;
      numArgs--;
      arguments++;

      v_nFramesPerImgDumped = atoi(arguments[0]);
      //   cout << v_nFramesPerImgDumped << endl;
      numArgs--;
      arguments++;

      if(v_pixPerCellDump <= 0 || v_pixPerCellDump > 5 || v_nFramesPerImgDumped <= 0){
        ErrorIntro(argc, argv);
	//   printf("1          ...\n");
        exit(1);
      }
    }
  }

  // Check info flag
  if(numArgs > 0) {
    if (strcmp(arguments[0], "-i") == 0){
      //   printf("caught the -i\n");
      v_dumpInfo = true;
      numArgs--;
      arguments++;
    }
  }

  // Check error flag
  if(numArgs > 0){
    if (strcmp(arguments[0], "-e") == 0){
      //   printf("caught the -e\n");
      v_dumpError = true;
      numArgs--;
      arguments++;
    }
  }

  // pull out the filenames
  if(numArgs < 2)
  {
    ErrorIntro(argc, argv);
    //    printf("3          ...\n");
    exit(1);
  }

  v_mapFilename = arguments[0];
  v_logFilename = arguments[1];
  arguments +=2;
  numArgs -= 2;


  // now check the action and sensor settings...
  if(numArgs < 2) {
    ErrorIntro(argc, argv);
    //    printf("4          ...\n");
    exit(1);
  }

  // check sensor
  char *sensorType = arguments[0];
  if(strcmp(sensorType, "bump") == 0) {
    v_useSM = USE_BUMP_SM;
    //    printf("bump\n");
  }
  else if(strcmp(sensorType, "lasers") == 0) {
    v_useSM = USE_LASERS_SM;
    //    printf("lasers\n");
  }
  else if(strcmp(sensorType, "custom") == 0) {
    v_useSM = USE_CUSTOM_SM;
    //    printf("custom\n");
  }
  else {
    ErrorIntro(argc, argv);
    //    printf("5         ...\n");
    exit(1);
  }

  arguments++;
  numArgs--;

  // check action
  char *actionType = arguments[0];
  if(strcmp(actionType, "random") == 0) {
    v_useAM = USE_RANDOM_AM;
    //    printf("random\n");
  }
  else if(strcmp(actionType, "simple") == 0) {
    v_useAM = USE_SIMPLE_AM;
    //    printf("simple\n");
  }
  else {
    ErrorIntro(argc, argv);
    //    printf("6          ...\n");
    exit(1);
  }

  arguments++;
  numArgs--;

  // check for the optional noHistory and kidnapped flags
  v_noHistory = false;
  v_kidnapped = false;

  if(numArgs > 0 && strcmp(arguments[0], "nohistory") == 0) {
    //    printf("nohistory\n");
    v_noHistory = true;
    arguments++;
    numArgs--;
  } else {
    v_noHistory = false;
  }
  
  if(numArgs > 0 && strcmp(arguments[0], "kidnapped") == 0) {
    //    printf("kidnapped\n");
    v_kidnapped = true;
    arguments++;
    numArgs--;
  }

  if(numArgs > 0) {
    // too many arguments...
    ErrorIntro(argc, argv);
    //    printf("7          ...\n");
    exit(1);
  }
}

/* ErrorIntro */
/* ---------- */
/* Prints out the proper usage of the command line arguments
 */

void ErrorIntro(int argc, char** argv) {
  printf("Usage: %s <-d digit1 digit2> <-i> <-e> <map file> <log file> <bump|lasers|custom> <random|simple> <nohistory> <kidnapped>\n", argv[0]);
  printf("        where <-d digit1 digit2>, <-i>, <-e>, <nohistory> and <kidnapped> are optional tags.\n");
}

/* ================================================================= */
/* CS221 Students: The following are functions you will need to edit */
/* ================================================================= */

/* RunExperiment */
/* ------------- */
/* This function basically sets up the parameters of the experiment.  It loads
 * the action model and sensor models to be used.  It calls InitializeBeliefs 
 * to initialize the belief map.  It then runs through the history of robot
 * actions and sensor readings at each time step and runs UpdateLocationBeliefs
 * to apply the motion and sensor updates to the belief map.
 *
 *
 * please note the following global variables that you should use.
 * int v_useSM;        Which sensor model to use (see the enums on top)
 * int v_useAM;        Which action model to use (see the enums on top)
 * bool v_kidnapped;   Run kidnapped experiment
 * bool v_noHistory;   Run noHistory (i.e. forgetful robot) experiment
 *
 */

void RunExperiment(SimpleActionModel *sam, RandomActionModel *ram, RobotSimulation *rsim, BeliefMap *bmap)
{
  int numCells;
  RobotState *robotState = NULL;
  unsigned int i;

  /** SENSOR MODELS **/
  // Laser Array Sensor model (DO NOT use this directly)
  LaserArraySensorModel lsm;
  // Custom Laser Array Sensor model (DO NOT use this directly)
  CustomLASensorModel custom_lsm;

  // Bump Sensor model (DO use this directly)
  BumpSensorModel bsm;
  // Laser Array And Bump Sensor model (DO use this directly)
  LaserArrayAndBumpSensorModel lbsm;
  // Custom Laser Array And Bump Sensor model (DO use this directly)
  LaserArrayAndBumpSensorModel custom_lbsm;


  /** Pointers to sensor/action models
   ** (passed into UpdateLocationBeliefs)
   **/
  // set to actual models in switch statements
  // below
  SensorModel *sm = NULL;
  ActionModel *am = NULL;

  numCells = bmap->allCells.size();

  // Set Up Sensor Models
  // --------------------
  // Set up the bump sensor model
  bsm.Initialize(bmap);

  // Set up the laser-array-and-bump model
  // NOTE: DO NOT use lsm, DO use lbsm
  lsm.Initialize(bmap, 2.5);
  lbsm.Initialize(&lsm, &bsm);

  // At the beginning of the experiment, you'll want to first initialize all the
  // beliefs for each cell in the belief map.  Then you'll also want to choose 
  // which action model and which sensor model to use by setting am and sm.

  // please note the following global variables that you should use.
  // int v_useSM;        Which sensor model to use (see the enums on top)
  // int v_useAM;        Which action model to use (see the enums on top)
  // bool v_kidnapped;   Run kidnapped experiment
  // bool v_noHistory;   Run noHistory (i.e. forgetful robot) experiment

  // For the final experiment... set up the
  // custom-laser-array-and-bump model here feel free to change
  // custom_lsm and do any initializing you need here...
  // NOTE: again, DO NOT use custom_lsm, DO use custom_lbsm
  custom_lsm.Initialize(bmap, 2.5);
  custom_lbsm.Initialize(&custom_lsm, &bsm);

  // choose sensor model here by setting sm to the address of
  // whichever sensor model
  switch(v_useSM) {
    case USE_LASERS_SM:
      sm = &lbsm;
      break;
    case USE_CUSTOM_SM:
      sm = &custom_lbsm;
      break;
    case USE_BUMP_SM:
    default:
      sm = &bsm;
      break;
  }

  // choose action model here by setting am to the address of
  // whichever action model
  switch(v_useAM) {
    case USE_SIMPLE_AM:
      am = sam;
      break;
    case USE_RANDOM_AM:
    default:
      am = ram;
      break;
  }

  // Initialize the belief map with InitializeBeliefs
  // NOTE: YOU must implement InitializeBeliefs
  InitializeBeliefs(bmap, rsim, v_kidnapped);

  // Once the map has been initialized and the action/sensor models have been chosen,
  // loop through the history of the robot's states starting at time 0.
  robotState = rsim->states.at(0);

  // output an image
  if(v_dumpImages)
    DumpImage(-1, bmap, robotState);

  for(i = 0; i < rsim->states.size(); i++)
  {
    cout << "Processing Frame " << i + 1 << " of " << rsim->states.size() << endl;

    if (v_noHistory) {
      /** CS221 TASK 4: BEGIN EDIT CODE **/
      // you'll need to handle the 'forgetful robot'
      // (i.e., no history) case here:


      /** CS221 TASK 4: END EDIT CODE **/
    }

    // Update the belief map by applying the motion and sensor updates.
    robotState = rsim->states.at(i);
    UpdateLocationBeliefs(sm, am, robotState, bmap);

    // dump out the belief values for all free cells.
    if (v_dumpInfo)
      DumpInfo(bmap, robotState);

    if (v_dumpError)
      DumpError(bmap, robotState);

    // output an image
    if (v_dumpImages)
      DumpImage((int) i, bmap, robotState);
  }

  // Dump out the average error
  if (v_dumpError)
    {
      v_totalError /= (double)i;
      cout << "Average Error = " << v_totalError << endl;
    }

  // Dump out the final image result
  DumpResultImage(bmap, robotState);
}

/* InitializeBeliefs */
/* ----------------- */
/* This function should initialize all of the beliefs for the belief map.
 */

void InitializeBeliefs(BeliefMap *bmap, RobotSimulation *rsim, bool kidnapped)
{
  // You'll want to initialize all the cell belief values before you start your 
  // experiment.  Make sure that everything adds up to 1...
  // This method should set all cell beliefs (i.e., Cell.belief field) to
  // the correct initial value. Note: you'll probably want to clear out
  // the Cell.buffer field since you'll be using it for temporary storage

  if(kidnapped) {
    /** CS221 TASK 3: BEGIN EDIT CODE **/
    // initialize beliefs as though robot has no idea
    // where it is starting (HINT: all squares are equally
    // likely so the distribution is "uniform" over squares)


    /** CS221 TASK 3: END EDIT CODE **/
  } else {
    /** CS221 TASK 1: BEGIN EDIT CODE **/
    // robot knows exactly where it is starting (HINT: use the
    // RobotSimulation::GetInitialPosition() method)
       MapCoord coord = rsim->GetInitialPosition();
       Cell* cell = bmap->GetCell(coord);
       cell->belief= 1;
    

    /** CS221 TASK 1: END EDIT CODE **/
  }
}

/* UpdateLocationBeliefs */
/* --------------------- */
/* This function should use the action model and and the sensor model to
 * apply the motion and sensor updates on the belief map.
 */
void UpdateLocationBeliefs(SensorModel *sm, ActionModel *am, RobotState *robotState, BeliefMap *bmap)
{
  // if neither the action model nor the sensor model is not set, then
  // just return.
  if (am == NULL || sm == NULL)
    return;

  // Set up the Sensor and Action Models so that they know what the current action and readings are.
  sm->SetReadings(robotState);
  am->SetAction(robotState);

  /** CS221 TASK 2: BEGIN EDIT CODE **/
  // 1.  Use the action model to update the beliefs.
  // 2.  Use the sensor model to update the beliefs.
  //
  // The robotState will contain the action the robot is going to take
  // as well as the sensor readings that the robot got.  Use these
  // with the passed in Sensor and Action Models to update
  // probabilities in the passed in Belief Map.
  //
  // Notes: 
  // a. For step 1, you will find that the "buffer" field in the cell is handy.  Be careful
  // about overwriting probabilities that you still need...
  // b. Make sure that when you're done, all the beliefs add up to 1.
  // c. Lastly, be sure to make use of the BeliefMap::IsCoordInMap function with the Action Model.
  
   MapCoord coord = bmap->GetMapCoordFromMeters(robotState->ptActual);
   Cell* cell = bmap->GetCell(coord);
   cell->buffer= cell->belief;

   //Action model update
   int n = am->GetNumNewLocations();
   for(int i=0; i<n; i++)
   {
	cell->belief = cell->belief + (cell->buffer * am->GetProbNewLocGivenAction(i));
   }
   
   //Sensor model update   
   cell->belief = cell->belief *sm->GetPFromMapCoord(coord);
   //TODO: normalize probabilities


  /** CS221 TASK 2: END EDIT CODE **/
}


/* Dump{ResultImage, Image, Info, Error} */
/* ------------------------------------- */
/* These functions are used to generate/display the results of the
 * localization process.  DumpResultImage is always called and writes
 * out "result.ppm".  DumpImage is called only if the correct flags
 * are used, and writes out to "locxxx.ppm" where xxx is the time step
 * the .ppm corresponds to.  The DumpInfo file dumps out all the
 * belief values for all of the free cells in the map.
 */

/* Dump{Result}Image */
/* ----------------- */
/* These functions dump the belief map to .ppm image.  The actual
 * location of the robot is marked in green.  The "dead-reckoned"
 * position of the robot is marked in cyan... this is where the robot
 * thinks it is if we assume that every action it takes is performed
 * perfectly.  The "redder" the squares are, the higher the belief we
 * have that the robot is located in that cell.  The cell with the
 * highest belief is marked the reddest... 
 */

void DumpResultImage(BeliefMap *bmap, RobotState *robotState)
{
    bmap->DumpBeliefsWithTruthAndDRToPpm(v_ppmResultImg, "result.ppm", 
					 bmap->GetMapCoordFromMeters(robotState->ptActual),
					 bmap->GetMapCoordFromMeters(robotState->ptDeadReckon),
					 5);
}

void DumpImage(int i, BeliefMap *bmap, RobotState *robotState)
{
  char buffer[100];
  if (((i + 1) % v_nFramesPerImgDumped) == 0) {
    sprintf(buffer, "./images/loc%.3d.ppm", i + 1);
    bmap->DumpBeliefsWithTruthAndDRToPpm(v_ppmImg, buffer, 
				    bmap->GetMapCoordFromMeters(robotState->ptActual), 
				    bmap->GetMapCoordFromMeters(robotState->ptDeadReckon),
				    v_pixPerCellDump);
  }
}

/* DumpInfo */
/* -------- */
/* Dumps out the beliefs of the 5x5 square of cells centered around
 * the actual location of the robot
 */

void DumpInfo(BeliefMap *bmap, RobotState *robotState)
{
  int i, j;
  // we want scientific notation...
  cout.flags(ios_base::scientific);

  MapCoord mc = bmap->GetMapCoordFromMeters(robotState->ptActual);

  cout << "True Pos = (" << mc.x << "," << mc.y << ")" << endl;
  for (i = -2; i <= 2; i++)
    for (j = -2; j <= 2; j++)
      {
	MapCoord mcCur;
	mcCur.x = mc.x + i;
	mcCur.y = mc.y + j;
	if (bmap->IsCoordInMap(mcCur))
	  cout << "\tB(" << mcCur.x << "," << mcCur.y << ") = " << bmap->GetCell(mcCur)->belief << endl;
      }
}

/* DumpError */
/* --------- */
/* Dumps out the weighted error of the current belief map.  Weighted
 * error is basically just the sum of the distance of each cell to the
 * true cell (where the robot is) each weighted by the belief of that
 * cell.
 */

void DumpError(BeliefMap *bmap, RobotState *robotState)
{
  vector<Cell*>::iterator it;
  Cell *curCell;
  double error = 0.0;
  MapCoord actualCoord = bmap->GetMapCoordFromMeters(robotState->ptActual);

  cout.flags(ios_base::scientific);

  for(it = bmap->allCells.begin(); it != bmap->allCells.end(); it++)  
  {
    curCell = *it;
    MapCoord dCoord = curCell->coord - actualCoord;
    error += sqrt((double)(dCoord.x*dCoord.x) + (double)(dCoord.y*dCoord.y)) * curCell->belief;
  }
  cout << "\tWeighted Error = " << error << endl;

  v_totalError += error;
}
