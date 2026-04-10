//$Id$
//------------------------------------------------------------------------------
//                                Variable
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
// Created: 2004/09/15
//
/**
 * Implements Variable class.
 */
//------------------------------------------------------------------------------

#include "Variable.hpp"
#include "ParameterException.hpp"
#include "ExpressionParser.hpp"
#include "MessageInterface.hpp"
#include "Linear.hpp"              // for ToString()
#include "StringUtil.hpp"          // for Replace()
#include "GmatGlobal.hpp"          // for IsWritingGmatKeyword()
#include <sstream>

//#define DEBUG_VARIABLE_SET
//#define DEBUG_VARIABLE_EVAL
//#define DEBUG_RENAME
//#define DEBUG_GEN_STRING
//#define DEBUG_VARIABLE_CLONE

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Variable(const std::string &name, const std::string &valStr = "",
//          const std::string &desc = "", const std::string &unit = "")
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 */
//------------------------------------------------------------------------------
Variable::Variable(const std::string &name, const std::string &valStr,
                   const std::string &desc, const std::string &unit)
   : RealVar(name, valStr, "Variable", GmatParam::USER_PARAM, NULL, desc, unit,
             GmatParam::NO_DEP, Gmat::UNKNOWN_OBJECT, false)
{
   objectTypes.push_back(Gmat::VARIABLE);
   objectTypeNames.push_back("Variable");
   blockCommandModeAssignment = false;
   
   #ifdef __ALLOW_SIMPLE_VAR_EXP__
   CreateSimpleExpression();
   #endif
   
   // Initialize real value and expression
   mIsNumber = true;
   mRealValue = 0.0;
   mExpr = "0";
}


//------------------------------------------------------------------------------
// Variable(const Variable &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
Variable::Variable(const Variable &copy)
   : RealVar(copy)
{
   #ifdef DEBUG_VARIABLE_CLONE
   MessageInterface::ShowMessage
      ("Variable copy constructor <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   
   #ifdef __ALLOW_SIMPLE_VAR_EXP__
   CopySimpleExpression(copy);
   #endif
   
   #ifdef DEBUG_VARIABLE_CLONE
   MessageInterface::ShowMessage
      ("Variable copy constructor this=<%p> '%s' leaving, mRealValue=%f\n",
       this, GetName().c_str(), mRealValue);
   #endif
}


//------------------------------------------------------------------------------
// Variable& operator=(const Variable &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
Variable& Variable::operator=(const Variable &right)
{
   if (this != &right)
   {
      // We don't want to change the name when copy
      std::string thisName = GetName();
      
      RealVar::operator=(right);

      #ifdef __ALLOW_SIMPLE_VAR_EXP__
      AssignSimpleExpression(right);
      #endif
      
      SetName(thisName);
   }
   
   #ifdef DEBUG_VARIABLE_ASSIGN
   MessageInterface::ShowMessage
      ("***** Variable(=) this=<%p> '%s', mRealValue=%f\n",
       this, GetName().c_str(), mRealValue);
   #endif
   
   return *this;
}


//------------------------------------------------------------------------------
// ~Variable()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Variable::~Variable()
{
   #ifdef __ALLOW_SIMPLE_VAR_EXP__
   DeleteSimpleExpression();
   #endif
   
}

//-------------------------------------
// Inherited methods from RealVar
//-------------------------------------

//------------------------------------------------------------------------------
// virtual Real GetReal()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Real Variable::GetReal() const
{
   return mRealValue;
}

//------------------------------------------------------------------------------
// virtual Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Real Variable::EvaluateReal()
{
   #ifdef DEBUG_VARIABLE_EVAL
   MessageInterface::ShowMessage
      ("Variable::EvaluateReal() this=<%p>'%s', mExpr=%s, mIsNumber=%d\n",
       this, GetName().c_str(), mExpr.c_str(), mIsNumber);
   #endif
   
   if (mIsNumber)
   {      
      #ifdef DEBUG_VARIABLE_EVAL
      MessageInterface::ShowMessage
         ("Variable::EvaluateReal() Returning just a number: mRealValue=%f\n",
          mRealValue);
      #endif
      
      return mRealValue;
   }
   else
   {
      //=======================================================
      #ifdef __ALLOW_SIMPLE_VAR_EXP__
      //=======================================================
      try
      {         
         // Evaluate the expression
         mRealValue = mExpParser->EvalExp(mExpr.c_str());
         
         #ifdef DEBUG_VARIABLE_EVAL
         MessageInterface::ShowMessage
            ("Variable::EvaluateReal() Returning expression evaluation: "
             "mRealValue=%f\n", mRealValue);
         #endif
         
         return mRealValue;
      }
      catch (BaseException &e)
      {
         throw ParameterException
            (e.GetFullMessage() + " for the Variable \"" + GetName() + "\"");
      }
      //=======================================================
      #endif
      //=======================================================
   }

   // If you get here, the evaluation failed
   throw ParameterException("Variable::EvaluateReal() failed for the Variable \"" + 
      GetName() + "\"");
}


//-------------------------------------
// Methods inherited from GmatBase
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
GmatBase* Variable::Clone() const
{
   return new Variable(*this);
}


//------------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//------------------------------------------------------------------------------
void Variable::Copy(const GmatBase* orig)
{
   operator=(*((Variable *)(orig)));
}


//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool Variable::IsParameterCommandModeSettable(const Integer id) const
{
   // For now, turn them all on
   return true;
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, throw if the parameter is not stored.
 */
//------------------------------------------------------------------------------
bool Variable::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_VARIABLE_SET
   MessageInterface::ShowMessage
      ("Variable::SetStringParameter() this=<%p>'%s', id=%d, value='%s'\n", this,
       GetName().c_str(), id, value.c_str());
   #endif
   
   switch (id)
   {
   case EXPRESSION:
      {         
         // if value is blank or number, call Parent class
         Real rval;
         if (value == "" || GmatStringUtil::ToReal(value, &rval))
         {
            return RealVar::SetStringParameter(id, value);
         }
         else
         {
            mValueSet = true;
            mIsNumber = false;
            mExpr = value;  
            
            #ifdef DEBUG_VARIABLE_SET
            MessageInterface::ShowMessage
               ("   Variable expression set to '%s'\n", value.c_str());
            #endif
            return true;
         }
      }
   default:
      return Parameter::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, false if not.
 */
//------------------------------------------------------------------------------
bool Variable::SetStringParameter(const std::string &label,
                                 const std::string &value)
{
   #ifdef DEBUG_VARIABLE_SET
   MessageInterface::ShowMessage
      ("Variable::SetStringParameter() label=%s value=%s\n",
       label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool Variable::RenameRefObject(const UnsignedInt type,
                               const std::string &oldName,
                               const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Variable::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::PARAMETER && type != Gmat::COORDINATE_SYSTEM &&
       type != Gmat::SPACECRAFT && type != Gmat::CALCULATED_POINT)
      return true;
   
   // change expression if it has object name and .
   std::string tmpOldName = oldName + ".";
   std::string::size_type pos = mExpr.find(tmpOldName);
   if (pos != mExpr.npos)
      mExpr = GmatStringUtil::Replace(mExpr, oldName, newName);
   
   #ifdef __ALLOW_SIMPLE_VAR_EXP__
   mParamDb->RenameParameter(oldName, newName);
   #endif
   
   RealVar::RenameRefObject(type, oldName, newName);
   return true;
}


//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string Variable::GetRefObjectName(const UnsignedInt type) const
{
   if (type != Gmat::PARAMETER)
   {
      throw ParameterException
         ("Variable::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   #ifdef __ALLOW_SIMPLE_VAR_EXP__
      return mParamDb->GetFirstParameterName();
   #else
      return "";
   #endif
}


//------------------------------------------------------------------------------
// virtual bool SetRefObjectName(const UnsignedInt type,
//                               const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets reference object name to given object type.
 *
 * @param <type> object type
 * @param <name> object name
 *
 */
//------------------------------------------------------------------------------
bool Variable::SetRefObjectName(const UnsignedInt type,
                                const std::string &name)
{
   #ifdef DEBUG_VARIABLE_SET
   MessageInterface::ShowMessage
      ("Variable::SetRefObjectName() this=<%p>'%s', type=%d, name=%s\n", this,
       GetName().c_str(), type, name.c_str());
   #endif
   
   if (type != Gmat::PARAMETER)
   {
      throw ParameterException
         ("Variable::SetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   #ifdef DEBUG_VARIABLE_SET
   MessageInterface::ShowMessage
      ("   Adding '%s' to '%s' parameter database\n", name.c_str(), GetName().c_str());
   #endif
   
   #ifdef __ALLOW_SIMPLE_VAR_EXP__
   mParamDb->Add(name);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* Variable::GetRefObject(const UnsignedInt type,
                                 const std::string &name)
{
   if (type != Gmat::PARAMETER)
   {
      throw ParameterException
         ("Variable::GetRefObject() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   #ifdef __ALLOW_SIMPLE_VAR_EXP__
      return mParamDb->GetParameter(name);
   #else
      return NULL;
   #endif
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool Variable::SetRefObject(GmatBase *obj, const UnsignedInt type,
                            const std::string &name)
{
   if (type != Gmat::PARAMETER)
   {
      throw ParameterException
         ("Variable::SetRefObject() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   #ifdef __ALLOW_SIMPLE_VAR_EXP__
      return mParamDb->SetParameter(name, (Parameter*)obj);
   #else
      return false;
   #endif
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& Variable::GetRefObjectNameArray(const UnsignedInt type)
{
   //=======================================================
   #ifdef __ALLOW_SIMPLE_VAR_EXP__
   //=======================================================
      if (mParamDb == NULL)
         throw ParameterException
            ("Variable::GetRefObjectNameArray() mParamDb is NULL\n");
      
      #ifdef DEBUG_REF_OBJ
      MessageInterface::ShowMessage
         ("Variable::GetRefObjectNameArray() type=%d\n", type);
      StringArray paramNames = mParamDb->GetNamesOfParameters();
      MessageInterface::ShowMessage
         ("Variable::GetStringArrayParameter() mParamDb->GetNamesOfParameters() "
          "size=%d\n", paramNames.size());
      for (unsigned int i=0; i<paramNames.size(); i++)
         MessageInterface::ShowMessage
            ("Variable::GetStringArrayParameter() "
             "paramNames[%d]=%s\n", i, paramNames[i].c_str());
      #endif
      
      return mParamDb->GetNamesOfParameters();
   //=======================================================
   #else
   //=======================================================
      static StringArray objectNames;
      return objectNames;
   //=======================================================
   #endif
   //=======================================================
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useName
//------------------------------------------------------------------------------
/**
 * Produces a string, possibly multi-line, containing the text that produces an
 * object.
 * 
 * @param mode Specifies the type of serialization requested.
 * @param prefix Optional prefix appended to the object's name
 * @param useName Name that replaces the object's name.
 * 
 * @return A string containing the text.
 */
//------------------------------------------------------------------------------
const std::string& Variable::GetGeneratingString(Gmat::WriteMode mode,
                                                 const std::string &prefix,
                                                 const std::string &useName)
{
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("Variable::GetGeneratingString() this=<%p>'%s', mode=%d, prefix='%s', "
       "useName='%s'\n   mExpr='%s', mRealValue=%f, mIsNumber=%d, mValueSet=%d\n",
       this, GetName().c_str(), mode, prefix.c_str(), useName.c_str(),
       mExpr.c_str(), mRealValue, mIsNumber, mValueSet);
   #endif
   
   bool generateStr = false;
   
   if (mode == Gmat::SHOW_SCRIPT)
   {
      #ifdef DEBUG_GEN_STRING
      MessageInterface::ShowMessage("   mode is SHOW_SCRIPT, so generating string\n");
      #endif
      generateStr = true;
   }
   else
   {
      if (mIsNumber)
      {
         Real rval = GetReal();
         
         // Write value if it is set from user or non-zero
         if (mValueSet || rval != 0.0)
         {
            #ifdef DEBUG_GEN_STRING
            MessageInterface::ShowMessage
               ("   '%s' is a non-zero number or set by user, so generating string\n",
                mExpr.c_str());
            #endif
            generateStr = true;
         }
      }
      else
      {
         #ifdef DEBUG_GEN_STRING
         MessageInterface::ShowMessage
            ("   '%s' is not a number, so generating string\n", mExpr.c_str());
         #endif
         generateStr = true;
      }
   }
   
   if (generateStr)
   {
      //generatingString = "GMAT " + GetName() + " = " + mExpr + ";";
      //generatingString = "GMAT " + GetName() + " = " + mInitialValue + ";";
      bool writeGmatKeyword = GmatGlobal::Instance()->IsWritingGmatKeyword();
      // We now write out GMAT prefix on option from the startup file (see GMT-3233)
      if (writeGmatKeyword)
         generatingString = "GMAT " + GetName() + " = " + GmatStringUtil::ToString(mRealValue, 16, false, 1) + ";";
      else
         generatingString = GetName() + " = " + GmatStringUtil::ToString(mRealValue, 16, false, 1) + ";";
      
      if ((mode != Gmat::OBJECT_EXPORT) && (mode != Gmat::NO_COMMENTS))
         generatingString = generatingString + inlineComment + "\n";
   }
   
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("Variable::GetGeneratingString() returning\n<%s>\n", generatingString.c_str());
   #endif
   
   return generatingString;
}


#ifdef __ALLOW_SIMPLE_VAR_EXP__
//------------------------------------------------------------------------------
// void CreateSimpleExpression()
//------------------------------------------------------------------------------
void Variable::CreateSimpleExpression()
{
   mParamDb = new ParameterDatabase();
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (mParamDb, name, "Variable::CreateSimpleExpression()", "mParamDb = new ParameterDatabase()");
   #endif
   
   mExpParser = new ExpressionParser();
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (mExpParser, name, "Variable::ParameterDatabase()", "mExpParser = new ExpressionParser()");
   #endif
   
   // Set parameter database to be used
   mExpParser->SetParameterDatabase(mParamDb);
   
   #ifdef DEBUG_VARIABLE_CREATE
   MessageInterface::ShowMessage("Variable::Variable() constructor\n");
   MessageInterface::ShowMessage
      ("   numDBParams = %d\n", mParamDb->GetNumParameters());
   #endif
}


//------------------------------------------------------------------------------
// void CopySimpleExpression(const Variable &copy)
//------------------------------------------------------------------------------
void Variable::CopySimpleExpression(const Variable &copy)
{
   #ifdef DEBUG_VARIABLE_CLONE
   MessageInterface::ShowMessage
      ("Variable::CopySimpleExpression() <%p>'%s' entered, mParamDb=<%p>, "
       "mExpParser=<%p>\n", this, GetName().c_str(), mParamDb, mExpParser);
   #endif
   
   mParamDb = new ParameterDatabase(*copy.mParamDb);
   
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (mParamDb, GetName(), "Variable::Variable(copy)", "mParamDb = new ParameterDatabase()");
   #endif
   
   mExpParser = new ExpressionParser();
   
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (mExpParser, GetName(), "Variable::Variable(copy)", "mExpParser = new ExpressionParser()");
   #endif
   
   mExpParser->SetParameterDatabase(mParamDb);
   
   #ifdef DEBUG_VARIABLE_CLONE
   MessageInterface::ShowMessage("Variable::Variable(copy)\n");
   MessageInterface::ShowMessage
      ("   copy.numDBParams = %d\n", copy.mParamDb->GetNumParameters());
   MessageInterface::ShowMessage
      ("   numDBParams = %d\n", mParamDb->GetNumParameters());
   #endif
}


//------------------------------------------------------------------------------
// void AssignSimpleExpression(const Variable &right)
//------------------------------------------------------------------------------
void Variable::AssignSimpleExpression(const Variable &right)
{
   if (mParamDb)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (mParamDb, GetName(), "Variable::operator=", "deleting mParamDb");
      #endif
      delete mParamDb;
   }
   
   mParamDb = new ParameterDatabase(*right.mParamDb);
   
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (mParamDb, GetName(), "Variable::operator=", "mParamDb = new ParameterDatabase()");
   #endif
   
   if (mExpParser)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (mExpParser, GetName(), "Variable::operator=", "deleting mExpParser");
      #endif
      delete mExpParser;
   }
   
   mExpParser = new ExpressionParser();
   
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (mExpParser, GetName(), "Variable::operator=", "mExpParser = new ExpressionParser()");
   #endif
   
   mExpParser->SetParameterDatabase(mParamDb);
   // Set expression to name of right side since expression is used for
   // writnig in GetGeneratingString() (loj: 2008.08.13)
   // For example:
   // var1 = 123.45;
   // var2 = var1;
   // We want to write "var2 = var1" instead of "var2 = 123.45"
   mExpr = right.GetName();
}


//------------------------------------------------------------------------------
// void DeleteSimpleExpression()
//------------------------------------------------------------------------------
void Variable::DeleteSimpleExpression()
{
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Remove
      (mParamDb, GetName(), "Variable::~Variable()", "deleting mParamDb");
   #endif
   delete mParamDb;
   
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Remove
      (mExpParser, GetName(), "Variable::~Variable()", "deleting mExpParser");
   #endif
   delete mExpParser;
}

#endif
