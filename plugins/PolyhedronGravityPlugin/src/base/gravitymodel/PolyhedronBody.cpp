// Polyhedron.cpp
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


#include <iostream>
#include <fstream>
#include <sstream>
#include "UtilityException.hpp"
#include "StringUtil.hpp"
#include "Rvector3.hpp"
#include "RealUtilities.hpp"

#include "MessageInterface.hpp"

#include "PolyhedronBody.hpp"

using namespace GmatMathUtil;
using namespace GmatStringUtil;

//#define DEBUG_CONSTRUCTION
//#define DEBUG_CALCULATION
//#define DEBUG_READ_DATAFILE
//#define DEBUG_INCENTERS_CALCULATION
//#define DEBUG_FACENORMALS_CALCULATION

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

PolyhedronBody::PolyhedronBody(const std::string &filename):
   isLoad       (false)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PolyhedronBody default construction <%p>\n", this);
#endif
   bodyShapeFilename = filename;
}


PolyhedronBody::~PolyhedronBody()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PolyhedronBody destruction <%p>\n", this);
#endif
   // clean up vertices list
   verticesList.clear();
   
   // clean up faces list
   for (UnsignedInt i = 0; i < facesList.size(); ++i)
   {
      facesList[i].clear();
   }
   facesList.clear();

   fn.clear();
   ic.clear();
   E.clear();
   attachmentA.clear();
   attachmentB.clear();

   edgeMap.clear();
   attachmentAMap.clear();
   attachmentBMap.clear();

}


PolyhedronBody::PolyhedronBody(const PolyhedronBody& polybody):
   verticesList  (polybody.verticesList),
   facesList 	  (polybody.facesList),
   isLoad        (polybody.isLoad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PolyhedronBody copy construction <%p>\n", this);
#endif
}


PolyhedronBody& PolyhedronBody::operator= (const PolyhedronBody& polybody)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PolyhedronBody operator = <%p>\n", this);
#endif
   if (&polybody == this)
      return *this;

   verticesList = polybody.verticesList;
   facesList 	 = polybody.facesList;
   isLoad       = polybody.isLoad;

   return *this;
}


bool PolyhedronBody::Initialize()
{
	return true;
}


PolyhedronBody* PolyhedronBody::Clone() const
{
   return (new PolyhedronBody(*this));
}


void PolyhedronBody::Copy(const PolyhedronBody* orig)
{
   operator=(*(orig));
}


//-------------------------------------------------------------------------------
// bool PolyhedronBody::LoadBodyShape()
//-------------------------------------------------------------------------------
/*
 * This function is used to load shape of a body from data file.
*/
//-------------------------------------------------------------------------------
bool PolyhedronBody::LoadBodyShape()
{
   if (isLoad)
      return true;
   
   #ifdef DEBUG_READ_DATAFILE
      MessageInterface::ShowMessage("PolyhedronBody::LoadBodyShape() loading "
            "data from %s\n", bodyShapeFilename.c_str());
   #endif

   // 1. Open data file:
   std::ifstream bsFile(bodyShapeFilename.c_str());
   if (!bsFile)
      throw UtilityException("Error opening body shape file: \'" + 
                             bodyShapeFilename + "\'");
   bsFile.setf(std::ios::skipws);

   #ifdef DEBUG_READ_DATAFILE
      MessageInterface::ShowMessage("Starting\n");
   #endif

   std::string  line, subline;
   // 2. Read all vertices and store them to pointsList:
   Integer numvertices;
   Integer index;
   Real x, y, z;
   Rvector3 vertex;

   // 2.1. Read number of vertices:
   getline(bsFile,line);
   GmatStringUtil::ToInteger(line,&numvertices);

   #ifdef DEBUG_READ_DATAFILE
      MessageInterface::ShowMessage("   %d vertices\n", numvertices);
   #endif

   // 2.2. Read all vertices:
   Integer first, last;
   for(Integer i=0; (i < numvertices)&&(!bsFile.eof()); ++i)
   {
	  // get a line from data file:
      getline(bsFile,line);

      // get record's index:
      line = GmatStringUtil::Trim(line, LEADING);
      GmatStringUtil::FindFirstAndLast(line,' ',first,last);
      subline = line.substr(0,first);
      GmatStringUtil::ToInteger(subline, &index);
      line = line.substr(first, line.size()-first);

      // get vertex's x-coordinate:
      line = GmatStringUtil::Trim(line, LEADING);
      GmatStringUtil::FindFirstAndLast(line,' ',first,last);
      subline = line.substr(0,first);
      GmatStringUtil::ToReal(subline, &x);
      line = line.substr(first, line.size()-first);

      // get vertex's y-coordinate:
      line = GmatStringUtil::Trim(line, LEADING);
      GmatStringUtil::FindFirstAndLast(line,' ',first,last);
      subline = line.substr(0,first);
      GmatStringUtil::ToReal(subline, &y);
      line = line.substr(first, line.size()-first);

      // get vertex's z-coordinate:
      line = GmatStringUtil::Trim(line, LEADING);
      GmatStringUtil::FindFirstAndLast(line,' ',first,last);
      subline = line.substr(0,first);
      GmatStringUtil::ToReal(subline, &z);

      // push the vertex to verticesList:
      vertex.Set(x, y,z);				             // unit: km
	   verticesList.push_back(vertex);
   }

   #ifdef DEBUG_READ_DATAFILE
      MessageInterface::ShowMessage("   %d loaded\n", verticesList.size());
   #endif

   if (bsFile.eof())
      throw UtilityException("Error: missing data in file: " +
                             bodyShapeFilename);

   #ifdef DEBUG_READ_DATAFILE
      MessageInterface::ShowMessage("number of vertices = %d\n", verticesList.size());
      for (UnsignedInt i = 0; i < verticesList.size(); ++i)
         MessageInterface::ShowMessage("%.15lf   %.15lf   %.15lf\n", verticesList[i].Get(0), verticesList[i].Get(1), verticesList[i].Get(2) );
      MessageInterface::ShowMessage("\n");
   #endif

   // 3. Read all faces and store them to facesList:
   Integer numfaces;
   Integer ix, iy, iz;
   PolygonFace triangularface;

   // 3.1. Read number of faces:
   getline(bsFile,line);
   GmatStringUtil::ToInteger(line,&numfaces);

   #ifdef DEBUG_READ_DATAFILE
      MessageInterface::ShowMessage("   %d faces\n", numfaces);
   #endif

   // 3.2. Read all faces:
   for(Integer i=0; (i < numfaces)&&(!bsFile.eof()); ++i)
   {
	  // get a line from data file:
      getline(bsFile,line);

      // get record's index:
      line = GmatStringUtil::Trim(line, LEADING);
      GmatStringUtil::FindFirstAndLast(line,' ',first,last);
      subline = line.substr(0,first);
      GmatStringUtil::ToInteger(subline, &index);
      line = line.substr(first, line.size()-first);

      // get the index of the first vertex of triangle :
      line = GmatStringUtil::Trim(line, LEADING);
      GmatStringUtil::FindFirstAndLast(line,' ',first,last);
      subline = line.substr(0,first);
      GmatStringUtil::ToInteger(subline, &ix);
      line = line.substr(first, line.size()-first);

      // get the index of the second vertex of triangle :
      line = GmatStringUtil::Trim(line, LEADING);
      GmatStringUtil::FindFirstAndLast(line,' ',first,last);
      subline = line.substr(0,first);
      GmatStringUtil::ToInteger(subline, &iy);
      line = line.substr(first, line.size()-first);

      // get the index of the third vertex of triangle :
      line = GmatStringUtil::Trim(line, LEADING);
      GmatStringUtil::FindFirstAndLast(line,' ',first,last);
      subline = line.substr(0,first);
      GmatStringUtil::ToInteger(subline, &iz);

	  // define a triangular face:
	  triangularface.push_back(ix-1);			// GMAT index starts from 0; MatLab index starts from 1
	  triangularface.push_back(iy-1);			// GMAT index starts from 0; MatLab index starts from 1
	  triangularface.push_back(iz-1);			// GMAT index starts from 0; MatLab index starts from 1

	  // add the triangular face into facesList:
	  facesList.push_back(triangularface);
     triangularface.clear();
   }

   #ifdef DEBUG_READ_DATAFILE
      MessageInterface::ShowMessage("   %d loaded\n", facesList.size());
   #endif

   if (bsFile.eof())
      throw UtilityException("Error: missing data in file: " +
                             bodyShapeFilename);

   // 4. Close data file:
   if (bsFile.is_open())  bsFile.close();

   isLoad = true;

   #ifdef DEBUG_READ_DATAFILE
      MessageInterface::ShowMessage("number of faces = %d\n", facesList.size());
      for (UnsignedInt i = 0; i < facesList.size(); ++i)
         MessageInterface::ShowMessage("%d:  %d   %d   %d\n", i,
               facesList[i].at(0), facesList[i].at(1), facesList[i].at(2));
      MessageInterface::ShowMessage("\n");
   #endif

   return true;
}


//-------------------------------------------------------------------------------
// bool PolyhedronBody::Incenters()
//-------------------------------------------------------------------------------
/*
 * This function is used to calculate incenters for all triangular faces
*/
//-------------------------------------------------------------------------------
bool PolyhedronBody::Incenters()
{
	PolygonFace face;
   Rvector3 ict;
	Rvector3 AB, BC, CA, A, B, C;

	ic.clear();

	// Create incenters list based on facesList:
	Real a, b, c, p;
	for (UnsignedInt i = 0; i < facesList.size(); ++i)
	{
		// Get indices of triangle ith:
		face = facesList[i];
		A.Set(verticesList[face[0]].Get(0), verticesList[face[0]].Get(1), verticesList[face[0]].Get(2));		// vertex A of triangle ABC
		B.Set(verticesList[face[1]].Get(0), verticesList[face[1]].Get(1), verticesList[face[1]].Get(2));		// vertex B of triangle ABC
		C.Set(verticesList[face[2]].Get(0), verticesList[face[2]].Get(1), verticesList[face[2]].Get(2));		// vertex C of triangle ABC
		AB = B - A;							// vector AB
		BC = C - B;							// vector BC
		CA = A - C;							// vector CA
		a = BC.Norm(); b = CA.Norm(); c = AB.Norm();
		p = a + b + c;

		ict = (a/p)*A + (b/p)*B + (c/p)*C;	// incenter O
		ic.push_back(ict);				// add incenter to the incenters list
	}

#ifdef DEBUG_INCENTERS_CALCULATION
	MessageInterface::ShowMessage("List of incenters:\n");
	for (UnsignedInt i = 0; i < ic.size(); ++i)
	{
		MessageInterface::ShowMessage("%.15lf   %.15lf   %.15lf\n", ic[i][0], ic[i][1], ic[i][2]);
	}
#endif
	return true;
}



//-------------------------------------------------------------------------------
// bool PolyhedronBody::FaceNormals()
//-------------------------------------------------------------------------------
/*
 * This function is used to calculate normals for all faces
*/
//-------------------------------------------------------------------------------
bool PolyhedronBody::FaceNormals()
{
	PolygonFace face;
   Rvector3 n;
	Rvector3 r1, r2, A, B, C;

	fn.clear();

	// Create normal vector list based on facesList:
	Real x, y, z;
	for (UnsignedInt i = 0; i < facesList.size(); ++i)
	{
		// Get indices of triangle ith:
		face = facesList[i];
		A.Set(verticesList[face[0]].Get(0), verticesList[face[0]].Get(1), verticesList[face[0]].Get(2));		// vertex A of triangle ABC
		B.Set(verticesList[face[1]].Get(0), verticesList[face[1]].Get(1), verticesList[face[1]].Get(2));		// vertex B of triangle ABC
		C.Set(verticesList[face[2]].Get(0), verticesList[face[2]].Get(1), verticesList[face[2]].Get(2));		// vertex C of triangle ABC
		r1 = B - A;						// vector AB
		r2 = C - B;						// vector BC
		x = r1[1]*r2[2] - r1[2]*r2[1];
		y = r1[2]*r2[0] - r1[0]*r2[2];
		z = r1[0]*r2[1] - r1[1]*r2[0];
      n.Set(x,y,z);		         // n = AB x BC
		if (n.Norm() < 1.0e-15)
			return false;

		n.Normalize();					// normal vector of triangle ABC
		fn.push_back(n);				// add normal vector to the normal vectors list
	}

   #ifdef DEBUG_FACENORMALS_CALCULATION
      MessageInterface::ShowMessage("List of normal vectors:\n");
      for (UnsignedInt i = 0; i < fn.size(); ++i)
      {
         MessageInterface::ShowMessage("%.15lf   %.15lf   %.15lf\n", fn[i][0], fn[i][1], fn[i][2]);
      }
   #endif

	return true;
}



//-------------------------------------------------------------------------------
// bool PolyhedronBody::IsInEdgesList(Edge& edge, bool& isAttachmentB)
//-------------------------------------------------------------------------------
/*
 * This function is used to verify an edge is in the current edges list
 *
 *  @param  edge           the edge that needs to verify
 *  @param  isAttachmentB  ouput true if the edge is attached to faceB
 *  return true if the edge is in the edges list          
*/
//-------------------------------------------------------------------------------
bool PolyhedronBody::IsInEdgesList(Edge& edge, bool& isAttachmentB)
{
   isAttachmentB = false;

   // Specify edge's indexKey 
   Integer indexKey = (Integer)Min((Real)edge.vertex1,(Real)edge.vertex2)*100000 +
                      (Integer)Max((Real)edge.vertex1,(Real)edge.vertex2);

   for (std::map<Integer, Edge>::iterator i = edgeMap.begin(); i != edgeMap.end(); ++i)
	{ 
      if ((*i).first == indexKey)
      {
         Edge e = (*i).second;

         if (edge.vertex1 == e.vertex2)
            isAttachmentB = true;
         return true;
      }
	}
	return false;
}


//-------------------------------------------------------------------------------
// bool PolyhedronBody::Edges()
//-------------------------------------------------------------------------------
/*
 * This function is used to create edges list and 2 attachment faces to each edge
*/
//-------------------------------------------------------------------------------
bool PolyhedronBody::Edges()
{
   bool isAttachmentB;
   Integer indexKey;

   PolygonFace face;
   Edge e1, e2, e3;

   edgeMap.clear();
   attachmentAMap.clear();
   attachmentBMap.clear();
   E.clear();
   attachmentA.clear();
   attachmentB.clear();

   for(unsigned int i=0; i < facesList.size(); ++i)
   {
      face = facesList[i];

      // For a triangular face <face[0], face[1], face[2]>, it has 3 edges e1, e2, and e3
      e1.vertex1 = face[0]; e1.vertex2 = face[1];
      e2.vertex1 = face[1]; e2.vertex2 = face[2];
      e3.vertex1 = face[2]; e3.vertex2 = face[0];

	   // Add e1 to edges list if it has not existed in edges list:
      indexKey = (Integer)Min((Real)e1.vertex1,(Real)e1.vertex2)*100000 +
                 (Integer)Max((Real)e1.vertex1,(Real)e1.vertex2);
      if (!IsInEdgesList(e1, isAttachmentB))
      {
         edgeMap[indexKey] = e1;
         attachmentAMap[indexKey] = i;                      // add atachment face A
         attachmentBMap[indexKey] = -1;                     // add null value to attachmentB list
      }
      else
      {
         if (isAttachmentB)
            attachmentBMap[indexKey] = i;                   // add attachment to face B
      }

      // Add e2 to edges list if it has not existed in edges list:
      indexKey = (Integer)Min((Real)e2.vertex1,(Real)e2.vertex2)*100000 +
                 (Integer)Max((Real)e2.vertex1,(Real)e2.vertex2);
      if (!IsInEdgesList(e2, isAttachmentB))
      {
         edgeMap[indexKey] = e2;
         attachmentAMap[indexKey] = i;                      // add atachment face A
         attachmentBMap[indexKey] = -1;                     // add null value to attachmentB list
      }
      else
      {
         if (isAttachmentB)
            attachmentBMap[indexKey] = i;                   // add attachment to face B
      }

      // Add e3 to edges list if it has not existed in edges list:
      indexKey = (Integer)Min((Real)e3.vertex1,(Real)e3.vertex2)*100000 +
                 (Integer)Max((Real)e3.vertex1,(Real)e3.vertex2);
      if (!IsInEdgesList(e3, isAttachmentB))
      {
         edgeMap[indexKey] = e3;
         attachmentAMap[indexKey] = i;                      // add atachment face A
         attachmentBMap[indexKey] = -1;                     // add null value to attachmentB list
      }
      else
      {
         if (isAttachmentB)
            attachmentBMap[indexKey] = i;                   // add attachment to face B
      }
   }

   for (std::map<Integer, Edge>::iterator i = edgeMap.begin(); i != edgeMap.end(); ++i)
   {
      E.push_back((*i).second);
   }

   for (std::map<Integer, Integer>::iterator i = attachmentAMap.begin(); i != attachmentAMap.end(); ++i)
   {
      attachmentA.push_back((*i).second);
   }

   for (std::map<Integer, Integer>::iterator i = attachmentBMap.begin(); i != attachmentBMap.end(); ++i)
   {
      attachmentB.push_back((*i).second);
   }
   

#ifdef DEBUG_CALCULATION
   MessageInterface::ShowMessage(" Edges list and face attachment A and B:\n");
   for(UnsignedInt i=0; i < E.size(); ++i)
   {
      MessageInterface::ShowMessage("%d  %d :  %d    %d    %d    %d\n", min(E[i].vertex1, E[i].vertex2), max(E[i].vertex1, E[i].vertex2), E[i].vertex1, E[i].vertex2, attachmentA[i], attachmentB[i]);
   }
#endif
   return true;
}


//------------------------------------------------------------------------------
// bool PolyhedronBody::EdgeAttachments(Integer edgeindex,
//                  Integer& faceA_index, Integer& faceB_index)
//------------------------------------------------------------------------------
/*
 * This function is used to get 2 faces attached to a given edge
 *
 *  @param edgeindex       index of a given edge
 *  @param faceA_index     index of the first face of the edge
 *  @param faceB_index     index of the second face of the edge
 *
*/
//------------------------------------------------------------------------------
bool PolyhedronBody::EdgeAttachments(Integer edgeindex, Integer& faceA_index, Integer& faceB_index)
{
   faceA_index = attachmentA[edgeindex];
   faceB_index = attachmentB[edgeindex];
   return true;
}


