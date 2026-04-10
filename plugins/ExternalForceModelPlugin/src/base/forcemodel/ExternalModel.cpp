//$Id$
//------------------------------------------------------------------------------
//                              ExternalModel
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
// Developed jointly by NASA/GSFC and Emergent Space Technologies, Inc. 
//
// Author: Alex Campbell
// Created: 07/25/2022
///

#include "ExternalModel.hpp"
#include <sstream>                      // For stringstream
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "GmatDefaults.hpp"
#include "PropagationStateManager.hpp"
#include "InterfaceException.hpp"
#include "FileManager.hpp"
#include "StringUtil.hpp"

//#define DEBUG_INITIALIZATION
//#define DEBUG_REGISTRATION


//---------------------------------
// static data
//---------------------------------
const std::string
ExternalModel::PARAMETER_TEXT[ExternalParamCount - PhysicalModelParamCount] =
{
    "ScriptFileName",
    "ExcludeOtherForces",
    "DerivativesFunction", 
};

const Gmat::ParameterType
ExternalModel::PARAMETER_TYPE[ExternalParamCount - PhysicalModelParamCount] =
{
   Gmat::FILENAME_TYPE,  
   Gmat::BOOLEAN_TYPE,
   Gmat::STRING_TYPE,  
};
           
//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// ExternalModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 * 
 * @param name Name of the force object
 */
//------------------------------------------------------------------------------
ExternalModel::ExternalModel(const std::string &name) :
   PhysicalModel       (Gmat::PHYSICAL_MODEL, "ExternalModel", name),   
   scriptFilename           (""),
   fullFilePath              (""),
   excludeForces            (false),
   entryPoint               ("GetDerivatives"),
   torqueEntryPoint         (""),
   pythonIf                 (NULL),
   satCount                 (0)
{
   parameterCount = ExternalParamCount;
   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   objectTypeNames.push_back("ExternalModel");
   objectTypeNames.push_back("External");

   isConservative = false;
   hasMassJacobian = true;
   initializedMessagePrinted = false;
}

//------------------------------------------------------------------------------
// ExternalModel(ExternalModel &srp)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 * 
 * @param external The force copied to this one.
 */
//------------------------------------------------------------------------------
ExternalModel::ExternalModel(const ExternalModel &external) :
    PhysicalModel           (external),
	scriptFilename          (external.scriptFilename),
   fullFilePath            (external.fullFilePath),
	excludeForces           (external.excludeForces),
	entryPoint              (external.entryPoint),
	torqueEntryPoint        (external.torqueEntryPoint),
	pythonIf                (external.pythonIf),
	satCount                (external.satCount)
{
   parameterCount = ExternalParamCount;
   initializedMessagePrinted = external.initializedMessagePrinted;
   isInitialized = false;
}

//------------------------------------------------------------------------------
// ExternalModel& operator=(ExternalModel &srp)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param external The force passing values to this one
 *
 * @return This external force, set to match the external input model
 */
//------------------------------------------------------------------------------
ExternalModel& ExternalModel::operator=(const ExternalModel &external)
{
   if (this != &external)
   {
      PhysicalModel::operator=(external);
      scriptFilename = external.scriptFilename;
      fullFilePath = external.fullFilePath;
      excludeForces = external.excludeForces;
      entryPoint = external.entryPoint;
	   torqueEntryPoint = external.torqueEntryPoint;
      pythonIf = external.pythonIf;
      satCount = external.satCount;
      initializedMessagePrinted = external.initializedMessagePrinted;
      isInitialized = false;
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// ExternalModel::~ExternalModel(void)
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ExternalModel::~ExternalModel()
{ 
    
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ExternalModel.
 *
 * @return clone of the ExternalModel.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ExternalModel::Clone() const
{
   return (new ExternalModel(*this));
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ExternalModel::GetParameterText(const Integer id) const
{
   if (id >= SCRIPT_FILENAME && id < ExternalParamCount)
      return ExternalModel::PARAMETER_TEXT[id - PhysicalModelParamCount];
   else
      return PhysicalModel::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer ExternalModel::GetParameterID(const std::string &str) const
{
   for (int i = SCRIPT_FILENAME; i < ExternalParamCount; i++)
   {
      if (str == ExternalModel::PARAMETER_TEXT[i - PhysicalModelParamCount])
         return i;
   }
   return PhysicalModel::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ExternalModel::GetParameterType(const Integer id) const
{
   if (id >= SCRIPT_FILENAME && id < ExternalParamCount)
      return PARAMETER_TYPE[id - PhysicalModelParamCount];
   else
      return PhysicalModel::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ExternalModel::GetParameterTypeString(const Integer id) const
{
   if (id >= SCRIPT_FILENAME && id < ExternalParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
    else
      return PhysicalModel::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Checks to see if the parameter is user settable
 *
 * @param id The parameter's ID
 *
 * @return true
 */
 //------------------------------------------------------------------------------
bool ExternalModel::IsParameterReadOnly(const Integer id) const
{     
   return true;
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool ExternalModel::GetBooleanParameter(const Integer id) const
{
    if (id == EXCLUDE_OTHER_FORCES)
        return excludeForces;
   
    return PhysicalModel::GetBooleanParameter(id);
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool ExternalModel::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == EXCLUDE_OTHER_FORCES)
   {
	  excludeForces = value;
      return excludeForces;
   }  
       
   return PhysicalModel::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param id    Integer ID for the requested parameter
 */
 //------------------------------------------------------------------------------
std::string ExternalModel::GetStringParameter(const Integer id) const
{
	if (id == SCRIPT_FILENAME) return scriptFilename;
	else if (id == ENTRY_POINT) return entryPoint;
	return PhysicalModel::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    id  Integer ID for the parameter
 * @param    val The new value for the parameter
 */
 //------------------------------------------------------------------------------
bool ExternalModel::SetStringParameter(const Integer id,
	const std::string &value)
{
	if (id == SCRIPT_FILENAME)
	{
      std::string val = value;
      if (GmatStringUtil::IsEnclosedWith(val, "'"))
      {
         val = GmatStringUtil::RemoveEnclosingString(val, "'");
      }
		scriptFilename = val;

		return true;
	}
	if (id == ENTRY_POINT)
	{
      std::string val = value;
      if (GmatStringUtil::IsEnclosedWith(val, "'"))
      {
         val = GmatStringUtil::RemoveEnclosingString(val, "'");
      }
		entryPoint = val;
		return true;
	}
	return PhysicalModel::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param label    string ID for the requested parameter
 */
 //------------------------------------------------------------------------------
std::string ExternalModel::GetStringParameter(const std::string &label) const
{	
	return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const char *value)
//------------------------------------------------------------------------------
bool ExternalModel::SetStringParameter(const std::string &label,
                                                const char *value)
{
   return SetStringParameter(GetParameterID(label), std::string(value));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool ExternalModel::SetStringParameter(const std::string &label,
                                                const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool ExternalModel::Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the external model for use
 */
//------------------------------------------------------------------------------
bool ExternalModel::Initialize()
{
   bool retval = PhysicalModel::Initialize();  

   if (!retval)
      return false; 
      
   isInitialized = true;   
   try
   {
	   pythonIf = PythonInterface::PyInstance();
	   FileManager *fileMgr = FileManager::Instance();

#ifdef DEBUG_INITIALIZATION
	   MessageInterface::ShowMessage("  pythonIf:  %p\n", pythonIf);
#endif
	   //Initialize Python engine
	   pythonIf->PyInitialize();
	   // Get all Python module paths from the startup file
	   StringArray paths = fileMgr->GetAllPythonModulePaths();

#ifdef DEBUG_INITIALIZATION
	   MessageInterface::ShowMessage("  Adding %d python paths\n", paths.size());
#endif
	   pythonIf->PyAddModulePath(paths);
   }   
   catch (...)
   {
	   throw InterfaceException("An unhandled Python exception was thrown during "
		   "initialization");
   }  
   
  


   return isInitialized;
}

//------------------------------------------------------------------------------
// bool ExternalModel::GetDerivatives()(Real * state, Real dt, Integer order)
//------------------------------------------------------------------------------
/**
 * Converts the given parametrs to what is needed to be sent to the python script. 
 * Converts what is returned from the python script into what the parent ODEModel wants. 
 *
 * @param state  The current state vector
 * @param dt     The time offset for the calculations
 * @param order  Order of the derivative being calculated
 */
 //------------------------------------------------------------------------------
bool ExternalModel::GetDerivatives(Real *state, Real dt, Integer order,
	const Integer id) {

   StringArray stateDescription = GetStateDateTypes();

	Real now = epoch + (elapsedTime + dt) / GmatTimeConstants::SECS_PER_DAY;

	for (Integer i = 0; i < satCount; i++)
	{
		Real * st = new Real[dimension];
		Real * dv = new Real[dimension];

      memcpy(st, state, dimension * sizeof(Real));

		dv = PythonDerivatives(st, now, stateDescription, order);

      memcpy(deriv, dv, dimension * sizeof(Real));

   }


	return true;
}

//------------------------------------------------------------------------------
// bool ExternalModel::PythonDerivatives()(Real * state, Real dt, Integer order)
//------------------------------------------------------------------------------
/**
 * Calls the python function 
 *
 * @param state  The current state vector
 * @param now    The time to calculate at
 * @param ep     Entry Point for the python file 
 * @param order  Order of the derivative being calculated
 *
 * @return The derivative vector
 */
 //------------------------------------------------------------------------------
Real* ExternalModel::PythonDerivatives(Real *state, Real now, StringArray stateDescription, Integer order,
	const Integer id) {

   if (initializedMessagePrinted == false)
   {

      fullFilePath = pythonIf->PyExternalGetPythonPath(scriptFilename);
      //print python script and entry point to log
      std::string msg = "External Force Model: \"" + instanceName + "\" was initialized using python file \"";
      msg += fullFilePath + "\" and entry point \"" + entryPoint + "\".\n";
      MessageInterface::ShowMessage(msg);
      initializedMessagePrinted = true;
   }
	PyObject* pyRet;
	Real * div = new Real[dimension];
	
   try
	{
		pyRet = pythonIf->PyExternalFunctionWrapper(scriptFilename, entryPoint, state, now, stateDescription, order, dimension, 4);
	}	
	catch (BaseException &ex)
	{
		throw CommandException("Error in the CallPython command execution:\n" +
			ex.GetFullMessage());
	}
	catch (...)
	{
		throw InterfaceException("An unhandled Python exception was thrown during execution");
	}

	if (pyRet) {
		for (Integer i = 0; i < dimension; ++i)
		{
			PyObject* pyItem = PyList_GetItem(pyRet, i);
			div[i] = PyFloat_AsDouble(pyItem);
		}
	}

	return div;
}


//------------------------------------------------------------------------------
// Rvector6 GetDerivativesForSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
/**
 * Retrieves the Cartesian state vector of derivatives w.r.t. time
 *
 * @param sc The spacecraft that holds the state vector
 *
 * @return The derivative vector
 */
//------------------------------------------------------------------------------
Rvector6 ExternalModel::GetDerivativesForSpacecraft(Spacecraft *sc)
{   
	Rvector6 dv;
	Real *j2kState = sc->GetState().GetState();
	Real state[6];
	Real now = sc->GetEpoch();
	GmatTime nowgt = sc->GetEpochGT();
   StringArray stateDescription = GetStateDateTypes();

	if (hasPrecisionTime)
	{
		BuildModelStateGT(nowgt, state, j2kState);
		dv = PythonDerivatives(state, nowgt.GetMjd(), stateDescription);
	}
	else
	{
		BuildModelState(now, state, j2kState);
		dv = PythonDerivatives(state, now, stateDescription);
	}	

   return dv;
}


//------------------------------------------------------------------------------
// bool IsUnique(const std::string &forBody)
//------------------------------------------------------------------------------
/**
 * Specifies if the External force is the one and only External force allowed
 *
 * @param forBody The body at which the force must be unique (unused for External)
 *
 * @return true for External.
 */
//------------------------------------------------------------------------------
bool ExternalModel::IsUnique(const std::string& forBody)
{
   return true;
}


//------------------------------------------------------------------------------
// bool SupportsDerivative(Gmat::StateElementId id)
//------------------------------------------------------------------------------
/**
 * Function used to determine if the physical model supports derivative 
 * information for a specified type.
 * 
 * @param id State Element ID for the derivative type
 * 
 * @return true if the type is supported, false otherwise. 
 */
//------------------------------------------------------------------------------
bool ExternalModel::SupportsDerivative(Gmat::StateElementId id)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage(
            "ExternalModel checking for support for id %d\n", id);
   #endif
      
   if (id == Gmat::CARTESIAN_STATE)
      return true;
   
   if (id == Gmat::ORBIT_STATE_TRANSITION_MATRIX)
      return true;
   
   if (id == Gmat::ORBIT_A_MATRIX)
      return true;

   return PhysicalModel::SupportsDerivative(id);
}


//------------------------------------------------------------------------------
// bool SetStart(Gmat::StateElementId id, Integer index, Integer quantity)
//------------------------------------------------------------------------------
/**
 * Function used to set the start point and size information for the state 
 * vector, so that the derivative information can be placed in the correct place 
 * in the derivative vector.
 * 
 * @param id           State Element ID for the derivative type
 * @param index        Starting index in the state vector for this type of derivative
 * @param quantity     Number of objects that supply this type of data
 * @param totalSize    For sizable types, the size to use.  For example, for STM,
 *                     this is the number of elements of STM
 * 
 * @return true if the type is supported, false otherwise. 
 */
//------------------------------------------------------------------------------
bool ExternalModel::SetStart(Gmat::StateElementId id, Integer index, 
                        Integer quantity, Integer totalSize)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage("ExternalModel setting start data "
            "for id = %d to index %d; %d objects identified\n", id, index, 
            quantity);
   #endif
   
   bool retval = false;
   
   switch (id)
   {
      case Gmat::CARTESIAN_STATE:
         satCount = quantity;
         cartesianCount = quantity;
         cartesianStart = index;
         fillCartesian = true;
         retval = true;
         break;
         
      case Gmat::ORBIT_STATE_TRANSITION_MATRIX:
         stmCount = quantity;
         stmStart = index;
         fillSTM = true;
         totalSTMSize = totalSize;
         retval = true;
         break;
         
      case Gmat::ORBIT_A_MATRIX:
         aMatrixCount = quantity;
         aMatrixStart = index;
         fillAMatrix = true;
         totalSTMSize = totalSize;
         retval = true;
         break;

      default:
         break;
   }
   
   return retval;
}

//------------------------------------------------------------------------------
// Rvector3 GetTorquesForSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
/**
 * Function used to determine the torque on the spacecraft
 *
 * @return The 3 element torque vector in Nm.
 */
//------------------------------------------------------------------------------
Rvector3 ExternalModel::GetTorquesForSpacecraft(Spacecraft *sc)
{
   //Work in Progress
   Rvector3 torque; 
   if (torqueEntryPoint == "")
	   return torque;

   Real *j2kState = sc->GetState().GetState();
   Real state[6];
   Real now = sc->GetEpoch();
   GmatTime nowgt = sc->GetEpochGT();
   PyObject* pyRet;  
   StringArray stateDescription = GetStateDateTypes();
   try
   {
	   if (hasPrecisionTime)
	   {
		   BuildModelStateGT(nowgt, state, j2kState);
		   pyRet = pythonIf->PyExternalFunctionWrapper(scriptFilename, torqueEntryPoint, state, nowgt.GetMjd(), stateDescription, 1, dimension, 3);
	   }
	   else
	   {
		   BuildModelState(now, state, j2kState);
		   pyRet = pythonIf->PyExternalFunctionWrapper(scriptFilename, torqueEntryPoint, state, now, stateDescription, 1, dimension, 3);
	   }
	  
   }
   catch (BaseException &ex)
   {
	   throw CommandException("Error in the CallPython command torque execution:\n" +
		   ex.GetFullMessage());
   }
   catch (...)
   {
	   throw InterfaceException("An unhandled Python exception was thrown during Torque execution");
   }

   if (pyRet) {
	   for (Integer i = 0; i < 3; ++i)
	   {
		   PyObject* pyItem = PyList_GetItem(pyRet, i);
		   torque[i] = PyFloat_AsDouble(pyItem);
	   }
   }

   return torque;
}

//------------------------------------------------------------------------------
// StringArray GetStateDateTypes()
//------------------------------------------------------------------------------
/**
 * Loops through the state map and renames everything to user friendly names
 * This is then outputted to the python call
 * Returns an StringArray with the names of all of the data types in the state
 * @TODO This should be reworked to not write out the string every single python
 * call which is incredibly expensive. Probably better as an API call or something
 * That can be turned on and off
 *
 */
 //------------------------------------------------------------------------------
StringArray ExternalModel::GetStateDateTypes()
{
   StringArray elements;
   std::vector<ListItem*> stateMap = *(psm->GetStateMap());
   int cartesianCount = 0;
   //GmatState* psmState = psm->GetState();
   //StringArray stateDescription = psmState->GetElementDescriptions();
   
   //This
   for (Integer i = 0; i < stateMap.size(); i++)
   {
      //Handles CartesianState values
      std::string elementStr = "";
      if (stateMap[i]->elementName == "CartesianState")
      {
         std::string elementOne;
         cartesianCount++;

         //Starts at one as it preincrements, could go either way
         switch (cartesianCount % 6)
         {
         case 1: elementOne = "X";
            break;
         case 2: elementOne = "Y";
            break;
         case 3: elementOne = "Z";
            break;
         case 4: elementOne = "Vx";
            break;
         case 5: elementOne = "Vy";
            break;
         case 0: elementOne = "Vz";
            break;
         }
         elementStr = "CartesianState." + elementOne;
      }
      else
      {
         //Handles Covariance values
         if ((stateMap[i]->elementName == "Covariance") || (stateMap[i]->elementName == "STM"))
         {
            std::string elementOne;
            std::string elementTwo;
            switch (stateMap[i]->rowIndex)
            {
            case 0: elementOne = "X";
               break;
            case 1: elementOne = "Y";
               break;
            case 2: elementOne = "Z";
               break;
            case 3: elementOne = "Vx";
               break;
            case 4: elementOne = "Vy";
               break;
            case 5: elementOne = "Vz";
               break;
            default:
               elementOne = psm->GetSTMParameterName(stateMap[i]->rowIndex);
               break;
            }
            switch (stateMap[i]->colIndex)
            {
            case 0: elementTwo = "X";
               break;
            case 1: elementTwo = "Y";
               break;
            case 2: elementTwo = "Z";
               break;
            case 3: elementTwo = "Vx";
               break;
            case 4: elementTwo = "Vy";
               break;
            case 5: elementTwo = "Vz";
               break;
            default:
               elementTwo = psm->GetSTMParameterName(stateMap[i]->colIndex);
               break;
            }
            if (stateMap[i]->elementName == "Covariance")
            {
               elementStr = "Covariance." + elementOne + elementTwo;
            }
            else if (stateMap[i]->elementName == "STM")
            {
               elementStr = "STM." + elementOne + elementTwo;
            }
         }
         else
         {
            //Handles extra cases on a case by case basis
            if (stateMap[i]->elementName == "MassFlow")
            {
               elementStr = "TotalMass";
            }
            else 
            {
               elementStr = stateMap[i]->elementName;
            }
         }
      }
      if (stateMap[i]->object->IsOfType("Formation"))
      {
         int index = (cartesianCount-1) / 6;
         elementStr = stateMap[i]->object->GetRefObjectNameArray(0)[index] + "." + elementStr;
      }
      elementStr = stateMap[i]->objectName + "." + elementStr;
      elements.push_back(elementStr);
   }
   return elements;
}