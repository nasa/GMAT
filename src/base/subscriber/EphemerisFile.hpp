//$Id$
//------------------------------------------------------------------------------
//                                  EphemerisFile
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
// Author: Linda Jun / NASA
// Created: 2009/09/02
//
/**
 * Base class for writing spacecraft orbit states to an ephemeris file.
 */
//------------------------------------------------------------------------------
#ifndef EphemerisFile_hpp
#define EphemerisFile_hpp

#include "Subscriber.hpp"
#include "Spacecraft.hpp"
#include "EphemerisWriter.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include <iostream>
#include <fstream>

class GMAT_API EphemerisFile : public Subscriber
{
public:
   EphemerisFile(const std::string &name, const std::string &type = "EphemerisFile");
   virtual ~EphemerisFile();
   EphemerisFile(const EphemerisFile &);
   EphemerisFile& operator=(const EphemerisFile&);
   
   // Methods for this class
   // This method called from GUI
   std::string          GetProperFileName(const std::string &fName,
                                          const std::string &fType,
                                          bool saveFileName);
   
   virtual void         SetBackgroundGeneration(bool inBackground);
   
   // Need to be able to close background SPKs and leave ready for appending
   // Finalization
   virtual void         CloseEphemerisFile(bool done = true, bool writeMetaData = true);
   virtual bool         InsufficientDataPoints();
   
   // methods inherited from Subscriber
   virtual void         SetProvider(GmatBase *provider, Real epochInMjd = -999.999);
   
   // methods inherited from GmatBase
   virtual bool         Validate();
   virtual bool         Initialize();
   virtual GmatBase*    Clone(void) const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;
   virtual bool         IsParameterCloaked(const Integer id) const;
   
   virtual UnsignedInt
                        GetPropertyObjectType(const Integer id) const;
   virtual const StringArray&
                        GetPropertyEnumStrings(const Integer id) const;
   
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);

   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);

   void         StartNewSegment(const std::string &comments,
      bool saveEpochInfo,
      bool writeAfterData,
      bool ignoreBlankComments);
      
protected:
   
   enum FileType
   {
      CCSDS_OEM, CCSDS_AEM, SPK_ORBIT, SPK_ATTITUDE, CK_ATTITUDE, CODE500_EPHEM,
      STK_TIMEPOSVEL, UNKNOWN_FILE_TYPE
   };
   
   Spacecraft        *spacecraft;
   CoordinateSystem  *outCoordSystem;
   EphemerisWriter   *ephemWriter;
   
   /// ephemeris full file name including the path
   std::string fullPathFileName;
   std::string spacecraftName;
   std::string spacecraftId;
   std::string prevFileName;
   std::string fileName;
   std::string fileFormat;
   
   std::string fileFormatVersion;
   bool        addingAccelerationOption;
   bool        addingCovarianceOption;

   std::string epochFormat;
   std::string initialEpochStr;
   std::string finalEpochStr;
   std::string stepSize;
   std::string interpolatorName;
   std::string stateType;
   std::string outCoordSystemName;
   std::string outputFormat;
   std::string covFormat;
   bool        writeEphemeris;
   bool        usingDefaultFileName;
   bool        generateInBackground;
   bool        allowMultipleSegments;
   bool        includeEventBoundaries;
   /// for propagator change
   std::string prevPropName;
   std::string currPropName;
   
   Integer     interpolationOrder;
   Integer     toggleStatus;
   Integer     propIndicator;
   Real        prevPropDirection;
   Real        currPropDirection;
   Real        initialEpochA1Mjd;
   Real        finalEpochA1Mjd;
   Real        blockBeginA1Mjd;
   Real        currEpochInDays;
   Real        currEpochInSecs;
   Real        prevEpochInSecs;
   Real        maneuverEpochInDays;
   Real        eventEpochInSecs;
   Real        currState[6];
   Real        currCov[21];

   Real        currAccel[3];
   Real        currQuat[4];
   
   Real        stepSizeInSecs;
   
   bool        firstTimeWriting;
   bool        useFixedStepSize;
   bool        checkForLargeTimeGap;
   bool        isEphemFileOpened;
   bool        canFinalize;
   
   FileType    fileType;
   
   std::string distanceUnit;

   /// for maneuver handling
   ObjectArray maneuversHandled;
   
   /// output data stream
   std::ofstream      dstream;
   
   /// Available file format list
   static StringArray fileFormatList;   
   /// Available epoch format list
   static StringArray epochFormatList;   
   /// Available initial epoch list
   static StringArray initialEpochList;
   /// Available final epoch list
   static StringArray finalEpochList;   
   /// Available step size list
   static StringArray stepSizeList;   
   /// Available state type list
   static StringArray stateTypeList;   
   /// Available write ephemeris list
   static StringArray writeEphemerisList;
   /// Available interpolator type list
   static StringArray interpolatorTypeList;
   /// Available output format list
   static StringArray outputFormatList;
   /// Available covariance type list
   static StringArray covFormatList;
   /// Available distance unit list
   static StringArray distanceUnitList;
   /// Available include event boundaries list
   static StringArray eventBoundariesList;
   
   // Initialization
   void         ValidateParameters(bool forInitialization);
   Real         ConvertInitialAndFinalEpoch();
   void         InitializeData(bool saveEpochInfo = false);
   void         SetFileName();
   void         CreateEphemerisFile();
   void         CreateEphemerisWriter();
   
   // Time and data
   bool         HandleEndOfRun();
   bool         SkipFunctionData();
   bool         RetrieveData(const Real *dat, Integer len);
   void         HandlePropDirectionChange();
   
   bool         IsBackwardPropAllowed();
   bool         ProcessOrbitData();
   
   void         HandleOrbitData();
   void         FinishUpWriting();
   bool         SetEpoch(Integer id, const std::string &value,
                         const StringArray &allowedValues);
   bool         SetStepSize(Integer id, const std::string &value,
                            const StringArray &allowedValues);
   void         HandleError(Integer id, const std::string &value,
                            const StringArray &allowedValues,
                            const std::string &additionalMsg = "");
   std::string  ToString(const StringArray &strList);
   std::string  GetBackwardPropWarning();
   
   // Event checking
   bool         IsEventFeasible(bool checkForNoData = true);
   void         SetEventEpoch();
   
   // Time formatting
   std::string  ToUtcGregorian(Real epoch, bool inDays = false, Integer format = 2);
   
   // Debug output
   void         DebugWriteTime(const std::string &msg, Real epoch, bool inDays = false,
                               Integer format = 2);
   void         DebugWriteOrbit(const std::string &msg, Real epoch, const Real state[6],
                                bool inDays = false);
   void         DebugWriteOrbit(const std::string &msg, A1Mjd *epochInDays,
                                Rvector6 *state);
   // Deprecated field
   void         WriteDeprecatedMessage(Integer id) const;
   
   // methods inherited from Subscriber
   virtual bool Distribute(Integer len);
   virtual bool Distribute(const Real *dat, Integer len);
   void         HandleManeuvering(GmatBase *originator, bool maneuvering, Real epoch,
                                  const StringArray &satNames,
                                  const std::string &desc);
   void         HandlePropagatorChange(GmatBase *provider, Real epochInMjd);
   void         HandleSpacecraftPropertyChange(GmatBase *originator, Real epoch,
                                               const std::string &satName,
                                               const std::string &desc);
   enum
   {
      SPACECRAFT = SubscriberParamCount,
      FILENAME,
      FULLPATH_FILENAME,
      FILE_FORMAT,
      
      FILE_FORMAT_VERSION,
      ADDING_ACCELERATION_OPTION,
      ADDING_COVARIANCE_OPTION,

      EPOCH_FORMAT,
      INITIAL_EPOCH,
      FINAL_EPOCH,
      STEP_SIZE,
      INTERPOLATOR,
      INTERPOLATION_ORDER,
      STATE_TYPE,
      COORDINATE_SYSTEM,
      OUTPUT_FORMAT,
      INCLUDE_COVARIANCE,
      WRITE_EPHEMERIS,
      FILE_NAME,                // deprecated
      DISTANCE_UNIT,            // Meters or kilometers
      INCLUDE_EVENT_BOUNDARIES,
      EphemerisFileParamCount   // Count of the parameters for this class
   };
   
   static const std::string
      PARAMETER_TEXT[EphemerisFileParamCount - SubscriberParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[EphemerisFileParamCount - SubscriberParamCount];
   
};

#endif // EphemerisFile_hpp
