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
 * Structure Class Family, these classes implement the structure
 * reader class for 3ds files.
 */
//====================================================================
#include <sstream>
#include "StructureReader3ds.hpp"
#include "StringUtil.hpp"
#include "FileUtil.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"
//====================================================================
ZIntegerVector::ZIntegerVector ()
// Constructor
   {
   V[0] = 0;
   V[1] = 0;
   V[2] = 0;
   }
//--------------------------------------------------------------------
ZIntegerVector::ZIntegerVector (const Integer& x, const Integer& y, 
   const Integer& z)
// Constructor
   {
   V[0] = x;
   V[1] = y;
   V[2] = z;
   }
//====================================================================
ZMaterialData::ZMaterialData ()
// Constructor
   : Name(""),
     FaceIndexesSize(0),
     FaceIndexes(NULL)
   {
   }
//--------------------------------------------------------------------
ZMaterialData::~ZMaterialData()
// Destructor
   {
   if (FaceIndexes != 0)
      delete[] FaceIndexes;
   FaceIndexesSize = 0;
   FaceIndexes = 0;
   }
//====================================================================
ChunkReport::ChunkReport ()
// Constructor
   : Id (0),
     Length (0),
     Good (false)
   {
   }
//====================================================================
// StructureReader3ds (const std::string& filename)
//--------------------------------------------------------------------
/**
 * Constructor
 */
//--------------------------------------------------------------------
StructureReader3ds::StructureReader3ds (const std::string& filename)
// Constructor
   : StructureReader (filename),
     Filename (filename),
     S (NULL),
     Writer(NULL),
     Dir (""),
// Utility group
     Ob001x_Color (NULL),
     Ob003x_Percentage (NULL),
// 4100 group for processig meshes
     Ob4000_Name (NULL),
     Ob4110_VectorsSize (0),
     Ob4110_Vectors (NULL),
     Ob4111_FlagsSize (0), 
     Ob4111_Flags (NULL), // Array
     Ob4120_FacesSize (0), 
     Ob4120_Faces (NULL),   // 1,2,3,flags   [N][3]
     Ob4130_MaterialSize (0), 
     Ob4140_TextureMapSize (0), 
     Ob4140_TextureMap (NULL),  // [N][2]
// A000 group for processing meshes
     ObA000_MatName (NULL),
     ObA010_Ambient (NULL),
     ObA020_Diffuse (NULL),
     ObA030_Specular (NULL),
     ObA040_Shininess (NULL),
     ObA041_Shin2pct (NULL),
     ObA042_Shin3pct (NULL),
     ObA050_Transparency (NULL),
     ObA052_XPFall (NULL),
     ObA053_RefBlur (NULL),
     ObA087_WireSize (NULL),
     ObA100_Shading (NULL),
     ObA300_MapName (NULL),
     ObA354_UScale (NULL),
     ObA356_VScale (NULL),
     ObA358_UOffset (NULL),
     ObA35A_VOffset (NULL)
   {
   }
//--------------------------------------------------------------------
StructureReader3ds::~StructureReader3ds()
// Destructor
   {
   void ClearUtility ();
   void Clear4100 ();
   void ClearA000 ();
   }
//--------------------------------------------------------------------
// Clear the "utility" arrays
void StructureReader3ds::ClearUtility ()
   {
   Clear (Ob001x_Color);
   Clear (Ob003x_Percentage);
   }
//--------------------------------------------------------------------
void StructureReader3ds::Clear4100 ()
// Clear the "4100" arrays
   {
   Clear (Ob001x_Color);
   Clear (Ob003x_Percentage);
   Clear (Ob4000_Name);
   ClearVectors (Ob4110_Vectors,Ob4110_VectorsSize);
   ClearVectors (Ob4111_Flags,Ob4111_FlagsSize);
   ClearVectors (Ob4120_Faces,Ob4120_FacesSize);
   Integer size = Ob4130_MaterialSize;
   for (Integer i=0;  i<=size-1;  ++i)
      delete[] Ob4130_Material[i]->FaceIndexes;
   Ob4130_MaterialSize = 0;
   ClearVectors (Ob4140_TextureMap,Ob4140_TextureMapSize);
   }
//--------------------------------------------------------------------
void StructureReader3ds::ClearA000 ()
// Clear the "A000" arrays
   {
   Clear (ObA000_MatName);
   Clear (ObA010_Ambient);
   Clear (ObA020_Diffuse);
   Clear (ObA030_Specular);
   Clear (ObA040_Shininess);
   Clear (ObA041_Shin2pct);
   Clear (ObA042_Shin3pct);
   Clear (ObA050_Transparency);
   Clear (ObA052_XPFall);
   Clear (ObA053_RefBlur);
   Clear (ObA087_WireSize);
   Clear (ObA100_Shading);
   Clear (ObA300_MapName);
   Clear (ObA354_UScale);
   Clear (ObA356_VScale);
   Clear (ObA358_UOffset);
   Clear (ObA35A_VOffset);
   }
//--------------------------------------------------------------------
void StructureReader3ds::Clear (Integer*& x)
// Clear an integer 
   {
   if (x != NULL)
      delete x;
   x = NULL;
   }
//--------------------------------------------------------------------
void StructureReader3ds::Clear (Real*& x)
// Clear an Real 
   {
   if (x != NULL)
      delete x;
   x = NULL;
   }
//--------------------------------------------------------------------
void StructureReader3ds::Clear (RgbColor*& x)
// Clear a RgbColor 
   {
   if (x != NULL)
      delete x;
   x = NULL;
   }
//--------------------------------------------------------------------
void StructureReader3ds::Clear (Rvector3*& x)
// Clear a Rvector3 
   {
   if (x != NULL)
      delete x;
   x = NULL;
   }
//--------------------------------------------------------------------
void StructureReader3ds::Clear (std::string*& x)
// Clear a string 
   {
   if (x != NULL)
      delete x;
   x = NULL;
   }
//--------------------------------------------------------------------
void StructureReader3ds::ClearVectors (Integer*& x, Integer& size)
// Clear a variable length interger array
   {
   if (x != NULL)
      delete[] x;
   x = NULL;
   size = 0;
   }
//--------------------------------------------------------------------
void StructureReader3ds::ClearVectors (Rvector3*& x, Integer& size)
// Clear a variable length Rvector3 array
   {
   if (x != NULL)
      delete[] x;
   x = NULL;
   size = 0;
   }
//--------------------------------------------------------------------
void StructureReader3ds::ClearVectors (ZIntegerVector*& x, Integer& size)
// Clear a variable length ZIntegerVector array
   {
   if (x != NULL)
      delete[] x;
   x = NULL;
   size = 0;
   }
//--------------------------------------------------------------------
void StructureReader3ds::ClearVectors (ZRealPoint*& x, Integer& size)
// Clear a variable length ZRealPoint array
   {
   if (x != NULL)
      delete[] x;
   x = NULL;
   size = 0;
   }
//--------------------------------------------------------------------
void StructureReader3ds::ReadChunk (const Integer& level)
// Reads a chunk from the file, and stores the data according to the 
// chunk id.  Then subchunks are read by calling this routine recursively.
// The "level" parameter is used to track the dpeth of the call, mainly
// for ensuring the the debug printout (via *Writer) is properly indented.
   {
   long startpos = ftell(S);
   int id = ReadLeShort();
   int length = ReadLeInt();
   int diffpos = 0;
   std::string prefix = "";
   for (int i=2;  i<=level;  ++i)
      prefix += "  ";
   if (Writer != 0)
      (*Writer) << prefix << "Id=" << std::hex << id << std::dec << " L=" << length << std::endl;
   
   switch (id) {
      case 0x4d4d:
         {
         }
         break;
      case 0x0002:
         {
         int version = ReadLeInt();
         if (Writer != NULL)
            (*Writer) << prefix << "Version= " << version << std::endl;
         }
         break;
      case 0x0010: // COLOR_F
         {
         int r = (int)ReadLeFloat()*255;
         int g = (int)ReadLeFloat()*255;
         int b = (int)ReadLeFloat()*255;
         Ob001x_Color = new RgbColor (r,g,b);
         }
         break;
      case 0x0011: // COLOR_24
         {
         int r = ReadUnsignedByte();
         int g = ReadUnsignedByte();
         int b = ReadUnsignedByte();
         Ob001x_Color = new RgbColor (r,g,b);
         }
         break;
      case 0x0012: // LIN_COLOR_24
         {
         int r = ReadUnsignedByte();
         int g = ReadUnsignedByte();
         int b = ReadUnsignedByte();
         Ob001x_Color = new RgbColor (r,g,b);
         }
         break;
      case 0x0013: // LIN_COLOR_F
         {
         int r = (int)ReadLeFloat()*255;
         int g = (int)ReadLeFloat()*255;
         int b = (int)ReadLeFloat()*255;
         Ob001x_Color = new RgbColor (r*255,g,b);
         }
         break;
      case 0x0030: // INT_PERCENTAGE
         {
         Ob003x_Percentage = new Real (ReadLeShort());
         }
         break;
      case 0x0031: // INT_PERCENTAGE
         {
         Ob003x_Percentage = new Real (ReadLeFloat());
         }
         break;
      case 0x0100: // MASTER_SCALE
         {
         double scale = ReadLeFloat();
         if (Writer != NULL)
            (*Writer) << prefix << "Scale= " << scale << std::endl;
         }
         break;
      case 0x1100: // BIT_MAP
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "BIT_MAP Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0x1200: // SOLID_BGND
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) <<prefix << "Solid Background Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0x1201: // USE_SOLID_BGND 
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "USE_SOLID_BGND Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0x1300: // V_GRADIENT
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "V_GRADIENT Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0x1400: // LO_SHADOW_BIAS
      case 0x1420: // SHADOW_MAP_SIZE
      case 0x1450: // ???
      case 0x1460: // RAY_BIAS
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "Shadow ? Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0x1500: // O_CONSTS
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "O_CONSTS Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0x2100: // AMBIENT_LIGHT
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "AMBIENT_LIGHT Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0x2200: // FOG
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "FOG Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0x2300: // DISTANCE_CUE
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "DISTANCE_CUE Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0x2302: // LAYER_FOG
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "LAYER_FOG Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0x3000: // DEFAULT_VIEW
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "DEFAULT_VIEW Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0x3d3d:
         {
         }
         break;
      case 0x3d3e:
         {
         int version = ReadLeInt();
         if (Writer != NULL)
            (*Writer) << prefix << "Mesh Version= " << version << std::endl;
         }
         break;
      case 0x4000: // NAMED_OBJECT
         {
         Ob4000_Name = new std::string (ReadCstr()); 
         if (Writer != NULL)
            (*Writer) << prefix << "Object name= " << *Ob4000_Name << std::endl;
         diffpos = ReadChunks(level,startpos,length);

         }
         break;
      case 0x4011: // OBJ_VIS_LOFTER
         {
         // Looks like an flag; no data.
         }
         break;
      case 0x4100: // NAMED_TRIANGLE_OBJECT
         {
             diffpos = ReadChunks(level,startpos,length);
             // There's something odd about casting integers to real, using a real function
             // and casting the function back to an integer to assign the result. There should be an overloaded
             // max function for integers in RealUtilities
             Integer count = (Integer)GmatMathUtil::Max (1.0,(Real)Ob4130_MaterialSize);
             for (int i=0;  i<=count-1;  ++i)
                {
                std::string meshname = *Ob4000_Name;
                if (Ob4130_MaterialSize > 0) meshname += "_" + 
                   GmatStringUtil::ToString(i+1,4);
                for (Integer ii=1;  ii<=meshname.length()-1;  ++ii)
                   if (meshname[ii] == ' ')
                      meshname[ii] = '0';

                SurfaceMesh* mesh = BuildMesh (meshname,i);                
                mesh->BuildNormals();
                if (Ob4130_MaterialSize > 0)
                   mesh->TheMaterial = Ob4130_Material[i]->Name;
                TheStructure->Appendages[0]->Body.AddChild(mesh);
                }

             Clear4100();
         }
         break;
      case 0x4110: // POINT_ARRAY
         {
         int count = ReadLeShort();
         if (Writer != NULL)
            (*Writer) << prefix << "Point Count= " << count << std::endl;
         Ob4110_VectorsSize = count;
         Ob4110_Vectors = new Rvector3[Ob4110_VectorsSize];
         for (int i=0;  i<=count-1;  ++i)
            {
            double x = ReadLeFloat();
            double y = ReadLeFloat();
            double z = ReadLeFloat();
            Ob4110_Vectors[i] = Rvector3 (x,y,z);
            }
         }
         break;
      case 0x4111: // POINT_FLAG_ARRAY
         {
         int count = ReadLeShort();
         if (Writer != NULL)
            (*Writer) << prefix << "Flag Count= " << count << std::endl;
         Ob4111_FlagsSize = count;
         Ob4111_Flags = new Integer[count];
         for (int i=0;  i<=count-1;  ++i)
            Ob4111_Flags[i] = ReadLeShort();
         }
         break;
      case 0x4120: // FACE_ARRAY
         {
         Ob4120_FacesSize = ReadLeShort();
         Ob4120_Faces = new ZIntegerVector[Ob4120_FacesSize];
         if (Writer != NULL)
            (*Writer) << prefix << "Face Count= " << Ob4120_FacesSize << std::endl;
         for (int i=0;  i<=Ob4120_FacesSize-1;  ++i)
            {
            Ob4120_Faces[i].V[0] = ReadLeShort();
            Ob4120_Faces[i].V[1] = ReadLeShort();
            Ob4120_Faces[i].V[2] = ReadLeShort();
            int flags = ReadLeShort(); // Flags
            }
         }
         break;
      case 0x4130: // MSH_MAT_GROUP
         {
         ZMaterialData* materialdata = new ZMaterialData();
         materialdata->Name = std::string (ReadCstr());   
         if (Writer != NULL)
            (*Writer) << prefix << "Material= " << materialdata->Name << std::endl;
         Integer count = ReadLeShort();
         if (Writer != NULL)
            (*Writer) << prefix << "Mat Group Count= " << count << std::endl;
         materialdata->FaceIndexesSize = count;
         materialdata->FaceIndexes = new Integer[materialdata->FaceIndexesSize];
         for (int i=0;  i<=materialdata->FaceIndexesSize-1;  ++i)
            materialdata->FaceIndexes[i] = ReadLeShort();
         Ob4130_Material[Ob4130_MaterialSize] = materialdata;
         ++Ob4130_MaterialSize;
         }
         break;
      case 0x4140: // TEX_VERTS
         {
         Integer count = ReadLeShort();
         if (Writer != NULL)
            (*Writer) << prefix << "Texture Coord Count= " << count << std::endl;
         Ob4140_TextureMapSize = count;
         Ob4140_TextureMap = new ZRealPoint[Ob4140_TextureMapSize];
         for (int i=0;  i<=count-1;  ++i)
            {
            Ob4140_TextureMap[i].V[0] = ReadLeFloat();
            Ob4140_TextureMap[i].V[1] = ReadLeFloat();
            }
         }
         break;
      case 0x4150: // SMOOTH_GROUP
         {
         int count = (length-6)/4;
         if (Writer != NULL)
            (*Writer) << prefix << "Smooth Face Count= " << count << std::endl;
         for (int i=0;  i<=count-1;  ++i)
            {
            int s1 = ReadLeShort();
            int s2 = ReadLeShort();
            }
         }
         break;
      case 0x4160: // MESH_MATRIX
         {
         float matrix[4][3];
         for (int i=0;  i<=4-1;  ++i)
            for (int j=0;  j<=3-1;  ++j)
               matrix[i][j] = ReadLeFloat();
         }
         break;
      case 0x4165: // MESH_COLOR
         {
         unsigned char b = ReadUnsignedByte();
         }
         break;
      case 0x4170: // MESH_TEXTURE_INFO
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "MESH_TEXTURE_INFO Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0x4600: // N_DIRECT_LIGHT
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "N_DIRECT_LIGHT Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0x4700: // N_CAMERA
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "Camera Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0x7001: // VIEWPORT_LAYOUT
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "Viewport Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0xAFFF: // MAT_ENTRY
         {
         diffpos = ReadChunks(level,startpos,length);
         ZMaterial* material = new ZMaterial (*ObA000_MatName);
         Clear (ObA000_MatName);
         if (ObA020_Diffuse != NULL)
            material->Color = *ObA020_Diffuse;  
         Clear (ObA010_Ambient);
         Clear (ObA020_Diffuse);
         Clear (ObA030_Specular);
         if (ObA040_Shininess != NULL)
            material->Shininess = (Integer)*ObA040_Shininess;
         Clear (ObA040_Shininess);
         Clear (ObA041_Shin2pct);
         Clear (ObA042_Shin3pct);
         Clear (ObA050_Transparency);
         Clear (ObA052_XPFall);
         Clear (ObA053_RefBlur);
         Clear (ObA087_WireSize);
         Clear (ObA100_Shading);
         if (ObA300_MapName != NULL)
            material->TextureMap = Dir + *ObA300_MapName;
         Clear (ObA300_MapName);
         if (ObA354_UScale != NULL)
            material->UScale = *ObA354_UScale;
         Clear (ObA354_UScale);
         if (ObA356_VScale != NULL)
            material->VScale = *ObA356_VScale;
         Clear (ObA356_VScale);
         if (ObA358_UOffset != NULL)
            material->UOffset = *ObA358_UOffset;
         Clear (ObA358_UOffset);
         if (ObA35A_VOffset != NULL)
            material->VOffset = *ObA35A_VOffset;
         Clear (ObA35A_VOffset);
         TheStructure->AddMaterial (material);
         }
         break;
      case 0xA000: // MAT_NAME
         {
         ObA000_MatName = new std::string (ReadCstr ());
         if (Writer != NULL)
            (*Writer) << prefix << "Material Name= " << *ObA000_MatName << std::endl;
         }
         break;
      case 0xA010: // MAT_AMBIENT
         {
         diffpos = ReadChunks(level,startpos,length);
         ObA010_Ambient = Ob001x_Color;
         Ob001x_Color = 0;
         }
         break;
      case 0xA020: // MAT_DIFFUSE
         {
         diffpos = ReadChunks(level,startpos,length);
         ObA020_Diffuse = Ob001x_Color;
         Ob001x_Color = 0;
         }
         break;
      case 0xA030: // MAT_SPECULAR
         {
         diffpos = ReadChunks(level,startpos,length);
         ObA030_Specular = Ob001x_Color;
         Ob001x_Color = 0;
         }
         break;
      case 0xA040: // MAT_SHININESS
         {
         diffpos = ReadChunks(level,startpos,length);
         ObA040_Shininess = Ob003x_Percentage;
         Ob003x_Percentage = 0;
         }
         break;
      case 0xA041: // MAT_SHIN2PCT
         {
         diffpos = ReadChunks(level,startpos,length);
         ObA041_Shin2pct = Ob003x_Percentage;
         Ob003x_Percentage = 0;
         }
         break;
      case 0xA042: // MAT_SHIN3PCT
         {
         diffpos = ReadChunks(level,startpos,length);
         ObA042_Shin3pct = Ob003x_Percentage;
         Ob003x_Percentage = 0;
         }
         break;
      case 0xA050: // MAT_TRANSPARENCY
         {
         diffpos = ReadChunks(level,startpos,length);
         ObA050_Transparency = Ob003x_Percentage;
         Ob003x_Percentage = 0;
         }
         break;
      case 0xA052: // MAT_XPFALL
         {
         diffpos = ReadChunks(level,startpos,length);
         ObA052_XPFall = Ob003x_Percentage;
         Ob003x_Percentage = 0;
         }
         break;
      case 0xA053: // MAT_REFBLUR
         {
         diffpos = ReadChunks(level,startpos,length);
         ObA053_RefBlur = Ob003x_Percentage;
         Ob003x_Percentage = 0;
         }
         break;
      case 0xA081: // MAT_TWO_SIDE
         {
         // Looks like an flag; no data.
         }
         break;
      case 0xA083: // MAT_ADDITIVE
         {
         // Looks like an flag; no data.
         }
         break;
      case 0xA084: // MAT_SELF_ILPCT
         {
         diffpos = ReadChunks(level,startpos,length);
         }
         break;
      case 0xA087: // MAT_WIRESIZE
         {
         double wiresize = ReadLeFloat();
         }
         break;
      case 0xA08A: // MAT_XPFALLIN
         {
         // Looks like an flag; no data.
         }
         break;
      case 0xA08C: // MAT_PHONGSOFT
         {
         // Looks like an autoflag; no data.
         }
         break;
      case 0xA08E: // MAT_WIREABS
         {
         // Looks like an autoflag; no data.
         }
         break;
      case 0xA100: // MAT_SHADING
         {
         ObA100_Shading = new Real((double)ReadLeShort());
         }
         break;
      case 0xA200: // MAT_TEXMAP
         {
         diffpos = ReadChunks(level,startpos,length);
         }
         break;
      case 0xA220: // MAT_REFLMAP
         {
         // This one overwrite texture map with reflection map.
         // Save and replace
         std::string* hold = ObA300_MapName;
         diffpos = ReadChunks(level,startpos,length);
         ObA300_MapName = hold;
         }
         break;
      case 0xA230: // MAT_BUMPMAP
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "MAT_BUMPMAP Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0xA250: // MAT_USE_REFBLUR
         {
         // Looks like an autoflag; no data.
         }
         break;
      case 0xA300: // MAT_MAPNAME
         {
         ObA300_MapName = new std::string (ReadCstr ());
         if (Writer != NULL)
            (*Writer) << prefix << "Map Name= " << *ObA300_MapName << std::endl;
         }
         break;
      case 0xA33A: // MAT_TEX2MAP
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "MAT_TEX2MAP Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0xA33C: // MAT_SHINMAP
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "MAT_SHINMAP Chunk -- Unprocessed" << std::endl;
         }
         break;
      case 0xA351: // MAT_MAP_TILING
         {
         int flags = ReadLeShort();
         }
         break;
      case 0xA353: // MAT_MAP_TEXBLUR
         {
         float x = ReadLeFloat();
         }
         break;
      case 0xA354: // MAT_MAP_USCALE
         {
         ObA354_UScale = new Real (ReadLeFloat());
         if (Writer != NULL)
            (*Writer) << prefix + "Map UScale= " << *ObA354_UScale << std::endl;
         }
         break;
      case 0xA356: // MAT_MAP_VSCALE
         {
         ObA356_VScale = new Real (ReadLeFloat());
         if (Writer != NULL)
            (*Writer) << prefix << "Map VScale= " << *ObA356_VScale << std::endl;
         }
         break;
      case 0xA358: // MAT_MAP_UOFFSET
         {
         ObA358_UOffset = new Real (ReadLeFloat());
         if (Writer != NULL)
            (*Writer) << prefix << "Map UOffset= " << *ObA358_UOffset << std::endl;
         }
         break;
      case 0xA35A: // MAT_MAP_VOFFSET
         {
         ObA35A_VOffset = new Real (ReadLeFloat());
         if (Writer != NULL)
            (*Writer) << prefix << "Map VOffset= " << *ObA35A_VOffset << std::endl;
         }
         break;
      case 0xA35C: // MAT_MAP_ANGLE
         {
         ReadLeFloat();
         }
         break;
      case 0xB000: // KFDATA
         {
         fseek (S,startpos+length,SEEK_SET);
         if (Writer != NULL)
            (*Writer) << prefix << "Lighting Chunk -- Unprocessed" << std::endl;
         }
         break;
      default:
         {
         fseek (S,startpos+length,SEEK_SET);
         std::stringstream s;
         s << std::hex << id;
         std::string idstr = s.str();
         MessageInterface::ShowMessage
            ("3DS ERROR!! -- Unprocessed chunk " + idstr + "\n");
         if (Writer != NULL)
            (*Writer) << prefix << "ERROR!! -- Unprocessed chunk" << std::endl;
         }
         break;
      }

   diffpos = ReadChunks(level,startpos,length);
   if (diffpos != length)
      if (Writer != NULL)
         (*Writer) << prefix << "ERROR!! Id=" << std::hex << id <<
                     " L=" << length << " Diff=" << diffpos << std::endl;
   }
//--------------------------------------------------------------------
Integer StructureReader3ds::ReadChunks (const Integer& level, const long& startpos, 
   const Integer& length)
// Reads the rest of the subchunks until the alloted length is reached.  If
// the subchunks do not match the length, this rader will probably fail
//  miserably
   {
   long endpos = ftell(S);
   int diffpos = (int)(endpos-startpos);
   while (diffpos < length) 
      {
      ReadChunk(level+1);
      endpos = ftell(S);
      diffpos = (int)(endpos-startpos);
      }
   return diffpos;
   }
//--------------------------------------------------------------------
SurfaceMesh* StructureReader3ds::BuildMesh 
   (const std::string name, const Integer& ix)
// Builds the surface mesh data type out of the 4100 seriies data.
   {
   // This is done for each material
   Integer vcount = 0;
   Integer* v_used = new Integer[Ob4110_VectorsSize];
   for (Integer i=0;  i<=Ob4110_VectorsSize-1;  ++i)
      v_used[i] = -1;
   
   Integer fcount = 0;
   if (Ob4130_MaterialSize == 0)
      fcount = Ob4120_FacesSize;
   else
      fcount = Ob4130_Material[ix]->FaceIndexesSize;

   SurfaceMesh* out = new SurfaceMesh (name,0,fcount);
   
   for (Integer i=0;  i<=fcount-1;  ++i)
      {
      out->Faces[i] = ZFace();
      Integer findex = 0;
      if (Ob4130_MaterialSize == 0)
         findex = i;
      else
         findex = Ob4130_Material[ix]->FaceIndexes[i];

      for (Integer j=0;  j<=3-1;  ++j)
         {
         Integer k = Ob4120_Faces[findex].V[j];
         if (v_used[k] == -1)
            {
            v_used[k] = vcount;
            ++vcount;
            }
         out->Faces[i].VertexIndex[j] = v_used[k];
         if (Ob4140_TextureMapSize != 0)
            {
            if (k <= Ob4140_TextureMapSize-1)
               {
               double x = Ob4140_TextureMap[k].V[0];
               double y = Ob4140_TextureMap[k].V[1];
               out->Faces[i].TextureCoord[j] = ZRealPoint (x,-y);
               }
            }
         }
      }
      
   for (Integer i=0;  i<=vcount-1;  ++i)
      {
      int size = out->Vectors.Size();
      out->MakeVector (Rvector3(),false);
      }
   for (Integer i=0;  i<=Ob4110_VectorsSize-1;  ++i)
      if (v_used[i] > -1)
         {
         out->Vectors[v_used[i]] = ZVectorCache(Ob4110_Vectors[i],true);
         }

   delete [] v_used;

   return out;
   }
//--------------------------------------------------------------------
Integer StructureReader3ds::ReadUnsignedByte ()
// Reads an unsigned byte
   {
   Integer out = 0;
   fread(&out,sizeof(unsigned char),1,S);
   return out;
   }
//--------------------------------------------------------------------
Integer StructureReader3ds::ReadLeInt ()
// Reads an integer
   {
   Integer out = 0;
   fread(&out,sizeof(Integer),1,S);
   return out;
   }
//--------------------------------------------------------------------
Integer StructureReader3ds::ReadLeShort ()
// Reads an short as integer
   {
   short sh = 0;
   fread(&sh,sizeof(short),1,S);
   int out = sh;
   out = out & 0x0000FFFF;
   return out;
   }
//--------------------------------------------------------------------
float StructureReader3ds::ReadLeFloat ()
// Reads a float
   {
   float out = 0;
   fread(&out,sizeof(float),1,S);
   return out;
   }
//--------------------------------------------------------------------
std::string StructureReader3ds::ReadCstr ()
// REads a Pascal formatted string
   {
   char out[100];
   int count = 0;
   do {
      fread(&out[count],sizeof(char),1,S);
      ++count;
      } while (out[count-1] != 0);
   return std::string (out);   
   }
//--------------------------------------------------------------------
void StructureReader3ds::Execute ()
// Does the actual reading of the file and filling TheStructure with useful data
   {
   S = fopen(Filename.c_str(),"rb");
   if (S != 0)
      {
      Dir = GmatFileUtil::ParsePathName(Filename,true);
      std::string name = GmatFileUtil::ParseFileName(Filename,true);
//      Writer = new std::ofstream (Dir + name + ".3dsout");
      TheStructure = new Structure(name);
      TheStructure->Appendages[0]->BodyCount = 0;
      ReadChunk (1);
      if (Writer != NULL)
         (*Writer) << "3ds load complete" << std::endl;
      if (Writer != NULL)
         Writer->close();
      delete Writer;
      TheStructure->CalcCenter();
      fclose(S);
      }
   else
      {
      MessageInterface::ShowMessage ("Unable to open " + Filename);
      }
   }
//====================================================================
