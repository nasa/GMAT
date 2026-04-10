//$Id$
//------------------------------------------------------------------------------
//                                  GroundTrackPlot
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
// number X-XXXXX-X
//
// Author: Linda Jun
// Created: 2011/05/31
//
/**
 * Implements GroundTrackPlot class.
 */
//------------------------------------------------------------------------------

#include "GroundTrackPlot.hpp"
#include "PlotInterface.hpp"       // for UpdateGlPlot()
#include "SubscriberException.hpp" // for SubscriberException()
#include "FileManager.hpp"         // for GetFullPathname()
#include "StringUtil.hpp"          // for ToUpper()
#include "MessageInterface.hpp"    // for ShowMessage()


//#define DBGLVL_PARAM_STRING 2
//#define DBGLVL_OBJ 1
//#define DBGLVL_INIT 2
//#define DBGLVL_UPDATE 1
//#define DEBUG_TEXTURE_MAP
//#define DEBUG_VALIDATION

//---------------------------------
// static data
//---------------------------------

/// Available show foot print options
StringArray GroundTrackPlot::footPrintOptions;

const std::string
GroundTrackPlot::FOOT_PRINT_OPTION_TEXT[FootPrintOptionCount] =
{
   "None", "All", 
};


const std::string
GroundTrackPlot::PARAMETER_TEXT[GroundTrackPlotParamCount - OrbitPlotParamCount] =
{
   "CentralBody",
   "TextureMap",
   "ShowFootPrints",
}; 


const Gmat::ParameterType
GroundTrackPlot::PARAMETER_TYPE[GroundTrackPlotParamCount - OrbitPlotParamCount] =
{
   Gmat::OBJECT_TYPE,         // "CentralBody"
   Gmat::FILENAME_TYPE,       // "TextureMap"
   Gmat::ENUMERATION_TYPE,    // "ShowFootPrints"
};


//------------------------------------------------------------------------------
// GroundTrackPlot(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * The default constructor
 */
//------------------------------------------------------------------------------
GroundTrackPlot::GroundTrackPlot(const std::string &name)
   : OrbitPlot("GroundTrackPlot", name)
{
   // GmatBase data
   parameterCount = GroundTrackPlotParamCount;
   objectTypeNames.push_back("GroundTrackPlot");
   
   centralBody = NULL;
   centralBodyName = "Earth";
   mViewCoordSysName = "EarthFixed";
   footPrints = "None";
   
   // Set default texture map file from the startup file through the FileManager
   //FileManager *fm = FileManager::Instance();
   //textureMapFileName = fm->GetFullPathname("EARTH_TEXTURE_FILE");
   // Find file name and full path (LOJ: 2014.06.18)
   #ifdef DEBUG_TEXTURE_MAP
   MessageInterface::ShowMessage
      ("GroundTrackPlot::GroundTrackPlot() constructor calling SetTextureMapFileName()\n");
   #endif
   SetTextureMapFileName("", false);
   
   footPrintOption = FP_NONE;
   
   footPrintOptions.clear();
   for (UnsignedInt i = 0; i < FootPrintOptionCount; i++)
      footPrintOptions.push_back(FOOT_PRINT_OPTION_TEXT[i]);
   
   #ifdef DEBUG_TEXTURE_MAP
   MessageInterface::ShowMessage
      ("GroundTrackPlot::GroundTrackPlot() constructor leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// GroundTrackPlot(const GroundTrackPlot &plot)
//------------------------------------------------------------------------------
/**
 * The copy consturctor
 */
//------------------------------------------------------------------------------
GroundTrackPlot::GroundTrackPlot(const GroundTrackPlot &plot)
   : OrbitPlot(plot)
{
   centralBody        = plot.centralBody;
   centralBodyName    = plot.centralBodyName;
   footPrints         = plot.footPrints;
   textureMapFileName = plot.textureMapFileName;
   textureMapFullPath = plot.textureMapFullPath;
   footPrintOption    = plot.footPrintOption;
}


//------------------------------------------------------------------------------
// GroundTrackPlot& operator=(const GroundTrackPlot &plot)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
GroundTrackPlot& GroundTrackPlot::operator=(const GroundTrackPlot& plot)
{
   if (this == &plot)
      return *this;
   
   OrbitPlot::operator=(plot);
   
   centralBody        = plot.centralBody;
   centralBodyName    = plot.centralBodyName;
   footPrints         = plot.footPrints;
   textureMapFileName = plot.textureMapFileName;
   textureMapFullPath = plot.textureMapFullPath;
   footPrintOption    = plot.footPrintOption;
   
   return *this;
}


//------------------------------------------------------------------------------
// ~GroundTrackPlot()
//------------------------------------------------------------------------------
/**
 * Destructor
 *
 * @note This destructor does not delete 3DView window, but clears data.
 *       3DView window is deleted when it is closed by the user or GMAT
 *       shuts down.
 */
//------------------------------------------------------------------------------
GroundTrackPlot::~GroundTrackPlot()
{
}


//----------------------------------
// inherited methods from Subscriber
//----------------------------------

//---------------------------------
// inherited methods from GmatBase
//---------------------------------

//------------------------------------------------------------------------------
//  bool Validate()
//------------------------------------------------------------------------------
/**
 * Performs any pre-run validation that the object needs.
 *
 * @return true unless validation fails.
 */
//------------------------------------------------------------------------------
bool GroundTrackPlot::Validate()
{
   bool retval = true;
   
   // Validate texture map file (LOJ: 2014.07.08)
   #ifdef DEBUG_VALIDATION
   MessageInterface::ShowMessage
      ("GroundTrackPlot::Validate() calling SetTextureMapFileName()\n");
   #endif
   
   SetTextureMapFileName(textureMapFileName, false, true);
   
   if (textureMapFullPath == "")
      retval = false;
   
   #ifdef DEBUG_VALIDATION
   MessageInterface::ShowMessage("GroundTrackPlot::Validate() retruning %d\n", retval);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool GroundTrackPlot::Initialize()
{
   //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING_NO_PLOTS)          // made changes by TUAN NGUYEN
   if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING_NO_PLOTS)     // made changes by TUAN NGUYEN
      return true;
   
   bool retval = OrbitPlot::Initialize();
   
   #if DBGLVL_INIT
   MessageInterface::ShowMessage
      ("GroundTrackPlot::Initialize() this=<%p>'%s', active=%d, isInitialized=%d, "
       "isEndOfReceive=%d, mAllSpCount=%d\n", this, GetName().c_str(), active,
       isInitialized, isEndOfReceive, mAllSpCount);
   #endif
   
   //--------------------------------------------------------
   // start initializing for ground track plot
   //--------------------------------------------------------
   if (active && !isInitialized)
   {
      #if DBGLVL_INIT
      MessageInterface::ShowMessage
         ("GroundTrackPlot::Initialize() CreateGlPlotWindow() theSolarSystem=%p\n",
          theSolarSystem);
      #endif
      
      // set ViewType
      PlotInterface::SetViewType(GmatPlot::GROUND_TRACK_PLOT);
      
      if (PlotInterface::CreateGlPlotWindow
          (instanceName, mOldName, mPlotUpperLeft[0], mPlotUpperLeft[1],
           mPlotSize[0], mPlotSize[1], isMaximized, mNumPointsToRedraw))
      {         
         //--------------------------------------------------------
         // Set Spacecraft and non-Spacecraft objects.
         // If non-Spacecraft, position has to be computed in the
         // GroundTrackCanvas, so need to pass those object pointers.
         //--------------------------------------------------------
         
         ClearDynamicArrays();
         BuildDynamicArrays();
         
         // Add central body to object list and always show
         if (centralBody != NULL)
            UpdateObjectList(centralBody, true);
         
         //===========================================================
         // Save this in in case we need to use light
         //===========================================================
         // Add Sun to list if it was not added already to enable light source 
         if (find(mObjectNameArray.begin(), mObjectNameArray.end(), "Sun") ==
             mObjectNameArray.end())
            UpdateObjectList(theSolarSystem->GetBody("Sun"), false);
         //===========================================================
         
         #if DBGLVL_INIT > 1
         MessageInterface::ShowMessage("   mObjectNameArray.size=%d\n", mObjectNameArray.size());
         bool draw, show;
         for (int i=0; i<mObjectCount; i++)
         {
            draw = mDrawOrbitArray[i] ? true : false;
            show = mDrawObjectArray[i] ? true : false;
            MessageInterface::ShowMessage
               ("   mObjectNameArray[%d]=%s, draw=%d, show=%d\n",
                i, mObjectNameArray[i].c_str(), draw, show);
         }
         #endif
         
         #if DBGLVL_INIT
         MessageInterface::ShowMessage
            ("   calling PlotInterface::SetGlSolarSystem(%p)\n", theSolarSystem);
         #endif
         
         // set SolarSystem
         PlotInterface::SetGlSolarSystem(instanceName, theSolarSystem);
         
         #if DBGLVL_INIT
         MessageInterface::ShowMessage
            ("   calling PlotInterface::SetGlObject()\n");
         for (UnsignedInt i=0; i<mObjectArray.size(); i++)
            MessageInterface::ShowMessage
               ("      mObjectArray[%d]=<%p>'%s'\n", i, mObjectArray[i],
                mObjectArray[i]->GetName().c_str());
         #endif

         //--------------------------------------------------------
         // set max data points to plot
         //--------------------------------------------------------
         PlotInterface::SetMaxGlDataPoints(instanceName, mMaxData);

         // Set all object array and pointers
         PlotInterface::SetGlObject(instanceName, mObjectNameArray, mObjectArray);
         
         //--------------------------------------------------------
         // set CoordinateSystem
         //--------------------------------------------------------
         #if DBGLVL_INIT
         MessageInterface::ShowMessage
            ("   calling PlotInterface::SetGlCoordSystem()\n");
         #endif
         
         if (mViewCoordSystem == NULL)
         {
            mViewCoordSystem = CoordinateSystem::CreateLocalCoordinateSystem
               (mViewCoordSysName, "BodyFixed", centralBody, NULL,
                NULL, theInternalCoordSystem->GetJ2000Body(), theSolarSystem);
         }
         
         PlotInterface::SetGlCoordSystem(instanceName, theInternalCoordSystem,
                                         mViewCoordSystem, mViewCoordSystem);
         
         
         //--------------------------------------------------------
         // set drawing options
         //--------------------------------------------------------
         #if DBGLVL_INIT
         MessageInterface::ShowMessage
            ("   calling PlotInterface::SetGl2dDrawingOption(), name='%s', "
             "centralBodyName='%s', textureMapFileName='%s'\n   textureMapFullPath='%s'\n",
             instanceName.c_str(), centralBodyName.c_str(), textureMapFileName.c_str(),
             textureMapFullPath.c_str());
         #endif
         
         // Pass textureMapFullPath instead of textureMapFileName
         // PlotInterface::SetGl2dDrawingOption(instanceName, centralBodyName,
         //                                     textureMapFileName, (Integer)footPrintOption);
         PlotInterface::SetGl2dDrawingOption(instanceName, centralBodyName,
                                             textureMapFullPath, (Integer)footPrintOption);
         
         //--------------------------------------------------------
         // set viewpoint info
         //--------------------------------------------------------
         #if DBGLVL_INIT
         MessageInterface::ShowMessage
            ("   calling PlotInterface::SetGlViewOption()\n");
         #endif
         
         PlotInterface::SetGlUpdateFrequency(instanceName, mUpdatePlotFrequency);
         
         //--------------------------------------------------------
         // set drawing object flag
         //--------------------------------------------------------
         PlotInterface::SetGlDrawOrbitFlag(instanceName, mDrawOrbitArray);
         PlotInterface::SetGlShowObjectFlag(instanceName, mDrawObjectArray);
         
         //--------------------------------------------------------
         // initialize GL
         //--------------------------------------------------------
         PlotInterface::InitializeGlPlot(instanceName);
         
         isInitialized = true;
         retval = true;
      }
      else
      {
         retval = false;
      }
   }
   else
   {
      #if DBGLVL_INIT
      MessageInterface::ShowMessage
         ("GroundTrackPlot::Initialize() Plot is active and initialized, "
          "so calling DeleteGlPlot()\n");
      #endif
      
      // Why do we want to delete plot if active and initialized?
      // This causes Global 3DView not to show, so commented out (loj: 2008.10.08)
      // We still need to delete non-active plots so that plot persistency works,
      // so uncommented (loj: 2011.09.23)
      if (!active)
         retval =  PlotInterface::DeleteGlPlot(instanceName);
   }
   
   #if DBGLVL_INIT
   MessageInterface::ShowMessage("GroundTrackPlot::Initialize() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the GroundTrackPlot.
 *
 * @return clone of the GroundTrackPlot.
 *
 */
//------------------------------------------------------------------------------
GmatBase* GroundTrackPlot::Clone() const
{
   return (new GroundTrackPlot(*this));
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
void GroundTrackPlot::Copy(const GmatBase* orig)
{
   operator=(*((GroundTrackPlot *)(orig)));
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
bool GroundTrackPlot::TakeAction(const std::string &action,
                                 const std::string &actionData)
{
   return OrbitPlot::TakeAction(action, actionData);
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool GroundTrackPlot::RenameRefObject(const UnsignedInt type,
                                      const std::string &oldName,
                                      const std::string &newName)
{
   #if DBGLVL_RENAME
   MessageInterface::ShowMessage
      ("GroundTrackPlot::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type == Gmat::CELESTIAL_BODY)
   {
      // for central body name
      if (centralBodyName == oldName)
         centralBodyName = newName;
   }
   
   return OrbitPlot::RenameRefObject(type, oldName, newName);
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
bool GroundTrackPlot::IsParameterReadOnly(const Integer id) const
{
   // Now GroundTrackPlot can configure orbit and target colors (LOJ: 2013.09.20)
   //if (id == COORD_SYSTEM || id == DRAW_OBJECT || id == ORBIT_COLOR ||
   //    id == TARGET_COLOR || id == SHOW_FOOT_PRINTS)
   if (id == COORD_SYSTEM || id == DRAW_OBJECT || id == SHOW_FOOT_PRINTS)
      return true;
   
   return OrbitPlot::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterValid(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool GroundTrackPlot::IsParameterValid(const Integer id,
                                       const std::string &value)
{
   #ifdef DEBUG_VALIDATION
   MessageInterface::ShowMessage
      ("GroundTrackPlot::IsParameterValid() entered, id=%d, value='%s'\n", id, value.c_str());
   #endif
   bool retval = true;
   if (id == TEXTURE_MAP)
   {
      #ifdef DEBUG_VALIDATION
      MessageInterface::ShowMessage("   Validating TEXTURE_MAP_FILE_NAME\n");
      #endif
      if (value == "" || value == "GenericCelestialBody.jpg")
         retval = true;
      else if (!SetTextureMapFileName(value, false, true))
         retval = false;
   }
   
   #ifdef DEBUG_VALIDATION
   MessageInterface::ShowMessage
      ("GroundTrackPlot::IsParameterValid() returning %d\n", retval);
   #endif
   return retval;
}

//------------------------------------------------------------------------------
// bool IsParameterValid(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool GroundTrackPlot::IsParameterValid(const std::string &label,
                                       const std::string &value)
{
   return IsParameterValid(GetParameterID(label), value);
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
bool GroundTrackPlot::IsParameterCommandModeSettable(const Integer id) const
{
   // Override for base class setting
   if ((id == SOLVER_ITERATIONS) )
      return true;

   return Subscriber::IsParameterCommandModeSettable(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string GroundTrackPlot::GetParameterText(const Integer id) const
{
   if (id >= OrbitPlotParamCount && id < GroundTrackPlotParamCount)
      return PARAMETER_TEXT[id - OrbitPlotParamCount];
   else
      return OrbitPlot::GetParameterText(id);
    
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer GroundTrackPlot::GetParameterID(const std::string &str) const
{
   for (int i=OrbitPlotParamCount; i<GroundTrackPlotParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - OrbitPlotParamCount])
         return i;
   }
   
   return OrbitPlot::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType GroundTrackPlot::GetParameterType(const Integer id) const
{
   if (id >= OrbitPlotParamCount && id < GroundTrackPlotParamCount)
      return PARAMETER_TYPE[id - OrbitPlotParamCount];
   else
      return OrbitPlot::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string GroundTrackPlot::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string GroundTrackPlot::GetStringParameter(const Integer id) const
{
   #if DBGLVL_PARAM_STRING
   MessageInterface::ShowMessage
      ("GroundTrackPlot::GetStringParameter() id = %d\n", id);
   #endif
   
   switch (id)
   {
   case ADD:
      return GetObjectStringList();
   case CENTRAL_BODY:
      return centralBodyName;
   case TEXTURE_MAP:
      #if DBGLVL_PARAM_STRING
		MessageInterface::ShowMessage
         ("   this = <%p>, returning '%s'\n", this, textureMapFileName.c_str());
		#endif
      return textureMapFileName;
   case SHOW_FOOT_PRINTS:
      return footPrints;
   default:
      return OrbitPlot::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string GroundTrackPlot::GetStringParameter(const std::string &label) const
{
   #if DBGLVL_PARAM_STRING
   MessageInterface::ShowMessage
      ("GroundTrackPlot::GetStringParameter() label = %s\n", label.c_str());
   #endif
   
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool GroundTrackPlot::SetStringParameter(const Integer id, const std::string &value)
{
   #if DBGLVL_PARAM_STRING
   MessageInterface::ShowMessage
      ("GroundTrackPlot::SetStringParameter() this=<%p>'%s', id=%d<%s>, value='%s'\n",
       this, instanceName.c_str(), id, GetParameterText(id).c_str(), value.c_str());
   #endif
   
   switch (id)
   {
   case COORD_SYSTEM:
      // script do not specify view coordinate system, so do nothing here
      // we want to create local body fixed coord system instead in Initialize()
      break;
   case CENTRAL_BODY:
   {
      if (centralBodyName != value)
      {
         centralBodyName = value;
         // Since ground track data uses body fixed coordinates, name it here
         mViewCoordSysName = value + "Fixed";
         
         // Get default texture map file for the new body
         // Get path from the FileManager (LOJ: 2014.06.18)
         #ifdef DEBUG_TEXTURE_MAP
         MessageInterface::ShowMessage
            ("GroundTrackPlot::SetStringParameter(CENTRAL_BODY) calling SetTextureMapFileName()\n");
         #endif
         SetTextureMapFileName("", false);
         #if DBGLVL_PARAM_STRING
         MessageInterface::ShowMessage
            ("   ==> this = <%p>'%s', textureMapFile changed to '%s'\n", this,
             GetName().c_str(), textureMapFileName.c_str());
         #endif
      }
      return true;
   }
   case TEXTURE_MAP:
   {
      textureMapFileName = value;
      // Get path from the FileManager (LOJ: 2014.06.18)
      #ifdef DEBUG_TEXTURE_MAP
      MessageInterface::ShowMessage
         ("GroundTrackPlot::SetStringParameter(TEXTURE_MAP) calling SetTextureMapFileName()\n");
      #endif
      SetTextureMapFileName(value, true);
      return true;
   }
   case SHOW_FOOT_PRINTS:
      {
         bool itemFound = false;
         int index = -1;
         for (int i = 0; i < FootPrintOptionCount; i++)
         {
            if (value == FOOT_PRINT_OPTION_TEXT[i])
            {
               itemFound = true;
               index = i;
               break;
            }
         }
         
         if (itemFound)
         {
            footPrints = value;
            footPrintOption = (FootPrintOption)index;
            #if DBGLVL_PARAM_STRING
            MessageInterface::ShowMessage("   footPrintOption = %d\n", footPrintOption);
            #endif
            return true;
         }
         else
         {
            SubscriberException se;
            std::string options;
            for (int i = 0; i < FootPrintOptionCount - 1; i++)               
               options = options + FOOT_PRINT_OPTION_TEXT[i] + " ,";
            options = options + FOOT_PRINT_OPTION_TEXT[FootPrintOptionCount - 1];
            
            se.SetDetails(errorMessageFormat.c_str(), value.c_str(),
                          GetParameterText(id).c_str(), options.c_str());
            throw se;
         }
      }
   default:
      ;           // Intentional drop through so all paths return a value
   }
   return OrbitPlot::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const char *value)
//------------------------------------------------------------------------------
bool GroundTrackPlot::SetStringParameter(const std::string &label,
                                         const char *value)
{
   return SetStringParameter(GetParameterID(label), std::string(value));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool GroundTrackPlot::SetStringParameter(const std::string &label,
                                         const std::string &value)
{
   #if DBGLVL_PARAM_STRING
   MessageInterface::ShowMessage
      ("GroundTrackPlot::SetStringParameter()<%s> label=%s, value=%s \n",
       instanceName.c_str(), label.c_str(), value.c_str());
   #endif
   
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& GroundTrackPlot::GetStringArrayParameter(const Integer id) const
{
   static StringArray actualObjectArray;
   switch (id)
   {
   case ADD:
   {
      actualObjectArray.clear();
      Integer objCount = mAllSpNameArray.size();
      for (Integer i = 0; i < objCount; i++)
      {
         if (mAllSpNameArray[i] != centralBodyName)
            actualObjectArray.push_back(mAllSpNameArray[i]);
      }
      return actualObjectArray;
   }
   default:
      return Subscriber::GetStringArrayParameter(id);
   }
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& GroundTrackPlot::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetTypesForList(const Integer id)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of types that need to be shown on a GUI for a parameter
 *
 * @param id The parameter ID
 *
 * @return The list of types
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& GroundTrackPlot::GetTypesForList(const Integer id)
{
   if (id == ADD)
   {
      listedTypes.clear();
      listedTypes.push_back(Gmat::SPACECRAFT);
      listedTypes.push_back(Gmat::GROUND_STATION);
   }

   return listedTypes;
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetTypesForList(const std::string &label)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of types that need to be shown on a GUI for a parameter
 *
 * @param label The parameter's label
 *
 * @return The list of types
 */
//------------------------------------------------------------------------------
const ObjectTypeArray&
      GroundTrackPlot::GetTypesForList(const std::string &label)
{
   return GetTypesForList(GetParameterID(label));
}

//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
std::string GroundTrackPlot::GetRefObjectName(const UnsignedInt type) const
{
   if (type == Gmat::CELESTIAL_BODY)
      return centralBodyName;
   
   return OrbitPlot::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool GroundTrackPlot::HasRefObjectTypeArray()
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
const ObjectTypeArray& GroundTrackPlot::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   return OrbitPlot::GetRefObjectTypeArray();
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& GroundTrackPlot::GetRefObjectNameArray(const UnsignedInt type)
{
   #if DBGLVL_OBJ
   MessageInterface::ShowMessage
      ("GroundTrackPlot::GetRefObjectNameArray() entered, type=%d\n", type);
   #endif
   
   refObjectNames.clear();
   refObjectNames = OrbitPlot::GetRefObjectNameArray(type);
   
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACE_POINT ||
       type == Gmat::CELESTIAL_BODY)
   {      
      #if DBGLVL_OBJ
      MessageInterface::ShowMessage("   Adding '%s' to array\n", centralBodyName.c_str());
      #endif
      refObjectNames.push_back(centralBodyName);
   }
   
   #if DBGLVL_OBJ
   MessageInterface::ShowMessage
      ("GroundTrackPlot::GetRefObjectNameArray() returning for type:%d\n", type);
   for (unsigned int i=0; i<refObjectNames.size(); i++)
      MessageInterface::ShowMessage("   %s\n", refObjectNames[i].c_str());
   #endif
   
   return refObjectNames;
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* GroundTrackPlot::GetRefObject(const UnsignedInt type,
                                        const std::string &name)
{
   // Any ref orbjet declared in this class?
   if (type == Gmat::CELESTIAL_BODY && name == centralBodyName)
      return centralBody;
   
   return OrbitPlot::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Set reference object pointer.
 *
 * @param <obj>  Reference object pointer to set to given object type and name
 * @param <type> Reference object type
 * @param <name> Reference object name
 */
//------------------------------------------------------------------------------
bool GroundTrackPlot::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                   const std::string &name)
{
   #if DBGLVL_OBJ
   MessageInterface::ShowMessage
      ("GroundTrackPlot::SetRefObject() this=<%p>'%s', obj=<%p>'%s', type=%d[%s], name='%s'\n",
       this, GetName().c_str(), obj, obj->GetName().c_str(), type,
       obj->GetTypeName().c_str(), name.c_str());
   #endif
   
   std::string realName = name;
   if (name == "")
      realName = obj->GetName();
   
   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      if (realName == centralBodyName)
         centralBody = (CelestialBody*)obj;
   }
   
   return OrbitPlot::SetRefObject(obj, type, realName);
}


//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// bool UpdateSolverData()
//------------------------------------------------------------------------------
bool GroundTrackPlot::UpdateSolverData()
{
   int size = mCurrEpochArray.size();
   int last = size - 1;
   
   #if DBGLVL_SOLVER_CURRENT_ITER
   MessageInterface::ShowMessage("===> num buffered data = %d\n", size);
   MessageInterface::ShowMessage("==========> now update solver plot\n");
   #endif
   
   if (size == 0)
      return true;

   #if 0
   UnsignedIntArray colorArray = mScOrbitColorArray;
   if (runstate == Gmat::SOLVING)
      colorArray = mScTargetColorArray;
   else
      colorArray = mScOrbitColorArray;
   #endif
   
   // Update plot with last iteration data
   for (int i=0; i<size-1; i++)
   {
      #if DBGLVL_SOLVER_CURRENT_ITER > 1
      for (int sc=0; sc<mScCount; sc++)
         MessageInterface::ShowMessage
            ("   i=%d, sc=%d, solver epoch = %f, X,Y,Z = %f, %f, %f\n", i, sc,
             mCurrEpochArray[i], mCurrXArray[i][sc], mCurrYArray[i][sc],
             mCurrZArray[i][sc]);
      #endif
      
      // Just buffer data up to last point - 1
      PlotInterface::
         UpdateGlPlot(instanceName, mOldName, mCurrScArray[i],
                      mCurrEpochArray[i], mCurrXArray[i], mCurrYArray[i],
                      mCurrZArray[i], mCurrVxArray[i], mCurrVyArray[i],
                      mCurrVzArray[i], mCurrentOrbitColorMap, mCurrentTargetColorMap,
                      true, mSolverIterOption, false, isDataOn);
   }
   
   // Buffer last point and Update the plot
   PlotInterface::
      UpdateGlPlot(instanceName, mOldName, mCurrScArray[last],
                   mCurrEpochArray[last], mCurrXArray[last], mCurrYArray[last],
                   mCurrZArray[last], mCurrVxArray[last], mCurrVyArray[last],
                   mCurrVzArray[last], mCurrentOrbitColorMap, mCurrentTargetColorMap,
                   true, mSolverIterOption, true, isDataOn);
   
   // clear arrays
   mCurrScArray.clear();
   mCurrEpochArray.clear();
   mCurrXArray.clear();
   mCurrYArray.clear();
   mCurrZArray.clear();
   mCurrVxArray.clear();
   mCurrVyArray.clear();
   mCurrVzArray.clear();
   
   if (runstate == Gmat::SOLVING)
   {
      PlotInterface::TakeGlAction(instanceName, "ClearSolverData");
   }
   return true;
}


//------------------------------------------------------------------------------
// virtual std::string GetObjectStringList() const
//------------------------------------------------------------------------------
/**
 * Returns all objects except central body.
 */
//------------------------------------------------------------------------------
std::string GroundTrackPlot::GetObjectStringList() const
{
   Integer objCount = mAllSpNameArray.size();
   std::string objList = "{ ";
   for (Integer i = 0; i < objCount; i++)
   {
      if (mAllSpNameArray[i] != centralBodyName)
      {
         if (i == objCount - 1)
            objList += mAllSpNameArray[i];
         else
            objList += mAllSpNameArray[i] + ", ";
      }
   }
   objList += " }";
   return objList;
}


//------------------------------------------------------------------------------
// bool Distribute(const Real *dat, Integer len)
//------------------------------------------------------------------------------
bool GroundTrackPlot::Distribute(const Real *dat, Integer len)
{
   #if DBGLVL_UPDATE
   MessageInterface::ShowMessage
      ("===========================================================================\n"
       "GroundTrackPlot::Distribute() instanceName=%s, active=%d, isEndOfRun=%d, "
       "isEndOfReceive=%d\n   mAllSpCount=%d, mScCount=%d, len=%d, runstate=%d, "
       "isDataStateChanged=%d\n", instanceName.c_str(), active, isEndOfRun, isEndOfReceive,
       mAllSpCount, mScCount, len, runstate, isDataStateChanged);
   #endif
   
   //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING_NO_PLOTS)            // made changes by TUAN NGUYEN
   if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING_NO_PLOTS)       // made changes by TUAN NGUYEN
      return true;
   
   // if data state changed from on to off or vice versa, update plot data so
   // data points can be flagged.
   if (isDataStateChanged)
   {
      if (isDataOn)
         PlotInterface::TakeGlAction(instanceName, "PenDown");
      else
         PlotInterface::TakeGlAction(instanceName, "PenUp");
      
      isDataStateChanged = false;
   }
   
   if (!active || mScCount <= 0)
      return true;
   
   // test isEndOfRun first
   if (isEndOfRun)
   {
      return PlotInterface::SetGlEndOfRun(instanceName);
   }
   
   if (isEndOfReceive)
   {
      if ((mSolverIterOption == SI_CURRENT) &&
          (runstate == Gmat::SOLVING || runstate == Gmat::SOLVEDPASS))
      {
         UpdateSolverData();
      }
      else
      {
         return PlotInterface::RefreshGlPlot(instanceName);
      }
   }
   
   
   if (len <= 0)
      return true;
   
   
   #if DBGLVL_DATA
   MessageInterface::ShowMessage("%s, len=%d\n", GetName().c_str(), len);
   for (int i=0; i<len; i++)
      MessageInterface::ShowMessage("%.11f  ", dat[i]);
   MessageInterface::ShowMessage("\n");
   #endif
   
   //------------------------------------------------------------
   // if targeting and draw target is None, just return
   //------------------------------------------------------------
   if ((mSolverIterOption == SI_NONE) && (runstate == Gmat::SOLVING))
   {
      #if DBGLVL_UPDATE > 1
      MessageInterface::ShowMessage
         ("   Just returning: SolverIterations is %d and runstate is %d\n",
          mSolverIterOption, runstate);
      #endif
      
      return true;
   }
   
   //------------------------------------------------------------
   // update plot data
   //------------------------------------------------------------
   
   UpdateData(dat, len);
   
   
   //loj: always return true otherwise next subscriber will not call ReceiveData()
   //     in Publisher::Publish()
   return true;
}


//------------------------------------------------------------------------------
// const std::string* GetFootPrintOptionList()
//------------------------------------------------------------------------------
const std::string* GroundTrackPlot::GetFootPrintOptionList()
{
   return FOOT_PRINT_OPTION_TEXT;
}


//---------------------------------------------------------------------------
// UnsignedInt GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
UnsignedInt GroundTrackPlot::GetPropertyObjectType(const Integer id) const
{
   if (id == CENTRAL_BODY)
      return Gmat::CELESTIAL_BODY;
   
   return OrbitPlot::GetPropertyObjectType(id);
}


//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//---------------------------------------------------------------------------
const StringArray& GroundTrackPlot::GetPropertyEnumStrings(const Integer id) const
{
   if (id == SHOW_FOOT_PRINTS)
      return footPrintOptions;
   
   return OrbitPlot::GetPropertyEnumStrings(id);
}


//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const std::string &label) const
//---------------------------------------------------------------------------
const StringArray& GroundTrackPlot::GetPropertyEnumStrings(const std::string &label) const
{
   return GetPropertyEnumStrings(GetParameterID(label));
}

//---------------------------------------------------------------------------
// bool SetTextureMapFileName(const std::string &fileName, bool writeWarning,
//                            bool validateOnly = false)
//---------------------------------------------------------------------------
/**
 * Sets full path name for input fileName. If input fileName does not exist
 * it sets to default body texture file.
 */
//---------------------------------------------------------------------------
bool GroundTrackPlot::SetTextureMapFileName(const std::string &fileName,
                                            bool writeWarning, bool validateOnly)
{
   // Note: Verify that the following bug fix still works
   // (Fix for GMT-4693 LOJ: 2014.09.19)
   
   #ifdef DEBUG_TEXTURE_MAP
   MessageInterface::ShowMessage
      ("\nGroundTrackPlot::SetTextureMapFileName() '%s' entered\n   fileName = '%s'\n   "
       "centralBodyName = '%s', writeWarning = %d, validateOnly = %d\n", GetName().c_str(),
       fileName.c_str(), centralBodyName.c_str(), writeWarning, validateOnly);
   #endif
   
   bool retval = true;
   lastErrorMessage = "";
   FileManager *fm = FileManager::Instance();
   std::string actualFile = fileName;
   std::string actualPath;
   
   #ifdef DEBUG_TEXTURE_MAP
   MessageInterface::ShowMessage("   Calling fm->GetTextureMapFile()\n");
   #endif
   
   bool success = fm->GetTextureMapFile(fileName, centralBodyName, GetName(),
                                        actualFile, actualPath, writeWarning);
   lastErrorMessage = fm->GetLastFilePathMessage() + " texture map file";
   
   if (success)
   {
      if (!validateOnly)
      {
         textureMapFileName = actualFile;
         textureMapFullPath = actualPath;
      }
   }
   else
   {
      retval = false;
      textureMapFullPath = "";
      std::string errMsg = lastErrorMessage;
      lastErrorMessage = "**** ERROR *** " + lastErrorMessage;
   }
   
   #ifdef DEBUG_TEXTURE_MAP
   MessageInterface::ShowMessage
      ("   textureMapFileName = '%s'\n   textureMapFullPath = '%s'\n   lastErrorMessage = '%s'\n",
       textureMapFileName.c_str(), textureMapFullPath.c_str(), lastErrorMessage.c_str());
   MessageInterface::ShowMessage
      ("GroundTrackPlot::SetTextureMapFileName() returning %d\n\n", retval);
   #endif
   
   return retval;
}
