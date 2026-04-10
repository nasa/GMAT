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
#ifndef StructureReader3ds_hpp
#define StructureReader3ds_hpp
#include "StructureReader.hpp"
#include <fstream>
//====================================================================
struct ZIntegerVector 
{
public:
// --------------------------- Constructors, Assignment, Destructors
   ZIntegerVector ();
   ZIntegerVector (const Integer& x, const Integer& y, 
      const Integer& z);
   // Default Copy Constructor OK
   // Default Assignment Operator OK
   // Default Destructor OK
// --------------------------- Data Members
public:
   Integer V[3];
// --------------------------- 
};
//====================================================================
struct ZMaterialData 
{
public:
// --------------------------- Constructors, Assignment, Destructors
   ZMaterialData ();
private: // Copy Protected
   ZMaterialData (const ZMaterialData&);
   ZMaterialData& operator= (const ZMaterialData&);
public:
   ~ZMaterialData();
// --------------------------- Data Members
public:
   std::string Name;
   Integer     FaceIndexesSize;
   Integer*    FaceIndexes;
// --------------------------- 
};
//====================================================================
struct ChunkReport 
{
public:
// --------------------------- Constructors, Assignment, Destructors
   ChunkReport ();
   // Default Copy Constructor OK
   // Default Assignment Operator OK
   // Default Destructor OK
// --------------------------- Data Members
public:
   Integer Id;
   Integer Length;
   bool    Good;
// --------------------------- 
};
//====================================================================
class StructureReader3ds : public StructureReader
{
public:
// --------------------------- Constructors, Assignment, Destructors
   StructureReader3ds (const std::string& filename);
   // Inherit copy protection
   virtual ~StructureReader3ds();
// --------------------------- Functions
private:
   void ClearUtility ();
   void Clear4100 ();
   void ClearA000 ();
   void Clear (Integer*& x);
   void Clear (Real*& x);
   void Clear (RgbColor*& x);
   void Clear (Rvector3*& x);
   void ClearVectors (Integer*& x, Integer& size);
   void ClearVectors (Rvector3*& x, Integer& size);
   void ClearVectors (ZIntegerVector*& x, Integer& size);
   void ClearVectors (ZRealPoint*& x, Integer& size);
   void Clear (std::string*& x);
   void ReadChunk (const Integer& level);
   Integer ReadChunks (const Integer& level, const long& startpos, 
      const Integer& length);
   SurfaceMesh* BuildMesh (const std::string name, const Integer& ix);
   Integer ReadUnsignedByte ();
   Integer ReadLeInt ();
   Integer ReadLeShort ();
   float ReadLeFloat ();
   std::string ReadCstr ();
public:
   virtual void Execute ();
// --------------------------- Data Members
private:
   std::string    Filename;
   FILE*          S;
   std::ofstream* Writer;
   std::string    Dir;

// Utility group
   RgbColor*      Ob001x_Color;
   Real*          Ob003x_Percentage;

// 4100 group for processig meshes
   std::string*    Ob4000_Name;
   Integer         Ob4110_VectorsSize;
   Rvector3*       Ob4110_Vectors;
   Integer         Ob4111_FlagsSize;
   Integer*        Ob4111_Flags; 
   Integer         Ob4120_FacesSize;
   ZIntegerVector* Ob4120_Faces;  
   Integer         Ob4130_MaterialSize;
   ZMaterialData*  Ob4130_Material[100];
   Integer         Ob4140_TextureMapSize;
   ZRealPoint*     Ob4140_TextureMap;  // [N][2]

// A000 group for processing meshes
   std::string*  ObA000_MatName;
   RgbColor*     ObA010_Ambient;
   RgbColor*     ObA020_Diffuse;
   RgbColor*     ObA030_Specular;
   Real*         ObA040_Shininess;
   Real*         ObA041_Shin2pct;
   Real*         ObA042_Shin3pct;
   Real*         ObA050_Transparency;
   Real*         ObA052_XPFall;
   Real*         ObA053_RefBlur;
   Real*         ObA087_WireSize;
   Real*         ObA100_Shading;
   std::string*  ObA300_MapName;
   Real*         ObA354_UScale;
   Real*         ObA356_VScale;
   Real*         ObA358_UOffset;
   Real*         ObA35A_VOffset;
// --------------------------- 
};
//====================================================================
#endif 
