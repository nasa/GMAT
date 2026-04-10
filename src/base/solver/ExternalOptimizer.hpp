//$Id$
//------------------------------------------------------------------------------
//                         ExternalOptimizer
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
// number NNG04CC06P
//
// Author: Wendy C. Shoan, GSFC
// Created: 2006.07.13
//
/**
 * Definition for the ExternalOptimizer base class. 
 */
//------------------------------------------------------------------------------


#ifndef ExternalOptimizer_hpp
#define ExternalOptimizer_hpp

#include "Optimizer.hpp"
//#include "GmatServer.hpp"  
#include "GmatInterface.hpp"  // a singleton

class GmatServer;             // Forward refefence the server


class GMAT_API ExternalOptimizer : public Optimizer
{
public:
   ExternalOptimizer(std::string type, std::string name);
   virtual ~ExternalOptimizer();
   ExternalOptimizer(const ExternalOptimizer &opt);
   ExternalOptimizer&      operator=(const ExternalOptimizer& opt);

   virtual bool        Initialize();

   // Access methods overriden from the base class
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   //virtual Integer     GetIntegerParameter(const Integer id) const;
   //virtual Integer     SetIntegerParameter(const Integer id,
   //                                        const Integer value);
   //virtual bool        GetBooleanParameter(const Integer id) const;
   //virtual bool        SetBooleanParameter(const Integer id,
   //                                        const bool value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);

   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool        SetStringParameter(const std::string &label,
                                          const std::string &value);
   virtual std::string GetStringParameter(const Integer id,
                                          const Integer index) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value,
                                          const Integer index);
   virtual std::string GetStringParameter(const std::string &label,
                                          const Integer index) const;
   virtual bool        SetStringParameter(const std::string &label, 
                                          const std::string &value,
                                          const Integer index);
   //virtual const StringArray&
   //                    GetStringArrayParameter(const Integer id) const;
   //virtual bool        TakeAction(const std::string &action,
   //                               const std::string &actionData = "");

//------------------------------------------------------------------------------
//  bool  Optimize()    <pure virtual> <from Optimizer>
//------------------------------------------------------------------------------
/**
 * This method performs the optimization.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
   virtual bool        Optimize() = 0;


protected:
   // Parameter IDs
   enum
   {
      FUNCTION_PATH = OptimizerParamCount,
      //SOURCE_TYPE,
      ExternalOptimizerParamCount
   };
   

   /// Path for function script
   std::string         functionPath;
   /// array of values for the optimizer - value of the objective funcion;
   /// elements of the gradient (if calculated); values of the constraints
   //std::vector<Real>   costConstraintArray;
   /// type of external interface used (as of 2006.07.13, only MATLAB is 
   /// supported) - Moved to Optimizer
   //std::string         sourceType;
   /// flag indicating whether or not the interface was opened successfully
   /// and the supporting structures needed by the interface were found
   bool                sourceReady;
   /// Pointer to GmatInterface
   GmatInterface       *inSource;
   /// Pointer to GmatServer
   GmatServer          *inSourceServer;
   
   static const std::string    PARAMETER_TEXT[ExternalOptimizerParamCount -
                                              OptimizerParamCount];
   static const Gmat::ParameterType
                               PARAMETER_TYPE[ExternalOptimizerParamCount -
                                              OptimizerParamCount];
   
   // Methods from Solver
   //virtual void                RunNominal();
   //virtual void                RunPerturbation();
   //virtual void                CalculateParameters();
   //virtual void                CheckCompletion();
   //virtual void                RunComplete();

   // Methods used to perform optimization
   //void                        CalculateJacobian();
   //void                        InvertJacobian();

   //void                        FreeArrays();
   //virtual std::string         GetProgressString();
   //virtual void                WriteToTextFile();
   
   virtual bool                  OpenConnection() = 0;
   virtual void                  CloseConnection() = 0;
};

#endif // ExternalOptimizer_hpp
