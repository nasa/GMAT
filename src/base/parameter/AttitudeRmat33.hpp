//$Id$
//------------------------------------------------------------------------------
//                                  AttitudeRmat33
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
//
/**
 * Declares SpacecraftState Rmatrix data class.
 */
//------------------------------------------------------------------------------
#ifndef AttitudeRmat33_hpp
#define AttitudeRmat33_hpp

#include "gmatdefs.hpp"
#include "Rmat33Var.hpp"
#include "AttitudeData.hpp"


class GMAT_API AttitudeRmat33 : public Rmat33Var, public AttitudeData
{
public:

   AttitudeRmat33(const std::string &name, const std::string &typeStr,
                  GmatBase *obj, const std::string &desc, const std::string &unit,
                  GmatParam::DepObject depObj, bool isSettable = false,
                  UnsignedInt paramOwnerType = Gmat::SPACECRAFT);
   AttitudeRmat33(const AttitudeRmat33 &copy);
   AttitudeRmat33& operator=(const AttitudeRmat33 &right);
   virtual ~AttitudeRmat33();

   // methods inherited from Parameter
   virtual const Rmatrix& EvaluateRmatrix();

   virtual Integer      GetNumRefObjects() const;
   virtual bool         AddRefObject(GmatBase*obj, bool replaceName = false);
   virtual bool         Validate();
   virtual bool         Initialize();

   // methods inherited from GmatBase
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);

   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObjectName(const UnsignedInt type,
                                         const std::string &name);
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");

protected:

};

#endif //AttitudeRmat33_hpp
