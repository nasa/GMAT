// $Id$
//------------------------------------------------------------------------------
//                           MessageInterface
//------------------------------------------------------------------------------
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/05/20
//
// This code is Copyright (c) 2005, Thinking Systems, Inc.
// Licensed under the Academic Free License version 3.0.
/**
 * Definitions used for compatibility issues and other items used in TsPlot.
 */
//------------------------------------------------------------------------------


#ifndef TSPLOTDEFS_HPP_
#define TSPLOTDEFS_HPP_

#include <wx/wx.h>

// The following block manages the change in deprecated functions between
// wx 2.6 and 2.8
#if wxCHECK_VERSION(2, 8, 0)

    #define wxRECT_CONTAINS Contains
    #define wxBEGIN_DRAWING
    #define wxEND_DRAWING  
    #define wxFILE_SAVE_FLAG wxFD_SAVE
    #define wxFILE_OVERWRITE_FLAG wxFD_OVERWRITE_PROMPT

#else // replacement code for old version
    
    #define wxRECT_CONTAINS Inside
    #define wxBEGIN_DRAWING dc.BeginDrawing();
    #define wxEND_DRAWING   dc.BeginDrawing();
    #define wxFILE_SAVE_FLAG wxSAVE
    #define wxFILE_OVERWRITE_FLAG wxOVERWRITE_PROMPT

#endif

// VC++ Omits definitions for some math numbers
#ifndef M_PI_2
#define M_PI_2 M_PI/2.0
#endif

// Enumeration that defines the implemented marker styles
enum MarkerType
{
   unsetMarker = -1,
   xMarker,
   circleMarker,
   plusMarker,
   starMarker,
   squareMarker,
   diamondMarker,
   xsquareMarker,
   triangleMarker,
   nablaMarker,
   gunsightMarker,
   MarkerCount,
   highlightMarker,         // Special case, only used for highlighted points
   hilowMarker,             // Special case, for showing error bars
   sigma13Marker,           // Special case, for showing 3 sigma error bars w/ a hole
};

#endif /*TSPLOTDEFS_HPP_*/
