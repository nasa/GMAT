//$Id$
//------------------------------------------------------------------------------
//                              RandomNumber
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract
//
// Author: Tuan Dang Nguyen, NASA/GSFC
// Created: 2016.06.15
//
/**
 * This class provides random number
 */
//------------------------------------------------------------------------------

#ifndef _RandomNumber_hpp
#define _RandomNumber_hpp

#include "utildefs.hpp"
#include <random>                // C++11 feature

class GMATUTIL_API RandomNumber
{
public:
	static RandomNumber* Instance();
	~RandomNumber();
   
   // Set seed for the random number generator
   void SetSeed(unsigned int s);
   void SetClockSeed();
   
   Real Gaussian();
   Real Gaussian(const Real mean, const Real stdev);

   void GaussianArray(Real *myArray, const Integer size);
   void GaussianArray(Real *myArray, const Integer size,  
	               const Real mean, const Real stdev);

   Real Uniform();
   Real Uniform(const Real a, const Real b);
   void UniformArray(Real *myArray, const Integer size);
   void UniformArray(Real *myArray, const Integer size,
       const Real a, const Real b);

private:
   /// The random number generator
   std::mt19937 generator;
   //std::default_random_engine generator;
   /// The white noise provider
   std::uniform_real_distribution<> white;
   
	static RandomNumber *theInstance;
   
   RandomNumber();
   Real rrand();
};

#endif //_RandomNumber_hpp

