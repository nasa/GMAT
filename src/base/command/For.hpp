//$Id$
//------------------------------------------------------------------------------
//                                  ForFor
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
// Author:  Joey Gurgnaus
// Created: 2004/01/22
//
/**
 * Definition for the For command class 
 */
//------------------------------------------------------------------------------

#ifndef For_hpp
#define For_hpp
 
#include "BranchCommand.hpp"
#include "Parameter.hpp"
#include "ElementWrapper.hpp"

/**
 * Command that manages processing for entry to the For loop
 *
 * The For command manages the For loop.
 *
 */
class GMAT_API For : public BranchCommand
{
public:
  // default constructor
   For();
   // copy constructor
   For(const For& f);
   // operator =
   For&                 operator=(const For& f);
   // destructor
   virtual ~For();

   // Inherited methods that need some enhancement from the base class
   virtual bool         Append(GmatCommand *cmd);

   // Methods used to run the command
   virtual bool         Initialize();
   virtual bool         Execute();
   virtual void         RunComplete();
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                            const std::string &prefix = "",
                                            const std::string &useName = "");
   
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual const ObjectTypeArray&
                       GetRefObjectTypeArray();
   virtual const StringArray&
                       GetRefObjectNameArray(const UnsignedInt type);
   
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const char *value);
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value);
   virtual const StringArray& 
                       GetWrapperObjectNameArray(bool completeSet = false);
   virtual bool        SetElementWrapper(ElementWrapper* toWrapper,
                                         const std::string &withName);
   virtual void        ClearWrappers();
    
   DEFAULT_TO_NO_CLONES

protected:
   enum
   {
      START_VALUE = BranchCommandParamCount,
      END_VALUE,
      STEP,
      INDEX_NAME,
      START_NAME,
      END_NAME,
      INCREMENT_NAME,
      ForParamCount
   };

   static const std::string PARAMETER_TEXT[
                ForParamCount - BranchCommandParamCount];

   static const Gmat::ParameterType PARAMETER_TYPE[
                ForParamCount - BranchCommandParamCount];
   
   static const Real UNINITIALIZED_VALUE;
   static const Real DEFAULT_START;
   static const Real DEFAULT_END;
   static const Real DEFAULT_INCREMENT;

   static bool       writeInvalidLoopMessage;

   /// Start value for the For loop
   Real           startValue;
   /// End value for the For loop
   Real           endValue;
   /// Step value for the For loop
   Real           stepSize;
   /// Current value for the For loop counter
   Real           currentValue;
   /// Number of passes to be made through the loop
   int            numPasses;
   /// Current pass number
   int            currentPass;
   
   /// Wrappers for index, start, end, and increment
   ElementWrapper *indexWrapper;
   ElementWrapper *startWrapper;
   ElementWrapper *incrWrapper;
   ElementWrapper *endWrapper;
   
   /// Are we incrementing the loop variable in a positive direction? i.e. is the stepsize positive?
   bool           incrPositive;

   /// String representations of index, start, end, and increment numbers
   std::string    indexName;
   std::string    startName;
   std::string    endName;
   std::string    incrName;
   
   /// Method to evaluate the counter to see if we are still looping
   bool StillLooping();
};
#endif  // For_hpp
