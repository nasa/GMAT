//$Id: BatchEstimator.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         BatchEstimator
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
 * Batch least squares estimator using direct inversion
 */
//------------------------------------------------------------------------------


#ifndef BatchEstimator_hpp
#define BatchEstimator_hpp


#include "BatchEstimatorBase.hpp"
//#include "PropSetup.hpp"
//#include "MeasurementManager.hpp"


/**
 * A simple batch least squares estimator.
 *
 * This estimator implements the algorithm described in Tapley, Schutz and Born,
 * Statistical Orbit Determination (2004), chapter 4, as illustrated in the
 * flowchart on pages 196-197.  The normal equations are solved through direct
 * inversion of the information matrix.
 */
class ESTIMATION_API BatchEstimator: public BatchEstimatorBase
{
public:
   BatchEstimator(const std::string &name);
   virtual ~BatchEstimator();
   BatchEstimator(const BatchEstimator& est);
   BatchEstimator& operator=(const BatchEstimator& est);

   virtual GmatBase*       Clone() const;
   virtual void            Copy(const GmatBase*);

   // methods overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

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

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);

protected:
   /// Parameters associated with the BatchEstimators
   enum
   {
      MAX_RESIDUAL_MULTIPLIER  = BatchEstimatorBaseParamCount,
      CONSTANT_MULTIPLIER,
      ADDITIVE_CONSTANT,
      USE_RMSP,
      ENABLE_ILSE,
      CONSTANT_MULTIPLIER_ILSE,
      MAX_ITERATIONS_ILSE,
      BatchEstimatorParamCount
   };

   /// String identifiers for the parameters
   static const std::string    PARAMETER_TEXT[BatchEstimatorParamCount -
                                              BatchEstimatorBaseParamCount];
   /// Types of the parameters
   static const Gmat::ParameterType
                               PARAMETER_TYPE[BatchEstimatorParamCount -
                                              BatchEstimatorBaseParamCount];

   /// Parameters for data sigma editting
   Real maxResidualMult;
   Real constMult;
   Real additiveConst;
   /// Flag to indicate to use weightedRMS or predictedRMS for OLSE
   bool chooseRMSP;
   /// Inner Loop parameters
   bool useInnerLoop;
   Real constMultIL;
   Integer maxIterationsIL;
   Integer iterationsTakenIL;

   /// Inner Loop status
   enum InnerLoopStatus
   {
      IL_UNKNOWN,
      IL_SAME_EDITS_CONVERGED,
      IL_MAX_ITERATIONS_DIVERGED,
   };

   InnerLoopStatus estimationStatusIL;

   virtual void            CompleteInitialization();
   virtual void            Accumulate();
   virtual void            Estimate();
   virtual void            InnerLoop();
   virtual void            SolveNormalEquations(const Rmatrix &infMatrix, Rmatrix &covMatrix);

   virtual bool            DataFilter();
   virtual void            EstimationPartials(std::vector<RealArray> &hMeas);

   Real                    CalculateWRMS(const UnsignedIntArray &measurementList) const;
   Real                    CalculateWRMS(const UnsignedIntArray &measurementList, const RealArray &dx) const;
   Real                    CalculateResidualChange(const RealArray &hAccum, const RealArray &dx) const;

   virtual void            WriteReportFileHeaderPart6(); // Contains estimator-specific options

   virtual void            WriteReportFileSummaryPart1(Solver::SolverState sState);
};

#endif /* BatchEstimator_hpp */
