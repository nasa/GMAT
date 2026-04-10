//$Id$
//------------------------------------------------------------------------------
//                               Interpolator  
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/23
//
/**
 * Definition for the Interpolator base class
 */
//------------------------------------------------------------------------------


#ifndef Interpolator_hpp
#define Interpolator_hpp

#include "utildefs.hpp"

/**
 * Base class for the GMAT Interpolators
 */
class GMATUTIL_API Interpolator
{
public:
   Interpolator(const std::string &name, const std::string &typestr,
                Integer dim = 1);
   virtual ~Interpolator();
   
   Interpolator(const Interpolator &i);
   Interpolator&   operator=(const Interpolator &i);
   
   virtual Integer IsInterpolationFeasible(Real ind);
   virtual void    SetForceInterpolation(bool flag);
   virtual bool    GetForceInterpolation();
   virtual bool    AddPoint(const Real ind, const Real *data);
   virtual void    Clear();
   virtual Integer GetBufferSize();
   virtual Integer GetPointCount();
   virtual void    SetExtrapolation(bool flag);
   
   std::string     GetName();
   bool            GetRange(Real &lower, Real &upper);

   //---------------------------------------------------------------------------
   // bool Interpolate(const Real ind, Real *results)
   //---------------------------------------------------------------------------
   /**
    * Interpolate the data.
    *
    * Derived classes implement this method to provide the mathematics that
    * perform the data interpolation, resulint in an array of interpolated data
    * valid at the desired value of the independent variable.
    *
    * @param <ind>     Value of the independent variable at which the data is
    *                  interpolated.
    * @param <results> Array of interpolated data.
    *
    * @return true on success, false (or throw) on failure.
    */
   //---------------------------------------------------------------------------
   virtual bool    Interpolate(const Real ind, Real *results) = 0;
   
   virtual Interpolator* Clone() const = 0;

//   DEFAULT_TO_NO_CLONES
//   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Data array used for the independent variable
   Real *independent;
   /// The data that gets interpolated
   Real **dependent;
   /// Previous independent value, used to determine direction data is going
   Real previousX; 
   
   // Parameters
   /// Number of dependent points to be interpolated
   Integer dimension;
   /// Number of points required to interpolate
   Integer requiredPoints;
   /// Number of points managed by the interpolator
   Integer bufferSize;
   /// Number of points fed to the interpolator
   Integer pointCount;
   /// Pointer to most recent point, for the ring buffer implementation
   Integer latestPoint;
   /// Valid range for the data points
   Real range[2];
   /// Flag used to detect if range has already been calculated
   bool rangeCalculated;
   /// Flag used to determine if independent variable increases or decreases
   bool dataIncreases;
   /// Flag used for additional feasiblity checking
   bool forceInterpolation;
   /// Flag to allow extrapolation
   bool allowExtrapolation;
   
   /// The name of this interpolator
   std::string instanceName;

   virtual void AllocateArrays();
   virtual void CleanupArrays();
   virtual void CopyArrays(const Interpolator &i);
   void SetRange();
};


#endif // Interpolator_hpp

