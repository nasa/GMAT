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
#ifndef CustomFOV_hpp
#define CustomFOV_hpp

#include "FieldOfView.hpp"
#include "gmatdefs.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"


class CustomFOV : public FieldOfView
{
public:

	/// class construction/destruction
	CustomFOV(const std::string &itsName = "");
	virtual ~CustomFOV();
	CustomFOV(const CustomFOV &copy);
	CustomFOV& operator=(const CustomFOV& copy);
	
	virtual bool Initialize();
	virtual GmatBase* Clone() const;

	/// visibility methods
	/// Check the target visibility given the input cone and clock angles:
	/// determines whether or not the point is in the FieldOfView FOV.
	bool CheckTargetVisibility(Real viewConeAngle, Real viewClockAngle);
           	/// Check the target visibility given the input unit vector:
            /// determines whether or not the point is in the  FOV
	virtual bool CheckTargetVisibility(const Rvector3 &target);
	bool CheckRegionVisibility(const Rvector &coneAngleVector,const Rvector &clockAngleVector);
	virtual Rvector GetMaskConeAngles();
	virtual Rvector GetMaskClockAngles();

	         /// Check Get parameter Text 
   virtual std::string GetParameterText(const Integer id) const;
            /// Check Get parameter ID 
   virtual Integer GetParameterID(const std::string &str) const;
           /// Check Get parameter Type 
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
          /// Check Get parameter Type string 
   virtual std::string GetParameterTypeString(const Integer id) const;
         ///  Get/Set Real parameter  
   virtual Real GetRealParameter(const Integer id, const Integer index) const;
   virtual Real GetRealParameter(const std::string &label,const Integer index) const;
   virtual Real SetRealParameter(const Integer id,const Real value,const Integer index);
   virtual Real SetRealParameter(const std::string &label,const Real value,const Integer index);

   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string &label) const;
   virtual Real SetRealParameter(const Integer id, const Real value) ;
   virtual Real SetRealParameter(const std::string &label, const Real value) ;

        ///  Get/Set String parameter  
   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,const std::string &value);
      ///  Get/Set Rvector parameter  
   virtual const Rvector& GetRvectorParameter(const Integer id) const;
   virtual const Rvector& GetRvectorParameter(const std::string &label) const;
   virtual const Rvector& SetRvectorParameter(const Integer id,const Rvector &value);
   virtual const Rvector& SetRvectorParameter(const std::string &label,const Rvector &value);

   DEFAULT_TO_NO_REFOBJECTS
   DEFAULT_TO_NO_CLONES
  
protected:

	static void LineSegmentIntersect(const Rmatrix &XY1,const Rmatrix &XY2,
		std::vector<IntegerArray>  &adjacency,
		Rmatrix &matrixX,Rmatrix &matrixY,
		Rmatrix &distance1To2,Rmatrix &distance2To1,
		std::vector<IntegerArray>  &parallelAdjacency,
		std::vector<IntegerArray>  &coincidentAdjacency);
	// data
	/// data computed from constructor inputs
	Integer numFOVPoints;   // number of points representing FOV boundaries
	Rvector coneAngleVec;   // numFOVPoints cone angles, measured from +Z (rad)
	Rvector clockAngleVec;  // numFOVPoints clock angles (right ascensions) (rad)

	// string projectionAlgorithm; -- seems to be unused so far

	/// stereographic projection of the numFOVpoints cone & clock angles
	Rvector xProjectionCoordArray;   // numFOVpoints x values
	Rvector yProjectionCoordArray;   // numFOVpoints y values
	Rmatrix segmentArray;   // numFOVpoints x 4 representing line segments
							// connecting points in stereographic projection
	/// test points computed in ComputeExternalPoints()
	Integer numTestPoints;
	Rmatrix externalPointArray; // n x 2 array where n <= numtestpoints
								// and each row is an (x,y) stereographic pair
	/// maximum and minimum values for x and y values in stereographic projection
	Real maxXExcursion;
	Real minXExcursion;
	Real maxYExcursion;
	Real minYExcursion;
	Real Max(const Rvector &v);
	Real Min(const Rvector &v);
	bool isInitialized;
   bool interpolatePoints = false;
   bool pointsInterpolated = false;
   bool isClockCone = true;

   Real interpolationSize = 0.2* GmatMathConstants::RAD_PER_DEG;
	std::string  fovFileName;
	
	// protected methods
    ///  Reads cone and clock angle from a file
   void  ReadConeClockAngles(const std::string &fileName = "");
   
	/// class hidden methods used by constructor
   bool CheckTargetMaxExcursionCoordinates(Real xCoord, Real yCoord);
   Rmatrix PointsToSegments(const Rvector &xCoords, const Rvector &yCoords);
   void ComputeExternalPoints();

   /// helper methods for checkRegionVisibility()
   bool RegionIsFullyContained(std::vector<IntegerArray> &adjacency);

   ///  rVector utilities
   void Sort(Rvector &v, bool ascending = true);
   void Sort(Rvector &v, IntegerArray &indices, bool ascending = true);

   enum
	{
		FILE_NAME = FieldOfViewParamCount,
		CONE_ANGLES, 
		CLOCK_ANGLES,
      FOV_AZIMUTH,
      FOV_ELEVATION,
      INTERPOLATION_SIZE,
		CustomFOVParamCount
	};
	/// Hardware Parameter labels
   static const std::string
		PARAMETER_TEXT[CustomFOVParamCount - FieldOfViewParamCount];
	/// Hardware Parameter types
   static const Gmat::ParameterType
		PARAMETER_TYPE[CustomFOVParamCount - FieldOfViewParamCount];
};
/// data type and comparison method used in Sort(Rvector,IntegerArray,bool)
/// to mimic Matlab sort of both values and indices of original array
   struct FOVElement
   {
	  Real value;
	  Integer index;
   };
   bool CompareFOVElements(const FOVElement &e1, const FOVElement &e2);


#endif /* CustomFOV_hpp */
