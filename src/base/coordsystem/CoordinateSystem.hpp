//$Id$
//------------------------------------------------------------------------------
//                                  CoordinateSystem
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2004/12/22
//
/**
 * Definition of the CoordinateSystem class.
 *
 */
//------------------------------------------------------------------------------

#ifndef CoordinateSystem_hpp
#define CoordinateSystem_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateBase.hpp"
#include "AxisSystem.hpp"
#include "Rvector.hpp"
#include "A1Mjd.hpp"


class GMAT_API CoordinateSystem : public CoordinateBase
{
public:
   // default constructor
   CoordinateSystem(const std::string &itsType,
                    const std::string &itsName = "");
   // copy constructor
   CoordinateSystem(const CoordinateSystem &coordSys);
   // operator = for assignment
   const CoordinateSystem& operator=(const CoordinateSystem &coordSys);
   // operator == for equality testing
   bool operator==(const CoordinateSystem &coordSys);
   // destructor
   virtual ~CoordinateSystem();
   
   AxisSystem* GetAxisSystem();
   
   // virtual methods to check to see how/if an AxisSystem uses 
   // a particular parameter (calls through to its AxisSystem)
   virtual GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem = "FK5") const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;
   virtual GmatCoordinate::ParameterUsage UsesEpoch() const;
   virtual GmatCoordinate::ParameterUsage UsesPrimary() const;
   virtual GmatCoordinate::ParameterUsage UsesSecondary() const;
   virtual GmatCoordinate::ParameterUsage UsesReferenceObject() const;
   virtual GmatCoordinate::ParameterUsage UsesXAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesYAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesZAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const;
   virtual bool                           UsesSpacecraft(const std::string &withName = "") const;
   virtual bool                           RequiresCelestialBodyOrigin() const;
   virtual bool                           HasCelestialBodyOrigin() const;
   // We need these extra methods to manage the case where the origin is a
   // Spacecraft but we don't need to worry about attitude rates
   virtual void                           SetAllowWithoutRates(bool allow);
   virtual bool                           AllowWithoutRates() const;
   virtual bool                           IsInertial();

   // methods to set parameters for the AxisSystems
   virtual void                  SetPrimaryObject(SpacePoint *prim);
   virtual void                  SetSecondaryObject(SpacePoint *second);
   virtual void                  SetReferenceObject(SpacePoint *reObj);
   virtual void                  SetEpoch(const A1Mjd &toEpoch);
   virtual void                  SetXAxis(const std::string &toValue);
   virtual void                  SetYAxis(const std::string &toValue);
   virtual void                  SetZAxis(const std::string &toValue);
   virtual void                  SetEopFile(EopFile *eopF);
   virtual void                  SetCoefficientsFile(
                                    ItrfCoefficientsFile *itrfF);

   virtual SpacePoint*           GetPrimaryObject() const;
   virtual SpacePoint*           GetSecondaryObject() const;
   virtual SpacePoint*           GetReferenceObject() const;
   virtual std::string           GetPrimaryObjectName() const;
   virtual std::string           GetSecondaryObjectName() const;
   virtual std::string           GetReferenceObjectName() const;
   virtual A1Mjd                 GetEpoch() const;
   virtual std::string           GetXAxis() const;
   virtual std::string           GetYAxis() const;
   virtual std::string           GetZAxis() const;
   virtual EopFile*              GetEopFile() const;
   virtual ItrfCoefficientsFile* GetItrfCoefficientsFile();
   virtual Rmatrix33             GetLastRotationMatrix() const;
   virtual void                  GetLastRotationMatrix(Real *mat) const;
   virtual Rmatrix33             GetLastRotationDotMatrix() const;
   virtual void                  GetLastRotationDotMatrix(Real *mat) const;

	virtual std::vector<Rmatrix33> 
		                           GetDerivativeOfLastRotationMatrix(); 

   virtual bool                  AreAxesOfType(const std::string &ofType) const;
   virtual std::string           GetBaseSystem() const;
   
   // initializes the CoordinateSystem
   virtual bool                  Initialize();

   virtual void                  SetModifyFlag(bool modFlag);
   
   // methods to convert between this CoordinateSystem and MJ2000Eq
   virtual Rvector ToBaseSystem(const A1Mjd &epoch, const Rvector &inState,
                                bool coincident = false,
                                bool forceComputation = false); 

   virtual void    ToBaseSystem(const A1Mjd &epoch, const Real *inState,
                                Real *outState, bool coincident = false,
                                bool forceComputation = false); 
   
   virtual Rvector FromBaseSystem(const A1Mjd &epoch, const Rvector &inState,
                                  bool coincident = false,
                                  bool forceComputation = false);
   virtual void    FromBaseSystem(const A1Mjd &epoch, const Real *inState,
                                  Real *outState, bool coincident = false,
                                  bool forceComputation = false);
   
   virtual Rvector ToBaseSystem(const GmatTime &epoch, const Rvector &inState,
                                bool coincident = false,
                                bool forceComputation = false); 

   virtual void    ToBaseSystem(const GmatTime &epoch, const Real *inState,
                                Real *outState, bool coincident = false,
                                bool forceComputation = false); 
   
   virtual Rvector FromBaseSystem(const GmatTime &epoch, const Rvector &inState,
                                  bool coincident = false,
                                  bool forceComputation = false);
   virtual void    FromBaseSystem(const GmatTime &epoch, const Real *inState,
                                  Real *outState, bool coincident = false,
                                  bool forceComputation = false);

   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);

   // Parameter access methods - overridden from GmatBase 
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   virtual std::string  GetParameterText(const Integer id) const;     
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
   std::string          GetStringParameter(const Integer id) const;
   std::string          GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label, 
                                           const char *value);
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value);
   virtual bool         GetBooleanParameter(const Integer id) const; 
   virtual bool         GetBooleanParameter(const std::string &label) const; 
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value); 
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual bool         IsOwnedObject(Integer id) const;
   virtual GmatBase*    GetOwnedObject(Integer whichOne);
   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const char *name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   
   virtual UnsignedInt  GetPropertyObjectType(const Integer id) const;
   
   // WARNING: The J200Body must be set identically for all objects in a GMAT run;
   // not doing so will give incorrect results.
   // In addition, the setting of a body other than Earth as the J2000Body has
   // not been tested.
   static CoordinateSystem* CreateLocalCoordinateSystem(
                            const std::string &csName, const std::string &axesType,
                            SpacePoint *origin, SpacePoint *primary,
                            SpacePoint *secondary, SpacePoint *j2000Body,
                            SolarSystem *solarSystem, bool initializeIt = true);
   
	bool  SetCalculateRotMatrixDeriv(bool turnOn);

   void SetForGravityUpdate(bool turnOn);

protected:

   enum
   {
      AXES = CoordinateBaseParamCount,
      UPDATE_INTERVAL,
      OVERRIDE_ORIGIN_INTERVAL,
      SPICE_FRAME_ID,
      // owned object parameters
      EPOCH,
      CoordinateSystemParamCount
   };
   
   static const std::string PARAMETER_TEXT[
      CoordinateSystemParamCount - CoordinateBaseParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[
      CoordinateSystemParamCount - CoordinateBaseParamCount];
   
   virtual bool TranslateToBaseSystem(const A1Mjd &epoch, const Rvector &inState,
                                      Rvector &outState);
   virtual bool TranslateToBaseSystem(const A1Mjd &epoch, const Real *inState,
                                      Real *outState);
   
   virtual bool TranslateFromBaseSystem(const A1Mjd &epoch, const Rvector &inState,
                                        Rvector &outState);
   virtual bool TranslateFromBaseSystem(const A1Mjd &epoch, const Real *inState,
                                        Real *outState);
   
   virtual bool TranslateToBaseSystem(const GmatTime &epoch, const Rvector &inState,
                                      Rvector &outState);
   virtual bool TranslateToBaseSystem(const GmatTime &epoch, const Real *inState,
                                      Real *outState);
   
   virtual bool TranslateFromBaseSystem(const GmatTime &epoch, const Rvector &inState,
                                        Rvector &outState);
   virtual bool TranslateFromBaseSystem(const GmatTime &epoch, const Real *inState,
                                        Real *outState);
   
   /// axis system
   AxisSystem    *axes;

   std::string spiceFrameId;
   
};
#endif // CoordinateSystem_hpp
