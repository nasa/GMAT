//$Id$
//------------------------------------------------------------------------------
//                                ParameterDatabase
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
// Created: 2003/09/18
//
/**
 * Declares parameter database class.
 */
//------------------------------------------------------------------------------
#ifndef ParameterDatabase_hpp
#define ParameterDatabase_hpp

#include "gmatdefs.hpp"
#include "paramdefs.hpp"
#include "Parameter.hpp"

class GMAT_API ParameterDatabase
{

public:
   
   ParameterDatabase();
   ParameterDatabase(const ParameterDatabase &copy);
   ParameterDatabase& operator=(const ParameterDatabase &right);
   virtual ~ParameterDatabase();
   
   Integer GetNumParameters() const;
   const StringArray& GetNamesOfParameters();
   ParameterPtrArray GetParameters() const;
   
   bool HasParameter(const std::string &name) const;
   bool RenameParameter(const std::string &oldName, const std::string &newName);
   Integer GetParameterCount(const std::string &name) const;
   
   Parameter* GetParameter(const std::string &name) const;
   std::string GetFirstParameterName() const;
   bool SetParameter(const std::string &name, Parameter *param);
   
   void Add(const std::string &name, Parameter *param = NULL);
   void Add(Parameter *param);
   void Remove(const std::string &name);
   void Remove(const Parameter *param);
   
protected:
private:

   StringParamPtrMap *mStringParamPtrMap;
   StringArray mParamNames;
   Integer mNumParams;

};
#endif // ParameterDatabase_hpp

