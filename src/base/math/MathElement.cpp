//$Id$
//------------------------------------------------------------------------------
//                                   MathElement
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
// number NNG04CC06P.
//
// Author: Waka Waktola
// Created: 2006/04/04
//
/**
 * Defines the Math elements class for Math in scripts.
 */
//------------------------------------------------------------------------------

#include "MathElement.hpp"
#include "StringUtil.hpp"  // for ToReal()
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_MATH_ELEMENT
//#define DEBUG_WRAPPERS
//#define DEBUG_INPUT_OUTPUT
//#define DEBUG_EVALUATE
//#define DEBUG_RENAME

//------------------------------------------------------------------------------
//  MathElement(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the MathElement object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
MathElement::MathElement(const std::string &typeStr, const std::string &name) :
   MathNode      ("MathElement", name),
   refObject     (NULL),
   refObjectName ("")
{
   #ifdef DEBUG_MATH_ELEMENT
   MessageInterface::ShowMessage
      ("MathElement::MathElement() typeStr='%s', name='%s' entered\n", typeStr.c_str(),
       name.c_str());
   #endif
   
   isFunction = false;
   objectTypeNames.push_back("MathElement");   
   theWrapperMap = NULL;
   
   Real rval;
   if (GmatStringUtil::ToReal(name, &rval))
   {
      SetRealValue(rval);
      SetNumberFlag(true);
   }
   else
   {
      SetRefObjectName(Gmat::PARAMETER, name);
   }
   
   #ifdef DEBUG_MATH_ELEMENT
   MessageInterface::ShowMessage
      ("MathElement::MathElement() created\n");
   #endif
}


//------------------------------------------------------------------------------
//  ~MathElement(void)
//------------------------------------------------------------------------------
/**
 * Destroys the MathElement object (destructor).
 */
//------------------------------------------------------------------------------
MathElement::~MathElement()
{
}


//------------------------------------------------------------------------------
//  MathElement(const MathElement &me)
//------------------------------------------------------------------------------
/**
 * Constructs the MathElement object (copy constructor).
 * 
 * @param <me> Object that is copied
 */
//------------------------------------------------------------------------------
MathElement::MathElement(const MathElement &me) :
   MathNode      (me),
   refObject     (me.refObject),
   refObjectName (me.refObjectName),
   refObjectType (me.refObjectType)
{
}


//------------------------------------------------------------------------------
//  MathElement& operator=(const MathElement &me)
//------------------------------------------------------------------------------
/**
 * Sets one MathElement object to match another (assignment operator).
 * 
 * @param <me> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
MathElement& MathElement::operator=(const MathElement &me)
{
   if (this == &me)
      return *this;
   
   MathNode::operator=(me);
   refObject = me.refObject;
   refObjectName = me.refObjectName;
   refObjectType = me.refObjectType;
   
   return *this;
}


//------------------------------------------------------------------------------
// void SetMathWrappers(std::map<std::string, ElementWrapper*> *wrapperMap)
//------------------------------------------------------------------------------
void MathElement::SetMathWrappers(std::map<std::string, ElementWrapper*> *wrapperMap)
{
   theWrapperMap = wrapperMap;
   
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("MathElement::SetMathWrappers() theWrapperMap=%p, Node=%s\n",
       theWrapperMap, GetName().c_str());
   #endif
}


// Inherited MathElement methods
//------------------------------------------------------------------------------
// virtual void SetMatrixValue(const Rmatrix &mat)
//------------------------------------------------------------------------------
void MathElement::SetMatrixValue(const Rmatrix &mat)
{
   // if we can set matrix value, it is not a parameter
   isNumber = true;
   refObjectName = "";

   MathNode::SetMatrixValue(mat);
}


//------------------------------------------------------------------------------
// void GetOutputInfo()
//------------------------------------------------------------------------------
void MathElement::GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount)
{
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("MathElement::GetOutputInfo() this=<%p><%s><%s>\n", this, GetTypeName().c_str(),
       GetName().c_str());
   #endif
   
   type = elementType;
   rowCount = 1;
   colCount = 1;
   
   #ifdef DEBUG_INPUT_OUTPUT
   MessageInterface::ShowMessage
      ("MathElement::GetOutputInfo() isNumber=%d, isFunctionInput=%d, refObjectName=%s\n",
       isNumber, isFunctionInput, refObjectName.c_str());
   #endif
   
   // if this this function input, just return
   if (isFunctionInput)
      return;
   
   if (refObjectName == "")
   {
      if (type == Gmat::RMATRIX_TYPE)
      {
         rowCount = matrix.GetNumRows();
         colCount = matrix.GetNumColumns();
      }
   }
   else
   {
      #ifdef DEBUG_INPUT_OUTPUT
      MessageInterface::ShowMessage
         ("MathElement::GetOutputInfo() %s is a parameter or an object, "
          "refObject=<%p><%s>'%s'\n", GetName().c_str(), refObject,
          refObject ? refObject->GetTypeName().c_str() : "NULL",
          refObject ? refObject->GetName().c_str() : "NULL");
      #endif
      
      if (refObject)
      {
         // Check if ref object is Parameter object
         if (!refObject->IsOfType(Gmat::PARAMETER))
            throw MathException("The math element \"" + GetName() + "\" of type \"" +
                                refObject->GetTypeName() + "\" is invalid");
         
         type = refObject->GetReturnType();
         #ifdef DEBUG_INPUT_OUTPUT
         MessageInterface::ShowMessage("   object return type = %d\n", type);
         #endif
         
         if (type == Gmat::RMATRIX_TYPE)
         {
            // Handle array index
            std::string newName, rowStr, colStr;
            GmatStringUtil::GetArrayIndexVar(refObjectName, rowStr, colStr, newName, "()");
            #ifdef DEBUG_INPUT_OUTPUT
            MessageInterface::ShowMessage
               ("   rowStr='%s', colStr='%s', newName=%s\n", rowStr.c_str(),
                colStr.c_str(), newName.c_str());
            #endif
            
            // Are we going to allow row/column slicing for future? such as:
            // a(:,1)   -> first column vector
            // a(1,:)   -> first row vector
            // a(1:2,1) -> first and second row, fisrt column vector
            
            bool wholeArray = false;
            if (rowStr == "-1" && colStr == "-1")
               wholeArray = true;
            
            // if whole array, row and colum count is actual dimension
            if (wholeArray)
            {
               rowCount = ((Array*)refObject)->GetRowCount();
               colCount = ((Array*)refObject)->GetColCount();
            }
            else
            {
               type = Gmat::REAL_TYPE;
               rowCount = 1;
               colCount = 1;
            }
         }
         
         #ifdef DEBUG_INPUT_OUTPUT
         MessageInterface::ShowMessage
            ("MathElement::GetOutputInfo() type=%d, row=%d, col=%d\n", type,
             rowCount, colCount);
         #endif
      }
      else
      {
         throw MathException("The output parameter: " + GetName() + " is NULL");
      }
   }
}


//------------------------------------------------------------------------------
// bool ValidateInputs()
//------------------------------------------------------------------------------
/**
 * This method always returns true if the node was sucessfully initialized.
 */
 //------------------------------------------------------------------------------
bool MathElement::ValidateInputs()
{
   // The function inputs shoule be handled by the FunctionRunner
   if (isFunctionInput)
      return false;
   
   if (elementType == Gmat::REAL_TYPE && refObjectName == "")
      return true;
   
   if (refObject)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// Real Evaluate()
//------------------------------------------------------------------------------
Real MathElement::Evaluate()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("MathElement::Evaluate() this='%s', refObjectName='%s', refObject=<%p>, "
       "elementType=%d\n", GetName().c_str(), refObjectName.c_str(), refObject, elementType);
   #endif
   
   // If this MathElement is function Input, just return since it is handled in
   // the FunctionRunner
   
   if (isFunctionInput)
      throw MathException("MathElement::Evaluate() Function input should "
                          "not be handled here");
   
   if (refObject)
   {
      #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage
         ("   refObject=<%p><%p>'%s'\n", refObject, refObject->GetTypeName().c_str(),
          refObject->GetName().c_str());
      #endif
      
      ElementWrapper *wrapper = FindWrapper(refObjectName);
      
      if (elementType == Gmat::REAL_TYPE || elementType == Gmat::RMATRIX_TYPE)
      {
         #ifdef DEBUG_EVALUATE
         MessageInterface::ShowMessage
            ("   wrapper type=%d, desc='%s'\n", wrapper->GetWrapperType(),
             wrapper->GetDescription().c_str());
         #endif
         ///@note ArrayWrapper::EvaluateReal() returns 1x1 matrix as real number
         realValue = wrapper->EvaluateReal();
      }
      else
      {
         throw MathException
            ("MathElement::Evaluate() Cannot Evaluate MathElementType of \"" +
             refObjectName + "\"");
      }
      
      #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage
         ("MathElement::Evaluate() It's a parameter: %s realValue = %f\n",
          refObject->GetName().c_str(), realValue);
      #endif
      
      return realValue;
   }
   else
   {
      #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage
         ("MathElement::Evaluate() It's a number: realValue = %f\n", realValue);
      #endif
      
      return realValue;
   }
}


//------------------------------------------------------------------------------
// bool MatrixEvaluate()
//------------------------------------------------------------------------------
Rmatrix MathElement::MatrixEvaluate()
{
   #ifdef DEBUG_EVALUATE
   MessageInterface::ShowMessage
      ("MathElement::MatrixEvaluate() this='%s', refObjectName='%s', refObject=<%p>, "
       "elementType=%d\n", GetName().c_str(), refObjectName.c_str(), refObject, elementType);
   #endif
   
   // If this MathElement is function Input, just return since it is handled in
   // the FunctionRunner
   
   if (isFunctionInput)
      throw MathException("MathElement::MatrixEvaluate() Function input should "
                          "not be handled here");
   
   if (elementType == Gmat::RMATRIX_TYPE)
   {
      if (refObject)
      {
         #ifdef DEBUG_EVALUATE
         Rmatrix rmat = refObject->GetRmatrix();
         MessageInterface::ShowMessage
            ("MathElement::MatrixEvaluate() It's an Array: %s matVal =\n%s\n",
             refObject->GetName().c_str(), rmat.ToString().c_str());
         #endif
         
         ElementWrapper *wrapper = FindWrapper(refObjectName);
         return wrapper->EvaluateArray();
      }
      else
      {
         #ifdef DEBUG_EVALUATE
         MessageInterface::ShowMessage
            ("MathElement::MatrixEvaluate() It's a Rmatrix. matVal =\n%s\n",
             matrix.ToString().c_str());
         #endif
         
         return matrix;
      }
   }
   else
   {
      Real rval = Evaluate();
      
      #ifdef DEBUG_EVALUATE
      MessageInterface::ShowMessage
         ("MathElement::MatrixEvaluate() It's a number: rval = %f\n", rval);
      #endif
      
      // Set matrix 1x1 and return
      Rmatrix rmat(1, 1, rval);
      return rmat;
      //throw MathException("MathElement::MatrixEvaluate() Invalid matrix");
   }
}


//------------------------------------------------------------------------------
// bool SetChildren(MathNode *leftChild, MathNode *rightChild)
//------------------------------------------------------------------------------
bool MathElement::SetChildren(MathNode *leftChild, MathNode *rightChild)
{
   throw MathException("SetChildren() is not valid for MathElement");
}


//------------------------------------------------------------------------------
// MathNode* GetLeft()
//------------------------------------------------------------------------------
MathNode* MathElement::GetLeft()
{
   return NULL;
}


//------------------------------------------------------------------------------
// MathNode* GetRight()
//------------------------------------------------------------------------------
MathNode* MathElement::GetRight()
{
   return NULL;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/*
 * Renames referenced objects
 *
 * @param <type> type of the reference object.
 * @param <oldName> old name of the reference object.
 * @param <newName> new name of the reference object.
 *
 * @return always true to indicate RenameRefObject() was implemented.
 */
//---------------------------------------------------------------------------
bool MathElement::RenameRefObject(const UnsignedInt type,
                                  const std::string &oldName,
                                  const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("MathElement::RenameRefObject() refObjectName=%s, type=%d, oldName=<%s>, "
       "newName=<%s>\n", refObjectName.c_str(), type, oldName.c_str(), newName.c_str());
   #endif
   
   if (refObjectName.find(oldName) != refObjectName.npos)
   {
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage("   old refObjectName=%s\n", refObjectName.c_str());
      #endif
         
      refObjectName = GmatStringUtil::ReplaceName(refObjectName, oldName, newName);
         
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage("   new refObjectName=%s\n", refObjectName.c_str());
      #endif
   }
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("   now checking %d objects wrapperObjectNames\n", wrapperObjectNames.size());
   #endif
   for (UnsignedInt i = 0; i < wrapperObjectNames.size(); i++)
   {
      std::string wrapperObjName = wrapperObjectNames[i];
      if (wrapperObjName.find(oldName) != wrapperObjName.npos)
      {
         wrapperObjName = GmatStringUtil::ReplaceName(wrapperObjName, oldName, newName);
         wrapperObjectNames[i] = wrapperObjName;
      }
   }
   
   if (theWrapperMap == NULL)
   {
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("MathElement::RenameRefObject() theWrapperMap is NULL, returning true\n");
      #endif
      return true;
   }
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("   now checking %d objects in theWrapperMap\n", theWrapperMap->size());
   #endif
   // Rename wrapper objects
   std::map<std::string, ElementWrapper *> tempMap;
   std::map<std::string, ElementWrapper *>::iterator ewi;
   bool needSwap = false;
   
   for (ewi = theWrapperMap->begin(); ewi != theWrapperMap->end(); ++ewi)
   {
      std::string name = ewi->first;
      
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("   old name=<%s>, wrapper=%p\n", (ewi->first).c_str(), ewi->second);
      #endif
      
      if (name == oldName)
      {
         name = newName;
         tempMap[name] = ewi->second;
         ewi->second->RenameObject(oldName, newName);
         needSwap = true;
      }
      else if (name.find(oldName) != name.npos)
      {
         name = GmatStringUtil::ReplaceName(name, oldName, newName);
         tempMap[name] = ewi->second;
         ewi->second->RenameObject(oldName, newName);
         needSwap = true;
      }
      else
      {
         // add other names into tempMap
         tempMap[name] = ewi->second;
      }
   }
   
   // If anything changed in the wrapper, swap the map
   if (needSwap)
      theWrapperMap->swap(tempMap);
   
   #ifdef DEBUG_RENAME
   for (ewi = theWrapperMap->begin(); ewi != theWrapperMap->end(); ++ewi)
      MessageInterface::ShowMessage
         ("   new name=<%s>, wrapper=%p\n", (ewi->first).c_str(), ewi->second);
   MessageInterface::ShowMessage
      ("MathElement::RenameRefObject() refObjectName=%s\n", refObjectName.c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MathElement.
 *
 * @return clone of the MathElement.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MathElement::Clone(void) const
{
   return (new MathElement(*this));
}

//---------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//---------------------------------------------------------------------------
/**
 * Returns the reference object pointer.
 *
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return reference object pointer.
 */
 //---------------------------------------------------------------------------
GmatBase* MathElement::GetRefObject(const UnsignedInt type,
                                    const std::string &name)
{
   switch (type)
   {
      case Gmat::PARAMETER:
         return refObject;
         
      default:
         break;
   }

   return GmatBase::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets the reference object pointer.
 *
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return true if successful false otherwise.
 */
 //---------------------------------------------------------------------------
bool MathElement::SetRefObject(GmatBase *obj, const UnsignedInt type,
                               const std::string &name)
{
   switch (type)
   {
      case Gmat::PARAMETER:
      {
         SetWrapperObject(obj, name);
         return true;
      }
      default:
         break;
   }    
   
   return GmatBase::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * This method returns a name of the referenced objects.
 *
 * @return the name of the object of the requested type.
 */
 //------------------------------------------------------------------------------
std::string MathElement::GetRefObjectName(const UnsignedInt type) const
{
   switch (type)
   {
      case Gmat::PARAMETER:
         return refObjectName;
         
      default:
         break;
   }
   
   return GmatBase::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets the name of the referenced objects.
 *
 * @return true when referenced object name is set sucessfully otherwise false.
 */
 //------------------------------------------------------------------------------
bool MathElement::SetRefObjectName(const UnsignedInt type, const std::string &name)
{
   #ifdef DEBUG_MATH_ELEMENT
   MessageInterface::ShowMessage
      ("MathElement::SetRefObjectName() name=%s\n", name.c_str());
   #endif
   
   switch (type)
   {
      case Gmat::PARAMETER:
         refObjectName = name;
         SetWrapperObjectNames(name);
         return true;
         
      default:
         break;
   }
   
   return GmatBase::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Returns the names of the reference object.
 *
 * @param <type> reference object type.  Gmat::UnknownObject returns all of the
 *               ref objects.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& MathElement::GetRefObjectNameArray(const UnsignedInt type)
{
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("MathElement::GetRefObjectNameArray() '%s' entered, type=%d\n",
       GetName().c_str(), type);
   MessageInterface::ShowMessage
      ("There are %d wrapper object names:\n", wrapperObjectNames.size());
   for (UnsignedInt i = 0; i < wrapperObjectNames.size(); i++)
      MessageInterface::ShowMessage("   [%d] %s\n", i, wrapperObjectNames[i].c_str());
   #endif
   
   if ((type == Gmat::PARAMETER) || (type == Gmat::UNKNOWN_OBJECT))
   {      
      #ifdef DEBUG_WRAPPERS
      MessageInterface::ShowMessage
         ("MathElement::GetRefObjectNameArray() returning %d wrapper object names:\n",
          wrapperObjectNames.size());
      for (UnsignedInt i = 0; i < wrapperObjectNames.size(); i++)
         MessageInterface::ShowMessage("   [%d] %s\n", i, wrapperObjectNames[i].c_str());
      #endif
      
      return wrapperObjectNames;
   }
   
   return GmatBase::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// void SetWrapperObjectNames(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Sets ElementWrapper object names. This will also set multiple input arguments
 * for GmatFunction.
 *
 * @param  name  The name or name list separated by comma
 */
//------------------------------------------------------------------------------
void MathElement::SetWrapperObjectNames(const std::string &name)
{
   wrapperObjectNames = GmatStringUtil::SeparateBy(refObjectName, ",", true);
   
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("MathElement::SetWrapperObjectNames() wrapperObjectNames are:\n");
   for (UnsignedInt i=0; i<wrapperObjectNames.size(); i++)
      MessageInterface::ShowMessage("   '%s'\n", wrapperObjectNames[i].c_str());
   #endif
}


//------------------------------------------------------------------------------
// void SetWrapperObject(GmatBase *obj, const std::string &name)
//------------------------------------------------------------------------------
/*
 * Sets ElementWrapper object. This will also set multiple input arguments
 * for GmatFunction.
 *
 * @param  obj  The object pointer
 */
//------------------------------------------------------------------------------
void MathElement::SetWrapperObject(GmatBase *obj, const std::string &name)
{
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("MathElement::SetWrapperObject() obj=<%p>, name='%s'\n", obj, name.c_str());
   #endif
   
   refObject = (Parameter*) obj;
   refObjectType = refObject->GetTypeName().c_str();
   
   // go through wrapperObjectNames
   for (UnsignedInt i=0; i<wrapperObjectNames.size(); i++)
   {
      if (name == wrapperObjectNames[i])
      {
         #ifdef DEBUG_WRAPPERS
         MessageInterface::ShowMessage
            ("   wrapperName = '%s'\n", wrapperObjectNames[i].c_str());
         #endif
         
         // Handle array index
         Integer row, col;
         std::string newName;
         GmatStringUtil::GetArrayIndex(wrapperObjectNames[i], row, col, newName);
         
         // Check if name is the same
         if (newName != name)
            throw MathException
               ("MathElement::SetRefObject() Cannot find parameter name:" + name);
         
         if (refObjectType == "Array")
         {
            Array *arr = (Array*)refObject;     
            elementType = Gmat::RMATRIX_TYPE;
            Integer theRowCount = arr->GetRowCount();
            Integer theColCount = arr->GetColCount();
            
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("MathElement::SetRefObject() elementType=%d, theRowCount=%d, "
                "theColCount=%d\n", elementType, theRowCount, theColCount);
            #endif
            
            if (!matrix.IsSized())
               matrix.SetSize(theRowCount, theColCount);
            else
            {
               #ifdef DEBUG_WRAPPERS
               MessageInterface::ShowMessage
                  ("MathElement::SetRefObject() matrix already sized. "
                   "matrix.size=%d, %d\n", matrix.GetNumRows(),
                   matrix.GetNumColumns());
               #endif
            }
            
            matrix = arr->GetRmatrix(); // initial value
            
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("MathElement::SetRefObject() name=%s, matrix=\n%s\n", name.c_str(),
                matrix.ToString().c_str());
            #endif
            
         }
         else if (refObject->GetReturnType() == Gmat::REAL_TYPE)
         {
            elementType = Gmat::REAL_TYPE;
            realValue = refObject->GetReal(); // initial value
            
            #ifdef DEBUG_WRAPPERS
            MessageInterface::ShowMessage
               ("MathElement::SetRefObject() name=%s, elementType=%d, "
                "realValue=%f\n", GetName().c_str(), elementType, realValue);
            #endif
         }
      }
   }
}


//------------------------------------------------------------------------------
// ElementWrapper* FindWrapper(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Finds ElementWrapper pointer from the map by given name
 */
//------------------------------------------------------------------------------
ElementWrapper* MathElement::FindWrapper(const std::string &name)
{
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("MathElement::FindWrapper() node='%s', name='%s', refObject=<%p>, "
       "elementType=%d, theWrapperMap=<%p>\n", GetName().c_str(), name.c_str(),
       refObject, elementType, theWrapperMap);
   #endif
   
   if (theWrapperMap == NULL)
      throw MathException("MathElement::FindWrapper() theWrapperMap is NULL");
   
   #ifdef DEBUG_WRAPPERS
   std::map<std::string, ElementWrapper *>::iterator ewi;
   for (ewi = theWrapperMap->begin(); ewi != theWrapperMap->end(); ++ewi)
      MessageInterface::ShowMessage
         ("   wrapperName='%s', wrapper=<%p>, wrapperType=%d, wrapperDesc='%s'\n",
          (ewi->first).c_str(), ewi->second, (ewi->second)->GetWrapperType(),
          (ewi->second)->GetDescription().c_str());
   #endif
   
   if (theWrapperMap->find(name) == theWrapperMap->end())
      throw MathException("MathElement::FindWrapper() Cannot find \"" + name +
                          "\" from theWrapperMap");
   
   ElementWrapper *wrapper = (*theWrapperMap)[name];
   if (wrapper == NULL)
      throw MathException
         ("MathElement::FindWrapper() The ElementWrapper of \"" + name + "\" is NULL");
   
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("MathElement::FindWrapper() returning wrapper <%p>\n", wrapper);
   #endif
   
   return wrapper;
}

