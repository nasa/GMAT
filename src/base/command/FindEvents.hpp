//$Id$
//------------------------------------------------------------------------------
//                            FindEvents
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
// Author: Wendy Shoan / GSFC/GSSB/583
// Created: 2015.03.11
//
/**
 *  Definition code for the FindEvents command.
 */
//------------------------------------------------------------------------------

#ifndef FindEvents_hpp
#define FindEvents_hpp


#include "GmatCommand.hpp"
#include "EventLocator.hpp"
#include "Spacecraft.hpp"


/**
 * FindEvents Class, used to turn on thrusters used in a finite burn.
 */
class GMAT_API FindEvents : public GmatCommand
{
public:
   FindEvents();
   virtual ~FindEvents();
   FindEvents(const FindEvents& copy);
   FindEvents&          operator=(const FindEvents& copy);

   virtual bool         TakeAction(const std::string &action,
                           const std::string &actionData = "");


   // Parameter accessors
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);

   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         SetRefObjectName(const UnsignedInt type,
                                         const std::string &name);

   virtual GmatBase*    Clone() const;
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual const std::string&
                        GetGeneratingString(
                                        Gmat::WriteMode mode = Gmat::SCRIPTING,
                                        const std::string &prefix = "",
                                        const std::string &useName = "");

   virtual bool         Initialize();
   virtual bool         Execute();

   // Methods used for interpreting the command
   virtual bool         InterpretAction();

   DEFAULT_TO_NO_CLONES

protected:

   // Parameter IDs
   enum
   {
      EVENT_LOCATOR = GmatCommandParamCount,
      APPEND_FLAG,
      FindEventsParamCount
   };

   static const std::string    PARAMETER_TEXT[FindEventsParamCount -
                                              GmatCommandParamCount];
   static const Gmat::ParameterType
                               PARAMETER_TYPE[FindEventsParamCount -
                                              GmatCommandParamCount];

   /// Name of the EventLocator object used
   std::string                   eventLocatorName;
   /// The FiniteBurn object
   EventLocator                  *eventLocator;
   /// The flag indicating whether or not to append to the EventLocator's
   /// report file
   bool                          appendFlag;

   /// Parsing function for Append
   void                CheckForOptions(std::string &opts);
};

#endif // FindEvents_hpp
