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
//
// Modification History:
//
// 11/9/2003 D. Conway
//   Made GetParameterCount virtual so PropSetup can override it, returning the
//   count for the member ForceModel, Forces, and Propagator.
/**
 * Definition for the base class for all GMAT extensible objects
 */
//------------------------------------------------------------------------------


#ifndef GmatBase_hpp
#define GmatBase_hpp

#include "gmatdefs.hpp"
#include "GmatBaseException.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "Covariance.hpp"
#include "GmatWidget.hpp"

// Make the typename singleton available everywhere
#include "GmatType.hpp"

#include <algorithm>                    // Required by GCC 4.3 for find


// Forward reference
class SolarSystem;
class CoordinateSystem;
class GmatWidget;
class RHSEquation;


// The allocation size used to construct estimation object parameter IDs
#define ESTIMATION_TYPE_ALLOCATION  250


//------------------------------------------------------------------------------
/**
 * Definition for the base class for all GMAT extensible objects
 *
 * The following class hierarchy trees use this class as their basis:
 *
 *     SpacePoint (hence Spacecraft and Formation, and all CelestialBody's)
 *     Propagator
 *     PhysicalModel (hence Force and ForceModel)
 *     PropConfig
 *     Parameter
 *     GmatCommand
 *
 * Every class that users can use to derive new classes, or that need to be
 * accessed through generic pointers, should be derived from this class to
 * ensure consistent interfaces accessed by the GMAT control systems (i.e. the
 * Moderator, FactoryManager, Configuration, Interpreter, and Sandbox, along
 * with the GUIInterpreter).
 */
//------------------------------------------------------------------------------
class GMAT_API GmatBase
{
// Class methods and data for the GMAT API
public:
   virtual std::string  Help(const std::string &forItem = "");
   virtual void         SetField(const Integer id, const Integer value);
   virtual void         SetField(const Integer id, const Real value);
   virtual void         SetField(const Integer id, const std::string &value);
   virtual void         SetField(const Integer id, const bool value);
   virtual void         SetField(const Integer id, const RealArray &value);
   virtual void         SetField(const Integer id, const StringArray &value);
   virtual void         SetField(const std::string &label, const Integer value);
   virtual void         SetField(const std::string &label, const Real value);
   virtual void         SetField(const std::string &label, const std::string &value);
   virtual void         SetField(const std::string &label, const bool value);
   virtual void         SetField(const std::string &label, const RealArray &value);
   virtual void         SetField(const std::string &label, const StringArray &value);
   virtual bool         SetReference(GmatBase* obj, Integer index = -1);

   virtual std::string  GetField(const Integer id);
   virtual std::string  GetField(const std::string &label);

   virtual void         SetNumber(const Integer id, const Real value);
   virtual void         SetNumber(const std::string &label, const Real value);
   virtual Real         GetNumber(const Integer id);
   virtual Real         GetNumber(const std::string &label);
   //RVector
   virtual void         SetNumber(const Integer id, const Real value, const Integer index);
   virtual void         SetNumber(const std::string &label, const Real value, const Integer index);
   virtual Real         GetNumber(const Integer id, const Integer index);
   virtual Real         GetNumber(const std::string &label, const Integer index);
   virtual void         SetVector(const Integer id, const Rvector &value);
   virtual void         SetVector(const std::string &label, const Rvector &value);
   virtual Rvector      GetVector(const Integer id);
   virtual Rvector      GetVector(const std::string &label);
   //RMatrix
   virtual void         SetNumber(const Integer id, const Real value, const Integer row,
                        const Integer col);
   virtual void         SetNumber(const std::string &label, const Real value, const Integer row,
                        const Integer col);
   virtual Real         GetNumber(const Integer id, const Integer row,
                        const Integer col);
   virtual Real         GetNumber(const std::string &label, const Integer row,
                        const Integer col);
   virtual void         SetMatrix(const Integer id, const Rmatrix &value);
   virtual void         SetMatrix(const std::string &label, const Rmatrix &value);
   virtual Rmatrix      GetMatrix(const Integer id);
   virtual Rmatrix      GetMatrix(const std::string &label);

   std::string          GetTypeAndValue(Integer i);

protected:
   virtual std::string  GetHelpString(const std::string &forItem = "");

public:


   // The usual suspects
   GmatBase(UnsignedInt typeId, const std::string &typeStr,
            const std::string &nomme = "");
   virtual ~GmatBase() = 0;
   GmatBase(const GmatBase &a);
   GmatBase&            operator=(const GmatBase &a);

   // Access methods called on the base class
   virtual UnsignedInt  GetType() const;
   inline std::string   GetTypeName() const;
   inline std::string   GetName() const;
   virtual bool         SetName(const std::string &who,
                                const std::string &oldName = "");
   virtual bool         SetName(const char *who,
                                const std::string &oldName = "");

   virtual const std::string  GetFullName();
   virtual bool         SetFullName(const std::string name);

   virtual std::string  ToString() const;

   virtual Integer      GetParameterCount() const;

   bool                 IsOfType(UnsignedInt ofType) const;
   bool                 IsOfType(const std::string &typeDescription) const;
   StringArray          GetTypeNames() const;
   
   virtual bool         IsInitialized();

   void                 SetShowPrefaceComment(bool show = true);
   void                 SetShowInlineComment(bool show = true);
   bool                 GetShowPrefaceComment();
   bool                 GetShowInlineComment();

   // Access methods derived classes can override on comments
   virtual const std::string
                        GetCommentLine() const;
   virtual void         SetCommentLine(const std::string &comment);

   virtual const std::string
                        GetInlineComment() const;
   virtual void         SetInlineComment(const std::string &comment);

   virtual const std::string
                        GetAttributeCommentLine(Integer index);
   virtual void         SetAttributeCommentLine(Integer index,
                                                const std::string &comment);

   virtual const std::string
                        GetInlineAttributeComment(Integer index);
   virtual void         SetInlineAttributeComment(Integer index,
                                                  const std::string &comment);

   // Access methods derived classes can override on reference objects
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObjectName(const UnsignedInt type,
                                         const char *name);
   virtual bool         SetRefObjectName(const UnsignedInt type,
                                         const std::string &name);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName) = 0;
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual ObjectArray& GetRefObjectArray(const UnsignedInt type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);

   bool                 HasEquation();
   virtual bool         SetupEquation(ObjectMap *configObjectMap);
   virtual void         GetEquations(std::vector<RHSEquation*> &equations);
   virtual bool         InitializeEquations(ObjectMap *lom, ObjectMap *gom);

   /// Returns the list of names used in the wrappers
   virtual const StringArray&
                        GetWrapperObjectNameArray(bool completeSet = false);

   virtual bool         HasOtherReferenceToObject(const std::string &withName);

   virtual bool         IsOwnedObject(Integer id) const;
   virtual Integer      GetOwnedObjectCount();
   virtual GmatBase*    GetOwnedObject(Integer whichOne);
   virtual bool         IncludeOwnedObjectsInValidation();
   virtual bool         SetIsGlobal(bool globalFlag);
   virtual bool         IsGlobal() const;
   virtual bool         IsAutomaticGlobal() const;
   virtual bool         SetIsLocal(bool localFlag);
   virtual bool         IsLocal() const;
   virtual bool         IsObjectCloaked() const;
   virtual bool         SaveAllAsDefault();
   virtual bool         SaveParameterAsDefault(const Integer id);
   virtual bool         SaveParameterAsDefault(const std::string &label);
   /// method to determine if a parameter value has been changed from the default - 
   // should be implemented in leaf classes that need to monitor changes to
   // parameter values (currently, SolarSystem and celestial bodies)
   
   virtual bool         ExecuteCallback();
   virtual bool         IsCallbackExecuting();
   virtual bool         PutCallbackData(std::string &data);
   virtual bool         PutCallbackRealData(RealArray &data);
   virtual std::string  GetCallbackResults();

   // required method for all subclasses
   virtual GmatBase*    Clone() const = 0;

   // required method for all subclasses that can be copied in a script
   virtual void         Copy(const GmatBase*);

   virtual bool         Validate();
   virtual bool         Initialize();
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);

   virtual bool         RequiresJ2000Body();

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterText(const Integer id, Integer width);
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual bool         HasDecoratedFields();
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   virtual bool         IsParameterEnabled(const Integer id) const;
   virtual bool         IsParameterEnabled(const std::string &label) const;
   virtual bool         IsParameterCloaked(const Integer id) const;
   virtual bool         IsParameterCloaked(const std::string &label) const;
   virtual bool         IsParameterEqualToDefault(const Integer id) const;
   virtual bool         IsParameterEqualToDefault(const std::string &label) const;
   virtual bool         IsParameterValid(const Integer id,
                                         const std::string &value);
   virtual bool         IsParameterValid(const std::string &label,
                                         const std::string &value);
   virtual bool         IsParameterVisible(const Integer id) const;
   virtual bool         IsParameterVisible(const std::string &label) const;
   
   virtual bool         ParameterAffectsDynamics(const Integer id) const;
   virtual bool         ParameterDvInitializesNonzero(const Integer id,
                              const Integer r = 0, const Integer c = 0) const;
   virtual Real         ParameterDvInitialValue(const Integer id,
                              const Integer r = 0, const Integer c = 0) const;
   virtual bool         ParameterUpdatesAfterSuperposition(const Integer id) const;

   virtual UnsignedInt
                        GetPropertyObjectType(const Integer id) const;
   virtual UnsignedInt
                        GetPropertyObjectType(const Integer id,
                              const Integer index) const;
   virtual const StringArray&
                        GetPropertyEnumStrings(const Integer id) const;
   virtual const StringArray&
                        GetPropertyEnumStrings(const std::string &label) const;
   virtual bool         CanAssignStringToObjectProperty(const Integer id) const;
   virtual bool         IsSquareBracketAllowedInSetting(const Integer id) const;
   
   virtual GmatTime     GetGmatTimeParameter(const Integer id) const;
   virtual GmatTime     SetGmatTimeParameter(const Integer id, 
                                             const GmatTime value);

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         GetRealParameter(const Integer id, const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real         SetRealParameter(const Integer id, const Real value,
                                         const Integer row, const Integer col);

   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const Integer id,
                                            const Integer index) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value,
                                            const Integer index);

   virtual UnsignedInt  GetUnsignedIntParameter(const Integer id) const;
   virtual UnsignedInt  SetUnsignedIntParameter(const Integer id,
                                                const UnsignedInt value);
   virtual UnsignedInt  GetUnsignedIntParameter(const Integer id,
                                                const Integer index) const;
   virtual UnsignedInt  SetUnsignedIntParameter(const Integer id,
                                                const UnsignedInt value,
                                                const Integer index);
   virtual const UnsignedIntArray&
                        GetUnsignedIntArrayParameter(const Integer id) const;

   virtual const IntegerArray&
                        GetIntegerArrayParameter(const Integer id) const;
   virtual const IntegerArray&
                        GetIntegerArrayParameter(const Integer id,
                                                 const Integer index) const;

   virtual const Rvector&
                        GetRvectorParameter(const Integer id) const;
   virtual const Rvector&
                        SetRvectorParameter(const Integer id,
                                            const Rvector &value);

   virtual const Rmatrix&
                        GetRmatrixParameter(const Integer id) const;
   virtual const Rmatrix&
                        SetRmatrixParameter(const Integer id,
                                            const Rmatrix &value);

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const char *value);
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const char *value,
                                           const Integer index);
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id,
                                                const Integer index) const;

   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id,
                                         const std::string &value);

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const Integer id,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value,
                                            const Integer index);

   virtual Integer      CheckGenericTypeSupport(const Integer id,
                                                const Integer genType,
                                                const std::string &setting = "");
   virtual Generic      GetGenericParameter(const Integer id) const;
   virtual Generic      SetGenericParameter(const Integer id,
                                            const Generic value);

   virtual Integer      CheckGenericTypeSupport(const std::string &label,
                                                const Integer genType,
                                                const std::string &setting = "");
   virtual Generic      GetGenericParameter(const std::string &label) const;
   virtual Generic      SetGenericParameter(const std::string &label,
                                            const Generic value);

   UnsignedInt          GetType(const Generic &forGeneric);

   virtual GmatTime     GetGmatTimeParameter(const std::string &label) const;
   virtual GmatTime     SetGmatTimeParameter(const std::string &label, 
                                             const GmatTime value);

   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value, const Integer row,
                                         const Integer col);

   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label,
                                            const Integer index) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value,
                                            const Integer index);

   virtual UnsignedInt  GetUnsignedIntParameter(const std::string &label) const;
   virtual UnsignedInt  SetUnsignedIntParameter(const std::string &label,
                                                const UnsignedInt value);
   virtual UnsignedInt  GetUnsignedIntParameter(const std::string &label,
                                                const Integer index) const;
   virtual UnsignedInt  SetUnsignedIntParameter(const std::string &label,
                                                const UnsignedInt value,
                                                const Integer index);
   virtual const UnsignedIntArray&
                        GetUnsignedIntArrayParameter(const std::string &label) const;

   virtual const Rvector&
                        GetRvectorParameter(const std::string &label) const;
   virtual const Rvector&
                        SetRvectorParameter(const std::string &label,
                                            const Rvector &value);

   virtual const Rmatrix&
                        GetRmatrixParameter(const std::string &label) const;
   virtual const Rmatrix&
                        SetRmatrixParameter(const std::string &label,
                                            const Rmatrix &value);

   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const char *value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label,
                                                const Integer index) const;

   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value,
                                            const Integer index);
   virtual const BooleanArray&
                        GetBooleanArrayParameter(const Integer id) const;
   virtual const BooleanArray&
                        GetBooleanArrayParameter(const std::string &label) const;
   virtual bool         SetBooleanArrayParameter(const Integer id,
                                                 const BooleanArray &valueArray);
   virtual bool         SetBooleanArrayParameter(const std::string &label,
                                                 const BooleanArray &valueArray);
   
   virtual std::string  GetOnOffParameter(const std::string &label) const;
   virtual bool         SetOnOffParameter(const std::string &label,
                                          const std::string &value);

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   virtual bool         TakeRequiredAction(const Integer id);
   virtual bool         TakeRequiredAction(const std::string &label);

   virtual const ObjectTypeArray& GetTypesForList(const Integer id);
   virtual const ObjectTypeArray& GetTypesForList(const std::string &label);

   virtual bool         WriteEmptyStringArray(Integer id);
   virtual bool         WriteEmptyStringParameter(const Integer id) const;
   
   virtual const std::string&
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");
   virtual StringArray  GetGeneratingStringArray(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");
   virtual std::string  BuildPropertyName(GmatBase *ownedObj);
   virtual void         FinalizeCreation();

   virtual std::string  GetLastErrorMessage();
   virtual void         SetLastErrorMessage(const std::string &msg);
   virtual std::string  GetErrorMessageFormat();
   virtual void         SetErrorMessageFormat(const std::string &fmt);

   /// Return value used if the parameter is not accessible as a Real
   static const Real         REAL_PARAMETER_UNDEFINED;
   /// Return value used if the parameter is not accessible as an Integer
   static const Integer      INTEGER_PARAMETER_UNDEFINED;
   /// Return value used if the parameter is not accessible as an UnsignedInt
   static const UnsignedInt  UNSIGNED_INT_PARAMETER_UNDEFINED;
   /// Return value used if the parameter is not accessible as a String
   static const std::string  STRING_PARAMETER_UNDEFINED;
   /// Return value used if the parameter is not accessible as a StringArray
   static const StringArray  STRINGARRAY_PARAMETER_UNDEFINED;
   /// Return value used if the parameter is not accessible as a IntegerArray
   static const IntegerArray INTEGERARRAY_PARAMETER_UNDEFINED;
   /// Return value used if the parameter is not accessible as a UnsignedIntArray
   static const UnsignedIntArray UNSIGNED_INTARRAY_PARAMETER_UNDEFINED;
   /// Return value used if the parameter is not accessible as a Rvector
   static const Rvector      RVECTOR_PARAMETER_UNDEFINED;
   /// Return value used if the parameter is not accessible as a Rmatrix
   static const Rmatrix      RMATRIX_PARAMETER_UNDEFINED;
   /// String mappings for the GMAT data types
   static const std::string  PARAM_TYPE_STRING[Gmat::TypeCount];
   /// String mappings for the GMAT object types
   static const std::string  OBJECT_TYPE_STRING[Gmat::UNKNOWN_OBJECT - Gmat::SPACECRAFT+1];
   static const bool         AUTOMATIC_GLOBAL_FLAGS[Gmat::UNKNOWN_OBJECT - Gmat::SPACECRAFT+1];

   /// Method to return the current number of instantiated objects
   static Integer          GetInstanceCount();
   /// Method for getting GMAT object type
   static UnsignedInt      GetObjectType(const std::string &typeString);
   /// Method for getting GMAT object type string
   static std::string      GetObjectTypeString(UnsignedInt type);
   /// Method for getting data precision
   static Integer          GetDataPrecision();
   /// Method for getting time precision
   static Integer          GetTimePrecision();
   /// Method for getting full path file name
   static std::string      GetFullPathFileName(std::string &outFileName,
                              const std::string &objName,
                              const std::string &inFileName,
                              const std::string &fileType, bool forInput,
                              const std::string &fileExt = "",
                              bool writeWarning = false, bool writeInfo = false);
   static  std::string     WriteObjectInfo(const std::string &title, GmatBase *obj,
                                           bool addEol = true);
   virtual Integer         GetPropItemID(const std::string &whichItem);
   virtual Integer         SetPropItem(const std::string &propItem);
   virtual StringArray     GetDefaultPropItems();
   virtual Real*           GetPropItem(const Integer item);
   virtual Integer         GetPropItemSize(const Integer item);
   virtual bool            PropItemNeedsFinalUpdate(const Integer item);
   virtual bool            HasAssociatedStateObjects();
   virtual std::string     GetAssociateName(UnsignedInt val = 0);

   virtual Integer         GetEstimationParameterID(const std::string &param);
   virtual std::string     GetParameterNameForEstimationParameter(const std::string &parmName);
   virtual std::string     GetParameterNameFromEstimationParameter(const std::string &parmName);
   virtual Integer         SetEstimationParameter(const std::string &param);
   virtual bool            IsEstimationParameterValid(const Integer id);
   virtual Integer         GetEstimationParameterSize(const Integer id);
   virtual Real*           GetEstimationParameterValue(const Integer id);
   virtual StringArray     GetSolveForList();

   virtual bool            HasDynamicParameterSTM(Integer parameterId);
   virtual Rmatrix*        GetParameterSTM(Integer parameterId);
   virtual Integer         GetStmRowId(const Integer forRow);

   // Covariance handling code
   virtual Integer         HasParameterCovariances(Integer parameterId);
   virtual Rmatrix*        GetParameterCovariances(Integer parameterId = -1);
   virtual Covariance*     GetCovariance();
   virtual bool            MapCovarianceToParameters();

   // Cloned object update management
   virtual bool HasLocalClones() = 0;
   virtual void UpdateClonedObject(GmatBase *obj);
   virtual void UpdateClonedObjectParameter(GmatBase *obj,
         Integer updatedParameterId);
   bool IsCommandModeAssignable() const;
   virtual bool IsParameterCommandModeSettable(const Integer id) const;
   void CopyParameter(const GmatBase& fromObject, const Integer forParameter);

   /// Methods for script name and flag where object is created from
   void SetScriptCreatedFrom(const std::string &script);
   std::string GetScriptCreatedFrom();
   void SetIsCreatedFromMainScript(bool flag);
   bool IsCreatedFromMainScript();
   void SetForceGenerateObjectString(bool flag);
   bool GetForceGenerateObjectString();

   virtual std::string AddFileInfoToErrorMessage(std::string msg);
   
   /// Functions use information from Moderator
   ObjectMap               GetConfiguredObjectMap();
   GmatBase*               GetConfiguredObject(const std::string &name);
   const StringArray       GetListOfObjects(UnsignedInt type);
   const StringArray       GetListOfObjects(const std::string &typeName);

   /// Hooks for GUI plugins
   virtual bool            HasGuiPlugin();
   virtual StringArray     GetGuiPanelNames(const std::string &type = "");
   virtual void            SetWidget(GmatWidget *widget);
   virtual const char**    GetIcon();
   virtual void            SetIconIndex(Integer index);
   virtual Integer         GetIconIndex();

   bool                    HasPrecisionTime();
   virtual bool            SetPrecisionTimeFlag(bool onOff);

   // Scripted Object Method Support
   StringArray             GetScriptedMethodList();
   Integer                 GetMethodParameterCount(const std::string &forMethod);
   ObjectTypeArray         GetMethodParameterTypes(const std::string &forMethod);
   virtual bool            SetMethodParameters(const std::string methodName,
                                 const StringArray & args,
                                 const std::string &precomment,
                                 const std::string &postcomment);
   // @todo Is this signature correct? Parameters are already set
   virtual Generic         ExecuteMethod(const std::string &forMethod,
                                 const StringArray &parameters);
   Integer                 GetScriptLineNumber() const;
   void                    SetScriptLineNumber(Integer val);

protected:

   // Code to handle generics
   UnsignedInt             GetGenericType(const std::string &fromString,
                                          StringArray &contents);
   UnsignedInt             GetGenericType(const Generic &g);
   Generic                 GetGeneric(const std::string &fromString);



   /// Parameter IDs
   enum
   {
      COVARIANCE = 0,
      GmatBaseParamCount,
   };

   static std::string helpStr;

   /// GmatBase parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[GmatBaseParamCount];
   /// GmatBase parameter labels
   static const std::string PARAMETER_LABEL[GmatBaseParamCount];

   /// count of the number of GmatBase objects currently instantiated
   static Integer      instanceCount;

   /// Count of the accessible parameters
   Integer             parameterCount;
   /// Script string used or this class
   std::string         typeName;
   /// Name of the object -- empty if it is nameless
   std::string         instanceName;
   
   /// Full name of this object
   std::string         instanceFullName;

   /// Script file name where object is created from
   std::string         scriptCreatedFrom;
   /// Flag indicating object is created from the main script
   bool                isCreatedFromMainScript;
   /// Flag indicating force generate string even though this object is not
   /// created from the main script
   bool                forceGenerateObjectString;
   /// Enumerated base type of the object
   UnsignedInt         type;
   /// Number of owned objects that belong to this instance
   Integer             ownedObjectCount;
   /// Script string used to build the object
   std::string         generatingString;
   /// The list of generic types that this class extends.
   ObjectTypeArray     objectTypes;
   /// The list types that this class extends, by name
   StringArray         objectTypeNames;

   /// The list of created objects 
   ObjectArray         createdObjects;

   /// Flag used to determine if associations have been made
   bool                isInitialized;

   /// The list of object types referenced by this class
   ObjectTypeArray     refObjectTypes;
   /// The list of object names referenced by this class
   StringArray         refObjectNames;
   /// The list of names of Wrapper objects
   StringArray         wrapperObjectNames;

   /// flag indicating whether or not the object using GmatTime for epoch
   bool                hasPrecisionTime;

   /// Flag indicating taht the object is an API generated object
   bool                isAPIObject;

   /// flag indicating whether or not the object is Global
   bool                isGlobal;
   /// flag indicating whether or not the object is automatic Global
   bool                isAutomaticGlobal;
   /// flag indicating whether or not the object is local inside a function
   bool                isLocal;
   
   /// flag indicating whether or not a Callback method is currently executing
   bool                callbackExecuting;

   /// error message and formats
   std::string         lastErrorMessage;
   std::string         errorMessageFormat;
   std::string         errorMessageFormatUnnamed;
   std::string         deprecatedMessageFormat;
   
   /// flag used to determine if the current write is in Matlab mode
   bool                inMatlabMode;

   /// Integer array used to hold the parameter write order
   /// This array is automatically created if array is empty
   IntegerArray        parameterWriteOrder;
   /// String used to hold the comment line
   std::string         commentLine;
   /// String used to hold inline comment
   std::string         inlineComment;
   /// String array used to hold the attribute comments
   StringArray         attributeCommentLines;
   /// String array used to hold the attribute inline comments
   StringArray         attributeInlineComments;
   /// Flag to indicating whether to show preface comment
   bool                showPrefaceComment;
   /// Flag to indicating whether to show inline comment
   bool                showInlineComment;
   /// flag indicating whether or not to omit the "Create" line when writing the script
   bool                cloaking;
   /// Utility array used to return types for objects to be shown on a GUI
   ObjectTypeArray     listedTypes;

   /// Flag used to turn off command mode assignment, throwing an exception if true and used
   bool                blockCommandModeAssignment;

   // Ordered list of parameters that have covariances
   StringArray         covarianceList;
   /// Ordered list of parameter IDs that have covariances
   IntegerArray        covarianceIds;
   // Size of the covariance element
   IntegerArray        covarianceSizes;
   /// Covariance matrix for parameters identified in covarianceList
   Covariance          covariance;
   /// Solve-for parameters that the object specifies
   StringArray         solveForList;
   
   // Some string arrays need to be written even if they are empty
   bool                writeEmptyStringArray;
   
   // Scripting interfaces
   void                CopyParameters(const GmatBase &a);
   virtual void        WriteParameters(Gmat::WriteMode mode,
                                       std::string &prefix,
                                       std::stringstream &stream);
   virtual void        WriteStringArrayValue(Gmat::WriteMode mode,
                                             std::string &prefix,
                                             Integer id, bool writeQuotes,
                                             std::stringstream &stream);
   virtual void        WriteParameterValue(Integer id,
                                           std::stringstream &stream);

   RHSEquation*        ParseEquation(const std::string &equationString,
                                     ObjectMap *configObjectMap);
                                     
   // Scripted Object Method Support
   /// Methods that were scripted
   StringArray                            objectMethods;
   /// Comments for scripted methods
   StringArray                            objectMethodPrefaces;
   /// Input counts requires for a method
   std::map<std::string, Integer>         methodRequiredInputCount;
   /// Input counts allowed for a method
   std::map<std::string, Integer>         methodTotalInputCount;
   /// The inputs on a method
   std::map<std::string, ObjectTypeArray> methodInputs;

   /// The (object mode) scripted methods, for use when saving the script
   StringArray                            scriptedMethods;

   virtual bool            SetMethodParameter(const std::string methodName,
                                 const std::string &arg, const Integer index,
                                 bool &commandComplete);

   /// Used to force equation evaluation, in case the eq'n isn't a field setting
   bool                    hasEquation;

   ///Line number in script where object was created or called. Used when throwing errors. 
   Integer scriptLineNumber;


private:

   virtual void PrepCommentTables();
};


//------------------------------------------------------------------------------
//  std::string GetTypeName() const
//------------------------------------------------------------------------------
/**
 * Retrieves the type name (i.e. the type used in scripting) for the object.
 *
 * @return The type name.
 */
std::string GmatBase::GetTypeName() const
{
   return typeName;
}


//------------------------------------------------------------------------------
//  std::string GetName() const
//------------------------------------------------------------------------------
/**
 * Retrieves the object's name.
 *
 * @return The name.
 */
std::string GmatBase::GetName() const
{
   return instanceName;
}
#endif // GmatBase_hpp

