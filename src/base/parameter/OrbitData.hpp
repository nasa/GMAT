//$Id$
//------------------------------------------------------------------------------
//                                  OrbitData
//------------------------------------------------------------------------------
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/09
//
/**
 * Declares Orbit related data class.
 */
//------------------------------------------------------------------------------
#ifndef OrbitData_hpp
#define OrbitData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Rvector6.hpp"
#include "Rmatrix66.hpp"
#include "Rmatrix33.hpp"
#include "Spacecraft.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "ODEModel.hpp"
#include "CoordinateConverter.hpp"

class GMAT_API OrbitData : public RefData
{
public:

   OrbitData(const std::string &name = "", const std::string &typeName = "",
             const UnsignedInt paramOwnerType = Gmat::SPACECRAFT,
             GmatParam::DepObject depObj = GmatParam::NO_DEP,
             bool isSettable = false);
   OrbitData(const OrbitData &data);
   OrbitData& operator= (const OrbitData& right);
   virtual ~OrbitData();
   
   Rvector6 GetCartState();
   Rvector6 GetKepState();
   Rvector6 GetModKepState();
   Rvector6 GetSphRaDecState();
   Rvector6 GetSphAzFpaState();
   Rvector6 GetEquinState();
   Rvector6 GetEquinDot(Rvector3 &pos, Rvector3 &vel, Real rMag);
   Rvector6 GetModEquinState();
   Rvector6 GetAltEquinState();
   Rvector6 GetDelaState();
   Rvector6 GetPlanetodeticState();
   Rvector6 GetIncAsymState();
   Rvector6 GetOutAsymState();
   Rvector6 GetBLshortState();
   Rvector6 GetBLlongState();
   
   void SetReal(Integer item, Real rval);
   void SetRvector6(const Rvector6 &val);
   
   Real GetCartReal(Integer item);
   Real GetCartReal(const std::string &str);
   
   Real GetKepReal(Integer item);
   Real GetKepReal(const std::string &str);
   
   Real GetModKepReal(Integer item);
   Real GetModKepReal(const std::string &str);
   
   Real GetOtherKepReal(Integer item);
   Real GetOtherKepReal(const std::string &str);
   
   Real GetSphRaDecReal(Integer item);
   Real GetSphRaDecReal(const std::string &str);
   
   Real GetSphAzFpaReal(Integer item);
   Real GetSphAzFpaReal(const std::string &str);
   
   Real GetAngularReal(Integer item);
   Real GetAngularReal(const std::string &str);
   
   Real GetOtherAngleReal(Integer item);
   Real GetOtherAngleReal(const std::string &str);
   
   Real GetEquinReal(Integer item);
   Real GetEquinReal(const std::string &str);
   
   // Modified by M.H.
   Real GetModEquinReal(Integer item);
   Real GetModEquinReal(const std::string &str);

   // AlternateEquinoctial by HYKim
   Real GetAltEquinReal(Integer item);
   Real GetAltEquinReal(const std::string &str);
   
   Real GetDelaReal(Integer item);
   Real GetDelaReal(const std::string &str);

   Real GetPlanetodeticReal(Integer item);
   Real GetPlanetodeticReal(const std::string &str);

   // Modified by YK
   Real GetIncAsymReal(Integer item);
   Real GetIncAsymReal(const std::string &str);
   
   Real GetOutAsymReal(Integer item);
   Real GetOutAsymReal(const std::string &str);

   Real GetBLshortReal(Integer item);
   Real GetBLshortReal(const std::string &str);
   
   Real GetBLlongReal(Integer item);
   Real GetBLlongReal(const std::string &str);

   const Rmatrix66& GetStmRmat66(Integer item);
   const Rmatrix33& GetStmRmat33(Integer item);

   const Rmatrix66& GetCovarianceRmat66();
   
   // The inherited methods from RefData
   virtual bool               ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;

   // More inherited methods from RefData
   virtual std::string        GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type);

   virtual bool               SetRefObjectName(const UnsignedInt type,
                                               const std::string &name);
   virtual GmatBase*          GetRefObject(const UnsignedInt type,
                                           const std::string &name = "");
   virtual bool               SetRefObject(GmatBase *obj, const UnsignedInt type,
                                           const std::string &name = "");

protected:

   // Inherited methods from RefData
   virtual bool AddRefObject(const UnsignedInt type,
                             const std::string &name, GmatBase *obj = NULL,
                             bool replaceName = false);
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(UnsignedInt type);
   
   // Methods
   SolarSystem* GetSolarSystem();
   CoordinateSystem* GetInternalCoordSys();
   
   void SetInternalCoordSys(CoordinateSystem *cs);
   Real GetPositionMagnitude(SpacePoint *origin);
   Rvector6 GetRelativeCartState(SpacePoint *origin);
   Rvector6 GetCartStateInParameterCS(Integer item, Real rval);
   Rvector6 GetCartStateInParameterOrigin(Integer item, Real rval);
   void SetRealParameters(Integer item, Real rval);
   void DebugWriteData(CoordinateSystem *paramOwnerCS);
   void DebugWriteRefObjInfo();
   
   Rmatrix66 mSTM;
   Rmatrix33 mSTMSubset;

   Rmatrix66 mOrbitCovariance;
   
   Real mCartEpoch;
   Real mGravConst;
   Real mFlattening;
   Real mEqRadius;
   
   Integer stateTypeId;
   
   Spacecraft *mSpacecraft;
   SpacePoint *mSpacePoint;
   SolarSystem *mSolarSystem;
   SpacePoint *mOrigin;
   CoordinateSystem *mInternalCS;
   CoordinateSystem *mParameterCS;
   ODEModel *ode;
   
   bool mIsParamOriginDep;
   bool firstTimeEpochWarning;
   
   // only one CoordinateConverter needed
   static CoordinateConverter mCoordConverter;
   
   // Other orbit items
   // @note - Do not add or remove items from this list without updating OrbitData.
   //         These enums are also used in OrbitData for passing parameter names to
   //         CalculationUtilitis::CalculateKeplerianData().
   enum {MM = 0, VEL_APOAPSIS, VEL_PERIAPSIS, ORBIT_PERIOD, C3_ENERGY, ENERGY, OtherOrbitCount1};
   
   // Angular orbit items
   enum {SEMILATUS_RECTUM = OtherOrbitCount1, HMAG, HX, HY, HZ, BETA_ANGLE, HYPERBOLIC_RLA, HYPERBOLIC_DLA, OtherOrbitCount2};
   
   // Orbit STM items
   enum {ORBIT_STM = OtherOrbitCount2, ORBIT_STM_A, ORBIT_STM_B, ORBIT_STM_C, ORBIT_STM_D, OrbitStmCount};
   
   // Orbit state type IDs
   // These IDs are used in GetCartStateInParameterCS(). If new types are added,
   // proper item Count must be used in the OrbitData.cpp
   
   // Cartesian items
   enum {CART_X = OrbitStmCount, CART_Y, CART_Z, CART_VX, CART_VY, CART_VZ, CART_STATE, CartCount};
   
   // Keplerian and modified keplerian items
   enum {KEP_SMA = CartCount, KEP_ECC, KEP_INC, KEP_RAAN, KEP_AOP, KEP_TA, KEP_MA,
         KEP_EA, KEP_HA, KEP_RADN, KEP_STATE, MODKEP_RADAPO, MODKEP_RADPER, KepCount};
   
   // Spherical RADEC items
   enum {RADEC_RMAG = KepCount, RADEC_RA, RADEC_DEC, RADEC_VMAG, RADEC_RAV, RADEC_DECV, RaDecCount};
   
   // Spherical AZIFPA items
   enum {AZIFPA_RMAG = RaDecCount, AZIFPA_RA, AZIFPA_DEC, AZIFPA_VMAG, AZIFPA_AZI, AZIFPA_FPA, AziFpaCount};
   
   // Equinoctial items
   enum {EQ_SMA = AziFpaCount, EQ_H, EQ_K, EQ_P, EQ_Q, EQ_MLONG, EquinCount};
   
   // Equinoctial rate items
   enum {EQ_SMA_DOT = EquinCount,
         EQ_H_DOT,
         EQ_K_DOT,
         EQ_P_DOT,
         EQ_Q_DOT,
         EQ_TLONG_DOT,        // Current implementation reports d(TLONG)/dt
         EquinRateCount};

   // Modified by M.H. and HYKim
   // ModifiedEquinoctial and AlternateEquinoctial items
   enum {MOD_EQ_P = EquinRateCount, MOD_EQ_F, MOD_EQ_G, MOD_EQ_H, MOD_EQ_K, MOD_EQ_TLONG, ModEquinCount};
   
   // AlternateEquinoctial items
   enum {ALT_EQ_SMA = ModEquinCount, ALT_EQ_H, ALT_EQ_K, ALT_EQ_P, ALT_EQ_Q, ALT_EQ_MLONG, AltEquinCount};
   
   // Modified by M.H.
   // Delaunay items
   enum {DEL_DELA_SL = AltEquinCount, DEL_DELA_SG, DEL_DELA_SH, DEL_DELA_L, DEL_DELA_G, DEL_DELA_H, DelaCount};

   // Modified by M.H.
   // Planetodetic items
   enum {PLD_RMAG = DelaCount, PLD_LON, PLD_LAT, PLD_VMAG, PLD_AZI, PLD_HFPA, PlanetoCount};

   // Modified by YK
   // Incoming Asymptote State
   enum {INCASYM_RADPER = PlanetoCount, INCASYM_C3_ENERGY, INCASYM_RHA, INCASYM_DHA, INCASYM_BVAZI, INCASYM_TA, InAsymCount};
   
   // Modified by YK
   // Outgoing Asymptote State
   enum {OUTASYM_RADPER = InAsymCount, OUTASYM_C3_ENERGY, OUTASYM_RHA, OUTASYM_DHA, OUTASYM_BVAZI, OUTASYM_TA, OutAsymCount};
   
   // Modified by YK
   // Brouwer-Lyddane Mean-short 
   enum {BLS_SMA = OutAsymCount, BLS_ECC, BLS_INC, BLS_RAAN, BLS_AOP, BLS_MA, BrouwerShortCount};
   
   // Modified by YK
   // Brouwer-Lyddane Mean-long 
   enum {BLL_SMA = BrouwerShortCount, BLL_ECC, BLL_INC, BLL_RAAN, BLL_AOP, BLL_MA, BrouwerLongCount};
   
   enum
   {
      SPACECRAFT = 0,
      SOLAR_SYSTEM,
      COORD_SYSTEM,
      SPACE_POINT,
      ODE_MODEL,
      OrbitDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[OrbitDataObjectCount];
   static const Real        ORBIT_DATA_TOLERANCE;
   static const std::string VALID_ANGLE_PARAM_NAMES[HYPERBOLIC_DLA - SEMILATUS_RECTUM + 1];
   static const std::string VALID_OTHER_ORBIT_PARAM_NAMES[ENERGY - MM + 1];
};
#endif // OrbitData_hpp

   
