//$Id$
//------------------------------------------------------------------------------
//                           IntrusionLocator
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: May 10, 2021
//
/**
 * Definition of the IntrusionLocator
 */
 //------------------------------------------------------------------------------


#ifndef IntrusionLocator_hpp
#define IntrusionLocator_hpp


#include "EventLocator.hpp"
#include "EventLocatorDefs.hpp"
#include "SpiceInterface.hpp"
#include "IntrusionResult.hpp"


/**
 * Event locator used for intrusion events
 *
 * This class manages station contacts for a set of target SpaceObjects
 * (typically Spacecraft).  Contact events require that two types of event
 * function be satisfied: an elevation function, placing the target above the
 * station's horizon, and (zero or more) line-of-sight event functions, ensuring
 * that there is no obstructing object between the station and the target.
 *
 * @note: Work on this event type is temporarily on hold.  The current state is
 * a set of event functions coded but untested, and a containing IntrusionLocator
 * which has untested initialization but is missing the reference object
 * methods.
 */
class LOCATOR_API IntrusionLocator : public EventLocator
{
public:
   IntrusionLocator(const std::string &name);
   virtual ~IntrusionLocator();
   IntrusionLocator(const IntrusionLocator& el);
   IntrusionLocator& operator=(const IntrusionLocator& cl);

   // Inherited (GmatBase) methods for parameters
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
      GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
      const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
      const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
      const std::string &value,
      const Integer index);
   virtual const StringArray&
      GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
      GetStringArrayParameter(const Integer id,
         const Integer index) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
      const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
      const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
      const std::string &value,
      const Integer index);
   virtual const StringArray&
      GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray&
      GetStringArrayParameter(const std::string &label,
         const Integer index) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
      const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
      const Real value);

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
      const bool value);
   virtual bool         GetBooleanParameter(const std::string& label) const;
   virtual bool         SetBooleanParameter(const std::string& label,
      const bool value);

   virtual UnsignedInt
      GetPropertyObjectType(const Integer id) const;
   virtual const StringArray&
      GetPropertyEnumStrings(const Integer id) const;
   virtual const ObjectTypeArray&
      GetTypesForList(const Integer id);
   virtual const ObjectTypeArray&
      GetTypesForList(const std::string &label);

   virtual GmatBase*    GetRefObject(const UnsignedInt type,
      const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
      const std::string &name);

   virtual bool         HasRefObjectTypeArray();
   virtual const StringArray&
      GetRefObjectNameArray(const UnsignedInt type);
   virtual const ObjectTypeArray&
      GetRefObjectTypeArray();


   virtual bool         RenameRefObject(const UnsignedInt type,
      const std::string &oldName,
      const std::string &newName);

   virtual bool         TakeAction(const std::string &action,
      const std::string &actionData = "");


   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   virtual bool         Initialize();
   virtual bool         ReportEventData(const std::string &reportNotice = "");

   DEFAULT_TO_NO_CLONES

protected:
   /// List of sensors
   StringArray sensorNames;
   /// Collection of sensors
   ObjectArray sensors;
   /// Name of central body
   std::string centralBodyName;
   /// The central body
   CelestialBody *centralBody;
   /// List of intuding bodies
   StringArray intrudingBodyNames;
   /// Collection of intruding bodies
   ObjectArray intrudingBodies;
   /// The minimum allowable phase for intrusion
   Real minimumPhase;
   /// The report coordinates
   std::string reportCoordinates;
   /// The SPICE interface
   SpiceInterface *spice;
   /// Vector holding all intrusion data collected
   std::vector<IntrusionResult*> intrusionResults;
   /// Name of the fixed grid frame file if fixed grid coordinates are in use
   std::string gridFrameFile;
   /// Filename of the instrument frame kernels to be created
   StringArray instFKFileNames;
   /// Filename of the spacecraft CK frame kernel to be created
   std::string scFKFileName;
   /// Filename of the instrument kernels to be created
   StringArray ikFileNames;

   void WriteFixedGridSPK(const std::string &sensorName, const std::string &frameName, 
      const Integer centerID, const Integer gridId, const RealArray gridPosition);


   /// Published parameters for contact locators
   enum
   {
      SENSORS = EventLocatorParamCount,
      CENTRAL_BODY,
      INTRUDING_BODIES,
      MINIMUM_PHASE,
      REPORT_COORDINATES,
      SPICE_GRID_FRAME_FILE,
      IntrusionLocatorParamCount
   };

   /// burn parameter labels
   static const std::string
      PARAMETER_TEXT[IntrusionLocatorParamCount - EventLocatorParamCount];
   /// burn parameter types
   static const Gmat::ParameterType
      PARAMETER_TYPE[IntrusionLocatorParamCount - EventLocatorParamCount];

   virtual void         FindEvents();
};

#endif /* IntrusionLocator_hpp */
