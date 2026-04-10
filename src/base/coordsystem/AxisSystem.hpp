//$Id$
//------------------------------------------------------------------------------
//                                  AxisSystem
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
// Created: 2004/12/20
//
/**
 * Definition of the AxisSystem class.  This is the base class for the 
 * InertialAxes and DynamicAxes classes.
 *
 */
//------------------------------------------------------------------------------

#ifndef AxisSystem_hpp
#define AxisSystem_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateBase.hpp"
#include "Rmatrix33.hpp"
#include "A1Mjd.hpp"
#include "EopFile.hpp"
#include "ItrfCoefficientsFile.hpp"
#include "ICRFFile.hpp"

class GMAT_API AxisSystem : public CoordinateBase
{
public:

   // default constructor
   AxisSystem(const std::string &itsType,
              const std::string &itsName = "");
   // copy constructor
   AxisSystem(const AxisSystem &axisSys);
   // operator = for assignment
   const AxisSystem& operator=(const AxisSystem &axisSys);
   // destructor
   virtual ~AxisSystem();

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

   // methods to set the files to use - for those AxisSystems that 
   // need all or part of the FK5 reduction
   virtual void                  SetEopFile(EopFile *eopF);
   virtual void                  SetCoefficientsFile(ItrfCoefficientsFile *itrfF);
   virtual void                  SetEpochFormat(const std::string &fmt);  // for GUI
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
   virtual std::string           GetBaseSystem() const;
   virtual ItrfCoefficientsFile* GetItrfCoefficientsFile();
   virtual std::string           GetEpochFormat() const; // for GUI
   virtual Rmatrix33             GetLastRotationMatrix() const;
   virtual void                  GetLastRotationMatrix(Real *mat) const;
   virtual Rmatrix33             GetLastRotationDotMatrix() const;
   virtual void                  GetLastRotationDotMatrix(Real *mat) const;
   
	virtual std::vector<Rmatrix33> 
		                           GetDerivativeOfLastRotationMatrix();

   virtual void                  SetCoordinateSystemName(const std::string &csName);

   // initializes the AxisSystem
   virtual bool Initialize();
   
   // methods to convert to/from the MJ2000 Equatorial axis system
   virtual bool RotateToBaseSystem(const A1Mjd &epoch, const Rvector &inState,
                                   Rvector &outState,
                                   bool forceComputation = false); 
   virtual bool RotateToBaseSystem(const A1Mjd &epoch, const Real *inState,
                                   Real *outState,
                                   bool forceComputation = false); 
   virtual bool RotateFromBaseSystem(const A1Mjd &epoch, const Rvector &inState,
                                     Rvector &outState,
                                     bool forceComputation = false);
   virtual bool RotateFromBaseSystem(const A1Mjd &epoch, const Real *inState,
                                     Real *outState,
                                     bool forceComputation = false);
   
   virtual bool RotateToBaseSystem(const GmatTime &epoch, const Rvector &inState,
                                   Rvector &outState,
                                   bool forceComputation = false); 
   virtual bool RotateToBaseSystem(const GmatTime &epoch, const Real *inState,
                                   Real *outState,
                                   bool forceComputation = false); 
   virtual bool RotateFromBaseSystem(const GmatTime &epoch, const Rvector &inState,
                                     Rvector &outState,
                                     bool forceComputation = false);
   virtual bool RotateFromBaseSystem(const GmatTime &epoch, const Real *inState,
                                     Real *outState,
                                     bool forceComputation = false);
   
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   //virtual GmatBase*       Clone(void) const;

   // Parameter access methods - overridden from GmatBase - may need these later??
   virtual std::string     GetParameterText(const Integer id) const;     
   virtual Integer         GetParameterID(const std::string &str) const; 
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;
   virtual bool            IsParameterReadOnly(const Integer id) const;
   virtual Real            GetRealParameter(const Integer id) const;
   virtual Real            SetRealParameter(const Integer id,
                                            const Real value);
   virtual Real            GetRealParameter(const std::string &label) const;
   virtual Real            SetRealParameter(const std::string &label,
                                            const Real value);
   virtual bool            GetBooleanParameter(const Integer id) const; 
   virtual bool            GetBooleanParameter(const std::string &label) const; 
   virtual bool            SetBooleanParameter(const Integer id,
                                               const bool value); 
   virtual bool            SetBooleanParameter(const std::string &label,
                                               const bool value);

   virtual const ObjectTypeArray&
                           GetRefObjectTypeArray();


	bool                    SetCalculateRotMatrixDeriv(bool turnOn);

   // currently, no access to RotMatrix and RotDotMatrix allowed

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:

   enum
   {
      EPOCH = CoordinateBaseParamCount,
      UPDATE_INTERVAL, 
      OVERRIDE_ORIGIN_INTERVAL,
      AxisSystemParamCount
   };
   
   static const std::string PARAMETER_TEXT[AxisSystemParamCount - CoordinateBaseParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[AxisSystemParamCount - CoordinateBaseParamCount];
   
   //---------------------------------------------------------------------------
   //  void CalculateRotationMatrix(const A1Mjd &atEpoch)
   //---------------------------------------------------------------------------
   /**
    * This method will compute the rotMatrix and rotDotMatrix used for rotations
    * from/to this AxisSystem to/from the MJ2000EqAxes system.
    *
    * @param atEpoch  epoch at which to compute the roration matrix
    */
   //---------------------------------------------------------------------------
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false)  
                                        = 0;
   
   virtual void CalculateRotationMatrix(const GmatTime &atEpoch,
                                        bool forceComputation = false);

   virtual bool CompleteRotateToBase(const Rvector &inState, Rvector &outState);
   virtual bool CompleteRotateToBase(const Real *inState, Real *outState);
   virtual bool CompleteRotateFromBase(const Rvector &inState, Rvector &outState);
   virtual bool CompleteRotateFromBase(const Real *inState, Real *outState);
   
   /// rotation matrix - 
   /// default constructor creates a 3x3 zero-matrix
   Rmatrix33   rotMatrix;
   /// derivative of rotation matrix - 
   /// default constructor creates a 3x3 zero-matrix
   Rmatrix33   rotDotMatrix;


	/// derivative of rotation matrix w.r.t. spacecraft state (x,y,z,vx,vy,vz)
	std::vector<Rmatrix33> rotMatrixDeriv; 
	bool specifyRotMatrixDeriv; 

   /// epoch
   A1Mjd epoch;
   /// Name of the coordinate system
   std::string coordName;
   
   /// @todo create class data to hold all of these settings (e.g.
   /// usesXAxis, etc., similar to usesPrimary & usesSecondary),
   /// then remove associated methods in subclasses when applicable
   /// Does this AxisSystem use a primary object?
   GmatCoordinate::ParameterUsage        usesPrimary;
   /// Does this AxisSystem use a secondary object?
   GmatCoordinate::ParameterUsage        usesSecondary;

   /// specifies the base inertial system for this AxisSystem
   /// valid values are: "FK5" and "ICRF" - must be set on each leaf AxisSystem-derived class
   std::string baseSystem;

   
   const Real *rotData;
   const Real *rotDotData;

   // data and methods for those AxisSystems that need all or part of the FK5 
   // reduction
   static const Real  JD_OF_JANUARY_1_1997;
   static const Real  DETERMINANT_TOLERANCE;

   EopFile                   *eop;
   ItrfCoefficientsFile      *itrf;
   
   std::string               epochFormat;
   
   bool                      needsCBOrigin;
   bool                      allowNoRates;

   Real                      updateInterval;
   Real                      updateIntervalToUse;
   bool                      overrideOriginInterval;
   A1Mjd                     lastPRECEpoch;
   A1Mjd                     lastNUTEpoch;
   GmatTime                  lastSTDerivEpoch;
   A1Mjd                     lastPMEpoch;
   Rmatrix33                 lastPREC;
   Rmatrix33                 lastNUT;
   Rmatrix33                 lastSTDeriv;
   Rmatrix33                 lastPM;
      
   Real                      lastDPsi; 
   
   GmatItrf::NutationTerms   nutationSrc;
   GmatItrf::PlanetaryTerms  planetarySrc; 
   
   std::vector<IntegerArray> a, ap;
   Rvector                   A, B, C, D, E, F, Ap, Bp, Cp, Dp;
   
   Integer                   *aVals;
   Integer                   *apVals;
   
   // Performance enhancements
   Rmatrix33 PREC;
   Rmatrix33 NUT;
   Rmatrix33 ST;
   Rmatrix33 STderiv;
   Rmatrix33 PM;
   
   // added for performance
   const Real *precData;
   const Real *nutData;
   const Real *stData;
   const Real *stDerivData;
   const Real *pmData;
   
   
   const Real  *AVals;
   const Real  *BVals;
   const Real  *CVals;
   const Real  *DVals;
   const Real  *EVals;
   const Real  *FVals;
   const Real  *ApVals;
   const Real  *BpVals;
   const Real  *CpVals;
   const Real  *DpVals;
   
   /// True if axes are inertial enough to support sensible Keplerian elements
   bool isInertial;

   /// Used to transform Fk5 (MJ2000Eq) to ICRF 
   ICRFFile* icrfFile;
   /// Used to transform Fk5 (MJ2000Eq) to ICRF
   Rmatrix33 icrfToFK5;
   
   A1Mjd lastIcrfToFk5Epoch = 0.0;

   // intermediate quantities needed by more than one method
   
   virtual void      InitializeFK5();

   virtual void ComputePrecessionMatrix(const Real tTDB, A1Mjd atEpoch);
   virtual void ComputeNutationMatrix(const Real tTDB, A1Mjd atEpoch, 
                                           Real &dPsi,
                                           Real &longAscNodeLunar,
                                           Real &cosEpsbar,
                                           bool forceComputation = false);
   virtual void ComputeSiderealTimeRotation(const Real jdTT,
                                                 const GmatTime &mjdUT1,
                                                 Real dPsi,
                                                 Real longAscNodeLunar,
                                                 Real cosEpsbar,
                                                 Real &cosAst,
                                                 Real &sinAst);
   virtual void ComputeSiderealTimeDotRotation(const GmatTime &mjdUTC,
                                                    const GmatTime &atEpoch,
                                                    Real cosAst, Real sinAst,
                                                    bool forceComputation = false);
   virtual void ComputePolarMotionRotation(const Real mjdUTC, A1Mjd atEpoch,
                                            bool forceComputation = false);

   virtual void InitializeReference(SpacePoint *refObj);

   void RotationMatrixFromICRFToFK5(const A1Mjd &atEpoch);

   void CalculateSpiceFrameRotationMatrix(const std::string spiceToFrame, const GmatTime &atEpoch);
};
#endif // AxisSystem_hpp
