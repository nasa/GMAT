//$Id$
//------------------------------------------------------------------------------
//                                  BallisticMassParameters
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
 * Implements BallisticMass related parameter classes.
 *    DryMass, DryCenterOfMass[XYZ], DryMomentOfInertia[XX-ZZ], 
 *    DragCoeff, ReflectCoeff, DragArea,
 *    SRPArea, TotalMass, SystemCenterOfMass[XYZ], SystemMomentOfInertia[XX-ZZ]
 */
//------------------------------------------------------------------------------
#ifndef BallisticMassParameters_hpp
#define BallisticMassParameters_hpp

#include "gmatdefs.hpp"
#include "BallisticMassReal.hpp"


//------------------------------------------------------------------------------
//  DryMass, DryCenterOfMass[XYZ], DryMomentOfInertia[XX-ZZ],
//  DragCoeff, ReflectCoeff, DragArea, SRPArea,
//  TotalMass,SystemCenterOfMass[XYZ], SystemMomentOfInertia[XX-ZZ]
//------------------------------------------------------------------------------

class GMAT_API DryMass : public BallisticMassReal
{
public:

   DryMass(const std::string &name = "", GmatBase *obj = NULL);
   DryMass(const DryMass &copy);
   DryMass& operator=(const DryMass &right);
   virtual ~DryMass();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API DryCenterOfMassX : public BallisticMassReal
{
public:

   DryCenterOfMassX(const std::string &name = "", GmatBase *obj = NULL);
   DryCenterOfMassX(const DryCenterOfMassX &copy);
   DryCenterOfMassX& operator=(const DryCenterOfMassX &right);
   virtual ~DryCenterOfMassX();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API DryCenterOfMassY : public BallisticMassReal
{
public:

   DryCenterOfMassY(const std::string &name = "", GmatBase *obj = NULL);
   DryCenterOfMassY(const DryCenterOfMassY &copy);
   DryCenterOfMassY& operator=(const DryCenterOfMassY &right);
   virtual ~DryCenterOfMassY();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API DryCenterOfMassZ : public BallisticMassReal
{
public:

   DryCenterOfMassZ(const std::string &name = "", GmatBase *obj = NULL);
   DryCenterOfMassZ(const DryCenterOfMassZ &copy);
   DryCenterOfMassZ& operator=(const DryCenterOfMassZ &right);
   virtual ~DryCenterOfMassZ();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API DryMomentOfInertiaXX : public BallisticMassReal
{
public:

   DryMomentOfInertiaXX(const std::string &name = "", GmatBase *obj = NULL);
   DryMomentOfInertiaXX(const DryMomentOfInertiaXX &copy);
   DryMomentOfInertiaXX& operator=(const DryMomentOfInertiaXX &right);
   virtual ~DryMomentOfInertiaXX();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API DryMomentOfInertiaXY : public BallisticMassReal
{
public:

   DryMomentOfInertiaXY(const std::string &name = "", GmatBase *obj = NULL);
   DryMomentOfInertiaXY(const DryMomentOfInertiaXY &copy);
   DryMomentOfInertiaXY& operator=(const DryMomentOfInertiaXY &right);
   virtual ~DryMomentOfInertiaXY();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API DryMomentOfInertiaXZ : public BallisticMassReal
{
public:

   DryMomentOfInertiaXZ(const std::string &name = "", GmatBase *obj = NULL);
   DryMomentOfInertiaXZ(const DryMomentOfInertiaXZ &copy);
   DryMomentOfInertiaXZ& operator=(const DryMomentOfInertiaXZ &right);
   virtual ~DryMomentOfInertiaXZ();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API DryMomentOfInertiaYY : public BallisticMassReal
{
public:

   DryMomentOfInertiaYY(const std::string &name = "", GmatBase *obj = NULL);
   DryMomentOfInertiaYY(const DryMomentOfInertiaYY &copy);
   DryMomentOfInertiaYY& operator=(const DryMomentOfInertiaYY &right);
   virtual ~DryMomentOfInertiaYY();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API DryMomentOfInertiaYZ : public BallisticMassReal
{
public:

   DryMomentOfInertiaYZ(const std::string &name = "", GmatBase *obj = NULL);
   DryMomentOfInertiaYZ(const DryMomentOfInertiaYZ &copy);
   DryMomentOfInertiaYZ& operator=(const DryMomentOfInertiaYZ &right);
   virtual ~DryMomentOfInertiaYZ();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API DryMomentOfInertiaZZ : public BallisticMassReal
{
public:

   DryMomentOfInertiaZZ(const std::string &name = "", GmatBase *obj = NULL);
   DryMomentOfInertiaZZ(const DryMomentOfInertiaZZ &copy);
   DryMomentOfInertiaZZ& operator=(const DryMomentOfInertiaZZ &right);
   virtual ~DryMomentOfInertiaZZ();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API DragCoeff : public BallisticMassReal
{
public:

   DragCoeff(const std::string &name = "", GmatBase *obj = NULL);
   DragCoeff(const DragCoeff &copy);
   DragCoeff& operator=(const DragCoeff &right);
   virtual ~DragCoeff();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API ReflectCoeff : public BallisticMassReal
{
public:

   ReflectCoeff(const std::string &name = "", GmatBase *obj = NULL);
   ReflectCoeff(const ReflectCoeff &copy);
   ReflectCoeff& operator=(const ReflectCoeff &right);
   virtual ~ReflectCoeff();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API DragArea : public BallisticMassReal
{
public:

   DragArea(const std::string &name = "", GmatBase *obj = NULL);
   DragArea(const DragArea &copy);
   DragArea& operator=(const DragArea &right);
   virtual ~DragArea();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API SRPArea : public BallisticMassReal
{
public:

   SRPArea(const std::string &name = "", GmatBase *obj = NULL);
   SRPArea(const SRPArea &copy);
   SRPArea& operator=(const SRPArea &right);
   virtual ~SRPArea();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API TotalMass : public BallisticMassReal
{
public:

   TotalMass(const std::string &name = "", GmatBase *obj = NULL);
   TotalMass(const TotalMass &copy);
   TotalMass& operator=(const TotalMass &right);
   virtual ~TotalMass();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API SystemCenterOfMassX : public BallisticMassReal
{
public:

   SystemCenterOfMassX(const std::string &name = "", GmatBase *obj = NULL);
   SystemCenterOfMassX(const SystemCenterOfMassX &copy);
   SystemCenterOfMassX& operator=(const SystemCenterOfMassX &right);
   virtual ~SystemCenterOfMassX();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API SystemCenterOfMassY : public BallisticMassReal
{
public:

   SystemCenterOfMassY(const std::string &name = "", GmatBase *obj = NULL);
   SystemCenterOfMassY(const SystemCenterOfMassY &copy);
   SystemCenterOfMassY& operator=(const SystemCenterOfMassY &right);
   virtual ~SystemCenterOfMassY();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API SystemCenterOfMassZ : public BallisticMassReal
{
public:

   SystemCenterOfMassZ(const std::string &name = "", GmatBase *obj = NULL);
   SystemCenterOfMassZ(const SystemCenterOfMassZ &copy);
   SystemCenterOfMassZ& operator=(const SystemCenterOfMassZ &right);
   virtual ~SystemCenterOfMassZ();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API SystemMomentOfInertiaXX : public BallisticMassReal
{
public:

   SystemMomentOfInertiaXX(const std::string &name = "", GmatBase *obj = NULL);
   SystemMomentOfInertiaXX(const SystemMomentOfInertiaXX &copy);
   SystemMomentOfInertiaXX& operator=(const SystemMomentOfInertiaXX &right);
   virtual ~SystemMomentOfInertiaXX();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API SystemMomentOfInertiaXY : public BallisticMassReal
{
public:

   SystemMomentOfInertiaXY(const std::string &name = "", GmatBase *obj = NULL);
   SystemMomentOfInertiaXY(const SystemMomentOfInertiaXY &copy);
   SystemMomentOfInertiaXY& operator=(const SystemMomentOfInertiaXY &right);
   virtual ~SystemMomentOfInertiaXY();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API SystemMomentOfInertiaXZ : public BallisticMassReal
{
public:

   SystemMomentOfInertiaXZ(const std::string &name = "", GmatBase *obj = NULL);
   SystemMomentOfInertiaXZ(const SystemMomentOfInertiaXZ &copy);
   SystemMomentOfInertiaXZ& operator=(const SystemMomentOfInertiaXZ &right);
   virtual ~SystemMomentOfInertiaXZ();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API SystemMomentOfInertiaYY : public BallisticMassReal
{
public:

   SystemMomentOfInertiaYY(const std::string &name = "", GmatBase *obj = NULL);
   SystemMomentOfInertiaYY(const SystemMomentOfInertiaYY &copy);
   SystemMomentOfInertiaYY& operator=(const SystemMomentOfInertiaYY &right);
   virtual ~SystemMomentOfInertiaYY();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API SystemMomentOfInertiaYZ : public BallisticMassReal
{
public:

   SystemMomentOfInertiaYZ(const std::string &name = "", GmatBase *obj = NULL);
   SystemMomentOfInertiaYZ(const SystemMomentOfInertiaYZ &copy);
   SystemMomentOfInertiaYZ& operator=(const SystemMomentOfInertiaYZ &right);
   virtual ~SystemMomentOfInertiaYZ();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API SystemMomentOfInertiaZZ : public BallisticMassReal
{
public:

   SystemMomentOfInertiaZZ(const std::string &name = "", GmatBase *obj = NULL);
   SystemMomentOfInertiaZZ(const SystemMomentOfInertiaZZ &copy);
   SystemMomentOfInertiaZZ& operator=(const SystemMomentOfInertiaZZ &right);
   virtual ~SystemMomentOfInertiaZZ();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API  SpadDragScaleFactor: public BallisticMassReal
{
public:

   SpadDragScaleFactor(const std::string &name = "", GmatBase *obj = NULL);
   SpadDragScaleFactor(const SpadDragScaleFactor &copy);
   SpadDragScaleFactor& operator=(const SpadDragScaleFactor &right);
   virtual ~SpadDragScaleFactor();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};


class GMAT_API SpadSRPScaleFactor : public BallisticMassReal
{
public:

   SpadSRPScaleFactor(const std::string &name = "", GmatBase *obj = NULL);
   SpadSRPScaleFactor(const SpadSRPScaleFactor &copy);
   SpadSRPScaleFactor& operator=(const SpadSRPScaleFactor &right);
   virtual ~SpadSRPScaleFactor();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API AtmosDensityScaleFactor : public BallisticMassReal
{
public:

   AtmosDensityScaleFactor(const std::string &name = "", GmatBase *obj = NULL);
   AtmosDensityScaleFactor(const AtmosDensityScaleFactor &copy);
   AtmosDensityScaleFactor& operator=(const AtmosDensityScaleFactor &right);
   virtual ~AtmosDensityScaleFactor();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API DragCoeffSigma : public BallisticMassReal
{
public:

   DragCoeffSigma(const std::string &name = "", GmatBase *obj = NULL);
   DragCoeffSigma(const DragCoeffSigma &copy);
   DragCoeffSigma& operator=(const DragCoeffSigma &right);
   virtual ~DragCoeffSigma();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API ReflectCoeffSigma : public BallisticMassReal
{
public:

   ReflectCoeffSigma(const std::string &name = "", GmatBase *obj = NULL);
   ReflectCoeffSigma(const ReflectCoeffSigma &copy);
   ReflectCoeffSigma& operator=(const ReflectCoeffSigma &right);
   virtual ~ReflectCoeffSigma();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API  SpadDragScaleFactorSigma : public BallisticMassReal
{
public:

   SpadDragScaleFactorSigma(const std::string &name = "", GmatBase *obj = NULL);
   SpadDragScaleFactorSigma(const SpadDragScaleFactorSigma &copy);
   SpadDragScaleFactorSigma& operator=(const SpadDragScaleFactorSigma &right);
   virtual ~SpadDragScaleFactorSigma();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API SpadSRPScaleFactorSigma : public BallisticMassReal
{
public:

   SpadSRPScaleFactorSigma(const std::string &name = "", GmatBase *obj = NULL);
   SpadSRPScaleFactorSigma(const SpadSRPScaleFactorSigma &copy);
   SpadSRPScaleFactorSigma& operator=(const SpadSRPScaleFactorSigma &right);
   virtual ~SpadSRPScaleFactorSigma();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class GMAT_API AtmosDensityScaleFactorSigma : public BallisticMassReal
{
public:

   AtmosDensityScaleFactorSigma(const std::string &name = "", GmatBase *obj = NULL);
   AtmosDensityScaleFactorSigma(const AtmosDensityScaleFactorSigma &copy);
   AtmosDensityScaleFactorSigma& operator=(const AtmosDensityScaleFactorSigma &right);
   virtual ~AtmosDensityScaleFactorSigma();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};


#endif //BallisticMassParameters_hpp
