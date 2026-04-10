//$Id$
//------------------------------------------------------------------------------
//                              Covariance
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
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/10/28
//
/**
 * Definition of the covariance class
 */
//------------------------------------------------------------------------------


#ifndef Covariance_hpp
#define Covariance_hpp

#include "Rmatrix.hpp"
#include "gmatdefs.hpp"

/**
 * Helper class used to represent an object's covariance data
 */
class GMAT_API Covariance
{
public:
   Covariance(GmatBase *owner = NULL);
   virtual ~Covariance();
   Covariance(const Covariance &cov);
   Covariance& operator=(const Covariance &cov);

   // Overload the parentheses operators
   Real& operator()(const Integer r, const Integer c);
   Real  operator()(const Integer r, const Integer c) const;

   void AddCovarianceElement(const std::string &name, GmatBase* owner);
   bool ConstructLHS(const std::string& lhs = "");
   bool ConstructRHS(const std::string& rhs);
   bool ConstructRHS(Rvector diagonal, Integer start = 0);
   bool ConstructRHS(Rmatrix data, Integer start = 0);
   bool FillMatrix(const Rmatrix& rhs, bool overrideAndFill);
   void SetDimension(Integer size);
   Integer GetDimension();

   Rmatrix *GetCovariance();
   Rmatrix *GetCovariance(const Integer forParameterID);
   
   Integer   GetElementIndex(const std::string elementName) const;
   Integer   GetElementIndex(const Integer forParameterID) const;

   Integer   GetElementID(const std::string elementName) const;
   
   Integer   GetElementSize(const std::string elementName) const;
   Integer   GetElementSize(const Integer forParameterID) const;
   bool      SetElementSize(const std::string elementName, const Integer elementSize);
   bool      SetElementSize(const Integer forParameterID, const Integer elementSize);

   GmatBase* GetElementOwner(const std::string elementName);
   GmatBase* GetElementOwner(const Integer forParameterID);

   Integer  GetSubMatrixLocationStart(const Integer forParameterID) const;
   Integer  GetSubMatrixLocationStart(const std::string paramName) const;

   bool     IncreasingElementSize(const std::string elementName, const Integer atRow, const Integer increasingSize);
   bool     IncreasingElementSize(const Integer forParameterID, const Integer atRow, const Integer increasingSize);
   bool     DecreasingElementSize(const std::string elementName, const Integer atRow, const Integer decreasingSize);
   bool     DecreasingElementSize(const Integer forParameterID, const Integer atRow, const Integer decreasingSize);

   void     ShowContent();

protected:
   GmatBase     *covarianceOwner;
   
   //Rmatrix      theCovariance;
   Rmatrix      *theCovariance;

   Rmatrix      *subCovariance;
   StringArray  elementNames;
   IntegerArray elementIDs;
   IntegerArray elementSizes;
   ObjectArray  elementOwners;

   Integer dimension;

   void PrepareMatrix();
};

#endif /* Covariance_hpp */
