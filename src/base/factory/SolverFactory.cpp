//$Id$
//------------------------------------------------------------------------------
//                            SolverFactory
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
// number NNG04CC06P
//
// Author: Darrel Conway
// Created: 2004/02/02  (Groundhog Day)
//
/**
 *  Implementation code for the SolverFactory class, responsible for creating
 *  targeters, optimizers, and other parameteric scanning objects.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SolverFactory.hpp"

// Headers for the supported Solvers
#include "DifferentialCorrector.hpp"
//#include "QuasiNewton.hpp"
//#include "Broyden.hpp"
//#include "ParametricScanner.hpp"
//#include "MonteCarlo.hpp"

#include "MessageInterface.hpp"  // temporary



//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Returns a GmatBase object pointer to a new object. 
 *
 * @param <ofType>   specific type of object to create.
 * @param <withName> name to give to the newly created object.
 *
 * @return pointer to a new object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of the requested type.
 */
//------------------------------------------------------------------------------
GmatBase* SolverFactory::CreateObject(const std::string &ofType,
                                      const std::string &withName)
{
   return CreateSolver(ofType, withName);
}

//------------------------------------------------------------------------------
//  Solver* CreateSolver(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Solver class. 
 *
 * @param <ofType> type of Solver object to create and return.
 * @param <withName> the name for the newly-created Solver object.
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
Solver* SolverFactory::CreateSolver(const std::string &ofType,
                                    const std::string &withName)
{
   if (ofType == "DifferentialCorrector")
      return new DifferentialCorrector(withName);
     
   //if (ofType == "QuasiNewton") 
   //   return new QuasiNewton(withName);
   // else if (ofType == "Broyden")
   //   return new Broyden(withName);
   //else if (ofType == "ParametricScanner")
   //   return new ParametricScanner(withName);
   //else if (ofType == "MonteCarlo")
   //   return new MonteCarlo(withName);
    
   return NULL;
}


//------------------------------------------------------------------------------
//  SolverFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SolverFactory. 
 * (default constructor)
 */
//------------------------------------------------------------------------------
SolverFactory::SolverFactory() :
    Factory     (Gmat::SOLVER)
{
   if (creatables.empty())
   {
      //creatables.push_back("QuasiNewton");
      creatables.push_back("DifferentialCorrector");
      //creatables.push_back("Broyden");
      //creatables.push_back("ParametricScanner");
      //creatables.push_back("MonteCarlo");
   }
   GmatType::RegisterType(Gmat::SOLVER, "Solver");
}

//------------------------------------------------------------------------------
//  SolverFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SolverFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
SolverFactory::SolverFactory(StringArray createList) :
Factory(createList, Gmat::SOLVER)
{
}


//------------------------------------------------------------------------------
//  SolverFactory(const SolverFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SolverFactory.  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
SolverFactory::SolverFactory(const SolverFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      //creatables.push_back("QuasiNewton");
      creatables.push_back("DifferentialCorrector");
      //creatables.push_back("Broyden");
      //creatables.push_back("ParametricScanner");
      //creatables.push_back("MonteCarlo");
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * SolverFactory operator for the SolverFactory base class.
 *
 * @param <fact> the SolverFactory object that is copied.
 *
 * @return "this" SolverFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
SolverFactory& SolverFactory::operator=(const SolverFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}
    

//------------------------------------------------------------------------------
// ~SolverFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the SolverFactory base class.
 */
//------------------------------------------------------------------------------
SolverFactory::~SolverFactory()
{
}


//------------------------------------------------------------------------------
// bool DoesObjectTypeMatchSubtype(const std::string &theType,
//       const std::string &theSubtype)
//------------------------------------------------------------------------------
/**
 * Checks if a creatable solver type matches a subtype.
 *
 * @param theType The script identifier for the object type
 * @param theSubtype The subtype being checked
 *
 * @return true if the scripted type and subtype match, false if the type does
 *         not match the subtype
 */
//------------------------------------------------------------------------------
bool SolverFactory::DoesObjectTypeMatchSubtype(const std::string &theType,
      const std::string &theSubtype)
{
   bool retval = false;

   if ((theType == "Boundary Value Solvers") || (theType == "Targeter"))
   {
      if (theSubtype == "DifferentialCorrector")
         retval = true;
   }

   return retval;
}



//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

