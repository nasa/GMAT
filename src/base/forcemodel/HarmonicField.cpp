//$Id$
//------------------------------------------------------------------------------
//                              HarmonicField
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// number S-67573-G
//
// Author: D. Conway
// Created: 2003/03/14
// Modified: 2004/04/15 W. Shoan GSFC Code 583
//           Updated for GMAT style, standards; changed quad-t to Real, int
//           to Integer, add use of GMAT util classes, etc.
///
/**
 * This is the HarmonicField class.
 *
 * @note original prolog information included at end of file prolog.
 */
//
// *** File Name : shfieldmodel.cpp  -> HarmonicField.cpp (wcs 2004.04.14)
// *** Created   : March 14, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-67521-G                             ***
// ***                                                                    ***
// ***  Copyright U.S. Government 2002                                    ***
// ***  Copyright United States Government as represented by the          ***
// ***  Administrator of the National Aeronautics and Space               ***
// ***  Administration                                                    ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C/C++ Source
// Development Environment   : Borland C++ 5.02
// Modification History      : 3/14/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
// **************************************************************************

#include "HarmonicField.hpp"

#include "GravityBase.hpp"
#include "ODEModelException.hpp"
#include "StringUtil.hpp"     // for ToString()
#include "RealUtilities.hpp"
#include "TimeTypes.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include <sstream>                 // for <<

/*
#include <iostream>
 
#define DEBUG_REFERENCE_SETTING
*/

using namespace GmatMathUtil;

//#define DEBUG_HARMONIC_FIELD
//#define DEBUG_HARMONIC_FIELD_FILENAME

//---------------------------------
// static data
//---------------------------------
const std::string
HarmonicField::PARAMETER_TEXT[HarmonicFieldParamCount - GravityBaseParamCount] =
{
   "MaxDegree",
   "MaxOrder",
   "Degree",
   "Order",
   "StmLimit",
   "PotentialFile",
   "PotentialFileFullPath",
   "InputCoordinateSystem",
   "FixedCoordinateSystem",
   "TargetCoordinateSystem",
};

const Gmat::ParameterType
HarmonicField::PARAMETER_TYPE[HarmonicFieldParamCount - GravityBaseParamCount] =
{
   Gmat::INTEGER_TYPE,   // "MaxDegree",
   Gmat::INTEGER_TYPE,   // "MaxOrder",
   Gmat::INTEGER_TYPE,   // "Degree",
   Gmat::INTEGER_TYPE,   // "Order",
   Gmat::INTEGER_TYPE,   // "StmLimit",
   Gmat::FILENAME_TYPE,  // "PotentialFile",
   Gmat::FILENAME_TYPE,  // "potentialFileFullPath",
   Gmat::STRING_TYPE,    // "InputCoordinateSystem",
   Gmat::STRING_TYPE,    // "FixedCoordinateSystem",
   Gmat::STRING_TYPE,    // "TargetCoordinateSystem",
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  HarmonicField(const std::string &name, Integer deg, Integer ord)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the HarmonicField class
 * (default constructor).
 *
 * @param <name> parameter indicating the name of the object.
 * @param <deg>  maximum degree of he polynomials.
 * @param <ord>  maximum order of he polynomials.
 */
//------------------------------------------------------------------------------
HarmonicField::HarmonicField(const std::string &name, const std::string &typeName,
                             Integer maxDeg, Integer maxOrd) :
GravityBase             (typeName, name),
hMinitialized           (false),
maxDegree               (maxDeg),
maxOrder                (maxOrd),
degree                  (4),
order                   (4),
stmLimit                (100),
filename                (""),
filenameFullPath        (""),
fileRead                (false),
usingDefaultFile        (false),
isFirstTimeDefault      (true),
inputCSName             ("EarthMJ2000Eq"),
fixedCSName             ("EarthFixed"),
targetCSName            ("EarthMJ2000Eq"),
potPath                 (""),
inputCS                 (NULL),
fixedCS                 (NULL),
targetCS                (NULL),
eop                     (NULL)
{
   objectTypeNames.push_back("HarmonicField");
   parameterCount = HarmonicFieldParamCount;
   FileManager *fm = FileManager::Instance();
   potPath = fm->GetAbsPathname(bodyName + "_POT_PATH");
   #ifdef DEBUG_EOP_FILE
   MessageInterface::ShowMessage
      ("HarmonicField() constructor, this=<%p>, name='%s'\n", this, name.c_str());
   #endif
}


//------------------------------------------------------------------------------
//  ~HarmonicField()
//------------------------------------------------------------------------------
/**
 * This method destroys the HarmonicField object.
 * (destructor).
 */
//------------------------------------------------------------------------------
HarmonicField::~HarmonicField()
{
}


//------------------------------------------------------------------------------
//  HarmonicField(const HarmonicField& hf)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the HarmonicField class with the state equal
 * to the input object (copy constructor).
 *
 * @param <hf> the HarmonicField object to copy.
 */
//------------------------------------------------------------------------------
HarmonicField::HarmonicField(const HarmonicField& hf) :
GravityBase             (hf),
hMinitialized           (false),
maxDegree               (hf.maxDegree),
maxOrder                (hf.maxOrder),
degree                  (hf.degree),
order                   (hf.order),
stmLimit                (hf.stmLimit),
filename                (hf.filename),
filenameFullPath        (hf.filenameFullPath),
fileRead                (false),
usingDefaultFile        (hf.usingDefaultFile),
isFirstTimeDefault      (hf.isFirstTimeDefault),
inputCSName             (hf.inputCSName),
fixedCSName             (hf.fixedCSName),
targetCSName            (hf.targetCSName),
potPath                 (hf.potPath),
inputCS                 (NULL),
fixedCS                 (NULL),
targetCS                (NULL),
eop                     (hf.eop)
{
   #ifdef DEBUG_EOP_FILE
   MessageInterface::ShowMessage
      ("HarmonicField() copy constructor, this=<%p>, eop=<%p>\n", this, eop);
   #endif
}


//------------------------------------------------------------------------------
//  operator=(const HarmonicField& hf)
//------------------------------------------------------------------------------
/**
 * This method sets the state of the input HarmonicField class to 'this' object.
 *
 * @param <hf> the HarmonicField object whose values to use.
 */
//------------------------------------------------------------------------------
HarmonicField& HarmonicField::operator=(const HarmonicField& hf)
{
   if (&hf == this)
      return *this;

   GravityBase::operator=(hf);
   hMinitialized  = false;  // or hf.hMinitialized?
   maxDegree      = hf.maxDegree;
   maxOrder       = hf.maxOrder;
   degree         = hf.degree;
   order          = hf.order;
   stmLimit       = hf.stmLimit;
   filename       = hf.filename;
   filenameFullPath = hf.filenameFullPath;
   fileRead       = false;
   usingDefaultFile   = hf.usingDefaultFile;
   isFirstTimeDefault = hf.isFirstTimeDefault;
   inputCSName    = hf.inputCSName;
   fixedCSName    = hf.fixedCSName;
   targetCSName   = hf.targetCSName;
   potPath        = hf.potPath;
   inputCS        = hf.inputCS;
   fixedCS        = hf.fixedCS;
   targetCS       = hf.targetCS;
   eop            = hf.eop;
   #ifdef DEBUG_EOP_FILE
   MessageInterface::ShowMessage
      ("HarmonicField() operator=, this=<%p>, eop=<%p>\n", eop, this);
   #endif
   return *this;
}

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes this HarmonicField object.
 *
 * @return flag indicating success of initialization.
 */
//------------------------------------------------------------------------------
bool HarmonicField::Initialize()
{
   // Write default file message per GMAT session (LOJ: 2014.12.23)
   static bool writeDefaultFileMessage = true;
   
   #ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage
      ("HarmonicField::Initialize() entered, this=<%p>, eop=<%p>\n", this, eop);
   #endif
   if (!GravityBase::Initialize())
      return false;

   // Set body to use the same potential file (loj: 3/24/06)
   // if we want to use mu and radius from this file later
   //body->SetPotentialFilename(filename);
   
   if (solarSystem == NULL) throw ODEModelException(
                            "Solar System undefined for Harmonic Field " 
                             + instanceName);
   if (!inputCS) throw ODEModelException(
                 "Input coordinate system undefined for Harmonic Field "
                  + instanceName);
   if (!fixedCS) throw ODEModelException(
                 "Body fixed coordinate system undefined for Harmonic Field "
                  + instanceName);
   if (!targetCS) targetCS = inputCS;

   if (!eop) throw ODEModelException(
             "EOP file is undefined for Harmonic Field " + instanceName);

   hMinitialized = true;
   //if (usingDefaultFile && isFirstTimeDefault)
   if (writeDefaultFileMessage && usingDefaultFile && isFirstTimeDefault)
   {
      MessageInterface::ShowMessage
         ("Using default potential file \"%s\" for GravityField object \"%s\"\n",
          //filename.c_str(), instanceName.c_str());
          filenameFullPath.c_str(), instanceName.c_str());
      isFirstTimeDefault = false;
      writeDefaultFileMessage = false;
   }
   #ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage
      ("HarmonicField::Initialize() returning true, this=<%p>, eop=<%p>\n", this, eop);
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool CheckQualifier(const std::string &qualifier)
//------------------------------------------------------------------------------
/**
 * Ensures that the string qualifier identifies the body for this model
 *
 * This method is used, for example, with degree and order strings of the form
 *
 *    Forces.GravityField.Earth.Degree = 8;
 *    Forces.GravityField.Earth.Order  = 8;
 *
 * to ensure that the model is actually Earth based.
 *
 * @param qualifier The string qualifier
 *
 * @return true if the qualifier matches the model, false if not
 */
//------------------------------------------------------------------------------
bool HarmonicField::CheckQualifier(const std::string &qualifier,
      const std::string &forType)
{
   std::string theType = forType.substr(0, forType.find("."));

   // Validate type for this force
   if (!IsOfType(theType) && (theType != ""))
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Type check failed for %s in %s\n",
               theType.c_str(), typeName.c_str());
      #endif
      return false;
   }

   return (qualifier == bodyName);
}




//------------------------------------------------------------------------------
//  bool SetDegreeOrder(Integer deg, Integer ord)
//------------------------------------------------------------------------------
/**
 * This method sets the degree and order for this HarmonicField object.
 *
 * @param <deg> degree to use.
 * @param <ord> order to use.
 *
 * @return flag indicating success of the operation.
 */
//------------------------------------------------------------------------------
bool HarmonicField::SetDegreeOrder (Integer deg, Integer ord, Integer stmlimit)
{
    bool retval = true;

    if (deg <= maxDegree)
        degree = deg;
    else
    {
        degree = maxDegree;
        retval = false;
        MessageInterface::ShowMessage(
          "In HarmonicField, Potential Degree exceeds maximum degree in model");
    }

    if ((ord <= deg) && (ord <= maxOrder))
        order = ord;
    else
    {
        order = (deg < maxOrder ? deg : maxOrder );
        retval = false;
        MessageInterface::ShowMessage(
          "In HarmonicField, Potential Order exceeds valid range in model");
    }
    stmLimit = stmlimit;
    return retval;
}


//------------------------------------------------------------------------------
//  bool SetFilename(const std::string &fn, bool validateOnly = false)
//------------------------------------------------------------------------------
/**
 * This method sets the filename for this HarmonicField object.
 *
 * @param <fn> file name to use to get coefficients, etc.
 *
 * @return flag indicating success of the operation.
 */
//------------------------------------------------------------------------------
bool HarmonicField::SetFilename(const std::string &fn, bool validateOnly)
{
   #ifdef DEBUG_HARMONIC_FIELD_FILENAME
   MessageInterface::ShowMessage
      ("HarmonicField::SetFilename() entered for %s\n   filenameFullPath = %s\n   "
       "filename = %s\n   newname  = %s\n   validateOnly = %s\n", bodyName.c_str(),
       filenameFullPath.c_str(), filename.c_str(), fn.c_str(), validateOnly ? "true" : "false");
   MessageInterface::ShowMessage("   potPath  = %s\n", potPath.c_str());
   #endif
   
   bool hasDefaultIndicator = false;
   std::string newfn;
   if (fn.substr(0, 6) == "DFLT__")  // Must match Interpreter static const std::string defaultIndicator
   {
      newfn               = fn.substr(6,fn.npos-6);
      hasDefaultIndicator = true;
   }
   else
   {
      newfn               = fn;
      hasDefaultIndicator = false;
   }
   if (filename != newfn)
   {
      #ifdef DEBUG_HARMONIC_FIELD_FILENAME
      MessageInterface::ShowMessage
         ("HarmonicField::SetFilename(): hasDefaultIndicator = %s, newfn = %s\n",
          (hasDefaultIndicator? "true" : "false"), newfn.c_str());
      MessageInterface::ShowMessage("   potPath  = %s\n", potPath.c_str());
      #endif
      
      // Changed to use GmatBase::GetFullPathFileName() (LOJ: 2014.06.25)
      
      // // Add default pathname if none specified
      // if (newfn.find("/") == newfn.npos && newfn.find("\\") == newfn.npos)
      // {
      //    try
      //    {
      //       filename = potPath + newfn;
      //    }
      //    catch (GmatBaseException &e)
      //    {
      //       filename = newfn;
      //       MessageInterface::ShowMessage(e.GetFullMessage());
      //    }
      // }
      // else
      // {
      //    filename = newfn;
      // }
      
      std::string newFile = newfn;
      std::string fullPath;
      std::string potFileType = GmatStringUtil::ToUpper(GetBodyName()) + "_POT_PATH";
      
      // Do not write informational file location message if default file (LOJ: 2014.06.25)
      fullPath =
         GmatBase::GetFullPathFileName(newFile, GetName(), newfn, potFileType,
                                       true, "", false, !hasDefaultIndicator);
      
      // std::ifstream potfile(filename.c_str());
      //std::ifstream potfile(filenameFullPath.c_str());
      //if (!potfile) 
      if (fullPath == "") 
      {
         lastErrorMessage = "The file name \"" + newFile + "\" does not exist";
         if (!validateOnly)
            throw ODEModelException(lastErrorMessage);
      }
      
      if (!validateOnly)
      {
         filename = newfn;
         filenameFullPath = fullPath;
         if (body != NULL)
            //body->SetPotentialFilename(filename);
            body->SetPotentialFilename(filenameFullPath);
      }
   }
   
   #ifdef DEBUG_HARMONIC_FIELD_FILENAME
   MessageInterface::ShowMessage
      ("HarmonicField::SetFilename() returning true, filename = %s\n   "
       "filenameFullPath = '%s'\n", filename.c_str(), filenameFullPath.c_str());
   #endif
   
   fileRead = false;
   usingDefaultFile = hasDefaultIndicator;
   return true;
}


//---------------------------------
// inherited methods from GmatBase
//---------------------------------
//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
std::string HarmonicField::GetParameterText(const Integer id) const
{
   if ((id >= GravityBaseParamCount) && (id < HarmonicFieldParamCount))
      return PARAMETER_TEXT[id - GravityBaseParamCount];
   return GravityBase::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
Integer HarmonicField::GetParameterID(const std::string &str) const
{
   std::string useStr = str;
   if (useStr == "Model")
      useStr = "PotentialFile";
 
   for (Integer i = GravityBaseParamCount; i < HarmonicFieldParamCount; i++)
   {
      if (useStr == PARAMETER_TEXT[i - GravityBaseParamCount])
         return i;
   }
   return GravityBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType HarmonicField::GetParameterType(const Integer id) const
{
   if ((id >= GravityBaseParamCount) && (id < HarmonicFieldParamCount))
      return PARAMETER_TYPE[id - GravityBaseParamCount];
   return GravityBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
std::string HarmonicField::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];   
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param <id>    Integer ID for the requested parameter
 */
//------------------------------------------------------------------------------
Integer HarmonicField::GetIntegerParameter(const Integer id) const
{
   if (id == MAX_DEGREE)  return maxDegree;
   if (id == MAX_ORDER)   return maxOrder;
   if (id == DEGREE)      return degree;
   if (id == ORDER)       return order;
   if (id == STMLIMIT)    return stmLimit;

   return GravityBase::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    <id>    Integer ID for the parameter
 * @param    <value> The new value for the parameter
 */
//------------------------------------------------------------------------------
Integer HarmonicField::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == MAX_DEGREE)  return (maxDegree = value);
   if (id == MAX_ORDER)   return (maxOrder  = value);
   if (id == DEGREE)
   {
      if (value >= 0)
         degree = value;
      else
      {
         std::stringstream buffer;
         buffer << value;
         throw ODEModelException(
            "The value of \"" + buffer.str() + "\" for field \"" 
            + GetParameterText(id).c_str() + "\" on object \"" 
            + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [Integer >= 0 "
            "and < the maximum specified by the model, Order <= Degree].");
      }
      return degree;
   }
   if (id == ORDER)
   {
      if (value >= 0)
         order = value;
      else
      {
         std::stringstream buffer;
         buffer << value;
         throw ODEModelException(
            "The value of \"" + buffer.str() + "\" for field \"" 
            + GetParameterText(id).c_str() + "\" on object \"" 
            + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [Integer >= 0 "
            "and < the maximum specified by the model, Order <= Degree].");
      }
      return order;
   }
   if (id == STMLIMIT)
   {
      if (value >= 0)
         stmLimit = value;
      else
      {
         std::stringstream buffer;
         buffer << value;
         throw ODEModelException(
            "The value of \"" + buffer.str() + "\" for field \"" 
            + GetParameterText(id).c_str() + "\" on object \"" 
            + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [Integer >= 0].");
      }
      return stmLimit;
   }

   return GravityBase::SetIntegerParameter(id, value);
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <label>  parameter label
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
Integer HarmonicField::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <label> Integer ID for the parameter
 * @param    <value> The new value for the parameter
 */
//------------------------------------------------------------------------------
Integer HarmonicField::SetIntegerParameter(const std::string &label,
                                           const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <id>  Integer ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string HarmonicField::GetStringParameter(const Integer id) const
{
   if (id == FILENAME)
   {
      // Just return filename since filenameFullPath has path info (LOJ: 2014.06.30)
      return filename;
      
      // //return filename;
      // std::string::size_type index = filename.find_last_of("/\\");
      
      // // if path not found, just write filename
      // if (index == filename.npos)
      //    return filename;
      // else
      // {
      //    // if actual pathname is the same as the default path, write only name part
      //    std::string actualPath = filename.substr(0, index+1);
      //    std::string fname = filename;
      //    if (potPath == actualPath)
      //       fname = filename.substr(index+1);
         
      //    return fname;
      // }
   }
   
   if (id == POT_FILE_FULLPATH)
      return filenameFullPath;
   
   if (id == INPUT_COORD_SYSTEM)  return inputCSName;
   if (id == FIXED_COORD_SYSTEM)  return fixedCSName;
   if (id == TARGET_COORD_SYSTEM) return targetCSName;
   return GravityBase::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <id>    Integer ID for the parameter
 * @param    <value> The new value for the parameter
 */
//------------------------------------------------------------------------------
bool HarmonicField::SetStringParameter(const Integer id,
                                       const std::string &value)
{
   if (id == FILENAME)
   {
      if (GmatStringUtil::IsBlank(value))
      {
         ODEModelException ome;
         ome.SetDetails(errorMessageFormatUnnamed.c_str(),
                       value.c_str(), "PotentialFile", "non-blank file name");
         throw ome;
      }

      #ifdef DEBUG_HARMONIC_FIELD_FILENAME
      MessageInterface::ShowMessage
         ("HarmonicField::SetStringParameter() new PotentialFile = '%s'\n", value.c_str());
      #endif
      std::string newValue = value;
      
      // if value has no file extension, add .cof as default (loj: 2008.10.14)
      if (value.find(".") == value.npos)
         newValue = value + ".cof";
      
      return SetFilename(newValue);  
   }
   if (id == INPUT_COORD_SYSTEM)
   {
      #ifdef DEBUG_HARMONIC_FIELD
         char str[1024];
         strcpy(str, value.c_str());
         
         MessageInterface::ShowMessage(
                "Setting input coordinate system name to \"%s\"\n", str);
      #endif
      inputCSName = value;
      
      return true;
   }
   if (id == FIXED_COORD_SYSTEM)
   {
      #ifdef DEBUG_HARMONIC_FIELD
         char str[1024];
         strcpy(str, value.c_str());
         
         MessageInterface::ShowMessage(
                "Setting fixed coordinate system name to \"%s\"\n", str);
      #endif
      fixedCSName = value;
      
      return true;
   }
   if (id == TARGET_COORD_SYSTEM)
   {
      #ifdef DEBUG_HARMONIC_FIELD
         char str[1024];
         strcpy(str, value.c_str());
         
         MessageInterface::ShowMessage(
                "Setting target coordinate system name to \"%s\"\n", str);
      #endif
      targetCSName = value;
      
      return true;
   }
   if (id == BODY_NAME)
   {      
      if (GravityBase::SetStringParameter(id, value))
      {
         // set default potential file path for the body
         FileManager *fm = FileManager::Instance();
         try
         {
            potPath = fm->GetAbsPathname(bodyName + "_POT_PATH");
         }
         catch (BaseException &ex)
         {
            //MessageInterface::ShowMessage("**** WARNING ****: %s\n",
            //      ex.GetFullMessage().c_str());
            ; // ignore this for now - GMT-2873
         }
         
         #ifdef DEBUG_HARMONIC_FIELD
         MessageInterface::ShowMessage
            ("Setting potential file path to \"%s\"\n", potPath.c_str());
         #endif
         
         fixedCSName = value + "Fixed";
         return true;
      }
      return false;
   }
   
   return GravityBase::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <label>  Integer ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string HarmonicField::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <label> Integer ID for the parameter
 * @param    <value> The new value for the parameter
 */
//------------------------------------------------------------------------------
bool HarmonicField::SetStringParameter(const std::string &label,
                                       const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const UnsignedInt type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the HarmonicField class.
 *
 * @param type  type of the reference object requested
 * @param name  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* HarmonicField::GetRefObject(const UnsignedInt type,
                                      const std::string &name)
{
   switch (type)
   {
      case Gmat::COORDINATE_SYSTEM:
         if ((inputCS) && (name == inputCSName))
            return inputCS;
         if ((fixedCS) && (name == fixedCSName))
            return fixedCS;
         if ((targetCS) && (name == targetCSName))
            return targetCS;
         break;

      default:
         break;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return GravityBase::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Returns the names of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.  Gmat::UnknownObject returns all of the
 *               ref objects.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& HarmonicField::GetRefObjectNameArray(const UnsignedInt type)
{
   static StringArray refs;
   
   // Start from an empty list
   refs.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT)
   {
      refs = GravityBase::GetRefObjectNameArray(type);
      //refs.clear(); // why would I want to clear this????
      
      refs.push_back(inputCSName);
      refs.push_back(fixedCSName);
      refs.push_back(targetCSName);
      
      #ifdef DEBUG_REFERENCE_SETTING
         MessageInterface::ShowMessage("+++ReferenceObjects:\n");
         for (StringArray::iterator i = refs.begin(); i != refs.end(); ++i)
            MessageInterface::ShowMessage("   %s\n", i->c_str());
      #endif
      
      return refs;
   }
   
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      refs = GravityBase::GetRefObjectNameArray(type);
      
      fixedCSName = bodyName + "Fixed";

      refs.push_back(inputCSName);
      refs.push_back(fixedCSName);
      refs.push_back(targetCSName);
      
      #ifdef DEBUG_REFERENCE_SETTING
         MessageInterface::ShowMessage("+++ReferenceObjects:\n");
         for (StringArray::iterator i = refs.begin(); i != refs.end(); ++i)
            MessageInterface::ShowMessage("   %s\n", i->c_str());
      #endif
      
      return refs;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return GravityBase::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the HarmonicField class.
 *
 * @param obj   pointer to the reference object
 * @param type  type of the reference object
 * @param name  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool HarmonicField::SetRefObject(GmatBase *obj,
                                 const UnsignedInt type,
                                 const std::string &name)
{
   if (obj->IsOfType(Gmat::COORDINATE_SYSTEM))
   {
      if (name == inputCSName)
      {
         #ifdef DEBUG_REFERENCE_SETTING
            MessageInterface::ShowMessage("Setting %s as input CS for %s\n",
                                          name.c_str(), instanceName.c_str());
         #endif
         inputCS = (CoordinateSystem*) obj;
      }
      if (name == fixedCSName)
      {
         #ifdef DEBUG_REFERENCE_SETTING
            MessageInterface::ShowMessage("Setting %s as body fixed CS for %s\n%s\n",
                                          name.c_str(), instanceName.c_str(),
                                          obj->GetGeneratingString(Gmat::SCRIPTING).c_str());
         #endif
         fixedCS = (CoordinateSystem*) obj;
      }
      if (name == targetCSName)
      {
         #ifdef DEBUG_REFERENCE_SETTING
            MessageInterface::ShowMessage("Setting %s as target CS for %s\n",
                                          name.c_str(), instanceName.c_str());
         #endif
         targetCS = (CoordinateSystem*) obj;
      }
      return true;
   }
   
   // Not handled here -- invoke the next higher SetRefObject call
   return GravityBase::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets the force model origin, and derived the base coordinate 
 * system names from that SpacePoint.
 *
 * @param toBody  pointer to the force model origin.
 */
//------------------------------------------------------------------------------
void HarmonicField::SetForceOrigin(CelestialBody* toBody)
{
   GravityBase::SetForceOrigin(toBody);
   
   std::string originName = toBody->GetName();
   inputCSName = originName + "MJ2000Eq";
   targetCSName = inputCSName;
}



//------------------------------------------------------------------------------
//  void SetEopFile(EopFile *eopF)
//------------------------------------------------------------------------------
/**
 * Method to set EopFile for this system.
 *
 * @param eopF  the eopFile.
 *
 */
//------------------------------------------------------------------------------
void HarmonicField::SetEopFile(EopFile *eopF)
{
   #ifdef DEBUG_EOP_FILE
   MessageInterface::ShowMessage
      ("HarmonicField::SetEopFile() entered, this=<%p>, eopF=<%p>\n", this, eopF);
   #endif
   eop = eopF;
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool HarmonicField::IsParameterReadOnly(const Integer id) const
{
   if (id < GravityBaseParamCount)
      return GravityBase::IsParameterReadOnly(id);
   
   if (id >= HarmonicFieldParamCount)
      throw ODEModelException(
         "Attempting to determine accessibility of a parameter outside of the "
         "scope of a HarmonicField object.");
   
   if ((id == DEGREE) || (id == ORDER) || (id == STMLIMIT) || (id == FILENAME))
      return false;
   
   if (id == POT_FILE_FULLPATH)
      return true;
   
   return true;
}


//------------------------------------------------------------------------------
// bool IsParameterValid(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool HarmonicField::IsParameterValid(const Integer id,
                                     const std::string &value)
{
   #ifdef DEBUG_VALIDATION
   MessageInterface::ShowMessage
      ("HarmonicField::IsParameterValid() entered, id=%d, value='%s'\n", id, value.c_str());
   #endif
   
   bool retval = true;
   if (id == FILENAME)
   {
      #ifdef DEBUG_VALIDATION
      MessageInterface::ShowMessage("   Validating FILE_NAME\n");
      #endif
      if (!SetFilename(value, true))
         retval = false;
   }
   
   #ifdef DEBUG_VALIDATION
   MessageInterface::ShowMessage
      ("CelestialBody::IsParameterValid() returning %d\n", retval);
   #endif
   return retval;
}

//------------------------------------------------------------------------------
// bool IsParameterValid(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool HarmonicField::IsParameterValid(const std::string &label,
                                     const std::string &value)
{
   return IsParameterValid(GetParameterID(label), value);
}


//---------------------------------
// protected methods
//---------------------------------
// N/A
