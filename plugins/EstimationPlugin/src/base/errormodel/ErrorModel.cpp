//------------------------------------------------------------------------------
//                                 ErrorModel
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
// Created: 2015/01/07
//
/**
 * Implementation for the ErrorModel class
 */
//------------------------------------------------------------------------------


#include "ErrorModel.hpp"
#include "Rvector.hpp"
#include "StringUtil.hpp"
#include "TimeSystemConverter.hpp"
#include "GmatBase.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include <sstream>


//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string ErrorModel::PARAMETER_TEXT[] =
{
   "Type",
//   "Trip",
//   "Strand",
   "NoiseSigma",
//   "NoiseModel",
   "Bias",
   "BiasSigma",
   "PassBiases",
   "SolveFors",
   "Id",
};

const Gmat::ParameterType ErrorModel::PARAMETER_TYPE[] =
{
   Gmat::STRING_TYPE,			// TYPE                  // Its value will be "Range", "SN_Range", "BRTS_Range", "DSN_Seq_Range", "RangeRate", "DSN_TCP", "SN_Doppler", "SN_Doppler_Rtn", "BRTS_Doppler", "SN_DOWD"
//   Gmat::INTEGER_TYPE,			// TRIP                  // Its value is 1 for one-way, 2 for two-way, 3 for three-way, and so on
//   Gmat::STRINGARRAY_TYPE,    // STRAND                // containing a name list of participants along signal path
   Gmat::REAL_TYPE,			   // NOISE_SIGMA           // Measurement noise sigma value
//   Gmat::STRING_TYPE,         // NOISE_MODEL           // Specify model of error. It is "RandomConstant" for Gausian distribution 
   Gmat::REAL_TYPE,			   // BIAS                  // Measurement bias
   Gmat::REAL_TYPE,			   // BIAS_SIGMA            // Measurement bias sigma
   Gmat::RVECTOR_TYPE,        // PASS_BIAS             // Pass bias vector
   Gmat::STRINGARRAY_TYPE,    // SOLVEFORS             // Contains a list of all solve-for parameters in ErrorModel 
   Gmat::STRING_TYPE,         // MODEL_ID              // The id of the error model, which is the instanceName, but using objects' ids instead of names 
};



//------------------------------------------------------------------------------
// ErrorModel(const std::string name)
//------------------------------------------------------------------------------
/**
 * Constructor for ErrorModel objects
 *
 * @param name The name of the object
 */
//------------------------------------------------------------------------------
ErrorModel::ErrorModel(const std::string name) :
   GmatBase          (Gmat::ERROR_MODEL, "ErrorModel", name),
   measurementType   ("DSN_SeqRange"),
// measurementTrip   (2),
// strand            (""),
   noiseSigma        (103.0),                 // (0.01),                  // 0.01 measurement unit (km, RU, Km/s, or Hz)
// noiseModel        ("NoiseConstant"), 
   bias              (0.0),                   // 0.0 measurement unit (km, RU, Km/s, or Hz)
   biasSigma         (1.0e70),                // 0.0 measurement unit (km, RU, Km/s, or Hz)
   timeGap           (0.0),
   modelId           ("ErrorModelId")
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ErrorModel default constructor <%s,%p>\n", GetName().c_str(), this);
#endif

   objectTypes.push_back(Gmat::ERROR_MODEL);
   objectTypeNames.push_back("ErrorModel");

   parameterCount = ErrorModelParamCount;

   covariance.AddCovarianceElement("Bias", this);
   Integer locationStart = covariance.GetSubMatrixLocationStart(BIAS);
   covariance(locationStart, locationStart) = biasSigma * biasSigma;


 
   passBiases.Set(1, bias);                                 // Initially, pass bias is set to be the value of bias
   passBiasStartEpoches.push_back(new GmatTime(0.0));       // Initially, pass bias' start time is undefined. We set it to 0

   covariance.AddCovarianceElement("PassBiases", this);
   locationStart = covariance.GetSubMatrixLocationStart(PASS_BIASES);
   Integer dim = covariance.GetElementSize(PASS_BIASES);
   for (Integer row = 0; row < dim; ++row)
   {
      for (Integer col = 0; col < dim; ++col)
      {
         covariance(locationStart + row, locationStart + col) = ((row == col) ? (biasSigma * biasSigma) : 0.0);
      }
   }
}


//------------------------------------------------------------------------------
// ~ErrorModel()
//------------------------------------------------------------------------------
/**
 * ErrorModel destructor
 */
//------------------------------------------------------------------------------
ErrorModel::~ErrorModel()
{
   // All passBiasStartEpoches' elements are created by other objects. 
   // So it does not need to delete here because other objects handle it.
}


//------------------------------------------------------------------------------
// ErrorModel(const ErrorModel& em)
//------------------------------------------------------------------------------
/**
 * Copy constructor for a ErrorModel
 *
 * @param em        The ErrorModel object that provides data for the new one
 */
//------------------------------------------------------------------------------
ErrorModel::ErrorModel(const ErrorModel& em) :
   GmatBase              (em),
   measurementType       (em.measurementType),
//   measurementTrip       (em.measurementTrip),
//   participantNameList   (em.participantNameList),
   noiseSigma            (em.noiseSigma),
//   noiseModel            (em.noiseModel),
   bias                  (em.bias),
   biasSigma             (em.biasSigma),
   
   timeGap               (em.timeGap),
   passBiases            (em.passBiases),
   passBiasStartEpoches  (em.passBiasStartEpoches),

   solveforNames         (em.solveforNames),
   modelId               (em.modelId)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ErrorModel copy constructor from <%s,%p>  to  <%s,%p>\n", em.GetName().c_str(), &em, GetName().c_str(), this);
#endif

   covariance.AddCovarianceElement("Bias", this);
   Integer locationStart = covariance.GetSubMatrixLocationStart(BIAS);
   covariance(locationStart, locationStart) = biasSigma*biasSigma;

   covariance.AddCovarianceElement("PassBiases", this);
   locationStart             = covariance.GetSubMatrixLocationStart(PASS_BIASES);
   Integer dim               = covariance.GetElementSize(PASS_BIASES);
   Integer locationStartDest = em.covariance.GetSubMatrixLocationStart(PASS_BIASES);
   Integer dimDest           = em.covariance.GetElementSize(PASS_BIASES);

   // Resize the element
   if (dim != dimDest)
   {
      covariance.SetElementSize(PASS_BIASES, dimDest);
   }

   for (Integer row = 0; row < dimDest; ++row)
   {
      for (Integer col = 0; col < dimDest; ++col)
      {
         if ((row >= dim) || (col >= dim))
            covariance(locationStart + row, locationStart + col) = ((row == col) ? (biasSigma*biasSigma) : 0.0);
         else
            covariance(locationStart + row, locationStart + col) = em.covariance(locationStartDest + row, locationStartDest + col);
      }
   }     
}


//------------------------------------------------------------------------------
// ErrorModel& operator=(const ErrorModel& em)
//------------------------------------------------------------------------------
/**
 * ErrorModel assignment operator
 *
 * @param em    The ErrorModel object that provides data for the this one
 *
 * @return This object, configured to match em
 */
//------------------------------------------------------------------------------
ErrorModel& ErrorModel::operator=(const ErrorModel& em)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ErrorModel operator = <%s,%p>\n", GetName().c_str(), this);
#endif

   if (this != &em)
   {
      GmatBase::operator=(em);

      measurementType     = em.measurementType;
//      measurementTrip     = em.measurementTrip;
//      participantNameList = em.participantNameList;
      noiseSigma          = em.noiseSigma;
//      noiseModel          = em.noiseModel;
      bias                = em.bias;
      biasSigma           = em.biasSigma;

      solveforNames       = em.solveforNames;
      modelId             = em.modelId;

      Integer locationStart = covariance.GetSubMatrixLocationStart(BIAS);
      covariance(locationStart, locationStart) = biasSigma * biasSigma;


      // Copy value
      timeGap                   = em.timeGap;
      passBiases                = em.passBiases;
      passBiasStartEpoches      = em.passBiasStartEpoches;

      // Resize element's covariance and copy its value
      locationStart             = covariance.GetSubMatrixLocationStart(PASS_BIASES);
      Integer dim               = covariance.GetElementSize(PASS_BIASES);
      Integer locationStartDest = em.covariance.GetSubMatrixLocationStart(PASS_BIASES);
      Integer dimDest           = em.covariance.GetElementSize(PASS_BIASES);

      // Resize the element
      if (dim != dimDest)
      {
         covariance.SetElementSize(PASS_BIASES, dimDest);
      }

      for (Integer row = 0; row < dimDest; ++row)
      {
         for (Integer col = 0; col < dimDest; ++col)
         {
            if ((row >= dim) || (col >= dim))
               covariance(locationStart + row, locationStart + col) = ((row == col) ? (biasSigma*biasSigma) : 0.0);
            else
               covariance(locationStart + row, locationStart + col) = em.covariance(locationStartDest + row, locationStartDest + col);
         }
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
// bool operator==(const ErrorModel& em)
//------------------------------------------------------------------------------
/**
 * ErrorModel equal comparing operator
 *
 * @param em    The ErrorModel object that provides data for the this one
 *
 * @return      true if both error model objects having the same measurement 
 *              type, trip, and strand; false otherwise
 */
//------------------------------------------------------------------------------
bool ErrorModel::operator==(const ErrorModel& em)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ErrorModel operator == <%s,%p>\n", GetName().c_str(), this);
#endif

   if (this != &em)
   {
      //retVal = GmatBase::operator==(em);

      if ((GetName() != em.GetName())||
          (measurementType != em.measurementType)   //||
//          (measurementTrip != em.measurementTrip)||
//          (participantNameList != em.participantNameList)
         )
         return false;
   }
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone method for ErrorModel
 *
 * @return A clone of this object.
 */
//------------------------------------------------------------------------------
GmatBase* ErrorModel::Clone() const
{
   return new ErrorModel(*this);
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
bool ErrorModel::Initialize()
{
#ifdef DEBUG_INITIALIZATION
	MessageInterface::ShowMessage("ErrorModel<%s,%p>::Initialize()   entered\n", GetName().c_str(), this);
#endif
   if (isInitialized)
      return true;

   if (measurementType == "GPS_PosVec")
   {
      if (find(solveforNames.begin(), solveforNames.end(), "Bias") != solveforNames.end())
      {
         throw MeasurementException("The GPS_PosVec measurement type does not allow 'Bias' as a solve-for.\n");
      }

      if (find(solveforNames.begin(), solveforNames.end(), "PassBiases") != solveforNames.end())
      {
         throw MeasurementException("The GPS_PosVec measurement type does not allow 'PassBiases' as a solve-for.\n");
      }
   }

   bool retval = true;

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("ErrorModel<%s,%p>::Initialize()   exit\n", GetName().c_str(), this);
#endif

   isInitialized = retval;
   return retval;
}


//------------------------------------------------------------------------------
// bool Finalize()
//------------------------------------------------------------------------------
/**
 * Code that executes after a run completes
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool ErrorModel::Finalize()
{
   bool retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the text string used to script a ErrorModel property
 *
 * @param id The ID of the property
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string ErrorModel::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ErrorModelParamCount)
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
std::string ErrorModel::GetParameterUnit(const Integer id) const
{
   switch (id)
   {
   case BIAS:
   case BIAS_SIGMA:
   case PASS_BIASES:
      if (measurementType != "")
         return GetUnit(measurementType);
      else
         return "";
      break;

   case TYPE:
   case SOLVEFORS:
   case MODEL_ID:
      return "";                  // It has no unit

   default:
      break;
   }

   return GmatBase::GetParameterUnit(id);
}


//----------------------------------------------------------------------
// std::string GetUnit(std::string type)
//----------------------------------------------------------------------
/**
* Get unit for a given observation data type
*/
//----------------------------------------------------------------------
std::string ErrorModel::GetUnit(const std::string type) const
{
   std::string unit = "";
   if (type == "GPS_PosVec")
      unit = "km";
   else if (type == "DSN_PNRange")
      unit = "RU";
   else if (type == "DSN_SeqRange")
      unit = "RU";
   else if (type == "DSN_TCP")
      unit = "Hz";
   else if (type == "Range")
      unit = "km";
   else if (type == "SN_Range")
      unit = "km";
   else if (type == "BRTS_Range")
      unit = "km";
   else if (type == "RangeRate")
      unit = "km/s";
   else if (type == "SN_Doppler")
      unit = "Hz";
   else if (type == "SN_Doppler_Rtn")
      unit = "Hz";
   else if (type == "SN_DOWD")
      unit = "Hz";
   else if (type == "BRTS_Doppler")
      unit = "Hz";
   else if (type == "Azimuth")
      unit = "deg";
   else if (type == "Elevation")
      unit = "deg";
   else if (type == "XEast")
      unit = "deg";
   else if (type == "YNorth")
      unit = "deg";
   else if (type == "XSouth")
      unit = "deg";
   else if (type == "YEast")
      unit = "deg";
   else if (type == "RightAscension")
      unit = "deg";
   else if (type == "Declination")
      unit = "deg";
   else if (type == "Range_Skin")
      unit = "km";
   else
      unit = "";

   return unit;
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
Integer ErrorModel::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < ErrorModelParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
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
Gmat::ParameterType ErrorModel::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ErrorModelParamCount)
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
std::string ErrorModel::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false if not,
 */
 //---------------------------------------------------------------------------
bool ErrorModel::IsParameterReadOnly(const Integer id) const
{
   if (id == MODEL_ID)
      return true;
   if (id == PASS_BIASES)
      return true;

   return GmatBase::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
 //---------------------------------------------------------------------------
bool ErrorModel::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//---------------------------------------------------------------------------
//  bool IsParameterEnabled(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is enabled for GUI access.
 *
 * @param <id> ID for the parameter.
 *
 * @return true if the parameter is enabled, false if not
 */
 //---------------------------------------------------------------------------
bool ErrorModel::IsParameterEnabled(const Integer id) const
{
   if (id == PASS_BIASES)
      return true;

   return GmatBase::IsParameterEnabled(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterEnabled(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is enabled for GUI access.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is enabled, false if not
 */
 //---------------------------------------------------------------------------
bool ErrorModel::IsParameterEnabled(const std::string &label) const
{
   return IsParameterEnabled(GetParameterID(label));
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
std::string ErrorModel::GetStringParameter(const Integer id) const
{
   if (id == TYPE)
      return measurementType;

   //if (id == NOISE_MODEL)
   //   return noiseModel;

   if (id == MODEL_ID)
      return modelId;

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
bool ErrorModel::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == SOLVEFORS)
   {
      // if it is an empty list, then clear the solve-for list
      if (GmatStringUtil::RemoveSpaceInBrackets(value, "{}") == "{}")
      {
         solveforNames.clear();
         return true;
      }

      // If is not "{}", it is a string containing solve-for variable  
      // Check for valid identity
      if ((value != "Bias")&&(value != "PassBiases"))
      {
         throw GmatBaseException("Error: '" + value + "' is an invalid value. "
            + GetName() + ".SolveFors parameter only accepts Bias as a solve-for.\n");
         return false;
      }

      if (find(solveforNames.begin(), solveforNames.end(), value) == solveforNames.end())
         solveforNames.push_back(value);
      else
      {
         throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".SolveFors parameter is replicated.\n");
         return false;
      }

      bool hasBias = (find(solveforNames.begin(), solveforNames.end(), "Bias") != solveforNames.end());
      bool hasPassBiases = (find(solveforNames.begin(), solveforNames.end(), "PassBiases") != solveforNames.end());
      if (hasBias && hasPassBiases)
      {
         solveforNames.pop_back();
         throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".SolveFors parameter cannot solve for both Bias and PassBiases at the same time. You need to choose one of them.\n");
         return false;
      }

      return true;
   }

   if (id == TYPE)
   {
      
      std::string value1 = value;
      //value1 = CheckTypeDeprecation(value1);

      // Get a list of all available types
      StringArray typesList = GetAllAvailableTypes();
      bool found = false;
      for (Integer i = 0; i < typesList.size(); ++i)
      {
         if (typesList[i] == value1)
         {
            found = true;
            break;
         }
      }
      if (!found)
         throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".Type parameter is an invalid measurement type.\n");

      measurementType = value1;
      return true;
   }

   //if (id == NOISE_MODEL)
   //{
   //   if (value != "RandomConstant")
   //   {
   //      throw GmatBaseException("Error: " + GetName() + "." + GetParameterText(id) + " cannot accept '" + value +"'\n");
   //      return false;
   //   }
   //   else
   //   {
   //      noiseModel = value;    
	  //    return true;
   //   }
   //}

   if (id == MODEL_ID)
   {
      modelId = value;    
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
std::string ErrorModel::GetStringParameter(const std::string &label) const
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
bool ErrorModel::SetStringParameter(const std::string &label,
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
std::string ErrorModel::GetStringParameter(const Integer id, const Integer index) const
{
   //if (id == STRAND)
   //{
   //   if ((index < 0)||(index >= (Integer)participantNameList.size()))
   //   {
   //      std::stringstream ss;
   //      ss << "Error: participant index (" << index << ") is out of bound.\n";
   //      throw GmatBaseException(ss.str());
   //   }

   //   return participantNameList[index];
   //}

   if (id == SOLVEFORS)
   {
      if ((index < 0)||(index >= (Integer)solveforNames.size()))
      {
         std::stringstream ss;
         ss << "Error: solve-for index (" << index << ") is out of bound.\n";
         throw GmatBaseException(ss.str());
      }

      return solveforNames[index];
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
bool ErrorModel::SetStringParameter(const Integer id, const std::string &value,
                                           const Integer index)
{
   //if (id == STRAND)
   //{
   //   if ((0 <= index)&&(index < (Integer)participantNameList.size()))
   //   {
   //      participantNameList[index] = value;
   //      return true;
   //   }
   //   else
   //   {
   //      if (index == -1)
   //      {
   //         participantNameList.clear();
   //         return true;
   //      }
   //      else if (index == participantNameList.size())
   //      {
   //         participantNameList.push_back(value);
   //         return true;
   //      }
   //      else
   //      {
   //         std::stringstream ss;
   //         ss << "Error: participant name's index (" << index << ") is out of bound.\n"; 
   //         throw GmatBaseException(ss.str());
   //      }
   //   }
   //}

   if (id == SOLVEFORS)
   {
      // an empty list is set to SolveFors parameter when index == -1
      if (index == -1)
      {
         solveforNames.clear();
         return true;
      }

      if ((value != "Bias") && (value != "PassBiases"))
         throw GmatBaseException("Error: '" + value + "' is an invalid value. "
         + GetName() + ".SolveFors parameter only accepts Bias and PassBiases as a solve-for.\n");

      if ((0 <= index)&&(index < (Integer)solveforNames.size()))
      {
         bool found = false;
         for (Integer i = 0; i < solveforNames.size(); ++i)
         {
            if ((i != index) && (solveforNames[i] == value))
            {
               found = true;
               break;
            }
         }
         if (found)
            throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".SolveFors parameter is replicated.\n");
         else
            solveforNames[index] = value;
      }
      else
      {
         if (index == -1)
         {
            solveforNames.clear();
            return true;
         }
         else if (index == solveforNames.size())
         {
            if (find(solveforNames.begin(), solveforNames.end(), value) == solveforNames.end())
               solveforNames.push_back(value);
            else
            {
               throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".SolveFors parameter is replicated.\n");
               return false;
            }
         }
         else
         {
            std::stringstream ss;
            ss << "Error: solve-for's index (" << index << ") is out of bound.\n";
            throw GmatBaseException(ss.str());
         }
      }

      bool hasBias = (find(solveforNames.begin(), solveforNames.end(), "Bias") != solveforNames.end());
      bool hasPassBiases = (find(solveforNames.begin(), solveforNames.end(), "PassBiases") != solveforNames.end());
      if (hasBias && hasPassBiases)
      {
         throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".SolveFors parameter cannot solve for both Bias and PassBiases at the same time. You need to choose one of them.\n");
         return false;
      }
      return true;
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
std::string ErrorModel::GetStringParameter(const std::string &label, const Integer index) const
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
bool ErrorModel::SetStringParameter(const std::string &label, const std::string &value,
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
const StringArray& ErrorModel::GetStringArrayParameter(const Integer id) const
{
   //if (id == STRAND)
   //   return participantNameList;

   if (id == SOLVEFORS)
      return solveforNames;

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
const StringArray& ErrorModel::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Real property
 *
 * @param id   Id of the property
 *
 * @return The property value
 */
 //------------------------------------------------------------------------------
Real ErrorModel::GetRealParameter(const Integer id) const
{
   if (id == NOISE_SIGMA)
      return noiseSigma;

   if (id == BIAS)
      return bias;

   if (id == BIAS_SIGMA)
      return biasSigma;

   return GmatBase::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Set value for a Real property
 *
 * @param id       Id of the property
 * @param value    value set to the property
 *
 * @return The property value
 */
 //------------------------------------------------------------------------------
Real ErrorModel::SetRealParameter(const Integer id, const Real value)
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("ErrorModel::SetRealParameter(id = %d, value = %le\n", id, value);
#endif
   if (id == NOISE_SIGMA)
   {
      if (value <= 0.0)
         throw GmatBaseException("Error: value set to "+ GetName() +".NoiseSigma is a non positive number. It has to be a positive number.\n");

	  noiseSigma = value;
	  return noiseSigma;
   }

   if (id == BIAS)
   {
	  bias = value;
	  return bias;
   }

   if (id == BIAS_SIGMA)
   {
      if (value <= 0.0)
         throw GmatBaseException("Error: value set to " + GetName() + ".BiasSigma is a nonpositive number. It has to be a positive number.\n");

      biasSigma = value;

      // Update covariance value for Bias and PassBiases elements
      Real biasSigma2 = biasSigma * biasSigma;
      Integer locationStart = covariance.GetSubMatrixLocationStart(BIAS);
      covariance(locationStart, locationStart) = biasSigma2;

      locationStart = covariance.GetSubMatrixLocationStart(PASS_BIASES);
      if (locationStart >= 0)
      {
         Integer dim = covariance.GetElementSize(PASS_BIASES);
         for (Integer row = 0; row < dim; ++row)
            for (Integer col = 0; col < dim; ++col)
               covariance(locationStart + row, locationStart + col) = biasSigma2;

      }

      return biasSigma;
   }

   return GmatBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Real property
 *
 * @param label   the label of the property
 *
 * @return The property value
 */
 //------------------------------------------------------------------------------
Real ErrorModel::GetRealParameter(const std::string& label) const
{
	return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string& label, const Real value)
//------------------------------------------------------------------------------
/**
 * Set value for a Real property
 *
 * @param label    the label of the property
 * @param value    value set to the property
 *
 * @return The property value
 */
 //------------------------------------------------------------------------------
Real ErrorModel::SetRealParameter(const std::string& label, const Real value)
{
	return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Real property
 *
 * @param id       Id of the property
 * @param index    index of real element in real array
 *
 * @return The property value
 */
 //------------------------------------------------------------------------------
Real ErrorModel::GetRealParameter(const Integer id, const Integer index) const
{
   if (id == PASS_BIASES)
   {
      if ((index < 0) || (index >= passBiases.GetSize()))
         throw GmatBaseException("Error: Index is out of bound in ErrorModel::GetRealParameter() function.\n");

      return passBiases[index];
   }

   return GmatBase::GetRealParameter(id, index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Set value for a Real property
 *
 * @param id       Id of the property
 * @param value    value set to the property
 * @param index    index of real element in real array
 *
 * @return The property value
 */
 //------------------------------------------------------------------------------
Real ErrorModel::SetRealParameter(const Integer id, const Real value, const Integer index)
{
   if (id == PASS_BIASES)
   {
      if ((index < 0) || (index > passBiases.GetSize()))
         throw GmatBaseException("Error: Index is out of bound in ErrorModel::GetRealParameter() function.\n");
      else if (index == passBiases.GetSize())
      {
         // Resize and set value for pass biases vector
         Rvector temp = passBiases;
         passBiases.SetSize(passBiases.GetSize() + 1);
         for (Integer i = 0; i < temp.GetSize(); ++i)
            passBiases[i] = temp[i];
         passBiases[passBiases.GetSize()-1] = value;

         // Resize element's covariance and copy its value
         Integer locationStart = covariance.GetSubMatrixLocationStart(PASS_BIASES);
         Integer dim = covariance.GetElementSize(PASS_BIASES);
         Integer newdim = passBiases.GetSize();
         Covariance table = covariance;
         
         // Resize the element
         covariance.SetElementSize(PASS_BIASES, newdim);
         for (Integer row = 0; row < dim; ++row)
         {
            for (Integer col = 0; col < dim; ++col)
            {
               covariance(locationStart + row, locationStart + col) = table(locationStart + row, locationStart + col);
            }
         }
      }
      else
         passBiases[index] = value;

      return passBiases[index];
   }

   return GmatBase::SetRealParameter(id, value, index);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string& label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Real property
 *
 * @param label   the label of the property
 * @param index    index of real element in real array
 *
 * @return The property value
 */
 //------------------------------------------------------------------------------
Real ErrorModel::GetRealParameter(const std::string &label, const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Set value for a Real property
 *
 * @param label    the label of the property
 * @param value    value set to the property
 * @param index    index of real element in real array
 *
 * @return The property value
 */
 //------------------------------------------------------------------------------
Real ErrorModel::SetRealParameter(const std::string &label, const Real value, const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves an Integer property
 *
 * @param id   Id of the property
 *
 * @return The property value
 */
 //------------------------------------------------------------------------------
Integer ErrorModel::GetIntegerParameter(const Integer id) const
{
   //if (id == TRIP)
   //   return measurementTrip;

   return GmatBase::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Set value for an Integer property
 *
 * @param id       Id of the property
 * @param value    value set to the property
 *
 * @return The property value
 */
 //------------------------------------------------------------------------------
Integer ErrorModel::SetIntegerParameter(const Integer id, const Integer value)
{
   //if (id == TRIP)
   //{
   //   if (value < 0)
   //      throw GmatBaseException("Error: value of "+ GetName() +".Trip has to be a non-positive integer.\n");

	  // measurementTrip = value;
	  // return measurementTrip;
   //}

   return GmatBase::SetIntegerParameter(id, value);
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves an Integer property
 *
 * @param label   the label of the property
 *
 * @return The property value
 */
 //------------------------------------------------------------------------------
Integer ErrorModel::GetIntegerParameter(const std::string& label) const
{
	return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const std::string& label, const Integer value)
//------------------------------------------------------------------------------
/**
 * Set value for an Integer property
 *
 * @param label    the label of the property
 * @param value    value set to the property
 *
 * @return The property value
 */
 //------------------------------------------------------------------------------
Integer ErrorModel::SetIntegerParameter(const std::string& label, const Integer value)
{
	return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves real valued vector of data
 *
 * @param id The parameter ID
 *
 * @return The data vector
 */
 //------------------------------------------------------------------------------
const Rvector& ErrorModel::GetRvectorParameter(const Integer id) const
{
   if (id == PASS_BIASES)
      return passBiases;

   return GmatBase::GetRvectorParameter(id);
}

//------------------------------------------------------------------------------
// const Rvector& SetRvectorParameter(const Integer id, const Rvector &value)
//------------------------------------------------------------------------------
/**
 * Set real valued parameter data in vector containers
 *
 * @param id The parameter ID
 * @param value The new parameter vector of values
 *
 * @return The vector after the setting has occurred
 */
 //------------------------------------------------------------------------------
const Rvector& ErrorModel::SetRvectorParameter(const Integer id, const Rvector &value)
{
   if (id == PASS_BIASES)
   {
      Integer locationStart = covariance.GetSubMatrixLocationStart(PASS_BIASES);
      Integer dim = covariance.GetElementSize(PASS_BIASES);
      Integer dimDest = value.GetSize();

      // Resize the element
      if (dim != dimDest)
      {
         covariance.SetElementSize(PASS_BIASES, dimDest);
      }

      for (Integer row = dim; row < dimDest; ++row)
      {
         for (Integer col = dim; col < dimDest; ++col)
         {
            covariance(locationStart + row, locationStart + col) = ((row==col) ? (biasSigma*biasSigma) : 0.0);
         }
      }

      passBiases.SetSize(value.GetSize());
      passBiases = value;

      return passBiases;
   }

   return GmatBase::SetRvectorParameter(id, value);
}


//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves real valued vector of data
 *
 * @param label The label of the parameter
 *
 * @return The data vector
 */
 //------------------------------------------------------------------------------
const Rvector& ErrorModel::GetRvectorParameter(const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const Rvector& SetRvectorParameter(const std::string &label, const Rvector &value)
//------------------------------------------------------------------------------
/**
 * Set real valued vector of data
 *
 * @param label The label of the parameter
 *
 * @return The data vector
 */
 //------------------------------------------------------------------------------
const Rvector& ErrorModel::SetRvectorParameter(const std::string &label, const Rvector &value)
{
   return SetRvectorParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool IsEstimationParameterValid(const Integer item)
//------------------------------------------------------------------------------
/**
* This function is used to verify an estimation paramter is either valid or not
*
* @param item      Estimation parameter ID (Note that: it is defferent from object ParameterID)
*
* return           true if it is valid, false otherwise 
*/
//------------------------------------------------------------------------------
bool ErrorModel::IsEstimationParameterValid(const Integer item)
{
   bool retval = false;

   // Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;    // convert Estimation ID to object parameter ID
   Integer id = (item > ESTIMATION_TYPE_ALLOCATION ? 
      item - type * ESTIMATION_TYPE_ALLOCATION : item);

   switch (id)
   {
      case BIAS:
      case PASS_BIASES:
         retval = true;
         break;

      // All other values call up the hierarchy
      default:
         retval = GmatBase::IsEstimationParameterValid(item);
   }

   return retval;
}


//------------------------------------------------------------------------------
// Integer GetEstimationParameterSize(const Integer item)
//------------------------------------------------------------------------------
Integer ErrorModel::GetEstimationParameterSize(const Integer item)
{
   Integer retval = 1;


   // Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;
   Integer id = (item > ESTIMATION_TYPE_ALLOCATION ? 
      item - type * ESTIMATION_TYPE_ALLOCATION : item);

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("ErrorModel::GetEstimationParameterSize(%d)"
            " called; parameter ID is %d\n", item, id);
   #endif


   switch (id)
   {
      case BIAS:
         retval = 1;
         break;

      case PASS_BIASES:
         return passBiases.GetSize();
         break;

      // All other values call up the hierarchy
      default:
         retval = GmatBase::GetEstimationParameterSize(item);
   }

   return retval;
}

//------------------------------------------------------------------------------
// Real* GetEstimationParameterValue(const Integer item)
//------------------------------------------------------------------------------
/** Get real value of a estimation parameter
*
* @param item         parameter index which is needed to specify its value
*
* @return   a pointer to its real value
*/
//------------------------------------------------------------------------------
Real* ErrorModel::GetEstimationParameterValue(const Integer item)
{
   Real* retval = NULL;

   //Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;
   Integer id = (item > ESTIMATION_TYPE_ALLOCATION ? 
      item - type * ESTIMATION_TYPE_ALLOCATION : item);

   static Real* passBiasVals = NULL;

   switch (id)
   {
      case BIAS:
         retval = &bias;
         break;

      case PASS_BIASES:
         passBiasVals = (Real*)(passBiases.GetDataVector());
         break;

      // All other values call up the class heirarchy
      default:
         retval = GmatBase::GetEstimationParameterValue(item);
   }

   return retval;
}


//------------------------------------------------------------------------------
// StringArray GetAllAvailableTypes()
//------------------------------------------------------------------------------
/** Get a list of all available measurement types implemented in GMAT
*
* @return   a list of all available measurement types implemented in GMAT
*/
//------------------------------------------------------------------------------
StringArray ErrorModel::GetAllAvailableTypes()
{
   static StringArray sa;
   sa.clear();

   sa.push_back("DSN_SeqRange");
   sa.push_back("DSN_PNRange");
   sa.push_back("DSN_TCP");
   sa.push_back("GPS_PosVec");
   sa.push_back("Range");
   sa.push_back("Range_Skin");
   sa.push_back("RangeRate");
   sa.push_back("Azimuth");
   sa.push_back("Elevation");
   sa.push_back("XEast");
   sa.push_back("YNorth");
   sa.push_back("XSouth");
   sa.push_back("YEast");

   sa.push_back("SN_Range");
   sa.push_back("SN_Doppler");                                        // fix bug: GMT-5955
   sa.push_back("BRTS_Range");
   sa.push_back("BRTS_Doppler");
   sa.push_back("SN_Doppler_Rtn");
   sa.push_back("SN_DOWD");

   if (GmatGlobal::Instance()->InTestingMode())                                   // fix bug: GMT-5955
   {
      sa.push_back("RightAscension");
      sa.push_back("Declination");
   }

   return sa;
}


//-------------------------------------------------------------------------
// Integer Spacecraft::HasParameterCovariances(Integer parameterId)
//-------------------------------------------------------------------------
/**
* This function is used to verify whether a parameter (with ID specified by
* parameterId) having a covariance or not.
*
* @param parameterId      ID of a parameter
* @return                 size of covarian matrix associated with the parameter
*                         return -1 when the parameter has no covariance
*/
//-------------------------------------------------------------------------
Integer ErrorModel::HasParameterCovariances(Integer parameterId)
{
   if (parameterId == BIAS)
      return 1;

   if (parameterId == PASS_BIASES)
      return passBiases.GetSize();

   return GmatBase::HasParameterCovariances(parameterId);
}


//------------------------------------------------------------------------------
// Rmatrix* GetParameterCovariances(Integer parameterId)
//------------------------------------------------------------------------------
/**
* Get covariance matrix for a given parameter.
*
* @param parameterId      ID of a parameter
* @return                 a pointer to covariance matrix associated with the parameter
*                         return NULL when the parameter has no covariance
*/
//-------------------------------------------------------------------------
Rmatrix* ErrorModel::GetParameterCovariances(Integer parameterId)
{
   if (isInitialized)
      return covariance.GetCovariance(parameterId);
   else
      throw GmatBaseException("Error: cannot get '" + GetName() + "' measurement ErrorModel's covariance when it is not initialized.\n");
   return NULL;
}


//------------------------------------------------------------------------------
// std::vector<GmatTime*>  GetPassBiasStartEpoches()
//------------------------------------------------------------------------------
/**
* Get start epoches of all passes
*
* @return       a list of start epoches for all passes
*/
//-------------------------------------------------------------------------
std::vector<GmatTime*>  ErrorModel::GetPassBiasStartEpoches()
{ 
   return passBiasStartEpoches; 
}


//------------------------------------------------------------------------------
// std::string GetTimeRangeInUTCGregorian(Integer passNum)
//------------------------------------------------------------------------------
/**
* Get time range for a given bias' pass
* 
* @param passNum        Pass number at which we need to know time range
*
* @return     Time range for given pass in UTC Gregorian time
*/
//------------------------------------------------------------------------------
std::string  ErrorModel::GetTimeRangeInUTCGregorian(Integer passNum)
{
   if (passNum >= passBiasStartEpoches.size())
   {
      std::stringstream ss;
      ss << "Error: Pass number " << passNum << " is out of range in PassBiasStartEpoches array (array size is" << passBiasStartEpoches.size() << ").\n";
      throw GmatBaseException(ss.str());
   }

   TimeSystemConverter *tc = TimeSystemConverter::Instance();
   GmatTime epoch = tc->Convert(*passBiasStartEpoches[passNum], tc->A1MJD, tc->UTCMJD);
   std::string startepoch = tc->ConvertMjdToGregorian(epoch.GetMjd(), true);

   // Display time range for this pass bias
   std::stringstream ss;
   ss << "[" << startepoch << ", ";
   if (passNum < passBiasStartEpoches.size() - 1)
   {
      GmatTime epoch = tc->Convert(*passBiasStartEpoches[passNum + 1], tc->A1MJD, tc->UTCMJD);
      std::string endepoch = tc->ConvertMjdToGregorian(epoch.GetMjd(), true);
      ss << endepoch;
   }
   else
      ss << "inf";
   ss << ")";

   return ss.str();
}


//------------------------------------------------------------------------------
// std::string GetTimeRangeStartInUTCGregorian(Integer passNum)
//------------------------------------------------------------------------------
/**
* Get time range start for a given bias' pass
*
* @param passNum        Pass number at which we need to know time range
*
* @return     Time range start for given pass in UTC Gregorian time
*/
//------------------------------------------------------------------------------
std::string  ErrorModel::GetTimeRangeStartInUTCGregorian(Integer passNum)
{
   if (passNum >= passBiasStartEpoches.size())
   {
      std::stringstream ss;
      ss << "Error: Pass number " << passNum << " is out of range in PassBiasStartEpoches array (array size is" << passBiasStartEpoches.size() << ").\n";
      throw GmatBaseException(ss.str());
   }

   TimeSystemConverter *tc = TimeSystemConverter::Instance();
   GmatTime epoch = tc->Convert(*passBiasStartEpoches[passNum], tc->A1MJD, tc->UTCMJD);
   std::string startepoch = tc->ConvertMjdToGregorian(epoch.GetMjd(), true);

   return startepoch;
}


//------------------------------------------------------------------------------
// std::string GetTimeRangeEndInUTCGregorian(Integer passNum)
//------------------------------------------------------------------------------
/**
* Get time range end for a given bias' pass
*
* @param passNum        Pass number at which we need to know time range
*
* @return     Time range end for given pass in UTC Gregorian time
*/
//------------------------------------------------------------------------------
std::string  ErrorModel::GetTimeRangeEndInUTCGregorian(Integer passNum)
{
   if (passNum >= passBiasStartEpoches.size())
   {
      std::stringstream ss;
      ss << "Error: Pass number " << passNum << " is out of range in PassBiasStartEpoches array (array size is" << passBiasStartEpoches.size() << ").\n";
      throw GmatBaseException(ss.str());
   }

   if (passNum == passBiasStartEpoches.size() - 1)
      return "inf";

   TimeSystemConverter *tc = TimeSystemConverter::Instance();
   GmatTime epoch = tc->Convert(*passBiasStartEpoches[passNum+1], tc->A1MJD, tc->UTCMJD);
   std::string endepoch = tc->ConvertMjdToGregorian(epoch.GetMjd(), true);

   return endepoch;
}


//------------------------------------------------------------------------------
// void SetPassBiasStartEpoches(std::vector<GmatTime*> startEpoches)
//------------------------------------------------------------------------------
/**
* Set start epoches of all passes
*
* @param startEpoches    a list of start epoches for all passes
*/
//-------------------------------------------------------------------------
void ErrorModel::SetPassBiasStartEpoches(std::vector<GmatTime*> startEpoches)
{
   passBiasStartEpoches.clear();
   passBiasStartEpoches = startEpoches;
};


//------------------------------------------------------------------------------
// Integer GetBiasPassNumber(GmatTime currentTime)
//------------------------------------------------------------------------------
/**
* Get pass number at a given time
*
* @param currentTime     the time at which pass number is specified
*
* @return    pass number
*/
//-------------------------------------------------------------------------
Integer ErrorModel::GetBiasPassNumber(GmatTime currentTime)
{
   Integer passNum = passBiasStartEpoches.size() - 1;
   for (Integer i = 0; i < passBiasStartEpoches.size()-1; ++i)
   {
      if (currentTime < (*passBiasStartEpoches[1]))
      {
         passNum = 0;
         break;
      }
      else if (((*passBiasStartEpoches[i]) <= currentTime) && (currentTime < (*passBiasStartEpoches[i + 1])))
      {
         passNum = i;
         break;
      }
   }

   return passNum;
}


//------------------------------------------------------------------------------
// Real GetPassBias(GmatTime& currentTime)
//------------------------------------------------------------------------------
/**
* Get pass bias at a given time
*
* @param currentTime     the time at which pass bias is specified
*
* @return    pass bias
*/
//-------------------------------------------------------------------------
Real ErrorModel::GetPassBias(GmatTime currentTime)
{
   return passBiases[GetBiasPassNumber(currentTime)];
}


//------------------------------------------------------------------------------
// Real GetPassBias(const Integer passNum)
//------------------------------------------------------------------------------
/**
* Get pass bias at a given pass number
*
* @param passNum     the pass number at which pass bias is specified
*
* @return    pass bias
*/
//-------------------------------------------------------------------------
Real ErrorModel::GetPassBias(const Integer passNum)
{
   return passBiases[passNum];
}


