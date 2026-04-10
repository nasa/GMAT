//$Id$
//------------------------------------------------------------------------------
//                              RungeKutta
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// *** File Name : RungeKutta.hpp
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
//                           : 1/15/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 09/24/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//
//                           : 10/21/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//                              Removals:
//                              Additions:
// **************************************************************************
/**
 * Base class for Adaptive Runge-Kutta integrators
 *
 *  The Runge-Kutta integration scheme is a single step method used to solve
 *  differential equations for \f$ n \f$ coupled variables of the form
 *
 *  \f[ {{dr^i}\over{dt}} = f(t,r) \f]
 *
 *  (The superscript in this discussion refers to the variables; hence \f$ f^i \f$
 *  is the \f$ i^{th} \f$ variable, and \f$ r^{(n)} \f$ refers to all \f$ n \f$
 *  variables.)  The method takes an integration step, \f$ h \f$, by breaking the
 *  interval into several stages (usually of smaller size) and calculating
 *  estimates of the integration result at each stage.  The later stages use the
 *  results of the earlier stages.  The cumulative effect of the integration is
 *  an approximate total step \f$ \delta t \f$, accurate to a given order in the
 *  series expansion of the differential equation, for the state variables
 *  \f$ r_i(t+\delta t) \f$ given the state \f$ r_i(t) \f$.
 *
 *  The time increment for a given stage is given as a multiple \f$ a_i \f$ of the
 *  total time step desired; thus for the \f$ i^{th} \f$ stage the interval used
 *  for the calculation is \f$ a_i \delta t \f$; the estimate of the integrated
 *  state at this stage is given by
 *
 *  \f[ k_i^{(n)} = \delta t f(t+a_i\delta t, r^{(n)}(t) +
 *                  \sum_{j=1}^{i-1}b_{ij}k_j^{(n)}) \f]
 *
 *  where \f$ b_{ij} \f$ contains a set of coefficients specific to the Runge-Kutta
 *  instance being calculated.  Given the results of the stage calculations, the
 *  total integration step can be calculated using another set of coefficients,
 *  \f$ c_j \f$ and the formula
 *
 *  \f[ r^{(n)}(t+\delta t) = r^{(n)}(t) + \sum_{j=1}^{stages}c_j k_j^{(n)} \f]
 *
 *  The error control for these propagators is implemented by comparing the
 *  results of two different orders of integration.  The difference between the
 *  two steps provides an estimate of the accuracy of the step; a second set of
 *  coefficients corresponding to this second integration scheme can be used to
 *  obtain a solution
 *
 *  \f[ r'^{(n)}(t+\delta t) = r^{(n)}(t) + \sum_{j=1}^{stages}c_j^* k_j^{*(n)} \f]
 *
 *  With care, the stage estimates \f$ k_j \f$ and \f$ k_j^* \f$ can be selected so
 *  that they are the same; in that case, the estimate of the error in the
 *  integration \f$ \Delta^{(n)} \f$ can be written
 *
 *  \f[ \Delta^{(n)} =
 *                  \left| \sum_{j=1}^{stages}(c_j - c_j^*) k_j^{(n)} \right| \f]
 *
 *  (The difference between the coefficients \f$ c_j - c_j^* \f$ is the array of
 *  error estimate coefficients (ee) in this code.)
 *
 *  Once the estimated error has been calculated, the size of the integration
 *  step can be adapted to a size more appropriate to the desired accuracy of the
 *  integration.  If the step results in a solution that is not accurate enough,
 *  the step needs to be recalculated with a smaller step size.  Labeling the
 *  desired accuracy \f$ \alpha \f$ and the obtained accuracy \f$ \epsilon \f$
 *  (calculated, for instance, as the largest element of the array \f$ \Delta \f$),
 *  the new step used by the Runge-Kutta integrator is
 *
 *  \f[ \delta t_{new}=
 *             \sigma\delta t\left({{\alpha}\over{\epsilon}}\right)^{1/(m-1)} \f]
 *
 *  where \f$ m \f$ is the order of truncation of the series expansion of the
 *  differential equations being solved.  The factor \f$ \sigma \f$ is a safety
 *  factor incorporated into the calculation to avoid unnecessary iteration over
 *  attempted steps.  Common practice is to set this factor to 0.9; that is the
 *  default value used in this implementation.
 *
 *  Similarly, if the step taken does not result in the desired accuracy, you may
 *  want to increase the step size parameter for the next integration step.  The
 *  new estimate for the desired stepsize is given by
 *
 *  \f[ \delta t_{new}=
 *             \sigma\delta t\left({{\alpha}\over{\epsilon}}\right)^{1/(m)} \f]
 *
 *  Sometimes you do not want to increase the stepsize in this manner; for example,
 *  you may want to keep the maximum step taken at some fixed value.  This
 *  implementation provides a mechanism for specifying a maximum allowed step.
 *
 *  Sometimes it is convenient to request steps of a specified size, regardless
 *  of the stepsize control algorithm or the calculation of the "best step"
 *  described above.  This implementation accomplishes that task by taking
 *  multiple error controlled steps is necessary to step across the requested
 *  interval.
 *
 *  Both of these features are implemented using the boolean flags described in
 *  the base class for the integrators.  See the documentation for the Integrator
 *  class for more information about these flags.
 */

#ifndef RungeKutta_hpp
#define RungeKutta_hpp

#include "gmatdefs.hpp"
#include "Integrator.hpp"

class GMAT_API RungeKutta : public Integrator
{
public:
    RungeKutta(Integer st, Integer order, const std::string &typeStr,
                           const std::string &nomme = "");
    virtual ~RungeKutta();
    RungeKutta(const RungeKutta&);
    RungeKutta & operator=(const RungeKutta&);

    virtual void SetPhysicalModel(PhysicalModel *pPhysicalModel);
    virtual bool Initialize();
    virtual bool Step();
    virtual bool Step(Real dt);
    virtual bool RawStep();

protected:
    /// The number of stages used to take an integration step
    Integer stages;
    /// The results of each stage are stored in this array
    Real ** ki;
    /// The portions of the total step taken for each stage
    Real * ai;
    /// The Runge-Kutta coefficients used to calculate the ki's
    Real ** bij;
    /// The array used to accumulate the results of the stage calculations
    Real * cj;

    /// The difference array used when calculating the error estimate
    Real * ee;
    /// A safety factor for the error control
    Real sigma;
    /// Exponent used to increase the stepsize for the next iteration
    Real incPower;
    /// Exponent used to decrease stepsize for this iteration (too much error)
    Real decPower;
    /// Accumulator for the intermediate states used in the stages
    Real * stageState;
    /// Candidate state for the step (used if the error is acceptable)
    Real * candidateState;


    bool SetupAccumulator();
    void ClearArrays();
    virtual Real EstimateError();
    virtual bool AdaptStep(Real maxerror);

    //------------------------------------------------------------------------------
    // virtual void SetCoefficients(void)
    //------------------------------------------------------------------------------
    /**
     * The function to set the coefficients for a given RK integrator
     *
     * This method, called from Initialize, sets the coefficients for a given
     * Runge-Kutta integrator.  Derived classes must allocate and initialize the
     * stages, ai, bij, cj, and ee.  The memory allocation is managed by the
     * Initialize() method.
     */
    //------------------------------------------------------------------------------
    virtual void SetCoefficients() = 0;
};

#endif  // RungeKutta_hpp
