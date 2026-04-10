//$Id$
//------------------------------------------------------------------------------
//                                  Parameter
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
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/16
//
/**
 * Declares base class of parameters.
 */
//------------------------------------------------------------------------------
#ifndef Parameter_hpp
#define Parameter_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Rvector6.hpp"
#include "Rmatrix.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"

class PhysicalModel;

namespace GmatParam
{
   enum ParameterKey
   {
      SYSTEM_PARAM,
      USER_PARAM,
      KeyCount
   };
   
   enum DepObject
   {
      COORD_SYS,
      ORIGIN,
      NO_DEP,
      OWNED_OBJ,
      ATTACHED_OBJ,
      ODE_MODEL,
      DepObjectCount
   };
   
   enum CycleType
   {
      NOT_CYCLIC,
      ZERO_90,
      ZERO_180,
      ZERO_360,
      PLUS_MINUS_90,
      PLUS_MINUS_180,
      ZERO_24,
      OTHER_CYCLIC
   };
};

class GMAT_API Parameter : public GmatBase
{
public:

   Parameter(const std::string &name, const std::string &typeStr,
             GmatParam::ParameterKey key, GmatBase *owner,
             const std::string &desc, const std::string &unit,
             GmatParam::DepObject depObj, UnsignedInt ownerType,
             bool isTimeParam, bool isSettable, bool isPlottable,
             bool isReportable,
             UnsignedInt ownedObjType = Gmat::UNKNOWN_OBJECT);
   Parameter(const Parameter &copy);
   Parameter& operator= (const Parameter& right);
   virtual ~Parameter();
   
   GmatParam::ParameterKey  GetKey() const;
   GmatBase*                GetOwner() const;
   UnsignedInt         GetOwnerType() const;
   Gmat::ParameterType      GetReturnType() const;
   GmatParam::CycleType     GetCycleType() const;
   std::string              GetParameterClassType() const;
   
   void  SetKey(const GmatParam::ParameterKey &key);
   void  SetOwner(GmatBase *owner);
   void  SetParameterClassType(const std::string &classType);
   
   bool  IsSystemParameter() const;
   bool  IsAngleParameter() const;
   bool  IsTimeParameter() const;
   bool  IsPlottable() const;
   bool  IsReportable() const;
   bool  IsSettable() const;
   bool  IsCoordSysDependent() const;
   bool  IsOriginDependent() const;
   bool  IsOwnedObjectDependent() const;
   bool  NeedCoordSystem() const;
   bool  NeedExternalClone() const;
   virtual bool IsOptionalField(const std::string &field) const;
   virtual const std::string GetExternalCloneName(Integer whichOne = 0);
   virtual void SetExternalClone(GmatBase *clone);
   
   virtual bool RequiresBodyFixedCS() const;
   virtual bool RequiresCelestialBodyCSOrigin() const;
   virtual void SetRequiresBodyFixedCS(bool flag);
   virtual void SetRequiresCelestialBodyCSOrigin(bool flag);
   
   // Methods needed to provide handling for transient forces (e.g. finite burn)
   virtual bool NeedsForces() const;
   virtual void SetTransientForces(std::vector<PhysicalModel*> *tf);
   
   bool operator==(const Parameter &right) const;
   bool operator!=(const Parameter &right) const;
   
   virtual std::string        ToString();
   
   virtual Real               GetReal() const;
   virtual const Rvector6&    GetRvector6() const;
   virtual const Rmatrix66&   GetRmatrix66() const;
   virtual const Rmatrix33&   GetRmatrix33() const;
   virtual const Rmatrix&     GetRmatrix() const;
   virtual const std::string& GetString() const;
   
   virtual void               SetReal(Real val);
   virtual void               SetRvector(const Rvector &val);
   virtual void               SetRvector6(const Rvector6 &val);
   virtual void               SetRmatrix66(const Rmatrix66 &mat);
   virtual void               SetRmatrix33(const Rmatrix33 &mat);
   virtual void               SetRmatrix(const Rmatrix &mat);
   virtual void               SetString(const std::string &val);
   
   virtual Real               EvaluateReal();
   virtual const Rvector&     EvaluateRvector();
   virtual const Rvector6&    EvaluateRvector6();
   virtual const Rmatrix66&   EvaluateRmatrix66();
   virtual const Rmatrix33&   EvaluateRmatrix33();
   virtual const Rmatrix&     EvaluateRmatrix();
   virtual const std::string& EvaluateString();
   
   virtual const std::string* GetParameterList() const;
   
   virtual CoordinateSystem*  GetInternalCoordSystem();
   
   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void         SetSolarSystem(SolarSystem *ss);
   
   virtual bool         Initialize();
   virtual bool         Evaluate();
   
   // methods all SYSTEM_PARAM should implement
   virtual bool         AddRefObject(GmatBase *object, bool replaceName = false);
   virtual Integer      GetNumRefObjects() const;
   virtual bool         Validate();
   
   // methods inherited from GmatBase
   virtual bool         SetName(const std::string &who,
                                const std::string &oldName = "");
   virtual void         Copy(const GmatBase*);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   
   virtual UnsignedInt  GetUnsignedIntParameter(const Integer id) const;
   virtual UnsignedInt  GetUnsignedIntParameter(const std::string &label) const;
   virtual UnsignedInt  SetUnsignedIntParameter(const Integer id,
                                                const UnsignedInt value);
   virtual UnsignedInt  SetUnsignedIntParameter(const std::string &label,
                                                const UnsignedInt value);
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   virtual const std::string
                        GetCommentLine(Integer which = 1);
   virtual void         SetCommentLine(const std::string &comment,
                                              Integer which = 0);
   
   static std::string   GetDependentTypeString(const GmatParam::DepObject depObj);
   
   DEFAULT_TO_NO_CLONES

protected:
   
   static const std::string PARAMETER_KEY_STRING[GmatParam::KeyCount];
   
   GmatParam::ParameterKey  mKey;
   GmatBase *mOwner;
   
   std::string   mDesc;
   std::string   mUnit;
   std::string   mExpr;
   std::string   mOwnerName;
   std::string   mDepObjectName;
   std::string   mCommentLine2;
   std::string   mInitialValue;
   std::string   mParameterClassType;
   
   UnsignedInt     mOwnerType;
   UnsignedInt     mOwnedObjectType;
   Gmat::ParameterType  mReturnType;
   GmatParam::DepObject mDepObj;
   GmatParam::CycleType mCycleType;
   UnsignedInt          mColor;

   bool mIsAngleParam;
   bool mIsTimeParam;
   bool mIsPlottable;
   bool mIsReportable;
   bool mIsSettable;
   bool mIsCoordSysDependent;
   bool mIsOriginDependent;
   bool mIsOwnedObjDependent;
   bool mNeedCoordSystem;
   bool mNeedExternalClone;
   bool mRequiresBodyFixedCS;
   bool mRequiresCelestialBodyCSOrigin;
   bool mIsCommentFromCreate;
   
   enum
   {
      OBJECT = GmatBaseParamCount,
      INITIAL_VALUE,
      EXPRESSION,
      DESCRIPTION,
      UNIT,
      DEP_OBJECT,
      COLOR,
      ParameterParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[ParameterParamCount - GmatBaseParamCount];
   static const std::string
      PARAMETER_TEXT[ParameterParamCount - GmatBaseParamCount];
};
#endif // Parameter_hpp

