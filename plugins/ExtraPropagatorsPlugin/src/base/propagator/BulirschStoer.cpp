//$Id: BulirschStoer.cpp 10412 2012-04-26 21:12:51Z djcinsb $
//------------------------------------------------------------------------------
//                                 BulirschStoer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// *** File Name : BulirschStoer.cpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  This software is subject to the Software Usage Agreement described***
// ***  by NASA Case Number GSC-14735-1.  The Software Usage Agreement    ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 1/7/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 06/15/2004 - W. Waktola, Missions Applications Branch
//                                               Changes:
//                                               - Updated style using GMAT cpp style guide
//                                               - All double types to Real types
//                                               - All int types to Integer types
//                               - Added Parameter accessor methods
//                           : 06/22/2004 - W. Waktola, Missions Applications Branch
//                                               Removals:
//                               - SetParameter()
//                               - GetParameter()
//                               - GetParameterName()
//                               - GetType()
//                               - BULIRSCHSTOER_MINIMUMTOLERANCE
// **************************************************************************

#include "BulirschStoer.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
BulirschStoer::PARAMETER_TEXT[BulirschStoerParamCount - IntegratorParamCount] =
{
   "MinimumReduction",
   "MaximumReduction",
   "MinimumTolerance",
};

const Gmat::ParameterType
BulirschStoer::PARAMETER_TYPE[BulirschStoerParamCount - IntegratorParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
};

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// BulirschStoer()
//------------------------------------------------------------------------------
/**
 * The Bulirsch-Stoer constructor
 */
//------------------------------------------------------------------------------
BulirschStoer::BulirschStoer(const std::string &nomme) :
   Integrator                      ("BulirschStoer", nomme),
   depth                           (8),
   depthInitialized                (false),
   level                           (0),
   levelError                      (NULL),
   ai                              (NULL),
   alpha                           (NULL),
   intermediates                   (NULL),
   coeffC                          (NULL),
   intervals                       (NULL),
   estimates                       (NULL),
   mstate                          (NULL),
   nstate                          (NULL),
   estimatedState                  (NULL),
   subinterval                     (NULL),
//   mintolerance                    (1.0e-12),
   bs_safety1                      (0.25),
   bs_safety2                      (0.70),
   minimumReduction                (0.7),
   maximumReduction                (1.0e-5),
   scale_dt                        (0.1)
{
   parameterCount = BulirschStoerParamCount;
   isInitialized = false;
}

//------------------------------------------------------------------------------
// ~BulirschStoer(void)
//------------------------------------------------------------------------------
/**
 * The Bulirsch-Stoer destructor
 */
//------------------------------------------------------------------------------
BulirschStoer::~BulirschStoer()
{
   if (subinterval)
      delete [] subinterval;

   if (levelError)
      delete [] levelError;

   if (coeffC)
      delete [] coeffC;
    
   if (intervals)
      delete [] intervals;
    
   if (estimatedState)
      delete [] estimatedState;

   if (intermediates)
   {
      for (Integer i = 0; i < depth; ++i)
         delete [] intermediates[i];
      delete [] intermediates;
   }

   if (estimates)
   {
      for (Integer i = 0; i < depth; ++i)
         delete [] estimates[i];
      delete [] estimates;
   }

   if (mstate)
      delete [] mstate;

   if (nstate)
      delete [] nstate;
}

//------------------------------------------------------------------------------
// BulirschStoer(const BulirschStoer& bs)
//------------------------------------------------------------------------------
/**
 * The Bulirsch-Stoer copy constructor
 */
//------------------------------------------------------------------------------
BulirschStoer::BulirschStoer(const BulirschStoer& bs) :
   Integrator                      (bs),
   depth                           (8),
   depthInitialized                (false),
   level                           (0),
   levelError                      (NULL),
   ai                              (NULL),
   alpha                           (NULL),
   intermediates                   (NULL),
   coeffC                          (NULL),
   intervals                       (NULL),
   estimates                       (NULL),
   mstate                          (NULL),
   nstate                          (NULL),
   estimatedState                  (NULL),
   subinterval                     (NULL),
//   mintolerance                    (bs.mintolerance),
   bs_safety1                      (0.25),
   bs_safety2                      (0.70),
   minimumReduction                (0.7),
   maximumReduction                (1.0e-5),
   scale_dt                        (0.1),
   kopt                            (bs.kopt),
   kmax                            (bs.kmax),
   kused                           (bs.kused),
   first                           (bs.first)
{
   parameterCount = BulirschStoerParamCount;
   isInitialized  = false;
}

//------------------------------------------------------------------------------
// BulirschStoer::operator=(const BulirschStoer& bs)
//------------------------------------------------------------------------------
/**
 * The Bulirsch-Stoer assignment operator
 */
//------------------------------------------------------------------------------
BulirschStoer& BulirschStoer::operator=(const BulirschStoer& bs)
{
   if (this == &bs)
      return *this;

   Integrator::operator= (bs);
   depth              = bs.depth;
   depthInitialized   = bs.depthInitialized;
   level              = bs.level;
   levelError         = NULL;
   ai                 = NULL;
   alpha              = NULL;
   intermediates      = NULL;
   coeffC             = NULL;
   intervals          = NULL;
   estimates          = NULL;
   mstate             = NULL;
   nstate             = NULL;
   estimatedState     = NULL;
   subinterval        = NULL;
//   mintolerance       = bs.mintolerance;
   bs_safety1         = bs.bs_safety1;
   bs_safety2         = bs.bs_safety2;
   minimumReduction   = bs.minimumReduction;
   maximumReduction   = bs.maximumReduction;
   scale_dt           = bs.scale_dt;
   kopt               = bs.kopt;
   kmax               = bs.kmax;
   kused              = bs.kused;
   first              = bs.first;
    
   isInitialized = false;

   return *this;
}

//------------------------------------------------------------------------------
// Clone(void) const
//------------------------------------------------------------------------------
/**
 * Used to create a copy of the object.
 * This method returns a copy of the current instance.  The copy has all of the
 * parameters set to the current values, but is not yet initialized becuse the 
 * PhysicalModel pointer is not set, nor are the internal arrays that depend on 
 * the PhysicalModel allocated.
 *
 * @return <Propagator*>
 */
//------------------------------------------------------------------------------
GmatBase* BulirschStoer::Clone(void) const
{
   return (new BulirschStoer(*this));
}

//------------------------------------------------------------------------------
// Initialize(void)
//------------------------------------------------------------------------------
/**
 * Method used to initialize the B-S integrator.  This method sets up (or resets)
 * the data arrays used by the Bulirsch-Stoer integrator.  
 * There are several things worth mentioning about the implementation provided
 * here if you are comparing this code to Numerical Recipes.  First, note that 
 * the array "d" in Numerical Recipes is not sized correctly.  It is set to be
 * d[depth][depth], but the polynomial code fills it as if it were 
 * d[dimension][depth].  This bug has been corrected here -- the corresponding 
 * array in this implementation is estimates[depth][dimension].  Also note that
 * I have reversed the indexing for the arrays, so that bulk copies for the two-
 * dimensional arrays can be made using calls to memcpy().
 */
//------------------------------------------------------------------------------
bool BulirschStoer::Initialize()
{
   Integer i, j;

   Real stepSign = (stepSizeBuffer >= 0.0 ? 1.0 : -1.0);
   if (fabs(stepSizeBuffer) < minimumStep)
      stepSizeBuffer = minimumStep * stepSign;
   if (fabs(stepSizeBuffer) > maximumStep)
      stepSizeBuffer = maximumStep * stepSign;

   Propagator::Initialize();

   isInitialized = false;
    
   if (!depthInitialized)
      if (!SetMaximumDepth(depth))
         return isInitialized;

   if (physicalModel)
   {
      dimension = physicalModel->GetDimension();

      // Clear all allocated memory, in case dimension or depth has changed
      if (errorEstimates)
      {
         delete [] errorEstimates;
         errorEstimates = NULL;
      }

      if (coeffC)
      {
         delete [] coeffC;
         coeffC = NULL;
      }
        
      if (estimatedState)
      {
         delete [] estimatedState;
         estimatedState = NULL;
      }

      if (intermediates)
      {
         for (Integer i = 0; i < depth; i++)
         {
            delete [] intermediates[i];
            intermediates[i] = NULL;
         }
      }

      if (estimates)
      {
         for (Integer i = 0; i < depth; i++)
         {
            delete [] estimates[i];
            estimates[i] = NULL;
         }
      }

      if (mstate)
      {
         delete [] mstate;
         mstate = NULL;
      }

      if (nstate)
      {
         delete [] nstate;
         nstate = NULL;
      }

      // Now rebuild the data structures
      errorEstimates = new Real[dimension];
      if (!errorEstimates)
      {
         return isInitialized;
      }

      coeffC = new Real[dimension];
      if (!coeffC)
      {
         delete [] errorEstimates;
         errorEstimates = NULL;
         return isInitialized;
      }
        
      estimatedState = new Real[dimension];
      if (!estimatedState)
      {
         delete [] errorEstimates;
         errorEstimates = NULL;
         delete [] coeffC;
         coeffC = NULL;
         return isInitialized;
      }

      mstate = new Real[dimension];
      if (!mstate)
      {
         delete [] estimatedState;
         estimatedState = NULL;
         delete [] errorEstimates;
         errorEstimates = NULL;
         delete [] coeffC;
         coeffC = NULL;
         return isInitialized;
      }

      nstate = new Real[dimension];
      if (!nstate)
      {
         delete [] mstate;
         mstate = NULL;
         delete [] estimatedState;
         estimatedState = NULL;
         delete [] errorEstimates;
         errorEstimates = NULL;
         delete [] coeffC;
         coeffC = NULL;
         return isInitialized;
      }

      // Now do the 2-dimensional arrays
      for (i = 0; i < depth; ++i)
      {
         intermediates[i] = new Real[dimension];
         if (!intermediates[i])
         {
            delete [] errorEstimates;
            errorEstimates = NULL;
            delete [] coeffC;
            coeffC = NULL;
            delete [] intervals;
            intervals = NULL;
            delete [] estimatedState;
            estimatedState = NULL;
            delete [] mstate;
            mstate = NULL;
            delete [] nstate;
            nstate = NULL;
            for (j = 0; j < i; j++)
               delete [] intermediates[j];
            delete [] intermediates;
            intermediates = NULL;
            return isInitialized;
         }
      }

      for (i = 0; i < depth; ++i)
      {
         estimates[i] = new Real[dimension];
         if (!estimates[i])
         {
            delete [] errorEstimates;
            errorEstimates = NULL;
            delete [] coeffC;
             coeffC = NULL;
            delete [] intervals;
            intervals = NULL;
            delete [] estimatedState;
            estimatedState = NULL;
            delete [] mstate;
            mstate = NULL;
            delete [] nstate;
            nstate = NULL;
            for (j = 0; j < depth; j++)
            {
               delete [] intermediates[j];
               intermediates[j] = NULL;
            }
            for (j = 0; j < i; ++j)
            {
               delete [] estimates[j];
               estimates[j] = NULL;
            }
            return isInitialized;
         }
      }
      ddt = physicalModel->GetDerivativeArray();
   }

//   tolerance = (tolerance > mintolerance ? tolerance : mintolerance);
   accuracyWarningTriggered = false;

   first = true;
   isInitialized = true;
    
   return isInitialized;
}

//------------------------------------------------------------------------------
//  SetMaximumDepth(Integer d)
//------------------------------------------------------------------------------
/**
 * Routine called when the depth is set or changed. 
 * This routine allocates the arrays that depend on the depth of the 
 * extrapolation in the BS integrator.  It also fills in the coefficients that 
 * can be filled in without a PhysicalModel member defined.
 * 
 * @param <d>  The depth for the extrapolation.  If d <= 0, the arrays are
 * deallocated and the method returns false.
 */
//------------------------------------------------------------------------------
bool BulirschStoer::SetMaximumDepth(Integer d)
{
   Integer i, j;
   depthInitialized = false;

   if (levelError)
   {
      delete [] levelError;
      levelError = NULL;
   }

   if (ai)
   {
      delete [] ai;
      ai = NULL;
   }

   if (subinterval)
   {
      delete [] subinterval;
      subinterval = NULL;
   }

   if (intervals)
   {
      delete [] intervals;
      intervals = NULL;
   }

   if (intermediates)
   {
      for (j = 0; j < level; ++j)
         if (intermediates[j])
            delete [] intermediates[j];
      delete [] intermediates;
      intermediates = NULL;
   }

   if (estimates)
   {
      for (j = 0; j < level; ++j)
         if (estimates[j])
            delete [] estimates[j];
      delete [] estimates;
      estimates = NULL;
   }

   if (alpha)
   {
      for (j = 0; j < level; j++)
         if (alpha[j])
            delete [] alpha[j];
      delete [] alpha;
      alpha = NULL;
   }

   if (d <= 0)
   {
      return false;
   }

   depth = d;

   levelError = new Real[depth];
   if (!levelError)
      return false;

   ai = new Real[depth+2];
   if (!ai)
   {
      delete [] levelError;
      levelError = NULL;
      return false;
   }

   subinterval = new Integer[depth+2];
   if (!subinterval)
   {
      delete [] levelError;
      levelError = NULL;
      delete [] ai;
      ai = NULL;
      return false;
   }
   // Set the number of fractional steps to take at each level
   for (i = 0; i < depth+2; ++i)
      subinterval[i] = i * 2;

   // // An alternative set of subintervals from Numerical Recipes; the set
   // // above gives the intervals preferred by the NR authors
   // subinterval[0] = 0;
   // subinterval[1] = 2;
   // subinterval[2] = 4;
   // subinterval[3] = 6;
   // for (i = 4; i < depth+2; ++i)
   //     subinterval[i] = 2 * subinterval[i-2];


   // Array of substep sizes (squared)
   intervals = new Real[depth];
   if (!intervals)
   {
      delete [] levelError;
      levelError = NULL;
      delete [] ai;
      ai = NULL;
      delete [] subinterval;
      subinterval = NULL;
      return false;
   }

   intermediates = new Real*[depth];
   if (!intermediates)
   {
      delete [] levelError;
      levelError = NULL;
      delete [] ai;
      ai = NULL;
      delete [] subinterval;
      subinterval = NULL;
      delete [] intervals;
      intervals = NULL;
      return false;
   }
   for (i = 0; i < depth; ++i)
      intermediates[i] = NULL;

   estimates = new Real*[depth];
   if (!estimates)
   {
      delete [] levelError;
      levelError = NULL;
      delete [] ai;
      ai = NULL;
      delete [] subinterval;
      subinterval = NULL;
      delete [] intervals;
      intervals = NULL;
      delete [] intermediates;
      intermediates = NULL;
      return false;
   }
   for (i = 0; i < depth; ++i)
      estimates[i] = NULL;

    
   alpha = new Real*[depth + 1];
   if (!alpha)
   {
      delete [] levelError;
      levelError = NULL;
      delete [] ai;
      ai = NULL;
      delete [] subinterval;
      subinterval = NULL;
      delete [] intervals;
      intervals = NULL;
      delete [] intermediates;
      intermediates = NULL;
      delete [] estimates;
      estimates = NULL;
      return false;
   }
   for (i = 0; i < depth+1; ++i)
   {
      alpha[i] = new Real[depth + 1];
      if (!alpha[i])
      {
         for (j = 0; j < i; ++j)
            delete [] alpha[j];
         delete [] alpha;
         alpha = NULL;
         delete [] levelError;
         levelError = NULL;
         delete [] ai;
         ai = NULL;
         delete [] subinterval;
         subinterval = NULL;
         delete [] intervals;
         intervals = NULL;
         delete [] intermediates;
         intermediates = NULL;
         delete [] estimates;
         estimates = NULL;
         return false;
      }
   }

   // Fill in ai, alpha, kopt and kmax
   ai[0] = subinterval[0] + 1;
   for (i = 0; i < depth; ++i)
      ai[i+1] = ai[i] + subinterval[i+1];

   for (i = 1; i < depth; ++i)
      for (j = 0; j <= i; ++j)
      {
         if (j == i)
            alpha[j][i] = 1.0;
         else
            alpha[j][i] = pow(tolerance*bs_safety1, (ai[j+2]-ai[i+2]) /
                         ((ai[i+2]-ai[1]+1.0)*(2.0*j+3.0)));
      }

   // Determine "optimal" row for convergence
   for (kopt = 1; kopt < depth; kopt++)
      if (ai[kopt+1] > ai[kopt]*alpha[kopt-1][kopt])
         break;

   kmax = kopt;

   isInitialized = false;
   depthInitialized = true;
   return true;
}

//------------------------------------------------------------------------------
//  Step(Real dt)
//------------------------------------------------------------------------------
/**
 * This method drives the Bulirsch-Stoer propagation.
 *
 * @param <dt>
 * @return retval
 */
//------------------------------------------------------------------------------
bool BulirschStoer::Step(Real dt)
{
   if (!isInitialized)
      return false;

   Real stepleft = dt;
   bool retval = true;

   do
   {
      stepSize = stepleft;
      if (!Step())
         return false;
      stepleft -= stepTaken;
   } while (stepleft > 0.0);

   return retval;
}

//------------------------------------------------------------------------------
//  Step(void)
//------------------------------------------------------------------------------
/**
 * Method to propagate by the default step.  This method drives the
 * Bulirsch-Stoer propagation.  The high-level procedure is similar to other
 * integrators; it goes as follows:
 * <OL>
 * <LI>Initialize the data structures for the integration</LI>
 * <LI>Set to evaluate the first level of the extrapolation tableau</LI>
 * <LI>Use the modified midpoint method to extrapolate the estimated state for 
 * the current level (MidpointMethod())</LI>
 * <LI>Perform the polynomial fit  in order to generate the estimated state at
 * this level (PolyExtrapolate())</LI>
 * <LI>Estimate the error at the current level (EstimateError())</LI>
 * <LI>If the estimated error is less than the tolerance, accept the estimated 
 * state and adapt the parameters to take a larger step next time through 
 * (AdaptStep()); go to step 8</LI>
 * <LI>If the level is not at the maximum value, increment it and go to 
 * step 3</LI>
 * <LI>If the error is too large, reset the parameters to either take a smaller
 * step or go deeper into the extrapolation table (AdaptStep()), and then go to 
 * step 2</LI>
 * <LI>The integration was successful.  Copy the estimated state into the output 
 * state, and increment the time parameter on the PhysicalModel</LI>
 * </OL>
 * 
 * See the descriptions for each of the called methods for further information 
 * about these steps.
 *
 * @return boolean
 */
//------------------------------------------------------------------------------
bool BulirschStoer::Step(void)
{
    if (!isInitialized)
        return false;
        
    //  waw: 06/28/04 Unused variable    
    // Real nextstep = stepSize;
    // bool reduct = false;
    bool converged = false;
    Real tnew = 0.0; // waw: 06/28/04 Initialized
    Real eEstimate = 0.0; // waw: 06/28/04 Initialized

    do
        {
        tnew = stepSize;
        if (tnew == 0.0)
            return false;
        for (kused = 0; kused < kmax; ++kused)
                {
            level = kused;
            MidpointMethod(subinterval[kused+1]);
            PolyExtrapolate();

            // Now do the error control
            eEstimate = EstimateError();
            levelError[kused] = eEstimate;
            if ((kused > 1) && (eEstimate < tolerance))
                        {
                converged = true;
                if (!AdaptStep(eEstimate))
                    return false;
                break;
            }
        }

        if (!converged)
                {
            if (!AdaptStep(eEstimate))
                return false;
        }

        if (stepAttempts >= maxStepAttempts)
            return false;

    } while (!converged);

    memcpy(outState, estimatedState, dimension * sizeof(Real));
    physicalModel->IncrementTime(stepTaken);

    return true;
}

//------------------------------------------------------------------------------
//  RawStep(void)
//------------------------------------------------------------------------------
/**
 * Method to take an uncontrolled BS step
 * The current implementation just calls Step(), so the step is error 
 * controlled.  The Bulirsch-Stoer method has the error control built into the 
 * basic propagation loop, so an uncontrolled step doesn't make much sense here.
 * Basically, the error control and the tableau walking play off of each other,
 * so an uncontrolled step won't have any way to monitor the tableau levels
 * either.  We could stay at the previous levels, but this might impact later 
 * steps.
 *
 * @return boolean
 */
//------------------------------------------------------------------------------

bool BulirschStoer::RawStep(void)
{
    return Step();
}

//------------------------------------------------------------------------------
//  MidpointMethod(Integer substeps)
//------------------------------------------------------------------------------
/**
 * Implementation of the modified midpoint calculations.
 * This method extrapolates the next state of the system from the current state 
 * and the derivative information generated by the PhysicalModel member class, 
 * deriv.  The method implemented for the extrapolation is a midpoint method
 * acting at intermediate points of the step (the so called "substeps").  The 
 * first and last steps are not generated from a midpoint formulation; hence 
 * this is a "modified midpoint" extrapolation, implemented as described below.
 * 
 * The modified midpoint extrapolation of the state \f$\vec r\f$ across a step 
 * \f$\delta t\f$ is performed over \f$n\f$ substeps, each of size
 * 
 * \f[ h = {{\delta t}\over{n}} \f]
 * 
 * Given a function \f$f(t, \vec r)\f$ that generates first derivative 
 * information for the state:
 * 
 * \f[ {{d \vec r}\over{dt}}\bigg|_{t=t_0} = f\left(t_0, \vec r(t_0)\right) \f]
 * 
 * the intermediate approximations \f$\vec z_n\f$ are given by
 * 
 * \f[\vec z_0 = \vec r(t_0)\f]
 * 
 * The first substep is generated from direct extrapolation, using 
 * 
 * \f[\vec z_1 = \vec z_0 + h f(t, \vec z_0)\f]
 * 
 * All of the subsequent substeps are centered; the \f$m+1^{st}\f$ is taken 
 * using the \f$m-1^{st}\f$ result and the derivative information at the 
 * \f$m^{th}\f$ intermediate state.  This centered intermediate state is given
 * by
 * 
 * \f[\vec z_{m+1} = \vec z_{m-1} + 2h f(t + m h, \vec z_m)\f]
 * 
 * This formula is evaluated for \f$m = 1, 2, ..., n-1\f$.  The final 
 * extrapolated state is given by
 * 
 * \f[\vec r(t+\delta t) = {1\over 2}\left[\vec z_n + \vec z_{n-1} 
 *       + h f(t+\delta t, \vec z_n)\right] \f]
 * 
 * This produces the final estimated state.
 * 
 * @param substeps  Number of intermediate evaluations used during extrapolation
 * @return boolean
 */
//------------------------------------------------------------------------------
bool BulirschStoer::MidpointMethod(Integer substeps)
{
    if (substeps < 2)
        return false;
    
    Integer i, j;

    Real substepsize = stepSize / substeps, swap, h2 = 2.0 * substepsize;

    memcpy(mstate, inState, dimension * sizeof(Real));

    if (!physicalModel->GetDerivatives(mstate))
        return false;
    for (j = 0; j < dimension; ++j)
        nstate[j] = mstate[j] + substepsize * ddt[j];

    for (i = 1; i < substeps; ++i)
        {
        if (!physicalModel->GetDerivatives(nstate, substepsize * (i-1)))
            return false;
        for (j = 0; j < dimension; ++j)
                {
            swap = mstate[j] + h2 * ddt[j];
            mstate[j] = nstate[j];
            nstate[j] = swap;
        }
    }

    if (!physicalModel->GetDerivatives(nstate, stepSize))
        return false;

    for (j = 0; j < dimension; ++j)
        estimates[level][j] = 0.5*(mstate[j] + nstate[j] + substepsize*ddt[j]);

    return true;
}

//------------------------------------------------------------------------------
// PolyExtrapolate(void)
//------------------------------------------------------------------------------
/**
 * Routine used to perform polynomial extrapolation of the state.
 * This method extrapolated the state based on the results of the modified 
 * midpoint estimates.  The generated data is the estimated state and the 
 * estimated error in the state.
 * 
 * This code implements Neville's algorithm for the polynomial interpolation, as
 * described in section 3.1 of Numerical Recipes.  The algorithm is applied to 
 * each component of the state vector in order to build the polynomial 
 * coefficients for that component.
 * 
 * The intervals[] array defines the independent variable for the Richardson 
 * extrapolation.  The ith interval \f$I_i\f$ is the square of the step size 
 * divided by the number of steps taken to extrapolate across the interval.  For
 * a given level \f$L\f$, this is
 * 
 * \f[I_i = \left({{h}\over{2 L}}\right)^2\f]
 * 
 * The implementation here resembles the implementation in <B>Numerical 
 * Recipes</B>.  Several refinements have been made; most notably, the arrays 
 * have been rearranged to facilitate block memory copies when appropriate.
 * 
 * The procedure is to treat successive extrapolations of the state vector as
 * a series of points on a curve \f$S(\vec r_i; h)\f$.  Here \f$h\f$ is the 
 * square of the size of the substeps taken for the extrapolation.  We then fit
 * a polynomial to the points generated by the extrapolation; the \f$N^{th}\f$ 
 * order polynomial fit is given by
 * 
 * \f[\vec P(h) = 
 *       {{(h-h_2)(h-h_3)...(h-h_N)}\over{(h_1-h_2)(h_1-h_3)...(h_1-h_N)}}\vec r_1 
 *       + {{(h-h_1)(h-h_3)...(h-h_N)}\over{(h_2-h_2)(h_2-h_3)...(h_2-h_N)}}\vec r_2 
 *       + ... 
 *       + {{(h-h_1)(h-h_1)...(h-h_{N-1})}\over{(h_N-h_2)(h_N-h_3)...
 *               (h_N-h_{N-1})}}\vec r_N \f]
 * 
 *  This polynomial is evaluated at \f$h=0\f$ to produce the estimated state from 
 *  the extrapolation.
 *
 * @return boolean
 */
//------------------------------------------------------------------------------
bool BulirschStoer::PolyExtrapolate(void)
{
    Integer i, j;
    Real delta, f1, f2, q;

    // Fill in the square of the extrapolation step taken at this level:
    intervals[level] = stepSize*stepSize/
                       (subinterval[level+1]*subinterval[level+1]);

    memcpy(errorEstimates, estimates[level], dimension * sizeof(Real));
    memcpy(estimatedState, estimates[level], dimension * sizeof(Real));

    if (level == 0)         // First time through, just store estimated state
        memcpy(intermediates[0], estimates[level], dimension * sizeof(Real));
    else {
        memcpy(coeffC, estimates[level], dimension * sizeof(Real));
        for (i = 0; i < level; ++i)
                {
            delta = 1.0 / (intervals[level-1-i] - intervals[level]);
            f1 = intervals[level] * delta;
            f2 = intervals[level-1-i] * delta;
            for (j = 0; j < dimension; ++j)
                        {
                q = intermediates[i][j];
                intermediates[i][j] = errorEstimates[j];
                delta = coeffC[j] - q;
                errorEstimates[j] = f1 * delta;
                coeffC[j] = f2 * delta;
                estimatedState[j] += errorEstimates[j];
            }
        }
        memcpy(intermediates[level], errorEstimates, dimension*sizeof(Real));
    }

    return true;
}

//------------------------------------------------------------------------------
//  EstimateError(void)
//------------------------------------------------------------------------------
/**
 * Error estimation calculations for the Bulirsch-Stoer Integrato.
 * The error estimate for the Bulirsch-Stoer integrator is the difference in the
 * correction to the estimated state between the current level and the previous
 * level.  This absolute error estimate is scaled by the change in the component
 * (i.e. the estimated state minus the initial state), and the largest scaled 
 * error estimate is returned to the caller.
 *
 * @return absolute error estimate
 */
//------------------------------------------------------------------------------
Real BulirschStoer::EstimateError(void)
{
    return physicalModel->EstimateError(errorEstimates, estimatedState);
}

// Former implementation
//    {
//        Real err = 0.0, diff;
//        for (Integer i = 0; i < dimension; ++i)
//        {
//            diff = inState[i] - estimatedState[i];
//
//            if (diff > 1.0e-8)
//                errorEstimates[i] = fabs(errorEstimates[i] / diff);
//            else
//                errorEstimates[i] = fabs(errorEstimates[i]);
//            if (errorEstimates[i] > err)
//                err = errorEstimates[i];
//        }
//        return err;
//    }

//------------------------------------------------------------------------------
//  AdaptStep(Real maxerror)
//------------------------------------------------------------------------------
/**
 * Stepsize adaptation for the Bulirsch-Stoer Integrator.
 * Stepsize control for the Bulirsch-Stoer integrator is a bit more complicated
 * than for the other intgration methods because there are two separate 
 * parameters that control the error in a given step: the size of the step and 
 * the depth of the extrapolation into the table of substeps.  The error can be
 * reduced by either increasing the depth (up to the maximum depth of the table,
 * 8 levels by default) or decreasing the stepsize.  Similarly, if the estimated 
 * error is less than the tolerance, it can be increased by taking larger steps.
 * In the latter case, we first increment the maximum level if possible, and 
 * then estimate the new stepsize to go with that level.
 * 
 * @param maxerror  The largest relative error estimate found in the integration
 * @return boolean
 */
//------------------------------------------------------------------------------
bool BulirschStoer::AdaptStep(Real maxerror)
{
    Real factor = 1.0, errkm;

    if (maxerror > tolerance)
    {
       if (GmatMathUtil::Abs(stepSize) == minimumStep)
       {
          if (stopIfAccuracyViolated)
          {
             throw PropagatorException(
                   "BulirschStoer: Accuracy settings will be violated with current step size values.\n");
          }
          else
          {
             if (!accuracyWarningTriggered) // so only write the warning once per propagation command
             {
                accuracyWarningTriggered = true;
                MessageInterface::PopupMessage(Gmat::WARNING_,
                   "BulirschStoer: Accuracy settings will be violated with current step size values.\n");
             }
             return false;
          }
       }
        // Step is too large: reduce it 
        errkm = pow(maxerror/(bs_safety1 * tolerance), 1.0 / (2*kused+1));

        if ((kused == kmax) || (kused == kopt+1)) 
            factor = bs_safety2 / errkm;
        else if ((kused == kopt) && (alpha[kopt-1][kopt] < errkm))
            factor = 1.0 / errkm;
        else if ((kopt == kmax) && (alpha[kused][kmax-1] < errkm))
            factor = alpha[kused][kmax-1] * bs_safety2 / errkm;
        else if (alpha[kused][kopt-1] < errkm)
            factor = alpha[kused][kopt-1] / errkm;

        factor = (factor > minimumReduction ? minimumReduction : factor);
        factor = (factor < maximumReduction ? maximumReduction : factor);

        stepSize *= factor;
        ++stepAttempts;
    }
    else
    {
        // The step succeeded; step or depth can be increased
        first = false;
        stepTaken = stepSize;
        Real workingMin = 1.0e35, work, scale = 1.0, errorRatio;
        for (Integer i = 0; i <= kused; i++)
        {
            errorRatio = levelError[i] / tolerance;
            factor = (errorRatio > scale_dt ? errorRatio : scale_dt);
            work = factor * ai[i+2];
            if (work < workingMin)
            {
                scale = factor;
                workingMin = work;
                kopt = (i+1 > kmax ? kmax : i+1);
            }
        }

        // Set stepSize to new value
        stepSize /= scale;

        if ((kopt >= kused) && (kopt < kmax))
        {
            work = scale / alpha[kopt-1][kopt];
            factor = (scale < scale_dt ? scale : scale_dt);
            if (ai[kopt+1] * factor < workingMin)
            {
                ++kopt;
                stepSize = stepTaken / factor;
            }
        }

        // Adapt to the range constraints on the step
        Real sign = (stepSize >= 0.0 ? 1.0 : -1.0);
        stepSize = (fabs(stepSize) > minimumStep ? stepSize : minimumStep*sign);
        stepSize = (fabs(stepSize) < maximumStep ? stepSize : maximumStep*sign);

        // Handle the fixed step mode case
        if (fixedStep) 
            stepSize = (fabs(stepSize) < fabs(fixedStepsize) ? 
                        stepSize : fixedStepsize);

        stepAttempts = 0;
    }

    return true;
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Helper method used to find the text name for the class parameters.
 * This static method is used to obtain text descriptions for the parameters 
 * defined by the class.  These strings can be used to fill in the descriptors
 * for user interface elements, so that those elements have labels that match 
 * the intent of each of the user selectable parameters.
 * 
 * @param <id>  ID of the parameter that needs the text string
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string BulirschStoer::GetParameterText(const Integer id) const
{
    if (id >= MINIMUM_REDUCTION && id < BulirschStoerParamCount)
        return PARAMETER_TEXT[id - IntegratorParamCount];
    else
        return Integrator::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer BulirschStoer::GetParameterID(const std::string &str) const
{
    for (Integer i = MINIMUM_REDUCTION; i < BulirschStoerParamCount; i++)
    {
        if (str == PARAMETER_TEXT[i - IntegratorParamCount])
            return i;
    }

    return Integrator::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType BulirschStoer::GetParameterType(const Integer id) const
{
    if (id >= MINIMUM_REDUCTION && id < BulirschStoerParamCount)
        return PARAMETER_TYPE[id - IntegratorParamCount];
    else
        return Integrator::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string BulirschStoer::GetParameterTypeString(const Integer id) const
{
   if (id >= MINIMUM_REDUCTION && id < BulirschStoerParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return Integrator::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Hides parameters that are not intended for user access
 *
 * @param id The ID of the parameter
 *
 * @return true if the parameter is supposed to be hidden, false if it is
 *         visible
 */
//------------------------------------------------------------------------------
bool BulirschStoer::IsParameterReadOnly(const Integer id) const
{
   if (id == MIN_TOLERANCE)
      return true;

   return Integrator::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Hides parameters that are not intended for user access
 *
 * @param label The script label of the parameter
 *
 * @return true if the parameter is supposed to be hidden, false if it is
 *         visible
 */
//------------------------------------------------------------------------------
bool BulirschStoer::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real BulirschStoer::GetRealParameter(const Integer id) const
{
   if      (id == MINIMUM_REDUCTION)       return minimumReduction;
   else if (id == MAXIMUM_REDUCTION)       return maximumReduction;
   else if (id == MIN_TOLERANCE)          // Deprecated
      return tolerance;

   return Integrator::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real BulirschStoer::GetRealParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);

   return GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Method used to Set parameters for the BS Integrator.
 * This method sets the tolerance and tolerance limit for the Bulirsch-Stoer 
 * integrator, and calls Integrator::SetParameter() for other parameters.  The
 * implementation here prevents setting tolerances of more than 100%.  If the
 * minimum tolerance is set to a value larger than the current tolerance, the
 * current value is raised to the minimum value.
 * 
 * @param <id>  ID for the parameter being set
 * @param <value>  New value for the parameter
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real BulirschStoer::SetRealParameter(const Integer id, const Real value)
{
   if (id == MINIMUM_REDUCTION)
   {
      minimumReduction = value;
      return minimumReduction;
   }
   else if (id == MAXIMUM_REDUCTION)
   {
      maximumReduction = value;
      return maximumReduction;
   }
   else if (id == MIN_TOLERANCE)
   {
      MessageInterface::ShowMessage
            ("*** WARNING *** The MinimumTolerance field of Bulirsch-Stoer "
             "integrators is deprecated, has no effect, and will be removed "
             "from a future build.\n");

      return tolerance;

//      if (fabs(value) <= 1.0)
//      {
//         mintolerance = fabs(value);
//         tolerance = (tolerance > mintolerance ? tolerance : mintolerance);
//         return mintolerance;
//      }
   }

   return Integrator::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real BulirschStoer::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}
