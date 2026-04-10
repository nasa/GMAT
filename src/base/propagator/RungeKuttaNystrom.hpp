//$Id$
//------------------------------------------------------------------------------
//                              RungeKuttaNystrom
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
// *** File Name : rungekuttanystrom.h
// *** Created   : January 21, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-67521-G                             ***
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
// Modification History      : 1/21/03 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//                                    Changes
//                           : 02/21/2004 - L. Ruley, Missions Applications Branch
//                                            - Updated style using GMAT cpp style guide
//                                            - All Real types to Real types
//                            - All int types to Integer types
// **************************************************************************

#ifndef RungeKuttaNystrom_hpp
#define RungeKuttaNystrom_hpp

#include "gmatdefs.hpp"
#include "RungeKutta.hpp"

/** \brief Base class for Adaptive Runge-Kutta-Nystrom integrators

  The Runge-Kutta-Nystrom integration scheme is an adaptation of the 
  Runge-Kutta integration method taylored to work with second order 
  differential equations.  The equations solved by this integration class have
  the form

  \f[ {{d^2\vec r^i}\over{dt^2}} = \vec g(t,\vec r) \f]

  (The superscript in this discussion refers to the variables; hence \f$ f^i \f$
  is the \f$ i^{th} \f$ variable, and \f$ r^{(n)} \f$ refers to all \f$ n \f$ 
  variables.)  The method works similarly to the Runge-Kutta method; refer to 
  that discussion for details of the methodology.  Note that the equation of 
  motion given here is independent of teh first dervative information.  This is
  a necessary condition for Nystrom methods (though small contributions from 
  first derivative terms can be accommodated with care).

  The Nystrom integrators use the following equations to propagate the state 
  variables.  The intermediate stages are given by

  \f[ \vec k_i = \vec g\left(t_0 + a_i h, \vec r_0 + a_i h \vec{\dot r_0} + 
                  h^2 \sum_{j=0}^{i-1}b_{ij} \vec k_j\right) \f]

  These stage calculations are then accumulated to give the estimated state 
  after time \f$ h \f$:

  \f[ \vec r(t_0 + h) = \vec r(t_0) + \sum_{i=0}^{n-1} c_j \vec k_j \f]

  where \f$ n \f$ is the number of stages set for the integrator.  The state 
  parameters for the first derivative data (e.g. the velocity data) are given
  using a separate set of coefficients:

  \f[ \vec{\dot r}(t_0 + h) = \vec{\dot r}(t_0) + 
                             \sum_{i=0}^{n-1} \dot c_j \vec k_j \f]

  The Runge-Kutta-Nystrom integrators need to know the mapping between the 
  state variables and their first derivatives in order to calculate the 
  intermediate stages.  The mapping data is obtained from the associated 
  PhysicalModel class pointer (phys) through a call to GetComponentMap().
 */
class GMAT_API RungeKuttaNystrom : public RungeKutta
{
public:
//    RungeKuttaNystrom(int st, int order);
    RungeKuttaNystrom(Integer st, Integer order, const std::string &typeStr,
                           const std::string &nomme = "");
    virtual ~RungeKuttaNystrom(void);

    RungeKuttaNystrom(const RungeKuttaNystrom& rkn);
    RungeKuttaNystrom&      operator=(const RungeKuttaNystrom& rkn);

    virtual bool            Initialize();
    virtual bool            Step();
    virtual bool            Step(double dt);
    virtual bool            RawStep();

//    virtual Propagator* Clone() const;
    virtual Integer         GetPropagatorOrder() const;

protected:
    /// Array of coefficients for advancing 1st derivatives (e.g. velocities)
    Real                * cdotj;
    /// Mapping between components and their derivatives
    Integer             * derivativeMap;
    /// Mapping between derivatives and their components 
    Integer             * inverseMap;
    /// Flag indicating if error estimates are available for derivative terms
    bool                  derivativeError;
    /// Coefficients for derivative error calculations
    Real                * eeDeriv;

    virtual Real          EstimateError(void);
};

#endif // RungeKutta89_hpp


