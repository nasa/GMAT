//$Id: ExtendedKalmanFilterFactory.cpp 1398 2016-12-19 20:39:37Z  $
//------------------------------------------------------------------------------
//                            ExtendedKalmanFilterFactory
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
// Developed by Dr. David E. Gaylor, Thinking Systems, Inc.
//
// Author: David E. Gaylor
// Created: 2016/12/19
//
/**
 *  Implementation code for the ExtendedKalmanFilterFactory class, responsible
 *  for creating EKF objects.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "ExtendedKalmanFilterFactory.hpp"
#include "MessageInterface.hpp"

// Here are the supported leaf classes
#include "ExtendedKalmanFilter.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested ExtendedKalmanFilter class.
 *
 * @param ofType type of ExtendedKalmanFilter object to create and return.
 * @param withName the name for the newly-created ExtendedKalmanFilter object.
 *
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
GmatBase* ExtendedKalmanFilterFactory::CreateObject(const std::string &ofType,
                                         const std::string &withName)
{
   return CreateSolver(ofType, withName);
}

//------------------------------------------------------------------------------
//  Solver* CreateSolver(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested ExtendedKalmanFilter class.
 *
 * @param ofType type of ExtendedKalmanFilter object to create and return.
 * @param withName the name for the newly-created ExtendedKalmanFilter object.
 *
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
Solver* ExtendedKalmanFilterFactory::CreateSolver(const std::string &ofType,
                                       const std::string &withName)
{
   // Not sure that we want to break these guys out like this; left in in case
   //it's desired

   //   return CreateExtendedKalmanFilter(ofType, withName);
   //}
   //
   ////------------------------------------------------------------------------------
   ////  ExtendedKalmanFilter* CreateExtendedKalmanFilter(const std::string &ofType, const std::string &withName)
   ////------------------------------------------------------------------------------
   ///**
   // * This method creates and returns an object of the requested ExtendedKalmanFilter class.
   // *
   // * @param ofType Type of ExtendedKalmanFilter object to create and return.
   // * @param withName The name for the newly-created ExtendedKalmanFilter object.
   // *
   // * @return A pointer to the created object.
   // */
   ////------------------------------------------------------------------------------
   //ExtendedKalmanFilter* ExtendedKalmanFilterFactory::CreateExtendedKalmanFilter(const std::string &ofType,
   //                                    const std::string &withName)
   //{
   //MessageInterface::ShowMessage("ExtendedKalmanFilterFactory is creating a %s named %s\n",
   //      ofType.c_str(), withName.c_str());


   if (ofType == "ExtendedKalmanFilter")
      return new ExtendedKalmanFilter(withName);

   // Here's a list of other potential estimators:
   //if (ofType == "BatchLeastSquares")
   //   return new BatchLeastSquares(withName);
   //else if (ofType == "SequentialLeastSquares")
   //   return new SequentialLeastSquares(withName);
   //else if (ofType == "SimpleLinearKalmanFilter")
   //   return new SimpleLinearKalmanFilter(withName);
   //else if (ofType == "LinearKalmanFilter")
   //   return new LinearKalmanFilter(withName);
   //else if (ofType == "ExtendedKalmanFilter")
   //   return new ExtendedKalmanFilter(withName);
   //else if (ofType == "UnscentedKalmanFilter")
   //   return new UnscentedKalmanFilter(withName);
   //else if (ofType == "ExtendedConsiderFilter")
   //   return new ExtendedConsiderFilter(withName);
   //else if (ofType == "UnscentedConsiderFilter")
   //   return new UnscentedConsiderFilter(withName);
   //else if (ofType == "ExtendedNeglectFilter")
   //   return new ExtendedNeglectFilter(withName);
   //else if (ofType == "UnscentedNeglectFilter")
   //   return new UnscentedNeglectFilter(withName);
   //else if (ofType == "CentralDifferenceKalmanFilter")
   //   return new CentralDifferenceKalmanFilter(withName);
   //else if (ofType == "SquareRootInformationFilter")
   //   return new SquareRootInformationFilter(withName);

   return NULL;
}


//------------------------------------------------------------------------------
//  ExtendedKalmanFilterFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ExtendedKalmanFilterFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
ExtendedKalmanFilterFactory::ExtendedKalmanFilterFactory() :
   Factory     (Gmat::SOLVER)

{
   if (creatables.empty())
   {

      creatables.push_back("ExtendedKalmanFilter");

      //creatables.push_back("BatchLeastSquares");
      //creatables.push_back("SequentialLeastSquares");
      //creatables.push_back("SimpleLinearKalmanFilter");
      //creatables.push_back("LinearKalmanFilter");
      //creatables.push_back("ExtendedKalmanFilter");
      //creatables.push_back("UnscentedKalmanFilter");
      //creatables.push_back("ExtendedConsiderFilter");
      //creatables.push_back("UnscentedConsiderFilter");
      //creatables.push_back("ExtendedNeglectFilter");
      //creatables.push_back("UnscentedNeglectFilter");
      //creatables.push_back("CentralDifferenceKalmanFilter");
      //creatables.push_back("SquareRootInformationFilter");

      //#if defined __USE_MATLAB__
      //creatables.push_back("FminconOptimizer");
      //#endif

   }
}

//------------------------------------------------------------------------------
//  ExtendedKalmanFilterFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ExtendedKalmanFilterFactory.
 *
 * @param createList list of creatable ExtendedKalmanFilter objects
 */
//------------------------------------------------------------------------------
ExtendedKalmanFilterFactory::ExtendedKalmanFilterFactory(StringArray createList) :
   Factory           (createList, Gmat::SOLVER)
{
}


//------------------------------------------------------------------------------
//  ExtendedKalmanFilterFactory(const ExtendedKalmanFilterFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ExtendedKalmanFilterFactory.  (copy constructor)
 *
 * @param fact the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
ExtendedKalmanFilterFactory::ExtendedKalmanFilterFactory(const ExtendedKalmanFilterFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {

      creatables.push_back("ExtendedKalmanFilter");

      //creatables.push_back("BatchLeastSquares");
      //creatables.push_back("SequentialLeastSquares");
      //creatables.push_back("SimpleLinearKalmanFilter");
      //creatables.push_back("LinearKalmanFilter");
      //creatables.push_back("ExtendedKalmanFilter");
      //creatables.push_back("UnscentedKalmanFilter");
      //creatables.push_back("ExtendedConsiderFilter");
      //creatables.push_back("UnscentedConsiderFilter");
      //creatables.push_back("ExtendedNeglectFilter");
      //creatables.push_back("UnscentedNeglectFilter");
      //creatables.push_back("CentralDifferenceKalmanFilter");
      //creatables.push_back("SquareRootInformationFilter");

      //#if defined __USE_MATLAB__
      //creatables.push_back("FminconOptimizer");
      //#endif


   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * ExtendedKalmanFilterFactory operator for the ExtendedKalmanFilterFactory base class.
 *
 * @param fact The ExtendedKalmanFilterFactory object that is copied.
 *
 * @return "this" ExtendedKalmanFilterFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
ExtendedKalmanFilterFactory& ExtendedKalmanFilterFactory::operator=(const ExtendedKalmanFilterFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}


//------------------------------------------------------------------------------
// ~ExtendedKalmanFilterFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the ExtendedKalmanFilterFactory base class.
 */
//------------------------------------------------------------------------------
ExtendedKalmanFilterFactory::~ExtendedKalmanFilterFactory()
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
bool ExtendedKalmanFilterFactory::DoesObjectTypeMatchSubtype(const std::string &theType,
      const std::string &theSubtype)
{
   bool retval = false;

   if (theType == "ExtendedKalmanFilter")
   {
      if (theSubtype == "Estimator")
         retval = true;
   }

   return retval;
}
