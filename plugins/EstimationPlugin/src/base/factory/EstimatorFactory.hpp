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


#ifndef EstimatorFactory_hpp
#define EstimatorFactory_hpp

#include "estimation_defs.hpp"
#include "Factory.hpp"


// Forward References for the supported Estimators
class BatchLeastSquares;
//class SequentialLeastSquares;
//class SimpleLinearKalmanFilter;
//class LinearKalmanFilter;
//class ExtendedKalmanFilter;
//class UnscentedKalmanFilter;
//class ExtendedConsiderFilter;
//class UnscentedConsiderFilter;
//class ExtendedNeglectFilter;
//class UnscentedNeglectFilter;
//class CentralDifferenceKalmanFilter;
//class SquareRootInformationFilter;

class ESTIMATION_API EstimatorFactory : public Factory
{
public:
//   virtual Estimator* CreateEstimator(const std::string &ofType,
//                                const std::string &withName /* = "" */);

   virtual GmatBase* CreateObject(const std::string &ofType,
                                  const std::string &withName /* = "" */);
   virtual Solver*   CreateSolver(const std::string &ofType,
                                  const std::string &withName /* = "" */);

   // default constructor
   EstimatorFactory();
   // constructor
   EstimatorFactory(StringArray createList);
   // copy constructor
   EstimatorFactory(const EstimatorFactory& fact);
   // assignment operator
   EstimatorFactory& operator=(const EstimatorFactory& fact);

   virtual ~EstimatorFactory();

   virtual bool DoesObjectTypeMatchSubtype(const std::string &theType,
         const std::string &theSubtype);
};

#endif // EstimatorFactory_hpp
