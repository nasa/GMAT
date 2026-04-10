//$Id$
//------------------------------------------------------------------------------
//                              Stars.hpp
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
// ** Legal **
//
// Author: Phillip Silvia, Jr., Derived from Gene Stillman's code for Odyssey
// Created: 2009/09/21
// Modified: 2015/08/31:  Added constellation borders and changed colors
/**
 * This class handles loading the star and constellation information and drawing
 * them in to the scene. 
 *
 * GLStars is a singletons class, as there is no real need to have multiple
 * instances of it active. To use it, simple call InitStars on the instance
 * in the canvas that is drawing the stars, then call DrawStarsVA during your
 * draw loop. The file location is determined by the FileManager
 */
//------------------------------------------------------------------------------

#ifndef _GLStars_h_
#define _GLStars_h_

#include "gmatdefs.hpp"
#include "gmatwxdefs.hpp"

const int   MAXSTARS   = 42101;     // Max Number Stars in arrays
const int   MAXLINES   = 1600;      // Max Number of Constellation line vertices
const int   MAXCON     = 90;
const int   MAXBORDERS = 64000;     // Max Number of Constellation line vertices
const int   MAXBORDERGROUP = 90;    // Max Number of border groups
const float STARSTEP   = 0.5f;      // Scale into this size groups
const int   GROUPCOUNT = 18;        // Number of groups

//------------------------------------------------------------------------------
class GLStars
{
public:
   static GLStars* Instance();
   void DrawStarsVA(GLfloat ColorAlpha, int starCount, bool drawConstellations);    // Vertex Array Method, with alpha
   void SetDesiredStarCount(int count) {
      if (DesiredStarCount <= MAXSTARS)
         DesiredStarCount = count;
      else
         DesiredStarCount = MAXSTARS;
      };
   int  GetDesiredStarCount() { return DesiredStarCount; };
  
private:
   GLStars();
   ~GLStars();
   GLStars(const GLStars&);
   GLStars& operator= (const GLStars&);

private:
   void InitStars();                        // Initialize the Stars, including loading from file
   void ReadStars();
   void ReadConstellations();
   void ReadBorders();
   void SetVector (GLfloat v[4], Real ra, Real dec);
   void Correct1875 (GLfloat v[4]);
   
   GLfloat     Stars[MAXSTARS][4];
   GLfloat     CLines[MAXLINES][4];          // Constellation points
   GLfloat     Borders[MAXBORDERS][4];       // Constellation borders
   wxString    ConstellationNames[MAXCON];   // The names of the constellations
   int         ConstellationIndex[MAXCON][2]; // The starting and ending indices for each constellation
   int         GroupIndex[GROUPCOUNT];      // Indexes into the m_StarsVA array
   int         GroupCount[GROUPCOUNT];      // # of stars in this group
   //int         BorderGroup[90];
   int         BorderGroup[MAXBORDERGROUP];
   Real        PointSize[GROUPCOUNT];       // Group GLpointsize
   int         LastGroupUsed;               // Not all groups may be used
   int         MaxDrawStars;                // Not Array Size
   int         DesiredStarCount;            // DesiredStars, if we want it to be adjustable
   int         NumLines;                    // Number of constellation lines loaded
   int         NumConstellations;           // Number of constellations loaded
   int         BorderGroupCount;
   static      GLStars* theInstance;        // The singleton instance of the Stars
};
//------------------------------------------------------------------------------
#endif
