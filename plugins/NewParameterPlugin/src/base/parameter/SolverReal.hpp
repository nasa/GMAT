//$Id$
//------------------------------------------------------------------------------
//                                SolverReal
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
// Author: Darrel Conway
// Created: 2013/05/24
//
/**
 * Declares SolverReal class which provides base class for environment realated
 * Real Parameters
 */
//------------------------------------------------------------------------------
#ifndef SolverReal_hpp
#define SolverReal_hpp

#include "newparameter_defs.hpp"
#include "RealVar.hpp"
#include "SolverData.hpp"


class NEW_PARAMETER_API SolverReal : public RealVar, public SolverData
{
public:

   SolverReal(const std::string &name, const std::string &typeStr,
           GmatBase *obj, const std::string &desc,
           const std::string &unit, UnsignedInt ownerType,
           GmatParam::DepObject depObj);
   SolverReal(const SolverReal &copy);
   SolverReal& operator=(const SolverReal &right);
   virtual ~SolverReal();

   // methods inherited from Parameter
   virtual Real EvaluateReal();
   
   virtual Integer GetNumRefObjects() const;
   virtual bool AddRefObject(GmatBase *obj, bool replaceName = false);
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

   virtual const std::string GetExternalCloneName(Integer whichOne);
   virtual void SetExternalClone(GmatBase *clone);

protected:
   
};

#endif // SolverReal_hpp
