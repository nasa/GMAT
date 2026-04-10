//------------------------------------------------------------------------------
//                              SNOPTFunctionWrapper
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
// Author: Jeremy Knittel / NASA
// Created: 2016.08.09 
//
/**
 * From original MATLAB prototype:
 * Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef SNOPTFunctionWrapper_hpp
#define SNOPTFunctionWrapper_hpp

#include "SnoptOptimizer.hpp"
#include "Rvector.hpp"

void SNOPTFunctionWrapper(SNOPT_INTEGER    *Status, 
                          SNOPT_INTEGER    *n,
                          SNOPT_DOUBLE     x[],
                          SNOPT_INTEGER    *needF,
                          SNOPT_INTEGER    *nF,
                          SNOPT_DOUBLE     F[],
                          SNOPT_INTEGER    *needG,
                          SNOPT_INTEGER    *neG,
                          SNOPT_DOUBLE     G[],
                          char             *cu,
                          SNOPT_INTEGER    *lencu,
                          SNOPT_INTEGER    iu[],
                          SNOPT_INTEGER    *leniu,
                          SNOPT_DOUBLE     ru[],
                          SNOPT_INTEGER    *lenru);

void StopOptimizer(SNOPT_INTEGER *iAbort, SNOPT_INTEGER KTcond[],
                   SNOPT_INTEGER *MjrPrt, SNOPT_INTEGER *minimz,
                   SNOPT_INTEGER *m, SNOPT_INTEGER *maxS, SNOPT_INTEGER *n,
                   SNOPT_INTEGER *nb, SNOPT_INTEGER *nnCon0,
                   SNOPT_INTEGER *nnCon, SNOPT_INTEGER *nnObj0,
                   SNOPT_INTEGER *nnObj, SNOPT_INTEGER *nS, SNOPT_INTEGER *itn,
                   SNOPT_INTEGER *nMajor, SNOPT_INTEGER *nMinor,
                   SNOPT_INTEGER *nSwap, SNOPT_DOUBLE *condHz,
                   SNOPT_INTEGER *iObj, SNOPT_DOUBLE *sclObj,
                   SNOPT_DOUBLE *ObjAdd, SNOPT_DOUBLE *fMrt,
                   SNOPT_DOUBLE *PenNrm, SNOPT_DOUBLE *step,
                   SNOPT_DOUBLE *prInf, SNOPT_DOUBLE *duInf,
                   SNOPT_DOUBLE *vimax, SNOPT_DOUBLE *virel, SNOPT_INTEGER hs[],
                   SNOPT_INTEGER *ne, SNOPT_INTEGER *nlocJ,
                   SNOPT_INTEGER locJ[], SNOPT_INTEGER indJ[],
                   SNOPT_DOUBLE Jcol[], SNOPT_INTEGER *negCon,
                   SNOPT_DOUBLE Ascale[], SNOPT_DOUBLE bl[], SNOPT_DOUBLE bu[],
                   SNOPT_DOUBLE fCon[], SNOPT_DOUBLE gCon[],
                   SNOPT_DOUBLE gObj[], SNOPT_DOUBLE yCon[], SNOPT_DOUBLE pi[],
                   SNOPT_DOUBLE rc[], SNOPT_DOUBLE rg[], SNOPT_DOUBLE x[],
                   char cu[], SNOPT_INTEGER *lencu, SNOPT_INTEGER iu[],
                   SNOPT_INTEGER *leniu, SNOPT_DOUBLE ru[],
                   SNOPT_INTEGER *lenru, char cw[], SNOPT_INTEGER *lencw,
                   SNOPT_INTEGER iw[], SNOPT_INTEGER *leniw, SNOPT_DOUBLE rw[],
                   SNOPT_INTEGER *lenrw);

#endif //SNOPTFunctionWrapper_hpp
