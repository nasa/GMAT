//$Id$
//------------------------------------------------------------------------------
//                                 Propagate
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/23
//
/**
 * Implementation for the Propagate command class
 */
//------------------------------------------------------------------------------

#include "Propagate.hpp"

#include "Propagator.hpp"
#include "ODEModel.hpp"

#include "Publisher.hpp"
#include "Parameter.hpp"
#include "TextParser.hpp"       // for SeparateBrackets()
#include "StringUtil.hpp"       // for Trim()
#include "AngleUtil.hpp"        // for PutAngleInDegRange()
#include "ColorTypes.hpp"       // for GmatColor::
#include "MessageInterface.hpp"
#include "RgbColor.hpp"         // for ToIntColor()
#include <sstream>
#include <cmath>

//#define DEBUG_PROPAGATE_ASSEMBLE
//#define DEBUG_PARSING
//#define DEBUG_COLOR
//#define DEBUG_PROPAGATE_OBJ 1
//#define DEBUG_PROPAGATE_INIT 1
//#define DEBUG_PROPAGATE_DIRECTION 1
//#define DEBUG_PROPAGATE_STEPSIZE 1
//#define DEBUG_PROPAGATE_EXE 1
//#define DEBUG_STOPPING_CONDITIONS 1
//#define DEBUG_FIRST_STEP_STOP 1
//#define DEBUG_FINITE_MANEUVER
//#define DEBUG_RENAME
//#define DEBUG_PROP_PERFORMANCE
//#define DEBUG_FIRST_CALL
//#define DEBUG_FIXED_STEP
//#define DEBUG_EPOCH_UPDATES
//#define DEBUG_EPOCH_SYNC
//#define DEBUG_SECANT_DETAILS
//#define DEBUG_BISECTION_DETAILS
//#define DEBUG_WRAPPERS
//#define DEBUG_PUBLISH_DATA
//#define DEBUG_TRANSIENT_FORCES
//#define DEBUG_FINAL_STEP
//#define DEBUG_EVENTLOCATORS
//#define DEBUG_CLONES
//#define DEBUG_CLEAN_STRING

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------
std::string Propagate::PropModeList[PropModeCount] =
{
   "", "Synchronized", "BackProp"
};

const std::string
Propagate::PARAMETER_TEXT[PropagateCommandParamCount - GmatCommandParamCount] =
{
   "AvailablePropModes",
   "PropagateMode",
   "InterruptFrequency",
   "StopTolerance",
   "Spacecraft",
   "Propagator",
   "StopCondition",
   "PropForward",
   "AllSTMs",
   "AllAMatrices",
   "AllCovariances"
};

const Gmat::ParameterType
Propagate::PARAMETER_TYPE[PropagateCommandParamCount - GmatCommandParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRING_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::BOOLEAN_TYPE
};

#ifdef DUMP_PLANET_DATA
   std::ofstream Propagate::planetData;

   CelestialBody* Propagate::body[11] =
   {
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
   };

   Integer Propagate::bodiesDefined = 0;
#endif

#ifdef DEBUG_FIRST_CALL
static bool firstStepFired = false;
#endif

//---------------------------------
// public members
//---------------------------------

//------------------------------------------------------------------------------
//  Propagate()
//------------------------------------------------------------------------------
/**
 * Constructs the Propagate Command (default constructor).
 */
//------------------------------------------------------------------------------
Propagate::Propagate() :
   PropagationEnabledCommand   ("Propagate"),
   currentPropMode             (""),
   interruptCheckFrequency     (30),
   inProgress                  (false),
   epochID                     (-1),
//   hasFormation                (false),
   stopInterval                (0.0),
   stopTrigger                 (-1),
   propAllSTMs                 (false),
   calcAllAmatrices            (false),
   calcAllCovariance           (false),
   resetLoopDataFlag           (false),
   checkFirstStep              (true),

   //state                       (NULL),
   //j2kState                    (NULL),

   stopCondMet                 (false),
   stopEpoch                   (0.0),
   stopAccuracy                (DEFAULT_STOP_TOLERANCE),
   timeAccuracy                (1.0e-6),
   dim                         (0),
   cartDim                     (0),
   singleStepMode              (false),
   currentMode                 (INDEPENDENT),
   stopCondEpochID             (-1),
   stopCondBaseEpochID         (-1),
   stopCondStopVarID           (-1),
   segmentOrbitColorStr        (""),
   segmentOrbitColor           (GmatColor::RED),
   overrideSegmentColor        (false)
{
   stepBrackets[0] = stepBrackets[1] = 0.0;
   parameterCount = PropagateCommandParamCount;

   firstStepTolerance = DEFAULT_STOP_TOLERANCE * 10.0;

   #ifdef DUMP_PLANET_DATA
      if (!planetData.is_open())
      {
         planetData.open("PlanetaryEphem.csv");
         planetData.precision(18);
      }
   #endif
}


//------------------------------------------------------------------------------
//  ~Propagate()
//------------------------------------------------------------------------------
/**
 * Destroys the Propagate Command.
 */
//------------------------------------------------------------------------------
Propagate::~Propagate()
{
   EmptyBuffer();

   for (UnsignedInt i = 0; i < stopWhen.size(); ++i)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (stopWhen[i], stopWhen[i]->GetName(), "Propagate::~Propagate()",
          "deleting stop condition");
      #endif
      delete stopWhen[i];
   }

   for (std::vector<PropSetup*>::iterator ps = propagators.begin(); ps != propagators.end();
        ++ps)
   {
      PropSetup *oldPs = *ps;
      *ps = NULL;
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (oldPs, oldPs->GetName(), "Propagate::~Propagate()",
          "deleting old PropSetup");
      #endif
      delete oldPs;
   }

   for (std::vector<StringArray*>::iterator i = satName.begin();
        i != satName.end(); ++i)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         ((*i), "satName", "Propagate::~Propagate()",
          "deleting sat names associated with PropSetup");
      #endif
      delete (*i);
   }

   ClearWrappers();

   #ifdef DUMP_PLANET_DATA
      if (planetData.is_open())
         planetData.close();
   #endif
}


//------------------------------------------------------------------------------
//  Propagate(const Propagate &prp)
//------------------------------------------------------------------------------
/**
 * Constructs a Propagate Command based on another instance (copy constructor).
 *
 * @param <p> Original we are copying
 */
//------------------------------------------------------------------------------
Propagate::Propagate(const Propagate &prp) :
   PropagationEnabledCommand   (prp),
   propName                    (prp.propName),
   satName                     (prp.satName),
   currentPropMode             (prp.currentPropMode),
   interruptCheckFrequency     (prp.interruptCheckFrequency),
   inProgress                  (false),
   epochID                     (prp.epochID),
//   hasFormation                (prp.hasFormation),
   stopInterval                (0.0),
   stopTrigger                 (-1),
   stopSatNames                (prp.stopSatNames),
   objectArray                 (prp.objectArray),
   //elapsedTime                 (prp.elapsedTime), 
   //currEpoch                   (prp.currEpoch), 
   //currEpochGT                 (prp.currEpochGT), 
   propAllSTMs                 (prp.propAllSTMs),
   calcAllAmatrices            (prp.calcAllAmatrices),
   calcAllCovariance           (prp.calcAllCovariance),
   resetLoopDataFlag           (prp.resetLoopDataFlag),
   checkFirstStep              (true),
   //state                       (NULL), 
   //j2kState                    (NULL), 
   stopCondMet                 (false),
   stopEpoch                   (prp.stopEpoch),
   stopAccuracy                (prp.stopAccuracy),
   timeAccuracy                (prp.timeAccuracy),
   dim                         (prp.dim),
   cartDim                     (prp.cartDim),
   singleStepMode              (prp.singleStepMode),
   currentMode                 (prp.currentMode),
   stopCondEpochID             (prp.stopCondEpochID),
   stopCondBaseEpochID         (prp.stopCondBaseEpochID),
   stopCondStopVarID           (prp.stopCondStopVarID),
   segmentOrbitColorStr        (prp.segmentOrbitColorStr),
   segmentOrbitColor           (prp.segmentOrbitColor),
   overrideSegmentColor        (prp.overrideSegmentColor)
{
   parameterCount = prp.parameterCount;
   isInitialized = false;
   baseEpoch.clear();
   baseEpochGT.clear();
   propagators.clear();
   
   currEpoch.clear(); 
   currEpochGT.clear();
   elapsedTime.clear();

   stopWhen.clear();
   stopSats.clear();
   satBuffer.clear();
   formBuffer.clear();
   p.clear();
   fm.clear();
   psm.clear();

   stepBrackets[0] = stepBrackets[1] = 0.0;
   firstStepTolerance = prp.firstStepTolerance;
}


//------------------------------------------------------------------------------
//  Propagate& operator=(const Propagate &prp)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Propagate Command.
 *
 * @param <p> Original we are copying
 *
 * @return reference to this copy
 */
//------------------------------------------------------------------------------
Propagate& Propagate::operator=(const Propagate &prp)
{
   if (&prp == this)
      return *this;

   // Call the base assignment operator
   PropagationEnabledCommand::operator=(prp);

   propName                = prp.propName;
   direction               = prp.direction;
   satName                 = prp.satName;
   currentPropMode         = prp.currentPropMode;
   interruptCheckFrequency = prp.interruptCheckFrequency;
   inProgress              = false;
   hasFired                = false;
   epochID                 = prp.epochID;
//   hasFormation            = prp.hasFormation;
   objectArray             = prp.objectArray;
   //elapsedTime             = prp.elapsedTime;
   //currEpoch               = prp.currEpoch;
   //currEpochGT             = prp.currEpochGT;
   checkFirstStep          = prp.checkFirstStep;

   //state                   = NULL;
   //j2kState                = NULL;
   
   // This step is handled in PropagationEnabledCommand::opreator=() function
   //pubdata                 = NULL;

   stopCondMet             = false;
   stopEpoch               = prp.stopEpoch;
   stopAccuracy            = prp.stopAccuracy;
   timeAccuracy            = prp.timeAccuracy;
   dim                     = prp.dim;
   cartDim                 = prp.cartDim;
   singleStepMode          = prp.singleStepMode;
   currentMode             = prp.currentMode;
   stopCondEpochID         = prp.stopCondEpochID;
   stopCondBaseEpochID     = prp.stopCondBaseEpochID;
   stopCondStopVarID       = prp.stopCondStopVarID;
   segmentOrbitColorStr    = prp.segmentOrbitColorStr;
   segmentOrbitColor       = prp.segmentOrbitColor;
   overrideSegmentColor    = prp.overrideSegmentColor;
   propAllSTMs             = prp.propAllSTMs;
   calcAllAmatrices        = prp.calcAllAmatrices;
   calcAllCovariance       = prp.calcAllCovariance;
   resetLoopDataFlag       = prp.resetLoopDataFlag;

   isInitialized             = false;

   baseEpoch.clear();
   baseEpochGT.clear();
   
   currEpoch.clear();
   currEpochGT.clear();
   elapsedTime.clear();

   for (std::vector<PropSetup*>::iterator ps = propagators.begin(); ps != propagators.end();
        ++ps)
   {
      PropSetup *oldPs = *ps;
      *ps = NULL;
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (oldPs, oldPs->GetName(), "Propagate::operator=()",
          "deleting old PropSetup");
      #endif
      delete oldPs;
   }
   propagators.clear();

   stopWhen.clear();
   stopSats.clear();
   satBuffer.clear();
   formBuffer.clear();
   p.clear();
   fm.clear();
   psm.clear();

   stepBrackets[0] = stepBrackets[1] = 0.0;

   return *this;
}


//------------------------------------------------------------------------------
// bool SetObject(const std::string &name, const UnsignedInt type,
//         const std::string &associate, const UnsignedInt associateType)
//------------------------------------------------------------------------------
/**
 * Sets objects referenced by the Propagate command
 *
 * @param <name> Name of the reference object.
 * @param <type> Type of the reference object.
 * @param <associate> Object associated with this reference object.
 * @param <associateType> Type of the associated object.
 *
 * @return true if the reference was set, false if not.
 */
//------------------------------------------------------------------------------
bool Propagate::SetObject(const std::string &name, const UnsignedInt type,
                          const std::string &associate,
                          const UnsignedInt associateType)
{
   #ifdef DEBUG_PROPAGATE_OBJ
   MessageInterface::ShowMessage
      ("Propagate::SetObject() entered, name='%s', type=%d, associate='%s', "
       "associateType=%d\n", name.c_str(), type, associate.c_str(), associateType);
   #endif

   Integer propNum = propName.size() - 1;

   switch (type) {
      case Gmat::SPACECRAFT:
      case Gmat::FORMATION:
         (satName[propNum])->push_back(name);
         return true;

      case Gmat::PROP_SETUP:
         {
            propName.push_back(name);
            if (name[0] == '-')
            {
               direction = -1.0;
               MessageInterface::ShowMessage("Please use the keyword \"BackProp\" "
                  "to set backwards propagation; the use of a minus sign in the "
                  "string \"%s\" is deprecated.\n", name.c_str());
            }
            StringArray *satNameArray = new StringArray;
            //satName.push_back(new StringArray);
            satName.push_back(satNameArray);
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (satNameArray, "satNameArray", "Propagate::SetObject()",
                "*satNameArray = new StringArray");
            #endif
            return true;
         }

      default:
         break;
   }

   return PropagationEnabledCommand::SetObject(name, type, associate, associateType);
}


//------------------------------------------------------------------------------
// bool SetObject(GmatBase *obj, const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Sets objects referenced by the Propagate command
 *
 * @param <name> Name of the reference object.
 * @param <type> Type of the reference object.
 *
 * @return true if the reference was set, false if not.
 */
//------------------------------------------------------------------------------
bool Propagate::SetObject(GmatBase *obj, const UnsignedInt type)
{
   switch (type)
   {
   case Gmat::STOP_CONDITION:
      stopWhen.push_back((StopCondition *)obj);

      #ifdef DEBUG_STOPPING_CONDITIONS
         MessageInterface::ShowMessage("Adding stopping condition named %s\n",
            obj->GetName().c_str());
      #endif

      stopCondEpochID = obj->GetParameterID("Epoch");
      stopCondBaseEpochID = obj->GetParameterID("BaseEpoch");
      stopCondStopVarID = obj->GetParameterID("StopVar");
      return true;

   default:
      break;
   }

   return PropagationEnabledCommand::SetObject(obj, type);
}

//------------------------------------------------------------------------------
// void ClearObject(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Clears the lists of objects referenced by the Propagate command.
 *
 * @param <type> Type of the objects to clear.
 */
//------------------------------------------------------------------------------
void Propagate::ClearObject(const UnsignedInt type)
{
   switch (type)
   {
   case Gmat::SPACECRAFT:
   case Gmat::FORMATION:
      satName.clear();
      break;
   case Gmat::STOP_CONDITION:
      stopWhen.clear();
      break;

   default:
      break;
   }
}


//------------------------------------------------------------------------------
// GmatBase* GetObject(const UnsignedInt type, const std::string objName)
//------------------------------------------------------------------------------
/**
 * Accesses objects referenced by the Propagate command.
 *
 * @param <type> Type of the reference object.
 * @param <objName> Name of the reference object.
 *
 * @return true if the reference was set, false if not.
 */
//------------------------------------------------------------------------------
GmatBase* Propagate::GetGmatObject(const UnsignedInt type,
                               const std::string objName)
{
   if (type == Gmat::STOP_CONDITION)
   {
      if (stopWhen.empty())
         return NULL;
      else
         return stopWhen[0];
   }

   return PropagationEnabledCommand::GetGmatObject(type, objName);
}

//------------------------------------------------------------------------------
//  const std::string GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param <mode>    Specifies the type of serialization requested.
 * @param <prefix>  Optional prefix appended to the object's name. (Used for
 *                  indentation)
 * @param <useName> Name that replaces the object's name (Not yet used
 *                  in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& Propagate::GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName)
{
   std::string gen = prefix + "Propagate";

   // Construct the generating string
   UnsignedInt index = 0;
   Integer stmCount = 0, aMatCount = 0, covCount = 0;

   if (direction < 0.0)
      gen += (" BackProp");

   if (currentPropMode != "")
      gen += (" " + currentPropMode);
   for (StringArray::iterator prop = propName.begin(); prop != propName.end();
        ++prop)
   {
      gen += " " + (*prop) + "(";
      // Spaceobjects that are propagated by this PropSetup
      StringArray *sats = satName[index];
      for (StringArray::iterator sc = sats->begin(); sc != sats->end(); ++sc)
      {
         // Add a comma if needed
         if (sc != sats->begin())
            gen += ", ";
         if ((*sc) == "STM")
            ++stmCount;
         if ((*sc) == "AMatrix")
            ++aMatCount;
         if ((*sc) == "Covariance")
         {
            ++stmCount;
            ++covCount;
         }
         gen += (*sc);
      }

      if (propAllSTMs && (stmCount == 0))
         gen += ", 'STM'";
      if (calcAllAmatrices && (aMatCount == 0))
         gen += ", 'AMatrix'";
      if (calcAllCovariance && (covCount == 0))
      {
         if(stmCount == 0)
            gen += ", 'STM'";
         gen += ", 'Covariance'";
      }

      gen += ")";
      ++index;
   }

   // Now the stopping conditions.  Note that stopping conditions are shown at
   // the end of the Propagate line, rather than inside of the PropSetup
   // delimiters.
   if (stopWhen.size() > 0) {
      gen += " {";

      for (std::vector<StopCondition*>::iterator stp = stopWhen.begin();
           stp != stopWhen.end(); ++stp) {
         std::stringstream stopCondDesc;
         if (stp != stopWhen.begin())
            gen += ", ";

         std::string stopName = (*stp)->GetStringParameter(stopCondStopVarID);
         stopCondDesc << stopName;

         if ((stopName.find(".Periapsis") == std::string::npos) &&
             (stopName.find(".Apoapsis") == std::string::npos))
            stopCondDesc << " = " << (*stp)->GetStringParameter("Goal");

         gen += stopCondDesc.str();
      }

      // Add the stop tolerance is it is not set to the default value
      if (stopAccuracy != DEFAULT_STOP_TOLERANCE)
      {
         gen += ", StopTolerance = ";
         std::stringstream stopTolDesc;
         stopTolDesc.precision(13);
         stopTolDesc << stopAccuracy;
         gen += stopTolDesc.str();
      }
      gen += "}";
   }
   
   // Now write orbit color
   if (overrideSegmentColor)
   {
      std::string colorStr = ", OrbitColor = " + segmentOrbitColorStr;
      if (stopWhen.size() > 0)
      {
         // Find last closing curly brace
         std::string::size_type lastPos = gen.find_last_of("}");
         if (lastPos != gen.npos)
            gen.insert(lastPos, colorStr);
         else
            ; // Will it ever reach here?
      }
      else
         gen.append("{" + colorStr + "}");
   }
   
   generatingString = gen + ";";
   // Then call the base class method
   return PropagationEnabledCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Propagate.
 *
 * @return clone of the Propagate.
 */
//------------------------------------------------------------------------------
GmatBase* Propagate::Clone() const
{
   #ifdef DEBUG_CLONE
   MessageInterface::ShowMessage("Propagate::Clone() entered\n");
   #endif
   return (new Propagate(*this));
}


//------------------------------------------------------------------------------
//  std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Accessor used to find the names of referenced objects.
 *
 * @param <type> reference object type.
 *
 * @return The name of the reference object.
 */
//------------------------------------------------------------------------------
std::string Propagate::GetRefObjectName(const UnsignedInt type) const
{
   switch (type) {
      // Propagator setups
      case Gmat::PROP_SETUP:
         return propName[0];

      // Objects that get propagated
      case Gmat::SPACECRAFT:
      case Gmat::FORMATION:
         if (satName.size() > 0)
            return (*satName[0])[0];
         break;

      default:
         break;
   }

   return PropagationEnabledCommand::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
//  bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Accessor used to set the names of referenced objects.
 *
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool Propagate::SetRefObjectName(const UnsignedInt type,
                                 const std::string &name)
{
   switch (type) {
      // Propagator setups
      case Gmat::PROP_SETUP:
         {
            propName.push_back(name);
            StringArray *satNameArray = new StringArray;
            //satName.push_back(new StringArray);
            satName.push_back(satNameArray);
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (satNameArray, "satNameArray", "Propagate::SetRefObjectName()",
                "*satNameArray = new StringArray");
            #endif

            return true;
         }

      // Objects that get propagated
      case Gmat::SPACECRAFT:
      case Gmat::FORMATION:
      {
         Integer propNum = propName.size()-1;
         satName[propNum]->push_back(name);
         return true;
      }

      default:
         break;
   }

   return PropagationEnabledCommand::SetRefObjectName(type, name);
}

// Reference object accessor methods
//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name,
//                        const Integer index
//------------------------------------------------------------------------------
/**
 * Accessor for reference object pointers.
 *
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 * @param <index> Index into the object array.
 *
 * @return reference object pointer.
 */
//------------------------------------------------------------------------------
GmatBase* Propagate::GetRefObject(const UnsignedInt type,
                                  const std::string &name, const Integer index)
{
   switch (type)
   {
   case Gmat::PROP_SETUP:
      if (index < (Integer)propagators.size())
      {
         return propagators[index];
      }
      else
      {
         throw CommandException("Propagate::GetRefObject() invalid PropSetup index\n");
      }
      break;
   case Gmat::STOP_CONDITION:
      if (index < (Integer)stopWhen.size())
      {
         return stopWhen[index];
      }
      else
      {
         throw CommandException("Propagate::GetRefObject() invalid index\n");
      }
   default:
      break;
   }

   return PropagationEnabledCommand::GetRefObject(type, name, index);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type, ...
//------------------------------------------------------------------------------
/**
 * Sets reference object pointer.
 *
 * @param <obj> Pointer to the reference object.
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 * @param <index> Index into the object array.
 *
 * @return true if object successfully set, false otherwise
 */
//------------------------------------------------------------------------------
bool Propagate::SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name, const Integer index)
{
   #if DEBUG_PROPAGATE_OBJ
      MessageInterface::ShowMessage
         ("Propagate::SetRefObject() entered, type=%s, name='%s', index=%d, objName='%s'\n",
          obj->GetTypeName().c_str(), name.c_str(), index, obj->GetName().c_str());
   #endif

   switch (type)
   {
      case Gmat::STOP_CONDITION:
         {
            std::string satName = obj->GetName();
            Integer strt = satName.find("StopOn") + 6;
            if (strt == (Integer)std::string::npos)
               strt = 0;
            Integer ndx = satName.find(".",0);
            if (ndx != (Integer)std::string::npos)
               satName = satName.substr(strt, ndx-strt);
            std::string stopStr = obj->GetStringParameter("StopVar");
            std::string goalStr = obj->GetStringParameter("Goal");
            Integer size = stopWhen.size();

            #if DEBUG_PROPAGATE_OBJ
            MessageInterface::ShowMessage
               ("   satName='%s', stopStr='%s', goalStr='%s', stopWhen.size()=%d, "
                "stopNames.size()=%d, goalNames.size()=%d\n",
                satName.c_str(), stopStr.c_str(), goalStr.c_str(), stopWhen.size(),
                stopNames.size(), goalNames.size());
            MessageInterface::ShowMessage
               ("   stopWrappers.size() = %d, goalWrappers.size() = %d\n",
                stopWrappers.size(), goalWrappers.size());
            #endif
            
            if ((stopWhen.empty() && index == 0) || (index == size))
            {
               stopWhen.push_back((StopCondition *)obj);
               stopSatNames.push_back(satName);
               stopNames.push_back(stopStr);
               goalNames.push_back(goalStr);
               if ((Integer)stopWrappers.size() == index)
                  stopWrappers.push_back(NULL);
               if ((Integer)goalWrappers.size() == index)
                  goalWrappers.push_back(NULL);
            }
            else if (index < size)
            {
               stopWhen[index] = (StopCondition *)obj;
               stopSatNames[index] = satName;
            }
            else
            {
               MessageInterface::ShowMessage
                  ("Propagate::SetRefObject() index=%d is not next available "
                   "index=%d. Setting %s:%s failed\n", index, size,
                   obj->GetTypeName().c_str(), obj->GetName().c_str());
               return false;
            }
            
            #if DEBUG_PROPAGATE_OBJ
            MessageInterface::ShowMessage
               ("   stopWrappers.size() = %d, goalWrappers.size() = %d\n",
                stopWrappers.size(), goalWrappers.size());
            for (UnsignedInt  j=0; j<stopSatNames.size(); j++)
               MessageInterface::ShowMessage
                  ("   stopSatNames=%s\n", stopSatNames[j].c_str());
            #endif
            
            #ifdef DEBUG_STOPPING_CONDITIONS
               MessageInterface::ShowMessage(
                  "Adding stopping condition named %s\n",
                  obj->GetName().c_str());
            #endif

            stopCondEpochID = obj->GetParameterID("Epoch");
            stopCondBaseEpochID = obj->GetParameterID("BaseEpoch");
            stopCondStopVarID = obj->GetParameterID("StopVar");

            #if DEBUG_PROPAGATE_OBJ
            MessageInterface::ShowMessage("Propagate::SetRefObject() returning true\n");
            #endif
            return true;
         }

      default:
         break;
   }

   return PropagationEnabledCommand::SetRefObject(obj, type, name, index);
}

//------------------------------------------------------------------------------
// virtual ObjectArray& GetRefObjectArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers by type.
 *
 * @param type The type of objects requested.
 *
 * @return Reference to the array.
 */
//------------------------------------------------------------------------------
ObjectArray& Propagate::GetRefObjectArray(const UnsignedInt type)
{
   objectArray.clear();

   switch (type)
   {
      case Gmat::STOP_CONDITION:
         for (UnsignedInt i=0; i<stopWhen.size(); i++)
            objectArray.push_back(stopWhen[i]);
         return objectArray;

      default:
         break;
   }

   return PropagationEnabledCommand::GetRefObjectArray(type);
}


// Parameter accessor methods

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the description for the parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return String description for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Propagate::GetParameterText(const Integer id) const
{
   if ((id < PropagateCommandParamCount) && (id >= GmatCommandParamCount))
      return PARAMETER_TEXT[id - GmatCommandParamCount];

   return PropagationEnabledCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieve the ID for the parameter given its description.
 *
 * @param <str> Description for the parameter.
 *
 * @return the parameter ID.
 */
//------------------------------------------------------------------------------
Integer Propagate::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < PropagateCommandParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }

   return PropagationEnabledCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the enumerated type of the object.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The enumeration for the type of the parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Propagate::GetParameterType(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < PropagateCommandParamCount)
      return PARAMETER_TYPE[id - GmatCommandParamCount];

   return PropagationEnabledCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the string associated with a parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return Text description for the type of the parameter.
 */
//------------------------------------------------------------------------------
std::string Propagate::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for an Integer parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Integer Propagate::GetIntegerParameter(const Integer id) const
{
   if (id == INTERRUPT_FREQUENCY)
      return interruptCheckFrequency;

   return PropagationEnabledCommand::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Set the value for an Integer parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call.
 */
//------------------------------------------------------------------------------
Integer Propagate::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == INTERRUPT_FREQUENCY) {
      if (value >= 0)
         interruptCheckFrequency = value;
      return interruptCheckFrequency;
   }

   return PropagationEnabledCommand::SetIntegerParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The string stored for this parameter.
 */
//------------------------------------------------------------------------------
std::string Propagate::GetStringParameter(const Integer id) const
{
   if (id == PROP_COUPLED)
      return currentPropMode;

   return PropagationEnabledCommand::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const char *value)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored.
 */
//------------------------------------------------------------------------------
bool Propagate::SetStringParameter(const Integer id, const char *value)
{
   return SetStringParameter(id, std::string(value));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored.
 */
//------------------------------------------------------------------------------
bool Propagate::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == PROP_COUPLED)
   {
      const StringArray pmodes = GetStringArrayParameter(AVAILABLE_PROP_MODES);
      if (find(pmodes.begin(), pmodes.end(), value) != pmodes.end())
      {
         // Back prop is a special case
         if (value == "BackProp")
         {
            direction = -1.0;
         }
         else
         {
            currentPropMode = value;
            for (Integer i = 0; i < PropModeCount; ++i)
               if (value == pmodes[i])
               {
                  currentMode = (PropModes)i;
                  return true;
               }
         }
      }
   }

   if (id == SAT_NAME)
   {
      Integer propNum = propName.size()-1;
      satName[propNum]->push_back(value);
      return true;
   }

   if (id == PROP_NAME)
   {
      std::string propNameString = value;
      if (propNameString[0] == '-')
      {
         direction = -1.0;
         MessageInterface::ShowMessage("Please use the keyword \"BackProp\" to "
            "set backwards propagation; the use of a minus sign in the string "
            "\"%s\" is deprecated.\n", propNameString.c_str());
         propNameString = propNameString.substr(1);
      }
      propName.push_back(propNameString);
      StringArray *satNameArray = new StringArray;
      //satName.push_back(new StringArray);
      satName.push_back(satNameArray);
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (satNameArray, "satNameArray", "Propagate::SetStringParameter()",
          "*satNameArray = new StringArray");
      #endif
      return true;
   }

   return PropagationEnabledCommand::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new string for this parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool Propagate::SetStringParameter(const Integer id, const std::string &value,
                                   const Integer index)
{
   if (id == SAT_NAME) {
      if (index < (Integer)propName.size())
         satName[index]->push_back(value);
      else
         throw CommandException("Propagate::SetStringParameter Attempting to "
                         "assign a spacecraft without an associated PropSetup");
      return true;
   }

   return PropagationEnabledCommand::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& Propagate::GetStringArrayParameter(const Integer id) const
{
   static StringArray modeList;

   if (id == AVAILABLE_PROP_MODES) {
      modeList.clear();
      for (Integer i = 0; i < PropModeCount; ++i)
         // BackProp isn't really a prop sync mode
         if (PropModeList[i] != "BackProp")
            modeList.push_back(PropModeList[i]);
      return modeList;
   }

   if (id == SAT_NAME)
      return *satName[0];

   if (id == PROP_NAME) {
      return propName;
   }

   return PropagationEnabledCommand::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//                                            const Integer index) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param id The integer ID for the parameter.
 * @param index The index when multiple StringArrays are supported.
 *
 * @return The requested StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& Propagate::GetStringArrayParameter(const Integer id,
                                               const Integer index) const
{
   if (id == SAT_NAME)
      return *satName[index];

   return PropagationEnabledCommand::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
// Integer GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for an Boolean parameter.
 *
 * Propagate currently contains the following Boolean parameter:
 *
 *    PROP_FORWARDS     Evaluates true if the prop direction is forward in time,
 *                      false if propagating backwards.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
bool Propagate::GetBooleanParameter(const Integer id) const
{
   if (id == PROP_FORWARD)
      return (direction > 0.0 ? true : false);

   if (id == PROP_ALL_STM)
      return propAllSTMs;

   if (id == CALC_ALL_AMATRIX)
      return calcAllAmatrices;

   if (id == CALC_ALL_COVARIANCE)
      return calcAllCovariance;

   return PropagationEnabledCommand::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// Integer SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * Set the value for an Boolean parameter.
 *
 * Propagate currently contains the following Boolean parameter:
 *
 *    PROP_FORWARDS     Set true if the prop direction is forward in time,
 *                      false if propagating backwards.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The parameter setting
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
bool Propagate::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == PROP_FORWARD)
   {
      if (value == true)
         direction = 1.0;
      else
         direction = -1.0;

      for (UnsignedInt i=0; i<stopWhen.size(); i++)
      {
         if (stopWhen[i] != NULL)
         {
            #ifdef DEBUG_PROPAGATE_DIRECTION
            MessageInterface::ShowMessage
               ("Setting direction %f to StopCondition '%s'\n", direction,
                stopWhen[i]->GetName().c_str());
            #endif
            stopWhen[i]->SetPropDirection(direction);  // Use direction of props
         }
      }
      return value;
   }

   if (id == PROP_ALL_STM)
   {
      propAllSTMs = value;
      if (propAllSTMs == false)
      {
         // STM was intentionally turned off; remove it everywhere
         for (UnsignedInt i = 0; i < satName.size(); ++i)
         {
            StringArray *names = satName[i];

            #ifdef DEBUG_STM_AMATRIX_SETTINGS
               MessageInterface::ShowMessage("Before erasure:\n");
               for (UnsignedInt m = 0; m < names->size(); ++m)
                  MessageInterface::ShowMessage("   %s\n",names->at(m).c_str());
            #endif

            for (StringArray::iterator j = names->begin(); j != names->end(); ++j)
            {
               if ((*j) == "STM")
                  j = names->erase(j);
               if (j == names->end())
                  break;
            }

            #ifdef DEBUG_STM_AMATRIX_SETTINGS
               MessageInterface::ShowMessage("After erasure:\n");
               for (UnsignedInt m = 0; m < names->size(); ++m)
                  MessageInterface::ShowMessage("   %s\n",names->at(m).c_str());
            #endif
         }
      }
      return true;
   }

   if (id == CALC_ALL_AMATRIX)
   {
      calcAllAmatrices = value;
      if (calcAllAmatrices == false)
      {
         // A-Matrix was intentionally turned off; remove it everywhere
         for (UnsignedInt i = 0; i < satName.size(); ++i)
         {
            StringArray *names = satName[i];

            #ifdef DEBUG_STM_AMATRIX_SETTINGS
               MessageInterface::ShowMessage("Before erasure:\n");
               for (UnsignedInt m = 0; m < names->size(); ++m)
                  MessageInterface::ShowMessage("   %s\n",names->at(m).c_str());
            #endif

            for (StringArray::iterator j = names->begin(); j != names->end(); ++j)
            {
               if ((*j) == "AMatrix")
                  j = names->erase(j);
               if (j == names->end())
                  break;
            }

            #ifdef DEBUG_STM_AMATRIX_SETTINGS
               MessageInterface::ShowMessage("After erasure:\n");
               for (UnsignedInt m = 0; m < names->size(); ++m)
                  MessageInterface::ShowMessage("   %s\n",names->at(m).c_str());
            #endif
         }
      }
      return true;
   }

   if (id == CALC_ALL_COVARIANCE)
   {
      calcAllCovariance = value;
      if (calcAllCovariance == false)
      {
         // Covariance was intentionally turned off; remove it everywhere
         for (UnsignedInt i = 0; i < satName.size(); ++i)
         {
            StringArray *names = satName[i];

            #ifdef DEBUG_STM_AMATRIX_SETTINGS
               MessageInterface::ShowMessage("Before erasure:\n");
               for (UnsignedInt m = 0; m < names->size(); ++m)
                  MessageInterface::ShowMessage("   %s\n", names->at(m).c_str());
            #endif

            for (StringArray::iterator j = names->begin(); j != names->end(); ++j)
            {
               if ((*j) == "Covariance")
                  j = names->erase(j);
               if (j == names->end())
                  break;
            }

            #ifdef DEBUG_STM_AMATRIX_SETTINGS
               MessageInterface::ShowMessage("After erasure:\n");
               for (UnsignedInt m = 0; m < names->size(); ++m)
                  MessageInterface::ShowMessage("   %s\n", names->at(m).c_str());
            #endif
         }
      }
      return true;
   }

   return PropagationEnabledCommand::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// Integer GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for an Boolean parameter.
 *
 * Propagate currently contains the following Boolean label:
 *
 *    "PropForward"     Evaluates true if the prop direction is forward in time,
 *                      false if propagating backwards.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
bool Propagate::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Integer SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * Set the value for an Boolean parameter.
 *
 * Propagate currently contains the following Boolean parameter:
 *
 *    "PropForward"     Set true if the prop direction is forward in time,
 *                      false if propagating backwards.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The parameter setting
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
bool Propagate::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


Real Propagate::GetRealParameter(const Integer id) const
{
   if (id == STOP_ACCURACY)
      return stopAccuracy;
   return PropagationEnabledCommand::GetRealParameter(id);
}

Real Propagate::SetRealParameter(const Integer id, const Real value)
{
   if (id == STOP_ACCURACY)
   {
      if (value > 0.0)
      {
         stopAccuracy = value;
         timeAccuracy = value;
         firstStepTolerance = stopAccuracy * 10.0;
      }
      else
      {
         std::stringstream val;
         val.precision(13);
         val << value;
         CommandException ce;
         ce.SetDetails(errorMessageFormatUnnamed.c_str(),
            val.str().c_str(), "StopTolerance", "a Real number > 0.0");
         throw ce;
      }
      return stopAccuracy;
   }
   return PropagationEnabledCommand::SetRealParameter(id, value);
}

Real Propagate::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

Real Propagate::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// bool GetPropStatus()
//------------------------------------------------------------------------------
/**
* Method used to check whether propagation command is just starting or
* continuing a run based off inProgress parameter.
*
* @return inProgress Boolean parameter showing whether a propagation run is 
* starting or continuing.
*/
//------------------------------------------------------------------------------
bool Propagate::GetPropStatus()
{
   return inProgress;
}

//------------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * @param <action> The string descriptor for the requested action.
 * @param <actionData> Optional data used for the action.
 *
 * @return true if the action was performed, false if not.
 */
//------------------------------------------------------------------------------
bool Propagate::TakeAction(const std::string &action,
                           const std::string &actionData)
{
   #ifdef DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("Propagate::TakeAction() this=<%p> entered, action='%s', actionData='%s'\n",
       this, action.c_str(), actionData.c_str());
   #endif

   if (action == "Clear")
   {
      if (actionData == "Propagator")
      {
         for (Integer i = 0; i < (Integer)satName.size(); ++i)
         {
            delete satName[i];
         }
         satName.clear();

         propName.clear();

         for (std::vector<PropSetup*>::iterator ps = propagators.begin();
              ps != propagators.end(); ++ps)
         {
            PropSetup *oldPs = *ps;
            *ps = NULL;
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (oldPs, oldPs->GetName(), "Propagate::TakeAction()",
                "deleting old PropSetup");
            #endif
            delete oldPs;
         }
         propagators.clear();
         p.clear();
         fm.clear();

         sats.clear();
      }
      else if (actionData == "StopCondition")
      {
         stopWhen.clear();
         stopSats.clear();
         stopSatNames.clear();
         ClearWrappers();
         stopNames.clear();
         goalNames.clear();
         return true;
      }
   }
   else if (action == "SetStopSpacecraft")
   {
      stopSatNames.push_back(actionData);
      return true;
   }
   else if (action == "ResetLoopData")
   {
      resetLoopDataFlag = true;
//      for (std::vector<Propagator*>::iterator i = p.begin(); i != p.end(); ++i)
//      {
//         (*i)->ResetInitialData();
//      }
      return true;
   }
   else if (action == "IsInFunction")
   {
      if (GetCurrentFunction() == NULL)
         return false;
      else
         return true;
   }
   else if (action == "PrepareToPropagate")
   {
      PrepareToPropagate();
      return true;
   }

   return PropagationEnabledCommand::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool Propagate::RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Propagate::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif

   // Propagate needs to know about Spacecraft, Formation, PropSetup, Parameter
   if (type != Gmat::SPACECRAFT && type != Gmat::FORMATION &&
       type != Gmat::PROP_SETUP && type != Gmat::PARAMETER &&
       type != Gmat::COORDINATE_SYSTEM)
      return true;

   StringArray::iterator pos;

   if (type == Gmat::PROP_SETUP)
   {
      // rename PropSetup
      for (pos = propName.begin(); pos != propName.end(); ++pos)
         if (*pos == oldName)
            *pos = newName;
   }
   else
   {
      // rename space object name used in prop setup
      for (UnsignedInt prop = 0; prop < propName.size(); ++prop)
         for (pos = satName[prop]->begin(); pos != satName[prop]->end(); ++pos)
            if (*pos == oldName)
               *pos = newName;

      // rename space object name used in stopping condition
      for (UnsignedInt i = 0; i < stopSatNames.size(); ++i)
         if (stopSatNames[i] == oldName)
            stopSatNames[i] = newName;

      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("Propagate::RenameRefObject() Rename StopCondition Object\n");
      #endif

      // rename stop condition parameter
      for (UnsignedInt i=0; i<stopWhen.size(); i++)
         stopWhen[i]->RenameRefObject(type, oldName, newName);

      // Rename the wrappers and goal/stopNames as well
      unsigned int sz = goalNames.size();
      for (unsigned int ii = 0; ii < sz; ii++)
      {
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            ("Propagate::RenameRefObject() goalNames[%d] = %s\n",
             (Integer) ii, goalNames.at(ii).c_str());
         #endif
         if (goalWrappers.at(ii))
         {
            #ifdef DEBUG_RENAME
            MessageInterface::ShowMessage
               ("Propagate::RenameRefObject() there is a goalWrapper at %d\n", (Integer) ii);
            #endif
            goalWrappers.at(ii)->RenameObject(oldName, newName);
            if (goalNames.at(ii).find(oldName) != goalNames.at(ii).npos)
            {
               #ifdef DEBUG_RENAME
               MessageInterface::ShowMessage
                  ("   old name '%s' found in goalNames.at(ii): '%s', so replacing with '%s'\n",
                   oldName.c_str(), goalNames.at(ii).c_str(), newName.c_str());
               #endif

               goalNames.at(ii) = GmatStringUtil::ReplaceName(goalNames.at(ii), oldName, newName);
            }
         }
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            ("Propagate::RenameRefObject() UPDATED goalNames[%d] = %s\n",
             (Integer) ii, goalNames.at(ii).c_str());
         #endif
      }
      sz = stopNames.size();
      for (unsigned int ii = 0; ii < sz; ii++)
      {
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            ("Propagate::RenameRefObject() stopNames[%d] = %s\n",
             (Integer) ii, stopNames.at(ii).c_str());
         #endif
         if (stopWrappers.at(ii))
         {
            #ifdef DEBUG_RENAME
            MessageInterface::ShowMessage
               ("Propagate::RenameRefObject() there is a stopWrapper at %d\n", (Integer) ii);
            #endif
            stopWrappers.at(ii)->RenameObject(oldName, newName);
            if (stopNames.at(ii).find(oldName) != stopNames.at(ii).npos)
            {
               #ifdef DEBUG_RENAME
               MessageInterface::ShowMessage
                  ("   old name '%s' found in stopNames.at(ii): '%s', so replacing with '%s'\n",
                   oldName.c_str(), stopNames.at(ii).c_str(), newName.c_str());
               #endif

               stopNames.at(ii) = GmatStringUtil::ReplaceName(stopNames.at(ii), oldName, newName);
            }
         }
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            ("Propagate::RenameRefObject() UPDATED stopNames[%d] = %s\n",
             (Integer) ii, stopNames.at(ii).c_str());
         #endif
      }
   }

   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the Propagate.
 *
 * @return the list of object types.
 *
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& Propagate::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::PROP_SETUP);
   refObjectTypes.push_back(Gmat::SPACECRAFT);
   refObjectTypes.push_back(Gmat::PARAMETER);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref objects used by the Propagate.
 *
 * @param <type> The type of object desired, or Gmat::UNKNOWN_OBJECT for the
 *               full list.
 *
 * @return the list of object names.
 *
 */
//------------------------------------------------------------------------------
const StringArray& Propagate::GetRefObjectNameArray(const UnsignedInt type)
{
   #ifdef DEBUG_PROPAGATE_OBJ
   MessageInterface::ShowMessage
      ("Propagate::GetRefObjectNameArray() <%p> entered, type=%d\n", this, type);
   #endif

   refObjectNames.clear();

   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::PROP_SETUP)
   {
      // Remove backward prop notation '-'
      std::string newPropName;
      for (UnsignedInt i=0; i<propName.size(); i++)
      {
         newPropName = propName[i];
         if (newPropName[0] == '-')
            newPropName = propName[i].substr(1);

         refObjectNames.push_back(newPropName);
      }
   }

   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACECRAFT)
   {
      refObjectNames.insert(refObjectNames.end(), stopSatNames.begin(),
                            stopSatNames.end());
   }

   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::PARAMETER)
   {
      #ifdef DEBUG_PROPAGATE_OBJ
      MessageInterface::ShowMessage
         ("   The type is Parameter, stopNames.size()=%d, goalNames.size()=%d, "
          "stopWhen.size()=%d\n", stopNames.size(), goalNames.size(), stopWhen.size());
      #endif

      // Add LHS of stopping condition
      for (UnsignedInt i = 0; i < stopNames.size(); i++)
      {
         #ifdef DEBUG_PROPAGATE_OBJ
         MessageInterface::ShowMessage
            ("      stopName[%d]='%s'\n", i, stopNames[i].c_str());
         #endif

         if (!GmatStringUtil::IsNumber(stopNames[i]) &&
             find(refObjectNames.begin(), refObjectNames.end(), stopNames[i]) ==
             refObjectNames.end())
            refObjectNames.push_back(stopNames[i]);
      }

      // Add RHS of stopping condition
      for (UnsignedInt i = 0; i < goalNames.size(); i++)
      {
         #ifdef DEBUG_PROPAGATE_OBJ
         MessageInterface::ShowMessage
            ("      goalName[%d]='%s'\n", i, goalNames[i].c_str());
         #endif

         if (!GmatStringUtil::IsNumber(goalNames[i]) &&
             find(refObjectNames.begin(), refObjectNames.end(), goalNames[i]) ==
             refObjectNames.end())
            refObjectNames.push_back(goalNames[i]);
      }

      // Add StopCondition parameters
      for (UnsignedInt i = 0; i < stopWhen.size(); i++)
      {
         StringArray refNames = stopWhen[i]->GetRefObjectNameArray(Gmat::PARAMETER);
         for (UnsignedInt j = 0; j < refNames.size(); j++)
         {
            #ifdef DEBUG_PROPAGATE_OBJ
            MessageInterface::ShowMessage
               ("      refNames[%d]='%s'\n", j, refNames[j].c_str());
            #endif

            if (find(refObjectNames.begin(), refObjectNames.end(), refNames[j]) ==
                refObjectNames.end())
               refObjectNames.push_back(refNames[j]);
         }
      }
   }
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::COORDINATE_SYSTEM)
   {
      #ifdef DEBUG_PROPAGATE_OBJ
      MessageInterface::ShowMessage
         ("   The type is CoordinateSystem, stopNames.size()=%d, goalNames.size()=%d, "
          "stopWhen.size()=%d\n", stopNames.size(), goalNames.size(), stopWhen.size());
      #endif

//      // Add LHS of stopping condition
//      for (UnsignedInt i = 0; i < stopNames.size(); i++)
//      {
//         #ifdef DEBUG_PROPAGATE_OBJ
//         MessageInterface::ShowMessage
//            ("      stopName[%d]='%s'\n", i, stopNames[i].c_str());
//         #endif
//
//         if (!GmatStringUtil::IsNumber(stopNames[i]) &&
//             find(refObjectNames.begin(), refObjectNames.end(), stopNames[i]) ==
//             refObjectNames.end())
//            refObjectNames.push_back(stopNames[i]);
//      }
//
//      // Add RHS of stopping condition
//      for (UnsignedInt i = 0; i < goalNames.size(); i++)
//      {
//         #ifdef DEBUG_PROPAGATE_OBJ
//         MessageInterface::ShowMessage
//            ("      goalName[%d]='%s'\n", i, goalNames[i].c_str());
//         #endif
//
//         if (!GmatStringUtil::IsNumber(goalNames[i]) &&
//             find(refObjectNames.begin(), refObjectNames.end(), goalNames[i]) ==
//             refObjectNames.end())
//            refObjectNames.push_back(goalNames[i]);
//      }


      // Add StopCondition coordinate systems
      for (UnsignedInt i = 0; i < stopWhen.size(); i++)
      {
         StringArray refNames = stopWhen[i]->GetRefObjectNameArray(Gmat::COORDINATE_SYSTEM);
         for (UnsignedInt j = 0; j < refNames.size(); j++)
         {
            #ifdef DEBUG_PROPAGATE_OBJ
            MessageInterface::ShowMessage
               ("      refNames[%d]='%s'\n", j, refNames[j].c_str());
            #endif

            if (find(refObjectNames.begin(), refObjectNames.end(), refNames[j]) ==
                refObjectNames.end())
               refObjectNames.push_back(refNames[j]);
         }
      }
   }

   return refObjectNames;
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Propagate command has the following syntax:
 *
 *     Propagate prop(Sat1, ... , {Sat1.ElapsedDays = 700}) ...;
 *
 * where prop is a PropSetup, "Sat1, ... ," is the list of SpaceObjects that are
 * propagated, and the items in curly braces are the (optional) stopping
 * conditions.  The Propagate command supports simultaneous propagation of
 * multiple spacecraft, either in a single PropSetup or in a list of PropSetups
 * on the same Propagate command line.
 *
 * This method breaks the script line into the corresponding pieces, and stores
 * the names of the PropSetups, SpaceObjects, and StoppingConditions so they can
 * be set to point to the correct objects during initialization.
 *
 * @return true on successful parsing of the command.
 */
//------------------------------------------------------------------------------
bool Propagate::InterpretAction()
{
   #ifdef DEBUG_PROPAGATE_ASSEMBLE
   MessageInterface::ShowMessage
      ("\nPropagate::InterpretAction() entered, genString = \"%s\"\n",
       generatingString.c_str());
   #endif

   Integer loc = generatingString.find("Propagate", 0) + 9;
   const char *str = generatingString.c_str();

   if (generatingString.find("..") != generatingString.npos)
      throw CommandException("Propagate::InterpretAction: Can not parse "
            "command\n " + generatingString);

   // Verify bracket/parentheses matching
   Integer parenCount[2] = {0,0}, bracketCount[2] = {0,0};
   for (UnsignedInt i = 0; i < strlen(str); ++i)
   {
      if (str[i] == '(')
         ++parenCount[0];
      if (str[i] == ')')
         ++parenCount[1];

      if (str[i] == '{')
         ++bracketCount[0];
      if (str[i] == '}')
         ++bracketCount[1];
   }
   std::string errmsg;
   if (parenCount[0] != parenCount[1])
      errmsg = "Parentheses are mismatched";
   if (bracketCount[0] != bracketCount[1])
   {
      if (errmsg.size() > 0)
         errmsg += " and ";
      errmsg += "Brackets are mismatched";
   }
   if (errmsg.length() > 0)
      throw CommandException(errmsg);

   while (str[loc] == ' ')
      ++loc;

   // Check to see if there are optional parameters (e.g. "Synchronized")
   CheckForOptions(loc, generatingString);
   // Now fill in the list of propagators
   AssemblePropagators(loc, generatingString);

   if (propName.size() == 0)
      throw CommandException("A Propagate command is not valid: no "
            "propagators are identified");


   // Load up the array listing the objects referenced so they can be validated
   objects.clear();
   StringArray satList, satDuplicates;

   #ifdef DEBUG_PROPSETUP_NAMES
      MessageInterface::ShowMessage("PropSetup Names:");
   #endif

   for (UnsignedInt i = 0; i < propName.size(); ++i)
   {
      if ((propName[i].c_str())[0] == '-')
         propName[i] = propName[i].substr(1);

      #ifdef DEBUG_PROPSETUP_NAMES
         MessageInterface::ShowMessage("   %s\n", propName[i].c_str());
      #endif

      objects.push_back(propName[i]);

      #ifdef DEBUG_PROPAGATE_ASSEMBLE
         MessageInterface::ShowMessage("Satellites to propagate:\n");
      #endif
      for (UnsignedInt j = 0; j < satName[i]->size(); ++j)
      {
         // todo: Point fix for STM and A-Matrix.
         /** @todo: This point fix needs to be generalized so that a list of
          *         keywords isn't maintained here.
          */
         if (((*satName[i])[j] != "STM") && ((*satName[i])[j] != "AMatrix") && ((*satName[i])[j] != "Covariance"))
         {
            #ifdef DEBUG_PROPAGATE_ASSEMBLE
               MessageInterface::ShowMessage("  [%d][%d] = %s\n", i, j,
                     (*satName[i])[j].c_str());
            #endif
            objects.push_back((*satName[i])[j]);
            satList.push_back((*satName[i])[j]);
         }
         else
         {
            #ifdef DEBUG_PROPAGATE_ASSEMBLE
               MessageInterface::ShowMessage("Setting globally for %s\n",
                     (*satName[i])[j].c_str());
            #endif
            if ((*satName[i])[j] == "STM")
               propAllSTMs = true;
            if ((*satName[i])[j] == "AMatrix")
               calcAllAmatrices = true;
            if ((*satName[i])[j] == "Covariance")
            {
               if(propAllSTMs == false)
                  propAllSTMs = true;
               calcAllCovariance = true;
            }
         }
      }
   }
   
   // Look for repeated spacecraft names in list (will miss formation members)
   for (UnsignedInt i = 0; i < satList.size(); ++i)
   {
      std::string currentSat = satList[i];
      for (UnsignedInt j = i+1; j < satList.size(); ++j)
      {
         if (currentSat == satList[j])
            satDuplicates.push_back(satList[j]);
      }
   }
   
   if (satDuplicates.size() > 0)
      throw CommandException("Duplicate Spacecraft names in a single Propagate "
            "line are not allowed");
   
   // Now OrbitColot is specified inside brackets without stopping conditions
   // so check it (LOJ: 2013.12.16)
   if ((bracketCount[0] > 0) && (stopNames.size() == 0))
   {
      if (!overrideSegmentColor)
         throw CommandException("Brackets for stopping conditions were found, "
                                "but no stopping conditions detected");
   }
   
   #ifdef DEBUG_PROPAGATE_ASSEMBLE
   MessageInterface::ShowMessage
      ("Propagate::InterpretAction() returning true for genString = \"%s\"\n",
       generatingString.c_str());
   #endif
   return true;
}

//------------------------------------------------------------------------------
// bool AcceptsObjectType(UnsignedInt theType)
//------------------------------------------------------------------------------
/**
 * Checks to see if a specific type is supported by the command instance
 *
 * @param theType The object type being checked
 *
 * @return true if the type is accepted, false if not
 */
//------------------------------------------------------------------------------
bool Propagate::AcceptsObjectType(UnsignedInt theType)
{
   bool retval = true;

   if (theType == Gmat::FORMATION)
   {
      if (propAllSTMs || calcAllAmatrices || calcAllCovariance)
      {
         std::string errmsg = "**** Error in Propagate configuration: "
                  "Formations cannot be propagated when ";
         std::string conjunction;

         if (propAllSTMs)
         {
            errmsg += "propagating the State Transition Matrix ";
            conjunction = "and ";
         }
         if (calcAllAmatrices)
         {
            errmsg += conjunction;
            errmsg += "calculating the A-Matrix";
         }
         if (calcAllCovariance)
         {
            errmsg += conjunction;
            errmsg += "calculating the Covariance";
         }
         errmsg += " ****";
         throw CommandException(errmsg);
      }
   }

   return retval;
}

//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
const StringArray& Propagate::GetWrapperObjectNameArray(bool completeSet)
{
   wrapperObjectNames.clear();
   wrapperObjectNames.insert(wrapperObjectNames.end(), stopNames.begin(),
                             stopNames.end());
   wrapperObjectNames.insert(wrapperObjectNames.end(), goalNames.begin(),
                             goalNames.end());
   return wrapperObjectNames;
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
//------------------------------------------------------------------------------
bool Propagate::SetElementWrapper(ElementWrapper *toWrapper,
                                  const std::string &withName)
{
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("Propagate::SetElementWrapper() entered with toWrapper=<%p>, withName='%s'\n",
       toWrapper, withName.c_str());
   #endif

   if (toWrapper == NULL)
      return false;

   // this would be caught by next part, but this message is more meaningful
   if (toWrapper->GetWrapperType() == Gmat::ARRAY_WT)
   {
      throw CommandException("A value of type \"Array\" on command \"" + typeName +
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ]. ");
   }

   CheckDataType(toWrapper, Gmat::REAL_TYPE, "Propagate", true);

   bool retval = false;
   ElementWrapper *ew;

   //-------------------------------------------------------
   // check stopping condition names
   //-------------------------------------------------------
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("   Checking %d Propagate Stop Conditions\n", stopNames.size());
   for (UnsignedInt i=0; i<stopNames.size(); i++)
      MessageInterface::ShowMessage("      %s\n", stopNames[i].c_str());
   MessageInterface::ShowMessage("   There are %d stopWrappers\n", stopWrappers.size());
   for (UnsignedInt i=0; i<stopWrappers.size(); i++)
      MessageInterface::ShowMessage
         ("      <%p>'%s'\n", stopWrappers[i], stopWrappers[i] ?
          stopWrappers[i]->GetDescription().c_str() : "NULL");
   #endif

   WrapperArray wrappersToDelete;
   Integer sz = stopNames.size();

   for (Integer i = 0; i < sz; i++)
   {
      if (stopNames.at(i) == withName)
      {
         #ifdef DEBUG_WRAPPERS
         MessageInterface::ShowMessage
            ("   Found wrapper name \"%s\" in stopNames\n", withName.c_str());
         #endif

         for (UnsignedInt j=0; j<stopWhen.size(); j++)
         {
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("   stopWhen[j]->GetName()='%s'\n", stopWhen[j]->GetName().c_str());
            #endif
            //if (stopWhen[j]->GetName() == ("StopOn" + withName))
            if (stopWhen[j]->GetLhsString() == withName)
            {
               stopWhen[j]->SetStopParameter((Parameter*)toWrapper->GetRefObject());
               stopWhen[j]->SetLhsWrapper(toWrapper);
            }
         }

         if (stopWrappers.at(i) != NULL)
         {
            ew = stopWrappers.at(i);
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("   Replacing stopWrapper[%d] <%p> with <%p>\n", i, ew, toWrapper);
            #endif
            stopWrappers.at(i) = toWrapper;

            // Delete old wrapper if wrapper name not found in the goalNames
            if (find(goalNames.begin(), goalNames.end(), withName) == goalNames.end())
               if (find(wrappersToDelete.begin(), wrappersToDelete.end(), ew) == wrappersToDelete.end())
                  wrappersToDelete.push_back(ew);
         }
         else
         {
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("   Setting wrapper <%p> to stopWrappers[%d]\n", toWrapper, i);
            #endif
            stopWrappers.at(i) = toWrapper;
         }
         retval = true;
      }
   }

   //-------------------------------------------------------
   // check goal names
   //-------------------------------------------------------
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("   Checking %d Propagate Stop Goals\n", goalNames.size());
   for (UnsignedInt i=0; i<goalNames.size(); i++)
      MessageInterface::ShowMessage("      %s\n", goalNames[i].c_str());
   MessageInterface::ShowMessage("   There are %d goalWrappers\n", goalWrappers.size());
   for (UnsignedInt i=0; i<goalWrappers.size(); i++)
      MessageInterface::ShowMessage
         ("      <%p>'%s'\n", goalWrappers[i], goalWrappers[i] ?
          goalWrappers[i]->GetDescription().c_str() : "NULL");
   #endif

   sz = goalNames.size();

   for (Integer i = 0; i < sz; i++)
   {
      if (goalNames.at(i) == withName)
      {
         #ifdef DEBUG_WRAPPERS
         MessageInterface::ShowMessage
            ("   Found wrapper name \"%s\" in goalNames\n", withName.c_str());
         #endif

         for (UnsignedInt j=0; j<stopWhen.size(); j++)
         {
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("   stopWhen[j]->GetName()='%s'\n", stopWhen[j]->GetName().c_str());
            #endif
            if (stopWhen[j]->GetRhsString() == withName)
            {
               stopWhen[j]->SetRhsWrapper(toWrapper);
            }
         }

         if (goalWrappers.at(i) != NULL)
         {
            ew = goalWrappers.at(i);
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("   Replacing goalWrapper[%d] <%p> with <%p>\n", i, ew, toWrapper);
            #endif
            goalWrappers.at(i) = toWrapper;
            
            // Delete old wrapper if wrapper name not found in the stopNames
            if (find(stopNames.begin(), stopNames.end(), withName) == stopNames.end())
               if (find(wrappersToDelete.begin(), wrappersToDelete.end(), ew) == wrappersToDelete.end())
                  wrappersToDelete.push_back(ew);
         }
         else
         {
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("   Setting wrapper <%p> to goalWrappers[%d]\n", toWrapper, i);
            #endif
            goalWrappers.at(i) = toWrapper;
         }
         retval = true;
      }
   }
   
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("   There are %d wrappers to delete\n", wrappersToDelete.size());
   for (UnsignedInt i=0; i<wrappersToDelete.size(); i++)
      MessageInterface::ShowMessage
         ("      <%p>'%s'\n", wrappersToDelete[i], wrappersToDelete[i] ?
          wrappersToDelete[i]->GetDescription().c_str() : "NULL");
   #endif
   
   // delete old wrappers (loj: 2008.12.15)
   for (WrapperArray::iterator ewi = wrappersToDelete.begin();
        ewi < wrappersToDelete.end(); ewi++)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         ((*ewi), (*ewi)->GetDescription(), "Propagate::SetElementWrapper()",
          "deleting wrapper");
      #endif
      delete (*ewi);
      (*ewi) = NULL;
   }

   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("Propagate::SetElementWrapper() returning %s\n", retval ? "true" : "false");
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// void ClearWrappers()
//------------------------------------------------------------------------------
/*
 * Deletes element wrappers and sets them to NULL without emptying the array.
 */
//------------------------------------------------------------------------------
void Propagate::ClearWrappers()
{
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("Propagate::ClearWrappers() entered, stopNames.size()=%d, stopWrappers.size()=%d, "
       "goalNames.size()=%d, goalWrappers.size()=%d\n", stopNames.size(), stopWrappers.size(),
       goalNames.size(), goalWrappers.size());
   #endif

   WrapperArray wrappersToDelete;
   ElementWrapper *ew;
   
   Integer sz = stopWrappers.size();
   for (Integer i = 0; i < sz; i++)
   {
      ew = stopWrappers.at(i);
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("   stopWrapper[%d] = <%p>'%s'\n", i, ew, ew ? ew->GetDescription().c_str() : "NULL");
      #endif
      if (stopWrappers.at(i) != NULL)
      {
         stopWrappers.at(i) = NULL;
         if (find(wrappersToDelete.begin(), wrappersToDelete.end(), ew) ==
             wrappersToDelete.end())
         {
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("   Adding stopWrapper: <%p>'%s' to delete\n", ew, ew->GetDescription().c_str());
            #endif
            wrappersToDelete.push_back(ew);
         }
      }
   }
   
   
   sz = goalWrappers.size();
   for (Integer i = 0; i < sz; i++)
   {
      ew = goalWrappers.at(i);
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("   goalWrapper[%d] = <%p>'%s'\n", i, ew, ew ? ew->GetDescription().c_str() : "NULL");
      #endif
      if (goalWrappers.at(i) != NULL)
      {
         goalWrappers.at(i) = NULL;
         if (find(wrappersToDelete.begin(), wrappersToDelete.end(), ew) ==
             wrappersToDelete.end())
         {
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("   Adding goalWrapper: <%p>'%s' to delete\n", ew, ew->GetDescription().c_str());
            #endif
            wrappersToDelete.push_back(ew);
         }
      }
   }
   
   
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("There are %d wrappers to delete\n", wrappersToDelete.size());
   #endif
   
   // delete old wrappers
   for (WrapperArray::iterator ewi = wrappersToDelete.begin();
        ewi < wrappersToDelete.end(); ewi++)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         ((*ewi), (*ewi)->GetDescription(), "Propagate::ClearWrappers()",
          "deleting wrapper");
      #endif
      delete (*ewi);
      (*ewi) = NULL;
   }

   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("Propagate::ClearWrappers() leaving, stopNames.size()=%d, stopWrappers.size()=%d, "
       "goalNames.size()=%d, goalWrappers.size()=%d\n", stopNames.size(), stopWrappers.size(),
       goalNames.size(), goalWrappers.size());
   #endif
}

//------------------------------------------------------------------------------
// virtual void SetSegmentOrbitColor(UnsignedInt &intColor)
//------------------------------------------------------------------------------
void Propagate::SetSegmentOrbitColor(UnsignedInt &intColor)
{
   segmentOrbitColor = intColor;
   segmentOrbitColorStr = RgbColor::ToRgbString(intColor);
}

//------------------------------------------------------------------------------
// virtual UnsignedInt GetSegmentOrbitColor()
//------------------------------------------------------------------------------
UnsignedInt Propagate::GetSegmentOrbitColor()
{
   return segmentOrbitColor;
}

//------------------------------------------------------------------------------
// virtual std::string GetFirstSpaceObjectName()
//------------------------------------------------------------------------------
std::string Propagate::GetFirstSpaceObjectName()
{
   #ifdef DEBUG_FIRST_SO
   MessageInterface::ShowMessage
      ("Propagate::GetFirstSpaceObjectName() entered, satName.size() = %d\n", satName.size());
   #endif
   std::string firstObjName;
   
   if (satName.size() > 0)
   {
      if (satName[0]->size() > 0)
         firstObjName = satName[0]->at(0);
   }
   
   #ifdef DEBUG_FIRST_SO
   MessageInterface::ShowMessage
      ("Propagate::GetFirstSpaceObjectName() returning '%s'\n", firstObjName.c_str();
   #endif
   return firstObjName;
}

//------------------------------------------------------------------------------
// virtual void SetOverrideSegmentColor(bool override)
//------------------------------------------------------------------------------
void Propagate::SetOverrideSegmentColor(bool override)
{
   overrideSegmentColor = override;
}

//------------------------------------------------------------------------------
// virtual bool GetOverrideSegmentColor()
//------------------------------------------------------------------------------
bool Propagate::GetOverrideSegmentColor()
{
   return overrideSegmentColor;
}

//------------------------------------------------------------------------------
// void CheckForOptions(Integer &loc, std::string &generatingString)
//------------------------------------------------------------------------------
/**
 * Looks for propagator options that exist prior to any PropSetup names.
 *
 * @param <loc>               The current location in the generating string.
 * @param <generatingString>  The generating string.
 */
//------------------------------------------------------------------------------
void Propagate::CheckForOptions(Integer &loc, std::string &generatingString)
{
   std::string modeStr;
   currentMode = INDEPENDENT;

   #ifdef DEBUG_PROPAGATE_ASSEMBLE
      MessageInterface::ShowMessage("Propagate::CheckForOptions(%d, %s) "
            "entered\n", loc, generatingString.c_str());
   #endif

   Integer maxLoc = loc;


   for (Integer modeId = INDEPENDENT+1; modeId != PropModeCount; ++modeId)
   {
      modeStr = PropModeList[modeId];
      modeStr += " ";

      #ifdef DEBUG_PROPAGATE_ASSEMBLE
         MessageInterface::ShowMessage("Propagate::CheckForOptions() looking"
                                " for \"%s\" starting at loc=%d in \n\"%s\"",
                                modeStr.c_str(), loc, generatingString.c_str());
      #endif

      Integer end = generatingString.find(modeStr, loc);
      if (end != (Integer)std::string::npos)
      {
         if (modeStr == "BackProp ")
         {
            direction = -1.0;

            #ifdef DEBUG_PROPAGATE_ASSEMBLE
               MessageInterface::ShowMessage("\nDirection is now %d\n", direction);
            #endif
         }
         else
         {
            currentMode = (PropModes)modeId;
            currentPropMode = PropModeList[modeId];
            #ifdef DEBUG_PROPAGATE_ASSEMBLE
               MessageInterface::ShowMessage("\nLocated at %d\n", end);
               MessageInterface::ShowMessage("Mode is now %d\n", currentMode);
            #endif
         }

         if (end >= maxLoc)
            maxLoc = end + modeStr.length();
      }
      else
      {
         #ifdef DEBUG_PROPAGATE_ASSEMBLE
         MessageInterface::ShowMessage("\n");
         #endif
      }
   }

   loc = maxLoc;
}


//------------------------------------------------------------------------------
// void AssemblePropagators(Integer &loc, std::string& generatingString)
//------------------------------------------------------------------------------
/**
 * Parses the PropSetup portion of the Propagate command.
 *
 * @param <loc>               The current location in the generating string.
 * @param <generatingString>  The generating string.
 */
//------------------------------------------------------------------------------
void Propagate::AssemblePropagators(Integer &loc,
   std::string& generatingString)
{
   // First parse the pieces from the string, starting at loc
   StringArray setupStrings, stopStrings;

   FindSetupsAndStops(loc, generatingString, setupStrings, stopStrings);

   #ifdef DEBUG_PROPAGATE_ASSEMBLE
      // Output the chunks for debugging
      MessageInterface::ShowMessage("PropSetups:\n");
      for (StringArray::iterator i = setupStrings.begin();
           i != setupStrings.end(); ++i)
         MessageInterface::ShowMessage("   '%s'\n", i->c_str());
      MessageInterface::ShowMessage("StopConditions:\n");
      for (StringArray::iterator i = stopStrings.begin();
           i != stopStrings.end(); ++i)
         MessageInterface::ShowMessage("   '%s'\n", i->c_str());
   #endif

   // Now build the prop setups
   for (StringArray::iterator i = setupStrings.begin();
        i != setupStrings.end(); ++i)
      ConfigurePropSetup(*i);

   // and the stopping conditions
   for (StringArray::iterator i = stopStrings.begin();
        i != stopStrings.end(); ++i)
      ConfigureStoppingCondition(*i);

   // Finally, set the prop mode
   if (stopWhen.empty())
      singleStepMode = true;  // If not, run in single step mode
}


//------------------------------------------------------------------------------
// void Propagate::FindSetupsAndStops(Integer &loc,
//   std::string& generatingString, StringArray &setupStrings,
//   StringArray &stopStrings)
//------------------------------------------------------------------------------
/**
 * Breaks out the PropSetup object strings and the stopping condition strings.
 *
 * @param <loc>               The current location in the generating string.
 * @param <generatingString>  The generating string.
 * @param <setupStrings>      The container for the PropSetup strings.
 * @param <stopStrings>       The container for teh stopping condition strings.
 */
//------------------------------------------------------------------------------
void Propagate::FindSetupsAndStops(Integer &loc,
   std::string& generatingString, StringArray &setupStrings,
   StringArray &stopStrings)
{
   //=================================================================
   #ifdef __NOT_USING_TEXTPARSER__
   //=================================================================
   #ifdef DEBUG_PARSING
   MessageInterface::ShowMessage
      ("Propagate::FindSetupsAndStops() entered, NOT Using the TextParser...\n");
   #endif
   // First parse the pieces from the string, starting at loc
   std::string tempString, setupWithStop, oneStop;
   const char *str = generatingString.c_str();
   Integer currentLoc = loc, parmstart, end, commaLoc;

   bool scanning = true;

   // First find the PropSetups
   parmstart = generatingString.find("(", currentLoc);
   while (scanning)
   {
      //end = generatingString.find(")", parmstart)+1;
      end = generatingString.find(")", parmstart);

      if (end == (Integer)std::string::npos)
         throw CommandException("Propagate::AssemblePropagators: Propagate"
                                " string does not identify propagator");
      ++end;

      if (generatingString[currentLoc] == '-')
      {
         direction = -1.0;
         MessageInterface::ShowMessage("Please use the keyword \"BackProp\" to "
            "set backwards propagation; the use of a minus sign in the string "
            "\"%s\" is deprecated.\n", generatingString.c_str());
         ++currentLoc;
      }

      tempString = generatingString.substr(currentLoc, end-currentLoc);
      // Remove stop condition here
      if (tempString.find("{", 0) != std::string::npos)
      {
         setupWithStop = tempString;

         Integer braceStart = setupWithStop.find("{", 0),
                 braceEnd   = setupWithStop.find("}", 0);

         if (braceEnd == (Integer)std::string::npos)
            throw CommandException("Propagate::AssemblePropagators: PropSetup"
                                  " string " + tempString +
                                  " starts a stopping condition, but does not"
                                  " have a closing brace.");
         // Now remove the bracketed chunk from the string
         tempString = setupWithStop.substr(0, braceStart);
         // Remove the comma
         Integer commaLoc = braceStart - 1;
         while ((tempString[commaLoc] == ',') || (tempString[commaLoc] == ' '))
         {
            --commaLoc;
         }
         tempString = tempString.substr(0, commaLoc+1);

         // Add on the trailing chunk
         tempString += setupWithStop.substr(braceEnd+1);
      }

      setupStrings.push_back(tempString);
      currentLoc = end; // +1;  Valgrind fix from Joris Olympio

      // Skip trailing comma or white space
      while ((str[currentLoc] == ',') || (str[currentLoc] == ' '))
         ++currentLoc;
      parmstart = generatingString.find("(", currentLoc);
      if (parmstart == (Integer)std::string::npos)
         scanning = false;
   }

   // Now find the stopping conditions
   scanning = true;
   currentLoc = loc;

   parmstart = generatingString.find("{", currentLoc);
   if ((std::string::size_type)parmstart == std::string::npos)
      scanning = false;

   while (scanning)
   {
      end = generatingString.find("}", parmstart)+1;

      if (end == (Integer)std::string::npos+1)
      {
         throw CommandException("Propagate::AssemblePropagators: PropSetup"
                                  " string " + generatingString +
                                  " starts a stopping condition, but does not"
                                  " have a closing brace.");
      }

      tempString = generatingString.substr(parmstart+1, end-parmstart-2);

      // Split out stops, one at a time
      currentLoc = 0;
      do
      {
         commaLoc = tempString.find(",", currentLoc);
         oneStop = tempString.substr(currentLoc, commaLoc - currentLoc);
         // Remove leading white space
         while (oneStop[0] == ' ')
            oneStop = oneStop.substr(1);
         // Remove trailing white space
         currentLoc = oneStop.length() - 1;
         while (oneStop[currentLoc] == ' ')
            --currentLoc;
         oneStop = oneStop.substr(0, currentLoc+1);
         stopStrings.push_back(oneStop);

         currentLoc = commaLoc + 1;
      } while (commaLoc != (Integer)std::string::npos);

      currentLoc = end; // +1;  Valgrind fix from Joris Olympio

      // Skip trailing comma or white space
      while ((str[currentLoc] == ',') || (str[currentLoc] == ' '))
         ++currentLoc;
      parmstart = generatingString.find("{", currentLoc);
      if (parmstart == (Integer)std::string::npos)
         scanning = false;
   }

   //=================================================================
   #else
   //=================================================================

   #ifdef DEBUG_PARSING
   MessageInterface::ShowMessage
      ("Propagate::FindSetupsAndStops() entered, Using the TextParser...\n");
   #endif
   
   TextParser tp;
   StringArray chunks;
   std::string str1 = generatingString.substr(loc);
   // Remove all blank spaces
   // Cannot remove blanks since OrbitColor can have triplet value, e.g [0 255 0]
   // So remove blanks after }
   //str1 = GmatStringUtil::RemoveAll(str1, ' ');
   std::string::size_type closeBrace = str1.find_last_of("}");
   str1 = GmatStringUtil::RemoveAll(str1, ' ', closeBrace);
   
   std::string str2;
   
   #ifdef DEBUG_PARSING
   MessageInterface::ShowMessage("str1 = '%s'\n", str1.c_str());
   #endif

   chunks = GmatStringUtil::SeparateBy(str1, ")", true, true, false);

   for (UnsignedInt i = 0; i < chunks.size(); i++)
   {
      str2 = chunks[i];
      #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("str2 = '%s'\n", str2.c_str());
      #endif

      std::string::size_type lastCloseParen = str2.find_last_of(")");
      
      // If syntax is "Propagate Prop(Sat1, {Sat1.ElapsedSecs = 600})",
      // make it to standard syntax, i.e "Propagate Prop(Sat1) {Sat1.ElapsedSecs = 600}"
      // to use TextParser::SeparateAllBrackets(str, "{})
      // Remove last ) after }
      if (lastCloseParen == (str2.size() - 1) && str2[lastCloseParen - 1] == '}')
      {
         // Remove last )
         str2 = GmatStringUtil::RemoveLastString(str2, ")");

         // Replace last comma before { with )
         std::string::size_type openBrace = str2.find("{");
         std::string::size_type lastComma = str2.find_last_of(",", openBrace);
         if (lastComma != str2.npos && str2[lastComma-1] != ')')
            str2[lastComma] = ')';
         else if (lastComma != str2.npos)
            str2.erase(lastComma, 1);

         #ifdef DEBUG_PARSING
         MessageInterface::ShowMessage("str2 = '%s'\n", str2.c_str());
         #endif
      }

      StringArray parts = tp.SeparateAllBrackets(str2, "{}");

      #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage
         ("Now separate propagator setups and stop conditions\n");
      #endif

      for (UnsignedInt i = 0; i < parts.size(); i++)
      {
         #ifdef DEBUG_PARSING
            MessageInterface::ShowMessage("   parts[%d] = '%s'\n", i,
                  parts[i].c_str());
         #endif

         // If it does not starts with {, it is propagator and spacecraft
         if (parts[i][0] != '{')
         {
            parts[i] = GmatStringUtil::Trim(parts[i]);
            #ifdef DEBUG_PARSING
            MessageInterface::ShowMessage("   adding prop setup '%s'\n",
                  parts[i].c_str());
            #endif
            setupStrings.push_back(parts[i]);
         }
         else
         {
            #ifdef DEBUG_PARSING
            MessageInterface::ShowMessage("   adding stop conditions\n");
            #endif
            
            // Check for empty string between commas (LOJ: 2013.12.24)
            std::string tempPart = GmatStringUtil::RemoveAll(parts[i], ' ');
            //if (parts[i].find(",,") != std::string::npos)
            if (tempPart.find(",,") != std::string::npos)
               throw CommandException("Stopping condition parsing error; is "
                     "there an extra comma?");
            
            StringArray tempStops = tp.SeparateBrackets(parts[i], "{}", ",", true);
            // Add only stop conditions, skip OrbitColor (LOJ: 2013.12.13)
            //copy(tempStops.begin(), tempStops.end(), back_inserter(stopStrings));
            for (UnsignedInt i = 0; i < tempStops.size(); i++)
            {
               #ifdef DEBUG_PARSING
               MessageInterface::ShowMessage("   tempStops[%d] = %s\n", i, tempStops[i].c_str());
               #endif
               if (tempStops[i].find("OrbitColor") == std::string::npos)
                  stopStrings.push_back(tempStops[i]);
               else
                  ParseSegmentColor(tempStops[i]);
            }
         }
      }
   }
   
   //=================================================================
   #endif
   //=================================================================
   
   #ifdef DEBUG_PARSING
   MessageInterface::ShowMessage("Propagate::FindSetupsAndStops() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void ParseSegmentColor(const std::string &colorString)
//------------------------------------------------------------------------------
void Propagate::ParseSegmentColor(const std::string &colorString)
{
   #ifdef DEBUG_COLOR
   MessageInterface::ShowMessage
      ("Propagate::ParseSegmentColor() entered, colorString = '%s'\n", colorString.c_str());
   #endif
   std::string colorStr = colorString;
   
   // Parse color value and check errors
   StringArray parts = GmatStringUtil::SeparateByComma(colorString);
   if (parts.size() > 1)
      colorStr = parts[0];
   
   StringArray colorParts = GmatStringUtil::DecomposeBy(colorStr, "=");
   #ifdef DEBUG_COLOR
   for (UnsignedInt i = 0; i < colorParts.size(); i++)
      MessageInterface::ShowMessage("   colorParts[%d] = %s\n", i, colorParts[i].c_str());
   #endif
   if (colorParts.size() == 2)
   {
      std::string colorVal = colorParts[1];
      colorVal = GmatStringUtil::Strip(colorVal);
      #ifdef DEBUG_COLOR
      MessageInterface::ShowMessage("   colorVal = '%s'\n", colorVal.c_str());
      #endif
      
      // Check if color value is valid
      try
      {
         UnsignedInt intColor = RgbColor::ToIntColor(colorVal);
         #ifdef DEBUG_COLOR
         MessageInterface::ShowMessage("   intColor = %06X\n", intColor);
         #endif
         segmentOrbitColor = intColor;
         overrideSegmentColor = true;
         if (colorVal.find('[') == colorVal.npos)
            segmentOrbitColorStr = colorVal;
         else
            segmentOrbitColorStr = RgbColor::ToRgbString(intColor);
      }
      catch (BaseException &be)
      {
         throw CommandException(be.GetFullMessage());
      }
   }
   #ifdef DEBUG_COLOR
   MessageInterface::ShowMessage
      ("Propagate::ParseSegmentColor() leaving, segmentOrbitColor = %06X\n",
       segmentOrbitColor);
   #endif
}

//------------------------------------------------------------------------------
// void ConfigurePropSetup(std::string &setupDesc)
//------------------------------------------------------------------------------
/**
 * Builds the data needed for the a PropSetup.  Stopping conditions are handled
 * separately.
 *
 * @param <setupDesc>  The string describing the PropSetup.
 */
//------------------------------------------------------------------------------
void Propagate::ConfigurePropSetup(std::string &setupDesc)
{
   #ifdef DEBUG_PROPAGATE_ASSEMBLE
      MessageInterface::ShowMessage("Building PropSetup '%s'\n",
         setupDesc.c_str());
   #endif

   // First separate the PropSetup from the SpaceObjects
   std::string prop, sats, sat;
   std::string::size_type loc = setupDesc.find("(");
   if (loc == std::string::npos)
      throw CommandException("The propsetup string '" + setupDesc +
         "' does not identify any spacecraft for propagation on "
         + "the command line\n" + generatingString);
   prop = setupDesc.substr(0, loc);
   if (prop == "")
      throw CommandException("The propsetup string '" + setupDesc +
         "' does not identify any PropSetup on "
         + "the command line\n" + generatingString);
   sats = setupDesc.substr(loc);

   CleanString(prop);

   #ifdef DEBUG_PROPAGATE_ASSEMBLE
      MessageInterface::ShowMessage("   PropSetup is '%s'\n", prop.c_str());
   #endif
   SetObject(prop, Gmat::PROP_SETUP);

   // Next the SpaceObjects
   StringArray extras;
   extras.push_back("(");
   extras.push_back(")");
   extras.push_back(",");

   loc = 0;
   while (loc != std::string::npos)
   {
      loc = sats.find(',');
      sat = sats.substr(0, loc);
      sats = sats.substr(loc+1);
      CleanString(sat, &extras);

      #ifdef DEBUG_PROPAGATE_ASSEMBLE
         MessageInterface::ShowMessage("   Found prop object \"%s\"\n", sat.c_str());
      #endif
      SetObject(sat, Gmat::SPACECRAFT);
   }
}


//------------------------------------------------------------------------------
// void ConfigureStoppingCondition(std::string &stopDesc)
//------------------------------------------------------------------------------
/**
 * Builds the data needed for a stopping condition.  PropSetups are handled
 * separately.
 *
 * @param <stopDesc>  The string describing the stopping condition.
 */
//------------------------------------------------------------------------------
void Propagate::ConfigureStoppingCondition(std::string &stopDesc)
{
   #ifdef DEBUG_PROPAGATE_ASSEMBLE
      MessageInterface::ShowMessage("Building Stop '%s'\n",
         stopDesc.c_str());
   #endif

   std::string lhs, rhs = "";
   std::string::size_type loc;
   StringArray extras;
   extras.push_back("{");
   extras.push_back("}");
   extras.push_back("=");

   loc = stopDesc.find("=");
   if (loc == std::string::npos)
   {
      lhs = stopDesc;
      CleanString(lhs, &extras);
   }
   else
   {
      lhs = stopDesc.substr(0,loc);
      CleanString(lhs, &extras);
      rhs = stopDesc.substr(loc+1);
      CleanString(rhs, &extras);

      if (lhs == "StopTolerance")
      {
         Real rval;
         if (GmatStringUtil::ToReal(rhs, rval))
            SetRealParameter(STOP_ACCURACY, rval);
         else
         {
            CommandException ce;
            ce.SetDetails(errorMessageFormatUnnamed.c_str(),
               rhs.c_str(), "StopTolerance", "a Real number > 0.0");
            throw ce;
         }
         return;
      }
   }

   #ifdef DEBUG_PROPAGATE_ASSEMBLE
      MessageInterface::ShowMessage("   Stop = '%s' with value '%s'\n",
         lhs.c_str(), rhs.c_str());
   #endif

   // Now to work!
   std::string paramType, paramObj, paramSystem;
   GmatStringUtil::ParseParameter(lhs, paramType, paramObj, paramSystem);

   // Create the stop parameter
   std::string paramName;
   if (paramSystem == "")
      //paramName = paramObj + "." + paramType;
      paramName = lhs;
   else
      paramName = paramObj + "." + paramSystem + "." + paramType;

   #ifdef DEBUG_PROPAGATE_ASSEMBLE
   MessageInterface::ShowMessage("   Creating local StopCondition\n");
   #endif

   StopCondition *stopCond = new StopCondition("StopOn" + paramName);

   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (stopCond, stopCond->GetName(), "Propagate::ConfigureStoppingCondition()",
       "*stopCond = new StopCondition(StopOn + paramName)");
   #endif

   if (find(stopNames.begin(), stopNames.end(), paramName) == stopNames.end())
   {
      #ifdef DEBUG_PROPAGATE_ASSEMBLE
      MessageInterface::ShowMessage("   Adding '%s' to stopNames\n", paramName.c_str());
      #endif
      stopNames.push_back(paramName);
      stopWrappers.push_back(NULL);
   }

   // Handle some static member initialization if this is the first opportunity
   if (stopCondEpochID == -1)
   {
      stopCondEpochID = stopCond->GetParameterID("Epoch");
      stopCondBaseEpochID = stopCond->GetParameterID("BaseEpoch");
      stopCondStopVarID = stopCond->GetParameterID("StopVar");
   }

   // Setup for backwards propagation
   stopCond->SetPropDirection(direction);  // Use direction of props
   stopCond->SetStringParameter(stopCondStopVarID, paramName);
   SetObject(stopCond, Gmat::STOP_CONDITION);

   #ifdef DEBUG_PROPAGATE_ASSEMBLE
   MessageInterface::ShowMessage("lhs paramObj='%s'\n", paramObj.c_str());
   #endif

   if (paramObj != "" && !GmatStringUtil::IsNumber(paramObj))
      TakeAction("SetStopSpacecraft", paramObj);


   if (paramType != "Apoapsis" && paramType != "Periapsis")
   {
      #ifdef DEBUG_PROPAGATE_ASSEMBLE
         MessageInterface::ShowMessage("Propagate::AssemblePropagators()"
            " component = <%s>\n", rhs.c_str());
      #endif

      // create goal parameter
      std::string component = rhs;

      if (!GmatStringUtil::IsNumber(rhs))
         GmatStringUtil::ParseParameter(rhs, paramType, paramObj, paramSystem);
      else
         paramObj = rhs;

      #ifdef DEBUG_PROPAGATE_ASSEMBLE
      MessageInterface::ShowMessage("rhs paramObj='%s'\n", paramObj.c_str());
      MessageInterface::ShowMessage("rhs = '%s' %s a number\n",
            rhs.c_str(),
            GmatStringUtil::IsNumber(rhs) ? "is" : "is not");
      #endif

      if (paramObj != "" && !GmatStringUtil::IsNumber(paramObj))
         TakeAction("SetStopSpacecraft", paramObj);

      if (find(goalNames.begin(), goalNames.end(), component) == goalNames.end())
      {
         #ifdef DEBUG_PROPAGATE_ASSEMBLE
         MessageInterface::ShowMessage("   Adding '%s' to goalNames\n", component.c_str());
         #endif
         goalNames.push_back(component);
         goalWrappers.push_back(NULL);
      }

      stopCond->SetStringParameter("Goal", component);
   }
   else
   {
      if (rhs.length() != 0)
      {
         throw CommandException("Stopping condition " + paramType +
            " does not take a value, but it is set using the string '" +
            stopDesc + "' in the line\n'" + generatingString + "'");
      }
   }
}


//------------------------------------------------------------------------------
// void CleanString(std::string &theString, const StringArray *extras)
//------------------------------------------------------------------------------
/**
 * Strips off leading and trailing whitespace, and additional characters if
 * specified.
 *
 * @param <theString>  The string that -- might -- need cleaned.
 * @param <extras>     All additional characters (other than a space) that
 *                     should be stripped off.
 */
//------------------------------------------------------------------------------
void Propagate::CleanString(std::string &theString, const StringArray *extras)
{
   #ifdef DEBUG_CLEAN_STRING
      MessageInterface::ShowMessage("In CleanString, theString = %s\n",
            theString.c_str());
//      for (Integer ii = 0; ii < extras->size(); ii++)
//         MessageInterface::ShowMessage("     %d     %s\n", ii, extras->at(ii).c_str());
   #endif
   Integer loc;
   Integer len = (Integer) theString.length();
   bool keepGoing = false;

   if (len == 0)
      return;

   // Clean up the start of the string
   for (loc = 0; loc < len; ++loc)
   {
      #ifdef DEBUG_CLEAN_STRING
         MessageInterface::ShowMessage("In CleanString, theString[%d] = %c\n",
               (Integer) loc, theString[loc]);
      #endif
      if ((theString[loc] != ' ') && (theString[loc] != '\''))
      {
         if (extras != NULL)
            for (StringArray::const_iterator i = extras->begin(); i != extras->end(); ++i)
               if (theString[loc] == (*i)[0])
                  keepGoing = true;
         if (!keepGoing)
            break;
         else
            keepGoing = false;
      }
   }
   theString = theString.substr(loc);
   #ifdef DEBUG_CLEAN_STRING
      MessageInterface::ShowMessage("In CleanString, (substring) theString = %s\n",
            theString.c_str());
   #endif

   // Clean up the end of the string
   keepGoing = false;
   for (loc = theString.length() - 1; loc >= 0; --loc)
   {
      if ((theString[loc] != ' ') && (theString[loc] != '\''))
      {
         if (extras != NULL)
            for (StringArray::const_iterator i = extras->begin(); i != extras->end(); ++i)
               if (theString[loc] == (*i)[0])
                  keepGoing = true;
         if (!keepGoing)
            break;
         else
            keepGoing = false;
      }
   }
   theString = theString.substr(0, loc+1);
   #ifdef DEBUG_CLEAN_STRING
      MessageInterface::ShowMessage("At the end of CleanString, (substring) theString = %s\n",
            theString.c_str());
   #endif
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Propagate command.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Propagate::Initialize()
{
   #if DEBUG_PROPAGATE_INIT
      MessageInterface::ShowMessage("Propagate::Initialize() <%p> entered\n", this);
      MessageInterface::ShowMessage("  Size of propName is %d\n",
                                    propName.size());
      for (UnsignedInt ind = 0; ind < propName.size(); ++ind)
         MessageInterface::ShowMessage("     %d:  %s\n",
                                    propName.size(), propName[ind].c_str());

      MessageInterface::ShowMessage("  Direction is %s\n",
                                    (direction == 1.0?"Forwards":"Backwards"));
   #endif

   bool retval = PropagationEnabledCommand::Initialize();

   inProgress = false;
   hasFired = false;
   sats.clear();           // Resetting: PropagationEnabledCommand did this once

   UnsignedInt index = 0;
   SpaceObject *so;
   std::string pName;
   GmatBase *mapObj = NULL;

   // Ensure that we are using fresh objects when buffering stops
   EmptyBuffer();
   cloneCount = 0;

   if (propagators.size() > 0)
   {
      for (std::vector<PropSetup*>::iterator ps = propagators.begin();
            ps != propagators.end(); ++ps)
         delete (*ps);

      propagators.clear();
      p.clear();
      fm.clear();
   }
   
   // Check uniqueness of objects we propagate
   // Make member data (LOJ: 2013.12.02)
   //StringArray fullSatList;
   fullSatList.clear();
   
   // Only do this if the command has a formation -- can't do this here, but
   // kept in code so that it can be addressed at refactoring
   // if (hasFormation)
   {
      for (UnsignedInt i = 0; i < satName.size(); ++i)
      {
         for (UnsignedInt j = 0; j < satName[i]->size(); ++j)
         {
            std::string scName = satName[i]->at(j);
            if ((mapObj = FindObject(scName)) != NULL)
            {
               if (mapObj->IsOfType(Gmat::SPACECRAFT))
                  fullSatList.push_back(scName);
               else  // It's a formation
               {
                  // Note: we do not recurse nested formations at this time
                  StringArray formSats = mapObj->GetStringArrayParameter("Add");
                  for (UnsignedInt k = 0; k < formSats.size(); ++k)
                     fullSatList.push_back(formSats[k]);
               }
            }
         }
      }
      // Now check uniqueness
      std::string repeats;
      for (StringArray::iterator i = fullSatList.begin(); i < fullSatList.end(); ++i)
      {
         StringArray::iterator j = i+1;
         if (find(j, fullSatList.end(), *i) != fullSatList.end())
         {
            if (repeats.length() > 0)
               repeats += ", ";
            repeats += (*i);
         }
      }
      if (repeats.length() > 0)
         throw CommandException("Duplicate Spacecraft [" + repeats +
               "] found in the Propagate command:\n   \"" +
               GetGeneratingString(Gmat::NO_COMMENTS) +
               "\"\nCheck spacecraft in Formations for repeated members");
   }

   for (StringArray::iterator i = propName.begin(); i != propName.end(); ++i)
   {
      ObjectArray els;

      if (satName.size() <= index)
         throw CommandException("Size mismatch for SpaceObject names\n");

      if ((*i)[0] == '-')
         pName = i->substr(1);
      else
        pName = *i;

      if ((mapObj = FindObject(pName)) == NULL)
         throw CommandException(
            "Propagate command cannot find Propagator Setup \"" + (pName) +
            "\"\n");

      if (satName[index]->empty())
         throw CommandException(
            "Propagate command does not have a SpaceObject for " + (pName) +
            " in \n\"" + generatingString + "\"\n");

      if (stopWhen.empty())
         singleStepMode = true;
      else
         singleStepMode = false;

      PropSetup *clonedProp = (PropSetup *)(mapObj->Clone());
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (clonedProp, clonedProp->GetName(), "Propagate::Initialize()",
          "(PropSetup *)(mapObj->Clone())");
      #endif
      //prop.push_back((PropSetup *)(mapObj->Clone()));

      #ifdef DEBUG_CLONES
         MessageInterface::ShowMessage("In Propagate::Initialize(), "
               "cloned PS <%p>, a clone of <%p>:\n%s", clonedProp, mapObj,
               clonedProp->GetGeneratingString(
                     Gmat::NO_COMMENTS, "      ").c_str());
      #endif

      propagators.push_back(clonedProp);
      ++cloneCount;
      if (!propagators[index])
         return false;

      Propagator *p = propagators[index]->GetPropagator();
      if (!p)
         throw CommandException("Propagator not set in PropSetup\n");
      p->TakeAction("PrepareForRun");

      // Toss the spacecraft into the prop state manager
      ODEModel *odem = propagators[index]->GetODEModel();
      if ((!odem) && p->UsesODEModel())
         throw CommandException("ForceModel not set in PropSetup\n");

      PropagationStateManager *psm = propagators[index]->GetPropStateManager();

      if (propAllSTMs)
      {
         if (odem == NULL)
            throw CommandException("Non-numeric propagators cannot propagate the"
                  " state transition matrix.\n");
      }
      if (calcAllAmatrices)
      {
         if (odem == NULL)
            throw CommandException("Non-numeric propagators cannot propagate the"
                  " A-matrix.\n");
      }

      StringArray::iterator scName;
//      StringArray owners, elements;

      /// @todo Check to see if All and All.Epoch belong in place for all modes.
//      owners.push_back("All");
//      elements.push_back("All.epoch");

      bool finiteBurnActive = false;

      for (scName = satName[index]->begin(); scName != satName[index]->end();
           ++scName)
      {
         #if DEBUG_PROPAGATE_INIT
            MessageInterface::ShowMessage(
                  "   Adding '%s' to prop state manager '%s'\n",
               scName->c_str(), i->c_str());
         #endif
         if ((mapObj = FindObject(*scName)) == NULL)
         {
            #if DEBUG_PROPAGATE_INIT
               MessageInterface::ShowMessage("   '%s' is not an object; "
                     "attempting to set as a prop property\n",
                     scName->c_str());
            #endif
            if (psm->SetProperty(*scName) == false)
            {
               std::string errmsg = "Unknown SpaceObject property \"";
               errmsg += *scName;
               errmsg += "\"";
               throw CommandException(errmsg);
            }
         }
         else
         {
            psm->SetObject(mapObj);

            // Set the globals for STM and/or A-Matrix if needed
            if (propAllSTMs)
               if (psm->SetProperty("STM") == false)
                  MessageInterface::ShowMessage("WARNING: Unable to set STM "
                        "propagation for %s\n", mapObj->GetName().c_str());
            if (calcAllAmatrices)
               if (psm->SetProperty("AMatrix") == false)
                  MessageInterface::ShowMessage("WARNING: Unable to set A-"
                        "Matrix computation for %s\n",
                        mapObj->GetName().c_str());
            so = (SpaceObject*)mapObj;
            if (calcAllCovariance)
            {
               if (psm->SetProperty("Covariance") == false)
                  MessageInterface::ShowMessage("WARNING: Unable to set "
                     "Covariance computation for %s\n",
                     mapObj->GetName().c_str());

               if (so->GetStringParameter("DisplayStateType") != "Cartesian")
               {
                  throw CommandException("Error: All Spacecraft undergoing Covariance propagation must be created with a" 
                     " Cartesian state. " + so->GetName() + " has " + so->GetStringParameter("DisplayStateType")+ " state type.");
               }
               // Currently Restrict CovarianceProp to Mj2000 based coordinate systems
               std::string csName = so->GetRefObject(Gmat::COORDINATE_SYSTEM, "CS")->GetStringParameter("Axes");
               if (csName != "MJ2000Eq")
               {
                  throw CommandException("Error: All Spacecraft undergoing Covariance propagation must have a CoordinateSystem with Axes defined as "
                     " MJ2000Eq. " + so->GetName() + " has axes of type " + csName);
               }
            }
            if (epochID == -1)
               epochID = so->GetParameterID("A1Epoch");
            if (so->IsManeuvering())
            {
               #ifdef DEBUG_FINITE_MANEUVER
                  MessageInterface::ShowMessage(
                     "SpaceObject %s is maneuvering\n", so->GetName().c_str());
               #endif
               finiteBurnActive = true;
            }
            #ifdef DEBUG_FINITE_MANEUVER
               else
                  MessageInterface::ShowMessage(
                        "SpaceObject %s is not maneuvering\n", so->GetName().c_str());
            #endif

            sats.push_back(so);
            AddToBuffer(so);

            if (so->GetType() == Gmat::FORMATION)
               ((FormationInterface*)(so))->BuildState();
//            FillFormation(so, owners, elements);
//            else
//            {
//               SetNames(so->GetName(), owners, elements);
//            }
         }
      }

      // Check for finite thrusts and update the force model if there are any
      if (finiteBurnActive == true)
      {
         if (odem != NULL)
            AddTransientForce(satName[index], odem, psm);
         else
            MessageInterface::ShowMessage("Spacecraft is performing a "
                  "finite maneuver but also propagating with an ephemeris "
                  "propagator; no independent maneuvering will be "
                  "performed.\n"); //, satName[index].c_str());
      }

      if (psm->BuildState() == false)
         throw CommandException("Could not build the state for the command \n" +
               generatingString);
      if (psm->MapObjectsToVector() == false)
         throw CommandException("Could not map state objects for the command\n" +
               generatingString);

      if (p->UsesODEModel())
      {
         odem->SetState(psm->GetState());
         // Set solar system to ForceModel for Propagate inside a GmatFunction
         odem->SetSolarSystem(solarSys);
      }
      else
      {
         ObjectArray pObjects;

         psm->GetStateObjects(pObjects, Gmat::SPACEOBJECT);
         for (UnsignedInt i = 0; i < pObjects.size(); ++i)
            p->SetRefObject(pObjects[i], Gmat::SPACEOBJECT,
                  pObjects[i]->GetName());
         p->SetSolarSystem(solarSys);
      }

      // #ifdef DEBUG_PUBLISH_DATA
      // MessageInterface::ShowMessage
      //       ("Propagate::Initialize() '%s' registering published data\n",
      //        GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      // #endif

//      streamID = publisher->RegisterPublishedData(this, streamID, owners,
//            elements);

      if (p->UsesODEModel())
         p->SetPhysicalModel(odem);
      p->SetRealParameter("InitialStepSize",
         fabs(p->GetRealParameter("InitialStepSize")) * direction);
      p->Initialize();

      // Set spacecraft parameters for forces that need them
      if (p->UsesODEModel())
         if (odem->SetupSpacecraftData(&sats, 0) <= 0)
            throw PropagatorException("Propagate::Initialize -- "
                  "ODE model cannot set spacecraft parameters");

      ++index;
   } // End of loop through PropSetups

   
   // It has to prepare data info and register to publishing right before
   // the first publishing data. Therefore this section of code is moved
   // to Propagate::PrepareToPropagate() function

   // Prep the publisher
   //StringArray owners, elements;
   //owners.push_back("All");
   //elements.push_back("All.epoch");

   //for (UnsignedInt i = 0; i < propagators.size(); ++i) 
   //{
   //   for (UnsignedInt j = 0; j < satName[i]->size(); ++j)
   //   {
   //      SpaceObject *so = NULL;
   //      for (UnsignedInt k = 0; k < sats.size(); ++k)
   //         if (sats[k]->GetName() == satName[i]->at(j))
   //            so = (SpaceObject*)sats[k];
   //      if (so == NULL)
   //         continue;
   //      if (so->GetType() == Gmat::FORMATION)
   //         FillFormation(so, owners, elements);
   //      else
   //      {
   //         #ifdef DEBUG_PUBLISH_DATA
   //            MessageInterface::ShowMessage
   //               ("Propagate::Initialize() '%s' setting data labels\n", 
   //               GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   //         #endif
   //         SetNames(so->GetName(), owners, elements);
   //      }
   //   }

   //   PropagationStateManager *psm2 = propagators[i]->GetPropStateManager();
   //   if (psm2->SatHasCovariance())
   //   {
   //      StringArray psmElements = psm2->ElementMapForSat();
   //      elements.insert(elements.end(), psmElements.begin(), psmElements.end());
   //   }
   //}


   //#ifdef DEBUG_PUBLISH_DATA
   //MessageInterface::ShowMessage
   //   ("Propagate::Initialize() '%s' registering published data\n", 
   //    GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   //#endif

   //streamID = publisher->RegisterPublishedData(this, streamID, owners, elements);
   
   isInitialized = true;

   stopSats.clear();
   // Setup spacecraft array used for stopping conditions
   for (StringArray::iterator sc = stopSatNames.begin();
        sc != stopSatNames.end(); ++sc)
   {
      if ((mapObj = FindObject(*sc)) == NULL)
      {
         std::string errmsg = "Unknown SpaceObject \"";
         errmsg += *sc;
         errmsg += "\" used in stopping conditions";
         throw CommandException(errmsg);
      }
      so = (SpaceObject*)mapObj;
      stopSats.push_back(so);
   }

   #if DEBUG_PROPAGATE_INIT
      for (UnsignedInt i=0; i<stopSats.size(); i++)
         MessageInterface::ShowMessage
            ("   stopSats[%d]=<%p>%s, IsStopSatGlobal:%d, IsStopSatLocal:%d\n",
             i, stopSats[i], stopSats[i]->GetName().c_str(), stopSats[i]->IsGlobal(),
             stopSats[i]->IsLocal());
      for (UnsignedInt i=0; i<stopWhen.size(); i++)
         MessageInterface::ShowMessage
            ("   stopWhen[%d]=<%p>%s, IsStopWhenGlobal:%d, IsStopWhenLocal:%d\n",
             i, stopWhen[i], stopWhen[i]->GetName().c_str(), stopWhen[i]->IsGlobal(),
             stopWhen[i]->IsLocal());
   #endif

   if ((stopWhen.size() == 0) && !singleStepMode)
      throw CommandException("No stopping conditions specified!");

   if (solarSys != NULL)
   {
      StringArray refNames;

      for (UnsignedInt i=0; i<stopWhen.size(); i++)
      {
         stopWhen[i]->SetSolarSystem(solarSys);

         //Set StopCondition parameters
         refNames = stopWhen[i]->GetRefObjectNameArray(Gmat::PARAMETER);

         for (UnsignedInt j=0; j<refNames.size(); j++)
         {
            #if DEBUG_PROPAGATE_INIT
               MessageInterface::ShowMessage("   refNames[%d]='%s'\n", j,
                  refNames[j].c_str());
            #endif
            mapObj = FindObject(refNames[j]);
            stopWhen[i]->SetRefObject(mapObj, Gmat::PARAMETER, refNames[j]);
         }

         try
         {
            stopWhen[i]->Initialize();
            stopWhen[i]->SetSpacecrafts(sats, stopSats);

            if (!stopWhen[i]->IsInitialized())
            {
               isInitialized = false;
               MessageInterface::ShowMessage(
                  "Propagate::Initialize() StopCondition %s is not initialized.\n",
                  stopWhen[i]->GetName().c_str());
               break;
            }
         }
         catch (BaseException &be)
         {
            CommandException ce;
            ce.SetDetails("%s in %s\n", be.GetFullMessage().c_str(),
                          GetGeneratingString(Gmat::NO_COMMENTS).c_str());
            throw ce;
         }
      }
   }
   else
   {
      isInitialized = false;
      MessageInterface::ShowMessage
         ("Propagate::Initialize() SolarSystem not set in StopCondition");
   }

   #if DEBUG_PROPAGATE_INIT
      MessageInterface::ShowMessage("Propagate::Initialize() complete.\n");
   #endif

   #ifdef DEBUG_PROPAGATE_DIRECTION
      MessageInterface::ShowMessage("Propagate::Initialize():"
                                    " Propagators Identified:\n");
      for (StringArray::iterator i = propName.begin(); i != propName.end();
           ++i)
         MessageInterface::ShowMessage("   \"%s\" running %s\n", i->c_str(),
         (direction > 0.0 ? "forwards" : "backwards"));
   #endif

   if (singleStepMode)
   {
      commandSummary = "Command Summary: ";
      commandSummary += typeName;
      commandSummary += " Command\nSummary not available in single step mode\n";
   }

   #ifdef DUMP_PLANET_DATA
      if (body[0] == NULL)
         body[0] = solarSys->GetBody("Earth");
      if (body[1] == NULL)
         body[1] = solarSys->GetBody("Sun");
      if (body[2] == NULL)
         body[2] = solarSys->GetBody("Luna");
      if (body[3] == NULL)
         body[3] = solarSys->GetBody("Mercury");
      if (body[4] == NULL)
         body[4] = solarSys->GetBody("Venus");
      if (body[5] == NULL)
         body[5] = solarSys->GetBody("Mars");
      if (body[6] == NULL)
         body[6] = solarSys->GetBody("Jupiter");
      if (body[7] == NULL)
         body[7] = solarSys->GetBody("Saturn");
      if (body[8] == NULL)
         body[8] = solarSys->GetBody("Uranus");
      if (body[9] == NULL)
         body[9] = solarSys->GetBody("Neptune");
      if (body[10] == NULL)
         body[10] = solarSys->GetBody("Pluto");

      bodiesDefined = 11;
   #endif

   #if DEBUG_PROPAGATE_INIT
      MessageInterface::ShowMessage
         ("Propagate::Initialize() <%p> returning initialized=%d\n", this,
          isInitialized);
   #endif

   return isInitialized && retval;
}

//------------------------------------------------------------------------------
// void FillFormation(SpaceObject *so)
//------------------------------------------------------------------------------
/**
 * Fill in the components of a formation (recursively).
 *
 * @param <so> The SpaceObject that needs to be filled.
 */
//------------------------------------------------------------------------------
void Propagate::FillFormation(SpaceObject *so, StringArray& owners,
                              StringArray& elements)
{
   GmatBase *mapObj = NULL;
   static Integer soEpochId = -1;
   if ((so == NULL) || (so->GetType() != Gmat::FORMATION))
      throw CommandException("Invalid SpaceObject passed to FillFormation");

   if (soEpochId == -1)
      soEpochId = so->GetParameterID("A1Epoch");

   StringArray comps = so->GetStringArrayParameter(so->GetParameterID("Add"));
   SpaceObject *el;
   Real ep;

   for (StringArray::iterator i = comps.begin(); i != comps.end(); ++i)
   {
      if ((mapObj = FindObject(*i)) == NULL)
         throw CommandException("Formation " + so->GetName() +
            " uses unknown object named '" + (*i) + "'");

      el = (SpaceObject*)mapObj;
      if (i == comps.begin())
      {
         ep = el->GetRealParameter(soEpochId);
         so->SetRealParameter(soEpochId, ep);
      }

      so->SetRefObject(el, el->GetType(), el->GetName());
      if (el->GetType() == Gmat::FORMATION)
         FillFormation(el, owners, elements);
      else     // Setup spacecraft data descriptions
      {
         #ifdef DEBUG_PUBLISH_DATA
         MessageInterface::ShowMessage
            ("Propagate::FillFormation() '%s' setting data labels\n",
             GetGeneratingString(Gmat::NO_COMMENTS).c_str());
         #endif
         SetNames(el->GetName(), owners, elements);
      }
   }

   ((FormationInterface*)(so))->BuildState();
}


//------------------------------------------------------------------------------
// GmatCommand* GetNext()
//------------------------------------------------------------------------------
/**
 * Returns pointer to next command to be executed.
 *
 * Propagate::GetNext overrides the base class's GetNext method so that it can
 * poll the moderator for user interrupts periodically.  If the stopping
 * conditions have not yet been met, GetNext returns this object; otherwise, it
 * returns the next one in the command list.
 *
 * @return The next pointer, as described above.
 */
//------------------------------------------------------------------------------
GmatCommand* Propagate::GetNext()
{
   if (!inProgress)
      return next;
   return this;
}

//------------------------------------------------------------------------------
// void PrepareToPropagate()
//------------------------------------------------------------------------------
/**
 * Performs initialization needed immediately before propagating.
 */
//------------------------------------------------------------------------------
void Propagate::PrepareToPropagate()
{
   #ifdef DEBUG_PROPAGATE_INIT
      MessageInterface::ShowMessage
         ("Propagate::PrepareToPropagate() <%p> entered\n", this);
      GmatState *dstate = propagators[0]->GetPropStateManager()->GetState();
      Integer dimension = dstate->GetSize();
      MessageInterface::ShowMessage(
            "PrepareToPropagate top; State vector contents and fm state are\n"
            "   Epoch = %.12lf\n", (dstate->GetEpoch()));
      for (Integer index = 0; index < dimension; ++index)
      {
         MessageInterface::ShowMessage("   %d:   %.12lf\n", index,
               (*dstate)[index]);
      }
   #endif

   dim = 0;

   if (hasFired == true)
   {
      // Handle the transient forces
      for (UnsignedInt i = 0; i < p.size(); ++i)
         if (fm[i] != NULL)
            AddTransientForce(satName[i], fm[i], psm[i]);

      for (Integer n = 0; n < (Integer)propagators.size(); ++n)
      {
         elapsedTime[n] = 0.0;
         currEpoch[n]   = 0.0;
         currEpochGT[n] = 0.0;

         if (propagators[n]->GetPropagator()->UsesODEModel())
         {
            fm[n]->SetTime(0.0);
            fm[n]->SetPropStateManager(propagators[n]->GetPropStateManager());
            fm[n]->UpdateInitialData();
            dim += fm[n]->GetDimension();
         }
         else
         {
            p[n]->SetPropStateManager(propagators[n]->GetPropStateManager());
            dim += p[n]->GetDimension();
         }

         if (resetLoopDataFlag)
            p[n]->ResetInitialData();
         p[n]->Initialize();

         p[n]->Update(direction > 0.0);
      }

      resetLoopDataFlag = false;
      baseEpoch.clear();
      baseEpochGT.clear();

      for (Integer n = 0; n < (Integer)propagators.size(); ++n)
      {
         #if DEBUG_PROPAGATE_INIT
            MessageInterface::ShowMessage
               ("Propagate::PrepareToPropagate() SpaceObject names\n");
            MessageInterface::ShowMessage
               ("SpaceObject Count = %d\n", satName[n]->size());
            StringArray *sar = satName[n];
            for (Integer i=0; i < (Integer)satName[n]->size(); i++)
            {
               MessageInterface::ShowMessage
                  ("   SpaceObjectName[%d] = %s\n", i, (*sar)[i].c_str());
            }
            Propagator *prop = propagators[n]->GetPropagator();
            ODEModel *om = propagators[n]->GetODEModel();
            MessageInterface::ShowMessage
               ("   Propagator = <%p>[%s]'%s'\n", prop,
                prop ? prop->GetTypeName().c_str() : "NULL",
                prop ? prop->GetName().c_str() : "NULL");
            MessageInterface::ShowMessage
               ("   ODEModel   = <%p>[%s]'%s'\n", om,
                om ? om->GetTypeName().c_str() : "NULL",
                om ? om->GetName().c_str() : "NULL");
            for (Integer i = 0; i < om->GetNumForces(); i++)
            {
               PhysicalModel *pm = om->GetForce(i);
               MessageInterface::ShowMessage
                  ("   om[%d] = <%p>[%s]'%s'\n", i, pm, pm->GetTypeName().c_str(),
                   pm->GetName().c_str());
            }
         #endif

         if (satName[n]->empty())
            throw CommandException(
               "Propagator has no associated space objects.");

         GmatBase* sat1 = FindObject(*satName[n]->begin());
         baseEpoch.push_back(sat1->GetRealParameter(epochID));
         baseEpochGT.push_back(sat1->GetGmatTimeParameter(epochID));

         if (propagators[n]->GetPropagator()->UsesODEModel())
         {
            elapsedTime[n] = fm[n]->GetTime();
         }
         else
         {
            elapsedTime[n] = p[n]->GetTime();
            baseEpoch[n] -= elapsedTime[n] / GmatTimeConstants::SECS_PER_DAY;
            baseEpochGT[n].SubtractSeconds(elapsedTime[n]);
         }
         currEpoch[n] = baseEpoch[n] + elapsedTime[n] /
            GmatTimeConstants::SECS_PER_DAY;
         currEpochGT[n] = baseEpochGT[n];
         currEpochGT[n].AddSeconds(elapsedTime[n]);

         #if DEBUG_PROPAGATE_DIRECTION
            MessageInterface::ShowMessage(
               "Propagate::PrepareToPropagate() running %s %s.\n",
               propagators[n]->GetName().c_str(),
               (propagators[n]->GetPropagator()->GetRealParameter("InitialStepSize") > 0.0
                  ? "forwards" : "backwards"));
             MessageInterface::ShowMessage("   direction =  %lf.\n",
               direction);
         #endif
      }

      // Now setup the stopping condition elements
      PrepareStoppingConditions();
      inProgress = true;
   }
   else
   {
      // Set the prop state managers for the PropSetup ODEModels
      for (std::vector<PropSetup*>::iterator i=propagators.begin(); i != propagators.end(); ++i)
      {
         ODEModel *ode = (*i)->GetODEModel();
         if (ode != NULL)    // Only do this for the PropSetups that integrate
            ode->SetPropStateManager((*i)->GetPropStateManager());
         else
            (*i)->GetPropagator()->SetPropStateManager(
                  (*i)->GetPropStateManager());
      }

      // Check if the attitude affects the dynamics of the force model
      bool attAffectDyn = false;
      for (UnsignedInt n = 0U; n < propagators.size(); n++)
      {
         ODEModel *odem = propagators[n]->GetODEModel();
         if (odem)
         {
            for (UnsignedInt ii = 0U; ii < odem->GetNumForces(); ii++)
            {
               PhysicalModel* f = odem->GetForce(ii);

               if (f->AttitudeAffectsDynamics())
               {
                  attAffectDyn = true;
                  break;
               }
            }
         }

         if (attAffectDyn)
            break;
      }

      // If the attitude of the Spacecraft affects the force model,
      // indicate that the Cartesian state affects dynamics.
      // This is needed when the Spacecraft's attitude uses a coordinate
      // system which is a function of the Spacecraft's state
      if (attAffectDyn)
      {
         for (std::vector<GmatBase *>::const_iterator obj = sats.begin();
            obj != sats.end(); ++obj)
         {
            if ((*obj)->IsOfType("Spacecraft"))
            {
               Spacecraft* s = (Spacecraft*)(*obj);
               s->AttitudeAffectsDynamics(true);
            }
         }
      }

      // Initialize the subsystem
      #ifdef DEBUG_PROPAGATE_INIT
         MessageInterface::ShowMessage("Initializing Propagate command\n");
      #endif
      Initialize();

      // Loop through the PropSetups and build the models
      for (std::vector<PropSetup*>::iterator i=propagators.begin(); i != propagators.end(); ++i)
      {
         #ifdef DEBUG_PROPAGATE_INIT
            MessageInterface::ShowMessage("Building models for Setup %s\n",
                  (*i)->GetName().c_str());
         #endif
         
         PropagationStateManager *psmObj = (*i)->GetPropStateManager();
         ObjectArray objList;
         psmObj->GetStateObjects(objList, Gmat::UNKNOWN_OBJECT);
         for (Integer i = 0; i < objList.size(); ++i)
         {
            if (objList[i]->IsOfType(Gmat::SPACECRAFT))
            {
               // Input value 2 tells spacecraft that propagation command is running
               ((Spacecraft*)objList[i])->SetRunningCommandFlag(2);
            }
         }

         ODEModel *ode = (*i)->GetODEModel();
         if (ode != NULL)    // Only do this for the PropSetups that integrate
         {
            // Build the ODE model
            ode->SetPropStateManager((*i)->GetPropStateManager());
            if (ode->BuildModelFromMap() == false)
               throw CommandException("Unable to assemble the ODE model for " +
                     (*i)->GetName());
         }
         else
         {
            (*i)->GetPropagator()->SetPropStateManager((*i)->
                  GetPropStateManager());
         }
      }

      p.clear();
      fm.clear();
      psm.clear();
      baseEpoch.clear();
      currEpoch.clear();
      baseEpochGT.clear();
      currEpochGT.clear();

      #ifdef DEBUG_PROPAGATE_INIT
         MessageInterface::ShowMessage("Loading p and fm vectors\n");
      #endif
      for (Integer n = 0; n < (Integer)propagators.size(); ++n)
      {
         elapsedTime.push_back(0.0);
         p.push_back(propagators[n]->GetPropagator());
         if (!p.back()->SetNoiseStep())
         {
            throw CommandException("The values of the ProcessNoiseModel UpdateTimeStep parameters for the Spacecraft propagated by " + propName[n] + " do not match.");
         }
         if (propagators[n]->GetPropagator()->UsesODEModel())
         {
            fm.push_back(propagators[n]->GetODEModel());
            dim += fm[n]->GetDimension();
         }
         else
         {
            fm.push_back(NULL);
            dim += p[n]->GetDimension();
         }

         psm.push_back(propagators[n]->GetPropStateManager());

         #ifdef DEBUG_PROPAGATE_INIT
            MessageInterface::ShowMessage("Initializing propagator %d\n", n);
         #endif
         p[n]->Initialize();
         psm[n]->MapObjectsToVector();
         currEpoch.push_back(psm[n]->GetState()->GetEpoch());
         currEpochGT.push_back(psm[n]->GetState()->GetEpochGT());

         p[n]->Update(direction > 0.0);

         //if (propagators[n]->GetPropagator()->UsesODEModel())
         //{
         //   state = fm[n]->GetState();
         //   j2kState = fm[n]->GetJ2KState();
         //}
         //else
         //{
         //   state = p[n]->GetState();
         //   j2kState = p[n]->GetJ2KState();
         //}
         
         baseEpoch.push_back(psm[n]->GetState()->GetEpoch());
         baseEpochGT.push_back(psm[n]->GetState()->GetEpochGT());

         #ifdef DEBUG_PROPAGATE_INIT
            Real *state, *j2kState;
            if (propagators[n]->GetPropagator()->UsesODEModel())
            {
               state = fm[n]->GetState();
               j2kState = fm[n]->GetJ2KState();
            }
            else
            {
               state = p[n]->GetState();
               j2kState = p[n]->GetJ2KState();
            }

            GmatState *dstate = psm[n]->GetState();
            Integer dimension = dstate->GetSize();
            MessageInterface::ShowMessage(
                     "State vector contents and fm state are\n"
                     "   Epoch = %.12lf\n", (dstate->GetEpoch()));
            for (Integer index = 0; index < dimension; ++index)
            {
               MessageInterface::ShowMessage("   %d:   %.12lf   %.12lf\n", index,
                     (*dstate)[index], state[index]);
            }
         #endif

         #ifdef DEBUG_PROPAGATE_INIT
            MessageInterface::ShowMessage("Setting up stopping conditions\n");
         #endif

         // Now setup the stopping condition elements
         PrepareStoppingConditions();
         hasFired = true;
         inProgress = true;

         #ifdef DEBUG_FIRST_CALL
            if (state)
            {
               MessageInterface::ShowMessage("Debugging first step\n");
               MessageInterface::ShowMessage(
                  "State = [%16.9lf %16.9lf %16.9lf %16.14lf %16.14lf %16.14lf]\n",
                  state[0], state[1], state[2], state[3], state[4], state[5]);
               MessageInterface::ShowMessage(
                  "Propagator = \n%s\n",
                  propagators[0]->GetGeneratingString(Gmat::SCRIPTING, "   ").c_str());
            }
            else
               MessageInterface::ShowMessage("Debugging first step: State not set\n");
            firstStepFired = true;
         #endif

      }
   }


   // Sanity check the Spacecraft epochs to be sure that they match for each propagator
   for (UnsignedInt i = 0; i < psm.size(); ++i)
   {
      if (psm[i]->ObjectEpochsMatch() == false)
         throw CommandException("Spacecraft epochs do not match in a "
            "Propagator used in the Propagate command");

      // Check if the Covariance is being propagated with improper setup
      if (calcAllCovariance)
      {
         for (Integer i = 0; i < propagators.size(); i++)
         {
            if (propagators[i]->GetODEModel()->GetStringParameter("ErrorControl") != "None")
            {
               throw CommandException("Covariance propagation requires use of fixed "
                  "step integration. The ErrorControl parameter specified for "
                  "the ForceModel resource associated with the propagator, " +
                  propName[i] + ", used with command \"" + GetGeneratingString() + "\"" +
                  instanceName + " must be 'None.' When using fixed step "
                  "control, the user must also choose a step size, as given by the "
                  "Propagator InitialStepSize field, for the chosen orbit regime "
                  "and force profile, that yields the desired accuracy.");
            }
         }
      }
   }
   // Step to calculate force apply to spacecrafts at initial epoch
   CalculateStateDotAtInitialEpoch();


   // It needs to register to publish data right before the first publishing data 
   #ifdef DEBUG_PUBLISH_DATA
      MessageInterface::ShowMessage
         ("Propagate::PrepareToPropagate() '%s' registering published data\n", 
         GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   
   //StringArray owners, elements;
   pubdataOwners.clear();
   pubdataElements.clear();
   expandedDim = PrepareElementInfoToPublish(pubdataOwners, pubdataElements, covStartIndex, attStartIndex);
   streamID = publisher->RegisterPublishedData(this, streamID, pubdataOwners, pubdataElements);

   if (pubdata)
   {
      #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (pubdata, "pubdata", "Propagate::PrepareToPropagate()",
             "deleting pub data");
      #endif
      delete [] pubdata;
      pubdata = NULL;
      pubdataSize = 0;
   }

   #ifdef DEBUG_PUBLISH_DATA
      MessageInterface::ShowMessage("Setting pubdata with expanded dimension %d to store"
         " for spacecrafts' covariance and accelleration\n", 
               expandedDim);
   #endif
   //pubdata = new Real[dim+1];
   pubdataSize = expandedDim;
   pubdata = new Real[pubdataSize];

   #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (pubdata, "pubdata", "Propagate::PrepareToPropagate()",
          "pubdata = new Real[pubdataSize]");
   #endif
   
   
   // Prepare data to buffer for publishing
   for (Integer idx = 0; idx < pubdataSize; ++idx)
      pubdata[idx] = GmatMathConstants::QUIET_NAN;

   if (hasPrecisionTime)
      pubdata[0] = currEpochGT[0].GetMjd();
   else
      pubdata[0] = currEpoch[0];
   
   //Integer len = 1;
   PrepareElementDataToPublish(pubdata, pubdataSize, pubdataElements, covStartIndex, attStartIndex);

   //// Walk the PropSetups to load the pubdata array
   //Integer index = 1, size;
   //Real *js;
   //const Real* jsDeriv;
   //for (UnsignedInt i = 0; i < propagators.size(); ++i)
   //{
   //   if (p[i]->UsesODEModel())
   //   {
   //      js = fm[i]->GetJ2KState();
   //      size = fm[i]->GetDimension();
   //      jsDeriv = fm[i]->GetDerivativeArray();
   //   }
   //   else
   //   {
   //      js   = p[i]->GetJ2KState();
   //      size = p[i]->GetDimension();
   //      //jsDeriv = p[i]->GetJ2KDerivativeArray();
   //   }
   //   memcpy(&pubdata[index], js, size*sizeof(Real));
   //   index += size;
   //}
   
   
   #ifdef DEBUG_PUBLISH_DATA
      if (fabs(pubdata[1]) + fabs(pubdata[2]) + fabs(pubdata[3]) < 100.0)
         MessageInterface::ShowMessage(">>>>>>>>>>>>> P2P: BAD DATA BEING PUBLISHED!!! <<<<<<<<<<<<<<\n");
      MessageInterface::ShowMessage
         ("Propagate::PrepareToPropagate()\n   '%s'\nPublishing initial %d "
               "data to stream %d, 1st data = [",
          GetGeneratingString(Gmat::NO_COMMENTS).c_str(), dim+1, streamID);
      for (Integer i = 0; i < dim+1; ++i)
      {
         MessageInterface::ShowMessage("%.12lf", pubdata[i]);
         if (i < dim)
            MessageInterface::ShowMessage("   ");
         else
            MessageInterface::ShowMessage("]\n");
      }
      if (fabs(pubdata[1]) + fabs(pubdata[2]) + fabs(pubdata[3]) < 100.0)
         MessageInterface::ShowMessage(">>>>>>>>>>>>>  P2P: BAD DATA WAS PUBLISHED!!!  <<<<<<<<<<<<<<\n");
   #endif

// Start on a fix for bug 648; also uncomment the execute block for single
// step to work on this approach
//   // Only publish if there is an unpublished SpaceObject in the data
//   bool unpublishedObjectExists = false;
//   for (UnsignedInt i = 0; i < sats.size(); ++i)
//   {
//      SpaceObject *sc = (SpaceObject *)sats[i];
//      if (!sc->HasPublished())
//         unpublishedObjectExists = true;
//   }
//   if (unpublishedObjectExists)
//   {
//      #ifdef DEBUG_PUBLISH_DATA
//         MessageInterface::ShowMessage("Unpublished Object Found\n");
//      #endif

      
   // Publish
   //publisher->Publish(this, streamID, pubdata, dim + 1, direction);
   publisher->Publish(this, streamID, pubdata, expandedDim, direction);
   

//   }
//   if (unpublishedObjectExists == true)
//   {
//      for (UnsignedInt i = 0; i < sats.size(); ++i)
//      {
//         SpaceObject *sc = (SpaceObject *)sats[i];
//         sc->HasPublished(true);
//      }
//   }
   #ifdef DEBUG_PROPAGATE_INIT
      MessageInterface::ShowMessage
         ("Propagate::PrepareToPropagate() <%p> leaving\n", this);
   #endif
}



//------------------------------------------------------------------------------
// void Propagate::PrepareStoppingConditions()
//------------------------------------------------------------------------------
/**
 * Sets up stopping conditions structures as part of PrepareToPropagate
 */
//------------------------------------------------------------------------------
void Propagate::PrepareStoppingConditions()
{
   #if DEBUG_PROPAGATE_INIT
      if (p[0]->UsesODEModel())
      {
         MessageInterface::ShowMessage
            ("Propagate::PrepareToPropagate() Propagate start; epoch = %f\n",
          (baseEpoch[0] + fm[0]->GetTime() / GmatTimeConstants::SECS_PER_DAY));
         MessageInterface::ShowMessage
            ("Propagate::PrepareToPropagate() Propagate start; fm epoch = %f\n",
            (fm[0]->GetRealParameter(fm[0]->GetParameterID("Epoch"))));
      }
      else
      {
         MessageInterface::ShowMessage("Propagator state data (No ODE "
               "Model):\n   To be filled in\n");
      }
      Integer stopCondCount = stopWhen.size();
      MessageInterface::ShowMessage
         ("Propagate::PrepareToPropagate() stopCondCount = %d\n", stopCondCount);

      for (Integer i=0; i<stopCondCount; i++)
      {
         MessageInterface::ShowMessage
            ("Propagate::PrepareToPropagate() stopCondName[%d]=%s\n", i,
                   stopWhen[i]->GetName().c_str());
      }
   #endif

   stopCondMet = false;
   stopEpoch = 0.0;
   std::string stopVar;
   Real stopEpochBase;

   #ifdef DEBUG_STOPPING_CONDITIONS
      if (!singleStepMode)
         MessageInterface::ShowMessage(
            "Stopping condition IDs are [%d, %d, %d]\n",
            stopCondEpochID, stopCondBaseEpochID, stopCondStopVarID);
   #endif

   try
   {
      for (UnsignedInt i = 0; i < stopWhen.size(); ++i)
      {
         if (i >= stopSats.size())
            throw CommandException("Stopping condition " +
            stopWhen[i]->GetName() + " has no associated spacecraft.");

         #if DEBUG_PROPAGATE_INIT
            MessageInterface::ShowMessage(
               "Propagate::PrepareToPropagate() stopSat = %s\n",
               stopSats[i]->GetName().c_str());
         #endif

         stopEpochBase = stopSats[i]->GetRealParameter(epochID);

         // StopCondition needs new base epoch
         stopWhen[i]->SetRealParameter(stopCondBaseEpochID, stopEpochBase);

         // Validate that time based stops are propagating in correct direction
         if (stopWhen[i]->IsTimeCondition())
         {
            // Calculate the direction needed
            stopWhen[i]->Evaluate();
            Real delta = stopWhen[i]->GetStopDifference();
            if (delta * direction < 0.0)
            {
               std::string msg = "Warning: The stopping condition " +
                     stopWhen[i]->GetName() + " cannot be met; propagation "
                     "runs in the wrong direction in line\n   \"" +
                     GetGeneratingString(Gmat::NO_COMMENTS) + "\"";
               MessageInterface::ShowMessage("%s\n\n", msg.c_str());
            }
         }
      }
   }
   catch (BaseException &ex)
   {
      // Use exception to remove Visual C++ warning
      ex.GetMessageType();
      MessageInterface::ShowMessage(
         "Propagate::PrepareToPropagate() Exception while initializing stopping "
         "conditions\n");
      inProgress = false;
      throw;
   }
}

//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Propagate the assigned members to the desired stopping condition
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Propagate::Execute()
{
   #if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage
         ("Propagate::Execute() <%s> entered.\n   initialized = %d, inProgress = %d\n",
          GetGeneratingString().c_str(), isInitialized, inProgress);
   #endif

   if (isInitialized == false)
      throw CommandException("Propagate Command was not Initialized\n");

   if (transientForces->size() != transientCount)
   {
      #ifdef DEBUG_TRANSIENT_FORCES
         MessageInterface::ShowMessage("Count changed from %d to %d\n", transientCount, transientForces->size());
      #endif

      // Force a reset of the transient forces
      transientCount = transientForces->size();
      hasFired = false;
   }

   // Parm used to check for interrupt in the propagation
   Integer checkCount = 0, trigger = 0;

   try
   {
      // If command is not reentering from checking interrupts, do final prep
      if (!inProgress)
      {
         stepBrackets[0] = 0.0;
         checkFirstStep = false;
         PrepareToPropagate();

         // Check for initial stop condition before first step in while loop
         // eg) elapsed time of 0
         // Added to also check Gmat::SOLVING, since Optimize.cpp was updated to
         // set the run state to Gmat::SOLVING (LOJ: 2010.01.12)
         if (publisher->GetRunState() == Gmat::RUNNING ||
             publisher->GetRunState() == Gmat::SOLVING)
         {
            // remove any old stop conditions that may have reported valid
            triggers.clear();
            stopTrigger = -1;

            // Evaluate Stop conditions to set initial values
            for (UnsignedInt i=0; i<stopWhen.size(); i++)
            {
               Real accuracy = (stopWhen[i]->IsTimeCondition() ? timeAccuracy :
                                firstStepTolerance);

               stopWhen[i]->Reset();
               stopWhen[i]->Evaluate();
               #ifdef DEBUG_STOPPING_CONDITIONS
                  MessageInterface::ShowMessage("timeAccuracy: %le "
                        "FirstStepTolerance: %le\n", timeAccuracy,
                        firstStepTolerance);
                  MessageInterface::ShowMessage
                     ("Achieved: %.12lf Goal: %.12lf Difference: %.12le Stop "
                      "accuracy: %le\n", stopWhen[i]->GetStopValue(),
                      stopWhen[i]->GetStopGoal(),
                      stopWhen[i]->GetStopValue() - stopWhen[i]->GetStopGoal(),
                      accuracy);
               #endif
               // Set the flag to check the first step only if
               //    (1) the stop value is <= stopAccuracy and
               //    (2) it was (one of) the last stop(s) triggered
               if (fabs(stopWhen[i]->GetStopValue() -
                     stopWhen[i]->GetStopGoal()) < accuracy)
               {
                  #ifdef DEBUG_FIRST_STEP_STOP
                     std::string scName = stopWhen[i]->GetName();
                     MessageInterface::ShowMessage(
                        "*** FirstStep: Value = %.12lf, goal = %.12lf\n*** "
                        "Name = %s, WLST = %s\n", stopWhen[i]->GetStopValue(),
                        stopWhen[i]->GetStopGoal(),
                        scName.c_str(),
                        (stopSats[i]->WasLastStopTriggered(scName) ?
                         "true" : "false"));
                  #endif

                  if (stopSats[i]->WasLastStopTriggered(stopWhen[i]->GetName()))
                  {
                     checkFirstStep = true;
                     stopWhen[i]->SkipEvaluation(true);
                  }
               }
            }
         }
      } // if (!inProgress)

      #ifdef DEBUG_EPOCH_SYNC
         MessageInterface::ShowMessage("Nominal steps executing:\n");
      #endif

      while (!stopCondMet)
      {
         // Update the epoch on the propagators
         for (UnsignedInt i = 0; i < fm.size(); ++i)
         {
            if (fm[i] != NULL)
               fm[i]->UpdateInitialData(false, false);
            else
               p[i]->UpdateFromSpaceObject();
         }
         #ifdef DEBUG_EPOCH_UPDATES
            for (UnsignedInt i = 0; i < fm.size(); ++i)
               if (fm[i])
                  fm[i]->ReportEpochData();
         #endif

         for (UnsignedInt i = 0; i < fm.size(); ++i)
            if (fm[i])
               fm[i]->BufferState();
            else
               p[i]->BufferState();

         if (!TakeAStep())
            throw CommandException(
               "Propagate::Execute() Propagator Failed to Step\n");

         for (UnsignedInt i = 0; i < fm.size(); ++i)
         {
            // orbit related parameters use spacecraft for data
            #ifdef DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("Propagate::Execute() Updating epoch data\n");
            #endif
            if (fm[i])
            {
               elapsedTime[i] = fm[i]->GetTime();
               currEpoch[i] = baseEpoch[i] + elapsedTime[i] /
                  GmatTimeConstants::SECS_PER_DAY;
               currEpochGT[i] = baseEpochGT[i]; currEpochGT[i].AddSeconds(elapsedTime[i]);
            }
            else
            {
               elapsedTime[i] = p[i]->GetTime();
               currEpoch[i] = baseEpoch[i] + elapsedTime[i] /
                  GmatTimeConstants::SECS_PER_DAY;
               currEpochGT[i] = baseEpochGT[i]; currEpochGT[i].AddSeconds(elapsedTime[i]);
            }
            #ifdef DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("   %d  BaseEpoch = %.12lf elapsedTime = %.12lf "
                   "currEpoch size = %.12lf\n", i, baseEpoch[i],
                   elapsedTime[i], currEpoch[i]);
            #endif

            // Update spacecraft epoch, without argument the spacecraft epoch
            // won't get updated for consecutive Propagate command
            #ifdef DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("Propagate::Execute() Updating SpaceObjects; first vector "
                   "element = %.12lf\n", (psm[i]->GetState()->GetState())[0]);
            #endif
            if (fm[i])
            {
               if (fm[i]->HasPrecisionTime())
                  fm[i]->UpdateSpaceObjectGT(currEpochGT[i]);
               else
                  fm[i]->UpdateSpaceObject(currEpoch[i]);
            }
            else
            {
               if (p[i]->HasPrecisionTime())
                  p[i]->UpdateSpaceObjectGT(currEpochGT[i]);
               else
                  p[i]->UpdateSpaceObject(currEpoch[i]);
               
            }
         }

         // In single step mode, we're done!
         if (singleStepMode)
         {
            #ifdef DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("Propagate::Breaking for Single Step\n");
            #endif

            /*
             * We are not publishing the final point when running in single step
             * mode except for publishing to ephemeris files; this code does that,
             * but we end up with repeated points in other subscribers if the ephem
             * only setting is not used (dsetting |direction| = 2) because the last
             * point from one step is the first point in the next.  GMT-6110
             * addresses the possibility of publishing everywhere, as we do for
             * stop mode propagation.
             *
             * The Publisher handles the repeated point issue by only sending step
             * mode data to EphemerisFile subscribers.
             */

            for (Integer idx = 0; idx < pubdataSize; ++idx)
               pubdata[idx] = GmatMathConstants::QUIET_NAN;

            if (hasPrecisionTime)
               pubdata[0] = currEpochGT[0].GetMjd();
            else
               pubdata[0] = currEpoch[0];

            //// For each PropSetup, fill the appropriate array elements
            //Integer index = 1;
            //Integer size = 0;
            //for (UnsignedInt i = 0; i < propagators.size(); ++i)
            //{
            //   j2kState = p[i]->GetJ2KState();
            //   size = p[i]->GetDimension();
            //   memcpy(&pubdata[index], j2kState, size*sizeof(Real));
            //   index += size;
            //}

            // Prepare data to buffer for publishing
            //Integer len = 1;
            PrepareElementDataToPublish(pubdata, pubdataSize, pubdataElements, covStartIndex, attStartIndex);

            #ifdef DEBUG_PUBLISH_DATA
               if (fabs(pubdata[1]) + fabs(pubdata[2]) + fabs(pubdata[3]) < 100.0)
                  MessageInterface::ShowMessage(">>>>>>>>>>>>> Exec: BAD DATA BEING PUBLISHED!!! <<<<<<<<<<<<<<\n");
               MessageInterface::ShowMessage
                     ("***Propagate::Execute() SingleStep '%s' publishing current "
                      "%d data to stream %d, 1st data = [%.12lf %.12lf %.12lf "
                      "%.12lf %.12lf %.12lf %.12lf]\n",
                      GetGeneratingString(Gmat::NO_COMMENTS).c_str(), dim+1,
                      streamID, pubdata[0], pubdata[1], pubdata[2], pubdata[3],
                      pubdata[4], pubdata[5], pubdata[6]);
               if (fabs(pubdata[1]) + fabs(pubdata[2]) + fabs(pubdata[3]) < 100.0)
                  MessageInterface::ShowMessage(">>>>>>>>>>>>>  Exec: BAD DATA WAS PUBLISHED!!!  <<<<<<<<<<<<<<\n");
            #endif

            // |Direction| is 2 here to indicate only ephems should get data
            // This is the code to only publish to ephems in step mode.
            //
            // Remove the "2.0 * " piece to implement GMT-6110.

            //publisher->Publish(this, streamID, pubdata, dim+1, 2.0 * direction);
            publisher->Publish(this, streamID, pubdata, expandedDim, 2.0 * direction);

            // Set segment orbit color (LOJ: 2013.12.16 Added for propagation segment color)
            publisher->SetSegmentOrbitColor(this, overrideSegmentColor, segmentOrbitColor, fullSatList);
            
            break;
         }

         CheckStopConditions(epochID);

         if (!stopCondMet)
         {
            // No longer need to check stopping conditions at the first step
            checkFirstStep = false;

            // Publish the data here
            for (Integer idx = 0; idx < pubdataSize; ++idx)
               pubdata[idx] = GmatMathConstants::QUIET_NAN;

            if (hasPrecisionTime)
               pubdata[0] = currEpochGT[0].GetMjd();
            else
               pubdata[0] = currEpoch[0];

            //// For each PropSetup, fill the appropriate array elements
            //Integer index = 1;
            //Integer size = 0;
            //for (UnsignedInt i = 0; i < propagators.size(); ++i)
            //{
            //   j2kState = p[i]->GetJ2KState();
            //   size = p[i]->GetDimension();
            //   memcpy(&pubdata[index], j2kState, size*sizeof(Real));
            //   index += size;
            //}

            // Prepare data to buffer to publish
            //Integer len = 1;
            PrepareElementDataToPublish(pubdata, pubdataSize, pubdataElements, covStartIndex, attStartIndex);

            #ifdef DEBUG_PUBLISH_DATA
               MessageInterface::ShowMessage
                     ("Propagate::Execute() '%s' publishing current %d data to "
                      "stream %d, data = [%f ",
                      GetGeneratingString(Gmat::NO_COMMENTS).c_str(), dim+1,
                      streamID, pubdata[0]);
               for (Integer i = 1; i < pubdataSize; ++i)
                  MessageInterface::ShowMessage(" %f", pubdata[i]);
               MessageInterface::ShowMessage("]\n");
            #endif

            // Set segment orbit color (LOJ: 2013.11.29 Added for propagation segment color)
            publisher->SetSegmentOrbitColor(this, overrideSegmentColor, segmentOrbitColor, fullSatList);
            // Publish
            //publisher->Publish(this, streamID, pubdata, dim+1, direction);
            publisher->Publish(this, streamID, pubdata, expandedDim, direction);
            if (createNewSegmentsNow == true)
            {
               CreateNewSegments(currEpoch[0]);
            }
         }
         else
         {
            #ifdef DEBUG_STOPPING_CONDITIONS
               MessageInterface::ShowMessage("Stopping condition met\n");
            #endif

            stopInterval = 0.0;
            for (UnsignedInt i = 0; i < fm.size(); ++i)
            {
               Real timestep = p[i]->GetStepTaken();
               if (fabs(timestep) > fabs(stopInterval))
                  stopInterval = timestep;

               switch (currentMode)
               {
                  case SYNCHRONIZED:
                     if (fm[0] != NULL)
                        elapsedTime[i] = fm[0]->GetTime();
                     else
                        elapsedTime[i] = p[0]->GetTime();
                     if (fm[i] != NULL)
                        fm[i]->SetTime(elapsedTime[i]);
                     else
                        p[i]->SetTime(elapsedTime[i]);
                     break;

                  case INDEPENDENT:
                  default:
                     if (fm[i] != NULL)
                        elapsedTime[i] = fm[i]->GetTime();
                     else
                        elapsedTime[i] = p[i]->GetTime();
                     break;
               }

               currEpoch[i] = baseEpoch[i] +
                  elapsedTime[i] / GmatTimeConstants::SECS_PER_DAY;
               currEpochGT[i] = baseEpochGT[i]; currEpochGT[i].AddSeconds(elapsedTime[i]);
            }

            #ifdef DEBUG_STOPPING_CONDITIONS
               MessageInterface::ShowMessage("   Prop #, baseEpoch, currEpoch, "
                     "elapsedTime\n");
               for (UnsignedInt i = 0; i < fm.size(); ++i)
                  MessageInterface::ShowMessage("      %d, %.12lf, %.12lf, "
                        "%.12lf\n", i, baseEpoch[i], currEpoch[i],
                        elapsedTime[i]);
            #endif

            stepBrackets[1] = stopInterval;

            #ifdef DEBUG_EPOCH_SYNC
               for (UnsignedInt i = 0; i < fm.size(); ++i)
                  MessageInterface::ShowMessage(
                     "   SC MET!  Force model[%d] has base epoch %16.11lf, "
                     "time dt = %.11lf, elapsedTime = %.11lf\n", i,
                     baseEpoch[i], fm[i]->GetTime(), elapsedTime[i]);
            #endif

            #ifdef DEBUG_EPOCH_UPDATES
               MessageInterface::ShowMessage("StopStep = %15.11lf\n",
                  stopInterval);
            #endif
         }

         #if DEBUG_PROPAGATE_EXE
            MessageInterface::ShowMessage(
               "Propagate::Execute() intermediate; epoch = %f\n", currEpoch[0]);
         #endif

         // Periodically see if the user has stopped the run
         ++checkCount;
         if ((checkCount == interruptCheckFrequency) && !stopCondMet)
         {
            inProgress = true;
            return true;
         }

         #ifdef DEBUG_EPOCH_SYNC
            for (UnsignedInt i = 0; i < fm.size(); ++i)
               MessageInterface::ShowMessage("   Force model[%d] has base "
                  "epoch %16.11lf, time dt = %.11lf\n",
                  i, baseEpoch[i], fm[i]->GetTime());
         #endif
      }
         #ifdef DEBUG_EPOCH_SYNC
            MessageInterface::ShowMessage("Nominal steps Finished\n");
         #endif
   }
   catch (BaseException &ex)
   {
      // Use exception to remove Visual C++ warning
      ex.GetMessageType();
      inProgress = false;
      throw;
   }

   #ifdef DEBUG_EPOCH_UPDATES
      fm[0]->ReportEpochData();
   #endif

   inProgress = false;
   if (!singleStepMode)
   {
      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         // Propagator::RevertSpaceObject already checks and reverts fm if present, so just call p here
         p[i]->RevertSpaceObject();

         // For synchronized propagation, the epochs can get out of sync here
         // if the stopping condition was applied to a later PropSetup.  The
         // Cartesian state is okay; it's just an issue with the epoch because
         // of the initial pass through the cubic spline.  Reset the epochs to
         // correct this issue.
         if (currentMode == SYNCHRONIZED)
            for (UnsignedInt i = 1; i < fm.size(); ++i)
               if (fm[i] != NULL)
               {
                  if (fm[0] != NULL)
                     fm[i]->SetTime(fm[0]->GetTime());
                  else
                     fm[i]->SetTime(p[0]->GetTime());
               }
      }

      TakeFinalStep(epochID, trigger);

      // reset the stopping conditions so that scanning starts over
      for (UnsignedInt i=0; i<stopWhen.size(); i++)
         stopWhen[i]->Reset();
   }
   else
   {
      // clear first step stopping condition flags
      for (ObjectArray::iterator i = sats.begin();
           i != sats.end(); ++i)
         ((SpaceObject*)(*i))->ClearLastStopTriggered();
   }

   // Clear the flag set by AttitudeAffectsDynamics flag in the Spacecraft
   for (std::vector<GmatBase *>::const_iterator obj = sats.begin();
      obj != sats.end(); ++obj)
   {
      if ((*obj)->IsOfType("Spacecraft"))
      {
         Spacecraft* s = (Spacecraft*)(*obj);
         s->AttitudeAffectsDynamics(false);
      }
   }

   #ifdef DEBUG_EPOCH_UPDATES
      if (fm[0]);
         fm[0]->ReportEpochData();
   #endif

   ClearTransientForces();
   // Only build command summary if not in single step mode
   if (!singleStepMode)
      BuildCommandSummary(true);

   return true;
}


//------------------------------------------------------------------------------
// bool TakeAStep(Real propStep)
//------------------------------------------------------------------------------
/**
 * Advances each of the contained PropSetups by one step.
 *
 * @param propStep The requested size of the step.
 *
 * @return true if the step succeeded.
 */
//------------------------------------------------------------------------------
bool Propagate::TakeAStep(Real propStep)
{
   bool retval = false;
   Real stepToTake;

   #ifdef DEBUG_FIXED_STEP
      std::vector<ODEModel *>::iterator fmod = fm.begin();
   #endif

   std::vector<Propagator*>::iterator current = p.begin();

   if (propStep == 0.0)
   {
      switch (currentMode)
      {
         case INDEPENDENT:
            // Advance each propagator individually, without regard for the
            // epochs of the others
            #ifdef DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("Propagate::TakeAStep() running in INDEPENDENT mode\n");
            #endif
            while (current != p.end())
            {
               (*current)->FindTimeStep();
               if (!(*current)->Step())
                  // Check if Failure is due to Finite Burn Segmenting
                  if ((*current)->GetHasFiniteBurn())
                  {
                     Real fullStep = (*current)->GetStepSize();
                     Real totalStepTaken = (*current)->GetStepTaken();
                     if (CreateNewSegments(currEpochGT[0].GetMjd()))
                     {
                        if ((*current)->GetRealParameter("ProcessNoiseTimeStep") != 0)
                        {
                           if ((*current)->GetRealParameter("ProcessNoiseTimeStep") == (*current)->GetRealParameter("TimeToNextNoiseStep"))
                           {
                              (*current)->SetRealParameter("TimeToNextNoiseStep", (fullStep - totalStepTaken));
                           }
                           else
                           {
                              (*current)->SetRealParameter("TimeToNextNoiseStep", (*current)->GetRealParameter("TimeToNextNoiseStep") + (fullStep - totalStepTaken));
                           }
                        }
                        (*current)->SetStepSize(fullStep);
                        (*current)->SetFollowUpStep(false);
                     }
                     else if ((*current)->GetFollowUpStep() == true)
                     {
                        if ((*current)->PropagatesForward())
                        {
                           while (fullStep > totalStepTaken)
                           {
                              (*current)->SetStepSize(fullStep - totalStepTaken);
                              if (!(*current)->Step())
                              {
                                 CreateNewSegments(currEpochGT[0].GetMjd());
                              }
                              totalStepTaken += (*current)->GetStepTaken();
                           }
                        }
                        else
                        {
                           while (fullStep < totalStepTaken)
                           {
                              (*current)->SetStepSize(fullStep - totalStepTaken);
                              if (!(*current)->Step())
                              {
                                 CreateNewSegments(currEpochGT[0].GetMjd());
                              }
                              totalStepTaken += (*current)->GetStepTaken();
                           }

                        }

                        (*current)->SetStepSize(totalStepTaken);
                        (*current)->SetFollowUpStep(false);
                     }
                  }
                  else
                  {
                     throw CommandException(
                        "Propagator failed to take a good step\n");
                  }
               ++current;
            }
            retval = true;
            break;

         case SYNCHRONIZED:
            // This mode advances the first propagator, and then brings the
            // others up to the epoch of that first one.
            #ifdef DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("Propagate::TakeAStep() running in SYNCHRONIZED mode\n");
            #endif
            (*current)->FindTimeStep();
            if (!(*current)->Step())
               throw CommandException("Initial synchronized Propagator failed "
                                      "to take a good step\n");
            stepToTake = (*current)->GetStepTaken();
            ++current;
            while (current != p.end())
            {
               if (!(*current)->Step(stepToTake))
                  throw CommandException("Propagator failed to take a good "
                                         "synchronized step\n");
               ++current;
            }
            retval = true;
            break;

         default:
            #ifdef DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("Propagate::TakeAStep() running in undefined mode "
                  "(mode = %d)\n", currentMode);
            #endif
            retval = false;
            break;
      }
      #ifdef DEBUG_PROPAGATE_EXE
         MessageInterface::ShowMessage
            ("Propagate::TakeAStep() Step Taken\n");
      #endif
   }
   else
   {
      // Step all of the propagators by the input amount
      while (current != p.end())
      {
         #ifdef DEBUG_FIXED_STEP
            MessageInterface::ShowMessage("Stepping '%s' by %le seconds from "
               "epoch = %16.11lf\n", (*current)->GetName().c_str(), propStep,
               (*fmod)->GetRealParameter((*fmod)->GetParameterID("Epoch")));

            Integer fmSize = (*fmod)->GetDimension();
            MessageInterface::ShowMessage("Fmod has dim = %d\n", fmSize);
            MessageInterface::ShowMessage("   Pre Prop:  ");

            Real *fmState = (*fmod)->GetState();
            for (Integer q = 0; q < fmSize; ++q)
               MessageInterface::ShowMessage(" %.12lf", fmState[q]);
            MessageInterface::ShowMessage("\n");
         #endif

         if (!(*current)->Step(propStep))
         {
            std::stringstream sizebuffer;
            sizebuffer << propStep;
            sizebuffer.precision(15);
            throw CommandException("In Propagate::TakeAStep, Propagator " + 
               (*current)->GetName() +
               " failed to take a good final step (size = " + sizebuffer.str() + ")\n");
         }


         #ifdef DEBUG_FIXED_STEP
            MessageInterface::ShowMessage("   Stepped to epoch = %16.11lf\n",
               (*fmod)->GetRealParameter((*fmod)->GetParameterID("Epoch")));
            MessageInterface::ShowMessage("   Post Prop: ");

            for (Integer q = 0; q < fmSize; ++q)
               MessageInterface::ShowMessage(" %.12lf", fmState[q]);
            MessageInterface::ShowMessage("\n");

            ++fmod;
         #endif

         ++current;
      }
      retval = true;
   }

   #ifdef DEBUG_PROPAGATE_STEPSIZE
      MessageInterface::ShowMessage("Prop step = %16.13lf\n",
         p[0]->GetStepTaken());
   #endif

   #ifdef DUMP_PLANET_DATA
      Real epoch = sats[0]->GetRealParameter("A1Epoch");
      Rvector6 planetrv;

      for (Integer h = 0; h < bodiesDefined; ++h)
      {
         if (body[h] != NULL)
         {
            planetrv = body[h]->GetState(epoch);
            planetData << (body[h]->GetName()) << "  " << epoch << "  "
                       << planetrv[0] << "  "
                       << planetrv[1] << "  "
                       << planetrv[2] << "  "
                       << planetrv[3] << "  "
                       << planetrv[4] << "  "
                       << planetrv[5] << "\n";
         }
      }
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// void CheckStopConditions(Integer epochID)
//------------------------------------------------------------------------------
/**
 * Checks the status of the stopping conditions.
 *
 * @param epochID The parameter ID associated with the epoch field.
 */
//------------------------------------------------------------------------------
void Propagate::CheckStopConditions(Integer epochID)
{
   //------------------------------------------
   // loop through StopCondition list
   //------------------------------------------
   #ifdef DEBUG_STOPPING_CONDITIONS
      try {
   #endif

      for (UnsignedInt i = 0; i < stopWhen.size(); i++)
      {
         // StopCondition need epoch for the Interpolator
         stopWhen[i]->SetRealParameter(stopCondEpochID,
            stopSats[i]->GetRealParameter(epochID));

         #ifdef DEBUG_STOPPING_CONDITIONS
            MessageInterface::ShowMessage("Evaluating \"%s\" Stopping "
                  "condition\n", stopWhen[i]->GetName().c_str());
         #endif

         if (stopWhen[i]->Evaluate())
         {
            #ifdef DEBUG_STOPPING_CONDITIONS
               MessageInterface::ShowMessage("\"%s\" evaluates true!\n",
                  stopWhen[i]->GetName().c_str());
            #endif

            stopInterval = stopWhen[i]->GetStopInterval();
            if (stopInterval == 0.0)
            {
               stopEpoch = stopWhen[i]->GetStopEpoch();
            }

            stopCondMet = true;
            if (stopTrigger < 0)
               stopTrigger = i;

            triggers.push_back(stopWhen[i]);

            #if DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("Propagate::CheckStopConditions() %s met for %d\n",
                   stopWhen[i]->GetName().c_str(), i);
            #endif

            #ifdef DEBUG_EPOCH_SYNC
               for (UnsignedInt i = 0; i < fm.size(); ++i)
                  MessageInterface::ShowMessage("   *** SC[%d] (%s) MET! ***\n",
                     i, stopWhen[i]->GetName().c_str());
            #endif
         }
         else if (checkFirstStep)
         {
            #ifdef DEBUG_FIRST_STEP_STOP
               MessageInterface::ShowMessage("%d: ", i);
            #endif

            // Turn condition back on
            stopWhen[i]->SkipEvaluation(false);
            if (CheckFirstStepStop(i))
            {
               #ifdef DEBUG_FIRST_STEP_STOP
                  MessageInterface::ShowMessage("***Continuing from %s\n",
                     stopWhen[i]->GetName().c_str());
               #endif

               stopInterval = stopWhen[i]->GetStopInterval();
               if (stopInterval == 0.0)
               {
                  stopEpoch = stopWhen[i]->GetStopEpoch();
               }
               stopCondMet = true;
               if (stopTrigger < 0)
                  stopTrigger = i;

               triggers.push_back(stopWhen[i]);
            }
         }
      }

   #ifdef DEBUG_STOPPING_CONDITIONS
      }
      catch (BaseException &ex) {
         MessageInterface::ShowMessage(
            "Propagate::PrepareToPropagate() Exception while evaluating stopping "
            "conditions\n");
         inProgress = false;
         throw;
      }
   #endif

   #ifdef DEBUG_EPOCH_SYNC
      for (UnsignedInt i = 0; i < stopWhen.size(); ++i)
         if (stopWhen[i]->Evaluate())
            MessageInterface::ShowMessage("   *** StopInterval[%d] = %.11lf\n",
               i, stopWhen[i]->GetStopInterval());
   #endif
}


//------------------------------------------------------------------------------
// bool CheckFirstStepStop(Integer i)
//------------------------------------------------------------------------------
/**
 * Method used during the first prop step to ensure that a stop encountered on
 * this step is not repeating the last stop encountered.
 *
 * @param i Index in the stopping condition list that needs to be checked
 *
 * @return true is the stop is a valid stop -- that is, if it is not a repeat of
 * the last stop -- and false if it is a repeat.
 */
//------------------------------------------------------------------------------
bool Propagate::CheckFirstStepStop(Integer i)
{
   #ifdef DEBUG_FIRST_STEP_STOP
      MessageInterface::ShowMessage(
         "CheckFirstStepStop checking %s; returning valid stop condition: %s\n",
         stopWhen[i]->GetName().c_str(),
         (stopSats[i]->WasLastStopTriggered(stopWhen[i]->GetName()) ?
          "false" : "true"));
   #endif

   if (stopSats[i]->WasLastStopTriggered(stopWhen[i]->GetName()))//;
   {
      // Only report as triggered if outside of the stop accuracy
      Real min, max, goal, temp;
      min = stopWhen[i]->GetStopValue();
      max = stopWhen[i]->GetStopParameter()->EvaluateReal();
      if (min > max)
      {
         temp = min;
         min = max;
         max = temp;
      }
      goal = stopWhen[i]->GetStopGoal();

      if (stopWhen[i]->IsCyclicParameter())
      {
         Real rangemin, rangemax;
         stopWhen[i]->GetRange(rangemin, rangemax);
         Real halfrange = (rangemax - rangemin)/2.0;
         min = stopWhen[i]->PutInRange(min, goal-halfrange, goal+halfrange);
         max = stopWhen[i]->PutInRange(max, goal-halfrange, goal+halfrange);
      }

      temp = fabs(goal - min);
      if (fabs(goal - max) < temp)
         temp = fabs(goal - max);

      // Only report true if outside of tolerance
      Real accuracy = (stopWhen[i]->IsTimeCondition() ? timeAccuracy :
                                                        firstStepTolerance);

      if (temp > accuracy)
         if ((goal > min) && (goal < max))
            return true;

      // Fill buffer data in the sc
      stopWhen[i]->UpdateBuffer();
   }

   return false;
}


//------------------------------------------------------------------------------
// void TakeFinalStep(Integer EpochID, Integer trigger)
//------------------------------------------------------------------------------
/**
 * Takes the final prop step based on data from the stopping conditions.
 *
 * @param epochID The parameter ID associated with the epoch field.
 * @param trigger Index indicating which stopping condition was met.
 */
//------------------------------------------------------------------------------
void Propagate::TakeFinalStep(Integer EpochID, Integer trigger)
{
   #ifdef DEBUG_FINAL_STEP
     MessageInterface::ShowMessage("*** Start of TakeFinalStep\n");
        MessageInterface::ShowMessage("      State data:\n");
     MessageInterface::ShowMessage("         time: %.12lf\n", p[0]->GetTime());
             MessageInterface::ShowMessage("         r:    [%.12lf %.12lf %.12lf]\n",
        p[0]->GetState()[0], p[0]->GetState()[1], p[0]->GetState()[2]);
   #endif

   // We've passed a stop condition, so remember that step size.  Include a 10%
   // safety factor.
   stepBrackets[1] = stopInterval * 1.1;

   #ifdef DEBUG_FINAL_STEP
      MessageInterface::ShowMessage("Propagate::TakeFinalStep stopping "
            "condition step brackets: [%.12lf <--> %.12lf]\n", stepBrackets[0],
            stepBrackets[1]);
   #endif

   #if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage(
         "Propagate::TakeFinalStep currEpoch = %f, stopEpoch = %f, "
         "elapsedTime = %f\n", currEpoch[0], stopEpoch, elapsedTime[0]);
   #endif

//   Real secsToStep = 1.0e99 * direction;
   Real secsToStep = stepBrackets[1] * direction;
   Real dt = 0.0;
   StopCondition *stopper = (triggers.size() > 0 ? triggers[0] : NULL);

   #ifdef DEBUG_EPOCH_SYNC
      MessageInterface::ShowMessage("Top of final step code:\n");
      for (UnsignedInt i = 0; i < fm.size(); ++i)
         if (fm[i])
            MessageInterface::ShowMessage(
               "   Force model[%d] has base epoch %16.11lf, time dt = %.11lf, "
               "stopInterval = %.12lf\n", i, baseEpoch[i], fm[i]->GetTime(),
               stopInterval);
         else
            MessageInterface::ShowMessage(
               "   Propagator[%d] has base epoch %16.11lf, time dt = %.11lf, "
               "stopInterval = %.12lf\n", i, baseEpoch[i], p[i]->GetTime(),
               stopInterval);
   #endif

   // Toggle propagators into final step mode
   /// @note This code should be removed if the minimum step is removed from
   /// the RK integrators
   for (std::vector<Propagator*>::iterator current = p.begin();
        current != p.end(); ++current)
      (*current)->SetAsFinalStep(true);

   // First save the spacecraft for later restoration
   for (UnsignedInt i = 0; i < fm.size(); ++i)
   {
      #if DEBUG_PROPAGATE_EXE
         MessageInterface::ShowMessage("   TakeFinalStep calling update with CurrentEpoch[%d] = %.12lf\n", i,
            currEpoch[i]);
      #endif

      if (fm[i])
      {
         if (fm[i]->HasPrecisionTime())
         {
            GmatTime gt = baseEpochGT[i]; gt.AddSeconds(fm[i]->GetTime());
            fm[i]->UpdateSpaceObjectGT(gt);
         }
         else
            fm[i]->UpdateSpaceObject(  // currEpoch[i]);
               baseEpoch[i] + fm[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
      }
      else
      {
         if (p[i]->HasPrecisionTime())
         {
            GmatTime gt = baseEpochGT[i]; gt.AddSeconds(p[i]->GetTime());
            p[i]->UpdateSpaceObjectGT(gt);
         }
         else
            p[i]->UpdateSpaceObject(  // currEpoch[i]);
               baseEpoch[i] + p[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
      }
   }
   BufferSatelliteStates(true);
   #ifdef DEBUG_FINAL_STEP
                MessageInterface::ShowMessage("   Buffered state data\n");
                MessageInterface::ShowMessage("      State data:\n");
                MessageInterface::ShowMessage("         time: %.12lf\n", p[0]->GetTime());
                MessageInterface::ShowMessage("         r:    [%.12lf %.12lf %.12lf]\n",
                p[0]->GetState()[0], p[0]->GetState()[1], p[0]->GetState()[2]);
   #endif

   #ifdef DEBUG_EPOCH_SYNC
      MessageInterface::ShowMessage("Prior to interpolation:\n");
      for (UnsignedInt i = 0; i < fm.size(); ++i)
         MessageInterface::ShowMessage(
            "   Force model[%d] has base epoch %16.11lf, time dt = %.11lf\n",
            i, baseEpoch[i], fm[i]->GetTime());
   #endif

   // Interpolate to get the stop epoch
   if (stopTrigger < 0)
      throw CommandException(
         "Stopping condition was not set for final step on the line \n" +
         GetGeneratingString(Gmat::SCRIPTING));

   for (std::vector<StopCondition*>::iterator i = triggers.begin();
        i != triggers.end(); ++i)
   {
      // Get estimated time to reach this stop condition, dt
      if ((*i)->IsTimeCondition())
      {
         dt = (*i)->GetStopEpoch();

         #ifdef DEBUG_PROPAGATE_STEPSIZE
            MessageInterface::ShowMessage("Stopping on time\n   current "
               "epoch = %.14lf\n   goal          = %.14lf\n   dt            = "
               "%.14lf\n   sc diff       = %.14lf\n   tolerance = %le\n",
               currEpoch[0], (*i)->GetStopGoal(), dt,
               (*i)->GetStopDifference(), timeAccuracy);
         #endif
      }
      else
      {
         dt = InterpolateToStop(*i);

         #ifdef DEBUG_PROPAGATE_STEPSIZE
            MessageInterface::ShowMessage(
               "Interpolated stop time = %.14lf\n", dt);
         #endif
      }

      // If dt is closer to current epoch, save this stop condition as trigger
      if (fabs(secsToStep) > fabs(dt))
      {
         secsToStep = dt;
         stopper = *i;
      }
   }

   #ifdef DEBUG_FINAL_STEP
      MessageInterface::ShowMessage("   First search -> dt = %.12lf\n", dt);
                MessageInterface::ShowMessage("      State data:\n");
                MessageInterface::ShowMessage("         time: %.12lf\n", p[0]->GetTime());
                MessageInterface::ShowMessage("         r:    [%.12lf %.12lf %.12lf]\n",
                p[0]->GetState()[0], p[0]->GetState()[1], p[0]->GetState()[2]);
        #endif

   #if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage(
         "Step = %.12lf sec, calculated off of %.12lf and  %.12lf\n",
         secsToStep, stopEpoch, currEpoch[trigger]);
   #endif

   // Perform stepsize rounding.  Note that the rounding precision can be set
   // by redefining the macro TIME_ROUNDOFF at the top of
   // PropagationEnabledCommand.hpp.  Set it to 0.0 to prevent rounding.
   //
   // Note that this code makes the final propagated state match the granularity
   // given by other software (aka STK)
   if (TIME_ROUNDOFF != 0.0)
      secsToStep = std::floor(secsToStep / TIME_ROUNDOFF + 0.5) * TIME_ROUNDOFF;

   #if defined DEBUG_PROPAGATE_STEPSIZE | defined DEBUG_PROPAGATE_DIRECTION
      MessageInterface::ShowMessage
         ("Propagate::TakeFinalStep secsToStep at stop = %16.10le\n",
          secsToStep);
   #endif
   #ifdef DEBUG_PROPAGATE_DIRECTION
      MessageInterface::ShowMessage
         ("   stopEpoch = %16.10lf\n   currEpoch = %16.10lf\n",
          stopEpoch, currEpoch[trigger]);
   #endif

   Real accuracy = (stopper->IsTimeCondition() ? timeAccuracy : stopAccuracy);

   // If we are not at the final state, move to it
   if (secsToStep != 0.0)
   {
      #if DEBUG_PROPAGATE_EXE
         MessageInterface::ShowMessage(
            "Propagate::TakeFinalStep: Step(%16.13le) from epoch = %16.10lf\n",
            secsToStep,
            (baseEpoch[0] + fm[0]->GetTime() / GmatTimeConstants::SECS_PER_DAY));
      #endif

      #ifdef DEBUG_EPOCH_SYNC
         MessageInterface::ShowMessage("Before final step:\n");
         for (UnsignedInt i = 0; i < fm.size(); ++i)
            MessageInterface::ShowMessage("   Force model[%d] has base "
               "epoch %16.11lf, time dt = %.11lf\n",
               i, baseEpoch[i], fm[i]->GetTime());
         MessageInterface::ShowMessage("   step by time %.11lf\n", secsToStep);
      #endif

      if (!TakeAStep(secsToStep))
         throw CommandException("Propagator Failed to Step fixed interval\n");

      #ifdef DEBUG_EPOCH_SYNC
         MessageInterface::ShowMessage("After final step:\n");
         for (UnsignedInt i = 0; i < fm.size(); ++i)
            MessageInterface::ShowMessage(
               "   Force model[%d] has base epoch %16.11lf, time dt = %.11lf\n",
               i, baseEpoch[i], fm[i]->GetTime());
      #endif

      #ifdef DEBUG_FINAL_STEP
         MessageInterface::ShowMessage("   After stepping to initial stop condition epoch\n");
         MessageInterface::ShowMessage("      State data:\n");
         MessageInterface::ShowMessage("         time: %.12lf\n", p[0]->GetTime());
         MessageInterface::ShowMessage("         r:    [%.12lf %.12lf %.12lf]\n",
               p[0]->GetState()[0], p[0]->GetState()[1], p[0]->GetState()[2]);
      #endif
      // Check the stopping accuracy
      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         if (fm[i])
         {
            if (fm[i]->HasPrecisionTime())
            {
               GmatTime gt = baseEpochGT[i]; gt.AddSeconds(fm[i]->GetTime());
               fm[i]->UpdateSpaceObjectGT(gt);
            }
            else
               fm[i]->UpdateSpaceObject(
                  baseEpoch[i] + fm[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
         }
         else
         {
            if (p[i]->HasPrecisionTime())
            {
               GmatTime gt = baseEpochGT[i]; gt.AddSeconds(p[i]->GetTime());
               p[i]->UpdateSpaceObjectGT(gt);
            }
            else
               p[i]->UpdateSpaceObject(
                  baseEpoch[i] + p[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
         }
      }

      stopper->Evaluate();

      if (fabs(stopper->GetStopDifference()) > accuracy)
      {
         #ifdef DEBUG_STOPPING_CONDITIONS
            MessageInterface::ShowMessage("   Using accuracy of %.12f\n", accuracy);
            MessageInterface::ShowMessage("   Stop condition %s missed by %le; "
               "refining step \n", stopper->GetName().c_str(),
               stopper->GetStopDifference());
            MessageInterface::ShowMessage(
               "   When stepping %15.10lf secs, parameter has value %.9le\n",
               secsToStep, stopper->GetStopParameter()->EvaluateReal());
         #endif

         if (stopper->IsTimeCondition())
         {
            MessageInterface::ShowMessage("**** WARNING **** For the line \""
                  "%s\" the achieved stop is outside of the stopping "
                  "tolerance (%le) for time based stops; the difference from "
                  "the desired value is %le\n", 
                  GetGeneratingString(Gmat::NO_COMMENTS).c_str(), accuracy, 
                  fabs(stopper->GetStopDifference()));
         }
         else
         {
            // The interpolated step was not close enough, so back it out
            BufferSatelliteStates(false);
            for (UnsignedInt i = 0; i < fm.size(); ++i)
            {
               // Back out the steps taken to build the ring buffer
               if (fm[i])
               {
                  fm[i]->UpdateFromSpaceObject();
                  fm[i]->SetTime(fm[i]->GetTime() - secsToStep);
               }
               else
               {
                  p[i]->UpdateFromSpaceObject();
                  p[i]->SetTime(p[i]->GetTime() - secsToStep);
               }
            }

            #ifdef DEBUG_FINAL_STEP
               MessageInterface::ShowMessage("   Finished backing out the spline generated step\n");
               MessageInterface::ShowMessage("      State data:\n");
               MessageInterface::ShowMessage("         time: %.12lf\n", p[0]->GetTime());
               MessageInterface::ShowMessage("         r:    [%.12lf %.12lf %.12lf]\n",
                     p[0]->GetState()[0], p[0]->GetState()[1], p[0]->GetState()[2]);
            #endif

            // Generate a better time step
            secsToStep = RefineFinalStep(secsToStep, stopper);

            // Perform stepsize rounding.  Note that the rounding precision can be
            // set by redefining the macro TIME_ROUNDOFF at the top of
            // PropagationEnabledCommand.hpp.  Set it to 0.0 to prevent rounding.
            //
            // Note that this code makes the final propagated state match the
            // granularity given by other software (aka STK)
            if (TIME_ROUNDOFF != 0.0)
               secsToStep = std::floor(secsToStep / TIME_ROUNDOFF + 0.5) *
                     TIME_ROUNDOFF;

            // If a refined step was needed, we still need to take it;
            // RefineFinalStep returns with the interpolated step backed out
            if (!TakeAStep(secsToStep))
               throw CommandException(
                  "Propagator Failed to Step fixed interval\n");

            for (UnsignedInt i = 0; i < psm.size(); ++i)
            {
               if (fm[i])
               {
                  if (fm[i]->HasPrecisionTime())
                  {
                     GmatTime gt = baseEpochGT[i]; gt.AddSeconds(fm[i]->GetTime());
                     fm[i]->UpdateSpaceObjectGT(gt);
                  }
                  else
                     fm[i]->UpdateSpaceObject(
                        baseEpoch[i] + fm[i]->GetTime() /
                        GmatTimeConstants::SECS_PER_DAY);
               }
               else
               {
                  if (p[i]->HasPrecisionTime())
                  {
                     GmatTime gt = baseEpochGT[i]; gt.AddSeconds(p[i]->GetTime());
                     p[i]->UpdateSpaceObjectGT(gt);
                  }
                  else
                     p[i]->UpdateSpaceObject(
                        baseEpoch[i] + p[i]->GetTime() /
                        GmatTimeConstants::SECS_PER_DAY);

               }
            }
            #ifdef DEBUG_STOPPING_CONDITIONS
               MessageInterface::ShowMessage(
                  "   Refined timestep of %15.10lf secs gives %.9le\n",
                  secsToStep, stopper->GetStopParameter()->EvaluateReal());
            #endif

            if (fabs(stopper->GetStopDifference()) > accuracy)
               MessageInterface::ShowMessage("**** WARNING **** For the line "
                     "\"%s\" the achieved stop is outside of the stopping "
                     "tolerance (%le); the difference from the desired value "
                     "is %le\n", GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
                     accuracy, fabs(stopper->GetStopDifference()));
         }
      }

      // Publish the final data point here
      for (Integer idx = 0; idx < pubdataSize; ++idx)
         pubdata[idx] = GmatMathConstants::QUIET_NAN;

      if (fm[0])
      {
         if (hasPrecisionTime)
         {
            GmatTime gt = baseEpochGT[0];
            gt.AddSeconds(fm[0]->GetTime());
            pubdata[0] = gt.GetMjd();
         }
         else
            pubdata[0] = baseEpoch[0] + fm[0]->GetTime() / GmatTimeConstants::SECS_PER_DAY;
      }
      else
      {
         if (hasPrecisionTime)
         {
            GmatTime gt = baseEpochGT[0];
            gt.AddSeconds(p[0]->GetTime());
            pubdata[0] = gt.GetMjd();
         }
         else
            pubdata[0] = baseEpoch[0] + p[0]->GetTime() / GmatTimeConstants::SECS_PER_DAY;
      }

      // Prepare data to buffer for publishing
      //memcpy(&pubdata[1], j2kState, dim*sizeof(Real));
      //Integer len = 1;
      PrepareElementDataToPublish(pubdata, pubdataSize, pubdataElements, covStartIndex, attStartIndex);

      #ifdef DEBUG_PUBLISH_DATA
         MessageInterface::ShowMessage
               ("Propagate::TakeFinalStep() '%s' publishing final %d data to "
                "stream %d, 1st data = %f\n",
                GetGeneratingString(Gmat::NO_COMMENTS).c_str(), dim+1, streamID,
                pubdata[0]);
      #endif

      //Integer index = 1, size;
      //for (UnsignedInt i = 0; i < propagators.size(); ++i)
      //{
      //   j2kState = p[i]->GetJ2KState();
      //   size = p[i]->GetDimension();
      //   memcpy(&pubdata[index], j2kState, size*sizeof(Real));
      //   index += size;
      //}
      
      // Set segment orbit color (LOJ: 2013.11.29 Added for propagation segment color)
      publisher->SetSegmentOrbitColor(this, overrideSegmentColor, segmentOrbitColor, fullSatList);

      // Publish
      //publisher->Publish(this, streamID, pubdata, dim+1, direction);
      publisher->Publish(this, streamID, pubdata, expandedDim, direction);

      #if DEBUG_PROPAGATE_EXE
         MessageInterface::ShowMessage
            ("Propagate::TakeFinalStep: Step(%16.13le) advanced to "
            "epoch = %16.10lf\n", secsToStep,
            (baseEpoch[0] + fm[0]->GetTime() / GmatTimeConstants::SECS_PER_DAY));
      #endif

      // FlushBuffers here causes to write EphemerisFile multiple data segments
      // when propagating inside a loop using  stop condition of Sat.TAIModJulian = stopTime.
      // Stop condition of Sat.ElapsedSecs = 60 has no effects.
      // So passed false for end of data block. (LOJ: 2010.10.22)
      publisher->FlushBuffers(false);

      #if DEBUG_PROPAGATE_EXE
         MessageInterface::ShowMessage
            ("Propagate::TakeFinalStep complete; epoch = %16.10lf\n",
             (baseEpoch[0] + fm[0]->GetTime() / GmatTimeConstants::SECS_PER_DAY));
      #endif
   }

   // Clear previous stop conditions from the spacecraft, and then store the
   // stop name in the spacecraft that triggered it
   for (ObjectArray::iterator it = sats.begin();
        it != sats.end(); ++it)
      ((SpaceObject*)(*it))->ClearLastStopTriggered();

   if (stopper != NULL)
   {
      // Save the stop condition and reset for next pass
      Integer stopperIndex = 0;

      Real howClose = fabs(stopper->GetStopDifference());
      // First step tolerance is one order of magnitude above stop accuracy.
      firstStepTolerance = (howClose > accuracy ?
            howClose : accuracy) * 10.0;

      #ifdef DEBUG_FIRST_STEP_STOP
         MessageInterface::ShowMessage("Final step; First step tolerance = %le,    "
               "achieved = %le, desired = %le\n", firstStepTolerance, howClose,
               accuracy);
      #endif

      for (std::vector<StopCondition*>::iterator i = stopWhen.begin();
            i != stopWhen.end(); ++i)
      {
         if ((*i == stopper) || (fabs((*i)->GetStopDifference()) <= accuracy))
         {
            #ifdef DEBUG_FIRST_STEP_STOP
               MessageInterface::ShowMessage(
                  "Setting stop name '%s' on sat '%s'\n",
                  stopper->GetName().c_str(),
                  stopSats[stopperIndex]->GetName().c_str());
            #endif

            stopSats[stopperIndex]->SetLastStopTriggered((*i)->GetName());
         }
         ++stopperIndex;
      }
      triggers.clear();
   }

   for (std::vector<StopCondition *>::iterator i = stopWhen.begin();
        i != stopWhen.end(); ++i)
   {
      if ((*i)->GetName() == "")
      {
         StopCondition *localSc = *i;
         stopWhen.erase(i);
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (localSc, localSc->GetName(), "Propagate::TakeFinalStep()",
             "deleting localSc");
         #endif
         delete localSc;
      }
   }

   // Toggle propagators out of final step mode
   /// @note This code should be removed if the minimum step is removed from
   /// the RK integrators
   for (std::vector<Propagator*>::iterator current = p.begin();
        current != p.end(); ++current)
      (*current)->SetAsFinalStep(false);
}


//------------------------------------------------------------------------------
// Real InterpolateToStop(StopCondition *sc)
//------------------------------------------------------------------------------
/**
 * Routine that drives the cubic spline, filling the ring buffer and
 * interpolating the time step needed to find the interval to the stop
 * condition.
 *
 * @param <sc> The stopping condition that is used for the interpolation.
 *
 * @return The time step to the stopping condition, as determined by the cubic
 * spline interpolator.
 */
//------------------------------------------------------------------------------
Real Propagate::InterpolateToStop(StopCondition *sc)
{
   #ifdef DEBUG_STOPPING_CONDITIONS
         MessageInterface::ShowMessage("Entered Propagate::"
               "InterpolateToStop\n");
   #endif
   // Now fill in the ring buffer
   Real ringStep = stopInterval / 4.0;
   Integer ringStepsTaken = 0;
   bool firstRingStep = true;
   bool stopIsBracketed = false;
   Real elapsedSeconds = 0.0;

   while ((!stopIsBracketed) && (ringStepsTaken < 8))
   {
      #ifdef DEBUG_STOPPING_CONDITIONS
         MessageInterface::ShowMessage("Taking ring step %d, step size = "
               "%.12lf\n", ringStepsTaken, ringStep);
      #endif
      // Take a fixed prop step
      if (!TakeAStep(ringStep))
         throw CommandException("Propagator Failed to Step fixed interval "
            "while filling ring buffer\n");
      elapsedSeconds += ringStep;

      // Update spacecraft for that step
      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         if (fm[i])
         {
            if (fm[i]->HasPrecisionTime())
            {
               GmatTime gt = baseEpochGT[i]; gt.AddSeconds(fm[i]->GetTime());
               fm[i]->UpdateSpaceObjectGT(gt);
            }
            else
               fm[i]->UpdateSpaceObject(
                  baseEpoch[i] + fm[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
         }
         else
         {
            if (p[i]->HasPrecisionTime())
            {
               GmatTime gt = baseEpochGT[i]; gt.AddSeconds(p[i]->GetTime());
               p[i]->UpdateSpaceObjectGT(gt);
            }
            else
               p[i]->UpdateSpaceObject(
                  baseEpoch[i] + p[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
         }
      }

      // Update the data in the stop condition
      sc->SetRealParameter(stopCondEpochID, elapsedSeconds);
      stopIsBracketed = sc->AddToBuffer(firstRingStep);

      ++ringStepsTaken;
      firstRingStep = false;
      #ifdef DEBUG_STOPPING_CONDITIONS
         MessageInterface::ShowMessage("   step = %.12lf, value = %.12lf\n",
               elapsedSeconds, sc->GetStopValue());
      #endif
   }

   // Now interpolate the epoch...
   stopEpoch = sc->GetStopEpoch();

   #if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage(
         "Propagate::TakeFinalStep set the stopEpoch = %.12lf\n", stopEpoch);
   #endif

   // ...and restore the spacecraft and force models
   BufferSatelliteStates(false);
   for (UnsignedInt i = 0; i < fm.size(); ++i)
   {
      if (fm[i])
      {
         fm[i]->UpdateFromSpaceObject();
         // Back out the steps taken to build the ring buffer
         fm[i]->SetTime(fm[i]->GetTime() - ringStepsTaken * ringStep);
      }
      else
      {
         p[i]->UpdateFromSpaceObject();
         // Back out the steps taken to build the ring buffer
         p[i]->SetTime(p[i]->GetTime() - ringStepsTaken * ringStep);
      }

      #if DEBUG_PROPAGATE_EXE
         if (fm[i])
            MessageInterface::ShowMessage(
               "Force model base Epoch = %.12lf  elapsedTime = %.12lf  "
               "net Epoch = %.12lf\n", baseEpoch[i], fm[i]->GetTime(),
               baseEpoch[i] + fm[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
         else
            MessageInterface::ShowMessage(
               "Propagator base Epoch = %.12lf  elapsedTime = %.12lf  "
               "net Epoch = %.12lf\n", baseEpoch[i], p[i]->GetTime(),
               baseEpoch[i] + p[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
      #endif
   }

   return stopEpoch;
}


//------------------------------------------------------------------------------
// Real RefineFinalStep(Real secsToStep, StopCondition *stopper)
//------------------------------------------------------------------------------
/**
 * Routine that refines the solution found by the cubic spline, by solving for
 * the stopping condition using secants until the step produced falls within the
 * desired accuracy.
 *
 * @param <secsToStep>  First guess at the duration needed for stopping.
 * @param <stopper>     The stopping condition used for the refinement.
 *
 * @return The time step to the stopping condition, as determined by the secant
 * solver.
 */
//------------------------------------------------------------------------------
Real Propagate::RefineFinalStep(Real secsToStep, StopCondition *stopper)
{
   #ifdef DEBUG_SECANT_DETAILS
      MessageInterface::ShowMessage("\nRefineFinalStep(%16.13lf) entered.\n",
            secsToStep);
   #endif

//   Real theMax = 0.0;
//   for (UnsignedInt i = 0; i < maxSteps.size(); ++i)
//      if (fabs(theMax) < fabs(maxSteps[i]))
//         theMax = maxSteps[i];
//
//   if (fabs(secsToStep) > fabs(theMax))
//      secsToStep = theMax;

   if (fabs(secsToStep) > fabs(stepBrackets[1]))
      return BisectFinalStep(stopper);

   bool closeEnough = false;
   bool nextTimeThrough = false;
   Integer attempts = 0;

   Real x[2], y[2], slope, target;
   Parameter *stopParam = stopper->GetStopParameter(),
             *targParam = stopper->GetGoalParameter();

   Integer cycleType = stopParam->GetCycleType();

   x[0] = x[1] = y[1] = 0.0;
   y[0] = stopParam->EvaluateReal();

   #ifdef DEBUG_SECANT_DETAILS
      MessageInterface::ShowMessage("InitialPoint: [%.12lf %.12lf]\n", x[0], y[0]);
   #endif

   if (stopper->IsTimeCondition())
   {
      // Handle time based stopping condition refinement
      Real prevStep = secsToStep;

      while ((attempts < 50) && !closeEnough)
      {
         // Restore the spacecraft and force models to the end state of the last step
         if (attempts > 0)
         {
            BufferSatelliteStates(false);
            for (UnsignedInt i = 0; i < fm.size(); ++i)
            {
               if (fm[i])
               {
                  fm[i]->UpdateFromSpaceObject();
                  fm[i]->SetTime(fm[i]->GetTime() - prevStep);
               }
               else
               {
                  p[i]->UpdateFromSpaceObject();
                  p[i]->SetTime(p[i]->GetTime() - prevStep);
               }
            }
         }

         if (!TakeAStep(secsToStep))
            throw CommandException("Unable to take a good step while searching "
               "for stopping step in command\n   \"" + GetGeneratingString() +
               "\"\n");

         // Update spacecraft for that step
         for (UnsignedInt i = 0; i < fm.size(); ++i)
         {
            if (fm[i])
            {
               if (fm[i]->HasPrecisionTime())
               {
                  GmatTime gt = baseEpochGT[i]; gt.AddSeconds(fm[i]->GetTime());
                  fm[i]->UpdateSpaceObjectGT(gt);
               }
               else
                  fm[i]->UpdateSpaceObject(
                     baseEpoch[i] + fm[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
            }
            else
            {
               if (p[i]->HasPrecisionTime())
               {
                  GmatTime gt = baseEpochGT[i]; gt.AddSeconds(p[i]->GetTime());
                  p[i]->UpdateSpaceObjectGT(gt);
               }
               else
                  p[i]->UpdateSpaceObject(
                     baseEpoch[i] + p[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
            }
         }

         if (targParam != NULL)
            target = targParam->EvaluateReal();
         else
            target = stopper->GetStopGoal();

         x[1] = secsToStep;
         y[1] = stopParam->EvaluateReal();

         #ifdef DEBUG_STOPPING_CONDITIONS
            if (fm[0])
               MessageInterface::ShowMessage(
                  "[%d] Time based secant target: %16.12le\n"
                  "     BaseEpoch: %16.9lf    fmTime: %16.9lf\n"
                  "     Secant data points:\n"
                  "        (%16.12le, %16.12le)\n"
                  "        (%16.12le, %16.12le)\n"
                  "     Secant timestep: %16.12lf\n",
                  attempts, target, baseEpoch[0], fm[0]->GetTime(), x[0], y[0],
                  x[1], y[1], secsToStep);
            else
               MessageInterface::ShowMessage(
                  "[%d] Time based secant target: %16.12le\n"
                  "     BaseEpoch: %16.9lf    fmTime: %16.9lf\n"
                  "     Secant data points:\n"
                  "        (%16.12le, %16.12le)\n"
                  "        (%16.12le, %16.12le)\n"
                  "     Secant timestep: %16.12lf\n",
                  attempts, target, baseEpoch[0], p[0]->GetTime(), x[0], y[0],
                  x[1], y[1], secsToStep);

         #endif

         if (fabs(target - y[1]) < timeAccuracy)
            closeEnough = true;
         else
         {
            prevStep = secsToStep;
            slope = (y[1] - y[0]) / (x[1] - x[0]);
            secsToStep = (target - y[0]) / slope;

            if (fabs(secsToStep) > fabs(stepBrackets[1]))
               return BisectFinalStep(stopper);
         }

         ++attempts;
      }
   }
   else
   {
      // Handle non-time based stopping condition refinement
      while (!closeEnough && (attempts < 50))
      {
         target = stopper->GetStopGoal();
         if (stopper->IsCyclicParameter())
         {
            y[0] = GetRangedParam(y[0], target, cycleType);
            y[1] = GetRangedParam(y[1], target, cycleType);
         }

         if (nextTimeThrough)
         {
            // Restore spacecraft and force models to end state of last step
            BufferSatelliteStates(false);
            for (UnsignedInt i = 0; i < fm.size(); ++i)
            {
               if (fm[i])
               {
                  fm[i]->UpdateFromSpaceObject();
                  fm[i]->SetTime(fm[i]->GetTime() - secsToStep);
               }
               else
               {
                  p[i]->UpdateFromSpaceObject();
                  p[i]->SetTime(p[i]->GetTime() - secsToStep);
               }
            }

            if (x[1] == x[0])
            {
               #ifdef DEBUG_SECANT_DETAILS
                  MessageInterface::ShowMessage("Infinite slope error!!!\n"
                        "[%2d] Secant target: %16.12le\n"
                        "   Secant data points:\n"
                        "      (%16.12le, %16.12le)\n"
                        "      (%16.12le, %16.12le)\n"
                        "   Secant timestep: %16.12lf\n",
                        attempts, target, x[0], y[0], x[1], y[1], secsToStep);
               #endif
               Real bisectStep = 0.0;
               try
               {
                  bisectStep = BisectToStop(stopper);
               }
               catch (BaseException &ex)
               {
                  MessageInterface::ShowMessage(
                        "Error found (%s) while bisecting after a zero slope "
                        "secant was detected.\n", ex.GetFullMessage().c_str());
                  throw;
               }
               if (bisectStep == 0.0)
               {
                  // Changed to a warning message
                  // throw CommandException("Error refining timestep for "
                  //       "Propagate command: infinite slope in secant and "
                  //       "bisection failed to stop on \"" +
                  //       stopper->GetName() + "\"; Exiting\n");
                  MessageInterface::ShowMessage("**** WARNING **** The "
                        "secant and bisection methods failed when attempting "
                        "to stop with tolerance %le  on stopping condition %s;"
                        "the achieved stopping condition error was %le\n",
                        stopAccuracy, stopper->GetName().c_str(),
                        fabs(stopper->GetStopDifference()));

                  break;
               }

               secsToStep = bisectStep;
               if (fabs(secsToStep) > fabs(stepBrackets[1]))
                  return BisectFinalStep(stopper);

               break;
            }
            slope = (y[1] - y[0]) / (x[1] - x[0]);
            if (slope == 0.0)
            {
               ++attempts;
               #ifdef DEBUG_SECANT_DETAILS
                  MessageInterface::ShowMessage("Zero slope error!!!\n"
                        "[%2d] Secant target: %16.12le\n"
                        "   Secant data points:\n"
                        "      (%16.12le, %16.12le)\n"
                        "      (%16.12le, %16.12le)\n"
                        "   Secant timestep: %16.12lf\n",
                        attempts, target, x[0], y[0], x[1], y[1], secsToStep);
               #endif

               Real bisectStep = 0.0;
               try
               {
                  bisectStep = BisectToStop(stopper);
               }
               catch (BaseException &ex)
               {
                  MessageInterface::ShowMessage(
                        "Error found (%s) while bisecting after a zero slope "
                        "secant was detected.\n", ex.GetFullMessage().c_str());
                  throw;
               }
               if (bisectStep == 0.0)
               {
                  // Changed to a warning message
                  // throw CommandException("Error refining timestep for "
                  //       "Propagate command: zero slope in secant and "
                  //       "bisection failed to stop on \"" +
                  //       stopper->GetName() + "\"; Exiting\n");
                  MessageInterface::ShowMessage("**** WARNING **** The "
                        "secant and bisection methods failed when attempting "
                        "to stop with tolerance %le  on stopping condition %s;"
                        "the achieved stopping condition error was %le\n",
                        stopAccuracy, stopper->GetName().c_str(),
                        fabs(stopper->GetStopDifference()));

                  break;
               }

               secsToStep = bisectStep;
               if (fabs(secsToStep) > fabs(stepBrackets[1]))
                  return BisectFinalStep(stopper);

               break;
            }
            secsToStep = x[1] + (target - y[1]) / slope;
            if (fabs(secsToStep) > fabs(stepBrackets[1]))
               return BisectFinalStep(stopper);

            #ifdef DEBUG_STOPPING_CONDITIONS
               MessageInterface::ShowMessage(
                     "[%2d] Secant target: %16.12le\n"
                     "   Secant data points:\n"
                     "      (%16.12le, %16.12le)\n"
                     "      (%16.12le, %16.12le)\n"
                     "   Secant timestep: %16.12lf\n",
                     attempts, target, x[0], y[0], x[1], y[1], secsToStep);
            #endif
         }
         else
         {
            if (stopper->IsCyclicParameter())
               y[0] = GetRangedParam(y[0], target, cycleType);
         }

         #ifdef DEBUG_STOPPING_CONDITIONS
            if (fm[0])
               MessageInterface::ShowMessage(
                     "Before step, param = %16.12lf, Stepping from %16.12lf by "
                     "%16.12lf, ", stopParam->EvaluateReal(), fm[0]->GetTime(),
                     secsToStep);
            else
               MessageInterface::ShowMessage(
                     "Before step, param = %16.12lf, Stepping from %16.12lf by "
                     "%16.12lf, ", stopParam->EvaluateReal(), p[0]->GetTime(),
                     secsToStep);
         #endif

         if (!TakeAStep(secsToStep))
            throw CommandException(
                  "Unable to take a good step while searching "
                  "for stopping step in command\n   \"" +
                  GetGeneratingString() + "\"\n");

         #ifdef DEBUG_STOPPING_CONDITIONS
            if (fm[0])
               MessageInterface::ShowMessage("\nAfter step, param = %16.12lf, "
                     "Stepped to %16.12lf\n", stopParam->EvaluateReal(),
                     fm[0]->GetTime());
            else
               MessageInterface::ShowMessage("\nAfter step, param = %16.12lf, "
                     "Stepped to %16.12lf\n", stopParam->EvaluateReal(),
                     p[0]->GetTime());
         #endif

         // Update spacecraft for that step
         for (UnsignedInt i = 0; i < fm.size(); ++i)
         {
            if (fm[i])
            {
               if (fm[i]->HasPrecisionTime())
               {
                  GmatTime gt = baseEpochGT[i]; gt.AddSeconds(fm[i]->GetTime());
                  fm[i]->UpdateSpaceObjectGT(gt);
               }
               else
                  fm[i]->UpdateSpaceObject(
                     baseEpoch[i] + fm[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
            }
            else
            {
               if (p[i]->HasPrecisionTime())
               {
                  GmatTime gt = baseEpochGT[i]; gt.AddSeconds(p[i]->GetTime());
                  p[i]->UpdateSpaceObjectGT(gt);
               }
               else
                  p[i]->UpdateSpaceObject(
                     baseEpoch[i] + p[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
            }
         }

         #ifdef DEBUG_SECANT_DETAILS
            MessageInterface::ShowMessage(
                  "Secant run %d segment points: [%16.12lf %16.12lf], "
                  "[%16.12lf %16.12lf] => Estimate ", attempts, x[0], y[0],
                  x[1], y[1]);
         #endif

         // Buffer data for next iteration
         if (nextTimeThrough)
         {
            x[0] = x[1];
            y[0] = y[1];
         }
         else
            nextTimeThrough = true;

         // And store current results
         x[1] = secsToStep;
         y[1] = stopParam->EvaluateReal();
         if (stopper->IsCyclicParameter())
            y[1] = GetRangedParam(y[1], target, cycleType);

         #ifdef DEBUG_SECANT_DETAILS
            MessageInterface::ShowMessage("[%16.12lf %16.12lf]\n", x[1], y[1]);
         #endif

         // Check to see if accuracy is within tolerance
         if (fabs(stopper->GetStopDifference()) < stopAccuracy)
         {
            #ifdef DEBUG_STOPPING_CONDITIONS
               MessageInterface::ShowMessage("Success!  Stop diff = %.13lf, "
                     "accuracy = %.13lf\n", stopper->GetStopDifference(),
                     stopAccuracy);
            #endif
            closeEnough = true;
         }

         ++attempts;

         #ifdef DEBUG_STOPPING_CONDITIONS
            MessageInterface::ShowMessage(
               "   %d: secsToStep = %16.12lf, Stop diff = %16.12lf, "
               "Accuracy = %16.12lf\n", attempts, secsToStep,
               stopper->GetStopDifference(), stopAccuracy);
         #endif
      }
   }

   if (attempts == 50)
   {
      // Back out last step, then try bisection
      BufferSatelliteStates(false);
      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         if (fm[i])
         {
            fm[i]->UpdateFromSpaceObject();
            fm[i]->SetTime(fm[i]->GetTime() - secsToStep);
         }
         else
         {
            p[i]->UpdateFromSpaceObject();
            p[i]->SetTime(p[i]->GetTime() - secsToStep);
         }
      }

      Real bisectSecsToStep = BisectToStop(stopper);
      if (bisectSecsToStep != 0.0)
         secsToStep = bisectSecsToStep;
      else
         MessageInterface::ShowMessage(
               "WARNING: Failed to find a good stopping point for condition "
               "\"%s\" in 50 attempts, and bisection failed as well!\n",
               stopper->GetName().c_str());
   }

   // Restore the spacecraft and force models to the end state of the last step
   BufferSatelliteStates(false);
   for (UnsignedInt i = 0; i < fm.size(); ++i)
   {
      if (fm[i])
      {
         fm[i]->UpdateFromSpaceObject();
         fm[i]->SetTime(fm[i]->GetTime() - secsToStep);
      }
      else
      {
         p[i]->UpdateFromSpaceObject();
         p[i]->SetTime(p[i]->GetTime() - secsToStep);
      }
   }

   return secsToStep;
}


//------------------------------------------------------------------------------
// Real RefineFinalStep(Real secsToStep, StopCondition *stopper)
//------------------------------------------------------------------------------
/**
 * Routine that refines the solution found by the cubic spline, by solving for
 * the stopping condition using bisection for a bracketed step to find the
 * closest obtainable step to the stopping condition.
 *
 * This implementation replaces the previous method that used secants until the
 * step produced fell within the desired accuracy.  That approach occasionally
 * found secants that were essentially flat, producing bad steps.
 *
 * @param <secsToStep>  First guess at the duration needed for stopping.
 * @param <stopper>     The stopping condition used for the refinement.
 *
 * @return The time step to the stopping condition, as determined by this
 * method.
 */
//------------------------------------------------------------------------------
Real Propagate::BisectFinalStep(StopCondition *stopper)
{
   #ifdef DEBUG_SECANT_DETAILS
      MessageInterface::ShowMessage("\nRefineFinalStep(%16.13lf) entered.\n",
            secsToStep);
   #endif
   Real dt = (stepBrackets[1] - stepBrackets[0]) / 2.0;

   bool closeEnough = false;
   bool nextTimeThrough = false;

   Integer attempts = 0;
   Real x[2], y[2], target, thisValue, nextStep, startTime, prevStep = 0.0;

   Parameter *stopParam = stopper->GetStopParameter(),
             *targParam = stopper->GetGoalParameter();

   x[0] = stepBrackets[0];
   x[1] = stepBrackets[1];

   nextStep = stepBrackets[1];

   if (fm[0])
      startTime = fm[0]->GetTime();
   else
      startTime = p[0]->GetTime();

   // Set the LHS setting
   if (x[0] != 0.0)  // Not at the starting epoch
   {
      if (!TakeAStep(x[0]))
         throw CommandException("Unable to take a good step while searching "
            "for stopping step in command\n   \"" + GetGeneratingString() +
            "\"\n");

      prevStep = x[0];

      // Update spacecraft for that step
      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         if (fm[i])
         {
            if (fm[i]->HasPrecisionTime())
            {
               GmatTime gt = baseEpochGT[i]; gt.AddSeconds(fm[i]->GetTime());
               fm[i]->UpdateSpaceObjectGT(gt);
            }
            else
               fm[i]->UpdateSpaceObject(
                  baseEpoch[i] + fm[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
         }
         else
         {
            if (p[i]->HasPrecisionTime())
            {
               GmatTime gt = baseEpochGT[i]; gt.AddSeconds(p[i]->GetTime());
               p[i]->UpdateSpaceObjectGT(gt);
            }
            else
               p[i]->UpdateSpaceObject(
                  baseEpoch[i] + p[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
         }
      }

      y[0] = stopParam->EvaluateReal();

   }
   else  // Starting at dt[0] = 0.0
      y[0] = stopParam->EvaluateReal();

   Integer count = 0;

   // Set RHS data
   // Reset to start
   BufferSatelliteStates(false);
   for (UnsignedInt i = 0; i < fm.size(); ++i)
   {
      if (fm[i])
      {
         fm[i]->UpdateFromSpaceObject();
         fm[i]->SetTime(startTime);
      }
      else
      {
         p[i]->UpdateFromSpaceObject();
         p[i]->SetTime(startTime);
      }
   }

   if (!TakeAStep(x[1]))
      throw CommandException("Unable to take a good step while searching "
         "for stopping step in command\n   \"" + GetGeneratingString() +
         "\"\n");

   // Update spacecraft for that step
   for (UnsignedInt i = 0; i < fm.size(); ++i)
   {
      if (fm[i])
      {
         if (fm[i]->HasPrecisionTime())
         {
            GmatTime gt = baseEpochGT[i];
            gt.AddSeconds(fm[i]->GetTime());
            fm[i]->UpdateSpaceObjectGT(gt);
         }
         else
            fm[i]->UpdateSpaceObject(
               baseEpoch[i] + fm[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
      }
      else
      {
         if (p[i]->HasPrecisionTime())
         {
            GmatTime gt = baseEpochGT[i];
            gt.AddSeconds(p[i]->GetTime());
            p[i]->UpdateSpaceObjectGT(gt);
         }
         else
            p[i]->UpdateSpaceObject(
               baseEpoch[i] + p[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
      }
   }
   y[1] = stopParam->EvaluateReal();

   while (count < 51)
   {
      // Reset to start
      BufferSatelliteStates(false);
      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         if (fm[i])
         {
            fm[i]->UpdateFromSpaceObject();
            fm[i]->SetTime(startTime);
         }
         else
         {
            p[i]->UpdateFromSpaceObject();
            p[i]->SetTime(startTime);
         }
      }

      if (!TakeAStep(nextStep))
         throw CommandException("Unable to take a good step while searching "
            "for stopping step in command\n   \"" + GetGeneratingString() +
            "\"\n");

      // Update spacecraft for that step
      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         if (fm[i])
         {
            if (fm[i]->HasPrecisionTime())
            {
               GmatTime gt = baseEpochGT[i];
               gt.AddSeconds(fm[i]->GetTime());
               fm[i]->UpdateSpaceObjectGT(gt);
            }
            else
               fm[i]->UpdateSpaceObject(
                  baseEpoch[i] + fm[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
         }
         else
         {
            if (p[i]->HasPrecisionTime())
            {
               GmatTime gt = baseEpochGT[i];
               gt.AddSeconds(p[i]->GetTime());
               p[i]->UpdateSpaceObjectGT(gt);
            }
            else
               p[i]->UpdateSpaceObject(
                  baseEpoch[i] + p[i]->GetTime() / GmatTimeConstants::SECS_PER_DAY);
         }
      }

      if (targParam != NULL)
         target = targParam->EvaluateReal();
      else
         target = stopper->GetStopGoal();

      thisValue = stopParam->EvaluateReal();

      if (fabs(stopper->GetStopDifference()) < stopAccuracy)
      {
         #ifdef DEBUG_SECANT_DETAILS
            MessageInterface::ShowMessage("Solution found at iteration %d:  "
                  "[%.12le, %.12le]\n", count, nextStep, thisValue);

            MessageInterface::ShowMessage("   %.12le is close enough!\n", thisValue);
         #endif
         break;
      }

      // Setup for next iteration
      if (y[0] * thisValue >= 0.0)  // Start and current on same side of 0
      {
         x[0] = nextStep;
         y[0] = thisValue;
      }
      else                          // or not
      {
         x[1] = nextStep;
         y[1] = thisValue;
      }

      #ifdef DEBUG_BISECTION
         MessageInterface::ShowMessage("States at iteration %d:  [%.12le, %.12le],"
               " [%.12le, %.12le]\n", count, x[0], y[0], x[1], y[1]);
      #endif

      prevStep = nextStep;
      nextStep = x[0] + (x[1] - x[0]) / 2.0;
      ++count;
   }

//   throw PropagatorException("Stopping to evaluate things");

   return nextStep;
}

//------------------------------------------------------------------------------
// Real BisectToStop(StopCondition *stopper)
//------------------------------------------------------------------------------
/**
 * Bisection method used as a "last resort" to find stopping point.
 *
 * @param <stopper> Stopping condition we're using.
 */
//------------------------------------------------------------------------------
Real Propagate::BisectToStop(StopCondition *stopper)
{
   Integer attempts = 0, attemptsMax = 52;  // 52 bits in ANSI double
   bool closeEnough = false;
   Real secsToStep = stepBrackets[1];
   Real values[2], currentValue, previousValue;
   Real target = 0.0;       // Bogus initialization to clear a warning
   Real dt = stepBrackets[1] - stepBrackets[0];
   Real increasing = 1.0;

   Parameter *stopParam = stopper->GetStopParameter(),
             *targParam = stopper->GetGoalParameter();

   Integer cycleType = stopParam->GetCycleType();

   currentValue = values[0] = values[1] = stopParam->EvaluateReal();
   previousValue = currentValue - 1.0;  // All that matters is that they differ

   while ((attempts < attemptsMax) && !closeEnough)
   {
      if (attempts > 0)
      {
         BufferSatelliteStates(false);
         for (UnsignedInt i = 0; i < fm.size(); ++i)
         {
            if (fm[i])
            {
               fm[i]->UpdateFromSpaceObject();
               fm[i]->SetTime(fm[i]->GetTime() - secsToStep);
            }
            else
            {
               p[i]->UpdateFromSpaceObject();
               p[i]->SetTime(p[i]->GetTime() - secsToStep);
            }
         }

         dt *= 0.5;

         if (attempts == 1)
         {

            values[1] = currentValue;
            secsToStep = stepBrackets[0] + dt;
            if (stopper->IsCyclicParameter())
               values[0] = GetRangedParam(values[0], target, cycleType);

            if (values[1] < values[0])
               increasing = -1.0;
         }
         else
         {
            if (currentValue > target)
            {
               secsToStep -= increasing * dt;
               if (increasing > 0.0)
                  values[1] = currentValue;
               else
                  values[0] = currentValue;
            }
            else
            {
               secsToStep += increasing * dt;
               if (increasing > 0.0)
                  values[0] = currentValue;
               else
                  values[1] = currentValue;
            }
         }
      }

      if (!TakeAStep(secsToStep))
         throw CommandException("Unable to take a good step while searching "
            "for stopping step in command\n   \"" + GetGeneratingString() +
            "\"\n");

      // Update spacecraft for that step
      for (UnsignedInt i = 0; i < fm.size(); ++i)
      {
         if (fm[i])
         {
            if (fm[i]->HasPrecisionTime())
            {
               GmatTime gt = baseEpochGT[i]; gt.AddSeconds(fm[i]->GetTime());
               fm[i]->UpdateSpaceObjectGT(gt);
            }
            else
               fm[i]->UpdateSpaceObject(baseEpoch[i] + fm[i]->GetTime() /
                  GmatTimeConstants::SECS_PER_DAY);
         }
         else
         {
            if (p[i]->HasPrecisionTime())
            {
               GmatTime gt = baseEpochGT[i]; gt.AddSeconds(p[i]->GetTime());
               p[i]->UpdateSpaceObjectGT(gt);
            }
            else
               p[i]->UpdateSpaceObject(baseEpoch[i] + p[i]->GetTime() /
                  GmatTimeConstants::SECS_PER_DAY);
         }
      }

      if (targParam != NULL)
         target = targParam->EvaluateReal();
      else
         target = stopper->GetStopGoal();

      previousValue = currentValue;
      currentValue = stopParam->EvaluateReal();
      if (stopper->IsCyclicParameter())
      {
         currentValue = GetRangedParam(currentValue, target, cycleType);
      }

      #ifdef DEBUG_BISECTION_DETAILS
         if (attempts == 0)
         {
            MessageInterface::ShowMessage(
               "Bisection[%d]: Stop(%16.13lf) = %16.13lf\n\n"
               "           Stop(%16.13lf) = %16.13lf\n"
               "           Goal           = %16.13lf\n",
               attempts, stepBrackets[0], values[0], secsToStep, currentValue,
               target);
         }
         else
         {
            MessageInterface::ShowMessage(
               "Bisection[%d]: Stop(%16.13lf) = %16.13lf\n"
               "           Stop(%16.13lf) = %16.13lf\n\n"
               "           Stop(%16.13lf) = %16.13lf\n"
               "           Goal           = %16.13lf\n",
               attempts, stepBrackets[0], values[0], stepBrackets[1], values[1],
               secsToStep, currentValue, target);
         }
      #endif

      ++attempts;

      if (fabs(target - currentValue) < stopAccuracy)
            closeEnough = true;

      if (previousValue == currentValue)
      {

         MessageInterface::ShowMessage("The command \"%s\" cannot satisfy "
               "the stopping tolerance of \"%le\" for the stopping condition "
               "\"%s\".  The achieved accuracy is \"%.12lf\".\n",
               GetGeneratingString(Gmat::NO_COMMENTS).c_str(), stopAccuracy,
               stopper->GetName().c_str(), fabs(target - currentValue));

         closeEnough = true;
      }
   }

//   if (attempts == attemptsMax)
//      secsToStep = 0.0;

   return secsToStep;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
/**
 * Resets the Propagate command to an uninitialized state.
 */
//------------------------------------------------------------------------------
void Propagate::RunComplete()
{
   if (inProgress)
   {
      publisher->FlushBuffers();
      publisher->UnregisterPublishedData(this);
   }

   inProgress = false;
   hasFired = false;

   #ifdef DEBUG_FIRST_CALL
      firstStepFired = false;
   #endif

   // Clear transient forces(LOJ: 2009.05.07)
   ClearTransientForces();

   PropagationEnabledCommand::RunComplete();
}

//------------------------------------------------------------------------------
// GmatBase* GetClone(Integer cloneIndex = 0)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a clone so its attributes can be accessed
 *
 * @param cloneIndex The index into the clone array
 *
 * @return The clone pointer, or NULL if no clone exists
 */
//------------------------------------------------------------------------------
GmatBase* Propagate::GetClone(Integer cloneIndex)
{
   #ifdef DEBUG_CLONE_UPDATES
      MessageInterface::ShowMessage("Entered Propagate::GetClone(%d)\n",
            cloneIndex);
   #endif

   GmatBase *retPtr = NULL;

   /// todo: Handle the ancestor classes
   if ((cloneIndex >= 0) && (cloneIndex < (Integer)propagators.size()))
      retPtr = propagators[cloneIndex];

   return retPtr;
}


//------------------------------------------------------------------------------
// MOVED TO PROPAGATIONENABLEDCOMMAND
//------------------------------------------------------------------------------

////------------------------------------------------------------------------------
//// void AddTransientForce(StringArray *sats, ForceModel *p,
////       PropagationStateManager *propMan)
////------------------------------------------------------------------------------
///**
// * Passes transient forces into the ForceModel(s).
// *
// * @param sats The array of satellites used in the ForceModel.
// * @param p    The current ForceModel that is receiving the forces.
// * @param propMan PropagationStateManager for this PropSetup
// */
////------------------------------------------------------------------------------
//void Propagate::AddTransientForce(StringArray *satnames, ODEModel *p,
//      PropagationStateManager *propMan)
//{
//   #ifdef DEBUG_TRANSIENT_FORCES
//   MessageInterface::ShowMessage
//      ("Propagate::AddTransientForce() entered, ODEModel=<%p>,"
//       " transientForces=<%p>\n", p, transientForces);
//   #endif
//
//   // Find any transient force that is active and add it to the force model
//   StringArray satsThatManeuver, formationSatsThatManeuver, formsThatManeuver;
//   bool flagMultipleBurns = false;
//
//   for (std::vector<PhysicalModel*>::iterator i = transientForces->begin();
//        i != transientForces->end(); ++i)
//   {
//      StringArray tfSats = (*i)->GetRefObjectNameArray(Gmat::SPACECRAFT);
//
//      // Loop through the spacecraft that go with the force model, and see if
//      // they are in the spacecraft list for the current transient force
//      for (StringArray::iterator current = satnames->begin();
//           current != satnames->end(); ++current)
//      {
//         if (find(tfSats.begin(), tfSats.end(), *current) != tfSats.end())
//         {
//            #ifdef DEBUG_TRANSIENT_FORCES
//            MessageInterface::ShowMessage
//               ("   Adding transientForce <%p>'%s' to ODEModel\n", *i,
//                (*i)->GetName().c_str());
//            #endif
////            if (find(satsThatManeuver.begin(), satsThatManeuver.end(), *current)
////                  == satsThatManeuver.end())
//               p->AddForce(*i);
////            else
////               flagMultipleBurns = true;
//            if (find(satsThatManeuver.begin(), satsThatManeuver.end(), *current) == satsThatManeuver.end())
//                  satsThatManeuver.push_back(*current);
//            if ((*i)->DepletesMass())
//            {
//               propMan->SetProperty("MassFlow");
////               propMan->SetProperty("MassFlow",
////                     (*i)->GetRefObject(Gmat::SPACECRAFT, *current));
//               #ifdef DEBUG_TRANSIENT_FORCES
//                  MessageInterface::ShowMessage("   %s depletes mass\n",
//                        (*i)->GetName().c_str());
//               #endif
//            }
//            break;      // Avoid multiple adds
//         }
//         else
//         {
//            // Check to see if a Formation is masking the Spacecraft, and if so
//            // whine and stop
//            // Find the object that matches the current name
//            GmatBase *obj;
//            for (UnsignedInt i = 0; i < sats.size(); ++i)
//            {
//               if (sats[i]->GetName() == *current)
//               {
//                  obj = sats[i];
//
//                  #ifdef DEBUG_TRANSIENT_FORCES
//                     MessageInterface::ShowMessage("Checking %s\n",
//                           current->c_str());
//                  #endif
//
//                  if (obj->IsOfType(Gmat::FORMATION))
//                  {
//                     StringArray mansats =
//                           ((SpaceObject*)(obj))->GetManeuveringMembers();
//
//                     #ifdef DEBUG_TRANSIENT_FORCES
//                        MessageInterface::ShowMessage("***It's a Formation "
//                              "with %d maneuvering sats\n", mansats.size());
//                     #endif
//
//                     for (UnsignedInt i = 0; i < mansats.size(); ++i)
//                     {
//                        #ifdef DEBUG_TRANSIENT_FORCES
//                           MessageInterface::ShowMessage("   %d: %s is maneuvering\n",
//                                 i, mansats[i].c_str());
//                        #endif
//                        if (find(tfSats.begin(), tfSats.end(), mansats[i]) != tfSats.end())
//                        {
//                           if (find(formationSatsThatManeuver.begin(),
//                                 formationSatsThatManeuver.end(),
//                                 mansats[i]) == formationSatsThatManeuver.end())
//                              formationSatsThatManeuver.push_back(mansats[i]);
//                           if (find(formsThatManeuver.begin(),
//                                 formsThatManeuver.end(), obj->GetName()) ==
//                                       formsThatManeuver.end())
//                              formsThatManeuver.push_back(obj->GetName());
//                        }
//                     }
//                  }
//               }
//            }
//         }
//      }
//   }
//
//   #ifdef DEBUG_TRANSIENT_FORCES
//      MessageInterface::ShowMessage("Found %d sats that maneuver (outside of "
//            "formations):\n", satsThatManeuver.size());
//      for (UnsignedInt i = 0; i < satsThatManeuver.size(); ++i)
//         MessageInterface::ShowMessage("   %s\n", satsThatManeuver[i].c_str());
//   #endif
//
//   if (flagMultipleBurns)
//   {
//      StringArray duplicates;
//      for (StringArray::iterator name = satsThatManeuver.begin();
//            name != satsThatManeuver.end(); ++name)
//      {
//         if (find(name+1, satsThatManeuver.end(), *name) !=
//               satsThatManeuver.end())
//         {
//            if (find(duplicates.begin(), duplicates.end(), *name) ==
//                  duplicates.end())
//               duplicates.push_back(*name);
//         }
//      }
//      std::string errmsg = "The Spacecraft [";
//      for (UnsignedInt i = 0; i < duplicates.size(); ++i)
//      {
//         if (i != 0)
//            errmsg += ", ";
//         errmsg += duplicates[i];
//      }
//      if (duplicates.size() == 1)
//         errmsg += "] has ";
//      else
//         errmsg += "] have ";
//      errmsg += "more than one finite burn active, but GMAT only allows one "
//         "finite burn on a given spacecraft.";
//      throw CommandException(errmsg);
//   }
//
//   if (formationSatsThatManeuver.size() > 0)
//   {
//      std::string whatToSay, formToSay;
//      if (formationSatsThatManeuver.size() == 1)
//         whatToSay = "a maneuvering Spacecraft named \"" +
//                     formationSatsThatManeuver[0] + "\"";
//      else
//      {
//         whatToSay = "maneuvering Spacecraft named ";
//         for (UnsignedInt i = 0; i < formationSatsThatManeuver.size(); ++i)
//         {
//            if (i != 0)
//            {
//               if (formationSatsThatManeuver.size() > 2)
//                  whatToSay += ", ";
//               else
//                  whatToSay += " ";
//               if (i == formationSatsThatManeuver.size()-1)
//                  whatToSay += "and ";
//            }
//            whatToSay += "\"" + formationSatsThatManeuver[i] + "\"";
//         }
//      }
//
//      if (formsThatManeuver.size() == 1)
//         formToSay = "Formation \"" + formsThatManeuver[0] + "\" contains " +
//         whatToSay;
//      else
//      {
//         formToSay = "Formations ";
//         for (UnsignedInt i = 0; i < formsThatManeuver.size(); ++i)
//         {
//            if (i != 0)
//            {
//               if (formsThatManeuver.size() > 2)
//                  formToSay += ", ";
//               else
//                  formToSay += " ";
//               if (i == formsThatManeuver.size()-1)
//                  formToSay += "and ";
//            }
//            formToSay += "\"" + formsThatManeuver[i] + "\"";
//         }
//
//         formToSay += " contain " + whatToSay;
//      }
//
//      throw CommandException("The " + formToSay + ", but GMAT does not support "
//            "finite burn maneuvers inside of Formation objects.");
//   }
//
//   #ifdef DEBUG_TRANSIENT_FORCES
//      ODEModel *fm;
//      PhysicalModel *pm;
//
//      MessageInterface::ShowMessage(
//         "Propagate::AddTransientForces completed; force details:\n");
//      for (std::vector<PropSetup*>::iterator p = propagators.begin();
//           p != propagators.end(); ++p)
//      {
//         fm = (*p)->GetODEModel();
//         if (!fm)
//            throw CommandException("ODEModel not set in PropSetup \"" +
//                                   (*p)->GetName() + "\"");
//         MessageInterface::ShowMessage(
//            "   Forces in %s:\n", fm->GetName().c_str());
//         for (Integer i = 0; i < fm->GetNumForces(); ++i)
//         {
//            pm = fm->GetForce(i);
//            MessageInterface::ShowMessage(
//               "      %15s   %s\n", pm->GetTypeName().c_str(),
//               pm->GetName().c_str());
//         }
//      }
//   #endif
//}

//------------------------------------------------------------------------------
// MOVED TO PROPAGATIONENABLEDCOMMAND
//------------------------------------------------------------------------------

////------------------------------------------------------------------------------
//// void ClearTransientForce()
////------------------------------------------------------------------------------
///**
// * Removes transient forces from the ForceModel(s) after propagation.
// */
////------------------------------------------------------------------------------
//void Propagate::ClearTransientForces()
//{
//   #ifdef DEBUG_TRANSIENT_FORCES
//   MessageInterface::ShowMessage
//      ("Propagate::ClearTransientForces() entered, prop.size()=%d\n",
//       propagators.size());
//   #endif
//
//   ODEModel *fm;
//   PhysicalModel *pm;
//
//   // Loop through the forces in each force model, and remove transient ones
//   for (std::vector<PropSetup*>::iterator p = propagators.begin();
//        p != propagators.end(); ++p)
//   {
//      if ((*p)->GetPropagator()->UsesODEModel())
//      {
//         fm = (*p)->GetODEModel();
//         if (!fm)
//            throw CommandException("ForceModel not set in PropSetup \"" +
//                                   (*p)->GetName() + "\"");
//
//         #ifdef DEBUG_TRANSIENT_FORCES
//         MessageInterface::ShowMessage("   ODEModel=<%p>\n", fm);
//         #endif
//         for (Integer i = 0; i < fm->GetNumForces(); ++i)
//         {
//            pm = fm->GetForce(i);
//            #ifdef DEBUG_TRANSIENT_FORCES
//            MessageInterface::ShowMessage
//               ("      Checking if pm<%p>'%s' is transient\n", pm, pm->GetName().c_str());
//            #endif
//            if (pm->IsTransient())
//            {
//               #ifdef DEBUG_TRANSIENT_FORCES
//               MessageInterface::ShowMessage("   calling fm->DeleteForce() for %p\n", pm);
//               #endif
//               fm->DeleteForce(pm->GetName());
//               --i;
//            }
//         }
//      }
//   }
//
//   #ifdef DEBUG_TRANSIENT_FORCES
//      MessageInterface::ShowMessage(
//         "Propagate::ClearTransientForces completed; force details:\n");
//      for (std::vector<PropSetup*>::iterator p = propagators.begin();
//           p != propagators.end(); ++p)
//      {
//         if ((*p)->GetPropagator()->UsesODEModel())
//         {
//            fm = (*p)->GetODEModel();
//            if (!fm)
//               throw CommandException("ForceModel not set in PropSetup \"" +
//                                      (*p)->GetName() + "\"");
//            #ifdef DEBUG_TRANSIENT_FORCES
//            MessageInterface::ShowMessage("   ODEModel=<%p>\n", fm);
//            #endif
//            MessageInterface::ShowMessage(
//               "      Forces in %s:\n", fm->GetName().c_str());
//            for (Integer i = 0; i < fm->GetNumForces(); ++i)
//            {
//               pm = fm->GetForce(i);
//               MessageInterface::ShowMessage(
//                   "      %15s   %s\n", pm->GetTypeName().c_str(),
//                   pm->GetName().c_str());
//             }
//         }
//      }
//   #endif
//}

//Moved to PropagationEnabledCommand for GMT-5101 fix (LOJ: 2015.05.14)
// //------------------------------------------------------------------------------
// // void SetNames(const std::string& name, StringArray& owners,
// //               StringArray& elements)
// //------------------------------------------------------------------------------
// /**
//  * Sets the parameter names used when publishing Spacecraft data.
//  *
//  * @param <name>     Name of the Spacecraft that is referenced.
//  * @param <owners>   Array of published data identifiers.
//  * @param <elements> Individual elements of the published data.
//  */
// //------------------------------------------------------------------------------
// void Propagate::SetNames(const std::string& name, StringArray& owners,
//                          StringArray& elements)
// {
//    // Add satellite labels
//    for (Integer i = 0; i < 6; ++i)
//       owners.push_back(name);       // X, Y, Z, Vx, Vy, Vz

//    elements.push_back(name+".X");
//    elements.push_back(name+".Y");
//    elements.push_back(name+".Z");
//    elements.push_back(name+".Vx");
//    elements.push_back(name+".Vy");
//    elements.push_back(name+".Vz");
// }


////------------------------------------------------------------------------------
//// void AddToBuffer(SpaceObject *so)
////------------------------------------------------------------------------------
///**
// * Adds satellites and formations to the state buffer.
// *
// * @param <so> The SpaceObject that is added.
// */
////------------------------------------------------------------------------------
//void Propagate::AddToBuffer(SpaceObject *so)
//{
//   #ifdef DEBUG_STOPPING_CONDITIONS
//      MessageInterface::ShowMessage("Buffering states for '%s'\n",
//         so->GetName().c_str());
//   #endif
//
//   if (so->IsOfType(Gmat::SPACECRAFT))
//   {
//      Spacecraft *clonedSat = (Spacecraft *)(so->Clone());
//      satBuffer.push_back(clonedSat);
//      #ifdef DEBUG_MEMORY
//      MemoryTracker::Instance()->Add
//         (clonedSat, clonedSat->GetName(), "Propagate::AddToBuffer()",
//          "(Spacecraft *)(so->Clone())");
//      #endif
//      //satBuffer.push_back((Spacecraft *)(so->Clone()));
//   }
//   else if (so->IsOfType(Gmat::FORMATION))
//   {
//      Formation *form = (Formation*)so;
//      Formation *clonedForm = (Formation *)(so->Clone());
//      #ifdef DEBUG_MEMORY
//      MemoryTracker::Instance()->Add
//         (clonedForm, clonedForm->GetName(), "Propagate::AddToBuffer()",
//          "(Formation *)(so->Clone())");
//      #endif
//      //formBuffer.push_back((Formation *)(so->Clone()));
//      formBuffer.push_back(clonedForm);
//      StringArray formSats = form->GetStringArrayParameter("Add");
//
//      for (StringArray::iterator i = formSats.begin(); i != formSats.end(); ++i)
//         AddToBuffer((SpaceObject *)(FindObject(*i)));
//   }
//   else
//      throw CommandException("Object " + so->GetName() + " is not either a "
//         "Spacecraft or a Formation; cannot buffer the object for propagator "
//         "stopping conditions.");
//}
//
//
////------------------------------------------------------------------------------
//// void EmptyBuffer()
////------------------------------------------------------------------------------
///**
// * Cleans up the satellite state buffer.
// */
////------------------------------------------------------------------------------
//void Propagate::EmptyBuffer()
//{
//   for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
//        i != satBuffer.end(); ++i)
//   {
//      #ifdef DEBUG_MEMORY
//      MemoryTracker::Instance()->Remove
//         ((*i), (*i)->GetName(), "Propagate::EmptyBuffer()", "deleting from satBuffer");
//      #endif
//      delete (*i);
//   }
//   satBuffer.clear();
//
//   for (std::vector<Formation *>::iterator i = formBuffer.begin();
//        i != formBuffer.end(); ++i)
//   {
//      #ifdef DEBUG_MEMORY
//      MemoryTracker::Instance()->Remove
//         ((*i), (*i)->GetName(), "Propagate::EmptyBuffer()", "deleting from fromBuffer");
//      #endif
//      delete (*i);
//   }
//   formBuffer.clear();
//}
//
////------------------------------------------------------------------------------
//// void BufferSatelliteStates(bool fillingBuffer)
////------------------------------------------------------------------------------
///**
// * Preserves satellite state data so it can be restored after interpolating the
// * stopping condition propagation time.
// *
// * @param <fillingBuffer> Flag used to indicate the fill direction.
// */
////------------------------------------------------------------------------------
//void Propagate::BufferSatelliteStates(bool fillingBuffer)
//{
//   Spacecraft *fromSat, *toSat;
//   Formation *fromForm, *toForm;
//   std::string soName;
//
//   for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
//        i != satBuffer.end(); ++i)
//   {
//      soName = (*i)->GetName();
//      if (fillingBuffer)
//      {
//         fromSat = (Spacecraft *)FindObject(soName);
//         toSat = *i;
//      }
//      else
//      {
//         fromSat = *i;
//         toSat = (Spacecraft *)FindObject(soName);
//      }
//
//      #ifdef DEBUG_STOPPING_CONDITIONS
//         MessageInterface::ShowMessage(
//            "   Sat is %s, fill direction is %s; fromSat epoch = %.12lf   "
//            "toSat epoch = %.12lf\n",
//            fromSat->GetName().c_str(),
//            (fillingBuffer ? "from propagator" : "from buffer"),
//            fromSat->GetRealParameter("A1Epoch"),
//            toSat->GetRealParameter("A1Epoch"));
//
//         MessageInterface::ShowMessage(
//            "   '%s' Satellite state:\n", fromSat->GetName().c_str());
//         Real *satrv = fromSat->GetState().GetState();
//         MessageInterface::ShowMessage(
//            "      %.12lf  %.12lf  %.12lf\n      %.12lf  %.12lf  %.12lf\n",
//            satrv[0], satrv[1], satrv[2], satrv[3], satrv[4], satrv[5]);
//      #endif
//
//      (*toSat) = (*fromSat);
//
//      #ifdef DEBUG_STOPPING_CONDITIONS
//         MessageInterface::ShowMessage(
//            "After copy, From epoch %.12lf to epoch %.12lf\n",
//            fromSat->GetRealParameter("A1Epoch"),
//            toSat->GetRealParameter("A1Epoch"));
//      #endif
//   }
//
//   for (std::vector<Formation *>::iterator i = formBuffer.begin();
//        i != formBuffer.end(); ++i)
//   {
//      soName = (*i)->GetName();
//      #ifdef DEBUG_STOPPING_CONDITIONS
//         MessageInterface::ShowMessage("Buffering formation %s, filling = %s\n",
//            soName.c_str(), (fillingBuffer?"true":"false"));
//      #endif
//      if (fillingBuffer)
//      {
//         fromForm = (Formation *)FindObject(soName);
//         toForm = *i;
//      }
//      else
//      {
//         fromForm = *i;
//         toForm = (Formation *)FindObject(soName);
//      }
//
//      #ifdef DEBUG_STOPPING_CONDITIONS
//         MessageInterface::ShowMessage(
//            "   Formation is %s, fill direction is %s; fromForm epoch = %.12lf"
//            "   toForm epoch = %.12lf\n",
//            fromForm->GetName().c_str(),
//            (fillingBuffer ? "from propagator" : "from buffer"),
//            fromForm->GetRealParameter("A1Epoch"),
//            toForm->GetRealParameter("A1Epoch"));
//      #endif
//
//      (*toForm) = (*fromForm);
//
//      toForm->UpdateState();
//
//      #ifdef DEBUG_STOPPING_CONDITIONS
//         Integer count = fromForm->GetStringArrayParameter("Add").size();
//
//         MessageInterface::ShowMessage(
//            "After copy, From epoch %.12lf to epoch %.12lf\n",
//            fromForm->GetRealParameter("A1Epoch"),
//            toForm->GetRealParameter("A1Epoch"));
//
//         MessageInterface::ShowMessage(
//            "   %s for '%s' Formation state:\n",
//            (fillingBuffer ? "Filling buffer" : "Restoring states"),
//            fromForm->GetName().c_str());
//
//         Real *satrv = fromForm->GetState().GetState();
//
//         for (Integer i = 0; i < count; ++i)
//            MessageInterface::ShowMessage(
//               "      %d:  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf\n",
//               i, satrv[i*6], satrv[i*6+1], satrv[i*6+2], satrv[i*6+3],
//               satrv[i*6+4], satrv[i*6+5]);
//      #endif
//   }
//
//   #ifdef DEBUG_STOPPING_CONDITIONS
//      for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
//           i != satBuffer.end(); ++i)
//         MessageInterface::ShowMessage(
//            "   Epoch of '%s' is %.12lf\n", (*i)->GetName().c_str(),
//            (*i)->GetRealParameter("A1Epoch"));
//   #endif
//}
//
//

//------------------------------------------------------------------------------
// Real GetRangedParam(const Real value, const Real midpt, const Integer type)
//------------------------------------------------------------------------------
/**
 * Puts a cyclic parameter into its valid range.
 *
 * @param <value> The parameter value.
 * @param <midpt> The center of the range.
 * @param <min>   The minimum of the nominal range.  Not yet implemented.
 * @param <max>   The maximum of the nominal range.  Not yet implemented.
 *
 * @return The ranged value.
 */
//------------------------------------------------------------------------------
Real Propagate::GetRangedParam(const Real value, const Real midpt,
                               const Integer type)
{
   #ifdef DEBUG_STOPPING_CONDITIONS
      MessageInterface::ShowMessage(
         "Setting angle range for %.12lf around %.12lf\n", angle, midpt);
   #endif

   switch (type)
   {
      case GmatParam::ZERO_24:
      {
         // @TODO: This should be moved to a time util when one is available
         Real minTime = midpt - 12.0;
         Real maxTime = midpt + 12.0;
         Real time = GmatMathUtil::Mod(value, 24);
         while (time < minTime)
            time += 24.0;
         while (time > maxTime)
            time -= 24.0;
         return time;
      }

      default:
         return AngleUtil::PutAngleInDegRange(value,
            midpt - GmatMathConstants::PI_DEG,
            midpt + GmatMathConstants::PI_DEG);
   }
}


//------------------------------------------------------------------------------
// Real DoPropsHaveFiniteBurn()
//------------------------------------------------------------------------------
/**
 * Checks if any of the propagators contain a Finite burn
 *
 *
 * @return true if there is a finite burn, false if not
 */
 //------------------------------------------------------------------------------
bool Propagate::DoPropsHaveFiniteBurn()
{

   bool retval = true;

   for (UnsignedInt i = 0; i < fm.size(); ++i)
   {
      if (fm[i])
      {
         fm[i]->UpdateInitialData(false, false);
         fm[i]->BufferState();
      }
      else
      {
         p[i]->BufferState();
      }
   }
   std::vector<Propagator*>::iterator current = p.begin();

   while (current != p.end())
   {
      if ((*current)->GetHasFiniteBurn())
      {
         return true;
      }
      ++current;
   }
   return false;
}
