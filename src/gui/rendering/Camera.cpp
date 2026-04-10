//$Id$
//------------------------------------------------------------------------------
//                              Camera
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
// Created: 2009/07/02
/**
 * Controls all camera functionality, including move the camera and tracking
 * objects with a camera.
 */
//------------------------------------------------------------------------------

#include "Camera.hpp"
#include <math.h>

const Real MAXZOOM = 180.;
const Real MINZOOM = 1.;

// Instantiate the Camera. The position and view center default to the origin
Camera::Camera(){
   Reset();
}

// Instantiate the Camera. Sets the position to (x,y,z)
Camera::Camera(Real x, Real y, Real z){
   Reset();
   position.Set(x,y,z);
}

// Instantiate the Camera. Sets the position to the given vector
Camera::Camera(Rvector3 initial_position){
   Reset();
   position.Set(initial_position[0],initial_position[1],initial_position[2]);
}

// Destroy the Camera
Camera::~Camera(){
}

// Translate the camera's position and possibly view center by x,y,z in the local plane.
// x translation is a left-right shift
// y translation is an up-down shift
// z translation is a forward-backward shift
void Camera::Translate (Real x, Real y, Real z, bool move_center){
   // Retrieve the right, up, and forward vectors
   Rvector3 r,u,f;
   // Make sure they are normalized
   r = right; r.Normalize();
   u = up; u.Normalize();
   f = forward; f.Normalize();
   // If we have a translation value, we move the position
   // We move the view_center as well if desired
   if (x != 0){
      position += r * x;
      if (move_center)
         view_center += r * x;
   }
   if (y != 0){
      position += u * y;
      if (move_center)
         view_center += u * y;
   }
   if (z != 0){
      position += f * z;
      if (move_center)
         view_center += f * z;
   }
   forward = view_center - position;
   // Reset the direction vectors to account for the new forward vector
   ReorthogonalizeVectors();
}

// Translate the camera's position and possibly view center by x,y,z in the world plane
void Camera::TranslateW (Real x, Real y, Real z, bool move_center){
   // Add the values to the position's x,y,z values
   position.Set(position[0] + x, position[1] + y, position[2] + z);
   // Move the view center as well if desired
   if (move_center)
      view_center.Set(view_center[0] + x, view_center[1] + y, view_center[2] + z);
   forward = view_center - position;
   // Reset the direction vectors to account for the new forward vector
   ReorthogonalizeVectors();
}

// Translate the camera's view center by the x,y,z in the local plane
void Camera::TranslateCenter (Real x, Real y, Real z){
   // Retrieve the right, up, and left direction vectors
   Rvector3 r, u, f;
   r = right; r.Normalize();
   u = up; u.Normalize();
   f = forward; f.Normalize();
   // If we have an x translation, use the right vector to make the shift
   if (x != 0){
      view_center += r * x;
   }
   // If we have an y translation, use the up vector to make the shift
   if (y != 0){
      view_center += u * y;
   }
   // If we have an z translation, use the forward vector to make the shift
   if (z != 0){
      view_center += f * z;
   }
   // Adjust the forward vector based on the new view center
   forward = view_center - position;
   // Reset the direction vectors to account for the new forward vector
   ReorthogonalizeVectors();
}

// Translates the camera's view center by the world coordinates given
void Camera::TranslateCenterW (Real x, Real y, Real z){
   // Add the values to the view center's x,y,z values
   view_center.Set(view_center[0] + x, view_center[1] + y, view_center[2] + z);
   // Adjust the forward vector based on the new view center
   forward = view_center - position;
   // Reset the direction vectors to account for the new forward vector
   ReorthogonalizeVectors();
}

// Inner function to rotate a vector around another vector by angle
Rvector3 Camera::RotateAround(Rvector3 vector, Real angle, Rvector3 axis){
   // Store the old x,y,z values
   Real oldX = vector[0], oldY = vector[1], oldZ = vector[2];
   // Store the cos and sin values, for convenience
   Real c = cos(angle), s = sin(angle);
   // Store the x,y,z values of the axis, for convenience
   axis.Normalize();
   Real x = axis[0], y = axis[1], z = axis[2];
   // Rotate the vector around the axis
   vector.Set((1+(1-c)*(x*x-1))*oldX + (-z*s+(1-c)*x*y)*oldY + (y*s+(1-c)*x*z)*oldZ,
      (z*s+(1-c)*x*y)*oldX + (1+(1-c)*(y*y-1))*oldY + (-x*s+(1-c)*y*z)*oldZ,
      (-y*s+(1-c)*x*z)*oldX + (x*s+(1-c)*y*z)*oldY + (1+(1-c)*(z*z-1))*oldZ);
   // Make sure the direction vectors are still orthogonal to each other
   //ReorthogonalizeVectors();
   return vector;
}

// Ensures the direction vectors are still orthogonal to each other
void Camera::ReorthogonalizeVectors(){
   // Calculates the direction vectors by taking cross products
	if (forward == up || forward == -up)
		up.Set(up[2], up[0], up[1]);
   right = Cross(forward, up);
   up = Cross(right, forward);
	forward = Cross(up, right);
   right.Normalize();
   forward.Normalize();
   up.Normalize();
}

// Rotates the camera by x,y,z angle. Can be degrees or radians
// The x_angle rotates the right and forward vectors
// The y_angle rotates the up and forward vectors
// The z_angle rotates the up and right vectors
// use_degrees determines whether we are in degrees or radians
// move_camera determines whether we are rotating around a point or rotating in place
void Camera::Rotate (Real x_angle, Real y_angle, Real z_angle, bool use_degrees, bool move_camera){
   // Grab the right, left, and up vectors are keep the original values
   Rvector3 r,u,f;
   if (use_degrees){
      x_angle *= (Real)(M_PI/180.);
      y_angle *= (Real)(M_PI/180.);
      z_angle *= (Real)(M_PI/180.);
   }
   // Make sure they are normalized, too
   r = right; r.Normalize();
   u = up; u.Normalize();
   f = forward; f.Normalize();
   // Rotate the position and respective direction vectors around the axis
   // if we have an angle value
   if (x_angle != 0.){
      if (move_camera)
         position = RotateAround(position, x_angle, u);
      right = RotateAround(right, x_angle, u);
      forward = RotateAround(forward, x_angle, u);
   }
   if (y_angle != 0){
      if (move_camera)
         position = RotateAround(position, y_angle, r);
      up = RotateAround(up, y_angle, r);
      forward = RotateAround(forward, y_angle, r);
   }
   if (z_angle != 0){
      if (move_camera)
         position = RotateAround(position, z_angle, f);
      up = RotateAround(up, z_angle, f);
      right = RotateAround(right, z_angle, f);
   }
   if (!move_camera)
      view_center = position + forward * view_center.GetMagnitude();
   ReorthogonalizeVectors();
}

// Move the camera and view center position to a new position. 
// Changes orientation since forward points toward view center. 
void Camera::Relocate (Real px, Real py, Real pz, Real cx, Real cy, Real cz){
   position.Set(px, py, pz);
   view_center.Set(cx, cy, cz);
   // We need to recalculate the direction vectors
   forward = view_center - position;
	forward.Normalize();
   //up.Set(0.0, 0.0, 1.0);
   ReorthogonalizeVectors();
}

// Move the camera and view center position to a new position.
// Changes orientation since foward points toward view center. 
void Camera::Relocate (Rvector3 new_position, Rvector3 new_center){
   position = new_position;
   view_center = new_center;
   // We need to recalculate the direction vectors
   forward = view_center - position;
	forward.Normalize();
   ReorthogonalizeVectors();
}

// Zoom the camera in by zoom
// Accomplishes this by decreasing the camera's FOV
// Maybe move the camera instead?
void Camera::ZoomIn(Real zoom)
{
   Real oldFov = fovDeg;

   fovDeg -= zoom;

   if (fovDeg <= 0.0)
      fovDeg = oldFov;

//	if (fovDeg == 0)
//		fovDeg = 1;
   /*if (fovDeg < MINZOOM)
      fovDeg = MINZOOM;
   if (fovDeg > MAXZOOM)
      fovDeg = MAXZOOM;*/
}

// Zoom the camera out by zoom
// Accomplishes this by increasing the camera's FOV
// Maybe move the camera instead?
void Camera::ZoomOut(Real zoom)
{
   Real oldFov = fovDeg;

   fovDeg += zoom;

   if (fovDeg > 180.0)
      fovDeg = oldFov;

//	if (fovDeg == 0)
//		fovDeg = 1;
   /*if (fovDeg < MINZOOM)
      fovDeg = MINZOOM;
   if (fovDeg > MAXZOOM)
      fovDeg = MAXZOOM;*/
}

// Resets the position, view_center, and direction vectors to defaults
// Also resets the camera mode back to Still.
void Camera::Reset (){
   fovDeg = 45.;
   position.Set(0,0,1);
   up.Set(0,0,1);
   right.Set(0,-1,0);
   forward.Set(-1,0,0);
   view_center.Set(0,0,0);
   //Untrack();
}

/*// Update the camera's position based on the new position of the object it is tracking
void Camera::TrackingUpdate(Rvector3 new_position){
   Rvector3 movement, movement2;
   // Action depends on our current mode
   switch (camera_mode){
      // If we are in still camera mode, we don't do anything
      case STILL:
         break;
      // If we are in still tracking, we adjust the view center's position but not the
      // camera position
      case STILL_TRACKING:
         // If the object has moved, we need to update 
         if (new_position != tracking_position){
            // Our movement will be the difference between the new and old positions
            movement = new_position - tracking_position;
            // Translate the center by the movement
            TranslateCenterW(-movement[0], -movement[1], movement[2]);
            // Store the tracking position for future comparisons
            tracking_position = new_position;
            forward = view_center - position;
            ReorthogonalizeVectors();
         }
         break;
      // If we are in follow tracking, we change the camera and view center's positions
      case FOLLOW_TRACKING:
         // If the object has moved, we need to update
         if (new_position != tracking_position){
            // Our movement will be the difference between the new and old positions
            movement = new_position - tracking_position;
            // Translate the center and position by the movement
            TranslateW(-movement[0], -movement[1], movement[2], true);
            // Store the tracking position for future comparisons
            tracking_position = new_position;
            forward = view_center - position;
            ReorthogonalizeVectors();
         }
         break;
   }
}

// Start the camera tracking an object with the given id.
// For still tracking, the camera's position doesn't move,
// it simply rotates to the view the object as it moves
void Camera::TrackStill(int objId){
   // Store the id of the object to be tracked
   trackingId = objId;
   // Set the view center to the origin
   view_center.Set(0, 0, 0);
   // Set the tracking position to the origin
   tracking_position.Set(0, 0, 0);
   // Set the camera's mode
   camera_mode = STILL_TRACKING;
}

// Start the camera tracking an object with the given id. 
// For follow tracking, the camera moves with the object. 
void Camera::TrackFollow(int objId){
   // Store the id of the object to be tracked
   trackingId = objId;
   // Set the view center to the origin
   view_center.Set(0,0,0);
   // Set the tracking position to the origin
   tracking_position.Set(0,0,0);
   // Set the camera's mode
   camera_mode = FOLLOW_TRACKING;
}

// Make the camera stop tracking an object
void Camera::Untrack(){
   // Negate the object id
   trackingId = -1;
   // Reset to Still camera mode
   camera_mode = STILL;
}

// Returns 0 if the camera is not tracking, 1 if it is still tracking, or 2 if it is follow tracking
int Camera::TrackingMode(){
   return (int)camera_mode;
}

// Returns true if the camera is in a tracking mode
bool Camera::IsTracking(){
   return camera_mode > 0;
}*/
