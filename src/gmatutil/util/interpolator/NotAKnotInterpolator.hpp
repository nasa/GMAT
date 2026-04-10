//$Id$
//------------------------------------------------------------------------------
//                            NotAKnotInterpolator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/02
//
/**
 * Defines interpolation class using the cubic spline algorithm with "not a 
 * knot" conditions at points 2 and 4, as described in the GMAT Math Spec.
 */
//------------------------------------------------------------------------------


#ifndef NotAKnotInterpolator_hpp
#define NotAKnotInterpolator_hpp


#include "Interpolator.hpp"

class GMATUTIL_API NotAKnotInterpolator : public Interpolator
{
public:
   NotAKnotInterpolator(const std::string &name = "", Integer dim = 1);
   virtual ~NotAKnotInterpolator();
   NotAKnotInterpolator(const NotAKnotInterpolator &csi);
   NotAKnotInterpolator&      operator=(const NotAKnotInterpolator &csi);

   virtual bool               Interpolate(const Real ind, Real *results);

   // inherited from GmatBase
   virtual Interpolator*      Clone() const;

protected:
   /// Array of ordered independent variables used to construct the splines.
   Real                        x[5];
   /// Array of ordered dependent variables used to construct the splines.
   Real                        *y[5];
   
   /// Coefficients of the splines
   Real                       *a[4];
   Real                       *b[4];
   Real	                     *c[4];
   Real                       *d[4];
   
   /// First derivative structures
   Real                       h[4];
   Real                       *delta[4];
   
   /// Intermediate data structures
   Real                       A[3][3];
   Real                       *B[3];
   Real                       *s[5];

   /// Value of the last point, to determine if the splines need updating
   Real                        lastX;

   // Inherited methods that need some revision for cubic splines
   virtual void                AllocateArrays(void);
   virtual void                CleanupArrays(void);
   virtual void                CopyArrays(const NotAKnotInterpolator &i);

   bool                        BuildSplines(void);
   void                        LoadArrays(void);
   bool                        Estimate(const Real ind, Real *results);
};


#endif // NotAKnotInterpolator_hpp
