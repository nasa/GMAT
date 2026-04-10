//$Id$
//------------------------------------------------------------------------------
//                                OrbitReal
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
// Created: 2004/03/29
//
/**
 * Declares OrbitReal class which provides base class for orbit realated Real
 * Parameters
 */
//------------------------------------------------------------------------------
#ifndef OrbitReal_hpp
#define OrbitReal_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "OrbitData.hpp"


class GMAT_API OrbitReal : public RealVar, public OrbitData
{
public:

   OrbitReal(const std::string &name, const std::string &typeStr, 
             GmatBase *obj, const std::string &desc,
             const std::string &unit,  GmatParam::DepObject depObj,
             Integer itemId = -999, bool isSettable = false,
             bool isPlottable = true, bool isReportable = true,
             UnsignedInt paramOwnerType = Gmat::SPACECRAFT);
   OrbitReal(const OrbitReal &copy);
   OrbitReal& operator=(const OrbitReal &right);
   virtual ~OrbitReal();
   
   // methods inherited from Parameter
   virtual GmatBase* GetOwner();
   virtual Real EvaluateReal();
   virtual void SetReal(Real val);
   
   virtual Integer GetNumRefObjects() const;
   virtual CoordinateSystem* GetInternalCoordSystem();
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual void SetInternalCoordSystem(CoordinateSystem *ss);
   virtual bool AddRefObject(GmatBase*obj, bool replaceName = false);
   virtual bool Validate();
   virtual bool Initialize();
   
   // methods inherited from GmatBase
   virtual bool RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual std::string GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type);
   virtual bool SetRefObjectName(const UnsignedInt type,
                                 const std::string &name);
   virtual GmatBase* GetRefObject(const UnsignedInt type,
                                  const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name = "");

protected:
   
   Integer mItemId;
};

#endif // OrbitReal_hpp
