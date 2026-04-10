//------------------------------------------------------------------------------
//                           CustomSensor
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Mike Stark, NASA/GSFC
// Created: 2017.04.03
//
/**
 * Implementation of the CustomSensor class
 */
//------------------------------------------------------------------------------

#include <list>
#include <iostream> // for messages, **REPLACE** with appropriate calls *****

#include "CustomSensor.hpp"
#include "MessageInterface.hpp"
#include "BaseException.hpp" 
#include "TATCException.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "LinearAlgebra.hpp"

//#define DEBUG_CUSTOM_SENSOR
//#define DEBUG_CHECK_TARGET


using namespace std;
using namespace GmatRealConstants;
using namespace GmatMathConstants;
using namespace GmatMathUtil;

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CustomSensor(Rvector coneAngleVecIn, clockAngleVecIn)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * coneAngleVec and clockAngleVec contain pairs of angles that describe the 
 * sensor FOV.  coneAngleVec[0] is paired with clockAngleVec[0], 
 * coneAngleVec[1] is paired with clockAngleVec[1] and so on.  The last 
 * point in each arrays should be the same as the first point to ensure
 * FOV closure.
 * @param coneAngleVec array of cone angles measured from +Z sensor axis (rad)
 *        if xP,yP,zP is a UNIT vector describing a FOV point, then the 
 *        cone angle for the point is pi/2 - asin(zP);
 * @param clockAngleVec array of clock angles (right ascencions) rad
 *        measured clockwise from the + X-axis.  if xP,yP,zP is a UNIT vector
 *        describing a FOV point, then the clock angle for the point
 *        is atan2(y,x);
 */
//------------------------------------------------------------------------------
CustomSensor::CustomSensor (const Rvector &coneAngleVecIn,
                            const Rvector &clockAngleVecIn) :
   Sensor()
{
   
   #ifdef DEBUG_CUSTOM_SENSOR
      MessageInterface::ShowMessage(
                           "DEBUG: Entering CustomSensor constructor\n");
   #endif
      
   numFOVPoints = coneAngleVecIn.GetSize();

   // validate cone & clock angle inputs
   if (numFOVPoints != clockAngleVecIn.GetSize())
   {
      MessageInterface::ShowMessage(
         "CustomSensor: Cone & Clock angle vectors must be the same length\n");
      throw TATCException(
            "ERROR: Cone and clock angle vectors must be the same length\n");
   }
   
   else if (numFOVPoints <= 2)
   {
      MessageInterface::ShowMessage(
            "CustomSensor: must have at least 3 points to form valid FOV\n");
      throw TATCException("ERROR: must have 3 points to form valid FOV\n");
   }
   
   else
      for (int i = 0; i < numFOVPoints; i++)
      {
         // avoid singlularity in stereographic projection
         //when input is -Z axis of sensor frame
         if (coneAngleVecIn(i) > PI - 100 * REAL_TOL)
         {
            MessageInterface::ShowMessage(
              "CustomSensor: must have cone angle < Pi to avoid singularity\n");
            throw TATCException(
                  "ERROR: must have cone angle < Pi to avoid singularity");
         }
      }

   // we're good with the input values
   
   // set size of arrays for computed stereographic projections
   xProjectionCoordArray.SetSize(numFOVPoints);
   yProjectionCoordArray.SetSize(numFOVPoints);
   
   // set size of array of line segments connecting points in
   // stereographic projection
   segmentArray.SetSize(numFOVPoints,4);
   
   // set basic member data items
   coneAngleVec = coneAngleVecIn;
   clockAngleVec = clockAngleVecIn;
   maxExcursionAngle = Max(coneAngleVec); // in superclass Sensor
   
   // Perform initial computations
   
   //initialize the projection of the FOV arrays
   ConeClockArraysToStereographic (coneAngleVec, clockAngleVec,
                                   xProjectionCoordArray,
                                   yProjectionCoordArray);
   //create bounding box for first test of whether points are in FOV
   maxXExcursion = Max(xProjectionCoordArray);
   minXExcursion = Min(xProjectionCoordArray);
   maxYExcursion = Max(yProjectionCoordArray);
   minYExcursion = Min(yProjectionCoordArray);
   
   // Matlab ComputeLineSegments() only called points to segments,
   // this is done in constructor instead
   
   // compute line segments from stereographic projections
   segmentArray = PointsToSegments(xProjectionCoordArray,
                                   yProjectionCoordArray);
   
   // numTestPoints is hardcoded in ComputeExternalPoints() Matlab code, may
   // want to make it an input parameter to the constructor if this can vary
   numTestPoints = 3;
   ComputeExternalPoints();
   #ifdef DEBUG_CUSTOM_SENSOR
      MessageInterface::ShowMessage(
                        "DEBUG: Exiting CustomSensor constructor\n\n");
   #endif
       
} // constructor

//------------------------------------------------------------------------------
// CustomSensor(const CustomSensor &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor 
 *
 * @param copy object to copy
 */
//------------------------------------------------------------------------------
CustomSensor::CustomSensor(const CustomSensor &copy) :
   Sensor(copy)
{
   numFOVPoints            = copy.numFOVPoints;
   coneAngleVec            = copy.coneAngleVec;
   clockAngleVec           = copy.clockAngleVec;
   xProjectionCoordArray   = copy.xProjectionCoordArray;
   yProjectionCoordArray   = copy.yProjectionCoordArray;
   segmentArray            = copy.segmentArray;
   numTestPoints           = copy.numTestPoints;
   externalPointArray      = copy.externalPointArray;
   maxXExcursion           = copy.maxXExcursion;
   minXExcursion           = copy.minXExcursion;
   maxYExcursion           = copy.maxYExcursion;
   minYExcursion           = copy.minYExcursion;
   
}
   
//------------------------------------------------------------------------------
// CustomSensor& operator= (const CustomSensor &copy)
//------------------------------------------------------------------------------
/**
 * operator= for CustomSensor
 *
 * @param copy object to copy
 *
 */
//------------------------------------------------------------------------------
CustomSensor& CustomSensor::operator= (const CustomSensor &copy)
{
   Sensor::operator=(copy);
   
   if (&copy == this)
      return *this;
   
   numFOVPoints            = copy.numFOVPoints;
   coneAngleVec            = copy.coneAngleVec;
   clockAngleVec           = copy.clockAngleVec;
   xProjectionCoordArray   = copy.xProjectionCoordArray;
   yProjectionCoordArray   = copy.yProjectionCoordArray;
   segmentArray            = copy.segmentArray;
   numTestPoints           = copy.numTestPoints;
   externalPointArray      = copy.externalPointArray;
   maxXExcursion           = copy.maxXExcursion;
   minXExcursion           = copy.minXExcursion;
   maxYExcursion           = copy.maxYExcursion;
   minYExcursion           = copy.minYExcursion;
   
   return *this;
}

//------------------------------------------------------------------------------
// ~CustomSensor ()
//------------------------------------------------------------------------------
//   Destructor
//------------------------------------------------------------------------------
CustomSensor::~CustomSensor()
{
}

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// bool CheckTargetVisibility(Real viewConeAngle, Real viewClockAngle)
//------------------------------------------------------------------------------
/*
 * determines if point represented by a cone angle and a clock angle is in the
 * sensor's field of view, returns true if this is so
 *
 * @param   viewConeAngle     cone angle for point being tested (rad)
 * @param   viewClockAngle    clock angle for point being tested (rad)
 * @return  returns true if point is within sensor field of view
 *
 */
//------------------------------------------------------------------------------
bool CustomSensor::CheckTargetVisibility(Real viewConeAngle,
                                         Real viewClockAngle)
{
   #ifdef DEBUG_CHECK_TARGET
      MessageInterface::ShowMessage(
                        "In CustomSensor::CheckTargetVisibility:\n");
      MessageInterface::ShowMessage(
                        "   viewConeAngle  = %12.10f\n", viewConeAngle);
      MessageInterface::ShowMessage(
                        "   viewClockAngle = %12.10f\n", viewClockAngle);
   #endif
   // declare data needed for fast checks, and perhaps beyond
   bool possiblyInView = true;
   Real xCoord, yCoord;
   ConeClockToStereographic (viewConeAngle,viewClockAngle,xCoord,yCoord);
   
   // first check if in view cone, if so check stereographic box
   if (!CheckTargetMaxExcursionAngle(viewConeAngle))
      possiblyInView = false;
   else if (!CheckTargetMaxExcursionCoordinates(xCoord,yCoord))
      possiblyInView = false;
   
   // we've executed the quick tests, if point is possibly in the FOV
   // then run a line intersection test to determine if it is or not
   bool inView;
   if (!possiblyInView)
      inView = false;
   else
   {
      // outputs of LineSegmentIntersect
      Rmatrix distanceMatrix(numTestPoints,1); //used in validation
      std::vector<IntegerArray> adjacency; // used in counting crossings
      
      // outputs not used in visibility check
      Rmatrix huey(numTestPoints,1);
      Rmatrix dewey(numTestPoints,1);
      Rmatrix louie(numTestPoints,1);
      std::vector<IntegerArray> romulus, remus;
      
      // vectors of integer arrays have to be loaded to allocate memory
      // N - vector where each element is an integer array of 1 element
      IntegerArray row;
      row.push_back(0);
      for (int i=0; i<numTestPoints; i++)
      {
         adjacency.push_back(row);
         romulus.push_back(row);
         remus.push_back(row);
      }
      
      //  other declarations
      Real distance;
      Real distTol = 1.0e-12;
      Rmatrix lineSeg(1,4);
      bool foundValidPoint = false;
      
      // valid point test:
      // See if there is at least 1 valid external point
      for (int i = 0; i < numTestPoints; i++)
      {
         //lineSeg = (xCoord, yCoord, externalPointArray.GetElement(i,0),
         //           externalPointArray.GetElement(i,1));
         lineSeg.SetElement(0,0,xCoord);
         lineSeg.SetElement(0,1,yCoord);
         lineSeg.SetElement(0,2,externalPointArray.GetElement(i,0));
         lineSeg.SetElement(0,3,externalPointArray.GetElement(i,1));
         
         LinearAlgebra::LineSegmentIntersect(segmentArray,lineSeg, adjacency,
                                             huey, dewey, louie,
                                             distanceMatrix, romulus, remus);
         
         // loop exits on finding first valid point or finding none at all
         // distance matrix computed by LineSegmentIntersect is a
         // numFOVpoints x 1 matrix
         for (int j= 0; j < numFOVPoints; j++)
         {
            distance = distanceMatrix.GetElement(j,0);
            if (!(abs(distance)<=distTol || abs(distance-1.0)<=distTol))
            {
               foundValidPoint = true;
               break;
            }
         }
         if (foundValidPoint) break;
         
      } // valid point test
      
      // count crossings by iterating across the (numFOVpoints x 1)
      // adjacency matrix, use result to determine if point is in FOV
      if (foundValidPoint)
      {
         Integer numCrossings = 0;
         for (int i=0; i < numFOVPoints; i++)
            if (adjacency[i][0] == 1)
               numCrossings++;
         if (numCrossings%2==1)
            inView = true;
         else
            inView = false;
      }
      else
      {
         MessageInterface::ShowMessage
            ("Internal Error: No valid external point was found");
         inView=false;
      }
      
   } // line intersection test
   
   #ifdef DEBUG_CHECK_TARGET
      MessageInterface::ShowMessage(
                        "LEAVING CustomSensor::CheckTargetVisibility, "
                        "inView = %s\n", (inView? "true" : "false"));
   #endif
   return inView;
}

//------------------------------------------------------------------------------
// bool CheckRegionVisibility(const Rvector &coneAngleVec,
//                            const Rvector &clockAngleVec)
//------------------------------------------------------------------------------
/*
 * returns true if the region enclosed by line segments connecting successive
 * points represented by the ith and i+1st element of the input vectors is
 * fully within the sensor FOV
 *
 * @param   coneAngleVec   cone angles for points on region boundary  (rad)
 * @param   clockAngVec    clock angles for points on region boundary (rad)
 * @return  returns true if region is fully in FOV, false otherwise
 *
 */
//------------------------------------------------------------------------------
bool CustomSensor::CheckRegionVisibility(const Rvector &coneAngleVec,
                                         const Rvector &clockAngleVec)
{
   #ifdef DEBUG_CUSTOM_SENSOR
      MessageInterface::ShowMessage(
                        "DEBUG: Entering CheckRegionVisibility()\n");
   #endif
   // linesegArray computed frominput cone and clock angles represents region
   // object state segmentArray represents sensor FOV
   
   Integer size = coneAngleVec.GetSize();
   IntegerArray row(size,0);
   
   #ifdef DEBUG_CUSTOM_SENSOR
      MessageInterface::ShowMessage("DEBUG:region defined by %d points\n",size);
   #endif
   
   //unused outputs of line intersection calculations
   Rmatrix matrixX(numFOVPoints,size);
   Rmatrix matrixY(numFOVPoints,size);
   Rmatrix d1To2(numFOVPoints,size);
   Rmatrix d2To1(numFOVPoints,size);
   std::vector<IntegerArray> parallel(numFOVPoints,row);
   std::vector<IntegerArray> coincident(numFOVPoints,row);
   
   // adjacency matrix indicating pairs of lines that intersect
   std::vector<IntegerArray> adjacency(numFOVPoints,row);
   
   //compute line segments for region
   Rvector xCoords(size), yCoords(size);
   Rmatrix lineSegArray;
   
   //MessageInterface::ShowMessage("REGION: to stereographic\n");
   
   ConeClockArraysToStereographic (coneAngleVec,clockAngleVec,
                                   xCoords,yCoords);
 
   #ifdef DEBUG_CUSTOM_SENSOR
      MessageInterface::ShowMessage("DEBUG: computed stereographic values:\n");
      for (int i = 0; i < size; i++)
         MessageInterface::ShowMessage("x= %13.10f, y = %13.10f\n",
                                       xCoords[i], yCoords[i]);
      MessageInterface::ShowMessage(
                        "DEBUG: converting points to lineSegArray\n");
   #endif
   
   lineSegArray = PointsToSegments(xCoords,yCoords);
   
   #ifdef DEBUG_CUSTOM_SENSOR
      MessageInterface::ShowMessage("DEBUG: computed result:\n");
      
      for (int i = 0; i < size; i++)
         MessageInterface::ShowMessage("(%13.10f %13.10f to %13.10f %13.10f)\n",
                                       lineSegArray.GetElement(i,0),
                                       lineSegArray.GetElement(i,1),
                                       lineSegArray.GetElement(i,2),
                                       lineSegArray.GetElement(i,3));
      MessageInterface::ShowMessage( "DEBUG: calling intersect routine\n");
   #endif
   
   // get adjacency matrix for containment test
   LinearAlgebra::LineSegmentIntersect (segmentArray,lineSegArray, adjacency,
                                        matrixX, matrixY, d1To2, d2To1,
                                        parallel, coincident);
   #ifdef DEBUG_CUSTOM_SENSOR
      MessageInterface::ShowMessage("DEBUG:Returned from intersect routine\n");
      MessageInterface::ShowMessage("   Evaluating RegionIsFullyContained()\n");
      MessageInterface::ShowMessage("   Then exiting CheckRegionVisibility()\n");
   #endif
   
   if (RegionIsFullyContained(adjacency) )
      return true;
   else
      return false;

}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// bool CheckTargetMaxExcursionCoordinates(Real xCoord, Real yCoord)
//------------------------------------------------------------------------------
/*
 * returns true if coordinates are in box defined by min and max
 * excursion each direction, false if point is outside that box.
 *  Provides quick and simple way to reject points.
 *
 * @param xCoord x coordinate for point being tested
 * @param yCoord y coordinate for point being tested
 */
//------------------------------------------------------------------------------
bool CustomSensor::CheckTargetMaxExcursionCoordinates(Real xCoord, Real yCoord)
{
   // first assume point is in bounding box
   bool possiblyInView =  true;
   
   // apply falsifying logic
   if (xCoord > maxXExcursion)
      possiblyInView = false;
   else if (xCoord < minXExcursion)
      possiblyInView = false;
   else if (yCoord > maxYExcursion)
      possiblyInView = false;
   else if (yCoord < minYExcursion)
      possiblyInView = false;
   
   return possiblyInView;
}



//------------------------------------------------------------------------------
// Rmatrix PointsToSegments(const Rvector &xCoords, const Rvector &yCoords)
//------------------------------------------------------------------------------
/*
 * Given a vector of N x coordinates and N y coordinates, with [x(i),y(i)]
 * representing a point, creates an Nx4 matrix with each row representing the
 * (x,y) pair for two successive points in the vectors and also the line
 * segment connecting these points. In the current implementation these points
 * are stereographic projections of vectors on the unit sphere onto a plane.
 *
 * @param   xCoords  vector of x coordinates (N elements)
 * @param   yCoords  vector of y coordinates (N elements)
 * @return  returns an Nx4 matrix representing N line segments connecting
 *          successive points to form a closed region.
 */
//------------------------------------------------------------------------------
Rmatrix CustomSensor::PointsToSegments(const Rvector &xCoords,
                                       const Rvector &yCoords)
{
   Integer size = xCoords.GetSize();
   Rmatrix lineSegArray(size,4);
   
   // convert arrays of points represented by xy stereographic coordinates
   // to an array of line segments connecting two consecutive points
   for (int i = 0; i < size - 1; i++)
   {
      lineSegArray.SetElement(i,0,xCoords[i]);
      lineSegArray.SetElement(i,1,yCoords[i]);
      lineSegArray.SetElement(i,2,xCoords[i+1]);
      lineSegArray.SetElement(i,3,yCoords[i+1]);
      
   }
   
   // form last segment by connecting end of xCoords,yCoords
   // to the beginning
   int last = size - 1;
   lineSegArray.SetElement(last,0,xCoords[last]);
   lineSegArray.SetElement(last,1,yCoords[last]);
   lineSegArray.SetElement(last,2,xCoords[0]);
   lineSegArray.SetElement(last,3,yCoords[0]);
   
   return lineSegArray;
}

//------------------------------------------------------------------------------
// void ComputeExternalPoints()
//------------------------------------------------------------------------------
/*
 * Computes points external to FOV to use in visibility tests
 */
//------------------------------------------------------------------------------
void CustomSensor::ComputeExternalPoints()
{
   #ifdef DEBUG_CUSTOM_SENSOR
      MessageInterface::ShowMessage(
                        "DEBUG: Entering ComputeExternalPoints()\n");
   #endif
   
   //setup output array
   Rvector coneAngles(numFOVPoints);
   Rvector clockAngles(numFOVPoints);
   Rvector xCoords(numFOVPoints);
   Rvector yCoords(numFOVPoints);
   Rmatrix externalPoints;
   
   // check to see if all points in stereographic projection
   // are inside unit circle, if so create 3 random points outside unit circle
   Rvector pointDist(numFOVPoints);
   for (int i=0; i < numFOVPoints; i++)
      pointDist[i] = sqrt(xProjectionCoordArray[i] * xProjectionCoordArray[i] +
                          yProjectionCoordArray[i] * yProjectionCoordArray[i]);
                          
   #ifdef DEBUG_CUSTOM_SENSOR
      MessageInterface::ShowMessage("DEBUG: max distance from center = %f\n",
                                    Max(pointDist));
   #endif
   
   Integer numCandidatePoints = 0;
   Rvector xCandidatePoints(numFOVPoints);
   Rvector yCandidatePoints(numFOVPoints);
   Rvector v1(2), v2(2);
   Real interiorAngle;
   Real safetyFactor = 1.1;
   Rvector candidateConeAngles; // will be set using count of candidates
   IntegerArray indices; // will also be set using count of candidates
   
   // loop over points and compute internal angle at vertex j
   for (int i = 0; i < numFOVPoints; i++)
   {
      Integer j,k;
      // test for wrap-around of indices
      if (i <= (numFOVPoints-1)-2)  // add -1 so can compare with MATLAB
      {
         j = i+1;
         k = j+1;
      }
      else if (i <= (numFOVPoints-1)-1) // add -1 so can compare with MATLAB
      {
         j = numFOVPoints-1; // last point in array range
         k = 0;              // wrap around
      }
      else //i == numFOVPoints - 1, last point in array wrap around
      {
         j = 0;
         k = 1;
      }
      #ifdef DEBUG_CUSTOM_SENSOR
         MessageInterface::ShowMessage("DEBUG:(i,j,k)= (%d %d %d)\n", i,j,k);
      #endif
      
      // compute internal angles
      v1[0] = xProjectionCoordArray[j]-xProjectionCoordArray[i];
      v1[1] = yProjectionCoordArray[j]-yProjectionCoordArray[i];
      v2[0] = xProjectionCoordArray[k]-xProjectionCoordArray[j];
      v2[1] = yProjectionCoordArray[k]-yProjectionCoordArray[j];
      interiorAngle = Mod (ATan2(v2[1],v2[0]), 2.0*PI) -
                      Mod (ATan2(v1[1],v1[0]), 2.0*PI);
      interiorAngle = Mod (interiorAngle, 2.0*PI);

      #ifdef DEBUG_CUSTOM_SENSOR
         MessageInterface::ShowMessage("DEBUG: interior angle = %f\n",
                                       interiorAngle);
      #endif
      
      // see if this is a candidate point
      if (interiorAngle <= PI)
      {
         numCandidatePoints++;
         xCandidatePoints[numCandidatePoints-1] =
         xProjectionCoordArray[j];
         yCandidatePoints[numCandidatePoints-1] =
         yProjectionCoordArray[j];
         coneAngles[numCandidatePoints-1] = coneAngleVec[j];
      }
   } // for
   
   #ifdef DEBUG_CUSTOM_SENSOR
      MessageInterface::ShowMessage("DEBUG: %d candidate points\n",
                                    numCandidatePoints);
   #endif
   
   // sort the candidate points in order of decreasing cone angle
   // and use three largest cone angles as the test points
   
   // check to see how array slices work to see if loop is needed here
   candidateConeAngles.SetSize(numCandidatePoints);
   for (int i = 0; i < numCandidatePoints; i++)
      candidateConeAngles[i] = coneAngles[i];
   
   // false == input defining descending order as not ascending order
   Sort(candidateConeAngles,indices,false);
   
   #ifdef DEBUG_CUSTOM_SENSOR
      MessageInterface::ShowMessage("DEBUG: Sorted candidate angles\n");
      for (int i = 0; i < numCandidatePoints; i++)
            MessageInterface::ShowMessage("DEBUG: value, index = (%f, %d)\n",
                                          candidateConeAngles[i],i);
   #endif

   if (numCandidatePoints < numTestPoints)
      numTestPoints = numCandidatePoints;
   
   externalPoints.SetSize(numTestPoints,2);
   for (int i = 0; i < numTestPoints; i++)
   {
      int index = indices[i];
      externalPoints.SetElement(i,0,safetyFactor*xCandidatePoints[index]);
      externalPoints.SetElement(i,1,safetyFactor*yCandidatePoints[index]);
   }

   
   externalPointArray = externalPoints;
   
   #ifdef DEBUG_CUSTOM_SENSOR
      MessageInterface::ShowMessage("DEBUG: Exiting ComputeExternalPoints\n");
   #endif
}

//------------------------------------------------------------------------------
// bool RegionIsFullyContained (std::vector<IntegerArray &adjacency);
//------------------------------------------------------------------------------
/*
 * Determines whether an adajency matrix indicates that a region is
 * fully contained within the sensor FOV, used by checkRegionVisibility()
 *
 * @param adjacency     [input] entry (i,j) is 1 if line segments XY1(i,*)
 *                      and XY2(j,*)intersect; 0 otherwise
 * @return              returns true if region is completely in FOV,
 *                      false otherwise
 */
//------------------------------------------------------------------------------
bool CustomSensor::RegionIsFullyContained(std::vector<IntegerArray> &adjacency)
{
   //assume no intersecting lines, then look to see if there are any
   bool isFullyContained = true;
   
   Integer nRows = adjacency.size();
   Integer nCols;
   IntegerArray row;
   
   #ifdef DEBUG_CUSTOM_SENSOR
      MessageInterface::ShowMessage("DEBUG: Adjacency being tested\n");
      for (int i = 0; i < nRows; i++)
      {
         row=adjacency[i];
         nCols=row.size();
         for (int j = 0; j < nCols; j++)
            MessageInterface::ShowMessage("%d ",row[j]);
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif
   
   // test to see if there is any line crossing
   for (int i = 0; i < nRows; i++)
   {
      row = adjacency[i];
      nCols = row.size();
      for (int j = 0; j < nCols; j++)
      {
         if (row[j] == 1)
         {
            isFullyContained = false;
            #ifdef DEBUG_CUSTOM_SENSOR
               MessageInterface::ShowMessage(
                                 "found a crossing at index %d,%d\n",i,j);
            #endif

            break; // does this exit j-loop ohly or both i & j loops
         }
         if (!isFullyContained)
            break;
      }
   }
   return isFullyContained;
}

// internal utilities for Rvector and coordinate conversion

// Rvector utilities

//------------------------------------------------------------------------------
// void Sort(Rvector &v, bool ascending = true)
//------------------------------------------------------------------------------
/*
 * sorts an rVector into ascending or descending order, in place
 * NOTE: THIS COULD BE MOVED INTO RVECTOR ITSELF AS A MEMBER FUNCTION
 *
 * @param   v  vector to be sorted
 * @param   Ascending indicates ascending sort if true, descending if false
 *
 */
//------------------------------------------------------------------------------
void CustomSensor::Sort(Rvector &v, bool ascending)
{
   // load list with Rvector & sort it
   Integer size = v.GetSize();
   std::list<Real> rList;
   for (int i = 0; i < size; i++)
      rList.push_back(v[i]);
   rList.sort();
   
   // move back to Rvector in either ascending or descending order
   if (ascending)
      for (int i = 0; i < size; i++)
      {
         v[i] = rList.front();
         rList.pop_front();
      }
   else // descending
      for (int i = 0; i < size; i++)
      {
         v[i] = rList.back();
         rList.pop_back();
      }
}


//------------------------------------------------------------------------------
// void Sort(Rvector &v, IntegerArray &indices, bool ascending = true)
//------------------------------------------------------------------------------
/*
 * sorts an rVector into ascending or descending order, in place
 * NOTE: THIS COULD BE MOVED INTO RVECTOR ITSELF AS A MEMBER FUNCTION
 *
 * @param   v  vector to be sorted
 * @param   indices  array of pre-sort indices of sorted vector
 * @param   Ascending indicates ascending sort if true, descending if false
 *
 */
//------------------------------------------------------------------------------
void CustomSensor::Sort(Rvector &v, IntegerArray &indices, bool ascending)
{
   Integer size = v.GetSize();
   indices.resize(size);
   SensorElement e;
   std::list<SensorElement> eList;
   eList.clear();
   
   // load list with elements - real value from Rvector and corresponding index
   for (int i = 0; i<size; i++)
   {
      e.value = v[i];
      e.index = i;
      eList.push_back(e);
   }

   // sort with std::list sort function; providing necessary compare function
   eList.sort(CompareSensorElements);
   
   // eList now is ascending order, pull data off appropriate end of the list
   if (ascending)
   {
      for (int i = 0; i < size; i++)
      {
         e = eList.front();
         v[i] = e.value;
         indices[i] = e.index;
         eList.pop_front();
      }
   }
   else // descending, pull highest values off of back to load v
   {
      for (int i = 0; i < size; i++)
      {
         e = eList.back();
         v[i] = e.value;
         indices[i] = e.index;
         eList.pop_back();
      }
   }
}

//------------------------------------------------------------------------------
// Real Max(const Rvector &v)
//------------------------------------------------------------------------------
/**
 * returns maximum value from an Rvector
 *
 * @param v vector containing real values to select maximum from
 * @return  maximum value contained in vector
 */
//------------------------------------------------------------------------------
Real CustomSensor::Max(const Rvector &v)
{
   Real maxval = v[0];
   Integer N = v.GetSize();
   for (int i = 0; i < N; i++)
      if (v[i] > maxval)
         maxval = v[i];
   return maxval;
}

//------------------------------------------------------------------------------
// Real Min(const Rvector &v)
//------------------------------------------------------------------------------
/**
 * returns minimum value from an Rvector
 *
 * @param v vector containing real values to select minimum from
 * @return  minimum value contained in vector
 */
//------------------------------------------------------------------------------
Real CustomSensor::Min(const Rvector &v)
{
   Real minval = v[0];
   Integer N = v.GetSize();
   for (int i = 0; i < N; i++)
      if (v[i] < minval)
         minval = v[i];
   return minval;
}


//------------------------------------------------------------------------------
// bool CompareSensorElements(SensorElement e1, SensorElement e2)
//------------------------------------------------------------------------------
/**
 * compares 2 elements using ordering of values
 *
 * @param e1   first element in comparison
 * @param e2   second element in comparison
 * @return     true if value of e1 is less than value of e2, false otherwise
 *
 */
//------------------------------------------------------------------------------
bool CompareSensorElements(const SensorElement &e1, const SensorElement &e2)
{
   return e1.value < e2.value;
}
