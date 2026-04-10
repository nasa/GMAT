//$Id$
//------------------------------------------------------------------------------
//                                Gradient
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
// Author: Darrel J. Conway/Thinking Systems, Inc.
// Created: 2008.03.26
//
/**
 * Class used to calculate gradients.
 */
//------------------------------------------------------------------------------


#include "Gradient.hpp"

#include "SolverException.hpp"
#include "MessageInterface.hpp"


// #define DEBUG_GRADIENT


//-----------------------------------------
// public methods
//-----------------------------------------


//------------------------------------------------------------------------------
// Gradient()
//------------------------------------------------------------------------------
/**
 * Default constructor
 * 
 * @return A new instance of the class, configured with default data
 */
//------------------------------------------------------------------------------
Gradient::Gradient() : 
   DerivativeModel   (),
   nominal           (9876.54321)
{
}


//------------------------------------------------------------------------------
// ~Gradient()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Gradient::~Gradient()
{
}

//------------------------------------------------------------------------------
// Gradient(const Gradient &grad)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 * 
 * @param grad The Gradient instance that is copied to the new one.
 * 
 * @return A new instance of the class, configured to match the input instance
 */
//------------------------------------------------------------------------------
Gradient::Gradient(const Gradient &grad) : 
   DerivativeModel   (grad),
   nominal           (grad.nominal),
   gradient          (grad.gradient)
{
}

//------------------------------------------------------------------------------
// Gradient& operator=(const Gradient &grad)
//------------------------------------------------------------------------------
/**
 * Gradient assignment operator
 * 
 * Copies the data from another Gradient instance into this one.
 * 
 * @param grad The supplier of the Gradient data.
 * 
 * @return A reference to this instance
 */
//------------------------------------------------------------------------------
Gradient& Gradient::operator=(const Gradient &grad)
{
   if (&grad != this)
   {
      DerivativeModel::operator=(grad);
      nominal = grad.nominal;
      gradient = grad.gradient;
      calcMode = grad.calcMode;
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// bool Initialize(UnsignedInt varCount, UnsignedInt componentCount)
//------------------------------------------------------------------------------
/**
 * Method used to set up the internal Gradient data structures prior to use.
 * 
 * @param varCount The number of variables used in this set of calculations.
 * @param componentCount The number of dependent parameters
 * 
 * @return true if initialization succeeds
 */
//------------------------------------------------------------------------------
bool Gradient::Initialize(UnsignedInt varCount, UnsignedInt componentCount)
{
   DerivativeModel::Initialize(varCount);
   
   gradient.clear();   
   for (UnsignedInt i = 0; i < varCount; ++i)
      gradient.push_back(0.0);

   #ifdef DEBUG_GRADIENT
      MessageInterface::ShowMessage(
         "Gradient initialized in mode %d with %d variables\n", calcMode, 
         varCount);
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
 * Sequence, for later use in calculation of the gradient.  
 * 
 * @param pertNumber  Number of the perturbation being run, or -1 for a 
 *                    nominal run.
 * @param componentId Identity of the dependent parameter being reported.
 * @param dx          The size of the perturbation
 * @param value       The resulting value of the dependent parameter
 * @param plusEffect  true for positive perturbations, false for negative, so 
 *                    that the differencing model can place the results in the 
 *                    correct vector.
 */
//------------------------------------------------------------------------------
void Gradient::Achieved(Integer pertNumber, Integer componentId, Real dx, 
                        Real value, bool plusEffect)
{
   if (pertNumber == -1)
   {
      #ifdef DEBUG_GRADIENT
         MessageInterface::ShowMessage(
            "Setting Gradient Nominal Value to %.12lf\n", value);
      #endif
      nominal = value;
   }
   else
   {
      DerivativeModel::Achieved(pertNumber, componentId, dx, value, plusEffect);
   }
}


//------------------------------------------------------------------------------
// bool Calculate(std::vector<Real> &grad)
//------------------------------------------------------------------------------
/**
 * Calculates the gradient.
 * 
 * This method calculates the gradient using the specified differencing mode and
 * puts the results in the input vector.
 * 
 * @note User supplied gradients are not yet implemented.
 * 
 * @param grad The vector that receives the calculated gradient.
 * 
 * @return true if the calculation succeeds.
 */
//------------------------------------------------------------------------------
bool Gradient::Calculate(std::vector<Real> &grad)
{
   if (calcMode == USER_SUPPLIED)
      return true;
 
   UnsignedInt gradSize = pert.size();
   for (UnsignedInt i = 0; i < gradSize; ++i)
   {
      #ifdef DEBUG_GRADIENT
            MessageInterface::ShowMessage(
                  "   Component %d of %d, pert = %.12lf\n", i, gradSize, 
                  pert.at(i));
      #endif   

            if (pert.at(i) == 0.0)
         throw SolverException(
               "Perturbation of size 0.0 found in gradient calculation");

      #ifdef DEBUG_GRADIENT
         MessageInterface::ShowMessage(
            "   Finding Gradient in mode %d\n", calcMode);
      #endif

      switch (calcMode) 
      {
         case FORWARD_DIFFERENCE:
            #ifdef DEBUG_GRADIENT
            MessageInterface::ShowMessage("   FD[%d]:  %.15lf - %.15lf / %.15lf\n", 
                  i, plusPertEffect[i], nominal, pert[i]);
            #endif

            gradient.at(i) = (plusPertEffect.at(i) - nominal) / pert.at(i);
            break;
            
         case CENTRAL_DIFFERENCE:
            #ifdef DEBUG_GRADIENT
            MessageInterface::ShowMessage("   CD[%d]:  %.15lf - %.15lf / 2 * %.15lf\n", 
                  i, plusPertEffect[i], minusPertEffect[i], pert[i]);
            #endif

            gradient[i] = (plusPertEffect[i] - minusPertEffect[i]) / 
                          (2.0 * pert[i]);
            break;
            
         case BACKWARD_DIFFERENCE:
            #ifdef DEBUG_GRADIENT
            MessageInterface::ShowMessage("   BD[%d]:  %.15lf - %.15lf / %.15lf\n", 
                  i, nominal, minusPertEffect[i], pert[i]);
            #endif

            gradient[i] = (nominal - minusPertEffect[i]) / pert[i];
            break;
            
         default:
            throw SolverException(
                  "Gradient differencing mode is not available");
      }
   }

   #ifdef DEBUG_GRADIENT
      MessageInterface::ShowMessage(
         "      Gradient = [");
      for (UnsignedInt i = 0; i < gradSize; ++i)
      {
         MessageInterface::ShowMessage("%.12lf", gradient[i]);
         if (i < gradSize - 1)
            MessageInterface::ShowMessage(", ");
      }
      MessageInterface::ShowMessage("]\n");
   #endif

   grad = gradient;
   return true;
}
