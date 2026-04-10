//$Id$
//------------------------------------------------------------------------------
//                                OrbitRvec6
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
// Created: 2004/09/08
//
/**
 * Declares OrbitRvec6 class which provides base class for orbit realated
 * Rvector6 Parameters.
 */
//------------------------------------------------------------------------------
#ifndef OrbitRvec6_hpp
#define OrbitRvec6_hpp

#include "gmatdefs.hpp"
#include "Rvec6Var.hpp"
#include "OrbitData.hpp"


class GMAT_API OrbitRvec6 : public Rvec6Var, public OrbitData
{
public:

   OrbitRvec6(const std::string &name, const std::string &typeStr, 
             GmatBase *obj, const std::string &desc,
             const std::string &unit, GmatParam::DepObject depObj,
             UnsignedInt objType = Gmat::SPACECRAFT);
   OrbitRvec6(const OrbitRvec6 &copy);
   OrbitRvec6& operator=(const OrbitRvec6 &right);
   virtual ~OrbitRvec6();
   
   // methods inherited from Parameter
   virtual const Rvector6& EvaluateRvector6();
   
   virtual Integer GetNumRefObjects() const;
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual void SetInternalCoordSystem(CoordinateSystem *ss);
   virtual bool AddRefObject(GmatBase *obj, bool replaceName = false);
   virtual bool Validate();
   virtual bool Initialize();
   
   // methods inherited from GmatBase
   virtual bool RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual std::string GetRefObjectName(const UnsignedInt type) const;
   virtual bool SetRefObjectName(const UnsignedInt type,
                                 const std::string &name);
   virtual GmatBase* GetRefObject(const UnsignedInt type,
                                  const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name = "");
   
protected:

};

#endif // OrbitRvec6_hpp
