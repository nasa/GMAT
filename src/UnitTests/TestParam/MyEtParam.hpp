//------------------------------------------------------------------------------
//                              MyEtParam
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
// Author: Linda Jun
// Created: 2003/09/22
//
/**
 * Declares my elapsed time parameter operations.
 */
//------------------------------------------------------------------------------
#ifndef MyEtParam_hpp
#define MyEtParam_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "ElapsedTimeParam.hpp"
#include "A1Mjd.hpp"

class GMAT_API MyEtParam : public ElapsedTimeParam
{
public:

   MyEtParam(const std::string &name, const std::string &desc,
             const ElapsedTime &elapsedTime, const Real anotherTime);
   MyEtParam(const std::string &name, const std::string &desc,
             const Real val, const Real anotherTime);
   MyEtParam(const MyEtParam &param);
   const MyEtParam& operator= (const MyEtParam &param); 
   virtual ~MyEtParam();

   Real GetAnotherTimeValue() const;

   // The inherited methods from GmatBase
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual std::string GetParameterText(const Integer id);
   virtual Integer GetParameterID(const std::string str);
   virtual Real GetRealParameter(const Integer id);
   virtual Real SetRealParameter(const Integer id, const Real value);

protected:

   Real mAnotherTime;

   enum
   {
      ANOTHER_ELAPSED_TIME = ElapsedTimeParamCount,
      MyEtParamCount
   };

   static const std::string PARAMETER_TEXT[MyEtParamCount];
   static const Gmat::ParameterType PARAMETER_TYPE[MyEtParamCount];

private:
};

#endif // MyEtParam_hpp
