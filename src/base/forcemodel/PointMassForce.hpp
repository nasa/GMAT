//$Id$
//------------------------------------------------------------------------------
//                             PointMassForce
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
// *** File Name : PointMassForce.hpp
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
//                           : 1/8/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 1/21/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces to support Nystrom code
//
//                           : 10/01/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//
//                           : 10/20/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//                                - GetParameter to GetRealParameter
//                                - SetParameter to SetRealParameter
//                              Removals:
//                                - GetParameterName()
//                              Additions:
//                                - PARAMTER_TEXT[]
//                                - PARAMETER_TYPE[]
//                                - GetParameterText()
//                                - GetParameterID()
//                                - GetParameterType()
//                                - GetParameterTypeString()
//                                - GetRealParameter()
//                                - SetRealParameter()
// **************************************************************************

#ifndef PointMassForce_hpp
#define PointMassForce_hpp

#include "gmatdefs.hpp"
#include "PhysicalModel.hpp"
#include "CelestialBody.hpp"
#include "Rvector6.hpp"

class GMAT_API PointMassForce : public PhysicalModel
{
public:
    
   PointMassForce(const std::string &name = "");
   virtual ~PointMassForce();
   PointMassForce(const PointMassForce& pmf);
   PointMassForce& operator= (const PointMassForce& pmf);

   bool GetDerivatives(Real *state, Real dt = 0.0, Integer order = 1, 
         const Integer id = -1);
   bool GetComponentMap(Integer * map, Integer order) const;
   bool Initialize();
   virtual Real EstimateError(Real *diffs, Real *answer) const;
   virtual Rvector6 GetDerivativesForSpacecraft(Spacecraft *sc);

   //CelestialBody* GetBody();  // wcs: 2004/06/21 moved to PhysicalModel
   //std::string GetBodyName(); //loj: 5/7/04 added

   //void SetBody(CelestialBody *body);
   //void SetBodyName(const std::string &name); //loj: 5/7/04 added

   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   // inherited methods from GmatBase
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual bool        IsParameterReadOnly(const Integer id) const;

   virtual Real GetRealParameter(const Integer id) const;
   virtual Real SetRealParameter(const Integer id, const Real value);    
    
   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &value) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);
   virtual bool        SetStringParameter(const std::string &label,
                                          const std::string &value);
   virtual bool           GetBooleanParameter(const Integer id) const;
   virtual bool           SetBooleanParameter(const Integer id,
                                              const bool value);

   // Methods used by the ODEModel to set the state indexes, etc
   virtual bool SupportsDerivative(Gmat::StateElementId id);
   virtual bool SetStart(Gmat::StateElementId id, Integer index, 
                         Integer quantity, Integer totalSize);
   virtual Rvector3 GetTorquesForSpacecraft(Spacecraft *sc);

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   // Parameter IDs
   enum
   {
      MU = PhysicalModelParamCount,
      /// ID for the radius, used by PointMass and PointMassForce instances
      RADIUS,
      /// Flattening factor ID, used by PointMass and PointMassForce instances
      //flatteningParameter,  
      /// Polar radius ID (read only), used by PointMass and PointMassForce instances
      //poleRadiusParameter,  
      /// Error estimate type: 1.0 for component estimate, 2.0 for vector
      ESTIMATE_METHOD,  
      PRIMARY_BODY,
      //BODY,    
      PointMassParamCount
   };

   static const std::string PARAMETER_TEXT[PointMassParamCount - PhysicalModelParamCount];
   static const Gmat::ParameterType PARAMETER_TYPE[PointMassParamCount - PhysicalModelParamCount];

   /// Gravitational constant for the central body (\f$G*M\f$)
   Real mu;
   /// Type of error estimate to perform
   Real estimationMethod;
   //CelestialBody *theBody; // wcs - 2004/06/21 - exists in PhysicalModel
   //std::string theBodyName; //loj: 5/6/04 added // wcs - 2004/06/21 - exists in PhysicalModel
   bool isPrimaryBody;
   
   // Moved here for performance
   Rvector6 bodyrv, orig;
   Rvector3 rv;
   A1Mjd now;
   GmatTime nowGT;

   Integer satCount;
//   Integer cartIndex;
   
   // for Debug
   void ShowBodyState(const std::string &header, Real time, Rvector6 &rv);
   void ShowDerivative(const std::string &header, Real *state, Integer satCount);
   
};

#endif  // PointMassForce_hpp

