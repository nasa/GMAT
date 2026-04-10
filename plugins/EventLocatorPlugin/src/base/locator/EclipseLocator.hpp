//$Id: EclipseLocator.hpp 2264 2012-04-05 22:12:37Z  $
//------------------------------------------------------------------------------
//                           EclipseLocator
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Sep 2, 2011
// Updated: 2015  Wendy Shoan / GSFC
//
/**
 * Definition of the EclipseLocator used for eclipses
 * Updates based on prototype by Yeerang Lim/KAIST
 */
//------------------------------------------------------------------------------


#ifndef EclipseLocator_hpp
#define EclipseLocator_hpp

#include "EventLocator.hpp"
#include "EventLocatorDefs.hpp"
#include "Star.hpp"
#include "EclipseTotalEvent.hpp"

/**
 * The EventLocator used for shadow entry and exit location
 */
class LOCATOR_API EclipseLocator : public EventLocator
{
public:
   EclipseLocator(const std::string &name);
   virtual ~EclipseLocator();
   EclipseLocator(const EclipseLocator& el);
   EclipseLocator& operator=(const EclipseLocator& el);

   // Inherited (GmatBase) methods for parameters
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual const StringArray&
                        GetPropertyEnumStrings(const Integer id) const;

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
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

   const ObjectTypeArray& GetTypesForList(const Integer id);
   const ObjectTypeArray& GetTypesForList(const std::string &label);

   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   virtual bool         Initialize();
   virtual bool         ReportEventData(const std::string &reportNotice = "");

   DEFAULT_TO_NO_CLONES

protected:
   // List of requested eclipse types
   StringArray eclipseTypes;
   /// The Sun
   Star        *sun;
   /// The stored events
   std::vector<EclipseTotalEvent*> theEvents;
   /// the maximum index of the stored events
   Integer     maxIndex;
   /// The maximum duration of the found events
   Real        maxDuration;
   /// the default types of eclipse
   StringArray defaultEclipseTypes;


   /// Published parameters for eclipse locators
   enum
   {
    ECLIPSE_TYPES = EventLocatorParamCount,
    EclipseLocatorParamCount
   };

   /// burn parameter labels
   static const std::string
    PARAMETER_TEXT[EclipseLocatorParamCount - EventLocatorParamCount];
   /// burn parameter types
   static const Gmat::ParameterType
    PARAMETER_TYPE[EclipseLocatorParamCount - EventLocatorParamCount];

   virtual void FindEvents();
   virtual std::vector<EclipseTotalEvent*> GetNestedEvents(Integer idx,
                                           EclipseTotalEvent *inEvent);

};

#endif /* EclipseLocator_hpp */
