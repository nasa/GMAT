//$Id$
//------------------------------------------------------------------------------
//                              Integrator
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
// *** File Name : Integrator.hpp
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
//                           : 2/19/2003 - D. Conway, Thinking Systems, Inc.
//                             Removed parms for fixed step mode
//
//                           : 3/3/2003 - D. Conway, Thinking Systems, Inc.
//                             Added error control threshold; moved time 
//                             granularity into this class
//
//                           : 09/26/2003 - W. Waktola, 
//                                          Mission Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//
//                           : 10/10/2003 - W. Waktola, 
//                                          Mission Applications Branch
//                              Changes:
//                                - SetParameter() to SetRealParameter()
//                                - GetParameter() to GetRealParameter()
//                                - virtual char* 
//                                  GetParameterName(const int parm) const to
//                                  virtual std::string 
//                                  GetParameterName(const int parm) const
//
//                           : 10/16/2003 - W. Waktola, 
//                                          Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//                              Removals:
//                                - GetParameterName(), replaced by 
//                                  GetParameterText()
//                              Additions:
//                                - IntegratorParamCount
//                                - PARAMTER_TEXT[]
//                                - PARAMETER_TYPE[]
//                                - GetIntegerParameter()
//                                - SetIntegerParameter()
//                                - GetParameterText()
//                                - GetParameterID()
//                                - GetParameterType()
//                                - GetParameterTypeString()
//
// **************************************************************************
/** \brief Base class for numerical integrators
 *
 * This class provides the interfaces needed to numerically integrate first 
 * order differential equations.  The basic equations that are solved must
 * have the form
 *
 * \f[ {{dr^i}\over{dt}} = f(t,r) \f]
 *
 * The right hand side of this equation, \f$ f(t,r) \f$, is provided by the 
 * PhysicalModel class instance that is passed to the Integrator.  Given this 
 * instance, the Integrator classes solve the equation above for 
 * \f$ r^i(t+\delta t) \f$, given \f$ r^i(t) \f$ and an integration step 
 * \f$ \delta t \f$.  
 *  
 * Most Integrators provide the capability to estimate the accuracy of the 
 * integration and adapt the stepsize accordingly.  The member variable 
 * tolerance is used to specify the desired integration accuracy.
 *
 * Sometimes you want to keep the maximum step taken at some fixed value.  This 
 * implementation provides a mechanism for specifying a maximum allowed step by
 * toggling the useMaxStep parameter.  It can be convenient to request steps of 
 * a specified size, regardless of the stepsize error control algorithm.  This 
 * class provides a Boolean flag (useFixedInterval) that can be used for that 
 * purpose, and a specialized stepping method (StepFixedInterval(dt))to take the
 * step.
 *
 * This class provides data structures that can be used to run in fixed step 
 * mode.  These parameters (fixedStep, fixedStepsize) are not used by this base
 * class; classes derived from this one use the parameters when implementing 
 * fixed step propagation.  The recommended approach is to code the Step() 
 * method so that if the integrator is running in fixed step mode, the system
 * takes multiple steps to achieve propagation across the requested 
 * (fixedStepsize) interval while maintaining the specified integration 
 * accuracy, as specified by the tolerance variable.
 */

#ifndef Integrator_hpp
#define Integrator_hpp

#include "gmatdefs.hpp"
#include "Propagator.hpp"

class GMAT_API Integrator : public Propagator
{
public:
    Integrator(const std::string &typeStr, const std::string &nomme = "");
    Integrator(const Integrator&);
    Integrator& operator=(const Integrator& i);
    virtual ~Integrator();
        
    // Parameter accessor methods -- overridden from GmatBase
    virtual std::string GetParameterText(const Integer id) const;
    virtual Integer GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;
    virtual bool IsParameterReadOnly(const Integer id) const;

    virtual Real GetRealParameter(const Integer id) const;
    virtual Real GetRealParameter(const std::string &label) const;
    virtual Real SetRealParameter(const Integer id, const Real value);
    virtual Real SetRealParameter(const std::string &label, const Real value);
    virtual Integer GetIntegerParameter(const Integer id) const;
    virtual Integer GetIntegerParameter(const std::string &label) const;
    virtual Integer SetIntegerParameter(const Integer id, const Integer value);
    virtual Integer SetIntegerParameter(const std::string &label, const Integer value);
    virtual bool    GetBooleanParameter(const Integer id) const;
    virtual bool    SetBooleanParameter(const Integer id, const bool value);

    virtual bool         TakeAction(const std::string &action,
                                    const std::string &actionData = "");

    virtual void SetPhysicalModel(PhysicalModel *pPhysicalModel);
    
    virtual Real GetStepTaken();

    virtual bool UsesErrorControl();
    
    //--------------------------------------------------------------------------
    // virtual void Initialize()
    //--------------------------------------------------------------------------
    /**
     * Method used to initialize the integrator
     * 
     * Each integrator will have class specific work to do to setup the initial
     * data structures and data elements.  This method is provided for that 
     * purpose.  At a minimum, be sure to call the Propagator::Initialize() 
     * method.
     *
     * @return true upon successful initialization.
     */
    //--------------------------------------------------------------------------
    virtual bool Initialize() = 0;
    
    virtual bool RawStep() = 0;
    virtual Integer GetPropagatorOrder(void) const; 

protected:

    //------------------------------------------------------------------------------
    // virtual Real EstimateError()
    //------------------------------------------------------------------------------
    /**
     * Interface used to estimate the error in the current step
     * 
     * This method definition is provided to make the interface to the error 
     * estimation routine consistent for all integrators.  The method should 
     * calculate a local estimate of the error from the integration and return 
     * the largest error estimate found.  There are several alternatives that 
     * users of this class can implement: the error could be calculated based on
     * the largest error in the individual components of the state vector, as 
     * the magnitude of the state vector (that is, the L2 (rss) norm of the 
     * error estimate vector).  The estimated error should never be negative,
     * so a return value less than 0.0 can be used to indicate an error 
     * condition.
     *       
     * For convenience, a member array pointer, errorEstimates, is provided in 
     * this class.  This array is not initialized -- derived classes take 
     * responsibility for allocating and freeing the memory for this array based 
     * on their estimation needs.
     * 
     * @return  The largest error found in the estimate
     */
    //------------------------------------------------------------------------------
    virtual Real EstimateError() = 0;

    //------------------------------------------------------------------------------
    // virtual bool AdaptStep(Real maxerror)
    //------------------------------------------------------------------------------
    /**
     * Interface used to change the stepsize of the current step 
     *     
     * This method adapts the step to a step compatible with the desired 
     * integration accuracy.  The implementation of this method is algorithm 
     * specific.  Implementors should be sure to use the minimumStep and 
     * maximumStep parameters to ensure that the stepsize does not grow or 
     * shrink without bounds, and thus escape from the domain of interest for 
     * the system.
     * 
     * @param maxerror  The largest error found in the propagation step.
     * 
     * @return  true if the step was adapted correctly, false otherwise
     */
    //------------------------------------------------------------------------------
    virtual bool AdaptStep(Real maxerror) = 0;
        
    enum
    {
        ACCURACY = PropagatorParamCount,  // Accuracy parameter for Integrators
        ERROR_THRESHOLD,                  // Accuracy parameter for Integrators
        SMALLEST_INTERVAL, // Smallest time interval -- used to hedge fixed step mode
        MIN_STEP,          // Minimum stepsize for the Integrator -- smaller steps fail
        MAX_STEP,          // Maximum stepsize for the Integrator -- larger steps get truncated
        MAX_STEP_ATTEMPTS, // Number of attempts to take before giving up
        STOP_IF_ACCURACY_VIOLATED,
        IntegratorParamCount
    };
    
    // Start with the parameter IDs and associates strings
    static const std::string 
                      PARAMETER_TEXT[IntegratorParamCount - PropagatorParamCount];
    static const Gmat::ParameterType 
                      PARAMETER_TYPE[IntegratorParamCount - PropagatorParamCount];
        
    /// The level of "acceptable" relative error for the integrator
    Real tolerance;
    /// Flag used to activate fixed step mode
    bool fixedStep;
    /// Step to take in fixed step mode
    Real fixedStepsize;
    /// Minimum allowed step to take (always positive - sign handled elsewhere)
    Real minimumStep;
    /// Maximum allowed step to take (always positive - sign handled elsewhere)
    Real maximumStep;
    /// Accuracy of the time step interval
    Real smallestTime;
    /// Number of failed attempts tried
    Integer stepAttempts;
    /// Number of failed attempts allowed before reporting failure
    Integer maxStepAttempts;
    /// Flag indicating whether or not execution should stop if/when the accuracy is violated
    bool    stopIfAccuracyViolated;
    /// Flag indicating whether or not the warning for the accuracy violation has already been
    /// written, for this integrator, for this run
    bool    accuracyWarningTriggered;
    /// String used to indicate object type in some warning messages
    std::string typeSource;
    /// Actual interval taken by the step
    Real stepTaken;
    /// Remaining time for a specified or fixed timestep
    Real timeleft;
    /// Derivative array pointer (obtained from the PhysicalModel instance)
    const Real * ddt;
    /// An array of the error estimates, sized by the dimension of the system
    Real * errorEstimates;
    /// An array of the error estimates, sized by the dimension of the system
    Real errorThreshold;
    /// Indicator for the integrator derivative order -- 2 for Nystrom methods
    Integer derivativeOrder;
    /// Flag indicating whether integrator has error control
    bool hasErrorControl;
};

#endif
