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

#include "Light.hpp"

// Initialize the Light as a black point light at position (0,0,0)
Light::Light(){
   position.Set(0,0,0);
   directional = false;
   for (int i = 0; i < 4; i++)
      specular[i] = 1.0f;
}

// Initialize the Light at an initial position with a given color
//
// initialPosition: Initial positon of the Light
// color: The rgba color of the Light
Light::Light(float *initialPosition, GLfloat *color){
   position[0] = initialPosition[0];
   position[1] = initialPosition[1];
   position[2] = initialPosition[2];
   directional = initialPosition[3] == 0;
   for (int i = 0; i < 4; i++)
      specular[i] = color[i];
}

// Initialize the Light in a given position
//
// initialPosition: The initial position of the Light
Light::Light(float *initialPosition){
   position[0] = initialPosition[0];
   position[1] = initialPosition[1];
   position[2] = initialPosition[2];
   directional = initialPosition[3] == 0;
   for (int i = 0; i < 4; i++)
      specular[i] = 1;
}

// Initialize the Light in a given position, and determine whether it is point or directional
//
// initialPosition: The initialPosition of the Light
// isDirectional: Determines whether or not the Light is directional
Light::Light(Rvector3 initialPosition, bool isDirectional){
   position = initialPosition;
   directional = isDirectional;
   for (int i = 0; i < 4; i++)
      specular[i] = 1;
}

// Retrieves the current position of the Light. This is used in
// OpenGL calls to glLightfv.
//
// pos: The float array into which the position will be placed
void Light::GetPositionf(float *pos){
   pos[0] = (float)position[0];
   pos[1] = (float)position[1];
   pos[2] = (float)position[2];
   pos[3] = directional? 0.0f : 1.0f;
}

Rvector3 Light::GetPosition(){
   return position;
}

// Returns the color of the Light
//
// return value: RGBA values for the color (GLfloat*)
GLfloat* Light::GetColor(){
   return specular;
}

// Returns true if the Light is directional, false if it is a point light
//
// return value: True if the Light is directional, false otherwise
bool Light::IsDirectional(){
   return directional;
}

// Sets the color of the Light
//
// red: Red value of the Light
// green: Green value of the Light
// blue: Blue value of the Light
// alpha: Alpha value of the Light
void Light::SetColor(float red, float green, float blue, float alpha){
   specular[0] = (GLfloat)red;
   specular[1] = (GLfloat)green;
   specular[2] = (GLfloat)blue;
   specular[3] = (GLfloat)alpha;
}

// Sets the color of the light
// 
// color: A float array containing the rgba color values
void Light::SetColor(float *color){
   for (int i = 0; i < 4; i++)
      specular[i] = (GLfloat)color[i];
}

// Sets whether or not the Light is directional
//
// isDirectional: Whether or not the Light will be true or not
void Light::SetDirectional(bool isDirectional){
   directional = isDirectional;
}

void Light::SetPosition(Rvector3 pos){
   position = pos;
}

void Light::SetPosition(float x, float y, float z){
   position.Set(x, y, z);
}
