//------------------------------------------------------------------------------
//                           OptimalControlFunction
//------------------------------------------------------------------------------
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
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
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: 2018.09.10
//
/**
* Defines the OptimalControlFunction class.
*
* From original MATLAB prototype:
* Author: S. Hughes.  steven.p.hughes@nasa.gov
*/
//------------------------------------------------------------------------------
#include "OptimalControlFunction.hpp"
#include "StringUtil.hpp"
#include "NumericJacobian.hpp"

//------------------------------------------------------------------------------
// OptimalControlFunction()
//------------------------------------------------------------------------------
/*
* Constructor
*/
//------------------------------------------------------------------------------
OptimalControlFunction::OptimalControlFunction(std::string functionName) :
   funcName        (functionName),
   numPoints       (0),
   numFunctions    (0),
   numPhases       (0),
   refEpoch        (-1.0),
   //TU              (1.0),
   //DU              (1.0),
   //VU              (1.0),
   //MU              (1.0),
   funcScaleUtil   (NULL),
   dataInitialized (false),
   boundsScaled    (false)
{
   
}

//------------------------------------------------------------------------------
// OptimalControlFunction(const OptimalControlFunction &copy)
//------------------------------------------------------------------------------
/*
* Copy constructor
*/
//------------------------------------------------------------------------------
OptimalControlFunction::OptimalControlFunction(const OptimalControlFunction &copy) : 
   funcName              (copy.funcName),
   numPoints             (copy.numPoints),
   numFunctions          (copy.numFunctions),
   numPhases             (copy.numPhases),
   refEpoch              (copy.refEpoch),
   //TU                    (copy.TU),
   //DU                    (copy.DU),
   //VU                    (copy.VU),
   //MU                    (copy.MU),
   funcScaleUtil         (NULL), //copy.funcScaleUtil),  GMT-7025
   dataInitialized       (copy.dataInitialized),
   boundsScaled          (copy.boundsScaled)
{
   phaseDepMap.clear();
   for (Integer i = 0; i < copy.phaseDepMap.size(); ++i)
      phaseDepMap.push_back(copy.phaseDepMap.at(i));
   pointDepMap.clear();
   for (Integer i = 0; i < copy.pointDepMap.size(); ++i)
      pointDepMap.push_back(copy.pointDepMap.at(i));
   stateDepMap.clear();
   for (Integer i = 0; i < copy.stateDepMap.size(); ++i)
      stateDepMap.push_back(copy.stateDepMap.at(i));
   controlDepMap.clear();
   for (Integer i = 0; i < copy.controlDepMap.size(); ++i)
      controlDepMap.push_back(copy.controlDepMap.at(i));
   timeDepMap.clear();
   for (Integer i = 0; i < copy.timeDepMap.size(); ++i)
      timeDepMap.push_back(copy.timeDepMap.at(i));
   paramDepMap.clear();
   for (Integer i = 0; i < copy.paramDepMap.size(); ++i)
      paramDepMap.push_back(copy.paramDepMap.at(i));
   analyticStateJacMap.clear();
   for (Integer i = 0; i < copy.analyticStateJacMap.size(); ++i)
      analyticStateJacMap.push_back(copy.analyticStateJacMap.at(i));
   analyticControlJacMap.clear();
   for (Integer i = 0; i < copy.analyticControlJacMap.size(); ++i)
      analyticControlJacMap.push_back(copy.analyticControlJacMap.at(i));
   analyticTimeJacMap.clear();
   for (Integer i = 0; i < copy.analyticTimeJacMap.size(); ++i)
      analyticTimeJacMap.push_back(copy.analyticTimeJacMap.at(i));
   analyticParamJacMap.clear();
   for (Integer i = 0; i < copy.analyticParamJacMap.size(); ++i)
      analyticParamJacMap.push_back(copy.analyticParamJacMap.at(i));
   constraintNames.clear();
   for (Integer i = 0; i < copy.constraintNames.size(); ++i)
      constraintNames.push_back(copy.constraintNames.at(i));
   funcUnitList.clear();
   for (unsigned int ii = 0; ii < copy.funcUnitList.size(); ii++)
      funcUnitList.push_back(copy.funcUnitList.at(ii));

   lowerBounds.SetSize(copy.lowerBounds.GetSize());
   lowerBounds = copy.lowerBounds;
   upperBounds.SetSize(copy.upperBounds.GetSize());
   upperBounds = copy.upperBounds;
   unscaledLowerBounds.SetSize(copy.unscaledLowerBounds.GetSize());
   unscaledLowerBounds = copy.unscaledLowerBounds;
   unscaledUpperBounds.SetSize(copy.unscaledUpperBounds.GetSize());
   unscaledUpperBounds = copy.unscaledUpperBounds;

   //for (Integer i = 0; i < phaseList.size(); ++i)
      //delete phaseList.at(i);
   phaseList.clear();

   for (Integer i = 0; i < copy.phaseList.size(); ++i)
      phaseList.push_back(copy.phaseList.at(i));
}

//------------------------------------------------------------------------------
// OptimalControlFunction& operator=(const OptimalControlFunction &copy)
//------------------------------------------------------------------------------
/*
* Assignment operator
*/
//------------------------------------------------------------------------------
OptimalControlFunction& OptimalControlFunction::operator=(const OptimalControlFunction &copy)
{
   if (&copy == this)
      return *this;

   funcName              = copy.funcName;
   numPoints             = copy.numPoints;
   numFunctions          = copy.numFunctions;
   numPhases             = copy.numPhases;
   refEpoch              = copy.refEpoch;
   //TU                    = copy.TU;
   //DU                    = copy.DU;
   //VU                    = copy.VU;
   //MU                    = copy.MU;
   funcScaleUtil         = copy.funcScaleUtil;  // ????
   dataInitialized       = copy.dataInitialized;
   boundsScaled          = copy.boundsScaled;

   phaseDepMap.clear();
   for (Integer i = 0; i < copy.phaseDepMap.size(); ++i)
      phaseDepMap.push_back(copy.phaseDepMap.at(i));
   pointDepMap.clear();
   for (Integer i = 0; i < copy.pointDepMap.size(); ++i)
      pointDepMap.push_back(copy.pointDepMap.at(i));
   stateDepMap.clear();
   for (Integer i = 0; i < copy.stateDepMap.size(); ++i)
      stateDepMap.push_back(copy.stateDepMap.at(i));
   controlDepMap.clear();
   for (Integer i = 0; i < copy.controlDepMap.size(); ++i)
      controlDepMap.push_back(copy.controlDepMap.at(i));
   timeDepMap.clear();
   for (Integer i = 0; i < copy.timeDepMap.size(); ++i)
      timeDepMap.push_back(copy.timeDepMap.at(i));
   paramDepMap.clear();
   for (Integer i = 0; i < copy.paramDepMap.size(); ++i)
      paramDepMap.push_back(copy.paramDepMap.at(i));
   analyticStateJacMap.clear();
   for (Integer i = 0; i < copy.analyticStateJacMap.size(); ++i)
      analyticStateJacMap.push_back(copy.analyticStateJacMap.at(i));
   analyticControlJacMap.clear();
   for (Integer i = 0; i < copy.analyticControlJacMap.size(); ++i)
      analyticControlJacMap.push_back(copy.analyticControlJacMap.at(i));
   analyticTimeJacMap.clear();
   for (Integer i = 0; i < copy.analyticTimeJacMap.size(); ++i)
      analyticTimeJacMap.push_back(copy.analyticTimeJacMap.at(i));
   analyticParamJacMap.clear();
   for (Integer i = 0; i < copy.analyticParamJacMap.size(); ++i)
      analyticParamJacMap.push_back(copy.analyticParamJacMap.at(i));
   constraintNames.clear();
   for (Integer i = 0; i < copy.constraintNames.size(); ++i)
      constraintNames.push_back(copy.constraintNames.at(i));
   funcUnitList.clear();
   for (unsigned int ii = 0; ii < copy.funcUnitList.size(); ii++)
      funcUnitList.push_back(copy.funcUnitList.at(ii));

   lowerBounds.SetSize(copy.lowerBounds.GetSize());
   lowerBounds = copy.lowerBounds;
   upperBounds.SetSize(copy.upperBounds.GetSize());
   upperBounds = copy.upperBounds;
   unscaledLowerBounds.SetSize(copy.unscaledLowerBounds.GetSize());
   unscaledLowerBounds = copy.unscaledLowerBounds;
   unscaledUpperBounds.SetSize(copy.unscaledUpperBounds.GetSize());
   unscaledUpperBounds = copy.unscaledUpperBounds;

   //for (Integer i = 0; i < phaseList.size(); ++i)
      //delete phaseList.at(i);
   phaseList.clear();

   for (Integer i = 0; i < copy.phaseList.size(); ++i)
      phaseList.push_back(copy.phaseList.at(i));

   return *this;
}

//------------------------------------------------------------------------------
// ~OptimalControlFunction()
//------------------------------------------------------------------------------
/*
* Destructor
*/
//------------------------------------------------------------------------------
OptimalControlFunction::~OptimalControlFunction()
{
   if (funcScaleUtil)
      delete funcScaleUtil;
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/*
* Method to initialize various data storage parameters based on the number of
* points in the optimal control function.  This method should be called before
* function evaluations begin.
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::Initialize()
{
   stateData.resize(numPoints);
   controlData.resize(numPoints);
   timeData.resize(numPoints);
   paramData.resize(numPoints);
   numjacStateWorkingStorage.resize(numPoints);
   numjacControlWorkingStorage.resize(numPoints);
   numjacParamWorkingStorage.resize(numPoints);
   stateJacElements.resize(numPoints);
   controlJacElements.resize(numPoints);
   timeJacElements.resize(numPoints);
   paramJacElements.resize(numPoints);

   for (Integer i = 0; i < numPoints; ++i)
   {
      stateData.at(i).SetSize(0);
      controlData.at(i).SetSize(0);
      timeData.at(i).SetSize(0);
      paramData.at(i).SetSize(0);
      numjacStateWorkingStorage.at(i).SetSize(0);
      numjacControlWorkingStorage.at(i).SetSize(0);
      numjacParamWorkingStorage.at(i).SetSize(0);
   }

   // If the analytic Jacobian maps were not set by the optimal control
   // function, set them to false here
   if (analyticStateJacMap.empty())
   {
      analyticStateJacMap.resize(numPoints);
      for (Integer i = 0; i < numPoints; ++i)
         analyticStateJacMap.at(i) = false;
   }
   if (analyticControlJacMap.empty())
   {
      analyticControlJacMap.resize(numPoints);
      for (Integer i = 0; i < numPoints; ++i)
         analyticControlJacMap.at(i) = false;
   }
   if (analyticTimeJacMap.empty())
   {
      analyticTimeJacMap.resize(numPoints);
      for (Integer i = 0; i < numPoints; ++i)
         analyticTimeJacMap.at(i) = false;
   }
   if (analyticParamJacMap.empty())
   {
      analyticParamJacMap.resize(numPoints);
      for (Integer i = 0; i < numPoints; ++i)
         analyticParamJacMap.at(i) = false;
   }

   // Fill in constraint names that weren't entered as "Unknown Constraint"
   for (Integer i = constraintNames.size(); i < numFunctions; ++i)
      constraintNames.push_back("Unknown Constraint");
   for (Integer i = 0; i < numFunctions; ++i)
      if (constraintNames.at(i) == "")
         constraintNames.at(i) = "Unknown Constraint";

   dataInitialized = true;
   boundsScaled = false;
}

//------------------------------------------------------------------------------
// bool HasDependency(std::string varType, Integer pointIdx)
//------------------------------------------------------------------------------
/*
* Method to determine whether a discretization point has a dependency on state,
* control, time or parameter data
*
* @param varType The variable type dependency to check for
* @param pointIdx The discretization point
*
* @return True if there is a dependency, false otherwise
*/
//------------------------------------------------------------------------------
bool OptimalControlFunction::HasDependency(VariableType varType, Integer pointIdx)
{
   ValidatePointIdx(pointIdx);

   switch (varType)
   {
   case STATE:
      return stateDepMap.at(pointIdx);
   case CONTROL:
      return controlDepMap.at(pointIdx);
   case TIME:
      return timeDepMap.at(pointIdx);
   case STATIC:
      return paramDepMap.at(pointIdx);
   default:
      throw LowThrustException("Unknown variable type provided to "
         "algebraic function " + funcName + " when checking dependency.");
   }
}

//------------------------------------------------------------------------------
// bool HasAnalyticJacobian(std::string varType, Integer pointIdx)
//------------------------------------------------------------------------------
/*
* Method to determine whether a discretization point has an analytic Jacobian
* on state, control, time or parameter data
*
* @param varType The variable type dependency to check for
* @param pointIdx The discretization point
*
* @return True if there is an analytic Jacobian, false otherwise
*/
//------------------------------------------------------------------------------
bool OptimalControlFunction::HasAnalyticJacobian(VariableType varType,
                                            Integer pointIdx)
{
   ValidatePointIdx(pointIdx);

   switch (varType)
   {
   case STATE:
      return analyticStateJacMap.at(pointIdx);
   case CONTROL:
      return analyticControlJacMap.at(pointIdx);
   case TIME:
      return analyticTimeJacMap.at(pointIdx);
   case STATIC:
      return analyticParamJacMap.at(pointIdx);
   default:
      throw LowThrustException("Unknown variable type provided to "
         "algebraic function " + funcName + " when checking for analytic "
         "Jacobian.");
   }
}

void OptimalControlFunction::SetJacobian(VariableType varType,
                                    Integer pointIdx)
{
   ValidatePointIdx(pointIdx);

   switch (varType)
   {
   case STATE:
      analyticStateJacMap.at(pointIdx) = true;
      break;
   case CONTROL:
      analyticControlJacMap.at(pointIdx) = true;
      break;
   case TIME:
      analyticTimeJacMap.at(pointIdx) = true;
      break;
   case STATIC:
      analyticParamJacMap.at(pointIdx) = true;
      break;
   default:
      throw LowThrustException("Unknown variable type provided to "
         "algebraic function " + funcName + " when setting Jacobian.");
   }
}

//------------------------------------------------------------------------------
// void SetData(std::string varType, Integer pointIdx, Rvector theData)
//------------------------------------------------------------------------------
/*
* Method to set the daa on a specified discretization point
*
* @param varType The variable type to apply the data to
* @param pointIdx The discretization point where the data is applied
* @param theData theData The new input data
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetData(VariableType varType, Integer pointIdx,
                                Rvector theData)
{
   ValidatePointIdx(pointIdx);
   Integer dataSize = theData.GetSize();

   switch (varType)
   {
   case STATE:
      stateData.at(pointIdx).SetSize(dataSize);
      stateData.at(pointIdx) = theData;
      break;
   case CONTROL:
      controlData.at(pointIdx).SetSize(dataSize);
      controlData.at(pointIdx) = theData;
      break;
   case TIME:
      timeData.at(pointIdx).SetSize(dataSize);
      timeData.at(pointIdx) = theData;
      break;
   case STATIC:
      paramData.at(pointIdx).SetSize(dataSize);
      paramData.at(pointIdx) = theData;
      break;
   default:
      throw LowThrustException("Unknown variable type provided to "
         "algebraic function " + funcName + " when setting data.");
   }
}

//------------------------------------------------------------------------------
// void SetReferenceEpoch(Real referenceEpoch)
//------------------------------------------------------------------------------
/*
* Method to set reference epoch that can be used in scaling
*
* @param referenceEpoch The reference epoch
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetReferenceEpoch(Real referenceEpoch)
{
   refEpoch = referenceEpoch;
}

//------------------------------------------------------------------------------
// Rvector GetUnscaledFunctionValues()
//------------------------------------------------------------------------------
/*
* Method to return the unscaled function values at the current data settings
*
* @return The unscaled function values
*/
//------------------------------------------------------------------------------
Rvector OptimalControlFunction::GetUnscaledFunctionValues()
{
   Rvector unscaledFuncs = EvaluateFunctions();
   unscaledFuncs = funcScaleUtil->UnscaleVector(unscaledFuncs, funcUnitList);
   return unscaledFuncs;
}

//------------------------------------------------------------------------------
// Rmatrix EvaluateJacobian()
//------------------------------------------------------------------------------
/*
* Method to return the Jacobian array if a dependency for the input variable
* type exists, otherwise an empty array is returned
*
* @return The Jacobian
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::EvaluateJacobian()
{
   std::vector<Rvector> nomStateData = stateData;
   std::vector<Rvector> nomControlData = controlData;
   std::vector<Rvector> nomTimeData = timeData;
   std::vector<Rvector> nomParamData = paramData;

   std::vector<VariableType> varTypeList;
   varTypeList.push_back(STATE);
   varTypeList.push_back(CONTROL);
   varTypeList.push_back(TIME);
   varTypeList.push_back(STATIC);
   for (Integer pointIdx = 0; pointIdx < numPoints; ++pointIdx)
   {
      for (Integer varIdx = 0; varIdx < varTypeList.size(); ++varIdx)
      {
         if (HasDependency(varTypeList.at(varIdx), pointIdx) &&
            numFunctions != 0)
         {
            if (HasAnalyticJacobian(varTypeList.at(varIdx), pointIdx))
            {
               bool hasAnalyticJacCheck = false;
               switch (varTypeList.at(varIdx))
               {
               case STATE:
                  EvaluateAnalyticJacobian(varTypeList.at(varIdx), pointIdx,
                     hasAnalyticJacCheck, stateJacElements.at(pointIdx));
                  break;
               case CONTROL:
                  EvaluateAnalyticJacobian(varTypeList.at(varIdx), pointIdx,
                     hasAnalyticJacCheck, controlJacElements.at(pointIdx));
                  break;
               case TIME:
                  EvaluateAnalyticJacobian(varTypeList.at(varIdx), pointIdx,
                     hasAnalyticJacCheck, timeJacElements.at(pointIdx));
                  break;
               case STATIC:
                  EvaluateAnalyticJacobian(varTypeList.at(varIdx), pointIdx,
                     hasAnalyticJacCheck, paramJacElements.at(pointIdx));
                  break;
               }
            }
            else
            {
               if (varTypeList.at(varIdx) != TIME)
               {
                  // Use numjac to calculate the Jacobian through finite
                  // differencing
                  Rvector nominalData, workingStorage;
                  switch (varTypeList.at(varIdx))
                  {
                  case STATE:
                     nominalData = stateData.at(pointIdx);
                     workingStorage = numjacStateWorkingStorage.at(pointIdx);
                     break;
                  case CONTROL:
                     nominalData = controlData.at(pointIdx);
                     workingStorage = numjacControlWorkingStorage.at(pointIdx);
                     break;
                  case STATIC:
                     nominalData = paramData.at(pointIdx);
                     workingStorage = numjacParamWorkingStorage.at(pointIdx);
                     break;
                  }
                  if (nominalData.GetSize() == 0)
                     continue;
                  Rmatrix yThresh(1, nominalData.GetSize());
                  for (Integer i = 0; i < yThresh.GetNumColumns(); ++i)
                     yThresh(0, i) = 1e-14;
                  Rvector nomFuncVals = EvaluateFunctions();
                  NumericJacobian numjac;
                  NumericJacobian::JacState currJacState = numjac.GetState();
                  numjac.SetInitialValues(nominalData, nomFuncVals, yThresh,
                     workingStorage);

                  // Step through the numjac state machine until the Jacobian is
                  // completed
                  while (currJacState != NumericJacobian::FINISHED)
                  {
                     if (currJacState == NumericJacobian::PERTURBING ||
                        currJacState == NumericJacobian::REFINING)
                     {
                        // Have numjac perturb each of the variables one by one
                        // and send the Jacobian calculator the resulting
                        // derivatives
                        Rvector pertValues;
                        switch (varTypeList.at(varIdx))
                        {
                        case STATE:
                           stateData.at(pointIdx) = numjac.GetCurrentVars();
                           pertValues = EvaluateFunctions();
                           numjac.SetDerivs(pertValues);
                           stateData.at(pointIdx) = nomStateData.at(pointIdx);
                           break;
                        case CONTROL:
                           controlData.at(pointIdx) = numjac.GetCurrentVars();
                           pertValues = EvaluateFunctions();
                           numjac.SetDerivs(pertValues);
                           controlData.at(pointIdx) = nomControlData.at(pointIdx);
                           break;
                        case STATIC:
                           paramData.at(pointIdx) = numjac.GetCurrentVars();
                           pertValues = EvaluateFunctions();
                           numjac.SetDerivs(pertValues);
                           paramData.at(pointIdx) = nomParamData.at(pointIdx);
                           break;
                        }
                     }
                     currJacState = numjac.AdvanceState();
                  }
                  Rmatrix jacValue = numjac.GetJacobian();

                  // Store the Jacobian elements and the new working storage
                  // values
                  switch (varTypeList.at(varIdx))
                  {
                  case STATE:
                     stateJacElements.at(pointIdx) = jacValue;
                     numjacStateWorkingStorage.at(pointIdx).SetSize(
                        numjac.GetWorkingStorage().GetSize());
                     numjacStateWorkingStorage.at(pointIdx) =
                        numjac.GetWorkingStorage();
                     break;
                  case CONTROL:
                     controlJacElements.at(pointIdx) = jacValue;
                     numjacControlWorkingStorage.at(pointIdx).SetSize(
                        numjac.GetWorkingStorage().GetSize());
                     numjacControlWorkingStorage.at(pointIdx) =
                        numjac.GetWorkingStorage();
                     break;
                  case STATIC:
                     paramJacElements.at(pointIdx) = jacValue;
                     numjacParamWorkingStorage.at(pointIdx).SetSize(
                        numjac.GetWorkingStorage().GetSize());
                     numjacParamWorkingStorage.at(pointIdx) =
                        numjac.GetWorkingStorage();
                     break;
                  }
               }
               else
               {
                  // Calculate the time Jacobian using central differencing
                  Real pertSize = 0.5e-4;
                  timeData.at(pointIdx)(0) += pertSize;
                  Rvector posStepFunc = EvaluateFunctions();
                  timeData.at(pointIdx)(0) -= 2.0 * pertSize;
                  Rvector negStepFunc = EvaluateFunctions();
                  timeData.at(pointIdx)(0) += pertSize;
                  Rmatrix jacValue(numFunctions, 1);
                  for (Integer i = 0; i < numFunctions; ++i)
                  {
                     jacValue(i, 0) = (posStepFunc(i) - negStepFunc(i)) /
                        (2 * pertSize);
                  }
                  timeJacElements.at(pointIdx) = jacValue;
               }
            }
         }
         else
         {
            switch (varTypeList.at(varIdx))
            {
            case STATE:
               stateJacElements.at(pointIdx).SetSize(numFunctions,
                  stateData.at(pointIdx).GetSize());
               break;
            case CONTROL:
               controlJacElements.at(pointIdx).SetSize(numFunctions,
                  controlData.at(pointIdx).GetSize());
               break;
            case TIME:
               timeJacElements.at(pointIdx).SetSize(numFunctions,
                  timeData.at(pointIdx).GetSize());
               break;
            case STATIC:
               paramJacElements.at(pointIdx).SetSize(numFunctions,
                  paramData.at(pointIdx).GetSize());
               break;
            }
         }
      }
   }
}

//------------------------------------------------------------------------------
// void EvaluateAnalyticJacobian(VariableType varType, Integer pointIdx)
//------------------------------------------------------------------------------
/*
* Method to evaluate a Jacobian using a provided analytic set of equations
*
* @param varType The type of Jacobian to evaluate
* @param pointIdx Which function point the Jacobian is evaluating at
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::EvaluateAnalyticJacobian(VariableType varType,
   Integer pointIdx, bool &hasAnalyticJac, Rmatrix &jacArray)
{
   std::string varString;
   hasAnalyticJac = false;

   switch (varType)
   {
   case STATE:
      varString = "State";
      break;
   case CONTROL:
      varString = "Control";
      break;
   case TIME:
      varString = "Time";
      break;
   case STATIC:
      varString = "Static";
      break;
   default:
      throw LowThrustException("Unknown variable type provided to "
         "algebraic function " + funcName + ".");
   }

   throw LowThrustException("The " + varString + " Jacobian does not have an "
      "analytic solution at point " + GmatStringUtil::ToString(pointIdx));
}

//------------------------------------------------------------------------------
// Rmatrix GetJacobian(VariableType varType, Integer pointIdx)
//------------------------------------------------------------------------------
/*
* Method to return the specified type of Jacobian
*
* @param varType The type of Jacobian to return
* @param pointIdx Which function point the Jacobian was evaluated at
*
* @return The Jacobian
*/
//------------------------------------------------------------------------------
Rmatrix OptimalControlFunction::GetJacobian(VariableType varType, Integer pointIdx)
{
   ValidatePointIdx(pointIdx);

   switch (varType)
   {
   case STATE:
      return stateJacElements.at(pointIdx);
   case CONTROL:
      return controlJacElements.at(pointIdx);
   case TIME:
      return timeJacElements.at(pointIdx);
   case STATIC:
      return paramJacElements.at(pointIdx);
   default:
      throw LowThrustException("Unknown variable type provided to "
         "algebraic function " + funcName + ".");
   }
}

//------------------------------------------------------------------------------
// Rvector GetLowerBounds()
//------------------------------------------------------------------------------
/*
* Method to return the scaled lower bounds on the algebraic function
*
* @return lowerBounds The function lower bounds
*/
//------------------------------------------------------------------------------
Rvector OptimalControlFunction::GetLowerBounds()
{
   return lowerBounds;
}

//------------------------------------------------------------------------------
// Rvector GetUpperBounds()
//------------------------------------------------------------------------------
/*
* Method to return the scaled upper bounds on the algebraic function
*
* @return upperBounds The function upper bounds
*/
//------------------------------------------------------------------------------
Rvector OptimalControlFunction::GetUpperBounds()
{
   return upperBounds;
}

//------------------------------------------------------------------------------
// Rvector GetUnscaledLowerBounds()
//------------------------------------------------------------------------------
/*
* Method to return the unscaled lower bounds on the algebraic function
*
* @return unscaledLowerBounds The function lower bounds
*/
//------------------------------------------------------------------------------
Rvector OptimalControlFunction::GetUnscaledLowerBounds()
{
   return funcScaleUtil->UnscaleVector(lowerBounds, funcUnitList);
}

//------------------------------------------------------------------------------
// Rvector GetUnscaledUpperBounds()
//------------------------------------------------------------------------------
/*
* Method to return the unscaled upper bounds on the algebraic function
*
* @return unscaledUpperBounds The function upper bounds
*/
//------------------------------------------------------------------------------
Rvector OptimalControlFunction::GetUnscaledUpperBounds()
{
   return funcScaleUtil->UnscaleVector(upperBounds, funcUnitList);
}

//------------------------------------------------------------------------------
// StringArray StringArray OptimalControlFunction::GetConstraintNames()
//------------------------------------------------------------------------------
/*
* Method to return the names of the function's constraints
*
* @return constraintNames The constraint names
*/
//------------------------------------------------------------------------------
StringArray OptimalControlFunction::GetConstraintNames()
{
   return constraintNames;
}

//------------------------------------------------------------------------------
// void SetLowerBounds(Rvector functionLB)
//------------------------------------------------------------------------------
/*
* Method to set the lower bounds on the algebraic function when a derived class
* implements this method, otherwise an error is returned
*
* @param functionLB The function lower bounds
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetLowerBounds(Rvector functionLB)
{
   throw LowThrustException("The bounds on this algebraic function cannot "
      "be changed");
}

//------------------------------------------------------------------------------
// void SetUpperBounds(Rvector functionUB)
//------------------------------------------------------------------------------
/*
* Method to set the upper bounds on the algebraic function when a derived class
* implements this method, otherwise an error is returned
*
* @param functionUB The function upper bounds
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetUpperBounds(Rvector functionUB)
{
   throw LowThrustException("The bounds on this algebraic function cannot "
      "be changed");
}

void OptimalControlFunction::ScaleData()
{
   // If there is no overridden method, do nothing
}

//------------------------------------------------------------------------------
// Integer GetNumPoints()
//------------------------------------------------------------------------------
/*
* Method to get the number of discretization points
*
* @return numPoints The number of discretization points
*/
//------------------------------------------------------------------------------
Integer OptimalControlFunction::GetNumPoints()
{
   return numPoints;
}

//------------------------------------------------------------------------------
// Integer GetNumFunctions()
//------------------------------------------------------------------------------
/*
* Method to get the number of functions
*
* @return numFunctions The total number of functions
*/
//------------------------------------------------------------------------------
Integer OptimalControlFunction::GetNumFunctions()
{
   return numFunctions;
}

//------------------------------------------------------------------------------
// Integer SetNumFunctions()
//------------------------------------------------------------------------------
/*
* Method to set the number of functions
*
* @param numFuncs The total number of functions
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetNumFunctions(Integer numFuncs)
{
   numFunctions = numFuncs;
   lowerBounds.SetSize(numFuncs);
   upperBounds.SetSize(numFuncs);
}

//------------------------------------------------------------------------------
// void SetPointData(Integer newPhaseIdx, std::string position,
//                   Integer pointIdx)
//------------------------------------------------------------------------------
/*
* Method to set the phase index and position in that phase for each point in
* the algebraic function
*
* @param newPhaseIdx The new phase index that the data at the point index is
*        from
* @param position The position on the specified newPhaseIdx that the data at
*        the point index is from
* @param pointIdx The point in the algebraic function receiving a new data
*        location.  If left as default -1, the first point that is
*        uninitialized will be filled in.
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetPointData(Integer newPhaseIdx, std::string position,
                                     Integer pointIdx)
{
   if (pointIdx == -1)
   {
      for (Integer i = 0; i < numPoints; ++i)
      {
         if (phaseDepMap.at(i) == -1)
         {
            phaseDepMap.at(i) = newPhaseIdx;
            if (position == "Initial")
               pointDepMap.at(i) = 0;
            else if (position == "Final")
               pointDepMap.at(i) = 1;
         }
      }
   }
   else
   {
      ValidatePointIdx(pointIdx);
      phaseDepMap.at(pointIdx) = newPhaseIdx;
      if (position == "Initial")
         pointDepMap.at(pointIdx) = 0;
      else if (position == "Final")
         pointDepMap.at(pointIdx) = 1;
   }
}

//------------------------------------------------------------------------------
// void GetPointData(Integer newPhaseIdx, std::string position,
//                   Integer pointIdx)
//------------------------------------------------------------------------------
/*
* Method to get the phase index and position in that phase for each point in
* the algebraic function
*
* @param currPhaseIdx The current phase index that the data at the point index
*        is from
* @param position The position on the currPhaseIdx that the data at the point
*        index is from
* @param pointIdx The point in the algebraic function to get phase and position
*        data for
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::GetPointData(Integer pointIdx, Integer &currPhaseIdx,
                                     std::string &position)
{
   ValidatePointIdx(pointIdx);
   currPhaseIdx = phaseDepMap.at(pointIdx);
   if (pointDepMap.at(pointIdx) == 0)
      position = "Initial";
   else if (pointDepMap.at(pointIdx) == 1)
      position = "Final";
   else
      position = "AllPoints";
}

//------------------------------------------------------------------------------
// void SetNumPhases(Integer theNumPhases)
//------------------------------------------------------------------------------
/*
* Method to set the number of phases referenced by this function
*
* @param theNumPhases The total number of phases
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetNumPhases(Integer theNumPhases)
{
   numPhases = theNumPhases;
}

//------------------------------------------------------------------------------
// void SetPhaseList(std::vector<Phase*> phaseVec)
//------------------------------------------------------------------------------
/*
* Method to set the list of phase objects.  The order of the phases must match
* the order of the phases in the optimization problem.
*
* @param pList The vector of phase objects
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetPhaseList(std::vector<Phase*> pList)
{
   // Clear the old vector if it has an old phase list
   if (!phaseList.empty())
      phaseList.clear();

   for (Integer i = 0; i < pList.size(); ++i)
   {
      if (pList.at(i) == NULL)
      {
         std::string errmsg = "The pointer for phase " +
            GmatStringUtil::ToString(i) + " is null and cannot be added to"
            " the optimal control function's list of phases.";
         throw LowThrustException(errmsg);
      }
      phaseList.push_back(pList.at(i));
   }
}

//------------------------------------------------------------------------------
// void SetPhaseDependencies(IntegerArray phaseDeps)
//------------------------------------------------------------------------------
/*
* Method to set phase dependencies for each point in the function
*
* @param phaseDeps An integer array of which phase each point depends on
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetPhaseDependencies(IntegerArray phaseDeps)
{
   if (phaseDeps.size() != numPoints)
   {
      Integer numPhaseDeps = phaseDeps.size();
      std::string numPhaseDepsStr =
         GmatStringUtil::RemoveMultipleSpaces(
            GmatStringUtil::ToString(numPhaseDeps));
      std::string numPointsStr =
         GmatStringUtil::RemoveMultipleSpaces(
            GmatStringUtil::ToString(numPoints));
      std::string errmsg = "The number of phase dependencies assigned does"
         " not match the number of function points in object \"" +
         funcName + "\".  There were " + numPhaseDepsStr + " phase "
         "dependencie(s) entered for " + numPointsStr + " point(s).";
      throw LowThrustException(errmsg);
   }
   phaseDepMap = phaseDeps;
}

//------------------------------------------------------------------------------
// void SetPointDependencies(IntegerArray pointDeps)
//------------------------------------------------------------------------------
/*
* Method to set where along the phase each point in the function depends on
*
* @param pointDeps An integer array of where along the phase each point depends
*        on. 0 represnts the initial point, 1 represents the final points, and
*        2 represents all points in the phase
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetPointDependencies(IntegerArray pointDeps)
{
   if (pointDeps.size() != numPoints)
   {
      std::string errmsg = "The number of point dependencies assigned does"
         " not match the number of points in OptimalControlFunction \"" +
         funcName + "\"";
      throw LowThrustException(errmsg);
   }

   for (Integer i = 0; i < pointDeps.size(); ++i)
   {
      if (!IsValidPhasePosition(pointDeps.at(i)))
      {
         std::string incorrectPos;
         if (pointDeps.at(i) == 0)
            incorrectPos = "initial point";
         else if (pointDeps.at(i) == 1)
            incorrectPos = "final point";
         else
            incorrectPos = "all points";

         std::string errmsg = "The phase position \"" + incorrectPos +
            "\" is not a valid position to use in OptimalControlFunction \"" +
            funcName + "\"";
         throw LowThrustException(errmsg);
      }
   }

   pointDepMap = pointDeps;
}

//------------------------------------------------------------------------------
// void SetStateDepMap(BooleanArray stateDeps)
//------------------------------------------------------------------------------
/*
* Method to set which function points depend on state
*
* @param stateDeps Boolean array of which points depend on state
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetStateDepMap(BooleanArray stateDeps)
{
   if (stateDeps.size() != numPoints)
   {
      std::string errmsg = "The number of state dependencies assigned does"
         " not match the number of points in OptimalControlFunction \"" +
         funcName + "\"";
      throw LowThrustException(errmsg);
   }
   stateDepMap = stateDeps;
}

//------------------------------------------------------------------------------
// void SetControlDepMap(BooleanArray controlDeps)
//------------------------------------------------------------------------------
/*
* Method to set which function points depend on control
*
* @param controlDeps Boolean array of which points depend on control
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetControlDepMap(BooleanArray controlDeps)
{
   if (controlDeps.size() != numPoints)
   {
      std::string errmsg = "The number of control dependencies assigned does"
         " not match the number of points in OptimalControlFunction \"" +
         funcName + "\"";
      throw LowThrustException(errmsg);
   }
   controlDepMap = controlDeps;
}

//------------------------------------------------------------------------------
// void SetTimeDepMap(BooleanArray timeDeps)
//------------------------------------------------------------------------------
/*
* Method to set which function points depend on time
*
* @param timeDeps Boolean array of which points depend on time
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetTimeDepMap(BooleanArray timeDeps)
{
   if (timeDeps.size() != numPoints)
   {
      std::string errmsg = "The number of time dependencies assigned does"
         " not match the number of points in OptimalControlFunction \"" +
         funcName + "\"";
      throw LowThrustException(errmsg);
   }
   timeDepMap = timeDeps;
}

//------------------------------------------------------------------------------
// void SetParamDepMap(BooleanArray paramDeps)
//------------------------------------------------------------------------------
/*
* Method to set which function points depend on static parameters
*
* @param paramDeps Boolean array of which points depend on static parameters
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetParamDepMap(BooleanArray paramDeps)
{
   if (paramDeps.size() != numPoints)
   {
      std::string errmsg = "The number of static parameter dependencies"
         " assigned does not match the number of points in"
         " OptimalControlFunction \"" + funcName + "\"";
      throw LowThrustException(errmsg);
   }
   paramDepMap = paramDeps;
}

//------------------------------------------------------------------------------
// void SetFuncScalingUtility(ScalingUtility *scaleUtil)
//------------------------------------------------------------------------------
/*
* Method to set scaling utility function values use
*
* @param scaleUtil The new function scaling utility
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::SetFuncScalingUtility(ScalingUtility *scaleUtil)
{
   if (funcScaleUtil)
      delete funcScaleUtil;
   funcScaleUtil = scaleUtil;
}

//------------------------------------------------------------------------------
// void ValidatePointIdx(Integer pointIdx)
//------------------------------------------------------------------------------
/*
* Method to check that an input discretization point index exists
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::ValidatePointIdx(Integer pointIdx)
{
   if (pointIdx < 0 || pointIdx > numPoints)
      throw LowThrustException("Invalid optimal control function point index");
}

//------------------------------------------------------------------------------
// void ValidateFunctionBounds()
//------------------------------------------------------------------------------
/*
* Method to check that bounds are the same size and have valid values
*/
//------------------------------------------------------------------------------
void OptimalControlFunction::ValidateFunctionBounds()
{
   if (lowerBounds.GetSize() != upperBounds.GetSize())
      throw LowThrustException("The lower and upper bounds on optimal "
         "control function \"" + funcName + "\" are not the same size");

   for (Integer i = 0; i < lowerBounds.GetSize(); ++i)
   {
      if (lowerBounds(i) > upperBounds(i))
         throw LowThrustException("Lower bound element " +
            GmatStringUtil::ToString(i + 1, 0) + " is greater than the "
            "respective upper bound element on optimal control function \"" +
            funcName + "\"");
   }
}

//------------------------------------------------------------------------------
// bool ValidatePhasePosition(const Integer &phasePos)
//------------------------------------------------------------------------------
/*
* Method to check that a selected phase position is allowed for the optimal
* control function.  This method can be derived by specific optimal control
* functions to select which phase positions can be used.  Otherwise, all 
* postions are allowed.  
*
* @param phasePos The phase position to validate.  0 represnts the initial
*        point, 1 represents the final points, and 2 represents all points
*        in the phase
*/
//------------------------------------------------------------------------------
bool OptimalControlFunction::IsValidPhasePosition(const Integer &phasePos)
{
   return true;
}
