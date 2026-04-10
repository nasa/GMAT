//$Id$
//------------------------------------------------------------------------------
//                                  RefData
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
// Created: 2004/01/09
//
/**
 * Declares base class of reference data.
 */
//------------------------------------------------------------------------------
#ifndef RefData_hpp
#define RefData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Parameter.hpp" // for namespace GmatParam::

/**
 * Moved out of the RefObjType class so that the Visual Studio exports could be implemented
 */
struct GMAT_API RefObjType
{
   UnsignedInt objType;
   std::string objName;
   GmatBase *obj;

   // Constructor -- default values required for the DevStudio export issues 
   RefObjType(UnsignedInt refType = Gmat::UNKNOWN_OBJECT,
      const std::string &refName = "", GmatBase *ref = NULL)
      {
         objType = refType;
         objName = refName;
         obj     = ref;
      };

   RefObjType& operator= (const RefObjType& right)
      {
         if (this == &right)
            return *this;
         objType = right.objType;
         objName = right.objName;
         obj     = right.obj;
         return *this;
      };
};

#ifdef EXPORT_TEMPLATES

    // Instantiate STL template classes used in GMAT  
    // This does not create an object. It only forces the generation of all
    // of the members of the listed classes. It exports them from the DLL 
    // and imports them into the .exe file.

    // This fixes std::string:
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<RefObjType>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<RefObjType>;

#endif

class GMAT_API RefData
{
public:

   RefData(const std::string &name = "", const std::string &typeName = "",
           const UnsignedInt paramOwnerType = Gmat::SPACECRAFT,
           GmatParam::DepObject depObj = GmatParam::NO_DEP,
           bool isSettable = false);
   RefData(const RefData &rd);
   RefData& operator= (const RefData &rd);
   virtual ~RefData();
   
   void                 SetParameter(Parameter *param);
   Parameter*           GetParameter();
   
   bool                 SetName(const std::string &newName,
                                const std::string &oldName = "");
   GmatBase*            GetSpacecraft();
   GmatBase*            GetParameterOwner();
   
   Integer              GetNumRefObjects() const;
   
   virtual std::string          GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray&   GetRefObjectNameArray(const UnsignedInt type);
   
   virtual bool                 SetRefObjectName(const UnsignedInt type,
                                                 const std::string &name);
   virtual GmatBase*            GetRefObject(const UnsignedInt type,
                                             const std::string &name = "");
   virtual bool                 SetRefObject(GmatBase *obj, const UnsignedInt type,
                                             const std::string &name = "");
   
   virtual bool                 RenameRefObject(const UnsignedInt type,
                                             const std::string &oldName,
                                             const std::string &newName);
   
   virtual bool                ValidateRefObjects(GmatBase *param) = 0;
   virtual const std::string*  GetValidObjectList() const;

protected:

   Parameter   *mParameter;
   std::string mActualParamName;
   std::string mParamOwnerName;
   std::string mParamDepName;
   std::string mParamTypeName;
   UnsignedInt mParamOwnerType;
   GmatParam::DepObject mParamDepObj;
   bool mIsParamSettable;
   std::vector<RefObjType> mRefObjList;
   
   StringArray mObjectTypeNames;
   StringArray mAllRefObjectNames;
   Integer mNumRefObjects;
   
   virtual bool         AddRefObject(const UnsignedInt type,
                                     const std::string &name, GmatBase *obj = NULL,
                                     bool replaceName = false);
   
   bool                 SetRefObjectWithNewName(GmatBase *obj,
                                                const UnsignedInt type,
                                                const std::string &name);
   
   bool                 HasObjectType(const std::string &type) const;
   GmatBase*            FindFirstObject(const std::string &type) const;
   GmatBase*            FindFirstObject(const UnsignedInt type) const;
   std::string          FindFirstObjectName(const std::string &type) const;
   std::string          FindFirstObjectName(const UnsignedInt type) const;
   
   StringArray          FindObjectNames(const UnsignedInt type) const;
   GmatBase*            FindObject(const UnsignedInt type, const std::string name) const;
   
   virtual void         InitializeRefObjects();
   virtual bool         IsValidObjectType(UnsignedInt type) = 0;
};
#endif // RefData_hpp

