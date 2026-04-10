//$Id: EstimationStateManager.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         EstimationStateManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/08/05
//
/**
 * Definition for the estimation state manager.
 */
//------------------------------------------------------------------------------


#ifndef EstimationStateManager_hpp
#define EstimationStateManager_hpp

#include "GmatBase.hpp"
#include "StateManager.hpp"
#include "MeasurementManager.hpp"
#include "PropagationStateManager.hpp"
#include "Rmatrix.hpp"
#include "PhysicalModel.hpp"

/**
 * Mediator between objects, the estimators, and observations and measurements
 *
 * The estimation state manager is responsible for constructing and filling the
 * estimation state vector, state transition matrix, and covariance matrix used
 * in the estimation process.
 */
class ESTIMATION_API EstimationStateManager : public StateManager
{
public:
   EstimationStateManager(Integer size = 0);
   virtual ~EstimationStateManager();
   virtual void CleanUp();
   EstimationStateManager(const EstimationStateManager &esm);
   EstimationStateManager& operator=(const EstimationStateManager &esm);

   // Methods used to define the estimation state vector
   virtual const StringArray& GetObjectList(std::string ofType = "");
   virtual bool               SetObject(GmatBase* obj);
   virtual bool               CheckObjects();
   virtual bool               SetProperty(std::string prop);
   virtual bool               SetProperty(std::string, Integer);

   virtual bool               IsPropertiesSetupCorrect();

   virtual bool               SetProperty(std::string, GmatBase*);
   virtual bool               SetProperty(GmatBase* solveforObj);
   virtual bool               SetConsider(std::string prop);
   virtual bool               SetConsider(std::string, Integer);
   virtual bool               SetConsider(std::string, GmatBase*);
   virtual void               SetParticipantList(StringArray &p);

   virtual bool               BuildState();
   virtual bool               MapObjectsToVector();
   virtual bool               MapVectorToObjects();
   virtual bool               MapFullVectorToObjects();
   virtual bool               MapObjectsToSTM();
   virtual bool               MapSTMToObjects();
   virtual bool               MapObjectsToCovariances();
   virtual bool               MapCovariancesToObjects();

   virtual IntegerArray       GetSTMMapping(GmatBase* obj, Integer elementId) const;

   /// Get state offset in J2000BodyMJ2000Eq coordinates (currently it is in EarthMJ2000Eq - GMAT internal coordinates).
   virtual GmatState*         GetStateOffset();
   virtual bool               HasStateOffset();
   virtual void               SetHasStateOffset(bool hasOffset);

   virtual Rmatrix*           GetSTM();
   virtual Covariance*        GetCovariance();

   virtual void               SetMeasurementManager(MeasurementManager *mm);

   virtual void               BufferObjects(ObjectArray *buffer = NULL);
   virtual void               RestoreObjects(ObjectArray *fromBuffer = NULL);
   const std::vector<PhysicalModel*>&  GetAllPhysicalModels();                  // Thrust Scale Factor Solve For

//   void                       SolveFor(std::string whichOne = "");
//   void                       Consider(std::string whichOne = "");

   /// Get estimation epoch
   GmatTime                   GetEstimationEpochGT();

   /// Get estimation state in form of participant's state type. State in form of Cr_Epsilon and Cd_Epsilon
   GmatState                  GetEstimationState();
   GmatState*                 SetEstimationState(GmatState& inputState);
   GmatState*                 SetEstimationStateObjects(const GmatState& inputState);
   GmatState*                 SetEstimationCartesianStateParticipant(const GmatState& inputState);
   GmatState*                 SetEstimationMJ2000EqCartesianStateParticipant(const GmatState& inputState);

   /// Get estimation state offset in form of participant's state type. State in form of Cr_Epsilon and Cd_Epsilon
   GmatState                  GetEstimationStateOffset();
   GmatState*                 SetEstimationStateOffset(GmatState& inputState);


   /// Get estimation state in MJ2000Eq Cartesian coordinate system. State in form of Cr_Epsilon and Cd_Epsilon
   GmatState                  GetEstimationMJ2000EqCartesianState();

   /// Get estimation state in MJ2000Eq Cartesian coordinate system. State in form of Cr_Epsilon and Cd_Epsilon
   GmatState                  GetEstimationMJ2000EqCartesianStateForReport();

   /// Get estimation state in participant's Cartesian coordinate system. State in form of Cr_Epsilon and Cd_Epsilon
   GmatState                  GetEstimationCartesianState();
   
   /// Get estimation state in form of participant's state type. State in form of Cr and Cd
   GmatState                  GetEstimationStateForReport(std::string anomalyType = "TA");

   /// Get estimation state in form of participant's Cartesian state type. State in form of Cr and Cd
   GmatState                  GetEstimationCartesianStateForReport();

   /// Get estimation state in form of participant's Keplerian state type. State in form of Cr and Cd
   GmatState                  GetEstimationKeplerianStateForReport(std::string anomalyType = "TA");

   
   Rmatrix66                  CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state);
   Rmatrix                    CartToSolveForStateConversionDerivativeMatrix();
   Rmatrix                    SolveForStateToKeplConversionDerivativeMatrix();

   std::map<Integer,Integer>  GetEqualConstrainsList();

   bool                       HasElementsOfType(const std::string &type);

   StringArray                GetObjectSolveFors(std::string objName);
protected:
   // Attributes

   /// Clones of the objects that supply estimation data, used to restore later
   ObjectArray                estimationObjectClones;

   // Estimation State Pieces
   /// The text identities of the SolveFor parameters
   StringArray                solveForNames;
   /// The names of the SolveFor objects
   StringArray                solveForObjectNames;
   /// The objects providing solve for data.  Repetition is allowed
   ObjectArray                solveForObjects;
   /// Strings used to set property IDs of the solve for parameters
   StringArray                solveForIDNames;
   /// Property IDs of the solve for parameters
   IntegerArray               solveForIDs;

   /// The text identities of the consider parameters
   StringArray                considerNames;
   /// Names of the objects providing consider data.  Repetition is allowed
   StringArray                considerObjectNames;
   /// The objects providing consider data.  Repetition is allowed
   ObjectArray                considerObjects;
   /// Strings used to set property IDs of the consider parameters
   StringArray                considerIDNames;
   /// Property IDs of the consider parameters
   IntegerArray               considerIDs;

   /// Names of the participants, so ones not in other lists are managed
   StringArray                participantNames;

   // STM Pieces
   /// The total state transition matrix used in the estimation
   Rmatrix                    stm;
   /// Mapping used to associate STM elements with objects
   ObjectMap                  stmMap;
   /// Number of columns in the STM matrix
   Integer                    stmColCount;
//   /// Number of rows in the STM
//   Integer                    stmRowCount;

   // Covariance pieces
   /// The covariance matrix
   Covariance                 covariance;
   /// Object associations for the covariances
   ObjectMap                  covMap;
   /// Number of columns in the covariance matrix
   Integer                    covColCount;
//   /// Number of rows in the covariance matrix
//   Integer                    covRowCount;

   /// Pointers to the FileThrust              Thrust Scale Factor Solve For
   std::vector<PhysicalModel*>    pms;

   // Propagation State Pieces
   /// PSM manages components that evolve over time via numerical integration
   PropagationStateManager    *psm;
   /// For convenience, the ESM has direct access to the PSM's state vector
   GmatState                  *propagationState;

   /// If the esm has a state offset from the reference trajectory
   bool                       hasStateOffset;
   /// State offset from the reference trajectory
   GmatState                  stateOffset;

   /// Pointer to the measurement manager used with this estimator
   MeasurementManager         *measMan;

   /// A buffer used for string parsing
   StringArray                chunks;

   // Protected methods
   void                       DecomposeParameter(std::string &param,
                                                 Integer howMany = -1);

   Integer                    SortVector();

private:
   StringArray                GetSolveForList(GmatBase* obj);

   /// Get participant's state in its own coordinate system and in a specify state type
   Rvector6                   GetParticipantState(GmatBase* spaceObj, std::string inStateType, std::string anomalyType = "MA");
   bool                       SetParticipantState(GmatBase* spaceObj, Rvector6& inputState, std::string inStateType);

   /// Get participant's state in MJ2000Eq coordinate system and in a specify state type
   Rvector6                   GetParticipantMJ2000EqState(GmatBase* spaceObj, std::string inStateType, std::string anomalyType = "MA");
   bool                       SetParticipantMJ2000EqState(GmatBase* spaceObj, Rvector6& inputState, std::string inStateType);

   /// Get participant's state offset in MJ2000Eq coordinate system and in a specify state type
   Rvector6                   GetParticipantMJ2000EqStateOffset(GmatBase* spaceObj, std::string inStateType, std::string anomalyType = "MA");
   bool                       SetParticipantMJ2000EqStateOffset(GmatBase* spaceObj, Rvector6& inputState, std::string inStateType);

   /// Get participant's state in its own coordinate system and Cartesian state type
   Rvector                    GetParticipantCartesianState(GmatBase* spaceObj);

   /// Get participant's state in MJ2000Eq Cartesian coordinate system
   Rvector                    GetParticipantMJ2000EqCartesianState(GmatBase* spaceObj);

   /// the ID of Spacecraft's CartesianState. It is used for mapping
   /// spacecarft's acceleration from vector to spacecraft
   Integer cartesianStateID;


};

#endif /* EstimationStateManager_hpp */
