//$Id$
//------------------------------------------------------------------------------
//                              RandomNumber
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
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

#include "RandomNumber.hpp"
#include "MessageInterface.hpp"
#include <time.h>

//#define DEBUG_CONSTRUCTOR

//---------------------------------
// static members
//---------------------------------

RandomNumber* RandomNumber::theInstance = NULL;

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// RandomNumber* RandomNumber::Instance()
//------------------------------------------------------------------------------
RandomNumber* RandomNumber::Instance()
{
   if (theInstance == NULL)
      theInstance = new RandomNumber;
   return theInstance;
}


//------------------------------------------------------------------------------
// ~RandomNumber()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
RandomNumber::~RandomNumber()
{
}


//------------------------------------------------------------------------------
//  void SetSeed(unsigned int s)
//------------------------------------------------------------------------------
/**
 *  Set the seed for the random number generator using a specified value.
 *
 *  @param <s> input seed
 */
//------------------------------------------------------------------------------
void RandomNumber::SetSeed(unsigned int s)
{
   generator.seed(s);
}


//------------------------------------------------------------------------------
//  void SetClockSeed()
//------------------------------------------------------------------------------
/**
 * Set the seed value based upon the current clock time.
 */
//------------------------------------------------------------------------------
void RandomNumber::SetClockSeed()
{
   unsigned int clockSeed = time(NULL);
   generator.seed(clockSeed);
}


//------------------------------------------------------------------------------
//  Real Gaussian()
//------------------------------------------------------------------------------
/**
 *  Returns a normally distributed Gaussian random deviate (zero mean, unit var)
 */
//------------------------------------------------------------------------------
Real RandomNumber::Gaussian()
{
   std::normal_distribution<double> Gauss(0.0, 1.0);
   return Gauss(generator);
}


//------------------------------------------------------------------------------
//  Real Gaussian(const Real mean, const Real stdev)
//------------------------------------------------------------------------------
/**
 *  Returns a normally distributed Gaussian random deviate.
 *
 *  @param <mean> Mean of Gaussian distribution
 *  @param <stdev> Standard deviation of Gaussian distribution
 *  @return The random deviate.
 */
//------------------------------------------------------------------------------
Real RandomNumber::Gaussian(const Real mean, const Real stdev)
{
   std::normal_distribution<double> Gauss(mean, stdev);
   return Gauss(generator);
}


//------------------------------------------------------------------------------
//  void GaussianArray(Real *myArray, const Integer size)
//------------------------------------------------------------------------------
/**
 *  Returns a normally distributed Gaussian random deviate (zero mean, unit var)
 *
 *  @param <myArray> Pointer to array where random deviates will be stored
 *  @param <size> size of the array of deviates
 *
 */
//------------------------------------------------------------------------------
void RandomNumber::GaussianArray(Real *myArray, const Integer size)
{
   for (Integer i = 0; i < size; i++)
   {
      myArray[i] = Gaussian();
   }
}

//------------------------------------------------------------------------------
//  void GaussianArray(Real *myArray, const Integer size,
//	                         const Real mean, const Real stdev)
//------------------------------------------------------------------------------
/**
 *  Returns an array of normally distributed Gaussian random deviates
 *  with specified mean and variance.
 *
 *  @param <myArray> Pointer to array where random deviates will be stored
 *  @param <size> size of the array of deviates
 *  @param <mean> Mean of Gaussian distribution
 *  @param <stdev> Standard deviation of Gaussian distribution
 */
//------------------------------------------------------------------------------
void RandomNumber::GaussianArray(Real *myArray, const Integer size,
                                 const Real mean, const Real stdev)
{
   for (Integer i = 0; i < size; i++)
   {
      myArray[i] = Gaussian(mean, stdev);
   }
}


//------------------------------------------------------------------------------
//  Real Uniform()
//------------------------------------------------------------------------------
/**
 *  Returns an uniformly distributed random deviate in the range [0,1)
 *  The range includes 0.0 but excludes 1.0;
 *
 *  @return The random deviate.
 */
//------------------------------------------------------------------------------
Real RandomNumber::Uniform()
{
   Real rn;
   do
   {
      rn = rrand();
   } while (rn == 1.0);

   return rn;
}


//------------------------------------------------------------------------------
//  Real Uniform(Real a, Real b)
//------------------------------------------------------------------------------
/**
 *  Returns an uniformly distributed random deviate in the range [a,b)
 *  The mean of this distribution is (a+b)/2.
 *  The variance of this distribution is (b-a)^2/12.
 *
 *  @param <a> Distribution start
 *  @param <b> Distribution end
 *  @return The random deviate.
 */
//------------------------------------------------------------------------------
Real RandomNumber::Uniform(const Real a, const Real b)
{
   return a + (b - a)*Uniform();
}


//------------------------------------------------------------------------------
//  void UniformArray(Real *myArray, const Integer size)
//------------------------------------------------------------------------------
/**
 *  Returns a uniformly distributed random deviate in the range [0,1)
 *  The range includes 0.0 but excludes 1.0;
 *
 *  @param <myArray> Pointer to array where random deviates will be stored
 *  @param <size> size of the array of deviates
 */
//------------------------------------------------------------------------------
void RandomNumber::UniformArray(Real* myArray, const Integer size)
{
   for (Integer i = 0; i < size; ++i)
      myArray[i] = Uniform();
}


//------------------------------------------------------------------------------
//  void UniformArray(Real *myArray, Real a, Real b, const Integer size)
//------------------------------------------------------------------------------
/**
 *  Returns an uniformly distributed random deviate in the range [a,b)
 *  The mean of this distribution is (a+b)/2.
 *  The variance of this distribution is (b-a)^2/12.
 *
 *  @param <myArray> Pointer to array where random deviates will be stored
 *  @param <size> size of the array of deviates
 *  @param <a> Distribution start
 *  @param <b> Distribution end
 */
//------------------------------------------------------------------------------
void RandomNumber::UniformArray(Real *myArray, const Integer size,
                                const Real a, const Real b)
{
   UniformArray(myArray, size);
   for (Integer i = 0; i < size; i++)
      myArray[i] = a + (b - a)*myArray[i];
}

//---------------------------------
// priviate methods
//---------------------------------

//------------------------------------------------------------------------------
// RandomNumber()
//------------------------------------------------------------------------------
/**
 * Class constructor that seeds the generator using the clock.
 */
//------------------------------------------------------------------------------
RandomNumber::RandomNumber()
{
   #ifdef DEBUG_CONSTRUCTOR
	MessageInterface::ShowMessage("RandomNumber  default constructor\n");
   #endif

   std::random_device rd;
   generator.seed(rd());
   
   std::uniform_real_distribution<>::param_type whiteParams(0.0, 1.0);
   white.param(whiteParams);
}

//------------------------------------------------------------------------------
// Real RandomNumber::rrand()
//------------------------------------------------------------------------------
Real RandomNumber::rrand()
{
   return white(generator);
}

