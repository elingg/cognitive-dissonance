#include <iostream>
#include <assert.h>
#include <math.h>

#include "Maps.h"

/* CS221 Students: Nothing to see here :) just a lot of smoke and
 * mirrors 
 */

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX_DISTANCE 100000

/* =================================================================== */
/*                             CellMap                                 */
/* =================================================================== */

CellMap::CellMap()
{
  cells = NULL;
  width = 0;
  height = 0;
  mPerCell = 1;
}

CellMap::CellMap(int w, int h, double mpcell)
{
  cells = NULL;
  Reset(w, h, mpcell);
}

CellMap::~CellMap()
{
  if(cells)
    delete cells;
}

// Reset
// -----
// Resets the cell map.  Allocates new memory for the new cell array and also sets the
// new measurement units for each cell.  Returns true on successful reset, or false on
// on a failed reset.
//
// Note: if width, height, or the measurement units are negative, then we return false

bool CellMap::Reset(int w, int h, double mpcell)
{
  int i, j;

  // sanity check
  if (w <= 0 || h <= 0 || mpcell <= 0.0)
  {
    cerr << "Failure in CellMap::Reset - Negative width, height, or meters per cell" << endl;
    return false;
  }

  // clear out old cells if they existed and allocate new cells
  if(cells)
      delete cells;
  cells = new Cell[w * h];
  if(!cells)
  {
    cerr << "Failure - Ran out of memory" << endl;
    width = 0;
    height = 0;
    return false;
  }

  // reset should now be a guaranteed success.
  width = w;
  height = h;
  mPerCell = mpcell;

  for (i = 0; i < width; i++)
    for (j = 0; j < height; j++)
      InitializeCell(i, j);

  return true;
}

// InitializeCell
// --------------
// sets the cell's values to default values

void CellMap::InitializeCell(int x, int y)
{
  int i;
  Cell* curCell;
  if(IsCoordInMap(x, y))
  {
    curCell = GetCell(x,y);
    curCell->coord.x = x;
    curCell->coord.y = y;
    curCell->belief = 0.0;
    curCell->buffer = 0.0;
    curCell->type = UNKNOWN;
    curCell->value = 0;
    for(i = 0; i < NUM_DIRECTIONS; i++)
      curCell->closestWall[i] = 0;
  }
}

// IsCoordInMap
// ------------
// Checks to see if the coordinates used are within the bounds of the 
// map extents.  Two versions available: one that takes in a MapCoord
// and one that takes in an x and y.

bool CellMap::IsCoordInMap(MapCoord coord)
{
  return (IsCoordInMap(coord.x, coord.y));
}

bool CellMap::IsCoordInMap(int x, int y)
{
  return(x >= 0 && y >= 0 && x < width && y < height);
}

// GetMapCoordFromMeters
// ---------------------
// Find what the coordinates of the cell that contain the position in
// meters that is passed in.  Two versions available... one that takes
// in a Point, and one that takes in two doubles.
//
// Note: We need to flip the y because we assume that the position in 
// meters follows the convention that 0,0 is located in the lower left
// part of the map... whereas the map coordinates place 0,0 in the
// upper right.  It's kind of annoying that we have to do this, but 
// we must since the robot's odometry follows the "0,0 in the lower left"
// convention, whereas images (i.e. the map image) always place 0,0 in
// the upper left.

MapCoord CellMap::GetMapCoordFromMeters(double x, double y) const
{
  MapCoord result;
  result.x = (int) (floor(x / (double)mPerCell));
  result.y = (int) (floor(y / (double)mPerCell));

  // we need to flip y because the log file uses a coordinate system
  // where y increases as you move North (up) the map.  The map, however,
  // uses the convention that y decreases as you move North (up).

  result.y = height - 1 - result.y;
  return(result);
}

MapCoord CellMap::GetMapCoordFromMeters(Point pt) const
{
  return(GetMapCoordFromMeters(pt.x, pt.y));
}

// GetCell
// -------
// Gets the pointer to the cell corresponding to the passed in coordinates.
// Two versions available: one that takes in two ints, another that takes in
// just a coord.

Cell *CellMap::GetCell(int x, int y)
{
  if(cells && IsCoordInMap(x, y))
  {
    return(&(cells[y*width + x]));
  }
  else
    return NULL;
}

Cell *CellMap::GetCell(MapCoord coord)
{
  return GetCell(coord.x, coord.y);
}

/* =================================================================== */
/*                              PPMMap                                 */
/* =================================================================== */

#define TOLERANCE_OCCUPIED 192 // "occupied" if lower than this tolerance

PPMMap::PPMMap() : CellMap()
{
  mapImg = NULL;
}

// ReadFromPPM
// -----------
// Tries to initialize the PPMMap from a PPM file.  Return value indicates
// success or failure.

bool PPMMap::ReadFromPPM(const char *filename)
{
  mapImg = new PpmImg();
  if(!mapImg)
    goto FAILURE;

  if(!mapImg->ReadPPM(filename))
    goto FAILURE;

  width = mapImg->width;
  height = mapImg->height;

  cells = new Cell[width * height];

  if(!cells)
    goto FAILURE;

  // Now we can initialize the cells.
  SetCellTypes();
  SetCellDistances();

  return true;

FAILURE:
  // When we fail, make sure that we deallocate any
  // memory we may have allocated, and to null the 
  // ivars.

  if(cells)
  {
    delete cells;
    cells = NULL;
  }

  if(mapImg)
  {
    delete mapImg;
    mapImg = NULL;
  }

  return false;
}

// SetCellTypes
// ------------
// Figures out if the cell is to be considered unknown, empty, or occupied.
// Also sets the value of the cell equal to one of the RGB values for kicks.

void PPMMap::SetCellTypes()
{
  int i, j;

  for(i = 0; i < width; i++)
    for(j = 0; j < height; j++)
    {
      GetCell(i,j)->type = GetCellType(mapImg->GetRGB(i,j));
      GetCell(i,j)->value = (mapImg->GetRGB(i,j)).b;
    }
}

// SetCellDistances
// ----------------
// Finds the closest obstacle cell in the north, south, east, west directions
// for each cell.

void PPMMap::SetCellDistances()
{
  int i, j, lastMark;
  CellType lastMarkType;
  Cell *curCell;
  
  // do this in four sweeps, one per direction...
  // there's probably some clever way of decomping this.  meh.

  // Set the North distance... so sweep from North to South
  for(i = 0; i < width; i++)
  {
    lastMarkType = UNKNOWN;
    lastMark = -1;
    for(j = 0; j < height; j++)
    {
      curCell = GetCell(i,j);
      if(curCell->type != FREE)
      {
        lastMarkType = curCell->type;
        lastMark = j;
      } 
      else if(lastMarkType == UNKNOWN)
        curCell->closestWall[NORTH] = MAX_DISTANCE;
      else // lastMarkType == OCCUPIED
        curCell->closestWall[NORTH] = j - lastMark;
    }
  }
  // Set the South distance... so sweep from South to North
  for(i = 0; i < width; i++)
  {
    lastMarkType = UNKNOWN;
    lastMark = height;
    for(j = height - 1; j >= 0; j--)
    {
      curCell = GetCell(i,j);
      if(curCell->type != FREE)
      {
        lastMarkType = curCell->type;
        lastMark = j;
      } 
      else if(lastMarkType == UNKNOWN)
        curCell->closestWall[SOUTH] = MAX_DISTANCE;
      else // lastMarkType == OCCUPIED
        curCell->closestWall[SOUTH] = lastMark - j;
    }
  }  

  // Set the West distance... so sweep from West to East
  for(j = 0; j < height; j++)
  {
    lastMarkType = UNKNOWN;
    lastMark = -1;
    for(i = 0; i < width; i++)
    {
      curCell = GetCell(i, j);
      if(curCell->type != FREE)
      {
        lastMarkType = curCell->type;
        lastMark = i;
      } 
      else if(lastMarkType == UNKNOWN)
        curCell->closestWall[WEST] = MAX_DISTANCE;
      else // lastMarkType == OCCUPIED
        curCell->closestWall[WEST] = i - lastMark;
    }
  }  

  // Set the East distance... so sweep from East to West
  for(j = 0; j < height; j++)
  {
    lastMarkType = UNKNOWN;
    lastMark = width;
    for(i = width - 1; i >= 0; i--)
    {
      curCell = GetCell(i,j);
      if(curCell->type != FREE)
      {
        lastMarkType = curCell->type;
        lastMark = i;
      } 
      else if(lastMarkType == UNKNOWN)
        curCell->closestWall[EAST] = MAX_DISTANCE;
      else // lastMarkType == OCCUPIED
        curCell->closestWall[EAST] = lastMark - i;
    }
  }
}

// SetDiscreteCellFromCellBlock
// ----------------------------
// The PPMMap looks at the block of its own cells defined by the coordinates and length
// passed in.  It then initializes the values inside of the passed in cell to represent
// the entire block.
//
// todo(jjt) make it so that its possible that there is an obstacle cell?

void PPMMap::SetDiscreteCellFromCellBlock(Cell *cellDisc, int x, int y, int numCells)
{
  int i, j, dir;
  Cell *curCell;

  assert(cellDisc);

  cellDisc->type = FREE;
  for(dir = FIRST_DIRECTION; dir < NUM_DIRECTIONS; dir++)
    cellDisc->closestWall[dir] = MAX_DISTANCE;

  for(i = 0; i < numCells; i++)
    for(j = 0; j < numCells; j++)
    {
      curCell = GetCell(x + i, y + j);
      assert(curCell);
      if (curCell->type != FREE)
      {
        cellDisc->type = curCell->type;
        break;
      }
      for(dir = FIRST_DIRECTION; dir < NUM_DIRECTIONS; dir++)
        cellDisc->closestWall[dir] = MIN(cellDisc->closestWall[dir], curCell->closestWall[dir]);
    }
}

// Discretize
// ----------
// Returns a belief map... basically a discretized version of the PPM map.
BeliefMap *PPMMap::Discretize(int numCells)
{
  return(new BeliefMap(this, numCells));
}

/* =================================================================== */
/*                           BeliefMap                               */
/* =================================================================== */


BeliefMap::BeliefMap(PPMMap *ppmMap, int numCells) : CellMap()
{
  int w, h, i, j;
  assert(numCells > 0);

  w = ppmMap->GetWidth() / numCells;
  h = ppmMap->GetHeight() / numCells;

  assert(w > 0 && h > 0);

  metersPerWallUnit = ppmMap->GetMetersPerCell();
  Reset(w, h, numCells * ppmMap->GetMetersPerCell());

  for(i = 0; i < GetWidth(); i++)
    for(j = 0; j < GetHeight(); j++)
      InitializeCellFromPpmMap(i, j, ppmMap, numCells);

  pixelsPerCell = numCells;
}

void BeliefMap::InitializeCellFromPpmMap(int x, int y, PPMMap *ppmMap, int numCells)
{
  ppmMap->SetDiscreteCellFromCellBlock(GetCell(x, y), x * numCells, y * numCells, numCells);
  allCells.push_back(GetCell(x,y));
  if(GetCell(x,y)->type == FREE) freeCells.push_back(GetCell(x,y));
}

double BeliefMap::GetDistanceToWall(MapCoord mc, int direction)
{
  return (double)GetCell(mc)->closestWall[direction] * this->metersPerWallUnit;
}
#define EPSILON 0.00000001

void BeliefMap::DumpBeliefsWithTruthToPpm(const PpmImg *mapImg, const char *newFilename, 
					  MapCoord realLoc, int pixPerCell)
{
  PpmImg *newMapImg;

  newMapImg = mapImg->Copy();
  if(!newMapImg)
    return;

  DrawBeliefsWithTruth(newMapImg, realLoc, pixPerCell);

  newMapImg->WritePPM(newFilename);

  delete newMapImg;
}

void BeliefMap::DumpBeliefsWithTruthAndDRToPpm(const PpmImg *mapImg, const char *newFilename, 
					       MapCoord realLoc, MapCoord drLoc, int pixPerCell)
{
  PpmImg *newMapImg;

  newMapImg = mapImg->Copy();
  if(!newMapImg)
    return;

  DrawBeliefsWithTruthAndDR(newMapImg, realLoc, drLoc, pixPerCell);

  newMapImg->WritePPM(newFilename);

  delete newMapImg;
}

void BeliefMap::DrawBeliefsWithTruthAndDR(PpmImg *mapImg, MapCoord realLoc, 
					  MapCoord drLoc, int pixPerCell)
{
  RGB color;

  DrawBeliefs(mapImg, pixPerCell);

  // Draw the dead reckoned position
  color.r = 0;  color.g = 255;  color.b = 255;
  mapImg->DrawSquare(drLoc.x * pixPerCell, drLoc.y * pixPerCell, pixPerCell, color);

  // Draw the actual position
  color.r = 0;  color.g = 255;  color.b = 0;
  mapImg->DrawSquare(realLoc.x * pixPerCell, realLoc.y * pixPerCell, pixPerCell, color);

}

void BeliefMap::DrawBeliefsWithTruth(PpmImg *mapImg, MapCoord realLoc, int pixPerCell)
{
  RGB color;
  color.r = 0;
  color.g = 255;
  color.b = 0;

  DrawBeliefs(mapImg, pixPerCell);
  mapImg->DrawSquare(realLoc.x * pixPerCell, realLoc.y * pixPerCell, pixPerCell, color);
}

void BeliefMap::DrawBeliefs(PpmImg *mapImg, int pixPerCell)
{
  MapCoord cellCoord;
  double largestBelief;
  PpmImg baseImg;
  vector<Cell*>::iterator it;

  RGB color;
  color.r = 255;
  color.g = 255;
  color.b = 255;

  largestBelief = EPSILON;

  for(it = allCells.begin(); it != allCells.end(); it++)
    largestBelief = ((*it)->belief > largestBelief) ? (*it)->belief : largestBelief;

  for(it = allCells.begin(); it != allCells.end(); it++)
  {
    int diff = (int) (255.0 * (*it)->belief / largestBelief);
    color.g = 255 - diff;
    color.b = 255 - diff;
    cellCoord = (*it)->coord;
    if ((*it)->belief > EPSILON)
      mapImg->DrawFilledSquare(cellCoord.x * pixPerCell, cellCoord.y * pixPerCell, pixPerCell, color);
  }
}


PpmImg *BeliefMap::DrawSmallMap(int pixPerCell)
{
  RGB color;
  PpmImg *img = NULL;
  int w = pixPerCell * width;
  int h = pixPerCell * height;
  MapCoord cellCoord;
  vector<Cell*>::iterator it;

  color.r = 0;
  color.g = 0;
  color.b = 255;

  // set the background color to be blue.
  img = new PpmImg(w, h, color);
  assert(img->rgbArray);
  assert(img);

  color.r = 255;
  color.g = 255;
  color.b = 255;

  // draw all the free cells filled in with white.
  for (it = freeCells.begin(); it != freeCells.end(); it++)
  {
    cellCoord = (*it)->coord;
    img->DrawFilledSquare(cellCoord.x * pixPerCell, cellCoord.y * pixPerCell, pixPerCell, color); 
  }
  return(img);
}

// GetCellType
// -----------
// Used by PPMMap to figure out what kind of cell a pixel corresponds to.
CellType GetCellType(RGB rgb)
{
  if(rgb.r == 0 && rgb.g == 0 && rgb.b == 255)
    return(UNKNOWN);
  else if(rgb.r < TOLERANCE_OCCUPIED)
    return(OCCUPIED);
  else
    return(FREE);
}
