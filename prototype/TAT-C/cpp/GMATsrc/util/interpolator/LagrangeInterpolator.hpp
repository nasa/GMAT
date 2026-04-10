//$Id$
//------------------------------------------------------------------------------
//                            LagrangeInterpolator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// number #####
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2009/09/28
//
/**
 * Declares LagrangeInterpolator class as specified in the GMAT Math Spec.
 */
//------------------------------------------------------------------------------
#ifndef LagrangeInterpolator_hpp
#define LagrangeInterpolator_hpp


#include "Interpolator.hpp"

class GMATUTIL_API LagrangeInterpolator : public Interpolator
{
public:
   LagrangeInterpolator(const std::string &name = "", Integer dim = 1,
                        Integer ord = 7);
   virtual ~LagrangeInterpolator();
   LagrangeInterpolator(const LagrangeInterpolator &li);
   LagrangeInterpolator& operator=(const LagrangeInterpolator &li);
   
   // inherited from Interpolator
   virtual Integer      IsInterpolationFeasible(Real ind);
   virtual void         Clear();
   virtual bool         AddPoint(const Real ind, const Real *data);
   virtual bool         Interpolate(const Real ind, Real *results);
   
   // inherited from GmatBase
   virtual Interpolator*    Clone() const;
   
protected:
   static const Integer MAX_BUFFER_SIZE = 80;
   
   /// Order of interpolation
   Integer order;
   /// Actual size to be used
   Integer actualSize;
   /// Starting index used in finding center point
   Integer beginIndex;
   /// Ending index used in finding center point
   Integer endIndex;
   /// Index of nearest data point of requested data
   Integer dataIndex;
   /// Starting index of interpolation range
   Integer startPoint;
   /// Value of the last point, to determine if the data buffer need updating
   Real  lastX;
   /// Array of ordered independent variables used
   Real  *x;
   /// Array of ordered dependent variables used
   Real  **y;
   
   // Inherited methods that need some revision for LagrangeInterpolator
   virtual void AllocateArrays();
   virtual void CleanupArrays();
   virtual void CopyArrays(const LagrangeInterpolator &i);
   
   void    BuildDataPoints(Real ind);
   bool    UpdateBeginAndEndIndex(Real ind);
   bool    IsDataNearCenter(Real ind);
   Integer FindStartingPoint(Real ind);
};


#endif // LagrangeInterpolator_hpp
