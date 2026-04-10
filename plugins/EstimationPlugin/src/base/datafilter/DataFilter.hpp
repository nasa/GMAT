//------------------------------------------------------------------------------
//                                 DataFilter
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen, NASA/GSFC.
// Created: 2015/03/03
//
/**
 * Definition for the SatisticAcceptFilter class used to define a data accepting 
 * filter for a TrackingFile
 */
//------------------------------------------------------------------------------


#ifndef DataFilter_hpp
#define DataFilter_hpp

#include "estimation_defs.hpp"
#include "GmatBase.hpp"
#include "ObservationData.hpp"

//class GMAT_API DataFilter : public GmatBase
class ESTIMATION_API DataFilter : public GmatBase
{
public:
   DataFilter(const std::string &ofType, std::string name);
   virtual ~DataFilter();
   DataFilter(const DataFilter& df);
   DataFilter& operator=(const DataFilter& df);

   virtual GmatBase* Clone() const;
   virtual bool Initialize();
   virtual bool CleanUp();

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;

   // Use to get initial epoch and fimal epoch
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         GetRealParameter(const std::string &label) const;


   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string & oldName, const std::string & newName);
   virtual const ObjectTypeArray& 
                        GetRefObjectTypeArray();
   virtual const StringArray& 
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string & name);
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string & name, const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string & name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string & name, const Integer index);
   virtual ObjectArray& GetRefObjectArray(const std::string & typeString);
   virtual ObjectArray& GetRefObjectArray(const UnsignedInt type);



   /// Filter observed data
   virtual ObservationData* 
                        FilteringData(ObservationData* dataObject, Integer& rejectedReason, Integer obDataId = -1);
//   virtual bool         ValidateInput();

   /// @todo: Check this
   DEFAULT_TO_NO_CLONES
   //DEFAULT_TO_NO_REFOBJECTS

protected:
   /// A list of file names specifies data files on which the filter to be applied 
   StringArray fileNames;
   bool isDataFileDefaultVal;

   /// A name list of observed objects
   StringArray observers;
   /// A list of observed objects
   ObjectArray observerObjects;
   bool isObserverDefaultVal;

   /// A name list of trackers
   StringArray trackers;
   /// A list of trackers
   ObjectArray trackerObjects;
   bool isTrackerDefaultVal;

   /// A list of all data types
   StringArray dataTypes;
   bool isDataTypeDefaultVal;

   /// Format of an epoch
   std::string epochFormat;
   /// Initial epoch and Final epoch
   std::string initialEpoch;
   std::string finalEpoch;
   GmatEpoch   epochStart;
   GmatEpoch   epochEnd;
   /// signal path
   StringArray strands;

   /// Flag indicate that ValidateInput() function was run
   bool isChecked;

   //std::map<std::string, std::string> dataTypesMap;

   /// Class parameter ID enumeration
   enum
   {
      FILENAMES = GmatBaseParamCount,
      OBSERVED_OBJECTS,
      TRACKERS,
      DATA_TYPES,
      EPOCH_FORMAT,
      INITIAL_EPOCH,
      FINAL_EPOCH,
      STRANDS,
      DataFilterParamCount
   };

   // Start with the parameter IDs and associates strings
   /// Strings associated with the DataFilter parameters
   static const std::string
                PARAMETER_TEXT[DataFilterParamCount - GmatBaseParamCount];
   /// Types of the DataFilter parameters
   static const Gmat::ParameterType
                PARAMETER_TYPE[DataFilterParamCount - GmatBaseParamCount];


   /// Get a list of all available measurement types 
   StringArray  GetListOfMeasurementTypes();
   /// Get a list of all spacecrafts 
   ObjectArray  GetListOfSpacecrafts();
   /// Get a list of all ground stations 
   ObjectArray  GetListOfGroundStations();
   /// Get a list of all ground stations
   ObjectArray  GetListOfFiles();
   ///// Get a list of valid epoch formats
   //StringArray  GetListOfValidEpochFormats();

   /// Check observation data containing a file name listing in filenames
   bool     HasFile(ObservationData* dataObject);
   /// Check observation data containing a spacecraft listing in observers
   bool     HasObserver(ObservationData* dataObject);
   /// Check observation data containing a ground station listing in trackers
   bool     HasTracker(ObservationData* dataObject);
   /// Check observation data containing a data type listing in trackers
   bool     HasDataType(ObservationData* dataObject);
   /// Check observation data containing measurement epoch in time window
   bool     IsInTimeWindow(ObservationData* dataObject);

private:
   Real    ConvertToRealEpoch(const std::string &theEpoch,
                                   const std::string &theFormat);
   bool    isEpochFormatSet;

//   std::map<std::string, std::string> GetDeprecatedTypeMap() {return depTypeMap;};
//   std::map<std::string, std::string> depTypeMap;
};

#endif /* DataFilter_hpp */
