//$Id: ExtendedKalmanFilter.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         ExtendedKalmanFilter
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/09/03
// Modified by: Jamie LaPointe, University of Arizona, Spring 2016
/**
 * Definition for a basic extended Kalman filter
 */
//------------------------------------------------------------------------------


#ifndef ExtendedKalmanFilter_hpp
#define ExtendedKalmanFilter_hpp

#include "kalman_defs.hpp"
#include "SeqEstimator.hpp"


/**
 * This implementation provides a basic extended Kalman filter (EKF)
 *
 * ExtendedKalmanFilter follows the EKF described in the flowchart on page 212 of
 * Tapley, Schutz and Born (2004).  Several refinements, suggested by R.
 * Carpenter, are included in this code:
 *
 * 1.  The state noise covariance matrix and the time updated covariance both
 * have symmetry enforced explicitly.
 *
 * 2.  The state covariance matrix update can be performed two different ways;
 * either using the simple form in the flow chart:
 *
 *    P = (I - K Htilde) Pbar
 *
 * or using the form derived by Bucy and Joseph (equation 4.7.19 on page 205).
 * This choice is made at compile time in the UpdateElements() method.  The
 * current default selection is the Bucy-Joseph update.
 */
class KALMAN_API ExtendedKalmanFilter : public SeqEstimator
{
public:
   ExtendedKalmanFilter(const std::string name);
   virtual ~ExtendedKalmanFilter();
   ExtendedKalmanFilter(const ExtendedKalmanFilter &ekf);
   ExtendedKalmanFilter&      operator=(const ExtendedKalmanFilter &ekf);

   GmatBase*               Clone() const;
   virtual void            Copy(const GmatBase*);

protected:
   /// The measurement Residuals (O-C)
   Rvector                 yi;
   /// The Kalman gain
   Rmatrix                 kalman;

   // Factorization variables
   Rmatrix                 sqrtP;
   Rmatrix                 sqrtPupdate;

   virtual void            CompleteInitialization();
   virtual void            Estimate();

   virtual void            FilterUpdate();

   virtual void            TimeUpdate();

   virtual std::string     DataFileCovHeader() const;
   virtual void            WriteCovarianceToDataFile();
   virtual void            ReadCovarianceFromDataFile(StringArray header, StringArray restartData,
                                                      UnsignedInt firstStateIndex, IntegerArray stateColumnNum);
   virtual void            UpdateCovarianceNominalValues(RealArray prevEpsilonConversions);

   virtual void            SqrtCovarianceEpsilonConversion(Rmatrix& sqrtCov);
   virtual void            SetCovariance(const Rmatrix& cov);

   virtual void            UpdateCov();

private:
   void                    SetupMeas();
   void                    ComputeObs(UpdateInfoType &updateStat);
   void                    ComputeGain(UpdateInfoType &updateStat);
   void                    UpdateElements(UpdateInfoType &updateStat);
   void                    AdvanceEpoch();

   void                    UpdateCovarianceSimple();
   void                    UpdateCovarianceJoseph();

   const MeasurementData *calculatedMeas;
   const ObservationData *currentObs;
};

#endif /* ExtendedKalmanFilter_hpp */
