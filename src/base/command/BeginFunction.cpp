//$Id$
//------------------------------------------------------------------------------
//                            BeginFunction
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// order 124.
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/08/30
//
/**
 * Implementation code for the BeginFunction command, a wrapper that manages the
 * commands in a GMAT function.
 */
//------------------------------------------------------------------------------


#include "BeginFunction.hpp"
#include "MessageInterface.hpp"
#include "StopCondition.hpp"
#include "ODEModel.hpp"
#include "PropSetup.hpp"
#include <iostream>                // for cout, endl

#define DEBUG_OBJECT_MAPPING


const std::string
BeginFunction::PARAMETER_TEXT[BeginFunctionParamCount - GmatCommandParamCount] =
{
   "FunctionName",
   "Input",
   "Output",
   "CallFunctionInput",
   "CallFunctionOutput"
};


const Gmat::ParameterType
BeginFunction::PARAMETER_TYPE[BeginFunctionParamCount - GmatCommandParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE
};




BeginFunction::BeginFunction() :
   GmatCommand          ("BeginFunction"),
   functionName         (""),
   gfun                 (NULL),
   transientForces      (NULL),
   internalCoordSys     (NULL)
{
}



BeginFunction::~BeginFunction()
{
   ClearReturnObjects();
}



BeginFunction::BeginFunction(const BeginFunction& bf) :
   GmatCommand          (bf),
   functionName         (bf.functionName),
   gfun                 (NULL),
   inputs               (bf.inputs),
   outputs              (bf.outputs),
   inputObjects         (bf.inputObjects),
   outputObjects        (bf.outputObjects),
   returnObjects        (bf.returnObjects),
   transientForces      (NULL),
   internalCoordSys     (NULL)
{
   localMap.clear();
}



BeginFunction& BeginFunction::operator=(const BeginFunction& bf)
{
   if (this != &bf)
   {
      functionName = bf.functionName;
      gfun = NULL;
      inputs = bf.inputs;
      outputs = bf.outputs;
      inputObjects = bf.inputObjects;
      outputObjects = bf.outputObjects;
      localMap = bf.localMap;
      returnObjects = bf.returnObjects;
      transientForces = bf.transientForces;
      internalCoordSys = bf.internalCoordSys;
   }

   return *this;
}


GmatBase* BeginFunction::Clone() const
{
   return new BeginFunction(*this);
}


GmatBase* BeginFunction::GetRefObject(const UnsignedInt type,
                                    const std::string &name)
{
   if (type == Gmat::UNKNOWN_OBJECT)  // Just find it by name
   {
      /// @todo Look up return object based on the name used in function call
      for (unsigned int i=0; i<outputObjects.size(); i++)
      {
         if (outputObjects[i] == name)
         {
//               MessageInterface::ShowMessage("\nFound output object %s in position %d.  Return Object size is %d\n",
//                  name.c_str(), i, returnObjects.size());
            return returnObjects[i];
         }
      }
      
      // Check local object map in case we are setting a local object parameter
      if (localMap.find(name) != localMap.end())
         return localMap[name];
   }


   return GmatCommand::GetRefObject(type, name);
}


// Function used to put objects in the local onject map
bool BeginFunction::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                    const std::string &name)
{
   bool retval = false;
   if (localMap.find(name) == localMap.end())
   {
      localMap[name] = obj;
      retval = true;
   }
   else
   {
      MessageInterface::ShowMessage(
         "Attempted to add the object %s which is already in the "
         "GmatFunction %s\n", name.c_str(), functionName.c_str());
   }

   return retval;
}

bool BeginFunction::RenameRefObject(const UnsignedInt type,
                                    const std::string &oldName,
                                    const std::string &newName)
{
   return true;
}



//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the name of the parameter with the input id.
 *
 * @param <id> Integer id for the parameter.
 *
 * @return The string name of the parameter.
 */
//------------------------------------------------------------------------------
std::string BeginFunction::GetParameterText(const Integer id) const
{
   if ((id >= GmatCommandParamCount) && (id < BeginFunctionParamCount))
      return PARAMETER_TEXT[id - GmatCommandParamCount];
   return GmatCommand::GetParameterText(id);
}



//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Gets the id corresponding to a named parameter.
 *
 * @param <str> Name of the parameter.
 *
 * @return The ID.
 */
//------------------------------------------------------------------------------
Integer BeginFunction::GetParameterID(const std::string &str) const
{
   for (int i = 0; i < BeginFunctionParamCount - GmatCommandParamCount; ++i)
      if (PARAMETER_TEXT[i] == str)
         return i+GmatCommandParamCount;

   return GmatCommand::GetParameterID(str);
}



//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the type of a parameter.
 *
 * @param <id> Integer ID of the parameter.
 *
 * @return The type of the parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType BeginFunction::GetParameterType(const Integer id) const
{
   if ((id >= GmatCommandParamCount) && (id < BeginFunctionParamCount))
      return PARAMETER_TYPE[id - GmatCommandParamCount];
   return GmatCommand::GetParameterType(id);
}



//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the text description for the type of a parameter.
 *
 * @param <id> Integer ID of the parameter.
 *
 * @return The text description of the type of the parameter.
 */
//------------------------------------------------------------------------------
std::string BeginFunction::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}



//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool BeginFunction::IsParameterReadOnly(const Integer id) const
{
   if ((id >= GmatCommandParamCount) && (id < BeginFunctionParamCount))
      return true;
   return GmatCommand::IsParameterReadOnly(id);
}



//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Text description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool BeginFunction::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}



//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a std::string parameter.
 *
 * @param <id> Integer ID of the parameter.
 *
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
std::string BeginFunction::GetStringParameter(const Integer id) const
{
   if (id == FUNCTION_NAME)
   {
      return functionName;
   }

   return GmatCommand::GetStringParameter(id);
}



//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a std::string parameter.
 *
 * @param <id>    Integer ID of the parameter.
 * @param <index> Index for array parameter values.
 *
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
std::string BeginFunction::GetStringParameter(const Integer id,
                                              const Integer index) const
{
   return GmatCommand::GetStringParameter(id, index);
}



//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets an array do std::strings.
 *
 * @param <id> Integer ID of the array.
 *
 * @return A reference to the array.
 */
//------------------------------------------------------------------------------
const StringArray& BeginFunction::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
      case INPUTS:
         return inputs;

      case OUTPUTS:
         return outputs;

      case INPUT_OBJECT_NAMES:
         return inputObjects;

      case OUTPUT_OBJECT_NAMES:
         return outputObjects;

      default:
         ;     // Do nothing if it isn't a BeginFunction parameter
   }

   return GmatCommand::GetStringArrayParameter(id);
}



//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 *
 * @param <id>    Integer ID of the parameter.
 * @param <value> New value for the parameter.
 *
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool BeginFunction::SetStringParameter(const Integer id,
                                       const std::string &value)
{
   bool retval = false;

   switch (id)
   {
      case FUNCTION_NAME:
         if (value != "")
         {
            functionName = value;
            retval = true;
         }
         break;

      case INPUTS:
         if ((value != "") &&
             (find(inputs.begin(), inputs.end(), value) == inputs.end()))
         {
            inputs.push_back(value);
            retval = true;
         }
         break;

      case OUTPUTS:
         if ((value != "") &&
             (find(outputs.begin(), outputs.end(), value) == outputs.end()))
         {
            outputs.push_back(value);
            retval = true;
         }
         break;

      case INPUT_OBJECT_NAMES:
         if ((value != "") &&
             (find(inputObjects.begin(), inputObjects.end(), value) ==
              inputObjects.end()))
         {
            inputObjects.push_back(value);
            retval = true;
         }
         break;

      case OUTPUT_OBJECT_NAMES:
         if ((value != "") &&
             (find(outputObjects.begin(), outputObjects.end(), value) ==
              outputObjects.end()))
         {
            outputObjects.push_back(value);
            retval = true;
         }
         break;

      default:
         ;     // Do nothing if it isn't a BeginFunction parameter
   }

   if (retval)
      return true;

   return GmatCommand::SetStringParameter(id, value);
}



//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const Real value,
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter in an array.
 *
 * @param <id>    Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * @param <index> Index into the array for the parameter.
 *
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool BeginFunction::SetStringParameter(const Integer id,
                                       const std::string &value,
                                       const Integer index)
{
   return GmatCommand::SetStringParameter(id, value, index);
}



//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a std::string parameter.
 *
 * @param <label> String identifier for the parameter.
 *
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
std::string BeginFunction::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}



//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a std::string parameter in an array.
 *
 * @param <label> String identifier for the parameter.
 * @param <index> Index into the array for the parameter.
 *
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
std::string BeginFunction::GetStringParameter(const std::string &label,
                                              const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}



//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets an array do std::strings.
 *
 * @param <label> Text name for the array.
 *
 * @return A reference to the array.
 */
//------------------------------------------------------------------------------
const StringArray&
         BeginFunction::GetStringArrayParameter(const std::string &label) const
{
   return BeginFunction::GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 *
 * @param <label> String identifier for the parameter.
 * @param <value> New value for the parameter.
 *
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool BeginFunction::SetStringParameter(const std::string &label,
                                       const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}



//------------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const Real value,
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter in an array.
 *
 * @param <label> String identifier for the parameter.
 * @param <value> New value for the parameter.
 * @param <index> Index into the array for the parameter.
 *
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool BeginFunction::SetStringParameter(const std::string &label,
                                       const std::string &value,
                                       const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}



bool BeginFunction::TakeAction(const std::string &action,
                               const std::string &actionData)
{
   if (action == "ClearLocalData")
   {
      inputObjects.clear();
      outputObjects.clear();
//      ClearInputMap();
   }
   else if (action == "SetReturnObjects")
   {
      ClearReturnObjects();
//      GmatBase *obj;
//      std::map<std::string, GmatBase *>::iterator omi;
//
////      for (StringArray::iterator i = outputs.begin();
////           i != outputs.end(); ++i)
//      for (unsigned int i=0; i< outputs.size(); ++i)
//      {
//         std::string tmpStr = outputs[i];
//         tmpStr = trimIt(tmpStr);
//         MessageInterface::ShowMessage("******i = '%s'\n", tmpStr.c_str());
//         for (omi = localMap.begin(); omi != localMap.end(); ++omi)
//         {
//            obj = omi->second;
//            MessageInterface::ShowMessage("obj = '%s'\n", obj->GetName().c_str());
////            if (strcmp(obj->GetName().c_str(), tmpStr.c_str()) == 0)
//            if (obj->GetName() == tmpStr)
//            {
//               MessageInterface::ShowMessage("object name and *i match '%s'\n", tmpStr.c_str());
//            }
//            else
//            {
//            }
//         }
//      }
      int index = 0;


      for (StringArray::iterator i = outputs.begin();
           i != outputs.end(); ++i)
      {
         (*i) = trimIt(*i);
         if (localMap.find(*i) != localMap.end())
         {
            MessageInterface::ShowMessage("Found %s \n", (*i).c_str());
            MessageInterface::ShowMessage("Found %s with value %f\n", (*i).c_str(),
                  ((Variable *)localMap[(*i).c_str()])->EvaluateReal() );
            GmatBase *outObj = localMap[(*i).c_str()];
            outObj->SetName(outputObjects[index]);
            returnObjects.push_back(outObj);
         }
         index++;
      }
   }


   return true;
}



void BeginFunction::ClearInputMap()
{
   for (std::map <std::string, GmatBase *>::iterator i = localMap.begin();
        i != localMap.end(); ++i)
   {
      delete i->second;
   }
   localMap.clear();
}



void BeginFunction::ClearReturnObjects()
{
   for (ObjectArray::iterator i = returnObjects.begin();
        i != returnObjects.end(); ++i)
   {
      delete *i;
   }
   returnObjects.clear();
}



void BeginFunction::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   transientForces = tf;
}



//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the command stream for a GmatCommand.
 *
 * This method maps the objects in the function call to the names used in the
 * function definition, and initializes the commands in the command stream using
 * these objects.  The objects are cloned; hence copy constructors must be
 * correct for successful initialization.
 *
 * @return true always.
 */
//------------------------------------------------------------------------------
bool BeginFunction::Initialize()
{
   #ifdef DEBUG_OBJECT_MAPPING
      MessageInterface::ShowMessage("Initializing function '%s'\n", 
         functionName.c_str());
   #endif
   
   // Find the GmatFunction
   GmatBase *mapObj;
   if ((mapObj = FindObject(functionName)) == NULL)
   {
      //std::string names; // do we need to do al this?  WCS 2008.03.12
      //for (std::map<std::string, GmatBase*>::iterator i = objectMap->begin();
      //     i != objectMap->end(); ++i)
      //{
      //   names += "\n   '";
      //   names += i->first;
      //   names += "'";
      //}
      //throw CommandException("Error setting up GMAT function '" + functionName +
      //   "'; there is no GmatFunction object with that name.\nHere's the list:" +
      //   names);
      throw CommandException("Error setting up GMAT function '" + functionName +
         "'; there is no GmatFunction object with that name.\n");
   }
   
   gfun = (Function *) mapObj;

   if (gfun->GetTypeName() != "GmatFunction")
      throw CommandException("Object type Error: The object named '" +
         functionName +
         "' is a " + gfun->GetTypeName() + ", but a GmatFunction is required.");

   if (inputObjects.size() != inputs.size())
   {
      std::string errmsg =
         "Size mismatch between the inputs and expected inputs to the Gmat "
         "function '";
      errmsg += functionName + "'; \n  Inputs: (";
      for (StringArray::iterator i = inputObjects.begin();
           i != inputObjects.end(); ++i)
         errmsg += (*i) + ", ";
      errmsg += ")\n  Expected: (";
      for (StringArray::iterator i = inputs.begin();
           i != inputs.end(); ++i)
         errmsg += (*i) + ", ";
      errmsg += ")";


      throw CommandException(errmsg);
   }

   if (!internalCoordSys)
      throw CommandException(
         "No reference (internal) coordinate system defined in BeginFunction!");



   // Fill in the local object array with clones of the expected inputs
   for (UnsignedInt i = 0; i < inputObjects.size(); ++i)
   {
      MessageInterface::ShowMessage("Copying object %s to %s\n",
               inputObjects[i].c_str(), inputs[i].c_str());

      GmatBase *mapObj;
      if ((mapObj = FindObject(inputObjects[i])) == NULL)
      {
         std::string errmsg = "Error initializing GmatFunction '";
         throw CommandException(errmsg + functionName + "': could not find " +
            "an object named '" + (inputObjects[i]) + "'");
      }
      GmatBase *inobj = mapObj;
               
      // Give the clone the local variable name
      inobj->SetName(inputs[i]);
      SetRefObject(inobj, Gmat::UNKNOWN_OBJECT, inputs[i]);
   }


   // Now pretend like we're a Sandbox!
   GmatBase *obj;
   GmatCommand *cmd;
   std::map<std::string, GmatBase *>::iterator omi;
   
   // Init local objects
   for (omi = localMap.begin(); omi != localMap.end(); ++omi)
   {
      obj = omi->second;
      obj->SetSolarSystem(solarSys);
      
      #ifdef DEBUG_OBJECT_MAPPING
         MessageInterface::ShowMessage("Solar system set on '%s'\n", 
            obj->GetName().c_str());
      #endif
      
      // Set J2000 Body for all SpacePoint derivatives before anything else
      if (obj->IsOfType(Gmat::SPACE_POINT))
      {
         #ifdef DEBUG_SANDBOX_INIT
            MessageInterface::ShowMessage(
               "Setting J2000 Body for %s in the Sandbox\n",
               obj->GetName().c_str());
         #endif
         SpacePoint *spObj = (SpacePoint *)obj;
         SpacePoint *j2k = FindSpacePoint(spObj->GetJ2000BodyName());
         if (j2k)
            spObj->SetJ2000Body(j2k);
         else
            throw CommandException("GmatFunction did not find the "
               "Spacepoint \"" +
               spObj->GetJ2000BodyName() + "\"");
      }
   }


   // Add the parameters and global coordinate systems from the Sandbox
   for (omi = objectMap->begin(); omi != objectMap->end(); ++omi)
   {
      obj = omi->second;
      // Set J2000 Body for all SpacePoint derivatives before anything else
      if (obj->IsOfType(Gmat::PARAMETER))
      {
         SetRefObject(obj, obj->GetType(), obj->GetName());
      }
      if (obj->IsOfType(Gmat::COORDINATE_SYSTEM))
      {
         std::string csName = obj->GetName();
         if ((csName == "EarthMJ2000Eq") ||
             (csName == "EarthMJ2000Ec") ||
             (csName == "EarthFixed")    ||
             (csName == "EarthICRF"))
            SetRefObject(obj, obj->GetType(), obj->GetName());
      }
   }
   for (omi = globalObjectMap->begin(); omi != globalObjectMap->end(); ++omi)
   {
      obj = omi->second;
      // Set J2000 Body for all SpacePoint derivatives before anything else
      if (obj->IsOfType(Gmat::PARAMETER))
      {
         SetRefObject(obj, obj->GetType(), obj->GetName());
      }
      if (obj->IsOfType(Gmat::COORDINATE_SYSTEM))
      {
         std::string csName = obj->GetName();
         if ((csName == "EarthMJ2000Eq") ||
             (csName == "EarthMJ2000Ec") ||
             (csName == "EarthFixed"))
            SetRefObject(obj, obj->GetType(), obj->GetName());
      }
   }

   ObjectArray newObj;
   ObjectArray newParam;


//   MessageInterface::ShowMessage("*****LOCAL MAP*****\n");
   // Add the parameters and global coordinate systems from the Sandbox
   for (omi = localMap.begin(); omi != localMap.end(); ++omi)
   {
      obj = omi->second;
//      MessageInterface::ShowMessage("obj name is %s and type is %s\n",
//      obj->GetName().c_str(), obj->GetTypeName().c_str());


      if (((FindObject(obj->GetName())) == NULL) &&
         (obj->GetType() != Gmat::PARAMETER))
      {
//        MessageInterface::ShowMessage("adding obj to obj array\n");
        newObj.push_back(obj);
      }
      else if (((FindObject(obj->GetName())) == NULL) &&
         (obj->GetType() == Gmat::PARAMETER))
      {
//        MessageInterface::ShowMessage("adding obj to param array\n");
        newParam.push_back(obj);
      }
//      else if (obj->GetType() == Gmat::COORDINATE_SYSTEM)
//      {
//         newObj.push_back(obj);
//      }


//      if (obj->GetType() == Gmat::PARAMETER)
//      {
//         MessageInterface::ShowMessage("initializing...\n");
//         obj->Initialize();
//      }


   }
//    MessageInterface::ShowMessage("*****LOCAL MAP********\n");


   for (unsigned int i=0; i<newParam.size(); i++)
   {
      obj = newParam[i];
      obj->SetSolarSystem(solarSys);


//      MessageInterface::ShowMessage("initializing...%s\n", obj->GetName().c_str());
      Parameter *param = (Parameter *)obj;
      param->SetSolarSystem(solarSys);


      // add all new objects
         for (unsigned int j=0; j<newObj.size(); j++)
         {
//            if (newObj[j]->GetType() == Gmat::COORDINATE_SYSTEM)
//            {
               param->SetInternalCoordSystem(internalCoordSys);
//               MessageInterface::ShowMessage("set internal cs %s\n", newObj[j]
//                           ->GetName().c_str());
//            }
//            else
//            {
                if (param->GetKey() == GmatParam::SYSTEM_PARAM)
                {
                   obj->SetRefObject(newObj[j], newObj[j]->GetType(),
                   newObj[j]->GetName());
                }
//            }
         }


      BuildReferences(obj);
      
      MessageInterface::ShowMessage("Initializing %s\n", obj->GetName().c_str());
      
      obj->Initialize();
//      MessageInterface::ShowMessage("finished initializing...%s, value = %f\n",
//         obj->GetName().c_str(), param->EvaluateReal());


    }

//    throw CommandException("hold on");


   // Init local objects
   for (omi = localMap.begin(); omi != localMap.end(); ++omi)
   {
      obj = omi->second;

      if (obj->GetType() != Gmat::PARAMETER)
      {
         //initialize
         obj->SetSolarSystem(solarSys);
         BuildReferences(obj);
         MessageInterface::ShowMessage("Initializing %s\n", obj->GetName().c_str());
         obj->Initialize();
      }
   }


   cmd = next;
   while (cmd)
   {
      cmd->SetObjectMap(&localMap);
      cmd->SetSolarSystem(solarSys);


//      if (cmd->GetTypeName() == "Propagate")
//      {
//         // set object map and solar sys for all stopping conditions
//         ObjectArray stopCond = cmd->GetRefObjectArray(Gmat::STOP_CONDITION);
//
//         for (unsigned int i=0; i<stopCond.size(); i++)
//         {
////            ((StopCondition *)stopCond[i])->SetObjectMap(&localMap);
//            stopCond[i]->SetSolarSystem(solarSys);
//
////            for (unsigned int j=0; j<newParam.size(); j++)
////            {
////               if (newParam[j]->GetType() != Gmat::COORDINATE_SYSTEM)
////               {
////                  stopCond[i]->SetRefObject(newParam[j], newParam[j]->GetType(),
////                  newParam[j]->GetName());
////               }
////            }
//         }
//
//      }


      // Handle nested GmatFunctions
      if (cmd->GetTypeName() == "CallFunction")
      {
         // Check to see if it is a GmatFunction
         std::string funName = cmd->GetStringParameter("FunctionName");
         GmatBase *mapObj;
         if ((mapObj = FindObject(funName)) == NULL)
            throw CommandException("The GmatFunction '" + functionName +
               "' references the function '" + funName +
               "', which cannot be found.");
         Function *fun = (Function*) mapObj;
         if (fun->GetTypeName() == "GmatFunction")
         {
//            /// @todo Make the GmatFunction file name handling more robust
//            std::string pathAndName =
//               fun->GetStringParameter(fun->GetParameterID("FunctionPath"));
//            if (pathAndName == "")
//               pathAndName = funName + ".gmf";
//
//            GmatCommand *funStream =
//               moderator->InterpretGmatFunction(pathAndName);
//            if (!current->SetRefObject(funStream, Gmat::COMMAND, ""))
//            {
//               std::string errstr = "Error setting the GmatFunction commands "
//                  "for the script line\n  '";
//               throw SandboxException(errstr +
//                  current->GetGeneratingString(Gmat::SCRIPTING) + "'");
//            }
            throw CommandException(
               "Nesting is currently disabled for GmatFunctions");
         }
      }

      if (cmd->GetTypeName() == "EndFunction")
      {
//         MessageInterface::ShowMessage("End function - outputs:\n");
         // print output and output objects
         for (StringArray::iterator i = outputs.begin();
           i != outputs.end(); ++i)
            MessageInterface::ShowMessage(*i);


//         MessageInterface::ShowMessage("\n\nOutput Objects:\n");
         for (StringArray::iterator i = outputObjects.begin();
           i != outputObjects.end(); ++i)
            MessageInterface::ShowMessage(*i);



//         MessageInterface::ShowMessage("\n\nReturn Objects:\n");
//         for (StringArray::iterator i = outputObjects.begin();
//           i != outputObjects.end(); ++i)
//            MessageInterface::ShowMessage(*i);


      }


      bool rv = cmd->Initialize();
      if (!rv)
         return false;


      cmd->SetTransientForces(transientForces);
      cmd = cmd->GetNext();
   }


//   MessageInterface::ShowMessage("\nreturning from begin function initialize\n");
   return true;
}



//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Runs the command.  This is a NoOp for the BeginFunction command
 *
 * @return true always.
 */
//------------------------------------------------------------------------------
bool BeginFunction::Execute()
{
   bool rv = true;
   
   // Refresh input objects
   for (UnsignedInt i = 0; i < inputObjects.size(); ++i)
   {
      #ifdef DEBUG_OBJECT_MAPPING
         MessageInterface::ShowMessage("Mapping %s to %s\n", 
            inputObjects[i].c_str(), inputs[i].c_str());
      #endif
      GmatBase *inObj, *localObj;
      
      inObj = FindObject(inputObjects[i]);
      localObj = localMap[inputs[i]];

      // For now just copy select objects
      if ( (localObj->IsOfType(Gmat::SPACECRAFT))
           // Add others here, using ||
         )
      {
         localObj->Copy(inObj); 
      }
   }

   #ifdef DEBUG_OBJECT_MAPPING
      MessageInterface::ShowMessage("reset the objects\n");
   #endif

   return rv;
}



//------------------------------------------------------------------------------
// SpacePoint* FindSpacePoint(const std::string &spname)
//------------------------------------------------------------------------------
/**
 *  Finds a SpacePoint by name.
 *
 *  @param <spname> The name of the SpacePoint.
 *
 *  @return A pointer to the SpacePoint, or NULL if it does not exist in the
 *          Sandbox.
 */
//------------------------------------------------------------------------------
SpacePoint* BeginFunction::FindSpacePoint(const std::string &spName)
{
   SpacePoint *sp = solarSys->GetBody(spName);


   if (sp == NULL)
   {
      GmatBase *mapObj;
      if ((mapObj = FindObject(spName)) != NULL)
      {
         if (mapObj->IsOfType(Gmat::SPACE_POINT))
            sp = (SpacePoint*)mapObj;
      }
   }


   return sp;
}


//------------------------------------------------------------------------------
// void BuildReferences()
//------------------------------------------------------------------------------
/**
 *  Sets all refence objects for the input object.
 */
//------------------------------------------------------------------------------
void BeginFunction::BuildReferences(GmatBase *obj)
{
   std::string oName;


   obj->SetSolarSystem(solarSys);
   // PropSetup probably should do this...
   if ((obj->GetType() == Gmat::PROP_SETUP) ||
       (obj->GetType() == Gmat::ODE_MODEL))
   {
      ODEModel *fm = ((PropSetup *)obj)->GetODEModel();
      fm->SetSolarSystem(solarSys);


      #ifdef DEBUG_FM_INITIALIZATION
         MessageInterface::ShowMessage(
            "Initializing force model references for '%s'\n",
            (fm->GetName() == "" ? obj->GetName().c_str() :
                                   fm->GetName().c_str()) );
      #endif


      try
      {
         StringArray fmRefs = fm->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
         for (StringArray::iterator i = fmRefs.begin();
              i != fmRefs.end(); ++i)
         {
            oName = *i;
            try
            {
               SetRefFromName(fm, oName);
            }
            catch (CommandException &ex)
            {
               // Use exception to remove Visual C++ warning
               ex.GetMessageType();

               // Handle SandboxExceptions first.
               #ifdef DEBUG_SANDBOX_INIT
                  MessageInterface::ShowMessage(
                     "RefObjectName " + oName + " not found; ignoring " +
                     ex.GetMessage() + "\n");
               #endif
               //throw ex;
            }
            catch (BaseException &ex)
            {
               // Use exception to remove Visual C++ warning
               ex.GetMessageType();
               // Post a message and -- otherwise -- ignore the exceptions
               // Handle SandboxExceptions first.
               #ifdef DEBUG_SANDBOX_INIT
                  MessageInterface::ShowMessage(
                     "RefObjectName not found; ignoring " +
                     ex.GetMessage() + "\n");
               #endif
            }
         }
      }
      catch (CommandException &ex)
      {
         // Use exception to remove Visual C++ warning
         ex.GetMessageType();
         // Handle SandboxExceptions first.
         #ifdef DEBUG_SANDBOX_INIT
            MessageInterface::ShowMessage(
               "RefObjectNameArray not found; ignoring " +
               ex.GetMessage() + "\n");
         #endif
         //throw ex;
      }
      catch (BaseException &ex) // Handles no refObject array
      {
         // Use exception to remove Visual C++ warning
         ex.GetMessageType();

         // Post a message and -- otherwise -- ignore the exceptions
         #ifdef DEBUG_SANDBOX_INIT
            MessageInterface::ShowMessage(
               "RefObjectNameArray not found; ignoring " +
               ex.GetMessage() + "\n");
         #endif
      }


      if (obj->GetType() == Gmat::ODE_MODEL)
         return;
   }


   try
   {
      // First set the individual reference objects
      oName = obj->GetRefObjectName(Gmat::UNKNOWN_OBJECT);
      SetRefFromName(obj, oName);
   }
   catch (CommandException &ex)
   {
      // Use exception to remove Visual C++ warning
      ex.GetMessageType();

      // Handle SandboxExceptions first.
      // For now, post a message and -- otherwise -- ignore exceptions
      #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage("RefObjectName not found; ignoring " +
            ex.GetMessage() + "\n");
      #endif
      //throw ex;
   }
   catch (BaseException &ex)
   {
      // Use exception to remove Visual C++ warning
      ex.GetMessageType();
      // Post a message and -- otherwise -- ignore the exceptions
      #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage("RefObjectName not found; ignoring " +
            ex.GetMessage() + "\n");
      #endif
   }


   // Next handle the array version
   try
   {
      StringArray oNameArray =
         obj->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
      for (StringArray::iterator i = oNameArray.begin();
           i != oNameArray.end(); ++i)
      {
         oName = *i;
         try
         {
            SetRefFromName(obj, oName);
         }
         catch (CommandException &ex)
         {
            // Use exception to remove Visual C++ warning
            ex.GetMessageType();
            // Handle SandboxExceptions first.
            #ifdef DEBUG_SANDBOX_INIT
               MessageInterface::ShowMessage(
                  "RefObjectName " + oName + " not found; ignoring " +
                  ex.GetMessage() + "\n");
            #endif
            //throw ex;
         }
         catch (BaseException &ex)
         {
            // Use exception to remove Visual C++ warning
            ex.GetMessageType();
            // Post a message and -- otherwise -- ignore the exceptions
            // Handle SandboxExceptions first.
            #ifdef DEBUG_SANDBOX_INIT
               MessageInterface::ShowMessage(
                  "RefObjectName not found; ignoring " +
                  ex.GetMessage() + "\n");
            #endif
         }
      }
   }
   catch (CommandException &ex)
   {
      // Use exception to remove Visual C++ warning
      ex.GetMessageType();
      // Handle SandboxExceptions first.
      #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage(
            "RefObjectNameArray not found; ignoring " +
            ex.GetMessage() + "\n");
      #endif
      //throw ex;
   }
   catch (BaseException &ex) // Handles no refObject array
   {
      // Use exception to remove Visual C++ warning
      ex.GetMessageType();
      // Post a message and -- otherwise -- ignore the exceptions
      #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage(
            "RefObjectNameArray not found; ignoring " +
            ex.GetMessage() + "\n");
      #endif
   }
}



//------------------------------------------------------------------------------
// void InitializeInternalObjects()
//------------------------------------------------------------------------------
/**
 *  Initializes internal objects in the sandbox.
 */
//------------------------------------------------------------------------------
void BeginFunction::InitializeInternalObjects()
{
//   SpacePoint *sp, *j2kBod;
//   std::string j2kName, oName;
//
//   // Set J2000 bodies for solar system objects -- should this happen here?
//   const StringArray biu = solarSys->GetBodiesInUse();
//   for (StringArray::const_iterator i = biu.begin(); i != biu.end(); ++i)
//   {
//      sp = solarSys->GetBody(*i);
//      j2kName = sp->GetStringParameter("J2000BodyName");
//      j2kBod = FindSpacePoint(j2kName);
//      sp->SetJ2000Body(j2kBod);
//   }
//
//   // set ref object for internal coordinate system
//   internalCoordSys->SetSolarSystem(solarSys);
//
//   BuildReferences(internalCoordSys);
//
//   // Set reference origin for internal coordinate system.
//   oName = internalCoordSys->GetStringParameter("Origin");
//   sp = FindSpacePoint(oName);
//   if (sp == NULL)
//      throw SandboxException("Cannot find SpacePoint named \"" +
//         oName + "\" used for the internal coordinate system origin");
//   internalCoordSys->SetRefObject(sp, Gmat::SPACE_POINT, oName);
//
//   // Set J2000 body for internal coordinate system
//   oName = internalCoordSys->GetStringParameter("J2000Body");
//   sp = FindSpacePoint(oName);
//   if (sp == NULL)
//      throw SandboxException("Cannot find SpacePoint named \"" +
//         oName + "\" used for the internal coordinate system J2000 body");
//   internalCoordSys->SetRefObject(sp, Gmat::SPACE_POINT, oName);
//
//   internalCoordSys->Initialize();
}


//------------------------------------------------------------------------------
// void SetRefFromName(GmatBase *obj, const std::string &oName)
//------------------------------------------------------------------------------
/**
 *  Sets a reference object on an object.
 *
 *  @param <obj>   The object that needs to set the reference.
 *  @param <oName> The name of the reference object.
 */
//------------------------------------------------------------------------------
void BeginFunction::SetRefFromName(GmatBase *obj, const std::string &oName)
{
   GmatBase *mapObj;
   if ((mapObj = FindObject(oName)) != NULL)
   {
      obj->SetRefObject(mapObj, mapObj->GetType(), mapObj->GetName());
   }
   else
   {
      // look SolarSystem
      GmatBase *refObj = FindSpacePoint(oName);


      if (refObj == NULL)
         throw CommandException("Unknown object " + oName + " requested by " +
                                obj->GetName());


      obj->SetRefObject(refObj, refObj->GetType(), refObj->GetName());
   }
}


//------------------------------------------------------------------------------
// bool SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 *  Sets the internal coordinate system used by the Sandbox.
 *
 *  @param <cs> The internal coordinate system.
 *
 *  @return true if the command was added to the sequence, false if not.
 */
//------------------------------------------------------------------------------
void BeginFunction::SetInternalCoordSystem(CoordinateSystem *cs)
{


//   MessageInterface::ShowMessage("BeginFunction::SetInternalCoordSystem entered\n");


   if (!cs)
      internalCoordSys = cs;
      //return false;
   
   
//   MessageInterface::ShowMessage("Set internal coord sys to %s\n", cs->GetTypeName().c_str());
   /// @todo Check initialization and cloning for the internal CoordinateSystem.
   //internalCoordSys = (CoordinateSystem*)(cs->Clone());
   //internalCoordSys = cs;
   //return true;
}


std::string BeginFunction::trimIt( std::string s ) {
        size_t offset = s.length();
        // Trim leading spaces ???
        s.erase( 0, s.find_first_not_of( " \t\n" ) );
        // Trim trailing spaces ???
        s.erase( s.find_last_not_of( " \t\n" ) + 1);
        // This line is always displayed
        if(s.length() == offset){ std::cout << "No Changes" << std::endl;}
        return s;


}
