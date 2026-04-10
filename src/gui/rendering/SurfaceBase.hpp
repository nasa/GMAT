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
 * Structure Class Family, these classes implement polygon mesh and
 * Groups of such meshes.  Thet could be improved for more efficient rendering
 */
//====================================================================
#ifndef SurfaceBase_hpp
#define SurfaceBase_hpp
#include "gmatdefs.hpp"
#include "Rvector3.hpp"
//====================================================================
class Structure;
class ZMinMax;
//====================================================================
class SurfaceBase
{
public:
// --------------------------- Constructors, Assignment, Destructors
   SurfaceBase (const std::string& name);
private: // Copy Protected
   SurfaceBase (const SurfaceBase&);
   SurfaceBase& operator= (const SurfaceBase&);
public:
   virtual ~SurfaceBase();
// --------------------------- Functions
   virtual void InitWrtSpacecraft () = 0;
   virtual ZMinMax MinMax () = 0;
   virtual void RotateBody (const Rmatrix33& ts, const Rvector3 baseoffset,
      const Rvector3 appendageoffset) = 0;
   virtual void Render (Structure* structure) const = 0;
   virtual void WriteSummary (std::ostream& s, const Integer level) const = 0;
// --------------------------- Static Functions
private:
   static std::string MakeSurfaceName (const Integer& ix);
// --------------------------- Data Members
public:
   std::string Name;
   std::string TheMaterial;
// --------------------------- 
};
//====================================================================
class ZChildArray
{
public:
// --------------------------- Constructors, Assignment, Destructors
   ZChildArray (const Integer size);
private: // Copy Protected
   ZChildArray (const ZChildArray&);
   ZChildArray& operator= (const ZChildArray&);
public:
   ~ZChildArray();
// --------------------------- Functions
   Integer Size () const;
   Integer Add (SurfaceBase* x);
   const SurfaceBase* operator[] (const Integer& ix) const;
   SurfaceBase* operator[] (const Integer& ix);
// --------------------------- Data Members
private:
   Integer       Max;
   Integer       TheSize;
   SurfaceBase** Data;
// --------------------------- 
};
//====================================================================
class SurfaceGroup : public SurfaceBase
{
public:
// --------------------------- Constructors, Assignment, Destructors
   SurfaceGroup (const std::string& name);
   // Inherit copy protection
   virtual ~SurfaceGroup ();
// --------------------------- Functions
   void AddChild (SurfaceBase* child);   
   virtual void InitWrtSpacecraft ();
   virtual ZMinMax MinMax ();
   virtual void RotateBody (const Rmatrix33& ts, const Rvector3 baseoffset,
      const Rvector3 appendageoffset);
   virtual void Render (Structure* structure) const;
   virtual void WriteSummary (std::ostream& s, const Integer level) const;
// --------------------------- Data Members
public:
   ZChildArray Children;
// --------------------------- 
};
//====================================================================
class ZFloat3 
{
public:
// --------------------------- Constructors, Assignment, Destructors
   ZFloat3 ();
   ZFloat3 (const Real& x, const Real& y, const Real& z);
   // Default Copy Constructor OK
   // Default Assignment Operator OK
   // Default Destructor OK
// --------------------------- Functions
   Rvector3 ConvertToRvector3 () const;
// --------------------------- Data Members
public:
   float V[3];
// --------------------------- 
};
//====================================================================
class ZMinMax
{
public:
// --------------------------- Constructors, Assignment, Destructors
   ZMinMax ();
   // Default Copy Constructor OK
   // Default Assignment Operator OK
   // Default Destructor OK
// --------------------------- Functions
   void Broaden (const ZFloat3 x);
   void Broaden (const ZMinMax x);
// --------------------------- Data Members
public:
   ZFloat3 Min;
   ZFloat3 Max;
// --------------------------- 
};
//====================================================================
class ZVectorCache
{
public:
// --------------------------- Constructors, Assignment, Destructors
   ZVectorCache ();
   ZVectorCache (const Rvector3& v, const bool& dotranslate);
   // Default Copy Constructor OK
   // Default Assignment Operator OK
   // Default Destructor OK
// --------------------------- Functions
   Rvector3 WrtGl ();
// --------------------------- Data Members
public:
   ZFloat3 WrtBody;
   bool    DoTranslate;
   ZFloat3 WrtSpacecraft;
// --------------------------- 
};
//====================================================================
class ZVectorCacheArray
{
public:
// --------------------------- Constructors, Assignment, Destructors
   ZVectorCacheArray (const Integer size);
private: // Copy Protected
   ZVectorCacheArray (const ZVectorCacheArray&);
   ZVectorCacheArray& operator= (const ZVectorCacheArray&);
public:
   ~ZVectorCacheArray();
// --------------------------- Functions
   Integer Size () const;
   Integer Add (const ZVectorCache x);
   ZMinMax MinMax () const;
   const ZVectorCache& operator[] (const Integer& ix) const;
   ZVectorCache& operator[] (const Integer& ix);
// --------------------------- Data Members
private:
   Integer       Max;
   Integer       TheSize;
   ZVectorCache* Data;
// --------------------------- 
};
//====================================================================
class ZRealPoint 
{
public:
// --------------------------- Constructors, Assignment, Destructors
   ZRealPoint ();
   ZRealPoint (const Real& x, const Real& y);
   // Default Copy Constructor OK
   // Default Assignment Operator OK
   // Default Destructor OK
// --------------------------- Data Members
public:
   float V[2];
// --------------------------- 
};
//====================================================================
class ZFace 
{
public:
// --------------------------- Constructors, Assignment, Destructors
   ZFace ();
   // Default Copy Constructor OK
   // Default Assignment Operator OK
   // Default Destructor OK
// --------------------------- Functions
   Real* Sides (const ZVectorCacheArray& v);  // be sure to delete result
   Real Perimeter (const ZVectorCacheArray& v);
   Real Area (const ZVectorCacheArray& v);
   Real Density (const ZVectorCacheArray& v);
// --------------------------- Data Members
public:
   Integer    VertexIndex[3];
   ZRealPoint TextureCoord[3];
   Integer    NormalIndex[3];
// --------------------------- 
};
//====================================================================
class SurfaceMesh : public SurfaceBase
{
public:
// --------------------------- Constructors, Assignment, Destructors
   SurfaceMesh (const std::string& name);
   SurfaceMesh (const std::string& name, const Integer& vertexcount,
      const Integer& facecount);
   // Inherit copy protection
   virtual ~SurfaceMesh();
// --------------------------- Functions
   Integer GetVectorCount () const;
   ZFloat3 GetVectorWrt (const Integer& ix, const bool& wrtspacecraft) const;
   ZFloat3 VectorWrtBody (const Integer& ix) const;
   ZFloat3 VectorWrtSpacecraft (const Integer& ix) const;
   void BuildNormals ();
   Integer MakeVector (const Rvector3& v, const bool& relative);
   virtual void InitWrtSpacecraft ();
   virtual ZMinMax MinMax ();
   virtual void RotateBody (const Rmatrix33& ts, const Rvector3 baseoffset,
      const Rvector3 appendageoffset);
   virtual void Render (Structure* structure) const;
   virtual void WriteSummary (std::ostream& s, const Integer level) const;
// --------------------------- Data Members
public:
   ZVectorCacheArray Vectors;
   Integer           FacesSize;
   ZFace*            Faces;
// --------------------------- 
};
//====================================================================
#endif 
