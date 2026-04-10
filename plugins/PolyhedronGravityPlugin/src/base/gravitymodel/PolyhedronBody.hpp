// PolyhedronBody.hpp
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
//  Created on: Aug 27, 2012
//      Author: tdnguye2

#ifndef PolyhedronBody_hpp
#define PolyhedronBody_hpp

#include "Rmatrix66.hpp"

struct Edge
{
	Integer vertex1, vertex2;
};
typedef std::vector<Integer>         PolygonFace;
typedef std::vector<Rvector3>        PointsList;
typedef PointsList        			    Vectors3List;
typedef std::vector<PolygonFace>           FacesList;
typedef std::vector<Edge>                  EdgesList;
typedef std::vector<Integer>		          FaceIndexList;
typedef std::map<Integer, Edge>            EdgesMap;
typedef std::map<Integer, Integer>		    FaceIndexMap;

class PolyhedronBody
{
public:

   PolyhedronBody(const std::string &filename);
   virtual ~PolyhedronBody();
   PolyhedronBody(const PolyhedronBody& polybody);
   PolyhedronBody& operator= (const PolyhedronBody& polybody);

   virtual bool Initialize();

   // inherited from GmatBase
   virtual PolyhedronBody*   Clone() const;
   virtual void              Copy(const PolyhedronBody* orig);

public:
   bool LoadBodyShape();	// load the data which define shape of the body

   bool FaceNormals();
   bool Incenters();
   bool IsInEdgesList(Edge& edge, bool& isAttachmentB);
   bool Edges();
   bool EdgeAttachments(Integer edgeindex, Integer& faceA_index, Integer& faceB_index);

   bool Calculation(Rvector6 x, Rvector6& xdot, Rmatrix66& A);		// calculate gravity


   std::string bodyShapeFilename;
   PointsList    verticesList;
   FacesList     facesList;

   Vectors3List  fn;
   PointsList 	  ic;
   EdgesList	  E;
   FaceIndexList attachmentA;
   FaceIndexList attachmentB;

   EdgesMap	    edgeMap;
   FaceIndexMap attachmentAMap;
   FaceIndexMap attachmentBMap;

private:
   bool isLoad;
};


#endif /* PolyhedronBody_hpp */
