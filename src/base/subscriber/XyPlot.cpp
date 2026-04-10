//$Id$
//------------------------------------------------------------------------------
//                                  XyPlot
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
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/22
//
/**
 * Implements XyPlot class.
 */
//------------------------------------------------------------------------------

#include "XyPlot.hpp"
#include "PlotInterface.hpp"         // for XY plot
#include "SubscriberException.hpp"
#include "StringUtil.hpp"            // for GmatStringUtil::ReplaceName(), GetArrayName()
#include "MessageInterface.hpp"      // for ShowMessage()
#include <sstream>                   // for <<

//#define DEBUG_XYPLOT_INIT 1
//#define DEBUG_XYPLOT_PARAM 1
//#define DEBUG_XYPLOT_OBJECT 1 
//#define DEBUG_XYPLOT_UPDATE 2
//#define DEBUG_ACTION 1
//#define DEBUG_RENAME 1
//#define DEBUG_WRAPPER_CODE

//---------------------------------
// static data
//---------------------------------

const std::string
XyPlot::PARAMETER_TEXT[XyPlotParamCount - SubscriberParamCount] =
{
   "XVariable",
   "YVariables",
//   "PlotTitle",
//   "XAxisTitle",
//   "YAxisTitle",
   "ShowGrid",
   "DataCollectFrequency",
   "UpdatePlotFrequency",
   "ShowPlot",
//   "UseLines",
//   "LineWidth",
//   "UseMarkers",
//   "MarkerSize",
   "Drawing",
   "IndVar",
   "Add",
   "Grid",
}; 

const Gmat::ParameterType
XyPlot::PARAMETER_TYPE[XyPlotParamCount - SubscriberParamCount] =
{
   Gmat::OBJECT_TYPE,      // "IndVar","XVariable"
   Gmat::OBJECTARRAY_TYPE, // "Add","YVariables"
//   Gmat::STRING_TYPE,      // "PlotTitle",
//   Gmat::STRING_TYPE,      // "XAxisTitle",
//   Gmat::STRING_TYPE,      // "YAxisTitle",
   Gmat::BOOLEAN_TYPE,     // "ShowGrid"
   Gmat::INTEGER_TYPE,     // "DataCollectFrequency",
   Gmat::INTEGER_TYPE,     // "UpdatePlotFrequency",
   Gmat::BOOLEAN_TYPE,     // "ShowPlot",
//   Gmat::BOOLEAN_TYPE,     // "UseLines",
//   Gmat::INTEGER_TYPE,     // "LineWidth",
//   Gmat::BOOLEAN_TYPE,     // "UseMarkers",
//   Gmat::INTEGER_TYPE,     // "MarkerSize",
   Gmat::BOOLEAN_TYPE,     // "Drawing"
   Gmat::OBJECT_TYPE,      // "IndVar","XVariable"
   Gmat::OBJECTARRAY_TYPE, // "Add","YVariables"
   Gmat::ON_OFF_TYPE,      // "Grid","ShowGrid"
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// XyPlot(const std::string &name, Parameter *xParam,
//        Parameter *firstYParam, const std::string &plotTitle,
//        const std::string &xAxisTitle, const std::string &yAxisTitle,
//        bool drawGrid)
//------------------------------------------------------------------------------
XyPlot::XyPlot(const std::string &name, Parameter *xParam,
               Parameter *firstYParam, const std::string &plotTitle,
               const std::string &xAxisTitle, const std::string &yAxisTitle) :
   Subscriber("XYPlot", name),
   mNumCollected(0),  
   mUpdatePlotFrequency(50)
{
   // GmatBase data
   objectTypes.push_back(Gmat::XY_PLOT);
   objectTypeNames.push_back("XYPlot");
   objectTypeNames.push_back("Plot");
   parameterCount = XyPlotParamCount;
   
   mDrawGrid = true;
   mNumYParams = 0;
   
   mXParamName = "";
   mNumXParams = 0;
   
   mXParam = xParam;
   if (firstYParam != NULL)
      AddYParameter(firstYParam->GetName(), mNumYParams);
   
   mOldName = instanceName;
   mPlotTitle = plotTitle;
   mXAxisTitle = xAxisTitle;
   mYAxisTitle = yAxisTitle;
   
   mIsXyPlotWindowSet = false;
   mDataCollectFrequency = 1;
   mUpdatePlotFrequency = 50;
   mNumDataPoints = 0;           // Found by Bob Wiegand w/ Valgrind

   useLines = true;
   lineWidth = 1;
   useMarkers = false;
   markerSize = 3;
   drawing = true;
   breakCount = 0;
}


//------------------------------------------------------------------------------
// XyPlot(const XyPlot &orig)
//------------------------------------------------------------------------------
XyPlot::XyPlot(const XyPlot &orig) :
   Subscriber(orig)
{
   mXParam = orig.mXParam;
   mYParams = orig.mYParams;
   
   mNumXParams = orig.mNumXParams;
   mNumYParams = orig.mNumYParams;
   
   mXParamName = orig.mXParamName;
   mYParamNames = orig.mYParamNames;
   mAllParamNames = orig.mAllParamNames;
   
   mOldName = orig.mOldName;
   mPlotTitle = orig.mPlotTitle;
   mXAxisTitle = orig.mXAxisTitle;
   mYAxisTitle = orig.mYAxisTitle;
   mDrawGrid = orig.mDrawGrid;
   mIsXyPlotWindowSet = orig.mIsXyPlotWindowSet;
   
   mDataCollectFrequency = orig.mDataCollectFrequency;
   mUpdatePlotFrequency = orig.mUpdatePlotFrequency;
   
   mNumDataPoints = orig.mNumDataPoints;
   mNumCollected = orig.mNumCollected;

   useLines   = orig.useLines;
   lineWidth  = orig.lineWidth;
   useMarkers = orig.useMarkers;
   markerSize = orig.markerSize;
   drawing    = orig.drawing;
   breakCount = orig.breakCount;
}


//------------------------------------------------------------------------------
// XyPlot& operator=(const XyPlot& orig)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
XyPlot& XyPlot::operator=(const XyPlot& orig)
{
   if (this == &orig)
      return *this;
   
   Subscriber::operator=(orig);
   
   mXParam = orig.mXParam;
   mYParams = orig.mYParams;
   
   mNumXParams = orig.mNumXParams;
   mNumYParams = orig.mNumYParams;
   
   mXParamName = orig.mXParamName;
   mYParamNames = orig.mYParamNames;
   mAllParamNames = orig.mAllParamNames;
   
   mOldName = orig.mOldName;
   mPlotTitle = orig.mPlotTitle;
   mXAxisTitle = orig.mXAxisTitle;
   mYAxisTitle = orig.mYAxisTitle;
   mDrawGrid = orig.mDrawGrid;
   mIsXyPlotWindowSet = orig.mIsXyPlotWindowSet;
   
   mDataCollectFrequency = orig.mDataCollectFrequency;
   mUpdatePlotFrequency = orig.mUpdatePlotFrequency;
   
   mNumDataPoints = orig.mNumDataPoints;
   mNumCollected = orig.mNumCollected;
   
   useLines   = orig.useLines;
   lineWidth  = orig.lineWidth;
   useMarkers = orig.useMarkers;
   markerSize = orig.markerSize;
   drawing    = orig.drawing;
   breakCount = orig.breakCount;
   
   return *this;
}


//------------------------------------------------------------------------------
// ~XyPlot(void)
//------------------------------------------------------------------------------
XyPlot::~XyPlot()
{
}

//------------------------------------------------------------------------------
// bool SetXParameter(const std::string &paramName)
//------------------------------------------------------------------------------
/**
 * Sets X variable name.
 *
 * @param  paramName  The name of X variable
 *
 * @return  true if X parameter name was set, false when input name is blank
 */
//------------------------------------------------------------------------------
bool XyPlot::SetXParameter(const std::string &paramName)
{
   #if DEBUG_XYPLOT_PARAM
   MessageInterface::ShowMessage
      ("XyPlot::SetXParameter() entered, paramName='%s'\n", paramName.c_str());
   #endif
   
   if (paramName != "")
   {
      mXParamName = paramName;
      mNumXParams = 1; //loj: only 1 X parameter for now
      if (xParamWrappers.size() == 0)
         xParamWrappers.push_back(NULL);
      
      #if DEBUG_XYPLOT_PARAM
      MessageInterface::ShowMessage("XyPlot::SetXParameter() returning true\n");
      #endif
      
      return true;
   }
   
   #if DEBUG_XYPLOT_PARAM
   MessageInterface::ShowMessage("XyPlot::SetXParameter() returning false\n");
   #endif
   return false;
}


//------------------------------------------------------------------------------
// bool AddYParameter(const std::string &paramName, Integer index)
//------------------------------------------------------------------------------
bool XyPlot::AddYParameter(const std::string &paramName, Integer index)
{
   #if DEBUG_XYPLOT_PARAM
   MessageInterface::ShowMessage
      ("XyPlot::AddYParameter() entered, name = '%s', index = %d\n",
       paramName.c_str(), index);
   #endif
   
   if (paramName != "" && index == mNumYParams)
   {
      // if paramName not found, add (based on loj fix to XYPlot)
      if (find(mYParamNames.begin(), mYParamNames.end(), paramName) ==
          mYParamNames.end())
      {
         mYParamNames.push_back(paramName);
         mNumYParams = mYParamNames.size();
         mYParams.push_back(NULL);
         yParamWrappers.push_back(NULL);
         
         #if DEBUG_XYPLOT_PARAM
         MessageInterface::ShowMessage("XyPlot::AddYParameter() returning true\n");
         #endif
         
         return true;
      }
   }
   
   #if DEBUG_XYPLOT_PARAM
   MessageInterface::ShowMessage("XyPlot::AddYParameter() returning false\n");
   #endif
   
   return false;
}

//----------------------------------
// methods inherited from Subscriber
//----------------------------------

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool XyPlot::Initialize()
{
   //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING_NO_PLOTS)         // made changes by TUAN NGUYEN
   if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING_NO_PLOTS)    // made changes by TUAN NGUYEN
      return true;
   
   #if DEBUG_XYPLOT_INIT
   MessageInterface::ShowMessage
      ("\nXyPlot::Initialize() <%p>'%s' entered, active=%d, mNumYParams=%d, "
       "mNumDataPoints=%d\n", this, GetName().c_str(), active,
       mNumYParams, mNumDataPoints);
   MessageInterface::ShowMessage
      ("   IsGlobal=%d, mIsXyPlotWindowSet=%d\n", IsGlobal(), mIsXyPlotWindowSet);
   #endif
   
   // Check if there are parameters selected for XyPlot
   if (active)
   {
      if (mNumXParams == 0 || mNumYParams == 0)
      {
         active = false;
         MessageInterface::ShowMessage
            ("*** WARNING *** The XYPlot named \"%s\" will not be shown.\n"
             "No parameters were selected for X Axis or Y Axis.\n",
             GetName().c_str());
         return false;
      }
      
      // Now using wrappers (LOJ: 2012.08.02)
      //if (mXParam == NULL || mYParams[0] == NULL)
      //if (xParamWrappers[0] == NULL || yParamWrappers[0] == NULL)
      if (xParamWrappers.empty() || yParamWrappers.empty())
      {
         active = false;
         MessageInterface::ShowMessage
            ("*** WARNING *** The XYPlot named \"%s\" will not be shown.\n"
             "The first parameter selected for X Axis or Y Axis is NULL\n",
             GetName().c_str());
         return false;
      }
   }
   
   Subscriber::Initialize();
   
   bool status = false;
   
   if (IsGlobal())
   {
      status = true;
      
      #if DEBUG_XYPLOT_INIT
      MessageInterface::ShowMessage
         ("   <%p>'%s' is global, XyPlotWindow already exist\n", this, GetName().c_str());
      #endif
   }
   else
   {
      #if DEBUG_XYPLOT_INIT
      MessageInterface::ShowMessage
         ("   <%p>'%s' calling DeletePlotCurves())\n", this, GetName().c_str());
      #endif
      DeletePlotCurves();
      
      mNumDataPoints = 0;
      
      if (active)
      {
         #if DEBUG_XYPLOT_INIT
         MessageInterface::ShowMessage
            ("   <%p>'%s' is active, building plot title and creating XyPlotWindow)\n",
             this, GetName().c_str());
         #endif
         
         // build plot title
         BuildPlotTitle();
         
         // Create XyPlotWindow, if not exist
         #if DEBUG_XYPLOT_INIT
         MessageInterface::ShowMessage
            ("   <%p>'%s' calling CreateXyPlotWindow()\n", this, GetName().c_str());
         #endif
         
         PlotInterface::CreateXyPlotWindow
            (instanceName, mOldName, mPlotUpperLeft[0], mPlotUpperLeft[1],
             mPlotSize[0], mPlotSize[1], isMaximized, mPlotTitle,
             mXAxisTitle, mYAxisTitle, mDrawGrid);
         
         PlotInterface::SetXyPlotTitle(instanceName, mPlotTitle);
         mIsXyPlotWindowSet = true;
         
         #if DEBUG_XYPLOT_INIT
         MessageInterface::ShowMessage
            ("   <%p>'%s' setting curveTitle and penColor\n", this, GetName().c_str());
         #endif
         
         for (int i=0; i<mNumYParams; i++)
         {
            // Now using wrapper for curve title so that it can display array
            // element such as MyArray(3,3) (Fix for GMT-2370 LOJ: 2012.08.03)
            if (mYParams[i] && yParamWrappers[i])
            {
               std::string curveTitle = yParamWrappers[i]->GetDescription();
               UnsignedInt penColor = mYParams[i]->GetUnsignedIntParameter("Color");
               
               #if DEBUG_XYPLOT_INIT
               MessageInterface::ShowMessage
                  ("   <%p>'%s' curveTitle = %s\n", this, GetName().c_str(),
                   curveTitle.c_str());
               #endif
               
               PlotInterface::AddXyPlotCurve(instanceName, i, curveTitle, penColor);
            }
         }
         
         PlotInterface::ShowXyPlotLegend(instanceName);
         status = true;
         
         #if DEBUG_XYPLOT_INIT
         MessageInterface::ShowMessage("   <%p>'%s' calling ClearXyPlotData()\n", this, GetName().c_str());
         #endif
         
         PlotInterface::ClearXyPlotData(instanceName);
         PlotInterface::XyPlotCurveSettings(instanceName, useLines, lineWidth, 100,
                                            useMarkers, markerSize, -1);
      }
      else
      {
         #if DEBUG_XYPLOT_INIT
         MessageInterface::ShowMessage
            ("   <%p>'%s' is not active, calling PlotInterface::DeleteXyPlot()\n",
             this, GetName().c_str());
         #endif
         
         status =  PlotInterface::DeleteXyPlot(instanceName);
      }
   }
   
   #if DEBUG_XYPLOT_INIT
   MessageInterface::ShowMessage
      ("XyPlot::Initialize() <%p>'%s' leaving, stauts=%d\n", this, GetName().c_str(), status);
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
 * This method returns a clone of the XyPlot.
 *
 * @return clone of the XyPlot.
 *
 */
//------------------------------------------------------------------------------
GmatBase* XyPlot::Clone() const
{
   return (new XyPlot(*this));
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
void XyPlot::Copy(const GmatBase* orig)
{
   operator=(*((XyPlot *)(orig)));
}


//------------------------------------------------------------------------------
// bool SetName(const std::string &who, const std;:string &oldName = "")
//------------------------------------------------------------------------------
/**
 * Set the name for this instance.
 *
 * @see GmatBase
 *
 */
//------------------------------------------------------------------------------
bool XyPlot::SetName(const std::string &who, const std::string &oldName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage("XyPlot::SetName() newName=%s\n", who.c_str());
   #endif
   
   if (oldName == "")
      mOldName = instanceName;
   else
      mOldName = oldName;
   
   return GmatBase::SetName(who);
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
 * @return true if action successfully performed
 *
 */
//------------------------------------------------------------------------------
bool XyPlot::TakeAction(const std::string &action,
                        const std::string &actionData)
{
   #if DEBUG_ACTION
   MessageInterface::ShowMessage
      ("XyPlot::TakeAction() <%p>'%s' entered, action=%s, actionData=%s\n",
       this, GetName().c_str(), action.c_str(), actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      return ClearYParameters();
   }
   else if (action == "Remove")
   {
      return RemoveYParameter(actionData);
   }
   else if (action == "Finalize")
   {
      // This action is usually called when GMAT function finalizes
      return PlotInterface::DeleteXyPlot(instanceName);
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
   else if (action == "MarkPoint")
   {
      return MarkPoint();
   }
   else if (action == "MarkBreak")
   {
      return MarkBreak();
   }
   else if (action == "ClearFromBreak")
   {
      return ClearFromBreak();
   }
   else if (action == "Darken")
   {
      std::stringstream data;
      data << actionData;
      Integer factor;
      data >> factor;
      return Darken(factor);
   }
   else if (action == "Lighten")
   {
      std::stringstream data;
      data << actionData;
      Integer factor;
      data >> factor;
      return Lighten(factor);
   }
   // Add color change and marker change here(?)
   
   #if DEBUG_ACTION
   MessageInterface::ShowMessage
      ("XyPlot::TakeAction() <%p>'%s' returning false, action=%s, actionData=%s "
       "was not handled\n", this, GetName().c_str(), action.c_str(), actionData.c_str());
   #endif
   
   return false;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool XyPlot::RenameRefObject(const UnsignedInt type,
                             const std::string &oldName,
                             const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("\nXyPlot::RenameRefObject() entered, this='%s', type=%s, oldName=%s, newName=%s\n",
       GetName().c_str(), GetObjectTypeString(type).c_str(), oldName.c_str(),
       newName.c_str());
   #endif
   
   // Check for allowed object types for name change.
   if (type != Gmat::PARAMETER && type != Gmat::SPACECRAFT &&
       type != Gmat::COORDINATE_SYSTEM && type != Gmat::BURN &&
       type != Gmat::IMPULSIVE_BURN && type != Gmat::CALCULATED_POINT &&
       type != Gmat::HARDWARE && type != Gmat::ODE_MODEL)
   {
      #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("XyPlot::RenameRefObject() >>>>> returning true, type is not one of Parameter, "
          "Spacecraft, CS, ImpBurn, CalcPoint, or Hardware\n\n");
      #endif
      return true;
   }
   
   if (type == Gmat::PARAMETER)
   {
      // X parameter
      if (mXParamName.find(oldName) != oldName.npos)
	  {
	     mXParamName =
			GmatStringUtil::ReplaceName(mXParamName, oldName, newName);
      }
      
      // Y parameters
      for (unsigned int i=0; i<mYParamNames.size(); i++)
      {
	      if (mYParamNames[i].find(oldName) != oldName.npos)
		  {
			mYParamNames[i] =
				GmatStringUtil::ReplaceName(mYParamNames[i], oldName, newName);
		  }
      }
   }
   else
   {
      // Replace X variable
      #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("   before replace mXParamName='%s'\n", mXParamName.c_str());
      #endif
      
      if (mXParamName.find(oldName) != mXParamName.npos)
         mXParamName = GmatStringUtil::ReplaceName(mXParamName, oldName, newName);
      
      #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("    after replace mXParamName='%s'\n", mXParamName.c_str());
      #endif
      
      // Replace Y variables
      for (unsigned int i=0; i<mYParamNames.size(); i++)
      {         
         #if DEBUG_RENAME
         MessageInterface::ShowMessage
            ("   before replace mYParamNames[%d] = '%s'\n", i, mYParamNames[i].c_str());
         #endif
         
         if (mYParamNames[i].find(oldName) != oldName.npos)
            mYParamNames[i] = GmatStringUtil::ReplaceName(mYParamNames[i], oldName, newName);
         
         #if DEBUG_RENAME
         MessageInterface::ShowMessage
            ("    after replace mYParamNames[%d] = '%s'\n", i, mYParamNames[i].c_str());
         #endif
      }
   }
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("XYFile::RenameRefObject() returning Subscriber::RenameRefObject()\n");
   #endif
   
   // Call Subscriber to rename wrapper object names
   return Subscriber::RenameRefObject(type, oldName, newName);
   //return true;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string XyPlot::GetParameterText(const Integer id) const
{
   if (id >= SubscriberParamCount && id < XyPlotParamCount)
      return PARAMETER_TEXT[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterText(id);
    
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer XyPlot::GetParameterID(const std::string &str) const
{
   for (int i=SubscriberParamCount; i<XyPlotParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SubscriberParamCount])
      {
         switch (i)
         {
            case IND_VAR:
            case ADD:
            case DRAW_GRID:
               WriteDeprecatedMessage(i);
               break;

            default:
               break;
         }
         return i;
      }
   }
   // deprecated labels
   
   return Subscriber::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType XyPlot::GetParameterType(const Integer id) const
{
   if (id >= SubscriberParamCount && id < XyPlotParamCount)
      return PARAMETER_TYPE[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string XyPlot::GetParameterTypeString(const Integer id) const
{
   if (id >= SubscriberParamCount && id < XyPlotParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return Subscriber::GetParameterTypeString(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool XyPlot::IsParameterReadOnly(const Integer id) const
{
   if (
//       (id == PLOT_TITLE)             ||
//       (id == X_AXIS_TITLE)           ||
//       (id == Y_AXIS_TITLE)           ||
       (id == DATA_COLLECT_FREQUENCY) ||
       (id == UPDATE_PLOT_FREQUENCY)  ||
//       (id == USE_LINES)              ||
//       (id == LINE_WIDTH)             ||
//       (id == USE_MARKERS)            ||
//       (id == MARKER_SIZE)            ||
       (id == DRAWING)                ||
       (id == IND_VAR)                ||
       (id == ADD)                    ||
       (id == DRAW_GRID)            
      )
      return true;
   
   return Subscriber::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool XyPlot::IsParameterCommandModeSettable(const Integer id) const
{
   // Override for base class setting
   if ((id == SOLVER_ITERATIONS) )
      return true;

   return Subscriber::IsParameterCommandModeSettable(id);
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer XyPlot::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
   case DATA_COLLECT_FREQUENCY:
      return mDataCollectFrequency;
   case UPDATE_PLOT_FREQUENCY:
      return mUpdatePlotFrequency;
//   case LINE_WIDTH:
//      return lineWidth;
//   case MARKER_SIZE:
//      return markerSize;
   default:
      return Subscriber::GetIntegerParameter(id);
   }
}

//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
Integer XyPlot::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer XyPlot::SetIntegerParameter(const Integer id, const Integer value)
{
   switch (id)
   {
   case DATA_COLLECT_FREQUENCY:
      mDataCollectFrequency = value;
      return mDataCollectFrequency;
   case UPDATE_PLOT_FREQUENCY:
      mUpdatePlotFrequency = value;
      return mUpdatePlotFrequency;
   default:
      return Subscriber::SetIntegerParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const std::string &label,
//                                     const Integer value)
//------------------------------------------------------------------------------
Integer XyPlot::SetIntegerParameter(const std::string &label,
                                    const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//---------------------------------------------------------------------------
//  std::string GetOnOffParameter(const Integer id) const
//---------------------------------------------------------------------------
std::string XyPlot::GetOnOffParameter(const Integer id) const
{
   switch (id)
   {
   case DRAW_GRID:
      WriteDeprecatedMessage(id);
      if (mDrawGrid)
         return "On";
      else
         return "Off";
   default:
      return Subscriber::GetOnOffParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string XyPlot::GetOnOffParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string XyPlot::GetOnOffParameter(const std::string &label) const
{
   return GetOnOffParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
//  bool SetOnOffParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
bool XyPlot::SetOnOffParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case DRAW_GRID:
      WriteDeprecatedMessage(id);
      mDrawGrid = value == "On";
      return true;
   default:
      return Subscriber::SetOnOffParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetOnOffParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool XyPlot::SetOnOffParameter(const std::string &label, const std::string &value)
{
   return SetOnOffParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string XyPlot::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case IND_VAR:
      WriteDeprecatedMessage(id);
   case XVARIABLE:
      return mXParamName;
//   case PLOT_TITLE:
//      return mPlotTitle;
//   case X_AXIS_TITLE:
//      return mXAxisTitle;
//   case Y_AXIS_TITLE:
//      return mYAxisTitle;
   default:
      return Subscriber::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string XyPlot::GetStringParameter(const std::string &label) const
{
   #if DEBUG_XY_PARAM
   MessageInterface::ShowMessage("XyPlot::GetStringParameter() label = %s\n",
                                 label.c_str());
   #endif
   
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool XyPlot::SetStringParameter(const Integer id, const std::string &value)
{
   #if DEBUG_XYPLOT_PARAM
   MessageInterface::ShowMessage("XyPlot::SetStringParameter() id = %d, "
                                 "value = %s \n", id, value.c_str());
   #endif
   
   switch (id)
   {
   case IND_VAR:
      WriteDeprecatedMessage(id);
   case XVARIABLE:
      return SetXParameter(value);
   case ADD:
      WriteDeprecatedMessage(id);
   case YVARIABLES:
      return AddYParameter(value, mNumYParams);
//   case PLOT_TITLE:
//      mPlotTitle = value;
//      return true;
//   case X_AXIS_TITLE:
//      mXAxisTitle = value;
//      return true;
//   case Y_AXIS_TITLE:
//      mYAxisTitle = value;
//      return true;
   default:
      return Subscriber::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool XyPlot::SetStringParameter(const std::string &label,
                                const std::string &value)
{
   #if DEBUG_XYPLOT_PARAM
   MessageInterface::ShowMessage("XyPlot::SetStringParameter() label = %s, "
                                 "value = %s \n", label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool XyPlot::SetStringParameter(const Integer id, const std::string &value,
                                const Integer index)
{
   switch (id)
   {
   case ADD:
      WriteDeprecatedMessage(id);
   case YVARIABLES:
      return AddYParameter(value, index);
   default:
      return Subscriber::SetStringParameter(id, value, index);
   }
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool XyPlot::SetStringParameter(const std::string &label,
                                const std::string &value,
                                const Integer index)
{
   #if DEBUG_XYPLOT_PARAM
   MessageInterface::ShowMessage
      ("XyPlot::SetStringParameter() label=%s, value=%s, index=%d \n",
       label.c_str(), value.c_str(), index);
   #endif
   
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& XyPlot::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case ADD:
      WriteDeprecatedMessage(id);
   case YVARIABLES:
      return mYParamNames;
   default:
      return Subscriber::GetStringArrayParameter(id);
   }
}


//------------------------------------------------------------------------------
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& XyPlot::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
bool XyPlot::GetBooleanParameter(const Integer id) const
{
   if (id == SHOW_PLOT)
      return active;
//   if (id == USE_MARKERS)
//      return useMarkers;
//   if (id == USE_LINES)
//      return useLines;
   if (id == SHOW_GRID)
      return mDrawGrid;
   if (id == DRAWING)
      return drawing;
   return Subscriber::GetBooleanParameter(id);
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
bool XyPlot::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
bool XyPlot::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  bool  SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * This method sets the bool parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> bool value for the requested parameter.
 *
 * @return  the boolean set.
 */
 //------------------------------------------------------------------------------
bool XyPlot::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == SHOW_PLOT)
   {
      active = value;
      return active;
   }
//   if (id == USE_MARKERS)
//   {
//      useMarkers = value;
//      // Always have to have either markers or lines
//      if (useMarkers == false)
//         useLines = true;
//      return useMarkers;
//   }
//   if (id == USE_LINES)
//   {
//      useLines = value;
//      if (useLines == false)
//         useMarkers = true;
//      return useLines;
//   }
   if (id == SHOW_GRID)
   {
      mDrawGrid = value;
      return mDrawGrid;
   }
   return Subscriber::SetBooleanParameter(id, value);
}

//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* XyPlot::GetRefObject(const UnsignedInt type,
                               const std::string &name)
{
   // if name is X parameter
   if (name == mXParamName)
   {
      return mXParam;
   }
   else
   {
      // name is Y parameter
      for (int i=0; i<mNumYParams; i++)
      {
         if (mYParamNames[i] == name)
            return mYParams[i];
      }
   }
   
   throw GmatBaseException("XyPlot::GetRefObject() the object name: " + name +
                           "not found\n");
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
bool XyPlot::SetRefObject(GmatBase *obj, const UnsignedInt type,
                          const std::string &name)
{
   #if DEBUG_XYPLOT_OBJECT
   MessageInterface::ShowMessage
      ("\nXyPlot::SetRefObject() this=<%p>'%s' entered, obj=<%p><%s>'%s', type=%d, name='%s'\n",
       this, GetName().c_str(), obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", type, name.c_str());
   #endif
   
   if (obj == NULL)
   {
      #if DEBUG_XYPLOT_OBJECT
      MessageInterface::ShowMessage
         ("XyPlot::SetRefObject() <%p>'%s' returning false, obj is NULL\n",
          this, GetName().c_str());
      #endif
      return false;
   }
   
   if (type == Gmat::PARAMETER)
   {
      SetWrapperReference(obj, name);
      
      // Handle array element
      std::string realName = GmatStringUtil::GetArrayName(mXParamName);
      
      // X parameter
      if (realName == name)
      {
         mXParam = (Parameter*)obj;
         
         if (!mXParam->IsPlottable())
         {
            // Array element is plottable, so check it
            if (!mXParam->IsOfType("Array") || 
                (mXParam->IsOfType("Array") && mXParamName.find("(") == name.npos))
            {
               #if DEBUG_XYPLOT_OBJECT
               MessageInterface::ShowMessage
                  ("XyPlot::SetRefObject() mXParam <%p>'%s' is not plottable, "
                   "so throwing an exception\n", mXParam, mXParamName.c_str());
               #endif
               throw SubscriberException
                  ("The X parameter: " + name + " of " + instanceName +
                   " is not plottable\n");
            }
         }
         
         #if DEBUG_XYPLOT_OBJECT
         MessageInterface::ShowMessage
            ("XyPlot::SetRefObject() mXParam <%p>'%s' successfully set to <%p>'%s'\n",
             mXParam, mXParamName.c_str(), obj, obj->GetName().c_str());
         #endif
      }
      
      // Y parameters
      for (int i=0; i<mNumYParams; i++)
      {
         realName = GmatStringUtil::GetArrayName(mYParamNames[i]);
         
         if (realName == name)
         {
            mYParams[i] = (Parameter*)obj;
            
            if (!mYParams[i]->IsPlottable())
            {
               // Array element is plottable, so check it
               if (!mYParams[i]->IsOfType("Array") || 
                   (mYParams[i]->IsOfType("Array") && mYParamNames[i].find("(") == name.npos))
               {
                  #if DEBUG_XYPLOT_OBJECT
                  MessageInterface::ShowMessage
                     ("XyPlot::SetRefObject() mYParams[%d] <%p>'%s' is not plottable, "
                      "so throwing an exception\n", i, mYParams[i], mYParamNames[i].c_str());
                  #endif
                  throw SubscriberException
                     ("The Y parameter: " + name + " of " + instanceName +
                      " is not plottable\n");
               }
            }
            
            #if DEBUG_XYPLOT_OBJECT
            MessageInterface::ShowMessage
               ("XyPlot::SetRefObject() mYParams[%d] <%p>'%s' successfully set to <%p>'%s'\n",
                i, mYParams[i], mYParamNames[i].c_str(), obj, obj->GetName().c_str());
            #endif
         }
      }

      #if DEBUG_XYPLOT_OBJECT
      MessageInterface::ShowMessage
         ("XyPlot::SetRefObject() this=<%p>'%s' returning true\n", this, GetName().c_str());
      #endif
      
      return true;
   }
   else
   {
      #if DEBUG_XYPLOT_OBJECT
      MessageInterface::ShowMessage
         ("XyPlot::SetRefObject() '%s' is not plottable, "
          "so throwing an exception\n", name.c_str());
      #endif
      std::string whichParam = "The Y parameter ";
      if (name == mXParamName)
         whichParam = "The X parameter";
      throw SubscriberException
         (whichParam + "\"" + name + "\" on object \"" + instanceName +
          "\" of XYPlot is not plottable.\n");
   }
   
   #if DEBUG_XYPLOT_OBJECT
   MessageInterface::ShowMessage
      ("XyPlot::SetRefObject() returning Subscriber::SetRefObject()\n");
   #endif
   
   return Subscriber::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool XyPlot::HasRefObjectTypeArray()
{
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& XyPlot::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::PARAMETER);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& XyPlot::GetRefObjectNameArray(const UnsignedInt type)
{
   mAllParamNames.clear();
   
   switch (type)
   {
   case Gmat::UNKNOWN_OBJECT:
   case Gmat::PARAMETER:
   {
      std::string realName;
      // Add x parameter
      // Handle array index
      if (mXParamName != "")
      {
         realName = GmatStringUtil::GetArrayName(mXParamName);
         mAllParamNames.push_back(realName);
      }
      
      // add y parameters
      for (int i=0; i<mNumYParams; i++)
         if (mYParamNames[i] != "")
         {
            realName = GmatStringUtil::GetArrayName(mYParamNames[i]);
            mAllParamNames.push_back(realName);
         }
      break;
   }
   default:
      break;
   }
   
   #ifdef DEBUG_XYPLOT_OBJECT
   MessageInterface::ShowMessage
      ("XyPlot::GetRefObjectNameArray() returning %d ref objects\n", mAllParamNames.size());
   for (unsigned int i = 0; i < mAllParamNames.size(); i++)
      MessageInterface::ShowMessage("   [%d]: '%s'\n", i, mAllParamNames[i].c_str());
   #endif
   
   return mAllParamNames;
}


//---------------------------------------------------------------------------
// UnsignedInt GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
UnsignedInt XyPlot::GetPropertyObjectType(const Integer id) const
{
   switch (id)
   {
   case XVARIABLE:
   case YVARIABLES:
   case IND_VAR:
   case ADD:
      return Gmat::PARAMETER;
   default:
      return Subscriber::GetPropertyObjectType(id);
   }
}

//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
const StringArray& XyPlot::GetWrapperObjectNameArray(bool completeSet)
{
   xWrapperObjectNames.clear();
   yWrapperObjectNames.clear();
   allWrapperObjectNames.clear();
   
   // Do not add blank name (Fix for GMT-3023 LOJ: 2012.08.09)
   if (mXParamName != "")
      xWrapperObjectNames.push_back(mXParamName);
   yWrapperObjectNames.insert(yWrapperObjectNames.begin(), mYParamNames.begin(),
                              mYParamNames.end());
   
   // Do not add blank name (Fix for GMT-3023 LOJ: 2012.08.09)
   if (mXParamName != "")
      allWrapperObjectNames.push_back(mXParamName);
   allWrapperObjectNames.insert(allWrapperObjectNames.end(), mYParamNames.begin(),
                                mYParamNames.end());
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("   xWrapperObjectNames.size() = %d, yWrapperObjectNames.size() = %d\n",
       xWrapperObjectNames.size(), yWrapperObjectNames.size());
   for (unsigned int i = 0; i < xWrapperObjectNames.size(); i++)
      MessageInterface::ShowMessage
         ("   xWrapperObjectNames[%d] = '%s'\n", i, xWrapperObjectNames[i].c_str());
   for (unsigned int i = 0; i < yWrapperObjectNames.size(); i++)
      MessageInterface::ShowMessage
         ("   yWrapperObjectNames[%d] = '%s'\n", i, yWrapperObjectNames[i].c_str());
   MessageInterface::ShowMessage
      ("XyPlot::GetWrapperObjectNameArray() returning, allWrapperObjectNames.size()=%d\n",
       allWrapperObjectNames.size());
   for (UnsignedInt i = 0; i < allWrapperObjectNames.size(); i++)
      MessageInterface::ShowMessage("   '%s'\n", allWrapperObjectNames[i].c_str());
   #endif
   
   return allWrapperObjectNames;
}


//------------------------------------------------------------------------------
// void XyPlot::SetRunState(Gmat::RunState rs)
//------------------------------------------------------------------------------
void XyPlot::SetRunState(Gmat::RunState rs)
{
   if (rs == Gmat::IDLE)
      PlotInterface::TakeXYAction(instanceName, "AlwaysRedraw");
   else
      PlotInterface::TakeXYAction(instanceName, "RunModeRedraw");
   Subscriber::SetRunState(rs);
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// void BuildPlotTitle()
//------------------------------------------------------------------------------
void XyPlot::BuildPlotTitle()
{
   //set X and Y axis title
   //if (mXAxisTitle == "")
   //{
      // Now using wrapppers (LOJ: 2012.08.02)
      //if (mXParam)
      //   mXAxisTitle = mXParam->GetName();
      if (xParamWrappers[0])
         mXAxisTitle = xParamWrappers[0]->GetDescription();
      else
      {
         // Thorow exception here, since it causes crash (LOJ: 2012.08.02)
         throw SubscriberException
            ("Error getting the X parameter title on " + instanceName +  " of XYPlot.\n");
         //mXAxisTitle = "No X parameters";
         //mYAxisTitle = "empty";
         //mPlotTitle  = "Plot not fully initialized";
         //return;
      }
      //}
      
   #if DEBUG_XYPLOT_INIT
   MessageInterface::ShowMessage("XyPlot::BuildPlotTitle() mXAxisTitle = %s\n",
                                 mXAxisTitle.c_str());
   #endif
   
   mYAxisTitle = "";
   for (int i= 0; i<mNumYParams-1; i++)
   {
      //if (mYParams[i])
      //   mYAxisTitle += (mYParams[i]->GetName() + ", ");         
      if (yParamWrappers[i])
         mYAxisTitle += (yParamWrappers[i]->GetDescription() + ", ");         
   }
   //mYAxisTitle += mYParams[mNumYParams-1]->GetName();
   mYAxisTitle += yParamWrappers[mNumYParams-1]->GetDescription();
   
   #if DEBUG_XYPLOT_INIT
   MessageInterface::ShowMessage("XyPlot::BuildPlotTitle() mYAxisTitle = %s\n",
                                 mYAxisTitle.c_str());
   #endif
   
   mPlotTitle = "(" + mXAxisTitle + ")" + " vs " + "(" + mYAxisTitle + ")";
   
   #if DEBUG_XYPLOT_INIT
   MessageInterface::ShowMessage("XyPlot::BuildPlotTitle() mPlotTitle = %s\n",
                                 mPlotTitle.c_str());
   #endif
}

//------------------------------------------------------------------------------
// bool ClearYParameters()
//------------------------------------------------------------------------------
bool XyPlot::ClearYParameters()
{
   #ifdef DEBUG_CLEAR
   MessageInterface::ShowMessage
      ("XyPlot::ClearYParameters() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   DeletePlotCurves();
   mYParams.clear();
   mYParamNames.clear();
   mAllParamNames.clear();
   mNumYParams = 0;
   mPlotTitle = "";
   mXAxisTitle = "";
   mYAxisTitle = "";
   mIsXyPlotWindowSet = false;
   ClearWrappers(false, true);
   #ifdef DEBUG_CLEAR
   MessageInterface::ShowMessage
      ("XyPlot::ClearYParameters() <%p>'%s' returning true\n", this, GetName().c_str());
   #endif
   return true;
}

//------------------------------------------------------------------------------
// bool RemoveYParameter(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Removes parameter from the Y parameter list
 *
 * @param <name> parameter name to be removed from the Y parameter list
 *
 * @return true if parameter was removed from the Y parameter list, false otherwise
 *
 */
//------------------------------------------------------------------------------
bool XyPlot::RemoveYParameter(const std::string &name)
{
   #if DEBUG_ACTION_REMOVE
   MessageInterface::ShowMessage
      ("XyPlot::RemoveYParameter() name=%s\n--- Before remove:\n", name.c_str());
   for (int i=0; i<mNumYParams; i++)
   {
      MessageInterface::ShowMessage("mYParamNames[%d]=%s\n", i,
                                    mYParamNames[i].c_str());
   }
   #endif
   
   StringArray::iterator pos1;
   std::vector<Parameter*>::iterator pos2 = mYParams.begin();

   for (pos1 = mYParamNames.begin(); pos1 != mYParamNames.end(); pos1++)
   {
      if (*pos1 == name)
      {
         mYParamNames.erase(pos1);
         mYParams.erase(pos2);
         mNumYParams = mYParamNames.size();
         
         #if DEBUG_ACTION_REMOVE
         MessageInterface::ShowMessage("---After remove\n");
         for (int i=0; i<mNumYParams; i++)
         {
            MessageInterface::ShowMessage("mYParamNames[%d]=%s\n", i,
                                          mYParamNames[i].c_str());
         }
         #endif
         
         return true;
      }
      else
      {
         advance(pos2, 1);
      }
   }
   
   //------------------------------------------
   // loj: 9/29/04
   // Need to remove from PlotCurves also
   //------------------------------------------
   
   #if DEBUG_ACTION_REMOVE
   MessageInterface::ShowMessage("XyPlot::RemoveYParameter() name=%s not found\n");
   #endif
   
   return false;
}

//------------------------------------------------------------------------------
// bool ResetYParameters()
//------------------------------------------------------------------------------
bool XyPlot::ResetYParameters()
{
   PlotInterface::ClearXyPlotData(instanceName);
   return true;
}

//------------------------------------------------------------------------------
// bool PenUp()
//------------------------------------------------------------------------------
bool XyPlot::PenUp()
{
   PlotInterface::XyPlotPenUp(instanceName);
   drawing = false;
   return true;
}

//------------------------------------------------------------------------------
// bool PenDown()
//------------------------------------------------------------------------------
bool XyPlot::PenDown()
{
   PlotInterface::XyPlotPenDown(instanceName);
   drawing = true;
   return true;
}

//------------------------------------------------------------------------------
// bool MarkPoint()
//------------------------------------------------------------------------------
/**
 * Places an X marker at the current point on all curves in a plot
 * @return true on success
 */
//------------------------------------------------------------------------------
bool XyPlot::MarkPoint()
{
   PlotInterface::XyPlotMarkPoint(instanceName);
   return true;
}

//------------------------------------------------------------------------------
// bool Darken(Integer factor)
//------------------------------------------------------------------------------
/**
 * Darkens the curves on a plot starting at the current position
 *
 * @param factor The darkening factor
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool XyPlot::Darken(Integer factor)
{
   PlotInterface::XyPlotDarken(instanceName, factor);
   return true;
}

//------------------------------------------------------------------------------
// bool Lighten(Integer factor)
//------------------------------------------------------------------------------
/**
 * Lightens the curves on a plot starting at the current position
 *
 * @param factor The lightening factor
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool XyPlot::Lighten(Integer factor)
{
   PlotInterface::XyPlotLighten(instanceName, factor);
   return true;
}


//------------------------------------------------------------------------------
// bool MarkBreak()
//------------------------------------------------------------------------------
/**
 * Sets a break point on all active curves
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool XyPlot::MarkBreak()
{
   if (mSolverIterOption == SI_CURRENT)
      PlotInterface::XyPlotMarkBreak(instanceName);
   return true;
}

//------------------------------------------------------------------------------
// bool ClearFromBreak()
//------------------------------------------------------------------------------
/**
 * Deletes all data after the most recent break point 
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool XyPlot::ClearFromBreak()
{
   if (mSolverIterOption == SI_CURRENT)
      PlotInterface::XyPlotClearFromBreak(instanceName);
   return true;
}


//------------------------------------------------------------------------------
// void DeletePlotCurves()
//------------------------------------------------------------------------------
void XyPlot::DeletePlotCurves()
{
   // delete exiting curves
   PlotInterface::DeleteAllXyPlotCurves(instanceName, mOldName);
}


// methods inherited from Subscriber
//------------------------------------------------------------------------------
// bool Distribute(int len)
//------------------------------------------------------------------------------
bool XyPlot::Distribute(int len)
{
   //loj: How do I convert data to Real data?
   return false;
}

//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
bool XyPlot::Distribute(const Real * dat, Integer len)
{
   if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING_NO_PLOTS)
      return true;
   
   #if DEBUG_XYPLOT_UPDATE > 1
   MessageInterface::ShowMessage
      ("\nXyPlot::Distribute() <%p>'%s' entered, len=%d isEndOfReceive=%d, active=%d, "
       "runState=%d\n", this, GetName().c_str(), len, isEndOfReceive, active, runstate);
   #endif
   
   if (isEndOfReceive)
   {
      // if targeting and draw target is None, just return
      if (mSolverIterations == "None" &&
          ((runstate == Gmat::TARGETING) || (runstate == Gmat::OPTIMIZING) ||
                (runstate == Gmat::SOLVING)))
      {
         #if DEBUG_XYPLOT_UPDATE > 1
         MessageInterface::ShowMessage
            ("XyPlot::Distribute() <%p>'%s' just returning true, end of data "
             "recieved and not writing solver data\n", this, GetName().c_str());
         #endif
         
         return true;
      }
      
      if (active)
      {
         #if DEBUG_XYPLOT_UPDATE > 1
         MessageInterface::ShowMessage
            ("XyPlot::Distribute() <%p>'%s' returning PlotInterface::RefreshXyPlot(), "
             "end of data received\n", this, GetName().c_str());
         #endif
         return PlotInterface::RefreshXyPlot(instanceName);
      }
   }
   
   // if targeting and draw target is None, just return
   if (mSolverIterations == "None" &&
       ((runstate == Gmat::TARGETING) || (runstate == Gmat::OPTIMIZING) ||
        (runstate == Gmat::SOLVING)))
   {
      #if DEBUG_XYPLOT_UPDATE > 1
      MessageInterface::ShowMessage
         ("XyPlot::Distribute() <%p>'%s' just returning true, not writing solver data\n",
          this, GetName().c_str());
      #endif
      return true;
   }
   
   if (len == 0)
   {
      #if DEBUG_XYPLOT_UPDATE > 1
      MessageInterface::ShowMessage
         ("XyPlot::Distribute() <%p>'%s' just returning true, length is zero\n",
          this, GetName().c_str());
      #endif
      return true;
   }
   
   // Now using wrappers and supports only one X parameter (LOJ: 2012.08.02)
   if (!xParamWrappers.empty() && mNumYParams > 0)
   {
      // Skip data if data publishing command such as Propagate is inside a function
      // and this ReportFile is not a global nor a local object (i.e declared in the main script)
      // (LOJ: 2015.08.13)
      if (currentProvider && currentProvider->TakeAction("IsInFunction"))
      {
         bool skipDataX = ShouldDataBeSkipped(1);
         bool skipDataY = ShouldDataBeSkipped(2);
         
         if (skipDataX || skipDataY)
         {
            #if DEBUG_XYPLOT_UPDATE > 1
            MessageInterface::ShowMessage
               ("XyPlot::Distribute() this=<%p>'%s' just returning true\n   data is "
                "from a function and drawing Parameter is not a global nor a local object\n",
                this, GetName().c_str());
            #endif
            return true;
         }
      }
      
      // Get x param value
      Real xval = xParamWrappers[0]->EvaluateReal();
      
      #if DEBUG_XYPLOT_UPDATE
      MessageInterface::ShowMessage
         ("XyPlot::Distribute() xParamWrappers[0]='%s', xval = %f\n",
          xParamWrappers[0]->GetDescription().c_str(), xval);
      #endif
      
      // Get y param values
      Rvector yvals = Rvector(mNumYParams);
      
      // put yvals in the order of parameters added
      for (int i=0; i<mNumYParams; i++)
      {
         #if DEBUG_XYPLOT_UPDATE
         MessageInterface::ShowMessage
            ("XyPlot::Distribute() yParamWrappers[%d]=<%p>, Calling yParamWrappers[%d]->EvaluateReal()\n",
             i, yParamWrappers[i], i);
         #endif
         // Now using wrappers (LOJ: 2012.08.02)
         if (yParamWrappers[i] == NULL)
         {
            MessageInterface::ShowMessage
               ("*** WARNING *** The XYPlot named \"%s\" will not be shown.\n"
                "The parameter selected for Y Axis is NULL\n",
                GetName().c_str());
            return true;
         }
         
         // Now using wrappers (LOJ: 2012.08.02)
         yvals[i] = yParamWrappers[i]->EvaluateReal();
         
         #if DEBUG_XYPLOT_UPDATE
         MessageInterface::ShowMessage
            ("XyPlot::Distribute() yParamWrappers[%d]='%s', yvals[%d] = %f\n",
             i, yParamWrappers[i]->GetDescription().c_str(), i, yvals[i]);
         #endif
      }
      
      // Update xy plot
      // X value must start from 0
      if (mIsXyPlotWindowSet)
      {
         mNumDataPoints++;
         
         if ((mNumDataPoints % mDataCollectFrequency) == 0)
         {
            //mNumDataPoints = 0;
            mNumCollected++;
            bool update = (mNumCollected % mUpdatePlotFrequency) == 0;
            
            #if DEBUG_XYPLOT_UPDATE > 1
            MessageInterface::ShowMessage
               ("XyPlot::Distribute() calling PlotInterface::UpdateXyPlot(), "
                "update = %d, mNumDataPoints = %d, \n", update, mNumDataPoints);
            #endif
            
            // return flag is ignored here since it needs to return true
            // for all case
            PlotInterface::UpdateXyPlot(instanceName, mOldName, xval,
                                        yvals, mPlotTitle, mXAxisTitle, mYAxisTitle,
                                        mSolverIterOption, update, mDrawGrid);
            
            if (update)
               mNumCollected = 0;
         }
      }
   }
   
   //loj: always return true otherwise next subscriber will not call ReceiveData()
   //     in Publisher::Publish()
   return true;
}

//------------------------------------------------------------------------------
// void WriteDeprecatedMessage(Integer id) const
//------------------------------------------------------------------------------
void XyPlot::WriteDeprecatedMessage(Integer id) const
{
   // Write only one message per session
   static bool writeXVariable = true;
   static bool writeYVariables = true;
   static bool writeShowGrid = true;
   
   switch (id)
   {
   case IND_VAR:
      if (writeXVariable)
      {
         MessageInterface::ShowMessage
            (deprecatedMessageFormat.c_str(), "IndVar", GetName().c_str(),
             "XVariable");
         writeXVariable = false;
      }
      break;
   case ADD:
      if (writeYVariables)
      {
         MessageInterface::ShowMessage
            (deprecatedMessageFormat.c_str(), "Add", GetName().c_str(),
             "YVariables");
         writeYVariables = false;
      }
      break;
   case DRAW_GRID:
      if (writeShowGrid)
      {
         MessageInterface::ShowMessage
            (deprecatedMessageFormat.c_str(), "Grid", GetName().c_str(),
             "ShowGrid");
         writeShowGrid = false;
      }
      break;
   default:
      break;
   }
}


