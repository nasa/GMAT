//$Id$
//------------------------------------------------------------------------------
//                            File: HardwareParameters
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. 
//
// Author: Linda Jun
// Created: 2009.03.20
//
/**
 * Declares Hardware related Parameter classes.
 *    FuelTank: FuelMass,
 *              FuelCenterOfMassX, FuelCenterOfMassY, FuelCenterOfMassZ,
 *              FuelMomentOfInertiaXX, FuelMomentOfInertiaXY, FuelMomentOfInertiaXZ,
 *              FuelMomentOfInertiaYY, FuelMomentOfInertiaYZ, FuelMomentOfInertiaZZ,
 *              FuelCenterOfMassX_BCS, FuelCenterOfMassY_BCS, FuelCenterOfMassZ_BCS,
 *              FuelMomentOfInertiaXX_BCS, FuelMomentOfInertiaXY_BCS, FuelMomentOfInertiaXZ_BCS,
 *              FuelMomentOfInertiaYY_BCS, FuelMomentOfInertiaYZ_BCS, FuelMomentOfInertiaZZ_BCS,
 *              Pressure, Temperature, RefTemperature, Volume,
 *              FuelDensity,
 *    Hardware: R_SB11, R_SB12, R_SB13, R_SB21, R_SB22, R_SB23, 
 *              R_SB31, R_SB32, R_SB33, 
 *    Thruster: DutyCycle, ThrustScaleFactor, GravitationalAccel, C1-C16,
 *              K1-K16, ThrustDirections, ThrustMagnitude, Isp, MassFlowRate
 *    PowerSystem: TotalPowerAvailable, RequiredBusPower, ThrustPowerAvailable
 */
//------------------------------------------------------------------------------
#ifndef HardwareParameters_hpp
#define HardwareParameters_hpp

#include "gmatdefs.hpp"
#include "HardwareReal.hpp"
#include "HardwareString.hpp"
#include "HardwareRvector.hpp"

class GMAT_API FuelMass : public HardwareReal
{
public:
   
   FuelMass(const std::string &name = "", GmatBase *obj = NULL);
   FuelMass(const FuelMass &copy);
   FuelMass& operator=(const FuelMass &right);
   virtual ~FuelMass();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelCenterOfMassX : public HardwareReal
{
public:
   
   FuelCenterOfMassX(const std::string &name = "", GmatBase *obj = NULL);
   FuelCenterOfMassX(const FuelCenterOfMassX &copy);
   FuelCenterOfMassX& operator=(const FuelCenterOfMassX &right);
   virtual ~FuelCenterOfMassX();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelCenterOfMassY : public HardwareReal
{
public:
   
   FuelCenterOfMassY(const std::string &name = "", GmatBase *obj = NULL);
   FuelCenterOfMassY(const FuelCenterOfMassY &copy);
   FuelCenterOfMassY& operator=(const FuelCenterOfMassY &right);
   virtual ~FuelCenterOfMassY();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelCenterOfMassZ : public HardwareReal
{
public:
   
   FuelCenterOfMassZ(const std::string &name = "", GmatBase *obj = NULL);
   FuelCenterOfMassZ(const FuelCenterOfMassZ &copy);
   FuelCenterOfMassZ& operator=(const FuelCenterOfMassZ &right);
   virtual ~FuelCenterOfMassZ();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelMomentOfInertiaXX : public HardwareReal
{
public:
   
   FuelMomentOfInertiaXX(const std::string &name = "", GmatBase *obj = NULL);
   FuelMomentOfInertiaXX(const FuelMomentOfInertiaXX &copy);
   FuelMomentOfInertiaXX& operator=(const FuelMomentOfInertiaXX &right);
   virtual ~FuelMomentOfInertiaXX();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelMomentOfInertiaXY : public HardwareReal
{
public:
   
   FuelMomentOfInertiaXY(const std::string &name = "", GmatBase *obj = NULL);
   FuelMomentOfInertiaXY(const FuelMomentOfInertiaXY &copy);
   FuelMomentOfInertiaXY& operator=(const FuelMomentOfInertiaXY &right);
   virtual ~FuelMomentOfInertiaXY();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelMomentOfInertiaXZ : public HardwareReal
{
public:
   
   FuelMomentOfInertiaXZ(const std::string &name = "", GmatBase *obj = NULL);
   FuelMomentOfInertiaXZ(const FuelMomentOfInertiaXZ &copy);
   FuelMomentOfInertiaXZ& operator=(const FuelMomentOfInertiaXZ &right);
   virtual ~FuelMomentOfInertiaXZ();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelMomentOfInertiaYY : public HardwareReal
{
public:
   
   FuelMomentOfInertiaYY(const std::string &name = "", GmatBase *obj = NULL);
   FuelMomentOfInertiaYY(const FuelMomentOfInertiaYY &copy);
   FuelMomentOfInertiaYY& operator=(const FuelMomentOfInertiaYY &right);
   virtual ~FuelMomentOfInertiaYY();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelMomentOfInertiaYZ : public HardwareReal
{
public:
   
   FuelMomentOfInertiaYZ(const std::string &name = "", GmatBase *obj = NULL);
   FuelMomentOfInertiaYZ(const FuelMomentOfInertiaYZ &copy);
   FuelMomentOfInertiaYZ& operator=(const FuelMomentOfInertiaYZ &right);
   virtual ~FuelMomentOfInertiaYZ();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelMomentOfInertiaZZ : public HardwareReal
{
public:
   
   FuelMomentOfInertiaZZ(const std::string &name = "", GmatBase *obj = NULL);
   FuelMomentOfInertiaZZ(const FuelMomentOfInertiaZZ &copy);
   FuelMomentOfInertiaZZ& operator=(const FuelMomentOfInertiaZZ &right);
   virtual ~FuelMomentOfInertiaZZ();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelCenterOfMassX_BCS : public HardwareReal
{
public:
   
   FuelCenterOfMassX_BCS(const std::string &name = "", GmatBase *obj = NULL);
   FuelCenterOfMassX_BCS(const FuelCenterOfMassX_BCS &copy);
   FuelCenterOfMassX_BCS& operator=(const FuelCenterOfMassX_BCS &right);
   virtual ~FuelCenterOfMassX_BCS();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelCenterOfMassY_BCS : public HardwareReal
{
public:
   
   FuelCenterOfMassY_BCS(const std::string &name = "", GmatBase *obj = NULL);
   FuelCenterOfMassY_BCS(const FuelCenterOfMassY_BCS &copy);
   FuelCenterOfMassY_BCS& operator=(const FuelCenterOfMassY_BCS &right);
   virtual ~FuelCenterOfMassY_BCS();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelCenterOfMassZ_BCS : public HardwareReal
{
public:
   
   FuelCenterOfMassZ_BCS(const std::string &name = "", GmatBase *obj = NULL);
   FuelCenterOfMassZ_BCS(const FuelCenterOfMassZ_BCS &copy);
   FuelCenterOfMassZ_BCS& operator=(const FuelCenterOfMassZ_BCS &right);
   virtual ~FuelCenterOfMassZ_BCS();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelMomentOfInertiaXX_BCS : public HardwareReal
{
public:
   
   FuelMomentOfInertiaXX_BCS(const std::string &name = "", GmatBase *obj = NULL);
   FuelMomentOfInertiaXX_BCS(const FuelMomentOfInertiaXX_BCS &copy);
   FuelMomentOfInertiaXX_BCS& operator=(const FuelMomentOfInertiaXX_BCS &right);
   virtual ~FuelMomentOfInertiaXX_BCS();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelMomentOfInertiaXY_BCS : public HardwareReal
{
public:
   
   FuelMomentOfInertiaXY_BCS(const std::string &name = "", GmatBase *obj = NULL);
   FuelMomentOfInertiaXY_BCS(const FuelMomentOfInertiaXY_BCS &copy);
   FuelMomentOfInertiaXY_BCS& operator=(const FuelMomentOfInertiaXY_BCS &right);
   virtual ~FuelMomentOfInertiaXY_BCS();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelMomentOfInertiaXZ_BCS : public HardwareReal
{
public:
   
   FuelMomentOfInertiaXZ_BCS(const std::string &name = "", GmatBase *obj = NULL);
   FuelMomentOfInertiaXZ_BCS(const FuelMomentOfInertiaXZ_BCS &copy);
   FuelMomentOfInertiaXZ_BCS& operator=(const FuelMomentOfInertiaXZ_BCS &right);
   virtual ~FuelMomentOfInertiaXZ_BCS();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelMomentOfInertiaYY_BCS : public HardwareReal
{
public:
   
   FuelMomentOfInertiaYY_BCS(const std::string &name = "", GmatBase *obj = NULL);
   FuelMomentOfInertiaYY_BCS(const FuelMomentOfInertiaYY_BCS &copy);
   FuelMomentOfInertiaYY_BCS& operator=(const FuelMomentOfInertiaYY_BCS &right);
   virtual ~FuelMomentOfInertiaYY_BCS();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelMomentOfInertiaYZ_BCS : public HardwareReal
{
public:
   
   FuelMomentOfInertiaYZ_BCS(const std::string &name = "", GmatBase *obj = NULL);
   FuelMomentOfInertiaYZ_BCS(const FuelMomentOfInertiaYZ_BCS &copy);
   FuelMomentOfInertiaYZ_BCS& operator=(const FuelMomentOfInertiaYZ_BCS &right);
   virtual ~FuelMomentOfInertiaYZ_BCS();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelMomentOfInertiaZZ_BCS : public HardwareReal
{
public:
   
   FuelMomentOfInertiaZZ_BCS(const std::string &name = "", GmatBase *obj = NULL);
   FuelMomentOfInertiaZZ_BCS(const FuelMomentOfInertiaZZ_BCS &copy);
   FuelMomentOfInertiaZZ_BCS& operator=(const FuelMomentOfInertiaZZ_BCS &right);
   virtual ~FuelMomentOfInertiaZZ_BCS();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API Pressure : public HardwareReal
{
public:

   Pressure(const std::string &name = "", GmatBase *obj = NULL);
   Pressure(const Pressure &copy);
   Pressure& operator=(const Pressure &right);
   virtual ~Pressure();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API Temperature : public HardwareReal
{
public:

   Temperature(const std::string &name = "", GmatBase *obj = NULL);
   Temperature(const Temperature &copy);
   Temperature& operator=(const Temperature &right);
   virtual ~Temperature();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API RefTemperature : public HardwareReal
{
public:

   RefTemperature(const std::string &name = "", GmatBase *obj = NULL);
   RefTemperature(const RefTemperature &copy);
   RefTemperature& operator=(const RefTemperature &right);
   virtual ~RefTemperature();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API Volume : public HardwareReal
{
public:

   Volume(const std::string &name = "", GmatBase *obj = NULL);
   Volume(const Volume &copy);
   Volume& operator=(const Volume &right);
   virtual ~Volume();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelDensity : public HardwareReal
{
public:

   FuelDensity(const std::string &name = "", GmatBase *obj = NULL);
   FuelDensity(const FuelDensity &copy);
   FuelDensity& operator=(const FuelDensity &right);
   virtual ~FuelDensity();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API R_SB11 : public HardwareReal
{
public:
   
   R_SB11(const std::string &name = "", GmatBase *obj = NULL);
   R_SB11(const R_SB11 &copy);
   R_SB11& operator=(const R_SB11 &right);
   virtual ~R_SB11();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API R_SB12 : public HardwareReal
{
public:
   
   R_SB12(const std::string &name = "", GmatBase *obj = NULL);
   R_SB12(const R_SB12 &copy);
   R_SB12& operator=(const R_SB12 &right);
   virtual ~R_SB12();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API R_SB13 : public HardwareReal
{
public:
   
   R_SB13(const std::string &name = "", GmatBase *obj = NULL);
   R_SB13(const R_SB13 &copy);
   R_SB13& operator=(const R_SB13 &right);
   virtual ~R_SB13();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API R_SB21 : public HardwareReal
{
public:
   
   R_SB21(const std::string &name = "", GmatBase *obj = NULL);
   R_SB21(const R_SB21 &copy);
   R_SB21& operator=(const R_SB21 &right);
   virtual ~R_SB21();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API R_SB22 : public HardwareReal
{
public:
   
   R_SB22(const std::string &name = "", GmatBase *obj = NULL);
   R_SB22(const R_SB22 &copy);
   R_SB22& operator=(const R_SB22 &right);
   virtual ~R_SB22();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API R_SB23 : public HardwareReal
{
public:
   
   R_SB23(const std::string &name = "", GmatBase *obj = NULL);
   R_SB23(const R_SB23 &copy);
   R_SB23& operator=(const R_SB23 &right);
   virtual ~R_SB23();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API R_SB31 : public HardwareReal
{
public:
   
   R_SB31(const std::string &name = "", GmatBase *obj = NULL);
   R_SB31(const R_SB31 &copy);
   R_SB31& operator=(const R_SB31 &right);
   virtual ~R_SB31();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API R_SB32 : public HardwareReal
{
public:
   
   R_SB32(const std::string &name = "", GmatBase *obj = NULL);
   R_SB32(const R_SB32 &copy);
   R_SB32& operator=(const R_SB32 &right);
   virtual ~R_SB32();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API R_SB33 : public HardwareReal
{
public:
   
   R_SB33(const std::string &name = "", GmatBase *obj = NULL);
   R_SB33(const R_SB33 &copy);
   R_SB33& operator=(const R_SB33 &right);
   virtual ~R_SB33();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API DutyCycle : public HardwareReal
{
public:

   DutyCycle(const std::string &name = "", GmatBase *obj = NULL);
   DutyCycle(const DutyCycle &copy);
   DutyCycle& operator=(const DutyCycle &right);
   virtual ~DutyCycle();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API ThrustScaleFactor : public HardwareReal
{
public:

   ThrustScaleFactor(const std::string &name = "", GmatBase *obj = NULL);
   ThrustScaleFactor(const ThrustScaleFactor &copy);
   ThrustScaleFactor& operator=(const ThrustScaleFactor &right);
   virtual ~ThrustScaleFactor();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API GravitationalAccel : public HardwareReal
{
public:

   GravitationalAccel(const std::string &name = "", GmatBase *obj = NULL);
   GravitationalAccel(const GravitationalAccel &copy);
   GravitationalAccel& operator=(const GravitationalAccel &right);
   virtual ~GravitationalAccel();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API ThrustCoefficients : public HardwareReal
{
public:

   ThrustCoefficients(const std::string &type = "",
                      const std::string &name = "",
                      GmatBase *obj = NULL);
   ThrustCoefficients(const ThrustCoefficients &copy);
   ThrustCoefficients& operator=(const ThrustCoefficients &right);
   virtual ~ThrustCoefficients();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
   Integer mThrustCoeffId;
   
};

class GMAT_API ImpulseCoefficients : public HardwareReal
{
public:

   ImpulseCoefficients(const std::string &type = "",
                       const std::string &name = "",
                       GmatBase *obj = NULL);
   ImpulseCoefficients(const ImpulseCoefficients &copy);
   ImpulseCoefficients& operator=(const ImpulseCoefficients &right);
   virtual ~ImpulseCoefficients();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
   Integer mImpulseCoeffId;
   
};

class GMAT_API ThrustDirections : public HardwareReal
{
public:

   ThrustDirections(const std::string &type = "",
                    const std::string &name = "",
                    GmatBase *obj = NULL);
   ThrustDirections(const ThrustDirections &copy);
   ThrustDirections& operator=(const ThrustDirections &right);
   virtual ~ThrustDirections();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
   Integer mThrustDirectionId;
   
};


class GMAT_API ThrustMagnitude : public HardwareReal
{
public:

   ThrustMagnitude(const std::string &name = "", GmatBase *obj = NULL);
   ThrustMagnitude(const ThrustMagnitude &copy);
   ThrustMagnitude& operator=(const ThrustMagnitude &right);
   virtual ~ThrustMagnitude();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


class GMAT_API Isp : public HardwareReal
{
public:

   Isp(const std::string &name = "", GmatBase *obj = NULL);
   Isp(const Isp &copy);
   Isp& operator=(const Isp &right);
   virtual ~Isp();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


class GMAT_API MassFlowRate : public HardwareReal
{
public:

   MassFlowRate(const std::string &name = "", GmatBase *obj = NULL);
   MassFlowRate(const MassFlowRate &copy);
   MassFlowRate& operator=(const MassFlowRate &right);
   virtual ~MassFlowRate();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


class GMAT_API TotalPowerAvailable : public HardwareReal
{
public:

   TotalPowerAvailable(const std::string &name = "",
                       GmatBase *obj = NULL);
   TotalPowerAvailable(const TotalPowerAvailable &copy);
   TotalPowerAvailable& operator=(const TotalPowerAvailable &right);
   virtual ~TotalPowerAvailable();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:
   Integer mTotalPowerId;

};

class GMAT_API RequiredBusPower : public HardwareReal
{
public:

   RequiredBusPower(const std::string &name = "",
                    GmatBase *obj = NULL);
   RequiredBusPower(const RequiredBusPower &copy);
   RequiredBusPower& operator=(const RequiredBusPower &right);
   virtual ~RequiredBusPower();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:
   Integer mRequiredBusPowerId;

};

class GMAT_API ThrustPowerAvailable : public HardwareReal
{
public:

   ThrustPowerAvailable(const std::string &name = "",
                        GmatBase *obj = NULL);
   ThrustPowerAvailable(const ThrustPowerAvailable &copy);
   ThrustPowerAvailable& operator=(const ThrustPowerAvailable &right);
   virtual ~ThrustPowerAvailable();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:
   Integer mThrustPowerId;

};


class GMAT_API AreaCoefficient : public HardwareReal
{
public:

   AreaCoefficient(const std::string &name = "", GmatBase *obj = NULL);
   AreaCoefficient(const AreaCoefficient &copy);
   AreaCoefficient& operator=(const AreaCoefficient &right);
   virtual ~AreaCoefficient();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API SpecularFraction : public HardwareReal
{
public:

   SpecularFraction(const std::string &name = "", GmatBase *obj = NULL);
   SpecularFraction(const SpecularFraction &copy);
   SpecularFraction& operator=(const SpecularFraction &right);
   virtual ~SpecularFraction();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API DiffuseFraction : public HardwareReal
{
public:

   DiffuseFraction(const std::string &name = "", GmatBase *obj = NULL);
   DiffuseFraction(const DiffuseFraction &copy);
   DiffuseFraction& operator=(const DiffuseFraction &right);
   virtual ~DiffuseFraction();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API Area : public HardwareReal
{
public:

   Area(const std::string &name = "", GmatBase *obj = NULL);
   Area(const Area &copy);
   Area& operator=(const Area &right);
   virtual ~Area();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API Type : public HardwareString
{
public:

   Type(const std::string &name = "", GmatBase *obj = NULL);
   Type(const Type &copy);
   Type& operator=(const Type &right);
   virtual ~Type();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetString(std::string& val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API PlateNormal : public HardwareRvector
{
public:

   PlateNormal(const std::string &name = "", GmatBase *obj = NULL);
   PlateNormal(const PlateNormal &copy);
   PlateNormal& operator=(const PlateNormal &right);
   virtual ~PlateNormal();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetRvector(Rvector& val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API LitFraction : public HardwareReal
{
public:

   LitFraction(const std::string &name = "", GmatBase *obj = NULL);
   LitFraction(const LitFraction &copy);
   LitFraction& operator=(const LitFraction &right);
   virtual ~LitFraction();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};




class GMAT_API PlateNormalHistoryFile : public HardwareString
{
public:

   PlateNormalHistoryFile(const std::string &name = "", GmatBase *obj = NULL);
   PlateNormalHistoryFile(const PlateNormalHistoryFile &copy);
   PlateNormalHistoryFile& operator=(const PlateNormalHistoryFile &right);
   virtual ~PlateNormalHistoryFile();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetString(std::string& val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API AreaCoefficientSigma : public HardwareReal
{
public:

   AreaCoefficientSigma(const std::string &name = "", GmatBase *obj = NULL);
   AreaCoefficientSigma(const AreaCoefficientSigma &copy);
   AreaCoefficientSigma& operator=(const AreaCoefficientSigma &right);
   virtual ~AreaCoefficientSigma();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API SpecularFractionSigma : public HardwareReal
{
public:

   SpecularFractionSigma(const std::string &name = "", GmatBase *obj = NULL);
   SpecularFractionSigma(const SpecularFractionSigma &copy);
   SpecularFractionSigma& operator=(const SpecularFractionSigma &right);
   virtual ~SpecularFractionSigma();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API DiffuseFractionSigma : public HardwareReal
{
public:

   DiffuseFractionSigma(const std::string &name = "", GmatBase *obj = NULL);
   DiffuseFractionSigma(const DiffuseFractionSigma &copy);
   DiffuseFractionSigma& operator=(const DiffuseFractionSigma &right);
   virtual ~DiffuseFractionSigma();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API PlateX : public HardwareReal
{
public:

   PlateX(const std::string &name = "", GmatBase *obj = NULL);
   PlateX(const PlateX &copy);
   PlateX& operator=(const PlateX &right);
   virtual ~PlateX();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API PlateY : public HardwareReal
{
public:

   PlateY(const std::string &name = "", GmatBase *obj = NULL);
   PlateY(const PlateY &copy);
   PlateY& operator=(const PlateY &right);
   virtual ~PlateY();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API PlateZ : public HardwareReal
{
public:

   PlateZ(const std::string &name = "", GmatBase *obj = NULL);
   PlateZ(const PlateZ &copy);
   PlateZ& operator=(const PlateZ &right);
   virtual ~PlateZ();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};
#endif //HardwareParameters_hpp
