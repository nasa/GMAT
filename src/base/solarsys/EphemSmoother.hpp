//------------------------------------------------------------------------------
//                              EphemSmoother
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Wendy Shoan / NASA
// Created: 2019.04.02
//
/**
 * Original Python prototype:
 * Author: N. Hatten
 *
 * This class is an Ephem Smoother
 */
//------------------------------------------------------------------------------

#ifndef EphemSmoother_hpp
#define EphemSmoother_hpp

#include "gmatdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "SpiceInterface.hpp"
#include "CoordinateSystem.hpp"
#include "SpacePoint.hpp"

/// forward declaration for CoordinateTranslation
class CoordinateTranslation;

class GMAT_API EphemSmoother
{
public:
   EphemSmoother(Integer defNRegionsPerRev = 359, Integer defNRegions = 359,
                 Real defH = 86400.0, const std::string defStepSizeType = "h",
                 Real defT0 = 0.0, Real defTf = 31557600.0);// defTf wrong?
   EphemSmoother(const EphemSmoother &copy);
   EphemSmoother& operator=(const EphemSmoother &copy);
   virtual ~EphemSmoother();
   
   virtual void    GetState(SpacePoint *refBody, SpacePoint *target,
                            CoordinateSystem *refFrame, Real ephemTime,
                            Rvector &state, Rvector &dState, Rvector &ddState);

   virtual void    CreateSmoothedEphem(SpacePoint *refBody, SpacePoint *target,
                                       CoordinateSystem *refFrame,
                                       Real theTime0, Real theTimef,
                                       Real stepSize = 1.0,
                                       Integer nRegions = 1,
                                       Integer nRegionsPerRev = 1,
                                       const std::string &stepSizeType = "h");
protected:
 
   /// Structure holding ephem data
   struct EphemData
   {
      EphemData(const std::string &refBodyName  = "Sun",
                const std::string &targetName   = "Earth",
                const std::string &refFrame     = "MJ2000Eq", // axis system
                Integer           nStates       = 6,
                Real              t0            = 0.0,
                Real              tf            = 31557600.0, // ???? wrong!
                Real              h             = 1.0,
                Integer           nRegions      = 1,
                const std::string stSzType      = "nRegions") :
         referenceBodyName  (refBodyName),
         targetBodyName     (targetName),
         referenceFrame     (refFrame),
         numStates          (nStates),
         time0              (t0),
         timef              (tf),
         stepSize           (h),
         numRegions         (nRegions),
         stepSizeType       (stSzType)
      {
         a.SetSize(numRegions, numStates); // will be resized before setting
         b.SetSize(numRegions, numStates); // will be resized before setting
         c.SetSize(numRegions, numStates); // will be resized before setting
         d.SetSize(numRegions, numStates); // will be resized before setting
         
//         theTimes.SetSize(numRegions - 1);
      }
      
      EphemData(const EphemData &copy) :
         referenceBodyName  (copy.referenceBodyName),
         targetBodyName     (copy.targetBodyName),
         referenceFrame     (copy.referenceFrame),
         numStates          (copy.numStates),
         time0              (copy.time0),
         timef              (copy.timef),
         stepSize           (copy.stepSize),
         numRegions         (copy.numRegions),
         stepSizeType       (copy.stepSizeType)
      {
         // Resize and set the arrays
         a.SetSize(numRegions, numStates);
         b.SetSize(numRegions, numStates);
         c.SetSize(numRegions, numStates);
         d.SetSize(numRegions, numStates);
         
         a = copy.a;
         b = copy.b;
         c = copy.c;
         d = copy.d;
         
         theTimes.clear();
         for (Integer ii = 0; ii < copy.theTimes.size(); ii++)
            theTimes.push_back(copy.theTimes.at(ii));
      }
      EphemData& operator=(const EphemData &copy)
      {
         referenceBodyName   = copy.referenceBodyName;
         targetBodyName      = copy.targetBodyName;
         referenceFrame      = copy.referenceFrame;
         numStates           = copy.numStates;
         time0               = copy.time0;
         timef               = copy.timef;
         stepSize            = copy.stepSize;
         numRegions          = copy.numRegions;
         stepSizeType        = copy.stepSizeType;
         
         // Resize and set the arrays
         a.SetSize(numRegions, numStates);
         b.SetSize(numRegions, numStates);
         c.SetSize(numRegions, numStates);
         d.SetSize(numRegions, numStates);
         
         a = copy.a;
         b = copy.b;
         c = copy.c;
         d = copy.d;
         
         theTimes.clear();
         for (Integer ii = 0; ii < copy.theTimes.size(); ii++)
            theTimes.push_back(copy.theTimes.at(ii));
         
         return *this;
      }
      
      EphemData* Clone() const
      {
         EphemData * ed = new EphemData(*this);         
         return ed;
      }

      
      /// All EphemData data is public
      
      /// Name of the reference body
      std::string referenceBodyName;
      /// Name of the target body
      std::string targetBodyName;
      /// Name of the reference frame
      std::string referenceFrame;
      /// Number of states splined
      Integer     numStates;
      /// Earliest A1Mjd time at which spline data is available
      Real        time0;
      /// Latest A1Mjd time at which spline data is available
      Real        timef;
      /// Step size - assumed constant for now
      Real        stepSize;
      /// Number of spline regions; the number of points used to create the
      /// spline is nRegions+1
      Integer     numRegions;
      /// Step size type
      std::string stepSizeType;
      
      /// Coefficients ???
      Rmatrix      a;
      Rmatrix      b;
      Rmatrix      c;
      Rmatrix      d;
      /// The A1Mjd times corresponding to the knot points
      RealArray   theTimes;
   }; // struct EphemData
   
   /// The default number of time regions to use per revolution of the target
   /// body about its central body for computing a spline
   Integer     defaultNumRegionsPerRev;
   /// The default number of time regions to use for computing a spline
   Integer     defaultNumRegions;
   /// The default step size (in ET seconds) to use when computing a spline
   Real        defaultH;
   /// The default method for calculating the time between knots:
   /// "h", "nRegions", or "nRegionsPerRevolution"
   std::string defaultStepSizeType;
   /// The default opening time of the spline domain (A1Mjd)
   Real        defaultT0;
   /// The default closing time of the spline domain (A1Mjd)
   Real        defaultTf;
   
//   /// a Cubic Spline
//   CubicSpline *cubicSpline;
   
   /// A list of EphemData objects
   std::vector<EphemData*> ephemDataList;
   
   virtual Integer ValidateInterpolationRequest(SpacePoint *refBody,
                                                SpacePoint *target,
                                                CoordinateSystem *refFrame,
                                                Real theTime);

};

#endif // EphemSmoother_hpp


