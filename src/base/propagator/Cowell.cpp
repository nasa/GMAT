//$Id$
// **************************************************************************
// GMAT: General Mission Analysis Tool
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
// Author: Waka Waktola
// Created: 2005/05/16
//
/**
 * @note This code is a shell for the Cowell integrator.  The integrator is not
 *       currently implemented.
 */
// **************************************************************************

#include "Cowell.hpp"

//---------------------------------
// static data
//---------------------------------
//VC++ error C2466: cannot allocate an array of constant size 0
// so commented out for possible later use
//const std::string
//Cowell::PARAMETER_TEXT[CowellParamCount - IntegratorParamCount] = {};
//const Gmat::ParameterType
//Cowell::PARAMETER_TYPE[CowellParamCount - IntegratorParamCount] = {};

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// Cowell(void)
//------------------------------------------------------------------------------
/**
 * The Cowell constructor
 */
//------------------------------------------------------------------------------
Cowell::Cowell(const std::string &typeStr, const std::string &nomme) :
    PredictorCorrector     (?, ?, "Cowell", nomme)
{
    parameterCount = CowellParamCount;
}

//------------------------------------------------------------------------------
// ~Cowell(void)
//------------------------------------------------------------------------------
/**
 * The Cowell destructor
 */
//------------------------------------------------------------------------------
Cowell::~Cowell(void)
{
}

//------------------------------------------------------------------------------
// Cowell(const Cowell& bs)
//------------------------------------------------------------------------------
/**
 * The Cowell copy constructor
 */
//------------------------------------------------------------------------------
Cowell::Cowell(const Cowell& cw) :
   PredictorCorrector     (cw)
{
    parameterCount = CowellParamCount;
}

//------------------------------------------------------------------------------
// Cowell::operator=(const Cowell& bs)
//------------------------------------------------------------------------------
/**
 * The Cowell assignment operator
 */
//------------------------------------------------------------------------------
Cowell& Cowell::operator=(const Cowell& cw)
{
    if (this == &cw)
        return *this;

    Integrator::operator= (cw);
    
    initialized = false;
    
    return *this;
}

//------------------------------------------------------------------------------
// Clone(void) const
//------------------------------------------------------------------------------
/**
 * Used to create a copy of the object.
 * This method returns a copy of the current instance.  The copy has all of the
 * parameters set to the current values, but is not yet initialized becuse the 
 * PhysicalModel pointer is not set, nor are the internal arrays that depend on 
 * the PhysicalModel allocated.
 *
 * @return <Propagator*>
 */
//------------------------------------------------------------------------------
GmatBase* Cowell::Clone(void) const
{
    return new Cowell(*this);
}

//------------------------------------------------------------------------------
// Initialize(void)
//------------------------------------------------------------------------------
/**
 * Method used to initialize the Cowell integrator.  This method sets up 
 * (or resets) the data arrays used by the Cowell integrator.  
 */
//------------------------------------------------------------------------------
bool Cowell::Initialize(void)
{
    Propagator::Initialize();
    initialized = false;
    
    if (physicalModel)
        {
        dimension = physicalModel->GetDimension();
        estimatedState = new Real[dimension];
    } 
    
    return initialized;
}

//------------------------------------------------------------------------------
//  Step(Real dt)
//------------------------------------------------------------------------------
/**
 * This method drives the Cowell propagation.
 *
 * @return retval
 */
//------------------------------------------------------------------------------
bool Cowell::Step(Real dt)
{
    if (!initialized)
        return false;

    bool retval = true;

    return retval;
}

//------------------------------------------------------------------------------
//  Step(void)
//------------------------------------------------------------------------------
/**
 * Method to propagate by the default step.  This method drives the
 * Cowell propagation.  The high-level procedure is similar to other
 * integrators; it goes as follows:
 * <OL>
 * <LI>Initialize the data structures for the integration</LI>
 * <LI>Set to evaluate the first level of the extrapolation tableau</LI>
 * <LI>Use the modified midpoint method to extrapolate the estimated state for 
 * the current level (MidpointMethod())</LI>
 * <LI>Perform the polynomial fit  in order to generate the estimated state at
 * this level (PolyExtrapolate())</LI>
 * <LI>Estimate the error at the current level (EstimateError())</LI>
 * <LI>If the estimated error is less than the tolerance, accept the estimated 
 * state and adapt the parameters to take a larger step next time through 
 * (AdaptStep()); go to step 8</LI>
 * <LI>If the level is not at the maximum value, increment it and go to 
 * step 3</LI>
 * <LI>If the error is too large, reset the parameters to either take a smaller
 * step or go deeper into the extrapolation table (AdaptStep()), and then go to 
 * step 2</LI>
 * <LI>The integration was successful.  Copy the estimated state into the output 
 * state, and increment the time parameter on the PhysicalModel</LI>
 * </OL>
 * 
 * See the descriptions for each of the called methods for further information 
 * about these steps.
 *
 * @return boolean
 */
//------------------------------------------------------------------------------
bool Cowell::Step(void)
{
    if (!initialized)
        return false;
        
    return true;
}

//------------------------------------------------------------------------------
//  RawStep(void)
//------------------------------------------------------------------------------
/**
 * Method to take an uncontrolled Cowell step
 * The current implementation just calls Step(), so the step is error 
 * controlled.
 *
 * @return boolean
 */
//------------------------------------------------------------------------------
bool Cowell::RawStep(void)
{
    return Step();
}

//------------------------------------------------------------------------------
//  EstimateError(void)
//------------------------------------------------------------------------------
/**
 * Error estimation calculations for the Cowell Integrator.
 *
 * @return absolute error estimate
 */
//------------------------------------------------------------------------------
Real Cowell::EstimateError(void)
{
    return physicalModel->EstimateError(errorEstimates, estimatedState);
}

//------------------------------------------------------------------------------
//  AdaptStep(Real maxerror)
//------------------------------------------------------------------------------
/**
 * Stepsize adaptation for the Cowell Integrator.
 * 
 * @param maxerror  The largest relative error estimate found in the integration
 * @return boolean
 */
//------------------------------------------------------------------------------
bool Cowell::AdaptStep(Real maxerror)
{
    return true;
}    

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Helper method used to find the text name for the class parameters.
 * This static method is used to obtain text descriptions for the parameters 
 * defined by the class.  These strings can be used to fill in the descriptors
 * for user interface elements, so that those elements have labels that match 
 * the intent of each of the user selectable parameters.
 * 
 * @param <id>  ID of the parameter that needs the text string
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string Cowell::GetParameterText(const Integer id) const
{
    return Integrator::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer Cowell::GetParameterID(const std::string &str) const
{
    return Integrator::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Cowell::GetParameterType(const Integer id) const
{
    return Integrator::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string Cowell::GetParameterTypeString(const Integer id) const
{
   return Integrator::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real Cowell::GetRealParameter(const Integer id) const
{
    return Integrator::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real Cowell::GetRealParameter(const std::string &label) const
{
    Integer id = GetParameterID(label);

    return GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Method used to Set parameters for the BS Integrator.
 * This method sets the tolerance and tolerance limit for the Bulirsch-Stoer 
 * integrator, and calls Integrator::SetParameter() for other parameters.  The
 * implementation here prevents setting tolerances of more than 100%.  If the
 * minimum tolerance is set to a value larger than the current tolerance, the
 * current value is raised to the minimum value.
 * 
 * @param <id>  ID for the parameter being set
 * @param <value>  New value for the parameter
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real Cowell::SetRealParameter(const Integer id, const Real value)
{
    return Integrator::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real Cowell::SetRealParameter(const std::string &label, const Real value)
{
    return SetRealParameter(GetParameterID(label), value);
}
