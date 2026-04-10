//$Id$
//------------------------------------------------------------------------------
//                                DynamicDataDisplay
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Created: 2018/01/24, Joshua Raymond, Thinking Systems, Inc.
//
/**
* Defines DynamicDataDisplay methods
*/
//------------------------------------------------------------------------------
#include "DynamicDataDisplay.hpp"

const std::string
DynamicDataDisplay::PARAMETER_TEXT[DynamicDataDisplayParamCount - SubscriberParamCount] =
{
   "AddParameters",
   "TextColor",
   "RowTextColors",
   "BackgroundColor",
   "WarnBounds",
   "CritBounds",
   "WarnColor",
   "CritColor"
};

const Gmat::ParameterType
DynamicDataDisplay::PARAMETER_TYPE[DynamicDataDisplayParamCount - SubscriberParamCount] = 
{
   Gmat::OBJECTARRAY_TYPE, //"AddParameters"
   Gmat::STRINGARRAY_TYPE, //"SetTextColor"
   Gmat::STRINGARRAY_TYPE, //"SetRowTextColors"
   Gmat::STRINGARRAY_TYPE, //"SetBackgroundText"
   Gmat::STRINGARRAY_TYPE, //"WarnBounds"
   Gmat::STRINGARRAY_TYPE, //"CritBounds"
   Gmat::COLOR_TYPE,       //"WarnColor"
   Gmat::COLOR_TYPE        //"CritColor"
};

//------------------------------------------------------------------------------
//  DynamicDataDisplay(const std::string &name)
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
DynamicDataDisplay::DynamicDataDisplay(const std::string &name) :
   Subscriber     ("DynamicDataDisplay", name)
{
   // GmatBase data
   parameterCount = DynamicDataDisplayParamCount;
   objectTypes.push_back(Gmat::DYNAMIC_DATA_DISPLAY);
   objectTypeNames.push_back("DynamicDataDisplay");

   numParams = 0;
   currentRowIndex = 0;
   maxRowCount = 0;
   maxColCount = 0;
   currentColIndex = 0;
   inf = 9.999e300;
   warnTextColor = RgbColor::ToIntColor("[218 165 32]");
   critTextColor = RgbColor::ToIntColor("[255 0 0]");
   paramToUpdate = "";
   errorAlreadyShown = false;
}

//------------------------------------------------------------------------------
//  DynamicDataDisplay(const DynamicDataDisplay &orig)
//------------------------------------------------------------------------------
/**
* Copy constructor
*/
//------------------------------------------------------------------------------
DynamicDataDisplay::DynamicDataDisplay(const DynamicDataDisplay &orig) :
   Subscriber     (orig)
{
   displayData = orig.displayData;
   oldName = instanceName;
   numParams = orig.numParams;
   currentRowIndex = orig.currentRowIndex;
   maxRowCount = orig.maxRowCount;
   maxColCount = orig.maxColCount;
   currentColIndex = orig.currentColIndex;
   inf = orig.inf;
   warnTextColor = orig.warnTextColor;
   critTextColor = orig.critTextColor;
   paramToUpdate = orig.paramToUpdate;
   errorAlreadyShown = orig.errorAlreadyShown;
}

//------------------------------------------------------------------------------
// DyanmicDataDisplay& operator=(const DynamicDataDisplay& orig)
//------------------------------------------------------------------------------
/**
* Assignment operator
*/
//------------------------------------------------------------------------------
DynamicDataDisplay& DynamicDataDisplay::operator=(const DynamicDataDisplay& orig)
{
   if (this == &orig)
      return *this;

   Subscriber::operator=(orig);

   displayData = orig.displayData;
   oldName = orig.oldName;
   numParams = orig.numParams;
   currentRowIndex = orig.currentRowIndex;
   maxRowCount = orig.maxRowCount;
   maxColCount = orig.maxColCount;
   currentColIndex = orig.currentColIndex;
   inf = orig.inf;
   warnTextColor = orig.warnTextColor;
   critTextColor = orig.critTextColor;
   paramToUpdate = orig.paramToUpdate;
   errorAlreadyShown = orig.errorAlreadyShown;

   return *this;
}

//------------------------------------------------------------------------------
// ~DyanmicDataDisplay()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
DynamicDataDisplay::~DynamicDataDisplay()
{

}

//------------------------------------------------------------------------------
// bool UpdateData()
//------------------------------------------------------------------------------
/**
* Method called by the UpdateDynamicData command to get the current values of
* the parameters being watched and sending them to be displayed on the table
*
* @return Returns true of the data was successfully sent to the display
*/
//------------------------------------------------------------------------------
bool DynamicDataDisplay::UpdateData(StringArray valuesToUpdate)
{
   bool checkParamNames = false;
   if (valuesToUpdate.size() > 0)
      checkParamNames = true;

   std::string desc;
   Integer wrapperIdx = 0;
   for (Integer i = 0; i < displayData.size(); ++i)
   {
      for (Integer j = 0; j < displayData[i].size(); ++j)
      {
         if (displayData[i][j].paramName == "")
            continue;

         bool skipCurrentParam = false;

         // If only updating certain data, check if the current data is one of,
         // otherwise skip this update step
         if (checkParamNames)
         {
            skipCurrentParam = true;
            for (Integer inputIdx = 0; inputIdx < valuesToUpdate.size();
               ++inputIdx)
            {
               if (displayData[i][j].paramName == valuesToUpdate[inputIdx])
               {
                  skipCurrentParam = false;
                  break;
               }
            }
         }

         if (skipCurrentParam)
         {
            ++wrapperIdx;
            continue;
         }
         desc = yParamWrappers[wrapperIdx]->GetDescription();
         Gmat::ParameterType dataType =
            yParamWrappers[wrapperIdx]->GetDataType();
         switch (dataType)
         {
         case Gmat::REAL_TYPE:
         {
            displayData[i][j].paramValue =
               GmatStringUtil::ToString(yParamWrappers[wrapperIdx]->EvaluateReal());

            if (!displayData[i][j].isTextColorUserSet)
            {
               if (yParamWrappers[wrapperIdx]->EvaluateReal() <
                  displayData[i][j].critLowerBound ||
                  yParamWrappers[wrapperIdx]->EvaluateReal() >
                  displayData[i][j].critUpperBound)
               {
                  displayData[i][j].paramTextColor = critTextColor;
               }
               else if (yParamWrappers[wrapperIdx]->EvaluateReal() <
                  displayData[i][j].warnLowerBound ||
                  yParamWrappers[wrapperIdx]->EvaluateReal() >
                  displayData[i][j].warnUpperBound)
               {
                  displayData[i][j].paramTextColor = warnTextColor;
               }
               else
                  displayData[i][j].paramTextColor =
                     RgbColor::ToIntColor("[0 0 0]");
            }
            break;
         }
         case Gmat::STRING_TYPE:
         {
            displayData[i][j].paramValue =
               yParamWrappers[wrapperIdx]->EvaluateString();
            break;
         }
         default:
            throw SubscriberException("DynamicDataDisplay cannot display \"" +
               desc + "\", only real or string parameter types can be used.");
         }
         ++wrapperIdx;

         
      }
   }

   PlotInterface::UpdateDynamicDataDisplay(instanceName, displayData);

   return true;
}

//------------------------------------------------------------------------------
// std::vector<std::vector<DDD>> GetDynamicDataStruct()
//------------------------------------------------------------------------------
/**
* Returns the struct containing all of the neccessary data for the values being
* used in this DynamicDataDisplay
*/
//------------------------------------------------------------------------------
std::vector<std::vector<DDD>> DynamicDataDisplay::GetDynamicDataStruct()
{
   return displayData;
}

//------------------------------------------------------------------------------
// void SetParamSettings(std::vector<std::vector<DDD>> newSettings)
//------------------------------------------------------------------------------
/**
* Sets the struct containing the parameter names, settings, and values
*/
//------------------------------------------------------------------------------
void DynamicDataDisplay::SetParamSettings(std::vector<std::vector<DDD>> newSettings)
{
   displayData = newSettings;

   if (displayData.size() != maxRowCount)
      maxRowCount = displayData.size();

   maxColCount = 0;
   for (Integer i = 0; i < maxRowCount; ++i)
   {
      if (displayData[i].size() > maxColCount)
         maxColCount = displayData[i].size();
   }

   ClearWrappers();

   for (Integer i = 0; i < displayData.size(); ++i)
   {
      for (Integer j = 0; j < displayData[i].size(); ++j)
      {
         /**
         if (displayData[i][j].warnLowerBound > displayData[i][j].warnUpperBound)
         {
            throw SubscriberException("The new lower warning bound set for \"" +
               paramToUpdate + "\" on object \"" + instanceName + "\" is greater "
               "than the new upper warning bound");
         }
         */
         if (displayData[i][j].paramName != "")
         {
            yParamWrappers.push_back(NULL);
         }
      }
   }
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
* Initializes the display be removing any previous grids that may exist,
* creating the new display, setting the size, and setting cell text colors if
* they were specified.
*
* @return Returns true if initialization was successful
*/
//------------------------------------------------------------------------------
bool DynamicDataDisplay::Initialize()
{
   Subscriber::Initialize();

   PlotInterface::DeleteDynamicData(instanceName, oldName);

   std::string displayTitle = "Dynamic Data Display " + instanceName;

   PlotInterface::CreateDynamicDataDisplay(instanceName, oldName, displayTitle,
      mPlotUpperLeft[0], mPlotUpperLeft[1], mPlotSize[0], mPlotSize[1]);

   PlotInterface::SetDynamicDataTableSize(instanceName, maxRowCount,
      maxColCount);
   StringArray unusedArray;
   UpdateData(unusedArray);
   //PlotInterface::SetDynamicDataTextColor(instanceName, displayData);

   return true;
}

//---------------------------------
// methods inherited from GmatBase
//---------------------------------

//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
* This method returns a clone of the DynamicDataDisplay.
*
* @return clone of the DynamicDataDisplay.
*/
//------------------------------------------------------------------------------
GmatBase* DynamicDataDisplay::Clone() const
{
   return (new DynamicDataDisplay(*this));
}

//------------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//------------------------------------------------------------------------------
/**
* Sets this object to match another one.
*
* @param orig The original that is being copied.
*/
//------------------------------------------------------------------------------
void DynamicDataDisplay::Copy(const GmatBase* orig)
{
   operator=(*((DynamicDataDisplay *)(orig)));
}

//------------------------------------------------------------------------------
// bool SetName(const std::string &who, const std;:string &oldName = "")
//------------------------------------------------------------------------------
/**
* Set the name for this instance.
*
* @see GmatBase
*/
//------------------------------------------------------------------------------
bool DynamicDataDisplay::SetName(const std::string &who,
                                 const std::string &inputOldName)
{
   if (oldName == "")
      oldName = instanceName;
   else
      oldName = oldName;

   return GmatBase::SetName(who);
}

//---------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type,
//                      const std::string &oldName,
//                      const std::string &newName)
//---------------------------------------------------------------------------
/*
* Renames referenced objects
*
* @param <type> type of the reference object.
* @param <oldName> old name of the reference object.
* @param <newName> new name of the reference object.
*
* @return always true to indicate RenameRefObject() was implemented.
*/
//---------------------------------------------------------------------------
bool DynamicDataDisplay::RenameRefObject(const UnsignedInt type,
                                         const std::string &oldName,
                                         const std::string &newName)
{
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

   for (Integer i = 0; i < displayData.size(); ++i)
   {
      for (Integer j = 0; j < displayData[i].size(); ++j)
      {
         if (displayData[i][j].paramName.find(oldName) != oldName.npos)
         {
            displayData[i][j].paramName =
               GmatStringUtil::ReplaceName(displayData[i][j].paramName, oldName, newName);
         }
      }
   }

   // Call Subscriber to rename wrapper object names
   return Subscriber::RenameRefObject(type, oldName, newName);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer DynamicDataDisplay::GetParameterID(const std::string &str) const
{
   for (Integer i = SubscriberParamCount; i < DynamicDataDisplayParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - SubscriberParamCount])
         return i;
   }

   return Subscriber::GetParameterID(str);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string DynamicDataDisplay::GetParameterText(const Integer id) const
{
   if (id >= SubscriberParamCount && id < DynamicDataDisplayParamCount)
      return PARAMETER_TEXT[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType DynamicDataDisplay::GetParameterType(const Integer id) const
{
   if (id >= SubscriberParamCount && id < DynamicDataDisplayParamCount)
      return PARAMETER_TYPE[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string DynamicDataDisplay::GetParameterTypeString(const Integer id) const
{
   if (id >= SubscriberParamCount && id < DynamicDataDisplayParamCount)
      return Subscriber::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return Subscriber::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
bool DynamicDataDisplay::IsParameterReadOnly(const Integer id) const
{
   switch (id)
   {
   case SOLVER_ITERATIONS:
      return true;
   case PARAM_TEXT_COLOR:
      return true;
   default:
      return Subscriber::IsParameterReadOnly(id);
   }
}

//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
bool DynamicDataDisplay::IsParameterCommandModeSettable(const Integer id) const
{
   if (id == PARAM_TEXT_COLOR)
      return true;
   else
      return Subscriber::IsParameterCommandModeSettable(id);
}

//------------------------------------------------------------------------------
// bool IsSquareBracketAllowedInSetting(const Integer id) const
//------------------------------------------------------------------------------
bool DynamicDataDisplay::IsSquareBracketAllowedInSetting(const Integer id) const
{
   if (id == WARN_COLOR || id == CRIT_COLOR)
      return true;
   else
      return Subscriber::IsSquareBracketAllowedInSetting(id);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id)
//------------------------------------------------------------------------------
std::string DynamicDataDisplay::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case WARN_COLOR:
      return RgbColor::ToRgbString(warnTextColor);
   case CRIT_COLOR:
      return RgbColor::ToRgbString(critTextColor);
   default:
      return Subscriber::GetStringParameter(id);
   }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool DynamicDataDisplay::SetStringParameter(const Integer id,
                                            const std::string &value)
{
   switch (id)
   {
   case ADD_PARAMETERS:
      return AddParameter(value, currentColIndex);
   case PARAM_TEXT_COLOR:
      return SetParameterTextColor(value, -1);
   case ROW_TEXT_COLORS:
      return SetRowTextColors(value, -1);
   case PARAM_BACKGROUND_COLOR:
      return SetParameterBackgroundColor(value, -1);
   case WARN_BOUNDS:
      return SetParameterWarnBounds(value, -1);
   case CRIT_BOUNDS:
      return SetParameterCritBounds(value, -1);
   case WARN_COLOR:
   {
      bool useColorName = true;
      if (GmatStringUtil::IsEnclosedWithBrackets(value))
      {
         useColorName = false;
         try
         {
            // Convert RGB value to unsigned integer
            warnTextColor = RgbColor::ToIntColor(value);
         }
         catch (BaseException &be)
         {
            throw;
         }
      }

      // Check if color is in the database
      if (useColorName)
      {
         // Get corresponding UnsignedInt color
         try
         {
            warnTextColor = ColorDatabase::Instance()->GetIntColor(value);
         }
         catch (BaseException &be)
         {
            throw;
         }
      }
      return true;
   }
   case CRIT_COLOR:
   {
      bool useColorName = true;
      if (GmatStringUtil::IsEnclosedWithBrackets(value))
      {
         useColorName = false;
         try
         {
            // Convert RGB value to unsigned integer
            critTextColor = RgbColor::ToIntColor(value);
         }
         catch (BaseException &be)
         {
            throw;
         }
      }

      // Check if color is in the database
      if (useColorName)
      {
         // Get corresponding UnsignedInt color
         try
         {
            critTextColor = ColorDatabase::Instance()->GetIntColor(value);
         }
         catch (BaseException &be)
         {
            throw;
         }
      }
      return true;
   }
   default:
      return Subscriber::SetStringParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string DynamicDataDisplay::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const str::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool DynamicDataDisplay::SetStringParameter(const std::string &label,
                                            const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
bool DynamicDataDisplay::SetStringParameter(const Integer id,
                                            const std::string &value,
                                            const Integer index)
{
   switch (id)
   {
   case ADD_PARAMETERS:
      return AddParameter(value, index);
   case PARAM_TEXT_COLOR:
      return SetParameterTextColor(value, index);
   case ROW_TEXT_COLORS:
      return SetRowTextColors(value, index);
   case PARAM_BACKGROUND_COLOR:
      return SetParameterBackgroundColor(value, index);
   case WARN_BOUNDS:
      return SetParameterWarnBounds(value, index);
   case CRIT_BOUNDS:
      return SetParameterCritBounds(value, index);
   default:
      return Subscriber::SetStringParameter(id, value, index);
   }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value, const Integer index)
//------------------------------------------------------------------------------
bool DynamicDataDisplay::SetStringParameter(const std::string &label,
                                            const std::string &value,
                                            const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& DynamicDataDisplay::GetStringArrayParameter(const Integer id) const
{
   return Subscriber::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& DynamicDataDisplay::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* DynamicDataDisplay::GetRefObject(const UnsignedInt type,
   const std::string &name)
{
   for (Integer i = 0; i<displayData.size(); i++)
   {
      for (Integer j = 0; j < displayData[i].size(); ++j)
      {
         if (displayData[i][j].paramName == name)
            return displayData[i][j].paramRef;
      }
   }

   throw SubscriberException("DynamicDataDisplay::GetRefObject() the object "
      "name: " + name + "not found\n");
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
bool DynamicDataDisplay::SetRefObject(GmatBase *obj, const UnsignedInt type,
   const std::string &name)
{
   #if DBGLVL_DYNAMICDATADISPLAY_REF_OBJ
      MessageInterface::ShowMessage
         ("DynamicDataDisplay::SetRefObject() <%p>'%s' entered, "
         "obj=<%p><%s>'%s', type=%d, name='%s'\n", this, GetName().c_str(), obj,
         obj ? obj->GetTypeName().c_str() : "NULL", obj ?
         obj->GetName().c_str() : "NULL", type, name.c_str());
   #endif

   if (obj == NULL)
   {
      #if DBGLVL_DYNAMICDATADISPLAY_REF_OBJ
            MessageInterface::ShowMessage
               ("DynamicDataDisplay::SetRefObject() <%p>'%s' returning false, "
               "obj is NULL\n");
      #endif
      return false;
   }

   if (type == Gmat::PARAMETER)
   {
      SetWrapperReference(obj, name);

      for (Integer i = 0; i<displayData.size(); ++i)
      {
         for (Integer j = 0; j < displayData[i].size(); ++j)
         {
            // Handle array elements
            std::string realName = displayData[i][j].paramName;

            #if DBGLVL_DYNAMICDATADISPLAY_REF_OBJ > 1
               MessageInterface::ShowMessage("   realName=%s\n",
                  realName.c_str());
            #endif

            if (realName == name)
               displayData[i][j].paramRef = (Parameter*)obj;
         }
      }

      return true;
   }

   return Subscriber::SetRefObject(obj, type, name);
}

//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
* @see GmatBase
*/
//------------------------------------------------------------------------------
bool DynamicDataDisplay::HasRefObjectTypeArray()
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
const ObjectTypeArray& DynamicDataDisplay::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::PARAMETER);
   return refObjectTypes;
}

//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& DynamicDataDisplay::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectNames.clear();

   switch (type)
   {
   case Gmat::UNKNOWN_OBJECT:
   case Gmat::PARAMETER:
   {
      // Handle array index
      for (Integer i = 0; i < displayData.size(); ++i)
      {
         for (Integer j = 0; j < displayData[i].size(); ++j)
         {
            std::string realName =
               GmatStringUtil::GetArrayName(displayData[i][j].paramName);
            if (realName != "")
               refObjectNames.push_back(realName);
         }
      }
   }
      break;
   default:
      break;
   }

   return refObjectNames;
}

//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
const StringArray& DynamicDataDisplay::GetWrapperObjectNameArray(bool completeSet)
{
   yWrapperObjectNames.clear();

   for (Integer i = 0; i < displayData.size(); ++i)
   {
      for (Integer j = 0; j < displayData[i].size(); ++j)
      {
         if (displayData[i][j].paramName != "")
            yWrapperObjectNames.push_back(displayData[i][j].paramName);
      }
   }


   #ifdef DEBUG_WRAPPER_CODE
      MessageInterface::ShowMessage
         ("DynamicDataDisplay::GetWrapperObjectNameArray() '%s' size=%d\n",
         GetName().c_str(), yWrapperObjectNames.size());
      for (UnsignedInt i = 0; i<yWrapperObjectNames.size(); i++)
         MessageInterface::ShowMessage("   %s\n", yWrapperObjectNames[i].c_str());
   #endif

   return yWrapperObjectNames;
}

//------------------------------------------------------------------------------
// void WriteStringArrayValue(Gmat::WriteMode mode, std::string &prefix,
//                            Integer id, bool writeQuotes,
//                            std::stringstream &stream)
//------------------------------------------------------------------------------
/**
* Code that writes the parameter details for a DynamidDataDisplay object.
*
* @param mode The writing mode
* @param prefix Starting portion of the script string used for the parameter
* @param id ID for the parameter that gets written
* @param writeQuotes Boolean determining whether quotes are to be written for
*        the string parameters
* @param stream The stringstream the object is written to
*/
//------------------------------------------------------------------------------
void DynamicDataDisplay::WriteStringArrayValue(Gmat::WriteMode mode,
                                               std::string &prefix,
                                               Integer id, bool writeQuotes,
                                               std::stringstream &stream)
{
   switch (id)
   {
   case ADD_PARAMETERS:
      // Write any parameters that were added for the display to their
      // respective rows
      for (Integer i = 0; i < displayData.size(); ++i)
      {
         stream << prefix << "AddParameters = {" << i + 1;
         for (Integer j = 0; j < displayData[i].size(); ++j)
         {
            if (displayData[i][j].paramName != "")
               stream << ", " << displayData[i][j].paramName;
            else
               stream << ", ''";
         }
         stream << "};\n";
      }
      break;
   case PARAM_TEXT_COLOR:
      for (Integer i = 0; i < displayData.size(); ++i)
      {
         for (Integer j = 0; j < displayData[i].size(); ++j)
         {
            // Check if a text color was set that isn't the default color
            if (displayData[i][j].paramTextColor !=
               RgbColor::ToIntColor("[0 0 0]"))
            {
               stream << prefix << "TextColor = {" <<
                  displayData[i][j].paramName << ", " <<
                  RgbColor::ToRgbString(displayData[i][j].paramTextColor)
                  << "};\n";
            }
         }
      }
      break;
   case ROW_TEXT_COLORS:
      // Write any parameters that were added for the display to their
      // respective rows
      for (Integer i = 0; i < displayData.size(); ++i)
      {
         stream << prefix << "RowTextColors = {" << i + 1;
         for (Integer j = 0; j < displayData[i].size(); ++j)
         {
            stream << ", " <<
               RgbColor::ToRgbString(displayData[i][j].paramTextColor);
         }
         stream << "};\n";
      }
      break;
   case PARAM_BACKGROUND_COLOR:
      for (Integer i = 0; i < displayData.size(); ++i)
      {
         for (Integer j = 0; j < displayData[i].size(); ++j)
         {
            // Check if a background color was set that isn't the default color
            if (displayData[i][j].paramBackgroundColor !=
               RgbColor::ToIntColor("[255 255 255]"))
            {
               stream << prefix << "BackgroundColor = {" <<
                  displayData[i][j].paramName << ", " <<
                  RgbColor::ToRgbString(displayData[i][j].paramBackgroundColor)
                  << "};\n";
            }
         }
      }
      break;
   case WARN_BOUNDS:
      for (Integer i = 0; i < displayData.size(); ++i)
      {
         for (Integer j = 0; j < displayData[i].size(); ++j)
         {
            // Check if new warning bounds were set on a parameter
            if ((displayData[i][j].warnLowerBound != -inf ||
               displayData[i][j].warnUpperBound != inf) &&
               displayData[i][j].paramName != "")
            {
               stream << prefix << "WarnBounds = {" <<
                  displayData[i][j].paramName << ", [" <<
                  displayData[i][j].warnLowerBound << " " <<
                  displayData[i][j].warnUpperBound << "] };\n";
            }
         }
      }
      break;
   case CRIT_BOUNDS:
      for (Integer i = 0; i < displayData.size(); ++i)
      {
         for (Integer j = 0; j < displayData[i].size(); ++j)
         {
            // Check if new critical bounds were set on a parameter
            if ((displayData[i][j].critLowerBound != -inf ||
               displayData[i][j].critUpperBound != inf) &&
               displayData[i][j].paramName != "")
            {
               stream << prefix << "CritBounds = {" <<
                  displayData[i][j].paramName << ", [" <<
                  displayData[i][j].critLowerBound << " " <<
                  displayData[i][j].critUpperBound << "] };\n";
            }
         }
      }
      break;
   default:
      Subscriber::WriteStringArrayValue(mode, prefix, id, writeQuotes, stream);
   }
}

//------------------------------------------------------------------------------
// bool AddParameter(const std::string &paramName, Integer index)
//------------------------------------------------------------------------------
/**
* Add a parameter to watch to the specified row
*
* @param paramName The name of the parameter which will be displayed in the
*        table
* @param index The index of the string from the script
*
* @return True if parameter was successfully added or new row was made
*/
//------------------------------------------------------------------------------
bool DynamicDataDisplay::AddParameter(const std::string &paramName,
                                      Integer index)
{
   // If this is the first entry in the string, it will be the row number to
   // set the following parameters to
   if (index == 0)
   {
      currentRowIndex = atoi(paramName.c_str()) - 1;

      if (currentRowIndex < 0)
      {
         throw SubscriberException("The value of \"" + paramName + "\" is not "
            "an allowed value for the first entry in \"AddParameters\" on "
            "object \"" + instanceName + "\".  Allowed values are "
            "[Integer > 0]");
      }

      if (atoi(paramName.c_str()) > maxRowCount)
      {
         maxRowCount = atoi(paramName.c_str());
         displayData.resize(currentRowIndex + 1);
      }

      if (displayData[currentRowIndex].size() != 0)
         currentColIndex = displayData[currentRowIndex].size();
      else
         currentColIndex = 0;

      return true;
   }

   if (currentRowIndex >= 0)
   {
      bool repeatName = false;

      if (paramName != "")
      {
         for (Integer i = 0; i < displayData.size(); ++i)
         {
            for (Integer j = 0; j < displayData[i].size(); ++j)
            {
               if (displayData[i][j].paramName == paramName)
               {
                  repeatName = true;
                  break;
               }
            }
            if (repeatName)
               break;
         }
      }

      if (!repeatName)
      {
         displayData[currentRowIndex].resize(currentColIndex + 1);
         displayData[currentRowIndex][currentColIndex].paramName = paramName;
         displayData[currentRowIndex][currentColIndex].paramRef = NULL;
         displayData[currentRowIndex][currentColIndex].refObjectName = "";
         displayData[currentRowIndex][currentColIndex].paramTextColor =
            ColorDatabase::Instance()->GetIntColor("Black");
         displayData[currentRowIndex][currentColIndex].paramBackgroundColor =
            ColorDatabase::Instance()->GetIntColor("White");
         displayData[currentRowIndex][currentColIndex].warnLowerBound = -inf;
         displayData[currentRowIndex][currentColIndex].warnUpperBound = inf;
         displayData[currentRowIndex][currentColIndex].critLowerBound = -inf;
         displayData[currentRowIndex][currentColIndex].critUpperBound = inf;
         displayData[currentRowIndex][currentColIndex].isTextColorUserSet =
            false;

         if (paramName != "")
            yParamWrappers.push_back(NULL);
         //displayData[currentRowIndex][currentColIndex].paramWrapper =
            //yParamWrappers[numParams];
         ++numParams;
         ++currentColIndex;
         if (currentColIndex > maxColCount)
            maxColCount = currentColIndex;

         return true;
      }
      else
         return true;
   }

   return false;
}

//------------------------------------------------------------------------------
// bool SetParameterTextColor(const std::string &scriptString, Integer index)
//------------------------------------------------------------------------------
/**
* Sets the text color of a chosen parameter to be displayed in the dynamic data
* display
*
* @param scriptString The string taken from the script
* @param index The index of the string from the script
*
* @return True if parameter's text color was set successfully
*/
//------------------------------------------------------------------------------
bool DynamicDataDisplay::SetParameterTextColor(const std::string &scriptString,
                                               Integer index)
{
   if (index == 0)
   {
      paramToUpdate = scriptString;
      return true;
   }
   else if (index == 1)
   {
      bool useColorName = true;
      UnsignedInt intColor;
      std::string colorStr = scriptString;
      // Check if RGB value is used
      if (GmatStringUtil::IsEnclosedWithBrackets(colorStr))
      {
         useColorName = false;
         try
         {
            // Convert RGB value to unsigned integer
            intColor = RgbColor::ToIntColor(colorStr);
         }
         catch (BaseException &be)
         {
            throw;
         }
      }

      // Check if color is in the database
      if (useColorName)
      {
         // Get corresponding UnsignedInt color
         try
         {
            intColor = ColorDatabase::Instance()->GetIntColor(colorStr);
         }
         catch (BaseException &be)
         {
            throw;
         }
      }

      bool paramFound = false;
      for (Integer i = 0; i < maxRowCount; ++i)
      {
         for (Integer j = 0; j < displayData[i].size(); ++j)
         {
            if (paramToUpdate == displayData[i][j].paramName)
            {
               displayData[i][j].paramTextColor = intColor;
               if (intColor != 0)
                  displayData[i][j].isTextColorUserSet = true;
               paramFound = true;
               break;
            }
         }
         if (paramFound)
            break;
      }

      if (!paramFound)
      {
         throw SubscriberException("The parameter \"" + paramToUpdate + "\" "
            "is not currently added to object \"" + instanceName + "\"");
      }

      paramToUpdate = "";
      return true;
   }
   else if (scriptString.front() == '{' && scriptString.back() == '}')
   {
      TextParser tp;
      StringArray scriptStringValues =
         tp.SeparateBrackets(scriptString, "{}", " ,", false);

      bool useColorName = true;
      UnsignedInt intColor;
      std::string colorStr = scriptStringValues[1];
      // Check if RGB value is used
      if (GmatStringUtil::IsEnclosedWithBrackets(colorStr))
      {
         useColorName = false;
         try
         {
            // Convert RGB value to unsigned integer
            intColor = RgbColor::ToIntColor(colorStr);
         }
         catch (BaseException &be)
         {
            throw;
         }
      }

      // Check if color is in the database
      if (useColorName)
      {
         // Get corresponding UnsignedInt color
         try
         {
            intColor = ColorDatabase::Instance()->GetIntColor(colorStr);
         }
         catch (BaseException &be)
         {
            throw;
         }
      }

      bool paramFound = false;
      for (Integer i = 0; i < maxRowCount; ++i)
      {
         for (Integer j = 0; j < displayData[i].size(); ++j)
         {
            if (scriptStringValues[0] == displayData[i][j].paramName)
            {
               displayData[i][j].paramTextColor = intColor;
               if (intColor != 0)
                  displayData[i][j].isTextColorUserSet = true;
               paramFound = true;
               break;
            }
         }
         if (paramFound)
            break;
      }

      if (!paramFound)
      {
         throw SubscriberException("The parameter \"" + paramToUpdate + "\" "
            "is not currently added to object \"" + instanceName + "\"");
      }
   }
   else
   {
      throw SubscriberException("Only two entries are allowed in "
         "\"TextColor\" on object \"" + instanceName + "\".  Allowed values "
         "are {[String], [String or RGB triplet]}");
   }

   return true;
}

//------------------------------------------------------------------------------
// bool SetRowTextColors(const std::string &scriptString, Integer index)
//------------------------------------------------------------------------------
/**
* Sets the text colors of parameters in a chosen row to be displayed in the
* dynamic data display
*
* @param scriptString The string taken from the script
* @param index The index of the string from the script
*
* @return True if row's text colors were set successfully
*/
//------------------------------------------------------------------------------
bool DynamicDataDisplay::SetRowTextColors(const std::string &scriptString,
                                          Integer index)
{
   if (index == -1)
      throw SubscriberException();

   if (index == 0)
   {
      errorAlreadyShown = false;
      currentRowIndex = atoi(scriptString.c_str()) - 1;
      if (currentRowIndex < 0)
      {
         throw SubscriberException("The value of \"" + scriptString + "\" is "
            "not an allowed value for the first entry in \"RowTextColors\" on "
            "object \"" + instanceName + "\".  Allowed values are "
            "[Integer > 0]");
      }
      else if (currentRowIndex > displayData.size() - 1)
      {
         throw SubscriberException("The given row index is outside the size "
            "of " + instanceName);
      }
      return true;
   }

   if (currentRowIndex < displayData.size())
   {
      if (index - 1 > displayData[currentRowIndex].size() - 1 ||
         displayData[currentRowIndex].size() == 0)
      {
         if (!errorAlreadyShown)
         {
            errorAlreadyShown = true;
            throw SubscriberException("The number of listed colors exceeds the "
               "number of parameters found on row \"" +
               GmatStringUtil::ToString(currentRowIndex + 1, 1) + "\" for object \"" +
               instanceName + "\"");
         }
         else
            return true;
      }

      bool useColorName = true;
      UnsignedInt intColor;
      std::string colorStr = scriptString;
      // Check if RGB value is used
      if (GmatStringUtil::IsEnclosedWithBrackets(colorStr))
      {
         useColorName = false;
         try
         {
            // Convert RGB value to unsigned integer
            intColor = RgbColor::ToIntColor(colorStr);
         }
         catch (BaseException &be)
         {
            throw;
         }
      }

      // Check if color is in the database
      if (useColorName)
      {
         // Get corresponding UnsignedInt color
         try
         {
            intColor = ColorDatabase::Instance()->GetIntColor(colorStr);
         }
         catch (BaseException &be)
         {
            throw;
         }
      }

      displayData[currentRowIndex][index - 1].paramTextColor = intColor;
      if (intColor != 0)
         displayData[currentRowIndex][index - 1].isTextColorUserSet = true;
   }
   return true;
}

//------------------------------------------------------------------------------
// bool SetParameterBackgroundColor(const std::string &scriptString,
//                                  Integer index)
//------------------------------------------------------------------------------
/**
* Sets the background color of a chosen parameter's cell to be displayed in the
* dynamic data display
*
* @param scriptString The string taken from the script
* @param index The index of the string from the script
*
* @return True if parameter's background color was set successfully
*/
//------------------------------------------------------------------------------
bool DynamicDataDisplay::SetParameterBackgroundColor(const std::string &scriptString,
                                                     Integer index)
{
   if (index == 0)
   {
      paramToUpdate = scriptString;
      return true;
   }
   else if (index == 1)
   {
      bool useColorName = true;
      UnsignedInt intColor;
      std::string colorStr = scriptString;
      // Check if RGB value is used
      if (GmatStringUtil::IsEnclosedWithBrackets(colorStr))
      {
         useColorName = false;
         try
         {
            // Convert RGB value to unsigned integer
            intColor = RgbColor::ToIntColor(colorStr);
         }
         catch (BaseException &be)
         {
            throw;
         }
      }

      // Check if color is in the database
      if (useColorName)
      {
         // Get corresponding UnsignedInt color
         try
         {
            intColor = ColorDatabase::Instance()->GetIntColor(colorStr);
         }
         catch (BaseException &be)
         {
            throw;
         }
      }

      bool paramFound = false;
      for (Integer i = 0; i < maxRowCount; ++i)
      {
         for (Integer j = 0; j < displayData[i].size(); ++j)
         {
            if (paramToUpdate == displayData[i][j].paramName)
            {
               displayData[i][j].paramBackgroundColor = intColor;
               paramFound = true;
               break;
            }
         }
         if (paramFound)
            break;
      }

      if (!paramFound)
      {
         throw SubscriberException("The parameter \"" + paramToUpdate + "\" "
            "is not currently added to object \"" + instanceName + "\"");
      }

      paramToUpdate = "";
      return true;
   }
   else
   {
      throw SubscriberException("Only two entries are allowed in "
         "\"BackgroundColor\" on object \"" + instanceName + "\".  Allowed "
         "values are {[String], [String or RGB triplet]}");
   }
}

//------------------------------------------------------------------------------
// bool SetParameterWarnBounds(const std::string &scriptString, Integer index)
//------------------------------------------------------------------------------
/**
* Sets the warning bounds of a parameter.  If the parameter steps out of one of
* these bounds, the text color changes to the warning color (unless the color
* has already been overridden.
*
* @param scriptString The string taken from the script
* @param index The index of the string from the script
*
* @return True if parameter's warning bounds were set successfully
*/
//------------------------------------------------------------------------------
bool DynamicDataDisplay::SetParameterWarnBounds(const std::string &scriptString,
                                                Integer index)
{
   if (index == 0)
   {
      paramToUpdate = scriptString;
      return true;
   }
   else if (index == 1)
   {
      RealArray newBounds = GmatStringUtil::ToRealArray(scriptString);
      if (newBounds[0] > newBounds[1])
      {
         throw SubscriberException("The new lower warning bound set for \"" +
            paramToUpdate + "\" on object \"" + instanceName + "\" is greater "
            "than the new upper warning bound");
      }

      bool paramFound = false;

      for (Integer i = 0; i < displayData.size(); ++i)
      {
         for (Integer j = 0; j < displayData[i].size(); ++j)
         {
            if (displayData[i][j].paramName == paramToUpdate)
            {
               displayData[i][j].warnLowerBound = newBounds[0];
               displayData[i][j].warnUpperBound = newBounds[1];
               paramFound = true;
               break;
            }
         }
         if (paramFound)
            break;
      }

      if (!paramFound)
      {
         throw SubscriberException("The parameter \"" + paramToUpdate + "\" "
            "is not currently added to object \"" + instanceName + "\"");
      }

      paramToUpdate = "";
      return true;
   }
   else
   {
      throw SubscriberException("Only two entries are allowed in "
         "\"WarnBounds\" on object \"" + instanceName + "\".  Allowed "
         "values are {[String], [Real Array]}");
   }

   return false;
}

//------------------------------------------------------------------------------
// bool SetParameterWarnBounds(const std::string &scriptString, Integer index)
//------------------------------------------------------------------------------
/**
* Sets the critical bounds of a parameter.  If the parameter steps out of one of
* these bounds, the text color changes to the warning color (unless the color
* has already been overridden.
*
* @param scriptString The string taken from the script
* @param index The index of the string from the script
*
* @return True if parameter's critical bounds were set successfully
*/
//------------------------------------------------------------------------------
bool DynamicDataDisplay::SetParameterCritBounds(const std::string &scriptString,
                                                Integer index)
{
   if (index == 0)
   {
      paramToUpdate = scriptString;
      return true;
   }
   else if (index == 1)
   {
      RealArray newBounds = GmatStringUtil::ToRealArray(scriptString);
      if (newBounds[0] > newBounds[1])
      {
         throw SubscriberException("The new lower warning bound set for \"" +
            paramToUpdate + "\" on object \"" + instanceName + "\" is greater "
            "than the new upper warning bound");
      }

      bool paramFound = false;

      for (Integer i = 0; i < displayData.size(); ++i)
      {
         for (Integer j = 0; j < displayData[i].size(); ++j)
         {
            if (displayData[i][j].paramName == paramToUpdate)
            {
               displayData[i][j].critLowerBound = newBounds[0];
               displayData[i][j].critUpperBound = newBounds[1];
               paramFound = true;
               break;
            }
         }
         if (paramFound)
            break;
      }
      if (!paramFound)
      {
         throw SubscriberException("The parameter \"" + paramToUpdate + "\" "
            "is not currently added to object \"" + instanceName + "\"");
      }

      paramToUpdate = "";
      return true;
   }
   else
   {
      throw SubscriberException("Only two entries are allowed in "
         "\"CritBounds\" on object \"" + instanceName + "\".  Allowed "
         "values are {[String], [Real Array]}");
   }

   return false;
}
