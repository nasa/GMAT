//------------------------------------------------------------------------------
//                            EstimatorFactory
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
// Developed by Dr. Matthew P. Wilkins, Schafer Corporation
//
// Author: Matthew P. Wilkins
// Created: 2008/05/20
//
/**
 *  Implementation code for the EstimatorFactory class, responsible
 *  for creating estimator objects.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "EstimatorFactory.hpp"
#include "MessageInterface.hpp"

// Here are the supported leaf classes
#include "Simulator.hpp"
#include "BatchEstimator.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Estimator class.
 *
 * @param ofType type of Estimator object to create and return.
 * @param withName the name for the newly-created Estimator object.
 *
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
GmatBase* EstimatorFactory::CreateObject(const std::string &ofType,
                                         const std::string &withName)
{
   return CreateSolver(ofType, withName);
}

//------------------------------------------------------------------------------
//  Solver* CreateSolver(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Estimator class.
 *
 * @param ofType type of Estimator object to create and return.
 * @param withName the name for the newly-created Estimator object.
 *
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
Solver* EstimatorFactory::CreateSolver(const std::string &ofType,
                                       const std::string &withName)
{
   // Not sure that we want to break these guys out like this; left in in case
   //it's desired

   //   return CreateEstimator(ofType, withName);
   //}
   //
   ////------------------------------------------------------------------------------
   ////  Estimator* CreateEstimator(const std::string &ofType, const std::string &withName)
   ////------------------------------------------------------------------------------
   ///**
   // * This method creates and returns an object of the requested Estimator class.
   // *
   // * @param ofType Type of Estimator object to create and return.
   // * @param withName The name for the newly-created Estimator object.
   // *
   // * @return A pointer to the created object.
   // */
   ////------------------------------------------------------------------------------
   //Estimator* EstimatorFactory::CreateEstimator(const std::string &ofType,
   //                                    const std::string &withName)
   //{
   //MessageInterface::ShowMessage("EstimatorFactory is creating a %s named %s\n",
   //      ofType.c_str(), withName.c_str());

   if (ofType == "Simulator")
      return new Simulator(withName);
   if (ofType == "BatchEstimator")
      return new BatchEstimator(withName);
   if (ofType == "BatchEstimatorInv")
   {
      // Write deprecated message once per GMAT session
      static bool batchEstimatorInvFirstWarning = true;

      if (batchEstimatorInvFirstWarning)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"BatchEstimatorInv\" resource is "
             "deprecated and will be removed from a future build; "
             "please use \"BatchEstimator\" instead.\n");
         batchEstimatorInvFirstWarning = false;
      }

      return new BatchEstimator(withName);
   }

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
//  EstimatorFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class EstimatorFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
EstimatorFactory::EstimatorFactory() :
   Factory     (Gmat::SOLVER)
//   Factory     (Gmat::ESTIMATOR)
{
   if (creatables.empty())
   {
      creatables.push_back("Simulator");
      creatables.push_back("BatchEstimator");
      creatables.push_back("BatchEstimatorInv"); // DEPRECATED: Renamed to BatchEstimator

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
   GmatType::RegisterType("Simulator");
   GmatType::RegisterType("BatchEstimator");
}

//------------------------------------------------------------------------------
//  EstimatorFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class EstimatorFactory.
 *
 * @param createList list of creatable estimator objects
 */
//------------------------------------------------------------------------------
EstimatorFactory::EstimatorFactory(StringArray createList) :
   Factory           (createList, Gmat::SOLVER)
{
   GmatType::RegisterType("Simulator");
   GmatType::RegisterType("BatchEstimator");
}


//------------------------------------------------------------------------------
//  EstimatorFactory(const EstimatorFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class EstimatorFactory.  (copy constructor)
 *
 * @param fact the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
EstimatorFactory::EstimatorFactory(const EstimatorFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("Simulator");
      creatables.push_back("BatchEstimatorInv");

#ifdef ENABLE_UNFINISHED_FEATURES
      creatables.push_back("BatchEstimatorSVD");
      creatables.push_back("ExtendedKalmanInv");
#endif

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
 * EstimatorFactory operator for the EstimatorFactory base class.
 *
 * @param fact The EstimatorFactory object that is copied.
 *
 * @return "this" EstimatorFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
EstimatorFactory& EstimatorFactory::operator=(const EstimatorFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}


//------------------------------------------------------------------------------
// ~EstimatorFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the EstimatorFactory base class.
 */
//------------------------------------------------------------------------------
EstimatorFactory::~EstimatorFactory()
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
bool EstimatorFactory::DoesObjectTypeMatchSubtype(const std::string &theType,
      const std::string &theSubtype)
{
   bool retval = false;

   if (theType == "Simulator")
   {
      if (theSubtype == "Simulator")
         retval = true;
   }

   if ((theType == "BatchEstimatorInv")
#ifdef ENABLE_UNFINISHED_FEATURES
       ||(theType == "BatchEstimatorSVD")  // Not yet implemented; leave off menu
       || (theType == "ExtendedKalmanInv")
#endif
       )
   {
      if (theSubtype == "Estimator")
         retval = true;
   }

   return retval;
}
