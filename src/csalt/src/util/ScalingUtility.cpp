//------------------------------------------------------------------------------
//                              ScalingUtility
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Author: Wendy Shoan / NASA
// Created: 2018.12.04
//
/**
 * From original Python prototype:
 * Author: S. Hughes, N. Hatten
 */
//------------------------------------------------------------------------------

#include "ScalingUtility.hpp"
#include "LowThrustException.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_ADD_SCALING

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
ScalingUtility::ScalingUtility()
{
   // maps are empty at the start by default
   AddUnitAndShift("DU",   1.0, 0.0);
   AddUnitAndShift("TU",   1.0, 0.0);
   AddUnitAndShift("VU",   1.0, 0.0);
   AddUnitAndShift("MU",   1.0, 0.0);
   AddUnitAndShift("ACCU", 1.0, 0.0);
   AddUnitAndShift("MFU",  1.0, 0.0);
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
ScalingUtility::ScalingUtility(const ScalingUtility &copy)
{
   if (!copy.unitFactors.empty())
   {
      unitFactors = copy.unitFactors;
   }
   if (!copy.unitShifts.empty())
   {
      unitShifts = copy.unitShifts;
   }
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
ScalingUtility& ScalingUtility::operator=(const ScalingUtility &copy)
{
   if (&copy == this)
      return *this;

   if (!copy.unitFactors.empty())
   {
      unitFactors.clear();
      unitFactors = copy.unitFactors;
   }
   if (!copy.unitShifts.empty())
   {
      unitShifts.clear();
      unitShifts = copy.unitShifts;
   }

   return *this;
   
}
   
//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
ScalingUtility::~ScalingUtility()
{
   unitFactors.clear();
   unitShifts.clear();
}

bool ScalingUtility::ValidateUnit(const std::string &unitName,
                                  bool  isException)
{
   bool unitOK = (unitFactors.find(unitName) != unitFactors.end()) &&
                 (unitShifts.find(unitName)  != unitShifts.end());
   if (!unitOK && isException)
   {
      std::string errmsg = "ERROR - unitName " + unitName;
      errmsg += " is not valid.\n";
      throw LowThrustException(errmsg);
   }
   return unitOK;
}

//------------------------------------------------------------------------------
// bool SetUnit(const std::string &unitName, Real factor)
//------------------------------------------------------------------------------
bool ScalingUtility::SetUnit(const std::string &unitName, Real factor)
{
   if (ValidateUnit(unitName))
   {
      unitFactors.at(unitName) = factor;
      #ifdef DEBUG_ADD_SCALING
         MessageInterface::ShowMessage("SetUnit: SET "
                                       "unitName = %s, "
                                       "factor = %12.10f\n",
                                       unitName.c_str(), factor);
      #endif
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
// bool SetShift(const std::string &unitName, Real shift)
//------------------------------------------------------------------------------
bool ScalingUtility::SetShift(const std::string &unitName, Real shift)
{
   if (ValidateUnit(unitName))
   {
      unitShifts.at(unitName) = shift;
      #ifdef DEBUG_ADD_SCALING
         MessageInterface::ShowMessage("SetShift: SET "
                                       "unitName = %s, "
                                       "shift = %12.10f\n",
                                       unitName.c_str(), shift);
      #endif
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
// bool SetUnitAndShift(const std::string &unitName, Real factor,
//                      Real shift)
//------------------------------------------------------------------------------
bool ScalingUtility::SetUnitAndShift(const std::string &unitName, Real factor,
                                     Real shift)
{
   if (ValidateUnit(unitName))
   {
      unitFactors.at(unitName) = factor;
      unitShifts.at(unitName)  = shift;
      #ifdef DEBUG_ADD_SCALING
         MessageInterface::ShowMessage("SetUnitAndShift: SET "
                                       "unitName = %s, "
                                       "factor = %12.10f, shift = %12.10f\n",
                                       unitName.c_str(), factor, shift);
      #endif
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
// bool AddUnitAndShift(const std::string &unitName, Real factor,
//                      Real shift)
//------------------------------------------------------------------------------
bool ScalingUtility::AddUnitAndShift(const std::string &unitName, Real factor,
                                    Real shift)
{
   if (ValidateUnit(unitName, false))
   {
      SetUnitAndShift(unitName, factor, shift);
      // @todo need a warning message here?
   }
   else
   {
      unitFactors.insert(std::make_pair(unitName, factor)); // @todo need validation here?
      unitShifts.insert(std::make_pair(unitName, shift));   // @todo need validation here?
      #ifdef DEBUG_ADD_SCALING
         MessageInterface::ShowMessage("AddUnitAndShift: INSERTED "
                                       "unitName = %s, "
                                       "factor = %12.10f, shift = %12.10f\n",
                                       unitName.c_str(), factor, shift);
      #endif
   }
   return true;
}

//------------------------------------------------------------------------------
// void GetUnitAndShift(const std::string &unitName, Real &factor,
//                      Real &shift)
//------------------------------------------------------------------------------
void ScalingUtility::GetUnitAndShift(const std::string &unitName, Real &factor,
                                    Real &shift)
{
   if (ValidateUnit(unitName))
   {
      factor = unitFactors.at(unitName);
      shift = unitShifts.at(unitName);
   }
}

//------------------------------------------------------------------------------
// Real ScaleParameter(const Real &unscaled, const std::string &unit)
//------------------------------------------------------------------------------
Real ScalingUtility::ScaleParameter(const Real &unscaled,
                                    const std::string &unit)
{
   Real scaled;
   if (ValidateUnit(unit))
      scaled = (unscaled - unitShifts.at(unit)) / unitFactors.at(unit);
   else // unit invalid - no scaling
      scaled = unscaled;
   return scaled;
}

//------------------------------------------------------------------------------
// Real UnscaleParameter(const Real &scaled, const std::string &unit)
//------------------------------------------------------------------------------
Real ScalingUtility::UnscaleParameter(const Real &scaled,
                                      const std::string &unit)
{
   Real unscaled;
   if (ValidateUnit(unit))
      unscaled = (scaled * unitFactors.at(unit)) + unitShifts.at(unit);
   else // unit invalid - cannot unscale
      unscaled = scaled;
   return unscaled;
}

//------------------------------------------------------------------------------
// Rvector ScaleVector(const Rvector &unscaled, const StringArray &units)
//------------------------------------------------------------------------------
Rvector ScalingUtility::ScaleVector(const Rvector &unscaled,
                                    const StringArray &units)
{
   Integer szVector = unscaled.GetSize();
   Integer szUnits  = units.size();
   Rvector scaled(szVector); // all zeroes by default
   if (szVector != szUnits)
   {
      scaled = unscaled;
      // @todo - should this just be a warning?
      throw LowThrustException(
                     "ERROR - unscaled vector and units sizes don't match!\n");
   }
   for (unsigned int ii = 0; ii < szVector; ii++)
   {
      std::string unit = units.at(ii);
      if (ValidateUnit(unit))
      {
         scaled(ii) = (unscaled(ii) - unitShifts.at(unit)) /
                      unitFactors.at(unit);
      }
      else // unit invalid - no scaling
      {
         scaled(ii) = unscaled(ii);
     }
   }
   return scaled;
}

//------------------------------------------------------------------------------
// Rvector UnscaleVector(const Rvector &scaled, const StringArray &units)
//------------------------------------------------------------------------------
Rvector ScalingUtility::UnscaleVector(const Rvector &scaled,
                                      const StringArray &units)
{
   Integer szVector = scaled.GetSize();
   Integer szUnits  = units.size();
   Rvector unscaled(szVector); // all zeroes by default
   if (szVector != szUnits)
   {
      unscaled = scaled;
      // @todo - should this just be a warning?
      throw LowThrustException(
                     "ERROR - scaled vector and units sizes don't match!\n");
   }
   for (unsigned int ii = 0; ii < szVector; ii++)
   {
      std::string unit = units.at(ii);
      if (ValidateUnit(unit))
      {
         unscaled(ii) = (scaled(ii) * unitFactors.at(unit)) +
                         unitShifts.at(unit);
      }
      else // unit invalid - no scaling
      {
         unscaled(ii) = scaled(ii);
      }
   }
   return unscaled;
}

//------------------------------------------------------------------------------
// Rmatrix ScaleJacobian(const Rmatrix &unscaled, const StringArray &funUnits,
//                       const StringArray &varUnits)
//------------------------------------------------------------------------------
Rmatrix ScalingUtility::ScaleJacobian(const Rmatrix &unscaled,
                                      const StringArray &funUnits,
                                      const StringArray &varUnits)
{
   Integer row, col;
   unscaled.GetSize(row, col);
   Integer szFunUnits  = funUnits.size();
   Integer szVarUnits  = varUnits.size();
   Rmatrix scaled(row, col);
   if (szFunUnits != row)
   {
      scaled = unscaled;
      // @todo - should this just be a warning?
      throw LowThrustException(
                  "ERROR - unscaled jacobian and units sizes don't match!\n");
   }
   if (szVarUnits != col)
   {
      scaled = unscaled;
      // @todo - should this just be a warning?
      throw LowThrustException(
                  "ERROR - unscaled jacobian and units sizes don't match!\n");
   }
   for (unsigned int ii = 0; ii < row; ii++)
   {
      std::string funUnit = funUnits.at(ii);
      for (unsigned int jj = 0; jj < col; jj++)
      {
         std::string varUnit = varUnits.at(jj);
         if (ValidateUnit(funUnit) && ValidateUnit(varUnit))
         {
            scaled(ii,jj) = (unscaled(ii,jj) * unitFactors.at(varUnit)) /
                             unitFactors.at(funUnit);
         }
         else
         {
            scaled(ii,jj) = unscaled(ii,jj);
         }
      }
   }
   return scaled;
}

//------------------------------------------------------------------------------
// Rmatrix ScaleJacobianByVars(const Rmatrix &unscaled,
//                             const StringArray &varUnits)
//------------------------------------------------------------------------------
Rmatrix ScalingUtility::ScaleJacobianByVars(const Rmatrix &unscaled,
                                            const StringArray &varUnits)
{
   Integer row, col;
   unscaled.GetSize(row, col);
   Integer szVarUnits = varUnits.size();
   Rmatrix scaled(row, col);
   if (szVarUnits != col)
   {
      scaled = unscaled;
      // @todo - should this just be a warning?
      throw LowThrustException(
         "ERROR - unscaled jacobian and units sizes don't match!\n");
   }
   for (unsigned int ii = 0; ii < row; ii++)
   {
      for (unsigned int jj = 0; jj < col; jj++)
      {
         std::string varUnit = varUnits.at(jj);
         if (ValidateUnit(varUnit))
         {
            scaled(ii, jj) = unscaled(ii, jj) * unitFactors.at(varUnit);
         }
         else
         {
            scaled(ii, jj) = unscaled(ii, jj);
         }
      }
   }
   return scaled;
}

//------------------------------------------------------------------------------
// Rmatrix ScaleJacobianByFun(const Rmatrix &unscaled,
//                            const StringArray &funUnits)
//------------------------------------------------------------------------------
Rmatrix ScalingUtility::ScaleJacobianByFun(const Rmatrix &unscaled,
                                           const StringArray &funUnits)
{
   Integer row, col;
   unscaled.GetSize(row, col);
   Integer szFunUnits = funUnits.size();
   Rmatrix scaled(row, col);
   if (szFunUnits != row)
   {
      scaled = unscaled;
      // @todo - should this just be a warning?
      throw LowThrustException(
         "ERROR - unscaled jacobian and units sizes don't match!\n");
   }
   for (unsigned int ii = 0; ii < row; ii++)
   {
      std::string funUnit = funUnits.at(ii);
      for (unsigned int jj = 0; jj < col; jj++)
      {
         if (ValidateUnit(funUnit))
         {
            scaled(ii, jj) = unscaled(ii, jj) / unitFactors.at(funUnit);
         }
         else
         {
            scaled(ii, jj) = unscaled(ii, jj);
         }
      }
   }
   return scaled;
}

//------------------------------------------------------------------------------
// Rmatrix UnscaleJacobian(const Rmatrix &scaled, const StringArray &funUnits,
//                         const StringArray &varUnits)
//------------------------------------------------------------------------------
Rmatrix ScalingUtility::UnscaleJacobian(const Rmatrix &scaled,
                                        const StringArray &funUnits,
                                        const StringArray &varUnits)
{
   Integer row, col;
   scaled.GetSize(row, col);
   Integer szFunUnits  = funUnits.size();
   Integer szVarUnits  = varUnits.size();
   Rmatrix unscaled(row, col);
   if (szFunUnits != row)
   {
      unscaled = scaled;
      // @todo - should this just be a warning?
      throw LowThrustException(
                     "ERROR - scaled jacobian and units sizes don't match!\n");
   }
   if (szVarUnits != col)
   {
      unscaled = scaled;
      // @todo - should this just be a warning?
      throw LowThrustException(
                     "ERROR - scaled jacobian and units sizes don't match!\n");
   }
   for (unsigned int ii = 0; ii < row; ii++)
   {
      std::string funUnit = funUnits.at(ii);
      for (unsigned int jj = 0; jj < col; jj++)
      {
         std::string varUnit = varUnits.at(jj);
         if (ValidateUnit(funUnit) && ValidateUnit(varUnit))
         {
            unscaled(ii,jj) = (scaled(ii,jj) * unitFactors.at(funUnit)) /
            unitFactors.at(varUnit);
         }
         else
         {
            unscaled(ii,jj) = scaled(ii,jj);
         }
      }
   }
   return unscaled;

}

//------------------------------------------------------------------------------
// Rmatrix UnscaleJacobianByVars(const Rmatrix &scaled,
//                               const StringArray &varUnits)
//------------------------------------------------------------------------------
Rmatrix ScalingUtility::UnscaleJacobianByVars(const Rmatrix &scaled,
                                              const StringArray &varUnits)
{
   Integer row, col;
   scaled.GetSize(row, col);
   Integer szVarUnits = varUnits.size();
   Rmatrix unscaled(row, col);
   if (szVarUnits != col)
   {
      unscaled = scaled;
      // @todo - should this just be a warning?
      throw LowThrustException(
         "ERROR - scaled jacobian and units sizes don't match!\n");
   }
   for (unsigned int ii = 0; ii < row; ii++)
   {
      for (unsigned int jj = 0; jj < col; jj++)
      {
         std::string varUnit = varUnits.at(jj);
         if (ValidateUnit(varUnit))
         {
            unscaled(ii, jj) = scaled(ii, jj) /  unitFactors.at(varUnit);
         }
         else
         {
            unscaled(ii, jj) = scaled(ii, jj);
         }
      }
   }
   return unscaled;
}

//------------------------------------------------------------------------------
// Rmatrix UnscaleJacobianByFun(const Rmatrix &scaled,
//                              const StringArray &funUnits)
//------------------------------------------------------------------------------
Rmatrix ScalingUtility::UnscaleJacobianByFun(const Rmatrix &scaled,
                                             const StringArray &funUnits)
{
   Integer row, col;
   scaled.GetSize(row, col);
   Integer szFunUnits = funUnits.size();
   Rmatrix unscaled(row, col);
   if (szFunUnits != row)
   {
      unscaled = scaled;
      // @todo - should this just be a warning?
      throw LowThrustException(
         "ERROR - scaled jacobian and units sizes don't match!\n");
   }
   for (unsigned int ii = 0; ii < row; ii++)
   {
      std::string funUnit = funUnits.at(ii);
      for (unsigned int jj = 0; jj < col; jj++)
      {
         if (ValidateUnit(funUnit))
         {
            unscaled(ii, jj) = scaled(ii, jj) * unitFactors.at(funUnit);
         }
         else
         {
            unscaled(ii, jj) = scaled(ii, jj);
         }
      }
   }
   return unscaled;
}
