
//$Id$
//------------------------------------------------------------------------------
//                                  HardwareString
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Peter Candell
// Created: 2022.08.29
//
/**
 * Declares Hardware String data class.
 */
 //------------------------------------------------------------------------------
#ifndef HardwareString_hpp
#define HardwareString_hpp

#include "gmatdefs.hpp"
#include "StringVar.hpp"
#include "SpacecraftData.hpp"


class GMAT_API HardwareString : public StringVar, public SpacecraftData
{
public:
   HardwareString(const std::string &name, const std::string &typeStr,
      UnsignedInt ownerType, UnsignedInt ownedObjType,
      GmatBase *obj, const std::string &desc, const std::string &unit,
      bool isSettable);
   HardwareString(const HardwareString &copy);
   HardwareString& operator=(const HardwareString &right);
   virtual ~HardwareString();

   // methods inherited from Parameter
   virtual const std::string& EvaluateString();
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

#endif