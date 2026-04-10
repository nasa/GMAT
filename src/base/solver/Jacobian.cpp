//$Id$
//------------------------------------------------------------------------------
//                              Jacobian
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
// Created: 2008/03/27
//
/**
 * Base class for Jacobian calculations used by the Solvers.
 */
//------------------------------------------------------------------------------

#include "Jacobian.hpp"

#include "SolverException.hpp"
#include "MessageInterface.hpp"


// #define DEBUG_JACOBIAN
// #define DEBUG_JACOBIAN_DETAILS


//-----------------------------------------
// public methods
//-----------------------------------------


//------------------------------------------------------------------------------
// Jacobian()
//------------------------------------------------------------------------------
/**
 * Default constructor
 * 
 * @return A new instance of the class, configured with default data
 */
//------------------------------------------------------------------------------
Jacobian::Jacobian() :
   DerivativeModel         (),
   numComponents           (0)
{
}

//------------------------------------------------------------------------------
// ~Jacobian()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Jacobian::~Jacobian()
{
}

//------------------------------------------------------------------------------
// Jacobian(const Jacobian& jac) 
//------------------------------------------------------------------------------
/**
 * Copy constructor
 * 
 * @param jac The Jacobian instance that is copied to the new one.
 * 
 * @return A new instance of the class, configured to match the input instance
 */
//------------------------------------------------------------------------------
Jacobian::Jacobian(const Jacobian &jac) :
   DerivativeModel         (jac),
   numComponents           (jac.numComponents)
{
   
}

//------------------------------------------------------------------------------
// Jacobian& operator=(const Jacobian &jac)
//------------------------------------------------------------------------------
/**
 * Jacobian assignment operator
 * 
 * Copies the data from another Jacobian instance into this one.
 * 
 * @param jac The supplier of the Jacobian data.
 * 
 * @return A reference to this instance
 */
//------------------------------------------------------------------------------
Jacobian& Jacobian::operator=(const Jacobian &jac)
{
   if (&jac != this)
   {
      DerivativeModel::operator=(jac);
      
      numComponents = jac.numComponents;
      
      jacobian      = jac.jacobian;
      nominal       = jac.nominal;
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// bool Initialize(UnsignedInt varCount, UnsignedInt componentCount)
//------------------------------------------------------------------------------
/**
 * Method used to set up the internal Jacobian data structures prior to use.
 * 
 * @param varCount The number of variables used in this set of calculations.
 * @param componentCount The number of dependent parameters
 * 
 * @return true if initialization succeeds
 */
//------------------------------------------------------------------------------
bool Jacobian::Initialize(UnsignedInt varCount, UnsignedInt componentCount)
{
   DerivativeModel::Initialize(varCount, componentCount);

   numComponents = componentCount;
   UnsignedInt elementCount = variableCount * numComponents;
   
   for (UnsignedInt i = 0; i < numComponents; ++i)
      nominal.push_back(0.0);
   
   for (UnsignedInt i = 0; i < elementCount; ++i)
      jacobian.push_back(0.0);
   
   #ifdef DEBUG_JACOBIAN
      MessageInterface::ShowMessage(
         "Jacobian initialized in mode %d with %d variables and %d components "
         "giving %d entries\n", calcMode, varCount, componentCount, 
         elementCount);
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
 * Sequence, for later use in calculation of the Jacobian.  
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
void Jacobian::Achieved(Integer pertNumber, Integer componentId, 
                        Real dx, Real value, bool plusEffect)
{
   if (pertNumber == -1)
   {
      #ifdef DEBUG_JACOBIAN
         MessageInterface::ShowMessage(
               "   Nominal data[%d], gives %.12lf\n", componentId, 
               value);
      #endif

      nominal.at(componentId) = value;
   }
   else
   {
      DerivativeModel::Achieved(pertNumber, componentId, dx, value, plusEffect);
   }
}

//------------------------------------------------------------------------------
// bool Calculate(std::vector<Real> &jac)
//------------------------------------------------------------------------------
/**
 * Calculates the Jacobian.
 * 
 * This method calculates the Jacobian using the specified differencing mode and
 * puts the results in the input vector.
 * 
 * The Jacobian data is filled column by column.  The final data in the vector 
 * is tabulated one row at a time -- for example, if there are 3 variables v0, 
 * v1, and v2 and two dependent parameters p0 and p1, the Jacobian vector 
 * contains these six elements on return from this method:
 * 
 *    jac = [dp0/dv0 dp0/dv1 dp0/dv2 dp1/dv0 dp1/dv1 dp1/dv2]
 * 
 * @note User supplied Jacobians are not yet implemented.
 * 
 * @param jac The vector that receives the calculated Jacobian.
 * 
 * @return true if the calculation succeeds.
 */
//------------------------------------------------------------------------------
bool Jacobian::Calculate(std::vector<Real> &jac)
{
   for (UnsignedInt i = 0; i < (UnsignedInt)variableCount; ++i)
   {
      if (pert[i] == 0.0)
         throw SolverException(
               "Perturbation of size 0.0 found in Jacobian calculation");

      #ifdef DEBUG_JACOBIAN
         MessageInterface::ShowMessage(
            "   Finding Jacobian in mode %d\n", calcMode);
      #endif
         
      for (UnsignedInt j = 0; j < numComponents; ++j)
      {
         UnsignedInt rowStart = j * variableCount;
         switch (calcMode) 
         {
            case FORWARD_DIFFERENCE:
               jacobian.at(rowStart+i) = (plusPertEffect.at(rowStart+i) - nominal.at(j))/ 
                                       pert.at(i);

               #ifdef DEBUG_JACOBIAN_DETAILS      
                  MessageInterface::ShowMessage(
                     "         FD[%d]: (%.12lf - %.12lf) / %.12lf = %.12lf\n", 
                     rowStart+i, plusPertEffect[rowStart+i], nominal[j], 
                     pert[i], jacobian[rowStart+i]);
               #endif

               break;
               
            case CENTRAL_DIFFERENCE:
               jacobian[rowStart+i] = (plusPertEffect[rowStart+i] - 
                             minusPertEffect[rowStart+i]) / 
                             (2.0 * pert[i]);

               #ifdef DEBUG_JACOBIAN_DETAILS      
                  MessageInterface::ShowMessage(
                     "         CD[%d]: (%.12lf - %.12lf) / 2 * %.12lf = %.12lf\n", 
                     rowStart+i, plusPertEffect[rowStart+i], 
                     minusPertEffect[rowStart+i], pert[i], 
                     jacobian[rowStart+i]);
               #endif
               
               break;
               
            case BACKWARD_DIFFERENCE:
               jacobian[rowStart+i] = (nominal[j] - 
                              minusPertEffect[rowStart+i]) / 
                              pert[i];

               #ifdef DEBUG_JACOBIAN_DETAILS      
                  MessageInterface::ShowMessage(
                     "         CD[%d]: (%.12lf - %.12lf) / %.12lf = %.12lf\n", 
                     rowStart+i, minusPertEffect[rowStart+i], 
                     nominal[j], pert[i], 
                     jacobian[rowStart+i]);
               #endif
               
               break;
               
            case USER_SUPPLIED:
            default:
               throw SolverException(
                     "Jacobian differencing mode is not available");
         }
      }
   }

   #ifdef DEBUG_JACOBIAN
      MessageInterface::ShowMessage("      Jacobian = \n");
      for (UnsignedInt i = 0; i < compSize; ++i)
      {
         MessageInterface::ShowMessage(
            "                 [");
         for (UnsignedInt j = 0; j < pertSize; ++j)
         {
            MessageInterface::ShowMessage("%.12lf", jacobian[i * pertSize + j]);
            if (j < pertSize - 1)
               MessageInterface::ShowMessage(", ");
         }
         MessageInterface::ShowMessage("]\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   jac = jacobian;
   return true;
}
