//$Id$
//------------------------------------------------------------------------------
//                           SolverData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Oct 4, 2013
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef SolverData_hpp
#define SolverData_hpp

#include "newparameter_defs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Solver.hpp"


/**
 * Data class for Solver parameters
 */
class NEW_PARAMETER_API SolverData : public RefData
{
public:
   SolverData();
   virtual ~SolverData();
   SolverData(const SolverData& sd);
   SolverData& operator=(const SolverData& sd);

   Real GetSolverReal(const std::string &str);
   std::string GetSolverString(const std::string &str);

   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;

   const static Real SOLVER_REAL_UNDEFINED;// = -9876543210.1234;
   const static std::string SOLVER_STRING_UNDEFINED;

protected:
   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(UnsignedInt type);
   virtual bool AddRefObject(const UnsignedInt type,
                             const std::string &name, GmatBase *obj = NULL,
                             bool replaceName = false);
   void GetSolver();


   Solver *mSolver;
   std::vector<Solver*> mSolvers;
   std::vector<std::string> solverStatuses;

   enum
   {
      SOLVER = 0,
      SolverDataObjectCount
   };

   static const std::string VALID_OBJECT_TYPE_LIST[SolverDataObjectCount];
};

#endif /* SolverData_hpp */
