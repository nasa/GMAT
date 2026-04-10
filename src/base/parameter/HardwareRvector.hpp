
//$Id$
//------------------------------------------------------------------------------
//                                HardwareRvector
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract task order 28
//
// Author: Peter Candell
// Created: 2022/08/30
//
/**
 * Declares HardwareRvector class which provides base class for environment realated
 * Real Parameters
 */
 //------------------------------------------------------------------------------
#ifndef HardwareRvector_hpp
#define HardwareRvector_hpp

#include "gmatdefs.hpp"
#include "RvectorVar.hpp"
#include "SpacecraftData.hpp"


class GMAT_API HardwareRvector : public RvectorVar, public SpacecraftData
{
public:
   HardwareRvector(const std::string &name, const std::string &typeStr,
      UnsignedInt ownerType, UnsignedInt ownedObjType,
      GmatBase *obj, const std::string &desc, const std::string &unit,
      bool isSettable, Integer size);
   HardwareRvector(const HardwareRvector &copy);
   HardwareRvector& operator=(const HardwareRvector &right);
   virtual ~HardwareRvector();

   // methods inherited from Parameter
   virtual const Rvector& EvaluateRvector();
   virtual bool IsOptionalField(const std::string &field) const;

   virtual Integer GetNumRefObjects() const;
   virtual bool Validate();
   virtual bool Initialize();
   virtual bool AddRefObject(GmatBase *obj, bool replaceName = false);

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
   virtual const std::string&
      GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
         const std::string &prefix = "",
         const std::string &useName = "");

protected:
};

#endif // HardwareRvector_hpp
