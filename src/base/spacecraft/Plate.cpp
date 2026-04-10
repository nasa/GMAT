//$Id$
//------------------------------------------------------------------------------
//                              Plate
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
// number NNG04CI63P
//
// Author: Tuan D. Nguyen, GSFC-NASA, Code 583.
// Created: 2019/05/28
//
/**
 * Implements Plate class. 
 */
//------------------------------------------------------------------------------


#include "Plate.hpp"
#include "MessageInterface.hpp"
#include "Rmatrix.hpp"
#include "Rmatrix33.hpp"
#include "CoordinateConverter.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "InterfaceException.hpp"
#include "StringUtil.hpp"
#include <sstream>

//#define DEBUG_PLATE
//#define DEBUG_RENAME
//#define DEBUG_REFLECTANCE
//#define DEBUG_REFLECTANCE_CALCULATION
//#define DEBUG_SHATI_DERIV
//#define DEBUG_REFLECTANCE_DERIVATIVE
//#define DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION


#define EPSILON      1.0e-10

//---------------------------------
// static data
//---------------------------------

const std::string Plate::PARAMETER_TEXT[PlateParamCount -
                                              GmatBaseParamCount] =
{
   "Type",                      //TYPE_ID
   "PlateNormalHistoryFile",    //PLATE_NORMAL_HISTORY_FILE_ID
   "PlateNormal",               //PLATE_NORMAL_ID
   "Area",                      //AREA_ID
   "AreaCoefficient",           //AREA_COEFFICIENT_ID
   "AreaCoefficientSigma",      //AREA_COEFFICIENT_SIGMA_ID
   "LitFraction",               //LIT_FRACTION_ID
   "SpecularFraction",          //SPECULAR_FRACTION_ID
   "SpecularFractionSigma",     //SPECULAR_FRACTION_SIGMA_ID
   "DiffuseFraction",           //DIFFUSE_FRACTION_ID
   "DiffuseFractionSigma",      //DIFFUSE_FRACTION_SIGMA_ID
   "SolveFors",                 //SOLVEFORS_ID

   //"PlatePosition",             //PLATE_POSITION
   "PlateX",                    //PLATEX
   "PlateY",                    //PLATEY
   "PlateZ",                    //PLATEZ
};


const Gmat::ParameterType Plate::PARAMETER_TYPE[PlateParamCount -
                                                      GmatBaseParamCount] =
{
   Gmat::STRING_TYPE,           //"Type",                      //TYPE_ID
   Gmat::STRING_TYPE,           //"PlateNormalHistoryFile",    //PLATE_NORMAL_HISTORY_FILE_ID
   Gmat::RVECTOR_TYPE,          //"PlateNormal",               //PLATE_NORMAL_ID
   Gmat::REAL_TYPE,             //"Area",                      //AREA_ID
   Gmat::REAL_TYPE,             //"AreaCoefficient",           //AREA_COEFFICIENT_ID
   Gmat::REAL_TYPE,             //"AreaCoefficientSigma",      //AREA_COEFFICIENT_SIGMA_ID
   Gmat::REAL_TYPE,             //"LitFraction",               //LIT_FRACTION_ID
   Gmat::REAL_TYPE,             //"SpecularFraction",          //SPECULAR_FRACTION_ID
   Gmat::REAL_TYPE,             //"SpecularFractionSigma",     //SPECULAR_FRACTION_SIGMA_ID
   Gmat::REAL_TYPE,             //"DiffuseFraction",           //DIFFUSE_FRACTION_ID
   Gmat::REAL_TYPE,             //"DiffuseFractionSigma",      //DIFFUSE_FRACTION_SIGMA_ID
   Gmat::STRINGARRAY_TYPE,      //"SolveFors"                  //SOLVEFORS_ID

   //Gmat::RVECTOR_TYPE,          //"PlatePosition",             //PLATE_POSITION
   Gmat::REAL_TYPE,            //PlateX                       //PLATEX
   Gmat::REAL_TYPE,            //PlateY                       //PLATEY
   Gmat::REAL_TYPE,            //PlateZ                       //PLATEZ
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Plate(UnsignedInt typeId, const std::string &typeStr,
//             const std::string &instName)
//------------------------------------------------------------------------------
/**
 * Default constructor.
 *
 * @param <typeId>   UnsignedInt of the constructed object.
 * @param <typeStr>  String describing the type of object created.
 * @param <instName> Name of the constructed instance.
 */
//------------------------------------------------------------------------------
Plate::Plate(UnsignedInt typeId, const std::string &typeStr,
                         const std::string &instName) :
   GmatBase          (typeId, typeStr, instName),
   plateType         ("SunFacing"),
   historyFileName   (""),
   faceNormalFile    (NULL),
   plateArea         (1.0),                       // unit: m
   areaCoeff         (1.0),
   areaCoeffSigma    (1.0e70),
   litFrac           (1.0),
   specularFrac      (1.0),
   specularFracSigma (1.0e70),
   diffuseFrac       (0.0),
   diffuseFracSigma  (1.0e70),
   errorSelection    (true),
   runningCommandFlag (0)                       // initially it is set to 0: not running any command
{
   objectTypes.push_back(Gmat::PLATE);
   objectTypeNames.push_back("Plate");
   parameterCount = PlateParamCount;

   plateNormal.Set(1.0, 0.0, 0.0);

   allowedSolveFors.push_back("AreaCoefficient");
   allowedSolveFors.push_back("SpecularFraction");
   allowedSolveFors.push_back("DiffuseFraction");

   scInertialCS = NULL;

   location[0] = 0.0;
   location[1] = 0.0;
   location[2] = 0.0;
}


//------------------------------------------------------------------------------
// ~Plate()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Plate::~Plate()
{
   if (faceNormalFile)
      delete faceNormalFile;
}


//------------------------------------------------------------------------------
// Plate(const Plate& pl)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <pl>   Plate object that is copied onto this one.
 */
//------------------------------------------------------------------------------
Plate::Plate(const Plate& pl) :
   GmatBase          (pl),
   plateType         (pl.plateType),
   historyFileName   (pl.historyFileName),
   plateNormal       (pl.plateNormal),
   plateArea         (pl.plateArea),
   areaCoeff         (pl.areaCoeff),
   areaCoeffSigma    (pl.areaCoeffSigma),
   litFrac           (pl.litFrac),
   specularFrac      (pl.specularFrac),
   specularFracSigma (pl.specularFracSigma),
   diffuseFrac       (pl.diffuseFrac),
   diffuseFracSigma  (pl.diffuseFracSigma),
   allowedSolveFors  (pl.allowedSolveFors),
   errorSelection    (pl.errorSelection),
   runningCommandFlag (pl.runningCommandFlag)
{
   if (pl.faceNormalFile)
      faceNormalFile = pl.faceNormalFile->Clone();
   else
      faceNormalFile = NULL;
   
   scInertialCS = pl.scInertialCS;

   location[0] = pl.location[0];
   location[1] = pl.location[1];
   location[2] = pl.location[2];
}


//------------------------------------------------------------------------------
// Plate& operator=(const Plate& so)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <pl> Plate object that is copied onto this one.
 *
 * @return this instance, configured like the input instance.
 */
//------------------------------------------------------------------------------
Plate& Plate::operator=(const Plate& pl)
{
   if (this == &pl)
      return *this;
      
   GmatBase::operator=(pl);

   plateType         = pl.plateType;
   historyFileName   = pl.historyFileName;
   plateNormal       = pl.plateNormal;
   plateArea         = pl.plateArea;
   areaCoeff         = pl.areaCoeff;
   areaCoeffSigma    = pl.areaCoeffSigma;
   litFrac           = pl.litFrac;
   specularFrac      = pl.specularFrac;
   specularFracSigma = pl.specularFracSigma;
   diffuseFrac       = pl.diffuseFrac;
   diffuseFracSigma  = pl.diffuseFracSigma;
   allowedSolveFors = pl.allowedSolveFors;

   if (faceNormalFile)
      delete faceNormalFile;
   if (pl.faceNormalFile)
      faceNormalFile = pl.faceNormalFile->Clone();
   else
      faceNormalFile = NULL;

   errorSelection = pl.errorSelection;
   runningCommandFlag = pl.runningCommandFlag;

   location[0] = pl.location[0];
   location[1] = pl.location[1];
   location[2] = pl.location[2];

   return *this;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
* This method returns a clone of the Plate.
*
* @return clone of the Plate.
*
*/
//------------------------------------------------------------------------------
GmatBase* Plate::Clone() const
{
   Plate *clone = new Plate(*this);

#ifdef DEBUG_PLATE
   MessageInterface::ShowMessage
   ("Plate::Clone() cloned <%p>'%s' to <%p>'%s'\n", this,
      instanceName.c_str(), clone, clone->GetName().c_str());
#endif

   return clone;
}

void Plate::Copy(const GmatBase* plate)
{
   operator=(*((Plate*)plate));
}


bool Plate::Initialize()
{
   if (!IsInitialized())
   {
      if (GmatMathUtil::Abs(diffuseFrac + specularFrac) > 1.0)
      {
         std::stringstream ss;
         ss << "Warning: SpecularFraction + DiffuseFraction = " << specularFrac << " + " << diffuseFrac << " > 1.0.\n";
         MessageInterface::ShowMessage(ss.str());
      }

      if (plateNormal.IsZeroVector())
      {
         std::stringstream ss;
         ss << "Error: Zero vector was set to " << GetName() << ".PlateNormal parameter. The value of this parameter has to be a non-zero vector.\n";
         throw GmatBaseException(ss.str());

      }

      plateNormal = plateNormal / plateNormal.GetMagnitude();

      ///@todo: add initialize code here
      if (plateType == "File")
      {
         //Read face normal history data from file
         if (faceNormalFile)
            delete faceNormalFile;

         faceNormalFile = new NPlateHistoryFileReader();
         faceNormalFile->SetFile(historyFileName);
         faceNormalFile->Initialize();
      }
      else
         faceNormalFile = NULL;

      // Set value for Plate's covariance
      for (Integer i = 0; i < solveForList.size(); ++i)
      {
         std::string sfName = solveForList[i];
         Integer index = covariance.GetElementIndex(sfName);
         if (index == -1)
            covariance.AddCovarianceElement(sfName, this);
         Integer startLocation = covariance.GetSubMatrixLocationStart(sfName);

         Real noiseSigma;
         if (sfName == "AreaCoefficient")
            noiseSigma = GetRealParameter("AreaCoefficientSigma");
         else if (sfName == "SpecularFraction")
            noiseSigma = GetRealParameter("SpecularFractionSigma");
         else if (sfName == "DiffuseFraction")
            noiseSigma = GetRealParameter("DiffuseFractionSigma");

         covariance(startLocation, startLocation) = noiseSigma * noiseSigma;
      }
   }

   isInitialized = true;
   return isInitialized;
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer Plate::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < PlateParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }

   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the parameter text, given the input parameter ID.
*
* @param <id> Id for the requested parameter text.
*
* @return parameter text for the requested parameter.
*/
//------------------------------------------------------------------------------
std::string Plate::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < PlateParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the parameter type, given the input parameter ID.
*
* @param <id> ID for the requested parameter.
*
* @return parameter type of the requested parameter.
*/
//------------------------------------------------------------------------------
Gmat::ParameterType Plate::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < PlateParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the parameter type string, given the input parameter ID.
*
* @param <id> ID for the requested parameter.
*
* @return parameter type string of the requested parameter.
*/
//------------------------------------------------------------------------------
std::string Plate::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


std::string Plate::GetStringParameter(const Integer id) const
{
   if (id == TYPE_ID)
      return plateType;
   if (id == PLATE_NORMAL_HISTORY_FILE_ID)
      return historyFileName;

   return GmatBase::GetStringParameter(id);
}


bool Plate::SetStringParameter(const Integer id, const char *value)
{
   std::string s = "";
   s.append(value);
   
   return SetStringParameter(id, s);
}

bool Plate::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == TYPE_ID)
   {
      if ((value != "FixedInBody") && (value != "SunFacing") && (value != "File"))
         throw GmatBaseException("Error: " + GetName() + ".Type parameter has invalid value ('" + value + "'). Allow value has to be 'FixedInBody', 'SunFacing', or 'File'.\n");

      plateType = value;
      return true;
   }

   if (id == PLATE_NORMAL_HISTORY_FILE_ID)
   {
      // verify a valid file name
      Integer error;
      if (!GmatStringUtil::IsValidFullFileName(value, error))
         throw InterfaceException("Error: '" + value + "' set to " + GetName() + ".PlateNormalHistoryFile is an invalid file name.\n");

      historyFileName = value; 
      return true;
   }

   if (id == SOLVEFORS_ID)
   {
      if (find(allowedSolveFors.begin(), allowedSolveFors.end(), value) == allowedSolveFors.end())
      {
         std::stringstream ss;
         ss << "Error: An invalid value ('" << value << "') was set to " << GetName() << ".SolveFors parameter\n";
         ss << "Allowed value(s) are ";
         for (Integer i = 0; i < allowedSolveFors.size(); ++i)
         {
            ss << " " << allowedSolveFors[i];
         }
         ss << "\n";
         throw GmatBaseException(ss.str());
      }
      solveForList.push_back(value);
      covariance.AddCovarianceElement(value, this);

      // Add the element to Plate's covariance
      Integer startlocation = -1;
      try
      {
         startlocation = covariance.GetSubMatrixLocationStart(value);
      }
      catch (...)
      {
         covariance.AddCovarianceElement(value, this);
         startlocation = covariance.GetSubMatrixLocationStart(value);
      }
      return true;
   }

   return GmatBase::SetStringParameter(id, value);
}


std::string Plate::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


bool  Plate::SetStringParameter(const std::string &label, const char *value)
{
   return SetStringParameter(GetParameterID(label), value);
}


bool Plate::SetStringParameter(const std::string &label, const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


std::string Plate::GetStringParameter(const Integer id, const Integer index) const
{
   if (id == SOLVEFORS_ID)
   {
      if ((index < 0) || (index >= solveForList.size()))
      {
         std::stringstream ss;
         ss << "Error: index out of bound when getting " << GetName() << ".SolveFors element index " << index << "\n";
         throw GmatBaseException(ss.str());
      }

      return solveForList[index];
   }

   return GmatBase::GetStringParameter(id, index);
}


bool Plate::SetStringParameter(const Integer id, const std::string &value, const Integer index)
{
   if (id == SOLVEFORS_ID)
   {
      if ((index < 0) || (index > solveForList.size()))
      {
         std::stringstream ss;
         ss << "Error: index out of bound when getting " << GetName() << ".SolveFors element index " << index << "\n";
         throw GmatBaseException(ss.str());
      }
      else if (find(allowedSolveFors.begin(), allowedSolveFors.end(), value) == allowedSolveFors.end())
      {
         std::stringstream ss;
         ss << "Error: An invalid value ('" << value << "') was set to " << GetName() << ".SolveFors parameter\n";
         ss << "Allowed value(s) are ";
         for (Integer i = 0; i < allowedSolveFors.size(); ++i)
         {
            ss << " " << allowedSolveFors[i];
         }
         ss << "\n";
         throw GmatBaseException(ss.str());
      }
      else
      {
         /// Turn off debug messages
         // MessageInterface::ShowMessage("Set '%s' to SolveFors\n", value.c_str());
         // MessageInterface::ShowMessage("List of allowed name for SolveFors\n");
         // for (Integer i = 0; i < allowedSolveFors.size(); ++i)
         //    MessageInterface::ShowMessage("%d: <%s>\n", i, allowedSolveFors[i].c_str());

         if (index == solveForList.size())
            solveForList.push_back(value);
         else
            solveForList[index] = value;

         // Add the element to Plate's covariance
         Integer startlocation = -1;
         try
         {
            startlocation = covariance.GetSubMatrixLocationStart(value);
         }
         catch (...)
         {
            covariance.AddCovarianceElement(value, this);
            startlocation = covariance.GetSubMatrixLocationStart(value);
         }
      }

      return true;
   }

   return GmatBase::SetStringParameter(id, value, index);

}


const StringArray& Plate::GetStringArrayParameter(const Integer id) const
{
   if (id == SOLVEFORS_ID)
   {
      static StringArray emptyList;
      if ((runningCommandFlag == 1) || (runningCommandFlag == 2))
      {
         // when the plate is used for running Simulation command or 
         // Propagation command, those command do not calculate partial 
         // derivative of variables in Plate.Solvefors. Therefore we return an empty list of solve-for
         return emptyList;
      }
      else
         return solveForList;
   }
   return GmatBase::GetStringArrayParameter(id);
}


std::string Plate::GetStringParameter(const std::string &label, const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


bool Plate::SetStringParameter(const std::string &label, const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


const StringArray& Plate::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


const Rvector& Plate::GetRvectorParameter(const Integer id) const
{
   return plateNormal;
}


const Rvector& Plate::GetRvectorParameter(const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
}


const Rvector& Plate::SetRvectorParameter(const Integer id, const Rvector &value)
{
   if (value.GetSize() != 3)
   {
      std::stringstream ss;
      ss << "Error: a vector with size " << value.GetSize() << " was set to " << GetName() << ".PlateNormal parameter.\n";
      throw GmatBaseException(ss.str());
   }

   if (value.IsZeroVector())
   {
      std::stringstream ss;
      ss << "Error: Zero vector was set to " << GetName() << ".PlateNormal parameter. The value of this parameter has to be a non-zero vector.\n";
      throw GmatBaseException(ss.str());
   }

   plateNormal[0] = value[0];
   plateNormal[1] = value[1];
   plateNormal[2] = value[2];

   return plateNormal;
}


const Rvector& Plate::SetRvectorParameter(const std::string &label, const Rvector &value)
{
   return SetRvectorParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter value.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real Plate::GetRealParameter(const Integer id) const
{
   if (id == AREA_ID)
      return plateArea;
   
   if (id == AREA_COEFFICIENT_ID)
      return areaCoeff;
   if (id == AREA_COEFFICIENT_SIGMA_ID)
      return areaCoeffSigma;
   
   if (id == LIT_FRACTION_ID)
      return litFrac;

   if (id == SPECULAR_FRACTION_ID)
      return specularFrac;
   if (id == SPECULAR_FRACTION_SIGMA_ID)
      return specularFracSigma;

   if (id == DIFFUSE_FRACTION_ID)
      return diffuseFrac;
   if (id == DIFFUSE_FRACTION_SIGMA_ID)
      return diffuseFracSigma;

   if (id == PLATEX)
      return location[0];

   if (id == PLATEY)
      return location[1];

   if (id == PLATEZ)
      return location[2];

   return GmatBase::GetRealParameter(id);
}


//------------------------------------------------------------------------------
//  Real  GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <label> String description for the requested parameter value.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real Plate::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param <id>    ID for the parameter whose value to change.
 * @param <value> value for the parameter.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real Plate::SetRealParameter(const Integer id, const Real value)
{
   if (id == AREA_ID)
   {
      if (value <= 0.0)
      {
         std::stringstream ss;
         ss << "Error: GMAT does not allow to set a non positive number (" << value << ") to " << GetName() << ".Area parameter.\n";
         throw GmatBaseException(ss.str());
      }

      plateArea = value;
      return plateArea;
   }

   if (id == AREA_COEFFICIENT_ID)
   {
      if (value <= 0.0)
      {
         std::stringstream ss;
         ss << "A non positive value (" << value << ") was set to " << GetName() << ".AreaCoefficient parameter.\n";
         if (errorSelection)
            throw GmatBaseException("Error: " + ss.str());
         else
            MessageInterface::ShowMessage("Warning: " + ss.str());
      }

      areaCoeff = value;
      return areaCoeff;
   }
   if (id == AREA_COEFFICIENT_SIGMA_ID)
   {
      if (value <= 0.0)
      {
         std::stringstream ss;
         ss << "Error: GMAT does not allow to set a non positive number (" << value << ") to " << GetName() << ".AreaCoefficientSigma parameter.\n";
         throw GmatBaseException(ss.str());
      }

      areaCoeffSigma = value; 
      return areaCoeffSigma;
   }

   if (id == LIT_FRACTION_ID)
   {
      if ((value <= 0.0)|| (value > 1.0))
      {
         std::stringstream ss;
         ss << "Error: GMAT does not allow a value out of range (0 , 1] set to " << GetName() << ".LitFraction parameter.\n";
         throw GmatBaseException(ss.str());
      }

      litFrac = value;
      return litFrac;
   }

   if (id == SPECULAR_FRACTION_ID)
   {
      if ((value < 0.0) || (value > 1.0))
      {
         std::stringstream ss;
         ss << "A value outside of range [0 , 1] was set to " << GetName() << ".SpecularFraction parameter.\n";
         if (errorSelection)
            throw GmatBaseException("Error: " + ss.str());
         else
            MessageInterface::ShowMessage("Warning: " + ss.str());
      }

      if (IsInitialized())
      {
         if (GmatMathUtil::Abs(diffuseFrac + value) > 1.0)
         {
            std::stringstream ss;
            ss << "Warning: " << value << " set to SpecularFraction causes total SpecularFraction + DiffuseFraction = " << value << " + " << diffuseFrac << " > 1.0.\n"
               << " It needs to set another value in order to SpecularFraction + DiffuseFraction < 1.0\n";
            MessageInterface::ShowMessage(ss.str());
         }
      }

      specularFrac = value;
      return specularFrac;
   }

   if (id == SPECULAR_FRACTION_SIGMA_ID)
   {
      if (value <= 0.0)
      {
         std::stringstream ss;
         ss << "Error: GMAT does not allow to set a non positive number (" << value << ") to " << GetName() << ".SpecularFractionSigma parameter.\n";
         throw GmatBaseException(ss.str());
      }

      specularFracSigma = value;
      return specularFracSigma;
   }

   if (id == DIFFUSE_FRACTION_ID)
   {
      if ((value < 0.0) || (value > 1.0))
      {
         std::stringstream ss;
         ss <<"A value outside of range [0 , 1] was set to " << GetName() << ".DiffuseFraction parameter.\n";
         if (errorSelection)
            throw GmatBaseException("Error: " + ss.str());
         else
            MessageInterface::ShowMessage("Warning: " + ss.str());
      }

      if (IsInitialized())
      {
         if (GmatMathUtil::Abs(specularFrac + value) > 1.0)
         {
            std::stringstream ss;
            ss << "Warning: " << value << " set to DiffuseFraction causes total SpecularFraction + DiffuseFraction = " << specularFrac << " + " << value << " > 1.0.\n"
               << " It needs to set another value in order to SpecularFraction + DiffuseFraction < 1.0\n";
            MessageInterface::ShowMessage(ss.str());
         }
      }

      diffuseFrac = value;
      return diffuseFrac;
   }

   if (id == DIFFUSE_FRACTION_SIGMA_ID)
   {
      if (value <= 0.0)
      {
         std::stringstream ss;
         ss << "Error: GMAT does not allow to set a non positive number (" << value << ") to " << GetName() << ".DiffuseFractionSigma parameter.\n";
         throw GmatBaseException(ss.str());
      }

      diffuseFracSigma = value;
      return diffuseFracSigma;
   }

   if (id == PLATEX)
   {
      location[0] = value;
      return location[0];
   }

   if (id == PLATEY)
   {
      location[1] = value;
      return location[1];
   }

   if (id == PLATEZ)
   {
      //if (value <= 0.0)
      //{
      //   std::stringstream ss;
      //   ss << "Error: GMAT does not allow to set a non positive number (" << value << ") to " << GetName() << ".PlateZ parameter.\n";
      //   throw GmatBaseException(ss.str());
      //}
      location[2] = value;
      return location[2];
   }

   return GmatBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
//  Real  SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param <label> String description for the parameter value.
 * @param <value> value for the parameter.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real Plate::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value, Integer index)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real Plate::SetRealParameter(const Integer id, const Real value, const Integer index)
{
   if (id == PLATE_NORMAL_ID)
   {
      if ((index < 0) || (index > 2))
      {
         std::stringstream ss;
         ss << "Error: Index is out of bound when setting value of " << GetName() << ".PlateNormal parameter.\n";
         throw GmatBaseException(ss.str());
      }

      plateNormal[index] = value;
      return plateNormal[index];
   }

   return GmatBase::SetRealParameter(id, value, index);
}

Rvector3 Plate::GetLocation() const
{
   return Rvector3(location[0],
                   location[1],
                   location[2]);
}

//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool Plate::RenameRefObject(const UnsignedInt type,
                            const std::string &oldName,
                            const std::string &newName)
{
#ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
   ("Plate::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
      GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
#endif

   return true;
}


//------------------------------------------------------------------------------
// bool HasLocalClones()
//------------------------------------------------------------------------------
/**
* Method to check for the presence of local clones
*
* @return true if there are local clones, false if not
*/
//------------------------------------------------------------------------------
bool Plate::HasLocalClones()
{
   return true;
}


//------------------------------------------------------------------------------
// void UpdateClonedObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
* Added method to remove message in the Message window.
*
* The current implementation needs to be updated to actually process parameters
* when they are updated in the system.  For now, it is just overriding the base
* class "do nothing" method so that the message traffic is not shown to the
* user.
*
* Turn on the debug to figure out the updates being requested.
*
* @param obj The master object holding the new parameter value
*/
//------------------------------------------------------------------------------
void Plate::UpdateClonedObject(GmatBase *obj)
{
}


//------------------------------------------------------------------------------
// void UpdateClonedObjectParameter(GmatBase *obj, Integer updatedParameterId)
//------------------------------------------------------------------------------
/**
* Added method to remove message in the Message window.
*
* The current implementation needs to be updated to actually process parameters
* when they are updated in the system.  For now, it is just overriding the base
* class "do nothing" method so that the message traffic is not shown to the
* user.
*
* Turn on the debug to figure out the updates being requested.
*
* @param obj The master object holding the new parameter value
* @param updatedParameterId The ID of the updated parameter
*/
//------------------------------------------------------------------------------
void Plate::UpdateClonedObjectParameter(GmatBase *obj, Integer updatedParameterId)
{
}


//-------------------------------------------------------------------------
// Integer Plate::HasParameterCovariances(Integer parameterId)
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
Integer Plate::HasParameterCovariances(Integer parameterId)
{
   //if (parameterId == AREA_ID)
   //   return 1;

   if (parameterId == AREA_COEFFICIENT_ID)
      return 1;

   //if (parameterId == LIT_FRACTION_ID)
   //   return 1;

   if (parameterId == SPECULAR_FRACTION_ID)
      return 1;

   if (parameterId == DIFFUSE_FRACTION_ID)
      return 1;

   return GmatBase::HasParameterCovariances(parameterId);
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
bool Plate::IsEstimationParameterValid(const Integer item)
{
   bool retval = false;

   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;    // convert Estimation ID to object parameter ID

   switch (id)
   {
   ////case AREA_ID:                  // Area parameter is not a solve-for
   ////   retval = true;
   ////   break;
   case AREA_COEFFICIENT_ID:
      retval = true;
      break;
   ////case LIT_FRACTION_ID:         // LiFraction parameter is not a solve-for
   ////   retval = true;
   ////   break;
   case SPECULAR_FRACTION_ID:
      retval = true;
      break;
   case DIFFUSE_FRACTION_ID:
      retval = true;
      break;
   default:
      // All other values call up the hierarchy
      retval = GmatBase::IsEstimationParameterValid(item);
   }

   return retval;
}


//------------------------------------------------------------------------------
// Integer GetEstimationParameterSize(const Integer item)
//------------------------------------------------------------------------------
Integer Plate::GetEstimationParameterSize(const Integer item)
{
   Integer retval = 0;

   Integer id = (item > ESTIMATION_TYPE_ALLOCATION ?
      item - type * ESTIMATION_TYPE_ALLOCATION : item);

#ifdef DEBUG_ESTIMATION
   MessageInterface::ShowMessage("Plate::GetEstimationParameterSize(%d)"
      " called; parameter ID is %d\n", item, id);
#endif

   switch (id)
   {
   ////case AREA_ID:
   ////   return 1;
   ////   break;
   case AREA_COEFFICIENT_ID:
      return 1;
      break;
   ////case LIT_FRACTION_ID:
   ////   return 1;
   ////   break;
   case SPECULAR_FRACTION_ID:
      return 1;
      break;
   case DIFFUSE_FRACTION_ID:
      return 1;
      break;
   default:
      // All other values call up the hierarchy
      return 0;
      break;
   }
}


//-----------------------------------------------------------------------------
// Rvector3 GetReflectanceI(Rvector3 &sHatI, GmatTime &epochGT, Rmatrix33 &MT)
//-----------------------------------------------------------------------------
/**
* Get reflectance vector in the spacecraft's Inertial frame
*
* @param sHatI   the Sun unit vector presenting in the spacecraft's inertial frame
* @param epochGT time need to specify nHat in NPlate history file
* @param MT      rotation matrix from spacecraft's body fixed frame to its inertial frame.
*                It needs for computing face normal unit vector for case using history data file.
*
* @return        plate's reflectance vector in the spacecraft's inertial frame (I-frame)
*/
//-----------------------------------------------------------------------------
Rvector3 Plate::GetReflectanceI(Rvector3 &sHatI, GmatTime &epochGT, Rmatrix33 &MT)
{
#ifdef DEBUG_REFLECTANCE
   MessageInterface::ShowMessage("Start Plate::GetReflectanceI()\n");
#endif

#ifdef DEBUG_REFLECTANCE_CALCULATION
   MessageInterface::ShowMessage("  sHatI = [%.16le  %.16le  %.16le]\n", sHatI[0], sHatI[1], sHatI[2]);
#endif

   // nHat in spacecraft's attitude frame
   Rvector3 nHatI;
   if (plateType == "FixedInBody")
      nHatI = MT*plateNormal;                   // plateNormal is constant vector in spacecraft's attitude frame (B-frame)
   else if (plateType == "SunFacing")
      nHatI = sHatI;                            // nHatI points to the Sun
   else if (plateType == "File")
   {
      // nHat is varied as defined w.r.t. time in NPlate history file
      nHatI = GetFaceNormalI(epochGT, MT);
#ifdef DEBUG_REFLECTANCE_CALCULATION
      MessageInterface::ShowMessage("Plate <%s>: FaceNormal = [%le  %le  %le]\n", GetName().c_str(), nHatI[0], nHatI[1], nHatI[2]);
#endif
   }

   // Specify A, rho, and delta
   Real A = areaCoeff * plateArea * litFrac;
   Real rho = specularFrac;
   Real delta = diffuseFrac;

   Real D = sHatI * nHatI;                                                // Eq.25 SRP N-Plates MathSpec
   Rvector3 C = (1.0 - rho)*sHatI + 2.0 * (delta / 3.0 + rho * D)*nHatI;  // Eq.26 SRP N-Plates MathSpec
   Rvector3 reflectance(0.0, 0.0, 0.0);
   if (D > EPSILON)
      reflectance = A * C*D;                                              // Eq.20 or Eq.27 SRP N-Plates MathSpec

#ifdef DEBUG_REFLECTANCE_CALCULATION
   MessageInterface::ShowMessage("   rho = %lf\n", rho);
   MessageInterface::ShowMessage("   delta = %lf\n", delta);
   MessageInterface::ShowMessage("   A = %lf\n", A);
   MessageInterface::ShowMessage("   D = %lf\n", D);
   MessageInterface::ShowMessage("   C = [%lf  %lf  %lf]\n", C[0], C[1], C[2]);
   MessageInterface::ShowMessage("   reflectance = [%lf  %lf  %lf]\n", reflectance[0], reflectance[1], reflectance[2]);
#endif

#ifdef DEBUG_REFLECTANCE
   MessageInterface::ShowMessage("End Plate::GetReflectanceI()\n");
#endif

   return reflectance;
}


//-----------------------------------------------------------------------------
// Rvector3 GetReflectance(Rvector3 &sHat, GmatTime &epochGT)
//-----------------------------------------------------------------------------
/**
* Get reflectance vector in the spacecraft's body fixed frame as shown in
* Eq.18 in SRP N_Plates MathSpec
*
* @param sHat    the Sun unit vector presenting in the spacecraft's
*                boby fixed frame
* @param epochGT time need to specify nHat in NPlate history file
* @param MT      rotation matrix from spacecraft's body fixed frame to its inertial frame. 
*                It needs for computing face normal unit vector for case using history data file.  
*
* @return        plate's reflectance vector in the spacecraft's attitude frame (B-frame)
*/
//-----------------------------------------------------------------------------
Rvector3 Plate::GetReflectance(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT)
{
#ifdef DEBUG_REFLECTANCE
   MessageInterface::ShowMessage("Start Plate::GetReflectance()\n");
#endif

#ifdef DEBUG_REFLECTANCE_CALCULATION
   MessageInterface::ShowMessage("  sHat = [%.16le  %.16le  %.16le]\n", sHat[0], sHat[1], sHat[2]);
#endif

   // nHat in spacecraft's attitude frame
   Rvector3 nHat;
   if (plateType == "FixedInBody")
      nHat = plateNormal;                       // nHat is constant vector in spacecraft's attitude frame
   else if (plateType == "SunFacing")
      nHat = sHat;                              // nHat points to the Sun
   else if (plateType == "File")
   {
      // nHat is varied as defined w.r.t. time in NPlate history file
      nHat = GetFaceNormal(epochGT, MT);
#ifdef DEBUG_REFLECTANCE_CALCULATION
      MessageInterface::ShowMessage("Plate <%s>: FaceNormal = [%le  %le  %le]\n", GetName().c_str(), nHat[0], nHat[1], nHat[2]);
#endif
   }

   // Specify A, rho, and delta
   Real A = areaCoeff * plateArea * litFrac;
   Real rho = specularFrac;
   Real delta = diffuseFrac;

   Real D = sHat * nHat;                                          // Eq.25 SRP N-Plates MathSpec
   Rvector3 C = (1.0 - rho)*sHat + 2.0 * (delta / 3.0 + rho * D)*nHat;  // Eq.26 SRP N-Plates MathSpec
   Rvector3 reflectance(0.0, 0.0, 0.0);
   if (D > EPSILON)
      reflectance = A*C*D;                                        // Eq.20 or Eq.27 SRP N-Plates MathSpec

#ifdef DEBUG_REFLECTANCE_CALCULATION
   MessageInterface::ShowMessage("   rho = %lf\n", rho);
   MessageInterface::ShowMessage("   delta = %lf\n", delta);
   MessageInterface::ShowMessage("   A = %lf\n", A);
   MessageInterface::ShowMessage("   D = %lf\n", D);
   MessageInterface::ShowMessage("   C = [%lf  %lf  %lf]\n", C[0], C[1], C[2]);
   MessageInterface::ShowMessage("   reflectance = [%lf  %lf  %lf]\n", reflectance[0], reflectance[1], reflectance[2]);
#endif

#ifdef DEBUG_REFLECTANCE
   MessageInterface::ShowMessage("End Plate::GetReflectance()\n");
#endif

   return reflectance;
}



//-----------------------------------------------------------------------------
//std::vector<Rvector3> Plate::GetReflectanceDerivativeI(const Rvector3 &sunSC,
//   Rmatrix33 &MT, std::vector<Rmatrix33> &dMT, GmatTime &epochGT)
//-----------------------------------------------------------------------------
/**
* Get derivative of reflectance in the spacecraft's I-frame
*
* @param sunSC   vector from the Sun to the spacecraft in inertial frame
* @param MT      rotation matrix from spacecraft's body fixed frame to its inertial frame.
*                It needs for computing face normal unit vector for case using history data file.
*
* @return        plate's reflectance derivative in the spacecraft's inertial frame (I-frame)
*/
//-----------------------------------------------------------------------------
std::vector<Rvector3> Plate::GetReflectanceDerivativeI(const Rvector3 &sunSC,
   Rmatrix33 &MT, std::vector<Rmatrix33> &dMT, GmatTime &epochGT)
{
#ifdef DEBUG_REFLECTANCE_DERIVATIVE
   MessageInterface::ShowMessage("Start Plate::GetReflectanceDerivativeI()\n");
#endif
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("  sunSC = [%lf  %lf  %lf]\n", sunSC[0], sunSC[1], sunSC[2]);
#endif

   // 1. Calculate derivative w.r.t. spacecraft state in I-frame
   std::vector<Rvector3> derivativeI =
      GetReflectanceDerivativeWRTSpacecraftStateI(sunSC, MT, dMT, epochGT);
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("derivativeI.size() = %d\n", derivativeI.size());
#endif

   // 2. Calculate derivative w.r.t. solve-for parameters in I-frame
   // 2.1. Specify sHatI
   Rvector3 sHatI = (-sunSC).GetUnitVector();

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("sHatI = [%lf  %lf  %lf]\n", sHatI[0], sHatI[1], sHatI[2]);
#endif

   // 2.3. Specify derivative w.r.t. solvefors
   Rvector3 deriv;
   for (Integer i = 0; i < solveForList.size(); ++i)
   {
      // Specify derivative w.r.t. solve-for in I-frame
      if (solveForList[i] == "Area")
         deriv = GetReflectanceDerivativeWRTPlateAreaI(sHatI, epochGT, MT);
      else if (solveForList[i] == "AreaCoefficient")
         deriv = GetReflectanceDerivativeWRTAreaCoeffI(sHatI, epochGT, MT);
      else if (solveForList[i] == "LitFraction")
         deriv = GetReflectanceDerivativeWRTLitFracI(sHatI, epochGT, MT);
      else if (solveForList[i] == "SpecularFraction")
         deriv = GetReflectanceDerivativeWRTSpecularFracI(sHatI, epochGT, MT);
      else if (solveForList[i] == "DiffuseFraction")
         deriv = GetReflectanceDerivativeWRTDiffuseFracI(sHatI, epochGT, MT);
      derivativeI.push_back(deriv);
   }

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("relectance derivativeI of plate <%s> (size = %d) = [\n", GetName().c_str(), derivativeI.size());
   for (Integer i = 0; i < derivativeI.size(); ++i)
      MessageInterface::ShowMessage("%le  %le  %le\n", derivativeI[i][0], derivativeI[i][1], derivativeI[i][2]);
   MessageInterface::ShowMessage("]\n");
   MessageInterface::ShowMessage("End Plate::GetReflectanceDerivativeI()\n");
#endif
   return derivativeI;
}



//-----------------------------------------------------------------------------
// std::vector<Rvector3> GetReflectanceDerivative(Rvector3 &sHat, Rmatrix33 MT, 
//                         std::vector<Rmatrix33> dMT, GmatTime &epochGT)
//-----------------------------------------------------------------------------
/**
* Get derivative of reflectance in the spacecraft's I-frame
*
* @param sunSC   vector from the Sun to the spacecraft in inertial frame
* @param MT      rotation matrix from spacecraft's body fixed frame to its inertial frame.
*                It needs for computing face normal unit vector for case using history data file.
*
* @return        plate's reflectance derivative in the spacecraft's inertial frame (I-frame)
*/
//-----------------------------------------------------------------------------
std::vector<Rvector3> Plate::GetReflectanceDerivative(const Rvector3 &sunSC, 
                                                      Rmatrix33 &MT, 
                                                      std::vector<Rmatrix33> &dMT,
                                                      GmatTime &epochGT)
{
#ifdef DEBUG_REFLECTANCE_DERIVATIVE
   MessageInterface::ShowMessage("Start Plate::GetReflectanceDerivativeB()\n");
#endif
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("  sunSC = [%lf  %lf  %lf]\n", sunSC[0], sunSC[1], sunSC[2]);
#endif

   // 1. Calculate derivative w.r.t. spacecraft state in B-frame
   std::vector<Rvector3> derivativeB =
      GetReflectanceDerivativeWRTSpacecraftState(sunSC, MT, dMT, epochGT);
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("derivativeB.size() = %d\n", derivativeB.size());
#endif

   // 2. Calculate derivative w.r.t. solve-for parameters in B-frame
   // 2.1. Specify sHatB
   Rvector3 sHatI = (-sunSC).GetUnitVector();
   Rvector3 sHatB = MT.Transpose()*sHatI;
   
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("sHatB = [%lf  %lf  %lf]\n", sHatB[0], sHatB[1], sHatB[2]);
#endif

   // 2.3. Specify derivative w.r.t. solvefors
   Rvector3 deriv;
   for (Integer i = 0; i < solveForList.size(); ++i)
   {
      // Specify derivative w.r.t. solve-for in B-frame
      if (solveForList[i] == "Area")
         deriv = GetReflectanceDerivativeWRTPlateArea(sHatB, epochGT, MT);
      else if (solveForList[i] == "AreaCoefficient")
         deriv = GetReflectanceDerivativeWRTAreaCoeff(sHatB, epochGT, MT);
      else if (solveForList[i] == "LitFraction")
         deriv = GetReflectanceDerivativeWRTLitFrac(sHatB, epochGT, MT);
      else if (solveForList[i] == "SpecularFraction")
         deriv = GetReflectanceDerivativeWRTSpecularFrac(sHatB, epochGT, MT);
      else if (solveForList[i] == "DiffuseFraction")
         deriv = GetReflectanceDerivativeWRTDiffuseFrac(sHatB, epochGT, MT);
      derivativeB.push_back(deriv);
   }

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("relectance derivativeB of plate <%s> (size = %d) = [\n", GetName().c_str(), derivativeB.size());
   for (Integer i = 0; i < derivativeB.size(); ++i)
      MessageInterface::ShowMessage("%le  %le  %le\n", derivativeB[i][0], derivativeB[i][1], derivativeB[i][2]);
   MessageInterface::ShowMessage("]\n");
#endif

   // 3. Specify derivative in I-frame
   // 3.1. Get plate reflectance in B-frame 
   Rvector3 A = GetReflectance(sHatB, epochGT, MT);

   // 3.2. Convert derivative from B-frame to I-frame by using equation [dAI/dX] = [dMT/dX]*A + MT*[dA/dX]
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("dMT.size() = %d\n", dMT.size());
#endif
   std::vector<Rvector3> derivativeI;
   for (Integer i = 0; i < derivativeB.size(); ++i)
   {
      Rvector3 dAdx = derivativeB[i];
      Rvector3 dAIdx;
      if (i < dMT.size())
         dAIdx = dMT[i] * A + MT * dAdx;
      else
      {
         // [dMT/dx] = [0]3x3 when x is a variable other than x,y,x and vx,vy,vz
         dAIdx = MT * dAdx;
      }
      derivativeI.push_back(dAIdx);
   }

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("derivativeI.size() = %d\n", derivativeI.size());
   MessageInterface::ShowMessage("End Plate::GetReflectanceDerivativeB()\n");
#endif
   return derivativeI;
}


//----------------------------------------------------------------------------------
//Rmatrix Calculate_sHatI_Deriv(const Rvector3 &sunSC)
//----------------------------------------------------------------------------------
/**
* This function is used to calculate the derivative of the Sun unit vector in the
* inertial frame
*
* @param sunSC     vector from the Sun to spacecraft in inertial frame
*
* @return          derivative w.r.t. spacecraft state of the Sun unit vector in
*                  the inertial frame. It return 3x6 matrix
*/
//----------------------------------------------------------------------------------
Rmatrix Plate::Calculate_sHatI_Deriv(const Rvector3 &sunSC)
{
#ifdef DEBUG_SHATI_DERIV
   MessageInterface::ShowMessage("Start Plate::Calculate_sHatI_Deriv()\n");
   MessageInterface::ShowMessage("  sunSC = [%lf  %lf  %lf]km\n", sunSC[0], sunSC[1], sunSC[2]);
#endif

   // 1. Specify the Sun unit vector sHatI in inertial frame
   Rvector3 sI = -sunSC;                      // vector pointing from spacecraft to the Sun
   Real     sIMag = sI.GetMagnitude();
   Rvector3 sHatI = sI / sIMag;               // Eq.36 SRP N-Plates MathSpec 

#ifdef DEBUG_SHATI_DERIV
   MessageInterface::ShowMessage("  sI = [%lf  %lf  %lf]km\n", sI[0], sI[1], sI[2]);
   MessageInterface::ShowMessage("  sHatI = [%lf  %lf  %lf]km\n", sHatI[0], sHatI[1], sHatI[2]);
#endif

   // 2. Calculate derivative w.r.t. position and velocity 
   // Eq.37 SRP N-Plates MathSpec
   Rmatrix33 dsHatIdr;
   for (Integer row = 0; row < 3; ++row)
   {
      for (Integer col = 0; col < 3; ++col)
      {
         dsHatIdr(row, col) = (((row == col) ? -1.0 : 0.0) + sHatI(row)*sHatI(col)) / sIMag;
      }
   }
#ifdef DEBUG_SHATI_DERIV
   MessageInterface::ShowMessage("dsHatIdr = [\n");
   for (Integer row = 0; row < 3; ++row)
   {
      for (Integer col = 0; col < 3; ++col)
         MessageInterface::ShowMessage("%le  ", dsHatIdr(row, col));
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("]\n");
#endif

   // 3. Store derivatives to 3x6 matrix
   static Rmatrix result;
   result.SetSize(3, 6);
   for (Integer row = 0; row < 3; ++row)
   {
      for (Integer col = 0; col < 3; ++col)
      {
         result(row, col) = dsHatIdr(row, col);
         result(row, col + 3) = 0.0;                  // Eq.38 in SRP N-Plates MathSpec  for dsHatI/dv
      }
   }

#ifdef DEBUG_SHATI_DERIV
   MessageInterface::ShowMessage("sHatIDeriv = [\n");
   for (Integer row = 0; row < result.GetNumRows(); ++row)
   {
      for (Integer col = 0; col < result.GetNumColumns(); ++col)
         MessageInterface::ShowMessage("%le  ", result(row, col));
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("]\n");
#endif

   return result;
}


std::vector<Rvector3> Plate::GetReflectanceDerivativeWRTSpacecraftState(const Rvector3 &sunSC,
   Rmatrix33 MT, std::vector<Rmatrix33> MTDeriv, GmatTime &epochGT)
{
#ifdef DEBUG_REFLECTANCE_DERIVATIVE
   MessageInterface::ShowMessage("Start Plate::GetReflectanceDerivativeWRTSpacecraftState()\n");
#endif
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Inputs:\n");
   MessageInterface::ShowMessage("  sunSC = [%lf  %lf  %lf]km\n", sunSC[0], sunSC[1], sunSC[2]);
#endif

   // 1. Specify sHat and nHat
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Step1.1. Calculate sHatI and sHat\n");
#endif

   Rvector3 sHatI = -sunSC.GetUnitVector();                // unit vector pointing from spacecraft to the Sun in inertial frame (I-frame)
   Rmatrix  sHatI_Deriv = Calculate_sHatI_Deriv(sunSC);    // dsHatI/dX

   Rvector3 sHat = MT.Transpose()*sHatI;                   // unit vector pointing from spacecraft to the Sun in spacecraft's attitude frame (B-frame)

   
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Step1.2. Calculate nHat\n");
#endif
   Rvector3 nHat;
   if (plateType == "FixedInBody")
      nHat = plateNormal;                                  // nHat is constant vector in spacecraft's attitude frame
   else if (plateType == "SunFacing")
      nHat = sHat;                                         // nHat points to the Sun
   else if (plateType == "File")
   {
      // nHat is varied based on epochGT as defined w.r.t. time in NPlate history file
      nHat = GetFaceNormal(epochGT, MT);
   }


#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Step1.3. Verify nHat*nHat <= 0\n");
#endif
   std::vector<Rvector3> deriv;
   if ((sHat[0] * nHat[0] + sHat[1] * nHat[1] + sHat[2] * nHat[2]) <= EPSILON)
   {
      // return zero derivative
      Rvector3 zero(0.0, 0.0, 0.0);
      for (Integer i = 0; i < 6; ++i)
         deriv.push_back(zero);
      return deriv;
   }


   // 2. Specify derivatives of sHat, sHatI, and nHat
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Step2.1. Calculate dsHatI/dX, dsHat/dX\n");
#endif

   // We have:   sHatI = [MT] * sHat
   // Therefore: [dsHatI/dx] = [dMT/dx]*sHat + MT*[dsHat/dx] 
   // Thus:      [dsHat/dx] = M * ([dsHatI/dx] - [dMT/dx]*sHat)
   Rmatrix dsHatdX(3, 6);
   for (Integer stateIndex = 0; stateIndex < MTDeriv.size(); ++stateIndex)
   {
      // Specify [dsHatI/dx]
      Rvector3 dsHatI_dx(sHatI_Deriv(0, stateIndex), sHatI_Deriv(1, stateIndex), sHatI_Deriv(2, stateIndex));

      // Specify [dsHat/dx] = M * ([dsHatI/dx] - [dMT/dx]*sHat)
      Rvector3 dsHat_dx = MT.Transpose() * (dsHatI_dx - MTDeriv[stateIndex] * sHat);

      dsHatdX(0, stateIndex) = dsHat_dx[0];
      dsHatdX(1, stateIndex) = dsHat_dx[1];
      dsHatdX(2, stateIndex) = dsHat_dx[2];
   }

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Step2.2. Calculate nHat/dX\n");
#endif
   Rmatrix dnHatdX(3,6);    // 3x6 zero matrix 
   if (plateType == "FixedInBody")
   {
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 6; ++j)
            dnHatdX(i, j) = 0.0;
      // nHat is constant when plateType is "FixedInBody" so dnHatdX is [0]3x6 matrix
   }
   if (plateType == "SunFacing")
      dnHatdX = dsHatdX;               // due to nHat = sHat
   else if (plateType == "File")
   {
      // vector nHat get from NPlate history file depends on time only but spacecraft's state X. Therefore, dnHat/dX = [0] 
   }


   // 3. Calculate A, C, and D
   // Specify A, rho, and delta
   Real A = areaCoeff * plateArea * litFrac;
   Real rho = specularFrac;
   Real delta = diffuseFrac;

   Real D = sHat * nHat;                                             // Eq.25 SRP N-Plates MathSpec
   Rvector3 C = (1.0 - rho)*sHat + 2.0 * (delta / 3.0 + rho * D)*nHat;     // Eq.26 SRP N-Plates MathSpec

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Step3. Calculate A, C, and D\n");
#endif

   // 4. Calculate partial derivative
   Rvector3 dDdsHat = nHat;                                          // Eq.30 SRP N-Plates MathSpec
   Rvector3 dDdnHat = sHat;                                          // Eq.31 SRP N-Plates MathSpec

   Rmatrix33 ninj, sinj, I, CdDdsHat, CdDdnHat;
   for (Integer i = 0; i < 3; ++i)
   {
      for (Integer j = 0; j < 3; ++j)
      {
         ninj(i, j) = nHat[i] * nHat[j];                           // in Eq.32 SRP N-Plates MathSpec
         sinj(i, j) = sHat[i] * nHat[j];                           // in Eq.33 SRP N-Plates MathSpec
         I(i, j) = ((i == j) ? 1.0 : 0.0);
         CdDdsHat(i, j) = C[i] * dDdsHat[j];                       // term C*[dD/dsHat] in Eq.28 SRP N-Plates MathSpec
         CdDdnHat(i, j) = C[i] * dDdnHat[j];                       // term C*[dD/dnHat] in Eq.29 SRP N-Plates MathSpec
      }
   }

   Rmatrix33 dnHatdsHat;
   Rmatrix33 dsHatdnHat;
   for (Integer i = 0; i < 3; ++i)
   {
      for (Integer j = 0; j < 3; ++j)
      {
         if (plateType == "FixedInBody")
         {
            dnHatdsHat(i, j) = 0.0;                      // nHat and sHat are independent variables when plateType is "FixedInBody" so dnHatdsHat is [0]3x3 matrix
            dsHatdnHat(i, j) = 0.0;                      // nHat and sHat are independent variables when plateType is "FixedInBody" so dsHatdnHat is [0]3x3 matrix
         }
         if (plateType == "SunFacing")
         {
            dnHatdsHat(i, j) = ((i == j) ? 1.0 : 0.0);         // nHat and sHat are dependent with constraint nHat = sHat. Therefore, dnHat/dsHat = [I]3x3
            dsHatdnHat(i, j) = ((i == j) ? 1.0 : 0.0);         // nHat and sHat are dependent with constraint nHat = sHat. Therefore, dsHat/dnHat = [I]3x3
         }
         else if (plateType == "File")
         {
            // vector nHat get from NPlate history file depends on time only but spacecraft's state X. Therefore, dnHat/dX = [0] 
         }
      }
   }

   Rmatrix33 dCdsHat = I*(1 - rho) + (ninj * rho + dnHatdsHat*(delta / 3.0 + rho * D))*2.0;      // Eq.32 SRP N-Plates MathSpec
   Rmatrix33 dCdnHat = dsHatdnHat*(1 - rho) + (sinj*rho + I*(delta / 3.0 + rho * D))*2.0;        // Eq.33 SRP N-Plates MathSpec

   Rmatrix33 dAdsHat = (CdDdsHat + dCdsHat * D) * A;                                             // Eq.28 SRP N-Plates MathSpec
   Rmatrix33 dAdnHat = (CdDdnHat + dCdnHat * D) * A;                                             // Eq.29 SRP N-Plates MathSpec

   // dAdX is a 3xm matrix with m is the size of vector X
   Rmatrix dAdX(3, 6);
   dAdX = ((Rmatrix)dAdsHat) * dsHatdX + ((Rmatrix)dAdnHat) * dnHatdX;                           // Eq.24 SRP N-Plates MathSpec

   for (Integer col = 0; col < dAdX.GetNumColumns(); ++col)
   {
      Rvector3 colVec(dAdX(0,col), dAdX(1, col), dAdX(2, col));
      deriv.push_back(colVec);
   }

#ifdef DEBUG_REFLECTANCE_DERIVATIVE
   MessageInterface::ShowMessage("End Plate::GetReflectanceDerivativeWRTSpacecraftState()\n");
#endif
   return deriv;
}


Rvector3 Plate::GetReflectanceDerivativeWRTSpecularFrac(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT)
{
   Rvector3 nHat;
   if (plateType == "FixedInBody")
      nHat = plateNormal;                       // nHat is constant vector in spacecraft's attitude frame
   else if (plateType == "SunFacing")
      nHat = sHat;                              // nHat points to the Sun
   else if (plateType == "File")
   {
      // nHat is varied based on epochGT as defined w.r.t. time in NPlate history file
      nHat = GetFaceNormal(epochGT, MT);
   }

   Real A = areaCoeff * plateArea * litFrac;
   Real D = sHat * nHat;                     // Eq.25 SRP N-Plates MathSpec
   Rvector3 derivative(0.0, 0.0, 0.0);
   if (D > EPSILON)
      derivative = A * D *(-sHat + (2*D)*nHat);

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Plate <%s>: A = %.12le   D = %.12le\n", GetName().c_str(), A, D);
   MessageInterface::ShowMessage("Plate <%s>: derivative w.r.t. SpecularFraction = [%.12le  %.12le  %.12le]\n",
      GetName().c_str(), derivative[0], derivative[1], derivative[2]);
#endif
   return derivative;
}


Rvector3 Plate::GetReflectanceDerivativeWRTDiffuseFrac(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT)
{
   Rvector3 nHat;
   if (plateType == "FixedInBody")
      nHat = plateNormal;                       // nHat is constant vector in spacecraft's attitude frame
   else if (plateType == "SunFacing")
      nHat = sHat;                              // nHat points to the Sun
   else if (plateType == "File")
   {
      // nHat is varied based on epochGT as defined w.r.t. time in NPlate history file
      nHat = GetFaceNormal(epochGT, MT);
   }

   Real A = areaCoeff * plateArea * litFrac;
   Real D = sHat * nHat;                     // Eq.25 SRP N-Plates MathSpec
   Rvector3 derivative(0.0, 0.0, 0.0);
   if (D > EPSILON)
      derivative = A * D * (2.0/3.0) * nHat;

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Plate <%s>: A = %.12le   D = %.12le\n", GetName().c_str(), A, D);
   MessageInterface::ShowMessage("Plate <%s>: derivative w.r.t. DiffuseFraction = [%.12le  %.12le  %.12le]\n",
      GetName().c_str(), derivative[0], derivative[1], derivative[2]);
#endif
   return derivative;
}


Rvector3 Plate::GetReflectanceDerivativeWRTAreaCoeff(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT)
{
#ifdef DEBUG_REFLECTANCE_DERIVATIVE
   MessageInterface::ShowMessage("Start Plate::GetReflectanceDerivativeWRTAreaCoeff()\n");
#endif

   reflectance = GetReflectance(sHat, epochGT, MT);

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Plate <%s>: derivative w.r.t. area coeff = [%.12le  %.12le  %.12le]/ %.12le\n", 
      GetName().c_str(), reflectance[0], reflectance[1], reflectance[2], areaCoeff);
#endif

#ifdef DEBUG_REFLECTANCE_DERIVATIVE
   MessageInterface::ShowMessage("End Plate::GetReflectanceDerivativeWRTAreaCoeff()\n");
#endif
   return (reflectance / areaCoeff);
}


Rvector3 Plate::GetReflectanceDerivativeWRTPlateArea(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT)
{
   reflectance = GetReflectance(sHat, epochGT, MT);

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Plate <%s>: derivative w.r.t. plate area = [%.12le  %.12le  %.12le]/ %.12le\n",
      GetName().c_str(), reflectance[0], reflectance[1], reflectance[2], plateArea);
#endif
   return (reflectance / plateArea);
}

Rvector3 Plate::GetReflectanceDerivativeWRTLitFrac(Rvector3 &sHat, GmatTime &epochGT, Rmatrix33 &MT)
{
   reflectance = GetReflectance(sHat, epochGT, MT);
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Plate <%s>: derivative w.r.t. LitFraction = [%.12le  %.12le  %.12le]/ %.12le\n",
      GetName().c_str(), reflectance[0], reflectance[1], reflectance[2], litFrac);
#endif
   return (reflectance / litFrac);
}


//------------------------------------------------------------------------------------------
// Rvector3 GetFaceNormalI(GmatTime &epochGT, Rmatrix33 &MT)
//------------------------------------------------------------------------------------------
/**
* This function uses to get face normal vector from NPlate NormalHistory file and converts 
* that vector to inertial frame. This function only uses for Plate.Type = 'File'
*
* @param epochGT     the epoch at which the normal vector of face is pointing to
* @param MT          rotation matrix from spacecraft attitude frame to inertial frame
*
* @return            face normal vector presenting in inertial frame
*/
//------------------------------------------------------------------------------------------
Rvector3 Plate::GetFaceNormalI(const GmatTime &epochGT, Rmatrix33 &MT)
{
   if (faceNormalFile == NULL)
   {
      std::stringstream ss;
      ss << "Error: No N-Plate face normal history file for Plate '" << GetName() << "' is defined.\n";
      throw GmatBaseException(ss.str());
   }

   // Get face normal unit
   Rvector3 nHat = faceNormalFile->GetFaceNormal(epochGT);

   // Get coordinate system
   std::string csName = faceNormalFile->GetCoordinateSystemName();
   if (csName == "FixedInBody")
      return MT*nHat;                // nHatI = MT * nHat. Where: nHat presents unit normal vector in B-frame
   else
   {
      GmatBase *obj = NULL;
      try
      {
         obj = GetConfiguredObject(csName);
      }
      catch (BaseException &be)
      {
         std::stringstream ss;
         ss << "Error: Plate NormalHistoryFile coordinate system '" << csName << "' is not defined.\n";
         throw GmatBaseException(ss.str());
      }

      if (obj->IsOfType(Gmat::COORDINATE_SYSTEM) == false)
      {
         std::stringstream ss;
         ss << "Error: Plate NormalHistoryFile coordinate system '" << csName << "' is not defined.\n";
         throw GmatBaseException(ss.str());
      }
      CoordinateSystem *historyCS = (CoordinateSystem*)obj;

      // Convert face normal unit to spacecraft body fixed coordinate system
      // Get rotation matrix from coordinate system defined in history data file to spacecraft's coordinate system
      Rvector6 inState(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
      Rvector6 outState(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      CoordinateConverter cc;
      cc.Convert(epochGT, inState, historyCS, outState, scInertialCS, false, true);
      //Rmatrix33 rot = cc.GetLastRotationMatrix();
      //Rmatrix33 R = MT.Transpose() * rot;
      Rmatrix33 R = cc.GetLastRotationMatrix();

      return R * nHat;
   }

}


Rvector3 Plate::GetFaceNormal(const GmatTime &epochGT, Rmatrix33 &MT)
{
   if (faceNormalFile == NULL)
   {
      std::stringstream ss;
      ss << "Error: No N-Plate face normal history file for Plate '" << GetName() << "' is defined.\n";
      throw GmatBaseException(ss.str());
   }

   // Get face normal unit
   Rvector3 nHat = faceNormalFile->GetFaceNormal(epochGT);

   // Get coordinate system
   std::string csName = faceNormalFile->GetCoordinateSystemName();
   if (csName == "FixedInBody")
      return nHat;
   else
   {
      GmatBase *obj = NULL;
      try 
      {
         obj = GetConfiguredObject(csName);
      }
      catch (BaseException &be)
      {
         std::stringstream ss;
         ss << "Error: Plate NormalHistoryFile coordinate system '" << csName << "' is not defined.\n";
         throw GmatBaseException(ss.str());
      }

      if (obj->IsOfType(Gmat::COORDINATE_SYSTEM) == false)
      {
         std::stringstream ss;
         ss << "Error: Plate NormalHistoryFile coordinate system '" << csName << "' is not defined.\n";
         throw GmatBaseException(ss.str());
      }
      CoordinateSystem *historyCS = (CoordinateSystem*)obj;
      
      // Convert face normal unit to spacecraft body fixed coordinate system
      // Get rotation matrix from coordinate system defined in history data file to spacecraft's coordinate system
      Rvector6 inState(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
      Rvector6 outState(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      CoordinateConverter cc;
      cc.Convert(epochGT, inState, historyCS, outState, scInertialCS, false, true);
      Rmatrix33 rot = cc.GetLastRotationMatrix();
      Rmatrix33 R = MT.Transpose() * rot;

      return R * nHat;
   }

}



std::vector<Rvector3> Plate::GetReflectanceDerivativeWRTSpacecraftStateI(const Rvector3 &sunSCI,
   Rmatrix33 MT, std::vector<Rmatrix33> MTDeriv, GmatTime &epochGT)
{
#ifdef DEBUG_REFLECTANCE_DERIVATIVE
   MessageInterface::ShowMessage("Start Plate::GetReflectanceDerivativeWRTSpacecraftStateI()\n");
#endif
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Inputs:\n");
   MessageInterface::ShowMessage("  sunSCI = [%lf  %lf  %lf]km\n", sunSCI[0], sunSCI[1], sunSCI[2]);
#endif

   // 1. Specify sHat and nHat
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Step1.1. Calculate sHatI and nHatI\n");
#endif

   Rvector3 sHatI = -sunSCI.GetUnitVector();                // unit vector pointing from spacecraft to the Sun in inertial frame (I-frame)

   Rvector3 nHatI;
   if (plateType == "FixedInBody")
      nHatI = MT * plateNormal;                            // plateNormal is constant vector in spacecraft's attitude frame
   else if (plateType == "SunFacing")
      nHatI = sHatI;                                       // nHatI points to the Sun
   else if (plateType == "File")
   {
      // nHatI is varied based on epochGT as defined w.r.t. time in NPlate history file
      nHatI = GetFaceNormalI(epochGT, MT);
   }

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Step1.2. Verify nHatI*nHatI <= 0\n");
#endif
   std::vector<Rvector3> deriv;
   if ((sHatI * nHatI) <= EPSILON)
   {
      // return zero derivative
      Rvector3 zero(0.0, 0.0, 0.0);
      for (Integer i = 0; i < 6; ++i)
         deriv.push_back(zero);
      return deriv;
   }


   // 2. Specify derivatives of sHatI and nHatI
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Step2.1. Calculate dsHatI/dX and dsHatI/dX\n");
#endif
   Rmatrix  dsHatIdX = Calculate_sHatI_Deriv(sunSCI);      // dsHatI/dX

   Rmatrix dnHatIdX(3, 6);                                 // Set to 3x6 zero matrix by default
   if (plateType == "FixedInBody")
   {
      // nHatI = MT * nHat               where: nHat (B-frame) is constant when plateType is "FixedInBody"
      Rvector3 nHat = MT.Transpose() * nHatI;

      //[dnHatI/dX] = dMT/dX * nHat      due to nHat is constant
      for (Integer i = 0; i < MTDeriv.size(); ++i)             // MTDeriv = [dMT/dx  dMT/dy  dMT/dz  dMT/dvx  dMT/dvy  dMT/dvz]
      {
         Rvector3 deriv = MTDeriv[i] * nHat;
         for (Integer j = 0; j < 3; ++j)
            dnHatIdX(j, i) = deriv[j];
      }
   }
   if (plateType == "SunFacing")
      dnHatIdX = dsHatIdX;                                // due to nHatI = sHatI
   else if (plateType == "File")
   {
      // vector nHat get from NPlate history file depends on time only but spacecraft's state X. Therefore, dnHat/dX = [0]3x6
   }


   // 3. Calculate A, C, and D
   // Specify A, rho, and delta
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Step3. Calculate A, C, and D\n");
#endif
   Real A = areaCoeff * plateArea * litFrac;
   Real rho = specularFrac;
   Real delta = diffuseFrac;

   Real D = sHatI * nHatI;                                                   // Eq.25 SRP N-Plates MathSpec
   Rvector3 C = (1.0 - rho)*sHatI + 2.0 * (delta / 3.0 + rho * D)*nHatI;     // Eq.26 SRP N-Plates MathSpec


   // 4. Calculate partial derivative
   Rvector3 dDdsHatI = nHatI;                                                // Eq.30 SRP N-Plates MathSpec
   Rvector3 dDdnHatI = sHatI;                                                // Eq.31 SRP N-Plates MathSpec

   Rmatrix33 ninj, nisj, I, CdDdsHatI, CdDdnHatI;
   for (Integer i = 0; i < 3; ++i)
   {
      for (Integer j = 0; j < 3; ++j)
      {
         ninj(i, j) = nHatI[i] * nHatI[j];                                   // in Eq.32 SRP N-Plates MathSpec
         nisj(i, j) = nHatI[i] * sHatI[j];                                   // in Eq.33 SRP N-Plates MathSpec
         I(i, j) = ((i == j) ? 1.0 : 0.0);
         CdDdsHatI(i, j) = C[i] * dDdsHatI[j];                               // term C*[dD/dsHat] in Eq.28 SRP N-Plates MathSpec
         CdDdnHatI(i, j) = C[i] * dDdnHatI[j];                               // term C*[dD/dnHat] in Eq.29 SRP N-Plates MathSpec
      }
   }

   Rmatrix33 dnHatIdsHatI;
   Rmatrix33 dsHatIdnHatI;
   for (Integer i = 0; i < 3; ++i)
   {
      for (Integer j = 0; j < 3; ++j)
      {
         if (plateType == "FixedInBody")
         {
            dnHatIdsHatI(i, j) = 0.0;                             // nHatI and sHatI are independent variables when plateType is "FixedInBody" so dnHatIdsHatI is [0]3x3 matrix
            dsHatIdnHatI(i, j) = 0.0;                             // nHatI and sHatI are independent variables when plateType is "FixedInBody" so dsHatIdnHatI is [0]3x3 matrix
         }
         if (plateType == "SunFacing")
         {
            dnHatIdsHatI(i, j) = ((i == j) ? 1.0 : 0.0);         // nHatI and sHatI are dependent with constraint nHatI = sHatI. Therefore, dnHatI/dsHatI = [I]3x3
            dsHatIdnHatI(i, j) = ((i == j) ? 1.0 : 0.0);         // nHatI and sHatI are dependent with constraint nHatI = sHatI. Therefore, dsHatI/dnHatI = [I]3x3
         }
         else if (plateType == "File")
         {
            // vector nHat get from NPlate history file depends on time only but the Sun unit vector sHat. Therefore, dnHat/dsHat = [0] 
            dnHatIdsHatI(i, j) = 0.0;
            dsHatIdnHatI(i, j) = 0.0;
         }
      }
   }

   Rmatrix33 dCdsHatI = I * (1 - rho) + (ninj * rho + dnHatIdsHatI * (delta / 3.0 + rho * D))*2.0;      // Eq.32 SRP N-Plates MathSpec
   Rmatrix33 dCdnHatI = dsHatIdnHatI * (1 - rho) + (nisj*rho + I * (delta / 3.0 + rho * D))*2.0;        // Eq.33 SRP N-Plates MathSpec

   Rmatrix33 dAdsHatI = (CdDdsHatI + dCdsHatI * D) * A;                                                 // Eq.28 SRP N-Plates MathSpec
   Rmatrix33 dAdnHatI = (CdDdnHatI + dCdnHatI * D) * A;                                                 // Eq.29 SRP N-Plates MathSpec

                                                                                                        // dAdX is a 3xm matrix with m is the size of vector X
   Rmatrix dAdX(3, 6);
   dAdX = ((Rmatrix)dAdsHatI) * dsHatIdX + ((Rmatrix)dAdnHatI) * dnHatIdX;                              // Eq.24 SRP N-Plates MathSpec

   for (Integer col = 0; col < dAdX.GetNumColumns(); ++col)
   {
      Rvector3 colVec(dAdX(0, col), dAdX(1, col), dAdX(2, col));
      deriv.push_back(colVec);
   }

#ifdef DEBUG_REFLECTANCE_DERIVATIVE
   MessageInterface::ShowMessage("End Plate::GetReflectanceDerivativeWRTSpacecraftStateI()\n");
#endif
   return deriv;
}


Rvector3 Plate::GetReflectanceDerivativeWRTSpecularFracI(Rvector3 &sHatI, GmatTime &epochGT, Rmatrix33 &MT)
{
   Rvector3 nHatI;
   if (plateType == "FixedInBody")
      nHatI = MT*plateNormal;                   // plateNormal is constant vector in spacecraft's attitude frame (B-frame)
   else if (plateType == "SunFacing")
      nHatI = sHatI;                            // nHatI points to the Sun
   else if (plateType == "File")
   {
      // nHatI is varied based on epochGT as defined w.r.t. time in NPlate history file
      nHatI = GetFaceNormalI(epochGT, MT);
   }

   Real A = areaCoeff * plateArea * litFrac;
   Real D = sHatI * nHatI;                     // Eq.25 SRP N-Plates MathSpec
   Rvector3 derivative(0.0, 0.0, 0.0);         // derivative equals zero for D <= EPSILON (0.0)
   if (D > EPSILON)
      derivative = A * D *(-sHatI + (2 * D)*nHatI);

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Plate <%s>: A = %.12le   D = %.12le\n", GetName().c_str(), A, D);
   MessageInterface::ShowMessage("Plate <%s>: derivative w.r.t. SpecularFraction = [%.12le  %.12le  %.12le]\n",
      GetName().c_str(), derivative[0], derivative[1], derivative[2]);
#endif
   return derivative;
}


Rvector3 Plate::GetReflectanceDerivativeWRTDiffuseFracI(Rvector3 &sHatI, GmatTime &epochGT, Rmatrix33 &MT)
{
   Rvector3 nHatI;
   if (plateType == "FixedInBody")
      nHatI = MT*plateNormal;                    // plateNormal is constant vector in spacecraft's attitude frame
   else if (plateType == "SunFacing")
      nHatI = sHatI;                             // nHatI points to the Sun
   else if (plateType == "File")
   {
      // nHatI is varied based on epochGT as defined w.r.t. time in NPlate history file
      nHatI = GetFaceNormalI(epochGT, MT);
   }

   Real A = areaCoeff * plateArea * litFrac;
   Real D = sHatI * nHatI;                       // Eq.25 SRP N-Plates MathSpec
   Rvector3 derivative(0.0, 0.0, 0.0);           // derivative equals zero for D <= EPSILON (0.0)
   if (D > EPSILON)
      derivative = A * D * (2.0 / 3.0) * nHatI;

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Plate <%s>: A = %.12le   D = %.12le\n", GetName().c_str(), A, D);
   MessageInterface::ShowMessage("Plate <%s>: derivative w.r.t. DiffuseFraction = [%.12le  %.12le  %.12le]\n",
      GetName().c_str(), derivative[0], derivative[1], derivative[2]);
#endif
   return derivative;
}


Rvector3 Plate::GetReflectanceDerivativeWRTAreaCoeffI(Rvector3 &sHatI, GmatTime &epochGT, Rmatrix33 &MT)
{
#ifdef DEBUG_REFLECTANCE_DERIVATIVE
   MessageInterface::ShowMessage("Start Plate::GetReflectanceDerivativeWRTAreaCoeffI()\n");
#endif

   reflectance = GetReflectanceI(sHatI, epochGT, MT);

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Plate <%s>: derivative w.r.t. area coeff = [%.12le  %.12le  %.12le]/ %.12le\n",
      GetName().c_str(), reflectance[0], reflectance[1], reflectance[2], areaCoeff);
#endif

#ifdef DEBUG_REFLECTANCE_DERIVATIVE
   MessageInterface::ShowMessage("End Plate::GetReflectanceDerivativeWRTAreaCoeffI()\n");
#endif
   return (reflectance / areaCoeff);
}


Rvector3 Plate::GetReflectanceDerivativeWRTPlateAreaI(Rvector3 &sHatI, GmatTime &epochGT, Rmatrix33 &MT)
{
   reflectance = GetReflectanceI(sHatI, epochGT, MT);

#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Plate <%s>: derivative w.r.t. plate area = [%.12le  %.12le  %.12le]/ %.12le\n",
      GetName().c_str(), reflectance[0], reflectance[1], reflectance[2], plateArea);
#endif
   return (reflectance / plateArea);
}

Rvector3 Plate::GetReflectanceDerivativeWRTLitFracI(Rvector3 &sHatI, GmatTime &epochGT, Rmatrix33 &MT)
{
   reflectance = GetReflectanceI(sHatI, epochGT, MT);
#ifdef DEBUG_REFLECTANCE_DERIVATIVE_CALCULATION
   MessageInterface::ShowMessage("Plate <%s>: derivative w.r.t. LitFraction = [%.12le  %.12le  %.12le]/ %.12le\n",
      GetName().c_str(), reflectance[0], reflectance[1], reflectance[2], litFrac);
#endif
   return (reflectance / litFrac);
}




void Plate::StoreSpacecraftInertialCoordinateSystem(CoordinateSystem* inertialCS) 
{
   scInertialCS = inertialCS; 
}


bool Plate::SetErrorSelection(bool select)
{
   bool prevSelection = errorSelection;
   errorSelection = select;

   return prevSelection;
}


//-------------------------------------------------------------------------------
// Integer SetRunningCommandFlag(Integer runningCommand)
//-------------------------------------------------------------------------------
/**
* This function is used to set a flag to indicate simulation, propagation, or
* estimation command is running and using this spacecraft for its command.
*
* @param runningCommand    flag is used to indicate what command is running
*                          0: not running;
*                          1: running simulation command
*                          2: running propagation command
*                          3: running estimation command
*
* @return  its previous flag
*/
//-------------------------------------------------------------------------------
Integer Plate::SetRunningCommandFlag(Integer runningCommand)
{
   bool temp = runningCommandFlag;
   runningCommandFlag = runningCommand;

   // When running command, turn off error message when Plate.AreaCoefficient, Plate.SpecularFraction, or Plate.DeffuseFraction is out of range  
   if (runningCommandFlag != 0)
      errorSelection = false;

   return temp;
};

