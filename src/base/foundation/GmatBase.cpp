//$Id$
//------------------------------------------------------------------------------
//                                  GmatBase
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
// Author: Darrel J. Conway
// Created: 2003/09/25
//
/**
 * Implementation for the base class for all GMAT extensible objects
 *
 * The following class hierarchy trees use this class as their basis:
 *
 *     Asset (hence Spacecraft and GroundStation)
 *     CelestialBody
 *     Propagator
 *     PhysicalModel (hence Force and ForceModel)
 *     PropConfig
 *     Parameter
 *     Command
 *
 * Every class that users can use to derive new classes, or that need to be
 * accessed through generic pointers, should be derived from this class to
 * ensure consistent interfaces accessed by the GMAT control systems (i.e. the
 * Moderator, FactoryManager, Configuration, Interpreter, and Sandbox, along
 * with the GUIInterpreter).
 */
//------------------------------------------------------------------------------


#include "GmatBase.hpp"
#include "GmatGlobal.hpp"  // for GetDataPrecision(), IsWritingGmatKeyword()
#include "Moderator.hpp"
#include <sstream>         // for StringStream
#include "StringUtil.hpp"
#include "FileManager.hpp" // for FindPath()
#include "FileUtil.hpp"    // for ParseFileName()
#include "MessageInterface.hpp"
#include "RHSEquation.hpp"

//#define DEBUG_OBJECT_TYPE_CHECKING
//#define DEBUG_COMMENT_LINE
//#define DEBUG_COMMENTS_ATTRIBUTE
//#define DEBUG_GENERATING_STRING
//#define DEBUG_OWNED_OBJECT_STRINGS
//#define DEBUG_WRITE_PARAM
//#define DEBUG_CLOAKING
//#define DEBUG_WRAPPER_REF
//#define DEBUG_FILE_PATH

//#define DEBUG_API

/// Set the static "undefined" parameters
//const Real        GmatBase::REAL_PARAMETER_UNDEFINED = -987654321.0123e-45;
const Real              GmatBase::REAL_PARAMETER_UNDEFINED = GmatRealConstants::REAL_UNDEFINED;
const Integer           GmatBase::INTEGER_PARAMETER_UNDEFINED = -987654321;
const UnsignedInt       GmatBase::UNSIGNED_INT_PARAMETER_UNDEFINED = 987654321;
const std::string       GmatBase::STRING_PARAMETER_UNDEFINED = "STRING_PARAMETER_UNDEFINED";
const StringArray       GmatBase::STRINGARRAY_PARAMETER_UNDEFINED = StringArray(1,
                        GmatBase::STRING_PARAMETER_UNDEFINED);
const IntegerArray      GmatBase::INTEGERARRAY_PARAMETER_UNDEFINED = IntegerArray(1,
                        GmatBase::INTEGER_PARAMETER_UNDEFINED);
const UnsignedIntArray  GmatBase::UNSIGNED_INTARRAY_PARAMETER_UNDEFINED = UnsignedIntArray(1,
                        GmatBase::UNSIGNED_INT_PARAMETER_UNDEFINED);
const Rvector           GmatBase::RVECTOR_PARAMETER_UNDEFINED = Rvector(1,
                        GmatBase::REAL_PARAMETER_UNDEFINED);
const Rmatrix           GmatBase::RMATRIX_PARAMETER_UNDEFINED = Rmatrix(1,1,
                        GmatBase::REAL_PARAMETER_UNDEFINED);


const Gmat::ParameterType GmatBase::PARAMETER_TYPE[GmatBaseParamCount] =
{
   Gmat::RMATRIX_TYPE,
};

const std::string GmatBase::PARAMETER_LABEL[GmatBaseParamCount] =
{
   "Covariance",
};




/**
 * Build the list of type names
 *
 * This list needs to be synchronized with the Gmat::ParameterType list found in
 * gmatutil/include/utildefs.hpp
 */
const std::string
GmatBase::PARAM_TYPE_STRING[Gmat::TypeCount] =
{
   "Integer",     "UnsignedInt", "UnsignedIntArray", "IntegerArray", "Real",
   "RealArray",
   "RealElement", "String",      "StringArray",      "Boolean",      "BooleanArray",
   "Rvector",     "Rmatrix",     "Time",             "Object",       "ObjectArray",
   "OnOff",       "Enumeration", "Filename",         "Color",        "GmatTime",
   "Generic",     "Equation"
};

/**
 * Build the list of object type names
 *
 * This list needs to be synchronized with the UnsignedInt list found in
 * base/include/gmatdefs.hpp
 */
const std::string
GmatBase::OBJECT_TYPE_STRING[Gmat::UNKNOWN_OBJECT - Gmat::SPACECRAFT+1] =
{
   "Spacecraft",    "Formation",        "SpaceObject",      "GroundStation",    "Plate",         "Burn",
   "ImpulsiveBurn", "FiniteBurn",       "Command",          "Propagator",       "ODEModel",
   "PhysicalModel", "TransientForce",   "Interpolator",     "SolarSystem",      "SpacePoint",
   "CelestialBody", "CalculatedPoint",  "LibrationPoint",   "Barycenter",       "Atmosphere",
   "Parameter",     "Variable",         "Array",            "String",           "StopCondition",
   "Solver",        "Subscriber",       "ReportFile",       "XYPlot",           "OrbitView",
   "DynamicDataDisplay", "Groundtrack",
   "EphemerisFile", "PropSetup",        "Function",         "FuelTank",         "Thruster",
   "ChemicalThruster", "ElectricThruster","ChemicalTank", "ElectricTank",
   "FieldOfView",   "ConicalFOV",    "RectangularFOV",   "CustomFOV",
   "PowerSystem", "SolarPowerSystem", "NuclearPowerSystem",
   "Hardware",      "CoordinateSystem", "AxisSystem",       "Attitude",         "MathNode",
   "MathTree",      "BodyFixedPoint",   "Event",            "EventLocator",     "DataInterface", 
   "MeasurementModel", //"CoreMeasurement",
   "ErrorModel",       //"TrackingData",     "TrackingSystem",   
   "DataStream",
   "DataFile",      "ObType",           "DataFilter",        "Interface",        "MediaCorrection",  "Imager",
   "Sensor",        "RFHardware",       "Antenna",           "ThrustSegment",    "PlanetographicRegion",
   "UnknownObject"
};
/**
 * Build the list of automatic global settings
 *
 * This list needs to be synchronized with the UnsignedInt list found in
 * base/include/gmatdefs.hpp
 *
 * Current automatic global objects: Propagator, CoordinateSystem, Function, PropSetup
 */
// There is some propblem if we make ODEModel global, so turned it off (loj: 2008.11.10)
const bool
GmatBase::AUTOMATIC_GLOBAL_FLAGS[Gmat::UNKNOWN_OBJECT - Gmat::SPACECRAFT+1] =
{
   false,     false,     false,     false,     false,     false,
   false,     false,     false,     true ,     false,
   false,     false,     false,     true,      false,
   true,      true,      true,      true,      false,
   false,     false,     false,     false,     false,
   false,     false,     false,     false,     false,
   false,     false,
   false,     true,      true,      false,     false,
   false,     false,     false,     false,
   false,     false,     false,     false,
   false,     false,     false,
   false,     true,      false,     false,     false,
   false,     false,     false,     false,     false,
   false,                                   //false,     
   false,                                   //false,     false,     
   false,
   false,     false,     false,     false,     false,     false,
   false,     false,     false,     false
};



/// initialize the count of instances
Integer GmatBase::instanceCount = 0;

//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  GmatBase(UnsignedInts typeId, std::string &typeStr, std::string &nomme)
//---------------------------------------------------------------------------
/**
 * Constructs base GmatBase structures used in GMAT extensible classes.
 *
 * @param <typeId> UnsignedInts enumeration for the object.
 * @param <typeStr> GMAT script string associated with this type of object.
 * @param <nomme> Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for GmatBase.  Derived classes
 *       must pass in the typeId and typeStr parameters.
 */
//------------------------------------------------------------------------------
GmatBase::GmatBase(const UnsignedInt typeId, const std::string &typeStr,
                   const std::string &nomme) :
   parameterCount             (GmatBaseParamCount),
   typeName                   (typeStr),
   instanceName               (nomme),
   instanceFullName           (nomme),
   scriptCreatedFrom          (""),
   isCreatedFromMainScript    (true),
   forceGenerateObjectString  (false),
   type                       (typeId),
   ownedObjectCount           (0),
   isInitialized              (false),
   callbackExecuting          (false),
   commentLine                (""),
   inlineComment              (""),
   showPrefaceComment         (true),
   showInlineComment          (true),
   cloaking                   (false),
   blockCommandModeAssignment (true),
   writeEmptyStringArray      (false),
   hasPrecisionTime           (false),
   isAPIObject                (false),
   hasEquation                (false), 
   scriptLineNumber           (0)
{
   attributeCommentLines.clear();
   attributeInlineComments.clear();
   
   if (type == Gmat::COMMAND)
   {
      deprecatedMessageFormat =
         "***WARNING*** \"%s\" field of " + typeName + " is deprecated and will "
         "be removed from a future build; please use \"%s\" instead.\n";
      errorMessageFormat =
         "The value of \"%s\" for field \"%s\" on command \""
         + typeName +  "\" is not an allowed value.\n"
         "The allowed values are: [%s]";
      errorMessageFormatUnnamed =
         "The value of \"%s\" for field \"%s\" on a command of type \""
         + typeName +  "\" is not an allowed value.\n"
         "The allowed values are: [%s]";
   }
   else
   {
      deprecatedMessageFormat =
         "*** WARNING *** \"%s\" field of " + typeName + " on object \"%s\" is "
         "deprecated and will be removed from a future build; please use \"%s\" "
         "instead.\n";
      errorMessageFormat =
         "The value of \"%s\" for field \"%s\" on object \""
         + instanceName +  "\" is not an allowed value.\n"
         "The allowed values are: [%s]";
      errorMessageFormatUnnamed =
         "The value of \"%s\" for field \"%s\" on an object of type \""
         + typeName +  "\" is not an allowed value.\n"
         "The allowed values are: [%s]";
   }
   
   // Set the isGlobal flag appropriately
   isGlobal = AUTOMATIC_GLOBAL_FLAGS[type - Gmat::SPACECRAFT];
	isAutomaticGlobal = isGlobal;
   isLocal = false;
   
   // one more instance - add to the instanceCount
   ++instanceCount;
}


//---------------------------------------------------------------------------
//  ~GmatBase()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GmatBase::~GmatBase()
{
   // subtract this instance from the instanceCount
   --instanceCount;

   // delete all GmatBase objects created in this
   for (Integer i = 0; i < createdObjects.size(); ++i)
   {
      if (createdObjects[i])
         delete createdObjects[i];
   }
}


//---------------------------------------------------------------------------
//  GmatBase(const GmatBase &a)
//---------------------------------------------------------------------------
/**
 * Copy Constructor for base GmatBase structures.
 *
 * @param <a> The original that is being copied.
 */
//------------------------------------------------------------------------------
GmatBase::GmatBase(const GmatBase &a) :
    parameterCount            (a.parameterCount),
    typeName                  (a.typeName),
    //instanceName    ("CopyOf"+a.instanceName),
    instanceName              (a.instanceName),
    instanceFullName          (a.instanceFullName),
    scriptCreatedFrom         (a.scriptCreatedFrom),
    isCreatedFromMainScript   (a.isCreatedFromMainScript),
    forceGenerateObjectString (a.forceGenerateObjectString),
    type                      (a.type),
    ownedObjectCount          (a.ownedObjectCount),
    generatingString          (a.generatingString),
    objectTypes               (a.objectTypes),
    objectTypeNames           (a.objectTypeNames),
    isInitialized             (a.isInitialized),
    hasPrecisionTime          (a.hasPrecisionTime),
    isGlobal                  (a.isGlobal),
	 isAutomaticGlobal         (a.isAutomaticGlobal),
    isLocal                   (a.isLocal),
    callbackExecuting         (false),
    errorMessageFormat        (a. errorMessageFormat),
    errorMessageFormatUnnamed (a. errorMessageFormatUnnamed),
    deprecatedMessageFormat   (a. deprecatedMessageFormat),
    commentLine               (a.commentLine),
    inlineComment             (a.inlineComment),
    attributeCommentLines     (a.attributeCommentLines),
    attributeInlineComments   (a.attributeInlineComments),
    showPrefaceComment        (a.showPrefaceComment),
    showInlineComment         (a.showInlineComment),
    cloaking                  (a.cloaking),
    blockCommandModeAssignment(a.blockCommandModeAssignment),
    covarianceList            (a.covarianceList),
    covarianceIds             (a.covarianceIds),
    covarianceSizes           (a.covarianceSizes),
    covariance                (a.covariance),
    writeEmptyStringArray     (a.writeEmptyStringArray),
    isAPIObject               (a.isAPIObject),
	 solveForList              (a.solveForList),
    objectMethods             (a.objectMethods),
    methodRequiredInputCount  (a.methodRequiredInputCount),
    methodTotalInputCount     (a.methodTotalInputCount),
    methodInputs              (a.methodInputs),
    hasEquation               (a.hasEquation), 
    scriptLineNumber          (a.scriptLineNumber)
{
   // one more instance - add to the instanceCount
   ++instanceCount;
}


//---------------------------------------------------------------------------
//  GmatBase& operator=(const GmatBase &a)
//---------------------------------------------------------------------------
/**
 * Assignment operator for GmatBase structures.
 *
 * @param <a> The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
GmatBase& GmatBase::operator=(const GmatBase &a)
{
   // Don't do anything if copying self
   if (&a == this)
      return *this;

   parameterCount            = a.parameterCount;
   typeName                  = a.typeName;
   // We don't want to copy instanceName (loj: 2008.02.15)
   //instanceName              = a.instanceName;
   scriptCreatedFrom         = a.scriptCreatedFrom;
   isCreatedFromMainScript   = a.isCreatedFromMainScript;
   forceGenerateObjectString = a.forceGenerateObjectString;
   type                      = a.type;
   ownedObjectCount          = a.ownedObjectCount;
   generatingString          = a.generatingString;
   objectTypes               = a.objectTypes;
   objectTypeNames           = a.objectTypeNames;

   isInitialized             = a.isInitialized;

   // don't want to modify isGlobal here
	isAutomaticGlobal         = a.isAutomaticGlobal;
   isLocal                   = a.isLocal;
   callbackExecuting         = false;
   errorMessageFormat        = a. errorMessageFormat;
   errorMessageFormatUnnamed = a. errorMessageFormatUnnamed;
   deprecatedMessageFormat   = a.deprecatedMessageFormat;
   commentLine               = a.commentLine;
   inlineComment             = a.inlineComment;
   attributeCommentLines     = a.attributeCommentLines;
   attributeInlineComments   = a.attributeInlineComments;
   showPrefaceComment        = a.showPrefaceComment;
   showInlineComment         = a.showInlineComment;
   cloaking                  = a.cloaking;
   blockCommandModeAssignment= a.blockCommandModeAssignment;
   covarianceList            = a.covarianceList;
   covarianceIds             = a.covarianceIds;
   covarianceSizes           = a.covarianceSizes;
   covariance                = a.covariance;
   writeEmptyStringArray     = a.writeEmptyStringArray;
   hasPrecisionTime          = a.hasPrecisionTime;
   isAPIObject               = a.isAPIObject;
   solveForList              = a.solveForList;

   // Scriptable methods
   objectMethods             = a.objectMethods;
   methodRequiredInputCount  = a.methodRequiredInputCount;
   methodTotalInputCount     = a.methodTotalInputCount;
   methodInputs              = a.methodInputs;
   hasEquation               = a.hasEquation;
   scriptLineNumber          = a.scriptLineNumber;

   return *this;
}


//---------------------------------------------------------------------------
//  UnsignedInts GetType() const
//---------------------------------------------------------------------------
/**
 * Retrieve the enumerated base type for the object
 *
 * @return Enumeration value for this object
 */
//------------------------------------------------------------------------------
UnsignedInt GmatBase::GetType() const
{
   return type;
}

//---------------------------------------------------------------------------
//  bool SetName(const char *who, const std::string &oldName = "")
//---------------------------------------------------------------------------
/**
 * @see SetName(const std::string &who, const std::string &oldName)
 */
//------------------------------------------------------------------------------
bool GmatBase::SetName(const char *who, const std::string &oldName)
{
   return SetName(std::string(who), oldName);
}

//---------------------------------------------------------------------------
//  bool SetName(const std::string &who, const std::string &oldName = "")
//---------------------------------------------------------------------------
/**
* Set the name for this instance.
 *
 * @param <who> the object's name.
 * @param <oldName> the object's old name to be set mainly for subscribers
 * @return true if the name was changed, false if an error was encountered.
 *
 * @note Some classes are unnamed.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetName(const std::string &who, const std::string &oldName)
{
   instanceName = who;
   return true;
}


//---------------------------------------------------------------------------
//  std::string ToString() const
//---------------------------------------------------------------------------
/**
 * Retrieves a string representation of this object
 *
 * @return A string representation of this object
 */
 //------------------------------------------------------------------------------
std::string GmatBase::ToString() const
{
   std::string outString = "Object of type " + GetTypeName() + " named " + GetName();
   return outString;
}

//---------------------------------------------------------------------------
//  Integer GetParameterCount() const
//---------------------------------------------------------------------------
/**
 * Find out how many parameters are accessible for this object.
 *
 * @return The number of parameters.
 */
//------------------------------------------------------------------------------
Integer GmatBase::GetParameterCount() const
{
   return parameterCount;
}


//---------------------------------------------------------------------------
//  bool IsOfType(UnsignedInt ofType)
//---------------------------------------------------------------------------
/**
* Detects if the object is a specified type.
 *
 * @param <ofType> The type that is being checked.
 *
 * @return true is the class was derived from the type, false if not.
 */
//---------------------------------------------------------------------------
bool GmatBase::IsOfType(UnsignedInt ofType) const
{
   #ifdef DEBUG_OBJECT_TYPE_CHECKING
   MessageInterface::ShowMessage
      ("Checking to see if %s is of type %d (Actual type is %d)\n",
       instanceName.c_str(), ofType, type);
   #endif

   if (std::find(objectTypes.begin(), objectTypes.end(), ofType) !=
       objectTypes.end())
   {
      #ifdef DEBUG_OBJECT_TYPE_CHECKING
      MessageInterface::ShowMessage("   Object %s is the requested type\n",
                                    instanceName.c_str());
      #endif
      return true;
   }

   #ifdef DEBUG_OBJECT_TYPE_CHECKING
   MessageInterface::ShowMessage
      ("   Not the requested type; current types are [");
   for (ObjectTypeArray::const_iterator i = objectTypes.begin();
        i != objectTypes.end(); ++i)
   {
      if (i != objectTypes.begin())
         MessageInterface::ShowMessage(", ");
      MessageInterface::ShowMessage("%d", *i);
   }
   MessageInterface::ShowMessage("]\n");
   #endif

   return false;
}


//---------------------------------------------------------------------------
//  bool IsOfType(std::string typeDescription)
//---------------------------------------------------------------------------
/**
* Detects if the object is a specified type.
 *
 * @param <typeDescription> The string describing the type.
 *
 * @return true is the class was derived from the type, false if not.
 */
//---------------------------------------------------------------------------
bool GmatBase::IsOfType(const std::string &typeDescription) const
{
   #ifdef DEBUG_OBJECT_TYPE_CHECKING
   MessageInterface::ShowMessage
      ("Checking to see if %s is of type %s (Actual type is %s)\n",
       instanceName.c_str(), typeDescription.c_str(), typeName.c_str());
   #endif

   if (std::find(objectTypeNames.begin(), objectTypeNames.end(),
                 typeDescription) != objectTypeNames.end())
   {
      #ifdef DEBUG_OBJECT_TYPE_CHECKING
      MessageInterface::ShowMessage("   Object %s is the requested type\n",
                                    instanceName.c_str());
      #endif
      return true;
   }
   
   #ifdef DEBUG_OBJECT_TYPE_CHECKING
   MessageInterface::ShowMessage
      ("   Not the requested type; current types are [");
   for (StringArray::const_iterator i = objectTypeNames.begin();
        i != objectTypeNames.end(); ++i)
   {
      if (i != objectTypeNames.begin())
         MessageInterface::ShowMessage(", ");
      MessageInterface::ShowMessage("%s", (*i).c_str());
   }
   MessageInterface::ShowMessage("]\n");
   #endif

   return false;
}

//------------------------------------------------------------------------------
// StringArray GetTypeNames() const
//------------------------------------------------------------------------------
StringArray GmatBase::GetTypeNames() const
{
   return objectTypeNames;
}

//------------------------------------------------------------------------------
// bool GmatBase::IsInitialized()
//------------------------------------------------------------------------------
/**
 * Returns the initialization flag
 *
 * @return true if the object thinks it is initialized, false if not
 */
//------------------------------------------------------------------------------
bool GmatBase::IsInitialized()
{
   return isInitialized;
}


//---------------------------------------------------------------------------
// void GmatBase::SetShowPrefaceComment(bool show = true)
//---------------------------------------------------------------------------
/*
 * Sets show preface comment flag.
 */
//---------------------------------------------------------------------------
void GmatBase::SetShowPrefaceComment(bool show)
{
   showPrefaceComment = show;
}

//---------------------------------------------------------------------------
// void SetShowInlineComment(bool show = true)
//---------------------------------------------------------------------------
/*
 * Sets show inline comment flag.
 */
//---------------------------------------------------------------------------
void GmatBase::SetShowInlineComment(bool show)
{
   showInlineComment = show;
}

//---------------------------------------------------------------------------
// bool GetShowPrefaceComment()
//---------------------------------------------------------------------------
/*
 * @return Show preface comment flag
 */
//---------------------------------------------------------------------------
bool GmatBase::GetShowPrefaceComment()
{
   return showPrefaceComment;
}

//---------------------------------------------------------------------------
// bool GetShowInlineComment()
//---------------------------------------------------------------------------
/*
 * @return Show inline comment flag
 */
//---------------------------------------------------------------------------
bool GmatBase::GetShowInlineComment()
{
   return showInlineComment;
}


//---------------------------------------------------------------------------
//  std::string GetRefObjectName(const UnsignedInt type) const
//---------------------------------------------------------------------------
/**
 * Returns the name of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.
 *
 * @return The name of the reference object.
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetRefObjectName(const UnsignedInt type) const
{
   throw GmatBaseException("Reference Object not defined for " + typeName +
                           " named \"" + instanceName + "\"\n");
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Returns flag indicating whether GetRefObjectTypeArray() is implemented or not.
 * Currently this method is used in the Interpreter::FinalPass() for validation.
 *
 * @note Derived classes should override this if GetRefObjectTypeArray() is
 *       implemented
 */
//------------------------------------------------------------------------------
bool GmatBase::HasRefObjectTypeArray()
{
   return false;
}


//---------------------------------------------------------------------------
//  const ObjectTypeArray& GetRefObjectTypeArray()
//---------------------------------------------------------------------------
/**
 * Returns the types of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @return The types of the reference object.
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& GmatBase::GetRefObjectTypeArray()
{
   // should return empty array
   refObjectTypes.clear();
   return refObjectTypes;
}


//---------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//---------------------------------------------------------------------------
/**
 * Returns the names of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& GmatBase::GetRefObjectNameArray(const UnsignedInt type)
{
   // Changed to return empty array (LOJ: 2010.05.13)
   refObjectNames.clear();
   return refObjectNames;
}

//---------------------------------------------------------------------------
//  bool SetRefObjectName(const UnsignedInt type, const char *name)
//---------------------------------------------------------------------------
/**
 * Sets the name of the reference object.  (Derived classes should implement
 * this as needed.)
 *
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetRefObjectName(const UnsignedInt type,
                                const char *name)
{
   return SetRefObjectName(type, std::string(name));
}

//---------------------------------------------------------------------------
//  bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//---------------------------------------------------------------------------
/**
 * Sets the name of the reference object.  (Derived classes should implement
 * this as needed.)
 *
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetRefObjectName(const UnsignedInt type,
                                const std::string &name)
{
   std::string objName = typeName;
   if (GetType() != Gmat::COMMAND)
      objName = objName + " named \"" + instanceName + "\"";

   throw GmatBaseException("SetRefObjectName(" + GetObjectTypeString(type) +
                           ", " + name + ") not defined for " + objName);
}

//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * @param <type> reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 *
 * @return true if object name changed, false if not.
 */
//------------------------------------------------------------------------------
bool GmatBase::RenameRefObject(const UnsignedInt type,
                               const std::string &oldName,
                               const std::string &newName)
{
   MessageInterface::ShowMessage
      ("*** ERROR *** The object type:%s needs RenameRefObject() "
       "implementation.\n", GetTypeName().c_str());

   return false;
}

//---------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//---------------------------------------------------------------------------
/**
 * Returns the reference object pointer.  (Derived classes should implement
 * this method as needed.)
 *
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return reference object pointer.
 */
//------------------------------------------------------------------------------
GmatBase* GmatBase::GetRefObject(const UnsignedInt type,
                                 const std::string &name)
{
   std::string objName = typeName;
   if (GetType() != Gmat::COMMAND)
      objName = objName + " named \"" + instanceName + "\"";

   throw GmatBaseException("GetRefObject(" + GetObjectTypeString(type) +
                           ", " + name + ") not defined for " + objName);
}


//---------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name,
//                        const Integer index)
//---------------------------------------------------------------------------
/**
 * Returns the reference object pointer.  (Derived classes should implement
 * this method as needed.)
 *
 * @param type type of the reference object.
 * @param name name of the reference object.
 * @param index Index into the object array.
 *
 * @return reference object pointer.
 */
//------------------------------------------------------------------------------
GmatBase* GmatBase::GetRefObject(const UnsignedInt type,
                                 const std::string &name, const Integer index)
{
   std::stringstream indexString;
   indexString << index;
   std::string objName = typeName;
   if (GetType() != Gmat::COMMAND)
      objName = objName + " named \"" + instanceName + "\"";

   throw GmatBaseException("GetRefObject(" + GetObjectTypeString(type) +
                           ", " + name + ", " + indexString.str() +
                           ") not defined for " + objName);
}

//---------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//---------------------------------------------------------------------------
/**
 * Sets the reference object.  (Derived classes should implement
 * this method as needed.)
 *
 * @param <obj> reference object pointer.
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetRefObject(GmatBase *obj, const UnsignedInt type,
                            const std::string &name)
{
   std::string objName = typeName;
   if (GetType() != Gmat::COMMAND)
      objName = objName + " named \"" + instanceName + "\"";

   throw GmatBaseException("SetRefObject(*obj, " + GetObjectTypeString(type) +
                           ", " + name + ") not defined for " + objName);
}


//---------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name, const Integer index)
//---------------------------------------------------------------------------
/**
 * Sets the reference object.  (Derived classes should implement
 * this method as needed.)
 *
 * @param obj reference object pointer.
 * @param type type of the reference object.
 * @param name name of the reference object.
 * @param index Index into the object array.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetRefObject(GmatBase *obj, const UnsignedInt type,
                            const std::string &name, const Integer index)
{
   std::stringstream indexString;
   indexString << index;
   std::string objName = typeName;
   if (GetType() != Gmat::COMMAND)
      objName = objName + " named \"" + instanceName + "\"";

   throw GmatBaseException("SetRefObject(*obj, " + GetObjectTypeString(type) +
                           ", " + name + ", " + indexString.str() +
                           ") not defined for " + objName);
}


//---------------------------------------------------------------------------
//  ObjectArray& GetRefObjectArray(const UnsignedInt type)
//---------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers by type.
 *
 * @param type The type of objects requested
 *
 * @return Reference to the array.  This default method returns an empty vector.
 */
//------------------------------------------------------------------------------
ObjectArray& GmatBase::GetRefObjectArray(const UnsignedInt type)
{
   static ObjectArray oa;
   oa.clear();
   return oa;
}


//---------------------------------------------------------------------------
//  ObjectArray& GetRefObjectArray(const UnsignedInt type)
//---------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers based on a string (e.g. the typename).
 *
 * @param typeString The string used to find the objects requested.
 *
 * @return Reference to the array.  This default method returns an empty vector.
 */
//------------------------------------------------------------------------------
ObjectArray& GmatBase::GetRefObjectArray(const std::string& typeString)
{
   return GetRefObjectArray(Gmat::UNKNOWN_OBJECT);
}


//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
/**
 * Retrieves the names of the wrapper objects used by the command or object
 *
 * @param completeSet   return names that would not ordinarily be returned
 *
 * @return The array of names
 *
 * @note  On the base side, only the Subscribers use wrappers
 */
//------------------------------------------------------------------------------
const StringArray& GmatBase::GetWrapperObjectNameArray(bool completeSet)
{
   return wrapperObjectNames;
}


//---------------------------------------------------------------------------
// bool HasOtherReferenceToObject(const std::string &withName)
//---------------------------------------------------------------------------
/*
 * Determines whether or not this object has a hidden or indirect
 * reference to the input object.  This could be the case when a command has
 * wrappers and one of those wrappers has the named object as a reference.
 *
 * @param  withName   the name of the object we're looking for
 *
 * @return  true if it has a reference to the named object
 */
//---------------------------------------------------------------------------
bool GmatBase::HasOtherReferenceToObject(const std::string &withName)
{
   #ifdef DEBUG_WRAPPER_REF
      MessageInterface::ShowMessage(
            "Entering HasOtherReferenceToObject for item \"%s\" of type \"%s\" and withName \"%s\"\n",
            instanceName.c_str(), typeName.c_str(), withName.c_str());
   #endif
   // Right now, the only "Other" place we look is in the wrappers
   StringArray wrapNames  = GetWrapperObjectNameArray(true);
   Integer     sz         = (Integer) wrapNames.size();
   std::string objName    = "";
   StringArray byDots;
   for (Integer ii = 0; ii < sz; ii++)
   {
      byDots  = GmatStringUtil::SeparateDots(wrapNames.at(ii));
      // Check the first part of the name for the object we're looking for
      objName = GmatStringUtil::Trim(GmatStringUtil::GetArrayName(byDots.at(0)));
      #ifdef DEBUG_WRAPPER_REF
         MessageInterface::ShowMessage("for object %s, wrapper name %d is: %s\n",
               instanceName.c_str(), ii, wrapNames.at(ii).c_str());
         MessageInterface::ShowMessage("... and extracted name is: %s\n", objName.c_str());
      #endif
      if (objName == withName) return true;
      // If there are more than two parts when separated by dots, then there is
      // a dependency, so we need to check it for the object
      if (byDots.size() > 2)
      {
         objName = GmatStringUtil::Trim(GmatStringUtil::GetArrayName(byDots.at(1)));
         #ifdef DEBUG_WRAPPER_REF
            MessageInterface::ShowMessage("for object %s, wrapper name %d is: %s\n",
                  instanceName.c_str(), ii, wrapNames.at(ii).c_str());
            MessageInterface::ShowMessage("... and 2nd extracted name is: %s\n", objName.c_str());
         #endif
         if (objName == withName) return true;
      }
   }

   return false;
}



//---------------------------------------------------------------------------
// bool IsOwnedObject(Integer id) const
//---------------------------------------------------------------------------
/*
 * Finds out if object property of given id is owned object
 * This method was added so that Interpreter can querry and create
 * proper owned objects.
 *
 * Any subclass should override this method as necessary.
 *
 * @param  id   The object property id to check for owned object
 *
 * @return  true if it is owned object
 */
//---------------------------------------------------------------------------
bool GmatBase::IsOwnedObject(Integer id) const
{
   return false;
}


//---------------------------------------------------------------------------
//  Integer GetOwnedObjectCount()
//---------------------------------------------------------------------------
/**
 * Find out how many GmatBase objects belong to this instance.
 *
 * This method is used find out how many unnamed objects belong to the current
 * instance.  The Interpreter subsystem used this method to count these objects,
 * so that it can access them one by one for reading and writing.
 *
 * One example of this use is the PropSetup class; each PropSetup contains an
 * unnamed Integrator.  The Integrator parameters are accessed for reading and
 * writing scripts. This method determines how many objects are available for
 * access.  The objects are then accessed using GetOwnedObject(Integer).  The
 * input parameter to GetOwnedObject method is found by calling this method,
 * and then looping through all of the owned objects, accessing the related
 * parameters for each.
 *
 * @return The number of owned objects.
 */
//------------------------------------------------------------------------------
Integer GmatBase::GetOwnedObjectCount()
{
   return ownedObjectCount;
}

//---------------------------------------------------------------------------
//  GmatBase* GetOwnedObject(Integer whichOne)
//---------------------------------------------------------------------------
/**
 * Access GmatBase objects belonging to this instance.
 *
 * This method is used to access the unnamed objects owned by a class derived
 * from GmatBase.  The Interpreter subsystem used this method to access
 * parameters for these objects.
 *
 * One example of this use is the PropSetup class; each PropSetup contains an
 * unnamed Integrator.  The Integrator parameters are accessed for reading and
 * writing scripts using this method.
 *
 * The input parameter to GetOwnedObject an integer that identifies which owned
 * object is requested.
 *
 * @param <whichOne> The index for this owned object.
 *
 * @return Pointer to the owned object.
 */
//---------------------------------------------------------------------------
GmatBase* GmatBase::GetOwnedObject(Integer whichOne)
{
   MessageInterface::ShowMessage
      ("GmatBase::GetOwnedObject() entered, this=<%p>'%s', whichOne = %d\n",
       this, this->GetName().c_str(), whichOne);
   
   throw GmatBaseException
      ("No owned objects for this instance \"" + instanceName + "\" of type \"" +
       typeName + "\"");
}


//------------------------------------------------------------------------------
// bool IncludeOwnedObjectsInValidation()
//------------------------------------------------------------------------------
/**
 * Indicates if owned objects are acceptable proxies during validation.
 *
 * This method is used to indicate that an object accepts referenced objects
 * that own a specified object type in addition to objects of that type.
 *
 * An example of an object that uses this capability is the Toggle command,
 * which can turn on and off owned plots and reports in objects that are not
 * Subscribers as long as they have the needed Subscriber members.
 *
 * @return true if owned objects are included in the type validation of a
 *         script line, false (the default) if not.
 */
//------------------------------------------------------------------------------
bool GmatBase::IncludeOwnedObjectsInValidation()
{
   return false;
}

//------------------------------------------------------------------------------
//  bool SetIsGlobal(bool globalFlag)
//------------------------------------------------------------------------------
/**
 * Method to set the isGlobal flag for an object.
 *
 * @param globalFlag  flag indicating whether or not this object is global
 *
 * @return value of isGlobal flag.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetIsGlobal(bool globalFlag)
{
   isGlobal = globalFlag;
   return isGlobal;
}

//------------------------------------------------------------------------------
//  bool tIsGlobal()
//------------------------------------------------------------------------------
/**
 * Method to return the isGlobal flag for an object.
 *
 * @return value of isGlobal flag (i.e. whether or not this object is Global)
 */
//------------------------------------------------------------------------------
bool GmatBase::IsGlobal() const
{
   return isGlobal;
}

//------------------------------------------------------------------------------
//  bool IsAutomaticGlobal()
//------------------------------------------------------------------------------
/**
 * Method to return the isAutomaticGlobal flag for an object.
 *
 * @return value of isAutomaticGlobal flag
 *         (i.e. whether or not this object is automatically Global)
 */
//------------------------------------------------------------------------------
bool GmatBase::IsAutomaticGlobal() const
{
   return isAutomaticGlobal;
}

//------------------------------------------------------------------------------
//  bool SetIsLocal(bool localFlag)
//------------------------------------------------------------------------------
/**
 * Method to set the isLocal flag for an object. Local object means that
 * the object was passed to a function or created inside a function.
 *
 * @param localFlag  flag indicating whether or not this object is local
 *
 * @return value of isLocal flag.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetIsLocal(bool localFlag)
{
   isLocal = localFlag;
   return isLocal;
}

//------------------------------------------------------------------------------
//  bool IsLocal()
//------------------------------------------------------------------------------
/**
 * Method to return the isLocal flag for an object.
 *
 * @return value of isLocal flag (i.e. whether or not this object is Local)
 */
//------------------------------------------------------------------------------
bool GmatBase::IsLocal() const
{
   return isLocal;
}

//------------------------------------------------------------------------------
// bool GmatBase::IsObjectCloaked() const
//------------------------------------------------------------------------------
/**
 * Method to return the cloaking flag for an object.
 *
 * @return value of cloaking flag (i.e. whether or not this object is cloaked)
 */
//------------------------------------------------------------------------------
bool GmatBase::IsObjectCloaked() const
{
   #ifdef DEBUG_CLOAKING
      MessageInterface::ShowMessage(
         "\nEntering GmatBase::IsObjectCloaked for object <%p>%s\n", this, instanceName.c_str());
   #endif
      
   if (!cloaking)
   {
      #ifdef DEBUG_CLOAKING
      MessageInterface::ShowMessage(
         "Exiting  GmatBase::IsObjectCloaked for object <%p>%s - object is not cloaked\n",
         this, instanceName.c_str());
      #endif
      return false;
   }
   
   for (Integer ii = 0; ii < parameterCount; ii++)
      if (!IsParameterCloaked(ii))
      {
         #ifdef DEBUG_CLOAKING
            MessageInterface::ShowMessage(
               "Exiting  GmatBase::IsObjectCloaked for object <%p>%s - parameter %d (%s) is not cloaked\n",
               this, instanceName.c_str(), ii, (GetParameterText(ii)).c_str());
         #endif
         return false; 
      }
   
   #ifdef DEBUG_CLOAKING
      MessageInterface::ShowMessage(
         "Exiting  GmatBase::IsObjectCloaked for object %s - returning true, as all parameters are cloaked\n",
         instanceName.c_str());
   #endif
   return true;
}

bool GmatBase::SaveAllAsDefault()
{
   return true;     
}

bool GmatBase::SaveParameterAsDefault(const Integer id)
{
   return true;
}

bool GmatBase::SaveParameterAsDefault(const std::string &label)
{
   return SaveParameterAsDefault(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  bool ExecuteCallback()
//------------------------------------------------------------------------------
/**
 * Default implementation for method that executes a callback function.
 *
 * @return true if successful; otherwise, return false (this default
 * implementation returns false).
 */
//------------------------------------------------------------------------------
bool GmatBase::ExecuteCallback()
{
   return false;
}

//------------------------------------------------------------------------------
//  bool IsCallbackExecuting()
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the callback is executing.
 *
 * @return true if executing; otherwise, else false
 */
//------------------------------------------------------------------------------
bool GmatBase::IsCallbackExecuting()
{
   return callbackExecuting;
}

//---------------------------------------------------------------------------
// bool PutCallbackData(std::string &data)
//---------------------------------------------------------------------------
bool GmatBase::PutCallbackData(std::string &data)
{
   return false;
}

//---------------------------------------------------------------------------
// bool PutCallbackRealData(RealArray &data)
//---------------------------------------------------------------------------
bool GmatBase::PutCallbackRealData(RealArray &data)
{
   return false;
}

//---------------------------------------------------------------------------
// std::string GetCallbackResults()
//---------------------------------------------------------------------------
std::string GmatBase::GetCallbackResults()
{
   return "no data";
}


//---------------------------------------------------------------------------
//  void Copy(GmatBase *inst)
//---------------------------------------------------------------------------
/**
 * Set this instance to match the one passed in.
 *
 * The default throws an exception.  Users must override this method for classes
 * that allow copying from a script.
 *
 * @param <inst> The object that is being copied.
 */
void GmatBase::Copy(const GmatBase*)
{
   throw GmatBaseException("Cannot copy objects of type " + typeName);
}


//------------------------------------------------------------------------------
//  bool Validate()
//------------------------------------------------------------------------------
/**
 * Performs any pre-run validation that the object needs.
 *
 * @param msg This receives any message after calling this
 * @return true unless validation fails.
 */
//------------------------------------------------------------------------------
bool GmatBase::Validate()
{
   return true;
}

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs any pre-run initialization that the object needs.
 *
 * @return true unless initialization fails.
 */
//------------------------------------------------------------------------------
bool GmatBase::Initialize()
{
   return true;
}

//------------------------------------------------------------------------------
//  void GmatBase::SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the internal solar system pointer for objects that have one.
 *
 * @note Derived classes that need the solar system must override this method.
 */
//------------------------------------------------------------------------------
void GmatBase::SetSolarSystem(SolarSystem *ss)
{
   ;     // Do nothing by default
}

//------------------------------------------------------------------------------
//  void GmatBase::SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Sets the internal CoordinateSystem pointer for objects that have one.
 *
 * @note Derived classes that need the internal coordinate system must override
 *       this method.
 */
//------------------------------------------------------------------------------
void GmatBase::SetInternalCoordSystem(CoordinateSystem *cs)
{
   ;     // Do nothing by default
}

//------------------------------------------------------------------------------
//  bool RequiresJ2000Body()
//------------------------------------------------------------------------------
/**
 * Identifies objects that need to have the J2000 body set in the Sandbox.
 *
 * @return true if the J2000 body needs to be set, false if not.
 */
//------------------------------------------------------------------------------
bool GmatBase::RequiresJ2000Body()
{
   return false;
}


//---------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the enumerated type of the object.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The enumeration for the type of the parameter, or
 *         UNKNOWN_PARAMETER_TYPE.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType GmatBase::GetParameterType(const Integer id) const
{
   if ((id < GmatBaseParamCount) && (id >= 0))
      return PARAMETER_TYPE[id];

   return Gmat::UNKNOWN_PARAMETER_TYPE;
}


//---------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the string associated with a parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return Text description for the type of the parameter, or the empty
 *         string ("").
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetParameterTypeString(const Integer id) const
{
   std::string retval = "";
   Gmat::ParameterType t = GetParameterType(id);
   if (t != Gmat::UNKNOWN_PARAMETER_TYPE)
      retval = PARAM_TYPE_STRING[t];
   return retval;
}


//---------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the description for the parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return String description for the requested parameter.
 *
 * @note The parameter strings should not include any white space
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetParameterText(const Integer id) const
{
   if ((id < GmatBaseParamCount) && (id >= 0))
      return PARAMETER_LABEL[id];

   std::stringstream indexString;
   indexString << id;
   throw GmatBaseException("Parameter id = " + indexString.str() +
                           " not defined on object " + instanceName);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id, Integer width)
//------------------------------------------------------------------------------
/**
 * Retrieve the description for the parameter in a string of specified width.
 *
 * The width input sets the length of the string, including a space at the end.
 * If the parameter text will not fit in the specified width, the return string
 * may exceed the width setting.
 *
 * @param id The parameter ID
 * @param width The number of characters in the return string
 *
 * @return The string set at the requested width
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetParameterText(const Integer id, Integer width)
{
   std::string txt = GetParameterText(id);

   while (txt.size() < width + 1)
      txt += " ";

   return txt;
}


//---------------------------------------------------------------------------
//  std::string GetParameterUnit(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the unit for the parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return unit for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetParameterUnit(const Integer id) const
{
   return "";
}


//------------------------------------------------------------------------------
// bool HasDecoratedFields()
//------------------------------------------------------------------------------
/**
 * Identifies objects that can handle complex fields in the script
 *
 * @return true if fields with multiple dots are handled
 */
//------------------------------------------------------------------------------
bool GmatBase::HasDecoratedFields()
{
   return false;
}


//---------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//---------------------------------------------------------------------------
/**
 * Retrieve the ID for the parameter given its description.
 *
 * @param <str> Description for the parameter.
 *
 * @return the parameter ID, or -1 if there is no associated ID.
 */
//---------------------------------------------------------------------------
Integer GmatBase::GetParameterID(const std::string &str) const
{
   for (Integer i = 0; i < GmatBaseParamCount; ++i)
      if (str == PARAMETER_LABEL[i])
      {
         return i;
      }
   
   throw GmatBaseException
      ("GmatBase::GetParameterID() The object named \"" + GetName() +
       "\" of type \"" + GetTypeName() + "\" has no parameter defined with \"" +
       str + "\"");
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
bool GmatBase::IsParameterReadOnly(const Integer id) const
{
   if ((id < 0) || (id >= parameterCount))
   {
      std::stringstream errmsg;
      errmsg << "GmatBase::IsParameterReadOnly() No parameter defined with id "
             << id << " on " << typeName << " named \"" << instanceName <<"\"";
      throw GmatBaseException(errmsg.str());
   }
   
   // Hide covariance if it hasn't been defined
   if (id == COVARIANCE)
   {
      if (covarianceList.size() == 0)
         return true;
   }
   
   return false;
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
bool GmatBase::IsParameterReadOnly(const std::string &label) const
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
bool GmatBase::IsParameterEnabled(const Integer id) const
{
   return true;
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
bool GmatBase::IsParameterEnabled(const std::string &label) const
{
   return IsParameterEnabled(GetParameterID(label));
}

//---------------------------------------------------------------------------
//  bool IsParameterCloaked(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is cloaked.
 *
 * @param <id> ID for the parameter.
 *
 * @return true if the parameter is cloaked, false if not
 */
//---------------------------------------------------------------------------
bool GmatBase::IsParameterCloaked(const Integer id) const
{
   return false;
}

//---------------------------------------------------------------------------
//  bool IsParameterCloaked(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is cloaked.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is cloaked, false if not
 */
//---------------------------------------------------------------------------
bool GmatBase::IsParameterCloaked(const std::string &label) const
{
   return IsParameterCloaked(GetParameterID(label));
}

//---------------------------------------------------------------------------
//  bool IsParameterEqualToDefault(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is equal to default
 *
 * @param <id> ID for the parameter.
 *
 * @return true if the parameter is equal to default, false if not
 */
//---------------------------------------------------------------------------
bool GmatBase::IsParameterEqualToDefault(const Integer id) const
{
   return false;
}

//---------------------------------------------------------------------------
//  bool IsParameterEqualToDefault(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is equal to default
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is equal to default, false if not
 */
//---------------------------------------------------------------------------
bool GmatBase::IsParameterEqualToDefault(const std::string &label) const
{
   return IsParameterEqualToDefault(GetParameterID(label));
}

//---------------------------------------------------------------------------
//  bool IsParameterValid(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter value is valid.
 *
 * @param <id> ID for the parameter.
 *
 * @return true if the parameter value is valid, false if not
 */
//---------------------------------------------------------------------------
bool GmatBase::IsParameterValid(const Integer id,
                                const std::string &value)
{
   return true;
}

//---------------------------------------------------------------------------
//  bool IsParameterValid(const std::string &label, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter value is valid.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter value is valid, false if not
 */
//---------------------------------------------------------------------------
bool GmatBase::IsParameterValid(const std::string &label,
                                const std::string &value)
{
   return IsParameterValid(GetParameterID(label), value);
}

//---------------------------------------------------------------------------
//  bool IsParameterVisible(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is visible from the GUI.
 *
 * @param <id> ID for the parameter.
 *
 * @return true if the parameter is visible from the GUI, false if not
 */
//---------------------------------------------------------------------------
bool GmatBase::IsParameterVisible(const Integer id) const
{
   return true;
}

//---------------------------------------------------------------------------
//  bool IsParameterVisible(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is visible from the GUI.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is visible from the GUI, false if not
 */
//---------------------------------------------------------------------------
bool GmatBase::IsParameterVisible(const std::string &label) const
{
   return IsParameterVisible(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool GmatBase::ParameterAffectsDynamics(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Determines if a parameter update affects propagation, and therfore forces a
 * reload of parameters used in propagation
 *
 * @param id The ID of the parameter
 *
 * @return true if the parameter affects propagation, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatBase::ParameterAffectsDynamics(const Integer id) const
{
   return false;
}


bool GmatBase::ParameterDvInitializesNonzero(const Integer id, const Integer r,
      const Integer c) const
{
   return false;
}


Real GmatBase::ParameterDvInitialValue(const Integer id, const Integer r,
      const Integer c) const
{
   return 0.0;
}


bool GmatBase::ParameterUpdatesAfterSuperposition(const Integer id) const
{
   return false;
}


//---------------------------------------------------------------------------
// UnsignedInt GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
UnsignedInt GmatBase::GetPropertyObjectType(const Integer id) const
{
   return Gmat::UNKNOWN_OBJECT;
}

//------------------------------------------------------------------------------
// UnsignedInt GetPropertyObjectType(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id and index.
 *
 * @param id ID for the parameter.
 * @param id Array index for the parameter.
 *
 * @return parameter ObjectType
 */
//------------------------------------------------------------------------------
UnsignedInt GmatBase::GetPropertyObjectType(const Integer id,
      const Integer index) const
{
   return GetPropertyObjectType(id);
}


//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves eumeration symbols of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return list of enumeration symbols
 */
//---------------------------------------------------------------------------
const StringArray& GmatBase::GetPropertyEnumStrings(const Integer id) const
{
   static StringArray enumStrings;
   return enumStrings;
}


//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieves eumeration symbols of parameter of given label
 *
 * @param <id> ID for the parameter.
 *
 * @return list of enumeration symbols
 */
//---------------------------------------------------------------------------
const StringArray& GmatBase::GetPropertyEnumStrings(const std::string &label) const
{
   return GetPropertyEnumStrings(GetParameterID(label));
}


//---------------------------------------------------------------------------
// bool CanAssignStringToObjectProperty(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Returns flag indicating whether or not a string can be assigned to the
 * specified object property.
 *
 * @param <id> ID for the property.
 *
 * @return true, if a string can be assigned; false otherwise
 */
//---------------------------------------------------------------------------
bool GmatBase::CanAssignStringToObjectProperty(const Integer id) const
{
   return true;    // default
}


//---------------------------------------------------------------------------
// bool IsSquareBracketAllowedInSetting(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Returns flag indicating whether or not a value with square bracket can be
 * assigned to the specified object property.
 *
 * @param <id> ID for the property.
 *
 * @return true, if a value with square bracket can be assigned; false otherwise
 */
//---------------------------------------------------------------------------
bool GmatBase::IsSquareBracketAllowedInSetting(const Integer id) const
{
   Gmat::ParameterType type = GetParameterType(id);
   if ((type == Gmat::BOOLEANARRAY_TYPE) ||
       (type == Gmat::INTARRAY_TYPE) ||
       (type == Gmat::UNSIGNED_INTARRAY_TYPE) ||
       (type == Gmat::RVECTOR_TYPE) ||
       (type == Gmat::RMATRIX_TYPE))
      return true;
   else
      return false;
}


GmatTime GmatBase::GetGmatTimeParameter(const Integer id) const
{
   std::stringstream indexString;
   indexString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot get GmatTime parameter with ID " +
      indexString.str() + " on " + typeName + " named \"" +
      instanceName + "\"");
}


GmatTime GmatBase::SetGmatTimeParameter(const Integer id, const GmatTime value)
{
   std::stringstream indexString;
   indexString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot set GmatTime parameter with ID " +
      indexString.str() + " on " + typeName + " named \"" +
      instanceName + "\"");
}


GmatTime GmatBase::GetGmatTimeParameter(const std::string &label) const
{
   return GetGmatTimeParameter(GetParameterID(label));
}


GmatTime GmatBase::SetGmatTimeParameter(const std::string &label, const GmatTime value)
{
   return SetGmatTimeParameter(GetParameterID(label), value);
}


bool GmatBase::HasPrecisionTime()
{
   return hasPrecisionTime;
}


bool GmatBase::SetPrecisionTimeFlag(bool onOff)
{
   hasPrecisionTime = onOff;
   return hasPrecisionTime;
}


//------------------------------------------------------------------------------
// StringArray GetScriptedMethodList()
//------------------------------------------------------------------------------
/**
 * Returns the list of methods available on an object
 *
 * @return The method list
 */
//------------------------------------------------------------------------------
StringArray GmatBase::GetScriptedMethodList()
{
   return objectMethods;
}

//------------------------------------------------------------------------------
// Integer GetMethodParameterCount(const std::string &forMethod)
//------------------------------------------------------------------------------
/**
 * Retrieves the number of parameters that a scripted method supports
 *
 * @param forMethod The scripted method
 *
 * @return The total number of supported parameters
 */
//------------------------------------------------------------------------------
Integer GmatBase::GetMethodParameterCount(const std::string &forMethod)
{
   return methodTotalInputCount[forMethod];
}


//------------------------------------------------------------------------------
// StringArray GetMethodParameterTypes(const std::string &forMethod)
//------------------------------------------------------------------------------
/**
 * Gets the method input list for a scriptable method
 *
 * @param forMethod The method that uses the parameters
 *
 * @return The parameter descriptions
 */
//------------------------------------------------------------------------------
ObjectTypeArray GmatBase::GetMethodParameterTypes(const std::string &forMethod)
{
   return methodInputs[forMethod];
}

//------------------------------------------------------------------------------
// bool SetMethodParameters(const std::string methodName, const StringArray & args)
//------------------------------------------------------------------------------
/**
 * Protected method used to pass in scripted method arguments
 *
 * The default implementation calls SetMethodParameter with each input parameter
 *
 * @param forMethod The scripted method
 * @param args The method input parameters
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool GmatBase::SetMethodParameters(const std::string methodName,
      const StringArray & args, const std::string &precomment,
      const std::string &postcomment)
{
   bool retval = false;

   if (std::find(objectMethods.begin(), objectMethods.end(), methodName) != objectMethods.end())
   {
      bool methodSet = true;
      bool commandComplete = GetMethodParameterCount(methodName) == 0;

      for (UnsignedInt i = 0; i < args.size(); ++i)
      {
         std::string theArg = args[i];
         if (GmatStringUtil::IsEnclosedWith(theArg, "'"))
         {
            size_t start, end;
            start = theArg.find("'");
            end = theArg.find_last_of("'");
            theArg = theArg.substr(start+1, end-start-1);
         }

         #ifdef DEBUG_METHODS
            MessageInterface::ShowMessage("Setting parameter %d to %s\n",
                  i, theArg.c_str());
         #endif

         methodSet &= SetMethodParameter(methodName, theArg, i, commandComplete);
      }

      // Build the script string
      if (methodSet)
      {
         std::string script = methodName + "(";
         for (UnsignedInt i = 0; i < args.size(); ++i)
         {
            if (i > 0)
               script += ", ";
            script += args[i];
         }

         // remove trailing \r and \n - copied from GmatStringUtil, removing
         // preconditions
         std::string thepostcomment = postcomment;
         if (thepostcomment.length() > 0)
         {
            while (thepostcomment[thepostcomment.size() - 1] == '\n' ||
               thepostcomment[thepostcomment.size() - 1] == '\r')
               thepostcomment.erase(thepostcomment.size() - 1, 1);
         }

         script += ");" + thepostcomment;
         objectMethodPrefaces.push_back(precomment);
         scriptedMethods.push_back(script);

         retval = true;
      }
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool SetMethodParameter(const std::string methodName, const std::string &arg,
//       const Integer index, bool &commandComplete);
//------------------------------------------------------------------------------
/**
 * Setup code for scripted methods.
 *
 * Scripted methods may have input parameters.  This method sets up those
 * parameters one by one.  A new method configuration is entered when the index
 * value is zero.  Subsequent settings set additional values for the script
 * method.
 *
 * Derived classes override this function to set up their supported script
 * methods.
 *
 * @param methodName The name of the scripted method
 * @param arg The scripted value
 * @param index The location of the argument in the argument list
 * @param commandComplete Flag indicating if the method is fully configured
 *
 * @return true if the parameter was setup
 */
//------------------------------------------------------------------------------
bool GmatBase::SetMethodParameter(const std::string methodName,
      const std::string &arg, const Integer index, bool &commandComplete)
{
   commandComplete = false;
   return false;
}


//------------------------------------------------------------------------------
// Generic ExecuteMethod(const std::string &forMethod,
//                       const StringArray &parameters)
//------------------------------------------------------------------------------
/**
 * Protected method used to pass in scripted method arguments
 *
 * Derived classes override this method with their execution call.
 *
 * @param forMethod The scripted method
 * @param parameters The method parameter list.  This parameter field is usually
 *                   ignored, but can be used to distinguish calls.
 *
 * @return The return value from the call
 *
 * @note This method is not yet used.  Once the RHS method calls are implemented,
 * it will be used.
 */
//------------------------------------------------------------------------------
Generic GmatBase::ExecuteMethod(const std::string &forMethod,
                              const StringArray &parameters)
{
   throw GmatBaseException("The scripted method " + forMethod +
               " is not supported on " + typeName + " objects");
}


//------------------------------------------------------------------------------
// UnsignedInt GetGenericType(const std::string fromString,
//       StringArray &contents)
//------------------------------------------------------------------------------
/**
 * Returns the type that the input string will use while filling a Generic
 *
 * @param fromString The string that contains data for the Generic
 * @param contents The string data, split into substrings
 *
 * @return The type
 */
//------------------------------------------------------------------------------
UnsignedInt GmatBase::GetGenericType(const std::string &fromString,
      StringArray &contents)
{
   UnsignedInt type = Gmat::UNKNOWN_PARAMETER_TYPE;

   // Check for array indicators
   bool isArray = ((fromString.find(",") != std::string::npos) ||
                   ((fromString.find("{") != std::string::npos) &&
                    (fromString.find("}") != std::string::npos)) ||
                   ((fromString.find("[") != std::string::npos) &&
                    (fromString.find("]") != std::string::npos)));

   if (isArray)
   {
      bool isbrace = (fromString.find("{") != std::string::npos);

      if (isbrace)
         contents = GmatStringUtil::SeparateBrackets(fromString, "{}", ",", false);
      else
         contents = GmatStringUtil::SeparateBrackets(fromString, "[]", ", ", false);
   }
   else
      contents.push_back(fromString);

   for (UnsignedInt i = 0; i < contents.size(); ++i)
   {
      // Is it a number?
      if (GmatStringUtil::IsNumber(contents[i]))
      {
         if (contents[i].find(".") != std::string::npos)
         {
            if ((type == Gmat::UNKNOWN_PARAMETER_TYPE) ||
                (type == (isArray ? Gmat::INTARRAY_TYPE : Gmat::INTEGER_TYPE)))
               type = (isArray ? Gmat::REALARRAY_TYPE : Gmat::REAL_TYPE);
         }
         else if (type == Gmat::UNKNOWN_PARAMETER_TYPE)
            type = (isArray ? Gmat::INTARRAY_TYPE : Gmat::INTEGER_TYPE);

         if ((type != (isArray ? Gmat::INTARRAY_TYPE : Gmat::INTEGER_TYPE)) &&
             (type != (isArray ? Gmat::REALARRAY_TYPE : Gmat::REAL_TYPE)))
            throw GmatBaseException("The data in the input string \"" +
                  fromString + "\" is not allowed; it contains mixed (string "
                        "and numeric) data types");
      }
      else // Treat as a string
      {
         if (type == Gmat::UNKNOWN_PARAMETER_TYPE)
         {
            type = (isArray ? Gmat::STRINGARRAY_TYPE : Gmat::STRING_TYPE);
         }

         if (type != (isArray ? Gmat::STRINGARRAY_TYPE : Gmat::STRING_TYPE))
            throw GmatBaseException("The data in the input string \"" +
                  fromString + "\" is not allowed; it contains mixed (numeric "
                        "and string) data types");
      }
   }

   return type;
}


//------------------------------------------------------------------------------
// UnsignedInt GetGenericType(const Generic g)
//------------------------------------------------------------------------------
/**
 * Returns the type contained in the input container
 *
 * GMAT's Generic container (a variant) supports these types:
 * Real, Integer, std::string, RealArray, IntegerArray, StringArray
 *
 * @param g The Generic that contains data
 *
 * @return The type of the data
 */
//------------------------------------------------------------------------------
UnsignedInt GmatBase::GetGenericType(const Generic &g)
{
   UnsignedInt type = Gmat::UNKNOWN_PARAMETER_TYPE;

   switch (g.VarIndex())
   {
   case 0:
      type = Gmat::REAL_TYPE;
      break;

   case 1:
      type = Gmat::INTEGER_TYPE;
      break;

   case 2:
      type = Gmat::STRING_TYPE;
      break;

   case 3:
      type = Gmat::REALARRAY_TYPE;
      break;

   case 4:
      type = Gmat::INTARRAY_TYPE;
      break;

   case 5:
      type = Gmat::STRINGARRAY_TYPE;
      break;

   default:
      break;
   }

   return type;
}


//------------------------------------------------------------------------------
// Generic GetGeneric(const std::string &fromString)
//------------------------------------------------------------------------------
/**
 * Retrieves a Generic object contained in a string
 *
 * @param fromString The data stream containing the generic data
 *
 * @return A Generic, populated by the data in the string
 */
//------------------------------------------------------------------------------
Generic GmatBase::GetGeneric(const std::string &fromString)
{
   Generic data;
   StringArray members;

   UnsignedInt type = GetGenericType(fromString, members);
   std::string msg;

   switch (type)
   {
   case Gmat::REAL_TYPE:
      {
         Real value;
         GmatStringUtil::ToReal(fromString, value);
         data = value;
      }
      break;

   case Gmat::INTEGER_TYPE:
      {
         Integer value;
         GmatStringUtil::ToInteger(fromString, value);
         data = value;
      }
      break;

   case Gmat::STRING_TYPE:
      data = fromString;
      break;

   /// @note We may need to determine how to detect real vs int inputs here.
   case Gmat::REALARRAY_TYPE:
   case Gmat::INTARRAY_TYPE:
      {
         RealArray values;
         Real value = 0.0;

         for (UnsignedInt i = 0; i < members.size(); ++i)
         {
            if (!GmatStringUtil::ToReal(members[i], value))
               msg += "Expected Real value in the string \"" + members[i] +
                      "\"\n";
            else
               values.push_back(value);
         }

         data = values;
      }
      break;

   case Gmat::STRINGARRAY_TYPE:
   {
      StringArray values;

      for (UnsignedInt i = 0; i < members.size(); ++i)
         values.push_back(members[i]);

      data = values;
   }
      break;

   default:
      msg = "The input string \"" + fromString + "\" was not parsed correctly";
      break;
   }

   if (msg != "")
      throw GmatBaseException(msg);

   return data;
}

//---------------------------------------------------------------------------
//  Integer GetScriptLineNumber() const
//---------------------------------------------------------------------------
/**
 * Retrieve the value of scriptLineNumber.
 *
 * @return scriptLineNumber.
 */
 //------------------------------------------------------------------------------
Integer GmatBase::GetScriptLineNumber() const
{
   return scriptLineNumber;
}

//---------------------------------------------------------------------------
//  Integer SetScriptLineNumber() const
//---------------------------------------------------------------------------
/**
 * Set the value of scriptLineNumber. Used to report the line number in script
 * that an object is created or called.
 */
 //------------------------------------------------------------------------------
void GmatBase::SetScriptLineNumber(Integer val)
{
   scriptLineNumber = val;
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real GmatBase::GetRealParameter(const Integer id) const
{
   std::stringstream indexString;
   indexString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot get real parameter with ID " +
                           indexString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real GmatBase::SetRealParameter(const Integer id, const Real value)
{
   std::stringstream indexString;
   indexString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot set real parameter with ID " +
                           indexString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id, Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param index Index for objects in arrays.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real GmatBase::GetRealParameter(const Integer id, const Integer index) const
{
   std::stringstream indexString, idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   indexString << index;
   throw GmatBaseException("Cannot get real parameter with ID " +
                           idString.str() + " and index " + indexString.str() +
                           " on " + typeName + " named \"" + instanceName + "\"");
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id, const Integer row,
//                        const Integer col) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param row Row index for parameters in arrays.
 * @param col Column index for parameters in arrays.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real GmatBase::GetRealParameter(const Integer id, const Integer row,
                                const Integer col) const
{
   if (id == COVARIANCE)
   {
      #ifdef DEBUG_COVARIANCE
         MessageInterface::ShowMessage("Getting covariance[%d,%d] = %le\n",
               row, col, covariance(row,col));
      #endif
      return covariance(row,col);
   }

   std::stringstream indexString, idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   indexString << ", row " << row << " and column " << col;
   throw GmatBaseException("Cannot get real array element with ID " +
                           idString.str() + indexString.str() +
                           " on " + typeName + " named \"" + instanceName + "\"");
}


//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value, Integer index)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param <id>    The integer ID for the parameter.
 * @param <value> The new parameter value.
 * @param <row>   The row in which the data is set.
 * @param <col>   The column in which the data is set.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real, or
 *         the parameter is not in an array.
 */
//---------------------------------------------------------------------------
Real GmatBase::SetRealParameter(const Integer id, const Real value,
                                const Integer row, const Integer col)
{
   if (id == COVARIANCE)
   {
      #ifdef DEBUG_COVARIANCE
         MessageInterface::ShowMessage("Setting covariance[%d,%d] = %le\n",
               row, col, value);
      #endif
      covariance(row,col) = value;
      return covariance(row,col);
   }

   std::stringstream idString;
   idString << id << " and label " << GetParameterText(id);
   throw GmatBaseException("Cannot set real parameter with ID " +
                           idString.str() + " using array indices on " +
                           typeName + " named \"" + instanceName + "\"");
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
Real GmatBase::SetRealParameter(const Integer id, const Real value,
                                const Integer index)
{
   std::stringstream indexString, idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   indexString << index;
   throw GmatBaseException("Cannot set real parameter with ID " +
                           idString.str() + " and index " + indexString.str() +
                           " on " + typeName + " named \"" + instanceName + "\"");
}


//---------------------------------------------------------------------------
//  Integer GetIntegerParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Integer parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Integer GmatBase::GetIntegerParameter(const Integer id) const
{
   std::stringstream idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot get integer parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//---------------------------------------------------------------------------
//  Integer SetIntegerParameter(const Integer id, const Integer value)
//---------------------------------------------------------------------------
/**
* Set the value for an Integer parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         INTEGER_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not an Integer.
 */
//------------------------------------------------------------------------------
Integer GmatBase::SetIntegerParameter(const Integer id, const Integer value)
{
   std::stringstream idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot set integer parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//---------------------------------------------------------------------------
//  Integer GetIntegerParameter(const Integer id, const Integer index) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Integer parameter.
 *
 * @param id The integer ID for the parameter.
 * @param index Index for objecs in arrays.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Integer GmatBase::GetIntegerParameter(const Integer id,
                                      const Integer index) const
{
   std::stringstream indexString, idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   indexString << index;
   throw GmatBaseException("Cannot get integer parameter with ID " +
                           idString.str() + " and index " + indexString.str() +
                           " on " + typeName + " named \"" + instanceName + "\"");
}


//---------------------------------------------------------------------------
//  Integer SetIntegerParameter(const Integer id, const Integer value,
//                              const Integer index)
//---------------------------------------------------------------------------
/**
* Set the value for an Integer parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or
 *         INTEGER_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not an Integer.
 */
//------------------------------------------------------------------------------
Integer GmatBase::SetIntegerParameter(const Integer id, const Integer value,
                                      const Integer index)
{
   std::stringstream indexString, idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   indexString << index;
   throw GmatBaseException("Cannot set integer parameter with ID " +
                           idString.str() + " and index " + indexString.str() +
                           " on " + typeName + " named \"" + instanceName + "\"");
}


//---------------------------------------------------------------------------
//  UnsignedInt GetUnsignedIntParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for an UnsignedInt parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
UnsignedInt GmatBase::GetUnsignedIntParameter(const Integer id) const
{
   std::stringstream idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot get unsigned integer parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//---------------------------------------------------------------------------
//  UnsignedInt SetUnsignedIntParameter(const Integer id,
//                                      const UnsignedInt value)
//---------------------------------------------------------------------------
/**
 * Set the value for an UnsignedInt parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         UNSIGNED_INT_PARAMETER_UNDEFINED if the parameter id is invalid or
 *         the parameter type is not an UnsignedInt.
 */
//------------------------------------------------------------------------------
UnsignedInt GmatBase::SetUnsignedIntParameter(const Integer id,
                                              const UnsignedInt value)
{
   std::stringstream idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot set unsigned integer parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//---------------------------------------------------------------------------
//  UnsignedInt GetUnsignedIntParameter(const Integer id, const Integer index) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an UnsignedInt parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
UnsignedInt GmatBase::GetUnsignedIntParameter(const Integer id,
                                              const Integer index) const
{
   std::stringstream indexString, idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   indexString << index;
   throw GmatBaseException("Cannot get unsigned integer parameter with ID " +
                           idString.str() + " and index " + indexString.str() +
                           " on " + typeName + " named \"" + instanceName + "\"");
}


//---------------------------------------------------------------------------
//  UnsignedInt SetUnsignedIntParameter(const Integer id,
//                                      const UnsignedInt value,
//                                      const Integer index)
//---------------------------------------------------------------------------
/**
* Set the value for an UnsignedInt parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or
 *         UNSIGNED_INT_PARAMETER_UNDEFINED if the parameter id is invalid or
 *         the parameter type is not an UnsignedInt.
 */
//------------------------------------------------------------------------------
UnsignedInt GmatBase::SetUnsignedIntParameter(const Integer id,
                                              const UnsignedInt value,
                                              const Integer index)
{
   std::stringstream indexString, idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   indexString << index;
   throw GmatBaseException("Cannot set unsigned integer parameter with ID " +
                           idString.str() + " and index " + indexString.str() +
                           " on " + typeName + " named \"" + instanceName + "\"");
}


//---------------------------------------------------------------------------
//  const UnsignedIntArray& GetUnsignedIntArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access an array of unsigned int data.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested UnsignedIntArray; throws if the parameter is not a
 *         UnsignedIntArray.
 */
//------------------------------------------------------------------------------
const UnsignedIntArray& GmatBase::GetUnsignedIntArrayParameter(const Integer id) const
{
   std::stringstream idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot get unsigned integer array parameter with "
                           " ID " + idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}

//---------------------------------------------------------------------------
//  const IntegerArray& GetIntegerArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access an array of integer data.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested IntegerArray; throws if the parameter is not a
 *         IntegerArray.
 */
const IntegerArray& GmatBase::GetIntegerArrayParameter(const Integer id) const
{
   std::stringstream idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot get integer array parameter with "
                           " ID " + idString.str() +
                           " on " + typeName + " named " + instanceName);
}

//---------------------------------------------------------------------------
//  const IntegerArray& GetIntegerArrayParameter(const Integer id,
//                                             const Integer index) const
//---------------------------------------------------------------------------
/**
 * Access an array of integer data.
 *
 * @param id The integer ID for the parameter.
 * @param index The index when multiple IntegerArrays are supported.
 *
 * @return The requested IntegerArray; throws if the parameter is not a
 *         IntegerArray.
 */
const IntegerArray& GmatBase::GetIntegerArrayParameter(const Integer id,
                                               const Integer index) const
{
   std::stringstream indexString, idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   indexString << index;
   throw GmatBaseException("Cannot get integer array parameter with ID " +
                           idString.str() + " and index " + indexString.str() +
                           " on " + typeName + " named " + instanceName);
}

//---------------------------------------------------------------------------
//  const Rvector& GetRvectorParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Rvector parameter.
*
* @param <id> The integer ID for the parameter.
*
* @return The parameter's value.
*/
//------------------------------------------------------------------------------
const Rvector& GmatBase::GetRvectorParameter(const Integer id) const
{
   std::stringstream idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot get Rvector parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//---------------------------------------------------------------------------
//  const Rvector& SetRvectorParameter(const Integer id, const Rvector& value)
//---------------------------------------------------------------------------
/**
* Set the value for an Rvector parameter.
*
* @param <id> The integer ID for the parameter.
* @param <value> The new parameter value.
*
* @return the parameter value at the end of this call, or
*         RVECTOR_PARAMETER_UNDEFINED if the parameter id is invalid or the
*         parameter type is not Rvector.
*/
//------------------------------------------------------------------------------
const Rvector& GmatBase::SetRvectorParameter(const Integer id,
                                             const Rvector& value)
{
   std::stringstream idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot set Rvector parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}

//---------------------------------------------------------------------------
//  const Rmatrix& GetRmatrixParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Rmatrix parameter.
*
* @param <id> The integer ID for the parameter.
*
* @return The parameter's value.
*/
//------------------------------------------------------------------------------
const Rmatrix& GmatBase::GetRmatrixParameter(const Integer id) const
{
   std::stringstream idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot get Rmatrix parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//---------------------------------------------------------------------------
//  const Rmatrix& SetRmatrixParameter(const Integer id, const Rmatrix& value)
//---------------------------------------------------------------------------
/**
* Set the value for an Rmatrix parameter.
*
* @param <id> The integer ID for the parameter.
* @param <value> The new parameter value.
*
* @return the parameter value at the end of this call, or
*         RMATRIX_PARAMETER_UNDEFINED if the parameter id is invalid or the
*         parameter type is not Rmatrix.
*/
//------------------------------------------------------------------------------
const Rmatrix& GmatBase::SetRmatrixParameter(const Integer id,
                                             const Rmatrix& value)
{
   return RMATRIX_PARAMETER_UNDEFINED;
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The string stored for this parameter, or throw ab=n exception if
 *         there is no string association.
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetStringParameter(const Integer id) const
{
   std::stringstream idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot get string parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}

//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const char *value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, throw if the parameter is not stored.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetStringParameter(const Integer id, const char *value)
{
   return SetStringParameter(id, std::string(value));
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, throw if the parameter is not stored.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetStringParameter(const Integer id, const std::string &value)
{
   std::stringstream idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot set string parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id, const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetStringParameter(const Integer id,
                                         const Integer index) const
{
   std::stringstream indexString, idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   indexString << index;
   throw GmatBaseException("Cannot get string parameter with ID " +
                           idString.str() + " and index " + indexString.str() +
                           " on " + typeName + " named \"" + instanceName + "\"");
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const char *value,
//                          const Integer index)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new string for this parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetStringParameter(const Integer id, const char *value,
                                  const Integer index)
{
   return SetStringParameter(id, std::string(value), index);
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value,
//                          const Integer index)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new string for this parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetStringParameter(const Integer id, const std::string &value,
                                  const Integer index)
{
   std::stringstream indexString, idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   indexString << index;
   throw GmatBaseException("Cannot set string parameter with ID " +
                           idString.str() + " and index " + indexString.str() +
                           " on " + typeName + " named \"" + instanceName + "\"");
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& GmatBase::GetStringArrayParameter(const Integer id) const
{
   std::stringstream idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot get string array parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id,
//                                             const Integer index) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param id The integer ID for the parameter.
 * @param index The index when multiple StringArrays are supported.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& GmatBase::GetStringArrayParameter(const Integer id,
                                               const Integer index) const
{
   std::stringstream indexString, idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   indexString << index;
   throw GmatBaseException("Cannot get string array parameter with ID " +
                           idString.str() + " and index " + indexString.str() +
                           " on " + typeName + " named \"" + instanceName + "\"");
}


//---------------------------------------------------------------------------
//  std::string GetOnOffParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve a On/Off parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The string stored for this parameter, or throw an exception if
 *         there is no string association.
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetOnOffParameter(const Integer id) const
{
   std::stringstream idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot get On/Off parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//---------------------------------------------------------------------------
//  bool SetOnOffParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, throw if the parameter is not stored.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetOnOffParameter(const Integer id, const std::string &value)
{
   std::stringstream idString;
   idString << id << ": \"" << GetParameterText(id) << "\"";
   throw GmatBaseException("Cannot set On/Off parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}



//-------------------------------------
// API methods
//-------------------------------------

//------------------------------------------------------------------------------
// std::string  Help(const std::string *forItem)
//------------------------------------------------------------------------------
/**
 * Retrieves the help string for the class or for documented class members
 *
 * DO NOT OVERRIDE THIS METHOD!  The API Help system calls into this method,
 * which calls member GetHelpString methods.  Class/object specific help should
 * be provided using that method.
 *
 * @param forItem Subitems for the system help
 *
 * @return The help string
 */
//------------------------------------------------------------------------------
std::string GmatBase::Help(const std::string &forItem)
{
   return GetHelpString(forItem);
}

//------------------------------------------------------------------------------
// std::string  GetHelpString(const std::string *forItem)
//------------------------------------------------------------------------------
/**
 * Retrieves the help string for the class or for documented class members
 *
 * Class/object specific help should be provided by overriding this method.
 *
 * @param forItem Subitems for the system help
 *
 * @return The help string
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetHelpString(const std::string &forItem)
{
   std::string helpStr = "\n" + typeName + "  " + instanceName + "\n\n";

   bool showVerbose = false;
   if (forItem == "Verbose")
   {
      showVerbose = true;

      #ifdef DEBUG_API
         helpStr += "Entered Verbose mode\n\n";
      #endif
   }
   #ifdef DEBUG_API
      else
         helpStr += "Showing normal mode\n\n";
   #endif

   if ((forItem == "") || showVerbose)
   {
      helpStr += "   Field                                   Type   Value\n"
                 "   --------------------------------------------------------\n\n";

      for (auto i = 0; i < parameterCount; ++i)
      {
         if (!IsParameterReadOnly(i) || showVerbose)
         {
            try
            {
               helpStr += "   " + GetParameterText(i, 24) + "   " +
                     GetTypeAndValue(i) + "\n";
            }
            catch (BaseException &)
            {
               helpStr += "   " + GetParameterText(i, 24) + "   " +
                     "GMAT Code Broken for this Parameter" + "\n";
            }
         }
      }
   }
   else
      helpStr += "Help for " + forItem + " is not available.\n\n";

   return helpStr;
}

//------------------------------------------------------------------------------
// std::string GetTypeAndValue(Integer i)
//------------------------------------------------------------------------------
/**
 * Retrieves a string describing a parameter (type and value)
 *
 * @param i ID for the parameter
 *
 * @return The descriptive string
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetTypeAndValue(Integer i)
{
   std::string retval = GetParameterTypeString(i);

   // Apply aliases
   if (retval == "Enumeration")
      retval = "List";

   Gmat::ParameterType type = GetParameterType(i);

   std::stringstream temp;
   temp.precision(GetDataPrecision());
   temp.width(16);
   temp << retval << "   ";
//   std::string value;

   switch (type)
   {
   case Gmat::STRING_TYPE:
   case Gmat::OBJECT_TYPE:
   case Gmat::ENUMERATION_TYPE:
   case Gmat::FILENAME_TYPE:
   case Gmat::COLOR_TYPE:
      temp << GetStringParameter(i);
      break;

   case Gmat::REAL_TYPE:
      temp << GetRealParameter(i);
      break;

   case Gmat::BOOLEAN_TYPE:
      temp << (GetBooleanParameter(i) ? "true" : "false");
      break;

   case Gmat::INTEGER_TYPE:
      temp << GetIntegerParameter(i);
      break;

   case Gmat::STRINGARRAY_TYPE:
   case Gmat::OBJECTARRAY_TYPE:
      temp << "{";
      {
         StringArray list = GetStringArrayParameter(i);
         for (UnsignedInt i = 0; i < list.size(); ++i)
         {
            if (i > 0)
               temp << ", ";
            temp << list[i];
         }
      }
      temp << "}";
      break;

   case Gmat::RVECTOR_TYPE:
      temp << "[";
      {
         Rvector data = GetRvectorParameter(i);
         for (Integer i = 0; i < data.GetSize(); ++i)
         {
            if (i > 0)
               temp << ", ";
            temp << data[i];
         }
      }
      temp << "]";
      break;

   case Gmat::ON_OFF_TYPE:
      {
         temp << GetOnOffParameter(i);
      }
      break;

   default:
      temp << "<not set>";
      break;
   }

//   value = "";
//   temp >> value;

   retval = temp.str();
   return retval;
}


//------------------------------------------------------------------------------
// void SetField(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value of an Integer parameter
 *
 * @param id The ID for the parameter
 * @param value The new value for the parameter
 *
 * @return True in success, false on failure
 */
//------------------------------------------------------------------------------
void GmatBase::SetField(const Integer id, const Integer value)
{
   bool retval = false;
   std::stringstream errormsg;

   try
   {
      UnsignedInt type = GetParameterType(id);
      if (type == Gmat::INTEGER_TYPE)
      {
         if (SetIntegerParameter(id, value) == value)
            retval = true;
      }
      // Handle Reals passed in as Integers
      else if (type == Gmat::REAL_TYPE)
      {
         if (SetRealParameter(id, value) == (Real)value)
            retval = true;
      }
      else
         errormsg << "Error setting field value; "
                  << GetParameterText(id) << " expects a "
                  << GetParameterTypeString(id)
                  << " value, but the received value is an integer.";
   }
   catch (BaseException &ex)
   {
      // Exceptions are dropped, and the return set to false
      retval = false;
      errormsg << ex.GetFullMessage();
   }

   if (retval == false)
      throw GmatBaseException(errormsg.str());
}

//------------------------------------------------------------------------------
// void SetField(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a Real parameter
 *
 * @param id The ID for the parameter
 * @param value The new value for the parameter
 *
 * @return True in success, false on failure
 */
//------------------------------------------------------------------------------
void GmatBase::SetField(const Integer id, const Real value)
{
   bool retval = false;
   std::stringstream errormsg;

   try
   {
      // Handle Integers passed in as if they were Reals
      UnsignedInt type = GetParameterType(id);
      if (type == Gmat::INTEGER_TYPE)
      {
         if (SetIntegerParameter(id, (Integer)value) == value)
            retval = true;
      }
      else if (type == Gmat::REAL_TYPE)
      {
         if (SetRealParameter(id, value) == (Real)value)
            retval = true;
      }
      else
         errormsg << "Error setting field value; "
                  << GetParameterText(id) << " expects a "
                  << GetParameterTypeString(id)
                  << " value, but the received value is a real number.";
   }
   catch (BaseException &ex)
   {
      // Exceptions are dropped, and the return set to false
      retval = false;
      errormsg << ex.GetFullMessage();
   }

   if (retval == false)
      throw GmatBaseException(errormsg.str());
}

//------------------------------------------------------------------------------
// void SetField(const Integer id, const string value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a string parameter
 *
 * @param id The ID for the parameter
 * @param value The new value for the parameter
 *
 * @return True in success, false on failure
 */
//------------------------------------------------------------------------------
void GmatBase::SetField(const Integer id, const std::string &value)
{
   bool retval = false;
   std::stringstream errormsg;

   try
   {
      // Handle Integers passed in as if they were Reals
      UnsignedInt type = GetParameterType(id);
      if ( (type == Gmat::STRING_TYPE) ||
           (type == Gmat::OBJECT_TYPE) ||
           (type == Gmat::ENUMERATION_TYPE) ||
           (type == Gmat::FILENAME_TYPE) ||
           (type == Gmat::COLOR_TYPE) )
      {
         if (SetStringParameter(id, value))
            retval = true;
      }
      else if ( (type == Gmat::STRINGARRAY_TYPE) ||
                (type == Gmat::OBJECTARRAY_TYPE) )
      {
         // Remove enclosing braces and Separate at commas
         StringArray theData =
               GmatStringUtil::SeparateBrackets(value, "{}", ",", false);

         // Pass in the settings
         retval = (theData.size() > 0 ? true : false);
         std::string failedSetting;
         for (UnsignedInt i = 0; i < theData.size(); ++i)
         {
            bool dataSet = SetStringParameter(id, theData[i], i);
            if (!dataSet)
            {
               if (failedSetting == "")
                  failedSetting = theData[i];
               else
                  failedSetting += ", " + theData[i];
               retval = false;
            }
         }
         if (!retval)
            errormsg << "Error setting field value; the settings "
                     << failedSetting << " from the string \""
                     << value << "\" were not set for the "
                     << GetParameterTypeString(id)
                     << " field.";
      }
      else
         errormsg << "Error setting field value; "
                  << GetParameterText(id) << " expects a "
                  << GetParameterTypeString(id)
                  << " value, but the received value is a string.";
   }
   catch (BaseException &ex)
   {
      // Exceptions are dropped, and the return set to false
      retval = false;
      errormsg << ex.GetFullMessage();
   }

   if (retval == false)
      throw GmatBaseException(errormsg.str());
}

//------------------------------------------------------------------------------
// void SetField(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a boolean or on/off parameter
 *
 * @param id The ID for the parameter
 * @param value The new value for the parameter
 *
 * @return True in success, false on failure
 */
//------------------------------------------------------------------------------
void GmatBase::SetField(const Integer id, const bool value)
{
   bool retval = false;
   std::stringstream errormsg;

   try
   {
      // Handle Integers passed in as if they were Reals
      UnsignedInt type = GetParameterType(id);
      if (type == Gmat::BOOLEAN_TYPE)
      {
         if (SetBooleanParameter(id, value) == value)
            retval = true;
      }

      // Handle On/Offs passed in as if they were Booleans
      else if (type == Gmat::ON_OFF_TYPE)
      {
         if (SetOnOffParameter(id, (value ? "On" : "Off")))
            retval = true;
      }
      else
         errormsg << "Error setting field value; "
                  << GetParameterText(id) << " expects a "
                  << GetParameterTypeString(id)
                  << " value, but the received value is a Boolean.";
   }
   catch (BaseException &ex)
   {
      // Exceptions are dropped, and the return set to false
      retval = false;
      errormsg << ex.GetFullMessage();
   }

   if (retval == false)
      throw GmatBaseException(errormsg.str());
}

//------------------------------------------------------------------------------
// void SetField(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value of an Integer parameter
 *
 * @param label The script label for the parameter
 * @param value The new value for the parameter
 *
 * @return True in success, false on failure
 */
//------------------------------------------------------------------------------
void GmatBase::SetField(const std::string &label, const Integer value)
{
   SetField(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// void SetField(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a Real parameter
 *
 * @param label The script label for the parameter
 * @param value The new value for the parameter
 *
 * @return True in success, false on failure
 */
//------------------------------------------------------------------------------
void GmatBase::SetField(const std::string &label, const Real value)
{
   SetField(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// void SetField(const std::string &label, const string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a string parameter
 *
 * @param label The script label for the parameter
 * @param value The new value for the parameter
 *
 * @return True in success, false on failure
 */
//------------------------------------------------------------------------------
void GmatBase::SetField(const std::string &label, const std::string &value)
{
   SetField(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// void SetField(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a boolean or on/off parameter
 *
 * @param label The script label for the parameter
 * @param value The new value for the parameter
 *
 * @return True in success, false on failure
 */
//------------------------------------------------------------------------------
void GmatBase::SetField(const std::string &label, const bool value)
{
   SetField(GetParameterID(label), value);
}

void GmatBase::SetField(const Integer id, const RealArray &value)
{
   Integer type = GetParameterType(id);

   switch (type)
   {
   case Gmat::RVECTOR_TYPE:
   {
//      Rvector array;
//      array.SetSize(value.size());
      for (UnsignedInt i = 0; i < value.size(); ++i)
//         array[i] = value[i];
//      SetRvectorParameter(id, array);
         SetRealParameter(id, value[i], i);
   }
      break;

   default:
      break;
   }
}

void GmatBase::SetField(const Integer id, const StringArray &value)
{
   MessageInterface::ShowMessage("Called SetField for StringArray %s\n");
}

void GmatBase::SetField(const std::string &label, const RealArray &value)
{
   SetField(GetParameterID(label), value);
}

void GmatBase::SetField(const std::string &label, const StringArray &value)
{
   SetField(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string  GetField(const std::string &label)
//------------------------------------------------------------------------------
/**
 * Retrieves a parameter value
 *
 * @param id The parameter's ID
 *
 * @return The parameter value, inside of a string
 */
//------------------------------------------------------------------------------
std::string  GmatBase::GetField(const Integer id)
{
   std::string value;
   std::stringstream toVal;

   try
   {
      // Handle Integers passed in as if they were Reals
      UnsignedInt type = GetParameterType(id);
      toVal.str("");
      toVal.precision(16);

      switch (type)
      {
      case Gmat::REAL_TYPE:
         toVal << GetRealParameter(id);
         toVal >> value;
         break;

      case Gmat::INTEGER_TYPE:
         toVal << GetIntegerParameter(id);
         toVal >> value;
         break;

      case Gmat::UNSIGNED_INT_TYPE:
         toVal << GetUnsignedIntParameter(id);
         toVal >> value;
         break;

      case Gmat::INTARRAY_TYPE:
      {
         IntegerArray data = GetIntegerArrayParameter(id);
         for (Integer i = 0; i < data.size(); ++i)
         {
            if (i > 0)
               toVal << ", ";
            toVal << data[i];
         }
         toVal >> value;
      }
         break;

      case Gmat::UNSIGNED_INTARRAY_TYPE:
      {
         UnsignedIntArray data = GetUnsignedIntArrayParameter(id);
         for (Integer i = 0; i < data.size(); ++i)
         {
            if (i > 0)
               toVal << ", ";
            toVal << data[i];
         }
         toVal >> value;
      }
         break;

      case Gmat::STRING_TYPE:
      case Gmat::OBJECT_TYPE:
      case Gmat::ENUMERATION_TYPE:
      case Gmat::FILENAME_TYPE:
      case Gmat::COLOR_TYPE:
         value = GetStringParameter(id);
         break;

      case Gmat::BOOLEAN_TYPE:
      {
         bool tf = GetBooleanParameter(id);
         value = (tf ? "true" : "false");
      }
         break;

      case Gmat::ON_OFF_TYPE:
         value = GetOnOffParameter(id);
         break;

      case Gmat::STRINGARRAY_TYPE:
      case Gmat::OBJECTARRAY_TYPE:
      {
         StringArray data = GetStringArrayParameter(id);
         toVal << "{";
         for (UnsignedInt i = 0; i < data.size(); ++i)
         {
            if (i > 0)
               toVal << ", ";
            toVal << data[i];
         }
         toVal << "}";
         value = toVal.str();
      }
         break;

      case Gmat::BOOLEANARRAY_TYPE:
      {
         BooleanArray data = GetBooleanArrayParameter(id);
         toVal << "[";
         for (UnsignedInt i = 0; i < data.size(); ++i)
         {
            if (i > 0)
               toVal << ", ";
            toVal << (data[i] ? "true" : "false");
         }
         toVal << "]";
         value = toVal.str();
      }
         break;

      case Gmat::RVECTOR_TYPE:
      {
         Rvector data = GetRvectorParameter(id);

         toVal << "[";
         for (UnsignedInt i = 0; i < data.GetSize(); ++i)
         {
            if (i > 0)
               toVal << ", ";
            toVal << data[i];
         }
         toVal << "]";
         value = toVal.str();
      }
         break;

      default:
         throw GmatBaseException("ERROR: unrecognized parameter ID: '" + std::to_string(id) + "' on object '" + GetName() + "' of type '" + GetTypeName() +"'");
      }
   }
   catch (BaseException &ex)
   {
      value = "API exception caught: " + ex.GetFullMessage();
   }

   return value;
}

//------------------------------------------------------------------------------
// std::string  GetField(const std::string &label)
//------------------------------------------------------------------------------
/**
 * Retrieves a parameter value
 *
 * @param label The script label for the parameter
 *
 * @return The parameter value, inside of a string
 */
//------------------------------------------------------------------------------
std::string  GmatBase::GetField(const std::string &label)
{
   return GetField(GetParameterID(label));
}

//------------------------------------------------------------------------------
// void SetNumber(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets numeric parameter data
 *
 * @param id The ID of the field that is to be set
 * @param value The new field value
 */
//------------------------------------------------------------------------------
void GmatBase::SetNumber(const Integer id, const Real value)
{
   UnsignedInt type = GetParameterType(id);
   switch (type)
   {
   case Gmat::REAL_TYPE:
      SetRealParameter(id, value);
      break;

   case Gmat::INTEGER_TYPE:
      SetIntegerParameter(id, (Integer)value);
      break;

   default:
      {
         std::stringstream msg;
         msg << "The field " << GetParameterText(id)
             << " cannot be used to set a numeric value because it has "
             << "the non-numeric  type " << GetParameterTypeString(id)
             << "; try using SetField instead.";
         throw GmatBaseException(msg.str());
      }
   }
}

//------------------------------------------------------------------------------
// void SetNumber(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets numeric parameter data
 *
 * @param label The name of the field that is to be set
 * @param value The new field value
 */
//------------------------------------------------------------------------------
void GmatBase::SetNumber(const std::string &label, const Real value)
{
   SetNumber(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// Real GetNumber(const Integer id)
//------------------------------------------------------------------------------
/**
 * Retrieves numeric parameter data
 *
 * @param id The parameter ID for the data
 *
 * @return The value
 */
//------------------------------------------------------------------------------
Real GmatBase::GetNumber(const Integer id)
{
   Real retval;

   UnsignedInt type = GetParameterType(id);
   switch (type)
   {
   case Gmat::REAL_TYPE:
      retval = GetRealParameter(id);
      break;

   case Gmat::INTEGER_TYPE:
      retval = GetIntegerParameter(id);
      break;

   default:
      {
         std::stringstream msg;
         msg << "The field " << GetParameterText(id)
             << " cannot be used to get a numeric value because it has "
             << "the non-numeric  type " << GetParameterTypeString(id);
         throw GmatBaseException(msg.str());
      }
   }

   return retval;
}

//------------------------------------------------------------------------------
// Real GetNumber(const std::string &label)
//------------------------------------------------------------------------------
/**
 * Retrieves numeric parameter data
 *
 * @param label The name of the field that is to be set
 *
 * @return The value
 */
//------------------------------------------------------------------------------
Real GmatBase::GetNumber(const std::string &label)
{
   return GetNumber(GetParameterID(label));
}

//------------------------------------------------------------------------------
// void SetNumber(const Integer id, const Real value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets numeric parameter data in an RVector
 *
 * @param id The parameter ID for the data to be set
 * @param value The new field value
 * @param index The index in the vector to set
 *
 */
 //------------------------------------------------------------------------------
void GmatBase::SetNumber(const Integer id, const Real value, const Integer index)
{
   UnsignedInt type = GetParameterType(id);
   switch (type)
   {
      case Gmat::RVECTOR_TYPE:
         SetRealParameter(id, value, index);
         break;

      default:
      {
         std::stringstream msg;
         msg << "The field " << GetParameterText(id)
            << " cannot be used to set an indexed numeric value because it has "
            << "the non indexed numeric type " << GetParameterTypeString(id)
            << ". When an index is passed, only Rvector types are allowed. Try using SetField instead.";
         throw GmatBaseException(msg.str());
      }
   }
}

//------------------------------------------------------------------------------
// void SetNumber(const std::string & label, const Real value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets numeric parameter data in an RVector
 *
 * @param label The name of the field that is to be set
 * @param value The new field value
 * @param index The index in the vector to set
 *
 */
 //------------------------------------------------------------------------------
void GmatBase::SetNumber(const std::string & label, const Real value, const Integer index)
{
   SetNumber(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// Real GetNumber(const Integer id, const Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves numeric parameter data from an RVector
 *
 * @param id The parameter ID for the data
 * @param index The index in the vector to return
 *
 * @return The value
 */
 //------------------------------------------------------------------------------
Real GmatBase::GetNumber(const Integer id, const Integer index)
{
   Real retval;

   UnsignedInt type = GetParameterType(id);
   switch (type)
   {
      case Gmat::RVECTOR_TYPE:
         retval = GetRealParameter(id, index);
         break;

      default:
      {
         std::stringstream msg;
         msg << "The field " << GetParameterText(id)
            << " cannot be used to get an indexed numeric value because it has "
            << "the non indexed numeric type " << GetParameterTypeString(id) << 
            ". When an index is passed, only Rvector types are allowed.";
         throw GmatBaseException(msg.str());
      }
   }

   return retval;
}

//------------------------------------------------------------------------------
// Real GetNumber(const std::string &label, const Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves numeric parameter data from an Rvector
 *
 * @param label The name of the field that is to be set
 * @param index The index in the vector to return
 *
 * @return The value
 */
 //------------------------------------------------------------------------------
Real GmatBase::GetNumber(const std::string & label, const Integer index)
{
   return GetNumber(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// void SetVector(const Integer id, const Rvector &value)
//------------------------------------------------------------------------------
/**
 * Sets a vector parameter
 *
 * @param id The parameter ID for the data
 * @param value The new field value
 *
 */
 //------------------------------------------------------------------------------
void GmatBase::SetVector(const Integer id, const Rvector &value)
{
   UnsignedInt type = GetParameterType(id);
   switch (type)
   {
      case Gmat::RVECTOR_TYPE:
         SetRvectorParameter(id, value);
         break;

      default:
      {
         std::stringstream msg;
         msg << "The field " << GetParameterText(id)
            << " cannot be used to set a vector value because it has "
            << "the non-vector type " << GetParameterTypeString(id)
            << ". Try using SetField instead.";
         throw GmatBaseException(msg.str());
      }
   }
}

//------------------------------------------------------------------------------
// void SetVector(const std::string & label, const Rvector &value)
//------------------------------------------------------------------------------
/**
 * Sets a vector parameter
 *
 * @param label The name of the field that is to be set
 * @param value The new field value
 *
 */
 //------------------------------------------------------------------------------
void GmatBase::SetVector(const std::string & label, const Rvector &value)
{
   SetVector(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// Rvector GetVector(const Integer id)
//------------------------------------------------------------------------------
/**
 * Gets a vector parameter
 *
 * @param id The parameter ID for the data
 *
 * @return The value
 */
//------------------------------------------------------------------------------
Rvector GmatBase::GetVector(const Integer id)
{
   Rvector retval;
   UnsignedInt type = GetParameterType(id);
   switch (type)
   {
      case Gmat::RVECTOR_TYPE:
         retval = GetRvectorParameter(id);
         break;

      default:
      {
         std::stringstream msg;
         msg << "The field " << GetParameterText(id)
            << " cannot be used to get a vector value because it has "
            << "the non-vector type " << GetParameterTypeString(id)
            << ". Try using GetField instead.";
         throw GmatBaseException(msg.str());
      }
   }
   return retval;
}

//------------------------------------------------------------------------------
// Rvector GetVector(const std::string & label)
//------------------------------------------------------------------------------
/**
 * Gets a vector parameter
 *
 * @param label The name of the field that is to be set
 * 
 * @return The value
 */
 //------------------------------------------------------------------------------
Rvector GmatBase::GetVector(const std::string & label)
{
   return GetVector(GetParameterID(label));
}

//------------------------------------------------------------------------------
// void SetNumber(const Integer id, const Real value, const Integer row, const Integer col)
//------------------------------------------------------------------------------
/**
 * Sets numeric parameter data in an RMatrix
 *
 * @param id The parameter ID for the data to be set
 * @param value The new field value
 * @param row The row in the matrix to return
 * @param col The column in the matrix to return
 *
 */
 //------------------------------------------------------------------------------
void GmatBase::SetNumber(const Integer id, const Real value, const Integer row, const Integer col)
{
   UnsignedInt type = GetParameterType(id);
   switch (type)
   {
      case Gmat::RMATRIX_TYPE:
         SetRealParameter(id, value, row, col);
         break;

      default:
      {
         std::stringstream msg;
         msg << "The field " << GetParameterText(id)
            << " cannot be used to set an indexed numeric value because it has "
            << "the non indexed numeric  type " << GetParameterTypeString(id)
            << ". When a row and a column are passed, only Rmatrix types are allowed. Try using SetField instead.";
         throw GmatBaseException(msg.str());
      }
   }
}

//------------------------------------------------------------------------------
// void SetNumber(const std::string & label, const Real value, const Integer row, const Integer col)
//------------------------------------------------------------------------------
/**
 * Sets numeric parameter data in an RMatrix
 *
 * @param label The name of the field that is to be set
 * @param value The new field value
 * @param row The row in the matrix to return
 * @param col The column in the matrix to return
 *
 */
 //------------------------------------------------------------------------------
void GmatBase::SetNumber(const std::string & label, const Real value, const Integer row, const Integer col)
{
   SetNumber(GetParameterID(label), value, row, col);
}


//------------------------------------------------------------------------------
// Real GetNumber(const Integer id, const Integer row, const Integer col)
//------------------------------------------------------------------------------
/**
 * Retrieves numeric parameter data from an RMatrix
 *
 * @param id The parameter ID for the data
 * @param row The row in the matrix to return
 * @param col The column in the matrix to return
 *
 * @return The value
 */
 //------------------------------------------------------------------------------
Real GmatBase::GetNumber(const Integer id, const Integer row, const Integer col)
{
   Real retval;

   UnsignedInt type = GetParameterType(id);
   switch (type)
   {
   case Gmat::RMATRIX_TYPE:
      retval = GetRealParameter(id, row, col);
      break;

   default:
   {
      std::stringstream msg;
      msg << "The field " << GetParameterText(id)
         << " cannot be used to get an indexed numeric value because it has "
         << "the non indexed numeric type " << GetParameterTypeString(id) <<
         ". When a row and a column are passed, only Rmatrix types are allowed.";
      throw GmatBaseException(msg.str());
   }
   }

   return retval;
}


//------------------------------------------------------------------------------
// Real GetNumber(const std::string & label, const Integer row, const Integer col)
//------------------------------------------------------------------------------
/**
 * Retrieves numeric parameter data from an RMatrix
 *
 * @param label The name of the field that is to be set
 * @param row The row in the matrix to return
 * @param col The column in the matrix to return
 *
 * @return The value
 */
 //------------------------------------------------------------------------------
Real GmatBase::GetNumber(const std::string & label, const Integer row, const Integer col)
{
   return GetNumber(GetParameterID(label), row, col);
}

//------------------------------------------------------------------------------
// void SetMatrix(const Integer id, const Rmatrix &value)
//------------------------------------------------------------------------------
/**
 * Sets a matrix parameter
 *
 * @param id The parameter ID for the data
 * @param value The new field value
 *
 */
 //------------------------------------------------------------------------------
void GmatBase::SetMatrix(const Integer id, const Rmatrix &value)
{
   UnsignedInt type = GetParameterType(id);
   switch (type)
   {
   case Gmat::RMATRIX_TYPE:
      SetRmatrixParameter(id, value);
      break;

   default:
   {
      std::stringstream msg;
      msg << "The field " << GetParameterText(id)
         << " cannot be used to set a matrix value because it has "
         << "the non-matrix  type " << GetParameterTypeString(id)
         << ". Try using SetField instead.";
      throw GmatBaseException(msg.str());
   }
   }
}

//------------------------------------------------------------------------------
// void SetMatrix(const std::string & label, const Rmatrix &value)
//------------------------------------------------------------------------------
/**
 * Sets a matrix parameter
 *
 * @param label The name of the field that is to be set
 * @param value The new field value
 *
 */
 //------------------------------------------------------------------------------
void GmatBase::SetMatrix(const std::string & label, const Rmatrix &value)
{
   SetMatrix(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// Rmatrix GetMatrix(const Integer id)
//------------------------------------------------------------------------------
/**
 * Sets a matrix parameter
 *
 * @param id The parameter ID for the data
 * 
 * @return The value
 */
 //------------------------------------------------------------------------------
Rmatrix GmatBase::GetMatrix(const Integer id)
{
   Rmatrix retval;
   UnsignedInt type = GetParameterType(id);
   switch (type)
   {
      case Gmat::RMATRIX_TYPE:
         retval = GetRmatrixParameter(id);
         break;

      default:
      {
         std::stringstream msg;
         msg << "The field " << GetParameterText(id)
            << " cannot be used to get a matrix value because it has "
            << "the non-matrix type " << GetParameterTypeString(id)
            << ". Try using GetField instead.";
         throw GmatBaseException(msg.str());
      }
   }
   return retval;
}

//------------------------------------------------------------------------------
// Rmatrix GetMatrix(const std::string & label)
//------------------------------------------------------------------------------
/**
 * Gets a matrix parameter
 *
 * @param label The name of the field that is to be set
 *
 * @return The value
 */
 //------------------------------------------------------------------------------
Rmatrix GmatBase::GetMatrix(const std::string & label)
{
   return GetMatrix(GetParameterID(label));
}

//---------------------------------------------------------------------------
//  const std::string GetCommentLine() const
//---------------------------------------------------------------------------
const std::string GmatBase::GetCommentLine() const
{
   #ifdef DEBUG_COMMENT_LINE
   MessageInterface::ShowMessage
      ("GmatBase::GetCommentLine() returning '%s'\n", commentLine.c_str());
   #endif
   return commentLine;
}


//------------------------------------------------------------------------------
// bool GmatBase::SetReference(GmatBase* obj, Integer index = -1)
//------------------------------------------------------------------------------
/**
 * Simplified reference object setting code
 *
 * @param obj The reference object
 * @param index The index for settigns that need an index
 *
 * @return true if the reference was set
 */
//------------------------------------------------------------------------------
bool GmatBase::SetReference(GmatBase* obj, Integer index)
{
   bool retval = false;

   if (index == -1)
      retval = SetRefObject(obj, obj->GetType(), obj->GetName());
   else
      retval = SetRefObject(obj, obj->GetType(), obj->GetName(), index);

   return retval;
}


//---------------------------------------------------------------------------
//  void SetCommentLine(const std::string &comment)
//---------------------------------------------------------------------------
/**
 * Sets comment line. Comment line starts with % sign.
 * ex) %--- This is comment line
 *     Create Spacecraft sat;
 */
//---------------------------------------------------------------------------
void GmatBase::SetCommentLine(const std::string &comment)
{
   #ifdef DEBUG_COMMENT_LINE
   MessageInterface::ShowMessage
      ("GmatBase::SetCommentLine() <%p><%s>'%s' setting commentLine to <%s>\n",
       this, typeName.c_str(), instanceName.c_str(), comment.c_str());
   #endif
   commentLine = comment;
}

//---------------------------------------------------------------------------
//  const std::string GetInlineComment() const
//---------------------------------------------------------------------------
const std::string GmatBase::GetInlineComment() const
{
   return inlineComment;
}

//---------------------------------------------------------------------------
//  void SetInlineComment(const std::string &comment)
//---------------------------------------------------------------------------
/**
 * Sets inline comment. Inline comment starts with % sign after GMAT statement.
 * ex) Create Spacecraft sat; %--- This is inline comment
 */
//---------------------------------------------------------------------------
void GmatBase::SetInlineComment(const std::string &comment)
{
   #ifdef DEBUG_COMMENT_LINE
   MessageInterface::ShowMessage
      ("GmatBase::SetInlineComment() <%p><%s>'%s' setting inlineComment to <%s>\n",
       this, typeName.c_str(), instanceName.c_str(), comment.c_str());
   #endif
   inlineComment = comment;
}

//---------------------------------------------------------------------------
//  const std::string GetAttributeCommentLine(Integer index))
//---------------------------------------------------------------------------
const std::string GmatBase::GetAttributeCommentLine(Integer index)
{
   #ifdef DEBUG_COMMENTS_ATTRIBUTE
   MessageInterface::ShowMessage
      ("GmatBase::GetAttributeCommentLine() <%p><%s>'%s' entered, index=%d\n",
       this, typeName.c_str(), instanceName.c_str(), index);
   #endif
   if (index >= (Integer)attributeCommentLines.size())
   {
      #ifdef DEBUG_COMMENTS_ATTRIBUTE
      MessageInterface::ShowMessage
         ("Attribute comment name:%s index:%d has not been retrieved.\n",
          instanceName.c_str(), index);
      #endif
      return "";
   }

   #ifdef DEBUG_COMMENTS_ATTRIBUTE
   MessageInterface::ShowMessage
      ("Getting Attribute comment name:%s index:%d - %s.\n",
       instanceName.c_str(), index, attributeCommentLines[index].c_str());
   #endif
   return attributeCommentLines[index];
}

//---------------------------------------------------------------------------
//  void SetAttributeCommentLine(Integer index, const std::string &comment)
//---------------------------------------------------------------------------
/**
 * Sets attribute comment line. Attribute comment line starts with % sign
 % before object property setting.
 * ex) %--- This is attribute comment line
 *     sat.X = 7000;
 */
//---------------------------------------------------------------------------
void GmatBase::SetAttributeCommentLine(Integer index,
                                       const std::string &comment)
{
   #ifdef DEBUG_COMMENTS_ATTRIBUTE
   MessageInterface::ShowMessage
      ("GmatBase::SetAttributeCommentLine() <%p><%s>'%s' entered, index=%d, "
       "comment=<%s>\n", this, typeName.c_str(), instanceName.c_str(), index,
       comment.c_str());
   #endif
   if (index >= (Integer)attributeCommentLines.size())
   {
      #ifdef DEBUG_COMMENTS_ATTRIBUTE
      MessageInterface::ShowMessage("Attribute comment index:%d - %s - has not been set. Size=%d\n",
         index, comment.c_str(), (Integer)attributeCommentLines.size());
      #endif
      return;
   }

   #ifdef DEBUG_COMMENTS_ATTRIBUTE
   MessageInterface::ShowMessage
      ("Setting Attribute comment index:%d - %s.\n", index, comment.c_str());
   #endif

   attributeCommentLines[index] = comment;

}

//---------------------------------------------------------------------------
//  const std::string GetInlineAttributeComment(Integer index)
//---------------------------------------------------------------------------
/**
 * Sets inline attribute comment. Attribute inline comment starts with % sign
 % after object property setting.
 * ex) sat.X = 7000; %--- This is attribute inline comment
 */
//---------------------------------------------------------------------------
const std::string GmatBase::GetInlineAttributeComment(Integer index)
{
   if (index >= (Integer)attributeInlineComments.size())
   {
      #ifdef DEBUG_COMMENTS_ATTRIBUTE
      MessageInterface::ShowMessage
         ("Inline attribute comment name:%s index:%d has not been retrieved.\n",
          instanceName.c_str(), index);
      #endif
      return "";
   }

   #ifdef DEBUG_COMMENTS_ATTRIBUTE
   MessageInterface::ShowMessage
      ("Getting Inline attribute comment name:%s index:%d - %s.\n",
       instanceName.c_str(), index, attributeInlineComments[index].c_str());
   #endif

   return attributeInlineComments[index];
}

//---------------------------------------------------------------------------
//  void SetInlineAttributeComment(Integer index, const std::string &comment)
//---------------------------------------------------------------------------
void GmatBase::SetInlineAttributeComment(Integer index,
                                         const std::string &comment)
{
   #ifdef DEBUG_COMMENTS_ATTRIBUTE
   MessageInterface::ShowMessage
      ("GmatBase::SetInlineAttributeComment() <%p><%s>'%s' entered, index=%d, "
       "comment=<%s>\n", this, typeName.c_str(), instanceName.c_str(), index,
       comment.c_str());
   #endif
   
   if (index >= (Integer)attributeInlineComments.size())
   {
      #ifdef DEBUG_COMMENTS_ATTRIBUTE
      MessageInterface::ShowMessage
         ("Inline attribute comment - %s - has not been set. Size=%d\n",
          comment.c_str(), (Integer)attributeInlineComments.size());
      #endif
      return;
   }
   
   #ifdef DEBUG_COMMENTS_ATTRIBUTE
   MessageInterface::ShowMessage
      ("Setting Inline attribute comment - %s.\n", comment.c_str());
   #endif
   attributeInlineComments[index] = comment;
}

//---------------------------------------------------------------------------
//  bool GetBooleanParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param  id  The integer ID for the parameter.
 *
 * @return the boolean value for this parameter, or throw an exception if the
 *         parameter access in invalid.
 */
//------------------------------------------------------------------------------
bool GmatBase::GetBooleanParameter(const Integer id) const
{
   std::stringstream idString;
   idString << id;
   throw GmatBaseException("Cannot get boolean parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//---------------------------------------------------------------------------
//  bool SetBooleanParameter(const Integer id, const bool value)
//---------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new value.
 *
 * @return the boolean value for this parameter, or throw an exception if the
 *         parameter is invalid or not boolean.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetBooleanParameter(const Integer id, const bool value)
{
   std::stringstream idString;
   idString << id;
   throw GmatBaseException("Cannot set boolean parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//---------------------------------------------------------------------------
//  bool GetBooleanParameter(const Integer id, const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param id The integer ID for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
//------------------------------------------------------------------------------
bool GmatBase::GetBooleanParameter(const Integer id, const Integer index) const
{
   std::stringstream indexString, idString;
   idString << id;
   indexString << index;
   throw GmatBaseException("Cannot get boolean parameter with ID " +
                           idString.str() + " and index " + indexString.str() +
                           " on " + typeName + " named \"" + instanceName + "\"");
}


//---------------------------------------------------------------------------
//  bool SetBooleanParameter(const Integer id, const bool value,
//                           const Integer index)
//---------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new value for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetBooleanParameter(const Integer id, const bool value,
                                   const Integer index)
{
   std::stringstream indexString, idString;
   idString << id;
   indexString << index;
   throw GmatBaseException("Cannot set boolean parameter with ID " +
                           idString.str() + " and index " + indexString.str() +
                           " on " + typeName + " named \"" + instanceName + "\"");
}


//---------------------------------------------------------------------------
//const BooleanArray& GetBooleanArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a BooleanArray parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
const BooleanArray& GmatBase::GetBooleanArrayParameter(const Integer id) const
{
   std::stringstream idString;
   idString << id;
   throw GmatBaseException("Cannot get BooleanArray parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//---------------------------------------------------------------------------
//const BooleanArray& GetBooleanArrayParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a BooleanArray parameter.
 *
 * @param label The string label for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
const BooleanArray& GmatBase::GetBooleanArrayParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetBooleanArrayParameter(id);
}


//---------------------------------------------------------------------------
//  bool SetBooleanArrayParameter(const Integer id, const BooleanArray &valueArray)
//---------------------------------------------------------------------------
/**
 * Sets the value for a BooleanArray parameter.
 *
 * @param id The integer ID for the parameter.
 * @param valueArray The string of new value list enclosed with brackets,
 *                  such as [true false]
 *
 * @return true if value set successfully, or throw an exception if the
 *         parameter is invalid or not boolean.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetBooleanArrayParameter(const Integer id,
                                        const BooleanArray &valueArray)
{
   std::stringstream idString;
   idString << id;
   throw GmatBaseException("Cannot set BooleanArray parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}

//------------------------------------------------------------------------------
// Generic GetGenericParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor for a Generic
 *
 * @param id ID for the generic parameter
 *
 * @return The generic, or throw if it is not available
 */
//------------------------------------------------------------------------------
Generic GmatBase::GetGenericParameter(const Integer id) const
{
   std::stringstream idString;
   idString << id;
   throw GmatBaseException("Cannot access Generic parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}

//------------------------------------------------------------------------------
// Generic SetGenericParameter(const Integer id, const Generic value)
//------------------------------------------------------------------------------
/**
 * Setter for a Generic
 *
 * @param id ID for the generic parameter
 * @param vaue The new data for the parameter
 *
 * @return The generic, or throw if it is not available
 */
//------------------------------------------------------------------------------
Generic GmatBase::SetGenericParameter(const Integer id, const Generic value)
{
   std::stringstream idString;
   idString << id;
   throw GmatBaseException("Cannot set Generic parameter with ID " +
                           idString.str() + " on " + typeName + " named \"" +
                           instanceName + "\"");
}


//------------------------------------------------------------------------------
// Integer CheckGenericTypeSupport(const Integer id, const Integer genType,
//                                const std::string setting)
//------------------------------------------------------------------------------
/**
 * Checks to see if a generic will translate to the underlying data type needed
 *
 * @param id The Parameter ID
 * @param genType The detected type of the generic data
 * @param setting Optional string for subtyping the incoming data
 *
 * @return -1 if the incoming data needs to be rejected.  Otherwise the type of
 *         the container that will receive the data.  Returns Gmat::GENERIC_TYPE
 *         if all types are accepted,
 */
//------------------------------------------------------------------------------
Integer GmatBase::CheckGenericTypeSupport(const Integer id,
                        const Integer genType, const std::string &setting)
{
   return Gmat::GENERIC_TYPE;
}


//------------------------------------------------------------------------------
// Generic GetGenericParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor for a Generic
 *
 * @param label Script name for the generic parameter
 *
 * @return The generic, or throw if it is not available
 */
//------------------------------------------------------------------------------
Generic GmatBase::GetGenericParameter(const std::string &label) const
{
   return GetGenericParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Generic SetGenericParameter(const std::string &label, const Generic value)
//------------------------------------------------------------------------------
/**
 * Setter for a Generic
 *
 * @param label Script name for the generic parameter
 * @param vaue The new data for the parameter
 *
 * @return The generic, or throw if it is not available
 */
//------------------------------------------------------------------------------
Generic GmatBase::SetGenericParameter(const std::string &label,
                                            const Generic value)
{
   return SetGenericParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// Integer CheckGenericTypeSupport(const std::string &label,
//                                 const Integer genType,
//                                 const std::string setting)
//------------------------------------------------------------------------------
/**
 * Checks to see if a generic will translate to the underlying data type needed
 *
 * @param label Script name for the generic parameter
 * @param genType The detected type of the generic data
 * @param setting Optional string for subtyping the incoming data
 *
 * @return -1 if the incoming data needs to be rejected.  Otherwise the type of
 *         the container that will receive the data.  Returns Gmat::TypeCount if
 *         all types are accepted,
 */
//------------------------------------------------------------------------------
Integer GmatBase::CheckGenericTypeSupport(const std::string &label,
                        const Integer genType, const std::string &setting)
{
   return CheckGenericTypeSupport(GetParameterID(label), genType, setting);
}


//------------------------------------------------------------------------------
// UnsignedInt GetType(const Generic &forGeneric)
//------------------------------------------------------------------------------
/**
 * Returns the contained type from a Generic
 *
 * @param forGeneric The generic containing data
 *
 * @return The type of the contained data
 */
//------------------------------------------------------------------------------
UnsignedInt GmatBase::GetType(const Generic &forGeneric)
{
   UnsignedInt retval = Gmat::GENERIC_TYPE;

   retval = GmatStringUtil::GetGenericType(forGeneric);

   return retval;
}


//---------------------------------------------------------------------------
//  bool SetBooleanArrayParameter(const std::string &label,
//                                const BooleanArray &valueArray)
//---------------------------------------------------------------------------
/**
 * Sets the value for a BooleanArray parameter.
 *
 * @param  label  The (string) label for the parameter.
 * @param  valueArray  The string of new value list enclosed with brackets,
 *                    such as [true false]
 *
 * @return true if value set successfully, or throw an exception if the
 *         parameter is invalid or not boolean.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetBooleanArrayParameter(const std::string &label,
                                        const BooleanArray &valueArray)
{
   Integer id = GetParameterID(label);
   return SetBooleanArrayParameter(id, valueArray);
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real GmatBase::GetRealParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetRealParameter(id);
}


//---------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real GmatBase::SetRealParameter(const std::string &label, const Real value)
{
   Integer id = GetParameterID(label);
   return SetRealParameter(id, value);
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label, const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param label The (string) label for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real GmatBase::GetRealParameter(const std::string &label,
                                const Integer index) const
{
   Integer id = GetParameterID(label);
   return GetRealParameter(id, index);
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label, const Integer row,
//                        const Integer col) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter from an array.
 *
 * @param label The (string) label for the parameter.
 * @param row Row index for parameters in arrays.
 * @param col Column index for parameters in arrays.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real GmatBase::GetRealParameter(const std::string &label, const Integer row,
                                const Integer col) const
{
   Integer id = GetParameterID(label);
   return GetRealParameter(id, row, col);
}



//---------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value,
//                        const Integer index)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real GmatBase::SetRealParameter(const std::string &label, const Real value,
                                const Integer index)
{
   Integer id = GetParameterID(label);
   return SetRealParameter(id, value, index);
}


//---------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value,
//                        const Integer row, const Integer col)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter in an array.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new parameter value.
 * @param <row>   The row that receives the value.
 * @param <col>   The column that receives the value value.
 *
 * @return the parameter value at the end of this call.
 */
//---------------------------------------------------------------------------
Real GmatBase::SetRealParameter(const std::string &label, const Real value,
                      const Integer row, const Integer col)
{
   Integer id = GetParameterID(label);
   return SetRealParameter(id, value, row, col);
}


//---------------------------------------------------------------------------
//  Integer GetIntegerParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Integer parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Integer GmatBase::GetIntegerParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetIntegerParameter(id);
}


//---------------------------------------------------------------------------
//  Integer SetIntegerParameter(const std::string &label, const Integer value)
//---------------------------------------------------------------------------
/**
* Set the value for an Integer parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         INTEGER_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not an Integer.
 */
//------------------------------------------------------------------------------
Integer GmatBase::SetIntegerParameter(const std::string &label,
                                      const Integer value)
{
   Integer id = GetParameterID(label);
   return SetIntegerParameter(id, value);
}

//---------------------------------------------------------------------------
//  Integer GetIntegerParameter(const std::string &label,
//                              const Integer index) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Integer parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Integer GmatBase::GetIntegerParameter(const std::string &label,
                                      const Integer index) const
{
   Integer id = GetParameterID(label);
   return GetIntegerParameter(id, index);
}


//---------------------------------------------------------------------------
//  Integer SetIntegerParameter(const std::string &label, const Integer value,
//                              const Integer index)
//---------------------------------------------------------------------------
/**
* Set the value for an Integer parameter.
 *
 * @param label The (string) label for the parameter.
 * @param value The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or
 *         INTEGER_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not an Integer.
 */
//------------------------------------------------------------------------------
Integer GmatBase::SetIntegerParameter(const std::string &label,
                                      const Integer value,
                                      const Integer index)
{
   Integer id = GetParameterID(label);
   return SetIntegerParameter(id, value, index);
}

//---------------------------------------------------------------------------
//  UnsignedInt GetUnsignedIntParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for an UnsignedInt parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
UnsignedInt GmatBase::GetUnsignedIntParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetUnsignedIntParameter(id);
}


//---------------------------------------------------------------------------
//  UnsignedInt SetUnsignedIntParameter(const std::string &label,
//                                      const UnsignedInt value)
//---------------------------------------------------------------------------
/**
* Set the value for an UnsignedInt parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         UNSIGNED_INT_PARAMETER_UNDEFINED if the parameter id is invalid
 *         or the parameter type is not an UnsignedInt.
 */
//------------------------------------------------------------------------------
UnsignedInt GmatBase::SetUnsignedIntParameter(const std::string &label,
                                              const UnsignedInt value)
{
   Integer id = GetParameterID(label);
   return SetUnsignedIntParameter(id, value);
}

//---------------------------------------------------------------------------
//  UnsignedInt GetUnsignedIntParameter(const std::string &label,
//                                      const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for an UnsignedInt parameter.
 *
 * @param label The (string) label for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
UnsignedInt GmatBase::GetUnsignedIntParameter(const std::string &label,
                                              const Integer index) const
{
   Integer id = GetParameterID(label);
   return GetUnsignedIntParameter(id, index);
}


//---------------------------------------------------------------------------
//  UnsignedInt SetUnsignedIntParameter(const std::string &label,
//                                      const UnsignedInt value,
//                                      const Integer index)
//---------------------------------------------------------------------------
/**
* Set the value for an UnsignedInt parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or
 *         UNSIGNED_INT_PARAMETER_UNDEFINED if the parameter id is invalid
 *         or the parameter type is not an UnsignedInt.
 */
//------------------------------------------------------------------------------
UnsignedInt GmatBase::SetUnsignedIntParameter(const std::string &label,
                                              const UnsignedInt value,
                                              const Integer index)
{
   Integer id = GetParameterID(label);
   return SetUnsignedIntParameter(id, value, index);
}

//---------------------------------------------------------------------------
//  const UnsignedIntArray&
//  GetUnsignedIntArrayParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Access an array of unsigned int data.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The requested UnsignedIntArray; throws if the parameter is not a
 *         UnsignedIntArray.
 */
//------------------------------------------------------------------------------
const UnsignedIntArray&
GmatBase::GetUnsignedIntArrayParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetUnsignedIntArrayParameter(id);
}

//---------------------------------------------------------------------------
// const  Rvector& GetRvectorParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Rvector parameter.
*
* @param <label> The (string) label for the parameter.
*
* @return The parameter's value.
*/
//------------------------------------------------------------------------------
const Rvector& GmatBase::GetRvectorParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetRvectorParameter(id);
}


//---------------------------------------------------------------------------
//  const Rvector& SetRvectorParameter(const std::string &label,
//                                     const Rvector& value)
//---------------------------------------------------------------------------
/**
* Set the value for an Rvector parameter.
*
* @param <label> The (string) label for the parameter.
* @param <value> The new parameter value.
*
* @return the parameter value at the end of this call, or
*         RVECTOR_PARAMETER_UNDEFINED if the parameter id is invalid or the
*         parameter type is not Rvector.
*/
//------------------------------------------------------------------------------
const Rvector& GmatBase::SetRvectorParameter(const std::string &label,
                                             const Rvector& value)
{
   Integer id = GetParameterID(label);
   return SetRvectorParameter(id, value);
}

//---------------------------------------------------------------------------
//  const Rmatrix& GetRmatrixParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
* Retrieve the value for an Rmatrix parameter.
*
* @param <label> The (string) label for the parameter.
*
* @return The parameter's value.
*/
//------------------------------------------------------------------------------
const Rmatrix& GmatBase::GetRmatrixParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetRmatrixParameter(id);
}


//---------------------------------------------------------------------------
//  const Rmatrix& SetRmatrixParameter(const std::string &label,
//                                     const Rmatrix& value)
//---------------------------------------------------------------------------
/**
* Set the value for an Rmatrix parameter.
*
* @param <label> The (string) label for the parameter.
* @param <value> The new parameter value.
*
* @return the parameter value at the end of this call, or
*         RMATRIX_PARAMETER_UNDEFINED if the parameter id is invalid or the
*         parameter type is not Rmatrix.
*/
//------------------------------------------------------------------------------
const Rmatrix& GmatBase::SetRmatrixParameter(const std::string &label,
                                             const Rmatrix& value)
{
   Integer id = GetParameterID(label);
   return SetRmatrixParameter(id, value);
}

//---------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetStringParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetStringParameter(id);
}

//---------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const char *value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetStringParameter(const std::string &label,
                                  const char *value)
{
   Integer id = GetParameterID(label);
   return SetStringParameter(id, std::string(value));
}

//---------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetStringParameter(const std::string &label,
                                  const std::string &value)
{
   Integer id = GetParameterID(label);
   return SetStringParameter(id, value);
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label,
//                                 const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetStringParameter(const std::string &label,
                                         const Integer index) const
{
   Integer id = GetParameterID(label);
   return GetStringParameter(id, index);
}

//---------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value,
//                          const Integer index)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new string for this parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetStringParameter(const std::string &label,
                                  const std::string &value,
                                  const Integer index)
{
   Integer id = GetParameterID(label);
   return SetStringParameter(id, value, index);
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param label The (string) label for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& GmatBase::GetStringArrayParameter(
                                          const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetStringArrayParameter(id);
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const std::string &label,
//                                             const Integer index) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param label The (string) label for the parameter.
 * @param index Which string array if more than one is supported.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& GmatBase::GetStringArrayParameter(const std::string &label,
                                               const Integer index) const
{
   Integer id = GetParameterID(label);
   return GetStringArrayParameter(id, index);
}

//---------------------------------------------------------------------------
//  bool GetBooleanParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
//------------------------------------------------------------------------------
bool GmatBase::GetBooleanParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetBooleanParameter(id);
}


//---------------------------------------------------------------------------
//  bool SetBooleanParameter(const std::string &label, const bool value)
//---------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetBooleanParameter(const std::string &label, const bool value)
{
   Integer id = GetParameterID(label);
   return SetBooleanParameter(id, value);
}


//---------------------------------------------------------------------------
//  bool GetBooleanParameter(const std::string &label, const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
//------------------------------------------------------------------------------
bool GmatBase::GetBooleanParameter(const std::string &label,
                                   const Integer index) const
{
   Integer id = GetParameterID(label);
   return GetBooleanParameter(id, index);
}


//---------------------------------------------------------------------------
//  bool SetBooleanParameter(const std::string &label, const bool value)
//---------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetBooleanParameter(const std::string &label, const bool value,
                                   const Integer index)
{
   Integer id = GetParameterID(label);
   return SetBooleanParameter(id, value, index);
}


//---------------------------------------------------------------------------
//  std::string GetOnOffParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve an On/Off parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The "On/Off" string stored for this parameter.
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetOnOffParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetOnOffParameter(id);
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Change the value of an On/Off parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new "On/Off" string for this parameter.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool GmatBase::SetOnOffParameter(const std::string &label,
                                 const std::string &value)
{
   Integer id = GetParameterID(label);
   return SetOnOffParameter(id, value);
}


//---------------------------------------------------------------------------
//  bool TakeAction(const std::string &action, const std::string &actionData)
//---------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * @param <action> The string descriptor for the requested action.
 * @param <actionData> Optional data used for the action.
 *
 * @return true if the action was performed, false if not.
 */
//------------------------------------------------------------------------------
bool GmatBase::TakeAction(const std::string &action,
                          const std::string &actionData)
{
   if (action == "FromAPI")
   {
      isAPIObject = true;
      return true;
   }
   return false;
}

//---------------------------------------------------------------------------
//  bool TakeRequiredAction(const Integer id))
//---------------------------------------------------------------------------
/**
 * Tells object to take whatever action it needs to take before the value
 * of the specified parameter is set (e.g. clearing out arrays)
 *
 * @param <id> parameter for which to take prerequisite action.
 *
 * @return true if the action was performed (or none needed), false if not.
 */
//------------------------------------------------------------------------------
bool GmatBase::TakeRequiredAction(const Integer id)
{
   return true;
}

//---------------------------------------------------------------------------
//  bool TakeRequiredAction(const std::string &label)
//---------------------------------------------------------------------------
/**
 * Tells object to take whatever action it needs to take before the value
 * of the specified parameter is set (e.g. clearing out arrays)
 *
 * @param <label> parameter for which to take prerequisite action.
 *
 * @return true if the action was performed (or none needed), false if not.
 */
//------------------------------------------------------------------------------
bool GmatBase::TakeRequiredAction(const std::string &label)
{
   Integer id = GetParameterID(label);
   return TakeRequiredAction(id);
}


//------------------------------------------------------------------------------
// const std::vector<UnsignedInt>& GetTypesForList(const Integer id)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of types that need to be shown on a GUI for a parameter
 *
 * @param id The parameter ID
 *
 * @return The list of types
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& GmatBase::GetTypesForList(const Integer id)
{
   listedTypes.clear();
   return listedTypes;
}

//------------------------------------------------------------------------------
// const std::vector<UnsignedInt>&
//       GetTypesForList(const std::string &label)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of types that need to be shown on a GUI for a parameter
 *
 * @param label The parameter's identifying string
 *
 * @return The list of types
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& GmatBase::GetTypesForList(const std::string &label)
{
   return GetTypesForList(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool WriteEmptyStringArray
//------------------------------------------------------------------------------
/**
 * Returns a flag specifying whether or not to write out a StringArray to
 * the script even if it is empty
 *
 * @param id    ID of the parameter
 */
//------------------------------------------------------------------------------
bool GmatBase::WriteEmptyStringArray(Integer id)
{
   return false;
}

//------------------------------------------------------------------------------
// bool WriteEmptyStringParameter
//------------------------------------------------------------------------------
/**
 * Returns a flag specifying whether or not to write out a string to
 * the script even if it is empty
 *
 * @param id    ID of the parameter
 */
//------------------------------------------------------------------------------
bool GmatBase::WriteEmptyStringParameter(const Integer id) const
{
   return false;
}

//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
//       const std::string &prefix = "", const std::string &useName = "")
//------------------------------------------------------------------------------
/**
 * Produces a string, possibly multi-line, containing the text that produces an
 * object.
 *
 * @param mode Specifies the type of serialization requested.
 * @param prefix Optional prefix appended to the object's name
 * @param useName Name that replaces the object's name.
 *
 * @return A string containing the text.
 */
//------------------------------------------------------------------------------
const std::string& GmatBase::GetGeneratingString(Gmat::WriteMode mode,
                                                 const std::string &prefix,
                                                 const std::string &useName)
{
   #ifdef DEBUG_GENERATING_STRING
   MessageInterface::ShowMessage
      ("GmatBase::GetGeneratingString() <%p><%s>'%s' entered, mode=%d, prefix='%s', "
       "useName='%s', cloaking=%s\n", this, GetTypeName().c_str(), GetName().c_str(), mode,
       prefix.c_str(), useName.c_str(), cloaking ? "true" : "false");
   MessageInterface::ShowMessage
      ("   showPrefaceComment=%d, commentLine=<%s>\n   showInlineComment=%d "
       "inlineComment=<%s>\n",  showPrefaceComment, commentLine.c_str(),
       showInlineComment, inlineComment.c_str());
   MessageInterface::ShowMessage("   isCreatedFromMainScript=%d\n", isCreatedFromMainScript);
   #endif

   // don't write anything for cloaked objects, unless we're in SHOW_SCRIPT mode
   if ((mode != Gmat::SHOW_SCRIPT) && IsObjectCloaked())
   {
      generatingString = "";
      return generatingString;
   }
   
   // Don't write unless object is created from the main script
   // Skip this if a breakpoint is using DEBUG_INSPECT (CRC 2022.03.21)
   if (mode != Gmat::SHOW_SCRIPT && mode != Gmat::DEBUG_INSPECT && !isCreatedFromMainScript)
   {
      if (!forceGenerateObjectString)
      {
         generatingString = "";
         #ifdef DEBUG_GENERATING_STRING
         MessageInterface::ShowMessage
            ("GmatBase::GetGeneratingString() just returning blank, it is not "
             "created from the main script\n");
         #endif
         return generatingString;
      }
   }
   
   std::stringstream data;
   bool writeGmatKeyword = GmatGlobal::Instance()->IsWritingGmatKeyword();
   
   data.precision(GetDataPrecision()); // Crank up data precision so we don't lose anything
   std::string preface = "", nomme;
   
   if ((mode == Gmat::SCRIPTING)     || (mode == Gmat::OWNED_OBJECT) ||
       (mode == Gmat::SHOW_SCRIPT)   || (mode == Gmat::NO_COMMENTS)  ||
       (mode == Gmat::OBJECT_EXPORT) || (mode == Gmat::DEBUG_INSPECT))
      inMatlabMode = false;
   if (mode == Gmat::MATLAB_STRUCT || mode == Gmat::EPHEM_HEADER)
      inMatlabMode = true;

   if (useName != "")
      nomme = useName;
   else
      nomme = instanceName;

   if (mode == Gmat::SCRIPTING     || mode == Gmat::SHOW_SCRIPT ||
       mode == Gmat::EPHEM_HEADER  || mode == Gmat::NO_COMMENTS ||
       mode == Gmat::OBJECT_EXPORT || mode == Gmat::DEBUG_INSPECT)
   {
      std::string tname = typeName;
      if (tname == "PropSetup")
         tname = "Propagator";
      else if (tname == "DataFilter")
         tname = objectTypeNames[objectTypeNames.size() - 1];

      if (mode == Gmat::EPHEM_HEADER)
      {
         data << tname << " = " << "'" << nomme << "';\n";
         preface = "";
      }
      else if ((mode == Gmat::NO_COMMENTS) || (mode == Gmat::OBJECT_EXPORT) ||
            (mode == Gmat::DEBUG_INSPECT))
      {
         #ifdef DEBUG_GENERATING_STRING
         MessageInterface::ShowMessage("==> Do not show comments\n");
         #endif
         if (!cloaking)
            data << "Create " << tname << " " << nomme << ";\n";
         else
            data << "";
         // We now write out GMAT prefix on option from the startup file (see GMT-3233)
         if (writeGmatKeyword)
            preface = "GMAT ";
         else
            preface = "";
      }
      else
      {
         if (showPrefaceComment)
         {
            if ((commentLine != "") &&
                ((mode == Gmat::SCRIPTING) || (mode == Gmat::OWNED_OBJECT) ||
                 (mode == Gmat::SHOW_SCRIPT)))
               data << commentLine;
         }
         if (!cloaking)
            data << "Create " << tname << " " << nomme << ";";
         else
            data << "";
         
         if (showInlineComment)
         {
            if ((inlineComment != "") &&
                ((mode == Gmat::SCRIPTING) || (mode == Gmat::OWNED_OBJECT) ||
                 (mode == Gmat::SHOW_SCRIPT)))
               data << inlineComment << "\n";
            else
            {
               // Do not write extra line here for cloaked object (LOJ: 2013.12.17)
               if (!cloaking)
                  data << "\n";
            }
         }
         else
         {
            // Do not write extra line here for cloaked object (LOJ: 2013.12.17)
           if (!cloaking)
               data << "\n";
         }
         
         // We now write out GMAT prefix on option from the startup file (see GMT-3233)
         if (writeGmatKeyword)
            preface = "GMAT ";
         else
            preface = "";
      }
   }
   
   nomme += ".";

   if (mode == Gmat::OWNED_OBJECT) 
   {
      preface = prefix;
      nomme = "";
   }
      
   preface += nomme;
   WriteParameters(mode, preface, data);
   
   generatingString = data.str();
   
   #ifdef DEBUG_GENERATING_STRING
   MessageInterface::ShowMessage
      ("GmatBase::GetGeneratingString() returning\n'%s'\n", generatingString.c_str());
   #endif
   
   return generatingString;
}


//------------------------------------------------------------------------------
// StringArray GetGeneratingStringArray(Gmat::WriteMode mode = Gmat::SCRIPTING,
//             const std::string &prefix = "", const std::string &useName = "")
//------------------------------------------------------------------------------
/**
 * Produces a string array containing the line-by-line text that produces an
 * object.
 *
 * @param mode Specifies the type of serialization requested.
 * @param prefix Optional prefix appended to the object's name
 * @param useName Name that replaces the object's name.
 *
 * @return A string array containing the text.
 *
 * @note The current implementation just calls GetGeneratingString, and returns
 *       a multiline string in the first element of the string array.  A later
 *       update is needed to break that string into multiple entries in the
 *       string array.
 */
//------------------------------------------------------------------------------
StringArray GmatBase::GetGeneratingStringArray(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName)
{
   StringArray sar;
   std::string genstr = GetGeneratingString(mode, prefix, useName);
   std::string text;
   Integer start = 0, end = 0, len = genstr.length();

   while (end < len)
   {
      if (genstr[end] == '\n')
      {
         text = genstr.substr(start, end - start);
         sar.push_back(text);
         start = end+1;
      }
      ++end;
   }

   return sar;
}


//------------------------------------------------------------------------------
//  void FinalizeCreation()
//------------------------------------------------------------------------------
/**
 * Completes any post-construction steps needed before the object can be used.
 *
 * This method performs initialization of GmatBase properties that depend on the
 * features of the derived classes.  Derived classes touch some of the base
 * class properties -- the parameterCount, for example.  This method is called
 * after the object creation process is complete, so that any of the object's
 * base-class properties can be updated to reflect the object's actual
 * properties.
 */
//------------------------------------------------------------------------------
void GmatBase::FinalizeCreation()
{
   #ifdef DEBUG_FINALIZE_CREATION
   MessageInterface::ShowMessage
      ("GmatBase::FinalizeCreation() entered for <%s>'%s'\n", GetTypeName().c_str(),
       GetName().c_str());
   #endif
   PrepCommentTables();
}


//------------------------------------------------------------------------------
// virtual std::string BuildPropertyName(GmatBase *ownedObj)
//------------------------------------------------------------------------------
/*
 * Builds property name of given owned object.
 * This method is called when special naming of owned object property is
 * required when writing object. For example, ForceModel requires additional
 * name Earth for GravityField as in FM.GravityField.Earth.Degree.
 *
 * @param obj The owned object that needs special property naming
 * @return The property name
 */
//------------------------------------------------------------------------------
std::string GmatBase::BuildPropertyName(GmatBase *ownedObj)
{
   return "";
}


//------------------------------------------------------------------------------
// std::string GetLastErrorMessage()
//------------------------------------------------------------------------------
/**
 * Retrieves the last error message
 *
 * @return The last error message.
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetLastErrorMessage()
{
   return lastErrorMessage;
}


//------------------------------------------------------------------------------
// void SetLastErrorMessage(const std::string &msg)
//------------------------------------------------------------------------------
/**
 * Sets the last error message
 *
 * @param msg The last error messsage to set
 */
//------------------------------------------------------------------------------
void GmatBase::SetLastErrorMessage(const std::string &msg)
{
   lastErrorMessage = msg;
}


//------------------------------------------------------------------------------
// std::string GetErrorMessageFormat()
//------------------------------------------------------------------------------
/**
 * Retrieves the error message format string.
 *
 * @return The format string.
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetErrorMessageFormat()
{
   return errorMessageFormat;
}


//------------------------------------------------------------------------------
// void SetErrorMessageFormat(const std::string &fmt)
//------------------------------------------------------------------------------
/**
 * Sets the error message format string.
 *
 * @param fmt The format string.
 */
//------------------------------------------------------------------------------
void GmatBase::SetErrorMessageFormat(const std::string &fmt)
{
   errorMessageFormat = fmt;
}

//-------------------------------------
// public static functions
//-------------------------------------

//---------------------------------------------------------------------------
//  static Integer GetInstanceCount()
//---------------------------------------------------------------------------
/**
 * Find out how many GmatBase objects have been instantiated.
 *
 * @return The number of instantiated objects.
 */
Integer GmatBase::GetInstanceCount()
{
   return GmatBase::instanceCount;
}

//---------------------------------------------------------------------------
// static UnsignedInt GetObjectType(const std::string &typeString)
//---------------------------------------------------------------------------
/**
 * @param <typeString> object type string
 *
 * @return object type for given type string
 *
 */
//---------------------------------------------------------------------------
UnsignedInt GmatBase::GetObjectType(const std::string &typeString)
{
   for (int i=0; i<Gmat::UNKNOWN_OBJECT - Gmat::SPACECRAFT; i++)
      if (OBJECT_TYPE_STRING[i] == typeString)
         return (UnsignedInt)(i + Gmat::SPACECRAFT);

   return Gmat::UNKNOWN_OBJECT;
}

//---------------------------------------------------------------------------
// static std::string GetObjectTypeString(UnsignedInt type)
//---------------------------------------------------------------------------
/**
 * @param <type> object type
 *
 * @return object type string for given type
 *
 */
//---------------------------------------------------------------------------
std::string GmatBase::GetObjectTypeString(UnsignedInt type)
{
   return OBJECT_TYPE_STRING[type - Gmat::SPACECRAFT];
}

//------------------------------------------------------------------------------
// static Integer GmatBase::GetDataPrecision()
//------------------------------------------------------------------------------
Integer GmatBase::GetDataPrecision()
{
   return GmatGlobal::Instance()->GetDataPrecision();
}

//------------------------------------------------------------------------------
// static Integer GmatBase::GetTimePrecision()
//------------------------------------------------------------------------------
Integer GmatBase::GetTimePrecision()
{
   return GmatGlobal::Instance()->GetTimePrecision();
}

//------------------------------------------------------------------------------
// static std::string GmatBase::GetFullPathFileName(std::string &outFileName,
//        const std::string &objName, const std::string &fileName,
//        const std::string &fileType, bool forInput, const std::string &fileExt = "",
//        bool writeWarning = false, bool writeInfo = false)
//------------------------------------------------------------------------------
/**
 * Static method for getting full path file name for given file name and file type.
 * This method calls FileManager::FindPath() so it is call-through function.
 *
 * @param  outFileName  The file name without path to be returned.  This file name
 *                      is built only if input file name is blank
 * @param  inFileName   The requested file name
 *                      Enter blank name if default name to be used for the file type
 * @param  typeName     The file type name of the input file
 * @param  forInput     Set to true if filename is for input
 * @param  fileExt      File extension to be used for default filename
 * @param  writeWarning Set to true if warning should be written when no path found
 * @param  writeInfo    Set to true if information should be written for output path
 *
 * @return full path name using search order. Returns an empty string when file is
 *         not found. "File not found" error message is handled in the caller but not
 *         here.
 *
 */
//------------------------------------------------------------------------------
std::string GmatBase::GetFullPathFileName(std::string &outFileName,
                                          const std::string &objName,
                                          const std::string &inFileName,
                                          const std::string &fileType, bool forInput,
                                          const std::string &fileExt, bool writeWarning,
                                          bool writeInfo)
{
   #ifdef DEBUG_FILE_PATH
   MessageInterface::ShowMessage
      ("\nGmatBase::GetFullPathFileName() entered, objName='%s', inFileName='%s', fileType='%s', "
       "forInput=%d, fileExt='%s', writeWarning=%d, writeInfo=%d\n", objName.c_str(),
       inFileName.c_str(), fileType.c_str(), forInput, fileExt.c_str(), writeWarning, writeInfo);
   #endif
   
   std::string fname = inFileName;
   
   // If file is for output and input file name is blank, build outFileName
   if (!forInput && fname == "")
   {
      fname = objName + fileExt;
      outFileName = fname;
   }
   
   std::string fullPath =
      FileManager::Instance()->FindPath(fname, fileType, forInput, writeWarning, writeInfo);
   
   // If file is for input and input file name is blank, build outFileName
   if (forInput && fname == "")
   {
      outFileName = GmatFileUtil::ParseFileName(fullPath);
   }
   
   #ifdef DEBUG_FILE_PATH
   MessageInterface::ShowMessage
      ("GmatBase::GetFullPathFileName() returning outFileName='%s', fullPath='%s'\n",
       outFileName.c_str(), fullPath.c_str());
   #endif
   
   return fullPath;
}

//------------------------------------------------------------------------------
// static std::string WriteObjectInfo(const std::string &title, GmatBase *obj,
//                                    bool addEol = true)
//------------------------------------------------------------------------------
std::string GmatBase::WriteObjectInfo(const std::string &title, GmatBase *obj,
                                      bool addEol)
{
   std::stringstream objStream;
   char buff[20];  // was 10]; but that was too small
   sprintf(buff, "<%p>", obj);
   objStream << title << buff;
   
   if (obj)
   {
      objStream << "[" << obj->GetTypeName() << "]" << "'" << obj->GetName() << "'";
      objStream << ", IsGlobal:" << (obj->IsGlobal() ? "true" : "false") << ", IsLocal():"
                << (obj->IsLocal() ? "true" : "false");
   }
   else
   {
      objStream << "[NULL]'NULL'";
   }
   
   if (addEol)
      objStream << "\n";
   
   return objStream.str();
}


// todo: comments
Integer GmatBase::GetPropItemID(const std::string &whichItem)
{
   return -1;
}


Integer GmatBase::SetPropItem(const std::string &propItem)
{
   return Gmat::UNKNOWN_STATE;
}

StringArray GmatBase::GetDefaultPropItems()
{
   StringArray empty;
   return empty;
}

Real* GmatBase::GetPropItem(const Integer item)
{
   return NULL;
}

Integer GmatBase::GetPropItemSize(const Integer item)
{
   return -1;
}

bool GmatBase::PropItemNeedsFinalUpdate(const Integer item)
{
   return false;
}

bool GmatBase::HasAssociatedStateObjects()
{
   return false;
}

std::string GmatBase::GetAssociateName(UnsignedInt val)
{
   return "";
}


Integer GmatBase::SetEstimationParameter(const std::string &param)
{
   return -1;
}

//------------------------------------------------------------------------------
// Integer GmatBase::GetEstimationParameterID(const std::string &param)
//------------------------------------------------------------------------------
/**
 * This method builds the parameter ID used in the estimation subsystem
 *
 * @param param The text name of the estimation parameter
 *
 * @return The ID used in estimation for the parameter
 */
//------------------------------------------------------------------------------
Integer GmatBase::GetEstimationParameterID(const std::string &param)
{
   Integer id = type * ESTIMATION_TYPE_ALLOCATION; // Base for estimation ID

   try
   {
      id += GetParameterID(param);
   }
   catch (BaseException &)
   {
      return -1;
   }

   return id;
}

std::string GmatBase::GetParameterNameForEstimationParameter(const std::string &parmName)
{
   return parmName;
}

std::string GmatBase::GetParameterNameFromEstimationParameter(const std::string &parmName)
{
   return parmName;
}

//------------------------------------------------------------------------------
// bool GmatBase::IsEstimationParameterValid(Integer id)
//------------------------------------------------------------------------------
/**
 * Derived classes override this method when they provide the methods used by
 * the estimators to build the elements needed for estimation.
 *
 * @param id The estimation parameter ID for the parameter
 *
 * @return true if estimation can proceed with the input parameter in the
 *         estimation state and related matrices, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatBase::IsEstimationParameterValid(Integer id)
{
   return false;
}


Integer GmatBase::GetEstimationParameterSize(Integer id)
{
   Integer retval = 1;

   return retval;
}

Real* GmatBase::GetEstimationParameterValue(Integer id)
{
   return NULL;
}


//------------------------------------------------------------------------------
// StringArray GetSolveForList()
//------------------------------------------------------------------------------
/**
 * Finds solve-for parameters in the object, and returns the decorated list
 *
 * By "decorated list," what is meant is a list of the form
 * <objectname>.<parameter>, so the returned list can be used to find the owning
 * object and the parameter inside of that object.
 *
 * @return The list of solve-fors.
 */
//------------------------------------------------------------------------------
StringArray GmatBase::GetSolveForList()
{
   return solveForList;
}

//---------------------------------------------------------------------------
// void SetScriptCreatedFrom(const std::string &script)
//---------------------------------------------------------------------------
/**
 * Sets script name where this object is created from.
 */
//---------------------------------------------------------------------------
void GmatBase::SetScriptCreatedFrom(const std::string &script)
{
   scriptCreatedFrom = script;
}

//---------------------------------------------------------------------------
// std::string GetScriptCreatedFrom()
//---------------------------------------------------------------------------
/**
 * Retrieves script name where this object is created from.
 */
//---------------------------------------------------------------------------
std::string GmatBase::GetScriptCreatedFrom()
{
   return scriptCreatedFrom;
}

//---------------------------------------------------------------------------
// void SetIsCreatedFromMainScript(bool flag)
//---------------------------------------------------------------------------
/**
 * Sets flag indicating this object is created from the main script.
 */
//---------------------------------------------------------------------------
void GmatBase::SetIsCreatedFromMainScript(bool flag)
{
   isCreatedFromMainScript = flag;
}

//---------------------------------------------------------------------------
// bool IsCreatedFromMainScript()
//---------------------------------------------------------------------------
/**
 * Returns flag indicating this object is created from the main script.
 */
//---------------------------------------------------------------------------
bool GmatBase::IsCreatedFromMainScript()
{
   return isCreatedFromMainScript;
}

//---------------------------------------------------------------------------
// void SetForceGenerateObjectString(bool flag)
//---------------------------------------------------------------------------
/**
 * Sets flag indicating to generate object string even though it is not
 * created from the main script.
 */
//---------------------------------------------------------------------------
void GmatBase::SetForceGenerateObjectString(bool flag)
{
   forceGenerateObjectString = flag;
}

//---------------------------------------------------------------------------
// bool GetForceGenerateObjectString()
//---------------------------------------------------------------------------
/**
 * Returns flag indicating to generate object string even though it is not
 * created from the main script.
 */
//---------------------------------------------------------------------------
bool GmatBase::GetForceGenerateObjectString()
{
   return forceGenerateObjectString;
}

//---------------------------------------------------------------------------
// std::string AddFileInfoToErrorMessage(std::string msg)
//---------------------------------------------------------------------------
/**
 * Takes in an error message and adds in the line number and script this object 
 * was created in
 * 
 * @param msg, a string containing the error message to be appended
 * 
 * @return the new error message
 */
 //---------------------------------------------------------------------------
std::string GmatBase::AddFileInfoToErrorMessage(std::string msg)
{
    if (GetScriptCreatedFrom() == "")
        return msg;

    msg = msg + "\n This error occurred on line ";
    msg = msg + std::to_string(GetScriptLineNumber());
    msg = msg + " of file: \"";
    msg = msg + GetScriptCreatedFrom();
    msg = msg + "\"";
    return msg;
}


//-------------------------------------
// protected methods
//-------------------------------------

//---------------------------------------------------------------------------
//  void CopyParameters(const GmatBase &a)
//---------------------------------------------------------------------------
/**
 * Copies the parameters from one object into this object.
 *
 * @param <a> The object that is being copied.
 */
//---------------------------------------------------------------------------
void GmatBase::CopyParameters(const GmatBase &a)
{
   Integer i, iVal;
   Real rVal;
   std::string sVal;
   bool bVal;
   Gmat::ParameterType parmType;

   for (i = 0; i < parameterCount; ++i)
   {
      parmType = a.GetParameterType(i);

      if (parmType == Gmat::REAL_TYPE)
      {
         rVal = a.GetRealParameter(i);
         SetRealParameter(i, rVal);
      }

      if (parmType == Gmat::INTEGER_TYPE)
      {
         iVal = a.GetIntegerParameter(i);
         SetIntegerParameter(i, iVal);
      }

      if ((parmType == Gmat::STRING_TYPE) || (parmType == Gmat::FILENAME_TYPE))
      {
         sVal = a.GetStringParameter(i);
         SetStringParameter(i, sVal);
      }

      if (parmType == Gmat::BOOLEAN_TYPE)
      {
         bVal = a.GetBooleanParameter(i);
         SetBooleanParameter(i, bVal);
      }
   }
}


//------------------------------------------------------------------------------
// void CopyParameter(const GmatBase&fromObject, const Integer forParameter)
//------------------------------------------------------------------------------
/**
 * Sets an internal parameter value to match the input object's setting
 *
 * @param fromObject The input object
 * @param forParameter The ID of the parameter
 */
//------------------------------------------------------------------------------
void GmatBase::CopyParameter(const GmatBase& fromObject,
      const Integer forParameter)
{

   if (fromObject.GetType() != type)
      throw GmatBaseException("In GmatBase::CopyParameter, the input object "
            "type must exactly match the object receiving the call, but the "
            "call was sent to a " + typeName + "(" + instanceName +
            ") with an input object of type " +
            fromObject.GetTypeName() + " named " +
            fromObject.GetName());

   Gmat::ParameterType parmType = fromObject.GetParameterType(forParameter);

   if (parmType == Gmat::REAL_TYPE)
   {
      Real rVal = fromObject.GetRealParameter(forParameter);
      SetRealParameter(forParameter, rVal);
   }

   if (parmType == Gmat::INTEGER_TYPE)
   {
      Integer iVal = fromObject.GetIntegerParameter(forParameter);
      SetIntegerParameter(forParameter, iVal);
   }

   if ((parmType == Gmat::STRING_TYPE) || (parmType == Gmat::FILENAME_TYPE))
   {
      std::string sVal = fromObject.GetStringParameter(forParameter);
      SetStringParameter(forParameter, sVal);
   }

   if (parmType == Gmat::BOOLEAN_TYPE)
   {
      bool bVal = fromObject.GetBooleanParameter(forParameter);
      SetBooleanParameter(forParameter, bVal);
   }
}


//------------------------------------------------------------------------------
// void WriteParameters(Gmat::WriteMode mode, std::string &prefix,
//                      std::stringstream &stream)
//------------------------------------------------------------------------------
/**
 * Code that writes the parameter details for an object.
 *
 * @param mode   The writing mode.
 * @param prefix Starting portion of the script string used for the parameter.
 * @param stream The stringstream the object is written to.
 */
//------------------------------------------------------------------------------
void GmatBase::WriteParameters(Gmat::WriteMode mode, std::string &prefix,
                               std::stringstream &stream)
{
   #ifdef DEBUG_WRITE_PARAM
   MessageInterface::ShowMessage
      ("GmatBase::WriteParameters() <%p><%s>'%s' entered, mode=%d, prefix='%s'\n",
       this, GetTypeName().c_str(), GetName().c_str(), mode, prefix.c_str());
   #endif
   
   Integer i;
   Gmat::ParameterType parmType;
   std::stringstream value;
   value.precision(GetDataPrecision());
   
   // Allow objects to muck with parameter counts, and use the mucked up value
   Integer paramCount = GetParameterCount();
   
   #ifdef DEBUG_WRITE_PARAM
   MessageInterface::ShowMessage("   =====> There are %d parameters\n", paramCount);
   #endif
   
   // Create parameter write order if it is empty (LOJ: 2009.02.13)
   if (parameterWriteOrder.empty())
   {
      for (i = 0; i < paramCount; ++i)
         parameterWriteOrder.push_back(i);
   }
   else
   {
      if ((Integer)parameterWriteOrder.size() < paramCount)
      {
         // Add GmatBase parameters since its count is no longer 0 (LOJ: 2009.11.20)
         for (i = 0; i < GmatBaseParamCount; ++i)
            parameterWriteOrder.push_back(i);
         
         if ((Integer)parameterWriteOrder.size() != paramCount)
         {
            GmatBaseException gbe;
            gbe.SetDetails("GmatBase::WriteParameters(), there are more actual "
                           "parameters [%d] than ordered parameters [%d]\n",
                           parameterCount, parameterWriteOrder.size());
         }
      }
   }
   
   Integer id;
   for (i = 0; i < paramCount; ++i)
   {
      id = parameterWriteOrder[i];
      
      #ifdef DEBUG_WRITE_PARAM
      MessageInterface::ShowMessage
         ("   %2d, checking %-30s, type=%-12s\n", i, GetParameterText(id).c_str(),
          PARAM_TYPE_STRING[GetParameterType(id)].c_str());
      MessageInterface::ShowMessage
         ("   %-10s, %-10s\n",
          (IsParameterReadOnly(id) ? "ReadOnly" : "Writable"),
          (IsParameterCloaked(id) ? "Cloaked" : "Uncloaked"));
      #endif
      
      // in SHOW_SCRIPT, we write cloaked parameters; otherwise, we don't
      bool parameterIsToBeWritten = !IsParameterReadOnly(id);
      if (mode != Gmat::SHOW_SCRIPT) 
         parameterIsToBeWritten = parameterIsToBeWritten &&
                                  (!IsParameterCloaked(id));
      
      if (parameterIsToBeWritten)
      {
         parmType = GetParameterType(id);

         // Handle STRINGARRAY_TYPE or OBJECTARRAY_TYPE
         if (parmType == Gmat::STRINGARRAY_TYPE ||
             parmType == Gmat::OBJECTARRAY_TYPE)
         {
            #ifdef DEBUG_WRITE_PARAM
            MessageInterface::ShowMessage
               ("       writing string array for '%s'\n", GetParameterText(id).c_str());
            #endif
            
            bool writeQuotes = inMatlabMode || parmType == Gmat::STRINGARRAY_TYPE;
            WriteStringArrayValue(mode, prefix, id, writeQuotes, stream);
         }
         else
         {
            // Skip unhandled types
            if ( (parmType != Gmat::RMATRIX_TYPE) &&
                 (parmType != Gmat::UNKNOWN_PARAMETER_TYPE) )
            {
               // Fill in the l.h.s.
               value.str("");
               WriteParameterValue(id, value);
               if (value.str() != "")
               {
                  std::string attCmtLn = "";
                  if (mode != Gmat::OBJECT_EXPORT)
                     attCmtLn = GetAttributeCommentLine(id);

                  if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) ||
                     (mode == Gmat::OWNED_OBJECT) || (mode == Gmat::SHOW_SCRIPT)))
                     stream << attCmtLn;

                  // REAL_ELEMENT_TYPE is handled specially in WriteParameterValue()
                  if (parmType == Gmat::REAL_ELEMENT_TYPE)
                     stream << value.str();
                  else
                     stream << prefix << GetParameterText(id)
                            << " = " << value.str() << ";";

                  // overwrite tmp variable for attribute cmt line
                  if (mode != Gmat::OBJECT_EXPORT)
                     attCmtLn = GetInlineAttributeComment(id);
                  if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) ||
                      (mode == Gmat::OWNED_OBJECT) || (mode == Gmat::SHOW_SCRIPT)))
                     stream << attCmtLn << "\n";
                  else
                     stream << "\n";
               }
            }
         }
      }
   }
   
   // Write the object methods
   for (UnsignedInt i = 0; i < scriptedMethods.size(); ++i)
   {
      stream << objectMethodPrefaces[i] << prefix << scriptedMethods[i] << "\n";
   }

   GmatBase *ownedObject;
   std::string nomme, newprefix;
   
   // Get owned object count since subclasses may have GetOwnedObjectCount()
   Integer ownedObjCount = GetOwnedObjectCount();
   #ifdef DEBUG_OWNED_OBJECT_STRINGS
   MessageInterface::ShowMessage
      ("   <%p><%s>'%s' has %d owned objects\n", this, GetTypeName().c_str(),
       instanceName.c_str(), ownedObjCount);
   #endif
   
   // don't include the owned objects (celestial bodies) for the Solar System 
   if (IsOfType("SolarSystem"))  return;
   
   for (i = 0; i < ownedObjCount; ++i)
   {
      newprefix = prefix;
      ownedObject = GetOwnedObject(i);
      nomme = ownedObject->GetName();
      
      #ifdef DEBUG_OWNED_OBJECT_STRINGS
      MessageInterface::ShowMessage
         ("   id %d has owned object of type name <%s> and name \"%s\"\n", i,
          ownedObject->GetTypeName().c_str(), ownedObject->GetName().c_str());
      #endif

      // Skip writing out owned Amosphere models.  A better solution for the
      // special case bits here should be implemented.
      if (ownedObject->IsOfType(Gmat::ATMOSPHERE)) continue;

      // if owned object is a propagator, don't append the propagator name
      if (ownedObject->GetType() != Gmat::PROPAGATOR)
      {
         // Call new method BuildPropertyName() first to handle additional property
         // name for owned object in general way. For example, additional "Earth" in
         // "FM.GravityField.Earth.Degree", (loj: 2008.01.25)
         std::string ownedPropName = BuildPropertyName(ownedObject);
         
         #ifdef DEBUG_OWNED_OBJECT_STRINGS
         MessageInterface::ShowMessage
            ("   ownedPropName=<%s>, name=<%s>\n", ownedPropName.c_str(), nomme.c_str());
         #endif
         
         if (ownedPropName != "")
            newprefix += ownedPropName + ".";
         else if (nomme != "")
         {
            ///@note Scritpting for drag model does not require atmosphere model name
            // so check it first (For bug 2555 fix) (LOJ: 2012.02.03)
            if (!(ownedObject->IsOfType(Gmat::ATMOSPHERE)))
               newprefix += nomme + ".";
         }
      }
      
      #ifdef DEBUG_OWNED_OBJECT_STRINGS
      MessageInterface::ShowMessage
         ("   Calling ownedObject->GetGeneratingString() with newprefix='%s'\n\n",
          newprefix.c_str());
      #endif
      
      stream << ownedObject->GetGeneratingString(Gmat::OWNED_OBJECT, newprefix);
   }
}


//------------------------------------------------------------------------------
// void WriteStringArrayValue(Gmat::WriteMode mode, Integer id, ...)
//------------------------------------------------------------------------------
/**
 * Writes out parameters of StringArrayType or ObjectArrayType in the GMAT script syntax.
 *
 * @param obj Pointer to the object containing the parameter.
 * @param id  ID for the parameter that gets written.
 */
//------------------------------------------------------------------------------
void GmatBase::WriteStringArrayValue(Gmat::WriteMode mode, std::string &prefix,
                                     Integer id, bool writeQuotes,
                                     std::stringstream &stream)
{   
   StringArray sar = GetStringArrayParameter(id);
   if ((sar.size() > 0) || WriteEmptyStringArray(id))
   {
      std::string attCmtLn = "";
      if (mode != Gmat::OBJECT_EXPORT)
         attCmtLn = GetAttributeCommentLine(id);
      
      if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) ||
                               (mode == Gmat::OWNED_OBJECT) ||
                               (mode == Gmat::SHOW_SCRIPT)))
      {
         stream << attCmtLn.c_str();
      }
      
      stream << prefix << GetParameterText(id);
      std::stringstream arrayStream;
      arrayStream << "{";
      
      for (StringArray::iterator n = sar.begin(); n != sar.end(); ++n)
      {
         if (n != sar.begin())
            arrayStream << ", ";
         if (writeQuotes) //(inMatlabMode)
            arrayStream << "'";
         arrayStream << (*n);
         if (writeQuotes)
            arrayStream << "'";
      }
      
      if (mode != Gmat::OBJECT_EXPORT)
         attCmtLn  = GetInlineAttributeComment(id);
      
      if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) ||
                               (mode == Gmat::OWNED_OBJECT) ||
                               (mode == Gmat::SHOW_SCRIPT)))
      {
         arrayStream << "};" << attCmtLn;
      }
      else
         arrayStream << "};";
      
      #ifdef DEBUG_WRITE_PARAM
      MessageInterface::ShowMessage
         ("       value = '%s'\n\n", arrayStream.str().c_str());
      #endif
      
      stream << " = " << arrayStream.str() << "\n";
   }
}


//------------------------------------------------------------------------------
// void WriteParameterValue(Integer id, std::stringstream stream)
//------------------------------------------------------------------------------
/**
 * Writes out parameters in the GMAT script syntax.
 *
 * @param obj Pointer to the object containing the parameter.
 * @param id  ID for the parameter that gets written.
 */
//------------------------------------------------------------------------------
void GmatBase::WriteParameterValue(Integer id, std::stringstream &stream)
{
   Gmat::ParameterType tid = GetParameterType(id);
   Integer precision = GmatGlobal::Instance()->GetDataPrecision();
   bool writeGmatKeyword = GmatGlobal::Instance()->IsWritingGmatKeyword();
   
   #ifdef DEBUG_WRITE_PARAM
   MessageInterface::ShowMessage
      ("   ==> GmatBase::WriteParameterValue() entered, id=%d\n", id);
   MessageInterface::ShowMessage
      ("   %2d, writing  '%s', type=%s\n", id, GetParameterText(id).c_str(),
       PARAM_TYPE_STRING[tid].c_str());
   #endif
   
   // Special case for ForceModel (For bug 2555 fix - LOJ: 2012.02.03)
   // If Drag is turned off we still want to write FM.Drag = None.
   // If Drag has AtmosphereModel name, then skip writing FM.Drag.
   // AtmosphereModel will write FM.Drag.AtmosphereModel = SomeModel.
   if (IsOfType(Gmat::ODE_MODEL) && GetParameterText(id) == "Drag")
   {
      if (GetStringParameter(id) != "None")
      {
         #ifdef DEBUG_WRITE_PARAM
         MessageInterface::ShowMessage
            ("   ==> GmatBase::WriteParameterValue() leaving, it is OdeModel "
             "and Drag is not None\n\n");
         #endif
         return;
      }
   }
   
   switch (tid)
   {
   // Objects and colors write out a string without quotes
   case Gmat::OBJECT_TYPE:
   case Gmat::COLOR_TYPE:
      if (inMatlabMode)
         stream << "'";
      stream << GetStringParameter(id);
      if (inMatlabMode)
         stream << "'";
      break;

   // Symbolic constants write out a string without quotes
   case Gmat::ENUMERATION_TYPE:
      if (inMatlabMode)
         stream << "'";
      stream << GetStringParameter(id);
      if (inMatlabMode)
         stream << "'";
      break;

   // Strings write out a string with quotes (loj: 2008.03.26)
   // Do not write blank string (loj: 2009.09.22)
   case Gmat::FILENAME_TYPE:
   case Gmat::STRING_TYPE:
   case Gmat::EQUATION_TYPE:
      {
         // Check if empty string parameter can be written (LOJ: 2015.01.30)
         bool writeString = false;
         std::string strVal = GetStringParameter(id);
         if (inMatlabMode)
            writeString = true;
         else if (strVal != "")
            writeString = true;
         else if (WriteEmptyStringParameter(id))
            writeString = true;
         
         #ifdef DEBUG_WRITE_PARAM
         MessageInterface::ShowMessage("   writeString = %d\n", writeString);
         #endif
         //if (inMatlabMode || (!inMatlabMode && strVal != ""))
         if (writeString)
            stream << "'" << strVal << "'";
         
         break;
      }
   // On/Off constants write out a string without quotes
   case Gmat::ON_OFF_TYPE:
      if (inMatlabMode)
         stream << "'";
      stream << GetOnOffParameter(id);
      if (inMatlabMode)
         stream << "'";
      break;

   case Gmat::INTEGER_TYPE:
      stream << GetIntegerParameter(id);
      break;

   case Gmat::UNSIGNED_INT_TYPE:
      stream << GetUnsignedIntParameter(id);
      break;

   case Gmat::UNSIGNED_INTARRAY_TYPE:
      {
         UnsignedIntArray arr = GetUnsignedIntArrayParameter(id);
         stream << "[ ";
         for (UnsignedInt i=0; i<arr.size(); i++)
         stream << arr[i] << " ";
         stream << "]";
      }
      break;
         
   case Gmat::INTARRAY_TYPE:
      {
         IntegerArray arr = GetIntegerArrayParameter(id);
         stream << "[ ";
         for (UnsignedInt i=0; i<arr.size(); i++)
         stream << arr[i] << " ";
         stream << "]";
      }
      break;
         
   case Gmat::REAL_TYPE:
   case Gmat::TIME_TYPE: // Treat TIME_TYPE as Real
      stream << GetRealParameter(id);
      break;

   case Gmat::REAL_ELEMENT_TYPE:
      {
         Integer row = GetIntegerParameter("NumRows");
         Integer col = GetIntegerParameter("NumCols");
         for (Integer i = 0; i < row; ++i)
         {
            // Do not write if value is zero since default is zero
            for (Integer j = 0; j < col; ++j)
            {
               if (GetRealParameter(id, i, j) != 0.0)
               {
                  // We now write out GMAT prefix on option from the startup file (see GMT-3233)
                  if (writeGmatKeyword)
                     stream << "GMAT " << instanceName << "(" << i+1 << ", " << j+1 <<
                        ") = " << GetRealParameter(id, i, j) << ";\n";
                  else
                     stream << instanceName << "(" << i+1 << ", " << j+1 <<
                        ") = " << GetRealParameter(id, i, j) << ";\n";
               }
            }
         }
      }
      break;

   case Gmat::RVECTOR_TYPE:
      {
         Rvector rv = GetRvectorParameter(id);
         stream << "[ " << rv.ToString(precision) << " ]";
      }
      break;

   case Gmat::RMATRIX_TYPE:
      {
         Rmatrix ra = GetRmatrixParameter(id);
         Integer r, c;
         ra.GetSize(r, c);

         stream << "[ ";
         for (Integer i = 0; i < r; ++i)
         {
            for (Integer j = 0; j < c; ++j)
               stream << ra.GetElement(i,j) << " ";
            if (i < r-1)
               stream << "; ";
         }
         stream << "]";
      }
      break;

   case Gmat::BOOLEAN_TYPE:
      stream << ((GetBooleanParameter(id)) ? "true" : "false");
      break;

   case Gmat::BOOLEANARRAY_TYPE:
      {
         BooleanArray arr = GetBooleanArrayParameter(id);
         stream << "[ ";
         for (UnsignedInt i=0; i<arr.size(); i++)
            stream << (arr[i] ? "true" : "false") << " ";
         stream << "]";
      }
      break;
      
   // Handle variants (GENERIC_TYPE)
   case (Gmat::GENERIC_TYPE):
      {
         Generic val = GetGenericParameter(id);

         switch (val.VarIndex())
         {
         case 0:  // Real
            {
               Real value = VarGet<Real>(val);
               stream << value;
            }
            break;
         case 1:  // Integer
            {
               Integer value = VarGet<Integer>(val);
               stream << value;
            }
            break;

         case 2:  // std::string
            {
               std::string value = VarGet<std::string>(val);
               stream << "'" << value << "'";
            }
            break;

         case 3:  // RealArray
            {
               RealArray value = VarGet<RealArray>(val);
               stream << "[" ;
               for (UnsignedInt i = 0; i < value.size(); ++i)
               {
                  if (i > 0)
                     stream << ", ";
                  stream << value[i];
               }
               stream << "]";
            }
            break;

         case 4:  // IntegerArray
            {
               IntegerArray value = VarGet<IntegerArray>(val);
               stream << "[" ;
               for (UnsignedInt i = 0; i < value.size(); ++i)
               {
                  if (i > 0)
                     stream << ", ";
                  stream << value[i];
               }
               stream << "]";
            }
            break;

         case 5:  // StringArray
            {
               StringArray value = VarGet<StringArray>(val);
               stream << "{" ;
               for (UnsignedInt i = 0; i < value.size(); ++i)
               {
                  if (i > 0)
                     stream << ", ";
                  stream << "'" << value[i] << "'";
               }
               stream << "}";
            }
            break;

         default:
            MessageInterface::ShowMessage
               ("*** GmatBase::WriteParameterValue() Writing of the selected "
                     "subtype for the \"%s\" type is not handled yet for "
                     "object \"%s\" of type \"%s\"\n",
                     PARAM_TYPE_STRING[tid].c_str(), GetName().c_str(),
                     GetTypeName().c_str());
            break;
         }
      }
      break;


   default:
      MessageInterface::ShowMessage
         ("*** GmatBase::WriteParameterValue() Writing of \"%s\" type is not handled "
			 "yet for object \"%s\" of type \"%s\"\n", PARAM_TYPE_STRING[tid].c_str(),
			 GetName().c_str(), GetTypeName().c_str());
      break;
   }
   
   #ifdef DEBUG_WRITE_PARAM
   MessageInterface::ShowMessage
      ("   ==> GmatBase::WriteParameterValue() leaving, value = '%s'\n\n",
       stream.str().c_str());
   #endif
}

//-------------------------------------
// private methods
//-------------------------------------

//------------------------------------------------------------------------------
//  void PrepCommentTables()
//------------------------------------------------------------------------------
/**
 * Sets the size and initializes the parameter-by-parameter comment tables.
 */
//------------------------------------------------------------------------------
void GmatBase::PrepCommentTables()
{
   #ifdef DEBUG_FINALIZE_CREATION
   MessageInterface::ShowMessage
      ("GmatBase::PrepCommentTables() entered, parameterCount=%d\n", parameterCount);
   #endif
   attributeCommentLines.resize(parameterCount);
   attributeInlineComments.resize(parameterCount);
   for (Integer i = 0; i < parameterCount; ++i)
   {
      attributeCommentLines[i] = "";
      attributeInlineComments[i] = "";
   }
}

//------------------------------------------------------------------------------
// bool HasDynamicParameterSTM(Integer parameterId)
//------------------------------------------------------------------------------
bool GmatBase::HasDynamicParameterSTM(Integer parameterId)
{
   return false;
}

//------------------------------------------------------------------------------
// Rmatrix* GetParameterSTM(Integer parameterId)
//------------------------------------------------------------------------------
Rmatrix* GmatBase::GetParameterSTM(Integer parameterId)
{
   return NULL;
}


//------------------------------------------------------------------------------
// Integer GmatBase::GetStmRowId(const Integer forRow)
//------------------------------------------------------------------------------
/**
 * Retrieves the ID associated with a given row/column of the STM
 *
 * Note that since the STM is of the form d(r(t)) / d(r(t_o)), the numerator
 * setting for each row matches the denominator setting for each column.
 *
 * @param forRow The associated row (for the numerator) or column (denominator)
 *
 * @return The ID
 */
//------------------------------------------------------------------------------
Integer GmatBase::GetStmRowId(const Integer forRow)
{
   return -1;
}

//------------------------------------------------------------------------------
// Integer HasParameterCovariances(Integer parameterId)
//------------------------------------------------------------------------------
Integer GmatBase::HasParameterCovariances(Integer parameterId)
{
   return -1;
}

//------------------------------------------------------------------------------
// Rmatrix* GetParameterCovariances(Integer parameterId)
//------------------------------------------------------------------------------
Rmatrix* GmatBase::GetParameterCovariances(Integer parameterId)
{
   return NULL; //&covariance;
}

//------------------------------------------------------------------------------
// Covariance* GetCovariance()
//------------------------------------------------------------------------------
Covariance* GmatBase::GetCovariance()
{
   return &covariance;
}

//------------------------------------------------------------------------------
// bool MapCovarianceToParameters()
//------------------------------------------------------------------------------
bool GmatBase::MapCovarianceToParameters()
{
   return false;
}

//------------------------------------------------------------------------------
// void UpdateClonedObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Entry point for updating locally owned clones
 *
 * @param obj The object that may have been cloned
 */
//------------------------------------------------------------------------------
void GmatBase::UpdateClonedObject(GmatBase *obj)
{
   MessageInterface::ShowMessage("UpdateClonedObject called in GmatBase; does "
         "the override exist in the %s class?\n", typeName.c_str());
}

//------------------------------------------------------------------------------
// void UpdateClonedObjectParameter(GmatBase *obj, Integer updatedParameterId)
//------------------------------------------------------------------------------
/**
 * This method changes a single parameter on an owned clone
 *
 * @param obj The master object holding the new parameter value
 * @param updatedParameterId The ID of the updated parameter
 */
//------------------------------------------------------------------------------
void GmatBase::UpdateClonedObjectParameter(GmatBase *obj,
      Integer updatedParameterId)
{
   MessageInterface::ShowMessage("UpdateClonedObjectParameter called in "
         "GmatBase; does the override exist in the %s class?\n",
         typeName.c_str());
}


//------------------------------------------------------------------------------
// bool IsCommandModeAssignable() const
//------------------------------------------------------------------------------
/**
 * Tests to see if the object accepts command mode assignment
 *
 * @retval true if command mode assignment is allowed, false if not
 */
//------------------------------------------------------------------------------
bool GmatBase::IsCommandModeAssignable() const
{
   return !blockCommandModeAssignment;
}


//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool GmatBase::IsParameterCommandModeSettable(const Integer id) const
{
   return false;
}


const std::string GmatBase::GetFullName()
{
   return instanceFullName;
}


bool GmatBase::SetFullName(const std::string name)
{
   instanceFullName = name;
   return true;
}


ObjectMap GmatBase::GetConfiguredObjectMap()
{
   // Get local object map
   ObjectMap om = Moderator::Instance()->GetSandbox()->GetObjectMap();

   // Add global object map
   ObjectMap gom = Moderator::Instance()->GetSandbox()->GetGlobalObjectMap();
   for (ObjectMap::iterator i = gom.begin(); i != gom.end(); ++i)
      om[(*i).first] = (*i).second;

   //return Moderator::Instance()->GetSandbox()->GetObjectMap();
   return om;
}


GmatBase* GmatBase::GetConfiguredObject(const std::string &name)
{
   return Moderator::Instance()->GetInternalObject(name);
}


const StringArray GmatBase::GetListOfObjects(UnsignedInt type)
{
   ObjectMap objMap = GetConfiguredObjectMap();
   StringArray nameList;
   for (ObjectMap::iterator i = objMap.begin(); i != objMap.end(); ++i)
   {
      if ((*i).second->IsOfType(type))
         nameList.push_back((*i).first);
   }

   return nameList;
}


const StringArray GmatBase::GetListOfObjects(const std::string &typeName)
{
   ObjectMap objMap = GetConfiguredObjectMap();
   StringArray nameList;
   for (ObjectMap::iterator i = objMap.begin(); i != objMap.end(); ++i)
   {
      if ((*i).second->IsOfType(typeName))
         nameList.push_back((*i).first);
   }

   return nameList;
}


//------------------------------------------------------------------------------
// bool HasEquation()
//------------------------------------------------------------------------------
/**
 * Method to check for equation fields
 *
 * @return true if there is at least one equation field
 */
//------------------------------------------------------------------------------
bool GmatBase::HasEquation()
{
   bool retval = false;
   for (Integer i = 0; i < parameterCount; ++i)
      if (GetParameterType(i) == Gmat::EQUATION_TYPE)
         retval = true;
   return retval || hasEquation;
}


//------------------------------------------------------------------------------
// bool SetupEquation(ObjectMap *configObjectMap)
//------------------------------------------------------------------------------
/**
 * Called to setup math tree structures for the equation field.
 *
 * Derived classes that have equation fields override this method to build their
 * MathTree structures.
 *
 * This method is called in FinalPass as a script in read in, and during cloning
 * in the Sandbox prior to running a script.  On return, the RHSEquation object
 * has been built and a MathTree has been constructed that matches the equation
 * contained in the expression for the field.
 *
 * This method marches through each equation field on the object, and reports
 * success if at least one MathTree was built.  Exceptions should be thrown if
 * finer error reporting is needed.
 *
 * @param configObjectMap The configured object map so that the tree can build
 *
 * @return true if at least one equation was converted to a MathTree
 */
//------------------------------------------------------------------------------
bool GmatBase::SetupEquation(ObjectMap *configObjectMap)
{
   return false;
}

//------------------------------------------------------------------------------
// void GetEquations(std::vector<RHSEquation*> &equations)
//------------------------------------------------------------------------------
/**
 * Retrieves RHSEquation pointers for validation and wrapper setup
 *
 * Derived classes need to fill in the RHSEquation pointers
 *
 * @param equations THe array of equation pointers that needs to be validated
 */
//------------------------------------------------------------------------------
void GmatBase::GetEquations(std::vector<RHSEquation*> &equations)
{
   equations.clear();
}

//------------------------------------------------------------------------------
// bool InitializeEquations(ObjectMap *lom, ObjectMap *gom)
//------------------------------------------------------------------------------
/**
 * Called to initialize equations on an object
 *
 * Derived classes that have equation fields override this method to initialize
 * their MathTree structures.
 *
 * @param lom The local object map
 * @param gom The global object map
 *
 * @return true if all equations initialized.
 */
//------------------------------------------------------------------------------
bool GmatBase::InitializeEquations(ObjectMap *lom, ObjectMap *gom)
{
   return true;
}

//------------------------------------------------------------------------------
// RHSEquation* ParseEquation(const std::string &equationString,
//       ObjectMap *configObjectMap)
//------------------------------------------------------------------------------
/**
 * Build the MathTree.  Called from overridden SetupEquation() methods.
 *
 * @param equationString The scripted equation
 * @param configObjectMap The configured object map so that the tree can build
 *
 * @return The RHSEquation containing the constructed MathTree
 */
//------------------------------------------------------------------------------
RHSEquation* GmatBase::ParseEquation(const std::string &equationString,
      ObjectMap *configObjectMap)
{
   RHSEquation *rhsParser = new RHSEquation;
   if (rhsParser->BuildExpression(equationString, configObjectMap, true) == false)
   {
      // Delete old MathTree and clear mathWrapperMap
      delete rhsParser;
      rhsParser = NULL;

      // Check if all brackets are balanced if not enclosed with single quotes
      if (!GmatStringUtil::IsEnclosedWith(equationString, "'"))
      {
         if (!GmatStringUtil::AreAllBracketsBalanced(equationString, "[({])}"))
            throw CommandException
               ("Parentheses or braces are unbalanced on the right-hand-side of "
                "the equation field " + equationString);
      }
   }

   return rhsParser;
}


//------------------------------------------------------------------------------
// bool HasGuiPlugin()
//------------------------------------------------------------------------------
/**
 * Checks an object to see if there is a matching GUI element in plugin code
 *
 * @return true if there is a GUI factory and plugin, false (the default) if not
 */
//------------------------------------------------------------------------------
bool GmatBase::HasGuiPlugin()
{
   return false;
}


//------------------------------------------------------------------------------
// StringArray GetGuiPanelNames(const std::string &type)
//------------------------------------------------------------------------------
/**
 * Returns the name(s) of GUI plugin objects this object uses
 *
 * @param type The type of panel/element requested.  An empty string returns all
 *             of the panels needed.  "ResourceTree" returns the resource tree
 *             entries.  "MissionTree" returns command references, and is not
 *             used because commands are picked up automatically.
 *
 * @return The names of the GUI elements, used in calls to the factories and in
 *         populating GUI trees
 */
//------------------------------------------------------------------------------
StringArray GmatBase::GetGuiPanelNames(const std::string &type)
{
   StringArray retval;
   return retval;
}

//------------------------------------------------------------------------------
// void SetWidget(void *widget)
//------------------------------------------------------------------------------
/**
 * Call used to set the widget used by a plugin component with a plugin GUI
 *
 * @param widget The widget that connects the GUI to the object
 */
//------------------------------------------------------------------------------
void GmatBase::SetWidget(GmatWidget *widget)
{
   // Default behavior: do nothing
}

//------------------------------------------------------------------------------
// const char** GetIcon()
//------------------------------------------------------------------------------
/**
 * Retrieves the icon associated with the object (if any)
 *
 * @return The xpm based icon strign
 */
//------------------------------------------------------------------------------
const char** GmatBase::GetIcon()
{
   return NULL;
}

//------------------------------------------------------------------------------
// void SetIconIndex(Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the index for icons used in GUI components
 *
 * The default method here does nothing but define the interface
 *
 * @param index The index value
 */
//------------------------------------------------------------------------------
void GmatBase::SetIconIndex(Integer index)
{
}


//------------------------------------------------------------------------------
// Integer GetIconIndex()
//------------------------------------------------------------------------------
/**
 * Returns index of an icon, used in tree structures that handle icons by index
 *
 * Tree widgets can handle icons using an indexed list.  This method provides
 * a mechanism to access such icons rather than adding a new copy to the tree
 * each time the icon is needed.
 *
 * Classes that use this mechism should set the index as a class (static) member
 * so that all objects created from the class access the same icon instance.
 *
 * @return The icon index, or -1 if the icon does not exist or has not set an
 * index.
 */
//------------------------------------------------------------------------------
Integer GmatBase::GetIconIndex()
{
   return -1;
}
