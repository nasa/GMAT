//$Id: ExtendedKalmanFilter.cpp 1398 2011-04-21 20:39:37Z  $
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
//
// Process Noise added by:
// Author: Jamie J. LaPointe, University of Arizona
// Modifed: 2016/05/09
//
/**
 * A simple extended Kalman filter
 */
//------------------------------------------------------------------------------

#include "ExtendedKalmanFilter.hpp"
#include "EstimatorException.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include "UtilityException.hpp"
#include <cmath>
#include <limits>


//#define DEBUG_ESTIMATION
//#define DEBUG_JOSEPH
//#define DEBUG_ESTIMATION_COVARIACE_PROP

//------------------------------------------------------------------------------
// ExtendedKalmanFilter(const std::string name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name of the new instance
 */
//------------------------------------------------------------------------------
ExtendedKalmanFilter::ExtendedKalmanFilter(const std::string name) :
   SeqEstimator  ("ExtendedKalmanFilter", name),
   calculatedMeas(0),
   currentObs(0)
{
   objectTypeNames.push_back("ExtendedKalmanFilter");

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage(" EKF default constructor: stateSize = %o, "
            "measSize = %o\n", stateSize, measSize);
   #endif
}

//------------------------------------------------------------------------------
// ~ExtendedKalmanFilter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ExtendedKalmanFilter::~ExtendedKalmanFilter()
{
}


//------------------------------------------------------------------------------
// ExtendedKalmanFilter(const ExtendedKalmanFilter & ekf) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ekf The instance used to configure this instance
 */
//------------------------------------------------------------------------------
ExtendedKalmanFilter::ExtendedKalmanFilter(const ExtendedKalmanFilter & ekf) :
   SeqEstimator  (ekf),
   calculatedMeas(0),
   currentObs(0)
{
}


//------------------------------------------------------------------------------
// ExtendedKalmanFilter& operator=(const ExtendedKalmanFilter &ekf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ekf The instance used to configure this instance
 *
 * @return this instance, configured to match ekf.
 */
//------------------------------------------------------------------------------
ExtendedKalmanFilter& ExtendedKalmanFilter::operator=(const ExtendedKalmanFilter &ekf)
{
   if (this != &ekf)
   {
      SeqEstimator::operator=(ekf);
      measSize = ekf.measSize;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Object cloner
 *
 * @return A clone of this object
 */
//------------------------------------------------------------------------------
GmatBase* ExtendedKalmanFilter::Clone() const
{
   return new ExtendedKalmanFilter(*this);
}

//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void  ExtendedKalmanFilter::Copy(const GmatBase* orig)
{
   operator=(*((ExtendedKalmanFilter*)(orig)));
}



//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void CompleteInitialization()
//------------------------------------------------------------------------------
/**
 * Prepares the estimator for a run
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::CompleteInitialization()
{
   SeqEstimator::CompleteInitialization();
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage(" EKF CompleteInitialization: stateSize = %o, "
            "measSize = %o\n", stateSize, measSize);
   #endif

   Integer size = stateCovariance->GetDimension();
   if (size != (Integer)stateSize)
   {
      throw EstimatorException("In ExtendedKalmanFilter::Estimate(), the "
            "covariance matrix is not sized correctly!!!");
   }

   I = Rmatrix::Identity(stateSize);

   sqrtPupdate.SetSize(stateSize, stateSize);
   if (!sqrtP.IsSized())
   {
      sqrtP.SetSize(stateSize, stateSize);
      cf.Factor(*(stateCovariance->GetCovariance()), sqrtP);
      sqrtP = sqrtP.Transpose();
   }

   currentObs =  measManager.GetObsData();
   if (currentObs == NULL)
   {
      throw EstimatorException("Error: No observation data was used for estimation.\n");
   }

   Rmatrix outSqrtCov = sqrtP;
   SqrtCovarianceEpsilonConversion(outSqrtCov);
   updateStats[0].sqrtCov.SetSize(outSqrtCov.GetNumRows(), outSqrtCov.GetNumColumns());
   updateStats[0].sqrtCov = outSqrtCov;

   prevUpdateEpochGT = currentEpochGT;
}


//------------------------------------------------------------------------------
// void Estimate()
//------------------------------------------------------------------------------
/**
 * Implements the time update, compute, and orbit updates for the estimator
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::Estimate()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("\n\n--ExtendedKalmanFilter::Estimate----\n");
      MessageInterface::ShowMessage("Current covariance:\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12le", stateCovariance->GetCovariance()->GetElement(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");

      MessageInterface::ShowMessage("Current stm:\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12lf", (*stm)(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");

      MessageInterface::ShowMessage("Current State: [ ");
      for (UnsignedInt i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage(" %.12lf ", (*estimationState)[i]);
      MessageInterface::ShowMessage("\n");
   #endif

   UpdateInfoType updateStat;

   // setup the measurement objects for the rest of this frame of data to use
   SetupMeas();

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Time updated matrix \\bar P:\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12le", pBar(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   // Construct the O-C data and H tilde
   ComputeObs(updateStat);

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("hTilde:\n");
      for (UnsignedInt i = 0; i < measSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12lf", hTilde[i][j]);
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   // Then the Kalman gain
   ComputeGain(updateStat);

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("The Kalman gain is: \n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < measSize; ++j)
            MessageInterface::ShowMessage("   %.12lf", kalman(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   // Finally, update everything
   UpdateElements(updateStat);

   // Plot residuals if set
   if (showAllResiduals)
   {
      PlotResiduals();
   }

   FillUpdateInfo(updateStat);
   updateStat.isObs = true;

   Rmatrix outSqrtCov = sqrtP;
   SqrtCovarianceEpsilonConversion(outSqrtCov);
   updateStat.sqrtCov.SetSize(outSqrtCov.GetNumRows(), outSqrtCov.GetNumColumns());
   updateStat.sqrtCov = outSqrtCov;

   updateStats.push_back(updateStat);
   BuildMeasurementLine(updateStat.measStat);
   WriteDataFile();
   AddMatlabData(updateStat.measStat);
   AddMatlabFilterData(updateStat);


   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Updated covariance:\n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12le", stateCovariance->GetCovariance()->GetElement(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");

      MessageInterface::ShowMessage("Updated State: [ ");
      for (UnsignedInt i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage(" %.12lf ", (*estimationState)[i]);
      MessageInterface::ShowMessage("\n\n---------------------\n");
   #endif

   if (textFileMode == "Verbose")
      ReportProgress();

   AdvanceEpoch();
}


//------------------------------------------------------------------------------
// void FilterUpdate()
//------------------------------------------------------------------------------
/**
 * This method performs actions common to sequential estimators to update the
 * filter information and add it to the updateStats vector
 */
 //------------------------------------------------------------------------------
void ExtendedKalmanFilter::FilterUpdate()
{
   // Update the STM
   esm.MapObjectsToSTM();
   esm.MapObjectsToVector();

   UpdateProcessNoise();
   TimeUpdate();
   (*(stateCovariance->GetCovariance())) = pBar;
   esm.MapCovariancesToObjects();

   if (currentState != CALCULATING)
   {
      // Get data for the covariance report
      UpdateInfoType updateStat;
      FillUpdateInfo(updateStat);
      updateStat.isObs = false;

      Rmatrix outSqrtCov = sqrtP;
      SqrtCovarianceEpsilonConversion(outSqrtCov);
      updateStat.sqrtCov.SetSize(outSqrtCov.GetNumRows(), outSqrtCov.GetNumColumns());
      updateStat.sqrtCov = outSqrtCov;

      // Don't want to add data to vector if predicting to an epoch that's not an anchor epoch
      if (!dontWriteDataInUpdate)
      {
         if (!(isPredicting && !hasAnchorEpoch))
         {
            WriteDataFile();
            AddMatlabFilterData(updateStat);
            updateStats.push_back(updateStat);
         }
         else if (addPredictToMatlab)
         {
            AddMatlabFilterData(updateStat);
         }
      }

      // Reset the STM
      PrepareForStep();
      esm.MapObjectsToVector();
      PropagationStateManager *psm = propagators[0]->GetPropStateManager();
      psm->MapObjectsToVector();
      resetState = true;
   }
}

//------------------------------------------------------------------------------
// void TimeUpdate()
//------------------------------------------------------------------------------
/**
 * Performs the time update of the state error covariance
 *
 * This method uses Cholesky factorization for covariance
 * This is based on section 5.7 of Brown and Hwang 4e
 */
 //------------------------------------------------------------------------------
void ExtendedKalmanFilter::TimeUpdate()
{
#ifdef DEBUG_ESTIMATION
   MessageInterface::ShowMessage("Performing time update\n");
#endif

#ifdef DEBUG_ESTIMATION_COVARIACE_PROP
   MessageInterface::ShowMessage("Q = \n");
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
         MessageInterface::ShowMessage("   %.12le", Q(i, j));
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("\n");
#endif

   /// Calculate conversion derivative matrixes
   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   cart2SolvMatrix = esm.CartToSolveForStateConversionDerivativeMatrix();
   // Calculate conversion derivative matrix [dS/dK] from solve-for state to Keplerian
   solv2KeplMatrix = esm.SolveForStateToKeplConversionDerivativeMatrix();

   Rmatrix dX_dS = cart2SolvMatrixPrev;
   Rmatrix dS_dX = cart2SolvMatrix.Inverse();

   Rmatrix Q_S = dS_dX * Q * dS_dX.Transpose();


#ifdef DEBUG_ESTIMATION_COVARIACE_PROP
   MessageInterface::ShowMessage("dS_dX = \n");
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
         MessageInterface::ShowMessage("   %.12le", dS_dX(i, j));
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("\n");
   MessageInterface::ShowMessage("dX_dS = \n");
   for (UnsignedInt i = 0; i < 6; ++i)
   {
      for (UnsignedInt j = 0; j < 6; ++j)
         MessageInterface::ShowMessage("   %.12le", dX_dS(i, j));
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("\n");
   MessageInterface::ShowMessage("stm = \n");
   for (UnsignedInt i = 0; i < 6; ++i)
   {
      for (UnsignedInt j = 0; j < 6; ++j)
         MessageInterface::ShowMessage("   %.12le", (*stm)(i, j));
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("\n");
   MessageInterface::ShowMessage("Q_s = \n");
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
         MessageInterface::ShowMessage("   %.12le", Q_S(i, j));
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("\n");
#endif

   Rmatrix stm_S = dS_dX * (*stm) * dX_dS;

   // Update offset from reference trajectory
   if (esm.HasStateOffset())
   {
      GmatState *offsetState = esm.GetStateOffset();
      Rvector xOffset(stateSize);
      xOffset.Set(offsetState->GetState(), stateSize);

      xOffset = (*stm) * xOffset;

      for (UnsignedInt i = 0; i < stateSize; ++i)
         (*offsetState)[i] = xOffset[i];
   }

   // Form perform thinQR decomposition to calculate pBar

   Rmatrix sqrtQ_T(stateSize, stateSize);

   bool hasZeroDiag = false;
   for (UnsignedInt ii = 0U; ii < stateSize; ii++)
   {
      if (Q_S(ii, ii) == 0U)
      {
         hasZeroDiag = true;
         break;
      }
   }

   if (!hasZeroDiag)
   {
      try
      {
         cf.Factor(Q_S, sqrtQ_T);
      }
      catch (UtilityException e)
      {
         throw EstimatorException("The process noise matrix is not positive definite!");
      }
   }
   else
   {
      // Remove all zero rows/columns first
      IntegerArray removedIndexes;
      IntegerArray auxVector;
      Integer numRemoved;
      Rmatrix reducedQ_S = MatrixFactorization::CompressNormalMatrix(Q_S,
         removedIndexes, auxVector, numRemoved);

      Rmatrix reducedSqrtQ_T(stateSize - numRemoved, stateSize - numRemoved);
      try
      {
         cf.Factor(reducedQ_S, reducedSqrtQ_T);
      }
      catch (UtilityException e)
      {
         throw EstimatorException("The process noise matrix is not positive definite!");
      }

      sqrtQ_T = MatrixFactorization::ExpandNormalMatrixInverse(reducedSqrtQ_T,
         auxVector, numRemoved);
   }

   Rmatrix stmP = stm_S * sqrtP;
   Rmatrix sqrtQ = sqrtQ_T.Transpose();

   #ifdef DEBUG_ESTIMATION_COVARIACE_PROP
      MessageInterface::ShowMessage("stmP = \n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", stmP(i, j));
         }
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("sqrtP = \n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", sqrtP(i, j));
         }
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("sqrtQ = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", sqrtQ(i, j));
         }
         MessageInterface::ShowMessage("\n");
      }
#endif
   sqrtP = thinQR(stmP, sqrtQ);

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("sqrtP = \n");
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", sqrtP(i,j));
         }
         MessageInterface::ShowMessage("\n");
      }
   #endif

   // Warn if covariance is not positive definite
   for (UnsignedInt ii = 0U; ii < stateSize; ii++)
   {
      if (GmatMathUtil::Abs(sqrtP(ii, ii)) < 1e-16)
      {
         MessageInterface::ShowMessage("WARNING The covariance is no longer positive definite! Epoch = %s\n", currentEpochGT.ToString().c_str());
         break;
      }
   }

   pBar = sqrtP * sqrtP.Transpose();

   // make it symmetric!
   Symmetrize(pBar);

#ifdef DEBUG_ESTIMATION_COVARIACE_PROP
   MessageInterface::ShowMessage("pBar = \n");
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      for (UnsignedInt j = 0; j < stateSize; ++j)
      {
         MessageInterface::ShowMessage("  %.12le", pBar(i, j));
      }
      MessageInterface::ShowMessage("\n");
   }
#endif
}

//------------------------------------------------------------------------------
// void SetupMeas()
//------------------------------------------------------------------------------
/**
 * This sets up the measurement information for others to use later
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::SetupMeas()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Performing measurement setup\n");
   #endif

   modelsToAccess = measManager.GetValidMeasurementList();
   currentObs =  measManager.GetObsData();

   if (modelsToAccess.size() > 0U)
   {
      measCount = measManager.CountFeasibleMeasurements(modelsToAccess[0]);
      calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);

      // verify media correction to be in acceptable range. It is [0m, 60m] for troposphere correction and [0m, 20m] for ionosphere correction
      ValidateMediaCorrection(calculatedMeas);

      // Make correction for observation value before running data filter
      if ((iterationsTaken == 0) && (currentObs->typeName == "DSN_SeqRange" || currentObs->typeName == "DSN_PNRange"))
      {
         // value correction is only applied for DSN_SeqRange and it is only performed at the first time
         for (Integer index = 0; index < currentObs->value.size(); ++index)
            measManager.GetObsDataObject()->value[index] = ObservationDataCorrection(calculatedMeas->value[index], currentObs->value[index], currentObs->rangeModulo);
      }

      // Get pre-update covariance and symmetrize
      pBar = sqrtP * sqrtP.Transpose();
      Symmetrize(pBar);

      // Size the measurement matricies
      measSize = currentObs->value.size();

      H.SetSize(measSize, stateSize);
      yi.SetSize(measSize);
      kalman.SetSize(stateSize, measSize);
   }

   /// Calculate conversion derivative matrixes
   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   cart2SolvMatrix = esm.CartToSolveForStateConversionDerivativeMatrix();
   // Calculate conversion derivative matrix [dS/dK] from solve-for state to Keplerian
   solv2KeplMatrix = esm.SolveForStateToKeplConversionDerivativeMatrix();
}


//------------------------------------------------------------------------------
// void ComputeObs(UpdateInfoType &updateStat)
//------------------------------------------------------------------------------
/**
 * Computes the measurement residuals and the H-tilde matrix
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::ComputeObs(UpdateInfoType &updateStat)
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Computing obs and hTilde\n");
   #endif
   // Compute the O-C, Htilde, and Kalman gain

   // Populate measurement statistics
   FilterMeasurementInfoType measStat;
   CalculateResiduals(measStat);

   // Populate H and y
   if (modelsToAccess.size() > 0)
   {
      // Adjust computed and residual based on value of xOffset
      Rvector xOffset(stateSize);
      xOffset.Set(esm.GetEstimationStateOffset().GetState(), stateSize);

      Rvector H_x = H * xOffset;
      for (UnsignedInt k = 0; k < measStat.residual.size(); ++k)
      {
         measStat.measValue[k] += H_x(k);
         measStat.residual[k] -= H_x(k);
      }

      if (measStat.isCalculated)
      {
         for (UnsignedInt k = 0; k < measStat.residual.size(); ++k)
            yi(k) = measStat.residual[k];
      }

      // get scaled residuals
      Rmatrix R = *(GetMeasurementCovariance()->GetCovariance());

      // Keep this line for when we implement the scaled residual for the entire measurement
      // instead of for each element of the measurement:
      // measStat.scaledResid = GmatMathUtil::Sqrt(yi * (H * pBar * H.Transpose() + R).Inverse() * yi);

      // The element-by-element scaled residual calculation:
      for (UnsignedInt k = 0; k < measStat.residual.size(); ++k)
      {
         Rmatrix Rbar = H * pBar * H.Transpose() + R;
         Real sigmaVal = GmatMathUtil::Sqrt(Rbar(k, k));
         Real scaledResid = measStat.residual[k] / sigmaVal;
         measStat.scaledResid.push_back(scaledResid);
      }

   }  // end of if (modelsToAccess.size() > 0)

   GmatState currentState = esm.GetEstimationMJ2000EqCartesianStateForReport();
   for (UnsignedInt ii = 0; ii < stateSize; ii++)
      measStat.state.push_back(currentState[ii]);

   // Add state offset if not rectified
   if (esm.HasStateOffset())
   {
      GmatState xOffset = *esm.GetStateOffset();
      for (UnsignedInt ii = 0; ii < stateSize; ii++)
      {
         Real conv = GetEpsilonConversion(ii);
         measStat.state[ii] += xOffset[ii] * conv;
      }
   }

   Rmatrix outCov = pBar;
   CovarianceEpsilonConversion(outCov);
   measStat.cov.SetSize(outCov.GetNumRows(), outCov.GetNumColumns());
   measStat.cov = outCov;
   Rmatrix covVNB = GetCovarianceVNB(pBar);
   measStat.covVNB.SetSize(covVNB.GetNumRows(), covVNB.GetNumColumns());
   measStat.covVNB = covVNB;

   Rmatrix outSqrtCov = sqrtP;
   SqrtCovarianceEpsilonConversion(outSqrtCov);
   measStat.sqrtCov.SetSize(outSqrtCov.GetNumRows(), outSqrtCov.GetNumColumns());
   measStat.sqrtCov = outSqrtCov;

   measStats.push_back(measStat);

   updateStat.epoch = currentEpochGT;
   updateStat.isObs = true;
   updateStat.measStat = measStat;

   BuildMeasurementLine(measStat);
   WriteToTextFile();
}


//------------------------------------------------------------------------------
// void ComputeGain(UpdateInfoType &updateStat)
//------------------------------------------------------------------------------
/**
 * Computes the Kalman gain
 *
 * The error estimates used for error bars on the residuals plots are calculated
 * as
 *
 *    sigma = sqrt(H P H' + R)
 *
 * Since the argument of the square root is calculated as part of the Kalman
 * gain calculation, this value is also stored in this method
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::ComputeGain(UpdateInfoType &updateStat)
{
   if (updateStat.measStat.isCalculated)
   {
      #ifdef DEBUG_ESTIMATION
         MessageInterface::ShowMessage("Computing Kalman Gain\n");
      #endif

      // Set up measurement underweighting (Lear's method)
      Real sqrtScale = 1.0;
      Real posCovTraceSqrt = GmatMathUtil::Sqrt(pBar(0, 0) + pBar(1, 1) + pBar(2, 2));
      if (posCovTraceSqrt > deweightThreshold && deweightCoeff > 0)
      {
         sqrtScale = GmatMathUtil::Sqrt(1.0 + deweightCoeff);

         bool handleLeapSecond;
         GmatTime utcMjdEpoch = theTimeConverter->Convert(updateStat.measStat.epoch, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
            GmatTimeConstants::JD_JAN_5_1941, &handleLeapSecond);
         std::string utcEpoch = theTimeConverter->ConvertMjdToGregorian(utcMjdEpoch.GetMjd(), handleLeapSecond);

         MessageInterface::ShowMessage("Measurement %d of type %s at %s UTCG was underweighted. (1 sigma pos uncertainty was %g km)\n",
            updateStat.measStat.recNum, updateStat.measStat.type.c_str(), utcEpoch.c_str(), posCovTraceSqrt);
      }

      // Perform thinQR decomposition to calculate K and P
      Rmatrix R = *(GetMeasurementCovariance()->GetCovariance());
      Integer measSize = R.GetNumRows();

      Rmatrix sqrtR_T(measSize, measSize);
      cf.Factor(R, sqrtR_T);

      Rmatrix Spbar = sqrtP;
      Rmatrix Sr = sqrtR_T.Transpose();
      Rmatrix Sw = thinQR(sqrtScale*H*Spbar, Sr);

      #ifdef DEBUG_ESTIMATION
         MessageInterface::ShowMessage("Sw = \n");
         for (UnsignedInt i = 0; i < Sw.GetNumRows(); ++i)
         {
           for (UnsignedInt j = 0; j < Sw.GetNumColumns(); ++j)
           {
             MessageInterface::ShowMessage("  %.12le", Sw(i,j));
           }
           MessageInterface::ShowMessage("\n");
         }
      #endif

      #ifdef DEBUG_ESTIMATION
         MessageInterface::ShowMessage("Calculating the Kalman gain\n");
      #endif

      kalman = Spbar * Spbar.Transpose() * H.Transpose() * (Sw*Sw.Transpose()).Inverse();
      sqrtPupdate = thinQR((I - kalman * H) * Spbar, kalman*Sr);

      updateStat.measStat.kalmanGain.SetSize(kalman.GetNumRows(), kalman.GetNumColumns());
      updateStat.measStat.kalmanGain = kalman;
   }
}


//------------------------------------------------------------------------------
// void UpdateElements(UpdateInfoType &updateStat)
//------------------------------------------------------------------------------
/**
 * Updates the estimation state and covariance matrix
 *
 * Programmers can select the covariance update method at the end of this
 * method.  The resulting covariance is symmetrized before returning.
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::UpdateElements(UpdateInfoType &updateStat)
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Updating elements\n");
   #endif

   if (updateStat.measStat.editFlag == NORMAL_FLAG)
   {
      dx = kalman * yi;

      if (esm.HasStateOffset())
      {
         GmatState offsetState = esm.GetEstimationStateOffset();
         for (UnsignedInt i = 0; i < stateSize; ++i)
         {
            offsetState[i] += dx[i];
         }
         esm.SetEstimationStateOffset(offsetState);

      }
      else
      {
         // Update the state, covariances, and so forth
         estimationStateS = esm.GetEstimationState();
         for (UnsignedInt i = 0; i < stateSize; ++i)
         {
            estimationStateS[i] += dx[i];
         }

         // Convert estimation state from Keplerian to Cartesian
         esm.SetEstimationState(estimationStateS);                       // update the value of estimation state
         esm.MapVectorToObjects();
         esm.MapCovariancesToObjects();
      }

      #ifdef DEBUG_ESTIMATION
         MessageInterface::ShowMessage("Calculated state change: [");
         for (UnsignedInt i = 0; i < stateSize; ++i)
            MessageInterface::ShowMessage(" %.12lf ", dx[i]);
         MessageInterface::ShowMessage("\n");
      #endif

      // Select the method used to update the covariance here:
      // UpdateCovarianceSimple();
      // UpdateCovarianceJoseph();

      Rmatrix P2 = sqrtPupdate * sqrtPupdate.Transpose();
      sqrtP = sqrtPupdate;

      // Warn if covariance is not positive definite
      for (UnsignedInt ii = 0U; ii < stateSize; ii++)
      {
         if (GmatMathUtil::Abs(sqrtP(ii, ii)) < 1e-16)
         {
            MessageInterface::ShowMessage("WARNING The covariance is no longer positive definite! Epoch = %s\n", currentEpochGT.ToString().c_str());
            break;
         }
      }

      (*(stateCovariance->GetCovariance())) = P2;
   }
   else
      (*(stateCovariance->GetCovariance())) = sqrtP * sqrtP.Transpose();

   Symmetrize(*stateCovariance);
   informationInverse = (*(stateCovariance->GetCovariance()));
   //information = informationInverse.Inverse(COV_INV_TOL);
}


//------------------------------------------------------------------------------
// void UpdateCovarianceSimple()
//------------------------------------------------------------------------------
/**
 * Applies equation (4.7.12) to update the state error covariance matrix
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::UpdateCovarianceSimple()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Updating covariance using simple "
            "method\n");
   #endif

   // P = (I - K * H) * Pbar
   (*(stateCovariance->GetCovariance())) = (I - (kalman * H)) * pBar;
}


//------------------------------------------------------------------------------
// void ExtendedKalmanFilter::UpdateCovarianceJoseph()
//------------------------------------------------------------------------------
/**
 * This method updates the state error covariance matrix using the method
 * developed by Bucy and Joseph, as presented in Tapley, Schutz and Born
 * eq (4.7.19)
 */
//------------------------------------------------------------------------------
void ExtendedKalmanFilter::UpdateCovarianceJoseph()
{
   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("Updating covariance using Joseph "
            "method\n");
   #endif

   Rmatrix *r = GetMeasurementCovariance()->GetCovariance();

   // P = (I - K * H) * Pbar * (I - K * H)^T + K * R * K^T
   (*(stateCovariance->GetCovariance())) =
         ((I - (kalman * H)) * pBar * (I - (kalman * H)).Transpose()) +
         (kalman * (*r) * kalman.Transpose());

   #ifdef DEBUG_JOSEPH
      for (UnsignedInt i = 0; i < stateSize; ++i)
      {
         for (UnsignedInt j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("  %.12lf  ", (*(stateCovariance->GetCovariance()))(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");

      throw EstimatorException("Intentional debug break!");
   #endif
}

void ExtendedKalmanFilter::AdvanceEpoch()
{
   // Reset the STM
   PrepareForStep();
   esm.MapVectorToObjects();
   esm.MapCovariancesToObjects();
   //PropagationStateManager *psm = propagators[0]->GetPropStateManager();
   //psm->MapObjectsToVector();

   for (int i = 0; i < propagators.size(); i++)
   {
      PropagationStateManager *psm = propagators[i]->GetPropStateManager();
      psm->MapObjectsToVector();
   }
   // Flag that a new current state has been loaded in the objects
   resetState = true;

   // Advance MeasMan to the next measurement and get its epoch
   bool isEndOfTable = measManager.AdvanceObservation();
   if (isEndOfTable)
   {
      currentState = CHECKINGRUN;
   }
   else
   {
      nextMeasurementEpochGT = measManager.GetEpochGT();

      // Check if rectification should begin here
      if (esm.HasStateOffset())
      {
         // Check if next measurement is after the delayed rectification span
         Real elapsedTime = (nextMeasurementEpochGT - estimationEpochGT).GetTimeInSec();

         if (GmatMathUtil::Abs(elapsedTime) > delayRectifySpan)
         {
            // Update the state with the state offset
            GmatState offsetStateS = esm.GetEstimationStateOffset();
            estimationStateS = esm.GetEstimationState();
            for (UnsignedInt i = 0; i < stateSize; ++i)
            {
               estimationStateS[i] += offsetStateS[i];
            }

            // Convert estimation state from Keplerian to Cartesian
            esm.SetEstimationState(estimationStateS);                       // update the value of estimation state
            esm.MapVectorToObjects();
            esm.MapCovariancesToObjects();

            // Zero out the state offset
            GmatState *offsetState = esm.GetStateOffset();
            for (UnsignedInt i = 0; i < stateSize; ++i)
               (*offsetState)[i] = 0.0;

            esm.SetHasStateOffset(false);

            bool handleLeapSecond;
            GmatTime utcMjdEpoch = theTimeConverter->Convert(currentEpochGT, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
               GmatTimeConstants::JD_JAN_5_1941, &handleLeapSecond);
            std::string utcEpoch = theTimeConverter->ConvertMjdToGregorian(utcMjdEpoch.GetMjd(), handleLeapSecond);

            MessageInterface::ShowMessage("Exiting DelayRectifyTimeSpan at %s UTCG\n", utcEpoch.c_str());
            WriteDataFile();
         }
      }

      // Reset nextNoiseUpdateGT if it is in the wrong direction
      Real dtNoise = (nextNoiseUpdateGT - currentEpochGT).GetTimeInSec();
      Real dtMeasurement = (nextMeasurementEpochGT - currentEpochGT).GetTimeInSec();

      // If filter has passed the noise epoch
      if (measManager.IsForward() && dtNoise < ESTTIME_ROUNDOFF * GmatTimeConstants::SECS_PER_DAY)
         nextNoiseUpdateGT.AddSeconds(processNoiseStep);
      else if (!measManager.IsForward() && dtNoise > -ESTTIME_ROUNDOFF * GmatTimeConstants::SECS_PER_DAY)
         nextNoiseUpdateGT.SubtractSeconds(processNoiseStep);

      FindTimeStep();

      #ifdef DEBUG_ESTIMATION
         MessageInterface::ShowMessage("ExtendedKalmanFilter::AdvanceEpoch CurrentEpoch = %.12lf, next "
               "epoch = %.12lf, timeStep = %.12lf\n", currentEpochGT.GetMjd(),
               nextMeasurementEpochGT.GetMjd(), timeStep);
      #endif

      // this magical number is from the Batch Estimator in its accumulating state...
      //if (currentEpoch <= (nextMeasurementEpoch+5.0e-12))
      if (nextMeasurementEpochGT >= 5.0e-12)
      {
         currentState = PROPAGATING;
      }
      else
      {
         currentState = CHECKINGRUN;  // Should this just go to FINISHED?
      }
   }
}


//------------------------------------------------------------------------------
// std::string DataFileCovHeader() const
//----------------------------------------------------------------------
/**
 * Write the name of the covariance type in header of the data file
 */
 //------------------------------------------------------------------------------
std::string ExtendedKalmanFilter::DataFileCovHeader() const
{
   return "SqrtCovariance";
}


//------------------------------------------------------------------------------
// void WriteCovarianceToDataFile()
//----------------------------------------------------------------------
/**
 * Writes the covariance to the data file in factorized form
 */
 //------------------------------------------------------------------------------
void ExtendedKalmanFilter::WriteCovarianceToDataFile()
{
   if (!sqrtP.IsSized())
   {
      sqrtP.SetSize(stateSize, stateSize);
      cf.Factor(*(stateCovariance->GetCovariance()), sqrtP);
      sqrtP = sqrtP.Transpose();
   }

   Rmatrix sqrtPOut = sqrtP;

   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   Rmatrix dX_dS = esm.CartToSolveForStateConversionDerivativeMatrix();
   Rmatrix cov = *(stateCovariance->GetCovariance());
   bool notCartesian = false;

   for (UnsignedInt ii = 0; ii < stateSize; ii++)
   {
      for (UnsignedInt jj = 0; jj < stateSize; jj++)
      {
         Real identityValue = (ii == jj) ? 1.0 : 0.0;

         if (dX_dS(ii, jj) != identityValue)
         {
            notCartesian = true;
            break;
         }
      }

      if (notCartesian)
         break;
   }

   if (notCartesian)
   {
      // If the solve for state is not Cartesian, convert covariance to cartesian
      // and find Cholesky decompsition
      cov = dX_dS * cov * dX_dS.Transpose();
      cf.Factor(cov, sqrtPOut);
      sqrtPOut = sqrtPOut.Transpose();
   }

   // Write covariance lower triangle
   for (UnsignedInt ii = 0; ii < stateSize; ii++)
   {
      Real conv = GetEpsilonConversion(ii);
      for (UnsignedInt jj = 0; jj <= ii; jj++)
      {
         Real value = sqrtPOut(ii, jj) * conv;

         std::string valueStr = GmatStringUtil::RealToString(value, false, true, true);
         dataFile << "," << valueStr;
      }
   }
}


//------------------------------------------------------------------------------
// void ReadCovarianceFromDataFile(StringArray header, StringArray restartData,
//                                 UnsignedInt firstStateIndex, IntegerArray stateColumnNum)
//----------------------------------------------------------------------
/**
 * Reads the covariance from the data file
 *
 * @param header The array of header elements.
 * @param restartData The array of data elements for the restart row.
 * @param firstStateIndex The column index of the first state element in restartData.
 * @param stateColumnNum The maping of the solve for states to the restart file states.
 */
 //------------------------------------------------------------------------------
void ExtendedKalmanFilter::ReadCovarianceFromDataFile(StringArray header, StringArray restartData,
                                                      UnsignedInt firstStateIndex, IntegerArray stateColumnNum)
{
   // Find column of first covariance element
   bool covFound = false;
   UnsignedInt firstCovIndex;
   for (UnsignedInt ii = 0U; ii < header.size(); ii++)
   {
      if (header[ii] == "SqrtCovariance_1_1")
      {
         covFound = true;
         firstCovIndex = ii;
         break;
      }
   }

   if (!covFound)
   {
      // If the EKF factorized covariance isn't found, try the default full covariance
      SeqEstimator::ReadCovarianceFromDataFile(header, restartData, firstStateIndex, stateColumnNum);
      return;
   }

   UnsignedInt fileStateSize = firstCovIndex - firstStateIndex;
   Integer stateAlignedIndex = -1;

   // Indicies in stateColumnNum must be in sequential order for factorized covariance to work
   for (UnsignedInt ii = 0; ii < fileStateSize && ii < stateColumnNum.size(); ii++)
   {
      if (stateColumnNum[ii] - firstStateIndex != ii)
         break;

      stateAlignedIndex = ii;
   }

   // Set covariance
   Rmatrix fileCov(fileStateSize, fileStateSize);
   UnsignedInt index = firstCovIndex;
   // Convert lower triangle array to lower triangle matrix 
   for (UnsignedInt ii = 0; ii < fileStateSize; ii++)
   {
      for (UnsignedInt jj = 0; jj <= ii; jj++)
      {
         Real value;
         GmatStringUtil::ToReal(restartData[index], value);
         fileCov(ii, jj) = value;

         index++;
      }
   }

   // Map file covariance matrix to state order of the covariance in the esm
   Covariance *cov = esm.GetCovariance();

   sqrtP.SetSize(esm.GetStateSize(), esm.GetStateSize());

   for (UnsignedInt ii = 0; ii < esm.GetStateSize(); ii++)
   {
      if (stateColumnNum[ii] >= 0)
      {
         Real conv = GetEpsilonConversion(ii);
         for (UnsignedInt jj = 0; jj <= ii; jj++)
         {
            if (stateColumnNum[jj] >= 0)
            {
               UnsignedInt idx1 = stateColumnNum[ii] - firstStateIndex;
               UnsignedInt idx2 = stateColumnNum[jj] - firstStateIndex;
               Real value = fileCov(idx1, idx2);
               value /= conv;
               sqrtP(ii, jj) = value;
            }
         }
      }
   }

   // If there are additional states being solved for that are not in the warm start file
   if (stateAlignedIndex + 1 < esm.GetStateSize())
   {
      Rmatrix covIn = fileCov * fileCov.Transpose();
      Rmatrix covInAll = (*(cov->GetCovariance()));

      for (UnsignedInt ii = 0; ii < fileStateSize && ii < stateColumnNum.size(); ii++)
      {
         if (stateColumnNum[ii] >= 0)
         {
            Real iiConv = GetEpsilonConversion(ii);
            for (UnsignedInt jj = 0; jj < fileStateSize && jj < stateColumnNum.size(); jj++)
            {
               if (stateColumnNum[jj] >= 0)
               {
                  Real jjConv = GetEpsilonConversion(jj);
                  UnsignedInt idx1 = stateColumnNum[ii] - firstStateIndex;
                  UnsignedInt idx2 = stateColumnNum[jj] - firstStateIndex;
                  Real value = covIn(idx1, idx2);
                  value /= iiConv * jjConv;
                  covInAll(ii, jj) = value;
               }
            }
         }
      }

      // Cholesky decomposition of the full covariance, including states not in the warm start file
      Rmatrix sqrtPFull(esm.GetStateSize(), esm.GetStateSize());
      cf.Factor(covInAll, sqrtPFull);
      sqrtPFull = sqrtPFull.Transpose();

      for (UnsignedInt ii = stateAlignedIndex + 1; ii < esm.GetStateSize(); ii++)
         for (UnsignedInt jj = ii; jj < esm.GetStateSize(); jj++)
            sqrtP(ii, jj) = sqrtPFull(ii, jj);
   }

   (*(cov->GetCovariance())) = sqrtP * sqrtP.Transpose();

   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   Rmatrix dX_dS = esm.CartToSolveForStateConversionDerivativeMatrix();
   bool notCartesian = false;

   for (UnsignedInt ii = 0; ii < esm.GetStateSize(); ii++)
   {
      for (UnsignedInt jj = 0; jj < esm.GetStateSize(); jj++)
      {
         Real identityValue = (ii == jj) ? 1.0 : 0.0;

         if (dX_dS(ii, jj) != identityValue)
         {
            notCartesian = true;
            break;
         }
      }

      if (notCartesian)
         break;
   }

   if (notCartesian)
   {
      // Convert covariance back to Keplerian
      Rmatrix dS_dX = dX_dS.Inverse();
      Rmatrix covMat = *(cov->GetCovariance());
      covMat = dS_dX * covMat * dS_dX.Transpose();

      for (UnsignedInt ii = 0; ii < esm.GetStateSize(); ii++)
         for (UnsignedInt jj = 0; jj < esm.GetStateSize(); jj++)
            (*cov)(ii, jj) = covMat(ii, jj);

      cf.Factor(covMat, sqrtP);
      sqrtP = sqrtP.Transpose();
   }
}


//------------------------------------------------------------------------------
// void UpdateCovarianceNominalValues(RealArray prevEpsilonConversions)
//----------------------------------------------------------------------
/**
 * Updates the nominal values of solve-fors which use epsilon values
 *
 * @param prevEpsilonConversions The array the previous epsilon conversion values.
 */
 //------------------------------------------------------------------------------
void ExtendedKalmanFilter::UpdateCovarianceNominalValues(RealArray prevEpsilonConversions)
{
   Covariance * cov = esm.GetCovariance();
   bool covChanged = false;

   if (!sqrtP.IsSized())
   {
      sqrtP.SetSize(esm.GetStateSize(), esm.GetStateSize());
      cf.Factor(*(cov->GetCovariance()), sqrtP);
      sqrtP = sqrtP.Transpose();
   }

   for (UnsignedInt ii = 0; ii < esm.GetStateSize(); ii++)
   {
      Real iiConv = GetEpsilonConversion(ii);

      if (iiConv != prevEpsilonConversions[ii])
      {
         for (UnsignedInt jj = 0; jj <= ii; jj++)
         {
            //Real jjConv = GetEpsilonConversion(jj);

            Real value = sqrtP(ii, jj);
            value *= prevEpsilonConversions[ii] / iiConv;
            sqrtP(ii, jj) = value;
            covChanged = true;
         }
      }
   }

   if (covChanged)
      (*(cov->GetCovariance())) = sqrtP * sqrtP.Transpose();
}


//------------------------------------------------------------------------------
// void CovarianceEpsilonConversion(Rmatrix& cov)
//------------------------------------------------------------------------------
/**
 * This method will convert the terms in the factorized covariance that require a
 * conversion from their epsilon value for reporting.
 *
 * @param cov The covariance matrix to convert
 */
 //------------------------------------------------------------------------------
void ExtendedKalmanFilter::SqrtCovarianceEpsilonConversion(Rmatrix& sqrtCov)
{
   for (UnsignedInt ii = 0; ii < stateSize; ii++)
   {
      Real conv = GetEpsilonConversion(ii);
      for (UnsignedInt jj = 0; jj <= ii; jj++)
      {
         sqrtCov(ii, jj) = sqrtCov(ii, jj) * conv;
      }
   }
}


//------------------------------------------------------------------------------
// void SetCovariance(const Rmatrix& cov)
//------------------------------------------------------------------------------
/**
 * Set the covariance for the filter
 *
 * @param cov The covariance to set
 */
 //------------------------------------------------------------------------------
void ExtendedKalmanFilter::SetCovariance(const Rmatrix& cov)
{
   SeqEstimator::SetCovariance(cov);

   sqrtP.SetSize(cov.GetNumRows(), cov.GetNumColumns()); // Zero out the matrix
   cf.Factor(*(esm.GetCovariance()->GetCovariance()), sqrtP);
   sqrtP = sqrtP.Transpose();
}


//------------------------------------------------------------------------------
// void ExtendedKalmanFilter::UpdateCov()
//------------------------------------------------------------------------------
/**
 * Updates the covariance without updating state or publishing
 */
 //------------------------------------------------------------------------------
void ExtendedKalmanFilter::UpdateCov()
{
   esm.MapObjectsToSTM();
   esm.MapObjectsToVector();
   TimeUpdate();
   (*(stateCovariance->GetCovariance())) = pBar;
   esm.MapCovariancesToObjects();
}


