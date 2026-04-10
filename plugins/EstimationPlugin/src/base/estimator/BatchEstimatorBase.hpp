//$Id: BatchEstimatorBase.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         BatchEstimatorBase
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
// Created: 2009/08/04
//
/**
 * Definition of the BatchEstimatorBase class
 */
//------------------------------------------------------------------------------


#ifndef BatchEstimatorBase_hpp
#define BatchEstimatorBase_hpp

#include "Estimator.hpp"
//#include "PropSetup.hpp"
//#include "MeasurementManager.hpp"

/**
 * Implementation of a standard batch estimation state machine
 *
 * This class provides a batch estimation state machine that follows a typical
 * batch estimation process.  Derived classes override specific methods to
 * implement the math required for their algorithm.  Every derived estimator
 * must implement the Accumulate and Estimate methods.  The other methods called
 * in the finite state machine provide default implementation that can be
 * overridden as  needed.
 *
 * BatchEstimatorBase is an abstract class; a derived class is needed to instantiate
 * a BatchEstimatorBase
 */
class ESTIMATION_API BatchEstimatorBase : public Estimator
{
public:
   BatchEstimatorBase(const std::string &type, const std::string &name);
   virtual ~BatchEstimatorBase();
   BatchEstimatorBase(const BatchEstimatorBase& est);
   BatchEstimatorBase& operator=(const BatchEstimatorBase& est);

   virtual bool         Initialize();
   virtual SolverState  AdvanceState();
   virtual bool         Finalize();

   virtual bool         IsFinalPass();

   // methods overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                        const Real value);

   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

protected:
   /// Tolerance measure applied to RMS state change to test for convergence
   Real                    absoluteTolerance;
   /// Tolerance measure applied to RMS residual change to test for convergence
   Real                    relativeTolerance;

   /// RMS residual value from the previous pass through the data
   Real                    oldResidualRMS;
   /// RMS residual value from the current pass through the data
   Real                    newResidualRMS;
   /// The best RMS residual
   Real                    bestResidualRMS;
   Real                    resetBestResidualRMS;
   /// Predicted RMS residual
   Real predictedRMS;
   /// Number consecutive iterations diverging
   Integer numDivIterations;
   /// Flag indicating to reset best RMS when estimation is diverged
   bool resetBestRMSFlag;

   /// Flag set when an a priori estimate is available
   bool                    useApriori;
   /// The most recently computed state vector changes
   RealArray               dx;
   /// The weighting matrix used when accumulating data
   Rmatrix                 weights;
   /// Flag used to indicate propagation to estimation epoch is executing
   bool                    advanceToEstimationEpoch;

   /// Estimation status
   Integer                 estimationStatus;
   /// String to show reason of convergence
   std::string             convergenceReason;

   /// Buffer of the participants for the outer batch loop
   ObjectArray             outerLoopBuffer;
   /// Inversion algorithm used 
   std::string             inversionType;

   /// Maximum consecutive divergences
   Integer                 maxConsDivergences;

   /// Freeze measurement editing option
   bool                    freezeEditing;
   Integer                 freezeIteration;
   
   //makes sure this class initializes before changing state
   bool						hasInitialized;

   //// Statistics information for sigma edited records
   //IntegerArray sumSERecords;               // total all sigma edited records
   //RealArray    sumSEResidual;              // sum of all O-C of all sigma edited records
   //RealArray    sumSEResidualSquare;        // sum of all (O-C)^2 of  all sigma edited records
   //RealArray    sumSEWeightResidualSquare;  // sum of all [W*(O-C)]^2 of all sigma edited records

   /// Parameter IDs for the BatchEstimatorBases
   enum
   {
      ABSOLUTETOLERANCE = EstimatorParamCount,
      RELATIVETOLERANCE,
      USE_INITIAL_COVARIANCE,
      INVERSION_ALGORITHM,
      MAX_CONSECUTIVE_DIVERGENCES,
      RESET_BEST_RMS,
      FREEZE_MEASUREMENT_EDITING,
      FREEZE_ITERATION,
      CONVERGENT_STATUS,
      BatchEstimatorBaseParamCount,
   };

   /// Strings describing the BatchEstimatorBase parameters
   static const std::string
                           PARAMETER_TEXT[BatchEstimatorBaseParamCount -
                                              EstimatorParamCount];
   /// Types of the BatchEstimatorBase parameters
   static const Gmat::ParameterType
                           PARAMETER_TYPE[BatchEstimatorBaseParamCount -
                                              EstimatorParamCount];

   virtual void            CompleteInitialization();
   virtual void            FindTimeStep();
   virtual void            CalculateData();
   virtual void            ProcessEvent();
   virtual void            CheckCompletion();
   virtual void            RunComplete();

   // Abstract methods defined in derived classes
   /// Abstract method that implements accumulation in derived classes
   virtual void            Accumulate() = 0;
   virtual bool            OverwriteEditFlag(const std::string &editFlag);
   virtual bool            WriteEditFlag();

   virtual Integer         TestForConvergence(std::string &reason);

   // progress string for reporting
   virtual std::string    GetProgressString();

   virtual void           InvertApriori(Rmatrix &Pdx0_inv) const;

   virtual void           WriteReportFileHeaderPart2b();

   virtual void           WriteIterationHeader();

   virtual bool           IsIterative();

   virtual std::string    GetHeaderName();

   virtual void           AddMatlabData(const MeasurementInfoType &measStat);
   virtual void           AddJsonData(const MeasurementInfoType &measStat);
   virtual void           AddMatlabData(const MeasurementInfoType &measStat,
                                        DataBucket &matData, IntegerMap &matIndex);
   virtual void           AddMatlabConfigData();
};

#endif /* BatchEstimatorBase_hpp */
