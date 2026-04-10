//$Id$
//------------------------------------------------------------------------------
//                            ModelManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
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
// number NNG04CC06P.
//
//
// Author: Phillip Silvia
// Created: 2009/09/11
/**
 * Contains all of the models loaded and maps thems to an ID to be referenced
 * by the spacecraft and used by the canvases. 
 */
//------------------------------------------------------------------------------

#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#include <map>
#include "gmatwxdefs.hpp"
#include "ModelObject.hpp"

typedef std::map<int, ModelObject*> ModelMap;
typedef std::map<std::string, int> ModelIdMap;

class ModelManager
{
public:
   static ModelManager* Instance();
	
   wxGLContext* GetSharedGLContext();
	void SetSharedGLContext(wxGLContext *glContext);
   
	void ClearModel();
   ModelObject* GetModel(int id);
   int LoadModel(std::string &modelPath);
   
private:
   // Hide these so no one can call them
   ModelManager();
   ~ModelManager();

   /// Shared GL context between GL canvases
   wxGLContext *theGLContext;
   int numElements;
   ModelMap modelMap;
   ModelIdMap modelIdMap;
   static ModelManager *theInstance;
};

#endif // MODEL_MANAGER_H
