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
#include "SurfaceBase.hpp"
#include "Structure.hpp"
#include "StringUtil.hpp"
#include "Rmatrix33.hpp"
#include "GmatOpenGLSupport.hpp"
#include "gmatwxdefs.hpp"
#include "MessageInterface.hpp"
//====================================================================
SurfaceBase::SurfaceBase (const std::string& name)
// Constructor
   : Name (name),
     TheMaterial ("")
   { }
//------------------------------------------------------------------------------
SurfaceBase::~SurfaceBase()
// Destructor
   {
   }
//--------------------------------------------------------------------
std::string SurfaceBase::MakeSurfaceName (const Integer& ix)
// Makes a unique name for an unnamed surface read from a file
   {
   std::string out = "S" + GmatStringUtil::ToString(ix,4);
   for (Integer i=1;  i<=4;  ++i)
      if (out[i] == ' ')
         out[i] = '0';
   return out;
   }
//====================================================================
ZChildArray::ZChildArray (const Integer size)
// Constructor.  In afect world, this would be
// std::vector<SurfaceBase*>
   : Max (100),
     TheSize (size),
     Data (new SurfaceBase*[Max])
   {
   }
//--------------------------------------------------------------------
ZChildArray::~ZChildArray()
// Destructor
   {
   for (Integer ic=0;  ic<=TheSize-1;  ++ic)
      {
      if (Data[ic] != 0)
         delete Data[ic];
      Data[ic] = 0;
      }
   delete[] Data;
   }
//--------------------------------------------------------------------
Integer ZChildArray::Size () const
// Returns the size of an array
   {
   return TheSize;
   }
//--------------------------------------------------------------------
Integer ZChildArray::Add (SurfaceBase* x)
// Adds a new surface to the array of surfaces, and returns the index
// of the new element
   {
   if (TheSize == Max)
      {
      SurfaceBase** old = Data;
      Data = new SurfaceBase*[Max*2];
      for (int i=0;  i<=TheSize-1;  ++i)
         Data[i] = old[i];
      delete[] old;
      Max = Max*2;
      }
   Data[TheSize] = x;
      ++TheSize;
   return TheSize-1;
   }
//--------------------------------------------------------------------
const SurfaceBase* ZChildArray::operator[] (const Integer& ix) const
// Returns an element of an array (const version for access)
   {
   if (ix < 0) return Data[0];
   if (ix >= TheSize) return Data[0];
   return Data[ix];
   }
//--------------------------------------------------------------------
SurfaceBase* ZChildArray::operator[] (const Integer& ix) 
// Returns an element of an array (non-const version for assignment)
   {
   if (ix < 0) return Data[0];
   if (ix >= TheSize) return Data[0];
   return Data[ix];
   }
//====================================================================
SurfaceGroup::SurfaceGroup (const std::string& name) 
// Constructor
   : SurfaceBase (name),
     Children (0)
   {
   }
//------------------------------------------------------------------------------
SurfaceGroup::~SurfaceGroup ()
// Destructor
   {
   }
//--------------------------------------------------------------------
void SurfaceGroup::AddChild (SurfaceBase* child)
// Adds a new surface to the array of surfaces, 
   {
   Children.Add (child);
   }   
//--------------------------------------------------------------------
// The following routines just bass the call through to each child, 
// collection the results as needed.
//--------------------------------------------------------------------
void SurfaceGroup::InitWrtSpacecraft ()
   {
   for (Integer ic=0;  ic<=Children.Size()-1;  ++ic)
      Children[ic]->InitWrtSpacecraft();
   }
//--------------------------------------------------------------------
ZMinMax SurfaceGroup::MinMax ()
   {
   ZMinMax out;
   for (Integer ic=0;  ic<=Children.Size()-1;  ++ic)
      out.Broaden(Children[ic]->MinMax());
   return out;
   }
//--------------------------------------------------------------------
void SurfaceGroup::RotateBody (const Rmatrix33& ts, const Rvector3 baseoffset,
   const Rvector3 appendageoffset)
   {
   for (Integer ic=0;  ic<=Children.Size()-1;  ++ic)
      Children[ic]->RotateBody(ts,baseoffset,appendageoffset);
   }
//--------------------------------------------------------------------
void SurfaceGroup::Render (Structure* structure) const
// Draw the mesh in opengl
   {
   for (Integer ic=0;  ic<=Children.Size()-1;  ++ic)
      Children[ic]->Render (structure);
   }
//--------------------------------------------------------------------
void SurfaceGroup::WriteSummary (std::ostream& s, const Integer level) const
// Write a breigf summary of the object
   {
   std::string indent = std::string (level*2,' ');
   s << indent << "SurfaceGroup, Child count = " << 
      GmatStringUtil::ToString (Children.Size(),true,4) << std::endl;
   for (Integer ic=0;  ic<=Children.Size()-1;  ++ic)
      Children[ic]->WriteSummary (s,level+1);
   }
//====================================================================
ZFloat3::ZFloat3 ()
// Constructor
   {
   V[0] = 0;
   V[1] = 0;
   V[2] = 0;
   }
//--------------------------------------------------------------------
ZFloat3::ZFloat3 (const Real& x, const Real& y, const Real& z)
// Constructor
   {
   V[0] = x;
   V[1] = y;
   V[2] = z;
   }
//--------------------------------------------------------------------
Rvector3 ZFloat3::ConvertToRvector3 () const
// Conversion to Real
   {
   Rvector3 out;
   for (int i=0;  i<=3-1;  ++i)
      out[i] = V[i];
   return out;
   }
//====================================================================
ZMinMax::ZMinMax ()
// Constructor
   {
   for (Integer i=0;  i<=3-1;  ++i)
      {
      Min.V[i] = 999999.0F;
      Max.V[i] = -999999.0F;
      }
   }
//--------------------------------------------------------------------
void ZMinMax::Broaden (const ZFloat3 x)
// Expands the minimum and maxcimum to include the new vector
   {
   for (Integer i=0;  i<=3-1;  ++i)
      {
      if (x.V[i] < Min.V[i]) Min.V[i] = x.V[i];
      if (x.V[i] > Max.V[i]) Max.V[i] = x.V[i];
      }
   }
//--------------------------------------------------------------------
void ZMinMax::Broaden (const ZMinMax x)
// Expands the minimum and maxcimum to include the limits in another
// ZMinMax object
   {
   for (Integer i=0;  i<=3-1;  ++i)
      {
      if (x.Min.V[i] < Min.V[i]) Min.V[i] = x.Min.V[i];
      if (x.Max.V[i] > Max.V[i]) Max.V[i] = x.Max.V[i];
      }
   }
//====================================================================
ZVectorCache::ZVectorCache ()
// Constructor
   : WrtBody (),
     DoTranslate (false),
     WrtSpacecraft ()
   {
   }
//--------------------------------------------------------------------
ZVectorCache::ZVectorCache (const Rvector3& v, const bool& dotranslate)
// Constructor
   : WrtBody (),
     DoTranslate (dotranslate),
     WrtSpacecraft ()
   {
   for (int i=0;  i<=3-1;  ++i)
      WrtBody.V[i] = v[i];
   }
//--------------------------------------------------------------------
Rvector3 ZVectorCache::WrtGl ()
   {
   return WrtSpacecraft.ConvertToRvector3();
   }
//====================================================================
ZVectorCacheArray::ZVectorCacheArray (const Integer size)
// Constructor.  In a perfect world, this would be
// std::vector<ZVectorCache>
   : Max (size),
     TheSize (size),
     Data (new ZVectorCache[Max])
   {
   }
//------------------------------------------------------------------------------
ZVectorCacheArray::~ZVectorCacheArray()
// Destructor
   {
   delete[] Data;
   }
//--------------------------------------------------------------------
Integer ZVectorCacheArray::Size () const
// Returns the size of an array
   {
   return TheSize;
   }
//--------------------------------------------------------------------
Integer ZVectorCacheArray::Add (const ZVectorCache x)
// Adds a new ZVectorCache to the array of ZVectorCaches, and returns the index
// of the new element
   {
   if (TheSize == Max)
      {
      ZVectorCache* old = Data;
      Data = new ZVectorCache[Max*2];
      for (int i=0;  i<=TheSize-1;  ++i)
         Data[i] = old[i];
      delete[] old;
      Max = Max*2;
      }
   Data[TheSize] = x;
      ++TheSize;
   return TheSize-1;
   }
//--------------------------------------------------------------------
ZMinMax ZVectorCacheArray::MinMax () const
// Returns the minimum and maximum for the vectors in the array
   {
   ZMinMax out;
   for (int i=0;  i<=TheSize-1;  ++i)
      if (Data[i].DoTranslate)
         out.Broaden(Data[i].WrtBody);
   return out;
   }
//--------------------------------------------------------------------
const ZVectorCache& ZVectorCacheArray::operator[] (const Integer& ix) const
// Returns an element of an array (const version for access)
   {
   if (ix < 0) return Data[0];
   if (ix >= TheSize) return Data[0];
   return Data[ix];
   }
//--------------------------------------------------------------------
ZVectorCache& ZVectorCacheArray::operator[] (const Integer& ix) 
// Returns an element of an array (non-const version for assignment)
   {
   if (ix < 0) return Data[0];
   if (ix >= TheSize) return Data[0];
   return Data[ix];
   }
//====================================================================
ZRealPoint::ZRealPoint ()
// Constructor
   {
   V[0] = 0;
   V[1] = 0;
   }
//--------------------------------------------------------------------
ZRealPoint::ZRealPoint (const Real& x, const Real& y)
// Constructor
   {
   V[0] = x;
   V[1] = y;
   }
//====================================================================
ZFace::ZFace ()
// Constructor
   {
   for (int i=0;  i<=3-1;  ++i)
      {
      VertexIndex[i] = -1;
      TextureCoord[i] = ZRealPoint(0,0);
      NormalIndex[i] = -1;
      }
   }
//--------------------------------------------------------------------
Real* ZFace::Sides (const ZVectorCacheArray& v)  // be sure to delete result
// Returns an array of side lengths for the face
   {
   Real* out = new Real[3];
   for (int i=0;  i<=3-1;  ++i)
      {
      Rvector3 v1 = v[VertexIndex[i]].WrtBody.ConvertToRvector3();
      Rvector3 v2 = v[VertexIndex[(i+1)%3]].WrtBody.ConvertToRvector3();
      out[i] = (v1-v2).GetMagnitude();
      }
   return out;
   }
//--------------------------------------------------------------------
Real ZFace::Perimeter (const ZVectorCacheArray& v)
// Returns the perimeter of the face
   {
   Real out = 0;
   Real* ss = Sides(v); 
   for (int i=0;  i<=3-1;  ++i)
      out += ss[i];
   return out;
   }
//--------------------------------------------------------------------
Real ZFace::Area (const ZVectorCacheArray& v)
// Returns the area of the face
   {
   Real* ss = Sides(v); 
   Real p = Perimeter(v)/2;
   // Heron's formula
   return sqrt (p*(p-ss[0])*(p-ss[1])*(p-ss[2]));
   }
//--------------------------------------------------------------------
Real ZFace::Density (const ZVectorCacheArray& v)
// Returns the Density of the face, which is the fraction of an 
// enclosing circle that it fills
   {
   Real c = Perimeter(v);
   Real a = Area(v);
   Real r = c/2/GmatMathConstants::PI;
   Real pir2 = GmatMathConstants::PI*r*r;
   return a/pir2;
   }
//====================================================================
SurfaceMesh::SurfaceMesh (const std::string& name, const Integer& vertexcount,
   const Integer& facecount)
// Constructor
   : SurfaceBase (name),
     Vectors (facecount),
     FacesSize (facecount),
     Faces (new ZFace[FacesSize])
   {
   }
//--------------------------------------------------------------------
SurfaceMesh::~SurfaceMesh()
// Destructor
   {
   delete[] Faces;
   Faces = 0;
   }
//------------------------------------------------------------------------------
int SurfaceMesh::GetVectorCount () const
// returns the count of vectors in the mesh
   {
   return Vectors.Size();
   }
//------------------------------------------------------------------------------
ZFloat3 SurfaceMesh::GetVectorWrt (const Integer& ix, const bool& wrtspacecraft) const
// returns a vector
   {
   return wrtspacecraft ? VectorWrtSpacecraft(ix) : VectorWrtBody(ix);
   }
//------------------------------------------------------------------------------
ZFloat3 SurfaceMesh::VectorWrtBody (const Integer& ix) const
// returns a vector with respect to body
   {
   return Vectors[ix].WrtBody;
   }
//------------------------------------------------------------------------------
ZFloat3 SurfaceMesh::VectorWrtSpacecraft (const Integer& ix) const
// returns a vector with respect to spacecraft, that is including rotation of appendages
   {
   return Vectors[ix].WrtSpacecraft;
   }
//------------------------------------------------------------------------------
int SurfaceMesh::MakeVector (const Rvector3& v, const bool& dotranslate)
// Adds a vector to the array, and returns the index.
   {
   return Vectors.Add (ZVectorCache(v,dotranslate));
   }
//--------------------------------------------------------------------
ZMinMax SurfaceMesh::MinMax ()
// Returns the minimum and maximum for the vectors in the array
   {
   return Vectors.MinMax();
   }
//--------------------------------------------------------------------
void SurfaceMesh::InitWrtSpacecraft ()
// Sets the WrtSpacecraft vectors to their default values
   {
   for (int ip=0;  ip<=GetVectorCount()-1;  ++ip)
      {
      ZVectorCache& v = Vectors[ip];
      v.WrtSpacecraft = v.WrtBody;
      }
   }
//--------------------------------------------------------------------
void SurfaceMesh::RotateBody (const Rmatrix33& ts, const Rvector3 baseoffset,
   const Rvector3 appendageoffset)
// Rotates the spacecraft vector valeus
   {
   for (int ip=0;  ip<=GetVectorCount()-1;  ++ip)
      {
      ZVectorCache& v = Vectors[ip];
      Rvector3 aoffset = v.DoTranslate ? appendageoffset : Rvector3(0,0,0);
      Rvector3 boffset = v.DoTranslate ? baseoffset : Rvector3(0,0,0);
      Rvector3 result = ts * (v.WrtSpacecraft.ConvertToRvector3() - aoffset) + boffset;
      v.WrtSpacecraft = ZFloat3(result[0], result[1], result[2]);
      }
   }
//--------------------------------------------------------------------
void SurfaceMesh::BuildNormals ()
// Create the normal vectors for the faces
   {
   for (Integer i=0;  i<=FacesSize-1;  ++i)
      {
      ZFace& face (Faces[i]);
      Rvector3 x = Vectors[face.VertexIndex[0]].WrtBody.ConvertToRvector3();
      Rvector3 y = Vectors[face.VertexIndex[1]].WrtBody.ConvertToRvector3();
      Rvector3 z = Vectors[face.VertexIndex[2]].WrtBody.ConvertToRvector3();
      Rvector3 xy = y-x;
      Rvector3 yz = z-y;
      Rvector3 n = Cross(xy,yz);
      if (n.GetMagnitude() != 0)
         n = n.Normalize();
      int index = MakeVector (n,false);
      for (Integer j=0;  j<=3-1;  ++j)
         face.NormalIndex[j] = index;
      }
   }
//--------------------------------------------------------------------
void SurfaceMesh::Render (Structure* structure) const
// Draw the mesh in opengl
   {
   float white[4] = {255.0f, 255.0f, 255.0f, 1.0f};
   RgbColor color = structure->Color;
   ZMaterial* mat = structure->FindMaterial (TheMaterial);
   bool havetexture = false;
   if (mat != 0)
      {
      color = mat->Color;
      if (mat->GlTextureId != -1)
         {
         // As long as have a texture id, we bind it and enable textures
         GLint params = -13;
         glBindTexture(GL_TEXTURE_2D, mat->GlTextureId);
         glGetIntegerv(GL_TEXTURE_BINDING_2D, &params);
         glEnable(GL_TEXTURE_2D);
         glColor4fv(white);  // This is mixed with texture, best if all 255.0F 
         havetexture = true;
         }
      if (mat->Shininess > 0)
         {
         glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat->Shininess);
         } 
      }
   if (!havetexture)
      {
      // No texture, set color
      float rgba[4];
      rgba[0] = color.Red()/255.0f;
      rgba[1] = color.Green()/255.0f;
      rgba[2] = color.Blue()/255.0f;
      rgba[3] = 1;
      glDisable(GL_TEXTURE_2D);
      glColor4fv(rgba);
      }

   glBegin(GL_TRIANGLES);
   for (Integer j=0; j<=FacesSize-1; ++j)
      {
      ZFace& face (Faces[j]);
      glEnable(GL_NORMALIZE); // added (LOJ: 2012.07.11)
      for (Integer k=0;  k<=3-1;  ++k)
         {
         glNormal3fv(GetVectorWrt(face.NormalIndex[k],false).V);
         glTexCoord2fv(face.TextureCoord[k].V);
         glVertex3fv(GetVectorWrt(face.VertexIndex[k],false).V);
         }
      }
   glEnd();
   }
//--------------------------------------------------------------------
void SurfaceMesh::WriteSummary (std::ostream& s, const Integer level) const
// Write a breigf summary of the object
   {
   std::string indent = std::string (level*2,' ');
   s << indent << "SurfaceMesh, Face count = " << FacesSize << 
        ", Material='" << TheMaterial << "'" << std::endl;
   }
//====================================================================
