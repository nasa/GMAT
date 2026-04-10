//------------------------------------------------------------------------------
//                                 AcceptFilter
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
 * Implementation for the AcceptFilter class
 */
//------------------------------------------------------------------------------


#include "AcceptFilter.hpp"
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

const std::string AcceptFilter::PARAMETER_TEXT[] =
{
   "ThinMode",
   "ThinningFrequency",
   "RecordNumbers",
};

const Gmat::ParameterType AcceptFilter::PARAMETER_TYPE[] =
{
   Gmat::STRING_TYPE,			      // THIN_MODE
   Gmat::INTEGER_TYPE,              // THINNING_FREQUENCY
   Gmat::STRINGARRAY_TYPE,			   // WORKING_FILE_IDS
};


//------------------------------------------------------------------------------
// AcceptFilter(const std::string &ofType, const std::string name)
//------------------------------------------------------------------------------
/**
 * Constructor for AcceptFilter objects
 *
 * @param name The name of the object
 */
//------------------------------------------------------------------------------
AcceptFilter::AcceptFilter(const std::string &ofType, const std::string name) :
   DataFilter              (ofType, name),
   thinMode                ("Frequency"),
   thinningFrequency       (1),
   isRecNumbersSet         (false)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("AcceptFilter default constructor <%s,%p>\n", GetName().c_str(), this);
#endif

   objectTypeNames.push_back("AcceptFilter");
   parameterCount = AcceptFilterParamCount;

   recNumbers.push_back("All");
   allRecNumbers = true;
}


//------------------------------------------------------------------------------
// ~AcceptFilter()
//------------------------------------------------------------------------------
/**
 * AcceptFilter destructor
 */
//------------------------------------------------------------------------------
AcceptFilter::~AcceptFilter()
{
}


//------------------------------------------------------------------------------
// AcceptFilter(const AcceptFilter& af)
//------------------------------------------------------------------------------
/**
 * Copy constructor for a AcceptFilter
 *
 * @param eaf        The AcceptFilter object that provides data for the new one
 */
//------------------------------------------------------------------------------
AcceptFilter::AcceptFilter(const AcceptFilter& af) :
   DataFilter            (af),
   thinMode              (af.thinMode),
   thinningFrequency     (af.thinningFrequency),
   isRecNumbersSet       (af.isRecNumbersSet),
   recNumbers            (af.recNumbers),
   recNumRanges          (af.recNumRanges),
   allRecNumbers         (af.allRecNumbers)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("AcceptFilter copy constructor from <%s,%p>  to  <%s,%p>\n", af.GetName().c_str(), &af, GetName().c_str(), this);
#endif

}


//------------------------------------------------------------------------------
// AcceptFilter& operator=(const AcceptFilter& af)
//------------------------------------------------------------------------------
/**
 * AcceptFilter assignment operator
 *
 * @param af    The AcceptFilter object that provides data for the this one
 *
 * @return This object, configured to match af
 */
//------------------------------------------------------------------------------
AcceptFilter& AcceptFilter::operator=(const AcceptFilter& af)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("AcceptFilter operator = <%s,%p>\n", GetName().c_str(), this);
#endif

   if (this != &af)
   {
      DataFilter::operator=(af);

      thinMode          = af.thinMode;
      thinningFrequency = af.thinningFrequency;

      isRecNumbersSet   = af.isRecNumbersSet;
      recNumbers        = af.recNumbers;
      recNumRanges      = af.recNumRanges;
      allRecNumbers     = af.allRecNumbers;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone method for AcceptFilter
 *
 * @return A clone of this object.
 */
//------------------------------------------------------------------------------
GmatBase* AcceptFilter::Clone() const
{
   return new AcceptFilter(*this);
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
bool AcceptFilter::Initialize()
{
#ifdef DEBUG_INITIALIZATION
	MessageInterface::ShowMessage("AcceptFilter<%s,%p>::Initialize()   entered\n", GetName().c_str(), this);
#endif

   bool retval = false;
   if (DataFilter::Initialize())
   {
      //@todo: Initialize code is here

      isInitialized = retval;
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("AcceptFilter<%s,%p>::Initialize()   exit\n", GetName().c_str(), this);
#endif
   return retval;
}


//------------------------------------------------------------------------------
// bool CleanUp()
//------------------------------------------------------------------------------
/**
 * Code fired in the Sandbox when the Sandbox initializes objects prior to a run
 *
 * @return true on success, false on failure
 */
 //------------------------------------------------------------------------------
bool AcceptFilter::CleanUp()
{
   bool retval = true;

   DataFilter::Initialize();

   recCountMap.clear();
   startTimeWindowMap.clear();

   return retval;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the text string used to script a AcceptFilter property
 *
 * @param id The ID of the property
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string AcceptFilter::GetParameterText(const Integer id) const
{
   if (id >= DataFilterParamCount && id < AcceptFilterParamCount)
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
Integer AcceptFilter::GetParameterID(const std::string &str) const
{
   for (Integer i = DataFilterParamCount; i < AcceptFilterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - DataFilterParamCount])
      {
         if (IsParameterReadOnly(i))
            throw MeasurementException("Error: " + str + "parameter was not defined in AcceptFilter.\n");
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
Gmat::ParameterType AcceptFilter::GetParameterType(const Integer id) const
{
   if (id >= DataFilterParamCount && id < AcceptFilterParamCount)
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
std::string AcceptFilter::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieves a string property
*
* @param id The ID of the property
*
* @return The property value
*/
//------------------------------------------------------------------------------
std::string AcceptFilter::GetStringParameter(const Integer id) const
{
   if (id == THIN_MODE)
      return thinMode;

   return DataFilter::GetStringParameter(id);
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
bool AcceptFilter::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == RECORD_NUMS)
   {
      if (!isRecNumbersSet)
      {
         recNumbers.clear();
         recNumRanges.clear();
         allRecNumbers   = false;
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

   if (id == THIN_MODE)
   {
      StringArray nameList = GetAllAvailableThinModes();
      if (find(nameList.begin(), nameList.end(), value) != nameList.end())
      {
         thinMode = value;
         return true;
      }
      else
         throw MeasurementException("Error: Value '" + value + "' set to " + GetName() + ".ThinMode is invalid.\n");
   }

   if (id == FILENAMES)
   {
      if (value == "All")
      {
         if (find(observers.begin(), observers.end(), "From_AddTrackingConfig") != observers.end())
            throw MeasurementException("Error: Both 'All' and 'From_AddTrackingConfig' can not set to " + GetName() + ".FileNames simultanously.\n");
      }
      else if (value == "From_AddTrackingConfig")
      {
         if (find(observers.begin(), observers.end(), "All") != observers.end())
            throw MeasurementException("Error: Both 'All' and 'From_AddTrackingConfig' can not set to " + GetName() + ".FileNames simultanously.\n");
      }
   }

   return DataFilter::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Retrieves a string property of a ErrorModel
*
* @param label The text description of the property
*
* @return The property value
*/
//------------------------------------------------------------------------------
std::string AcceptFilter::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
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
bool AcceptFilter::SetStringParameter(const std::string &label,
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
std::string AcceptFilter::GetStringParameter(const Integer id, const Integer index) const
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
bool AcceptFilter::SetStringParameter(const Integer id, const std::string &value, const Integer index)
{
   if (id == RECORD_NUMS)
   {
      // Remove default value
      if (!isRecNumbersSet)
      {
         recNumbers.clear();
         recNumRanges.clear();
         allRecNumbers   = false;
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
      else if ((0 <= index)&&(index <= (Integer)recNumbers.size()))
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
               recNumRanges[2 * index+1] = -1;
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
std::string AcceptFilter::GetStringParameter(const std::string &label, const Integer index) const
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
bool AcceptFilter::SetStringParameter(const std::string &label,
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
const StringArray& AcceptFilter::GetStringArrayParameter(const Integer id) const
{
   if (id == RECORD_NUMS)
      return recNumbers;

   return DataFilter::GetStringArrayParameter(id);
}

const StringArray& AcceptFilter::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieves an integer parameter
*
* @param id   Parameter ID
*
* @return     The parameter value
*/
//------------------------------------------------------------------------------
Integer AcceptFilter::GetIntegerParameter(const Integer id) const
{
   if (id == THINNING_FREQUENCY)
      return thinningFrequency;

   return DataFilter::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
* Set value to an integer parameter
*
* @param id     Parameter ID
* @param value  Value which is set to the parameter
*
* @return     The parameter value
*/
//------------------------------------------------------------------------------
Integer AcceptFilter::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == THINNING_FREQUENCY)
   {
      if (value > 0)
      {
         thinningFrequency = value;
         return thinningFrequency;
      }
      else
      {
         std::stringstream ss;
         ss << "Error: An invalid value (" << value << ") is set to " << GetName() << ".ThinningFrequency parameter.\n";
         throw MeasurementException(ss.str());
      }
   }

   return DataFilter::SetIntegerParameter(id, value);
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Retrieves an integer parameter
*
* @param label   Name of the parameter
*
* @return        The parameter value
*/
//------------------------------------------------------------------------------
Integer AcceptFilter::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
* Set value to an integer parameter
*
* @param label     Parameter's name
* @param value     Value which is set to the parameter
*
* @return          The parameter value
*/
//------------------------------------------------------------------------------
Integer AcceptFilter::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
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
bool AcceptFilter::SetTrackingConfigs(StringArray tkconfigs)
{
   tkConfigList = tkconfigs;
   return true;
}


//------------------------------------------------------------------------------
// ObservationData* FilteringData(ObservationData* dataObject, Integer obDataId, Integer& rejectedReason)
//------------------------------------------------------------------------------
/**
* This function is used to filter data
*
* @param dataObject        An input data record
* @param rejectedReason    Reason the record is rejected 
* @param obDataId          Id of the data record in working file
*
* @return                  NULL if the input data record is rejected, 
*                          the input data record otherwise 
*/
//------------------------------------------------------------------------------
ObservationData* AcceptFilter::FilteringData(ObservationData* dataObject, Integer& rejectedReason, Integer obDataId)
{
#ifdef DEBUG_DATA_FILTER
   MessageInterface::ShowMessage("AcceptFilter::FilteringData(dataObject = <%p>, rejectedReason = %d, obDataId = %d) enter\n", dataObject, rejectedReason, obDataId);
#endif
   if (obDataId == -1)            // It only applys for statistics accepted filters 
   {
      // 0. FileNames verify: data is rejected when file name list is empty
      bool pass0 = true;
      rejectedReason = 0;   // 0: no rejected due to accept all spacecrafts
      if (fileNames.empty())
      {
         pass0 = false;
         rejectedReason = 8;   // 8: rejected due to file name list is empty
      }

      if (!pass0)
      {
#ifdef DEBUG_FILTER
         MessageInterface::ShowMessage("AcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit6 return NULL\n", GetName().c_str(), this, dataObject, rejectedReason);
#endif
         return NULL;             // return NULL when it does not pass the test. The value of rejectedReason has to be 8 
      }

      // Filter data using tracking configs
      bool pass5 = true;
      rejectedReason = 0;
      if (find(fileNames.begin(), fileNames.end(), "From_AddTrackingConfig") != fileNames.end())
      {
         // Filter data based on tracking configs when tracking configs are not generated automatically
         if (tkConfigList.size() != 0)
         {
            bool pass5 = false;
            rejectedReason = 9;         // 9: reject due to tracking configuration 
            for (UnsignedInt i = 0; i < tkConfigList.size(); ++i)
            {
               if (dataObject->GetTrackingConfig() == tkConfigList[i])
               {
                  pass5 = true;
                  rejectedReason = 0;
                  break;
               }
            }
            if (!pass5)
            {
#ifdef DEBUG_FILTER
               MessageInterface::ShowMessage("AcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit7 return NULL\n", GetName().c_str(), this, dataObject, rejectedReason);
#endif
               return NULL;
            }
         }
      }
      else
      {
         if (!HasFile(dataObject))
         {
#ifdef DEBUG_FILTER
            MessageInterface::ShowMessage("AcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit6.1 return NULL\n", GetName().c_str(), this, dataObject, rejectedReason);
#endif

            rejectedReason = 8;
            return NULL;
         }
      }
   }


   // 1. Observated objects verify: It will be passed the test when observation data contains one spacecraft in "observers" array
   if (!HasObserver(dataObject))
   {
      rejectedReason = 6;          // 6: rejected due to spacecraft is not found
#ifdef DEBUG_FILTER
      MessageInterface::ShowMessage("AcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit1 return NULL\n", GetName().c_str(), this, dataObject, rejectedReason);
#endif
      return NULL;                 // return NULL when it does not have any observer listing in DataFilter object. The value of rejectedReason has to be 6 
   }


   // 2. Trackers verify: It will be passed the test when observation data contains one ground station in "trackers" array
   if (!HasTracker(dataObject))
   {
      rejectedReason = 5;        // 5: rejected due to ground station is not found
#ifdef DEBUG_FILTER
      MessageInterface::ShowMessage("AcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit2 return NULL\n", GetName().c_str(), this, dataObject, rejectedReason);
#endif
      return NULL;             // return NULL when it does not have any tracker listing in DataFilter object. The value of rejectedReason has to be 5 
   }

   // 3. Measurement type verify: It will be passed the test when data type of observation data is found in "dataTypes" array
   if (!HasDataType(dataObject))
   {
      rejectedReason = 7;        // 7: rejected due to data type of observation data is not found
#ifdef DEBUG_FILTER
      MessageInterface::ShowMessage("AcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit3 return NULL\n", GetName().c_str(), this, dataObject, rejectedReason);
#endif
      return NULL;             // return NULL when it does not have any data type listing in DataFilter object. The value of rejectedReason has to be 7
   }

   // Strands verify:

   // 4. Time interval verify:
   if (!IsInTimeWindow(dataObject))
   {
      rejectedReason = 2;      // 2: rejected due to time span
#ifdef DEBUG_FILTER
      GmatEpoch currentEpoch = TimeConverterUtil::Convert(dataObject->epoch, dataObject->epochSystem, TimeConverterUtil::A1MJD);
      MessageInterface::ShowMessage(" currentEpoch = %.12lf    epochStart = %.12lf     epochEnd = %.12lf\n", currentEpoch, epochStart, epochEnd);
      MessageInterface::ShowMessage("AcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit4 return NULL  time out of range\n", GetName().c_str(), this, dataObject, rejectedReason);
#endif
      return NULL;             // return NULL when measurement epoch is not in time window. The value of rejectedReason has to be 2 
   }

   if (obDataId == -1)            // It only applys for statistics accepted filters 
   {
      // 5. Data thin verify:
      if (!IsThin(dataObject))
      {
         rejectedReason = 1;    // 1: reject due to thinning ratio
#ifdef DEBUG_FILTER
         MessageInterface::ShowMessage("AcceptFilter::FilteringData(dataObject = <%p>, rejectedReason = %d) exit5  return NULL   thinningFrequency = %d\n", dataObject, rejectedReason, thinningFrequency);
#endif
         return NULL;
      }
   }


   if (obDataId != -1)            // It only applys for estimation accepted filters 
   {
      // 6. Record numbers verify:
      if (!IsInRecordNumbersList(obDataId))
      {
         rejectedReason = 3;       // 3: not in the list of selected records
#ifdef DEBUG_FILTER
         MessageInterface::ShowMessage("AcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit3 return NULL  record number is not in the list of selected record\n", GetName().c_str(), this, dataObject, rejectedReason);
#endif
         return NULL;             // return NULL when observation data id is not in working file IDs list. The value of rejectedReason has to be 5 
      }
   }

#ifdef DEBUG_FILTER
   MessageInterface::ShowMessage("AcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit0 return <%p>\n", GetName().c_str(), this, dataObject, rejectedReason, dataObject);
#endif
   return dataObject;
}


bool AcceptFilter::IsInRecordNumbersList(Integer recNum)
{
   if (allRecNumbers)
      return true;

   //MessageInterface::ShowMessage("   recNumRanges.size() = %d   ", recNumRanges.size());
   for (Integer i = 0; i < recNumRanges.size(); i = i + 2)
   {
      
      if ((recNumRanges[i] <= recNum) && (recNum <= recNumRanges[i + 1]))
      {
         //MessageInterface::ShowMessage("Range [%d - %d]  recNum = %d  It is in   ", recNumRanges[i], recNumRanges[i + 1], recNum);
         return true;
      }
      //else
      //   MessageInterface::ShowMessage("Range [%d - %d]  recNum = %d  It is out   ", recNumRanges[i], recNumRanges[i + 1], recNum);
   }

   return false;
}


//------------------------------------------------------------------------------
// StringArray GetAllAvailableThinModes()
//------------------------------------------------------------------------------
/**
* Retrieves a list of all available thin modes
*
* @return        A list of all available thin modes
*/
//------------------------------------------------------------------------------
StringArray AcceptFilter::GetAllAvailableThinModes()
{
   StringArray nameList;
   nameList.push_back("Frequency");           // data thinning is applied based on record count
   nameList.push_back("Time");                // data thinning is applied based on time range

   return nameList;
}


//------------------------------------------------------------------------------
// bool IsThin(ObservationData* dataObject)
//------------------------------------------------------------------------------
/**
* Verify a data record to be satisfied thinning criteria
*
* @return        true if it is satisfied, false otherwise
*/
//------------------------------------------------------------------------------
bool AcceptFilter::IsThin(ObservationData* dataObject)
{
   bool isAccepted = false;
   // 1. Get tracking config from observation data 
   std::string trackingConfig = dataObject->GetTrackingConfig();

   if (thinMode == "Frequency")
   {
      // 2. If tracking config is not in recCountMap, set record count to 0 for that tracking config
      bool found = false;
      for (std::map<std::string, Integer>::iterator i = recCountMap.begin(); i != recCountMap.end(); ++i)
      {
         if (i->first == trackingConfig)
         {
            found = true;
            break;
         }
      }
      if (!found)
         recCountMap[trackingConfig] = 0;

      // 3. Book keeping record count for this tracking config 
      if (recCountMap[trackingConfig] == (thinningFrequency - 1))
      {
         isAccepted = true;
         recCountMap[trackingConfig] = 0;
      }
      else
      {
         ++recCountMap[trackingConfig];
      }
   }
   else if (thinMode == "Time")
   {
      // 2. If tracking config is not in startTimeWindowMap, set start time window to epochStart for that tracking config
      bool found = false;
      for (std::map<std::string, GmatEpoch>::iterator i = startTimeWindowMap.begin(); i != startTimeWindowMap.end(); ++i)
      {
         if (i->first == trackingConfig)
         {
            found = true;
            break;
         }
      }
      if (!found)
         startTimeWindowMap[trackingConfig] = epochStart;

      if (dataObject->epoch > startTimeWindowMap[trackingConfig])
      {
         isAccepted = true;
         Real step = GmatMathUtil::Floor((dataObject->epoch - startTimeWindowMap[trackingConfig]) * GmatTimeConstants::SECS_PER_DAY / thinningFrequency);
         startTimeWindowMap[trackingConfig] = startTimeWindowMap[trackingConfig] + (step + 1)*(thinningFrequency / GmatTimeConstants::SECS_PER_DAY);
      }
   }
   else
      throw MeasurementException("Error: " + GetName() + ".ThinMode parameter has an invalid value ('" + thinMode + "'.\n");

   return isAccepted;
}
