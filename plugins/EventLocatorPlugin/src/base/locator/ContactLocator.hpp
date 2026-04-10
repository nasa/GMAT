//$Id: ContactLocator.hpp 2264 2012-04-05 22:12:37Z  $
//------------------------------------------------------------------------------
//                           ContactLocator
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
// Created: Nov 7, 2011
// Updated: 2015  Wendy Shoan / GSFC and Yeerang Lim/KAIST
//
/**
 * Definition of the ContactLocator
 * Updates based on prototype by Joel Parker / GSFC
 */
//------------------------------------------------------------------------------


#ifndef ContactLocator_hpp
#define ContactLocator_hpp


#include "EventLocator.hpp"
#include "EventLocatorDefs.hpp"
#include "ContactResult.hpp"
#include "Imager.hpp"


/**
 * Event locator used for station contact events
 *
 * This class manages station contacts for a set of target SpaceObjects
 * (typically Spacecraft).  Contact events require that two types of event
 * function be satisfied: an elevation function, placing the target above the
 * station's horizon, and (zero or more) line-of-sight event functions, ensuring
 * that there is no obstructing object between the station and the target.
 *
 * @note: Work on this event type is temporarily on hold.  The current state is
 * a set of event functions coded but untested, and a containing ContactLocator
 * which has untested initialization but is missing the reference object
 * methods.
 */
class LOCATOR_API ContactLocator : public EventLocator
{
public:
   ContactLocator(const std::string &name);
   virtual ~ContactLocator();
   ContactLocator(const ContactLocator& el);
   ContactLocator& operator=(const ContactLocator& cl);

   // Inherited (GmatBase) methods for parameters
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   virtual Real      GetRealParameter(const Integer id) const;
   virtual Real      SetRealParameter(const Integer id,
      const Integer value);
   virtual Real      SetRealParameter(const Integer id,
      const Real value);
   virtual Real      GetRealParameter(const std::string &label) const;
   virtual Real      SetRealParameter(const std::string &label,
      const Integer value);
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
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
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
                                     const std::string &name,
                                     const Integer index);
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
   void                 FormatReportData(Integer &maxColumnWidth, Integer ii, Integer jj, Integer kk, 
                                         Integer iii, std::string s);
   virtual bool         ReportEventData(const std::string &reportNotice = "");
   bool                 ReportEventDataLegacy(const std::string &reportNotice = "");


   DEFAULT_TO_NO_CLONES

protected:
   /// List of stations
   StringArray observerNames;
   /// Collection of observers
   ObjectArray observers;
   

   /// List of direct observers (imagers with FOV and ground stations without FOV imagers)
   StringArray directObserversNames;
   /// Name of ground station or spacecraft the imager is attached to (is groundstation's name if not an imager)
   IntegerArray directObserversHostIndex;
   /// Collection of direct observers (imagers with FOV and ground stations without FOV imagers)
   ObjectArray directObservers;
   /// Collection of Planetographic Regions to observe
   ObjectArray observedRegions;

   /// Light time Direction
   std::string lightTimeDirection;
   /// Output mode for 
   bool leftJustified = false;
   /// Precision for report values except time
   Integer reportPrecision = 6;
   /// List of columns for report, in order
   StringArray reportColumnsInOrder;
   /// Changes report to an Azimuth, Elevation and Range report
   bool reportIntervals = false;
   /// The interval for AER reports to report data for each pass
   Real intervalStep = 0; //Defaults to 0 seconds to not report info
   /// The report Time format
   std::string reportTimeFormat = "UTCGregorian";
   /// The report format template
   std::string reportTemplateFormat = "Legacy";
   // The stored results
   std::vector<ContactResult*> contactResults;


   ////From Intrusion Locator

   /// The report coordinates
   std::string reportCoordinates;
   /// The SPICE interface
   SpiceInterface *spice;
   /// Vector holding all intrusion data collected
   StringArray spiceInstNames;
   StringArray fkFileName;
   StringArray ikFileName;


   bool SetUpImagerSPICEFKandIK(Imager * imagerInUsee, Integer indxNum);

   /// Published parameters for contact locators
   enum
   {
      STATIONS = EventLocatorParamCount,
      LIGHT_TIME_DIRECTION,
      LEFT_JUSTIFIED,
      REPORT_PRECISION,
      REPORT_TEMPLATE_FORMAT,
      INTERVAL_STEP,
      REPORT_TIME_FORMAT,
      ContactLocatorParamCount
    };

    /// burn parameter labels
    static const std::string
       PARAMETER_TEXT[ContactLocatorParamCount - EventLocatorParamCount];
    /// burn parameter types
    static const Gmat::ParameterType
       PARAMETER_TYPE[ContactLocatorParamCount - EventLocatorParamCount];

    static const std::string LT_DIRECTIONS[2];

    virtual void         FindEvents();

    Real         InterpolateRegionCrossing(Real low, Real high, bool isWithin, Real tolerance, Rvector3 & latLongHeight);

    bool         IsSatWithinRegion(Real epoch, Rvector3 & latLongHeight, Real & distance);

    virtual std::string  GetAbcorrString();

    static std::string GetSpiceFrame(const std::string & bodyName);
private:
   Real ShyEndEphemeris();

   // Spice data
   SpiceInt targetSat;
   ConstSpiceChar * obsFrameConstSpiceChar;
   ConstSpiceChar * theAbCorrConstSpiceChar;
   SpiceInt obs;
};

#endif /* ContactLocator_hpp */
