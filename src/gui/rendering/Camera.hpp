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

#ifndef _CAMERA_H
#define _CAMERA_H

#include "Rvector3.hpp"
#include "Rmatrix.hpp"

class Camera
{
public:
   Camera();
   Camera(Real x, Real y, Real z);
   Camera(Rvector3 initial_location);
   ~Camera();

   // Camera's current location
   Rvector3 position;
   // Three vectors to track the orientation of the camera
   Rvector3 forward;
   Rvector3 up;
   Rvector3 right;
   // View center location
   Rvector3 view_center;
   // The field of view, in degrees
   Real fovDeg;
   // The id of the object the camera is tracking
   int trackingId;

   void Relocate (Real position_x, Real position_y, Real position_z, 
      Real center_x, Real center_y, Real center_z);
   void Relocate (Rvector3 new_position, Rvector3 new_center);
   void Translate (Real x, Real y, Real z, bool move_center);
   void TranslateW (Real x, Real y, Real z, bool move_center);
   void TranslateCenter (Real x, Real y, Real z);
   void TranslateCenterW (Real x, Real y, Real z);
   void Rotate (Real x_angle, Real y_angle, Real z_angle, bool use_degrees, bool move_camera);
   void Reset ();
   void ReorthogonalizeVectors();
   void ZoomIn(Real zoom);
   void ZoomOut(Real zoom);
   /*void TrackStill(int tracking_object_id);
   void TrackFollow(int tracking_object_id);
   void Untrack();
   int TrackingMode();
   bool IsTracking();
   void TrackingUpdate(Rvector3 new_position);*/

private:
   enum CameraMode { STILL = 0, STILL_TRACKING = 1, FOLLOW_TRACKING = 2, TACKED_TRACKING = 3 };

   // The location that the camera is linked to
   Rvector3 tracking_position;
   CameraMode camera_mode;
   
   Rvector3 RotateAround(Rvector3 vector, Real angle, Rvector3 axis);
};

#endif
