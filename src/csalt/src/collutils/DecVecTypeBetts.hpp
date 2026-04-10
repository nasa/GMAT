//------------------------------------------------------------------------------
//                              DecVecTypeBetts
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
// Author: Wendy Shoan / NASA
// Created: 2015.06.17
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 *
 * DecVecTypeBetts DecisionVector organized similar to that used by
 * Bett's.
 *  Z = [t_o t_f y_10 u_10 y_11 u_11 ... y_nm u_nm s_1 .. s_o w_1..w_p]
 *
 */
//------------------------------------------------------------------------------

#ifndef DecVecTypeBetts_hpp
#define DecVecTypeBetts_hpp

#include "csaltdefs.hpp"
#include "DecisionVector.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"

class CSALT_API DecVecTypeBetts : public DecisionVector
{
public:
   // class methods
   /// default constructor
   DecVecTypeBetts();
   /// copy constructor
   DecVecTypeBetts(const DecVecTypeBetts &copy);
   /// operator =
   DecVecTypeBetts& operator=(const DecVecTypeBetts &copy);
   
   /// destructor
   virtual ~DecVecTypeBetts();
   
   virtual Rvector  GetStateVector(Integer meshIdx, Integer stageIdx);
   virtual Rvector  GetControlVector(Integer meshIdx, Integer stageIdx);
   virtual IntegerArray GetFinalStateIdxs();
   virtual IntegerArray GetInitialStateIdxs();
   virtual IntegerArray GetStateIdxsAtMeshPoint(Integer meshIdx,
                                                Integer stageIdx);
   virtual IntegerArray GetControlIdxsAtMeshPoint(Integer meshIdx,
                                                  Integer stageIdx);
   virtual IntegerArray GetStaticIdxs();
   virtual Integer  GetFinalTimeIdx();
   virtual Integer  GetInitialTimeIdx();
   virtual Rmatrix  GetControlArray();
   virtual Rmatrix  GetStateArray();
   virtual void     SetControlArray(const Rmatrix &cArray);
   virtual void     SetStateArray(const Rmatrix &sArray);
   
   // These are here only for test drivers?
   virtual bool     SetStateVector(Integer meshIdx,Integer stageIdx,
                                   const Rvector &stateVec);
   virtual bool     SetControlVector(Integer meshIdx,Integer stageIdx,
                                     const Rvector &controlVec);

protected:
   
   /// The number of optimization params in one mesh step
   Integer numParamsPerMesh;
   /// Number of state vars+control vars in opt. control prob
   Integer numStateAndControlVars;
   /// Number of stage points
   Integer numStagePoints;
   /// Has control at last mesh point (HS does, Radau does not)
   bool hasControlAtFinalMesh;
   
   // protected methods
   virtual void SetChunkIndeces();  // inherited
   virtual bool ValidateMeshStageIndeces(Integer meshIdx, Integer stageIdx);

};
#endif // DecVecTypeBetts_hpp

