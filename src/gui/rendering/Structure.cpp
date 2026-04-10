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
#include "Structure.hpp"
#include "StringUtil.hpp"
#include "AttitudeConversionUtility.hpp"
#include "ColorTypes.hpp"
#include <fstream>
#undef max
//====================================================================
ZMaterial::ZMaterial (const std::string& name) 
// Constructor
   : Name (name),
     Color (255,255,0),
     Shininess (50),
     TextureMap (""),
     UScale (1),
     VScale (1),
     UOffset (0),
     VOffset (0),
     GlTextureId (-1) // Added for GMAT Rendering
   {
   }
//--------------------------------------------------------------------
ZMaterial::ZMaterial (const std::string& name, const Integer& r, const Integer& g, 
   const Integer& b, const Integer& shininess) 
// Constructor
   : Name (name),
     Color (r,g,b),
     Shininess (shininess),
     TextureMap (""),
     UScale (1),
     VScale (1),
     UOffset (0),
     VOffset (0),
     GlTextureId (-1)
   {
   }
//--------------------------------------------------------------------
ZMaterial::~ZMaterial ()
// Destructor
   {
   }
//--------------------------------------------------------------------
void ZMaterial::WriteSummary (std::ostream& s, const Integer level) const
// Write a breif summary of the object
   {
   std::string indent = std::string (level*2,' ');
   RgbColor& c (const_cast<RgbColor&>(Color));
   s << indent << "Material '" << Name << "', Color = (" << 
      RgbColor::ToRgbString (c.GetIntColor()) << ")" << std::endl;
   if (TextureMap != "")
      s << indent << "Material, Texture = " << TextureMap << std::endl;
   }
//====================================================================
ZMaterialArray::ZMaterialArray (const Integer size)
// Constructor.  In a perfect world, this would be an 
// std::vector<ZMaterial>
   : Max (100),
     TheSize (size),
     Data (new ZMaterial*[Max])
   {
   }
//--------------------------------------------------------------------
ZMaterialArray::~ZMaterialArray()
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
Integer ZMaterialArray::Size () const
// Returns the count of objects of an array
   {
   return TheSize;
   }
//--------------------------------------------------------------------
Integer ZMaterialArray::Add (ZMaterial* x)
// Adds a new surface to the array of surfaces, and returns the index
// of the new element
   {
   if (TheSize == Max)
      {
      ZMaterial** old = Data;
      Data = new ZMaterial*[Max*2];
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
const ZMaterial* ZMaterialArray::operator[] (const Integer& ix) const
// Returns an element of an array (const version for access)
   {
   if (ix < 0) return Data[0];
   if (ix >= TheSize) return Data[0];
   return Data[ix];
   }
//--------------------------------------------------------------------
ZMaterial* ZMaterialArray::operator[] (const Integer& ix) 
// Returns an element of an array (non-const version for assignment)
   {
   if (ix < 0) return Data[0];
   if (ix >= TheSize) return Data[0];
   return Data[ix];
   }
//====================================================================
Joint::Joint () 
// Constructor
   : BaseOffset (0,0,0),
     AppendageOffset (0,0,0)
   {
   Axis[0] = ZZ;
   Axis[1] = XX;
   Axis[2] = ZZ;
   for (Integer i=0;  i<=3-1;  ++i)
      {
      Use[i] = false;
      Vary[i] = true;
      Minimum[i] = -180;
      Maximum[i] = 180;
      DefaultDeg[i] = 0;
      CurrDeg[i] = 0;
      }
   }
//--------------------------------------------------------------------
Joint::~Joint ()
// Destructor
   {
   }
//--------------------------------------------------------------------
Real Joint::GetAngleDeg (const Integer& ix) const
// Return value of ith angle in degrees
   {
   if (ix < 0 || ix > 3-1) return 0;
   return CurrDeg[ix];
   }
//--------------------------------------------------------------------
Rmatrix33 Joint::GetMatrix () const
// Returns the rotation matrix for all the turns in a joint combined
   {
   //Rmatrix33 out = new Rmatrix33(true);
   Rmatrix33 out = Rmatrix33(true);
   for (Integer jx=0;  jx<=3-1;  ++jx)
      if (Use[jx])
         {
         Rmatrix33 qq = AttitudeConversionUtility::EulerAxisAndAngleToDCM
            (Axis[jx],GetAngleDeg(jx));
         out = qq*out;
         }
   return out;
   }
//--------------------------------------------------------------------
// Set all the values of one turn in the series of three
void Joint::SetTurn (const bool& inuse, const Integer& ix,
   const Integer& axis, const Real& angledeg)
   {
   if (ix < 0 || ix > 3-1) return;
   bool use = inuse;
   Use[ix] = use;
   switch (axis) {
      case 1:  Axis[ix] = XX;  break;
      case 2:  Axis[ix] = YY;  break;
      case 3:  Axis[ix] = ZZ;  break;
      default: break;
      }
   Vary[ix] = use;
   DefaultDeg[ix] = angledeg;
   SetAngleDeg (ix,angledeg);
   }
//--------------------------------------------------------------------
void Joint::SetToDefault ()
// Set all values in a joint to their default value
   {
   for (Integer i=0;  i<=3-1;  ++i)
      if (Use[i])
         CurrDeg[i] = DefaultDeg[i];
   }
//--------------------------------------------------------------------
void Joint::SetAngleDeg (const Integer& ix, const Real& angledeg)
// Set an angle to a specified value
   {
   if (ix < 0 || ix > 3-1) return;
      if (Use[ix])
         {
         CurrDeg[ix] = angledeg;
         }
   }
//--------------------------------------------------------------------
std::string Joint::AxisString (const Integer& ix) const
// Create a string showing the current state, for human consumption
   {
   if      (Axis[ix] == XX)
      return "1";
   else if (Axis[ix] == YY)
      return "2";
   else if (Axis[ix] == ZZ)
      return "3";
   else 
      return "?";
   }
//--------------------------------------------------------------------
void Joint::WriteSummary (std::ostream& s, const Integer level) const
// Write a breif summary of the object
   {
   std::string indent = std::string (level*2,' ');
   std::string st = "";
   for (Integer ix=0;  ix<=3-1;  ++ix)
      if (Use[ix])
         {
         if (st.length()!=0) st += "-";
         st += AxisString (ix);
         if (!Vary[ix]) st += "f";
         }
   s << indent << "Joint, Axes = " << st << std::endl;
   }
//--------------------------------------------------------------------
const Rvector3 Joint::XX (1,0,0);
const Rvector3 Joint::YY (0,1,0);
const Rvector3 Joint::ZZ (0,0,1);
//====================================================================
ZAppendage::ZAppendage () 
// Constructor
   : Color (0),
     Body ("Surface List"),
     TheJoint (),
     Base ("")
   {
   switch (BodyCount%10) {
      case 0:  Color = new RgbColor(GmatColor::AQUA);  break;
      case 1:  Color = new RgbColor(GmatColor::BEIGE);  break;
      case 2:  Color = new RgbColor(GmatColor::YELLOW);  break;
      case 3:  Color = new RgbColor(GmatColor::CORAL);  break;
      case 4:  Color = new RgbColor(GmatColor::LIME);  break;
      case 5:  Color = new RgbColor(GmatColor::CYAN);  break;
      case 6:  Color = new RgbColor(GmatColor::MAGENTA);  break;
      case 7:  Color = new RgbColor(GmatColor::ORCHID);  break;
      case 8:  Color = new RgbColor(GmatColor::SKY_BLUE);  break;
      case 9:  Color = new RgbColor(GmatColor::PINK);  break;
      }
   ++BodyCount;
   }
//--------------------------------------------------------------------
ZAppendage::~ZAppendage ()
// Destructor
   { 
   if (Color != 0) delete Color;
   Color = 0;
   }
//--------------------------------------------------------------------
Integer ZAppendage::GetSurfaceCount () const
// Return a count of surfaces elements in this appendage
   {
   return Body.Children.Size();
   }
//--------------------------------------------------------------------
SurfaceBase* ZAppendage::GetSurface (const Integer& ix) 
// Return a surface
   {
   return Body.Children[ix];
   }
//--------------------------------------------------------------------
void ZAppendage::WriteSummary (std::ostream& s, const Integer level) const
// Write a breif summary of the object
   {
   std::string indent = std::string (level*2,' ');
   s << indent << "Appendage, Base = " << Base;  
   if (Color != 0)
      s << ", Color = " << RgbColor::ToRgbString (Color->GetIntColor());
   s << std::endl;
   Body.WriteSummary (s,level+1);
   TheJoint.WriteSummary (s,level+1);
   }
//--------------------------------------------------------------------
Integer ZAppendage::BodyCount = 0;
//====================================================================
ZAppendageArray::ZAppendageArray (const Integer size)
// Constructor.  In a perfect world, this would be an 
// std::vector<ZAppendage>
   : Max (100),
     TheSize (size),
     Data (new ZAppendage*[Max])
   {
   }
//--------------------------------------------------------------------
ZAppendageArray::~ZAppendageArray()
// Destructor, being sure to delete all elements, and then the array of pointers
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
Integer ZAppendageArray::Size () const
// Returns the size of an array
   {
   return TheSize;
   }
//--------------------------------------------------------------------
Integer ZAppendageArray::Add (ZAppendage* x)
// Adds a new surface to the array of surfaces, and returns the index
// of the new element
   {
   if (TheSize == Max)
      {
      ZAppendage** old = Data;
      Data = new ZAppendage*[Max*2];
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
const ZAppendage* ZAppendageArray::operator[] (const Integer& ix) const
// Returns an element of an array (const version for access)
   {
   if (ix < 0) return Data[0];
   if (ix >= TheSize) return Data[0];
   return Data[ix];
   }
//--------------------------------------------------------------------
ZAppendage* ZAppendageArray::operator[] (const Integer& ix) 
// Returns an element of an array (non-const version for assignment)
   {
   if (ix < 0) return Data[0];
   if (ix >= TheSize) return Data[0];
   return Data[ix];
   }
//====================================================================
Structure::Structure (const std::string& name) 
// Constructor using name of future object
   : Name (name),
     Color (220,220,128),
     Appendages (0),
     Materials (0)
   {
   ZAppendage* appendage = new ZAppendage();
   appendage->Body.Name = "Bus";
   appendage->Base = "";
   for (Integer i=0;  i<=3-1;  ++i)
      appendage->TheJoint.Use[i] = false;
   AddAppendage (appendage);
   }
//--------------------------------------------------------------------
Structure::~Structure ()
// Destructor
   {
   }
//--------------------------------------------------------------------
void Structure::AddAppendage (ZAppendage* appendage)
// Add an appendage to the structure
   {
   Appendages.Add (appendage);
   }
//--------------------------------------------------------------------
void Structure::AddMaterial (ZMaterial* material)
// Add an material to the structure
   {
   Materials.Add(material);
   }
//--------------------------------------------------------------------
void Structure::SetJointsToDefault ()
// Return all rotation joints to their default values
   {
   for (Integer ix=0;  ix<=Appendages.Size()-1;  ++ix)
      Appendages[ix]->TheJoint.SetToDefault();
   CalcBodyRotations();
   }
//--------------------------------------------------------------------
ZMaterial* Structure::FindMaterial (const std::string& name) 
// Look up a materisl by name.  Retunr 0 (null poinyter) if not found
   {
   for (Integer im=0;  im<=Materials.Size()-1;  ++im)
      if (Materials[im]->Name == name)
         return Materials[im];
   return 0;
   }
//--------------------------------------------------------------------
ZAppendage* Structure::FindAppendage (const std::string& name) 
// Look up a appendage by name.  Retunr 0 (null poinyter) if not found
   {
   for (Integer ix=0;  ix<=Appendages.Size()-1;  ++ix)
       if (Appendages[ix]->Body.Name == name)
          return Appendages[ix];
   return 0;
   }
//--------------------------------------------------------------------
void Structure::CalcBodyRotations ()
// Implement all of the joint rotations to their current values
   {
   for (Integer ix=0;  ix<=Appendages.Size()-1;  ++ix)
      {
      SurfaceGroup& sg (Appendages[ix]->Body);
      ZAppendage* a = Appendages[ix];
      ZAppendage* base = FindAppendage (Appendages[ix]->Base);
      while (base != 0)
         {
         sg.RotateBody (a->TheJoint.GetMatrix(),
            a->TheJoint.BaseOffset,a->TheJoint.AppendageOffset);
         a = base;
         base = FindAppendage (a->Base);
         }
      }
   }
//--------------------------------------------------------------------
void Structure::CalcCenter ()
// Find the center of the object
   {
   ZMinMax minmax;
   for (Integer ix=0;  ix<=Appendages.Size()-1;  ++ix)
      minmax.Broaden(Appendages[ix]->Body.MinMax());
   for (int i=0;  i<=3-1;  ++i)
      Center[i] = (minmax.Min.V[i]+minmax.Max.V[i])/2;
   Rvector3 size;
   for (int i=0;  i<=3-1;  ++i)
      //size[i] = std::max(Center[i]-minmax.Min.V[i],minmax.Max.V[i]-Center[i]);
      size[i] = GmatMathUtil::Max(Center[i]-minmax.Min.V[i],minmax.Max.V[i]-Center[i]);
   Radius = size.GetMagnitude();
   }
//--------------------------------------------------------------------
void Structure::Write (std::string filename) const
// Write a breigf summary of the object to the specified file
   {
   std::ofstream s (filename.c_str());
   WriteSummary (s,0);
   s.close();
   }
//--------------------------------------------------------------------
void Structure::Render () 
// Draw the mesh in opengl
   {
   for (Integer ix=0;  ix<=Appendages.Size()-1;  ++ix)
      Appendages[ix]->Body.Render (this);
   }
//--------------------------------------------------------------------
void Structure::WriteSummary (std::ostream& s, const Integer level) const
// Write a breigf summary of the object
   {
   std::string indent = std::string (level*2,' ');
   RgbColor c = Color;
   s << indent << "Structure " + Name+ ", Color = (" << 
      RgbColor::ToRgbString (c.GetIntColor()) << ")" << std::endl;
   s << indent << "Appendage count = " << 
      GmatStringUtil::ToString (Appendages.Size(),true,4) << std::endl;
   for (Integer ix=0;  ix<=Appendages.Size()-1;  ++ix)
      Appendages[ix]->WriteSummary (s,level+1);
   s << indent << "Material count = " << 
      GmatStringUtil::ToString (Materials.Size(),true,4) << std::endl;
   for (Integer im=0;  im<=Materials.Size()-1;  ++im)
      Materials[im]->WriteSummary (s,level+1);
   s << indent << "End Structure" << std::endl;
   }
//====================================================================
