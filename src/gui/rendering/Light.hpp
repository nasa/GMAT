//------------------------------------------------------------------------------
//                              Light
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
// Author: Phillip Silvia, Jr.
// Created: 2009/08/04
/**
 * Stores information about a light source
 */
//------------------------------------------------------------------------------
#ifndef _LIGHT_H
#define _LIGHT_H

#include "Rvector3.hpp"

#include "gmatwxdefs.hpp"    // Makes Mac happier to see this
#ifdef __WXMAC__
#  ifdef __DARWIN__
#    include <OpenGL/gl.h>
#    include <OpenGL/glu.h>
#  else
#    include <gl.h>
#    include <glu.h>
#  endif
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

class Light{
private:
   GLfloat specular[4];
   Rvector3 position;
   bool directional;
public:

   Light();
   Light(float *position, GLfloat *specularColor);
   Light(float *position);
   Light(Rvector3 position, bool directional);

   void GetPositionf(float *pos);
   Rvector3 GetPosition();
   GLfloat* GetColor();
   bool IsDirectional();

   void SetColor(float red, float green, float blue, float alpha);
   void SetColor(float *color);
   void SetDirectional(bool isDirectional);
   void SetPosition(Rvector3 position);
   void SetPosition(float x, float y, float z);
};

#endif
