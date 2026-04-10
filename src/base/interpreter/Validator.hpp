//$Id$
//------------------------------------------------------------------------------
//                                  Validator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// number NNG06CCA54C
//
// Author: Linda Jun
// Created: 2008/04/01 (Made separate class out of Interpreter::CreateElementWrapper())
//
// Original Wrapper code from the Interpreter.
// Author: Wendy C. Shoan
//
/**
 * Definition for the Validator class.
 * This class validates objects command and builds ElemementWrappers for
 * various data types.
 */
//------------------------------------------------------------------------------
#ifndef Validator_hpp
#define Validator_hpp

#include "gmatdefs.hpp"

// Forward references
class Interpreter;
class Moderator;
class SolarSystem;
class GmatCommand;
class Function;
class Parameter;
class AxisSystem;
class ElementWrapper;
class RHSEquation;

class GMAT_API Validator
{
public:
   
   static Validator* Instance();
   
   void SetInterpreter(Interpreter *interp);
   void SetSolarSystem(SolarSystem *ss);
   void SetObjectMap(ObjectMap *objMap);
   void SetFunction(Function *func);
   
   bool StartMatlabServer(GmatCommand *cmd);
   bool CheckUndefinedReference(GmatBase *obj, bool contOnError = true/*,
         bool includeOwnedObjects = false*/);
   bool ValidateCommand(GmatCommand *cmd, bool contOnError = true,
                        Integer manage = 1);
   bool ValidateEquation(RHSEquation *eq);
   
   void HandleCcsdsEphemerisFile(ObjectMap *objMap, bool deleteOld = false);
   
   ElementWrapper* CreateElementWrapper(const std::string &desc,
                                        bool parametersFirst = false,
                                        Integer manage = 1);
   const StringArray& GetErrorList();
   void ClearErrorList();
   
   GmatBase* FindObject(const std::string &name, 
                        const std::string &ofType = "");
   Parameter* GetParameter(const std::string name);
   Parameter* CreateParameter(const std::string &type,
                              const std::string &name,
                              const std::string &ownerName = "",
                              const std::string &depName = "",
                              Integer manage = 1);
   Parameter* CreateAutoParameter(const std::string &type,
                                  const std::string &name,
                                  bool &alreadyManaged,
                                  const std::string &ownerName = "",
                                  const std::string &depName = "",
                                  Integer manage = 1);
   Parameter* CreateArray( const std::string &arrayStr, Integer manage = 1);   
   Parameter* CreateSystemParameter(bool &paramCreated, const std::string &str,
                                    Integer manage = 1);
   AxisSystem* CreateAxisSystem(std::string type, GmatBase *owner);
   
   bool IsParameterType(const std::string &desc);
   bool ValidateParameter(const StringArray &refNames, GmatBase *obj);
   bool ValidateSaveCommand(GmatBase *obj);
   bool UpdateLists();
   
private:

   bool CreateAssignmentWrappers(GmatCommand *cmd, Integer manage);
   bool CreateEquationWrappers(RHSEquation *obj);
   
   ElementWrapper* CreateSolarSystemWrapper(GmatBase *obj, const std::string &owner,
                                            const std::string &depobj,
                                            const std::string &type,
                                            Integer manage = 1);
   ElementWrapper* CreateForceModelWrapper(GmatBase *obj, const std::string &owner,
                                           const std::string &depobj,
                                           const std::string &type,
                                           Integer manage = 1);
   ElementWrapper* CreateWrapperWithDot(bool parametersFirst, Integer manage = 1);
   ElementWrapper* CreateValidWrapperWithDot(GmatBase *obj,
                                             const std::string &type,
                                             const std::string &owner,
                                             const std::string &depobj,
                                             bool parametersFirst,
                                             Integer manage);
   ElementWrapper* CreateOtherWrapper(Integer manage = 1);
   ElementWrapper* CreateParameterWrapper(Parameter *param,
                                          Gmat::WrapperDataType &itsType);
   ElementWrapper* CreatePropertyWrapper(GmatBase *obj, const std::string &type,
                                         Integer manage, bool checkSubProp = true);
   ElementWrapper* CreateSubPropertyWrapper(GmatBase *obj,
                                            const std::string &type,
                                            Integer manage);
   
   bool ValidateSubCommand(GmatCommand *cmd, Integer level, Integer manage = 1);
   bool CreateCoordSystemProperty(GmatBase *obj, const std::string &prop,
                                  const std::string &value);
   bool CreatePropSetupProperty(GmatBase *obj, const std::string &prop,
                                const std::string &value);
   bool CreateForceModelProperty(GmatBase *obj, const std::string &prop,
                                 const std::string &value);
   bool HandleError(bool addFunction = true);
   void ShowObjectMap(const std::string &label = "");
   
   Interpreter     *theInterpreter;
   Moderator       *theModerator;
   SolarSystem     *theSolarSystem;
   GmatCommand     *theCommand;
   Function        *theFunction;
   ObjectMap       *theObjectMap;
   StringArray     theParameterList;
   
   std::string     theDescription;
   /// If object not found, it will automatically create StringWrapper
   /// This flag indicates whether or not to create default StringWrapper
   bool            createDefaultStringWrapper;
   
   // If wrapper type of LHS of assignment command is object property wrapper,
   // the LHS object field type will be used to create proper RHS wrapper
   Gmat::ParameterType lhsParamType;
   
   /// Error handling data
   bool            continueOnError;
   bool            skipErrorMessage;
   bool            isFinalError;
   bool            allowObjectProperty;
   StringArray     validatorErrorList;
   std::string     theErrorMsg;
   
   static Validator *instance;
   Validator();
   ~Validator();      
};

#endif // Validator_hpp

