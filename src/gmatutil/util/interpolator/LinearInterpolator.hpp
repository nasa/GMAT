//$Id$
//------------------------------------------------------------------------------
//                             LinearInterpolator
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
// Author: Darrel J. Conway
// Created: 2003/10/15
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * A basic linear interpolator.
 */
//------------------------------------------------------------------------------


#ifndef LinearInterpolator_hpp
#define LinearInterpolator_hpp

#include "Interpolator.hpp"

/**
 * A linear interpolator for quick calculation of interpolated data
 */
class GMATUTIL_API LinearInterpolator : public Interpolator
{
public:
    LinearInterpolator(const std::string &name = "", Integer dim = 1); 
    ~LinearInterpolator();
    LinearInterpolator(const LinearInterpolator &l);
    LinearInterpolator& operator=(const LinearInterpolator &li);

    virtual bool Interpolate(const Real ind, Real *results);
    
    // inherited from GmatBase
    virtual Interpolator* Clone() const;

};


#endif // LinearInterpolator_hpp

