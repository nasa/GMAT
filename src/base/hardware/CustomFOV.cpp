//------------------------------------------------------------------------------
//                           CustomFOV
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Syeda Kazmi, NASA/GSFC
// Created: 2019.04.03
//
/**
 * Implementation of the CustomFOV class
 */
 //------------------------------------------------------------------------------

#include <list>
#include <iostream> // for messages, **REPLACE** with appropriate calls *****
#include <fstream>
#include <sstream>
#include "utildefs.hpp"
#include "CustomFOV.hpp"
#include "MessageInterface.hpp"
#include "FieldOfViewException.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "Mod.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"
#include "StringUtil.hpp"

//#define DEBUG_LINE_SEGMENT
//#define DEBUG_CUSTOM_FOV

const std::string
CustomFOV::PARAMETER_TEXT[CustomFOVParamCount - FieldOfViewParamCount] =
{
  "FOVFileName",
  "ConeAngles",
  "ClockAngles",
  "Azimuth",
  "Elevation",
  "InterpolationStepSize",
};


/// Types of the parameters used by field of view.
const Gmat::ParameterType
CustomFOV::PARAMETER_TYPE[CustomFOVParamCount - FieldOfViewParamCount] =
{
   Gmat::FILENAME_TYPE,     // FILENAME
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::REAL_TYPE,
};
//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CustomFOV(Rvector coneAngleVecIn, clockAngleVecIn)
//------------------------------------------------------------------------------

CustomFOV::CustomFOV(const std::string &itsName) :
   FieldOfView(Gmat::CUSTOM_FOV, "CustomFOV", itsName),
   numFOVPoints(0),
   numTestPoints(0),
   maxXExcursion (0),
   minXExcursion (0),
   maxYExcursion (0),
   minYExcursion(0),
   fovFileName  (""),
   isInitialized (false)
{
   parameterCount = CustomFOVParamCount;
   objectTypes.push_back(Gmat::CUSTOM_FOV);
   objectTypeNames.push_back("CustomFOV"); 
   // We need to tell the FileManager the default location for this type of file
   FileManager::Instance()->AddFileType("FOV_DATA_PATH", "../data/hardware");
   
   // Set defaults for the cone and clock angles
   coneAngleVec.SetSize(1);
   coneAngleVec[0] = 30.0 * GmatMathConstants::RAD_PER_DEG;
   maxExcursionAngle =  coneAngleVec[0];
   clockAngleVec.SetSize(1);
   clockAngleVec[0] = 0.0;

   #ifdef DEBUG_CUSTOM_FOV
      MessageInterface::ShowMessage("CREATED CustomFOV with data path\n");
   #endif
} // constructor

//------------------------------------------------------------------------------
// CustomFOV(const CustomFOV &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param copy object to copy
 */
 //------------------------------------------------------------------------------
CustomFOV::CustomFOV(const CustomFOV &copy) :
   FieldOfView(copy)
{
   numFOVPoints = copy.numFOVPoints;
   if (copy.coneAngleVec.IsSized())
   {
	   Integer cone = copy.coneAngleVec.GetSize();
	   coneAngleVec.SetSize(cone);
      coneAngleVec = copy.coneAngleVec;
   }
   if (copy.clockAngleVec.IsSized())
   {
	   Integer clock = copy.clockAngleVec.GetSize();
      clockAngleVec.SetSize(clock);
      clockAngleVec = copy.clockAngleVec;
   }
   if (copy.xProjectionCoordArray.IsSized())
   {
	   Integer x = copy.xProjectionCoordArray.GetSize();
	   xProjectionCoordArray.SetSize(x);
	   xProjectionCoordArray = copy.xProjectionCoordArray;
   }
   if (copy.yProjectionCoordArray.IsSized())
   {
	   Integer y = copy.yProjectionCoordArray.GetSize();
	   yProjectionCoordArray.SetSize(y);
      yProjectionCoordArray = copy.yProjectionCoordArray;
   }
   if (copy.segmentArray.IsSized())
   {
	   Integer s, a;
       copy.segmentArray.GetSize(s, a);
       segmentArray.SetSize(s, a);
       segmentArray = copy.segmentArray;
   }
   numTestPoints = copy.numTestPoints;
   if (copy.externalPointArray.IsSized())
   {
	   Integer r, c;
	   copy.externalPointArray.GetSize(r, c);
      externalPointArray.SetSize(r, c);
      externalPointArray = copy.externalPointArray;
   }
   maxXExcursion = copy.maxXExcursion;
   minXExcursion = copy.minXExcursion;
   maxYExcursion = copy.maxYExcursion;
   minYExcursion = copy.minYExcursion;
   fovFileName   = copy.fovFileName;
   interpolatePoints = copy.interpolatePoints;
   isClockCone = copy.isClockCone;
   interpolationSize = copy.interpolationSize;
   pointsInterpolated = copy.pointsInterpolated;
   isInitialized = copy.isInitialized;
}

//------------------------------------------------------------------------------
// CustomFOV& operator= (const CustomFOV &copy)
//------------------------------------------------------------------------------
/**
 * operator= for CustomFOV
 *
 * @param copy object to copy
 *
 */
 //------------------------------------------------------------------------------
CustomFOV& CustomFOV::operator= (const CustomFOV &copy)
{
	FieldOfView::operator=(copy);
	if (&copy == this)
		return *this;

   numFOVPoints = copy.numFOVPoints;
   if (copy.coneAngleVec.IsSized())
   {
	   Integer cone = copy.coneAngleVec.GetSize();
      coneAngleVec.SetSize(cone);
      coneAngleVec = copy.coneAngleVec;
   }
   if (copy.clockAngleVec.IsSized())
   {
	   Integer clock = copy.clockAngleVec.GetSize();
      clockAngleVec.SetSize(clock);
      clockAngleVec = copy.clockAngleVec;
   }
   if (copy.xProjectionCoordArray.IsSized())
   {
	   Integer x = copy.xProjectionCoordArray.GetSize();
      xProjectionCoordArray.SetSize(x);
      xProjectionCoordArray = copy.xProjectionCoordArray;
   }
   if (copy.yProjectionCoordArray.IsSized())
   {
	   Integer y = copy.yProjectionCoordArray.GetSize();
      yProjectionCoordArray.SetSize(y);
      yProjectionCoordArray = copy.yProjectionCoordArray;
   }
   if (copy.segmentArray.IsSized())
   {
	   Integer s, a;
      copy.segmentArray.GetSize(s, a);
      segmentArray.SetSize(s, a);
      segmentArray = copy.segmentArray;
   }
   numTestPoints = copy.numTestPoints;
   if (copy.externalPointArray.IsSized())
   {
	   Integer r, c;
      copy.externalPointArray.GetSize(r, c);
      externalPointArray.SetSize(r, c);
      externalPointArray = copy.externalPointArray;
   }
   maxXExcursion = copy.maxXExcursion;
   minXExcursion = copy.minXExcursion;
   maxYExcursion = copy.maxYExcursion;
   minYExcursion = copy.minYExcursion;
   fovFileName   = copy.fovFileName;
   interpolatePoints = copy.interpolatePoints;
   interpolationSize = copy.interpolationSize;
   pointsInterpolated = copy.pointsInterpolated;
   isClockCone = copy.isClockCone;
   isInitialized = copy.isInitialized;
	return *this;
}

//------------------------------------------------------------------------------
// ~CustomFOV ()
//------------------------------------------------------------------------------
//   Destructor
//------------------------------------------------------------------------------
CustomFOV::~CustomFOV()
{

}
//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool CustomFOV::Initialize()
{

   #ifdef DEBUG_CUSTOM_FOV
      MessageInterface::ShowMessage(
                        "DEBUG: Entering CustomFOV::Initialize()\n\n");
   #endif
   
      if (!pointsInterpolated)
      {
         // Check for failure conditions
         if ((fovFileName != "") && (clockAngleVec.GetSize() > 1 || coneAngleVec.GetSize() > 1 ))
         {
            throw FieldOfViewException("ERROR: The CustomFOV " + GetName() + " may be defined with either with a FOVFile or with explicit angles, but not both.");
         }
         if (fovFileName != "")
         {
            std::string fovFullPath = FileManager::Instance()->FindPath(fovFileName, "FOV_DATA_PATH",
               true, false, false, instanceName);
            ReadConeClockAngles(fovFullPath);
         }
         if (clockAngleVec.GetSize() != coneAngleVec.GetSize())
         {
            if (interpolatePoints)
            {
               throw FieldOfViewException("ERROR: The number of Azimuth angles, " + GmatStringUtil::RealToString(clockAngleVec.GetSize()) + ", does not match the number of Elevation angles, " + GmatStringUtil::RealToString(coneAngleVec.GetSize()) + ", on " + GetName());
            }
            else
            {
               throw FieldOfViewException("ERROR: The number of ClockAngles, " + GmatStringUtil::RealToString(clockAngleVec.GetSize()) + ", does not match the number of ConeAngles, " + GmatStringUtil::RealToString(coneAngleVec.GetSize()) + ", on " + GetName());
            }
         }

         if (clockAngleVec[0] == 0 && clockAngleVec[clockAngleVec.GetSize() - 1] == GmatMathConstants::TWO_PI)
         {
            if (coneAngleVec[0] != coneAngleVec[coneAngleVec.GetSize() - 1])
            {
               throw FieldOfViewException("ERROR: There is a discontinuity at 0 and 360 degrees on "+ GetName());
            }
         }

         // Perform Spherical Interpolation if Az/El entered, or InterpolationSize set to non-zero value
         if (!isClockCone && interpolatePoints && !pointsInterpolated && interpolationSize != 0.0)
         {
            RealArray tempCones;
            RealArray tempClocks;
            RealArray tempBetweenClocks;
            RealArray tempBetweenCones;
            Real interpSteps = 0;
            Integer clockIdx = 0;

            while (interpSteps <= GmatMathConstants::TWO_PI)
            {
               // Always add truth points
               if (clockIdx < clockAngleVec.GetSize() && (clockAngleVec(clockIdx) == 0 || clockAngleVec(clockIdx) < interpSteps))
               {
                  if (tempClocks.size() > 0)
                  {
                     if (abs(tempClocks.back() - clockAngleVec[clockIdx]) > 1e-14)
                     {
                        tempClocks.push_back(clockAngleVec[clockIdx]);
                        tempCones.push_back(coneAngleVec[clockIdx]);
                     }
                     else
                     {
                        tempClocks.back() = (clockAngleVec[clockIdx]);
                        tempCones.back() = (coneAngleVec[clockIdx]);
                     }
                  }
                  else
                  {
                     tempClocks.push_back(clockAngleVec[clockIdx]);
                     tempCones.push_back(coneAngleVec[clockIdx]);
                  }
                  clockIdx++;
               }
               // Add interpolated points
               else
               {
                  Real coneStep = 0;
                  if (clockIdx == 0)
                  {
                     coneStep = coneAngleVec[clockAngleVec.GetSize() - 1] + (interpSteps + GmatMathConstants::TWO_PI - clockAngleVec[clockAngleVec.GetSize() - 1])*((coneAngleVec[0] - coneAngleVec[clockAngleVec.GetSize() - 1]) / (clockAngleVec[0] + GmatMathConstants::TWO_PI - clockAngleVec[clockAngleVec.GetSize() - 1]));
                  }
                  else if (clockIdx == clockAngleVec.GetSize())
                  {
                     coneStep = coneAngleVec[clockIdx - 1] + (interpSteps - clockAngleVec[clockIdx - 1])*((coneAngleVec[0] - coneAngleVec[clockIdx - 1]) / (clockAngleVec[0] + GmatMathConstants::TWO_PI - clockAngleVec[clockIdx - 1]));
                  }
                  else
                  {
                     coneStep = coneAngleVec[clockIdx - 1] + (interpSteps - clockAngleVec[clockIdx - 1])*((coneAngleVec[clockIdx] - coneAngleVec[clockIdx - 1]) / (clockAngleVec[clockIdx] - clockAngleVec[clockIdx - 1]));
                  }
                  if (tempClocks.size() > 0)
                  {
                     if (GmatMathUtil::Abs(tempClocks.back() - interpSteps) > 1e-14)
                     {
                        tempCones.push_back(coneStep);
                        tempClocks.push_back(interpSteps);
                     }
                  }
                  else
                  {
                     tempCones.push_back(coneStep);
                     tempClocks.push_back(interpSteps);
                  }
                  interpSteps = interpSteps + interpolationSize;
               }
            }

            coneAngleVec.SetSize(tempCones.size());
            clockAngleVec.SetSize(tempClocks.size());
            coneAngleVec = tempCones;
            clockAngleVec = tempClocks;
         }
         pointsInterpolated = true;
      }
   numFOVPoints = coneAngleVec.GetSize();
	// validate cone & clock angle inputs
   if (numFOVPoints != clockAngleVec.GetSize())
   {
      #ifdef DEBUG_CUSTOM_FOV
         MessageInterface::ShowMessage("CustomFOV: Cone & Clock angle vectors "
                                       "must be the same length\n");
      #endif
		throw FieldOfViewException(
            "ERROR: Cone and clock angle vectors must be the same length\n");
   }
   else if (numFOVPoints <= 2)
   {
      #ifdef DEBUG_CUSTOM_FOV
         MessageInterface::ShowMessage("CustomFOV: must have at least 3 points "
                                       "to form valid FOV\n");
      #endif
		throw FieldOfViewException(
            "ERROR: must have 3 points to form valid FOV\n");
   }
   else
   {
	   for (int i = 0; i < numFOVPoints; i++)
	   {
			// avoid singlularity in stereographic projection when input is -Z axis of FOV frame
		   if (coneAngleVec(i) > GmatMathConstants::PI - 100 * GmatRealConstants::REAL_TOL)
		   {
            #ifdef DEBUG_CUSTOM_FOV
               MessageInterface::ShowMessage(
                        "CustomFOV: must have cone angle < Pi to avoid "
                        "singularity\n");
            #endif
				throw FieldOfViewException(
                  "ERROR: must have cone angle < Pi to avoid singularity");
			}
		}
   }

	// we're good with the input values
	// set size of arrays for computed stereographic projections
   xProjectionCoordArray.SetSize(numFOVPoints);
   yProjectionCoordArray.SetSize(numFOVPoints);
	// set size of array of line segments connecting points in stereographic projection
   segmentArray.SetSize(numFOVPoints, 4);

	// set basic member data items
	
   maxExcursionAngle = Max(coneAngleVec); // in superclass FOV

	// Perform initial computations

	//initialize the projection of the FOV arrays
   ConeClockArraysToStereographic(coneAngleVec, clockAngleVec,xProjectionCoordArray,yProjectionCoordArray);
	//create bounding box for first test of whether points are in FOV
   maxXExcursion = Max(xProjectionCoordArray);
   minXExcursion = Min(xProjectionCoordArray);
   maxYExcursion = Max(yProjectionCoordArray);
   minYExcursion = Min(yProjectionCoordArray);

	// compute line segments from stereographic projections
   segmentArray = PointsToSegments(xProjectionCoordArray,yProjectionCoordArray);
	// numTestPoints is hardcoded in ComputeExternalPoints() Matlab code, may
	// want to make it an input parameter to the constructor if this can vary
   numTestPoints = 3;
   ComputeExternalPoints();

   #ifdef DEBUG_CUSTOM_FOV
      MessageInterface::ShowMessage(
                        "DEBUG: Exiting CustomFOV::Initialize()\n\n");
   #endif
   isInitialized = true;
   return true;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the object.
 *
 * @return clone of the object.
 */
//------------------------------------------------------------------------------
GmatBase* CustomFOV::Clone() const
{
	return (new CustomFOV(*this));
}

//------------------------------------------------------------------------------
// bool CheckTargetVisibility(Real viewConeAngle, Real viewClockAngle)
//------------------------------------------------------------------------------
/*
 * determines if point represented by a cone angle and a clock angle is in the
 * FieldOfView's field of view, returns true if this is so
 *
 * @param   viewConeAngle     cone angle for point being tested (rad)
 * @param   viewClockAngle    clock angle for point being tested (rad)
 * @return  returns true if point is within FieldOfView field of view
 *
 */
 //------------------------------------------------------------------------------
bool CustomFOV::CheckTargetVisibility(Real viewConeAngle,Real viewClockAngle)
{
	// declare data needed for fast checks, and perhaps beyond
   bool possiblyInView = true;
   Real xCoord, yCoord;
   ConeClockToStereographic(viewConeAngle, viewClockAngle, xCoord, yCoord);

	// first check if in view cone, if so check stereographic box
   if (!CheckTargetMaxExcursionAngle(viewConeAngle))
	   possiblyInView = false;
   else if (!CheckTargetMaxExcursionCoordinates(xCoord, yCoord))
	   possiblyInView = false;

	// we've executed the quick tests, if point is possibly in the FOV
	// then run a line intersection test to determine if it is or not
   bool inView;
   if (!possiblyInView)
	   inView = false;
   else
   {
	   // outputs of LineSegmentIntersect
	   Rmatrix distanceMatrix(numTestPoints, 1); //used in validation
	   std::vector<IntegerArray> adjacency; // used in counting crossings

		// outputs not used in visibility check
		Rmatrix huey(numTestPoints, 1);
		Rmatrix dewey(numTestPoints, 1);
		Rmatrix louie(numTestPoints, 1);
		std::vector<IntegerArray> romulus, remus;

		// vectors of integer arrays have to be loaded to allocate memory
		// N - vector where each element is an integer array of 1 element
		IntegerArray row;
		row.push_back(0);
		for (int i = 0; i < numTestPoints; i++)
		{
			adjacency.push_back(row);
			romulus.push_back(row);
			remus.push_back(row);
		}

		//  other declarations
		Real distance;
		Real distTol = 1.0e-12;
		Rmatrix lineSeg(1, 4);
		bool foundValidPoint = false;

		// valid point test:
		// See if there is at least 1 valid external point
		for (int i = 0; i < numTestPoints; i++)
		{
			// lineSeg = (xCoord, yCoord, externalPointArray.GetElement(i,0),
		   // externalPointArray.GetElement(i,1));
			lineSeg.SetElement(0, 0, xCoord);
			lineSeg.SetElement(0, 1, yCoord);
			lineSeg.SetElement(0, 2, externalPointArray.GetElement(i, 0));
			lineSeg.SetElement(0, 3, externalPointArray.GetElement(i, 1));
			LineSegmentIntersect(segmentArray, lineSeg, adjacency,huey, dewey,
                              louie, distanceMatrix, romulus, remus);

			// loop exits on finding first valid point or finding none at all
			// distance matrix computed by LineSegmentIntersect is a
			// numFOVpoints x 1 matrix
			for (int j = 0; j < numFOVPoints; j++)
			{
				distance = distanceMatrix.GetElement(j, 0);
				if (!(abs(distance) <= distTol || abs(distance - 1.0) <= distTol))
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
			for (int i = 0; i < numFOVPoints; i++)
				if (adjacency[i][0] == 1)
					numCrossings++;
			if (numCrossings % 2 == 1)
				inView = true;
			else
				inView = false;
		}
		else
		{
         #ifdef DEBUG_CUSTOM_FOV
            MessageInterface::ShowMessage(
                     "Internal Error: No valid external point was found");
         #endif // DEBUG_CUSTOM_FOV
			inView = false;
		}
   } // line intersection test
   return inView;
}

//------------------------------------------------------------------------------
// bool CheckTargetVisibility(const Rvector3 &target)
//------------------------------------------------------------------------------
/*
 *unit vector visibility check
 */
//------------------------------------------------------------------------------
bool CustomFOV::CheckTargetVisibility(const Rvector3 &target)
{
	Real RA, dec;
	Real cone, clock;
	UnitVecToRADEC(target, RA, dec);
	RADECtoConeClock(RA, dec, cone, clock);
	return CheckTargetVisibility(cone, clock);
}

//------------------------------------------------------------------------------
// bool CheckRegionVisibility(const Rvector &coneAngleVec,
//                            const Rvector &clockAngleVec)
//------------------------------------------------------------------------------
/*
 * returns true if the region enclosed by line segments connecting successive
 * points represented by the ith and i+1st element of the input vectors is
 * fully within the FieldOfView FOV
 *
 * @param   coneAngleVec   cone angles for points on region boundary  (rad)
 * @param   clockAngVec    clock angles for points on region boundary (rad)
 * @return  returns true if region is fully in FOV, false otherwise
 *
 */
 //------------------------------------------------------------------------------
bool CustomFOV::CheckRegionVisibility(const Rvector &coneAngleVector,const Rvector &clockAngleVector)
{
   #ifdef DEBUG_CUSTOM_FOV
      MessageInterface::ShowMessage(
                        "DEBUG: Entering CheckRegionVisibility()\n");
   #endif
	// linesegArray computed frominput cone and clock angles represents region
	// object state segmentArray represents FOV FOV

   Integer size = coneAngleVector.GetSize();
   IntegerArray row(size, 0);

   #ifdef DEBUG_CUSTOM_FOV
      MessageInterface::ShowMessage("DEBUG:region defined by %d points\n",
                                    size);
   #endif

	//unused outputs of line intersection calculations
   Rmatrix matrixX(numFOVPoints, size);
   Rmatrix matrixY(numFOVPoints, size);
   Rmatrix d1To2(numFOVPoints, size);
   Rmatrix d2To1(numFOVPoints, size);
   std::vector<IntegerArray> parallel(numFOVPoints, row);
   std::vector<IntegerArray> coincident(numFOVPoints, row);

	// adjacency matrix indicating pairs of lines that intersect
   std::vector<IntegerArray> adjacency(numFOVPoints, row);

	//compute line segments for region
   Rvector xCoords(size), yCoords(size);
   Rmatrix lineSegArray;

	//MessageInterface::ShowMessage("REGION: to stereographic\n");
   ConeClockArraysToStereographic(coneAngleVector, clockAngleVector, xCoords, yCoords);

   #ifdef DEBUG_CUSTOM_FOV
      MessageInterface::ShowMessage("DEBUG: computed stereographic values:\n");
      for (int i = 0; i < size; i++)
         MessageInterface::ShowMessage("x= %13.10f, y = %13.10f\n",xCoords[i],
                                       yCoords[i]);
          MessageInterface::ShowMessage(
                           "DEBUG: converting points to lineSegArray\n");
   #endif

   lineSegArray = PointsToSegments(xCoords, yCoords);

   #ifdef DEBUG_CUSTOM_FOV
      MessageInterface::ShowMessage("DEBUG: computed result:\n");
      for (int i = 0; i < size; i++)
         MessageInterface::ShowMessage("(%13.10f %13.10f to %13.10f %13.10f)\n",
            lineSegArray.GetElement(i, 0),
            lineSegArray.GetElement(i, 1),
            lineSegArray.GetElement(i, 2),
            lineSegArray.GetElement(i, 3));
      MessageInterface::ShowMessage("DEBUG: calling intersect routine\n");
   #endif

	// get adjacency matrix for containment test
	LineSegmentIntersect(segmentArray, lineSegArray, adjacency,matrixX, matrixY, d1To2, d2To1,
		                 parallel, coincident);
   #ifdef DEBUG_CUSTOM_FOV
      MessageInterface::ShowMessage("DEBUG:Returned from intersect routine\n");
      MessageInterface::ShowMessage("   Evaluating RegionIsFullyContained()\n");
      MessageInterface::ShowMessage(
                        "   Then exiting CheckRegionVisibility()\n");
   #endif
	if (RegionIsFullyContained(adjacency))
		return true;
	else
		return false;

}

//------------------------------------------------------------------------------
//  Rvector GetMaskConeAngles()
//------------------------------------------------------------------------------
/**
 * Returns the mask cone angles for the sensor
 *
 * @return array of mask cone angles
 */
//------------------------------------------------------------------------------
Rvector  CustomFOV::GetMaskConeAngles()
{
	return  coneAngleVec;

}

//------------------------------------------------------------------------------
//  Rvector GetMaskClockAngles()
//------------------------------------------------------------------------------
/**
 * Returns the mask clock angles for the sensor
 *
 * @return array of mask clock angles
 */
//------------------------------------------------------------------------------
Rvector  CustomFOV::GetMaskClockAngles()
{
	return  clockAngleVec;
}


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
 //------------------------------------------------------------------------------
std::string CustomFOV::GetParameterText(const Integer id) const
{
	if (id >= FieldOfViewParamCount && id < CustomFOVParamCount)
		return PARAMETER_TEXT[id - FieldOfViewParamCount];
	return FieldOfView::GetParameterText(id);
}
//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
 //------------------------------------------------------------------------------
Integer CustomFOV::GetParameterID(const std::string &str) const
{
	for (Integer i = FieldOfViewParamCount; i < CustomFOVParamCount; i++)
	{
		if (str == PARAMETER_TEXT[i - FieldOfViewParamCount])
			return i;
	}
	return FieldOfView::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
 //------------------------------------------------------------------------------
Gmat::ParameterType CustomFOV::GetParameterType(const Integer id) const
{
	if (id >= FieldOfViewParamCount && id < CustomFOVParamCount)
		return PARAMETER_TYPE[id - FieldOfViewParamCount];
	return FieldOfView::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
 //------------------------------------------------------------------------------
std::string CustomFOV::GetParameterTypeString(const Integer id) const
{
	return FieldOfView::PARAM_TYPE_STRING[GetParameterType(id)];
}
//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
 //------------------------------------------------------------------------------
Real CustomFOV::GetRealParameter(const Integer id, const Integer index) const
{
	if (id == CONE_ANGLES || id == FOV_ELEVATION)
	{
		if ((index >= 0) && (index < coneAngleVec.GetSize()))
      {
         Real value = coneAngleVec.GetElement(index) * GmatMathConstants::DEG_PER_RAD;
         Real tempValue = value;
         if (id == FOV_ELEVATION)
         {
            tempValue = 90 - value;
         }
         return tempValue;
      }
		else
			throw FieldOfViewException("Index out of bounds" +instanceName);
	}
	else if (id == CLOCK_ANGLES || id == FOV_AZIMUTH)
	{
		if ((index >= 0) && (index < clockAngleVec.GetSize()))
			return clockAngleVec.GetElement(index) * GmatMathConstants::DEG_PER_RAD;
		else
			throw FieldOfViewException("Index out of bounds " +instanceName);
	}
	return FieldOfView::GetRealParameter(id, index);
}

//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
 //------------------------------------------------------------------------------
Real CustomFOV::GetRealParameter(const Integer id) const
{
   if (id == INTERPOLATION_SIZE)
   {
      return interpolationSize * GmatMathConstants::DEG_PER_RAD;
   }
   return FieldOfView::GetRealParameter(id);
}
//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves real valued parameter data
 *
 * @param label The parameter script string
 *
 * @return The value of th4e vector element
 */
 //------------------------------------------------------------------------------
Real CustomFOV::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value,
//                       const Integer index)
//------------------------------------------------------------------------------
/**
 * Set real valued parameter data
 *
 * @param id        The parameter ID
 * @param value The new parameter element value
 *
 * @return The value after the setting has occurred
 */
 //------------------------------------------------------------------------------
Real CustomFOV::SetRealParameter(const Integer id, const Real value)
{
   if (id == INTERPOLATION_SIZE)
   {

      if ((value != 0.0) && (value < 0.2 || value > 180 ))
      {
         throw FieldOfViewException("ERROR: The InterpolationSize must be equal to or between 0.2 and 180 degrees, or exactly 0.0\n");
      }
      interpolationSize = value * GmatMathConstants::RAD_PER_DEG;
      interpolatePoints = true;
      return interpolationSize;
   }
   return FieldOfView::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Pass through method for setting a real parameter
 *
 * @param id The parameter ID
 * @param value The new parameter value
 *
 * @return The parameter value after the call
 */
 //------------------------------------------------------------------------------
Real CustomFOV::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves real valued parameter data from vector containers
 *
 * @param label The parameter script string
 * @param index The element index in the vector
 *
 * @return The value of th4e vector element
 */
 //------------------------------------------------------------------------------
Real CustomFOV::GetRealParameter(
                        const std::string &label, const Integer index) const
{
	return GetRealParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value,
//                       const Integer index)
//------------------------------------------------------------------------------
/**
 * Set real valued parameter data in vector containers
 *
 * @param id        The parameter ID
 * @param value The new parameter element value
 * @param index The element index in the vector
 *
 * @return The value after the setting has occurred
 */
 //------------------------------------------------------------------------------
Real CustomFOV::SetRealParameter(const Integer id, const Real value,
                                 const Integer index)
{
	if (id == CONE_ANGLES || id == FOV_ELEVATION)
	{
		// Assume that, if the index is zero, the user is resetting the
		// entire array (so override defaults or earlier set values)
		if (value >= 0.0)
		{
			if (index == 0)
				coneAngleVec.SetSize(0);

			Integer sz = 0;
			if (coneAngleVec.IsSized())
			{
				sz = coneAngleVec.GetSize();

				if ((index < 0) || (index > sz))
					throw FieldOfViewException(
						"Index out of bounds setting the cone angles on " +
						instanceName);

				if (index == sz)
				{
					// Rvector deletes array on resize; gymnastics required to compensate
					Rvector temp(coneAngleVec);
					coneAngleVec.SetSize(index + 1);
					for (Integer i = 0; i < temp.GetSize(); ++i)
						coneAngleVec[i] = temp[i];
				}
			}
         Real tempValue = value;
         if (id == FOV_ELEVATION)
         {
            if (value > 90)
            {
               throw FieldOfViewException("Error: The elevation value " +
                  GmatStringUtil::RealToString(value) + " is greater than 90 degrees");
            }
            else if (value < 0)
            {
               throw FieldOfViewException("Error: The elevation value " +
                  GmatStringUtil::RealToString(value) + " is less than 0 degrees");
            }

            tempValue = 90 - value;
            interpolatePoints = true;
            isClockCone = false;
         }
			coneAngleVec[index] = tempValue * GmatMathConstants::RAD_PER_DEG;
			return coneAngleVec[index];
		}
		else
		{
         if (id == FOV_ELEVATION)
         {
            FieldOfViewException fe("");
            fe.SetDetails(errorMessageFormat.c_str(),
               GmatStringUtil::ToString(value, 16).c_str(),
               "Elevation", "90.0 >= Real Number >= 0.0");
            throw fe;
         }
         else
         {
            FieldOfViewException fe("");
            fe.SetDetails(errorMessageFormat.c_str(),
               GmatStringUtil::ToString(value, 16).c_str(),
               "ConeAngles", "Real Number >= 0.0");
            throw fe;
         }

		}
	}
	else if (id == CLOCK_ANGLES || id == FOV_AZIMUTH)
	{
      // Assume that, if the index is zero, the user is resetting the
      // entire array (so override defaults or earlier set values)
		if (value >= 0.0)
		{
			if (index == 0)
				clockAngleVec.SetSize(0);

			Integer sz = 0;
			if (clockAngleVec.IsSized())
			{
				sz = clockAngleVec.GetSize();

				if ((index < 0) || (index > sz))
					throw FieldOfViewException(
						"Index out of bounds setting the clock angles on " +
						instanceName);

				if (index == sz)
				{
					// Rvector deletes array on resize; gymnastics required to compensate
					Rvector temp(clockAngleVec);
					clockAngleVec.SetSize(index + 1);
					for (Integer i = 0; i < temp.GetSize(); ++i)
						clockAngleVec[i] = temp[i];
				}
			}
         if (id == FOV_AZIMUTH)
         {
            interpolatePoints = true;
            isClockCone = false;
         }
			clockAngleVec[index] = value * GmatMathConstants::RAD_PER_DEG;
			return clockAngleVec[index];
		}
		else
		{
         if (id == FOV_AZIMUTH)
         {
            FieldOfViewException fe("");
            fe.SetDetails(errorMessageFormat.c_str(),
               GmatStringUtil::ToString(value, 16).c_str(),
               "Azimuth", "Real Number >= 0.0");
            throw fe;
         }
         else
         {
            FieldOfViewException fe("");
            fe.SetDetails(errorMessageFormat.c_str(),
               GmatStringUtil::ToString(value, 16).c_str(),
               "ClockAngles", "Real Number >= 0.0");
            throw fe;
         }
		}
   }
	return FieldOfView::SetRealParameter(id, value, index);
}
//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Pass through method for setting a real parameter
 *
 * @param id The parameter ID
 * @param value The new parameter value
 * @param index Array index for the parameter
 *
 * @return The parameter value after the call
 */
 //------------------------------------------------------------------------------
Real CustomFOV::SetRealParameter(const std::string &label, const Real value,
                                 const Integer index)
{
	return SetRealParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string CustomFOV::GetStringParameter(const Integer id) const
{
	switch (id)
	{
	case FILE_NAME:
		return fovFileName;
	default:
		return FieldOfView::GetStringParameter(id);
	}
}

//------------------------------------------------------------------------------
//  Real GetStringParameter(const std::string &label)
//------------------------------------------------------------------------------
std::string CustomFOV::GetStringParameter(const std::string &label) const
{
	return GetStringParameter(GetParameterID(label));
}

//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new string for this parameter.
 *
 * @return true if the string is stored, throw if the string is not stored.
 */
 //---------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CustomFOV::SetStringParameter(const Integer id, const std::string &value)
{
	switch (id)
	{
	case FILE_NAME:
		fovFileName = value;
		return true;
	default:
		return FieldOfView::SetStringParameter(id, value);
	}
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool CustomFOV::SetStringParameter(const std::string &label,
                                   const std::string &value)
{
	return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves real valued vector of data
 *
 * @param id The parameter ID
 *
 * @return The data vector
 */
 //------------------------------------------------------------------------------
const Rvector& CustomFOV::GetRvectorParameter(const Integer id) const
{
   static Rvector result;
   Integer sz = 0;
   if (id == CONE_ANGLES || id == FOV_ELEVATION)
   {
      if (!coneAngleVec.IsSized())
         result.SetSize(0);
      else
      {
         sz = coneAngleVec.GetSize();
         result.SetSize(sz);
      }
      for (Integer ii = 0; ii < sz; ii++)
      {
         Real value = coneAngleVec[ii] * GmatMathConstants::DEG_PER_RAD;
         Real tempValue = value;
         if (id == FOV_ELEVATION)
         {
            tempValue = 90 - value;
         }
         result[ii] = tempValue;
      }
         
      return result;
   }

   if (id == CLOCK_ANGLES || id == FOV_AZIMUTH)
   {
      if (!clockAngleVec.IsSized())
         result.SetSize(0);
      else
      {
         sz = clockAngleVec.GetSize();
         result.SetSize(sz);
      }
      for (Integer ii = 0; ii < sz; ii++)
         result[ii] = clockAngleVec[ii] * GmatMathConstants::DEG_PER_RAD;
      return result;
   }
   return FieldOfView::GetRvectorParameter(id);
}

//------------------------------------------------------------------------------
//  const Rvector&  GetRvectorParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * This method gets the Rvector parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  the Rvector parameter.
 *
 */
 //------------------------------------------------------------------------------
const Rvector& CustomFOV::GetRvectorParameter(const std::string &label) const
{
	return GetRvectorParameter(GetParameterID(label));
}
//------------------------------------------------------------------------------
//  const Rvector&  SetRvectorParameter(const Integer id, const Rvector& value)
//------------------------------------------------------------------------------
/**
 * This method sets the Rvector parameter value, given the input
 * parameter ID.
 *
 * @param <id>    ID for the requested parameter.
 * @param <value> Rvector value for the requested parameter.
 *
 * @return  success flag.
 *
 */
 //------------------------------------------------------------------------------
const Rvector& CustomFOV::SetRvectorParameter(const Integer id,
                                              const Rvector &value)
{
	if (id == CONE_ANGLES || id == FOV_ELEVATION)
	{
		coneAngleVec.SetSize(value.GetSize());
      Rvector tempValue = value;
      if (id == FOV_ELEVATION)
      {
         tempValue = Rvector(3, 90.0, 90.0, 90.0) - value;
      }
		coneAngleVec = tempValue * GmatMathConstants::RAD_PER_DEG;
		return coneAngleVec;
	}
	
   if (id == CLOCK_ANGLES || id == FOV_AZIMUTH)
	{
		clockAngleVec.SetSize(value.GetSize());
		clockAngleVec = value * GmatMathConstants::RAD_PER_DEG;
		return clockAngleVec;
	}
	return FieldOfView::SetRvectorParameter(id, value);
}
//------------------------------------------------------------------------------
//  const Rvector&  SetRvectorParameter(const std::string &label, 
//                                      const Rvector& value)
//------------------------------------------------------------------------------
/**
 * This method sets the Rvector parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 * @param <value> Rvector value for the requested parameter.
 *
 * @return  Rvector parameter value.
 */
 //------------------------------------------------------------------------------
const Rvector& CustomFOV::SetRvectorParameter(const std::string &label,
                                              const Rvector &value)
{
	return SetRvectorParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
void CustomFOV::LineSegmentIntersect(const Rmatrix &XY1,const Rmatrix &XY2,
	std::vector<IntegerArray>  &adjacency,Rmatrix &matrixX,
	Rmatrix &matrixY,Rmatrix &distance1To2,Rmatrix &distance2To1,
	std::vector<IntegerArray>  &parallelAdjacency,
	std::vector<IntegerArray>  &coincidentAdjacency)
{
	if ((!XY1.IsSized()) || (!XY2.IsSized()))
		throw FieldOfViewException(
            "ERROR: arguments to LineSegmentIntersect must be sized\n");
	Integer sz1row, sz1col, sz2row, sz2col;
	XY1.GetSize(sz1row, sz1col);
	XY2.GetSize(sz2row, sz2col);
	if ((sz1col != 4) || (sz2col != 4))
		throw FieldOfViewException(
            "ERROR: arguments to LineSegmentIntersect must be nx4 matrices\n");
   #ifdef DEBUG_LINE_SEGMENT
      MessageInterface::ShowMessage("Entering LineSegmentIntersect with:\n");
      MessageInterface::ShowMessage("  XY1(%d, %d)\n", sz1row, sz1col);
      MessageInterface::ShowMessage("  XY2(%d, %d)\n", sz2row, sz2col);
   #endif
	// Prepare matrices for vectorized computation of line intersection points.
	Rmatrix X1(sz1row, sz2row); // sz2row copies of XY1's column 0
	Rmatrix X2(sz1row, sz2row); // sz2row copies of XY1's column 2
	Rmatrix Y1(sz1row, sz2row); // sz2row copies of XY1's column 1
	Rmatrix Y2(sz1row, sz2row); // sz2row copies of XY1's column 3
	for (unsigned int jj = 0; jj < sz2row; jj++)
	{
		for (unsigned int ii = 0; ii < sz1row; ii++)
		{
			X1(ii, jj) = XY1(ii, 0);
			X2(ii, jj) = XY1(ii, 2);
			Y1(ii, jj) = XY1(ii, 1);
			Y2(ii, jj) = XY1(ii, 3);
		}
	}
	Rmatrix XY2T(sz2col, sz2row);
	XY2T = XY2.Transpose();
   #ifdef DEBUG_LINE_SEGMENT
      MessageInterface::ShowMessage("  XY2 transposed to XY2T(%d, %d): \n%s\n",
                                    sz2col, sz2row, XY2T.ToString(6).c_str());
   #endif
	Rmatrix X3(sz1row, sz2row); // sz1row copies of XY2T's row 0
	Rmatrix X4(sz1row, sz2row); // sz1row copies of XY2T's row 2
	Rmatrix Y3(sz1row, sz2row); // sz1row copies of XY2T's row 1
	Rmatrix Y4(sz1row, sz2row); // sz1row copies of XY2T's row 3
	for (unsigned int ii = 0; ii < sz1row; ii++)
	{
		for (unsigned int jj = 0; jj < sz2row; jj++)
		{
			X3(ii, jj) = XY2T(0, jj);
			X4(ii, jj) = XY2T(2, jj);
			Y3(ii, jj) = XY2T(1, jj);
			Y4(ii, jj) = XY2T(3, jj);
		}
	}

   #ifdef DEBUG_LINE_SEGMENT
      MessageInterface::ShowMessage("X3 = %s\n", X3.ToString(6).c_str());
      MessageInterface::ShowMessage("X4 = %s\n", X4.ToString(6).c_str());
      MessageInterface::ShowMessage("Y3 = %s\n", Y3.ToString(6).c_str());
      MessageInterface::ShowMessage("Y4 = %s\n", Y4.ToString(6).c_str());
   #endif
	Rmatrix X4_X3(sz1row, sz2row);
	Rmatrix Y1_Y3(sz1row, sz2row);
	Rmatrix Y4_Y3(sz1row, sz2row);
	Rmatrix X1_X3(sz1row, sz2row);
	Rmatrix X2_X1(sz1row, sz2row);
	Rmatrix Y2_Y1(sz1row, sz2row);

	X4_X3 = (X4 - X3);
	Y1_Y3 = (Y1 - Y3);
	Y4_Y3 = (Y4 - Y3);
	X1_X3 = (X1 - X3);
	X2_X1 = (X2 - X1);
	Y2_Y1 = (Y2 - Y1);

	Rmatrix numA(sz1row, sz2row);
	Rmatrix numB(sz1row, sz2row);
	Rmatrix denom(sz1row, sz2row);
	Rmatrix u_a(sz1row, sz2row);
	Rmatrix u_b(sz1row, sz2row);

	numA = X4_X3.ElementWiseMultiply(Y1_Y3) - Y4_Y3.ElementWiseMultiply(X1_X3);
	numB = X2_X1.ElementWiseMultiply(Y1_Y3) - Y2_Y1.ElementWiseMultiply(X1_X3);
	denom = Y4_Y3.ElementWiseMultiply(X2_X1) - X4_X3.ElementWiseMultiply(Y2_Y1);

	u_a = numA.ElementWiseDivide(denom);
	u_b = numB.ElementWiseDivide(denom);

	// Find the adjacency matrix A of intersecting lines.

	Rmatrix INT_X(sz1row, sz2row);
	Rmatrix INT_Y(sz1row, sz2row);
	Rmatrix INT_B(sz1row, sz2row); // need Real version first, to multiply

	INT_X = X1 + X2_X1.ElementWiseMultiply(u_a);
	INT_Y = Y1 + Y2_Y1.ElementWiseMultiply(u_a);

	adjacency.clear();
	IntegerArray adjRow;
	IntegerArray parallelRow;
	IntegerArray coincidentRow;
	for (Integer ii = 0; ii < sz1row; ii++)
	{
		adjRow.clear();
		parallelRow.clear();
		coincidentRow.clear();
		// First check for adjacency
		for (Integer jj = 0; jj < sz2row; jj++)
		{
			if ((u_a(ii, jj) >= -GmatRealConstants::REAL_EPSILON) &&
				(u_a(ii, jj) <= 1.0 + GmatRealConstants::REAL_EPSILON) &&
				(u_b(ii, jj) >= -GmatRealConstants::REAL_EPSILON) &&
				(u_b(ii, jj) <= 1.0 + GmatRealConstants::REAL_EPSILON))
			{
				INT_B(ii, jj) = 1.0;
				adjRow.push_back(1);
			}
			else
			{
				INT_B(ii, jj) = 0.0;
				adjRow.push_back(0);
			}
			// Now check for parallel or coincident adjacency
			if (denom(ii, jj) == 0)
			{
				parallelRow.push_back(1);
				if (numA(ii, jj) == 0 && (numB(ii, jj) == 0))
					coincidentRow.push_back(1);
				else
					coincidentRow.push_back(0);
			}
			else
			{
				parallelRow.push_back(0);
				coincidentRow.push_back(0);
			}
		}
		adjacency.push_back(adjRow);
		parallelAdjacency.push_back(parallelRow);
		coincidentAdjacency.push_back(coincidentRow);
	}

	//   INT_B   = (u_a >= 0) & (u_a <= 1) & (u_b >= 0) & (u_b <= 1);
	//   PAR_B   = denominator == 0;
	//   COINC_B = (numerator_a == 0 & numerator_b == 0 & PAR_B);

	   // Size Rmatrix outputs
	matrixX.SetSize(sz1row, sz2row);
	matrixY.SetSize(sz1row, sz2row);
	distance1To2.SetSize(sz1row, sz2row);
	distance2To1.SetSize(sz1row, sz2row);

	// Arrange output.
	matrixX = INT_X.ElementWiseMultiply(INT_B);
	matrixY = INT_Y.ElementWiseMultiply(INT_B);
	distance1To2 = u_a;
	distance2To1 = u_b;
	//   out.parAdjacencyMatrix = PAR_B;
	//   out.coincAdjacencyMatrix= COINC_B;

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
Real CustomFOV::Max(const Rvector &v)
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
Real CustomFOV::Min(const Rvector &v)
{
	Real minval = v[0];
	Integer N = v.GetSize();
	for (int i = 0; i < N; i++)
		if (v[i] < minval)
			minval = v[i];
	return minval;
}
//------------------------------------------------------------------------------
// void ReadConeClockAngles(const std::string &fileName = "")
//------------------------------------------------------------------------------
/**
 * Reads GMAT startup file.
 *
 * @param <fileName> startup file name.
 *
 */
 //------------------------------------------------------------------------------
void  CustomFOV::ReadConeClockAngles(const std::string &fileName)
{
   #ifdef DEBUG_CUSTOM_FOV
      MessageInterface::ShowMessage(
                                    "ReadConeClockAngles entered ------\n");
   #endif
   if (isInitialized)  return;

	// Assume if there is no filename, that we are usng the user-input data
	// (though this should be checked in the calling method)
	if (fileName == "")
		return;

	StringArray fileTextLines = GmatFileUtil::GetTextLines( fileName);

   // @todo Add header information later
   Integer idx = 0;
   RealArray cones; // for reading in a variable
   RealArray clocks;
   Real firstAngle = 0.0;
   Real secondAngle = 0.0;
   bool done = false;
   std::string firstAngleFormat = "";
   std::string secondAngleFormat = "";
   std::string format = "";
   bool timeToReadData = false;

   for (Integer i = 0; i < fileTextLines.size(); i++)
   {
      if (!GmatStringUtil::IsBlank(fileTextLines[i]))
      {

         if (timeToReadData)
         {
            // If line is not blank, read the code and clock angles
            // There is only one cone angle and clock angle on each line
            std::istringstream lineS;
            lineS.str(fileTextLines[i]);
            lineS >> firstAngle >> secondAngle;
            Integer found = fileTextLines[i].find_first_not_of(" \t");
            if (GmatStringUtil::IsNumber(std::string(1, fileTextLines[i][found])))
            {
               for (Integer lpr = 0; lpr < 2; lpr++)
               {
                  std::string angleFormat;
                  Real entry;
                  if (lpr == 0)
                  {
                     angleFormat = firstAngleFormat;
                     entry = firstAngle;
                  }
                  else
                  {
                     angleFormat = secondAngleFormat;
                     entry = secondAngle;
                  }
                  if (angleFormat == "Azimuth" || angleFormat == "Clock")
                  {
                     clocks.push_back(entry * GmatMathConstants::RAD_PER_DEG);
                  }
                  else if (angleFormat == "Elevation")
                  {
                     cones.push_back((90 - entry) * GmatMathConstants::RAD_PER_DEG);

                  }
                  else if (angleFormat == "Cone")
                  {
                     cones.push_back(entry * GmatMathConstants::RAD_PER_DEG);

                  }
               }
               idx++;
            }

         }
         else
         {
            format = GmatStringUtil::RemoveAllBlanks(fileTextLines[i]);
            if (format == "AzimuthElevationAngles" || fileTextLines[i] == "BEGIN AzElMaskData")
            {
               interpolatePoints = true;
               firstAngleFormat = "Azimuth";
               secondAngleFormat = "Elevation";
               timeToReadData = true;
               isClockCone = false;
            }
            else if (format == "ElevationAzimuthAngles" || fileTextLines[i] == "BEGIN AzElMaskData")
            {
               interpolatePoints = true;
               firstAngleFormat = "Elevation";
               secondAngleFormat = "Azimuth";
               timeToReadData = true;
               isClockCone = false;
            }
            else if (format == "ClockElevationAngles")
            {
               firstAngleFormat = "Clock";
               secondAngleFormat = "Elevation";
               timeToReadData = true;
            }
            else if (format == "ElevationClockAngles")
            {
               firstAngleFormat = "Elevation";
               secondAngleFormat = "Clock";
               timeToReadData = true;
            }
            else if (format == "ClockConeAngles")
            {
               firstAngleFormat = "Clock";
               secondAngleFormat = "Cone";
               timeToReadData = true;
            }
            else if (format == "ConeClockAngles")
            {
               firstAngleFormat = "Cone";
               secondAngleFormat = "Clock";
               timeToReadData = true;
            }
            else if (format == "AzimuthConeAngles")
            {
               firstAngleFormat = "Azimuth";
               secondAngleFormat = "Cone";
               timeToReadData = true;
            }
            else if (format == "ConeAzimuthAngles")
            {
               firstAngleFormat = "Cone";
               secondAngleFormat = "Azimuth";
               timeToReadData = true;
            }
         }
      }
   }
   if (!timeToReadData)
      throw FieldOfViewException("ERROR: The file \"" + fovFileName + "\" does not contain a line defining the format of the angles. See the GMAT User's Guide for details on the file specifications for CustomFOVs.");

   coneAngleVec.SetSize(idx);
   clockAngleVec.SetSize(idx);
	
	for (Integer ii = 0; ii < idx; ii++)
	{
		coneAngleVec(ii) = cones.at(ii);
		clockAngleVec(ii) = clocks.at(ii);
	}

   #ifdef DEBUG_CUSTOM_FOV
         MessageInterface::ShowMessage("EXITING ReadConeClockAngles\n",
                                       fileName.c_str());
   #endif

	// Make sure to set isInitialized = true in the calling method Initialize()!!
}

// ------------------------------------------------------------------------------
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
bool CustomFOV::CheckTargetMaxExcursionCoordinates(Real xCoord, Real yCoord)
{
	// first assume point is in bounding box
	bool possiblyInView = true;
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
Rmatrix CustomFOV::PointsToSegments(const Rvector &xCoords,const Rvector &yCoords)
{
	Integer size = xCoords.GetSize();
	Rmatrix lineSegArray(size, 4);
     // convert arrays of points represented by xy stereographic coordinates
	// to an array of line segments connecting two consecutive points
	for (int i = 0; i < size - 1; i++)
	{
		lineSegArray.SetElement(i, 0, xCoords[i]);
		lineSegArray.SetElement(i, 1, yCoords[i]);
		lineSegArray.SetElement(i, 2, xCoords[i + 1]);
		lineSegArray.SetElement(i, 3, yCoords[i + 1]);
	}

	// form last segment by connecting end of xCoords,yCoords
	// to the beginning
	int last = size - 1;
	lineSegArray.SetElement(last, 0, xCoords[last]);
	lineSegArray.SetElement(last, 1, yCoords[last]);
	lineSegArray.SetElement(last, 2, xCoords[0]);
	lineSegArray.SetElement(last, 3, yCoords[0]);

	return lineSegArray;
}

//------------------------------------------------------------------------------
// void ComputeExternalPoints()
//------------------------------------------------------------------------------
/*
 * Computes points external to FOV to use in visibility tests
 */
 //------------------------------------------------------------------------------
void CustomFOV::ComputeExternalPoints()
{
   #ifdef DEBUG_CUSTOM_FOV
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
	for (int i = 0; i < numFOVPoints; i++)
		pointDist[i] = sqrt(xProjectionCoordArray[i] * xProjectionCoordArray[i] +
			yProjectionCoordArray[i] * yProjectionCoordArray[i]);

   #ifdef DEBUG_CUSTOM_FOV
      MessageInterface::ShowMessage("DEBUG: max distance from center = %f\n",Max(pointDist));
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
		Integer j, k;
		// test for wrap-around of indices
		if (i <= (numFOVPoints - 1) - 2)  // add -1 so can compare with MATLAB
		{
			j = i + 1;
			k = j + 1;
		}
		else if (i <= (numFOVPoints - 1) - 1) // add -1 so can compare with MATLAB
		{
			j = numFOVPoints - 1; // last point in array range
			k = 0;              // wrap around
		}
		else //i == numFOVPoints - 1, last point in array wrap around
		{
			j = 0;
			k = 1;
		}
      #ifdef DEBUG_CUSTOM_FOV
            MessageInterface::ShowMessage(
                              "DEBUG:(i,j,k)= (%d %d %d)\n", i, j, k);
      #endif

		// compute internal angles
		v1[0] = xProjectionCoordArray[j] - xProjectionCoordArray[i];
		v1[1] = yProjectionCoordArray[j] - yProjectionCoordArray[i];
		v2[0] = xProjectionCoordArray[k] - xProjectionCoordArray[j];
		v2[1] = yProjectionCoordArray[k] - yProjectionCoordArray[j];
		interiorAngle = GmatMathUtil::Mod(GmatMathUtil::ATan2(v2[1], v2[0]), 2.0*GmatMathConstants::PI) -
			GmatMathUtil::Mod(GmatMathUtil::ATan2(v1[1], v1[0]), 2.0*GmatMathConstants::PI);
		interiorAngle = GmatMathUtil::Mod(interiorAngle, 2.0*GmatMathConstants::PI);

      #ifdef DEBUG_CUSTOM_FOV
            MessageInterface::ShowMessage(
                              "DEBUG: interior angle = %f\n",interiorAngle);
      #endif

		// see if this is a candidate point
		if (interiorAngle <= GmatMathConstants::PI)
		{
			numCandidatePoints++;
			xCandidatePoints[numCandidatePoints - 1] = xProjectionCoordArray[j];
			yCandidatePoints[numCandidatePoints - 1] = yProjectionCoordArray[j];
			coneAngles[numCandidatePoints - 1] = coneAngleVec[j];
		}
	} // for

   #ifdef DEBUG_CUSTOM_FOV
      MessageInterface::ShowMessage(
                        "DEBUG: %d candidate points\n",numCandidatePoints);
   #endif

	// sort the candidate points in order of decreasing cone angle
	// and use three largest cone angles as the test points

	// check to see how array slices work to see if loop is needed here
	candidateConeAngles.SetSize(numCandidatePoints);
	for (int i = 0; i < numCandidatePoints; i++)
		candidateConeAngles[i] = coneAngles[i];

	// false == input defining descending order as not ascending order
	Sort(candidateConeAngles, indices, false);

   #ifdef DEBUG_CUSTOM_FOV
      MessageInterface::ShowMessage("DEBUG: Sorted candidate angles\n");
      for (int i = 0; i < numCandidatePoints; i++)
         MessageInterface::ShowMessage("DEBUG: value, index = (%f, %d)\n",
                                       candidateConeAngles[i], i);
   #endif
	if (numCandidatePoints < numTestPoints)
		numTestPoints = numCandidatePoints;
	externalPoints.SetSize(numTestPoints, 2);
	for (int i = 0; i < numTestPoints; i++)
	{
		int index = indices[i];
		externalPoints.SetElement(i, 0, safetyFactor*xCandidatePoints[index]);
		externalPoints.SetElement(i, 1, safetyFactor*yCandidatePoints[index]);
	}

	externalPointArray = externalPoints;

   #ifdef DEBUG_CUSTOM_FOV
      MessageInterface::ShowMessage("DEBUG: Exiting ComputeExternalPoints\n");
   #endif
}

//------------------------------------------------------------------------------
// bool RegionIsFullyContained (std::vector<IntegerArray &adjacency);
//------------------------------------------------------------------------------
/*
 * Determines whether an adajency matrix indicates that a region is
 * fully contained within the  FOV, used by checkRegionVisibility()
 *
 * @param adjacency     [input] entry (i,j) is 1 if line segments XY1(i,*)
 *                      and XY2(j,*)intersect; 0 otherwise
 * @return              returns true if region is completely in FOV,
 *                      false otherwise
 */
 //------------------------------------------------------------------------------
bool CustomFOV::RegionIsFullyContained(std::vector<IntegerArray> &adjacency)
{
	//assume no intersecting lines, then look to see if there are any
	bool isFullyContained = true;
	Integer nRows = adjacency.size();
	Integer nCols;
	IntegerArray row;

   #ifdef DEBUG_CUSTOM_FOV
      MessageInterface::ShowMessage("DEBUG: Adjacency being tested\n");
      for (int i = 0; i < nRows; i++)
      {
         row = adjacency[i];
         nCols = row.size();
         for (int j = 0; j < nCols; j++)
            MessageInterface::ShowMessage("%d ", row[j]);
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
            #ifdef DEBUG_CUSTOM_FOV
               MessageInterface::ShowMessage(
                           "found a crossing at index %d,%d\n", i, j);
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
void CustomFOV::Sort(Rvector &v, bool ascending)
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
void CustomFOV::Sort(Rvector &v, IntegerArray &indices, bool ascending)
{
	Integer size = v.GetSize();
	indices.resize(size);
	FOVElement e;
	std::list<FOVElement> eList;
	eList.clear();

	// load list with elements - real value from Rvector and corresponding index
	for (int i = 0; i < size; i++)
	{
		e.value = v[i];
		e.index = i;
		eList.push_back(e);
	}

	// sort with std::list sort function; providing necessary compare function
	eList.sort(CompareFOVElements);

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
// bool CompareFOVElements(FOVElement e1, FOVElement e2)
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
bool CompareFOVElements(const FOVElement &e1, const FOVElement &e2)
{
	return e1.value < e2.value;
}

