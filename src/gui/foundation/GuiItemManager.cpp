//$Id$
//------------------------------------------------------------------------------
//                              GuiItemManager
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun
// Created: 2004/02/06
//
// Modified:
// 2013.08.29 Linda Jun - Changed all wxListBox to show horizontal scrollbar if
//                        contents are too wide (It works on Windows only). 
/**
 * Implements GuiItemManager class.
 */
//------------------------------------------------------------------------------

#include "GuiItemManager.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "gmatdefs.hpp"           //put this one after GUI includes
#include "Parameter.hpp"
#include "Array.hpp"
#include "ParameterInfo.hpp"
#include "SpacePoint.hpp"
#include "Hardware.hpp"
#include "GmatGlobal.hpp"         // for GetDataPrecision()
#include "StringUtil.hpp"         // for GmatStringUtil::
#include "FileManager.hpp"        // for GetFullPathname()
#include "FileUtil.hpp"           // for DoesDirectoryExist()
#include "MessageInterface.hpp"
#include <algorithm>              // for sort(), set_difference()
#include <sstream>
#include "GmatPanel.hpp"
#include "GmatStaticBoxSizer.hpp"
#include <wx/config.h>

//#define DEBUG_LOAD_ICON
//#define DEBUG_GUI_ITEM_VALIDATE
//#define DEBUG_OBJECT_TYPE_COMBOBOX
//#define DEBUG_PROPERTY_LISTBOX
//#define DEBUG_PARAM_SIZER 2
//#define DBGLVL_GUI_ITEM 1
//#define DBGLVL_GUI_ITEM_UPDATE 2
//#define DBGLVL_GUI_ITEM_REG 1
//#define DBGLVL_GUI_ITEM_UNREG 1
//#define DBGLVL_GUI_ITEM_PARAM 2
//#define DBGLVL_GUI_ITEM_PROPERTY 2
//#define DBGLVL_GUI_ITEM 2
//#define DBGLVL_GUI_ITEM_EL 2
//#define DBGLVL_GUI_ITEM_FN 2
//#define DBGLVL_GUI_ITEM_SO 2
//#define DBGLVL_GUI_ITEM_SC 2
//#define DBGLVL_GUI_ITEM_SP 2
//#define DBGLVL_GUI_ITEM_CS 2
//#define DBGLVL_GUI_ITEM_GS 2
//#define DBGLVL_GUI_ITEM_HW 2
//#define DBGLVL_GUI_ITEM_POWER_SYSTEM 2
//#define DBGLVL_GUI_ITEM_BURN 2
//#define DBGLVL_GUI_ITEM_SUBS 2
//#define DBGLVL_GUI_ITEM_SOLVER 2
//#define DBGLVL_GUI_ITEM_PROP 2
//#define DBGLVL_GUI_ITEM_FM 2
//#define DBGLVL_GUI_ITEM_ALL_OBJECT 2
//#define DBGLVL_SC_LISTBOX 2

//------------------------------
// static data
//------------------------------
GuiItemManager* GuiItemManager::theInstance = NULL;

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
//  GuiItemManager* GetInstance()
//------------------------------------------------------------------------------
/**
 * @return Instance of the GuiItemManager.
 */
//------------------------------------------------------------------------------
GuiItemManager* GuiItemManager::GetInstance()
{
   if (theInstance == NULL)
   {
      theInstance = new GuiItemManager();
   }
   return theInstance;
}


//------------------------------------------------------------------------------
// void LoadIcon(const wxString &filename, long bitmapType, wxBitmap *bitmap,
//               const char* xpm[]
//------------------------------------------------------------------------------
/**
 * Loads icon from image file if available if not from xpm file.
 *
 * @param  filename  Image file name
 * @param  bitmapType  Image bitmap type
 * @param  bitmap  wxBitmap pointer to receive loaded bitmap image
 * @param  xpm  xpm bytes to be used if image file is unavailable
 */
//------------------------------------------------------------------------------
void GuiItemManager::LoadIcon(const wxString &filename, long bitmapType,
                              wxBitmap **bitmap, const char* xpm[])
{
   #ifdef DEBUG_LOAD_ICON
   MessageInterface::ShowMessage
      ("GuiItemManager::LoadIcon() entered, filename='%s', bitmap=<%p>\n",
       filename.WX_TO_C_STRING, bitmap);
   #endif
   
   if (bitmapType == wxBITMAP_TYPE_PNG && !mPngHandlerLoaded)
   {
      FileManager *fm = FileManager::Instance();
      std::string loc = fm->GetFullPathname("ICON_PATH");
      wxString    locWx = STD_TO_WX_STRING(loc.c_str());
      
      #ifdef DEBUG_LOAD_ICON
      MessageInterface::ShowMessage("   loc = '%s'\n", loc.c_str());
      #endif
      
      // Check if icon file directory exist
      if (GmatFileUtil::DoesDirectoryExist(loc, false))
      {
         #ifdef DEBUG_LOAD_ICON
         MessageInterface::ShowMessage("   Loading images from '%s'\n", loc.c_str());
         MessageInterface::ShowMessage("   Loading .png files\n");
         #endif
         
         wxImage::AddHandler(new wxPNGHandler);
         mPngHandlerLoaded = true;
         mPngIconLocation = locWx;
      }
   }
      
   wxImage iconImage;
   wxString fileType = ".png";
   wxString fullFileName = mPngIconLocation + filename + fileType;
   #ifdef DEBUG_LOAD_ICON
   MessageInterface::ShowMessage("   fullFileName = '%s'\n", fullFileName.WX_TO_C_STRING);
   #endif
   if (mPngHandlerLoaded && GmatFileUtil::DoesFileExist(fullFileName.c_str()))
   {
      iconImage.LoadFile(fullFileName, (wxBitmapType)bitmapType);
      #ifdef DEBUG_LOAD_ICON
      MessageInterface::ShowMessage("   creating bitmap from png image\n");
      #endif
      *bitmap = new wxBitmap(iconImage);
   }
   else
   {
      #ifdef DEBUG_LOAD_ICON
      MessageInterface::ShowMessage("   creating bitmap from xpm file\n");
      #endif
      *bitmap = new wxBitmap(xpm);
   }
   
   #ifdef DEBUG_LOAD_ICON
   MessageInterface::ShowMessage("GuiItemManager::LoadIcon() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// int GetGuiStatus()
//------------------------------------------------------------------------------
/**
 * Retrieves GUI status: 1 = clean, 2 = dirty, 3 = error
 */
//------------------------------------------------------------------------------
int GuiItemManager::GetGuiStatus()
{
   return mGuiStatus;
}


//------------------------------------------------------------------------------
// void SetGuiStatus(int status)
//------------------------------------------------------------------------------
/**
 * Sets status of GUI
 *
 * @param  status  1 = clean, 2 = dirty, 3 = error
 */
//------------------------------------------------------------------------------
void GuiItemManager::SetGuiStatus(int status)
{
   mGuiStatus = status;
}


//------------------------------------------------------------------------------
// int  GetActiveScriptStatus()
//------------------------------------------------------------------------------
/**
 * Retrieves active script status: 1 = clean, 2 = dirty, 3 = error
 */
//------------------------------------------------------------------------------
int GuiItemManager::GetActiveScriptStatus()
{
   return mActiveScriptStatus;
}


//------------------------------------------------------------------------------
// void SetActiveScriptStatus(int status)
//------------------------------------------------------------------------------
/**
 * Sets status of active script
 *
 * @param  status  1 = clean, 2 = dirty, 3 = error
 */
//------------------------------------------------------------------------------
void GuiItemManager::SetActiveScriptStatus(int status)
{
   mActiveScriptStatus = status;
}


//------------------------------------------------------------------------------
// wxString ToWxString(Real rval)
//------------------------------------------------------------------------------
wxString GuiItemManager::ToWxString(Real rval)
{
   wxString str;
   std::stringstream ss;
   ss.precision(theDataPrecision);
   
   ss << rval;
   str.Printf("%s", ss.str().c_str());
   return str;
}

//------------------------------------------------------------------------------
// wxString ToWxString(Integer ival)
//------------------------------------------------------------------------------
wxString GuiItemManager::ToWxString(Integer ival)
{
   wxString str;
   std::stringstream ss;
   
   ss << ival;
   str.Printf("%s", ss.str().c_str());
   return str;
}


//------------------------------------------------------------------------------
// wxArrayString ToWxArrayString(const StringArray &array)
//------------------------------------------------------------------------------
/**
 * Converts std::string array to wxString array.
 */
//------------------------------------------------------------------------------
wxArrayString GuiItemManager::ToWxArrayString(const StringArray &array)
{
   wxArrayString newArray;
   for (UnsignedInt i=0; i<array.size(); i++)
      newArray.Add(array[i].c_str());
   
   return newArray;
}


//------------------------------------------------------------------------------
// wxString GetLastErrorMessage()
//------------------------------------------------------------------------------
wxString GuiItemManager::GetLastErrorMessage()
{
   return mLastErrorMsg;
}


//------------------------------------------------------------------------------
// void SetLastErrorMessage(const wxString &errMsg)
//------------------------------------------------------------------------------
void GuiItemManager::SetLastErrorMessage(const wxString &errMsg)
{
   mLastErrorMsg = errMsg;
}


//------------------------------------------------------------------------------
// int IsValidObjectProperty(const std::string &varName, UnsignedInt allowedType,
//                           bool allowNonPlottable = true)
//------------------------------------------------------------------------------
/**
 * Checks if input name has valid object field of exisiting object.
 *
 * @return  1 if varName contains valid object field
 *          0 if varName contains no object field
 *          3 if varName contains undefined object
 *          6 if varName contains invalid object field
 */
//------------------------------------------------------------------------------
int GuiItemManager::IsValidObjectProperty(const std::string &varName,
                                          UnsignedInt allowedType,
                                          bool allowNonPlottable)
{
   #ifdef DEBUG_GUI_ITEM_VALIDATE
   MessageInterface::ShowMessage
      ("GuiItemManager::IsValidObjectProperty() entered, varName='%s', allowedType=%d, "
       "allowNonPlottable=%d\n", varName.c_str(), allowedType, allowNonPlottable);
   #endif
   
   std::string type, ownerName, depObj;
   GmatStringUtil::ParseParameter(varName, type, ownerName, depObj);
   
   // The name has no object field
   if (type == "")
   {
      #ifdef DEBUG_GUI_ITEM_VALIDATE
      MessageInterface::ShowMessage
         ("GuiItemManager::IsValidObjectProperty() returning -1, the name "
          "has no object field\n");
      #endif
      mLastErrorMsg = "the name has no object field";
      return -1;
   }
   
   if (depObj != "")
   {
      #ifdef DEBUG_GUI_ITEM_VALIDATE
      MessageInterface::ShowMessage
         ("GuiItemManager::IsValidObjectProperty() returning -1, the name "
          "has more than one object field\n");
      #endif
      mLastErrorMsg = "the name has more than one object field";
      return -1;
   }
   
   GmatBase *obj = theGuiInterpreter->GetConfiguredObject(ownerName);
   
   // Owner object not found
   if (obj == NULL)
   {
      #ifdef DEBUG_GUI_ITEM_VALIDATE
      MessageInterface::ShowMessage
         ("GuiItemManager::IsValidObjectProperty() returning 0, cannot find "
          "object named '%s'\n", ownerName.c_str());
      #endif
      mLastErrorMsg = ("cannot find object named \"" + ownerName + "\"").c_str();
      return -1;
   }
   
   GmatBase *owner;
   Integer id;
   Gmat::ParameterType paramType;
   bool fieldFound =
      theGuiInterpreter->FindPropertyID(obj, varName, &owner, id, paramType);
   
   int validStatus = -1;
   if (fieldFound)
      validStatus = 1;
   else
      mLastErrorMsg = "invalid object field";
   
   #ifdef DEBUG_GUI_ITEM_VALIDATE
   MessageInterface::ShowMessage
      ("GuiItemManager::IsValidObjectProperty() returning %d, %s "
       "object field '%s'\n", validStatus, fieldFound ? "found" : "NOT found",
       type.c_str());
   #endif
   
   return validStatus;
}


//------------------------------------------------------------------------------
// int IsValidParameter(const std::string &varName, UnsignedInt allowedType,
//                      bool allowNonPlottable = true, bool allowWholeArray = false)
//------------------------------------------------------------------------------
/**
 * Checks if input variable is a Variable, Array element, or plottable
 * parameter of input owner type. The plottable parameter returns Real number.
 *
 * @param  varName  input variable name
 * @param  allowedType  input allowed owner type (such as Gmat::SPACECRAFT)
 * @param  allowNumber  true if varName can be a Real number 
 * @param  allowNonPlottable  true if varName can be a non-plottable
 * @param  allowWholeArray  true if varName can be an whole array [false]
 *
 * @return  -1 if error found with varName such as varName found in the configuration or
 *                        invalid array index
 *           0 if varName found BUT is not one of Variable, Array element, or parameter
 *           1 if varName found AND is one of Variable, Array element, or parameter OR
 *                        it is array and allowWholeArray is true
 *
 * @note mLastErrorMsg contains the detailed error message
 */
//------------------------------------------------------------------------------
int GuiItemManager::IsValidParameter(const std::string &varName,
                                     UnsignedInt allowedType,
                                     bool allowNonPlottable, bool allowWholeArray)
{
   #ifdef DEBUG_GUI_ITEM_VALIDATE
   MessageInterface::ShowMessage
      ("GuiItemManager::IsValidParameter() entered, varName=<%s>, allowedType=%d,\n   "
       "allowNonPlottable=%d, allowWholeArray=%d\n", varName.c_str(), allowedType,
       allowNonPlottable, allowWholeArray);
   #endif
   
   std::string nameToUse = GmatStringUtil::GetArrayName(varName);
   GmatBase *obj = theGuiInterpreter->GetConfiguredObject(nameToUse);
   
   if (obj == NULL)
   {
      mLastErrorMsg = ("cannot find object named \"" + nameToUse + "\"").c_str();
      #ifdef DEBUG_GUI_ITEM_VALIDATE
      MessageInterface::ShowMessage
         ("GuiItemManager::IsValidParameter() returning -1, cannot find object\n");
      #endif
      return -1;
   }
   
   if (!obj->IsOfType("Parameter"))
   {
      #ifdef DEBUG_GUI_ITEM_VALIDATE
      MessageInterface::ShowMessage
         ("GuiItemManager::IsValidParameter() returning 0, it is not type of Parameter\n");
      #endif
      return 0;
   }
   
   Parameter *param = (Parameter*)obj;
   bool isValid = false;
   
   #ifdef DEBUG_GUI_ITEM_VALIDATE
   MessageInterface::ShowMessage
      ("   Object name '%s'<%p> found\n", obj->GetName().c_str(), obj);
   #endif
   
   if (param->IsOfType("String"))
   {
      if (allowNonPlottable)
         isValid = true;
   }
   else if (param->IsOfType("Variable"))
   {
      isValid = true;
   }
   else if (param->IsOfType("Array"))
   {
      Array *arr = (Array*)param;
      Integer rowCount = arr->GetRowCount();
      Integer colCount = arr->GetColCount();
      #ifdef DEBUG_GUI_ITEM_VALIDATE
      MessageInterface::ShowMessage
         ("   rowCount=%d, colCount=%d\n", rowCount, colCount);
      #endif
      // check to see if it is array element or whole array of 1x1
      if (GmatStringUtil::IsParenPartOfArray(varName))
      {
         // Check for array index
         std::string name;
         Integer row, col;
         GmatStringUtil::GetArrayIndex(varName, row, col, name);
         #ifdef DEBUG_GUI_ITEM_VALIDATE
         MessageInterface::ShowMessage
            ("   name='%s', row=%d, col=%d\n", name.c_str(), row, col);
         #endif
         
         // We need to allow Variable index such as array(I,J), so check it
         if (row == -1 && col == -1)
            isValid = true;
         else if ((row >= 0 && row < rowCount) && col == -1)
            isValid = true;
         else if ((col >= 0 && col < colCount) && row == -1)
            isValid = true;
         else if ((row >= 0 && row < rowCount) && (col >= 0 && col < colCount))
            isValid = true;
         else
         {
            mLastErrorMsg = "invalid array index";
            #ifdef DEBUG_GUI_ITEM_VALIDATE
            MessageInterface::ShowMessage
               ("GuiItemManager::IsValidParameter() returning -1, array index is invalid\n");
            #endif
            return -1;
         }
      }
      else
      {
         if (arr->GetRowCount() == 1 && arr->GetColCount() == 1)
            isValid = true;
         else if (allowWholeArray)
            isValid = true;
      }
   }
   else if (param->GetKey() == GmatParam::SYSTEM_PARAM)
   {
      std::string ownerName = param->GetStringParameter("Object");
      GmatBase *owner = theGuiInterpreter->GetConfiguredObject(ownerName);
      UnsignedInt ownerType = allowedType;
      
      // If we don't want to check for the type, just use owner type
      if (allowedType == Gmat::UNKNOWN_OBJECT)
         ownerType = owner->GetType();
      
      #ifdef DEBUG_GUI_ITEM_VALIDATE
      MessageInterface::ShowMessage
         ("   It is a system Parameter, owner='%s'<%p>\n", ownerName.c_str(), owner);
      #endif
      
      if (allowNonPlottable)
      {
         #ifdef DEBUG_GUI_ITEM_VALIDATE
         MessageInterface::ShowMessage
            ("   Allowing non-plottable Parameter, so checking for owner type %d\n",
             param->GetOwnerType());
         #endif
         
         // check to see if it is parameter of owner type
         if (owner && owner->IsOfType(ownerType))
            isValid = true;
      }
      else
      {
         #ifdef DEBUG_GUI_ITEM_VALIDATE
         MessageInterface::ShowMessage
            ("   Not allowing non-plottable Parameter, so checking for owner type %d "
             "and plottable %d\n", param->GetOwnerType(), param->IsPlottable());
         #endif
         // check to see if it is parameter of owner type and plottable
         if (owner && owner->IsOfType(ownerType) && param->IsPlottable())
            isValid = true;
      }
   }
   
   int validStatus = 0;
   if (isValid)
      validStatus = 1;
   
   #ifdef DEBUG_GUI_ITEM_VALIDATE
   MessageInterface::ShowMessage
      ("GuiItemManager::IsValidParameter() returning %d\n", validStatus);
   #endif
   return validStatus;
}


//------------------------------------------------------------------------------
// int IsValidVariable(const std::string &varName, UnsignedInt allowedType,
//           bool allowNumber, bool allowNonPlottable, bool allowObjectProperty,
//           bool allowWholeArray)
//------------------------------------------------------------------------------
/*
 * Checks if input variable is a Variable, Array element, or plottable
 * parameter of input owner type. The plottable parameter returns Real number.
 *
 * @param  varName  input variable name
 * @param  allowedType  input allowed owner type (such as Gmat::SPACECRAFT)
 * @param  allowNumber  true if varName can be a Real number [false]
 * @param  allowNonPlottable  true if varName can be a non-plottable [false]
 * @param  allowObjectProperty  true if varName can be an object property [false]
 * @param  allowWholeArray  true if varName can be an whole array [false]
 *
 * @return -1 if error found in varName (Use GetLastErrorMessage() to retrieve error message)
 *          0 if varName found BUT is not one of Variable, Array element, or parameter 
 *          1 if varName found AND is one of Variable, Array element, or parameter OR
 *                       it is a whole array and allowWholeArray is true
 *          2 if number is allowed and varName is Real number (this is not an error)
 *          3 if varName contains undefined object of Parameter type
 *          4 if varName contains valid Parameter type
 *          5 if varName found BUT array element is out of bounds
 */
//------------------------------------------------------------------------------
int GuiItemManager::IsValidVariable(const std::string &varName,
                                    UnsignedInt allowedType, bool allowNumber,
                                    bool allowNonPlottable, bool allowObjectProperty,
                                    bool allowWholeArray)
{
   #ifdef DEBUG_GUI_ITEM_VALIDATE
   MessageInterface::ShowMessage
      ("GuiItemManager::IsValidVariable() entered, varName=<%s>, allowedType=%d,\n   "
       "allowNumber=%d, allowNonPlottable=%d, allowObjectProperty=%d, allowWholeArray=%d\n",
       varName.c_str(), allowedType, allowNumber, allowNonPlottable, allowObjectProperty,
       allowWholeArray);
   #endif
   
   if (allowNumber)
   {
      // If input string is a number, just return 2
      Real rval;
      if (GmatStringUtil::ToReal(varName.c_str(), rval))
      {
         #ifdef DEBUG_GUI_ITEM_VALIDATE
         MessageInterface::ShowMessage
            ("GuiItemManager::IsValidVariable() returning 2, it is a number\n");
         #endif
         return 2;
      }
   }
   
   std::string type, ownerName, depObj;
   GmatStringUtil::ParseParameter(varName, type, ownerName, depObj);
   Integer numberOfDots = GmatStringUtil::NumberOfOccurrences(varName, '.');
   
   #ifdef DEBUG_GUI_ITEM_VALIDATE
   MessageInterface::ShowMessage
      ("   type=<%s>, ownerName=<%s>, depObj=<%s>, numberOfDots=%d\n", varName.c_str(),
       type.c_str(), ownerName.c_str(), depObj.c_str(), numberOfDots);
   #endif
      
   int validStatus = 0;
   bool checkForParameter = false;
   GmatBase *obj = theGuiInterpreter->GetConfiguredObject(varName);
   Parameter *param = NULL;
   
   // If name has a system Parameter type and owner object exist, create
   if (obj == NULL)
   {
      #ifdef DEBUG_GUI_ITEM_VALIDATE
      MessageInterface::ShowMessage
         ("   Object name '%s' not found\n", varName.c_str());
      #endif
      if (type != "")
      {
         ParameterInfo *paramInfo = ParameterInfo::Instance();
         GmatParam::DepObject depType = paramInfo->GetDepObjectType(type);
         UnsignedInt ownedObjType = paramInfo->GetOwnedObjectType(type);
         bool isParameter = theGuiInterpreter->IsParameter(type);
         if (isParameter && depType == GmatParam::ATTACHED_OBJ && numberOfDots != 2)
            isParameter = false;
         
         #ifdef DEBUG_GUI_ITEM_VALIDATE
         MessageInterface::ShowMessage
            ("   Now checking if Parameter can be created, depType=%d, ownedObjType=%d\n",
             depType, ownedObjType);
         #endif
         if (isParameter)
         {
            if (theGuiInterpreter->GetConfiguredObject(ownerName))
            {
               #ifdef DEBUG_GUI_ITEM_VALIDATE
               MessageInterface::ShowMessage
                  ("   Creating system Parameter '%s'\n", varName.c_str());
               #endif
               //Parameter *param = theGuiInterpreter->CreateSystemParameter(varName);
               param = theGuiInterpreter->CreateSystemParameter(varName);
               if (param == NULL)
               {
                  #ifdef DEBUG_GUI_ITEM_VALIDATE
                  MessageInterface::ShowMessage
                     ("**** Unable to create Parameter of '%s'\n", varName.c_str());
                  #endif
               }
               else
               {
                  #ifdef DEBUG_GUI_ITEM_VALIDATE
                  MessageInterface::ShowMessage("   Parameter '%s' created\n", varName.c_str());
                  #endif
                  checkForParameter = true;
               }
            }
            else
               validStatus = 3;
         }
         else
         {
            #ifdef DEBUG_GUI_ITEM_VALIDATE
            MessageInterface::ShowMessage
               ("   '%s' is not a Parameter type, so checking if it is an object property\n",
                type.c_str());
            #endif
            if (allowObjectProperty)
               validStatus = IsValidObjectProperty(varName, allowedType, allowNonPlottable);
            else
            {
               validStatus = -1;
               mLastErrorMsg = "invalid object field";
            }
         }
      }
      else
      {
         // type is blank, so it may be a Variable, String, or Array
         checkForParameter = true;
         validStatus = -1;
      }
   }
   else
   {
      checkForParameter = true;
      param = (Parameter*)obj;
      #ifdef DEBUG_GUI_ITEM_VALIDATE
      MessageInterface::ShowMessage
         ("   obj<%p><%p>'%s' found\n", obj, obj->GetTypeName().c_str(),
          obj->GetName().c_str());
      #endif
   }
   
   if (!checkForParameter)
   {
      #ifdef DEBUG_GUI_ITEM_VALIDATE
      MessageInterface::ShowMessage
         ("GuiItemManager::IsValidVariable() returning %d since not checking "
          "for a Parameter\n", validStatus);
      #endif
      return validStatus;
   }
   
   // Now check for valid Parameter
   #ifdef DEBUG_GUI_ITEM_VALIDATE
   MessageInterface::ShowMessage
      ("   Now checking if it is valid Parameter, param=<%p>'%s'\n",
       param, param ? param->GetName().c_str() : "NULL");
   #endif
   
   // Check for the dependency or owned object
   if (depObj != "" && theGuiInterpreter->GetConfiguredObject(depObj) == NULL)
   {
      // Since middle field is dependency or owned object name,
      // check if middle field is optional
      if (param->IsOptionalField(depObj))
      {
         #ifdef DEBUG_GUI_ITEM_VALIDATE
         MessageInterface::ShowMessage
            ("   '%s' is optional field so ignored\n", depObj.c_str());
         #endif
      }
      else
      {
         mLastErrorMsg = "cannot find owned object named \"";
         mLastErrorMsg = mLastErrorMsg + depObj.c_str() + "\"";
         #ifdef DEBUG_GUI_ITEM_VALIDATE
         MessageInterface::ShowMessage
            ("GuiItemManager::IsValidVariable() returning -1, cannot find owned object\n");
         #endif
         return -1;
      }
   }
   
   
   validStatus = IsValidParameter(varName, allowedType, allowNonPlottable, allowWholeArray);
   
   #ifdef DEBUG_GUI_ITEM_VALIDATE
   MessageInterface::ShowMessage
      ("GuiItemManager::IsValidVariable() returning %d\n", validStatus);
   #endif
   
   return validStatus;
}


//------------------------------------------------------------------------------
//  void UpdateAll(UnsignedInt objType = UNKNOWN_OBJECT)
//------------------------------------------------------------------------------
/**
 * Updates all objects.
 *
 * @param  objType  Type of the object to be updated
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateAll(UnsignedInt objType)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage
      ("==================> GuiItemManager::UpdateAll() entered, objType=%d\n",
       objType);
   #endif
   
   // Update specific object type and all object list
   if (objType != Gmat::UNKNOWN_OBJECT)
   {
      switch (objType)
      {
      case Gmat::GROUND_STATION:
         UpdateGroundStation(true);
         break;
      case Gmat::SPACECRAFT:
         UpdateSpacecraft(true);
         break;
      case Gmat::FORMATION:
         UpdateFormation(true);
         break;
      case Gmat::HARDWARE:
      case Gmat::FUEL_TANK:
      case Gmat::CHEMICAL_FUEL_TANK:
      case Gmat::THRUSTER:
      case Gmat::CHEMICAL_THRUSTER:
         UpdateFuelTank(true);
         UpdateThruster(true);
      case Gmat::POWER_SYSTEM:
         UpdatePowerSystem(true);
         break;
      case Gmat::IMAGER:
      case Gmat::SENSOR:
         UpdateImager(true);
         break;
      case Gmat::BURN:
      case Gmat::IMPULSIVE_BURN:
      case Gmat::FINITE_BURN:
         UpdateBurn(true);
         break;
      case Gmat::PARAMETER:
         UpdateParameter(true);
         break;
      case Gmat::CALCULATED_POINT:
      case Gmat::SOLAR_SYSTEM:
         UpdateSolarSystem(true);
         break;
      case Gmat::COORDINATE_SYSTEM:
         UpdateCoordSystem(true);
         break;
      case Gmat::PROP_SETUP:
      case Gmat::PROPAGATOR:
         UpdatePropagator(true);
         break;
      case Gmat::FUNCTION:
         UpdateFunction(true);
         break;
      case Gmat::SUBSCRIBER:
         UpdateSubscriber(true);
         break;
      case Gmat::SOLVER:
         UpdateSolver(true);
         break;
      case Gmat::EVENT_LOCATOR:
         UpdateLocator(true);
         break;
      case Gmat::INTERFACE:
         UpdateInterface(true);
         break;
      default:
         MessageInterface::ShowMessage
            ("*** INTERNAL ERROR *** GuiItemManager::UpdateAll() the object type "
             "%d ('%s') has not been implemented yet.\n", objType,
             GmatBase::GetObjectTypeString(objType).c_str());
         break;
      }
      return;
   }
   
   // Handle all object types
   UpdateCelestialPoint(false); // All CelestialBodies and CalculatedPoints
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateCelestialPoint()\n");
   #endif
   
   UpdateFormation(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateFormation()\n");
   #endif
   
   UpdateSpacecraft(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateSpacecraft()\n");
   #endif
   
   UpdateBurn(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateBurn()\n");
   #endif
   
   UpdateParameter(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateParameter()\n");
   #endif
   
   UpdateSolarSystem(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateSolarSystem()\n");
   #endif
   
   UpdateCoordSystem(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateCoordSystem()\n");
   #endif
   
   UpdatePropagator(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdatePropagator()\n");
   #endif
   
   UpdateForceModel(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateForceModel()\n");
   #endif
   
   UpdateFuelTank(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateFuelTank()\n");
   #endif
   
   UpdateThruster(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateThruster()\n");
   #endif
   
   UpdatePowerSystem(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdatePowerSystem()\n");
   #endif

   UpdateGroundStation(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateGroundStation()\n");
   #endif
   
   UpdateImager(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateSensor()\n");
   #endif
   
   UpdateFunction(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateFunction()\n");
   #endif
   
   UpdateSubscriber(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateSubscriber()\n");
   #endif
   
   UpdateSolver(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateSolver()\n");
   #endif
   
   UpdateLocator(false);
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("======> after UpdateLocator()\n");
   #endif

   RefreshAllObjectArray();
   
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage
      ("==================> GuiItemManager::UpdateAll() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
//  void UpdateGroundStation(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates GroundStation gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateGroundStation(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateGroundStation\n");
   #endif
   
   UpdateGroundStationList();
   UpdateSpacePointList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}


//------------------------------------------------------------------------------
//  void UpdateCelestialPoint(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates CelestialBody and CalculatedPoint objects.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateCelestialPoint(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateCelestialPoint\n");
   #endif
   
   UpdateCelestialPointList();
   UpdateSpacePointList();
   if (updateObjectArray)
      RefreshAllObjectArray();
   // update the listening GmatPanels
   for (UnsignedInt i=0; i<mResourceUpdateListeners.size(); i++)
   {
	   mResourceUpdateListeners[i]->RefreshObjects(Gmat::CELESTIAL_BODY);
   }
}


//------------------------------------------------------------------------------
//  void UpdateFormation(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates Formation related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateFormation(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateFormation\n");
   #endif
   
   UpdateFormationList();
   UpdateSpaceObjectList();
   UpdateSpacePointList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}


//------------------------------------------------------------------------------
//  void UpdateForceModel(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates parameter related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateForceModel(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateForceModel\n");
   #endif
   
   UpdateForceModelList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}


//------------------------------------------------------------------------------
//  void UpdateSpacecraft(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates spacecraft related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpacecraft(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateSpacecraft\n");
   #endif
   
   UpdateSpacecraftList();
   UpdateSpaceObjectList();
   UpdateSpacePointList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}


//------------------------------------------------------------------------------
//  void UpdateSpacePoint(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates spacecraft related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpacePoint(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateSpacePoint\n");
   #endif
   
   UpdateSpacePointList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}


//------------------------------------------------------------------------------
//  void UpdateBurn(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates burn related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateBurn(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateBurn\n");
   #endif
   
   UpdateBurnList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}


//------------------------------------------------------------------------------
//  void UpdateCoordSystem(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates CoordinateSystem related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateCoordSystem(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateCoordSystem\n");
   #endif
   
   UpdateCoordSystemList();
   if (updateObjectArray)
      RefreshAllObjectArray();
   // update the listening GmatPanels
   for (UnsignedInt i=0; i<mResourceUpdateListeners.size(); i++)
   {
      mResourceUpdateListeners[i]->RefreshObjects(Gmat::COORDINATE_SYSTEM);
   }
}


//------------------------------------------------------------------------------
//  void UpdateFunction(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates Function related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateFunction(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateFunction\n");
   #endif
   
   UpdateFunctionList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}


//------------------------------------------------------------------------------
//  void UpdateFuelTank(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates FuelTank gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateFuelTank(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateFuelTank\n");
   #endif
   
   UpdateFuelTankList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}


//------------------------------------------------------------------------------
//  void UpdateThruster(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates Thruster gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateThruster(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateThruster\n");
   #endif
   
   UpdateThrusterList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}

//------------------------------------------------------------------------------
//  void UpdatePowerSystem(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates Power System gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdatePowerSystem(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdatePowerSystem\n");
   #endif

   UpdatePowerSystemList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}


//------------------------------------------------------------------------------
//  void UpdateSensor(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates Imager gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateImager(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateSensor\n");
   #endif

   // Always update antenna first, since sensor includes Antennas and RFHardwares
   UpdateAntennaList();
   UpdateSensorList();
   UpdateImagerList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}


//------------------------------------------------------------------------------
//  void UpdateParameter(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates parameter related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateParameter(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateParameter\n");
   #endif
   
   UpdateParameterList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}


//------------------------------------------------------------------------------
//  void UpdatePropagator(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates parameter related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdatePropagator(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdatePropagator\n");
   #endif
   
   UpdatePropagatorList();
   UpdateForceModelList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}


//------------------------------------------------------------------------------
//  void UpdateSolarSystem(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates SolarSystem related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSolarSystem(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateSolarSystem\n");
   #endif
   
   UpdateCelestialBodyList();
   UpdateCelestialPointList();
   UpdateSpacePointList();
   if (updateObjectArray)
      RefreshAllObjectArray();
   // update the listening GmatPanels
   for (UnsignedInt i=0; i<mResourceUpdateListeners.size(); i++)
   {
	   mResourceUpdateListeners[i]->RefreshObjects(Gmat::SOLAR_SYSTEM);
   }
}


//------------------------------------------------------------------------------
//  void UpdateSolver(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates Solver related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSolver(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateSolver\n");
   #endif
   
   UpdateSolverList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}


//------------------------------------------------------------------------------
//  void UpdateSubscriber(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates Subscriber related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSubscriber(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateSubscriber\n");
   #endif
   
   UpdateSubscriberList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}

//------------------------------------------------------------------------------
// void UpdateLocator(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates event locator related GUI components
 *
 * @param updateObjectArray TBD
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateLocator(bool updateObjectArray)
{
   #if DBGLVL_GUI_ITEM_UPDATE
      MessageInterface::ShowMessage("===> UpdateLocator\n");
   #endif

   UpdateLocatorList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}


//------------------------------------------------------------------------------
// void UpdateInterface(bool updateObjectArray = true)
//------------------------------------------------------------------------------
/**
 * Updates interface related GUI components
 *
 * @param updateObjectArray TBD
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateInterface(bool updateObjectArray)
{
   UpdateInterfaceList();
   if (updateObjectArray)
      RefreshAllObjectArray();
}

//------------------------------------------------------------------------------
// void AddToResourceUpdateListeners(GmatPanel *panel)
//------------------------------------------------------------------------------
void GuiItemManager::AddToResourceUpdateListeners(GmatPanel *panel)
{
   mResourceUpdateListeners.push_back(panel);
}


//------------------------------------------------------------------------------
// void RemoveFromResourceUpdateListeners(GmatPanel *panel)
//------------------------------------------------------------------------------
void GuiItemManager::RemoveFromResourceUpdateListeners(GmatPanel *panel)
{
   std::vector<GmatPanel*>::iterator pos1 =
      find(mResourceUpdateListeners.begin(), mResourceUpdateListeners.end(), panel);
   
   if (pos1 != mResourceUpdateListeners.end())
      mResourceUpdateListeners.erase(pos1);
}


//------------------------------------------------------------------------------
// virtual bool PrepareObjectNameChange()
//------------------------------------------------------------------------------
bool GuiItemManager::PrepareObjectNameChange()
{
   for (UnsignedInt i=0; i<mResourceUpdateListeners.size(); i++)
   {
      if (!mResourceUpdateListeners[i]->PrepareObjectNameChange())
         return false;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// virtual void NotifyObjectNameChange(UnsignedInt type,
//                                     const wxString &oldName,
//                                     const wxString &newName)
//------------------------------------------------------------------------------
void GuiItemManager::NotifyObjectNameChange(UnsignedInt type,
                                            const wxString &oldName,
                                            const wxString &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("GuiItemManager::NotifyObjectNameChange() entered, type=%d, oldName='%s', "
       "newName='%s'\n", type, oldName.c_str(), newName.c_str());
   #endif
   
   for (UnsignedInt i=0; i<mResourceUpdateListeners.size(); i++)
   {
      mResourceUpdateListeners[i]->ObjectNameChanged(type, oldName, newName);
   }
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("GuiItemManager::NotifyObjectNameChange() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void UnregisterListBox(const wxString &type, wxListBox *lb)
//                        wxArrayString *excList = NULL)
//------------------------------------------------------------------------------
void GuiItemManager::UnregisterListBox(const wxString &type, wxListBox *lb,
                                       wxArrayString *excList)
{
   #if DBGLVL_GUI_ITEM_UNREG
   MessageInterface::ShowMessage
      ("GuiItemManager::UnregisterListBox() entered, type='%s', lb=<%p>, excList=<%p>\n",
       type.WX_TO_C_STRING, lb, excList);
   #endif
   
   if (type == "CelestialPoint")
   {
      std::vector<wxListBox*>::iterator pos1 =
         find(mCelestialPointLBList.begin(), mCelestialPointLBList.end(), lb);
      
      if (pos1 != mCelestialPointLBList.end())
         mCelestialPointLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mCelestialPointExcList.begin(), mCelestialPointExcList.end(), excList);
      
      if (pos2 != mCelestialPointExcList.end())
         mCelestialPointExcList.erase(pos2);
   }
   else if (type == "CelestialBody")
   {
      std::vector<wxListBox*>::iterator pos1 =
         find(mCelestialBodyLBList.begin(), mCelestialBodyLBList.end(), lb);
      
      if (pos1 != mCelestialBodyLBList.end())
         mCelestialBodyLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mCelestialBodyExcList.begin(), mCelestialBodyExcList.end(), excList);
      
      if (pos2 != mCelestialBodyExcList.end())
         mCelestialBodyExcList.erase(pos2);
   }
   else if (type == "SpaceObject")
   {
      std::vector<wxListBox*>::iterator pos1 =
         find(mSpaceObjectLBList.begin(), mSpaceObjectLBList.end(), lb);
      
      if (pos1 != mSpaceObjectLBList.end())
         mSpaceObjectLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mSpaceObjectExcList.begin(), mSpaceObjectExcList.end(), excList);
      
      if (pos2 != mSpaceObjectExcList.end())
         mSpaceObjectExcList.erase(pos2);
   }
   else if (type == "Spacecraft")
   {
      std::vector<wxListBox*>::iterator pos1 =
         find(mSpacecraftLBList.begin(), mSpacecraftLBList.end(), lb);
      
      if (pos1 != mSpacecraftLBList.end())
         mSpacecraftLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mSpacecraftExcList.begin(), mSpacecraftExcList.end(), excList);
      
      if (pos2 != mSpacecraftExcList.end())
         mSpacecraftExcList.erase(pos2);
   }
   else if (type == "ImpulsiveBurn")
   {
      std::vector<wxListBox*>::iterator pos1 =
         find(mImpBurnLBList.begin(), mImpBurnLBList.end(), lb);
      
      if (pos1 != mImpBurnLBList.end())
         mImpBurnLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mImpBurnExcList.begin(), mImpBurnExcList.end(), excList);
      
      if (pos2 != mImpBurnExcList.end())
         mImpBurnExcList.erase(pos2);
   }
   else if (type == "FiniteBurn")
   {
      std::vector<wxListBox*>::iterator pos1 =
         find(mFiniteBurnLBList.begin(), mFiniteBurnLBList.end(), lb);
      
      if (pos1 != mFiniteBurnLBList.end())
         mFiniteBurnLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mFiniteBurnExcList.begin(), mFiniteBurnExcList.end(), excList);
      
      if (pos2 != mFiniteBurnExcList.end())
         mFiniteBurnExcList.erase(pos2);
   }
   else if (type == "Hardware")
   {
      std::vector<wxListBox*>::iterator pos1 =
         find(mHardwareLBList.begin(), mHardwareLBList.end(), lb);
      
      if (pos1 != mHardwareLBList.end())
         mHardwareLBList.erase(pos1);
   }
   else if (type == "FuelTank")
   {
      std::vector<wxListBox*>::iterator pos1 =
         find(mFuelTankLBList.begin(), mFuelTankLBList.end(), lb);
      
      if (pos1 != mFuelTankLBList.end())
         mFuelTankLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mFuelTankExcList.begin(), mFuelTankExcList.end(), excList);
      
      if (pos2 != mFuelTankExcList.end())
         mFuelTankExcList.erase(pos2);
   }
   else if (type == "Thruster")
   {
      std::vector<wxListBox*>::iterator pos1 =
         find(mThrusterLBList.begin(), mThrusterLBList.end(), lb);
      
      if (pos1 != mThrusterLBList.end())
         mThrusterLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mThrusterExcList.begin(), mThrusterExcList.end(), excList);
      
      if (pos2 != mThrusterExcList.end())
         mThrusterExcList.erase(pos2);
   }
   #if DBGLVL_GUI_ITEM_UNREG
   MessageInterface::ShowMessage
      ("GuiItemManager::UnregisterListBox() leaving, type='%s'\n", type.WX_TO_C_STRING);
   #endif
}


//------------------------------------------------------------------------------
// void UnregisterCheckListBox(const wxString &type, wxCheckListBox *lb)
//------------------------------------------------------------------------------
void GuiItemManager::UnregisterCheckListBox(const wxString &type, wxCheckListBox *clb)
{
   #if DBGLVL_GUI_ITEM_UNREG
   MessageInterface::ShowMessage
      ("GuiItemManager::UnregisterCheckListBox() type='%s', clb=<%p>\n", type.WX_TO_C_STRING,
       clb);
   #endif
   
   if (type == "Subscriber")
   {
      std::vector<wxCheckListBox*>::iterator pos1 =
         find(mSubscriberCLBList.begin(), mSubscriberCLBList.end(), clb);
      
      if (pos1 != mSubscriberCLBList.end())
         mSubscriberCLBList.erase(pos1);
      
      #if 0
      std::vector<wxArrayString*>::iterator pos2 =
         find(mSubscriberExcList.begin(), mSubscriberExcList.end(), excList);
      
      if (pos2 != mSubscriberExcList.end())
         mSubscriberExcList.erase(pos2);
      #endif
   }
   else if (type == "XYPlot")
   {
      std::vector<wxCheckListBox*>::iterator pos1 =
         find(mXyPlotCLBList.begin(), mXyPlotCLBList.end(), clb);
      
      if (pos1 != mXyPlotCLBList.end())
         mXyPlotCLBList.erase(pos1);
      
      #if 0
      std::vector<wxArrayString*>::iterator pos2 =
         find(mXyPlotExcList.begin(), mXyPlotExcList.end(), excList);
      
      if (pos2 != mXyPlotExcList.end())
         mXyPlotExcList.erase(pos2);
      #endif
   }
   else if (type == "Spacecraft")
   {
      std::vector<wxCheckListBox*>::iterator pos1 =
         find(mSpacecraftCLBList.begin(), mSpacecraftCLBList.end(), clb);
      
      if (pos1 != mSpacecraftCLBList.end())
         mSpacecraftCLBList.erase(pos1);
      
      #if 0
      std::vector<wxArrayString*>::iterator pos2 =
         find(mSpacecraftExcList.begin(), mSpacecraftExcList.end(), excList);
      
      if (pos2 != mSpacecraftExcList.end())
         mSpacecraftExcList.erase(pos2);
      #endif
   }
   else if (type == "GroundStation")
   {
      std::vector<wxCheckListBox*>::iterator pos1 =
         find(mGroundStationCLBList.begin(), mGroundStationCLBList.end(), clb);

      if (pos1 != mGroundStationCLBList.end())
         mGroundStationCLBList.erase(pos1);

      #if 0
      std::vector<wxArrayString*>::iterator pos2 =
         find(mGroundStationExcList.begin(), mGroundStationExcList.end(), excList);

      if (pos2 != mGroundStationExcList.end())
         mGroundStationExcList.erase(pos2);
      #endif
   }
   else if (type == "Imager")
   {
      std::vector<wxCheckListBox*>::iterator pos1 =
         find(mImagerCLBList.begin(), mImagerCLBList.end(), clb);

      if (pos1 != mImagerCLBList.end())
         mImagerCLBList.erase(pos1);

      #if 0
      std::vector<wxArrayString*>::iterator pos2 =
         find(mImagerExcList.begin(), mImagerExcList.end(), excList);

      if (pos2 != mImagerExcList.end())
         mImagerExcList.erase(pos2);
      #endif
   }
   else if (type == "SpacePoint")
   {
      std::vector<wxCheckListBox*>::iterator pos1 =
         find(mSpacePointCLBList.begin(), mSpacePointCLBList.end(), clb);
      
      if (pos1 != mSpacePointCLBList.end())
         mSpacePointCLBList.erase(pos1);
      
      #if 0
      std::vector<wxArrayString*>::iterator pos2 =
         find(mSpacecraftExcList.begin(), mSpacecraftExcList.end(), excList);
      
      if (pos2 != mSpacecraftExcList.end())
         mSpacecraftExcList.erase(pos2);
      #endif
   }
   else if (type == "AllObject")
   {
      std::vector<wxCheckListBox*>::iterator pos1 =
         find(mAllObjectCLBList.begin(), mAllObjectCLBList.end(), clb);
      
      if (pos1 != mAllObjectCLBList.end())
         mAllObjectCLBList.erase(pos1);
      
      #if 0
      std::vector<wxArrayString*>::iterator pos2 =
         find(mAllObjectExcList.begin(), mAllObjectExcList.end(), excList);
      
      if (pos2 != mAllObjectExcList.end())
         mAllObjectExcList.erase(pos2);
      #endif
   }
}


//------------------------------------------------------------------------------
// void UnregisterComboBox(const wxString &type, wxComboBox *cb)
//------------------------------------------------------------------------------
void GuiItemManager::UnregisterComboBox(const wxString &type, wxComboBox *cb)
{
   #if DBGLVL_GUI_ITEM_UNREG
   MessageInterface::ShowMessage
      ("GuiItemManager::UnregisterComboBox() entered, type='%s', cb=<%p>\n",
       type.WX_TO_C_STRING, cb);
   #endif
   if (type == "SpacePoint")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mSpacePointCBList.begin(), mSpacePointCBList.end(), cb);
      
      if (pos != mSpacePointCBList.end())
         mSpacePointCBList.erase(pos);
   }
   else if (type == "CelestialBody")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mCelestialBodyCBList.begin(), mCelestialBodyCBList.end(), cb);
      
      if (pos != mCelestialBodyCBList.end())
         mCelestialBodyCBList.erase(pos);
   }
   else if (type == "Spacecraft")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mSpacecraftCBList.begin(), mSpacecraftCBList.end(), cb);
      
      if (pos != mSpacecraftCBList.end())
         mSpacecraftCBList.erase(pos);
   }
   else if (type == "ImpulsiveBurn")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mImpBurnCBList.begin(), mImpBurnCBList.end(), cb);
      
      if (pos != mImpBurnCBList.end())
         mImpBurnCBList.erase(pos);
   }
   else if (type == "FiniteBurn")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mFiniteBurnCBList.begin(), mFiniteBurnCBList.end(), cb);
      
      if (pos != mFiniteBurnCBList.end())
         mFiniteBurnCBList.erase(pos);
   }
   else if (type == "CoordinateSystem")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mCoordSysCBList.begin(), mCoordSysCBList.end(), cb);
      
      if (pos != mCoordSysCBList.end())
      {
         #if DBGLVL_GUI_ITEM_CS
         MessageInterface::ShowMessage
            ("GuiItemManager::UnregisterComboBox() UNregistering coord sys combobox %s\n",
             ((*pos)->GetName()).c_str());
         #endif
         mCoordSysCBList.erase(pos);
      }
   }
   else if (type == "Function")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mFunctionCBList.begin(), mFunctionCBList.end(), cb);
      
      if (pos != mFunctionCBList.end())
         mFunctionCBList.erase(pos);
   }
   else if (type == "FuelTank")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mFuelTankCBList.begin(), mFuelTankCBList.end(), cb);
      
      if (pos != mFuelTankCBList.end())
         mFuelTankCBList.erase(pos);
   }
   else if (type == "Thruster")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mThrusterCBList.begin(), mThrusterCBList.end(), cb);
      
      if (pos != mThrusterCBList.end())
         mThrusterCBList.erase(pos);
   }
   else if (type == "PowerSystem")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mPowerSystemCBList.begin(), mPowerSystemCBList.end(), cb);

      if (pos != mPowerSystemCBList.end())
         mPowerSystemCBList.erase(pos);
   }
   else if (type == "EventLocator")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mLocatorCBList.begin(), mLocatorCBList.end(), cb);

      if (pos != mLocatorCBList.end())
         mLocatorCBList.erase(pos);
   }
   else if (type == "Sensor")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mSensorCBList.begin(), mSensorCBList.end(), cb);
      
      if (pos != mSensorCBList.end())
         mSensorCBList.erase(pos);
   }
   else if (type == "Antenna")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mAntennaCBList.begin(), mAntennaCBList.end(), cb);
      
      if (pos != mAntennaCBList.end())
         mAntennaCBList.erase(pos);
   }
   else if (type == "Subscriber")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mSubscriberCBList.begin(), mSubscriberCBList.end(), cb);
      
      if (pos != mSubscriberCBList.end())
         mSubscriberCBList.erase(pos);
   }
   else if (type == "ReportFile")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mReportFileCBList.begin(), mReportFileCBList.end(), cb);
      
      if (pos != mReportFileCBList.end())
         mReportFileCBList.erase(pos);
   }
   else if (type == "Solver")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mSolverCBList.begin(), mSolverCBList.end(), cb);
      
      if (pos != mSolverCBList.end())
         mSolverCBList.erase(pos);
   }
   else if (type == "BoundarySolver")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mBoundarySolverCBList.begin(), mBoundarySolverCBList.end(), cb);
      
      if (pos != mBoundarySolverCBList.end())
         mBoundarySolverCBList.erase(pos);
   }
   else if (type == "Optimizer")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mOptimizerCBList.begin(), mOptimizerCBList.end(), cb);
      
      if (pos != mOptimizerCBList.end())
         mOptimizerCBList.erase(pos);
   }
   else
   {
      #if DBGLVL_GUI_ITEM_UNREG
      MessageInterface::ShowMessage
         ("GuiItemManager::UnregisterComboBox() *** INTERNAL ERROR *** Unhandled type: '%s'\n",
          type.WX_TO_C_STRING);
      #endif
   }
   
   #if DBGLVL_GUI_ITEM_UNREG
   MessageInterface::ShowMessage
      ("GuiItemManager::UnregisterComboBox() leaving, type='%s'\n",
       type.WX_TO_C_STRING);
   #endif
}


//------------------------------------------------------------------------------
// wxArrayString GetSpacePointList(bool excludeSC = false)
//------------------------------------------------------------------------------
wxArrayString GuiItemManager::GetSpacePointList(bool excludeSC)
{
   wxArrayString spacePtList;
   GmatBase *obj = NULL;
   for (int i=0; i<theNumSpacePoint; i++)
   {
      obj = theGuiInterpreter->GetConfiguredObject(theSpacePointList[i].c_str());
      bool isSC = obj->IsOfType(Gmat::SPACECRAFT);
      if (!isSC || (isSC && !excludeSC))
         spacePtList.push_back(theSpacePointList[i]);
   }
   return spacePtList;
}


//------------------------------------------------------------------------------
// wxArrayString GetAttachedHardwareList(const wxString &scName)
//------------------------------------------------------------------------------
wxArrayString GuiItemManager::GetAttachedHardwareList(const wxString &scName)
{
   wxArrayString hardwareList;
   
   // Get requested spacecraft
   GmatBase *obj = theGuiInterpreter->GetConfiguredObject(scName.c_str());
   if (obj)
   {
      StringArray tanks = obj->GetStringArrayParameter("Tanks");
      StringArray thrusters = obj->GetStringArrayParameter("Thrusters");
      std::string pwrSystem = obj->GetStringParameter("PowerSystem");
      
      // Add Tanks
      for (unsigned int i = 0; i < tanks.size(); i++)
         hardwareList.Add(tanks[i].c_str());
      
      // Add Thrusters
      for (unsigned int i = 0; i < thrusters.size(); i++)
         hardwareList.Add(thrusters[i].c_str());

      // Add Power System
      if (pwrSystem != "")
         hardwareList.Add(pwrSystem.c_str());
   }
   
   return hardwareList;
}


//------------------------------------------------------------------------------
// wxArrayString GetPropertyList(const wxString &objName, const wxString &ownedObjName,
//               int showOption = SHOW_PLOTTABLE, bool showSettableOnly = false)
//------------------------------------------------------------------------------
wxArrayString GuiItemManager::GetPropertyList(const wxString &objName,
                                              const wxString &ownedObjName,
                                              int showOption, bool showSettableOnly,
                                              bool forStopCondition)
{
   #if DBGLVL_GUI_ITEM_PROPERTY
   MessageInterface::ShowMessage
      ("GuiItemManager::GetPropertyList() entered, objName='%s', ownedObjName='%s', "
       "showOption=%d, showSettableOnly=%d, forStopCondition\n", objName.WX_TO_C_STRING,
       ownedObjName.WX_TO_C_STRING, showOption, showSettableOnly, forStopCondition);
   #endif
   
   wxArrayString array;
   
   if (objName == "")
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** There are no objects of ImpulsiveBurn\n");
      
      #if DBGLVL_GUI_ITEM_PROPERTY
      MessageInterface::ShowMessage
         ("GuiItemManager::GetPropertyList() returning empty properties\n");
      #endif
      
      return array;
   }
   
   std::string paramName;
   wxString objTypeName, ownedObjTypeName;
   UnsignedInt objType = Gmat::UNKNOWN_OBJECT;
   UnsignedInt ownedObjType = Gmat::UNKNOWN_OBJECT;
   
   // Get object and owned object type name
   GmatBase *obj = theGuiInterpreter->GetConfiguredObject(objName.c_str());
   if (obj)
   {
      objTypeName = obj->GetTypeName().c_str();
      objType = obj->GetType();
   }
   
   if (ownedObjName != "")
   {
      obj = theGuiInterpreter->GetConfiguredObject(ownedObjName.c_str());
      if (obj)
      {
         ownedObjTypeName = obj->GetTypeName().c_str();
         ownedObjType = obj->GetType();
      }
   }
   
   #if DBGLVL_GUI_ITEM_PROPERTY
   MessageInterface::ShowMessage
      ("   objTypeName='%s', ownedObjTypeName='%s', ownedObjType=%d\n",
       objTypeName.WX_TO_C_STRING, ownedObjTypeName.WX_TO_C_STRING, ownedObjType);
   #endif
   
   if (objTypeName == "Spacecraft")
   {
      ParameterInfo *theParamInfo = ParameterInfo::Instance();
      // if owned object property
      if (ownedObjTypeName != "")
      {
         int numScOwnedObjProperty = theScAttachedObjPropertyList.GetCount();
         for (int i = 0; i < numScOwnedObjProperty; i++)
         {
            paramName = theScAttachedObjPropertyList[i].c_str();
            UnsignedInt paramOwnedObjType = theParamInfo->GetOwnedObjectType(paramName);
            #if DBGLVL_GUI_ITEM_PROPERTY > 1
            MessageInterface::ShowMessage
               ("   '%s' ownedObjType=%d, paramOwnedObjType=%d \n", paramName.c_str(),
                ownedObjType, paramOwnedObjType);
            #endif
            
            // Use IsOfType() for checking owned object type
            // (LOJ: 2015-05-27 Fix for GMT5128)
            // if (paramOwnedObjType == ownedObjType)
            if (obj->IsOfType(paramOwnedObjType))
            {
               // Currently all spacecraft owned object (hardware) Parameters are plottable
               #if DBGLVL_GUI_ITEM_PROPERTY > 1
               MessageInterface::ShowMessage("   Adding '%s'\n", paramName.c_str());
               #endif
               array.Add(paramName.c_str());
            }
         }
      }
      else
      {
         array =  GetSpacecraftProperties(showOption, showSettableOnly);
      }
      
      // Check if it is for propagate stopping condition
      if (forStopCondition)
      {
         #if DBGLVL_GUI_ITEM_PROPERTY > 1
         MessageInterface::ShowMessage
            ("   Adding 'Apoapsis' and 'Periapsis' for stopping condition to property list\n");
         #endif
         array.Add("Apoapsis");
         array.Add("Periapsis");
      }
      
      #if DBGLVL_GUI_ITEM_PROPERTY
      MessageInterface::ShowMessage
         ("GuiItemManager::GetPropertyList() returning Spacecraft properties\n");
      #endif
      return array;
   }
   else if (objTypeName == "ImpulsiveBurn" )
   {
      // for now all impulsive burn parameters are reportable and plottable
      array = theImpBurnPropertyList;
      #if DBGLVL_GUI_ITEM_PROPERTY
      MessageInterface::ShowMessage
         ("GuiItemManager::GetPropertyList() returning ImpulsiveBurn properties\n");
      #endif
      return array;
   }
   else if (objTypeName == "FiniteBurn" )
   {
      // for now all finite burn parameters are reportable and plottable
      array = theFiniteBurnPropertyList;
      #if DBGLVL_GUI_ITEM_PROPERTY
      MessageInterface::ShowMessage
         ("GuiItemManager::GetPropertyList() returning FiniteBurn properties\n");
      #endif
      return array;
   }
   else if (obj->IsOfType("SpacePoint"))
   {
      // for now all space point parameters are reportable and plottable
      // However, cartesian parameters are NOT settable
      array =  GetSpacePointProperties(showOption, showSettableOnly);
      return array;
   }
   else
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** Property list for '%s' is not available at this time.\n",
          objTypeName.WX_TO_C_STRING);
      
      #if DBGLVL_GUI_ITEM_PROPERTY
      MessageInterface::ShowMessage
         ("GuiItemManager::GetPropertyList() returning empty properties\n");
      #endif
      
      return array;
   }
}


//------------------------------------------------------------------------------
// int GetNumProperty(const wxString &objType)
//------------------------------------------------------------------------------
/*
 * @return number of properties associated with objType
 */
//------------------------------------------------------------------------------
int GuiItemManager::GetNumProperty(const wxString &objType)
{
   if (objType == "Spacecraft")
      return theNumScProperty;
   else if (objType == "SpacePoint")
      return theNumSpacePtProperty;
   else if (objType == "ImpulsiveBurn")
      return theNumImpBurnProperty;
   else if (objType == "FiniteBurn")
      return theNumFiniteBurnProperty;
   else
      return 0;
}


//------------------------------------------------------------------------------
// wxArrayString GetCoordSystemWithAxesOf(const std::string &axesType,
//                                        bool cbOriginOnly = false)
//------------------------------------------------------------------------------
/**
 * Constructs array with CoordinateSystem names with given axes type only.
 *
 * @param axesType  Axes type for the coordinate system
 *                  Use blank "" for retrieving all CoordinateSystem
 * @param cbOriginOnly Include only those CSs with a Celestial Body origin
 */
//------------------------------------------------------------------------------
wxArrayString GuiItemManager::GetCoordSystemWithAxesOf(const std::string &axesType,
                              bool cbOriginOnly)
{
   #ifdef DEBUG_CS_WITH_AXES
   MessageInterface::ShowMessage
      ("GuiItemManager::GetCoordSystemWithAxesOf() entered, axesType='%s'\n", axesType.c_str());
   #endif
   
   wxArrayString csList;
   bool          originOK = true;
   GmatBase      *cs;
   for (int i=0; i<theNumCoordSys; i++)
   {
      std::string csName     = theCoordSysList[i].WX_TO_STD_STRING;
      cs                     = theGuiInterpreter->GetConfiguredObject(csName);
      if (cbOriginOnly)
      {
         std::string originName  = cs->GetStringParameter("Origin");
         SpacePoint* origin      = (SpacePoint*) theGuiInterpreter->GetConfiguredObject(originName);
         originOK                = origin->IsOfType("CelestialBody");
      }
      else
         originOK = true;

      if (axesType == "")
      {
         if (originOK)
            csList.Add(csName.c_str());
      }
      else
      {
         // check for axis type
         if (cs)
         {
            GmatBase *axis = cs->GetOwnedObject(0);
            if (axis && axis->IsOfType(axesType))
               if (originOK)
                  csList.Add(csName.c_str());
         }
      }
   }
   
   #ifdef DEBUG_CS_WITH_AXES
   MessageInterface::ShowMessage
      ("GuiItemManager::GetCoordSystemWithAxesOf() returning %d CoordinateSystems\n", csList.size());
   for (unsigned int i = 0; i < csList.size(); i++)
      MessageInterface::ShowMessage("   %s\n", csList[i].c_str());
   #endif
   
   return csList;
}


//------------------------------------------------------------------------------
//  wxComboBox* GetObjectTypeComboBox(wxWindow *parent, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return object type combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetODEModelComboBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size)
{
   #ifdef DEBUG_OBJECT_TYPE_COMBOBOX
   MessageInterface::ShowMessage("GuiItemManager::GetODEModelComboBox() entered\n");
   #endif
   
   wxArrayString emptyList;
   wxComboBox *cb =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size, emptyList,
                     wxCB_READONLY);
   
   for (UnsignedInt j = 0; j < theForceModelList.size(); ++j)
      cb->Append(theForceModelList[j]);
   
   cb->SetSelection(0);
   
   #ifdef DEBUG_OBJECT_TYPE_COMBOBOX
      MessageInterface::ShowMessage("==> GuiItemManager::GetODEModelComboBox() leaving\n");
   #endif
   
   return cb;
}


//------------------------------------------------------------------------------
//  wxComboBox* GetObjectTypeComboBox(wxWindow *parent, const wxSize &size, ...)
//------------------------------------------------------------------------------
/**
 * @return object type combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetObjectTypeComboBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size,
                                                  const wxArrayString objectTypeList)
{
   #ifdef DEBUG_OBJECT_TYPE_COMBOBOX
   MessageInterface::ShowMessage("GuiItemManager::GetObjectTypeComboBox() entered\n");
   MessageInterface::ShowMessage("There are %d object types\n", objectTypeList.size());
   for (unsigned int i = 0; i < objectTypeList.size(); i++)
      MessageInterface::ShowMessage("   %s\n", objectTypeList[i].c_str());
   #endif
   
   wxArrayString emptyList;
   wxComboBox *cb =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size, emptyList,
                     wxCB_READONLY);
   
   for (unsigned int i=0; i<objectTypeList.size(); i++)
   {
      if (objectTypeList[i] == "Spacecraft")
         cb->Append(objectTypeList[i]);
      else if (objectTypeList[i] == "SpacePoint")
         cb->Append(objectTypeList[i]);
      else if (objectTypeList[i] == "ImpulsiveBurn")
         cb->Append(objectTypeList[i]);
      else if (objectTypeList[i] == "FiniteBurn")
         cb->Append(objectTypeList[i]);
      else if (objectTypeList[i] == "Array")
         cb->Append(objectTypeList[i]);
      else if (objectTypeList[i] == "Variable")
         cb->Append(objectTypeList[i]);
      else if (objectTypeList[i] == "String")
         cb->Append(objectTypeList[i]);
   }
   
   cb->SetSelection(0);
   
   #ifdef DEBUG_OBJECT_TYPE_COMBOBOX
   MessageInterface::ShowMessage("==> GuiItemManager::GetObjectTypeComboBox() leaving\n");
   #endif
   
   return cb;
}


//------------------------------------------------------------------------------
//  wxComboBox* GetSpacecraftComboBox(wxWindow *parent, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return spacecraft combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetSpacecraftComboBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size)
{
   wxComboBox *spacecraftComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theSpacecraftList, wxCB_READONLY);
   
   if (theNumSpacecraft == 0)
      spacecraftComboBox->Append("No Spacecrafts Available");
   
   // show first spacecraft
   spacecraftComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mSpacecraftCBList.push_back(spacecraftComboBox);
   #if DBGLVL_GUI_ITEM_SC
   MessageInterface::ShowMessage
      ("GuiItemManager::GetSpacecraftComboBox() Spacecraft:%d registered\n",
       spacecraftComboBox);
   #endif
   
   return spacecraftComboBox;
}


//------------------------------------------------------------------------------
//  wxComboBox* GetImpBurnComboBox(wxWindow *parent, wxWindowID id, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return burn combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetImpBurnComboBox(wxWindow *parent, wxWindowID id,
                                               const wxSize &size)
{
   wxComboBox *burnComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theImpBurnList, wxCB_READONLY);
   
   if (theNumImpBurn == 0)
      burnComboBox->Append("No Impulsive Burns Available");
   
   // show first burn
   burnComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mImpBurnCBList.push_back(burnComboBox);
   
   return burnComboBox;
}


//------------------------------------------------------------------------------
//  wxComboBox* GetFiniteBurnComboBox(wxWindow *parent, wxWindowID id,
//                                    const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return burn combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetFiniteBurnComboBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size)
{
   wxComboBox *burnComboBox =
      //new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
      //               theNumFiniteBurn, theFiniteBurnList, wxCB_READONLY);
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theFiniteBurnList, wxCB_READONLY);
   
   if (theNumFiniteBurn == 0)
      burnComboBox->Append("No Finite Burns Available");
   
   // show first burn
   burnComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mFiniteBurnCBList.push_back(burnComboBox);
   
   return burnComboBox;
}


//------------------------------------------------------------------------------
//  wxComboBox* GetCoordSysComboBox(wxWindow *parent, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return coordinate system combo box pointer
 *
 * @note This method will be removed in a future build
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetCoordSysComboBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size)
{
   return GetCoordSystemComboBox(parent, id, size);
}


//------------------------------------------------------------------------------
//  wxComboBox* GetCoordSystemComboBox(wxWindow *parent, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return coordinate system combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetCoordSystemComboBox(wxWindow *parent, wxWindowID id,
                                                   const wxSize &size, bool getMJ2000EqOnly,
                                                   bool getCode500Only)
{
   wxComboBox *coordSysComboBox;
   if (getMJ2000EqOnly)
   {
      wxArrayString mj2000AxisList;
      for (int i=0; i<theNumCoordSys; i++)
      {
         std::string csName = theCoordSysList[i].WX_TO_STD_STRING;
         // check for axis type
         GmatBase *cs = theGuiInterpreter->GetConfiguredObject(csName);
         if (cs)
         {
            GmatBase *axis = cs->GetOwnedObject(0);
            if (axis && axis->IsOfType("MJ2000EqAxes"))
               mj2000AxisList.Add(csName.c_str());
         }
      }
      coordSysComboBox =
         new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                        mj2000AxisList, wxCB_READONLY);
   }
   else if (getCode500Only)
   {
      wxArrayString code500AxisList;
      for (int i = 0; i<theNumCoordSys; i++)
      {
         std::string csName = theCoordSysList[i].WX_TO_STD_STRING;
         // check for axis type
         GmatBase *cs = theGuiInterpreter->GetConfiguredObject(csName);
         if (cs)
         {
            GmatBase *axis = cs->GetOwnedObject(0);
            if (axis)
            {
               if (axis->IsOfType("MJ2000EqAxes") ||
                  axis->IsOfType("BodyFixedAxes") ||
                  axis->IsOfType("TrueOfDateAxes"))
               {
                  code500AxisList.Add(csName.c_str());
               }
            }
         }
      }
      coordSysComboBox =
         new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
         code500AxisList, wxCB_READONLY);
   }
   else
   {
      coordSysComboBox =
         new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                        theCoordSysList, wxCB_READONLY);
   }
   
   // show first coordinate system
   coordSysComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mCoordSysCBList.push_back(coordSysComboBox);
   
   return coordSysComboBox;
}


//------------------------------------------------------------------------------
//  wxComboBox* GetCelestialBodyComboBox(wxWindow *parent, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return CelestialBody ComboBox pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetCelestialBodyComboBox(wxWindow *parent, wxWindowID id,
                                                     const wxSize &size)
{
   int numBody = theNumCelesBody;
   
   if (theNumCelesBody == 0)
      numBody = 1;
   
   wxComboBox *celesBodyComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theCelestialBodyList, wxCB_READONLY);
   
   // show Earth as a default body
   celesBodyComboBox->SetStringSelection("Earth");
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mCelestialBodyCBList.push_back(celesBodyComboBox);
   
   return celesBodyComboBox;
}


//------------------------------------------------------------------------------
//  wxComboBox* GetFunctionComboBox(wxWindow *parent, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return CelestialBody ComboBox pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetFunctionComboBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size)
{   
   wxComboBox *functionComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theFunctionList, wxCB_READONLY);
   
   if (theNumFunction == 0)
      functionComboBox->Append("No Functions Available");
   
   functionComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mFunctionCBList.push_back(functionComboBox);
   
   return functionComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetSpacePointComboBox(wxWindow *parent, wxWindowID id,
//                                   const wxSize &size, bool addVector = false,
//                                   bool excludeSC = false)
//------------------------------------------------------------------------------
/**
 * @return configured SpacePoint object ComboBox pointer
 */
//------------------------------------------------------------------------------
wxComboBox*
GuiItemManager::GetSpacePointComboBox(wxWindow *parent, wxWindowID id,
                                      const wxSize &size, bool addVector,
                                      bool excludeSC)
{
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::GetSpacePointComboBox() theNumSpacePoint=%d\n",
      theNumSpacePoint);
   #endif
   
   wxArrayString emptyList;
   wxComboBox *spacePointComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size, emptyList,
                     wxCB_READONLY);
   
   if (addVector)
      spacePointComboBox->Append("Vector");
   
   GmatBase *obj = NULL;
   for (int i=0; i<theNumSpacePoint; i++)
   {
      if (spacePointComboBox->FindString(theSpacePointList[i]) == wxNOT_FOUND)
      {
         obj = theGuiInterpreter->GetConfiguredObject(theSpacePointList[i].c_str());
         bool isSC = obj->IsOfType(Gmat::SPACECRAFT);
         if (!isSC || (isSC && !excludeSC))
         {
            spacePointComboBox->Append(theSpacePointList[i]);
         }
      }
   }
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mSpacePointCBList.push_back(spacePointComboBox);
   
   return spacePointComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetCelestialPointComboBox(wxWindow *parent, wxWindowID id,
//                                   const wxSize &size, bool addVector = false)
//------------------------------------------------------------------------------
/**
 * @return configured CelestialBody and CalculatedPoint object ComboBox pointer
 */
//------------------------------------------------------------------------------
wxComboBox*
GuiItemManager::GetCelestialPointComboBox(wxWindow *parent, wxWindowID id,
                                          const wxSize &size, bool addVector)
{
   #if DBGLVL_GUI_ITEM
   MessageInterface::ShowMessage
      ("GuiItemManager::GetCelestialPointComboBox() theNumCelesPoint=%d\n",
      theNumCelesPoint);
   #endif
      
   wxArrayString emptyList;
   wxComboBox * celestialPointComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size, emptyList,
                     wxCB_READONLY);
   
   if (addVector)
      celestialPointComboBox->Append("Vector");
   
   for (int i=0; i<theNumCelesPoint; i++)
      celestialPointComboBox->Append(theCelestialPointList[i]);
   
   // select first item
   celestialPointComboBox->SetSelection(0);
   
   return celestialPointComboBox;
}

//------------------------------------------------------------------------------
//  wxComboBox* GetUserVariableComboBox(wxWindow *parent, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return configured user parameter combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox*
GuiItemManager::GetUserVariableComboBox(wxWindow *parent, wxWindowID id,
                                        const wxSize &size)
{
   wxComboBox *userParamComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theUserVariableList, wxCB_READONLY);
   
   if (theNumUserVariable == 0)
      userParamComboBox->Append("No Variables Available");
   
   // show first parameter
   userParamComboBox->SetSelection(0);
   
   return userParamComboBox;
}


//------------------------------------------------------------------------------
//  wxComboBox* GetFuelTankComboBox(wxWindow *parent, wxWindowID id, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return fuel tank combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetFuelTankComboBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size)
{
   wxComboBox *fuelTankComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theFuelTankList, wxCB_READONLY);
   
   if (theNumFuelTank == 0)
      fuelTankComboBox->Append("No Fuel Tanks Available");
   
   // show first burn
   fuelTankComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mFuelTankCBList.push_back(fuelTankComboBox);
   
   return fuelTankComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetThrusterComboBox(wxWindow *parent, wxWindowID id,
//                                 const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Thruster combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetThrusterComboBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size)
{
   wxComboBox *thrusterComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theThrusterList, wxCB_READONLY);
   
   if (theNumThruster == 0)
      thrusterComboBox->Append("No Thrusters Available");
   
   // show first Thruster
   thrusterComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mThrusterCBList.push_back(thrusterComboBox);
   
   return thrusterComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetSensorComboBox(wxWindow *parent, wxWindowID id,
//                                 const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Sensor combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetSensorComboBox(wxWindow *parent, wxWindowID id,
                                              const wxSize &size)
{
   wxComboBox *sensorComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theSensorList, wxCB_READONLY);
   
   if (theNumSensor == 0)
      sensorComboBox->Append("No Sensors Available");
   
   // show first Sensor
   sensorComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mSensorCBList.push_back(sensorComboBox);
   
   return sensorComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetAntennaComboBox(wxWindow *parent, wxWindowID id,
//                                const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Antenna combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetAntennaComboBox(wxWindow *parent, wxWindowID id,
                                               const wxSize &size)
{
   wxComboBox *antennaComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theAntennaList, wxCB_READONLY);
   
   if (theNumAntenna == 0)
      antennaComboBox->Append("No Antennas Available");
   else
      antennaComboBox->Insert("No Antenna Selected", 0);
   
   // show first Antenna
   antennaComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mAntennaCBList.push_back(antennaComboBox);
   
   return antennaComboBox;
}

//------------------------------------------------------------------------------
// wxComboBox* GetPowerSystemComboBox(wxWindow *parent, wxWindowID id,
//                                    const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return PowerSystem combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetPowerSystemComboBox(wxWindow *parent, wxWindowID id,
                                                   const wxSize &size)
{
   wxComboBox *powerSystemComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     thePowerSystemList, wxCB_READONLY);

   if (theNumPowerSystem == 0)
      powerSystemComboBox->Append("No Power Systems Available");
   else
      powerSystemComboBox->Insert("", 0);

   // show first Power System
   powerSystemComboBox->SetSelection(0);

   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mPowerSystemCBList.push_back(powerSystemComboBox);

   return powerSystemComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetLocatorComboBox(wxWindow *parent, wxWindowID id,
//                                const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Event Locator combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetLocatorComboBox(wxWindow *parent, wxWindowID id,
                                               const wxSize &size)
{
   wxComboBox *eventLocatorComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theLocatorList, wxCB_READONLY);

   if (theNumLocator == 0)
      eventLocatorComboBox->Append("No Event Locators Available");

   // show first Power System
   eventLocatorComboBox->SetSelection(0);

   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mLocatorCBList.push_back(eventLocatorComboBox);

   return eventLocatorComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetSubscriberComboBox(wxWindow *parent, wxWindowID id,
//                                   const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Subscriber combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetSubscriberComboBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size)
{
   wxComboBox *subsComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theSubscriberList, wxCB_READONLY);
   
   if (theNumSubscriber == 0)
      subsComboBox->Append("No Subscribers Available");
   
   // show first Subscriber
   subsComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mSubscriberCBList.push_back(subsComboBox);
   
   return subsComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetReportFileComboBox(wxWindow *parent, wxWindowID id,
//                                   const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return ReportFile combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetReportFileComboBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size)
{
   wxComboBox *rfComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theReportFileList, wxCB_READONLY);
   
   if (theNumReportFile == 0)
      rfComboBox->Append("No Report Files Available");
   
   // show first ReportFile
   rfComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mReportFileCBList.push_back(rfComboBox);
   
   return rfComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetSolverComboBox(wxWindow *parent, wxWindowID id,
//                               const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Solver combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetSolverComboBox(wxWindow *parent, wxWindowID id,
                                              const wxSize &size)
{
   wxComboBox *solverComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theSolverList, wxCB_READONLY);
   
   if (theNumSolver == 0)
      solverComboBox->Append("No Solvers Available");
   
   // show first Solver
   solverComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mSolverCBList.push_back(solverComboBox);
   
   return solverComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetBoundarySolverComboBox(wxWindow *parent, wxWindowID id,
//                                      const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return BoundarySolver combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetBoundarySolverComboBox(wxWindow *parent, wxWindowID id,
                                                      const wxSize &size)
{
   wxComboBox *bvsComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theBoundarySolverList, wxCB_READONLY);
   
   // Is this correct name?
   if (theNumBoundarySolver == 0)
      bvsComboBox->Append("No Differential Correctors Available");
   
   // show first Boundary Solver
   bvsComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mBoundarySolverCBList.push_back(bvsComboBox);
   
   return bvsComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetOptimizerComboBox(wxWindow *parent, wxWindowID id,
//                                  const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Optimizer combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetOptimizerComboBox(wxWindow *parent, wxWindowID id,
                                                 const wxSize &size)
{
   wxComboBox *optimizerComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theOptimizerList, wxCB_READONLY);
   
   // Is this correct name?
   if (theNumOptimizer == 0)
      optimizerComboBox->Append("No Optimizers Available");
   
   // show first Optimizer
   optimizerComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mOptimizerCBList.push_back(optimizerComboBox);
   
   return optimizerComboBox;
}


// CheckListBox
//------------------------------------------------------------------------------
// wxCheckListBox* GetSubscriberCheckListBox(wxWindow *parent, wxWindowID id,
//                 const wxSize &size, bool isForPen)
//------------------------------------------------------------------------------
/**
 * @return Available Subscriber ListBox pointer
 */
//------------------------------------------------------------------------------
wxCheckListBox* GuiItemManager::GetSubscriberCheckListBox(wxWindow *parent, wxWindowID id,
                                                          const wxSize &size,
                                                          bool isForPen)
{
   wxArrayString emptyList;
   wxCheckListBox *checkListBox =
      new wxCheckListBox(parent, id, wxDefaultPosition, size, emptyList,
                         wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   
   if (isForPen)
   {
      for (int i=0; i<theNumSubscriber; i++)
      {
		 GmatBase *otype = theGuiInterpreter->FindObject(theSubscriberList[i].c_str());
		 if ((!otype->IsOfType(Gmat::EPHEMERIS_FILE)) && (!otype->IsOfType(Gmat::REPORT_FILE)))
            checkListBox->Append(theSubscriberList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumSubscriber; i++)
         checkListBox->Append(theSubscriberList[i]);
   }
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mSubscriberCLBList.push_back(checkListBox);
   mSubscriberExcList.push_back(NULL);
   
   checkListBox->SetSelection(0);
   return checkListBox;
}


// CheckListBox
//------------------------------------------------------------------------------
// wxCheckListBox* GetSubscriberCheckListBox(wxWindow *parent, wxWindowID id,
//                 const wxSize &size, wxArrayString &excList = NULL)
//------------------------------------------------------------------------------
/**
 * @return Available Subscriber ListBox pointer
 */
//------------------------------------------------------------------------------
wxCheckListBox* GuiItemManager::GetSubscriberCheckListBox(wxWindow *parent, wxWindowID id,
                                                          const wxSize &size,
                                                          wxArrayString *excList)
{
   wxArrayString emptyList;
   wxCheckListBox *checkListBox =
      new wxCheckListBox(parent, id, wxDefaultPosition, size, emptyList,
                         wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumSubscriber; i++)
      {
         if (excList->Index(theSubscriberList[i]) == wxNOT_FOUND)
            checkListBox->Append(theSubscriberList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumSubscriber; i++)
         checkListBox->Append(theSubscriberList[i]);
   }
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mSubscriberCLBList.push_back(checkListBox);
   mSubscriberExcList.push_back(excList);
   
   checkListBox->SetSelection(0);
   return checkListBox;
}


//------------------------------------------------------------------------------
// wxCheckListBox* GetXyPlotCheckListBox(wxWindow *parent, wxWindowID id,
//                 const wxSize &size, wxArrayString &excList = NULL)
//------------------------------------------------------------------------------
/**
 * @return Available XyPlot ListBox pointer
 */
//------------------------------------------------------------------------------
wxCheckListBox* GuiItemManager::GetXyPlotCheckListBox(wxWindow *parent, wxWindowID id,
                                                      const wxSize &size,
                                                      wxArrayString *excList)
{
   wxArrayString emptyList;
   wxCheckListBox *checkListBox =
      new wxCheckListBox(parent, id, wxDefaultPosition, size, emptyList,
                         wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumXyPlot; i++)
      {
         if (excList->Index(theXyPlotList[i]) == wxNOT_FOUND)
            checkListBox->Append(theXyPlotList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumXyPlot; i++)
         checkListBox->Append(theXyPlotList[i]);
   }
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mXyPlotCLBList.push_back(checkListBox);
   mXyPlotExcList.push_back(excList);
   
   checkListBox->SetSelection(0);
   return checkListBox;
}


//------------------------------------------------------------------------------
// wxCheckListBox* GetSpacePointCheckListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * @return Available CelestialPoint ListBox pointer
 */
//------------------------------------------------------------------------------
wxCheckListBox*
GuiItemManager::GetSpacePointCheckListBox(wxWindow *parent, wxWindowID id, const wxSize &size,
                                          bool includeCelestialBodies, bool includeCalculatedPoints,
                                          bool includeSpacecraft, bool includeGroundStation)
{
   wxArrayString emptyList;
   wxCheckListBox *checkListBox =
      new wxCheckListBox(parent, id, wxDefaultPosition, size, emptyList,
                         wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   
   // Set CheckListBox name so that it can be updated appropriately when new
   // SpacePoint resource is created
   wxString clbName;
   if (includeCelestialBodies)
      clbName = "CelestialBody";
   if (includeCalculatedPoints)
      clbName = clbName + "+CalculatedPoint";
   if (includeSpacecraft)
      clbName = clbName + "+Spacecraft";
   if (includeGroundStation)
      clbName = clbName + "+GroundStation";
   
   // Set name to CheckListBox and build SpacePoint list
   checkListBox->SetName(clbName);
   checkListBox->Append(BuildSpacePointList(clbName));
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mSpacePointCLBList.push_back(checkListBox);
   
   return checkListBox;
}


//------------------------------------------------------------------------------
// wxCheckListBox* GetSpacecraftCheckListBox(wxWindow *parent, wxWindowID id,
//                                           const wxSize &size, wxArrayString &excList)
//------------------------------------------------------------------------------
/**
 * @return Available Spacecraft ListBox pointer
 */
//------------------------------------------------------------------------------
wxCheckListBox* GuiItemManager::GetSpacecraftCheckListBox(wxWindow *parent, wxWindowID id,
                                                          const wxSize &size,
                                                          wxArrayString *excList)
{
   wxArrayString emptyList;
   wxCheckListBox *checkListBox =
      new wxCheckListBox(parent, id, wxDefaultPosition, size, emptyList,
                         wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumSpacecraft; i++)
      {
         if (excList->Index(theSpacecraftList[i]) == wxNOT_FOUND)
            checkListBox->Append(theSpacecraftList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumSpacecraft; i++)
         checkListBox->Append(theSpacecraftList[i]);
   }
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mSpacecraftCLBList.push_back(checkListBox);
   mSpacecraftExcList.push_back(excList);
   
   return checkListBox;
}

//------------------------------------------------------------------------------
// wxCheckListBox* GetGroundStationCheckListBox(wxWindow *parent, wxWindowID id,
//                                             const wxSize &size,
//                                             wxArrayString &excList)
//------------------------------------------------------------------------------
/**
 * @return Available GroundStation ListBox pointer
 */
//------------------------------------------------------------------------------
wxCheckListBox* GuiItemManager::GetGroundStationCheckListBox(wxWindow *parent, wxWindowID id,
                                                            const wxSize &size,
                                                            wxArrayString *excList)
{
   wxArrayString emptyList;
   wxCheckListBox *checkListBox =
      new wxCheckListBox(parent, id, wxDefaultPosition, size, emptyList,
                         wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);

   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumGroundStation; i++)
      {
         if (excList->Index(theGroundStationList[i]) == wxNOT_FOUND)
            checkListBox->Append(theGroundStationList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumGroundStation; i++)
         checkListBox->Append(theGroundStationList[i]);
   }

   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mGroundStationCLBList.push_back(checkListBox);
   mGroundStationExcList.push_back(excList);

   return checkListBox;
}

//------------------------------------------------------------------------------
// wxCheckListBox* GetImagerCheckListBox(wxWindow *parent, wxWindowID id,
//                                             const wxSize &size,
//                                             wxArrayString &excList)
//------------------------------------------------------------------------------
/**
 * @return Available GroundStation CheckListBox pointer
 */
//------------------------------------------------------------------------------
wxCheckListBox* GuiItemManager::GetImagerCheckListBox(wxWindow *parent, wxWindowID id,
                                                      const wxSize &size,
                                                      wxArrayString *excList)
{
   wxArrayString emptyList;
   wxCheckListBox *checkListBox =
      new wxCheckListBox(parent, id, wxDefaultPosition, size, emptyList,
         wxLB_SINGLE | wxLB_SORT | wxLB_HSCROLL);

   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i = 0; i < theNumImager; i++)
      {
         if (excList->Index(theImagerList[i]) == wxNOT_FOUND)
            checkListBox->Append(theImagerList[i]);
      }
   }
   else
   {
      for (int i = 0; i < theNumImager; i++)
         checkListBox->Append(theImagerList[i]);
   }

   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mImagerCLBList.push_back(checkListBox);
   mImagerExcList.push_back(excList);

   return checkListBox;
}

//------------------------------------------------------------------------------
// wxCheckListBox* GetAllObjectCheckListBox(wxWindow *parent, wxWindowID id,...)
//------------------------------------------------------------------------------
/**
 *
 * @param  includeAutoGlobal  If this flag is set, it adds automatic global objects
 *                            to CheckListBox
 * @return  All available objects wxCheckListBox pointer.
 */
//------------------------------------------------------------------------------
wxCheckListBox*
GuiItemManager::GetAllObjectCheckListBox(wxWindow *parent, wxWindowID id,
                                         const wxSize &size, bool includeAutoGlobal,
                                         wxArrayString *excList)
{
   #ifdef DEBUG_ALL_OBJ_CHECKLISTBOX
   MessageInterface::ShowMessage
      ("GuiItemManager::GetAllObjectCheckListBox() entered, theNumAllObject=%d\n",
       theNumAllObject);
   #endif
   
   wxArrayString emptyList;
   wxCheckListBox *checkListBox =
      new wxCheckListBox(parent, id, wxDefaultPosition, size, emptyList,
                         wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   
   //---------------------------------------------
   // Get all object list
   //---------------------------------------------
   
   if (excList != NULL && excList->GetCount() > 0)
   {
		if (includeAutoGlobal)
		{
			for (int i=0; i<theNumAllObject; i++)
			{
				if (excList->Index(theAllObjectList[i]) == wxNOT_FOUND)
					checkListBox->Append(theAllObjectList[i]);
			}
		}
		else
		{
			wxString objStr, objName;
			for (int i=0; i<theNumAllObject; i++)
			{
				objStr = theAllObjectList[i];
				objName = objStr.BeforeFirst(' ');
            GmatBase *obj = theGuiInterpreter->GetConfiguredObject(objName.c_str());
            #ifdef DEBUG_ALL_OBJ_CHECKLISTBOX
            MessageInterface::ShowMessage
               ("   objName='%s', obj=<%p>'%s'\n", objName.c_str(), obj, obj ? obj->GetName() : "NULL");
            #endif
				if (obj != NULL && !obj->IsAutomaticGlobal())
					checkListBox->Append(theAllObjectList[i]);
			}
		}
   }
   else
   {
		if (includeAutoGlobal)
		{
			for (int i=0; i<theNumAllObject; i++)
				checkListBox->Append(theAllObjectList[i]);
			checkListBox->SetName("AllObjectsIncludingAutoGlobal");
		}
		else
		{
			wxString objStr, objName;
			for (int i=0; i<theNumAllObject; i++)
			{
				objStr = theAllObjectList[i];
				objName = objStr.BeforeFirst(' ');
            GmatBase *obj = theGuiInterpreter->GetConfiguredObject(objName.c_str());
            #ifdef DEBUG_ALL_OBJ_CHECKLISTBOX
            MessageInterface::ShowMessage
               ("   objName='%s', obj=<%p>'%s'\n", objName.c_str(), obj, obj ? obj->GetName() : "NULL");
            #endif
            if (obj != NULL && !obj->IsAutomaticGlobal())
					checkListBox->Append(theAllObjectList[i]);
			}
			checkListBox->SetName("AllObjectsExcludingAutoGlobal");
		}
   }
   
	
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mAllObjectCLBList.push_back(checkListBox);
   mAllObjectExcList.push_back(excList);
   
   checkListBox->SetSelection(0);
   return checkListBox;
}


// ListBox
//------------------------------------------------------------------------------
// wxListBox* GetSpacePointListBox(wxWindow *parent, wxWindowID id,
//                                 const wxSize &size, bool addVector = false,
//                                 bool excludeSC = false)
//------------------------------------------------------------------------------
/**
 * @return configured CelestialBody and CalculatedPoint object ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetSpacePointListBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size, bool addVector,
                                                bool excludeSC)
{
   #if DBGLVL_GUI_ITEM
   MessageInterface::ShowMessage("GuiItemManager::GetSpacePointListBox() entered\n");
   #endif
   
   int numSpacePoint = theNumSpacePoint;
   
   if (theNumSpacePoint == 0)
      numSpacePoint = 1;

   wxArrayString emptyList;
   wxListBox *spacePointListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, emptyList,
                    wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   
   if (addVector)
      spacePointListBox->Append("Vector");
   
   GmatBase *obj = NULL;
   for (int i=0; i<theNumSpacePoint; i++)
   {
      obj = theGuiInterpreter->GetConfiguredObject(theSpacePointList[i].c_str());
      bool isSC = obj->IsOfType(Gmat::SPACECRAFT);
      if (!isSC || (isSC && !excludeSC))
      {
         spacePointListBox->Append(theSpacePointList[i]);
      }
   }
   
   // select first item
   spacePointListBox->SetSelection(0);
   
   return spacePointListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetCelestialPointListBox(wxWindow *parent, wxWindowID id,
//                                     const wxSize &size, wxArrayString *excList = NULL)
//------------------------------------------------------------------------------
/**
 * @return configured CelestialBody and CalculatedPoint object ListBox pointer
 * excluding names in the excList array.
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetCelestialPointListBox(wxWindow *parent, wxWindowID id,
                                                    const wxSize &size,
                                                    wxArrayString *excList)
{
   #if DBGLVL_GUI_ITEM
   MessageInterface::ShowMessage
      ("GuiItemManager::GetCelestialPointListBox() theNumCelesPoint=%d\n",
       theNumCelesPoint);
   #endif
   
   wxArrayString emptyList;
   wxListBox *celesPointListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, emptyList,
                    wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumCelesPoint; i++)
      {
         if (excList->Index(theCelestialPointList[i].c_str()) == wxNOT_FOUND)
            celesPointListBox->Append(theCelestialPointList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumCelesPoint; i++)
         celesPointListBox->Append(theCelestialPointList[i]);
   }
   
   celesPointListBox->SetSelection(0);
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mCelestialPointLBList.push_back(celesPointListBox);
   mCelestialPointExcList.push_back(excList);
   
   #if DBGLVL_GUI_ITEM_REG
   MessageInterface::ShowMessage
      ("GuiItemManager::GetCelestialPointListBox() mCelestialPointLBList.size()=%d, "
       "mCelestialPointExcList.size()=%d, celesBodyListBox=<%p>\n",
       mCelestialPointLBList.size(), mCelestialPointExcList.size(), celesPointListBox);
   #endif
   
   return celesPointListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetCelestialBodyListBox(wxWindow *parent, wxWindowID id,
//                                    const wxSize &size, wxArrayString *excList = NULL)
//------------------------------------------------------------------------------
/**
 * @return  Celestial body ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetCelestialBodyListBox(wxWindow *parent, wxWindowID id,
                                                   const wxSize &size,
                                                   wxArrayString *excList)
{
   wxArrayString emptyList;
   wxListBox *celesBodyListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, emptyList, wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumCelesBody; i++)
      {
         if (excList->Index(theCelestialBodyList[i].c_str()) == wxNOT_FOUND)
            celesBodyListBox->Append(theCelestialBodyList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumCelesBody; i++)
         celesBodyListBox->Append(theCelestialBodyList[i]);
   }
   
   celesBodyListBox->SetSelection(0);
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mCelestialBodyLBList.push_back(celesBodyListBox);
   mCelestialBodyExcList.push_back(excList);
   
   #if DBGLVL_GUI_ITEM_REG
   MessageInterface::ShowMessage
      ("GuiItemManager::GetCelestialBodyListBox() mCelestialBodyLBList.size()=%d, "
       "mCelestialBodyExcList.size()=%d, celesBodyListBox=<%p>\n",
       mCelestialBodyLBList.size(), mCelestialBodyExcList.size(), celesBodyListBox);
   #endif
   
   return celesBodyListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetSpaceObjectListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * @return configured Spacecraft and Formation object ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetSpaceObjectListBox(wxWindow *parent, wxWindowID id,
                                                 const wxSize &size,
                                                 wxArrayString *excList,
                                                 bool includeFormation)
{
   #if DBGLVL_GUI_ITEM_SO
   MessageInterface::ShowMessage
      ("GuiItemManager::GetSpaceObjectListBox() theNumSpaceObject=%d\n",
       theNumSpaceObject);
   for (unsigned int i=0; i<excList->GetCount(); i++)
   {
      MessageInterface::ShowMessage("excList[%d]=<%s>\n",
                                    i, (*excList)[i].c_str());
   }
   #endif
   
   wxArrayString emptyList;
   wxListBox *spaceObjectListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, emptyList,
                    wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);

   // get Formation list
   StringArray fmList =
      theGuiInterpreter->GetListOfObjects(Gmat::FORMATION);
   
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumSpaceObject; i++)
      {
         if (excList->Index(theSpaceObjectList[i]) == wxNOT_FOUND)
            spaceObjectListBox->Append(theSpaceObjectList[i]);
      }

      // if include formation
      if (includeFormation)
      {
         for (unsigned int i=0; i<fmList.size(); i++)
            if (excList->Index(fmList[i].c_str()) == wxNOT_FOUND)
               spaceObjectListBox->Append(fmList[i].c_str());
      }
   }
   else
   {
      for (int i=0; i<theNumSpaceObject; i++)
         spaceObjectListBox->Append(theSpaceObjectList[i]);

      // if include formation
      if (includeFormation)
         for (unsigned int i=0; i<fmList.size(); i++)
            spaceObjectListBox->Append(fmList[i].c_str());
      
   }
   
   spaceObjectListBox->SetSelection(0);
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mSpaceObjectLBList.push_back(spaceObjectListBox);
   mSpaceObjectExcList.push_back(excList);
   
   #if DBGLVL_GUI_ITEM_REG
   MessageInterface::ShowMessage
      ("GuiItemManager::GetSpaceObjectListBox() mSpaceObjectLBList.size()=%d, "
       "mSpaceObjectExcList.size()=%d, celesBodyListBox=<%p>\n",
       mSpaceObjectLBList.size(), mSpaceObjectExcList.size(), spaceObjectListBox);
   #endif
   
   return spaceObjectListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetSpacecraftListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * @return Available Spacecraft ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetSpacecraftListBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size,
                                                wxArrayString *excList,
                                                bool multiSelect)
{
   #if DBGLVL_SC_LISTBOX
   MessageInterface::ShowMessage
      ("GetSpacecraftListBox() multiSelect=%d\n", multiSelect);
   #endif
   
   wxArrayString emptyList;
   wxListBox *spacecraftListBox = NULL;
   
   if (multiSelect)
   {
      spacecraftListBox = new wxListBox(parent, id, wxDefaultPosition, size, 
                                        emptyList, wxLB_EXTENDED|wxLB_SORT|wxLB_HSCROLL);
   }
   else
   {
      spacecraftListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                        emptyList, wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   }
   
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumSpacecraft; i++)
      {
         if (excList != NULL && excList->Index(theSpacecraftList[i]) == wxNOT_FOUND)
         {
            #if DBGLVL_SC_LISTBOX
            MessageInterface::ShowMessage
               ("GetSpacecraftListBox() some things are excluded ... now adding %s\n",
                     theSpacecraftList[i].c_str());
            #endif
            spacecraftListBox->Append(theSpacecraftList[i]);
         }
      }
   }
   else
   {
      for (int i=0; i<theNumSpacecraft; i++)
      {
         #if DBGLVL_SC_LISTBOX
         MessageInterface::ShowMessage
            ("GetSpacecraftListBox() NOTHING excluded ... now adding %s\n",
                  theSpacecraftList[i].c_str());
         #endif
         spacecraftListBox->Append(theSpacecraftList[i]);
      }
   }
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mSpacecraftLBList.push_back(spacecraftListBox);
   mSpacecraftExcList.push_back(excList);
   
   if (!multiSelect)
      spacecraftListBox->SetSelection(0);
   
   #if DBGLVL_SC_LISTBOX
   MessageInterface::ShowMessage
      ("GetSpacecraftListBox() returning %p\n", spacecraftListBox);
   #endif
   
   return spacecraftListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetImpBurnListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * @return Available ImpBurn ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetImpBurnListBox(wxWindow *parent, wxWindowID id,
                                             const wxSize &size,
                                             wxArrayString *excList,
                                             bool multiSelect)
{
   wxArrayString emptyList;
   wxListBox *impBurnListBox = NULL;
   
   if (multiSelect)
   {
      impBurnListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                     emptyList, wxLB_EXTENDED|wxLB_SORT|wxLB_HSCROLL);
   }
   else
   {
      impBurnListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                     emptyList, wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   }
   
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumImpBurn; i++)
      {
         if (excList->Index(theImpBurnList[i]) == wxNOT_FOUND)
            impBurnListBox->Append(theImpBurnList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumImpBurn; i++)
         impBurnListBox->Append(theImpBurnList[i]);
   }
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mImpBurnLBList.push_back(impBurnListBox);
   mImpBurnExcList.push_back(excList);
   
   if (!multiSelect)
      impBurnListBox->SetSelection(0);
   
   return impBurnListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetFiniteBurnListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * @return Available FiniteBurn ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetFiniteBurnListBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size,
                                                wxArrayString *excList,
                                                bool multiSelect)
{
   wxArrayString emptyList;
   wxListBox *finiteBurnListBox = NULL;
   
   if (multiSelect)
   {
      finiteBurnListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                     emptyList, wxLB_EXTENDED|wxLB_SORT|wxLB_HSCROLL);
   }
   else
   {
      finiteBurnListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                     emptyList, wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   }
   
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumFiniteBurn; i++)
      {
         if (excList->Index(theFiniteBurnList[i]) == wxNOT_FOUND)
            finiteBurnListBox->Append(theFiniteBurnList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumFiniteBurn; i++)
         finiteBurnListBox->Append(theFiniteBurnList[i]);
   }
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mFiniteBurnLBList.push_back(finiteBurnListBox);
   mFiniteBurnExcList.push_back(excList);
   
   if (!multiSelect)
      finiteBurnListBox->SetSelection(0);
   
   return finiteBurnListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetPropertyListBox(wxWindow *parent, wxWindowID id, const wxSize &size,
//            const wxString &objType, int showOption, bool showSettableOnly = false,
//            bool multiSelect = false, bool forStopCondition = false)
//------------------------------------------------------------------------------
/**
 * @return Available Parameter ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetPropertyListBox(wxWindow *parent, wxWindowID id,
                                              const wxSize &size,
                                              const wxString &objType,
                                              int showOption, bool showSettableOnly,
                                              bool multiSelect, bool forStopCondition)
{
   #ifdef DEBUG_PROPERTY_LISTBOX
   MessageInterface::ShowMessage
      ("GuiItemManager::GetPropertyListBox() objType='%s', showOption=%d, "
       "showSettableOnly=%d, multiSelect=%d, forStopCondition=%d\n", objType.c_str(),
       showOption, showSettableOnly, multiSelect, forStopCondition);
   #endif
   
   ParameterInfo *theParamInfo = ParameterInfo::Instance();
   wxArrayString emptyList;
   wxListBox *propertyListBox = NULL;
   bool add = false;
   std::string paramName;
   
   if (multiSelect)
   {
      propertyListBox = new wxListBox(parent, id, wxDefaultPosition, size, 
                                      emptyList, wxLB_EXTENDED|wxLB_SORT|wxLB_HSCROLL);
   }
   else
   {
      propertyListBox = new wxListBox(parent, id, wxDefaultPosition, size, 
                                      emptyList, wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   }
   
   // now append properties
   if (objType == "Spacecraft")
   {
      if (showOption == SHOW_REPORTABLE)
      {
         for (int i=0; i<theNumScProperty; i++)
         {
            std::string paramName = theScPropertyList[i].WX_TO_STD_STRING;
            add = false;
            #ifdef DEBUG_PROPERTY_LISTBOX
            MessageInterface::ShowMessage
               ("   Checking if '%s' is reportable\n", paramName.c_str());
            #endif
            if (theParamInfo->IsReportable(paramName))
            {
               #ifdef DEBUG_PROPERTY_LISTBOX
               MessageInterface::ShowMessage
                  ("   '%s' is reportable\n", paramName.c_str());
               #endif
               if (!showSettableOnly)
                  add = true;
               else if (theParamInfo->IsSettable(paramName))
                  add = true;
               
               if (add)
               {
                  #ifdef DEBUG_PROPERTY_LISTBOX
                  MessageInterface::ShowMessage
                     ("   Adding '%s' to propertyListBox\n", paramName.c_str());
                  #endif
                  propertyListBox->Append(theScPropertyList[i]);
               }
            }
         }
      }
      else if (showOption == SHOW_PLOTTABLE)
      {
         for (int i=0; i<theNumScProperty; i++)
         {
            std::string paramName = theScPropertyList[i].WX_TO_STD_STRING;
            add = false;
            if (theParamInfo->IsPlottable(paramName))
            {
               if (!showSettableOnly)
                  add = true;
               else if (theParamInfo->IsSettable(paramName))
                  add = true;
               
               if (add)
               {
                  #ifdef DEBUG_PROPERTY_LISTBOX
                  MessageInterface::ShowMessage("   Adding '%s' to propertyListBox\n", paramName.c_str());
                  #endif
                  propertyListBox->Append(theScPropertyList[i]);
               }
            }
         }
      }
      
      // Check if it is for propagate stopping condition
      if (forStopCondition)
      {
         #ifdef DEBUG_PROPERTY_LISTBOX
         MessageInterface::ShowMessage
            ("   Adding 'Apoapsis' and 'Periapsis' for stopping condition to property list box\n");
         #endif
         propertyListBox->Append("Apoapsis");
         propertyListBox->Append("Periapsis");
      }
   }
   else if (objType == "SpacePoint")
   {
      wxArrayString spacePtProps = GetSpacePointProperties(
                    showOption, showSettableOnly);
      int sz = spacePtProps.GetCount();
      for (int ii = 0; ii < sz; ii++)
      {
         propertyListBox->Append(spacePtProps[ii]);
      }
   }
   else if (objType == "ImpulsiveBurn")
   {
      // Currently all ImpulsiveBurn Parameters are settable
      if (showOption == SHOW_REPORTABLE)
      {
         for (int i=0; i<theNumImpBurnProperty; i++)
         {
            paramName = theImpBurnPropertyList[i].c_str();
            if (theParamInfo->IsReportable(paramName))
               propertyListBox->Append(theImpBurnPropertyList[i]);
         }
      }
      else if (showOption == SHOW_PLOTTABLE)
      {
         for (int i=0; i<theNumImpBurnProperty; i++)
         {
            paramName = theImpBurnPropertyList[i].c_str();
            if (theParamInfo->IsPlottable(paramName))
               propertyListBox->Append(theImpBurnPropertyList[i]);
         }
      }
   }
   else if (objType == "FiniteBurn")
   {
      for (int i=0; i<theNumFiniteBurnProperty; i++)
         propertyListBox->Append(theFiniteBurnPropertyList[i]);
   }
   else if (objType == "Array") ;
   else if (objType == "Variable") ;
   else
   {
      throw GmatBaseException("There are no properties associated with " +
                              std::string(objType.c_str()));
   }
   
   if (!multiSelect)
      propertyListBox->SetSelection(0);
   
   return propertyListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetPlottableParameterListBox(wxWindow *parent, wxWindowID id,
//                                         const wxSize &size,
//                                         const wxString &nameToExclude = "")
//------------------------------------------------------------------------------
/**
 * @return Configured PlottableParameterListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetPlottableParameterListBox(wxWindow *parent,
                                                        wxWindowID id,
                                                        const wxSize &size,
                                                        const wxString &nameToExclude)
{       
   wxArrayString emptyList;
   
   wxListBox *plottableParamListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, emptyList,
                    wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   
   if (nameToExclude != "")
   {
      for (int i=0; i<theNumPlottableParam; i++)
         if (thePlottableParamList[i] != nameToExclude)
            plottableParamListBox->Append(thePlottableParamList[i]);
   }
   else
   {
      for (int i=0; i<theNumPlottableParam; i++)
         plottableParamListBox->Append(thePlottableParamList[i]);
   }
   
   plottableParamListBox->SetSelection(theNumPlottableParam - 1);
   return plottableParamListBox;
   
   
}

//------------------------------------------------------------------------------
// wxListBox* GetAllUserParameterListBox(wxWindow *parent, wxWindowID id,
//                                   const wxSize &size, bool showArray)
//------------------------------------------------------------------------------
/**
 * @return Configured all user parameter (Varialbe, Array, String) ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetAllUserParameterListBox(wxWindow *parent, wxWindowID id,
                                                      const wxSize &size, bool showArray)
{
   wxString *allUserParamList;
   int numParams = 0;
   int allUserParamCount = 0;
   wxListBox *allUserParamListBox = NULL;
   
   if (showArray)
      allUserParamCount = theNumUserVariable + theNumUserArray + theNumUserString;
   else
      allUserParamCount = theNumUserVariable + theNumUserString;
   
   if (allUserParamCount > 0)
   {
      allUserParamList = new wxString[allUserParamCount];

      for (int i=0; i<theNumUserVariable; i++)
         allUserParamList[numParams++] = theUserVariableList[i];
      
      for (int i=0; i<theNumUserString; i++)
         allUserParamList[numParams++] = theUserStringList[i];
      
      if (showArray)
      {
         for (int i=0; i<theNumUserArray; i++)
            allUserParamList[numParams++] = theUserArrayList[i];
      }
      
      allUserParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, allUserParamCount,
                       allUserParamList, wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
      
      delete [] allUserParamList;
   }
   else
   {
      wxArrayString emptyList;
      
      allUserParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, emptyList,
                       wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   }
   
   return allUserParamListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetUserVariableListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * @return Configured User Variable ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetUserVariableListBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size,
                                                  const wxString &nameToExclude,
                                                  bool multiSelect)
{   
   wxArrayString emptyList;
   wxListBox *userVariableListBox = NULL;

   // create ListBox
   if (multiSelect)
   {
      userVariableListBox = new wxListBox(parent, id, wxDefaultPosition, size, 
                                          emptyList, wxLB_EXTENDED|wxLB_SORT|wxLB_HSCROLL);
   }
   else
   {      
      userVariableListBox = new wxListBox(parent, id, wxDefaultPosition, size, 
                                          emptyList, wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   }
   
   // add to ListBox
   if (nameToExclude != "")
   {
      for (int i=0; i<theNumUserVariable; i++)
         if (theUserVariableList[i] != nameToExclude)
            userVariableListBox->Append(theUserVariableList[i]);
   }
   else
   {
      for (int i=0; i<theNumUserVariable; i++)
         userVariableListBox->Append(theUserVariableList[i]);
   }
   
   if (!multiSelect && userVariableListBox->GetCount() > 0)
      userVariableListBox->SetSelection(userVariableListBox->GetCount() - 1);
   
   return userVariableListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetUserStringListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * @return Configured User Valiable ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetUserStringListBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size,
                                                const wxString &nameToExclude,
                                                bool multiSelect)
{
   wxArrayString emptyList;
   wxListBox *userStringListBox = NULL;
   
   // create ListBox
   if (multiSelect)
   {
      userStringListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                        emptyList, wxLB_EXTENDED|wxLB_SORT|wxLB_HSCROLL);
   }
   else
   {
      userStringListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                        emptyList, wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   }
   
   // add to ListBox
   if (nameToExclude != "")
   {
      for (int i=0; i<theNumUserString; i++)
         if (theUserStringList[i] != nameToExclude)
            userStringListBox->Append(theUserStringList[i]);
   }
   else
   {
      for (int i=0; i<theNumUserString; i++)
         userStringListBox->Append(theUserStringList[i]);
   }
   
   if (!multiSelect && userStringListBox->GetCount() > 0)
      userStringListBox->SetSelection(userStringListBox->GetCount() - 1);
   
   return userStringListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetUserArrayListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * @return Configured User Array ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetUserArrayListBox(wxWindow *parent, wxWindowID id,
                                               const wxSize &size,
                                               const wxString &nameToExclude,
                                               bool multiSelect)
{
   wxArrayString emptyList;
   wxListBox *userArrayListBox = NULL;
   
   // create ListBox
   if (multiSelect)
   {
      userArrayListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                       emptyList, wxLB_EXTENDED|wxLB_SORT|wxLB_HSCROLL);
   }
   else
   {
      userArrayListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                       emptyList, wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   }
   
   // add to ListBox
   if (nameToExclude != "")
   {
      for (int i=0; i<theNumUserArray; i++)
         if (theUserArrayList[i] != nameToExclude)
            userArrayListBox->Append(theUserArrayList[i]);
   }
   else
   {
      for (int i=0; i<theNumUserArray; i++)
         userArrayListBox->Append(theUserArrayList[i]);
   }
   
   if (!multiSelect && userArrayListBox->GetCount() > 0)
      userArrayListBox->SetSelection(userArrayListBox->GetCount() - 1);
   
   return userArrayListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetUserParameterListBox(wxWindow *parent, wxWindowID id,
//                                    const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Configured User Array ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetUserParameterListBox(wxWindow *parent, wxWindowID id,
                                                   const wxSize &size)
{
   wxArrayString emptyList;
   wxListBox *userParamListBox = NULL;
   
   if (theNumUserParam > 0)
   {       
      userParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, theUserParamList,
                       wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   }
   else
   {       
      userParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, emptyList,
                       wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   }
   
   return userParamListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetAttachedHardwareListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * Returns spacecraft attached hardware ListBox.
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetAttachedHardwareListBox(wxWindow *parent, wxWindowID id,
                                              const wxSize &size, const wxString &scName)
{
   wxArrayString emptyList;
   wxListBox *hardwareListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, emptyList,
                    wxLB_SINGLE|wxLB_HSCROLL);

   wxArrayString hardwareList = GetAttachedHardwareList(scName);
   
   // Add hardware to ListBox
   for (unsigned int i = 0; i < hardwareList.size(); i++)
      hardwareListBox->Append(hardwareList[i]);
   
   hardwareListBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mHardwareLBList.push_back(hardwareListBox);
   
   return hardwareListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetFuelTankListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetFuelTankListBox(wxWindow *parent, wxWindowID id,
                                              const wxSize &size,
                                              wxArrayString *excList)
{
   #if DBGLVL_GUI_ITEM_HW
   MessageInterface::ShowMessage
      ("GuiItemManager::GetFuelTankListBox() theNumFuelTank=%d, "
       "Exclude count=%d\n", theNumFuelTank, excList->GetCount());
   #endif
   
   wxArrayString emptyList;
   wxListBox *fuelTankListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, emptyList,
                    wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);

   //----------------------------------------------------------------------
   #ifdef __EXCLUDE_FUELTANKS_IN_USE__
   //-------------------------------------------------------
   // exclude FuelTanks used in other spacecrafts
   //-------------------------------------------------------
   Spacecraft *sc = NULL;
   StringArray tankNames, allTankNames;
   int paramID;
   
   for (int i=0; i<theNumSpacecraft; i++)
   {
      sc = theGuiInterpreter->GetSpacecraft(std::string(theSpacecraftList[i].c_str()));
      paramID = sc->GetParameterID("Tanks");
      tankNames = sc->GetStringArrayParameter(paramID);
      allTankNames.insert(allTankNames.end(), tankNames.begin(), tankNames.end());
   }
   
   if (allTankNames.size() > 0)
   {
      for (int i=0; i<theNumFuelTank; i++)
      {
         StringArray::iterator pos =
            find(allTankNames.begin(), allTankNames.end(),
                 std::string(theFuelTankList[i].c_str()));
         
         if (pos == allTankNames.end())
            fuelTankListBox->Append(theFuelTankList[i]);
      }
   }
   #endif
   //----------------------------------------------------------------------
   
   // It's ok to have the same FuleTank in more than one spacecraft since
   // the Sandbox will clone it.
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumFuelTank; i++)
         if (excList->Index(theFuelTankList[i].c_str()) == wxNOT_FOUND)
            fuelTankListBox->Append(theFuelTankList[i]);

   }
   else
   {
      for (int i=0; i<theNumFuelTank; i++)
         fuelTankListBox->Append(theFuelTankList[i]);
   }
   
   fuelTankListBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mFuelTankLBList.push_back(fuelTankListBox);
   mFuelTankExcList.push_back(excList);
   
   return fuelTankListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetThrusterListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetThrusterListBox(wxWindow *parent, wxWindowID id,
                                              const wxSize &size,
                                              wxArrayString *excList)
{
   wxArrayString emptyList;
   wxListBox *thrusterListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, emptyList,
                    wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   
   // It's ok to have the same Thruster in more than one spacecraft isince
   // the Sandbox will clone it
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumThruster; i++)
         if (excList->Index(theThrusterList[i].c_str()) == wxNOT_FOUND)
            thrusterListBox->Append(theThrusterList[i]);
   }
   else
   {
      for (int i=0; i<theNumThruster; i++)
         thrusterListBox->Append(theThrusterList[i]);
   }
   
   thrusterListBox->SetSelection(0);

   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mThrusterLBList.push_back(thrusterListBox);
   mThrusterExcList.push_back(excList);
   
   return thrusterListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetSensorListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetSensorListBox(wxWindow *parent, wxWindowID id,
                                            const wxSize &size,
                                            wxArrayString *excList)
{
   wxArrayString emptyList;
   wxListBox *sensorListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, emptyList,
                    wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
   
   // It's ok to have the same Sensor in more than one spacecraft isince
   // the Sandbox will clone it
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumSensor; i++)
         if (excList->Index(theSensorList[i].c_str()) == wxNOT_FOUND)
            sensorListBox->Append(theSensorList[i]);
   }
   else
   {
      for (int i=0; i<theNumSensor; i++)
         sensorListBox->Append(theSensorList[i]);
   }
   
   sensorListBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mSensorLBList.push_back(sensorListBox);
   mSensorExcList.push_back(excList);
   
   return sensorListBox;
}


//------------------------------------------------------------------------------
// Wxboxsizer* CreateParameterSizer(...)
//------------------------------------------------------------------------------
/**
 * Creates new parameter sizer.
 */
//------------------------------------------------------------------------------
wxSizer* GuiItemManager::CreateParameterSizer
   (wxWindow *parent,
    wxCheckBox **entireObjCheckBox, wxWindowID entireObjCheckBoxId,
    wxComboBox **objectTypeComboBox, wxWindowID objectTypeComboBoxId,
    wxListBox **objectListBox, wxWindowID objectListBoxId,
    wxStaticText **hardwareStaticText, wxWindowID hardwareTextId,
    wxListBox **hardwareListBox, wxWindowID hardwareListBoxId,
    wxStaticText **rowStaticText, wxWindowID rowStaticTextId,
    wxStaticText **colStaticText, wxWindowID colStaticTextId,
    wxTextCtrl **rowTextCtrl, wxWindowID rowTextCtrlId,
    wxTextCtrl **colTextCtrl, wxWindowID colTextCtrlId,
    wxListBox **propertyListBox, wxWindowID propertyListBoxId,
    wxComboBox **coordSysComboBox, wxWindowID coordSysComboBoxId,
    wxComboBox **originComboBox, wxWindowID originComboBoxId,
    wxComboBox **odeModelComboBox, wxWindowID odeModelComboBoxId,
    wxStaticText **coordSysLabel, wxBoxSizer **coordSysBoxSizer,
    wxButton **upButton, wxWindowID upButtonId,
    wxButton **downButton, wxWindowID downButtonId,
    wxButton **addButton, wxWindowID addButtonId,
    wxButton **removeButton, wxWindowID removeButtonId,
    wxButton **addAllButton, wxWindowID addAllButtonId,
    wxButton **removeAllButton, wxWindowID removeAllButtonId,
    wxListBox **selectedListBox, wxWindowID selectedListBoxId,
    const wxArrayString &objectTypeList, int showOption,
    int showObjectOption, bool showSettableOnly, bool allowMultiSelect,
    bool showString, bool showSysParam, bool showVariable, bool showArray,
    bool showArrayElement, bool forStopCondition, const wxString &objectType,
    const wxString configSection)
{
   #if DEBUG_PARAM_SIZER
   MessageInterface::ShowMessage
      ("GuiItemManager::CreateParameterSizer() entered\n"
       "   showOption=%d, showObjectOption=%d, allowMultiSelect=%d, showString=%d, "
       "showSysParam=%d\n   showVariable=%d, showArray=%d, showArrayElement=%d, "
       "forStopCondition=%d, objectType=%s\n", showOption, showObjectOption,
       allowMultiSelect, showString, showSysParam, showVariable, showArray,
       showArrayElement, forStopCondition, objectType.c_str());
   #endif
   
   int bsize = 1;
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/"+configSection));
   
   //-----------------------------------------------------------------
   // Object type and list
   //-----------------------------------------------------------------
   wxStaticText *objectTypeStaticText =
      new wxStaticText(parent, -1, "Object " GUI_ACCEL_KEY "Type",
                       wxDefaultPosition, wxDefaultSize, 0);
   
   *entireObjCheckBox =
      new wxCheckBox(parent, entireObjCheckBoxId, "Select " GUI_ACCEL_KEY "Entire Object");
   (*entireObjCheckBox)->SetToolTip(pConfig->Read(_T("SelectEntireObjectHint")));

   if (showObjectOption == 0)
      (*entireObjCheckBox)->Disable();
   
   wxArrayString tmpObjTypeList;
   
   // add more object types to the list
   if (showSysParam || (showObjectOption == 1))
   {
      // Add object types to list only if there are objects of that type is available
      // (LOJ: 2012.12.28)
      //tmpObjTypeList = objectTypeList;
      StringArray objList;
      for (unsigned int i = 0; i < objectTypeList.size(); i++)
      {
         #if DEBUG_PARAM_SIZER
         MessageInterface::ShowMessage
            ("   objectTypeList[%d] = '%s'\n", i, objectTypeList[i].c_str());
         #endif
         
         objList = theGuiInterpreter->GetListOfObjects(objectTypeList[i].c_str());
         if (objList.size() > 0)
            tmpObjTypeList.Add(objectTypeList[i]);
      }
   }
   
   if (showVariable)
      tmpObjTypeList.Add("Variable");
   if (showArray)
      tmpObjTypeList.Add("Array");
   if (showString)
      tmpObjTypeList.Add("String");
   
   *objectTypeComboBox =
      GetObjectTypeComboBox(parent, objectTypeComboBoxId, wxSize(170, -1),
                            tmpObjTypeList);
   (*objectTypeComboBox)->SetToolTip(pConfig->Read(_T("ObjectTypeListHint")));
   
   // set default object type and get appropriate ListBox
   (*objectTypeComboBox)->SetValue(objectType);
   
   wxStaticText *objectStaticText =
      new wxStaticText(parent, -1, GUI_ACCEL_KEY"Object List",
                       wxDefaultPosition, wxDefaultSize, 0);   
   
   if (objectType == "Spacecraft")
   {
      // Show entire object option
      (*entireObjCheckBox)->Hide();
      if (showObjectOption == 1)
      {
         (*entireObjCheckBox)->SetLabel("Select Entire Object");
         (*entireObjCheckBox)->Show();
      }
      // create Spacecraft ListBox
      *objectListBox =
         GetSpacecraftListBox(parent, objectListBoxId, wxSize(170, 95), NULL,
                              allowMultiSelect);
      (*objectListBox)->SetToolTip(pConfig->Read(_T("SpacecraftListHint")));
   }
   else if (objectType == "SpacePoint")
   {
      // Show entire object option
      (*entireObjCheckBox)->Hide();
      if (showObjectOption == 1)
      {
         (*entireObjCheckBox)->SetLabel("Select Entire Object");
         (*entireObjCheckBox)->Show();
      }
      // create SpacePoint ListBox
      *objectListBox =
         GetSpacePointListBox(parent, objectListBoxId, wxSize(170, 95),
                             false, true);
      (*objectListBox)->SetToolTip(pConfig->Read(_T("SpacePointListHint")));
   }
   else if (objectType == "ImpulsiveBurn")
   {
      // Show entire object option
      (*entireObjCheckBox)->Hide();
      if (showObjectOption == 1)
      {
         (*entireObjCheckBox)->SetLabel("Select Entire Object");
         (*entireObjCheckBox)->Show();
      }
      // create ImpulsiveBurn ListBox
      *objectListBox =
         GetImpBurnListBox(parent, objectListBoxId, wxSize(170, 95), NULL,
                           allowMultiSelect);
      (*objectListBox)->SetToolTip(pConfig->Read(_T("ImpulsiveBurnListHint")));
   }
   else
   {
      // create Variable ListBox
      *objectListBox =
         GetUserVariableListBox(parent, objectListBoxId, wxSize(170, 95), "",
                                allowMultiSelect);
      (*objectListBox)->SetToolTip(pConfig->Read(_T("VariableListHint")));
      
      // set object type to Variable
      (*objectTypeComboBox)->SetValue("Variable");
   }
   
   //-----------------------------------------------------------------
   // Spacecraft attached hardware list
   //-----------------------------------------------------------------
   *hardwareStaticText =
      new wxStaticText(parent, -1, GUI_ACCEL_KEY"Attached Hardware List",
                       wxDefaultPosition, wxDefaultSize, 0);
   *hardwareListBox =
      GetAttachedHardwareListBox(parent, hardwareListBoxId, wxSize(170, 63), "");
   (*hardwareListBox)->SetToolTip(pConfig->Read(_T("AttachedHardwareListHint")));
   
   //----- hardwareSizer
   wxFlexGridSizer *hardwareSizer = new wxFlexGridSizer(1);
   hardwareSizer->Add(*hardwareStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   hardwareSizer->Add(*hardwareListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Array row and column
   //-----------------------------------------------------------------
   //----- arrayIndexSizer
   wxFlexGridSizer *arrayIndexSizer = new wxFlexGridSizer(3);
   if (showArrayElement)
   {
      *rowStaticText = new wxStaticText(parent, -1, GUI_ACCEL_KEY"Row [xx]");
      *colStaticText = new wxStaticText(parent, -1, GUI_ACCEL_KEY"Col [xx]");
      
      *rowTextCtrl =
         new wxTextCtrl(parent, -1, wxT("1"), wxDefaultPosition, wxSize(40, -1));
      (*rowTextCtrl)->SetToolTip(pConfig->Read(_T("ArrayRowHint")));
      
      *colTextCtrl =
         new wxTextCtrl(parent, -1, wxT("1"), wxDefaultPosition, wxSize(40, -1));
      (*colTextCtrl)->SetToolTip(pConfig->Read(_T("ArrayColHint")));
      
      //----- arrayIndexSizer
      arrayIndexSizer->Add(*rowStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
      arrayIndexSizer->Add(10, 10);
      arrayIndexSizer->Add(*colStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
      arrayIndexSizer->Add(*rowTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
      arrayIndexSizer->Add(10, 10);
      arrayIndexSizer->Add(*colTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   }
   
   //----- objectSizer
   GmatStaticBoxSizer *objectSizer =
      new GmatStaticBoxSizer(wxVERTICAL, parent, "");
   
   objectSizer->Add(objectTypeStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   objectSizer->Add(*objectTypeComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   objectSizer->Add(10, 5);
   objectSizer->Add(*entireObjCheckBox, 0, wxALIGN_CENTER|wxALL, bsize);
   objectSizer->Add(10, 10);
   objectSizer->Add(objectStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   objectSizer->Add(*objectListBox, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   objectSizer->Add(20, 2);
   objectSizer->Add(hardwareSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   objectSizer->Add(20, 2);
   if (showArrayElement)
      objectSizer->Add(arrayIndexSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Object properties
   //-----------------------------------------------------------------
   //----- propertySizer
   GmatStaticBoxSizer *propertySizer =
      new GmatStaticBoxSizer(wxVERTICAL, parent, "");
   
   if (showOption == SHOW_WHOLE_OBJECT_ONLY)
   {
      (*entireObjCheckBox)->SetValue(true);
      (*entireObjCheckBox)->Disable();
   }
   else
   {
      wxStaticText *propertyStaticText =
         new wxStaticText(parent, -1, "Object " GUI_ACCEL_KEY "Properties",
                          wxDefaultPosition, wxDefaultSize, 0);
      
      *propertyListBox = 
         GetPropertyListBox(parent, propertyListBoxId, wxSize(170, 230), objectType,
                            showOption, showSettableOnly, allowMultiSelect, forStopCondition);
      (*propertyListBox)->SetToolTip(pConfig->Read(_T("ObjectPropertiesHint")));
      
      *coordSysLabel =
         new wxStaticText(parent, -1, "Coordinate " GUI_ACCEL_KEY "System",
                          wxDefaultPosition, wxDefaultSize, 0);
      
      *coordSysComboBox =
         GetCoordSysComboBox(parent, coordSysComboBoxId, wxSize(170, -1));
      (*coordSysComboBox)->SetToolTip(pConfig->Read(_T("CoordinateSystemHint")));
      
      *originComboBox =
         GetCelestialBodyComboBox(parent, originComboBoxId, wxSize(170, -1));
      (*originComboBox)->SetToolTip(pConfig->Read(_T("OriginHint")));

      *odeModelComboBox = 
         GetODEModelComboBox(parent, odeModelComboBoxId, wxSize(170, -1));
      (*odeModelComboBox)->SetToolTip(pConfig->Read(_T("ODEModelHint")));

      
      //----- coordSysBoxSizer
      *coordSysBoxSizer = new wxBoxSizer(wxVERTICAL);   
      (*coordSysBoxSizer)->Add(*coordSysLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
      
      propertySizer->Add(propertyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
      propertySizer->Add(*propertyListBox, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
      propertySizer->Add(*coordSysBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   }
   
   //-----------------------------------------------------------------
   // Arrows
   //-----------------------------------------------------------------
   wxSize buttonSize(60, -1);
   
   #ifdef __WXMAC__
   buttonSize.Set(60, -1);
   #endif
   
   *upButton = new wxButton
      (parent, addButtonId, GUI_ACCEL_KEY"UP", wxDefaultPosition, buttonSize, 0);
   (*upButton)->SetToolTip(pConfig->Read(_T("MoveUpHint"),"Move Up"));
   if (!allowMultiSelect)
      (*upButton)->Disable();
   
   *downButton = new wxButton
      (parent, addButtonId, GUI_ACCEL_KEY"DN", wxDefaultPosition, buttonSize, 0);
   (*downButton)->SetToolTip(pConfig->Read(_T("MoveDownHint"),"Move Down"));
   if (!allowMultiSelect)
      (*downButton)->Disable();
   
   *addButton = new wxButton
      (parent, addButtonId, "-" GUI_ACCEL_KEY ">", wxDefaultPosition, buttonSize, 0);
   (*addButton)->SetToolTip(pConfig->Read(_T("AddSelectedHint"),"Add Selected Item(s)"));
   
   *removeButton = new wxButton
      (parent, removeButtonId, GUI_ACCEL_KEY"<-", wxDefaultPosition, buttonSize, 0);
   (*removeButton)->SetToolTip(pConfig->Read(_T("RemoveSelectedHint"),"Remove Selected Item"));
   
   *addAllButton = new wxButton
      (parent, removeAllButtonId, wxT("=>"), wxDefaultPosition, buttonSize, 0);
   (*addAllButton)->SetToolTip(pConfig->Read(_T("AddAllHint"),"Add All Items"));
   if (!allowMultiSelect)
      (*addAllButton)->Disable();
   
   *removeAllButton = new wxButton
      (parent, removeAllButtonId, "<" GUI_ACCEL_KEY "=", wxDefaultPosition, buttonSize, 0);
   (*removeAllButton)->SetToolTip(pConfig->Read(_T("RemoveAllHint"),"Remove All Items"));
   
   //----- arrowButtonsBoxSizer
   wxBoxSizer *arrowButtonsSizer = new wxBoxSizer(wxVERTICAL);
   arrowButtonsSizer->Add(*upButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsSizer->Add(*downButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsSizer->Add(10, 15);
   arrowButtonsSizer->Add(*addButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsSizer->Add(*removeButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsSizer->Add(10, 15);
   arrowButtonsSizer->Add(*addAllButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsSizer->Add(*removeAllButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Selected values
   //-----------------------------------------------------------------
   wxStaticText *selectedLabel =
      new wxStaticText(parent, -1, "Selected " GUI_ACCEL_KEY "Value(s)",
                       wxDefaultPosition, wxDefaultSize, 0);
   
   wxArrayString emptyList;
   
   // make single selection so we can do move up and down
   // LOJ: Changed to show horizontal scrollbar if contents are too wide (It works on Windows only). 
   *selectedListBox =
      new wxListBox(parent, -1, wxDefaultPosition, wxSize(200, 270), emptyList,
                    wxLB_SINGLE|wxLB_HSCROLL);
   (*selectedListBox)->SetToolTip(pConfig->Read(_T("SelectedListHint")));
   
   //----- selectedSizer
   GmatStaticBoxSizer *selectedSizer =
      new GmatStaticBoxSizer(wxVERTICAL, parent, "");
   
   selectedSizer->Add(selectedLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   selectedSizer->Add(*selectedListBox, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   
   //-----------------------------------------------------------------
   // Sizer to return
   //-----------------------------------------------------------------
   //----- paramSizer
   wxFlexGridSizer *paramSizer = new wxFlexGridSizer(4);
   paramSizer->Add(objectSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   if (showOption != SHOW_WHOLE_OBJECT_ONLY)
      paramSizer->Add(propertySizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   paramSizer->Add(arrowButtonsSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   paramSizer->Add(selectedSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   return paramSizer;
}


//------------------------------------------------------------------------------
// wxSizer* CreateUserVarSizer(...)
//------------------------------------------------------------------------------
/**
 * Creates parameter sizer.
 */
//------------------------------------------------------------------------------
wxSizer* GuiItemManager::CreateUserVarSizer
   (wxWindow *parent,
    wxListBox **userParamListBox, wxWindowID userParamListBoxId,
    wxButton **createVarButton, wxWindowID createVarButtonId,
    int showOption, bool showArray)
{
   #if DBGLVL_GUI_ITEM
   MessageInterface::ShowMessage("GuiItemManager::CreateUserVarSizer() entered\n");
   #endif
   
   int bsize = 1;
   
   //wxStaticBox
   wxStaticBox *userParamStaticBox = new wxStaticBox(parent, -1, wxT(""));
   
   //wxStaticText
   wxStaticText *userVarStaticText =
      new wxStaticText(parent, -1, wxT("Variables"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // wxButton
   *createVarButton =
      new wxButton(parent, createVarButtonId, wxT("Create"),
                   wxDefaultPosition, wxSize(-1,-1), 0 );
   
   if (showOption == SHOW_REPORTABLE)
   {
      *userParamListBox =
         GetAllUserParameterListBox(parent, userParamListBoxId, wxSize(170, 50), showArray);
   }
   else
   {
      *userParamListBox =
         GetUserVariableListBox(parent, userParamListBoxId, wxSize(170, 50), "");
   }
   
   //----- userParamBoxSizer
   wxStaticBoxSizer *userParamBoxSizer =
      new wxStaticBoxSizer(userParamStaticBox, wxVERTICAL);
   
   userParamBoxSizer->Add
      (userVarStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
   userParamBoxSizer->Add
      (*userParamListBox, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
   userParamBoxSizer->Add
      (*createVarButton, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
   
   
   //----- paramBoxSizer
   wxBoxSizer *paramSizer = new wxBoxSizer(wxVERTICAL);
   paramSizer->Add(userParamBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   return paramSizer;
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// wxArrayString BuildSpacePointList(const wxString &spTypeNames)
//------------------------------------------------------------------------------
/**
 * Builds SpacePoint list by parsing input space point type names.
 *
 * @param spTypeNames  String contain space point type names to include in the
 *                     build array (e.g. "Spacecraft+GroundStation")
 */
//------------------------------------------------------------------------------
wxArrayString GuiItemManager::BuildSpacePointList(const wxString &spTypeNames)
{
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::BuildSpacePointList() entered, spTypeNames='%s'\n", spTypeNames.WX_TO_C_STRING);
   #endif
   
   wxArrayString spacePointArray;
   std::string typeNames = spTypeNames.WX_TO_STD_STRING;
   // wcs 2015.05.08 I think this needs to be here if Spacecraft is
   // not part of the list (so that the list starts with a '+')
   //   if (typeNames[0] == '+')
   //      typeNames.erase(0,1);
   StringArray objTypes = GmatStringUtil::SeparateBy(typeNames, "+", false, false, false);
   
   #if DBGLVL_GUI_ITEM_SP > 1
   for (unsigned int i = 0; i < objTypes.size(); i++)
      MessageInterface::ShowMessage("   objTypes[%2d] = '%s'\n", i, objTypes[i].c_str());
   #endif
   
   GmatBase *obj = NULL;
   for (int i = 0; i < theNumSpacePoint; i++)
   {
      obj = theGuiInterpreter->GetConfiguredObject(theSpacePointList[i].c_str());
      #if DBGLVL_GUI_ITEM_SP > 1
      MessageInterface::ShowMessage("   objNames[%2d] = '%s'\n", i, theSpacePointList[i].WX_TO_C_STRING);
      #endif
      for (unsigned int j = 0; j < objTypes.size(); j++)
      {
         if (obj->IsOfType(objTypes[j]))
         {
            spacePointArray.Add(theSpacePointList[i]);
            break;
         }
      }
   }
   
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::BuildSpacePointList() returning %d SpacePoints\n", spacePointArray.size());
   for (unsigned int i = 0; i < spacePointArray.size(); i++)
      MessageInterface::ShowMessage("   '%s'\n", spacePointArray[i].WX_TO_C_STRING);
   #endif
   
   return spacePointArray;
}

//------------------------------------------------------------------------------
// wxArrayString GetSpacecraftProperties(int showOption, bool showSettableOnly)
//------------------------------------------------------------------------------
wxArrayString GuiItemManager::GetSpacecraftProperties(int showOption,
                                                      bool showSettableOnly)
{
   #if DBGLVL_GUI_ITEM_PROPERTY > 1
   MessageInterface::ShowMessage
      ("GuiItemManager::GetSpacecraftProperties() entered, showOption=%d, "
       "showSettableOnly=%d\n", showOption, showSettableOnly);
   #endif
   
   wxArrayString array;
   ParameterInfo *theParamInfo = ParameterInfo::Instance();
   std::string paramName;
   bool add = false;
   
   for (int i=0; i<theNumScProperty; i++)
   {
      paramName = theScPropertyList[i].c_str();
      add = false;
      #if DBGLVL_GUI_ITEM_PROPERTY > 1
      MessageInterface::ShowMessage("   ==> Checking '%s'\n", paramName.c_str());
      #endif
      
      if (theParamInfo->IsReportable(paramName) && showOption == SHOW_REPORTABLE)
      {
         if (!showSettableOnly)
            add = true;
         else if (theParamInfo->IsSettable(paramName))
            add = true;
      }
      else if (theParamInfo->IsPlottable(paramName) && showOption == SHOW_PLOTTABLE)
      {
         if (!showSettableOnly)
            add = true;
         else if (theParamInfo->IsSettable(paramName))
            add = true;
      }
      
      if (add)
      {
         #if DBGLVL_GUI_ITEM_PROPERTY > 1
         MessageInterface::ShowMessage("       Adding %s\n", paramName.c_str());
         #endif
         
         array.Add(theScPropertyList[i]);
      }
   }
   
   #if DBGLVL_GUI_ITEM_PROPERTY > 1
   MessageInterface::ShowMessage
      ("GuiItemManager::GetSpacecraftProperties() returning %d items\n",
       array.GetCount());
   #endif
   
   return array;
}

//------------------------------------------------------------------------------
// wxArrayString GetSpacePointProperties(int showOption, bool showSettableOnly)
//------------------------------------------------------------------------------
wxArrayString GuiItemManager::GetSpacePointProperties(int showOption,
                                                      bool showSettableOnly)
{
   #if DBGLVL_GUI_ITEM_PROPERTY > 1
   MessageInterface::ShowMessage
      ("GuiItemManager::GetSpacePointProperties() entered, showOption=%d, "
       "showSettableOnly=%d\n", showOption, showSettableOnly);
   #endif

   wxArrayString array;
   ParameterInfo *theParamInfo = ParameterInfo::Instance();
   std::string p;
   bool add = false;

   for (int i=0; i<theNumSpacePtProperty; i++)
   {
      p = theSpacePointPropertyList[i].c_str();
      add = false;
      #if DBGLVL_GUI_ITEM_PROPERTY > 1
      MessageInterface::ShowMessage("   ==> Checking '%s'\n", p.c_str());
      #endif

      if (theParamInfo->IsReportable(p) && showOption == SHOW_REPORTABLE)
      {
         if (!showSettableOnly)
            add = true;
         else if (theParamInfo->IsSettable(p))
         {
            if ((p != "X")  && (p != "Y")  && (p != "Z")    &&
                (p != "VX") && (p != "VY") && (p != "VZ"))
               add = true;
         }
      }
      else if (theParamInfo->IsPlottable(p) && showOption == SHOW_PLOTTABLE)
      {
         if (!showSettableOnly)
            add = true;
         else if (theParamInfo->IsSettable(p))
         {
            if ((p != "X")  && (p != "Y")  && (p != "Z")    &&
                (p != "VX") && (p != "VY") && (p != "VZ"))
               add = true;
         }
      }

      if (add)
      {
         #if DBGLVL_GUI_ITEM_PROPERTY > 1
         MessageInterface::ShowMessage("       Adding %s\n", p.c_str());
         #endif

         array.Add(p.c_str());
      }
   }

   #if DBGLVL_GUI_ITEM_PROPERTY > 1
   MessageInterface::ShowMessage
      ("GuiItemManager::GetSpacePointProperties() returning %d items\n",
       array.GetCount());
   #endif

   return array;
}


//------------------------------------------------------------------------------
//  void UpdatePropertyList()
//------------------------------------------------------------------------------
/**
 * Updates available parameter list associated with Spacecraft and ImpulsiveBurn.
 *
 * @note Only plottable parameters (returning single value) are added to the list.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdatePropertyList()
{
   #if DBGLVL_GUI_ITEM_PROPERTY
   MessageInterface::ShowMessage("GuiItemManager::UpdatePropertyList()\n");
   #endif
   
   StringArray items =
      theGuiInterpreter->GetListOfFactoryItems(Gmat::PARAMETER);
   int numParams = items.size();
   
   #if DBGLVL_GUI_ITEM_PROPERTY
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdatePropertyList() numParams=%d\n", numParams);
   #endif
   
   theNumScProperty = 0;
   theNumSpacePtProperty = 0;
   theNumImpBurnProperty = 0;
   UnsignedInt objectType;
   ParameterInfo *theParamInfo = ParameterInfo::Instance();
   theScPropertyList.Clear();
   theSpacePointPropertyList.Clear();
   theImpBurnPropertyList.Clear();
   
   for (int i=0; i<numParams; i++)
   {
      // Skip CurrA1Mjd from the GUI. (loj: 5/3/06)
      // CurrA1MJD will be removed in a future since it still used in some scripts.
      if (items[i] == "CurrA1MJD")
         continue;
      
      // Do not include attached object dependent Parameters such as Sat1.Tank1.Temperature.
      // ParameterSelectDialog is not ready to handle this kind of Parameters
      // since it needs to show dependent object in the ComboBox. (LOJ: 2012.03.09)
      // Changed to call ParameterInfo::IsForAttachedObject() (LOJ: 2012.04.02)
      // Now ParameterSelectDialog has implemented showing attached hardware Parameters,
      // but we don't want to show attached object Parameters here (2012.06.07)
      //if (theParamInfo->IsForAttachedObject(items[i]))
      //   continue;
      
      // add only reportable parameters (Real, String for now) to list
      // we may want add Rvector6 later, then Rvec6Var needs to be set reportable true
      if (theParamInfo->IsReportable(items[i]))
      {
         objectType = theParamInfo->GetObjectType(items[i]);
         
         #if DBGLVL_GUI_ITEM_PROPERTY > 1
         MessageInterface::ShowMessage
            ("   name: %-20s, objectType: %d\n",  items[i].c_str(), objectType);
         #endif
         // Need to check for SPACE_POINT now too, since for TimeParameters, we
         // can use any SpacePoint, not just SPACECRAFT
         if ((objectType == Gmat::SPACECRAFT) || (objectType == Gmat::SPACE_POINT))
         {
            if (theParamInfo->IsForAttachedObject(items[i]))
               // update Spacecraft attached object property list
               theScAttachedObjPropertyList.Add(items[i].c_str());
            else
               // update Spacecraft property list
               theScPropertyList.Add(items[i].c_str());

            if (objectType == Gmat::SPACE_POINT)
            {
               theSpacePointPropertyList.Add(items[i].c_str());
            }
         }
         else if (objectType == Gmat::IMPULSIVE_BURN)
         {
            // update ImpulsiveBurn property list
            // Do not add depreciated Parameters
            if (items[i] != "V" && items[i] != "N" && items[i] != "B")
               theImpBurnPropertyList.Add(items[i].c_str());
         }
         else if (objectType == Gmat::FINITE_BURN)
         {
            // update FiniteBurn property list
            theFiniteBurnPropertyList.Add(items[i].c_str());
         }
      }
   }
   
   theNumScProperty      = theScPropertyList.GetCount();
   theNumSpacePtProperty = theSpacePointPropertyList.GetCount();
   theNumImpBurnProperty = theImpBurnPropertyList.GetCount();
   theNumFiniteBurnProperty = theFiniteBurnPropertyList.GetCount();
   
   #if DBGLVL_GUI_ITEM_PROPERTY
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdatePropertyList() theNumScProperty=%d, "
       "theScAttachedObjPropertyList.GetCount()=%d, theNumImpBurnProperty=%d\n",
       theNumScProperty, theScAttachedObjPropertyList.GetCount(), theNumImpBurnProperty);
   #endif
}


//------------------------------------------------------------------------------
// void UpdateParameterList()
//------------------------------------------------------------------------------
/**
 * Updates confugured parameter list (thePlottableParamList, theSystemParamList,
 * theUserVariableList, theUserStringList, theUserArrayList).
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateParameterList()
{    
   #if DBGLVL_GUI_ITEM_PARAM
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateParameterList() entered\ntheNumPlottableParam=%d, "
       "theNumSystemParam=%d, theNumUserVariable=%d\ntheNumUserString=%d, "
       "theNumUserArray=%d, theNumUserParam=%d\n", theNumPlottableParam,
       theNumSystemParam, theNumUserVariable, theNumUserString,
       theNumUserArray, theNumUserParam);
   #endif
   
   StringArray items =
      theGuiInterpreter->GetListOfObjects(Gmat::PARAMETER);
   int numParamCount = items.size();
   
   Parameter *param;
   
   theSystemParamList.Clear();
   thePlottableParamList.Clear();
   theUserParamList.Clear();
   theUserVariableList.Clear();
   theUserArrayList.Clear();
   theUserStringList.Clear();
   
   for (int i=0; i<numParamCount; i++)
   {
      param = (Parameter*)theGuiInterpreter->GetConfiguredObject(items[i]);
      
      #if DBGLVL_GUI_ITEM_PARAM > 1
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateParameterList() items[%d]=%s, type=%s\n", i, 
          items[i].c_str(), param->GetTypeName().c_str());
      #endif
      
      // Do not include owned object dependent Paramters such as Sat1.Tank1.Temperature.
      // ParameterSelectDialog is not ready to handle this kind of Parameters
      // since it needs to show dependent object in the ComboBox. (LOJ: 2012.03.09)
      if (param->IsOwnedObjectDependent())
         continue;
      
      // add if parameter plottable (returning single value)
      if (param->IsPlottable())
      {
         thePlottableParamList.Add(items[i].c_str());
      }
      
      // system Parameter (object property)
      if (param->GetKey() == GmatParam::SYSTEM_PARAM)
      {
         theSystemParamList.Add(items[i].c_str());
      }
      else
      {
         // user Variable
         if (param->GetTypeName() == "Variable")
         {
            theUserVariableList.Add(items[i].c_str());
            theUserParamList.Add(items[i].c_str());
         }
         else // not plottable parameters
         {
            // user String
            if (param->GetTypeName() == "String")
            {
               theUserStringList.Add(items[i].c_str());
               theUserParamList.Add(items[i].c_str());
            }
            // user Array
            else if (param->GetTypeName() == "Array")
            {
               theUserArrayList.Add(items[i].c_str());
               theUserParamList.Add(items[i].c_str());
            }
         }
      }
   }
   
   theNumPlottableParam = thePlottableParamList.GetCount();
   theNumSystemParam = theSystemParamList.GetCount();
   theNumUserVariable = theUserVariableList.GetCount();
   theNumUserString = theUserStringList.GetCount();
   theNumUserArray = theUserArrayList.GetCount();
   theNumUserParam = theUserParamList.GetCount();
   
   #if DBGLVL_GUI_ITEM_PARAM
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateParameterList() exiting\ntheNumPlottableParam=%d, "
       "theNumSystemParam=%d, theNumUserVariable=%d\ntheNumUserString=%d, "
       "theNumUserArray=%d, theNumUserParam=%d\n", theNumPlottableParam,
       theNumSystemParam, theNumUserVariable, theNumUserString,
       theNumUserArray, theNumUserParam);
   #endif
}


//------------------------------------------------------------------------------
// void UpdateGroundStationList()
//------------------------------------------------------------------------------
/**
 * Updates configured GroundStation list.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateGroundStationList()
{
   StringArray items =
      theGuiInterpreter->GetListOfObjects(Gmat::GROUND_STATION);
   int numGroundStation = items.size();
   
   #if DBGLVL_GUI_ITEM_GS
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateGroundStationList() numGroundStation=%d\n", numGroundStation);
   #endif
   
   theNumGroundStation = 0;
   theGroundStationList.Clear();
   
   for (int i=0; i<numGroundStation; i++)
   {
      theGroundStationList.Add(items[i].c_str());
      
      #if DBGLVL_GUI_ITEM_GS > 1
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateGroundStationList() " + items[i] + "\n");
      #endif
   }
   
   theNumGroundStation = theGroundStationList.GetCount();
   
   //-------------------------------------------------------
   // update registered GroundStation CheckListBox
   //-------------------------------------------------------
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateGroundStationList() About to update the registered CheckListbox\n");
   #endif
   wxArrayString itemCheckedArray;
   for (std::vector<wxCheckListBox*>::iterator pos = mGroundStationCLBList.begin();
        pos != mGroundStationCLBList.end(); ++pos)
   {
      if ((*pos) == NULL)
         continue;

      itemCheckedArray.Clear();

      // save checked item
      int count = (*pos)->GetCount();
      for (int i=0; i<count; i++)
         if ((*pos)->IsChecked(i))
            itemCheckedArray.Add((*pos)->GetString(i));

      (*pos)->Clear();
      (*pos)->Append(theGroundStationList);

      // restore checked item
      count = (*pos)->GetCount();
      for (int i=0; i<count; i++)
         if (itemCheckedArray.Index((*pos)->GetString(i)) != wxNOT_FOUND)
            (*pos)->Check(i);
   }
} // end UpdateGroundStationList()


//------------------------------------------------------------------------------
//  void UpdateSpacecraftList()
//------------------------------------------------------------------------------
/**
 * updates Spacecraft list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpacecraftList()
{
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSpacecraftList() entered. theNumSpacecraft=%d\n",
       theNumSpacecraft);
   #endif
   
   StringArray scList = theGuiInterpreter->GetListOfObjects(Gmat::SPACECRAFT);
   int numSc = scList.size();
   theSpacecraftList.Clear();
   
   if (numSc > 0)  // check to see if any spacecrafts exist
   {
      for (int i=0; i<numSc; i++)
      {
         theSpacecraftList.Add(scList[i].c_str());
         
         #if DBGLVL_GUI_ITEM_SP
         MessageInterface::ShowMessage
            ("GuiItemManager::UpdateSpacecraftList() theSpacecraftList[%d]=%s\n",
             i, theSpacecraftList[i].WX_TO_C_STRING);
         #endif
      }
   }
   
   theNumSpacecraft = numSc;
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSpacecraftList() setting theNumSpacecraft = %d\n",
            theNumSpacecraft);
   #endif
   
   //-------------------------------------------------------
   // update registered Spacecraft ListBox
   //-------------------------------------------------------
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSpacecraftList() About to update the registered SpacecraftListbox\n");
   MessageInterface::ShowMessage("      size of mSpacecraftExcList is %d\n",
         (Integer) mSpacecraftExcList.size());
   #endif
   std::vector<wxArrayString*>::iterator exPos = mSpacecraftExcList.begin();
   
   for (std::vector<wxListBox*>::iterator pos = mSpacecraftLBList.begin();
        pos != mSpacecraftLBList.end(); ++pos)
   {
      wxArrayString *excList = *exPos++;
      (*pos)->Clear();
      #if DBGLVL_GUI_ITEM_SP
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateSpacecraftList() excList is %s\n",
               (excList? "NOT NULL" : "NULL"));
      #endif

      for (int i=0; i<theNumSpacecraft; i++)
      {
         #if DBGLVL_GUI_ITEM_SP
         MessageInterface::ShowMessage
            ("GuiItemManager::UpdateSpacecraftList() checking for index %s\n",
                  theSpacecraftList[i].WX_TO_C_STRING);
         #endif
         if (excList != NULL && (excList->Index(theSpacecraftList[i].c_str()) == wxNOT_FOUND))
            (*pos)->Append(theSpacecraftList[i]);
      }
   }
   
   //-------------------------------------------------------
   // update registered Spacecraft ComboBox
   //-------------------------------------------------------
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSpacecraftList() About to update the registered ComboBox\n");
   #endif
   for (std::vector<wxComboBox*>::iterator pos = mSpacecraftCBList.begin();
        pos != mSpacecraftCBList.end(); ++pos)
   {
      if ((*pos) == NULL)
         continue;
      
      int sel = (*pos)->GetSelection();
      
      (*pos)->Clear();
      (*pos)->Append(theSpacecraftList);
      (*pos)->SetSelection(sel);
   }
   
   //-------------------------------------------------------
   // update registered Spacecraft CheckListBox
   //-------------------------------------------------------
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSpacecraftList() About to update the registered CheckListbox\n");
   #endif
   wxArrayString itemCheckedArray;
   for (std::vector<wxCheckListBox*>::iterator pos = mSpacecraftCLBList.begin();
        pos != mSpacecraftCLBList.end(); ++pos)
   {
      if ((*pos) == NULL)
         continue;

      itemCheckedArray.Clear();
      
      // save checked item
      int count = (*pos)->GetCount();
      for (int i=0; i<count; i++)
         if ((*pos)->IsChecked(i))
            itemCheckedArray.Add((*pos)->GetString(i));
      
      (*pos)->Clear();
      (*pos)->Append(theSpacecraftList);
      
      // restore checked item
      count = (*pos)->GetCount();
      for (int i=0; i<count; i++)
         if (itemCheckedArray.Index((*pos)->GetString(i)) != wxNOT_FOUND)
            (*pos)->Check(i);
   }
   
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSpacecraftList() exiting. theNumSpacecraft=%d\n",
       theNumSpacecraft);
   #endif
   
} // end UpdateSpacecraftList()


//------------------------------------------------------------------------------
//  void UpdateFormationList()
//------------------------------------------------------------------------------
/**
 * updates Formation list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateFormationList()
{
   StringArray listForm = theGuiInterpreter->GetListOfObjects(Gmat::FORMATION);
   int numForm = listForm.size();
   
   #if DBGLVL_GUI_ITEM_SO
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateFormationList() numForm=%d\n", numForm);
   #endif
   
   theFormationList.Clear();
   
   // check to see if any spacecrafts exist
   for (int i=0; i<numForm; i++)
   {
      theFormationList.Add(listForm[i].c_str());
      
      #if DBGLVL_GUI_ITEM_SO > 1
      MessageInterface::ShowMessage
         ("   %s added to theFormationList\n", theFormationList[i].c_str());
      #endif
   }
   
   theNumFormation = theFormationList.GetCount();
   
}


//------------------------------------------------------------------------------
//  void UpdateSpaceObjectList()
//------------------------------------------------------------------------------
/**
 * updates Spacecraft and Formation list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpaceObjectList()
{
   #if DBGLVL_GUI_ITEM_SO
   MessageInterface::ShowMessage("GuiItemManager::UpdateSpaceObjectList() entered\n");
   #endif
   
   StringArray scList = theGuiInterpreter->GetListOfObjects(Gmat::SPACECRAFT);
   StringArray fmList = theGuiInterpreter->GetListOfObjects(Gmat::FORMATION);
   
   int numSc = scList.size();
   int numFm = fmList.size();
   int numObj = 0;
   int soCount = 0;
   
   #if DBGLVL_GUI_ITEM_SO > 1
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSpaceObjectList() ==========>\n");
   MessageInterface::ShowMessage("numSc=%d, scList=", numSc);
   for (int i=0; i<numSc; i++)
      MessageInterface::ShowMessage("%s ", scList[i].c_str());
   MessageInterface::ShowMessage("\nnumFm=%d, fmList=", numFm);
   for (int i=0; i<numFm; i++)
      MessageInterface::ShowMessage("%s ", fmList[i].c_str());
   MessageInterface::ShowMessage("\n");   
   #endif
   
   theSpaceObjectList.Clear();
   
   //--------------------------------------
   // if any space objects are configured
   //--------------------------------------
   if ((numSc + numFm) > 0)
   {
      // if formation exists
      if (numFm > 0)
      {
         StringArray fmscListAll;
         
         //------------------------------------------
         // Merge spacecrafts in Formation
         //------------------------------------------
         for (int i=0; i<numFm; i++)
         {
            GmatBase *fm = theGuiInterpreter->GetConfiguredObject(fmList[i]);
            StringArray fmscList = fm->GetStringArrayParameter(fm->GetParameterID("Add"));
            fmscListAll.insert(fmscListAll.begin(), fmscList.begin(), fmscList.end());
         }
         
         sort(scList.begin(), scList.end());
         sort(fmscListAll.begin(), fmscListAll.end());
         
         //------------------------------------------
         // Make list of spacecrafts not in Formation
         //------------------------------------------
         StringArray result;
         set_difference(scList.begin(), scList.end(), fmscListAll.begin(),
                        fmscListAll.end(), back_inserter(result));
         
         numObj = result.size();
         
         //------------------------------------------
         // Add new list to theSpaceObjectList
         //------------------------------------------
         if (numObj > 0)  // check to see if any objects exist
         {
            for (int i=0; i<numObj; i++)
            {
               theSpaceObjectList.Add(result[i].c_str());
               
               #if DBGLVL_GUI_ITEM_SO > 1
               MessageInterface::ShowMessage
                  ("   %s added to theSpaceObjectList\n",
                   theSpaceObjectList[i].c_str());
               #endif
            }
         }
         
         
         //----------------------------------------------------------------
         #ifdef __ADD_FORMATION_TO_ARRAY__
         //----------------------------------------------------------------
         //loj: 7/14/05
         // Do not add Formation to theSpaceObjectList until it can handle
         // not including each other; e.g) form1 includes form2 and form2
         // includes form1.
         //----------------------------------------------------------------
         
         //------------------------------------------
         // Add formation to theSpaceObjectList
         //------------------------------------------
         for (int i=0; i<numFm; i++)
         {
            theSpaceObjectList.Add(fmList[i].c_str());
            
            #if DBGLVL_GUI_ITEM_SO > 1
            MessageInterface::ShowMessage
               ("   %s added to theSpaceObjectList\n",
                theSpaceObjectList[i].c_str());
            #endif
         }
         //----------------------------------------------------------------
         #endif
      }
      // no formation, Save scList to theSpaceObjectList
      else
      {
         soCount = numSc;
         for (int i=0; i<soCount; i++)
         {
            theSpaceObjectList.Add(scList[i].c_str());
            
            #if DBGLVL_GUI_ITEM_SO > 1
            MessageInterface::ShowMessage
               ("   %s added to theSpaceObjectList\n",
                theSpaceObjectList[i].c_str());
            #endif
         }
      }
   }
   
   theNumSpaceObject = theSpaceObjectList.GetCount();
   
   //-------------------------------------------------------
   // update registered SpaceObjectListBox
   //-------------------------------------------------------
   std::vector<wxArrayString*>::iterator exPos = mSpaceObjectExcList.begin();
   
   for (std::vector<wxListBox*>::iterator pos = mSpaceObjectLBList.begin();
        pos != mSpaceObjectLBList.end(); ++pos)
   {
      wxArrayString *excList = *exPos++;
      (*pos)->Clear();
      
      for (int i=0; i<theNumSpaceObject; i++)
      {
         if (excList->Index(theSpaceObjectList[i].c_str()) == wxNOT_FOUND)
            (*pos)->Append(theSpaceObjectList[i]);
      }
      
      (*pos)->SetSelection((*pos)->GetCount() - 1);

   }
   
   #if DBGLVL_GUI_ITEM_SO
   MessageInterface::ShowMessage
      ("theNumSpaceObject=%d\n"
       "<==========GuiItemManager::UpdateSpaceObjectList() exiting\n",
       theNumSpaceObject);
   #endif
}


//------------------------------------------------------------------------------
// void UpdateCelestialBodyList()
//------------------------------------------------------------------------------
/**
 * Updates configured CelestialBody list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateCelestialBodyList()
{
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage("GuiItemManager::UpdateCelestialBodyList() entered\n");
   #endif
   
   StringArray items = theGuiInterpreter->GetListOfObjects(Gmat::CELESTIAL_BODY);
   theNumCelesBody = items.size();
   theCelestialBodyList.Clear();
   
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateCelestialBodyList() Adding %d items\n", theNumCelesBody);
   #endif
   
   for (int i=0; i<theNumCelesBody; i++)
   {
      theCelestialBodyList.Add(items[i].c_str());
      
      #if DBGLVL_GUI_ITEM > 1
      MessageInterface::ShowMessage("GuiItemManager::UpdateCelestialBodyList() " +
                                    std::string(theCelestialBodyList[i].c_str()) + "\n");
      #endif
   }
   
   //-------------------------------------------------------
   // update registered CelestialBody ComboBox
   //-------------------------------------------------------
   for (std::vector<wxComboBox*>::iterator pos = mCelestialBodyCBList.begin();
        pos != mCelestialBodyCBList.end(); ++pos)
   {
      wxString str = (*pos)->GetStringSelection();
      (*pos)->Clear();
      (*pos)->Append(theCelestialBodyList);
      (*pos)->SetStringSelection(str);
   }
   
   //-------------------------------------------------------
   // update registered CelestialBodyListBox
   //-------------------------------------------------------
   std::vector<wxArrayString*>::iterator exPos = mCelestialBodyExcList.begin();
   
   for (std::vector<wxListBox*>::iterator pos = mCelestialBodyLBList.begin();
        pos != mCelestialBodyLBList.end(); ++pos)
   {
      wxArrayString *excList = *exPos;
      (*pos)->Clear();
      
      #if DBGLVL_GUI_ITEM_REG
      MessageInterface::ShowMessage
         ("===============> registerdListBox=<%p>, excList=<%p>\n", *pos, excList);
      #endif
      
      for (int i=0; i<theNumCelesBody; i++)
      {
         if (excList != NULL && excList->Index(theCelestialBodyList[i].c_str()) == wxNOT_FOUND)
         {
            #if DBGLVL_GUI_ITEM_REG
            MessageInterface::ShowMessage
               ("   ==> Appending '%s'\n", theCelestialBodyList[i].c_str());
            #endif
            (*pos)->Append(theCelestialBodyList[i]);
         }
         else
         {
            #if DBGLVL_GUI_ITEM_REG
            MessageInterface::ShowMessage("   ==> Excluding '%s'\n", theCelestialBodyList[i].c_str());
            #endif
         }
      }
      
      (*pos)->SetSelection(0);
      exPos++;
   }
   
}


//------------------------------------------------------------------------------
// void UpdateCelestialPointList()
//------------------------------------------------------------------------------
/**
 * Updates confugured CelestialBody and CalculatedPoint list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateCelestialPointList()
{
   StringArray celesBodyList =
      theGuiInterpreter->GetListOfObjects(Gmat::CELESTIAL_BODY);
   StringArray calPointList =
      theGuiInterpreter->GetListOfObjects(Gmat::CALCULATED_POINT);
   
   theNumCelesBody = celesBodyList.size();
   theNumCalPoint = calPointList.size();
   theNumCelesPoint = theNumCelesBody + theNumCalPoint;
   
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateCelestialPointList() entered, theNumCelesBody=%d, "
       "theNumCalPoint=%d, theNumCelesPoint=%d\n", theNumCelesBody, theNumCalPoint,
       theNumCelesPoint);
   #endif
   
   theCelestialBodyList.Clear();
   theCalPointList.Clear();
   theCelestialPointList.Clear();
   
   // update CelestialBody list
   for (int i=0; i<theNumCelesBody; i++)
      theCelestialBodyList.Add(celesBodyList[i].c_str());
   
   // update CalculatedPoint list
   for (int i=0; i<theNumCalPoint; i++)
      theCalPointList.Add(calPointList[i].c_str());
   
   // add CelestialBody to CelestionPoint list
   for (int i=0; i<theNumCelesBody; i++)
      theCelestialPointList.Add(theCelestialBodyList[i]);
   
   // add CalculatedPoint to CelestialPoint list
   for (int i=0; i<theNumCalPoint; i++)
      theCelestialPointList.Add(theCalPointList[i]);
   
   //-------------------------------------------------------
   // update registered CelestialPointListBox
   //-------------------------------------------------------
   std::vector<wxArrayString*>::iterator exPos = mCelestialPointExcList.begin();
   
   for (std::vector<wxListBox*>::iterator pos = mCelestialPointLBList.begin();
        pos != mCelestialPointLBList.end(); ++pos)
   {
      wxArrayString *excList = *exPos;
      (*pos)->Clear();
      
      #if DBGLVL_GUI_ITEM_REG
      MessageInterface::ShowMessage
         ("===============> registerdListBox=<%p>, excList=<%p>\n", *pos, excList);
      #endif
      
      for (int i=0; i<theNumCelesPoint; i++)
      {
         if (excList != NULL && excList->Index(theCelestialPointList[i].c_str()) == wxNOT_FOUND)
            (*pos)->Append(theCelestialPointList[i]);
      }
      
      (*pos)->SetSelection(0);
      exPos++;
   }
   
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage("GuiItemManager::UpdateCelestialPointList() leaving\n");
   #endif
} //UpdateCelestialPointList()


//------------------------------------------------------------------------------
// void UpdateSpacePointList()
//------------------------------------------------------------------------------
/**
 * Updates configured SpacePoint list:
 *    Spacecraft, CelestialBody, CalculatedPoint, GroundStation
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpacePointList()
{
   StringArray spList = theGuiInterpreter->GetListOfObjects(Gmat::SPACE_POINT);
   theNumSpacePoint = spList.size();
   
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSpacePointList() entered, theNumSpacePoint=%d\n",
       theNumSpacePoint);
   #endif
   
   theSpacePointList.Clear();
   
   for (int i=0; i<theNumSpacePoint; i++)
   {
      theSpacePointList.Add(spList[i].c_str());
      
      #if DBGLVL_GUI_ITEM_SP > 1
      MessageInterface::ShowMessage
         ("   theSpacePointList[%d]=%s\n", i, theSpacePointList[i].WX_TO_C_STRING);
      #endif
   }
   
   //-------------------------------------------------------
   // update registered SpacePoint ComboBox
   //-------------------------------------------------------
   for (std::vector<wxComboBox*>::iterator pos = mSpacePointCBList.begin();
        pos != mSpacePointCBList.end(); ++pos)
   {
      wxString str = (*pos)->GetStringSelection();
      
      if ((*pos)->FindString("Vector") == wxNOT_FOUND)
      {
         (*pos)->Clear();
      }
      else
      {
         (*pos)->Clear();
         (*pos)->Append("Vector");
      }
      
      (*pos)->Append(theSpacePointList);
      (*pos)->SetStringSelection(str);
   }
   
   //-------------------------------------------------------
   // update registered SpacePoint CheckListBox (LOJ: added 2013.09.17)
   //-------------------------------------------------------
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage("   About to update registered CheckListbox\n");
   #endif
   wxArrayString itemCheckedArray;
   for (std::vector<wxCheckListBox*>::iterator pos = mSpacePointCLBList.begin();
        pos != mSpacePointCLBList.end(); ++pos)
   {
      if ((*pos) == NULL)
         continue;
      
      itemCheckedArray.Clear();
      
      // save checked item
      int count = (*pos)->GetCount();
      for (int i=0; i<count; i++)
         if ((*pos)->IsChecked(i))
            itemCheckedArray.Add((*pos)->GetString(i));
      
      (*pos)->Clear();
      // Build new space point list (LOJ: 2013.09.18)
      //(*pos)->Append(theSpacePointList);
      wxArrayString newSpacePointList = BuildSpacePointList((*pos)->GetName());
      (*pos)->Append(newSpacePointList);
      
      // restore checked item
      count = (*pos)->GetCount();
      for (int i=0; i<count; i++)
         if (itemCheckedArray.Index((*pos)->GetString(i)) != wxNOT_FOUND)
            (*pos)->Check(i);
   }
   
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage("GuiItemManager::UpdateSpacePointList() leaving\n");
   #endif
  
}


//------------------------------------------------------------------------------
// void UpdateBurnList()
//------------------------------------------------------------------------------
/**
 * Updates confugured burn list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateBurnList()
{
   StringArray items = theGuiInterpreter->GetListOfObjects(Gmat::BURN);
   int numBurn = items.size();
   
   #if DBGLVL_GUI_ITEM_BURN
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateBurnList() numBurn=%d\n", numBurn);
   #endif
   
   theNumImpBurn = 0;
   theNumFiniteBurn = 0;
   GmatBase *obj;
   theImpBurnList.Clear();
   theFiniteBurnList.Clear();
   
   for (int i=0; i<numBurn; i++)
   {
      #if DBGLVL_GUI_ITEM_BURN
      MessageInterface::ShowMessage("   items[%d] = '%s'\n", i, items[i].c_str());
      #endif
      
      obj = theGuiInterpreter->GetConfiguredObject(items[i]);
      if (obj->GetTypeName() == "ImpulsiveBurn")
      {
         theImpBurnList.Add(items[i].c_str());
         
         #if DBGLVL_GUI_ITEM_BURN > 1
         MessageInterface::ShowMessage
            ("   '%s' added to theImpBurnList\n", theImpBurnList.Last().WX_TO_C_STRING);
         #endif
      }
      else if (obj->GetTypeName() == "FiniteBurn")
      {
         theFiniteBurnList.Add(items[i].c_str());
         
         #if DBGLVL_GUI_ITEM_BURN > 1
         MessageInterface::ShowMessage
            ("   '%s' added to theFiniteBurnList\n", theFiniteBurnList.Last().WX_TO_C_STRING);
         #endif
      }
   }
   
   theNumImpBurn = theImpBurnList.GetCount();
   theNumFiniteBurn = theFiniteBurnList.GetCount();
   
   //-------------------------------------------------------
   // update registered ImpulsiveBurn ComboBox
   //-------------------------------------------------------
   int sel;
   for (std::vector<wxComboBox*>::iterator pos = mImpBurnCBList.begin();
        pos != mImpBurnCBList.end(); ++pos)
   {
      sel = (*pos)->GetSelection();
      
      (*pos)->Clear();
      (*pos)->Append(theImpBurnList);
      (*pos)->SetSelection(sel);
   }
   
   //-------------------------------------------------------
   // update registered FiniteBurn ComboBox
   //-------------------------------------------------------
   for (std::vector<wxComboBox*>::iterator pos = mFiniteBurnCBList.begin();
        pos != mFiniteBurnCBList.end(); ++pos)
   {      
       sel = (*pos)->GetSelection();

      (*pos)->Clear();
      (*pos)->Append(theFiniteBurnList);
      (*pos)->SetSelection(sel);
   }
   
} //UpdateBurnList()


//------------------------------------------------------------------------------
// void UpdateCoordSystemList()
//------------------------------------------------------------------------------
/**
 * Updates configured coordinate system list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateCoordSystemList()
{
   StringArray items =
      theGuiInterpreter->GetListOfObjects(Gmat::COORDINATE_SYSTEM);
   theNumCoordSys = items.size();
   
   #if DBGLVL_GUI_ITEM_CS
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateCoordSystemList() theNumCoordSys=%d\n",
       theNumCoordSys);
   #endif
   
   theCoordSysList.Clear();
   
   for (int i=0; i<theNumCoordSys; i++)
   {
      theCoordSysList.Add(items[i].c_str());
      
      #if DBGLVL_GUI_ITEM_CS > 1
      MessageInterface::ShowMessage
         ("   %s added to theCoordSysList\n", theCoordSysList[i].c_str());
      #endif
   }
   
   
   //-------------------------------------------------------
   // update registered CoordinateSystem ComboBox
   //-------------------------------------------------------
   int sel;
   #if DBGLVL_GUI_ITEM_CS > 1
   MessageInterface::ShowMessage
      ("   There are %d CoordSys combo boxes\n", (Integer) mCoordSysCBList.size());
   #endif
   
   for (std::vector<wxComboBox*>::iterator pos = mCoordSysCBList.begin();
        pos != mCoordSysCBList.end(); ++pos)
   {
      // How can I catch bad pointer?
      
      if ((*pos) == NULL)
         continue;

      #if DBGLVL_GUI_ITEM_CS > 1
      MessageInterface::ShowMessage
         ("   Coord Sys Combo Box name = %s\n",((*pos)->GetName()).c_str());
      #endif

      if ((*pos)->GetParent() != NULL)
      {
         // Filter coordinate systems with MJ2000Eq axis
         if ((*pos)->GetName() == "_MJ2000EqOnly_")
         {
            wxArrayString mj2000AxisList;
            sel = (*pos)->GetSelection();
            (*pos)->Clear();
            for (unsigned int i = 0; i < theCoordSysList.size(); i++)
            {
               std::string csName = theCoordSysList[i].WX_TO_STD_STRING;
               GmatBase *cs = theGuiInterpreter->GetConfiguredObject(csName.c_str());
               if (cs)
               {
                  GmatBase *axis = cs->GetOwnedObject(0);
                  if (axis && axis->IsOfType("MJ2000EqAxes"))
                     mj2000AxisList.Add(csName.c_str());
               }
            }
            (*pos)->Append(mj2000AxisList);
            (*pos)->SetSelection(sel);
         }
         else
         {
            sel = (*pos)->GetSelection();
            
            (*pos)->Clear();
            (*pos)->Append(theCoordSysList);
            (*pos)->SetSelection(sel);
         }
      }
   }
} //UpdateCoordSystemList()


//------------------------------------------------------------------------------
// void UpdateFuelTankList()
//------------------------------------------------------------------------------
/**
 * Updates configured FuelTank list.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateFuelTankList()
{
   StringArray items =
      theGuiInterpreter->GetListOfObjects(Gmat::FUEL_TANK);
   int numFuelTank = items.size();
   
   #if DBGLVL_GUI_ITEM_HW
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateFuelTankList() numFuelTank=%d\n", numFuelTank);
   #endif
   
   theNumFuelTank = 0;
   theFuelTankList.Clear();
   
   for (int i=0; i<numFuelTank; i++)
   {
      theFuelTankList.Add(items[i].c_str());
      
      #if DBGLVL_GUI_ITEM_HW > 1
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateFuelTankList() " + items[i] + "\n");
      #endif
   }
   
   theNumFuelTank = theFuelTankList.GetCount();
   
   //-------------------------------------------------------
   // update registered FuelTank ListBox
   //-------------------------------------------------------   
   // It's ok to have the same FuelTank in more than one spacecraft since
   // the Sandbox will clone it.
   std::vector<wxArrayString*>::iterator exPos = mFuelTankExcList.begin();
   
   for (std::vector<wxListBox*>::iterator pos = mFuelTankLBList.begin();
        pos != mFuelTankLBList.end(); ++pos)
   {
      wxArrayString *excList = *exPos++;
      (*pos)->Clear();
      
      for (int i=0; i<theNumFuelTank; i++)
      {
         if (excList->Index(theFuelTankList[i].c_str()) == wxNOT_FOUND)
            (*pos)->Append(theFuelTankList[i]);
      }
      
      (*pos)->SetSelection((*pos)->GetCount() - 1);
   }
   
   //-------------------------------------------------------
   // update registered FuelTank ComboBox
   //-------------------------------------------------------   
   int sel;
   wxString selStr;
   for (std::vector<wxComboBox*>::iterator pos = mFuelTankCBList.begin();
        pos != mFuelTankCBList.end(); ++pos)
   {
      sel = (*pos)->GetSelection();
      selStr = (*pos)->GetValue();
      wxString noSelStr = "No Fuel Tank Selected";
      
      #if DBGLVL_GUI_ITEM_HW
      MessageInterface::ShowMessage("   sel=%d, selStr='%s'\n", sel, selStr.c_str());
      #endif
      
      if (theNumFuelTank > 0)
      {
         (*pos)->Clear();
         (*pos)->Append(theFuelTankList);
         
         // Insert first item as "No Fuel Tank Selected"
         if (selStr == noSelStr)
         {
            (*pos)->Insert(noSelStr, 0);
            (*pos)->SetSelection(0);
         }
         else
         {
            (*pos)->SetSelection(sel);
         }
      }
   }
   
} // end UpdateFuelTankList()


//------------------------------------------------------------------------------
// void UpdateThrusterList()
//------------------------------------------------------------------------------
/**
 * Updates configured Thruster list.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateThrusterList()
{
   StringArray items =
      theGuiInterpreter->GetListOfObjects(Gmat::THRUSTER);
   int numThruster = items.size();
   
   #if DBGLVL_GUI_ITEM_HW
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateThrusterList() numThruster=%d\n", numThruster);
   #endif
   
   theNumThruster = 0;
   theThrusterList.Clear();
   
   for (int i=0; i<numThruster; i++)
   {
      theThrusterList.Add(items[i].c_str());
      
      #if DBGLVL_GUI_ITEM_HW > 1
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateThrusterList() " + items[i] + "\n");
      #endif
   }
   
   theNumThruster = theThrusterList.GetCount();
   
   //-------------------------------------------------------
   // update registered Thruster ListBox
   //-------------------------------------------------------
   // It's ok to have the same Thruster in more than one spacecraft since
   // the Sandbox will clone it.
   std::vector<wxArrayString*>::iterator exPos = mThrusterExcList.begin();
   
   for (std::vector<wxListBox*>::iterator pos = mThrusterLBList.begin();
        pos != mThrusterLBList.end(); ++pos)
   {
      wxArrayString *excList = *exPos++;
      (*pos)->Clear();
      
      for (int i=0; i<theNumThruster; i++)
      {
         if (excList->Index(theThrusterList[i].c_str()) == wxNOT_FOUND)
            (*pos)->Append(theThrusterList[i]);
      }
      
      (*pos)->SetSelection((*pos)->GetCount() - 1);
   }
   
   //-------------------------------------------------------
   // update registered Thruster ComboBox
   //-------------------------------------------------------
   int sel;
   wxString selStr;
   for (std::vector<wxComboBox*>::iterator pos = mThrusterCBList.begin();
        pos != mThrusterCBList.end(); ++pos)
   {
      sel = (*pos)->GetSelection();
      selStr = (*pos)->GetValue();
      wxString noSelStr = "No Thruster Selected";
      
      #if DBGLVL_GUI_ITEM_HW
      MessageInterface::ShowMessage("   sel=%d, selStr='%s'\n", sel, selStr.c_str());
      #endif
      
      if (theNumThruster > 0)
      {
         (*pos)->Clear();
         (*pos)->Append(theThrusterList);
         
         // Insert first item as "No Thruster Selected"
         if (selStr == noSelStr)
         {
            (*pos)->Insert(noSelStr, 0);
            (*pos)->SetSelection(0);
         }
         else
         {
            (*pos)->SetSelection(sel);
         }
      }
   }
   
} // end UpdateThrusterList()


//------------------------------------------------------------------------------
// void UpdateAntennaList()
//------------------------------------------------------------------------------
/**
 * Updates configured Antenna list. This list includes Sensors and Antennas.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateAntennaList()
{
   StringArray antennas =
      theGuiInterpreter->GetListOfObjects(Gmat::ANTENNA);
   int numAntenna = antennas.size();
   
   #if DBGLVL_GUI_ITEM_HW
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateAntennaList() numAntenna=%d\n",
       numAntenna);
   #endif
   
   theNumAntenna = 0;
   theAntennaList.Clear();
   
   for (int i=0; i<numAntenna; i++)
   {
      theAntennaList.Add(antennas[i].c_str());
      
      #if DBGLVL_GUI_ITEM_HW > 1
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateAntennaList() " + antennas[i] + "\n");
      #endif
   }
   
   theNumAntenna = theAntennaList.GetCount();
   
   //-------------------------------------------------------
   // update registered Antenna ComboBox
   //-------------------------------------------------------
   int sel;
   wxString selStr;
   for (std::vector<wxComboBox*>::iterator pos = mAntennaCBList.begin();
        pos != mAntennaCBList.end(); ++pos)
   {
      sel = (*pos)->GetSelection();
      selStr = (*pos)->GetValue();
      
      if (theNumAntenna > 0)
      {
         (*pos)->Clear();
         (*pos)->Append(theAntennaList);
         
         // Insert first item as "No Antenna Selected"
         if (theAntennaList[0] != selStr)
         {
            (*pos)->Insert("No Antenna Selected", 0);
            (*pos)->SetSelection(0);
         }
         else
         {
            (*pos)->SetSelection(sel);
         }
      }
   }
   
} // end UpdateAntennaList()

//------------------------------------------------------------------------------
// void UpdatePowerSystemList()
//------------------------------------------------------------------------------
/**
 * Updates configured Power System list.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdatePowerSystemList()
{
   StringArray powerSystems =
      theGuiInterpreter->GetListOfObjects(Gmat::POWER_SYSTEM);
   int numPowerSystems = powerSystems.size();

   #if DBGLVL_GUI_ITEM_POWER_SYSTEM
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdatePowerSystemList() numPowerSystems=%d\n",
            numPowerSystems);
   #endif

   theNumPowerSystem = 0;
   thePowerSystemList.Clear();

   for (int i=0; i<numPowerSystems; i++)
   {
      thePowerSystemList.Add(powerSystems[i].c_str());

      #if DBGLVL_GUI_ITEM_POWER_SYSTEM > 1
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdatePowerSystemList() " + powerSystems[i] + "\n");
      #endif
   }

   theNumPowerSystem = thePowerSystemList.GetCount();
   #if DBGLVL_GUI_ITEM_POWER_SYSTEM > 1
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdatePowerSystemList() number of registered ComboBoxes = %d\n",
            (Integer) mPowerSystemCBList.size());
   #endif

   //-------------------------------------------------------
   // update registered PowerSystem ComboBox
   //-------------------------------------------------------
   int sel;
   wxString selStr;
   for (std::vector<wxComboBox*>::iterator pos = mPowerSystemCBList.begin();
        pos != mPowerSystemCBList.end(); ++pos)
   {
      sel = (*pos)->GetSelection();
      selStr = (*pos)->GetValue();

      if (theNumPowerSystem > 0)
      {
         (*pos)->Clear();
         (*pos)->Append(thePowerSystemList);

         // Insert first item as "No Power System Selected"
         if (thePowerSystemList[0] != selStr)
         {
            (*pos)->Insert("No Power System Selected", 0);
            (*pos)->SetSelection(0);
         }
         else
         {
            (*pos)->SetSelection(sel);
         }
      }
   }

   #if DBGLVL_GUI_ITEM_HW > 1
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdatePowerSystemList() exiting\n");
   #endif
} // end UpdatePowerSystemList()

//------------------------------------------------------------------------------
// void UpdateSensorList()
//------------------------------------------------------------------------------
/**
 * Updates configured Sensor list. This list includes Sensors and Antennas.
 */
 //------------------------------------------------------------------------------
void GuiItemManager::UpdateImagerList()
{
   StringArray imagers =
      theGuiInterpreter->GetListOfObjects(Gmat::IMAGER);
   int numImager = imagers.size();

   #if DBGLVL_GUI_ITEM_HW
      MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSensorList() numImager=%d, numSensor=%d, numAntenna=%d\n",
         numImager, numSensor, numAntenna);
   #endif

   theNumImager = 0;
   theImagerList.Clear();

   for (int i = 0; i < numImager; i++)
   {
      theImagerList.Add(imagers[i].c_str());

      #if DBGLVL_GUI_ITEM_HW > 1
            MessageInterface::ShowMessage
            ("GuiItemManager::UpdateSensorList() " + imagers[i] + "\n");
      #endif
   }

   theNumImager = theImagerList.GetCount();

   //-------------------------------------------------------
   // update registered Imager CheckListBox
   //-------------------------------------------------------
   wxArrayString itemCheckedArray;
   for (std::vector<wxCheckListBox*>::iterator pos = mImagerCLBList.begin();
      pos != mImagerCLBList.end(); ++pos)
   {
      if ((*pos) == NULL)
         continue;

      itemCheckedArray.Clear();

      // save checked item
      int count = (*pos)->GetCount();
      for (int i = 0; i < count; i++)
         if ((*pos)->IsChecked(i))
            itemCheckedArray.Add((*pos)->GetString(i));

      (*pos)->Clear();
      (*pos)->Append(theImagerList);

      // restore checked item
      count = (*pos)->GetCount();
      for (int i = 0; i < count; i++)
         if (itemCheckedArray.Index((*pos)->GetString(i)) != wxNOT_FOUND)
            (*pos)->Check(i);
   }

} // end UpdateImagerList()

//------------------------------------------------------------------------------
// void UpdateSensorList()
//------------------------------------------------------------------------------
/**
 * Updates configured Sensor list. This list includes Sensors and Antennas.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSensorList()
{
   StringArray sensors =
      theGuiInterpreter->GetListOfObjects(Gmat::SENSOR);
   int numSensor = sensors.size();
   StringArray antennas =
      theGuiInterpreter->GetListOfObjects(Gmat::ANTENNA);
   int numAntenna = antennas.size();
   
   #if DBGLVL_GUI_ITEM_HW
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSensorList() numSensor=%d, numAntenna=%d\n",
       numSensor, numAntenna);
   #endif
   
   theNumSensor = 0;
   theSensorList.Clear();
   
   for (int i=0; i<numSensor; i++)
   {
      theSensorList.Add(sensors[i].c_str());
      
      #if DBGLVL_GUI_ITEM_HW > 1
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateSensorList() " + sensors[i] + "\n");
      #endif
   }
   
   for (int i=0; i<numAntenna; i++)
   {
      theSensorList.Add(antennas[i].c_str());
      
      #if DBGLVL_GUI_ITEM_HW > 1
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateSensorList() " + antennas[i] + "\n");
      #endif
   }
   
   theNumSensor = theSensorList.GetCount();
   
   //-------------------------------------------------------
   // update registered Sensor ListBox
   //-------------------------------------------------------
   // It's ok to have the same Sensor in more than one spacecraft since
   // the Sandbox will clone it.
   std::vector<wxArrayString*>::iterator exPos = mSensorExcList.begin();
   
   for (std::vector<wxListBox*>::iterator pos = mSensorLBList.begin();
        pos != mSensorLBList.end(); ++pos)
   {
      wxArrayString *excList = *exPos++;
      (*pos)->Clear();
      
      for (int i=0; i<theNumSensor; i++)
      {
         if (excList->Index(theSensorList[i].c_str()) == wxNOT_FOUND)
            (*pos)->Append(theSensorList[i]);
      }
      
      (*pos)->SetSelection((*pos)->GetCount() - 1);
   }
   
   //-------------------------------------------------------
   // update registered Sensor ComboBox
   //-------------------------------------------------------
   int sel;
   wxString selStr;
   for (std::vector<wxComboBox*>::iterator pos = mSensorCBList.begin();
        pos != mSensorCBList.end(); ++pos)
   {
      sel = (*pos)->GetSelection();
      selStr = (*pos)->GetValue();
      
      if (theNumSensor > 0)
      {
         (*pos)->Clear();
         (*pos)->Append(theSensorList);
         
         // Insert first item as "No Sensor Selected"
         if (theSensorList[0] != selStr)
         {
            (*pos)->Insert("No Sensor Selected", 0);
            (*pos)->SetSelection(0);
         }
         else
         {
            (*pos)->SetSelection(sel);
         }
      }
   }
   
} // end UpdateSensorList()


//------------------------------------------------------------------------------
// void UpdateFunctionList()
//------------------------------------------------------------------------------
/**
 * Updates configured Function list.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateFunctionList()
{
   StringArray items = theGuiInterpreter->GetListOfObjects(Gmat::FUNCTION);
   theNumFunction = items.size();
   
   #if DBGLVL_GUI_ITEM_FN
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateFunctionList() theNumFunction=%d\n", theNumFunction);
   #endif
   
   theFunctionList.Clear();
   
   for (int i=0; i<theNumFunction; i++)
   {
      theFunctionList.Add(items[i].c_str());
      
      #if DBGLVL_GUI_ITEM_FN > 1
      MessageInterface::ShowMessage
         ("   %s added to theFunctionList\n", theFunctionList[i].c_str());
      #endif
   }
   
   //-------------------------------------------------------
   // update registered Function ComboBox
   //-------------------------------------------------------
   int sel;
   for (std::vector<wxComboBox*>::iterator pos = mFunctionCBList.begin();
        pos != mFunctionCBList.end(); ++pos)
   {
      sel = (*pos)->GetSelection();
      
      #if DBGLVL_GUI_ITEM_FN > 1
      MessageInterface::ShowMessage
         ("   appending %s to FunctionComboBox\n", theFunctionList[0].c_str());
      #endif
      
      (*pos)->Clear();
      (*pos)->Append(theFunctionList);      
      (*pos)->SetSelection(sel);
   }
   
} // UpdateFunctionList()


//------------------------------------------------------------------------------
// void UpdateSubscriberList()
//------------------------------------------------------------------------------
/**
 * Updates configured Subscriber list.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSubscriberList()
{
   StringArray items =
      theGuiInterpreter->GetListOfObjects(Gmat::SUBSCRIBER);
   theNumSubscriber = items.size();
   theNumReportFile = 0;
   theNumXyPlot = 0;
   theSubscriberList.Clear();
   theReportFileList.Clear();
   theXyPlotList.Clear();
   
   #if DBGLVL_GUI_ITEM_SUBS
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSubscriberList() theNumSubscriber=%d\n", theNumSubscriber);
   #endif
   
   wxArrayString subsNames;
   wxArrayString rfNames;
   wxArrayString xyPlotNames;
   GmatBase *obj;
   
   // Update Subscriber list
   for (int i=0; i<theNumSubscriber; i++)
   {
      theSubscriberList.Add(items[i].c_str());
      
      #if DBGLVL_GUI_ITEM_SUBS > 1
      MessageInterface::ShowMessage
         ("   %s added to theSubscriberList\n", theSubscriberList[i].WX_TO_C_STRING);
      #endif
   }
   
   // Update ReportFile list
   for (int i=0; i<theNumSubscriber; i++)
   {
      // check for ReportFile
      obj = theGuiInterpreter->GetConfiguredObject(items[i]);
      if (obj->IsOfType("ReportFile"))
      {
         theReportFileList.Add(items[i].c_str());         
      }
   }
   
   theNumReportFile = theReportFileList.GetCount();
   #if DBGLVL_GUI_ITEM_SUBS
   MessageInterface::ShowMessage("   theNumReportFile=%d\n", theNumReportFile);
   #endif
   
   // Update ReportFile list
   for (int i=0; i<theNumSubscriber; i++)
   {
      // check for ReportFile
      obj = theGuiInterpreter->GetConfiguredObject(items[i]);
      if (obj->IsOfType("XYPlot"))
      {
         theXyPlotList.Add(items[i].c_str());         
      }
   }
   
   theNumXyPlot = theXyPlotList.GetCount();
   
   //-------------------------------------------------------
   // update registered Subscriber ComboBox
   //-------------------------------------------------------
   int sel;
   for (std::vector<wxComboBox*>::iterator pos = mSubscriberCBList.begin();
        pos != mSubscriberCBList.end(); ++pos)
   {      
       sel = (*pos)->GetSelection();

      (*pos)->Clear();
      (*pos)->Append(theSubscriberList);
      (*pos)->SetSelection(sel);
   }
   
   //-------------------------------------------------------
   // update registered ReportFile ComboBox
   //-------------------------------------------------------
   for (std::vector<wxComboBox*>::iterator pos = mReportFileCBList.begin();
        pos != mReportFileCBList.end(); ++pos)
   {      
       sel = (*pos)->GetSelection();
       
      (*pos)->Clear();
      (*pos)->Append(theReportFileList);
      (*pos)->SetSelection(sel);
   }
   
   //-------------------------------------------------------
   // update registered Subscriber CheckListBox
   //-------------------------------------------------------
   for (std::vector<wxCheckListBox*>::iterator pos = mSubscriberCLBList.begin();
        pos != mSubscriberCLBList.end(); ++pos)
   {
      int guiCount = (*pos)->GetCount();
      bool found = false;
      wxString item;
      
      // if deleted item remove from the list
      for (int i=0; i<guiCount; i++)
      {
         found = false;
         for (int j=0; j<theNumSubscriber; j++)
         {
            item = (*pos)->GetString(i);
            if (item == theSubscriberList[j])
            {
               found = true;
               break;
            }
         }
         
         if (!found)
         {
            (*pos)->Delete(i);
            guiCount--;
         }
      }
      
      // if new item add to the list
      for (int i=0; i<theNumSubscriber; i++)
         if ((*pos)->FindString(theSubscriberList[i]) == wxNOT_FOUND)
            (*pos)->Append(theSubscriberList[i]);
      
   }
   
   //-------------------------------------------------------
   // update registered XYPlot CheckListBox
   //-------------------------------------------------------
   for (std::vector<wxCheckListBox*>::iterator pos = mXyPlotCLBList.begin();
        pos != mXyPlotCLBList.end(); ++pos)
   {
      int guiCount = (*pos)->GetCount();
      bool found = false;
      wxString item;
      
      // if deleted item remove from the list
      for (int i=0; i<guiCount; i++)
      {
         found = false;
         for (int j=0; j<theNumXyPlot; j++)
         {
            item = (*pos)->GetString(i);
            if (item == theXyPlotList[j])
            {
               found = true;
               break;
            }
         }
         
         if (!found)
         {
            (*pos)->Delete(i);
            guiCount--;
         }
      }
      
      // if new item add to the list
      for (int i=0; i<theNumXyPlot; i++)
         if ((*pos)->FindString(theXyPlotList[i]) == wxNOT_FOUND)
            (*pos)->Append(theXyPlotList[i]);

      // If new item is the only item, put check mark
      if ((*pos)->GetCount() == 1)
      {
         GmatPanel *parent = (GmatPanel*)((*pos)->GetParent());
         (*pos)->Check(0, true);
         parent->TakeAction("EnableUpdate");
      }
   }
   
} // UpdateSubscriberList()


//------------------------------------------------------------------------------
// void UpdateSolverList()
//------------------------------------------------------------------------------
/**
 * Updates configured Suolver list.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSolverList()
{
   StringArray items = theGuiInterpreter->GetListOfObjects(Gmat::SOLVER);
   
   theNumSolver = items.size();
   theNumBoundarySolver = 0;
   theNumOptimizer = 0;
   
   #if DBGLVL_GUI_ITEM_SOLVER
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSolverList() entered, theNumSolver=%d\n", theNumSolver);
   #endif
   
   GmatBase *obj;
   theSolverList.Clear();
   
   //-------------------------------------------------------
   // Update Solver list
   //-------------------------------------------------------
   for (int i=0; i<theNumSolver; i++)
   {
      #if DBGLVL_GUI_ITEM_SOLVER > 1
      MessageInterface::ShowMessage
         ("   Adding '%s' to theSolverList\n", items[i].c_str());
      #endif
      
      theSolverList.Add(items[i].c_str());
   }
   
   //-------------------------------------------------------
   // Update Boundary Value Solver and Optimizer list
   //-------------------------------------------------------
   theBoundarySolverList.Clear();
   theOptimizerList.Clear();
   
   for (int i=0; i<theNumSolver; i++)
   {
      obj = theGuiInterpreter->GetConfiguredObject(items[i]);
      // check for Boundary Solver
      if (obj->IsOfType("DifferentialCorrector"))
      {
         #if DBGLVL_GUI_ITEM_SOLVER
         MessageInterface::ShowMessage
            ("   Adding '%s' to theBoundarySolverList\n", items[i].c_str());
         #endif
         
         theBoundarySolverList.Add(items[i].c_str());
      }
      else if (obj->IsOfType("Optimizer"))
      {
         #if DBGLVL_GUI_ITEM_SOLVER
         MessageInterface::ShowMessage
            ("   Adding '%s' to theOptimizerList\n", items[i].c_str());
         #endif
         
         theOptimizerList.Add(items[i].c_str());
      }
   }
   
   theNumBoundarySolver = theBoundarySolverList.GetCount();
   theNumOptimizer = theOptimizerList.GetCount();
   #if DBGLVL_GUI_ITEM_SOLVER
   MessageInterface::ShowMessage
      ("   theNumBoundarySolver=%d\n   theNumOptimizer=%d\n",
       theNumBoundarySolver, theNumOptimizer);
   #endif
   
   //-------------------------------------------------------
   // update registered Solver ComboBox
   //-------------------------------------------------------
   int sel;
   for (std::vector<wxComboBox*>::iterator pos = mSolverCBList.begin();
        pos != mSolverCBList.end(); ++pos)
   {
       sel = (*pos)->GetSelection();
       
      (*pos)->Clear();
      (*pos)->Append(theSolverList);
      (*pos)->SetSelection(sel);
   }
   
   //-------------------------------------------------------
   // update registered Boundary Solver ComboBox
   //-------------------------------------------------------
   for (std::vector<wxComboBox*>::iterator pos = mBoundarySolverCBList.begin();
        pos != mBoundarySolverCBList.end(); ++pos)
   {
       sel = (*pos)->GetSelection();
       
      (*pos)->Clear();
      (*pos)->Append(theBoundarySolverList);      
      (*pos)->SetSelection(sel);
   }
   
   //-------------------------------------------------------
   // update registered Optimizer ComboBox
   //-------------------------------------------------------
   for (std::vector<wxComboBox*>::iterator pos = mOptimizerCBList.begin();
        pos != mOptimizerCBList.end(); ++pos)
   {
       sel = (*pos)->GetSelection();
       
      (*pos)->Clear();
      (*pos)->Append(theOptimizerList);      
      (*pos)->SetSelection(sel);
   }
   
   #if DBGLVL_GUI_ITEM_SOLVER
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSolverList() leaving\n");
   #endif
   
} //UpdateSolverList()


//------------------------------------------------------------------------------
//  void UpdatePropagatorList()
//------------------------------------------------------------------------------
/**
 * updates Propagator list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdatePropagatorList()
{
   StringArray listProp;
   int numProp;

   thePropagatorList.Clear();
   
   listProp = theGuiInterpreter->GetListOfObjects(Gmat::PROP_SETUP);
   numProp = listProp.size();
   #if DBGLVL_GUI_ITEM_PROP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdatePropagatorList() numProp=%d\n", numProp);
   #endif
   
   // check to see if any propagators exist
   for (int i=0; i<numProp; i++)
   {
      thePropagatorList.Add(listProp[i].c_str());

      #if DBGLVL_GUI_ITEM_PROP > 1
      MessageInterface::ShowMessage
         ("   %s added to thePropagatorList\n", thePropagatorList[i].c_str());
      #endif
   }

   #ifdef __USE_SPICE__
   listProp = theGuiInterpreter->GetListOfObjects("SPK");
   numProp = listProp.size();
   #if DBGLVL_GUI_ITEM_PROP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdatePropagatorList() SPK numProp=%d\n", numProp);
   #endif
   
    // check to see if any SPK propagators exist
   for (int i=0; i<numProp; i++)
   {
      thePropagatorList.Add(listProp[i].c_str());
      
      #if DBGLVL_GUI_ITEM_PROP > 1
      MessageInterface::ShowMessage
         ("   %s added to thePropagatorList\n", thePropagatorList[i].c_str());
      #endif
   }
   #endif

   theNumPropagator = thePropagatorList.GetCount();
   
}


//------------------------------------------------------------------------------
//  void UpdateForceModelList()
//------------------------------------------------------------------------------
/**
 * updates ForceModel list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateForceModelList()
{
   StringArray listFm = theGuiInterpreter->GetListOfObjects(Gmat::ODE_MODEL);
   int numForceModel = listFm.size();
   
   #if DBGLVL_GUI_ITEM_FM
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateForceModelList() numForceModel=%d\n", numForceModel);
   #endif
   
   theForceModelList.Clear();
   
   // check to see if any spacecrafts exist
   for (int i=0; i<numForceModel; i++)
   {
      theForceModelList.Add(listFm[i].c_str());
      
      #if DBGLVL_GUI_ITEM_FM > 1
      MessageInterface::ShowMessage
         ("   %s added to theForceModelList\n", theForceModelList[i].c_str());
      #endif
   }
   
   theNumForceModel = theForceModelList.GetCount();
   
}

//------------------------------------------------------------------------------
// void UpdateLocatorList()
//------------------------------------------------------------------------------
/**
 * Updates event locator list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateLocatorList()
{
   StringArray listEl = theGuiInterpreter->GetListOfObjects(Gmat::EVENT_LOCATOR);
   int numLocator = listEl.size();

   #if DBGLVL_GUI_ITEM_EL
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateLocatorList() numLocator=%d\n", numLocator);
   #endif

   theLocatorList.Clear();

   // check to see if any locators exist
   for (int i=0; i<numLocator; i++)
   {
      theLocatorList.Add(listEl[i].c_str());

      #if DBGLVL_GUI_ITEM_EL > 1
      MessageInterface::ShowMessage
         ("   %s added to theLocatorList\n", theLocatorList[i].WX_TO_STD_STRING.c_str());
      #endif
   }

   theNumLocator = theLocatorList.GetCount();
   //-------------------------------------------------------
   // update registered EventLocator ComboBox
   //-------------------------------------------------------
   int sel;
   wxString selStr;
   for (std::vector<wxComboBox*>::iterator pos = mLocatorCBList.begin();
        pos != mLocatorCBList.end(); ++pos)
   {
      sel = (*pos)->GetSelection();
      selStr = (*pos)->GetValue();

      if (theNumLocator > 0)
      {
         (*pos)->Clear();
         (*pos)->Append(theLocatorList);

         // Insert first item as "No Event Locator Selected"
         if (theLocatorList[0] != selStr)
         {
            (*pos)->Insert("No Event Locator Selected", 0);
            (*pos)->SetSelection(0);
         }
         else
         {
            (*pos)->SetSelection(sel);
         }
      }
   }

}

//------------------------------------------------------------------------------
// void SortUserVariables()
//------------------------------------------------------------------------------
/**
 * Sorts the user variable list alphabetically.
 *
 */
//------------------------------------------------------------------------------
void GuiItemManager::SortUserVariables()
{
   theUserVariableList.Sort(CaseInsensitiveCompare);
}

int GuiItemManager::CaseInsensitiveCompare(const wxString& str1, const wxString& str2)
{
   int minLen = std::min(str1.size(), str2.size());
   int retval = -999;

   for (int i = 0; i < minLen; ++i)
      if (tolower(str2[i]) - tolower(str1[i]) != 0)
         retval = tolower(str2[i]) - tolower(str1[i]);

   if (retval == -999)
      retval = str2.size() - str1.size();

   return retval;
}

//------------------------------------------------------------------------------
// void RefreshAllObjectArray()
//------------------------------------------------------------------------------
/**
 * It refresh array by clearing and re-adding objects to array.
 *
 */
//------------------------------------------------------------------------------
void GuiItemManager::RefreshAllObjectArray()
{
   #if DBGLVL_GUI_ITEM_ALL_OBJECT
   MessageInterface::ShowMessage
      ("GuiItemManager::RefreshAllObjectArray() entered\n");
   #endif
   
   theNumAllObject = 0;
   theAllObjectList.Clear();
   theAutoGlobalObjectList.Clear();
	
   // Add CoordinateSystem objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d CoordinateSystem\n", theNumCoordSys);
   #endif
   
   for (int i=0; i<theNumCoordSys; i++)
      theAllObjectList.Add(theCoordSysList[i] + " <CoordinateSystem>");
   
   // Add Spacecraft objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d Spacecraft\n", theNumSpacecraft);
   #endif
   for (int i=0; i<theNumSpacecraft; i++)
      theAllObjectList.Add(theSpacecraftList[i] + " <Spacecraft>");
   
   // Add Formation objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d Formation\n", theNumFormation);
   #endif
   for (int i=0; i<theNumFormation; i++)
      theAllObjectList.Add(theFormationList[i] + " <Formation>");
   
   // Add CalculatedPoint objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d CalculatedPoint\n", theNumCalPoint);
   #endif
   for (int i=0; i<theNumCalPoint; i++)
   {
      GmatBase *obj = theGuiInterpreter->GetConfiguredObject(theCalPointList[i].c_str());
      if (obj->GetTypeName() == "LibrationPoint")
         theAllObjectList.Add(theCalPointList[i] + " <LibrationPoint>");
      else if (obj->GetTypeName() == "Barycenter")
         theAllObjectList.Add(theCalPointList[i] + " <Barycenter>");
   }
   
   // Add Function objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d Function\n", theNumFunction);
   #endif
   for (int i=0; i<theNumFunction; i++)
      theAllObjectList.Add(theFunctionList[i] + " <Function>");
   
   // Add FuelTank objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d FuelTank\n", theNumFuelTank);
   #endif
   for (int i=0; i<theNumFuelTank; i++)
      theAllObjectList.Add(theFuelTankList[i] + " <FuelTank>");
   
   // Add Thruster objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d Thruster\n", theNumThruster);
   #endif
   for (int i=0; i<theNumThruster; i++)
      theAllObjectList.Add(theThrusterList[i] + " <Thruster>");
   
   // Add ImpulsiveBurn objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d ImpulsiveBurn\n", theNumImpBurn);
   #endif
   for (int i=0; i<theNumImpBurn; i++)
      theAllObjectList.Add(theImpBurnList[i] + " <ImpulsiveBurn>");
   
   // Add FiniteBurn objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d FiniteBurn\n", theNumFiniteBurn);
   #endif
   for (int i=0; i<theNumFiniteBurn; i++)
      theAllObjectList.Add(theFiniteBurnList[i] + " <FiniteBurn>");
   
   // Add Propagator objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d Propagator\n", theNumPropagator);
   #endif
   for (int i=0; i<theNumPropagator; i++)
      theAllObjectList.Add(thePropagatorList[i] + " <Propagator>");
   
   // Add ForceModel objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d ForceModel\n", theNumForceModel);
   #endif
   for (int i=0; i<theNumForceModel; i++)
      theAllObjectList.Add(theForceModelList[i] + " <ForceModel>");
   
   // Add Solver objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d Solver\n", theNumSolver);
   #endif
   for (int i=0; i<theNumSolver; i++)
      theAllObjectList.Add(theSolverList[i] + " <Solver>");
   
   // Add Variable objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d Variable\n", theNumUserVariable);
   #endif
   for (int i=0; i<theNumUserVariable; i++)
      theAllObjectList.Add(theUserVariableList[i] + " <Variable>");
   
   // Add Array objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d Array\n", theNumUserArray);
   #endif
   for (int i=0; i<theNumUserArray; i++)
      theAllObjectList.Add(theUserArrayList[i] + " <Array>");
   
   // Add String objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d String\n", theNumUserString);
   #endif
   for (int i=0; i<theNumUserString; i++)
      theAllObjectList.Add(theUserStringList[i] + " <String>");
   
   // Add Subscriber objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d Subscriber\n", theNumSubscriber);
   #endif
   for (int i=0; i<theNumSubscriber; i++)
   {
      GmatBase *obj = theGuiInterpreter->GetConfiguredObject(theSubscriberList[i].c_str());
      
      if (obj == NULL)
         continue;
      
      wxString typeName = obj->GetTypeName().c_str();
      theAllObjectList.Add(theSubscriberList[i] + " <" + typeName + ">");
   }
   
   // Add GroundStation objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d GroundStations\n", theNumGroundStation);
   #endif
   for (int i=0; i<theNumGroundStation; i++)
      theAllObjectList.Add(theGroundStationList[i] + " <GroundStation>");
   
   // Add Imager objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d Sensors\n", theNumSensor);
   #endif
   for (int i=0; i<theNumSensor; i++)
      theAllObjectList.Add(theImagerList[i] + " <Imager>");
   
   // Add EventLocator objects to the list
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage("   Adding %d EventLocators\n", theNumLocator);
   #endif
   for (int i=0; i<theNumLocator; i++)
      theAllObjectList.Add(theLocatorList[i] + " <EventLocator>");

   theNumAllObject = theAllObjectList.GetCount();
   
   // Add SolarSystem objects to the list
   if (theNumAllObject > 0)
   {
      #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
      MessageInterface::ShowMessage("   Adding 1 SolarSystem\n");
      MessageInterface::ShowMessage
         ("   SolarSystemInUse=<%p>\n", theGuiInterpreter->GetSolarSystemInUse());
      #endif
      SolarSystem *ss = theGuiInterpreter->GetSolarSystemInUse();
      if (ss)
      {
         wxString ssName = ss->GetName().c_str();
         ssName = ssName + " <SolarSystem>";
         theAllObjectList.Add(ssName);
         theNumAllObject = theAllObjectList.GetCount();
      }
   }
   
   // Add automatic global objects to list
	wxString objStr, objName;
	for (int i=0; i<theNumAllObject; i++)
	{
		objStr = theAllObjectList[i];
		objName = objStr.BeforeFirst(' ');
            
      GmatBase *obj = theGuiInterpreter->GetConfiguredObject(objName.c_str());
      
      #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
      MessageInterface::ShowMessage
         ("   objStr = '%s', objName = '%s', obj = <%p>'%s'\n", objStr.c_str(),
          objName.c_str(), obj, obj ? obj->GetName().c_str() : "NULL");
      #endif
      
		if (obj && obj->IsAutomaticGlobal())
      {
         #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
         MessageInterface::ShowMessage
            ("   objName: '%s' is global object so adding to theAutoGlobalObjectList\n",
             objName.c_str());
         #endif
			theAutoGlobalObjectList.Add(objName);
      }
	}
	
   //-------------------------------------------------------
   // update registered All Object CheckListBox
   //-------------------------------------------------------
   #if DBGLVL_GUI_ITEM_ALL_OBJECT > 1
   MessageInterface::ShowMessage
      ("   Updating registerd All Object CheckListBox, count=%d\n", mAllObjectCLBList.size());
   #endif
   for (std::vector<wxCheckListBox*>::iterator pos = mAllObjectCLBList.begin();
        pos != mAllObjectCLBList.end(); ++pos)
   {
      int guiCount = (*pos)->GetCount();
      bool found = false;
      wxString item;
      
      // if deleted item remove from the list
      for (int i=0; i<guiCount; i++)
      {
         found = false;
         for (int j=0; j<theNumAllObject; j++)
         {
            item = (*pos)->GetString(i);
            if (item == theAllObjectList[j])
            {
               found = true;
               break;
            }
         }
         
         if (!found)
         {
            (*pos)->Delete(i);
            guiCount--;
         }
      }

		bool includeAutoGlobal = true;
		if ((*pos)->GetName() == "AllObjectsExcludingAutoGlobal")
			includeAutoGlobal = false;
		
      // if new item add to the list
      for (int i=0; i<theNumAllObject; i++)
		{
         if ((*pos)->FindString(theAllObjectList[i]) == wxNOT_FOUND)
			{
				if (includeAutoGlobal)
				{
					(*pos)->Append(theAllObjectList[i]);
				}
				else
				{
					objStr = theAllObjectList[i];
					objName = objStr.BeforeFirst(' ');
					if (!theGuiInterpreter->GetConfiguredObject(objName.c_str())->IsAutomaticGlobal())
						(*pos)->Append(theAllObjectList[i]);
				}
			}
		}
   }
   
   #if DBGLVL_GUI_ITEM_ALL_OBJECT
   MessageInterface::ShowMessage
      ("GuiItemManager::RefreshAllObjectArray() exiting, theNumAllObject = %d\n",
       theNumAllObject);
   #endif
}


//------------------------------------------------------------------------------
// void UpdateInterfaceList()
//------------------------------------------------------------------------------
/**
 * Updates configured interface list.
 *
 * @note: Not implemented yet because it is not needed until the GUI got the 
 *        FileInterface is put in place
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateInterfaceList()
{
}


//------------------------------------------------------------------------------
// GuiItemManager()
//------------------------------------------------------------------------------
GuiItemManager::GuiItemManager()
{
   #if DBGLVL_GUI_ITEM
   MessageInterface::ShowMessage("GuiItemManager::GuiItemManager() entered\n");
   #endif
   
   mPngHandlerLoaded = false;
   mGuiStatus = 1;
   mActiveScriptStatus = 1;
   
   theDataPrecision = GmatGlobal::Instance()->GetDataPrecision();   
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   
   theNumScProperty = 0;
   theNumSpacePtProperty = 0;
   theNumImpBurnProperty = 0;
   theNumFiniteBurnProperty = 0;
   theNumAllObject = 0;
   theNumSpacePoint = 0;
   theNumCelesPoint = 0;
   theNumCelesBody = 0;
   theNumCalPoint = 0;
   theNumSpaceObject = 0;
   theNumSpacecraft = 0;
   theNumFormation = 0;
   theNumGroundStation = 0;
   theNumCoordSys = 0;
   theNumPropagator = 0;
   theNumForceModel = 0;
   theNumImpBurn = 0;
   theNumFiniteBurn = 0;
   theNumSolver = 0;
   theNumBoundarySolver = 0;
   theNumOptimizer = 0;
   theNumFuelTank = 0;
   theNumThruster = 0;
   theNumSensor = 0;
   theNumAntenna = 0;
   theNumPowerSystem = 0;
   theNumFunction = 0;
   theNumSubscriber = 0;
   theNumReportFile = 0;
   theNumXyPlot = 0;
   theNumPlottableParam = 0;
   theNumSystemParam = 0;
   theNumUserVariable = 0;
   theNumUserString = 0;
   theNumUserArray = 0;
   theNumUserParam = 0;
   theNumLocator = 0;
   
   // update property list
   UpdatePropertyList();
}


//------------------------------------------------------------------------------
// ~GuiItemManager()
//------------------------------------------------------------------------------
GuiItemManager::~GuiItemManager()
{
}
