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
 * Contains all of the models loaded and maps them to an ID to be referenced
 * by the spacecraft and used by the canvases. 
 */
//------------------------------------------------------------------------------

#include "ModelManager.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_LOAD_MODEL

ModelManager* ModelManager::theInstance = NULL;

//------------------------------------------------------------------------------
// static ModelManager* ModelManager::Instance()
//------------------------------------------------------------------------------
ModelManager* ModelManager::Instance()
{
   if (theInstance == NULL)
      theInstance = new ModelManager;
   return theInstance;
}

//------------------------------------------------------------------------------
// ModelManager()
//------------------------------------------------------------------------------
ModelManager::ModelManager()
{
   #ifdef DEBUG_MODEL_MANAGER
   MessageInterface::ShowMessage("ModelManager::ModelManager() entered\n");
   #endif
   
   theGLContext = NULL;
   numElements = 0;
	modelMap.clear();
	modelIdMap.clear();
	// Why create new ModelObject here and set to [-1]? (LOJ: 2011.12.08)
   //modelMap[-1] = new ModelObject(); // Commented out (LOJ: 2012.07.17)
   
   #ifdef DEBUG_MODEL_MANAGER
   MessageInterface::ShowMessage("ModelManager::ModelManager() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// ~ModelManager()
//------------------------------------------------------------------------------
ModelManager::~ModelManager()
{
   #ifdef DEBUG_MODEL_MANAGER
   MessageInterface::ShowMessage
      ("ModelManager::~ModelManager() entered, modelMap.size() = %d\n", modelMap.size());
   #endif
   
   for (ModelMap::iterator pos = modelMap.begin(); pos != modelMap.end(); ++pos)
   {
      if (pos->second)
         delete pos->second;
   }
   
   // Patch from Tristan Moody
   //   This patch moves the theGLContext deletion to the ModelManager
   //   destructor from the OrbitViewCanvas and GroundTrackCanvas destructors.
   //   As long as the ModelManager destructor is called after all other
   //   OpenGL-related code is finished (to be verified), this should fix the
   //   problem reported in Bug 2591.
   //
   // New code (next 2 lines):
   if (theGLContext != NULL)
      delete theGLContext;
}


//------------------------------------------------------------------------------
// wxGLContext* GetSharedGLContext()
//------------------------------------------------------------------------------
wxGLContext* ModelManager::GetSharedGLContext()
{
   return theGLContext;
}


//------------------------------------------------------------------------------
// void SetSharedGLContext(wxGLContext *glContext)
//------------------------------------------------------------------------------
void ModelManager::SetSharedGLContext(wxGLContext *glContext)
{
   theGLContext = glContext;
}


//------------------------------------------------------------------------------
// void ClearModel()
//------------------------------------------------------------------------------
void ModelManager::ClearModel()
{
   if (theGLContext)
      delete theGLContext;
   
   theGLContext = NULL;
   numElements = 0;
	modelMap.clear();
	modelIdMap.clear();
}


//------------------------------------------------------------------------------
// ModelObject* GetModel(int id)
//------------------------------------------------------------------------------
ModelObject* ModelManager::GetModel(int id)
{
   if (modelMap.find(id) != modelMap.end())
      return modelMap[id];
   return NULL;
}


//------------------------------------------------------------------------------
// int LoadModel(std::string &modelPath)
//------------------------------------------------------------------------------
int ModelManager::LoadModel(std::string &modelPath)
{
	#ifdef DEBUG_LOAD_MODEL
	MessageInterface::ShowMessage
		("ModelManager::LoadModel() entered,  modelIdMap.size() = %d\n   input modelPath = '%s'\n",
		 modelIdMap.size(), modelPath.c_str());
	for (ModelIdMap::iterator pos = modelIdMap.begin(); pos != modelIdMap.end(); ++pos)
		MessageInterface::ShowMessage
			("   model id = %d, modelPath = '%s'\n", pos->second, (pos->first).c_str());
	#endif
   
   // Check if modelPath found in the map, if found return modelId
   if (modelIdMap.find(modelPath) != modelIdMap.end())
	{
      int modelId = modelIdMap[modelPath];      
	   #ifdef DEBUG_LOAD_MODEL
		MessageInterface::ShowMessage
			("ModelManager::LoadModel() modelPath found, returning %d\n",
			 modelId);
		#endif
      return modelId;
	}
	
   ModelObject *newModel = new ModelObject();
	
	#ifdef DEBUG_LOAD_MODEL	
	MessageInterface::ShowMessage
		("ModelManager::LoadModel() Created new ModelObject <%p>, numElements = %d\n",
		 newModel, numElements);
	#endif
	
   newModel->Load(modelPath);
   modelMap[numElements] = newModel;
   modelIdMap[modelPath] = numElements;
   numElements++;
	
	#ifdef DEBUG_LOAD_MODEL	
	MessageInterface::ShowMessage
		("ModelManager::LoadModel() created a new model and returning %d\n", numElements-1);
	#endif
	
   return numElements-1;
}

