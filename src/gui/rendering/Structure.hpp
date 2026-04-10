//$Id$
//====================================================================
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
// Author: John P. Downing/GSFC/595
// Created: 2015.08.31
/**
 * Structure Class Family, these classes implement the appenage, joint
 * and Structure classes.
 */
//====================================================================
#ifndef Structure_hpp
#define Structure_hpp
#include "gmatdefs.hpp"
#include "RgbColor.hpp"
#include "Rvector3.hpp"
#include "Rmatrix33.hpp"
#include "SurfaceBase.hpp"
//====================================================================
class ZMaterial
{
public:
// --------------------------- Constructors, Assignment, Destructors
   ZMaterial (const std::string& name);
   ZMaterial (const std::string& name, const Integer& r, const Integer& g, 
      const Integer& b, const Integer& shininess);
private: // Copy Protected
   ZMaterial (const ZMaterial&);
   ZMaterial& operator= (const ZMaterial&);
public:
   ~ZMaterial();
// --------------------------- Functions
   void WriteSummary (std::ostream& s, const Integer level) const;
// --------------------------- Data Members
public:
   std::string Name;
   RgbColor    Color;
   Integer     Shininess;
   std::string TextureMap;
   Real        UScale;
   Real        VScale;
   Real        UOffset;
   Real        VOffset;
   Integer     GlTextureId;
// --------------------------- 
};
//====================================================================
class ZMaterialArray
{
public:
// --------------------------- Constructors, Assignment, Destructors
   ZMaterialArray (const Integer size);
private: // Copy Protected
   ZMaterialArray (const ZMaterialArray&);
   ZMaterialArray& operator= (const ZMaterialArray&);
public:
   ~ZMaterialArray();
// --------------------------- Functions
   Integer Size () const;
   Integer Add (ZMaterial* x);
   const ZMaterial* operator[] (const Integer& ix) const;
   ZMaterial* operator[] (const Integer& ix);
// --------------------------- Data Members
private:
   Integer       Max;
   Integer       TheSize;
   ZMaterial**   Data;
// --------------------------- 
};
//====================================================================
class Joint
{
public:
// --------------------------- Constructors, Assignment, Destructors
   Joint ();
private: // Copy Protected
   Joint (const Joint&);
   Joint& operator= (const Joint&);
public:
   ~Joint();
// --------------------------- Functions
   Real GetAngleDeg (const Integer& ix) const;
   Rmatrix33 GetMatrix () const;
   void SetTurn (const bool& inuse, const Integer& ix,
      const Integer& axis, const Real& angledeg);
   void SetToDefault ();
   void SetAngleDeg (const Integer& ix, const Real& angledeg);
   std::string AxisString (const Integer& ix) const;

   void WriteSummary (std::ostream& s, const Integer level) const;
// --------------------------- Data Members
public:
   Rvector3    BaseOffset;
   Rvector3    AppendageOffset;
   bool        Use[3];
   Rvector3    Axis[3];
   bool        Vary[3];
   Real        Minimum[3];
   Real        Maximum[3];
   Real        DefaultDeg[3];
   Real        CurrDeg[3];
// --------------------------- Data Members
public:
   static const Rvector3    XX;
   static const Rvector3    YY;
   static const Rvector3    ZZ;
// --------------------------- 
};
//====================================================================
class ZAppendage
{
public:
// --------------------------- Constructors, Assignment, Destructors
   ZAppendage ();
private: // Copy Protected
   ZAppendage (const ZAppendage&);
   ZAppendage& operator= (const ZAppendage&);
public:
   ~ZAppendage();
// --------------------------- Functions
   Integer GetSurfaceCount () const;
   SurfaceBase* GetSurface (const Integer& ix);

   void WriteSummary (std::ostream& s, const Integer level) const;
// --------------------------- Data Members
public:
   RgbColor*    Color;
   SurfaceGroup Body;
   Joint        TheJoint;
   std::string  Base;
// --------------------------- StaticData Members
public: 
   static Integer BodyCount;
// --------------------------- 
};
//====================================================================
class ZAppendageArray
{
public:
// --------------------------- Constructors, Assignment, Destructors
   ZAppendageArray (const Integer size);
private: // Copy Protected
   ZAppendageArray (const ZAppendageArray&);
   ZAppendageArray& operator= (const ZAppendageArray&);
public:
   ~ZAppendageArray();
// --------------------------- Functions
   Integer Size () const;
   Integer Add (ZAppendage* x);
   const ZAppendage* operator[] (const Integer& ix) const;
   ZAppendage* operator[] (const Integer& ix);
// --------------------------- Data Members
private:
   Integer       Max;
   Integer       TheSize;
   ZAppendage**  Data;
// --------------------------- 
};
//====================================================================
class Structure
{
public:
// --------------------------- Constructors, Assignment, Destructors
   Structure (const std::string& name);
private: // Copy Protected
   Structure (const Structure&);
   Structure& operator= (const Structure&);
public:
   ~Structure();
// --------------------------- Functions
   void AddAppendage (ZAppendage* appendage);
   void AddMaterial (ZMaterial* material);
   void SetJointsToDefault ();
   ZMaterial* FindMaterial (const std::string& name);
private:
   ZAppendage* FindAppendage (const std::string& name);
   void CalcBodyRotations ();
public:
   void CalcCenter ();
   void Write (std::string filename) const;
   void Render ();
   void WriteSummary (std::ostream& s, const Integer level) const;
// --------------------------- Data Members
public:
   std::string     Name;
   RgbColor        Color;
   ZAppendageArray Appendages;
   ZMaterialArray  Materials;
   Rvector3        Center;
   Real            Radius;
// --------------------------- 
};
//====================================================================
#endif 
