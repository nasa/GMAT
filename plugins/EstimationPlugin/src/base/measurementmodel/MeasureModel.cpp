//------------------------------------------------------------------------------
//                           MeasureModel
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 31, 2014
/**
 * The Measurement Model base class for estimation
 */
//------------------------------------------------------------------------------

#include "MeasureModel.hpp"
#include "SignalBase.hpp"
#include "ProgressReporter.hpp"
#include "MeasurementException.hpp"

#include "PhysicalSignal.hpp"
#include "PassivePhysicalSignal.hpp"
//#include "SinglePointSignal.hpp"
#include "ObservationData.hpp"

#include "PropSetup.hpp"
#include "ODEModel.hpp"
#include "Propagator.hpp"
#include "MessageInterface.hpp"
#include "TextParser.hpp"
#include "GroundstationInterface.hpp"
#include "Transmitter.hpp"

#include <sstream>

//#define DEBUG_CONSTRUCTION
//#define DEBUG_SET_PARAMETER
//#define DEBUG_INITIALIZATION
//#define DEBUG_LIGHTTIME
//#define DEBUG_TIMING
//#define DEBUG_FEASIBILITY
//#define DEBUG_EXECUTION
//#define DEBUG_DERIVATIVE
//#define DEBUG_CALCULATE_MEASUREMENT
//#define DEBUG_OFFSET
//#define DEBUG_API
//#define DEBUG_TRANSIENT_FORCES

#ifdef DEBUG_API
   #include <fstream>
   std::ofstream apimmFile;
   bool apimmFileOpen = false;
#endif



//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

const std::string
MeasureModel::PARAMETER_TEXT[MeasurementParamCount - GmatBaseParamCount] =
{
   "SignalPath",
};


const Gmat::ParameterType
MeasureModel::PARAMETER_TYPE[MeasurementParamCount - GmatBaseParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,
};



//------------------------------------------------------------------------------
// MeasureModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The scripted name (when needed) for the measurement model
   @param usesPassiveSignal flag that identifies the model as a passive signal if true
 */
//------------------------------------------------------------------------------
MeasureModel::MeasureModel(const std::string &name, const bool usesPassiveSignal) :
   GmatBase          (Gmat::MEASUREMENT_MODEL, "SignalBasedMeasurement", name),
   feasible          (false),
   withLighttime     (true),
   propsNeedInit     (false),          // Only need init if one is set
   navLog            (NULL),
   logLevel          (0),              // Default to everything
   epochIsAtEnd      (true),
   countInterval     (0.0),
   isPhysical        (true),
   isPassive         (usesPassiveSignal),
   solarsys          (NULL),
   transientForces   (NULL),
   measParticipantIndex  (-1)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("MeasureModel default constructor  <%p>\n", this);
#endif
   
   #ifdef DEBUG_API
      if (!apimmFileOpen)
      {
         apimmFile.open("MeasureModel_API.txt");
         apimmFile << "API Debug data for MeasureModel\n" << std::endl;
         apimmFileOpen = true;
      }
   #endif
}

//------------------------------------------------------------------------------
// ~MeasureModel()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
MeasureModel::~MeasureModel()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("MeasureModel default destructor  <%p>\n", this);
#endif

   CleanUp();
}


void MeasureModel::CleanUp()
{
   // clean up std::vector<StringArray*> participantLists;
   for (UnsignedInt i = 0; i < participantLists.size(); ++i)
   {
      if (participantLists[i])
      {
         participantLists[i]->clear();
         delete participantLists[i];
      }
   }
   participantLists.clear();

   // clean up std::vector<ObjectArray*> participants;
   for (UnsignedInt i = 0; i < participants.size(); ++i)
   {
      if (participants[i])
      {
         participants[i]->clear();
         delete participants[i];
      }
   }
   participants.clear();

   // clean up std::map<SpacePoint*, PropSetup*> propMap;
   // The step to delete PropSetup object is handled by the delete of createdObjects
   //for (std::map<SpacePoint*, PropSetup*>::iterator i = propMap.begin(); i != propMap.end(); ++i)
   //{
   //   if (i->second)
   //      delete i->second;
   //}
   propMap.clear();

   // clean up std::vector<RealArray> theDataDerivatives;
   for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
   {
      theDataDerivatives[i].clear();
   }
   theDataDerivatives.clear();

   correctionTypeList.clear();
   correctionModelList.clear();
   navLog = NULL;
   solarsys = NULL;

   // clean up ObjectArray candidates;
   candidates.clear();

   // clean up std::vector<SignalBase*> signalPaths;
   // The delete of SignalBase objects is handled by the delete of createdObjcects
   signalPaths.clear();

   // clean up std::vector<SignalData*> theData;         // theData[i] points to theData of the head of signalPaths[i]
   // all SignalData objects in theData are declared in SignalBase objects. When SignalBase objects are deleted, SignalData objects will be deleted automatically as well.
   theData.clear();

   // clean up std::vector<PhysicalModel *> *transientForces;
   transientForces = NULL;

   #ifdef DEBUG_API
      if (apimmFileOpen)
      {
         apimmFile.close();
         apimmFileOpen = false;
      }
   #endif
}


//------------------------------------------------------------------------------
// MeasureModel(const MeasureModel& mm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param mm The model used as a template for this one
 */
//------------------------------------------------------------------------------
MeasureModel::MeasureModel(const MeasureModel& mm) :
   GmatBase          (mm),
   feasible          (false),
   withLighttime     (mm.withLighttime),
   propsNeedInit     (false),
   navLog            (mm.navLog),
   logLevel          (mm.logLevel),
   isPhysical        (mm.isPhysical),
   isPassive         (mm.isPassive),
   solarsys          (mm.solarsys),
   transientForces   (NULL),
   epochIsAtEnd      (mm.epochIsAtEnd),
   countInterval     (mm.countInterval),
   correctionTypeList(mm.correctionTypeList),
   correctionModelList(mm.correctionModelList),
   measParticipantIndex (mm.measParticipantIndex)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("MeasureModel copy constructor  from <%p> to <%p>\n", &mm, this);
#endif

}


//------------------------------------------------------------------------------
// MeasureModel& operator=(const MeasureModel& mm)
//------------------------------------------------------------------------------
/**
 * Assignmant operator
 *
 * @param mm The measurement model used to configure this one
 *
 * @return This model, set to match mm
 */
//------------------------------------------------------------------------------
MeasureModel& MeasureModel::operator=(const MeasureModel& mm)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("MeasureModel operator =   set <%p> = <%p>\n", this, &mm);
#endif

   if (this != &mm)
   {
      GmatBase::operator=(mm);

      // all SignalData objects in theData are declared in SignalBase objects. When SignalBase objects are deleted, SignalData objects will be deleted automatically as well.
      theData.clear();

      feasible            = false;
      withLighttime       = mm.withLighttime;
      navLog              = mm.navLog;
      logLevel            = mm.logLevel;
      isPhysical          = mm.isPhysical;
      isPassive           = mm.isPassive;
      solarsys            = mm.solarsys;
      epochIsAtEnd        = mm.epochIsAtEnd;
      countInterval       = mm.countInterval;
      correctionTypeList  = mm.correctionTypeList;
      correctionModelList = mm.correctionModelList;
      transientForces     = NULL;
      measParticipantIndex = mm.measParticipantIndex;

      //for (std::map<SpacePoint*,PropSetup*>::iterator i = propMap.begin();
      //      i != propMap.end(); ++i)
      //{
      //   if (i->second)
      //   {
      //      delete i->second;
      //      i->second = NULL;
      //   }
      //}
      propMap.clear();

      propsNeedInit = false;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this model
 *
 * @return A clone of this model
 */
//------------------------------------------------------------------------------
GmatBase* MeasureModel::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("MeasureModel::Clone()   clone this <%p>\n", this);
#endif

   return new MeasureModel(*this);
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 *
 * @param ss The solar system the model should use
 */
//------------------------------------------------------------------------------
void MeasureModel::SetSolarSystem(SolarSystem *ss)
{
   solarsys = ss;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the script name for the parameter
 *
 * @param id The id of the parameter
 *
 * @return The script name
 */
//------------------------------------------------------------------------------
std::string MeasureModel::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < MeasurementParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string& str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a scriptable parameter
 *
 * @param str The string used for the parameter
 *
 * @return The parameter ID
 */
//------------------------------------------------------------------------------
Integer MeasureModel::GetParameterID(const std::string& str) const
{
   for (Integer i = GmatBaseParamCount; i < MeasurementParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type for a specified parameter
 *
 * @param id The ID for the parameter
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType MeasureModel::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < MeasurementParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a text description for a parameter
 *
 * @param id The ID for the parameter
 *
 * @return The description string
 */
//------------------------------------------------------------------------------
std::string MeasureModel::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string parameter
 *
 * @param id The ID for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string MeasureModel::GetStringParameter(const Integer id) const
{
   return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return true if the paramter was set, false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::SetStringParameter(const Integer id,
      const std::string& value)
{
   if (id == SIGNAL_PATH)
   {
      return false;
   }

   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a string array
 *
 * @param id The ID for the parameter
 * @param index The index into the array
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string MeasureModel::GetStringParameter(const Integer id,
      const Integer index) const
{
   return GmatBase::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in an array of strings
 *
 * @param id The ID for the parameter
 * @param value The new value for the parameter
 * @index The desired location of the parameter in the array
 *
 * @return true if the parameter was set, false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("MeasureModel<%p>::SetStringParameter(id = '%s',value = '%s', index = %d)\n", this, GetParameterText(id).c_str(), value.c_str(), index);
#endif

   if (id == SIGNAL_PATH)
   {
      if ((index < 0) || (index > (Integer)participantLists.size()))
         throw MeasurementException("Index out of bounds when setting string "
            "parameter");

      if ((Integer)participantLists.size() == index)
      {
         StringArray *partList = new StringArray;
         participantLists.push_back(partList);
      }
      participantLists[index]->push_back(value);
      return true;
   }

   return GmatBase::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array parameter
 *
 * @param id The ID for the parameter
 *
 * @return The string array
 */
//------------------------------------------------------------------------------
const StringArray& MeasureModel::GetStringArrayParameter(const Integer id) const
{
   return GmatBase::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retireves a string array from an array of string arrays
 *
 * @param id The ID for the parameter
 * @param index The index into the array of arrays
 *
 * @return The requested string array
 */
//------------------------------------------------------------------------------
const StringArray& MeasureModel::GetStringArrayParameter(const Integer id,
      const Integer index) const
{
   if (id == SIGNAL_PATH)
   {
      if ((index >= 0) && ((Integer)participantLists.size() > index))
         return *(participantLists[index]);
      throw MeasurementException("Index out of bounds when accessing the "
            "signal path");
   }

   return GmatBase::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string parameter
 *
 * @param label The scriptable name of the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string MeasureModel::GetStringParameter(const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param label The scriptable name of the parameter
 * @param value The value for the parameter
 *
 * @return true if the paramter was set, false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a string array
 *
 * @param label The scriptable name of the parameter
 * @param index The index into the array
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string MeasureModel::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in an array of strings
 *
 * @param label The scriptable name of the parameter
 * @param value The new value for the parameter
 * @index The desired location of the parameter in the array
 *
 * @return true if the parameter was set, false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array parameter
 *
 * @param label The scriptable name of the parameter
 *
 * @return The string array
 */
//------------------------------------------------------------------------------
const StringArray& MeasureModel::GetStringArrayParameter(
      const std::string& label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retireves a string array from an array of string arrays
 *
 * @param label The scriptable name of the parameter
 * @param index The index into the array of arrays
 *
 * @return The requested string array
 */
//------------------------------------------------------------------------------
const StringArray& MeasureModel::GetStringArrayParameter(
      const std::string& label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of the reference objects used in the model
 *
 * @param type The type of object requested (not used)
 *
 * @return The list of names of reference objects
 */
//------------------------------------------------------------------------------
const StringArray& MeasureModel::GetRefObjectNameArray(
      const UnsignedInt type)
{
   refObjectNames.clear();
   
   // Build the list
   for (std::vector<StringArray*>::iterator sa = participantLists.begin();
         sa != participantLists.end(); ++sa)
   {
      for (UnsignedInt i = 0; i < ((*sa)->size()); ++i)
      {
         std::string candidate = (*sa)->at(i);
         if (find(refObjectNames.begin(), refObjectNames.end(), candidate) ==
               refObjectNames.end())
         refObjectNames.push_back(candidate);
      }
   }

   return refObjectNames;
}

//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string &oldName,
//       const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames the reference objects
 *
 * @param type The type of the object being renamed
 * @param oldName The name of the object before the rename
 * @param newName The new name of the object
 *
 * @return true if an object reference name was changed, false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::RenameRefObject(const UnsignedInt type,
      const std::string &oldName, const std::string &newName)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("MeasureModel<%p>::RenameRefObject(type = %d, oldName = '%s', newName = '%s')\n", this, type, oldName.c_str(), newName.c_str());
#endif

   bool retval = false;

   // Handle participant name changes
   for (UnsignedInt i = 0; i < participantLists.size(); ++i)
   {
      for (UnsignedInt j = 0; j < participantLists.at(i)->size(); ++j)
      {
         if (participantLists.at(i)->at(j) == oldName)
         {
            participantLists.at(i)->at(j) = newName;
            retval = true;
         }
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const UnsignedInt type,
//       const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets pointers to the model's reference objects
 *
 * @param obj The object pointer
 * @param type The type of the object (not used)
 * @param name The name of the object  (not used)
 *
 * @return true if the pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::SetRefObject(GmatBase* obj, const UnsignedInt type,
      const std::string& name)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("MeasureModel<%p>::SetRefObject(obj = <%p>, type = %d, name = '%s')\n", this, obj, type, name.c_str());
#endif

   bool retval = false;

   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      std::string name = obj->GetName();
      for (UnsignedInt i = 0; i < participantLists.size(); ++i)
      {
         if (find(participantLists[i]->begin(), participantLists[i]->end(),
               name) != participantLists[i]->end())
         {
            if (SetRefObject(obj, type, name, i))
               retval = true;
         }
      }
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const UnsignedInt type,
//       const std::string& name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the object pointers used in the measurement
 *
 * @param obj   The object used in the measurement
 * @param type  The object's type (not used)
 * @param name  The object's name (not used)
 * @param index The index of participant list that receives the SpacePoint
 *              participants
 *
 * @return true is the object pointer was set anywhere; false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::SetRefObject(GmatBase* obj, const UnsignedInt type,
      const std::string& name, const Integer index)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("MeasureModel<%p>::SetRefObject(obj = <%p>, type = %d, name = '%s', index = %d)\n", this, obj, type, name.c_str(), index);
#endif

   bool retval = false;

   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      // Put the object into the map
      if (propMap.find((SpacePoint*)obj) == propMap.end())
      {
         // Save a place for the propagator clone for this object
         propMap[(SpacePoint*)obj] = NULL;
      }

      if (find(candidates.begin(), candidates.end(), obj) ==
            candidates.end())
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Adding object %s to %s.candidates\n",
                  obj->GetName().c_str(), instanceName.c_str());
         #endif
         candidates.push_back(obj);
         retval = true;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// void SetPropagator(PropSetup* ps)
//------------------------------------------------------------------------------
/**
 * Sets the propagator for use in signal classes to find light time solutions
 *
 * @param ps The propagator
 *
 * @todo: Extend this code to support multiple propagators
 */
//------------------------------------------------------------------------------
void MeasureModel::SetPropagators(std::vector<PropSetup*> *ps,
      std::map<std::string, StringArray> *spMap)
{
   #ifdef DEBUG_SET_PARAMETER
      MessageInterface::ShowMessage("MeasureModel<%p>::SetPropagators(ps = <%p>)\n", this, ps);
   #else
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("MeasureModel<%p>::SetPropagators(ps = <%p>)\n", this, ps);
      #endif
   #endif

   std::string defaultPropName = (*ps)[0]->GetName();

   // Invert the prop to (multiple) sat map
   std::map<std::string, std::string> satToPropMap;
   for (UnsignedInt i = 0; i < ps->size(); ++i)
   {
      std::string propName = (*ps)[i]->GetName();
      StringArray satNames = (*spMap)[propName];

      for (UnsignedInt j = 0; j < satNames.size(); ++j)
      {
         if (satToPropMap.find(satNames[j]) == satToPropMap.end())
            satToPropMap[satNames[j]] = propName;
         else
            if (satToPropMap[satNames[j]] != propName)
               throw MeasurementException("The spacecraft " + satNames[j] +
                     " is set to propagate with more than one propagator, "
                     "which is not allowed.");
      }
   }
   
   for (std::map<SpacePoint*,PropSetup*>::iterator i = propMap.begin();
         i != propMap.end(); ++i)
   {
      SpacePoint *obj = i->first;

      if (obj->IsOfType(Gmat::SPACEOBJECT))
      {
         // Find the name of the propagator assigned to this spaceobject
         std::string propToUse = spMap->begin()->first;
         std::string scName = obj->GetName();

         if (satToPropMap.find(scName) != satToPropMap.end())
            propToUse = satToPropMap[scName];

         // Locate it in the propsetup list
         PropSetup *thePropagator = nullptr;
         for (UnsignedInt i = 0; i < ps->size(); ++i)
         {
            if ((*ps)[i]->GetName() == propToUse)
            {
               thePropagator = (*ps)[i];
               break;
            }
         }

         if (thePropagator)
         {
            // Clone it, and manage the clone in GmatBase code
            PropSetup *propagator = (PropSetup*)(thePropagator->Clone());
            createdObjects.push_back(propagator);

            if (propagator)
            {
               // Set flag to tell propagator using precision time
               propagator->SetPrecisionTimeFlag(true);

               propMap[obj] = propagator;
               propsNeedInit = true;
               for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
               {
                  signalPaths[i]->SetPropagator(propagator, obj);
               }
            }
         }
      }
   }
}


//------------------------------------------------------------------------------
//  void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
/**
* Passes the transient force vector into the measure model
*
* The transient force vector is a set of models used in GMAT's ODEModel for
* affects that are turned on and off over the course of a mission.  An example
* of a transient force is a finite burn, which is toggled by the
* BeginFiniteBurn and EndFiniteBurn commands.  These components are only used
* by commands that need them.  Typical usage is found in the propagation
* enabled commands.
*
* @param tf The vector of transient forces
*/
//------------------------------------------------------------------------------
void MeasureModel::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
    transientForces = tf;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the measurement model for use
 *
 * @return true if the initialization succeeds, false if it fails
 */
//------------------------------------------------------------------------------
bool MeasureModel::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Start MeasurementModel<%s,%p>::Initialize()\n", GetName().c_str(), this);
   #endif

   #ifdef DEBUG_API
      if (apimmFileOpen)
      {
         apimmFile << "   Calling MeasureModel::Initialize()" << std::endl;
      }
   #endif

   bool retval = false;

   if (GmatBase::Initialize())
   {
      if (navLog)
      {
         logLevel = navLog->GetLogLevel("Measurement");
         #ifdef DEBUG_LOGGING
            MessageInterface::ShowMessage("Current log level for measurements "
                  "is %d\n", logLevel);
         #endif
      }
      else
         logLevel = 32767;
      
      if (participantLists.size() > 0)
      {
         std::string theMissing = "";

         // Clear stale pointers
         for (UnsignedInt i = 0; i < participants.size(); ++i)
         {
            delete participants[i];
         }
         participants.clear();

         //for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
         //{
         //   if (signalPaths[i])
         //      delete signalPaths[i];
         //}
         signalPaths.clear();

         // all SignalData objects in theData are declared in SignalBase objects. When SignalBase objects are deleted, SignalData objects will be deleted automatically as well.
         theData.clear();

         for (UnsignedInt i = 0; i < participantLists.size(); ++i)
         {
            if (participantLists[i]->size() < 2)
               throw MeasurementException("Participant list size is too short");

            participants.push_back(new ObjectArray);
         }

         // Put all participants in place for the model
         for (UnsignedInt i = 0; i < participantLists.size(); ++i)
         {
            for (UnsignedInt j = 0; j < participantLists[i]->size(); ++j)
            {
               std::string pname = participantLists[i]->at(j);
               GmatBase *obj = NULL;
               for (UnsignedInt k = 0; k < candidates.size(); ++k)
                  if (candidates[k]->GetName() == pname)
                     obj = candidates[k];
               if (obj != NULL)
               {
                  // Set flag to tell participant using precision time
                  obj->SetPrecisionTimeFlag(true);
                  participants[i]->push_back(obj);
               }
               else
               {
                  if (theMissing != "")
                     theMissing += ", ";
                  theMissing += pname;
               }
            }
         }

         if (theMissing == "")
         {
            // Build the signals
            for (UnsignedInt i = 0; i < participantLists.size(); ++i)
            {
               SignalBase *head = NULL;
               unsigned long strandId = SignalDataCache::StrandToHash(participantLists[i]);

               for (UnsignedInt j = 0; j < participantLists[i]->size()-1; ++j)
               {
                  // 1. Create a signal leg
                  SignalBase *sb = NULL;
                  if (isPhysical && isPassive)
                     sb = new PassivePhysicalSignal("");
                  else if (isPhysical)
                  {
                     sb = new PhysicalSignal("Signal","");
                     // It is used to book keeping all created objects in order to clean up
                     createdObjects.push_back((GmatBase*)sb);
                  }
                  else
                  {
                     // The signals are single point
//                     sb = new SinglePointSignal("");
                     throw MeasurementException("Single point signals are not "
                           "yet implemented");
                  }

                  // 2. Set solar system, lightime correction, names of transmit and receive participants,
                  //    participant objects, and their propagators 
                  if (sb)
                  {
                     if (navLog)
                        sb->SetProgressReporter(navLog);
                     sb->SetSolarSystem(solarsys);
                     sb->UsesLighttime(withLighttime);
                     sb->SetStrandId(strandId);

                     // Set name for transmit participant and receive participant 
                     if (sb->SetTransmitParticipantName(
                           participantLists[i]->at(j)) == false)
                     {
                        throw MeasurementException("Failed to set the name of "
                              "the transmit participant to " +
                              participantLists[i]->at(j) +
                              " on measurement model " + instanceName);
                     }
                     if (sb->SetReceiveParticipantName(
                           participantLists[i]->at(j+1)) == false)
                     {
                        throw MeasurementException("Failed to set the name of "
                              "the receive participant to " +
                              participantLists[i]->at(j+1) +
                              " on measurement model " + instanceName);
                     }

                     // Set transmit participant and its propagator in SignalBase object
                     GmatBase *obj = participants[i]->at(j);
                     if (sb->SetRefObject(obj, obj->GetType(), obj->GetName()) == false)
                        throw MeasurementException("Failed to set the transmit "
                              "participant");
                     else
                        if (obj->IsOfType(Gmat::SPACEOBJECT) && propMap[(SpacePoint*)obj])
                           sb->SetPropagator(propMap[(SpacePoint*)obj], obj);

                     // Set receive participant and its propagator in SignalBase object
                     obj = participants[i]->at(j+1);
                     if (sb->SetRefObject(obj, obj->GetType(), obj->GetName()) == false)
                        throw MeasurementException("Failed to set the receive "
                              "participant\n");
                     else
                        if (obj->IsOfType(Gmat::SPACEOBJECT) && propMap[(SpacePoint*)obj])
                           sb->SetPropagator(propMap[(SpacePoint*)obj], obj);

                     if (!sb->Initialize())
                     {
                        throw MeasurementException("Signal initialization "
                              "failed in measurement model " + instanceName);
                     }

                     if (j == 0)
                     {
                        signalPaths.push_back(sb);
                        head = sb;
                        theData.push_back(sb->GetSignalDataObject());
                     }
                     else
                        head->Add(sb);
                  }
               }
            }

            // Add new types of measurement corrections to signal path
            for(UnsignedInt i = 0; i < correctionTypeList.size(); ++i)
               AddCorrection(correctionModelList[i], correctionTypeList[i]);

            ////// For each ground station, clone all ErrorModel objects for each signal path
            ////for (UnsignedInt i = 0; i < participants.size(); ++i)
            ////{
            ////   GmatBase* firstPart = participants[i]->at(0);
            ////   GmatBase* lastPart = participants[i]->at(participants[i]->size() - 1);

            ////   if ((firstPart->IsOfType(Gmat::GROUND_STATION)) &&
            ////      (lastPart->IsOfType(Gmat::GROUND_STATION) == false))
            ////   {
            ////      // clone all ErrorModel objects belonging to groundstation firstPart
            ////      std::string spacecraftName = "";
            ////      std::string spacecraftId = "";
            ////      GmatBase* obj = participants[i]->at(1);
            ////      if (obj->IsOfType(Gmat::SPACECRAFT))
            ////      {
            ////         spacecraftName = obj->GetName();
            ////         spacecraftId = obj->GetStringParameter("Id");
            ////      }
            ////      else
            ////         throw MeasurementException("Error: It has 2 ground stations (" +
            ////            firstPart->GetName() + ", " + obj->GetName() +
            ////            ") next to each other in signal path.\n");

            ////      ((GroundstationInterface*)firstPart)->CreateErrorModelFo
            ////         rSignalPath(spacecraftName, spacecraftId);
            ////   }
            ////   else
            ////   {
            ////      // clone all ErrorModel objects belonging to groundstation firstPart
            ////      std::string spacecraftName = "";
            ////      std::string spacecraftId = "";
            ////      GmatBase* obj = participants[i]->at(participants[i]->size() - 2);
            ////      if (obj->IsOfType(Gmat::SPACECRAFT))
            ////      {
            ////         spacecraftName = obj->GetName();
            ////         spacecraftId = obj->GetStringParameter("Id");
            ////      }
            ////      else
            ////         throw MeasurementException("Error: It has 2 ground stations (" +
            ////            obj->GetName() + ", " +
            ////            lastPart->GetName() + ") next to each other in signal path.\n");

            ////      ((GroundstationInterface*)lastPart)->CreateErrorModelForSignalPath(spacecraftName, spacecraftId);
            ////   }
            ////}

            // For each measuring participant, clone ErrorModel objects for it
            for (UnsignedInt i = 0; i < participants.size(); ++i)
            {
               Integer measPartIndex = (measParticipantIndex == -1 ? (participants[i]->size() - 1) : measParticipantIndex);
               GmatBase * measParticipant = participants[i]->at(measPartIndex);

               // clone all ErrorModel objects
               GmatBase* obj = participants[i]->at(measPartIndex-1);
               std::string name = obj->GetName();
               std::string id = obj->GetStringParameter("Id");
               if (measParticipant->IsOfType(Gmat::SPACECRAFT))
               {
                  ((Spacecraft*)measParticipant)->CreateErrorModelForSignalPath(name, id,((MeasureModel *)this)->GetFullName());
               }
               else
               {
                  ((GroundstationInterface*)measParticipant)->CreateErrorModelForSignalPath(name, id);
               }
            }


            /// @todo: this code will be removed when ground station allows to
            /// places on any celestial body.
            // Verify ground station's central body is Earth
            for (UnsignedInt i = 0; i < participants.size(); ++i)
            {
               for (Integer j = 0; j < participants[i]->size(); ++j)
               {
                  GmatBase* participant = participants[i]->at(j);
                  if (participant->IsOfType(Gmat::GROUND_STATION))
                  {
                     std::string cbName = ((GroundstationInterface*)participant)->GetStringParameter("CentralBody");
                     if (cbName != "Earth")
                        throw GmatBaseException(
                           "Error: The current GMAT version only allows Earth"
                           " ground stations for tracking data measurement simulation or estimation.");
                  }
               }
            }

            retval = true;
         }
         else
            throw MeasurementException("Cannot configure the measurement "
                  "model " + instanceName + "; the following participants are "
                  "not in the object: " + theMissing);

         if (navLog)
         {
            std::stringstream data;
            data.precision(16);

            if (logLevel <= 1)
            {
               data << "   " << instanceName
                    << (retval ? " initialized\n" : " failed to initialize\n");
            }

            if (logLevel == 0)
            {
               data << "   For measurement model " << instanceName
                    << ", constructed the signal path(s):" << "\n";
               for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
                  data << "      "
                       << (signalPaths[i]->GetPathDescription(true))
                       << "\n";
            }

            navLog->WriteData(data.str());
         }
      }
      else
      {
         throw MeasurementException("Measurement has no participants");
      }
   }

   #ifdef DEBUG_API
      if (apimmFileOpen)
      {
         apimmFile << "   Leaving " << (retval ? "successful" : "unsuccessful")
                 << " call to MeasureModel::Initialize()" << std::endl;
      }
   #endif


   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("End MeasurementModel<%s,%p>::Initialize()\n", GetName().c_str(), this);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// const std::vector<SignalData*>& GetSignalData()
//------------------------------------------------------------------------------
/**
 * Retrieves the data calculated the last time the signal was computed
 *
 * @return The vector of raw signal data
 */
//------------------------------------------------------------------------------
const std::vector<SignalData*>& MeasureModel::GetSignalData()
{
   return theData;
}

//------------------------------------------------------------------------------
// const std::vector<SignalBase*>& GetSignalPaths()
//------------------------------------------------------------------------------
/**
 * Retrieves the signal paths
 *
 * @return The vector of signal paths
 */
//------------------------------------------------------------------------------
const std::vector<SignalBase*>& MeasureModel::GetSignalPaths()
{
   return signalPaths;
}

//------------------------------------------------------------------------------
// bool CheckEpochValidity(GmatTime theEpoch)
//------------------------------------------------------------------------------
/**
 * Tests an input epoch to see if measurement can be calculated at that epoch
 *
 * This method is used in measurements that have participants that may not have
 * data at certain times.  An example is a SN measurement that uses a TDRS
 * propagated using an ephemeris file.  The file may have gaps in the data
 * inserted, for example, during maneuvers.
 *
 * @param theTime The epoch that needs to be tested
 *
 * @return true if the model can support the epoch
 */
//------------------------------------------------------------------------------
bool MeasureModel::CheckEpochValidity(const GmatTime &theEpoch)
{
   bool retval = true;
   PrepareToPropagate();
   // Test each propagator to see if the epoch is supported
   for (auto const& [obj, prop] : propMap)
   {
      if (prop)
      {
         #ifdef DEBUG_GAPS
            MessageInterface::ShowMessage("%s for %s,  ", prop->GetName().c_str(),
                  obj->GetName().c_str());
         #endif
         if (prop->GetPropagator()->IsValidEpoch(theEpoch.GetMjd()) == false)
         {
            #ifdef DEBUG_GAPS
               MessageInterface::ShowMessage("%.12lf is not a valid epoch\n",
                     theEpoch.GetMjd());
            #endif
            retval = false;
            break;
         }
      }
   }

   #ifdef DEBUG_GAPS
      MessageInterface::ShowMessage("%.11lf %s valid\n", theEpoch.GetMjd(),
            (retval ? "is" : "is not"));
   #endif
   
   return retval;
}

//------------------------------------------------------------------------------
// bool IsMeasurementFeasible()
//------------------------------------------------------------------------------
/**
 * Returns the feasibility flag for the most recent measurement computation
 *
 * @return true if the measurement was feasible, false if not (or if no
 * calculation has been performed)
 */
//------------------------------------------------------------------------------
bool MeasureModel::IsMeasurementFeasible()
{
   return feasible;
}

//------------------------------------------------------------------------------
// bool SetProgressReporter(ProgressReporter* reporter)
//------------------------------------------------------------------------------
/**
 * Sets the logging object for the measurements
 *
 * @param reporter The progress reporter used for log messages
 *
 * @return true if the reporter is set
 */
//------------------------------------------------------------------------------
bool MeasureModel::SetProgressReporter(ProgressReporter* reporter)
{
   bool retval = false;

   if (reporter)
   {
      navLog = reporter;
      retval = true;
   }

   return retval;
}


//@todo: Move the calling parameters here into the adapters
//------------------------------------------------------------------------------
// bool CalculateMeasurement(bool withEvents, ObservationData* forObservation,
//       std::vector<RampTableData>* rampTB)
//------------------------------------------------------------------------------
/**
 * Fires all of the Signal objects to generate the raw measurement data
 *
 * @param withEvents            Flag used to indicate if a light time solution should be
 *                              computed (not used)
 * @param forObservation        An observation supplying data needed for the
 *                              calculation (not used)
 * @param rampTB                A ramp table for the data (not used)
 * @param atTimeOffset          Time offset, in seconds, from the base epoch (used for
 *                              differenced measurements)
 * @param withMediaCorrection   true for adding media correction to measurement,
 *                              false otherwise
 *
 * @return true if the the calculation succeeded, false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::CalculateMeasurement(bool withEvents, bool withMediaCorrection,
      ObservationData* forObservation, std::vector<RampTableData>* rampTB, bool forSimulation, 
      Real atTimeOffset, Integer forStrand)
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage(" Enter MeasureModel::CalculateMeasurement(%s, <%p>, <%p>)\n", (withEvents?"true":"false"), forObservation, rampTB); 
   #endif

   #ifdef DEBUG_TIMING
      MessageInterface::ShowMessage("Calculating Signal Data in MeasureModel\n");
   #endif

   #ifdef DEBUG_API
      if (apimmFileOpen)
      {
         apimmFile << "   Calling MeasureModel::CalculateMeasurement()" << std::endl;
      }
   #endif

   bool retval = false;
   feasible = true;
   
   // 1. Prepare the propagators
#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "   Prepping to prop" << std::endl;
   }
#endif
   #ifdef DEBUG_TIMING
      MessageInterface::ShowMessage("1. Prepare the propagators\n");
   #endif
   //if (propsNeedInit)
   //   PrepareToPropagate();
   PrepareToPropagate();

   // 1.1. Save the states of the objects being propagated, as they can be changed during calculation of the measurement values
   std::vector<bool> precTimeVec;
   std::vector<GmatEpoch> epochVec;
   std::vector<GmatTime> epochGTVec;
   std::vector<Real> valsVec;
   
   SaveState(precTimeVec, epochVec, epochGTVec, valsVec);
   
   ///// Clean up the assumption that epoch is at the end
   //bool epochIsAtEnd = true;

   // 2. Find the measurement epoch needed for the computation
   #ifdef DEBUG_TIMING
      MessageInterface::ShowMessage("2. Find the measurement epoch needed for the computation\n");
   #endif

#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "   Finding the epoch" << std::endl;
   }
#endif

   GmatTime forEpoch;
   if (forObservation)
   {
      forEpoch = forObservation->epochGT;
   }
   else // Grab epoch from the first SpaceObject in the participant data
   {
      for (UnsignedInt i = 0; i < candidates.size(); ++i)
      {
         if (candidates[i]->IsOfType(Gmat::SPACEOBJECT))
         {
            
            forEpoch = ((SpaceObject*)candidates[i])->GetEpochGT();
            if(!propMap[(SpaceObject*)candidates[i]]->GetPropagator()->IsOfType("EphemerisPropagator"))
               break;
         }
      }
   }
   
   #ifdef DEBUG_OFFSET
      MessageInterface::ShowMessage("Base epoch: %s, timeOffset %lf sec, New epoch ",
            forEpoch.ToString().c_str(), atTimeOffset);
   #endif

   if (atTimeOffset != 0.0)
   {
      //forEpoch += atTimeOffset * GmatTimeConstants::DAYS_PER_SEC;
      forEpoch.AddSeconds(atTimeOffset);
   }

   #ifdef DEBUG_OFFSET
      MessageInterface::ShowMessage("%s\n", forEpoch.ToString().c_str());
   #endif

   // 3. Synchronize the propagators to the measurement epoch by propagating each
   // spacecraft that is off epoch to that epoch
   #ifdef DEBUG_TIMING
      MessageInterface::ShowMessage("3. Synchronizing in MeasureModel at time = %s\n", forEpoch.ToString().c_str());
   #endif
#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "   Getting in sync" << std::endl;
   }
#endif

   for (std::map<SpacePoint*,PropSetup*>::iterator i = propMap.begin();
         i != propMap.end(); ++i)
   {
      if (i->first->IsOfType(Gmat::SPACEOBJECT) && (i->second != NULL))
      {
         GmatTime satTime = ((SpaceObject*)i->first)->GetEpochGT();
         Real dt = (forEpoch - satTime).GetTimeInSec();

         #ifdef DEBUG_EXECUTION
            MessageInterface::ShowMessage("forEpoch: %s, satTime = %s, "
               "dt = %le\n", forEpoch.ToString().c_str(), satTime.ToString().c_str(), dt);
         #endif

         // Make sure the propagators are set to the spacecraft data
         Propagator *prop = i->second->GetPropagator();
         prop->UpdateFromSpaceObject();

         if (dt != 0.0)
         {
            retval = prop->Step(dt);
         }
      }
   }
 
   // 4.Calculate the measurement data ("C" value data) for all signal paths
   #ifdef DEBUG_CALCULATE_MEASUREMENT
      MessageInterface::ShowMessage("*************************************************************\n");

      MessageInterface::ShowMessage("*          Calculate Measurement Data %s at Epoch (%s) \n", (withEvents?"with Event":"w/o Event"), forEpoch.ToString().c_str());
     
      MessageInterface::ShowMessage("*************************************************************\n");
   #endif
#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "   Running the numbers" << std::endl;
   }
#endif
   
   SignalBase *leg, *lastleg, *firstleg;
   for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
   {
      // For a measurement that need to control strand by strand with time
      // offsets (e.g. for differencing
      if (forStrand != -1)
         i = forStrand;

      #ifdef DEBUG_CALCULATE_MEASUREMENT
         MessageInterface::ShowMessage("*** Calculate Measurement Data for Path %d of %d:  ", i, signalPaths.size());
         SignalBase* s = signalPaths[i];
         SignalData* sdata; 
         while (s != NULL)
         {
            // Display leg shown in SignalBase s:
            sdata = s->GetSignalDataObject();
            MessageInterface::ShowMessage("<%s -> %s>  ", sdata->tNode->GetName().c_str(), sdata->rNode->GetName().c_str());
            s = s->GetNext();
         }
         MessageInterface::ShowMessage("\n\n");
      #endif
      
      // 4.1. Initialize all signal legs in this path:
      #ifdef DEBUG_TIMING
         MessageInterface::ShowMessage("4.1. Initialize all signal legs in signal path %d:\n", i);
      #endif
#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "      Initing signal paths" << std::endl;
   }
#endif
      leg = signalPaths[i];
#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "         Leg " << i << " named " << leg->GetName()
                << " with type " << leg->GetTypeName() << std::endl;
   }
#endif
      leg->InitializeSignal(epochIsAtEnd);

      // 4.2. Compute hardware delay (in forward direction of signal path). It has to be specified before running ModelSignal 
      #ifdef DEBUG_TIMING
         MessageInterface::ShowMessage("4.2. Calculate hardware delays in signal path %d:\n", i);
      #endif
#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "      Managing hardware delays" << std::endl;
   }
#endif
      leg = firstleg = lastleg = signalPaths[i];
      while(leg != NULL)
      {
         leg->HardwareDelayCalculation();                  // caluclate hardware delay for signal leg

         // Add count time interval to the reveiver's hardware delay of the last participant when measurement time tag is at the end of signal path
         // (or to the transmiter's hardware delay of the first participant when measurement time tag is at the begining of signal path)
         if (epochIsAtEnd)
         {
            if ((leg->GetNext() == NULL) && (countInterval != 0.0))
               leg->GetSignalDataObject()->rDelay += countInterval;
         }
         else
         {
            if ((leg == firstleg)&&(countInterval != 0.0))
               leg->GetSignalDataObject()->tDelay += countInterval;
         }

         leg = leg->GetNext();
         if (leg != NULL)
            lastleg = leg;
      }

      // 4.3. Sync transmitter and receiver epochs to forEpoch, and Spacecraft state
      // data to the state known in the PropSetup for the starting Signal
      leg = signalPaths[i];
#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "      Syncing transmitters and receivers" << std::endl;
   }
#endif

      /// @todo Adjust the following code for multiple spacecraft

      while (leg != NULL)
      {
         SignalData *sdObj = leg->GetSignalDataObject();

         sdObj->tPrecTime = sdObj->rPrecTime = forEpoch;
         if (sdObj->tNode->IsOfType(Gmat::SPACECRAFT))
         {
            // this spacecraft's state presents in MJ2000Eq with origin at ForceModel.CentralBody
            const Real* propState =
               propMap[sdObj->tNode]->GetPropagator()->AccessOutState();
            Rvector6 state(propState);        // state of spacecraft presenting in MJ2000Eq coordinate system with origin at ForceModel.CentralBody

            // This step is used to convert spacecraft's state to Spacecraft.CoordinateSystem                                                                          // fix bug GMT-5364
            SpacePoint* spacecraftOrigin = ((Spacecraft*)(sdObj->tNode))->GetOrigin();                 // the origin of the transmit spacecraft's cooridinate system   // fix bug GMT-5364


            SpacePoint* propOrigin = nullptr;
            if (propMap[sdObj->tNode]->GetPropagator()->UsesODEModel())
               propOrigin = propMap[sdObj->tNode]->GetODEModel()->GetForceOrigin();     // the origin of the coordinate system used in forcemodel       // fix bug GMT-5364
            else
               propOrigin = propMap[sdObj->tNode]->GetPropagator()->GetPropOrigin();

            state = state + (propOrigin->GetMJ2000PrecState(sdObj->tPrecTime) - spacecraftOrigin->GetMJ2000PrecState(sdObj->tPrecTime));                         // fix bug GMT-5364
            sdObj->tLoc = state.GetR();
            sdObj->tVel = state.GetV();

            // Specify transmit participant's STM at measurement time tm
	   		// Set size for tSTMtm
            Integer stmRowCount = sdObj->tNode->GetIntegerParameter("FullSTMRowCount");
            if ((sdObj->tSTMtm.GetNumRows() != stmRowCount)||(sdObj->tSTMtm.GetNumColumns() != stmRowCount))
               sdObj->tSTMtm.ChangeSize(stmRowCount, stmRowCount, true);

			   // Get start index of STM
			   const std::vector<ListItem*>* stateMap = propMap[sdObj->tNode]->GetPropStateManager()->GetStateMap();
			   UnsignedInt stmStartIndex = -1;
   			for (UnsignedInt index = 0; index < stateMap->size(); ++index)
	   		{
		   	   if (((*stateMap)[index]->object == sdObj->tNode) && ((*stateMap)[index]->elementName == "STM"))
			      {
				   	stmStartIndex = index;
					   break;
			      }
			   }

   			// Set value for tSTMtm
            for (UnsignedInt ii = 0; ii < stmRowCount; ++ii)
               for (UnsignedInt jj = 0; jj < stmRowCount; ++jj)
                  sdObj->tSTMtm(ii,jj) = propState[stmStartIndex + ii*stmRowCount + jj];

            // transmit participant STM at transmit time t1
            if ((sdObj->tSTM.GetNumRows() != sdObj->tSTMtm.GetNumRows())||(sdObj->tSTM.GetNumColumns() != sdObj->tSTMtm.GetNumColumns()))
               sdObj->tSTM.ChangeSize(sdObj->tSTMtm.GetNumRows(), sdObj->tSTMtm.GetNumColumns(), true);
            sdObj->tSTM = sdObj->tSTMtm;
         }

         if (sdObj->rNode->IsOfType(Gmat::SPACECRAFT))
         {
            // this spacecraft's state presents in MJ2000Eq with origin at ForceModel.CentralBody

            if (propMap[sdObj->rNode] == NULL)
               throw MeasurementException("MeasureModel::CalculateMeasurement(): "
                  "The propagator for " + sdObj->rNode->GetName() + " is not defined");

            const Real* propState =
               propMap[sdObj->rNode]->GetPropagator()->AccessOutState();
            Rvector6 state(propState);

            // This step is used to convert spacecraft's state to Spacecraft.CoordinateSystem                                                                          // fix bug GMT-5364
            SpacePoint* spacecraftOrigin = ((Spacecraft*)(sdObj->rNode))->GetOrigin();                 // the origin of the receive spacecraft's cooridinate system    // fix bug GMT-5364

            SpacePoint* propOrigin = nullptr;
            if (propMap[sdObj->rNode]->GetPropagator()->UsesODEModel())
               propOrigin = propMap[sdObj->rNode]->GetODEModel()->GetForceOrigin();     // the origin of the coordinate system used in forcemodel    // fix bug GMT-5364
            else
               propOrigin = propMap[sdObj->rNode]->GetPropagator()->GetPropOrigin();

            state = state + (propOrigin->GetMJ2000PrecState(sdObj->rPrecTime) - spacecraftOrigin->GetMJ2000PrecState(sdObj->rPrecTime));                         // fix bug GMT-5364
            sdObj->rLoc = state.GetR();
            sdObj->rVel = state.GetV();

            // receive participant STM at measurement type tm
            Integer stmRowCount = sdObj->rNode->GetIntegerParameter("FullSTMRowCount");
			   // Set size for rSTMtm
            if ((sdObj->rSTMtm.GetNumRows() != stmRowCount)||(sdObj->rSTMtm.GetNumColumns() != stmRowCount))
               sdObj->rSTMtm.ChangeSize(stmRowCount, stmRowCount, true);

			   // Get start index of STM
			   const std::vector<ListItem*>* stateMap = propMap[sdObj->rNode]->GetPropStateManager()->GetStateMap();
            UnsignedInt stmStartIndex = -1;
            for (UnsignedInt index = 0; index < stateMap->size(); ++index)
            {
               if (((*stateMap)[index]->object == sdObj->rNode) && ((*stateMap)[index]->elementName == "STM"))
               {
                  stmStartIndex = index;
                  break;
               }
            }

			   // Set value for eSTMtm
            for (UnsignedInt ii = 0; ii < stmRowCount; ++ii)
               for (UnsignedInt jj = 0; jj < stmRowCount; ++jj)
                  sdObj->rSTMtm(ii,jj) = propState[stmStartIndex + ii*stmRowCount + jj];

            // receive participant STM at receive time t2
            if ((sdObj->rSTM.GetNumRows() != sdObj->rSTMtm.GetNumRows())||(sdObj->rSTM.GetNumColumns() != sdObj->rSTMtm.GetNumColumns()))
               sdObj->rSTM.ChangeSize(sdObj->rSTMtm.GetNumRows(), sdObj->rSTMtm.GetNumColumns(), true);
            sdObj->rSTM = sdObj->rSTMtm;
         }

         leg = leg->GetNext();
      }

      // 4.4. Get the start signal:
      SignalBase *startSignal = signalPaths[i]->GetStart(epochIsAtEnd);
      SignalData *sd = startSignal->GetSignalDataObject();
      #ifdef DEBUG_TIMING
         MessageInterface::ShowMessage("4.4. Get the start signal leg for signal path %d:\n", i);
      #endif
      
      // 4.5. Compute C-value:
      // 4.5.1. Compute Light Time range, relativity correction, and ET-TAI correction (backward or forward direction that depends on where measurement time is get):
      #ifdef DEBUG_TIMING
         MessageInterface::ShowMessage("4.5. Compute C-value:\n");
         MessageInterface::ShowMessage("4.5.1 Calculate range, relativity correction, and ET-TAI correction for signal path %d:\n", i);
      #endif
#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "      Let's see what value C has" << std::endl;
   }
#endif
      //if (epochIsAtEnd)
      //   forEpoch1 = forEpoch - lastleg->GetSignalData().rDelay/GmatTimeConstants::SECS_PER_DAY;
      //else
      //   forEpoch1 = forEpoch + firstleg->GetSignalData().tDelay/GmatTimeConstants::SECS_PER_DAY;

      if (startSignal->ModelSignal(forEpoch, forSimulation, epochIsAtEnd) == false)
      {
#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "         ModelSignal failed" << std::endl;
   }
#endif
         throw MeasurementException("Signal modeling failed in model " +
               instanceName);
      }

      // 4.5.2. Compute signal frequency on each leg(in forward direction of signal path)
      #ifdef DEBUG_TIMING
         MessageInterface::ShowMessage("4.5.2 Compute signal frequency on each leg for signal path %d:\n", i);
      #endif
      leg = signalPaths[i];
      while(leg != NULL)
      {
#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "         Freq Calc for leg " << i << std::endl;
   }
#endif
         leg->SignalFrequencyCalculation(rampTB, 0.0, forEpoch);          // calculate signal frequency on each signal leg
         leg = leg->GetNext();
      }

      if (withMediaCorrection)
      {
         // 4.5.3. Compute media correction and hardware delay (in forward direction of signal path)
         #ifdef DEBUG_TIMING
            MessageInterface::ShowMessage("4.5.3 Calculate media correction for signal path %d:\n", i);
         #endif
         leg = signalPaths[i];
         while(leg != NULL)
         {
#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "         Media Calc for leg " << i << std::endl;
   }
#endif
            leg->MediaCorrectionCalculation(rampTB);          // calculate media corrections for signal leg
            leg = leg->GetNext();
         }
      }


      // 4.6. Reset value of hardware delay
#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "      Reverting hardware delays" << std::endl;
   }
#endif
      leg = firstleg = lastleg = signalPaths[i];
      while(leg != NULL)
      {
         if (epochIsAtEnd)
         {
            if ((leg->GetNext() == NULL)&&(countInterval != 0.0))
               leg->GetSignalDataObject()->rDelay -= countInterval;
         }
         else
         {
            if ((leg == firstleg)&&(countInterval != 0.0))
               leg->GetSignalDataObject()->tDelay -= countInterval;
         }

         leg = leg->GetNext();
         if (leg != NULL)
            lastleg = leg;
      }

      #ifdef DEBUG_CALCULATE_MEASUREMENT
         SignalData *current  = theData[i];
         Real lightTimeRange  = 0.0;                      // unit: km
         Real tropoCorrection = 0.0;                      // unit: km
         Real ionoCorrection  = 0.0;                      // unit: km
         Real relCorrection   = 0.0;                      // unit: km
         Real ettaiCorrection = 0.0;                      // unit: km
         Real delayCorrection = 0.0;                      // unit: km
         while (current != NULL)
         {
            // accumulate all light time correction
            lightTimeRange += current->rangeVecInertial.GetMagnitude();

            // accumulate all range corrections
            for (UnsignedInt j = 0; j < current->correctionIDs.size(); ++j)
            {
               if (current->useCorrection[j])
               {
                  if (current->correctionIDs[j] == "Troposphere")
                     tropoCorrection += current->corrections[j];
                  else if (current->correctionIDs[j] == "Ionosphere")
                     ionoCorrection += current->corrections[j];
                  else if (current->correctionIDs[j] == "Relativity")
                     relCorrection += current->corrections[j];
                  else if (current->correctionIDs[j] == "ET-TAI")
                     ettaiCorrection += current->corrections[j];
               }
            }

            // accumulate all range correction associate with hardware delay
            delayCorrection += (current->tDelay + current->rDelay)*(GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM);

            current = current->next;
         }
         Real realRange = lightTimeRange + relCorrection + ettaiCorrection + tropoCorrection + ionoCorrection + delayCorrection;

         MessageInterface::ShowMessage("*** Summary of path %d ******************\n", i);
         if (signalPaths[i]->IsSignalFeasible())
            MessageInterface::ShowMessage("   .This path is feasible\n");
         else
            MessageInterface::ShowMessage("   .This path is unfeasible\n");

         MessageInterface::ShowMessage("   .Light time range         : %.12lf km\n", lightTimeRange);
         MessageInterface::ShowMessage("   .Relativity Correction    : %.12lf km\n", relCorrection);
         MessageInterface::ShowMessage("   .ET-TAI correction        : %.12lf km\n", ettaiCorrection);
         MessageInterface::ShowMessage("   .Troposphere correction   : %.12lf km\n", tropoCorrection);
         MessageInterface::ShowMessage("   .Ionosphere correction    : %.12lf km\n", ionoCorrection);
         MessageInterface::ShowMessage("   .Hardware delay correction: %.12lf km\n", delayCorrection);
         MessageInterface::ShowMessage("   .Real range               : %.12lf km\n", realRange);
         MessageInterface::ShowMessage("******************************************************************\n\n");
      #endif

      // 4.5.3. Verify feasibility:
      #ifdef DEBUG_TIMING
         MessageInterface::ShowMessage("4.3. Specify feasibility for signal path %d:\n", i);
      #endif

#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "      Feasible, or not" << std::endl;
   }
#endif

      feasible = feasible && signalPaths[i]->IsSignalFeasible();

      if (forStrand != -1)
         break;
   }
   
   // 5. Save the states of the objects being propagated, as they can be changed during calculation of the measurement values
   RestoreState(precTimeVec, epochVec, epochGTVec, valsVec);
   
   retval = true;

   #ifdef DEBUG_CALCULATE_MEASUREMENT
      if (feasible)
         MessageInterface::ShowMessage("**** All paths in this measurement are feasible\n");
      else
         MessageInterface::ShowMessage("**** Some paths in this measurement are unfeasible\n");

      MessageInterface::ShowMessage("**** Calculation for this measurement is completed ****\n\n");
   #endif

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage(" Exit MeasureModel::CalculateMeasurement(%s, <%p>, <%p>)\n", (withEvents?"true":"false"), forObservation, rampTB); 
   #endif

#ifdef DEBUG_API
   if (apimmFileOpen)
   {
      apimmFile << "   Leaving MeasureModel::CalculateMeasurement(); "
            "wasn't that fun?" << std::endl;
   }
#endif

   return retval;
}


//------------------------------------------------------------------------------
// bool ReCalculateFrequencyAndMediaCorrection(UnsignedInt pathIndex, 
//        Real uplinkFrequency, std::vector<RampTableData>* rampTB)
//------------------------------------------------------------------------------
/**
* This function is used to recalculate frequency and media correction for TDRS
* Doppler measurement.
*
* @param pathIndex           Calculation for the given signal path specified by
*                            pathIndex
* @param uplinkFrequency     Transmit frequency
* @param rampTB              Ramp table for a ramped measurement
*
* @return                    true if no error ocurrs, false otherwise
*/
//------------------------------------------------------------------------------
bool MeasureModel::ReCalculateFrequencyAndMediaCorrection(UnsignedInt pathIndex, 
                        Real uplinkFrequency, std::vector<RampTableData>* rampTB)
{
   bool retval = false;
   
   SignalBase *leg;
   // 1. Compute signal frequency
   #ifdef DEBUG_TIMING
      MessageInterface::ShowMessage("Compute signal frequency on each leg for signal path %d:\n", pathIndex);
   #endif
   leg = signalPaths[pathIndex];
   leg->SignalFrequencyCalculation(rampTB, uplinkFrequency);          // calculate signal frequency on each signal leg

   leg = leg->GetNext();
   while(leg != NULL)
   {
      leg->SignalFrequencyCalculation(rampTB);                       // calculate signal frequency on each signal leg
      leg = leg->GetNext();
   }

   // 2. Compute media correction and hardware delay
   #ifdef DEBUG_TIMING
      MessageInterface::ShowMessage("Calculate media correction for signal path %d:\n", pathIndex);
   #endif
   leg = signalPaths[pathIndex];
   while(leg != NULL)
   {
      leg->MediaCorrectionCalculation(rampTB);                       // calculate media corrections for signal leg
      leg = leg->GetNext();
   }


   #ifdef DEBUG_CALCULATE_MEASUREMENT
         SignalData *current  = theData[pathIndex];
         Real lightTimeRange  = 0.0;                      // unit: km
         Real tropoCorrection = 0.0;                      // unit: km
         Real ionoCorrection  = 0.0;                      // unit: km
         Real relCorrection   = 0.0;                      // unit: km
         Real ettaiCorrection = 0.0;                      // unit: km
         Real delayCorrection = 0.0;                      // unit: km
         while (current != NULL)
         {
            // accumulate all light time correction
            lightTimeRange += current->rangeVecInertial.GetMagnitude();

            // accumulate all range corrections
            for (UnsignedInt j = 0; j < current->correctionIDs.size(); ++j)
            {
               if (current->useCorrection[j])
               {
                  if (current->correctionIDs[j] == "Troposphere")
                     tropoCorrection += current->corrections[j];
                  else if (current->correctionIDs[j] == "Ionosphere")
                     ionoCorrection += current->corrections[j];
                  else if (current->correctionIDs[j] == "Relativity")
                     relCorrection += current->corrections[j];
                  else if (current->correctionIDs[j] == "ET-TAI")
                     ettaiCorrection += current->corrections[j];
               }
            }

            // accumulate all range correction associate with hardware delay
            delayCorrection += (current->tDelay + current->rDelay)*(GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM);

            current = current->next;
         }
         Real realRange = lightTimeRange + relCorrection + ettaiCorrection + tropoCorrection + ionoCorrection + delayCorrection;

         MessageInterface::ShowMessage("*** Summary of path %d ******************\n", pathIndex);
         if (signalPaths[pathIndex]->IsSignalFeasible())
            MessageInterface::ShowMessage("   .This path is feasible\n");
         else
            MessageInterface::ShowMessage("   .This path is unfeasible\n");

         MessageInterface::ShowMessage("   .Light time range         : %.12lf km\n", lightTimeRange);
         MessageInterface::ShowMessage("   .Relativity Correction    : %.12lf km\n", relCorrection);
         MessageInterface::ShowMessage("   .ET-TAI correction        : %.12lf km\n", ettaiCorrection);
         MessageInterface::ShowMessage("   .Troposphere correction   : %.12lf km\n", tropoCorrection);
         MessageInterface::ShowMessage("   .Ionosphere correction    : %.12lf km\n", ionoCorrection);
         MessageInterface::ShowMessage("   .Hardware delay correction: %.12lf km\n", delayCorrection);
         MessageInterface::ShowMessage("   .Real range               : %.12lf km\n", realRange);
         MessageInterface::ShowMessage("******************************************************************\n\n");
   #endif


   return retval;
}


////------------------------------------------------------------------------------
//// StringArray* DecomposePathString(const std::string &value)
////------------------------------------------------------------------------------
///**
// * Breaks apart a signal path string into a participant list
// *
// * @param value The string
// *
// * @return
// */
////------------------------------------------------------------------------------
//StringArray* MeasureModel::DecomposePathString(const std::string &value)
//{
//   TextParser theParser;
//   StringArray *partList = new StringArray;
//
//   (*partList)  = theParser.SeparateBy(value, ", ");
//
//   MessageInterface::ShowMessage("Path string %s has members:\n",
//         value.c_str());
//   for (UnsignedInt i = 0; i < partList->size(); ++i)
//      MessageInterface::ShowMessage("   %s\n", partList->at(i).c_str());
//
//   return partList;
//}


//------------------------------------------------------------------------------
// const std::vector<RealArray>& CalculateMeasurementDerivatives(GmatBase* obj, 
//       Integer id, bool forRangeRate, Integer forStrand)
//------------------------------------------------------------------------------
/**
 * Computes the measurement derivative
 *
 * @param obj            The "with respect to" object owning the "with respect to"
 *                       parameter.
 * @param id             The ID of the "with respect to" field
 * @param forRangeRate   flag to indicate calculate range rate partial derivative
 * @param forStrand      calculate derivative for a given strand (It is not used
 *                       in current GMAT version
 *
 * @return        The vector of derivative data
 */
//------------------------------------------------------------------------------
const std::vector<RealArray>& MeasureModel::CalculateMeasurementDerivatives(
      GmatBase* obj, Integer id, GmatTime* measTime, bool forRangeRate, Integer forStrand)
{
   #ifdef DEBUG_DERIVATIVE
      MessageInterface::ShowMessage("MeasureModel::CalculateMeasurementDerivatives(obj name = %s, id = %d) called\n", obj->GetName().c_str(), id);
   #endif

   theDataDerivatives.clear();
   
   // Collect the data from the signals
   if (forStrand == -1)
   {
      for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
      {
         std::vector<RealArray> pathDerivative =
               signalPaths[i]->ModelSignalDerivative(obj, id, measTime, forRangeRate);
         for (UnsignedInt j = 0; j < pathDerivative.size(); ++j)
            theDataDerivatives.push_back(pathDerivative[j]);
      }
   }
   else
   {
      std::vector<RealArray> pathDerivative =
      signalPaths[forStrand]->ModelSignalDerivative(obj, id, measTime, forRangeRate);
      for (UnsignedInt j = 0; j < pathDerivative.size(); ++j)
         theDataDerivatives.push_back(pathDerivative[j]);
   }

   #ifdef DEBUG_DERIVATIVE
      MessageInterface::ShowMessage("Exit MeasureModel::CalculateMeasurementDerivatives()\n");
   #endif
   return theDataDerivatives;
}

//------------------------------------------------------------------------------
// void UsesLightTime(const bool tf)
//------------------------------------------------------------------------------
/**
 * Method used to set or clear the light time solution flag
 *
 * @param tf true to find light time solutions, false to omit them
 */
//------------------------------------------------------------------------------
void MeasureModel::UsesLightTime(const bool tf)
{
   withLighttime = tf;
}


//------------------------------------------------------------------------------
// void PrepareToPropagate()
//------------------------------------------------------------------------------
/**
 * Prepares the propagators used in light time solution computations
 */
//------------------------------------------------------------------------------
void MeasureModel::PrepareToPropagate()
{
   if (propsNeedInit == false)
      return;

   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Called SignalBase::PrepareToPropagate()\n");
   #else
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Called SignalBase::PrepareToPropagate()\n");
      #endif
   #endif
   
   // Set propagators for spacecraft and formations only
   for (std::map<SpacePoint*,PropSetup*>::iterator i = propMap.begin();
         i != propMap.end(); ++i)
   {
      PropSetup *tProp = i->second;
      if ((i->first->IsOfType(Gmat::SPACEOBJECT)) && (tProp != NULL))
      {
         Propagator *prop = tProp->GetPropagator();
         ODEModel *ode = tProp->GetODEModel();
         PropagationStateManager *psm = tProp->GetPropStateManager();
         
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Integrator for %s has address "
                  "<%p>\n", i->first->GetName().c_str(), prop);
            MessageInterface::ShowMessage("ODEModel has address <%p>\n",
                  ode);
            MessageInterface::ShowMessage("PropSetup:\n***********************"
                  "*************************************************\n%s\n"
                  "***********************************************************"
                  "*************\n",
                  tProp->GetGeneratingString(Gmat::NO_COMMENTS).c_str());

            MessageInterface::ShowMessage("space object:\n***********************"
               "*************************************************\n%s\n"
               "***********************************************************"
               "*************\n",
               i->first->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
         #endif

         ObjectArray objects;
         objects.push_back(i->first);

         psm->SetObject(i->first);
         psm->SetProperty("CartesianState");
         // For now, always propagate the STM.  Toggle off for simulation?
         psm->SetProperty("STM");

         // Add properties for EstimatedParameters
         ObjectArray estParamObjs = (i->first)->GetRefObjectArray(GmatType::GetTypeId("EstimatedParameter"));
         for (ObjectArray::iterator ep = estParamObjs.begin(); ep != estParamObjs.end(); ++ep)
         {
            if ((*ep)->GetStringParameter("SolveFor") == "Cd")
               psm->SetProperty("Cd_Epsilon");
            else if ((*ep)->GetStringParameter("SolveFor") == "AtmosDensityScaleFactor")
               psm->SetProperty("AtmosDensity_Epsilon");
         }

         // This command needs to move here after the line: psm->SetObject(i->first)
         AddTransientForce(i->first, ode, psm);
         
         psm->BuildState();
         
         // Only update psm's state vector for ode model. 
         // For ephemeris propagator, spacecraft's state sometime is undefined 
         // in the script. It makes GMAT crashes when undefined state is mapped 
         // to psm's state vector 
         //psm->MapObjectsToVector();
         
         if (ode)
         {
            psm->MapObjectsToVector();

            ode->SetState(psm->GetState());
            ode->SetSolarSystem(solarsys);
         
            prop->SetPhysicalModel(ode);
            prop->SetSolarSystem(solarsys);
            prop->Initialize();

            ode->SetPropStateManager(psm);
            if (ode->BuildModelFromMap() == false)
               throw MeasurementException("MeasureModel::PrepareToPropagate(): "
                     "Unable to assemble the ODE model for " + tProp->GetName());
         }
         else
         {
            prop->SetSolarSystem(solarsys);
            prop->SetPropStateManager(psm);
//            SpacePoint *sat = i->first;
            prop->SetRefObject(i->first, i->first->GetType(), i->first->GetName());
            prop->Initialize();
         }
         
         prop->Update(true);
         
         if (ode)
         {
            if (ode->SetupSpacecraftData(&objects, 0) <= 0)
               throw MeasurementException("Propagate::Initialize -- "
                     "ODE model for Signal cannot set spacecraft parameters");
         }
      }
   }

   propsNeedInit = false;
}


void MeasureModel::SaveState(std::vector<bool>& precTimeVec, std::vector<GmatEpoch>& epochVec,
   std::vector<GmatTime>& epochGTVec, std::vector<Real>& valsVec)
{
   // this will iterate in sorted order of the keys
   for (std::map<SpacePoint*, PropSetup*>::iterator i = propMap.begin();
      i != propMap.end(); ++i)
   {
      PropSetup *tProp = i->second;
      if ((i->first->IsOfType(Gmat::SPACEOBJECT)) && (tProp != NULL))
      {
         // copy Object values to Vector
         PropagationStateManager *psm1 = tProp->GetPropStateManager();
         psm1->MapObjectsToVector();
         
         // copy Vector values to saved state values
         GmatState *state1 = psm1->GetState();
         bool hasPrecTime1 = state1->HasPrecisionTime();
         GmatEpoch epoch1 = state1->GetEpoch();
         GmatTime epochGT1 = state1->GetEpochGT();
         Real *vals1 = state1->GetState();
         int size1 = state1->GetSize();
         
         precTimeVec.push_back(hasPrecTime1);
         epochVec.push_back(epoch1);
         epochGTVec.push_back(epochGT1);
         for (int j = 0; j < size1; j++)
            valsVec.push_back(vals1[j]);
      }
   }
}


void MeasureModel::RestoreState(std::vector<bool>& precTimeVec, std::vector<GmatEpoch>& epochVec,
   std::vector<GmatTime>& epochGTVec, std::vector<Real>& valsVec)
{
   int index = 0, valsIndex = 0;
   
   // this will iterate in sorted order of the keys
   for (std::map<SpacePoint*, PropSetup*>::iterator i = propMap.begin();
      i != propMap.end(); ++i)
   {
      PropSetup *tProp = i->second;
      if ((i->first->IsOfType(Gmat::SPACEOBJECT)) && (tProp != NULL))
      {
         PropagationStateManager *psm1 = tProp->GetPropStateManager();
         GmatState *state1 = psm1->GetState();
         
         // copy saved state values to Vector
         state1->SetPrecisionTimeFlag(precTimeVec.at(index));
         state1->SetEpoch(epochVec.at(index));
         state1->SetEpochGT(epochGTVec.at(index));
         
         int size1 = state1->GetSize();
         for (int j = 0; j < size1; j++, valsIndex++)
            (*state1)[j] = valsVec.at(valsIndex);
         
         // copy Vector values to Objects
         psm1->MapVectorToObjects();
         
         index++;
      }
   }
}

//------------------------------------------------------------------------------
// void UseIonosphereCache(SignalDataCache::SimpleSignalDataCache* cache)
//------------------------------------------------------------------------------
/**
 * Passes the ionosphere cache to the signal path
 *
 * @param cache The ionosphere cache
 *
 */
 //------------------------------------------------------------------------------
void MeasureModel::UseIonosphereCache(SignalDataCache::SimpleSignalDataCache* cache)
{
   for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
   {
      signalPaths[i]->SetIonosphereCache(cache);
   }
}

//------------------------------------------------------------------------------
// bool MeasureModel::IsCrossLink()
//------------------------------------------------------------------------------
/**
 * Returns true if the measurement is crosslink tracking, ie spacecraft to spacecraft
 *
 * @return true if the measurement is crosslink tracking, false if not
 *
 */
 //------------------------------------------------------------------------------
bool MeasureModel::IsCrossLink()
{
    bool retval = false;
    
    for (int i = 0; i < participants.size(); i++)
    {
        bool isCrossLink = true;
        for (int j = 0; j < participants[i]->size(); j++)
        {
            if (!participants[i]->at(j)->IsOfType(Gmat::SPACECRAFT))
                isCrossLink = false;
        }
        retval = retval || isCrossLink;
    }

    return retval;
}

//------------------------------------------------------------------------------
// void SetCorrection(const std::string correctionName,
//       const std::string correctionType)
//------------------------------------------------------------------------------
/**
 * Passes a correction name into the owned CoreMeasurement
 *
 * @param correctionName The name of the correction
 * @param correctionType The type of correction
 *
 */
//------------------------------------------------------------------------------
void MeasureModel::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   // Add new measurement correction type to correctionTypeList
   // Update correction model into correctionModelList as needed
   UnsignedInt i = 0;
   for (; i < correctionTypeList.size(); ++i)
   {
      if (correctionType == correctionTypeList[i])
         break;
   }

   if (i == correctionTypeList.size())
   {
      correctionTypeList.push_back(correctionType);
      correctionModelList.push_back(correctionName);
   }
   else
      correctionModelList[i] = correctionName;
}


//------------------------------------------------------------------------------
// void AddCorrection(const std::string correctionName,
//       const std::string correctionType)
//------------------------------------------------------------------------------
/**
 * Add a type of measurement correction to measurement signal path
 *
 * @param correctionName The name of the correction
 * @param correctionType The type of correction
 *
 */
//------------------------------------------------------------------------------
void MeasureModel::AddCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
   {
      signalPaths[i]->AddCorrection(correctionName, correctionType);
   }
}


void MeasureModel::SetTimeTagFlag(const bool flag)
{
   epochIsAtEnd = flag;
}


bool MeasureModel::GetTimeTagFlag()
{
   return epochIsAtEnd;
}


void MeasureModel::SetCountInterval(Real timeInterval)
{
   countInterval = timeInterval;
}



const std::vector<ObjectArray*>& MeasureModel::GetParticipantObjectLists()
{
   return participants;
}


//-------------------------------------------------------------------------------------------------------------
// Real GetUplinkFrequency(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB)
//-------------------------------------------------------------------------------------------------------------
/**
* This fucntion is used to get uplink frequency at transmit epoch.
*
* @param pathIndex        index of a signal path
* @param ramTB            a pointer to a ramp table
*
* @return                 uplink frequency (in MHz) at transmit epoch
*/
//-------------------------------------------------------------------------------------------------------------
Real MeasureModel::GetUplinkFrequency(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB)
{
   
   // 1. Specify the first signal leg
   PhysicalSignal* fleg = (PhysicalSignal*)signalPaths[pathIndex];
   SignalData* sd = fleg->GetSignalDataObject();
   
   // 2. Get frequency from sd.tNode
   if (sd->tNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Transmit participant of signal path is NULL.\n";
      throw MeasurementException(ss.str());
   }
   
   Real frequency = 0.0;                                                                     // unit: Mhz
   if (sd->tNode->IsOfType(Gmat::GROUND_STATION))
   {
      // Get frequency from ground station's transmiter or from ramped frequency table
      if (rampTB)
      {
         // Get frequency from ramp table if it is used
         GmatTime t1 = sd->tPrecTime - sd->tDelay/GmatTimeConstants::SECS_PER_DAY;
         frequency = fleg->GetFrequencyFromRampTable(t1.GetMjd(), rampTB)/1.0e6;       // unit: Mhz
      }
      else
      {
         // Get frequency from ground station' transmitter
         ObjectArray hardwareList = ((GroundstationInterface*)(sd->tNode))->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            // Before getting transmit signal, it has to specify time at which the signal is transmitted
            // This step is only needed for frequency bias caused by Transmitter
            // Get transmit frequency
            if (hardwareList[i]->IsOfType("Transmitter"))
            {
               GmatTime t1 = sd->tPrecTime - sd->tDelay / GmatTimeConstants::SECS_PER_DAY;
               ((Transmitter*)hardwareList[i])->SetEpoch(t1);
               frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();   // unit: MHz 
               break;
            }
         }
         
         if (i == hardwareList.size())
         {
            std::stringstream ss;
            ss << "Error: Ground station " << sd->tNode->GetName() << " does not have a transmitter to transmit signal.\n";
            throw MeasurementException(ss.str());
         }
      }
   }
   else
   {
      // Get frequency from spacecraft's transmitter or transponder
      ObjectArray hardwareList = ((Spacecraft*)sd->tNode)->GetRefObjectArray(Gmat::HARDWARE);
      UnsignedInt i;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         // Before getting transmit signal, it has to specify time at which the signal is transmitted
         // This step is only needed for variant frequency caused by Transmitter
         // Get transmit frequency
         if (hardwareList[i]->IsOfType("Transmitter"))
         {
            GmatTime t1 = sd->tPrecTime - sd->tDelay / GmatTimeConstants::SECS_PER_DAY;
            ((Transmitter*)hardwareList[i])->SetEpoch(t1);
            frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();    // unit: MHz 
            break;
         }
      }

      if (i == hardwareList.size())
      {
         std::stringstream ss;
         ss << "Error: Spacecraft " << sd->tNode->GetName() << " does not have a transmitter to transmit signal.\n";
         throw MeasurementException(ss.str());
      }
   }

   return frequency;
}


//-------------------------------------------------------------------------------------------------------------
// Real GetUplinkFrequencyAtReceivedEpoch(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB)
//-------------------------------------------------------------------------------------------------------------
/**
* This fucntion is used to get uplink frequency at received epoch.
*
* @param pathIndex        index of a signal path
* @param ramTB            a pointer to a ramp table
*
* @return                 uplink frequency (in MHz) at recieved epoch
*/
//-------------------------------------------------------------------------------------------------------------
Real MeasureModel::GetUplinkFrequencyAtReceivedEpoch(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB)
{
   // 1. Specify the first signal leg and last signal leg
   PhysicalSignal* fleg = (PhysicalSignal*)signalPaths[pathIndex];
   PhysicalSignal* lastLeg = fleg;
   while (lastLeg->GetNext() != NULL)
      lastLeg = (PhysicalSignal*)(lastLeg->GetNext());

   // 2. Get uplink frequency at receive epoch
   Real frequency = 0.0;                                                                              // unit: Mhz
   if (rampTB)
   {
      // 2.1.1. Get received epoch
      SignalData *sd = lastLeg->GetSignalDataObject();
      GmatTime t1 = sd->rPrecTime + sd->rDelay / GmatTimeConstants::SECS_PER_DAY;

      // 2.1.2. Get frequency from ramp table at received epoch
      frequency = fleg->GetFrequencyFromRampTable(t1.GetMjd(), rampTB) / 1.0e6;             // unit: MHz
   }
   else
   {
      // 2.2.1. Get frequency from transmitter
      SignalData *sd = fleg->GetSignalDataObject();
      if (sd->tNode == NULL)
         throw MeasurementException("Error: transmit participant of signal path is NULL.\n");

      if (sd->tNode->IsOfType(Gmat::GROUND_STATION))
      {
         // Get frequency from ground station' transmitter
         ObjectArray hardwareList = ((GroundstationInterface*)(sd->tNode))->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            // Before getting transmit signal, it has to specify time at which the signal is transmitted
            // This step is only needed for variant frequency caused by Transmitter
            // Get transmit frequency
            if (hardwareList[i]->IsOfType("Transmitter"))
            {
               GmatTime t1 = sd->tPrecTime - sd->tDelay / GmatTimeConstants::SECS_PER_DAY;
               ((Transmitter*)hardwareList[i])->SetEpoch(t1);
               frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();   // unit: MHz 
               break;
            }
         }

         if (i == hardwareList.size())
         {
            std::stringstream ss;
            ss << "Error: Ground station " << sd->tNode->GetName() << " does not have a transmitter to transmit signal.\n";
            throw MeasurementException(ss.str());
         }
      }
      else
      {
         // Get frequency from spacecraft's transmitter or transponder
         ObjectArray hardwareList = ((Spacecraft*)(sd->tNode))->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            // Before getting transmit signal, it has to specify time at which the signal is transmitted
            // This step is only needed for variant frequency caused by Transmitter
            // Get transmit frequency
            if (hardwareList[i]->IsOfType("Transmitter"))
            {
               GmatTime t1 = sd->tPrecTime - sd->tDelay / GmatTimeConstants::SECS_PER_DAY;
               ((Transmitter*)hardwareList[i])->SetEpoch(t1);
               frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();    // unit: MHz 
               break;
            }
         }

         if (i == hardwareList.size())
         {
            std::stringstream ss;
            ss << "Error: Spacecraft " << sd->tNode->GetName() << " does not have a transmitter to transmit signal.\n";
            throw MeasurementException(ss.str());
         }
      }
   }

   return frequency;
}


//-------------------------------------------------------------------------------------------------------------
// Real GetUplinkFrequencyBand(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB)
//-------------------------------------------------------------------------------------------------------------
/**
* This fucntion is used to get uplink frequency band.
*
* @param pathIndex        index of a signal path
* @param ramTB            a pointer to a ramp table
*
* @return                 uplink's frequency band
*/
//-------------------------------------------------------------------------------------------------------------
Integer MeasureModel::GetUplinkFrequencyBand(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB)
{
   // 1. Specify the first signal leg
   PhysicalSignal* fleg = (PhysicalSignal*)signalPaths[pathIndex];
   SignalData *sd = fleg->GetSignalDataObject();

   // 2. Get frequency from sd.tNode
   if (sd->tNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Transmit participant of signal path is NULL.\n";
      throw MeasurementException(ss.str());
   }

   Integer freqBand = 0;
   Real frequency = 0.0;
   if (sd->tNode->IsOfType(Gmat::GROUND_STATION))
   {
      // Get frequency from ground station's transmiter or from ramped frequency table
      if (rampTB)
      {
         // Get frequency from ramp table if it is used
         GmatTime t1 = sd->tPrecTime - sd->tDelay/GmatTimeConstants::SECS_PER_DAY;
         freqBand = fleg->GetFrequencyBandFromRampTable(t1.GetMjd(), rampTB);
      }
      else
      {
         // Get frequency from ground station' transmitter
         ObjectArray hardwareList = ((GroundstationInterface*)(sd->tNode))->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            // Before getting transmit signal, it has to specify time at which the signal is transmitted
            // This step is only needed for variant frequency caused by Transmitter
            // Get transmit frequency

            if (hardwareList[i]->IsOfType("Transmitter"))
            {
               GmatTime t1 = sd->tPrecTime - sd->tDelay / GmatTimeConstants::SECS_PER_DAY;
               ((Transmitter*)hardwareList[i])->SetEpoch(t1);
               frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();   // unit: MHz
               freqBand = fleg->FrequencyBand(frequency*1.0e6);
               break;
            }
         }
         
         if (i == hardwareList.size())
         {
            std::stringstream ss;
            ss << "Error: Ground station " << sd->tNode->GetName() << " does not have a transmitter to transmit signal.\n";
            throw MeasurementException(ss.str());
         }
      }
   }
   else
   {
      // Get frequency from spacecraft's transmitter or transponder
      ObjectArray hardwareList = ((Spacecraft*)(sd->tNode))->GetRefObjectArray(Gmat::HARDWARE);
      UnsignedInt i;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         // Before getting transmit signal, it has to specify time at which the signal is transmitted
         // This step is only needed for variant frequency caused by Transmitter
         // Get transmit frequency
         if (hardwareList[i]->IsOfType("Transmitter"))
         {
            GmatTime t1 = sd->tPrecTime - sd->tDelay / GmatTimeConstants::SECS_PER_DAY;
            ((Transmitter*)hardwareList[i])->SetEpoch(t1);
            frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();    // unit: MHz 
            freqBand = fleg->FrequencyBand(frequency*1.0e6);
            break;
         }
      }

      if (i == hardwareList.size())
      {
         std::stringstream ss;
         ss << "Error: Spacecraft " << sd->tNode->GetName() << " does not have a transmitter to transmit signal.\n";
         throw MeasurementException(ss.str());
      }
   }

   return freqBand;
}

//-------------------------------------------------------------------------------------------------------------
// Real GetUplinkFrequency(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB)
//-------------------------------------------------------------------------------------------------------------
/**
* This fucntion is used to get uplink frequency at transmit epoch.
*
* @param pathIndex        index of a signal path
* @param ramTB            a pointer to a ramp table
* @param obsData          a pointer to the observation data
*
* @return                 uplink frequency (in MHz) at transmit epoch
*/
//-------------------------------------------------------------------------------------------------------------
Real MeasureModel::GetUplinkFrequency(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB, ObservationData* obsData)
{

   // 1. Specify the first signal leg
   PhysicalSignal* fleg = (PhysicalSignal*)signalPaths[pathIndex];
   SignalData* sd = fleg->GetSignalDataObject();

   // 2. Get frequency from sd.tNode
   if (sd->tNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Transmit participant of signal path is NULL.\n";
      throw MeasurementException(ss.str());
   }

   Real frequency;                                                                     // unit: Mhz
   if (sd->tNode->IsOfType(Gmat::GROUND_STATION))
   {
      // Get frequency from ground station's transmiter or from ramped frequency table
      if (rampTB)
      {
         // Get frequency from ramp table if it is used
         GmatTime t1 = sd->tPrecTime - sd->tDelay / GmatTimeConstants::SECS_PER_DAY;
         frequency = fleg->GetFrequencyFromRampTable(t1.GetMjd(), rampTB) / 1.0e6;       // unit: Mhz
      }
      else if (obsData)
      {
         //Gets data from GMD file
         frequency = obsData->uplinkFreqAtRecei / 1.0e6;
      }
      else
      {
         // Get frequency from ground station' transmitter
         ObjectArray hardwareList = ((GroundstationInterface*)(sd->tNode))->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            // Before getting transmit signal, it has to specify time at which the signal is transmitted
            // This step is only needed for variant frequency caused by Transmitter
            // Get transmit frequency
            if (hardwareList[i]->IsOfType("Transmitter"))
            {
               GmatTime t1 = sd->tPrecTime - sd->tDelay / GmatTimeConstants::SECS_PER_DAY;
               ((Transmitter*)hardwareList[i])->SetEpoch(t1);
               frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();   // unit: MHz 
               break;
            }
         }

         if (i == hardwareList.size())
         {
            std::stringstream ss;
            ss << "Error: Ground station " << sd->tNode->GetName() << " does not have a transmitter to transmit signal.\n";
            throw MeasurementException(ss.str());
         }
      }
   }
   else if (obsData)
   {
      //Gets data from GMD file
      frequency = obsData->uplinkFreqAtRecei / 1.0e6;
   }
   else
   {
      // Get frequency from spacecraft's transmitter or transponder
      ObjectArray hardwareList = ((Spacecraft*)sd->tNode)->GetRefObjectArray(Gmat::HARDWARE);
      UnsignedInt i;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         // Before getting transmit signal, it has to specify time at which the signal is transmitted
         // This step is only needed for variant frequency caused by Transmitter
         // Get transmit frequency
         if (hardwareList[i]->IsOfType("Transmitter"))
         {
            GmatTime t1 = sd->tPrecTime - sd->tDelay / GmatTimeConstants::SECS_PER_DAY;
            ((Transmitter*)hardwareList[i])->SetEpoch(t1);
            frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();    // unit: MHz 
            break;
         }
      }

      if (i == hardwareList.size())
      {
         std::stringstream ss;
         ss << "Error: Spacecraft " << sd->tNode->GetName() << " does not have a transmitter to transmit signal.\n";
         throw MeasurementException(ss.str());
      }
   }

   return frequency;
}


//-------------------------------------------------------------------------------------------------------------
// Real GetUplinkFrequencyAtReceivedEpoch(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB)
//-------------------------------------------------------------------------------------------------------------
/**
* This fucntion is used to get uplink frequency at received epoch.
*
* @param pathIndex        index of a signal path
* @param ramTB            a pointer to a ramp table
* @param obsData          a pointer to the observation data
*
* @return                 uplink frequency (in MHz) at recieved epoch
*/
//-------------------------------------------------------------------------------------------------------------
Real MeasureModel::GetUplinkFrequencyAtReceivedEpoch(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB, ObservationData* obsData)
{
   // 1. Specify the first signal leg and last signal leg
   PhysicalSignal* fleg = (PhysicalSignal*)signalPaths[pathIndex];
   PhysicalSignal* lastLeg = fleg;
   while (lastLeg->GetNext() != NULL)
      lastLeg = (PhysicalSignal*)(lastLeg->GetNext());

   // 2. Get uplink frequency at receive epoch
   Real frequency = 0.0;                                                                              // unit: Mhz
   if (rampTB)
   {
      // 2.1.1. Get received epoch
      SignalData *sd = lastLeg->GetSignalDataObject();
      GmatTime t1 = sd->rPrecTime + sd->rDelay / GmatTimeConstants::SECS_PER_DAY;

      // 2.1.2. Get frequency from ramp table at received epoch
      frequency = lastLeg->GetFrequencyFromRampTable(t1.GetMjd(), rampTB) / 1.0e6;             // unit: MHz
   }
   else if (obsData)
   {
      //Gets data from GMD file
      frequency = obsData->uplinkFreqAtRecei / 1.0e6;
   }
   else
   {
      // 2.2.1. Get frequency from transmitter
      SignalData *sd = fleg->GetSignalDataObject();
      if (sd->tNode == NULL)
         throw MeasurementException("Error: transmit participant of signal path is NULL.\n");

      if (sd->tNode->IsOfType(Gmat::GROUND_STATION))
      {
         // Get frequency from ground station' transmitter
         ObjectArray hardwareList = ((GroundstationInterface*)(sd->tNode))->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            // Before getting transmit signal, it has to specify time at which the signal is transmitted
            // This step is only needed for variant frequency caused by Transmitter
            // Get transmit frequency
            if (hardwareList[i]->IsOfType("Transmitter"))
            {
               GmatTime t1 = sd->tPrecTime - sd->tDelay / GmatTimeConstants::SECS_PER_DAY;
               ((Transmitter*)hardwareList[i])->SetEpoch(t1);
               frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();   // unit: MHz 
               break;
            }
         }

         if (i == hardwareList.size())
         {
            std::stringstream ss;
            ss << "Error: Ground station " << sd->tNode->GetName() << " does not have a transmitter to transmit signal.\n";
            throw MeasurementException(ss.str());
         }
      }
      else
      {
         // Get frequency from spacecraft's transmitter or transponder
         ObjectArray hardwareList = ((Spacecraft*)(sd->tNode))->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            // Before getting transmit signal, it has to specify time at which the signal is transmitted
            // This step is only needed for variant frequency caused by Transmitter
            // Get transmit frequency
            if (hardwareList[i]->IsOfType("Transmitter"))
            {
               GmatTime t1 = sd->tPrecTime - sd->tDelay / GmatTimeConstants::SECS_PER_DAY;
               ((Transmitter*)hardwareList[i])->SetEpoch(t1);
               frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();    // unit: MHz 
               break;
            }
         }

         if (i == hardwareList.size())
         {
            std::stringstream ss;
            ss << "Error: Spacecraft " << sd->tNode->GetName() << " does not have a transmitter to transmit signal.\n";
            throw MeasurementException(ss.str());
         }
      }
   }

   return frequency;
}


//-------------------------------------------------------------------------------------------------------------
// Real GetUplinkFrequencyBand(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB)
//-------------------------------------------------------------------------------------------------------------
/**
* This fucntion is used to get uplink frequency band.
*
* @param pathIndex        index of a signal path
* @param ramTB            a pointer to a ramp table
* @param obsData          a pointer to the observation data
*
* @return                 uplink's frequency band
*/
//-------------------------------------------------------------------------------------------------------------
Integer MeasureModel::GetUplinkFrequencyBand(UnsignedInt pathIndex, std::vector<RampTableData>* rampTB, ObservationData* obsData)
{
   // 1. Specify the first signal leg
   PhysicalSignal* fleg = (PhysicalSignal*)signalPaths[pathIndex];
   SignalData *sd = fleg->GetSignalDataObject();

   // 2. Get frequency from sd.tNode
   if (sd->tNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Transmit participant of signal path is NULL.\n";
      throw MeasurementException(ss.str());
   }

   Integer freqBand = 0;
   Real frequency = 0.0;
   if (sd->tNode->IsOfType(Gmat::GROUND_STATION))
   {
      // Get frequency from ground station's transmiter or from ramped frequency table
      if (rampTB)
      {
         // Get frequency from ramp table if it is used
         GmatTime t1 = sd->tPrecTime - sd->tDelay / GmatTimeConstants::SECS_PER_DAY;
         freqBand = fleg->GetFrequencyBandFromRampTable(t1.GetMjd(), rampTB);
      }
      else if (obsData)
      {
         //Gets data from GMD file
         frequency = obsData->uplinkFreqAtRecei;
         freqBand = fleg->FrequencyBand(frequency);
      }
      else
      {
         // Get frequency from ground station' transmitter
         ObjectArray hardwareList = ((GroundstationInterface*)(sd->tNode))->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            // Before getting transmit signal, it has to specify time at which the signal is transmitted
            // This step is only needed for variant frequency caused by Transmitter
            // Get transmit frequency

            if (hardwareList[i]->IsOfType("Transmitter"))
            {
               GmatTime t1 = sd->tPrecTime - sd->tDelay / GmatTimeConstants::SECS_PER_DAY;
               ((Transmitter*)hardwareList[i])->SetEpoch(t1);
               frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();   // unit: MHz
               freqBand = fleg->FrequencyBand(frequency*1.0e6);
               break;
            }
         }

         if (i == hardwareList.size())
         {
            std::stringstream ss;
            ss << "Error: Ground station " << sd->tNode->GetName() << " does not have a transmitter to transmit signal.\n";
            throw MeasurementException(ss.str());
         }
      }
   }
   else if (obsData)
   {
      //Gets data from GMD file
      frequency = obsData->uplinkFreqAtRecei;
      freqBand = fleg->FrequencyBand(frequency);
   }
   else
   {
      // Get frequency from spacecraft's transmitter or transponder
      ObjectArray hardwareList = ((Spacecraft*)(sd->tNode))->GetRefObjectArray(Gmat::HARDWARE);
      UnsignedInt i;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         // Before getting transmit signal, it has to specify time at which the signal is transmitted
         // This step is only needed for variant frequency caused by Transmitter
         // Get transmit frequency
         if (hardwareList[i]->IsOfType("Transmitter"))
         {
            GmatTime t1 = sd->tPrecTime - sd->tDelay / GmatTimeConstants::SECS_PER_DAY;
            ((Transmitter*)hardwareList[i])->SetEpoch(t1);
            frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();    // unit: MHz 
            freqBand = fleg->FrequencyBand(frequency*1.0e6);
            break;
         }
      }

      if (i == hardwareList.size())
      {
         std::stringstream ss;
         ss << "Error: Spacecraft " << sd->tNode->GetName() << " does not have a transmitter to transmit signal.\n";
         throw MeasurementException(ss.str());
      }
   }

   return freqBand;
}

//-------------------------------------------------------------------------------------------------------------
// Real GetTransmitFrequency(const Integer atNode, const Integer pathIndex)
//-------------------------------------------------------------------------------------------------------------
/**
* This fucntion is used to get transmit.
*
* @param atNode        index of the signal leg
* @param pathIndex     index of the signal path
*
* @return                 Transmit frequency
*/
//-------------------------------------------------------------------------------------------------------------
Real MeasureModel::GetTransmitFrequency(const Integer atNode, const Integer pathIndex)
{
   if (atNode < 0)
      throw MeasurementException("Error: Input node index has a negative integer.\n");

   // 1. Specify the first signal leg
   SignalBase* leg = signalPaths[pathIndex];

   // 2. Specify signal leg containing atNode
   Integer i = 0;
   while (leg != NULL)
   {
      if (i < atNode)
      {
         leg = leg->GetNext();
         ++i;
      }
      else if (i == atNode)
         break;
   }

   if (i < atNode)
   {
      std::stringstream ss;
      ss << "Error: node index " << atNode << "is beyond the maximum node index in the signal path.\n";
      throw MeasurementException(ss.str());
   }

   // 3. Get transmit frequency at the specified node
   SignalData* sd = leg->GetSignalDataObject();

   return sd->transmitFreq;
}

//-------------------------------------------------------------------------------------------------------------
// Real MeasureModel::GetTransmitFrequencyBand(const Integer atNode, const Integer pathIndex)
//-------------------------------------------------------------------------------------------------------------
/**
* This fucntion is used to get transmit frequency band.
*
* @param atNode        index of the signal leg
* @param pathIndex     index of the signal path
*
* @return                 Transmit frequency band
*/
//-------------------------------------------------------------------------------------------------------------
Integer MeasureModel::GetTransmitFrequencyBand(const Integer atNode, const Integer pathIndex)
{
   if (atNode < 0)
      throw MeasurementException("Error: Input node index has a negative integer.\n");

   // 1. Specify the first signal leg
   SignalBase* leg = signalPaths[pathIndex];

   // 2. Specify signal leg containing atNode
   Integer i = 0;
   while (leg != NULL)
   {
      if (i < atNode)
      {
         leg = leg->GetNext();
         ++i;
      }
      else if (i == atNode)
         break;
   }

   if (i < atNode)
   {
      std::stringstream ss;
      ss << "Error: node index " << atNode << "is beyond the maximum node index in the signal path.\n";
      throw MeasurementException(ss.str());
   }

   // 3. Get transmit frequency band at the specified node 
   SignalData* sd = leg->GetSignalDataObject();
   
   return ((PhysicalSignal*)leg)->FrequencyBand(sd->transmitFreq*1.0e6);
}

//-------------------------------------------------------------------------------------------------------------
// Real MeasureModel::GetTransmitFrequencyOscillatorBias(const Integer atNode, const Integer pathIndex)
//-------------------------------------------------------------------------------------------------------------
/**
* This fucntion is used to get transmit frequency band.
*
* @param atNode        index of the signal leg
* @param pathIndex     index of the signal path
*
* @return                 Transmit frequency oscillator bias
*/
//-------------------------------------------------------------------------------------------------------------
Real MeasureModel::GetTransmitFrequencyOscillatorBias(const Integer atNode, const Integer pathIndex)
{
   if (atNode < 0)
      throw MeasurementException("Error: Input node index has a negative integer.\n");

   // 1. Specify the first signal leg
   SignalBase* leg = signalPaths[pathIndex];

   // 2. Specify signal leg containing atNode
   Integer i = 0;
   while (leg != NULL)
   {
      if (i < atNode)
      {
         leg = leg->GetNext();
         ++i;
      }
      else if (i == atNode)
         break;
   }

   if (i < atNode)
   {
      std::stringstream ss;
      ss << "Error: node index " << atNode << " is beyond the maximum node index in the signal path.\n";
      throw MeasurementException(ss.str());
   }

   // 3. Get transmit frequency at the specified node
   SignalData* sd = leg->GetSignalDataObject();

   return sd->transmitFreqOscillatorBias;
}


RealArray MeasureModel::GetFrequencyMixerProperty(const Integer atNode, const Integer pathIndex)
{
   if (atNode < 0)
      throw MeasurementException("Error: Input node index has a negative integer.\n");

   // 1. Specify the first signal leg
   SignalBase* leg = signalPaths[pathIndex];

   // 2. Specify signal leg containing atNode
   Integer i = 0;
   while (leg != NULL)
   {
      if (i < atNode)
      {
         leg = leg->GetNext();
         ++i;
      }
      else if (i == atNode)
         break;
   }

   if (i < atNode)
   {
      std::stringstream ss;
      ss << "Error: node index " << atNode << "is beyond the maximum node index in the signal path.\n";
      throw MeasurementException(ss.str());
   }

   // 3. Get frequency mixer's properties 
   SignalData* sd = leg->GetSignalDataObject();
   SpacePoint* participant = sd->tNode;

   ///@todo: In the current version, frequency mixer's properties a and b are hard coded and set to 1.0.
   ///       In future, we need to implement code for those properties into TDRS. Those properties
   ///       will get from TDRS sd->tNode on the signal leg from TDRS to ground station
   RealArray freqMixerProperties;             // frequency mixer's prrperties have 2 parameters: a and b
   freqMixerProperties.push_back(1.0);        // currently frequency mixer's parameter a is set to 1.0
   freqMixerProperties.push_back(1.0);        // currently frequency mixer's parameter b is set to 1.0
   
   return freqMixerProperties;
}


//------------------------------------------------------------------------------
// void AddTransientForce(GmatBase *spacePoint, ForceModel *odeModel,
//       PropagationStateManager *propMan)
//------------------------------------------------------------------------------
/**
* Passes transient forces into the ForceModel(s).
*
* @param spacePoint The satellite used in the ForceModel.
* @param odeModel   The current ForceModel that is receiving the forces.
* @param propMan    PropagationStateManager for this PropSetup
*/
//------------------------------------------------------------------------------
void MeasureModel::AddTransientForce(GmatBase *spacePoint,
    ODEModel *odeModel, PropagationStateManager *propMan)
{
#ifdef DEBUG_TRANSIENT_FORCES
    MessageInterface::ShowMessage
        ("MeasureModel::AddTransientForce() entered, ODEModel=<%p>,"
        " transientForces=<%p>\n", odeModel, transientForces);
#endif

    // Find any transient force that is active and add it to the force model
    StringArray satsThatManeuver, formationSatsThatManeuver, formsThatManeuver;
    bool flagMultipleBurns = false;

    if (transientForces)
    {
       for (std::vector<PhysicalModel*>::iterator i = transientForces->begin();
           i != transientForces->end(); ++i)
       {
           StringArray tfSats = (*i)->GetRefObjectNameArray(Gmat::SPACECRAFT);

           // See if the spacecraft that goes with the force model is in the spacecraft
           // list for the current transient force
           std::string satname = spacePoint->GetName();
            
           if (find(tfSats.begin(), tfSats.end(), satname) != tfSats.end())
           {
#ifdef DEBUG_TRANSIENT_FORCES
            MessageInterface::ShowMessage
                ("   Adding transientForce <%p>'%s' to ODEModel\n", *i,
                (*i)->GetName().c_str());
#endif
               //            if (find(satsThatManeuver.begin(), satsThatManeuver.end(), satname) == satsThatManeuver.end())
               odeModel->AddForce(*i);
               //            else
               //               flagMultipleBurns = true;
               if (find(satsThatManeuver.begin(), satsThatManeuver.end(), satname) == satsThatManeuver.end())
                   satsThatManeuver.push_back(satname);
               if ((*i)->DepletesMass())
               {
                   propMan->SetProperty("MassFlow");
               //      //               propMan->SetProperty("MassFlow",
               //      //                     (*i)->GetRefObject(Gmat::SPACECRAFT, *current));
#ifdef DEBUG_TRANSIENT_FORCES
            //      MessageInterface::ShowMessage("   %s depletes mass\n",
            //          (*i)->GetName().c_str());
#endif
               }
               break;      // Avoid multiple adds
           }
           // if satname not found, PropagationEnabledCommand should have already checked whether it is a Formation
       }
    }

#ifdef DEBUG_TRANSIENT_FORCES
    MessageInterface::ShowMessage("MeasureModel: Found %d sats that maneuver (outside of "
        "formations):\n", satsThatManeuver.size());
    for (UnsignedInt i = 0; i < satsThatManeuver.size(); ++i)
        MessageInterface::ShowMessage("   %s\n", satsThatManeuver[i].c_str());
#endif

    // the PropagationEnabledCommand should have already checked for multiple burns by the same S/C,
    // if that check is being done

//#ifdef DEBUG_TRANSIENT_FORCES
//    ODEModel *fm;
//    PhysicalModel *pm;
//
//    MessageInterface::ShowMessage(
//        "MeasureModel::AddTransientForces completed; force details:\n");
//    for (std::vector<PropSetup*>::iterator p = propagators.begin();
//        p != propagators.end(); ++p)
//    {
//        fm = (*p)->GetODEModel();
//        if (!fm)
//            throw CommandException("ODEModel not set in PropSetup \"" +
//            (*p)->GetName() + "\"");
//        MessageInterface::ShowMessage(
//            "   Forces in %s:\n", fm->GetName().c_str());
//        for (Integer i = 0; i < fm->GetNumForces(); ++i)
//        {
//            pm = fm->GetForce(i);
//            MessageInterface::ShowMessage(
//                "      %15s   %s\n", pm->GetTypeName().c_str(),
//                pm->GetName().c_str());
//        }
//    }
//#endif
}
