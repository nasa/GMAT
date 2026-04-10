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
// ** Legal **
//
// Author: Phillip Silvia, Jr.
// Created: 2009/06/24
//
// Modified: 2015/08/31:  John Downing -- Major rewrite to include new Structure.
/**
 * Object for storing model data loaded from a file. Also handles self-rendering
 */
//====================================================================
#ifndef MODEL_OBJECT_H
#define MODEL_OBJECT_H
#include <wx/string.h>
#include "gmatwxdefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector3.hpp"
#include "Structure.hpp"
//====================================================================
typedef struct
{
   float element[16];
} matrix_type, *matrix_type_ptr;
//====================================================================
// Color type, rgba format
typedef struct{
   float r;
   float g;
   float b;
   float a;
} rgba_type;
//====================================================================
class ModelObject
{
private:
   std::string name;      // The name of the object
   std::string filename;  // The name of the file this object came from
   Structure* TheStructure;
   bool isLoaded;
   matrix_type matrix;      // Object matrix
   std::string filePath;

   float StructureOffset[3];
   float StructureScale;
   float BodyPosition[3];
   float BodyRotation[3];
   float Attitude[3];
   float Scale;
   
public:
   ModelObject();
private: // Copy Protected
   ModelObject(const ModelObject& mo);
   ModelObject& operator=(const ModelObject& mo);
public:
   ~ModelObject();
   
   // Load the model information from a .3ds file
   char Load (const std::string &full_path);
   void LoadTextures();
   
   // Accessors for the private data
   void SetName(const std::string &nameFromFile);
   bool IsLoaded();
   void SetBodyPosition (float x, float y, float z);
   void SetRotation (bool use_degrees, float angle_x, float angle_y, float angle_z);
   void SetAttitude (bool use_degrees, float angle_x, float angle_y, float angle_z);
   void SetScale (float x);
   void Reset ();
   
   void DrawAsSpacecraft (bool isLit);
   void DrawAsCelestialBody (bool isLit);
   void RenderShadowVolume (float *light_pos, int frame);
   
private:
   void Draw (bool isLit, bool ispacecraft);
   void SetMatrix (bool ispacecraft);
   
   int  LoadTexture(const std::string &file_name);

   /// Helper functions for the matrix structure used within the class
   /// The structures are used instead of Rmatrix because Rmatrix is finicky and this was faster
   void  MatrixSetElement(matrix_type_ptr matrix, int r, int c, float value);
   float MatrixGetElement(matrix_type_ptr matrix, int r, int c);
   void  MatrixIdentity(matrix_type_ptr matrix);
   void  MatrixRotation (matrix_type_ptr matrix, float angle[3]);
   void  MatrixZero(matrix_type_ptr matrix);
   void  MatrixCopy(matrix_type_ptr source, matrix_type_ptr destination);
   void  MatrixMult(matrix_type_ptr matrix1, matrix_type_ptr matrix2, matrix_type_ptr result);
};
//====================================================================
#endif
