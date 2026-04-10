//$Id$
//------------------------------------------------------------------------------
//                        File: Rendering.cpp      
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
// Author: Phillip Silvia, Jr.
// Created: 2009/06/17
/**
 * 
 */
//------------------------------------------------------------------------------

#include "gmatwxdefs.hpp"
#include "Rendering.hpp"
#include "GmatAppData.hpp"         // for GetGuiInterpreter()
#include "GmatOpenGLSupport.hpp"   // for OpenGL support
//#include <GL/gl.h>
//#include <GL/glu.h>

//------------------------------------------------------------------------------
// void SetColor(GlColorType color, Byte red, Byte green, Byte blue)
//------------------------------------------------------------------------------
/**
 * Sets the color values for a GlColorType. Purely for convenience
 */
//------------------------------------------------------------------------------
void SetColor(GlColorType color, Byte red, Byte green, Byte blue)
{
   color.red = red;
   color.green = green;
   color.blue = blue;
}

//------------------------------------------------------------------------------
// void DrawSphere(GLdouble radius, GLint slices, GLint stacks, ...
//------------------------------------------------------------------------------
/**
 * Draws a sphere with the given radius, number of slices, stacks, style, etc.
 */
//------------------------------------------------------------------------------
void DrawSphere(GLdouble radius, GLint slices, GLint stacks,
                GLenum style, GLenum orientation, GLenum normals,
                GLenum textureCoords)
{
   GLenum error = glGetError(); 
   GLUquadricObj* qobj = gluNewQuadric();
   error = glGetError(); 
   gluQuadricDrawStyle(qobj, style);
   error = glGetError(); 
   gluQuadricNormals(qobj, normals);
   error = glGetError(); 
   gluQuadricTexture(qobj, textureCoords);
   error = glGetError(); 
   gluSphere(qobj, radius, slices, stacks);
   error = glGetError(); 
   gluQuadricOrientation(qobj, orientation);
   error = glGetError(); 
   gluDeleteQuadric(qobj);
   error = glGetError(); 
}

//------------------------------------------------------------------------------
// void DrawLine(GlColorType *color, const Rvector3 &start, const Rvector3 &end)
//------------------------------------------------------------------------------
/**
 * Draw a line of the given color from start to end
 */
//------------------------------------------------------------------------------
void DrawLine(GlColorType *color, const Rvector3 &start, const Rvector3 &end)
{
   glPushMatrix();
   glBegin(GL_LINES);
   
   glColor3ub(color->red, color->green, color->blue);
   glVertex3f(start[0], start[1], start[2]);
   glVertex3f(end[0], end[1], end[2]);
   
   glEnd();
   glPopMatrix();
}

//------------------------------------------------------------------------------
// void DrawLine(float red, float green, float blue, const Rvector3 &start, const Rvector3 &end)
//------------------------------------------------------------------------------
/**
 * Draw a line of the given color from start to end
 */
//------------------------------------------------------------------------------
void DrawLine(float red, float green, float blue, const Rvector3 &start, const Rvector3 &end)
{
   glPushMatrix();
   glBegin(GL_LINES);
   
   glColor3f(red, green, blue);
   glVertex3f(start[0], start[1], start[2]);
   glVertex3f(end[0], end[1], end[2]);
   
   glEnd();
   glPopMatrix();
}


//------------------------------------------------------------------------------
// void DrawLine(double x1, double y1, double x2, double y2)
//------------------------------------------------------------------------------
/**
 * Draw a line from one point to another point in 2D
 */
//------------------------------------------------------------------------------
void DrawLine(double x1, double y1, double x2, double y2)
{
   glBegin(GL_LINES);
   glVertex2d(x1, y1);
   glVertex2d(x2, y2);
   glEnd();
}


//------------------------------------------------------------------------------
// void DrawCube(float x, float y, float z)
//------------------------------------------------------------------------------
void DrawCube(float x, float y, float z)
{
   glBegin(GL_QUADS);
        
   glNormal3f( 0.0F, 0.0F, 1.0F);
   glVertex3f( x, y, z);
   glVertex3f(-x, y, z);
   glVertex3f(-x,-y, z);
   glVertex3f( x,-y, z);

   glNormal3f( 0.0F, 0.0F,-1.0F);
   glVertex3f(-x,-y,-z);
   glVertex3f(-x, y,-z);
   glVertex3f( x, y,-z);
   glVertex3f( x,-y,-z);

   glNormal3f( 0.0F, 1.0F, 0.0F);
   glVertex3f( x, y, z);
   glVertex3f( x, y,-z);
   glVertex3f(-x, y,-z);
   glVertex3f(-x, y, z);

   glNormal3f( 0.0F,-1.0F, 0.0F);
   glVertex3f(-x,-y,-z);
   glVertex3f( x,-y,-z);
   glVertex3f( x,-y, z);
   glVertex3f(-x,-y, z);

   glNormal3f( 1.0F, 0.0F, 0.0F);
   glVertex3f( x, y, z);
   glVertex3f( x,-y, z);
   glVertex3f( x,-y,-z);
   glVertex3f( x, y,-z);

   glNormal3f(-1.0F, 0.0F, 0.0F);
   glVertex3f(-x,-y,-z);
   glVertex3f(-x,-y, z);
   glVertex3f(-x, y, z);
   glVertex3f(-x, y,-z);
   glEnd();
   glFlush();
}

//------------------------------------------------------------------------------
// void DrawSpacecraft(float radius, GlColorType *color1, GlColorType *color2,
//                     bool drawSphere = true)
//------------------------------------------------------------------------------
void DrawSpacecraft(float radius, GlColorType *color1, GlColorType *color2,
                    bool drawSphere)
{
   if (drawSphere)
   {
      glColor3ub(color1->red, color1->green, color1->blue);
      DrawSphere(radius, 50, 50, GLU_FILL);
   }
   else
   {
      glColor3ub(color1->red, color1->green, color1->blue);
      DrawCube(radius, radius, radius*2);
      glColor3ub(color2->red, color2->green, color2->blue);
      DrawCube(radius/4, radius*4, radius*1.5);
   }
}

//------------------------------------------------------------------------------
// void DrawEquatorialPlanes()
//------------------------------------------------------------------------------
void DrawEquatorialPlanes()
{
}

//------------------------------------------------------------------------------
// void DrawCircle(GLUquadricObj *qobj, Real radius)
//------------------------------------------------------------------------------
void DrawCircle(GLUquadricObj *qobj, Real radius)
{
   gluQuadricDrawStyle(qobj, GLU_LINE  );
   gluQuadricNormals  (qobj, GLU_SMOOTH);
   gluQuadricTexture  (qobj, GL_FALSE  );
   gluDisk(qobj, radius, radius, 50, 1);
}

//------------------------------------------------------------------------------
// void DrawCircle(double x, double y, double radius, bool fill = true)
//------------------------------------------------------------------------------
/**
 * Draws a circle around the point x and y.
 */
//------------------------------------------------------------------------------
void DrawCircle(double x, double y, double radius, bool fill)
{
   double angle = 0.0;
   double x1 = 0.0;
   double y1 = 0.0;
   double sinAngle = 0.0;
   double cosAngle  = 0.0;
   
   if (fill)
      glPolygonMode(GL_FRONT, GL_FILL);
   else
      glPolygonMode(GL_FRONT, GL_LINE);
   
   glBegin(GL_POLYGON);
   for (int i = 0; i < 360; i += 10)
   {
      angle = i * GmatMathConstants::RAD_PER_DEG;
      sinAngle = GmatMathUtil::Sin(angle);
      cosAngle = GmatMathUtil::Cos(angle);
      x1 = x - sinAngle * radius;
      y1 = y + cosAngle * radius;
      glVertex2d(x1, y1);
   }
   glEnd();
}

//------------------------------------------------------------------------------
// void DrawSquare(double x, double y, double radius, bool fill = true)
//------------------------------------------------------------------------------
/**
 * Draws a circle around the point x and y.
 */
//------------------------------------------------------------------------------
void DrawSquare(double x, double y, double radius, bool fill)
{
   if (fill)
      glPolygonMode(GL_FRONT, GL_FILL);
   else
      glPolygonMode(GL_FRONT, GL_LINE);
   
   glBegin(GL_QUADS);
   glVertex2d(x - radius, y - radius);
   glVertex2d(x + radius, y - radius);
   glVertex2d(x + radius, y + radius);
   glVertex2d(x - radius, y + radius);
   glEnd();
}


//------------------------------------------------------------------------------
// void DrawStringAt(const wxString &str, const Rvector3 &point)
//------------------------------------------------------------------------------
void DrawStringAt(const wxString &str, const Rvector3 &point)
{
   glRasterPos3d(point[0], point[1], point[2]);
   // Changed to use c_str() from std::string (LOJ: 2014.08.07)
   //glCallLists(strlen(str.c_str()), GL_BYTE, (GLubyte*)str.c_str());
   glCallLists(strlen(str.c_str()), GL_BYTE, (GLubyte*)(str.WX_TO_C_STRING));
}


//------------------------------------------------------------------------------
// void DrawStringAt(const wxString &str, GLfloat x, GLfloat y, GLfloat z, GLfloat k)
//------------------------------------------------------------------------------
void DrawStringAt(const wxString &str, GLfloat x, GLfloat y, GLfloat z, GLfloat k)
{
   glRasterPos4f(x, y, z, k);
   // Changed to use WX_TO_STD_STRING mactro for wx3.0 (LOJ: 2014.08.07)
   //glCallLists(strlen(str.c_str()), GL_BYTE, (GLubyte*)str.c_str());
   glCallLists(strlen(str.c_str()), GL_BYTE, (GLubyte*)(str.WX_TO_C_STRING));
}
