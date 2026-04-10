//$Id$
//------------------------------------------------------------------------------
//                                  OwnedPlot
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
// number NNG06CA54C
//
// Author: Darrel Conway, based on XyPlot code by Linda Jun
// Created: 2009/09/28
//
/**
 * Implements XyPlot class controlled by Sandbox elements rather than
 * Subscribers.
 */
//------------------------------------------------------------------------------

#include "OwnedPlot.hpp"
#include "PlotInterface.hpp"     // for XY plot
#include "SubscriberException.hpp"
#include "MessageInterface.hpp"  // for ShowMessage()
#include "ColorTypes.hpp"

//#define DEBUG_OwnedPlot_INIT 1
//#define DEBUG_OwnedPlot_PARAM 1
//#define DEBUG_OwnedPlot_OBJECT 1
//#define DEBUG_OwnedPlot_UPDATE 2
//#define DEBUG_ACTION_REMOVE 1
//#define DEBUG_RENAME 1

//---------------------------------
// static data
//---------------------------------

const std::string
OwnedPlot::PARAMETER_TEXT[OwnedPlotParamCount - GmatBaseParamCount] =
{
   "Add",
   "PlotTitle",
   "XAxisTitle",
   "YAxisTitle",
   "Grid",
   "DataCollectFrequency",
   "UpdatePlotFrequency",
   "ShowPlot",
   "ShowLegend",
   "DefaultColor",
   "UseLines",
   "LineWidth",
   "LineStyle",
   "UseMarkers",
   "MarkerSize",
   "Marker",
   "UseHiLow"
}; 


const Gmat::ParameterType
OwnedPlot::PARAMETER_TYPE[OwnedPlotParamCount - GmatBaseParamCount] =
{
   Gmat::OBJECTARRAY_TYPE, // "Add",
   Gmat::STRING_TYPE,      // "PlotTitle",
   Gmat::STRING_TYPE,      // "XAxisTitle",
   Gmat::STRING_TYPE,      // "YAxisTitle",
   Gmat::ON_OFF_TYPE,      // "Grid",
   Gmat::INTEGER_TYPE,     // "DataCollectFrequency",
   Gmat::INTEGER_TYPE,     // "UpdatePlotFrequency",
   Gmat::BOOLEAN_TYPE,     // "ShowPlot",
   Gmat::BOOLEAN_TYPE,     // "ShowLegend",
   Gmat::INTEGER_TYPE,     // "DefaultColor",  <-- May need to be unsigned
   Gmat::BOOLEAN_TYPE,     // "UseLines",
   Gmat::INTEGER_TYPE,     // "LineWidth",
   Gmat::INTEGER_TYPE,     // "LineStyle",
   Gmat::BOOLEAN_TYPE,     // "UseMarkers",
   Gmat::INTEGER_TYPE,     // "MarkerSize",
   Gmat::INTEGER_TYPE,     // "Marker",        <-- Maybe make a string
   Gmat::BOOLEAN_TYPE,     // "UseHiLow"
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// OwnedPlot(const std::string &name, const std::string &plotTitle,
//       const std::string &xAxisTitle, const std::string &yAxisTitle)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name Name of the plot.  This name should be unique in the Sandbox.
 * @param plotTitle Title of the plot
 * @param xAxisTitle Label for the x-axis
 * @param yAxisTitle Label for the y-axis
 */
//------------------------------------------------------------------------------
OwnedPlot::OwnedPlot(const std::string &name, const std::string &plotTitle,
               const std::string &xAxisTitle, const std::string &yAxisTitle) :
   Subscriber              ("OwnedPlot", name),
   mOldName                (name),
   mPlotTitle              (plotTitle),
   mXAxisTitle             (xAxisTitle),
   mYAxisTitle             (yAxisTitle),
   mDrawGrid               ("On"),
   mIsOwnedPlotWindowSet   (false),
   mDataCollectFrequency   (1),
   mUpdatePlotFrequency    (1),
   markerSize              (3),
   markerStyle             (-1),
   lineWidth               (1),
   lineStyle               (100),      // wxSOLID = 100, in wx/defs.h
   useLines                (true),
   useMarkers              (false),
   useHiLow                (false),
   active                  (true),
   showLegend              (true),
   isEndOfReceive          (false),
   isEndOfRun              (false),
   isInitialized           (false),
   mSolverIterations       ("All"),
   runstate                (Gmat::RUNNING)
{
   // GmatBase data
   objectTypes.push_back(Gmat::XY_PLOT);
   objectTypeNames.push_back("XYPlot");
   objectTypeNames.push_back("OwnedPlot");
   parameterCount = OwnedPlotParamCount;

   // Buffer 16 default colors
   defaultColor.push_back(GmatColor::BLUE);
   defaultColor.push_back(GmatColor::GREEN);
   defaultColor.push_back(GmatColor::RED);
   defaultColor.push_back(GmatColor::PURPLE);
   defaultColor.push_back(GmatColor::ORANGE);
   defaultColor.push_back(GmatColor::GOLDEN_ROD);
   defaultColor.push_back(GmatColor::MAROON);
   defaultColor.push_back(GmatColor::BROWN);
   defaultColor.push_back(GmatColor::BLUE_VIOLET);
   defaultColor.push_back(GmatColor::DARK_OLIVE_GREEN);
   defaultColor.push_back(GmatColor::MAGENTA);
   defaultColor.push_back(GmatColor::DARK_GRAY);
   defaultColor.push_back(GmatColor::PLUM);
   defaultColor.push_back(GmatColor::MIDNIGHT_BLUE);
   defaultColor.push_back(GmatColor::SIENNA);
   defaultColor.push_back(GmatColor::MEDIUM_SPRING_GREEN);

   // Swap R and B values: ColorTypes follows a different order from TSPlot
   for (UnsignedInt i = 0; i < defaultColor.size(); ++i)
   {
      Integer red   = defaultColor[i] & 0xff;
      Integer green = defaultColor[i] & 0x00ff00;
      Integer blue  = defaultColor[i] & 0xff0000;
      red = red << 16;
      blue = blue >> 16;
      defaultColor[i] = red + green + blue;
   }
}


//------------------------------------------------------------------------------
// OwnedPlot(const OwnedPlot &orig)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param orig The original owned plot getting copied here
 */
//------------------------------------------------------------------------------
OwnedPlot::OwnedPlot(const OwnedPlot &orig) :
   Subscriber              (orig),
   mOldName                (orig.mOldName),
   mPlotTitle              (orig.mPlotTitle),
   mXAxisTitle             (orig.mXAxisTitle),
   mYAxisTitle             (orig.mYAxisTitle),
   mDrawGrid               (orig.mDrawGrid),
   mIsOwnedPlotWindowSet   (orig.mIsOwnedPlotWindowSet),
   mDataCollectFrequency   (orig.mDataCollectFrequency),
   mUpdatePlotFrequency    (orig.mUpdatePlotFrequency),
   defaultColor            (orig.defaultColor),
   markerSize              (orig.markerSize),
   markerStyle             (orig.markerStyle),
   lineWidth               (orig.lineWidth),
   lineStyle               (orig.lineStyle),
   useLines                (orig.useLines),
   useMarkers              (orig.useMarkers),
   useHiLow                (orig.useHiLow),
   active                  (true),
   showLegend              (orig.showLegend),
   isEndOfReceive          (false),
   isEndOfRun              (false),
   isInitialized           (false),
   mSolverIterations       ("All"),
   runstate                (Gmat::RUNNING)
{
   curveNames        = orig.curveNames;
   curveDataIDs      = orig.curveDataIDs;
   supportedData     = orig.supportedData;
   supportedObjects  = orig.supportedObjects;
}


//------------------------------------------------------------------------------
// OwnedPlot& operator=(const OwnedPlot& orig)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 *
 * @param orig The original owned plot providing data for this one
 */
//------------------------------------------------------------------------------
OwnedPlot& OwnedPlot::operator=(const OwnedPlot& orig)
{
   if (this == &orig)
      return *this;
   
   Subscriber::operator=(orig);
   
   mOldName                = orig.mOldName;
   mPlotTitle              = orig.mPlotTitle;
   mXAxisTitle             = orig.mXAxisTitle;
   mYAxisTitle             = orig.mYAxisTitle;
   mDrawGrid               = orig.mDrawGrid;
   mIsOwnedPlotWindowSet   = orig.mIsOwnedPlotWindowSet;
   mDataCollectFrequency   = orig.mDataCollectFrequency;
   mUpdatePlotFrequency    = orig.mUpdatePlotFrequency;
   defaultColor            = orig.defaultColor;
   markerSize              = orig.markerSize;
   markerStyle             = orig.markerStyle;
   lineWidth               = orig.lineWidth;
   lineStyle               = orig.lineStyle;
   useLines                = orig.useLines;
   useMarkers              = orig.useMarkers;
   useHiLow                = orig.useHiLow;
   active                  = true;
   showLegend              = orig.showLegend;
   isEndOfReceive          = false;
   isEndOfRun              = false;
   isInitialized           = false;
   mSolverIterations       = "All";
   runstate                = Gmat::RUNNING;
   
   curveNames              = orig.curveNames;
   curveDataIDs            = orig.curveDataIDs;
   supportedData           = orig.supportedData;
   supportedObjects        = orig.supportedObjects;

   return *this;
}


//------------------------------------------------------------------------------
// ~OwnedPlot()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
OwnedPlot::~OwnedPlot()
{
}


//------------------------------------------------------------------------------
// bool OwnedPlot::Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the structures used for plotting
 *
 * @return true on success, false if initialization failed
 */
//------------------------------------------------------------------------------
bool OwnedPlot::Initialize()
{
   #if DEBUG_OwnedPlot_INIT
      MessageInterface::ShowMessage
            ("OwnedPlot::Initialize() active=%d, mNumYParams=%d\n", active,
             mNumYParams);
   #endif
   
   Subscriber::Initialize();
   isEndOfReceive = false;
   isEndOfRun = false;
   
   bool status = false;
   DeletePlotCurves();
   
   if (active)
   {
      // build plot title
      BuildPlotTitle();
      
      // Create OwnedPlotWindow, if not exist
      #if DEBUG_OwnedPlot_INIT
         MessageInterface::ShowMessage
               ("OwnedPlot::Initialize() calling CreateOwnedPlotWindow()\n");
      #endif
      
      PlotInterface::CreateXyPlotWindow(instanceName, mOldName, 0.0, 0.0, 0.0, 0.0, false, mPlotTitle,
            mXAxisTitle, mYAxisTitle, (mDrawGrid == "On"), false);
      PlotInterface::TakeXYAction(instanceName, "AlwaysRedraw");
      PlotInterface::SetXyPlotTitle(instanceName, mPlotTitle);
      mIsOwnedPlotWindowSet = true;
            
      #if DEBUG_OwnedPlot_INIT
         MessageInterface::ShowMessage
               ("OwnedPlot::Initialize() Get curveTitle and penColor\n");
      #endif
      
      for (UnsignedInt i = 0; i < curveNames.size(); ++i)
      {
         #if DEBUG_OwnedPlot_INIT
            MessageInterface::ShowMessage("OwnedPlot::Initialize() "
                  "curveTitle = %s\n", curveNames[i].c_str());
         #endif
         
         PlotInterface::AddXyPlotCurve(instanceName, i, curveNames[i], curveColor[i]);

         #ifdef DEBUG_ERROR_BARS
            MessageInterface::ShowMessage("In OwnedPlot::Initialize, "
                  "curveUseHiLow[%d] = %s\n", i,
                  (curveUseHiLow[i] == true ? "true" : "false"));
         #endif

         PlotInterface::XyPlotCurveSettings(instanceName, curveUseLines[i],
               curveLineWidth[i], curveLineStyle[i], curveUseMarkers[i],
               curveMarkerSize[i], curveMarker[i], curveUseHiLow[i], i);
      }
      
      PlotInterface::ShowXyPlotLegend(instanceName);
      status = true;
      
      #if DEBUG_OwnedPlot_INIT
         MessageInterface::ShowMessage("OwnedPlot::Initialize() calling "
               "ClearOwnedPlotData()\n");
      #endif
      
      PlotInterface::ClearXyPlotData(instanceName);
   }
   else
   {
      #if DEBUG_OwnedPlot_INIT
      MessageInterface::ShowMessage("OwnedPlot::Initialize() DeleteOwnedPlot()\n");
      #endif
      
      status =  PlotInterface::DeleteXyPlot(instanceName);
   }
   
   #if DEBUG_OwnedPlot_INIT
      MessageInterface::ShowMessage("OwnedPlot::Initialize() leaving "
            "status=%d\n", status);
   #endif
   
   return status;
}

//---------------------------------
// methods inherited from GmatBase
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the OwnedPlot.
 *
 * @return clone of the OwnedPlot.
 */
//------------------------------------------------------------------------------
GmatBase* OwnedPlot::Clone() const
{
   return (new OwnedPlot(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void OwnedPlot::Copy(const GmatBase* orig)
{
   operator=(*((OwnedPlot *)(orig)));
}


//------------------------------------------------------------------------------
// bool SetName(const std::string &who, const std;:string &oldName)
//------------------------------------------------------------------------------
/**
 * Set the name for this instance.
 *
 * @param who The new name
 * @param oldName The previous name
 *
 * @return true on success, false if initialization failed
 */
//------------------------------------------------------------------------------
bool OwnedPlot::SetName(const std::string &who, const std::string &oldName)
{
   #if DEBUG_RENAME
      MessageInterface::ShowMessage("OwnedPlot::SetName() newName=%s\n",
            who.c_str());
   #endif
   
   if (oldName == "")
      mOldName = instanceName;
   else
      mOldName = oldName;
   
   return Subscriber::SetName(who);
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,  
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 *
 * @return true if action successfully performed
 */
//------------------------------------------------------------------------------
bool OwnedPlot::TakeAction(const std::string &action,
                        const std::string &actionData)
{
   #if DEBUG_ACTION_REMOVE
      MessageInterface::ShowMessage("OwnedPlot::TakeAction() action=%s, "
            "actionData=%s\n", action.c_str(), actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      return ClearYParameters();
   }
   else if (action == "Remove")
   {
      return RemoveYParameter(actionData);
   }
   else if (action == "ClearData")
   {
      return ResetYParameters();
   }
   else if (action == "PenUp")
   {
      return PenUp();
   }
   else if (action == "PenDown")
   {
      return PenDown();
   }
   else if (action == "Rescale")
   {
      return RescaleData();
   }
   
   return Subscriber::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string &oldName,
//       const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames reference objects used by this plot
 *
 * @param type The type of the object getting renamed
 * @param oldName The old object name
 * @param newName The new name of the object
 *
 * @return true if no issue was detected for the rename
 */
//------------------------------------------------------------------------------
bool OwnedPlot::RenameRefObject(const UnsignedInt type,
      const std::string &oldName, const std::string &newName)
{
   #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("OwnedPlot::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
          GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::PARAMETER && type != Gmat::COORDINATE_SYSTEM &&
       type != Gmat::SPACECRAFT)
      return true;
   
   if (type == Gmat::PARAMETER)
   {
      // Y parameters
      for (UnsignedInt i = 0; i < curveNames.size(); ++i)
         if (curveNames[i] == oldName)
            curveNames[i] = newName;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the script string used for the parameter with the input ID
 *
 * @param id The ID of the parameter
 *
 * @return The parameter's string used in GMAT scripts
 */
//------------------------------------------------------------------------------
std::string OwnedPlot::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < OwnedPlotParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   else
      return Subscriber::GetParameterText(id);
    
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a scriptable string
 *
 * @param str The string that maps to an ID
 *
 * @return The ID matching that string
 */
//------------------------------------------------------------------------------
Integer OwnedPlot::GetParameterID(const std::string &str) const
{
   for (int i=GmatBaseParamCount; i<OwnedPlotParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return Subscriber::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type for a scriptable parameter
 *
 * @param id The ID of the parameter
 *
 * @return The type of the parameter
 */
//------------------------------------------------------------------------------
Gmat::ParameterType OwnedPlot::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < OwnedPlotParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else
      return Subscriber::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string describing the parameter type
 *
 * @param id The ID of the parameter
 *
 * @return The string description
 */
//------------------------------------------------------------------------------
std::string OwnedPlot::GetParameterTypeString(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < OwnedPlotParamCount)
      return Subscriber::PARAM_TYPE_STRING[GetParameterType(id -
            GmatBaseParamCount)];
   else
      return Subscriber::GetParameterTypeString(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param id Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool OwnedPlot::IsParameterReadOnly(const Integer id) const
{
   if (
       (id == PLOT_TITLE)             ||
       (id == X_AXIS_TITLE)           ||
       (id == Y_AXIS_TITLE)           ||
       (id == DATA_COLLECT_FREQUENCY) ||
       (id == UPDATE_PLOT_FREQUENCY)  ||
       (id == USE_LINES)              ||
       (id == LINE_WIDTH)             ||
       (id == USE_MARKERS)            ||
       (id == MARKER_SIZE)
      )
      return true;
   
   return Subscriber::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves an integer parameter
 *
 * @param id The ID of the desired parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Integer OwnedPlot::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
   case DATA_COLLECT_FREQUENCY:
      return mDataCollectFrequency;

   case UPDATE_PLOT_FREQUENCY:
      return mUpdatePlotFrequency;

   /// @todo: add code to index into the array
   case DEFAULT_COLOR:
      return defaultColor[0];

   case LINE_WIDTH:
      return lineWidth;

   case LINE_STYLE:
      return lineStyle;

   case MARKER_SIZE:
      return markerSize;

   case MARKER_STYLE:
      return markerStyle;

   default:
      return Subscriber::GetIntegerParameter(id);
   }
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves an integer parameter
 *
 * @param label The script name of the desired parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Integer OwnedPlot::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets an integer parameter
 *
 * @param id The ID of the parameter
 * @param value The new parameter value
 *
 * @return The parameter value after the assignment
 */
//------------------------------------------------------------------------------
Integer OwnedPlot::SetIntegerParameter(const Integer id, const Integer value)
{
   switch (id)
   {
      case DATA_COLLECT_FREQUENCY:
         mDataCollectFrequency = value;
         return mDataCollectFrequency;

      case UPDATE_PLOT_FREQUENCY:
         mUpdatePlotFrequency = value;
         return mUpdatePlotFrequency;

      case DEFAULT_COLOR:
         /// @todo: add code to index into the array
         defaultColor[0] = value;
         return defaultColor[0];

      case LINE_WIDTH:
         lineWidth = value;
         return lineWidth;

      case LINE_STYLE:
         lineStyle = value;
         return lineStyle;

      case MARKER_SIZE:
         markerSize = value;
         return markerSize;

      case MARKER_STYLE:
         markerStyle = value;
         return markerStyle;

      default:
         return Subscriber::SetIntegerParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets an integer parameter
 *
 * @param label The script string used for the parameter
 * @param value The new parameter value
 *
 * @return The parameter value after the assignment
 */
//------------------------------------------------------------------------------
Integer OwnedPlot::SetIntegerParameter(const std::string &label,
                                    const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetOnOffParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a boolean parameter scripted using the words On and Off
 *
 * @param id The ID for the parameter
 *
 * @return On if the parameter is true, Off if it is false
 */
//------------------------------------------------------------------------------
std::string OwnedPlot::GetOnOffParameter(const Integer id) const
{
   switch (id)
   {
      case DRAW_GRID:
         return mDrawGrid;

      default:
         return Subscriber::GetOnOffParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetOnOffParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a boolean parameter scripted using the words On and Off
 *
 * @param label The script string for the parameter
 *
 * @return On if the parameter is true, Off if it is false
 */
//------------------------------------------------------------------------------
std::string OwnedPlot::GetOnOffParameter(const std::string &label) const
{
   return GetOnOffParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetOnOffParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets a boolean parameter scripted using the words On and Off
 *
 * @param id The ID for the parameter
 * @param value The new setting for the parameter
 *
 * @return true on success, false if the parameter could not be set
 */
//------------------------------------------------------------------------------
bool OwnedPlot::SetOnOffParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
      case DRAW_GRID:
         mDrawGrid = value;
         return true;

      default:
         return Subscriber::SetOnOffParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool OwnedPlot::SetOnOffParameter(const std::string &label,
//       const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets a boolean parameter scripted using the words On and Off
 *
 * @param label The script string for the parameter
 * @param value The new setting for the parameter
 *
 * @return true on success, false if the parameter could not be set
 */
//------------------------------------------------------------------------------
bool OwnedPlot::SetOnOffParameter(const std::string &label,
      const std::string &value)
{
   return SetOnOffParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter by ID
 *
 * @param id The parameter's ID
 *
 * @return The parameter
 */
//------------------------------------------------------------------------------
std::string OwnedPlot::GetStringParameter(const Integer id) const
{
   switch (id)
   {
      case PLOT_TITLE:
         return mPlotTitle;

      case X_AXIS_TITLE:
         return mXAxisTitle;

      case Y_AXIS_TITLE:
         return mYAxisTitle;

      default:
         return Subscriber::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter by script string
 *
 * @param label The parameter's scriptable string
 *
 * @return The parameter
 */
//------------------------------------------------------------------------------
std::string OwnedPlot::GetStringParameter(const std::string &label) const
{
   #if DEBUG_XY_PARAM
      MessageInterface::ShowMessage("OwnedPlot::GetStringParameter() "
            "label = %s\n", label.c_str());
   #endif
   
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter
 *
 * @param id The ID of the parameter
 * @param value The new parameter value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool OwnedPlot::SetStringParameter(const Integer id, const std::string &value)
{
   #if DEBUG_OwnedPlot_PARAM
      MessageInterface::ShowMessage("OwnedPlot::SetStringParameter() id = %d, "
                                    "value = %s \n", id, value.c_str());
   #endif
   
   switch (id)
   {
      case ADD:
         if (find(curveNames.begin(), curveNames.end(), value) ==
               curveNames.end())
         {
            curveNames.push_back(value);
            curveColor.push_back(defaultColor[curveColor.size() % defaultColor.size()]);
            curveLineWidth.push_back(lineWidth);
            curveLineStyle.push_back(lineStyle);
            if (markerStyle == -1)
               curveMarker.push_back(curveNames.size() % 10);
            else
               curveMarker.push_back(markerStyle);
            curveMarkerSize.push_back(markerSize);
            curveUseLines.push_back(useLines);
            curveUseMarkers.push_back(useMarkers);
            curveUseHiLow.push_back(useHiLow);
         }
         return true;

      case PLOT_TITLE:
         mPlotTitle = value;
         return true;

      case X_AXIS_TITLE:
         mXAxisTitle = value;
         return true;

      case Y_AXIS_TITLE:
         mYAxisTitle = value;
         return true;

      default:
         return Subscriber::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter
 *
 * @param label The script string of the parameter
 * @param value The new parameter value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool OwnedPlot::SetStringParameter(const std::string &label,
                                const std::string &value)
{
   #if DEBUG_OwnedPlot_PARAM
      MessageInterface::ShowMessage("OwnedPlot::SetStringParameter() "
            "label = %s, value = %s \n", label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in a StringArray
 *
 * @param id The ID of the parameter
 * @param value The new parameter value
 * @param index Location of the parameter in the array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool OwnedPlot::SetStringParameter(const Integer id, const std::string &value,
                                const Integer index)
{
   switch (id)
   {
      case ADD:
         if ((index < 0) || (index > (Integer)curveNames.size()))
            return false;
         if (index < (Integer)curveNames.size())
         {
            curveNames[index] = value;
         }
         else
         {
            curveNames.push_back(value);
            curveColor.push_back(defaultColor[curveColor.size() % defaultColor.size()]);
            curveLineWidth.push_back(lineWidth);
            curveLineStyle.push_back(lineStyle);
            if (markerStyle == -1)
               curveMarker.push_back(curveNames.size() % 10);
            else
               curveMarker.push_back(markerStyle);
            curveMarkerSize.push_back(markerSize);
            curveUseLines.push_back(useLines);
            curveUseMarkers.push_back(useMarkers);
            curveUseHiLow.push_back(useHiLow);
         }
         return true;

      default:
         return Subscriber::SetStringParameter(id, value, index);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in a StringArray
 *
 * @param label The script string of the parameter
 * @param value The new parameter value
 * @param index Location of the parameter in the array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool OwnedPlot::SetStringParameter(const std::string &label,
                                const std::string &value,
                                const Integer index)
{
   #if DEBUG_OwnedPlot_PARAM
      MessageInterface::ShowMessage
         ("OwnedPlot::SetStringParameter() label=%s, value=%s, index=%d \n",
          label.c_str(), value.c_str(), index);
   #endif
   
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter
 *
 * @param id The ID of the parameter
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& OwnedPlot::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
      case ADD:
         return curveNames;
      default:
         return Subscriber::GetStringArrayParameter(id);
   }
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter
 *
 * @param label The script string of the parameter
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& OwnedPlot::GetStringArrayParameter(
      const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Boolean parameter
 *
 * @param id The ID of the parameter
 *
 * @return The parameter value -- true or false
 */
//------------------------------------------------------------------------------
bool OwnedPlot::GetBooleanParameter(const Integer id) const
{
   if (id == SHOW_PLOT)
      return active;

   if (id == SHOW_LEGEND)
      return showLegend;

   if (id == USE_LINES)
      return useLines;

   if (id == USE_MARKERS)
      return useMarkers;

   if (id == USE_HI_LOW)
      return useHiLow;

   return Subscriber::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Boolean parameter
 *
 * @param label The script string of the parameter
 *
 * @return The parameter value -- true or false
 */
//------------------------------------------------------------------------------
bool OwnedPlot::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets a Boolean parameter
 *
 * @param label The script string of the parameter
 * @param value The new value for the parameter
 *
 * @return The parameter value -- true or false
 */
//------------------------------------------------------------------------------
bool OwnedPlot::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets a Boolean parameter
 *
 * @param id The ID of the parameter
 * @param value The new value for the parameter
 *
 * @return The parameter value -- true or false
 */
//------------------------------------------------------------------------------
bool OwnedPlot::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == SHOW_PLOT)
   {
      active = value;
      return active;
   }

   if (id == SHOW_LEGEND)
   {
      showLegend = value;
      return showLegend;
   }

   if (id == USE_LINES)
   {
      useLines = value;
      if (useLines == false)
         useMarkers = true;
      return useLines;
   }

   if (id == USE_MARKERS)
   {
      useMarkers = value;
      // Always have to have either markers or lines
      if (useMarkers == false)
         useLines = true;
      return useMarkers;
   }

   if (id == USE_HI_LOW)
   {
      useHiLow = value;
      return useHiLow;
   }

   return Subscriber::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// bool Activate()
//------------------------------------------------------------------------------
/**
 * Turns on the plot component on the GUI, so that it refreshes and processes
 * data as it is received.
 *
 * This method helps reduce processing overhead when a large block of data is
 * available for plotting
 *
 * @return true
 */
//------------------------------------------------------------------------------
bool OwnedPlot::Activate(bool)
{
   PlotInterface::ActivateXyPlot(instanceName);
   return true;
}


//------------------------------------------------------------------------------
// bool Deactivate()
//------------------------------------------------------------------------------
/**
 * Turns off the plot component on the GUI, so that it receives and stores data,
 * but does not refresh or process that data until the plot is activated.
 *
 * This method helps reduce processing overhead when a large block of data is
 * available for plotting
 *
 * @return true
 */
//------------------------------------------------------------------------------
bool OwnedPlot::Deactivate()
{
   PlotInterface::DeactivateXyPlot(instanceName);
   return true;
}


//------------------------------------------------------------------------------
// void SetData(std::vector<RealArray*> &dataBlast, RealArray hiErrors,
//       RealArray lowErrors)
//------------------------------------------------------------------------------
/**
 * Sends a large block of data to the plot component
 *
 * The dataBlast array is a vector or RealArray pointers.  The first RealArray
 * pointer points to the independent (X-axis) data, the subsequent pointers
 * point to dependent (Y-axis) data that corresponds to the dependent data
 * values.
 *
 * Error bars can be displayed if the receiving plot component supports them.
 * The error data is passed in in the hiErrors and lowErrors arrays.  These
 * arrays contain the + and - error on the reported value.  If lowErrors is
 * empty but data exists in hiErrors, hiErrors is treated as +/- error.
 *
 * @param dataBlast The data (x and y values) that is sent to the plot
 * @param hiErrors (Optional) The +sigma errors used for error bars
 * @param lowErrors (Optional) The -sigma errors used for error bars
 */
//------------------------------------------------------------------------------
void OwnedPlot::SetData(std::vector<RealArray*> &dataBlast,
      RealArray hiErrors, RealArray lowErrors)
{
   #if DEBUG_OwnedPlot_UPDATE > 1
      MessageInterface::ShowMessage
         ("OwnedPlot::SetData() entered. isEndOfReceive=%d, active=%d, "
               "runState=%d\n", isEndOfReceive, active, runstate);
   #endif

   RealArray *xData = dataBlast[0];

   #if DEBUG_OwnedPlot_UPDATE > 1
      RealArray *yData = dataBlast[1];

      MessageInterface::ShowMessage("   Sample points: 0->[%le %le] "
            "5->[%le %le]\n", (*xData)[0], (*yData)[0], (*xData)[5],
            (*yData)[5]);
   #endif

   Real xval;
   Rvector yvals = Rvector(curveNames.size());
   Rvector his = Rvector(curveNames.size());
   Rvector lows = Rvector(curveNames.size());

   for (UnsignedInt i = 0; i < xData->size(); ++i)
   {
      xval = (*xData)[i];
      for (UnsignedInt j = 0; j < curveNames.size(); ++j)
      {
         yvals[j] = (*(dataBlast[j+1]))[i];
         if (hiErrors.size() > i)
            his[j] = hiErrors[i];
         if (lowErrors.size() > i)
            lows[j] = lowErrors[i];
         else
            lows[j] = his[j];
      }

      PlotInterface::UpdateXyPlotData(instanceName, xval, yvals, his,
            lows);
   }
}


//------------------------------------------------------------------------------
// void SetCurveData(const Integer forCurve, RealArray *xData, RealArray *yData,
//       const RealArray *yhis, const RealArray *ylows)
//------------------------------------------------------------------------------
/**
 * Adds data to a single curve on a plot
 *
 * @param forCurve The index of the curve
 * @param xData The array of independent data for the curve
 * @param yData The array of dependent data for the curve
 * @param yhis The optional "+ sigma" data for error bars
 * @param ylows The optional "- sigma" data for error bars
 */
//------------------------------------------------------------------------------
void OwnedPlot::SetCurveData(const Integer forCurve, RealArray *xData,
           RealArray *yData, const RealArray *yhis, const RealArray *ylows)
{
   #if DEBUG_OwnedPlot_UPDATE > 1
      MessageInterface::ShowMessage
         ("OwnedPlot::SetData() entered. isEndOfReceive=%d, active=%d, "
          "runState=%d\n", isEndOfReceive, active, runstate);
   #endif

   Real hi, low;

   for (UnsignedInt i = 0; i < xData->size(); ++i)
   {
      hi = low = 0.0;
      if (yhis)
      {
         if (yhis->size() > i)
         {
            hi = (*yhis)[i];
            if ((ylows) && (ylows->size() > i))
               low = (*ylows)[i];
            else
               low = hi;
         }
      }

      PlotInterface::UpdateXyPlotCurve(instanceName, forCurve, (*xData)[i],
            (*yData)[i], hi, low);
   }
}


//------------------------------------------------------------------------------
// bool MarkPoint(Integer whichOne, Integer forCurve)
//------------------------------------------------------------------------------
/**
 * Passes a point marker in to the plot
 *
 * @param whichOne Index of the point that gets marked; -1 indicates the next
 *                 point that the curve receives
 * @param forCurve The index of the curve that contains the marked point
 *
 * @return true if the mark was sent to the curve, false if not
 *
 * @todo: This method is not yet implemented
 */
//------------------------------------------------------------------------------
bool OwnedPlot::MarkPoint(Integer whichOne, Integer forCurve)
{
   bool retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// Integer SetUsedDataID(Integer id, Integer forCurve)
//------------------------------------------------------------------------------
/**
 * Passes in the ID of the data source used by a given curve.
 *
 * @param id The data source ID
 * @param forCurve The curve that uses that data source
 *
 * @return The ID of the curve that supports the data.  If forCurve == -1 or if
 *         it is the number of currently defined curves, the ID is applied to
 *         the next curve defined for the plot and the return value is the index
 *         that curve will receive.  Out-of-bounds cases -- that is, those not
 *         covered here -- will generate a return value of -1, and the ID will
 *         not be stored.
 */
//------------------------------------------------------------------------------
Integer OwnedPlot::SetUsedDataID(Integer id, Integer forCurve)
{
   if (forCurve < -1)
      return -1;

   Integer curveId = -1;

   if (forCurve == -1)
   {
      curveId = (int)supportedData.size();
      supportedData.push_back(id);
   }
   else
   {
      if (forCurve < (int)supportedData.size())
      {
         supportedData[forCurve] = id;
         curveId = forCurve;
      }
      else if (forCurve == (int)supportedData.size())
      {
         curveId = (int)supportedData.size();
         supportedData.push_back(id);
      }
   }

   return curveId;
}

//------------------------------------------------------------------------------
// void SetUsedObjectID(Integer id)
//------------------------------------------------------------------------------
/**
 * Passes in the ID of the objects used by the plot.
 *
 * @param id The object ID
 */
//------------------------------------------------------------------------------
void OwnedPlot::SetUsedObjectID(Integer id)
{
   bool alreadyThere = false;

   for (UnsignedInt i = 0; i < supportedObjects.size(); ++i)
      if (supportedObjects[i] == id)
      {
         alreadyThere = true;
         break;
      }

   if (!alreadyThere)
      supportedObjects.push_back(id);
}

//------------------------------------------------------------------------------
// Integer UsesData(Integer id)
//------------------------------------------------------------------------------
/**
 * Checks an OwnedPlot to see if a specific data source is used
 *
 * @param id The data source's ID
 *
 * @return the index of the curve plotting that data source
 */
//------------------------------------------------------------------------------
Integer OwnedPlot::UsesData(Integer id)
{
   Integer retval = -1;

   for (UnsignedInt i = 0; i < supportedData.size(); ++i)
      if (supportedData[i] == id)
      {
         retval = i;
         break;
      }

   return retval;
}

//------------------------------------------------------------------------------
// Integer UsesObject(Integer id)
//------------------------------------------------------------------------------
/**
 * Checks an OwnedPlot to see if a specific object is used
 *
 * @param id The object's ID
 *
 * @return The index of the object in the object list
 */
//------------------------------------------------------------------------------
Integer OwnedPlot::UsesObject(Integer id)
{
   Integer retval = -1;

   for (UnsignedInt i = 0; i < supportedObjects.size(); ++i)
      if (supportedObjects[i] == id)
      {
         retval = i;
         break;
      }

   return retval;
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// void BuildPlotTitle()
//------------------------------------------------------------------------------
/**
 * Sets the plot title and axis titles to default values if no values have been
 * set.
 */
//------------------------------------------------------------------------------
void OwnedPlot::BuildPlotTitle()
{
   if (mXAxisTitle == "")
      mXAxisTitle = "Epoch";
   if (mYAxisTitle == "")
      mYAxisTitle = "Residual";
   if (mPlotTitle == "")
      mPlotTitle  = "Residual data";
}

//------------------------------------------------------------------------------
// bool ClearYParameters()
//------------------------------------------------------------------------------
/**
 * Clears the curve data and prepares teh plot for a new set of curves
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool OwnedPlot::ClearYParameters()
{
   DeletePlotCurves();
   curveNames.clear();
   // mPlotTitle = "";
   // mXAxisTitle = "";
   // mYAxisTitle = "";
   mIsOwnedPlotWindowSet = false;
   return true;
}

//------------------------------------------------------------------------------
// bool RemoveYParameter(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Removes curve from the curve list
 *
 * @param <name> parameter name to be removed from the Y parameter list
 *
 * @return true if parameter was removed from the Y parameter list, false
 *         otherwise
 *
 */
//------------------------------------------------------------------------------
bool OwnedPlot::RemoveYParameter(const std::string &name)
{
   #if DEBUG_ACTION_REMOVE
      MessageInterface::ShowMessage
         ("OwnedPlot::RemoveYParameter() name=%s\n--- Before remove:\n",
               name.c_str());
      for (int i=0; i<mNumYParams; i++)
      {
         MessageInterface::ShowMessage("mYParamNames[%d]=%s\n", i,
                                       mYParamNames[i].c_str());
      }
   #endif

   StringArray::iterator pos1;

   for (pos1 = curveNames.begin(); pos1 != curveNames.end(); ++pos1)
   {
      if (*pos1 == name)
      {
         curveNames.erase(pos1);

         #if DEBUG_ACTION_REMOVE
            MessageInterface::ShowMessage("---After remove\n");
            for (UnsignedInt i = 0; i < curveNames.size(); ++i)
            {
               MessageInterface::ShowMessage("curveNames[%d] = %s\n", i,
                     curveNames[i].c_str());
            }
         #endif

         return true;
      }
   }

   #if DEBUG_ACTION_REMOVE
      MessageInterface::ShowMessage("OwnedPlot::RemoveYParameter() name = %s "
            "not found\n", name.c_str());
   #endif

   return false;
}


//------------------------------------------------------------------------------
// bool ResetYParameters()
//------------------------------------------------------------------------------
/**
 * Clears the plot data
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool OwnedPlot::ResetYParameters()
{
   PlotInterface::ClearXyPlotData(instanceName);
   return true;
}

//------------------------------------------------------------------------------
// bool PenUp()
//------------------------------------------------------------------------------
/**
 * Stops writing to the plot
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool OwnedPlot::PenUp()
{
   PlotInterface::XyPlotPenUp(instanceName);
   return true;
}

//------------------------------------------------------------------------------
// bool PenDown()
//------------------------------------------------------------------------------
/**
 * Resumes writing on the plot
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool OwnedPlot::PenDown()
{
   PlotInterface::XyPlotPenDown(instanceName);
   return true;
}


//------------------------------------------------------------------------------
// bool RescaleData()
//------------------------------------------------------------------------------
/**
 * Resets the plot scales to match the current curve data
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool OwnedPlot::RescaleData()
{
   PlotInterface::XyPlotRescale(instanceName);
   return true;
}


//------------------------------------------------------------------------------
// void OwnedPlot::DeletePlotCurves()
//------------------------------------------------------------------------------
/**
 * Deletes all of the PlotCurves
 */
//------------------------------------------------------------------------------
void OwnedPlot::DeletePlotCurves()
{
   // delete existing curves
   PlotInterface::DeleteAllXyPlotCurves(instanceName, mOldName);
}
