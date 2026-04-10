//$Id: BulirschStoer.hpp 10412 2012-04-26 21:12:51Z djcinsb $
//------------------------------------------------------------------------------
//                             BulirschStoer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// *** File Name : BulirschStoer.hpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 1/7/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 06/15/2004 - W. Waktola, Missions Applications Branch
//                                               Changes:
//                                               - Updated style using GMAT cpp style guide
//                                               - All double types to Real types
//                                               - All int types to Integer types
//                               - Added Parameter accessor methods
//                           : 06/22/2004 - W. Waktola, Missions Applications Branch
//                                               Removals:
//                                               - SetParameter()
//                               - GetParameter()
//                               - GetParameterName()
//                               - GetType()
//                               - BULIRSCHSTOER_MINIMUMTOLERANCE
// **************************************************************************

#ifndef BulirschStoer_hpp
#define BulirschStoer_hpp

#include "ExtraPropagatorDefs.hpp"
#include "Integrator.hpp"

/**
 * The Bulirsch-Stoer integrator from Numerical Recipes in C.
 * This class implements the Gragg-Bulirsch-Stoer integration method as 
 * described in Numerical Recipes in C.  The basic algorithm uses a modified-
 * midpoint algorithm, evaluated at intermediate points in the interval of 
 * interest, to extrapolate the next state vector from the current state.  This 
 * extrapolation is performed for several different sized of substeps, and the 
 * resulting states are used to estimate the results as the stepsize of the 
 * substeps is reduced to zero.
 */
class PROPAGATOR_API BulirschStoer : public Integrator
{
public:
   BulirschStoer(const std::string &nomme = "");
   virtual ~BulirschStoer(void);
   BulirschStoer(const BulirschStoer& bs);
   BulirschStoer& operator=(const BulirschStoer& bs);
    
   virtual GmatBase* Clone(void) const;
    
   virtual bool Initialize();
   bool         SetMaximumDepth(Integer d);
   virtual bool Step(Real dt);
   virtual bool Step(void);
   virtual bool RawStep(void);
   virtual bool MidpointMethod(Integer substeps);
   virtual bool PolyExtrapolate(void);
   Real EstimateError(void);
   bool AdaptStep(Real maxerror);

   // Parameter accessor methods -- overridden from GmatBase
   virtual std::string         GetParameterText(const Integer id) const;
   virtual Integer             GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string         GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual Real    GetRealParameter(const Integer id) const;
   virtual Real    GetRealParameter(const std::string &label) const;
   virtual Real    SetRealParameter(const Integer id, const Real value);
   virtual Real    SetRealParameter(const std::string &label, const Real value);
        
protected:
   enum
   {
      MINIMUM_REDUCTION = IntegratorParamCount,
      MAXIMUM_REDUCTION,
      MIN_TOLERANCE,
      BulirschStoerParamCount  /// Count of the parameters for this class
   };
   static const std::string PARAMETER_TEXT[BulirschStoerParamCount - IntegratorParamCount];
   static const Gmat::ParameterType PARAMETER_TYPE[BulirschStoerParamCount - IntegratorParamCount];

private:
   /// The maximum depth to go in the extrapolation
   Integer depth;
   /// Flag  indicating status of the arrays sized by depth
   bool depthInitialized;
   /// Current level (depth to go in "this" step) for the extrapolation
   Integer level;
   /// Maximum error encountered at each level
   Real *levelError;
   /// Work coefficients for the algorithm
   Real *ai;
   /// \f$\alpha(k,q)\f$ coefficients from the algorithm
   Real **alpha;
   /// Intermediate states; corresponds to Numerical Recipes' d[][] array
   Real **intermediates;
   /// C array in the polynomial extrapolator
   Real *coeffC;
   /// Array of substep sized used in the computations, squared
   Real *intervals;
   /// The midpoint method states
   Real **estimates;
   /// Working states for the midpoint method
   Real *mstate;
   /// Working states for the midpoint method
   Real *nstate;
   /// The estimated state
   Real *estimatedState;
   /// Array containing number of subintervals at each level
   Integer *subinterval;
//   /// Minimum tolerance permitted
//   Real mintolerance;
   /// Safety factor used when estimating error
   Real bs_safety1;
   /// Safety factor used to prevent thrashing on the stepsize
   Real bs_safety2;
   /// Smallest change in the stepsize when cutting it back
   Real minimumReduction;
   /// Largest change in the stepsize when cutting it back
   Real maximumReduction;
   /// The largest increase in stepsize is 1.0 / SCALE_DT
   Real scale_dt;
   /// Estimated optimal row for convergence
   Integer kopt;
   /// Maximum row allowed for convergence
   Integer kmax;
   /// Used depth for the current estimate
   Integer kused;
   /// Flag used to mark first call to the stepper
   bool first;
/// Limit on the smallest tolerance for the BS integrator
//const Integer BULIRSCHSTOER_MINIMUMTOLERANCE;
//    /// Minimum allowed stepsize
//    Real minstep;
//    /// Maximum allowed stepsize (set to 0.0 for no limit)
//    Real maxstep;
};
#endif
