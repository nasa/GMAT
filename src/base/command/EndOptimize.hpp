//$Id$
//------------------------------------------------------------------------------
//                                EndOptimize 
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// number NNG04CC06P
//
// Author:  Daniel Hunter/GSFC/MAB (CoOp)
// Created: 2006.07.20
//
/**
 * Declaration for the EndOptimize command class
 */
//------------------------------------------------------------------------------

#ifndef EndOptimize_hpp
#define EndOptimize_hpp

#include "GmatCommand.hpp"


class GMAT_API EndOptimize : public GmatCommand
{
public:
   EndOptimize();
   EndOptimize(const EndOptimize& eo);
   EndOptimize&            operator=(const EndOptimize& eo);
   virtual ~EndOptimize();
    
   virtual bool            Initialize();
   virtual bool            Execute();
    
   virtual bool            Insert(GmatCommand *cmd, GmatCommand *prev);

   // inherited from GmatBase
   virtual GmatBase*       Clone() const;
   virtual const std::string&
                           GetGeneratingString(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName);
   virtual bool            RenameRefObject(const UnsignedInt type,
                                           const std::string &oldName,
                                           const std::string &newName);
   DEFAULT_TO_NO_CLONES

protected:

   enum
   {
      EndOptimizeParamCount = GmatCommandParamCount,
   };

   // save for possible later use
   //static const std::string
   //PARAMETER_TEXT[EndOptimizeParamCount - GmatCommandParamCount];   
   //static const Gmat::ParameterType
   //PARAMETER_TYPE[EndOptimizeParamCount - GmatCommandParamCount];
};


#endif /*EndOptimize_hpp*/
