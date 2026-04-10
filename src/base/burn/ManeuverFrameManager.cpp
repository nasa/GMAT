//$Id$
//------------------------------------------------------------------------------
//                          ManeuverFrameManager
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/26
//
/**
 * Manager for all of the maneuver frame types. 
 * 
 * This class implements the interface to different representations of a 
 * maneuver based on classes derived from the ManeuverFrame class.  To add a new
 * frame, insert the calls for the derived classes in the three indicated 
 * places.
 */
//------------------------------------------------------------------------------


#include "ManeuverFrameManager.hpp"

// Add headers for the Maneuver frames here
#include "InertialManeuverFrame.hpp"
#include "VnbManeuverFrame.hpp"
// #include "LvlhManeuverFrame.hpp"


//------------------------------------------------------------------------------
//  ManeuverFrameManager()
//------------------------------------------------------------------------------
/**
 * Constructs the maneuver frame manager object (default constructor).
 */
//------------------------------------------------------------------------------
ManeuverFrameManager::ManeuverFrameManager() :
    currentFrame            (NULL)
{
    // Add the name of each supported frame to this list
//    frames.push_back("Inertial");
    frames.push_back("MJ2000Eq");
    frames.push_back("VNB");
}


//------------------------------------------------------------------------------
//  ~ManeuverFrameManager()
//------------------------------------------------------------------------------
/**
 * Destroys the maneuver frame manager object (destructor).
 */
//------------------------------------------------------------------------------
ManeuverFrameManager::~ManeuverFrameManager()
{
    if (currentFrame)
        delete currentFrame;
}


//------------------------------------------------------------------------------
//  StringArray& GetSupportedFrames()
//------------------------------------------------------------------------------
/**
 * Obtains a list of the maneuver frames available for use.
 * 
 * @return A StringArray containing the list of frames that can be used.
 */
//------------------------------------------------------------------------------
StringArray& ManeuverFrameManager::GetSupportedFrames()
{
    return frames;
}


//------------------------------------------------------------------------------
//  ManeuverFrame* GetFrameInstance(const std::string &frameType)
//------------------------------------------------------------------------------
/**
 * Obtains a requested maneuver frame.
 * 
 * @param <frameType> The maneuver frame that is needed.
 * 
 * @return A pointer to the requested frame.
 * 
 * @todo GetFrameInstance can be made more efficient by buffering the frame 
 *       type; the current implementation always destroys and reconstructs the 
 *       frame.
 *
 * @todo Add coordinate system support.
 */
//------------------------------------------------------------------------------
ManeuverFrame* ManeuverFrameManager::GetFrameInstance(
                                        const std::string &frameType)
{
    if (currentFrame)
    {
        delete currentFrame;
        currentFrame = NULL;
    }
    
    // Add constructor calls for the supported frames here
    // For now, keep "Inertial" for backwards compatibility
    if ((frameType == "Inertial") || (frameType == "MJ2000Eq"))
        currentFrame = new InertialManeuverFrame;
    if (frameType == "VNB")
        currentFrame = new VnbManeuverFrame;

    return currentFrame;
}
