//$Id$
//------------------------------------------------------------------------------
//                                  Array
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
// Created: 2004/09/20
//
/**
 * Declares Array class which handles one or two dimentional array.
 */
//------------------------------------------------------------------------------
#ifndef Array_hpp
#define Array_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"

class GMAT_API Array : public Parameter
{
public:
   
   Array(const std::string &name = "", const std::string &desc = "",
         const std::string &unit = "");
   Array(const Array &copy);
   Array& operator= (const Array& right);
   virtual ~Array();
   
   bool operator==(const Array &right) const;
   bool operator!=(const Array &right) const;
   
   bool IsSized() { return mSizeSet; }
   bool SetSize(const Integer row, const Integer col, bool zeroElements = true);
   void GetSize(Integer &row, Integer &col) { row = mNumRows; col = mNumCols; }
   Integer GetRowCount() { return mNumRows; }
   Integer GetColCount() { return mNumCols; }
   
   // methods inherited from Parameter
   virtual void SetRmatrix(const Rmatrix &mat);
   virtual const Rmatrix& GetRmatrix() const { return mRmatValue; }
   virtual const Rmatrix& EvaluateRmatrix() { return mRmatValue; } /// assumes it has only numbers
   virtual std::string ToString();
   virtual const std::string* GetParameterList() const;
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   virtual void Copy(const GmatBase*);
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual bool IsParameterReadOnly(const Integer id) const;
   virtual bool IsParameterCommandModeSettable(const Integer id) const;
   virtual bool IsParameterCloaked(const Integer id) const;

   virtual Integer GetIntegerParameter(const Integer id) const;
   virtual Integer GetIntegerParameter(const std::string &label) const;
   virtual Integer SetIntegerParameter(const Integer id, const Integer value);
   virtual Integer SetIntegerParameter(const std::string &label,
                                       const Integer value);
   
   virtual const Rmatrix& GetRmatrixParameter(const Integer id) const;
   virtual const Rmatrix& GetRmatrixParameter(const std::string &label) const;
   virtual const Rmatrix& SetRmatrixParameter(const Integer id,
                                              const Rmatrix &value);
   virtual const Rmatrix& SetRmatrixParameter(const std::string &label,
                                              const Rmatrix &value);
   
   virtual Real GetRealParameter(const Integer id, const Integer index) const;
   virtual Real GetRealParameter(const std::string &label,
                                 const Integer index) const;
   
   virtual Real GetRealParameter(const Integer id, const Integer row,
                                 const Integer col) const;
   virtual Real GetRealParameter(const std::string &label, const Integer row,
                                 const Integer col) const;
   
   virtual Real SetRealParameter(const Integer id, const Real value,
                                 const Integer row, const Integer col);
   virtual Real SetRealParameter(const std::string &label,
                                 const Real value, const Integer row,
                                 const Integer col);
   
   virtual Rvector GetRvectorParameter(const Integer id,
                                       const Integer index) const;
   virtual Rvector GetRvectorParameter(const std::string &label,
                                       const Integer index) const;
   
   virtual const Rvector& SetRvectorParameter(const Integer id,
                                              const Rvector &value,
                                              const Integer index);   
   virtual const Rvector& SetRvectorParameter(const std::string &label,
                                              const Rvector &value,
                                              const Integer index);
   
   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label, const char *value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);
   
   virtual const std::string&
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");
protected:

   Integer mNumRows;
   Integer mNumCols;
   Integer mInitialValueType; // 1 = number, 2 = Variable or other Array
   bool mSizeSet;
   Rmatrix mRmatValue;
   std::map<std::string, std::string> initialValueMap;
   
   std::string GetArrayDefString() const;
   std::string GetInitialValueString(const std::string &prefix = "");
   
   enum
   {
      NUM_ROWS = ParameterParamCount,
      NUM_COLS,
      RMAT_VALUE,
      SINGLE_VALUE,
      ROW_VALUE,
      COL_VALUE,
      INITIAL_VALUE,
      INITIAL_VALUE_TYPE,
      ArrayParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[ArrayParamCount - ParameterParamCount];
   static const std::string
      PARAMETER_TEXT[ArrayParamCount - ParameterParamCount];
    
private:

};
#endif // Array_hpp
