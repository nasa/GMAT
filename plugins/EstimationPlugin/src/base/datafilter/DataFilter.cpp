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
 * Implementation for the DataFilter class
 */
//------------------------------------------------------------------------------


#include "DataFilter.hpp"
#include "GmatBase.hpp"
#include "TimeSystemConverter.hpp"
#include "MessageInterface.hpp"
#include "MeasurementException.hpp"
#include "DateUtil.hpp" 
#include "StringUtil.hpp"
#include "DataFile.hpp"
#include <sstream>


//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION
//#define DEBUG_FILTER
//#define DEBUG_SET_PARAMETER



//#define TIME_EPSILON   5.0e-11          // 5.0e-11 Mdj

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string DataFilter::PARAMETER_TEXT[] =
{
   "FileNames",
   "ObservedObjects",
   "Trackers",
   "DataTypes",
   "EpochFormat",
   "InitialEpoch",
   "FinalEpoch",
   "Strands",
};

const Gmat::ParameterType DataFilter::PARAMETER_TYPE[] =
{
   Gmat::STRINGARRAY_TYPE,			// FILENAMES
   Gmat::OBJECTARRAY_TYPE,       //Gmat::STRINGARRAY_TYPE,			// OBSERVED_OBJECTS
   Gmat::OBJECTARRAY_TYPE,       //Gmat::STRINGARRAY_TYPE,       // TRACKERS
   Gmat::STRINGARRAY_TYPE,       // DATA_TYPES
   Gmat::STRING_TYPE,			   // EPOCH_FORMAT
   Gmat::STRING_TYPE,            // INITIAL_EPOCH
   Gmat::STRING_TYPE,			   // FINIAL_EPOCH
   Gmat::STRINGARRAY_TYPE,       // STRANDS
};



//------------------------------------------------------------------------------
// DataFilter(const std::string &ofType, const std::string name)
//------------------------------------------------------------------------------
/**
 * Constructor for DataFilter objects
 *
 * @param name The name of the object
 */
//------------------------------------------------------------------------------
DataFilter::DataFilter(const std::string &ofType, const std::string name) :
   GmatBase          (Gmat::DATA_FILTER, ofType, name),
   epochFormat       ("TAIModJulian"),
   initialEpoch      (DateUtil::EARLIEST_VALID_MJD), 
   finalEpoch        (DateUtil::LATEST_VALID_MJD),
   isChecked         (false),
   isDataFileDefaultVal  (true),
   isObserverDefaultVal  (true),
   isTrackerDefaultVal   (true),
   isDataTypeDefaultVal  (true),
   isEpochFormatSet  (false)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("DataFilter default constructor <%s,%p>\n", GetName().c_str(), this);
#endif

   objectTypes.push_back(Gmat::DATA_FILTER);
   objectTypeNames.push_back("DataFilter");

   parameterCount = DataFilterParamCount;
   
   epochStart = ConvertToRealEpoch(initialEpoch, epochFormat);
   epochEnd = ConvertToRealEpoch(finalEpoch, epochFormat);

   observers.push_back("All");
   //fileNames.push_back("From_AddTrackingConfig");
   fileNames.push_back("All");
   trackers.push_back("All");
   dataTypes.push_back("All");
}


//------------------------------------------------------------------------------
// ~DataFilter()
//------------------------------------------------------------------------------
/**
 * DataFilter destructor
 */
//------------------------------------------------------------------------------
DataFilter::~DataFilter()
{
   // clean up StringArray fileNames;
   fileNames.clear();
   // clean up StringArray observers;
   observers.clear();
   // clean up ObjectArray observerObjects;
   observerObjects.clear();
   // clean up StringArray trackers;
   trackers.clear();
   // clean up ObjectArray trackerObjects;
   trackerObjects.clear();
   // clean up StringArray dataTypes;
   dataTypes.clear();
   // clean up StringArray strands;
   strands.clear();
}


//------------------------------------------------------------------------------
// DataFilter(const DataFilter& saf)
//------------------------------------------------------------------------------
/**
 * Copy constructor for a DataFilter
 *
 * @param saf        The DataFilter object that provides data for the new one
 */
//------------------------------------------------------------------------------
DataFilter::DataFilter(const DataFilter& saf) :
   GmatBase              (saf),
   fileNames             (saf.fileNames),
   isDataFileDefaultVal  (saf.isDataFileDefaultVal),
   observers             (saf.observers),
   observerObjects       (saf.observerObjects),
   isObserverDefaultVal  (saf.isObserverDefaultVal),
   trackers              (saf.trackers),
   trackerObjects        (saf.trackerObjects),
   isTrackerDefaultVal   (saf.isTrackerDefaultVal),
   dataTypes             (saf.dataTypes),
   isDataTypeDefaultVal  (saf.isDataTypeDefaultVal),
   epochFormat           (saf.epochFormat),
   initialEpoch          (saf.initialEpoch),
   finalEpoch            (saf.finalEpoch),
   epochStart            (saf.epochStart),
   epochEnd              (saf.epochEnd),
   strands               (saf.strands),
   //dataTypesMap          (saf.dataTypesMap),
   isChecked             (false),
   isEpochFormatSet      (saf.isEpochFormatSet)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("DataFilter copy constructor from <%s,%p>  to  <%s,%p>\n", saf.GetName().c_str(), &saf, GetName().c_str(), this);
#endif

}


//------------------------------------------------------------------------------
// DataFilter& operator=(const DataFilter& saf)
//------------------------------------------------------------------------------
/**
 * DataFilter assignment operator
 *
 * @param saf    The DataFilter object that provides data for the this one
 *
 * @return This object, configured to match saf
 */
//------------------------------------------------------------------------------
DataFilter& DataFilter::operator=(const DataFilter& saf)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("DataFilter operator = <%s,%p>\n", GetName().c_str(), this);
#endif

   if (this != &saf)
   {
      GmatBase::operator=(saf);

      fileNames            = saf.fileNames;
      isDataFileDefaultVal = saf.isDataFileDefaultVal;
      observers            = saf.observers;
      observerObjects      = saf.observerObjects;
      isObserverDefaultVal = saf.isObserverDefaultVal;
      trackers             = saf.trackers;
      trackerObjects       = saf.trackerObjects;
      isTrackerDefaultVal  = saf.isTrackerDefaultVal;
      dataTypes            = saf.dataTypes;
      isDataTypeDefaultVal = saf.isDataTypeDefaultVal;
      epochFormat  = saf.epochFormat;
      initialEpoch = saf.initialEpoch;
      finalEpoch   = saf.finalEpoch;
      epochStart   = saf.epochStart;
      epochEnd     = saf.epochEnd;
      strands      = saf.strands;
      //dataTypesMap = saf.dataTypesMap;
      isChecked    = false;
      isEpochFormatSet = saf.isEpochFormatSet;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone method for DataFilter
 *
 * @return A clone of this object.
 */
//------------------------------------------------------------------------------
GmatBase* DataFilter::Clone() const
{
   return new DataFilter(*this);
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
bool DataFilter::Initialize()
{
#ifdef DEBUG_INITIALIZATION
	MessageInterface::ShowMessage("DataFilter<%s,%p>::Initialize()   entered\n", GetName().c_str(), this);
#endif

   bool retval = true;

   if (epochStart > epochEnd)
   {
      std::stringstream ss;
      ss << "Error: " << GetName() << ".InitialEpoch (" << epochStart << ") is greater than " << GetName() << ".FinalEpoch (" << epochEnd << ")\n";
      throw MeasurementException(ss.str());
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("DataFilter<%s,%p>::Initialize()   exit\n", GetName().c_str(), this);
#endif

   isInitialized = retval;
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
bool DataFilter::CleanUp()
{
   bool retval = true;

   // Nothing yet that needs to be cleaned up from the DataFilter class

   return retval;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the text string used to script a DataFilter property
 *
 * @param id The ID of the property
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string DataFilter::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < DataFilterParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the units used for a property
 *
 * @param id The ID of the property
 *
 * @return The text string specifying the property's units
 */
//------------------------------------------------------------------------------
std::string DataFilter::GetParameterUnit(const Integer id) const
{
   // @Todo: It needs to add code to specify unit used for each parameter
   return GmatBase::GetParameterUnit(id);
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
Integer DataFilter::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < DataFilterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
      {
         if (IsParameterReadOnly(i))
            throw MeasurementException("Error: " + str + "parameter was not defined in DataFilter.\n");
         return i;
      }
   }

   return GmatBase::GetParameterID(str);
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
Gmat::ParameterType DataFilter::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < DataFilterParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
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
std::string DataFilter::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
* Specify either parameter read only or not
*
* @param id  The ID of the property
*
* @return    true if parameter read only, false otherwise
*/
//------------------------------------------------------------------------------
bool DataFilter::IsParameterReadOnly(const Integer id) const
{
   if (id == STRANDS)
      return true;

   return GmatBase::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Specify either parameter read only or not
*
* @param label  name of parameter
*
* @return       true if parameter read only, false otherwise
*/
//------------------------------------------------------------------------------
bool DataFilter::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
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
std::string DataFilter::GetStringParameter(const Integer id) const
{
   if (id == EPOCH_FORMAT)
      return epochFormat;

   if (id == INITIAL_EPOCH)
      return initialEpoch;

   if (id == FINAL_EPOCH)
      return finalEpoch;

   return GmatBase::GetStringParameter(id);
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
bool DataFilter::SetStringParameter(const Integer id, const std::string &value)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("DataFilter<%s,%p>::SetStringParameter(id = %d, value = <%s>) enter1\n", GetName().c_str(), this, id, value.c_str());
#endif
   if (id == FILENAMES)
   {
      if (isDataFileDefaultVal)
      {
         fileNames.clear();
         isDataFileDefaultVal = false;
      }

      // Check for empty list
      if (value == "")
         throw MeasurementException("Error: an empty string is set to " + GetName() + ".FileNames parameter.\n");
      else if (GmatStringUtil::RemoveSpaceInBrackets(value, "{}") == "{}")
      {
         // throw MeasurementException("Error: an empty list of file name was set to " + GetName() + ".FileNames parameter.\n");
         // We accept an empty list of file names
         //allDataFile = false;
         fileNames.clear();
      }
      else
      {
         // If is not empty list, it is a name containing file name or "All"
         //if (value == "All")
         //{
         //   allDataFile = true;
         //}
         //else
         //{
            // Check for valid file name
            Integer err = 0;
            if (!GmatStringUtil::IsValidFullFileName(value, err))
               throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".FileNames parameter is an invalid file name.\n");
         //}

         //if ((allDataFile == false) && (fileNames[0] == "All"))
         //   fileNames.erase(fileNames.begin());

         if (find(fileNames.begin(), fileNames.end(), value) == fileNames.end())
            fileNames.push_back(value);
         else
            throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".FileNames is replicated.\n");
      }
      return true;
   }

   if (id == EPOCH_FORMAT)
   {
      if (TimeSystemConverter::Instance()->IsValidTimeSystem(value) == false)
         throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".EpochFormat parameter is invalid.\n");

      epochFormat = value;
      isEpochFormatSet = true;

      //Real t1;
      //TimeSystemConverter::Convert("A1ModJulian", epochStart, "", value, t1, initialEpoch);
      //TimeSystemConverter::Convert("A1ModJulian", epochEnd, "", value, t1, finalEpoch);
      
      return true;
   }

   if (id == INITIAL_EPOCH)
   {
      if (!isEpochFormatSet)
         MessageInterface::ShowMessage("Warning: In your script, %s.EpochFormat parameter has to be set before setting %s.InitialEpoch.\n", GetName().c_str(), GetName().c_str());

      initialEpoch = value;
      // Convert to a.1 time for internal processing
      epochStart = ConvertToRealEpoch(initialEpoch, epochFormat);
      return true;
   }

   if (id == FINAL_EPOCH)
   {
      if (!isEpochFormatSet)
         MessageInterface::ShowMessage("Warning: In your script, %s.EpochFormat parameter has to be set before setting %s.FinalEpoch.\n", GetName().c_str(), GetName().c_str());

      finalEpoch = value;
      // Convert to a.1 time for internal processing
      epochEnd = ConvertToRealEpoch(finalEpoch, epochFormat);
      return true;
   }

   if (id == OBSERVED_OBJECTS)
   {
      // Remove default value whenever setting value
      if (isObserverDefaultVal)
      {
         observers.clear();
         isObserverDefaultVal = false;
      }

      if (value == "")
         throw MeasurementException("Error: an empty string is set to " + GetName() + ".ObservedObjects.\n");  
      else if (GmatStringUtil::RemoveSpaceInBrackets(value,"{}") == "{}")
      {
         //allObserver = false;
         observers.clear();
      }
      else
      {
         if (!GmatStringUtil::IsValidExtendedIdentity(value))        // it is for GPS Point Solution
            throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".ObservedObjects parameter is an invalid observed object's name.\n");

         if (find(observers.begin(), observers.end(), value) == observers.end())
            observers.push_back(value);
      }
      return true;
   }

   if (id == TRACKERS)
   {
      if (isTrackerDefaultVal)
      {
         trackers.clear();
         isTrackerDefaultVal = false;
      }

      if (value == "")
         throw MeasurementException("Error: an empty string is set to " + GetName() + ".Trackers.\n");  
      else if (GmatStringUtil::RemoveSpaceInBrackets(value,"{}") == "{}")
      {
         //allTracker = false;
         trackers.clear();
      }
      else
      {
         //if (value == "All")
         //   allTracker = true;
         //else 
         if (!GmatStringUtil::IsValidIdentity(value))
            throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".Trackers parameter is invalid tracker's name.\n");

         //if ((allTracker == false) && (trackers[0] == "All"))
         //   trackers.erase(trackers.begin());

         if (find(trackers.begin(), trackers.end(), value) == trackers.end())
            trackers.push_back(value);
      }
      return true;
   }

   if (id == DATA_TYPES)
   {
      if (isDataTypeDefaultVal)
      {
         dataTypes.clear();
         isDataTypeDefaultVal = false;
      }

      if (value == "")
         throw MeasurementException("Error: an empty string is set to " + GetName() + ".DataTypes.\n");
      else if (GmatStringUtil::RemoveSpaceInBrackets(value,"{}") == "{}")
      {
         //allDataType = false;
         dataTypes.clear();
      }
      else
      {
         std::string name = value;
         //if (name == "All")
         //   allDataType = true;
         //else
         //{
            //// Check for deprecation
            //std::map<std::string,std::string> typeMap = GetDeprecatedTypeMap();
            //for (std::map<std::string, std::string>::iterator i = typeMap.begin(); i != typeMap.end(); ++i)
            //{
            //   if ((*i).first == name)
            //   {
            //      MessageInterface::ShowMessage("Warning: '" + GetName() + ".DataTypes name + '" 
            //         + name + "' is deprecated and will be removed in a future release. Use '"
            //         + (*i).second + "' instead.\n");
            //      name = (*i).second;
            //      break;
            //   }
            //}

            // Check for valid measurement type
            ObservationData obData;
            if (!obData.IsValidMeasurementType(name))
               throw MeasurementException("Error: '" + name + "' set to " + GetName() + ".DataTypes parameter is an invalid measurement type.\n");
         //}

         //if ((allDataType == false) && (dataTypes[0] == "All"))
         //   dataTypes.erase(dataTypes.begin());

         if (find(dataTypes.begin(), dataTypes.end(), name) == dataTypes.end())
            dataTypes.push_back(name);
      }
      return true;
   }

   return GmatBase::SetStringParameter(id, value);
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
std::string DataFilter::GetStringParameter(const std::string &label) const
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
bool DataFilter::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property from a StringArray object
 *
 * @param id      The ID of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string DataFilter::GetStringParameter(const Integer id, const Integer index) const
{
   if (id == FILENAMES)
   {
      if ((index < 0)||(index >= (Integer)fileNames.size()))
      {
         std::stringstream ss;
         ss << "Error: file name's index (" << index << ") is out of bound.\n";
         throw GmatBaseException(ss.str());
      }

      return fileNames[index];
   }

   if (id == OBSERVED_OBJECTS)
   {
      if ((index < 0)||(index >= (Integer)observers.size()))
      {
         std::stringstream ss;
         ss << "Error: observer name's index (" << index << ") is out of bound.\n";
         throw GmatBaseException(ss.str());
      }

      return observers[index];
   }

   if (id == TRACKERS)
   {
      if ((index < 0)||(index >= (Integer)trackers.size()))
      {
         std::stringstream ss;
         ss << "Error: tracker name's index (" << index << ") is out of bound.\n";
         throw GmatBaseException(ss.str());
      }

      return trackers[index];
   }

   if (id == DATA_TYPES)
   {
      if ((index < 0)||(index >= (Integer)dataTypes.size()))
      {
         std::stringstream ss;
         ss << "Error: data type's index (" << index << ") is out of bound.\n";
         throw GmatBaseException(ss.str());
      }

      return dataTypes[index];
   }

   if (id == STRANDS)
   {
      if ((index < 0)||(index >= (Integer)strands.size()))
      {
         std::stringstream ss;
         ss << "Error: strand's index (" << index << ") is out of bound.\n";
         throw GmatBaseException(ss.str());
      }

      return strands[index];
   }

   return GmatBase::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Set value to a property of StringArray type
 *
 * @param id      The ID of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return true if the setting successes and false otherwise
 */
//------------------------------------------------------------------------------
bool DataFilter::SetStringParameter(const Integer id, const std::string &value,
                                           const Integer index)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("DataFilter<%s,%p>::SetStringParameter(id = %d, value = <%s>, index = %d) enter2\n", GetName().c_str(), this, id, value.c_str(), index);
#endif

   if (id == FILENAMES)
   {
      // an empty list is set to FileNames parameter when index == -1
      if (index == -1)
      {
         fileNames.clear();
         return true;
      } 
      else if ((0 <= index)&&(index <= (Integer)fileNames.size()))
      {
         if (value == "")
            throw GmatBaseException("Error: cannot assign an empty string to " + GetName() + ".FileNames parameter.\n");

         Integer error = 0;
         if (!GmatStringUtil::IsValidFullFileName(value, error))
            throw GmatBaseException("Error: '" + value + "' set to " + GetName() + ".FileNames parameter is an invalid file name.\n");

         if (index == fileNames.size())
            fileNames.push_back(value);
         else
            fileNames[index] = value;
         return true;
      }
      else
      {
         std::stringstream ss;
         ss << "Error: file name's index (" << index << ") is out of bound.\n"; 
         throw GmatBaseException(ss.str());
      }
   }

   if (id == OBSERVED_OBJECTS)
   {
      if (isObserverDefaultVal)
      {
         observers.clear();
         isObserverDefaultVal = false;
      }

      // an empty list is set to ObservedObjects parameter when index == -1 
      if (index == -1)
      {
         observers.clear();
         return true;
      } 
      else if ((0 <= index)&&(index <= (Integer)observers.size()))
      {
         if (value == "")
            throw MeasurementException("Error: cannot assign an empty string to observer ID.\n");

         if (!GmatStringUtil::IsValidExtendedIdentity(value))           // It is for GPS Point Solution
            throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".ObservedObjects parameter is an invalid GMAT object name.\n");

         if (index == observers.size())
            observers.push_back(value);
         else
            observers[index] = value;
         return true;
      }
      else
      {
         std::stringstream ss;
         ss << "Error: observer's index (" << index << ") is out of bound.\n"; 
         throw GmatBaseException(ss.str());
      }
   }

   if (id == TRACKERS)
   {
      if (isTrackerDefaultVal)
      {
         trackers.clear();
         isTrackerDefaultVal = false;
      }

      // an empty list is set to Trackers parameter when index == -1
      if (index == -1)
      {
         trackers.clear();
         return true;
      } 
      else if ((0 <= index)&&(index <= (Integer)trackers.size()))
      {
         if (value == "")
            throw MeasurementException("Error: cannot accept an empty string to a tracker ID.\n");
         //else
         //   allTracker = true;         // set flag to indicate choosing all trackers

         if (!GmatStringUtil::IsValidIdentity(value))
            throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".Trackers parameter is an invalid GMAT object name.\n");

         if (index == trackers.size())
            trackers.push_back(value);
         else
            trackers[index] = value;
         return true;
      }
      else
      {
         std::stringstream ss;
         ss << "Error: tracker's index (" << index << ") is out of bound.\n"; 
         throw GmatBaseException(ss.str());
      }
   }

   if (id == DATA_TYPES)
   {
      // an empty list is set to DataTypes parameter when index == -1
      if (index == -1)
      {
         dataTypes.clear();
         return true;
      } 
      else if ((0 <= index)&&(index <= (Integer)dataTypes.size()))
      {
         // Check for deprecation
         std::string name = value;
         //std::map<std::string, std::string> typeMap = GetDeprecatedTypeMap();
         //for (std::map<std::string, std::string>::iterator i = typeMap.begin(); i != typeMap.end(); ++i)
         //{
         //   if ((*i).first == name)
         //   {
         //      MessageInterface::ShowMessage("Warning: " + GetName() + ".DataTypes name '" 
         //         + name + "' is deprecated and will be removed in a future release. Use '"
         //         + (*i).second + "' instead.\n");
         //      name = (*i).second;
         //      break;
         //   }
         //}

         // Check for valid measurement type
         StringArray nameList = GetListOfMeasurementTypes();
         if ((name != "All")&&(find(nameList.begin(), nameList.end(), name) == nameList.end()))
            throw MeasurementException("Error: Value '" + name + "' set to " + GetName() + ".DataTypes parameter is invalid.\n");

         if (index == dataTypes.size())
            dataTypes.push_back(name);
         else
            dataTypes[index] = name;
         return true;
      }
      else
      {
         std::stringstream ss;
         ss << "Error: data type's index (" << index << ") is out of bound.\n"; 
         throw GmatBaseException(ss.str());
      }
   }


   if (id == STRANDS)
   {
      // an empty list is set to Strands parameter when index == -1
      if (index == -1)
      {
         strands.clear();
         return true;
      } 
      else if ((0 <= index)&&(index <= (Integer)strands.size()))
      {
         if (value == "")
            throw MeasurementException("Error: cannot assign an empty string to strand.\n");

         if (index == strands.size())
            strands.push_back(value);
         else
            strands[index] = value;
         return true;
      }
      else
      {
         std::stringstream ss;
         ss << "Error: strand's index (" << index << ") is out of bound.\n"; 
         throw GmatBaseException(ss.str());
      }
   }

   return GmatBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property from a StringArray object
 *
 * @param labe    The name of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string DataFilter::GetStringParameter(const std::string &label, const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Set value to a property of StringArray type
 *
 * @param label   The name of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return true if the setting successes and false otherwise
 */
//------------------------------------------------------------------------------
bool DataFilter::SetStringParameter(const std::string &label, const std::string &value,
                                           const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const std::string GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray property
 *
 * @param id The ID of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
const StringArray& DataFilter::GetStringArrayParameter(const Integer id) const
{
   if (id == FILENAMES)
      return fileNames;

   if (id == OBSERVED_OBJECTS)
      return observers;

   if (id == TRACKERS)
      return trackers;

   if (id == DATA_TYPES)
      return dataTypes;

   if (id == STRANDS)
      return strands;

   return GmatBase::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const std::string GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray property
 *
 * @param label   The name of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
const StringArray& DataFilter::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


Real DataFilter::GetRealParameter(const Integer id) const
{
   if (id == INITIAL_EPOCH)
      return epochStart;
   else if (id == FINAL_EPOCH)
      return epochEnd;

   return GmatBase::GetRealParameter(id);
}


Real DataFilter::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type,
//------------------------------------------------------------------------------
/**
 * Renames references objects
 *
 * @param type The type of object that is renamed
 * @param oldName The name of the object that is changing
 * @param newName the new object name
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataFilter::RenameRefObject(const UnsignedInt type,
      const std::string & oldName, const std::string & newName)
{
   /// @todo Estimator rename code needs to be implemented
   return GmatBase::RenameRefObject(type, oldName, newName);
}



//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * This method retrieves an array of reference object types
 *
 * @return The array
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& DataFilter::GetRefObjectTypeArray()
{
   static std::vector<UnsignedInt> objTypes;
   objTypes.clear();

   objTypes.push_back(Gmat::SPACECRAFT);
   objTypes.push_back(Gmat::GROUND_STATION);

   return objTypes;
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Initialization method that identifies the reference objects needed
 *
 * @param type The ObjectType for the references; UNKNOWN_OBJECT retrieves all
 *
 * @return A StringArray with all of the object names.
 */
//------------------------------------------------------------------------------
const StringArray& DataFilter::GetRefObjectNameArray(const UnsignedInt type)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
            "DataFilter::GetRefObjectNameArray(%d) entered\n", type);
   #endif

   static StringArray nameList;
   nameList.clear();

   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::SPACE_POINT) || (type == Gmat::SPACECRAFT) || (type == Gmat::GROUND_STATION))
   {
      if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::SPACE_POINT) || (type == Gmat::SPACECRAFT))
      {
         for (UnsignedInt i = 0; i < observers.size(); ++i)
         {
            if (observers[i] == "All")
               continue;                             // do not add "All" to name list

            if (find(nameList.begin(), nameList.end(), observers[i]) == nameList.end())
            {
               #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage(
                  "   Adding name of observed objects: %s\n", observers[i].c_str());
               #endif

               nameList.push_back(observers[i]);
            }
         }
      }

      if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::SPACE_POINT) || (type == Gmat::GROUND_STATION))
      {
         for (UnsignedInt i = 0; i < trackers.size(); ++i)
         {
            if (trackers[i] == "All")
               continue;                          // do not add "All" to name list

            if (find(nameList.begin(), nameList.end(), trackers[i]) == nameList.end())
            {
               #ifdef DEBUG_ESTIMATOR_INITIALIZATION
                  MessageInterface::ShowMessage(
                     "   Adding name of tracker: %s\n", trackers[i].c_str());
               #endif
               nameList.push_back(trackers[i]);
            }
         }
      }

   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Name list of observer objects: size = %d\n", observers.size());
      for(UnsignedInt i = 0; i < observers.size(); ++i)
         MessageInterface::ShowMessage("%d: <%s>\n", i, observers[i].c_str());
      MessageInterface::ShowMessage("Name list of tracker objects: size = %d\n", trackers.size());
      for(UnsignedInt i = 0; i < trackers.size(); ++i)
         MessageInterface::ShowMessage("%d: <%s>\n", i, trackers[i].c_str());
      MessageInterface::ShowMessage("Name list of Refrerent Objects: size = %d\n", nameList.size());
      for(UnsignedInt i = 0; i < nameList.size(); ++i)
         MessageInterface::ShowMessage("%d: <%s>\n", i, nameList[i].c_str());
      MessageInterface::ShowMessage(
            "DataFilter::GetRefObjectNameArray(%d) exit\n", type);
   #endif
   return nameList;
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a referenced object of a given type
 *
 * @param type The object's type
 *
 * @return The name of the associated object
 */
//------------------------------------------------------------------------------
std::string DataFilter::GetRefObjectName(const UnsignedInt type) const
{
   return GmatBase::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string & name)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a referenced object of a given type and name
 *
 * @param type The object's type
 * @param name The object's name
 *
 * @return The pointer to the associated object
 */
//------------------------------------------------------------------------------
GmatBase* DataFilter::GetRefObject(const UnsignedInt type,
      const std::string & name)
{
   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::SPACECRAFT))
   {
      for(UnsignedInt i = 0; i < observerObjects.size(); ++i)
      {
         if (observerObjects[i]->GetName() == name)
            return observerObjects[i];
      }
   }

   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::GROUND_STATION))
   {
      for(UnsignedInt i = 0; i < trackerObjects.size(); ++i)
      {
         if (trackerObjects[i]->GetName() == name)
            return trackerObjects[i];
      }
   }

   return GmatBase::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string & name,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a referenced object of a given type and name from an
 * array of reference objects
 *
 * @param type The object's type
 * @param name The object's name
 * @param index The index to the object
 *
 * @return The pointer to the associated object
 */
//------------------------------------------------------------------------------
GmatBase* DataFilter::GetRefObject(const UnsignedInt type,
      const std::string & name, const Integer index)
{
   return GmatBase::GetRefObject(type, name, index);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//       const std::string & name)
//------------------------------------------------------------------------------
/**
 * Sets a pointer to a referenced object of a given type and name
 *
 * @param obj  The object
 * @param type The object's type
 * @param name The object's name
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataFilter::SetRefObject(GmatBase *obj, const UnsignedInt type,
      const std::string & name)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting ref object %s with type %s  %d\n",
            name.c_str(), obj->GetTypeName().c_str(), type);
   #endif
      bool isTracker = false;
      bool isObserver = false;

      for (int i = 0;i<trackers.size();i++)
      {
         if (trackers[i] == obj->GetName())
         {
            isTracker = true;
         }
      }
      for (int i = 0; i < observers.size(); i++)
      {
         if (observers[i] == obj->GetName())
         {
            isObserver = true;
         }
      }
   if (obj->IsOfType(Gmat::SPACECRAFT) && isObserver)
   {
      for(UnsignedInt i = 0; i < observerObjects.size(); ++i)
      {
         if (observerObjects[i]->GetName() == name)
            return true;
      }
      observerObjects.push_back(obj);
      return true;
   }


   if ((obj->IsOfType(Gmat::GROUND_STATION) || obj->IsOfType(Gmat::SPACECRAFT)) && isTracker)
   {
      for(UnsignedInt i = 0; i < trackerObjects.size(); ++i)
      {
         if (trackerObjects[i]->GetName() == name)
            return true;
      }
      trackerObjects.push_back(obj);
      return true;
   }

   return GmatBase::SetRefObject(obj, type, name);
}

//------------------------------------------------------------------------------
// ObjectArray& GetRefObjectArray(const std::string & typeString)
//------------------------------------------------------------------------------
/**
 * This method retrieves an array of reference objects of a given type
 *
 * @param typeString The type of object requested
 *
 * @return The array of objects
 */
//------------------------------------------------------------------------------
ObjectArray& DataFilter::GetRefObjectArray(const std::string & typeString)
{
   return GetRefObjectArray(GetObjectType(typeString));
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//       const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a pointer to a referenced object of a given type and name in an array of
 * objects of that type
 *
 * @param obj  The object
 * @param type The object's type
 * @param name The object's name
 * @param index The index into the object array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataFilter::SetRefObject(GmatBase *obj, const UnsignedInt type,
      const std::string & name, const Integer index)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(""
            "Setting indexed ref object %s with type %s\n", name.c_str(),
            obj->GetTypeName().c_str());
   #endif

   return GmatBase::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// ObjectArray & GetRefObjectArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * This method retrieves an array of reference objects of a given type
 *
 * @param type The type of object requested
 *
 * @return The array of objects
 */
//------------------------------------------------------------------------------
ObjectArray & DataFilter::GetRefObjectArray(const UnsignedInt type)
{
   if (type == Gmat::SPACECRAFT)
   {
      return observerObjects;
   }

   if (type == Gmat::GROUND_STATION)
   {
      return trackerObjects;
   }

   return GmatBase::GetRefObjectArray(type);
}


//------------------------------------------------------------------------------
// Real ConvertToRealEpoch(const std::string &theEpoch,
//                         const std::string &theFormat)
//------------------------------------------------------------------------------
/**
 * Converts an epoch string is a specified format into
 *
 * @param theEpoch The input epoch
 * @param theFormat The format of the input epoch
 *
 * @return
 */
//------------------------------------------------------------------------------
Real DataFilter::ConvertToRealEpoch(const std::string &theEpoch,
                                   const std::string &theFormat)
{
   Real fromMjd = -999.999;
   Real retval = -999.999;
   std::string outStr;

   TimeSystemConverter::Instance()->Convert(theFormat, fromMjd, theEpoch, "A1ModJulian",
         retval, outStr);

   if (retval == -999.999)
      throw GmatBaseException("Error converting the time string \"" + theEpoch +
            "\"; please check the format for the input string.");
   return retval;
}


//-------------------------------------------------------------------------------
// StringArray GetListOfMeasurementTypes()
//-------------------------------------------------------------------------------
/**
* This function is used to get a name list of all measurement types (in new syntax)
*
* @return          a string array containing a name list of all measurement types 
*                  defined in new syntax
*/
//-------------------------------------------------------------------------------
StringArray DataFilter::GetListOfMeasurementTypes()
{
   static StringArray typeList;
   typeList.clear();

   //@todo: need to add any new measurement type here
   typeList.push_back("DSN_SeqRange");
   typeList.push_back("DSN_PNRange");
   typeList.push_back("DSN_TCP");
   typeList.push_back("GPS_PosVec");
   typeList.push_back("Range");
   typeList.push_back("RangeRate");
   typeList.push_back("Azimuth");
   typeList.push_back("Elevation");
   typeList.push_back("XEast");
   typeList.push_back("YNorth");
   typeList.push_back("XSouth");
   typeList.push_back("YEast");
   typeList.push_back("SN_Range");
   typeList.push_back("SN_Doppler");
   typeList.push_back("BRTS_Range");
   typeList.push_back("BRTS_Doppler");
   typeList.push_back("SN_Doppler_Rtn");
   typeList.push_back("SN_DOWD");

   if (GmatGlobal::Instance()->InTestingMode())
   {
      typeList.push_back("RightAscension");
      typeList.push_back("Declination");   
   }

   
   return typeList;
}


//-------------------------------------------------------------------------------
// ObjectArray GetListOfSpacecrafts()
//-------------------------------------------------------------------------------
/**
* This function is used to get a list of all spacecraft objects
*
* @return    an object array containing all spacecraft objects
*/
//-------------------------------------------------------------------------------
ObjectArray DataFilter::GetListOfSpacecrafts()
{
   StringArray nameList = GetListOfObjects(Gmat::SPACECRAFT);
   ObjectArray objectList;
   for (UnsignedInt i = 0; i < nameList.size(); ++i)
      objectList.push_back(GetConfiguredObject(nameList[i]));

   return objectList;
}


//-------------------------------------------------------------------------------
// ObjectArray GetListOfGroundStations()
//-------------------------------------------------------------------------------
/**
* This function is used to get a list of all ground station objects
*
* @return    an object array containing all ground station objects
*/
//-------------------------------------------------------------------------------
ObjectArray DataFilter::GetListOfGroundStations()
{
   StringArray nameList = GetListOfObjects(Gmat::GROUND_STATION);

   ObjectArray objectList;
   for (UnsignedInt i = 0; i < nameList.size(); ++i)
      objectList.push_back(GetConfiguredObject(nameList[i]));

   return objectList;
}


//-------------------------------------------------------------------------------
// ObjectArray DataFilter::GetListOfFiles()
//-------------------------------------------------------------------------------
/**
* This function is used to get a list of all data files
*
* @return    an object array containing all DataFile objects
*/
//-------------------------------------------------------------------------------
ObjectArray DataFilter::GetListOfFiles()
{
   StringArray nameList = GetListOfObjects(Gmat::DATA_FILE);

   ObjectArray objectList;
   for (UnsignedInt i = 0; i < nameList.size(); ++i)
      objectList.push_back(GetConfiguredObject(nameList[i]));

   return objectList;
}


//bool DataFilter::ValidateInput()
//{
//   if (isChecked)
//      return true;
//
//#ifdef DEBUG_FILTER
//   MessageInterface::ShowMessage("DataFilter<%s,%p>::ValidateInput()  enter\n", GetName().c_str(), this);
//#endif
//
//   ObjectArray objectList, objectList1;
//   StringArray nameList, nameList1;
//   bool found;
//
//   bool retval = true;
//   
//   // 1. Verify observers:
//   //MessageInterface::ShowMessage("Validate observers\n");
//   objectList = GetListOfSpacecrafts();
//   for (UnsignedInt i = 0; i < observerObjects.size(); ++i)
//   {
//      // validate observers[i]
//      found = false;
//      for (UnsignedInt j = 0; j < objectList.size(); ++j)
//      {
//         if (observerObjects[i]->GetName() == objectList[j]->GetName())
//         {
//            found = true;
//            break;
//         }
//      }
//
//      if (!found)
//      {
//         //MessageInterface::ShowMessage("Data filter %s:\n%s\n", GetName().c_str(), this->GetGeneratingString().c_str());
//         throw GmatBaseException("Error: observer '" + observerObjects[i]->GetName() + "' set to parameter " + GetName() + ".ObservedObjects was not defined in script\n"); 
//      }
//   }
//
//   // 2. Verify trackers:
//   //MessageInterface::ShowMessage("Validate trackers\n");
//   objectList1 = GetListOfGroundStations();
//   for (UnsignedInt i = 0; i < trackerObjects.size(); ++i)
//   {
//      // validate of trackerObjects[i]
//      found = false;
//      for (UnsignedInt j = 0; j < objectList1.size(); ++j)
//      {
//         if (trackerObjects[i]->GetName() == objectList1[j]->GetName())
//         {
//            found = true;
//            break;
//         }
//      }
//
//      if (!found)
//         throw GmatBaseException("Error: tracker '" + trackerObjects[i]->GetName() + "' which is set to parameter " + GetName() + ".Trackers was not defined in script\n"); 
//   }
//
//   // 3. Verify strands:
//   //MessageInterface::ShowMessage("Validate strands\n");
//   // 3.1. Get a list of all spacecrafts and ground stations
//   for (UnsignedInt i = 0; i < objectList1.size(); ++i)
//      objectList.push_back(objectList1[i]);
//
//   // 3.2. Verify all strands
//   for (UnsignedInt i = 0; i < strands.size(); ++i)
//   {
//      // 3.2.1. Get a strand in strand list:
//      std::string strand = strands[i];
//      
//      //3.2.2. Get a partcipant from the strand and verify it in valid participant list  
//      size_t pos = strand.find_first_of('-');                      // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
//      while (pos != std::string::npos)
//      {
//         // Get participant
//         std::string participant = "";
//         if (pos > 0)
//            participant = strand.substr(0, pos);
//
//         // Verify particicpant
//         found = false;
//         for (UnsignedInt j = 0; j < objectList.size(); ++j)
//         {
//            if (participant == objectList[j]->GetName())
//            {
//               found = true;
//               break;
//            }
//         }
//         if (!found)
//            throw GmatBaseException("Error: participant '" + participant + "' which is set to parameter " + GetName() + ".Strands was not defined in script\n"); 
//
//         // Reset value of strand
//         strand.substr(pos+1);
//         pos = strand.find_first_of('-');
//      }
//   }
//
//   // 6. Verify InitialEpoch:
//
//   // 7. Verify FinalEpoch:
//
//   isChecked = true;
//
//#ifdef DEBUG_FILTER
//   MessageInterface::ShowMessage("DataFilter<%s,%p>::ValidateInput()  exit\n", GetName().c_str(), this);
//#endif
//
//   return retval;
//}


ObservationData* DataFilter::FilteringData(ObservationData* dataObject, Integer& rejectedReason, Integer obDataId)
{
   throw MeasurementException("Error: Do not allow to run DataFilter::FilteringData()\n");
   return NULL;
}


bool DataFilter::HasFile(ObservationData* dataObject)
{
   bool has = false;
   if (!fileNames.empty())
   {
      if (find(fileNames.begin(), fileNames.end(), "All") == fileNames.end())
      {
         DataFile* df = dataObject->fileIndex;
         std::string fname = df->GetStringParameter("Filename");
         for (Integer i = 0; i < fileNames.size(); ++i)
         {
            if (fname == fileNames[i])
            {
               // When it found a DataFile object with a pointer matching to file index in observation data, it set true to "has" variable
               has = true;
               break;
            }
         }
      }
      else
         has = true;
   }
   
   return has;
}


bool DataFilter::HasObserver(ObservationData* dataObject)
{
   bool has = false;
   if (!observers.empty())
   {
      if (find(observers.begin(), observers.end(), "All") == observers.end())
      {
         for(UnsignedInt i = 0; i < observerObjects.size(); ++i)
         {
            for(UnsignedInt j = 1; j < dataObject->participantIDs.size(); ++j)
            {
               if (observerObjects[i]->GetStringParameter("Id") == dataObject->participantIDs[j])
               {
                  // When it found a spacecraft matching to a observer list in DataFilter object, it set true to "has" variable
                  has = true;
                  break;
               }
            }

            if (has)
               break;
         }
      }
      else
         has = true;
   }

   return has;
}


bool DataFilter::HasTracker(ObservationData* dataObject)
{
   bool has = false;
   if (!trackers.empty())
   {
      if (find(trackers.begin(), trackers.end(), "All") == trackers.end())
      {
         for(UnsignedInt i = 0; i < trackerObjects.size(); ++i)
         {
            //Only checks receive station
            if (trackerObjects[i]->GetStringParameter("Id") == dataObject->participantIDs[dataObject->participantIDs.size()-1])
            {
               has = true;
               break;
            }
         }
      }
      else
         has = true;
   }

   return has;
}


bool DataFilter::HasDataType(ObservationData* dataObject)
{
   bool has = false;
   if (!dataTypes.empty())
   {
      if (find(dataTypes.begin(), dataTypes.end(), "All") == dataTypes.end())
      {
         for(UnsignedInt i = 0; i < dataTypes.size(); ++i)
         {
            //if (dataTypesMap[dataTypes[i]] == dataObject->typeName)
            if (dataTypes[i] == dataObject->typeName)
            {
               has = true;
               break;
            }
         }
      }
      else
         has = true;
   }

   return has;
}


bool DataFilter::IsInTimeWindow(ObservationData* dataObject)
{
   bool isIn = true;
   
   GmatEpoch currentEpoch = TimeSystemConverter::Instance()->Convert(dataObject->epoch, dataObject->epochSystem, TimeSystemConverter::A1MJD);
   Real epsilon = 1.0e-12;
   if (((currentEpoch - epochStart)/currentEpoch < - epsilon) || ((currentEpoch - epochEnd)/currentEpoch  > epsilon))
      isIn = false;
   return isIn;
}


