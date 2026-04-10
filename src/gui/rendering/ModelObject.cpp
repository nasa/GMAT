// $Id$
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
// Developed jointly by NASA/GSFC, Thinking Systems, Inc., and Schafer Corp.,
// under AFRL NOVA Contract #FA945104D03990003
//
// Author: Phillip Silvia, Jr.
// Created: 2009/06/24
// Modified: Dunning Idle 5th
// Date:     Summer 2010
// Changes:  Took out all minus signs from spacecraft position to correct 
//           attitude.
// Modified: 2015/08/31:  John Downing -- Major rewrite to include new Structure.
//
/**
 * Object for storing model data loaded from a file. Also handles self-rendering
 */
//====================================================================
#include "ModelObject.hpp"
#include "gmatdefs.hpp"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "GmatConstants.hpp"
#include "GmatOpenGLSupport.hpp" 
#include "StringUtil.hpp"
#include <wx/image.h>
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "SubscriberException.hpp"
#include "StructureReader3ds.hpp"
//====================================================================
//#define DEBUG_LOAD_TEXTURE
//int control_object;
//--------------------------------------------------------------------
// ModelObject()
//--------------------------------------------------------------------
/**
 * Constructor
 */
//--------------------------------------------------------------------
ModelObject::ModelObject()
   : isLoaded (false)
   {
   StructureScale = 1;
   StructureOffset[0] = 0;
   StructureOffset[1] = 0;
   StructureOffset[2] = 0;
   Reset();
   }
//--------------------------------------------------------------------
// ~ModelObject()
//--------------------------------------------------------------------
/**
 * Destructor
 */
//--------------------------------------------------------------------
ModelObject::~ModelObject()
   {
   if (TheStructure != 0)
      delete TheStructure;
   TheStructure = 0;
   }
//--------------------------------------------------------------------
// char Load(const std::string &full_path)
//--------------------------------------------------------------------
// Loads a model from a 3ds file and sets some parameters
//
// full_path: The full path description of the model file to be loaded
//
// Return value: 1 if the object loaded correctly, 0 otherwise (char)
//--------------------------------------------------------------------
char ModelObject::Load (const std::string &full_path)
   {
   std::string path, string, extension;
   size_t loc = full_path.find_last_of('/');
   if (loc != std::string::npos)
      path = full_path.substr(0,loc);

   // Might have Windows path delimiters
   if (path.length() <= 0)
      {
      loc = full_path.find_last_of('\\');
      if (loc != std::string::npos)
         path = full_path.substr(0,loc);
      path += '\\';
      }
   else
      path += '/';
   
   loc = full_path.find_last_of('.');
   extension = full_path.substr(loc+1);

   if (extension == wxT("3ds") || extension == wxT("3DS"))
      {
      StructureReader3ds* reader = new StructureReader3ds(full_path);
      reader->Execute();
      TheStructure = reader->TheStructure;
      reader->TheStructure = 0;
//      std::ofstream f ("structure.out");
//      TheStructure->WriteSummary (f,0);
//      f.close();
      delete reader;
      reader = 0;
      isLoaded = true;
      }
   else if (extension == wxT("pov") || extension == wxT("POV"))
      {
      MessageInterface::ShowMessage("POV model files are not currently "
            "supported in GMAT\n");
      return 0;
      }
   filePath = path;

   // Then we go through each texture from the materials and load those. Any that failed
   // we bind to the default texture passed in
   if (TheStructure != 0)
      {
      LoadTextures();
      StructureScale = 1.0/TheStructure->Radius;
      for (int i=0;  i<=3-1;  ++i)
         StructureOffset[i] = -TheStructure->Center[i];
      isLoaded = true;
      return 1;
      }
   else
      return 0;
   }
//--------------------------------------------------------------------
// void LoadTextures()
//--------------------------------------------------------------------
void ModelObject::LoadTextures()
   {
   #ifdef DEBUG_LOAD_TEXTURE
   MessageInterface::ShowMessage
      ("ModelObject::LoadTextures() entered, num_materials = %d\n",
       num_materials);
   #endif
   
   GLuint result = -1;
   for (Integer i=0;  i<=TheStructure->Materials.Size()-1;  ++i)
      {
      if (TheStructure->Materials[i]->TextureMap != "")
         {
         result = LoadTexture(TheStructure->Materials[i]->TextureMap);
         TheStructure->Materials[i]->GlTextureId = result;
         }
      }
   }
//--------------------------------------------------------------------
// int LoadTexture(const std::string &filename)
//--------------------------------------------------------------------
/**
 * Loads a texture, binds it, and returns the generated texture id
 */
//--------------------------------------------------------------------
int ModelObject::LoadTexture(const std::string &filename)
{
   #ifdef DEBUG_LOAD_TEXTURE
   MessageInterface::ShowMessage
      ("ModelObject::LoadTexture() entered, filename='%s'\n", filename.c_str());
   #endif
   
   GLuint id;
   GLenum error;
   wxImage image; 
   bool result = false;
   std::string ext;
   
   if (!wxFileExists(filename.c_str()))
   {
      #ifdef DEBUG_LOAD_TEXTURE
      MessageInterface::ShowMessage
         ("ModelObject::LoadTexture() returning -1, file does not exist\n");
      #endif
      return -1;
   }
  
   // Load the file
   size_t loc = filename.find_last_of('.');
   if (loc != std::string::npos)
      ext = filename.substr(loc+1);
   if (strcmp(ext.c_str(), "bmp") == 0 || strcmp(ext.c_str(), "BMP") == 0)
      result = image.LoadFile(filename.c_str(), wxBITMAP_TYPE_BMP, -1);
   else if (strcmp(ext.c_str(), "jpg") == 0 || strcmp(ext.c_str(), "JPG") == 0)
      result = image.LoadFile(filename.c_str(), wxBITMAP_TYPE_JPEG, -1);
   else if (strcmp(ext.c_str(), "png") == 0 || strcmp(ext.c_str(), "PNG") == 0)
      result = image.LoadFile(filename.c_str(), wxBITMAP_TYPE_PNG, -1);
   else if (strcmp(ext.c_str(), "tif") == 0 || strcmp(ext.c_str(), "TIF") == 0)
      result = image.LoadFile(filename.c_str(), wxBITMAP_TYPE_TIF, -1);
   else if (strcmp(ext.c_str(), "tga") == 0 || strcmp(ext.c_str(), "TGA") == 0)
      result = image.LoadFile(filename.c_str(), wxBITMAP_TYPE_TGA, -1);
   else
      return -1;
   
   if (!result)
   {
      #ifdef DEBUG_LOAD_TEXTURE
      MessageInterface::ShowMessage
         ("ModelObject::LoadTexture() returning -1, Could not load image\n");
      #endif
      return -1;
   }
   
   int width = image.GetWidth();
   int height = image.GetHeight();
   GLubyte *data = image.GetData();
   
   #ifdef DEBUG_LOAD_TEXTURE
   MessageInterface::ShowMessage("   Calling glGenTextures() to generate texture id\n");
   #endif
   // Generate the texture id
   glGenTextures(1, &id);
   error = glGetError();

   #ifdef DEBUG_LOAD_TEXTURE
   MessageInterface::ShowMessage("   Calling glBindTexture() with texture id: %d\n", id);
   #endif
   // Bind the texture id
   glBindTexture(GL_TEXTURE_2D, id);
   error = glGetError();
   
   // Set the texture parameters
   // Repeat the image if the u,v coordinates exceed the 0,1 range
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   
   //=================================================================
   // Create the 2D mipmaps for minification non liniux platform
   // gluBuild2DMipmaps() crashes on Linux, so it is excluded here.
   #ifndef __WXGTK__
   //=================================================================
   
   // Linearly minify the texture (is that even a real word?)
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
   // Linearly magnify the texture
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   // This call crashes GMAT on Linux, so it is excluded here. 
   gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
   
   //=================================================================
   #else
   //=================================================================
   
   // Linearly minify the texture (is that even a real word?)
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   // Linearly magnify the texture
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   // Define the 2D texture
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
   
   //=================================================================
   #endif
   //=================================================================
   
   #ifdef DEBUG_LOAD_TEXTURE
   MessageInterface::ShowMessage
      ("ModelObject::LoadTexture() returning texture id: %d\n", id);
   #endif
   
   return id;
}
//------------------------------------------------------------------------------
// void SetName(const std::string &nameFromFile)
//------------------------------------------------------------------------------
/**
 * Sets the model name, usually from the data in the model file
 *
 * @param nameFromFile The model name read from the file
 */
//------------------------------------------------------------------------------
void ModelObject::SetName(const std::string &nameFromFile)
{
   name = nameFromFile;
}
//--------------------------------------------------------------------
// bool IsLoaded()
//--------------------------------------------------------------------
/**
 * Retrieves the flag indicating if the model has been loaded
 *
 * @return The isLoaded flag
 */
//--------------------------------------------------------------------
bool ModelObject::IsLoaded()
{
   return isLoaded;
}
//--------------------------------------------------------------------
// void SetBodyPosition (float x, float y, float z)
//--------------------------------------------------------------------
// Sets the object's matrix translation values
//
// x: x coordinate
// y: y coordinate
// z: z coordinate
//--------------------------------------------------------------------
void ModelObject::SetBodyPosition (float x, float y, float z)
   {
   BodyPosition[0] = x;
   BodyPosition[1] = y;
   BodyPosition[2] = z;
   }
//--------------------------------------------------------------------
// void SetBodyRotation (bool useDegrees, float x, float y, float z)
//--------------------------------------------------------------------
// Rotates the object around its local axi by the given angles
//
// use_degrees: use radians or degrees
// x: x angle
// y: y angle
// z: z angle
//--------------------------------------------------------------------
void ModelObject::SetRotation (bool useDegrees, float x, float y, float z)
   {
   float factor = 1.0;
   // If we're using degrees, we'll need to add the conversion factor
   if (useDegrees)
      factor = (float)(GmatMathConstants::RAD_PER_DEG);
   BodyRotation[0] = x * factor;
   BodyRotation[1] = y * factor;
   BodyRotation[2] = z * factor;
   }
//--------------------------------------------------------------------
// void SetAttitude (bool useDegrees, float x, float y, float z)
//--------------------------------------------------------------------
// Rotates the object around its local axi by the given angles
//
// use_degrees: use radians or degrees
// x: x angle
// y: y angle
// z: z angle
//--------------------------------------------------------------------
void ModelObject::SetAttitude (bool useDegrees, float x, float y, float z)
   {
   float factor = 1.0;
   // If we're using degrees, we'll need to add the conversion factor
   if (useDegrees)
      factor = (float)(GmatMathConstants::RAD_PER_DEG);
   Attitude[0] = x * factor;
   Attitude[1] = y * factor;
   Attitude[2] = z * factor;
   }
//--------------------------------------------------------------------
// void Scale(float x, float y, float z)
//--------------------------------------------------------------------
// Scales the object
//
// x: The amount to scale by in the x direction
// y: The amount to scale by in the y direction
//-------------------------------------------------------------------------------
void ModelObject::SetScale (float x)
   {
   Scale = x;
   }
//--------------------------------------------------------------------
// void Reset()
//--------------------------------------------------------------------
// Resets the model's matrix to the identity matrix
void ModelObject::Reset()
   {
   for (int i = 0; i < 3; i++)
      {
      BodyRotation[i] = 0;
      Attitude[i] = 0;
      BodyPosition[i] = 0;
      }
   Scale = 1;
   }
//--------------------------------------------------------------------
void ModelObject::DrawAsSpacecraft (bool isLit)
   {
   Draw (isLit,true);
   }
//--------------------------------------------------------------------
void ModelObject::DrawAsCelestialBody (bool isLit)
   {
   Draw (isLit,false);
   }
//--------------------------------------------------------------------
// void ModelObject::Draw(bool isLit)
//--------------------------------------------------------------------
/**
 * Draw the object on the screen
 */
//--------------------------------------------------------------------
void ModelObject::Draw (bool isLit, bool ispacecraft)
   {
   SetMatrix(ispacecraft);
   // Save the current matrix
   glPushMatrix();
   // Multiply the object matrix by the object's matrix
   glMultMatrixf(matrix.element);
   TheStructure->Render ();

   // Put the matrix back on the stack
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
   // Flush openGL
   glFlush();
   }
//--------------------------------------------------------------------
// void SetMatrix()
//--------------------------------------------------------------------
// Sets the matrix to use the base and currents for translation, rotation, etc.
//--------------------------------------------------------------------
void ModelObject::SetMatrix (bool isspacecraft)
   {
   matrix_type mat;
   MatrixIdentity(&matrix);
   float rotcos, rotsin;

   MatrixRotation (&mat,Attitude);
   MatrixMult(&mat, &matrix, &matrix);

   MatrixRotation (&mat,BodyRotation);
   MatrixMult(&mat, &matrix, &matrix);

   MatrixIdentity(&mat);
   if (isspacecraft)
      {
      MatrixSetElement(&mat, 0, 0, Scale*1000);
      MatrixSetElement(&mat, 1, 1, Scale*1000);
      MatrixSetElement(&mat, 2, 2, Scale*1000);
      }
   else
      {
      MatrixSetElement(&mat, 0, 0, Scale);
      MatrixSetElement(&mat, 1, 1, Scale);
      MatrixSetElement(&mat, 2, 2, Scale);
      }
   MatrixMult(&mat, &matrix, &matrix);
   
   MatrixIdentity(&mat);
   MatrixSetElement(&mat, 3, 0, BodyPosition[0]);
   MatrixSetElement(&mat, 3, 1, BodyPosition[1]);
   MatrixSetElement(&mat, 3, 2, BodyPosition[2]);
   MatrixMult(&mat, &matrix, &matrix);
   
   if (isspacecraft)
      {
      MatrixIdentity(&mat);
      MatrixSetElement(&mat, 0, 0, StructureScale);
      MatrixSetElement(&mat, 1, 1, StructureScale);
      MatrixSetElement(&mat, 2, 2, StructureScale);
      MatrixMult(&mat, &matrix, &matrix);

      MatrixIdentity(&mat);
      MatrixSetElement(&mat, 3, 0, StructureOffset[0]);
      MatrixSetElement(&mat, 3, 1, StructureOffset[1]);
      MatrixSetElement(&mat, 3, 2, StructureOffset[2]);
      MatrixMult(&mat, &matrix, &matrix);
      }
   }
//--------------------------------------------------------------------
// void MatrixSetElement(matrix_type_ptr matrix, int r, int c, float value)
//--------------------------------------------------------------------
// Sets the the element of matrix.
//
// matrix: The matrix whose element we are setting
// r: The row location of the element
// c: The column location of the element
// value: The value to set the element to
//--------------------------------------------------------------------
void ModelObject::MatrixSetElement(matrix_type_ptr matrix, int r, int c, float value)
{
   matrix->element[4*r+c] = value;
}
//--------------------------------------------------------------------
// float MatrixGetElement(matrix_type_ptr matrix, int r, int c)
//--------------------------------------------------------------------
// Retrieve an element of a matrix
//
// matrix: The matrix whose element we are getting
// r: The row location of the element
// c: The column location of the element
// 
// Return value: The value of the element at the given location (float)
//--------------------------------------------------------------------
float ModelObject::MatrixGetElement(matrix_type_ptr matrix, int r, int c)
{
   return matrix->element[4*r+c];
}
//--------------------------------------------------------------------
// void MatrixIdentity(matrix_type_ptr matrix)
//--------------------------------------------------------------------
// Sets the matrix to an identity matrix. All data previously stored will be lost
//
// matrix: The matrix we are setting to an identity matrix
//--------------------------------------------------------------------
void ModelObject::MatrixIdentity(matrix_type_ptr matrix)
{
   for (int i = 0; i < 16; i++)
   {
      if (i%5 == 0)
         matrix->element[i] = 1;
      else
         matrix->element[i] = 0;
   }
}
//--------------------------------------------------------------------
// void MatrixRotation(matrix_type_ptr matrix)
//--------------------------------------------------------------------
// Buids a rotation matrix
//
// matrix: The matrix we are setting to an rotation matrix
//--------------------------------------------------------------------
void ModelObject::MatrixRotation (matrix_type_ptr matrix, float angle[3])
{
   matrix_type mat;
   MatrixIdentity(matrix);
   float rotcos, rotsin;
   
   MatrixIdentity(&mat);
   rotcos = (float)cos(angle[0]);
   rotsin = (float)sin(angle[0]);
   MatrixSetElement(&mat, 1, 1, rotcos);
   MatrixSetElement(&mat, 1, 2, rotsin);
   MatrixSetElement(&mat, 2, 1, -rotsin);
   MatrixSetElement(&mat, 2, 2, rotcos);
   MatrixMult(&mat, matrix, matrix);
   
   MatrixIdentity(&mat);
   rotcos = (float)cos(angle[1]);
   rotsin = (float)sin(angle[1]);
   MatrixSetElement(&mat, 0, 0, rotcos);
   MatrixSetElement(&mat, 0, 2, -rotsin);
   MatrixSetElement(&mat, 2, 0, rotsin);
   MatrixSetElement(&mat, 2, 2, rotcos);
   MatrixMult(&mat, matrix, matrix);
   
   MatrixIdentity(&mat);
   rotcos = (float)cos(angle[2]);
   rotsin = (float)sin(angle[2]);
   MatrixSetElement(&mat, 0, 0, rotcos);
   MatrixSetElement(&mat, 0, 1, rotsin);
   MatrixSetElement(&mat, 1, 0, -rotsin);
   MatrixSetElement(&mat, 1, 1, rotcos);
   MatrixMult(&mat, matrix, matrix);
}
//--------------------------------------------------------------------
// void MatrixZero(matrix_type_ptr matrix)
//--------------------------------------------------------------------
// Zeros out all elements of a matrix (sets the elements to zero).
//
// maitrx: The matrix we will zero out
//--------------------------------------------------------------------
void ModelObject::MatrixZero(matrix_type_ptr matrix)
{
   for (int i = 0; i < 16; i++)
      matrix->element[i] = 0;
}
//--------------------------------------------------------------------
// void MatrixCopy(matrix_type_ptr source, matrix_type_ptr destination)
//--------------------------------------------------------------------
// Copy all of the elements from one matrix to another
//
// source: The matrix we are copying from
// destination: The matrix we are copying to
//--------------------------------------------------------------------
void ModelObject::MatrixCopy(matrix_type_ptr source, matrix_type_ptr destination)
{
   for (int i = 0; i < 16; i++)
      destination->element[i] = source->element[i];
}
//--------------------------------------------------------------------
// void MatrixMult(matrix_type_ptr matrix1, matrix_type_ptr matrix2, matrix_type_ptr result)
//--------------------------------------------------------------------
// Multiply one matrix by another.
// As order matters with matrices, the order is thus: matrix1 * matrix2
//
// matrix1: The first matrix in the multiplication
// matrix2: The second matrix in the multiplication
// result: Where the resulting matrix is stored
//--------------------------------------------------------------------
void ModelObject::MatrixMult(matrix_type_ptr matrix1, matrix_type_ptr matrix2, matrix_type_ptr result)
{
   matrix_type res;
   MatrixZero(&res);
   for (int i = 0; i < 16; i++)
      for (int j = 0; j < 4; j++)
         res.element[i] += matrix1->element[j+(i/4*4)] * matrix2->element[j*4+i%4];
   MatrixCopy(&res, result);
}
//====================================================================
