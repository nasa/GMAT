//------------------------------------------------------------------------------
//                                 RejectFilter
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
// Created: 2017/01/03
//
/**
 * Definition for the RejectFilter class used to define a data 
 * filter for an Estimator
 */
//------------------------------------------------------------------------------

#ifndef RejectFilter_hpp
#define RejectFilter_hpp

#include "estimation_defs.hpp"
#include "GmatBase.hpp"
#include "DataFilter.hpp"

class ESTIMATION_API RejectFilter : public DataFilter
{
public:
   RejectFilter(const std::string &ofType, std::string name);
   virtual ~RejectFilter();
   RejectFilter(const RejectFilter& rf);
   RejectFilter& operator=(const RejectFilter& rf);

   virtual GmatBase* Clone() const;
   virtual bool Initialize();
   
   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         SetStringParameter(const Integer id, const std::string &value);
   virtual bool         SetStringParameter(const std::string &label, const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id, const std::string &value, const Integer index);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label, const std::string &value, const Integer index);

   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;


   /// This function is used to set tracking configs to data file object in order to use for data filter
   virtual bool         SetTrackingConfigs(StringArray tkconfigs);

   virtual ObservationData* 
                        FilteringData(ObservationData* dataObject, Integer& rejectedReason, Integer obDataId = -1);

   /// @todo: Check this
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   // A list of record numbers
   StringArray  recNumbers;
   IntegerArray recNumRanges;
   bool         allRecNumbers;
   bool         isRecNumbersSet;

   /// List of tracking configs used for data filter
   StringArray tkConfigList;

   /// Class parameter ID enumeration
   enum
   {
      RECORD_NUMS = DataFilterParamCount,
      RejectFilterParamCount
   };

   // Start with the parameter IDs and associates strings
   /// Strings associated with the RejectFilter parameters
   static const std::string
                PARAMETER_TEXT[RejectFilterParamCount - DataFilterParamCount];
   /// Types of the RejectFilter parameters
   static const Gmat::ParameterType
                PARAMETER_TYPE[RejectFilterParamCount - DataFilterParamCount];

private:
   bool         IsInRecordNumberList(Integer recNum);

};

#endif /* RejectFilter_hpp */
