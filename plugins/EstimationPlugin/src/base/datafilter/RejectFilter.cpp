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
 * Implementation for the RejectFilter class
 */
//------------------------------------------------------------------------------


#include "RejectFilter.hpp"
#include "GmatBase.hpp"
#include "MessageInterface.hpp"
#include "MeasurementException.hpp"
#include "StringUtil.hpp"
#include <sstream>


//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION
//#define DEBUG_DATA_FILTER
//#define DEBUG_FILTER

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string RejectFilter::PARAMETER_TEXT[] =
{
   "RecordNumbers",
};

const Gmat::ParameterType RejectFilter::PARAMETER_TYPE[] =
{
   Gmat::STRINGARRAY_TYPE,			   // RECORD_NUMS
};



//------------------------------------------------------------------------------
// RejectFilter(const std::string &ofType, const std::string name)
//------------------------------------------------------------------------------
/**
 * Constructor for RejectFilter objects
 *
 * @param name The name of the object
 */
//------------------------------------------------------------------------------
RejectFilter::RejectFilter(const std::string &ofType, const std::string name) :
   DataFilter        (ofType, name),
   isRecNumbersSet   (false)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("RejectFilter default constructor <%s,%p>\n", GetName().c_str(), this);
#endif

   objectTypeNames.push_back("RejectFilter");
   parameterCount = RejectFilterParamCount;

   // Set initial value to variables used in DataFilters if those need to changes:
   // default value of observers is {Al}
   // default value of FileNames is {All}
   // default value of trackers is {All}
   // default value of dataTypes is {All}

   // Set initial value to variables in RejectFilter
   recNumbers.push_back("All");
   allRecNumbers = true;
}


//------------------------------------------------------------------------------
// ~RejectFilter()
//------------------------------------------------------------------------------
/**
 * RejectFilter destructor
 */
//------------------------------------------------------------------------------
RejectFilter::~RejectFilter()
{
}


//------------------------------------------------------------------------------
// RejectFilter(const RejectFilter& rf)
//------------------------------------------------------------------------------
/**
 * Copy constructor for a RejectFilter
 *
 * @param rf        The RejectFilter object that provides data for the new one
 */
//------------------------------------------------------------------------------
RejectFilter::RejectFilter(const RejectFilter& rf) :
   DataFilter            (rf),
   isRecNumbersSet       (rf.isRecNumbersSet),
   recNumbers            (rf.recNumbers),
   recNumRanges          (rf.recNumRanges),
   allRecNumbers         (rf.allRecNumbers)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("RejectFilter copy constructor from <%s,%p>  to  <%s,%p>\n", rf.GetName().c_str(), &rf, GetName().c_str(), this);
#endif

}


//------------------------------------------------------------------------------
// RejectFilter& operator=(const RejectFilter& rf)
//------------------------------------------------------------------------------
/**
 * RejectFilter assignment operator
 *
 * @param rf    The RejectFilter object that provides data for the this one
 *
 * @return This object, configured to match erf
 */
//------------------------------------------------------------------------------
RejectFilter& RejectFilter::operator=(const RejectFilter& rf)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("RejectFilter operator = <%s,%p>\n", GetName().c_str(), this);
#endif

   if (this != &rf)
   {
      DataFilter::operator=(rf);

      isRecNumbersSet = rf.isRecNumbersSet;
      recNumbers      = rf.recNumbers;
      recNumRanges    = rf.recNumRanges;
      allRecNumbers   = rf.allRecNumbers;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone method for RejectFilter
 *
 * @return A clone of this object.
 */
//------------------------------------------------------------------------------
GmatBase* RejectFilter::Clone() const
{
   return new RejectFilter(*this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Code fired in the Sandbox when the Sandbox initializes objects prior to a run
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RejectFilter::Initialize()
{
#ifdef DEBUG_INITIALIZATION
	MessageInterface::ShowMessage("RejectFilter<%s,%p>::Initialize()   entered\n", GetName().c_str(), this);
#endif

   bool retval = false;
   if (DataFilter::Initialize())
   {
      //@todo: Initialize code is here

      isInitialized = retval;
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("RejectFilter<%s,%p>::Initialize()   exit\n", GetName().c_str(), this);
#endif
   return retval;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the text string used to script a RejectFilter property
 *
 * @param id The ID of the property
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string RejectFilter::GetParameterText(const Integer id) const
{
   if (id >= DataFilterParamCount && id < RejectFilterParamCount)
      return PARAMETER_TEXT[id - DataFilterParamCount];

   return DataFilter::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID associated with a scripted property string
 *
 * @param str The scripted string used for the property
 *
 * @return The associated ID
 */
//------------------------------------------------------------------------------
Integer RejectFilter::GetParameterID(const std::string &str) const
{
   for (Integer i = DataFilterParamCount; i < RejectFilterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - DataFilterParamCount])
      {
         if (IsParameterReadOnly(i))
            throw MeasurementException("Error: " + str + "parameter was not defined in RejectFilter.\n");
         return i;
      }
   }

   return DataFilter::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameter type for a property
 *
 * @param id The ID of the property
 *
 * @return The ParameterType of the property
 */
//------------------------------------------------------------------------------
Gmat::ParameterType RejectFilter::GetParameterType(const Integer id) const
{
   if (id >= DataFilterParamCount && id < RejectFilterParamCount)
      return PARAMETER_TYPE[id - DataFilterParamCount];

   return DataFilter::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string describing the type of a property
 *
 * @param id The ID of the property
 *
 * @return The text description of the property type
 */
//------------------------------------------------------------------------------
std::string RejectFilter::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
* Sets a string property
*
* @param id The ID of the property
* @param value The new value
*
* @return true on success, false on failure
*/
//------------------------------------------------------------------------------
bool RejectFilter::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == RECORD_NUMS)
   {
      if (!isRecNumbersSet)
      {
         recNumbers.clear();
         recNumRanges.clear();
         allRecNumbers = false;
         isRecNumbersSet = true;
      }

      if (value == "")
         throw MeasurementException("Error: an empty string is set to " + GetName() + ".RecordNumbers.\n");
      else if (GmatStringUtil::RemoveSpaceInBrackets(value, "{}") == "{}")
      {
         allRecNumbers = false;
         recNumbers.clear();
         recNumRanges.clear();
      }
      else
      {
         Integer val1 = -1;
         Integer val2 = -1;

         if (value == "All")
            allRecNumbers = true;
         else
         {
            std::string::size_type pos = value.find_first_of('-');
            if (pos == value.npos)
            {
               Integer iVal;
               if (GmatStringUtil::ToInteger(value, iVal) == false)
                  throw MeasurementException("Error: A non integer is used in the ranges set to " + GetName() + ".RecordNumbers parameter.\n");
               val1 = val2 = iVal;
            }
            else if (pos != 0)
            {
               std::string s1 = value.substr(0, pos);
               std::string s2 = value.substr(pos + 1);
               if ((GmatStringUtil::ToInteger(s1, val1) == false) || (GmatStringUtil::ToInteger(s2, val2) == false))
                  throw MeasurementException("Error: A non integer is used in the range '" + value + "' set to " + GetName() + ".WorkingFileIds parameter.\n");

               if (val1 > val2)
                  throw MeasurementException("Error: The first number in the range '" + value + "' should be less than or equal the second number.\n");
            }
            else
               throw MeasurementException("Error: the range defined for " + GetName() + ".WorkingFileIds parameter has syntax error.\n");
         }

         recNumbers.push_back(value);
         recNumRanges.push_back(val1);
         recNumRanges.push_back(val2);
      }
      return true;
   }

   if (id == FILENAMES)
   {
      if (value == "From_AddTrackingConfig")
         throw MeasurementException("Error: 'From_AddTrackingConfig' is an invalid value for " + GetName() + ".FileNames parameter.\n");
   }

   return DataFilter::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
* Sets a string property
*
* @param label The text description of the property
* @param value The new value
*
* @return true on success, false on failure
*/
//------------------------------------------------------------------------------
bool RejectFilter::SetStringParameter(const std::string &label,
   const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
* Retrieves a string property
*
* @param id        parameter ID
* @param index     index of an element of string array
*
* @return The property value
*/
//------------------------------------------------------------------------------
std::string RejectFilter::GetStringParameter(const Integer id, const Integer index) const
{
   if (id == RECORD_NUMS)
   {
      if ((0 <= index) || (index < recNumbers.size()))
         return recNumbers[index];
      else
         throw MeasurementException("Error: index is out of bound.\n");
   }

   return DataFilter::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string property
 *
 * @param id         The ID of the property
 * @param value      The new value
 * @param index      Index of array element
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RejectFilter::SetStringParameter(const Integer id, const std::string &value, const Integer index)
{
   if (id == RECORD_NUMS)
   {
      // Remove default value
      if (!isRecNumbersSet)
      {
         recNumbers.clear();
         recNumRanges.clear();
         allRecNumbers = false;
         isRecNumbersSet = true;
      }

      // an empty list is set to Trackers parameter when index == -1
      if (index == -1)
      {
         allRecNumbers = false;
         recNumbers.clear();
         recNumRanges.clear();
         return true;
      }
      else if ((0 <= index) && (index <= (Integer)recNumbers.size()))
      {
         if (value == "")
            throw MeasurementException("Error: cannot accept an empty string to " + GetName() + ".RecordNumbers parameter.\n");
         else if (value == "All")
         {
            allRecNumbers = true;         // set flag to indicate choosing all trackers
            if (index == recNumbers.size())
            {
               recNumbers.push_back(value);
               recNumRanges.push_back(-1);
               recNumRanges.push_back(-1);
            }
            else
            {
               recNumbers[index] = value;
               recNumRanges[2 * index] = -1;
               recNumRanges[2 * index + 1] = -1;
            }
         }
         else
         {
            Integer val1 = -1;
            Integer val2 = -1;

            std::string::size_type pos = value.find_first_of('-');
            if (pos == value.npos)
            {
               Integer iVal;
               if (GmatStringUtil::ToInteger(value, iVal) == false)
                  throw MeasurementException("Error: A non integer is used in the ranges set to " + GetName() + ".RecordNumbers parameter.\n");
               val1 = val2 = iVal;
            }
            else if (pos != 0)
            {
               std::string s1 = value.substr(0, pos);
               std::string s2 = value.substr(pos + 1);
               if ((GmatStringUtil::ToInteger(s1, val1) == false) || (GmatStringUtil::ToInteger(s2, val2) == false))
                  throw MeasurementException("Error: A non integer is used in the range '" + value + "' set to " + GetName() + ".WorkingFileIds parameter.\n");

               if (val1 > val2)
                  throw MeasurementException("Error: The first number in the range '" + value + "' should be less than or equal the second number.\n");
            }
            else
               throw MeasurementException("Error: the range defined for " + GetName() + ".RecordNumbers parameter has syntax error.\n");

            if (index == recNumbers.size())
            {
               recNumbers.push_back(value);
               recNumRanges.push_back(val1);
               recNumRanges.push_back(val2);
            }
            else
            {
               recNumbers[index] = value;
               recNumRanges[2 * index] = val1;
               recNumRanges[2 * index + 1] = val2;
            }
         }
         return true;
      }
      else
      {
         std::stringstream ss;
         ss << "Error: recNumbers's index (" << index << ") is out of bound.\n";
         throw GmatBaseException(ss.str());
      }
   }

   if (id == FILENAMES)
   {
      if (value == "From_AddTrackingConfig")
         throw MeasurementException("Error: 'From_AddTrackingConfig' is an invalid value for " + GetName() + ".FileNames parameter.\n");
   }

   return DataFilter::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property
 *
 * @param label     The text description of the property
 * @param index     index of an element of string array 
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string RejectFilter::GetStringParameter(const std::string &label, const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string property
 *
 * @param label     The text description of the property
 * @param value     The new value
 * @param index     index of an element of string array 
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RejectFilter::SetStringParameter(const std::string &label,
      const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieves a string array property
*
* @param id The ID of the property
*
* @return  a string array
*/
//------------------------------------------------------------------------------
const StringArray& RejectFilter::GetStringArrayParameter(const Integer id) const
{
   if (id == RECORD_NUMS)
      return recNumbers;

   return DataFilter::GetStringArrayParameter(id);
}

const StringArray& RejectFilter::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetTrackingConfigs(StringArray tkconfigs)
//------------------------------------------------------------------------------
/**
* Set value to an integer parameter
*
* @param tkconfigs     A list of tracking configuration
*
* @return              true if the setting is successfull
*/
//------------------------------------------------------------------------------
bool RejectFilter::SetTrackingConfigs(StringArray tkconfigs)
{
   tkConfigList = tkconfigs;
   return true;
}


//------------------------------------------------------------------------------
// ObservationData* FilteringData(ObservationData* dataObject, Integer obDataId, 
//                                Integer& rejectedReason)
//------------------------------------------------------------------------------
/**
* This function is used to filter data
*
* @param dataObject        An input data record
* @param rejectedReason    Reason the record is rejected 
* @param obDataId          Id of the data record
*
* @return                  NULL if the input data record is rejected, 
*                          the input data record otherwise 
*/
//------------------------------------------------------------------------------
ObservationData* RejectFilter::FilteringData(ObservationData* dataObject, Integer& rejectedReason, Integer obDataId)
{
#ifdef DEBUG_FILTER
   MessageInterface::ShowMessage("RejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d, obDataId = %d)  enter\n", GetName().c_str(), this, dataObject, rejectedReason, obDataId);
#endif

   rejectedReason = 0;             // no reject

   if (obDataId == -1)             // It applies only for statistics reject filters
   {
      // 0. File name verify: It will be passed the test when observation data does not contain any file name in "FileNames" array
      if (!fileNames.empty())
      {
         if (!HasFile(dataObject))
         {
#ifdef DEBUG_FILTER
            MessageInterface::ShowMessage("RejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit11 return <%p>\n", GetName().c_str(), this, dataObject, rejectedReason, dataObject);
#endif
            return dataObject;             // return dataObject when name of the file specified in dataObject does not match to any file in FileNames list. The value of rejectedReason has to be 0 
         }
      }
   }

   // 1. Observated objects verify: It will be passed the test when observation data does not contain any spacecraft in "observers" array
   if (!observers.empty())
   {
      if (!HasObserver(dataObject))
      {
#ifdef DEBUG_FILTER
         MessageInterface::ShowMessage("RejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit1 return <%p>\n", GetName().c_str(), this, dataObject, rejectedReason, dataObject);
#endif
         return dataObject;             // return dataObject when it does not have any spacecraft matching to observers list. The value of rejectedReason has to be 0 
      }
   }

   // 2. Trackers verify: It will be passed the test when observation data contains one ground station in "trackers" array
   if (!trackers.empty())
   {
      if (!HasTracker(dataObject))
      {
#ifdef DEBUG_FILTER
         MessageInterface::ShowMessage("RejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit2 return <%p>\n", GetName().c_str(), this, dataObject, rejectedReason, dataObject);
#endif
         return dataObject;             // return dataObject when it does not have any spacecraft matching to observers list. The value of rejectedReason has to be 0 
      }
   }


   // 3. Measurement type verify: It will be passed the test when data type of observation data is found in "dataTypes" array
   if (!dataTypes.empty())
   {
      if (!HasDataType(dataObject))
      {
#ifdef DEBUG_FILTER
         MessageInterface::ShowMessage("RejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit3 return <%p>\n", GetName().c_str(), this, dataObject, rejectedReason, dataObject);
#endif
         return dataObject;             // return dataObject when it does not have any spacecraft matching to observers list. The value of rejectedReason has to be 0 
      }
   }

   // Strands verify:

   // 4. Time interval verify:
   if (!IsInTimeWindow(dataObject))
   {
#ifdef DEBUG_FILTER
      MessageInterface::ShowMessage("RejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit4 return <%p>\n", GetName().c_str(), this, dataObject, rejectedReason, dataObject);
#endif
      return dataObject;             // return dataObject when it does not have any spacecraft matching to observers list. The value of rejectedReason has to be 0 
   }

   
   if (obDataId != -1)
   {
      // 5. Record number verify:
      if (!IsInRecordNumberList(obDataId))
      {
#ifdef DEBUG_FILTER
         MessageInterface::ShowMessage("RejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d, obDataId = %d) exit5 return <%p>\n", GetName().c_str(), this, dataObject, rejectedReason, obDataId, dataObject);
#endif
         return dataObject;             // return dataObject when it's id doesn not march to any record number in RecordNumbers list. The value of rejectedReason has to be 0
      }
   }


   rejectedReason = 100;             // reject due to reject filter
   dataObject = NULL;

#ifdef DEBUG_FILTER
   MessageInterface::ShowMessage("RejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d, obDataId = %d)  enter\n", GetName().c_str(), this, dataObject, rejectedReason, obDataId);
#endif

   return dataObject;
}


bool RejectFilter::IsInRecordNumberList(Integer recNum)
{
   if (allRecNumbers)
      return true;

   for (Integer i = 0; i < recNumRanges.size(); i = i + 2)
   {
      if ((recNumRanges[i] <= recNum) && (recNum <= recNumRanges[i + 1]))
         return true;
   }

   return false;
}