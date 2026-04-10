//$Id$
//------------------------------------------------------------------------------
//                           DerivativeModel
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
// number NNG06CA54C
//
// Created: 2008/03/28
//
/**
 * Base class for gradients, Jacobians, Hessians, and so forth.
 */
//------------------------------------------------------------------------------


#include "DerivativeModel.hpp"

#include "SolverException.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_DERIVMODEL


//-----------------------------------------
// public methods
//-----------------------------------------


//------------------------------------------------------------------------------
// DerivativeModel()
//------------------------------------------------------------------------------
/**
 * Default constructor (Called via a derived class)
 * 
 * @return A new instance of the class, configured with default data
 */
//------------------------------------------------------------------------------
DerivativeModel::DerivativeModel() :
   calcMode          (FORWARD_DIFFERENCE),
   variableCount     (0)
{
}

//------------------------------------------------------------------------------
// DerivativeModel()
//------------------------------------------------------------------------------
/**
 * Destructor (Called via a derived class)
 */
//------------------------------------------------------------------------------
DerivativeModel::~DerivativeModel()
{
}

//------------------------------------------------------------------------------
// DerivativeModel(const DerivativeModel& dm)
//------------------------------------------------------------------------------
/**
 * Copy constructor (Called via a derived class)
 * 
 * @param dm The DerivativeModel that is copied into this one.
 * 
 * @return A new instance of the class, configured with default data
 */
//------------------------------------------------------------------------------
DerivativeModel::DerivativeModel(const DerivativeModel& dm) :
   calcMode          (dm.calcMode),
   variableCount     (dm.variableCount),
   pert              (dm.pert),
   plusPertEffect    (dm.plusPertEffect),
   minusPertEffect   (dm.minusPertEffect)
{
   
}

//------------------------------------------------------------------------------
// DerivativeModel& operator=(const DerivativeModel &dm)
//------------------------------------------------------------------------------
/**
 * DerivativeModel assignment operator
 * 
 * Copies the data from another DerivativeModel instance into this one.
 * 
 * @param dm The supplier of the DerivativeModel data.
 * 
 * @return A reference to this instance
 */
//------------------------------------------------------------------------------
DerivativeModel& DerivativeModel::operator=(const DerivativeModel& dm)
{
   if (&dm != this)
   {
      calcMode = dm.calcMode;
      variableCount = dm.variableCount;
      pert = dm.pert;
      plusPertEffect = dm.plusPertEffect;
      minusPertEffect = dm.minusPertEffect;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// void SetDifferenceMode(derivativeMode mode)
//------------------------------------------------------------------------------
/**
 * Sets the mode used to calculate the derivative.
 * 
 * @param mode The new difference mode.  Difference modes are idempotent.
 */
//------------------------------------------------------------------------------
void DerivativeModel::SetDifferenceMode(derivativeMode mode)
{
   calcMode = mode;
}


//------------------------------------------------------------------------------
// bool Initialize(UnsignedInt varCount, UnsignedInt componentCount)
//------------------------------------------------------------------------------
/**
 * Method used to set up the internal DerivativeModel data structures prior to 
 * use.
 * 
 * @param varCount The number of variables used in this set of calculations.
 * @param componentCount The number of dependent parameters
 * 
 * @return true if initialization succeeds
 */
//------------------------------------------------------------------------------
bool DerivativeModel::Initialize(UnsignedInt varCount, 
                                 UnsignedInt componentCount)
{
   UnsignedInt elementCount = varCount * componentCount;
   
   if (elementCount == 0)
      throw SolverException(
            "DerivativeModel cannot initialize because elementCount == 0");
   
   variableCount = varCount;
   
   pert.clear();
   for (int i = 0; i < variableCount; ++i)
   {
      pert.push_back(0.0);
   }
   
   plusPertEffect.clear();
   minusPertEffect.clear();
   for (UnsignedInt i = 0; i < elementCount; ++i)
   {
      plusPertEffect.push_back(0.0);
      minusPertEffect.push_back(0.0);
   }
   
   #ifdef DEBUG_DERIVMODEL
      MessageInterface::ShowMessage(
         "Derivative Model initialized in mode %d with %d elements\n", 
         calcMode, elementCount);
   #endif

   return true;
}


//------------------------------------------------------------------------------
// void Achieved(Integer pertNumber, Integer componentId, Real dx, Real value, 
//               bool plusEffect)
//------------------------------------------------------------------------------
/**
 * Method used to specify values obtained for the dependent parameters.
 * 
 * This method sets values for nominal and perturbed runs of the Mission Control 
 * Sequence, for later use in calculation of the derivative.  
 * 
 * @param pertNumber  Number of the perturbation being run, or -1 for a 
 *                    nominal run.  (Nominal run data should be handled in the 
 *                    derived classes; if the call reaches this method, an 
 *                    exception is thrown.)
 * @param componentId Identity of the dependent parameter being reported.
 * @param dx          The size of the perturbation
 * @param value       The resulting value of the dependent parameter
 * @param plusEffect  true for positive perturbations, false for negative, so 
 *                    that the differencing model can place the results in the 
 *                    correct vector.
 */
//------------------------------------------------------------------------------
void DerivativeModel::Achieved(Integer pertNumber, Integer componentId, Real dx, 
                               Real value, bool plusEffect)
{
   if (pertNumber == -1)
      throw SolverException(
         "Setting a nominal value in the DerivativeModel base class Achieved() "
         "method is not allowed.");
   else
   {
      if (pertNumber >= (Integer)pert.size())
         throw SolverException(
               "Invalid pert element when setting an achieved value.");

      #ifdef DEBUG_DERIVMODEL
         MessageInterface::ShowMessage(
            "   %s perturbation #%d, size %.12lf gives %.12lf for id %d\n", 
            (plusEffect ? "Positive" : "Negative"), pertNumber, dx, value, 
            componentId);
      #endif
      
      pert.at(pertNumber) = dx;
      if (plusEffect)
         plusPertEffect.at(pertNumber + componentId * variableCount) = value;
      else
         minusPertEffect.at(pertNumber + componentId * variableCount) = value;
   }
}
