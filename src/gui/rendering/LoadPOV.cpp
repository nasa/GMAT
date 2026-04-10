//$Id$
//------------------------------------------------------------------------------
//                            LoadPOV.cpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Phillip Silvia, Jr.
// Created: 2009/10/09
/**
 * This is the utility to loading in the model data from a POV-ray file. 
 * Derived from Dunning Idle 5's code from Odyssey. 
 */
//------------------------------------------------------------------------------

#include "LoadPOV.hpp"
#include "wx/textfile.h"
#include "MessageInterface.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

const int MAXOBJECTS = 300;
const int MAXFC      = 70000;
const int MAXCONOBJ  = 11;

enum POVTYPE
{
   PovSearch = 1,
   PovObject,
   PovNonTexture,
   PovSmoothTriangle,
   PovArticulation,
   PovTexture
};


//------------------------------------------------------------------------------
// void GetMaterialColor (ifstream &InFile, ModelObject *obj)
//------------------------------------------------------------------------------
void GetMaterialColor (ifstream &InFile, ModelObject *obj)
{
   char *tokenPtr;
   char ChRed[12];
   char ChGreen[12];
   char ChBlue[12];
   char ChAlpha[12];
   int IntDummy;
   int n = obj->num_materials;
   
   InFile >> ChRed >> ChGreen >> ChBlue >> ChAlpha;
   
   tokenPtr = strtok(ChRed, "<");
   IntDummy = *strcpy(ChRed, tokenPtr);
   tokenPtr = strtok(ChRed, ",");
   obj->material[n].mat_specular.r = (float)atof(ChRed);
   
   tokenPtr = strtok(ChGreen, ",");
   obj->material[n].mat_specular.g = (float)atof(ChGreen);
   
   tokenPtr = strtok(ChBlue, ",");
   obj->material[n].mat_specular.b = (float)atof(ChBlue);
   
   tokenPtr = strtok(ChAlpha, ">");
   obj->material[n].mat_specular.a = 1.0f - (float)atof(ChAlpha);
   
   obj->num_materials++;
}


//------------------------------------------------------------------------------
// void LoadPOV(ModelObject *obj, const std::string &modelPath)
//------------------------------------------------------------------------------
void LoadPOV(ModelObject *obj, const std::string &modelPath)
{
   const int ASize = 32;
   
   char Command[ASize];
   char PrevCommand[ASize];
   char ChDiffuse[ASize];
   char ChAmbient[ASize];
   char ChPhong[ASize];
   char ChPhongSize[ASize];
   char ChDummy[ASize];
   char ChNorm1x[ASize];
   char ChNorm1y[ASize];
   char ChNorm1z[ASize];
   char ChNorm2x[ASize];
   char ChNorm2y[ASize];
   char ChNorm2z[ASize];
   char ChNorm3x[ASize];
   char ChNorm3y[ASize];
   char ChNorm3z[ASize];
   char ChVert1x[ASize];
   char ChVert1y[ASize];
   char ChVert1z[ASize];
   char ChVert2x[ASize];
   char ChVert2y[ASize];
   char ChVert2z[ASize];
   char ChVert3x[ASize];
   char ChVert3y[ASize];
   char ChVert3z[ASize];
   char Pathname[256];
   
   //std::string line, word;
   
   POVTYPE CommandCase = PovSearch;
   bool IsMoreData = true;
   int FacetCounter = 1;
   int I;
   int IntDummy;
   int IntObNum;
   int J;
   int NumArtOb;
   int NumConSets;
   int TotObjects;
   
   ifstream InFile(modelPath.c_str(), ios::in);
   
   if (InFile.fail())
   {
      std::string msg = "Could not open model file: ";
      msg += modelPath;
      MessageInterface::LogMessage(msg + "\n");
      return;
   }
   
   // --------- File entry
   
   while (IsMoreData)
   {
      switch (CommandCase)
      {
      case PovSearch:
         if (FacetCounter == MAXFC + 1)
         {
            IsMoreData = false;
            break;
         }
                        
         InFile >> Command;
         
         if (strncmp(Command, "EndOfFile", 9) == 0 || InFile.eof())
         {
            IsMoreData = false;
         }
         else
         {
            if (strncmp(Command, "(Object", 7) == 0)
            {
               CommandCase = PovObject;
               // --------- File entry
            }
            else
            {
               IntDummy = *strcpy(PrevCommand, Command);
            }
         }
         break;

      case PovObject:
         // --------- File entry

         InFile >> Command;

         if (strncmp(Command, "EndOfFile", 9) == 0 || InFile.eof())
         {
            IsMoreData = false;
         }
         else
         {
            if (strncmp(Command, "rgbf", 4) == 0)
            {
               GetMaterialColor(InFile, obj);
               CommandCase = PovNonTexture;
            }
            else
            {
               if (strncmp(Command, "rh_layercolor", 13) == 0)
               {
                  InFile >> ChDummy;
                  GetMaterialColor(InFile, obj);
                  CommandCase = PovTexture;
               }
            }
         }
         break;

      case PovNonTexture:
         InFile >> Command;
                                
         if (strncmp(Command, "EndOfFile", 9) == 0 || InFile.eof())
         {
            IsMoreData = false;
         }
         else
         {
            if (strncmp(Command, "ambient", 7) == 0)
            {
               InFile >> ChAmbient >> ChDummy >> ChDiffuse;

               // --------- File entry

               InFile >> ChDummy >> ChPhong >> ChDummy >> ChPhongSize;

               if (strncmp(ChDummy, "phong", 5) == 0)
               {
                  // --------- File entry
               }
               CommandCase = PovSmoothTriangle; 
            }
         }
         break;

      case PovSmoothTriangle:
         InFile >> Command;

         if (strncmp (Command, "EndOfFile", 9) == 0 || InFile.eof())
         {
            IsMoreData = false;
         }
         else
         {
            if (strncmp(Command, "object", 6) == 0)
            {
               CommandCase = PovSearch;
               // --------- File entry
               InFile >> ChDummy >> ChDummy >> ChDummy 
                      >> ChDummy >> ChDummy >> ChDummy;
               break;
            }
            else
            {
               if (strncmp(Command, "smooth_triangle", 15) == 0)
               {
                  InFile >> ChDummy >> ChVert1x >> ChVert1z >> ChVert1y
                         >> ChNorm1x >> ChNorm1z >> ChNorm1y
                         >> ChVert2x >> ChVert2z >> ChVert2y
                         >> ChNorm2x >> ChNorm2z >> ChNorm2y
                         >> ChVert3x >> ChVert3z >> ChVert3y
                         >> ChNorm3x >> ChNorm3z >> ChNorm3y
                         >> ChDummy;

                  int n = obj->num_vertices;

                  strcpy(ChVert1x, strtok(ChVert1x, "<"));
                  strtok(ChVert1x, ",");
                  strtok(ChVert1z, ",");
                  strtok(ChVert1y, ">");
                  obj->vertex[n].x = (float)atof(ChVert1x);
                  obj->vertex[n].y = (float)atof(ChVert1y);
                  obj->vertex[n].z = (float)atof(ChVert1z);

                  strcpy(ChVert2x, strtok(ChVert2x, "<"));
                  strtok(ChVert2x, ",");
                  strtok(ChVert2z, ",");
                  strtok(ChVert2y, ">");
                  obj->vertex[n+1].x = (float)atof(ChVert2x);
                  obj->vertex[n+1].y = (float)atof(ChVert2y);
                  obj->vertex[n+1].z = (float)atof(ChVert2z);
                                                        
                  strcpy(ChVert3x, strtok(ChVert3x, "<"));
                  strtok(ChVert3x, ",");
                  strtok(ChVert3z, ",");
                  strtok(ChVert3y, ",");
                  obj->vertex[n+2].x = (float)atof(ChVert3x);
                  obj->vertex[n+2].y = (float)atof(ChVert3y);
                  obj->vertex[n+2].z = (float)atof(ChVert3z);

                  obj->polygon[obj->num_polygons].a = n+2;
                  obj->polygon[obj->num_polygons].b = n+1;
                  obj->polygon[obj->num_polygons].c = n;

                  int m = obj->num_materials - 1;
                  obj->material[m].faces[obj->material[m].num_faces] = obj->num_polygons;
                  obj->material[m].num_faces++;
                  
                  strcpy(ChNorm1x, strtok(ChNorm1x, "<"));
                  strtok(ChNorm1x, ",");
                  strtok(ChNorm1z, ",");
                  strtok(ChNorm1y, ">");
                  obj->normal[n].x = -(float)atof(ChNorm1x);
                  obj->normal[n].y = -(float)atof(ChNorm1y);
                  obj->normal[n].z = -(float)atof(ChNorm1z);

                  strcpy(ChNorm2x, strtok(ChNorm2x, "<"));
                  strtok(ChNorm2x, ",");
                  strtok(ChNorm2z, ",");
                  strtok(ChNorm2y, ",");
                  obj->normal[n+1].x = -(float)atof(ChNorm2x);
                  obj->normal[n+1].y = -(float)atof(ChNorm2y);
                  obj->normal[n+1].z = -(float)atof(ChNorm2z);

                  strcpy(ChNorm3x, strtok(ChNorm3x, "<"));
                  strtok(ChNorm3x, ",");
                  strtok(ChNorm3z, ",");
                  strtok(ChNorm3y, ">");
                  obj->normal[n+2].x = -(float)atof(ChNorm3x);
                  obj->normal[n+2].y = -(float)atof(ChNorm3y);
                  obj->normal[n+2].z = -(float)atof(ChNorm3z);

                  obj->num_vertices += 3;
                  obj->num_polygons++;
                  FacetCounter = FacetCounter + 1;
                  CommandCase = PovSmoothTriangle;
               }
            }
         }
         break;
         
      case PovArticulation: // I am blatantly ignoring articulation
         // This space intentionally left blank;

         InFile >> ChDummy;
         InFile >> NumArtOb;
         InFile >> ChDummy;
         InFile >> NumConSets;
         // --------- File entry
         
         assert(MAXCONOBJ >= NumConSets);
         for (J = 0; J < NumConSets; J++)
         {
            InFile >> ChDummy;
            InFile >> ChDummy;
            InFile >> TotObjects;
            // --------- File entry

            assert(MAXCONOBJ >= TotObjects);
            for (I = 0; I < TotObjects; I++)
            {
               InFile >> IntDummy; // --------- File entry
            }
         }
         for (I = 0; I < NumArtOb; I++)
         {
            InFile >> ChDummy;
            // Some cout/cin stuff. What?
            InFile >> IntObNum;
            // --------- File entry
            // More cout/cin...why?
            // --------- File Entry below
            InFile >> ChDummy >> IntDummy;
            InFile >> ChDummy >> IntDummy;
            InFile >> ChDummy >> IntDummy;
            InFile >> ChDummy >> IntDummy;
            InFile >> ChDummy >> IntDummy;
            InFile >> ChDummy >> IntDummy;
            InFile >> ChDummy >> IntDummy;
            InFile >> ChDummy >> IntDummy;
            InFile >> ChDummy >> IntDummy;
            InFile >> ChDummy >> IntDummy;
            InFile >> ChDummy >> IntDummy;
            InFile >> ChDummy >> IntDummy;
            InFile >> ChDummy >> IntDummy;
         }
         IsMoreData = false;
         break;
         
      case PovTexture:
         InFile >> Command;

         if (strncmp(Command, "EndOfFile", 9) == 0 || InFile.eof() )
         {
            IsMoreData = false;
         }
         else if (strncmp(Command, "rh_phong", 7) == 0)
         {
            // --------- File entry
            InFile >> ChDummy >> ChPhong;
            // --------- File entry
         }
         else if (strncmp(Command, "rh_phong_size", 7) == 0)
         {
            InFile >> ChDummy >> ChPhongSize;
            // --------- File entry
         }
         else if (strncmp(Command, "rh_image_map", 7) == 0)
         {
            InFile >> ChDummy >> Pathname;
            // --------- File entry
            CommandCase = PovSmoothTriangle;
         }
         break;
      }
   }
}
