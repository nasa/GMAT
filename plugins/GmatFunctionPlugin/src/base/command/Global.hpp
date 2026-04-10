//$Id$
//------------------------------------------------------------------------------
//                                 Global
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Wendy C. Shoan
// Created: 2008.03.14
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CCA54C
//
/**
 * Class declaration for the Global command
 */
//------------------------------------------------------------------------------


#ifndef Global_hpp
#define Global_hpp

#include "GmatFunction_defs.hpp"
#include "ManageObject.hpp"


/**
 * Declaration of the Global command
 */
class GMATFUNCTION_API Global : public ManageObject
{
public:
   Global();
   virtual          ~Global();
   Global(const Global &gl);
   Global&        operator=(const Global &gl);
   
   virtual GmatBase*    Clone() const;
   
   bool                 Initialize();
   bool                 Execute();
   
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);

   DEFAULT_TO_NO_CLONES

protected:
   enum
   {
      GlobalParamCount = ManageObjectParamCount, 
   };
   //static const std::string PARAMETER_TEXT[GlobalParamCount - ManageObjectParamCount];

   //static const Gmat::ParameterType PARAMETER_TYPE[GlobalParamCount - ManageObjectParamCount];

};

#endif /* Global_hpp */
