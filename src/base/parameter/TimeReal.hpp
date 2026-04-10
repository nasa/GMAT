//$Id$
//------------------------------------------------------------------------------
//                                TimeReal
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
// Created: 2004/04/28
//
/**
 * Declares TimeReal class which provides base class for time realated Real
 * Parameters
 */
//------------------------------------------------------------------------------
#ifndef TimeReal_hpp
#define TimeReal_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "TimeData.hpp"


class GMAT_API TimeReal : public RealVar, public TimeData
{
public:

   TimeReal(const std::string &name, const std::string &typeStr, 
            GmatBase *obj, const std::string &desc, const std::string &unit,
            bool isSettable = false, UnsignedInt paramOwnerType = Gmat::SPACECRAFT);
   TimeReal(const TimeReal &copy);
   TimeReal& operator=(const TimeReal &right);
   virtual ~TimeReal();

   // methods inherited from Parameter
   virtual Real EvaluateReal();
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
protected:
   

};

#endif // TimeReal_hpp
