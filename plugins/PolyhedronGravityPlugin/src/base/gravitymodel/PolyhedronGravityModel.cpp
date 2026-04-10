// PolyhedronGravityModel.cpp
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
//  Created on: Jul 16, 2012
//      Author: tdnguye2

#include <iostream>
#include <fstream>
#include <sstream>
#include "UtilityException.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"

#include "PolyhedronGravityModel.hpp"
#include "Rmatrix33.hpp"
#include "Rmatrix66.hpp"

//#define DEBUG_FIRST_CALL
//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZE
//#define DEBUG_CALCULATION
//#define DEBUG_DERIVATIVE
//#define DEBUG_REGISTRATION
//#define DEBUG_SOLID_ANGLE


using namespace GmatMathUtil;
using namespace GmatStringUtil;

//---------------------------------
// static data
//---------------------------------

#ifdef DEBUG_FIRST_CALL
static bool firstCallFired = false;
#endif

const std::string
PolyhedronGravityModel::PARAMETER_TEXT[PolyhedronGravityModelParamCount - GravityBaseParamCount] =
{
   "CreateForceBody",
   "ShapeFileName",
   "BodyDensity",
};

const Gmat::ParameterType
PolyhedronGravityModel::PARAMETER_TYPE[PolyhedronGravityModelParamCount - GravityBaseParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
};

// const Real 	PolyhedronGravityModel::UniverisalGravityConstant = 6.67300e-20;    // unit: Km^3 / (Kg x s^2)

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

PolyhedronGravityModel::PolyhedronGravityModel(const std::string &name):
   GravityBase           	 ("PolyhedronGravityModel", name),
   firstcalculation         (true),
   createForceBodyName      (""),
   createForceBody          (NULL),
   bodyDensity				    (1000.0),
   bodyShapeFilename        (""),
   polybody                 (NULL),
   sumWf                    (0.0),
   isPHGMInitialized        (false),
   isShapeLoaded            (false)
{
   #ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("PolyhedronGravityModel default construction <%p>\n", this);
   #endif
   objectTypeNames.push_back("PolyhedronGravityModel");
   dimension = 6;										// 6 * satCount;
   
   parameterCount = PolyhedronGravityModelParamCount;
}


PolyhedronGravityModel::~PolyhedronGravityModel()
{
   #ifdef DEBUG_CONSTRUCTION
	  MessageInterface::ShowMessage("PolyhedronGravityModel destruction <%p>\n", this);
   #endif

   if (polybody)
      delete polybody;
}


PolyhedronGravityModel::PolyhedronGravityModel(const PolyhedronGravityModel& polgm):
   GravityBase			   (polgm),
   firstcalculation     (true),
   createForceBodyName  (polgm.createForceBodyName),
   createForceBody      (polgm.createForceBody),
   bodyDensity				(polgm.bodyDensity),
   bodyShapeFilename    (polgm.bodyShapeFilename),
   polybody					(NULL),
   sumWf                (0.0),
   isPHGMInitialized    (false),
   isShapeLoaded        (false)
{
   #ifdef DEBUG_CONSTRUCTION
	  MessageInterface::ShowMessage("PolyhedronGravityModel copy construction <%p>\n", this);
   #endif
   objectTypeNames.push_back("PolyhedronGravityModel");

   if (polgm.polybody)
      polybody = polgm.polybody->Clone();

   parameterCount = PolyhedronGravityModelParamCount;
}


PolyhedronGravityModel& PolyhedronGravityModel::operator= (const PolyhedronGravityModel& polgm)
{
   #ifdef DEBUG_CONSTRUCTION
	  MessageInterface::ShowMessage("PolyhedronGravityModel operator =  <%p>\n", this);
   #endif
   if (&polgm == this)
      return *this;

   GravityBase::operator=(polgm);
   firstcalculation     = true;
   createForceBodyName	= polgm.createForceBodyName;
   createForceBody		= polgm.createForceBody;
   bodyDensity			   = polgm.bodyDensity;
   bodyShapeFilename	   = polgm.bodyShapeFilename;
   isPHGMInitialized    = false;
   isShapeLoaded        = false;
   sumWf                = 0.0;

   if(polybody)
      delete polybody;
   polybody				   = NULL;
   if (polgm.polybody)
      polybody = polgm.polybody->Clone();

   return *this;
}


bool PolyhedronGravityModel::Initialize()
{
   if (isPHGMInitialized)
      return true;

   #ifdef DEBUG_INITIALIZE
      MessageInterface::ShowMessage("PolyhedronGravityModel initialize\n");
   #endif

   bool retval = GravityBase::Initialize();

   isPHGMInitialized = false;
   isInitialized = true;
   if (retval)
   {
      if (solarSystem != NULL)
      {
		   #ifdef DEBUG_INITIALIZE
            MessageInterface::ShowMessage(
				"PolyhedronGravityModel::Initialize():  GravityBase::createForceBodyName = '%s'\n", createForceBodyName.c_str());
         #endif

         createForceBody =  solarSystem->GetBody(createForceBodyName);

         if (createForceBody != NULL)
         {
 		    #ifdef DEBUG_INITIALIZE
               MessageInterface::ShowMessage(
				   "PolyhedronGravityModel::Initialize():  GravityBase::createForceBody = %p\n", createForceBody);
            #endif
            if (polybody == NULL)
            {
			      // create a PolyhedronBody object for the asteroid in order to calculate gravity at spacecrafts' locations:
               polybody = new PolyhedronBody(bodyShapeFilename);
               retval = polybody->Initialize();								// initialize() will load body shape information from data file
               firstcalculation = true;
            }
            isPHGMInitialized = true;
         }
         else
         {
            //MessageInterface::ShowMessage(
            //   "PolyhedronGravityModel::Initialize() createForceBody \"%s\" is not in the solar "
            //   "system\n", createForceBodyName.c_str());

            isInitialized = false;
            //throw ODEModelException("PolyhedronGravityModel::Initialize() createForceBody \"" + 
            throw ODEModelException("Error: Celestial body \"" + 
               createForceBodyName + "\" is not in the solar system.\n");
         }
      }
      else
      {
         MessageInterface::ShowMessage(
            "PolyhedronGravityModel::Initialize() solarSystem is NULL\n");
         isInitialized = false;
         throw ODEModelException(
            "PolyhedronGravityModel::Initialize() solarSystem is NULL\n");
      }
   }
   else
      isInitialized = false;

   return retval;
}


GmatBase* PolyhedronGravityModel::Clone() const
{
   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("PolyhedronGravityModel clone\n");
   #endif


   return (new PolyhedronGravityModel(*this));
}


void PolyhedronGravityModel::Copy(const GmatBase* orig)
{
   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("PolyhedronGravityModel copy\n");
   #endif

   operator=(*((PolyhedronGravityModel *)(orig)));
}


bool PolyhedronGravityModel::HasLocalClones()
{
	return true;
}

bool PolyhedronGravityModel::IsUserForce()
{
	return true;
}

bool PolyhedronGravityModel::RenameRefObject(const UnsignedInt type,
                                     const std::string &oldName,
                                     const std::string &newName)
{
	return true;
}


// set density for the body
bool PolyhedronGravityModel::SetDensity(Real density)
{
	bodyDensity = density;
	return true;
}


// get density of the body
Real PolyhedronGravityModel::GetDensity()
{
	return bodyDensity;
}


// set filename for bodyShapeFilename
bool PolyhedronGravityModel::SetBodyShapeFileName(const std::string &filename)
{
	bodyShapeFilename = filename;
	return true;
}


// get filename of the file containing information about shape of the body
std::string PolyhedronGravityModel::GetBodyShapeFileName()
{
	return bodyShapeFilename;
}



// Specify coordinate transformation matrix:8
Rmatrix33 PolyhedronGravityModel::CalculateTransformationMatrix_UsingIAUSimplified() const
{
   //   w = options.w;
   //   PRA = options.PRA;
   //   RA = options.RA;
   //   DEC = options.DEC;

   Real pi = GmatMathConstants::PI;

   Real t0	= initialtime;						// unit: day  
   Real t	= now -t0;							// unit: day
   Real w	= (bodyOrientation[5]*pi/180);		// unit: radian per day
   Real PRA = bodyOrientation[4]*pi/180;		// unit: radian
   Real RA	= bodyOrientation[0]*pi/180;		// unit: radian
   Real DEC = bodyOrientation[2]*pi/180;		// unit: radian
   Real theta = PRA + w*t;						// unit: radian

   Rmatrix33 D3w(cos(-theta), sin(-theta), 0.0,
         -sin(-theta), cos(-theta), 0.0,
         0.0, 0.0, 1.0);
   
   Rmatrix33 D1(1.0, 0.0, 0.0,
         0.0, cos(DEC-pi/2), sin(DEC-pi/2),
         0.0, -sin(DEC-pi/2), cos(DEC-pi/2));

   Rmatrix33 D3(cos(-pi/2-RA), sin(-pi/2-RA), 0.0,
         -sin(-pi/2-RA), cos(-pi/2-RA), 0.0,
         0.0, 0.0, 1.0);

   Rmatrix33 C_IB = D3*D1*D3w;
   Rmatrix33 D = C_IB.Transpose();

   #ifdef DEBUG_CALCULATION
      MessageInterface::ShowMessage("now = %f\n", now); 
      MessageInterface::ShowMessage("t = %f day,   w = %f rad/day,   PRA = %f rad,   theta =%f rad\n", t, w, PRA, theta);
      MessageInterface::ShowMessage("RA = %f rad,   DEC = %frad\n", RA, DEC);
      MessageInterface::ShowMessage("D = [\n");
      for (int i = 0; i < 3; ++i)
      {
	     MessageInterface::ShowMessage("        ");
         for (int j = 0; j < 3; ++j)
         {
    	    MessageInterface::ShowMessage("%le  ",D(i,j));
         }
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("          ]\n");
   #endif
   
   return D;
}

const std::vector<Rmatrix33> PolyhedronGravityModel::CalculateTransformationMatrix() const
{
   // create MJ2000Eq coordinate system:
   CoordinateSystem* bodyFK5 = CoordinateSystem::CreateLocalCoordinateSystem("BodyFK5", "MJ2000Eq", body,NULL, NULL, body, solarSystem);
   
   // create BodyFixed coordinate system:
   CoordinateSystem* bodyBodyFixed = CoordinateSystem::CreateLocalCoordinateSystem("BodyBodyFixed", "BodyFixed", body, NULL, NULL, body, solarSystem);
   
   // specify rotation matrix:
   Rvector inState(6, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
   Rvector outState(6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   CoordinateConverter* cv = new CoordinateConverter();
   A1Mjd time(now);
   cv->Convert(time, inState, bodyFK5, outState, bodyBodyFixed);
   
   std::vector<Rmatrix33> transformMatrix;

   Rmatrix33 D = cv->GetLastRotationMatrix();
   transformMatrix.push_back(D);

   Rmatrix33 Ddot = cv->GetLastRotationDotMatrix();
   transformMatrix.push_back(D);

   // Clean up memory
   if (cv)
     delete cv;

   if (bodyFK5)
      delete bodyFK5;

   if (bodyBodyFixed)
      delete bodyBodyFixed;

   #ifdef DEBUG_CALCULATION
      MessageInterface::ShowMessage("time = %f day\n",time.GetReal());
      MessageInterface::ShowMessage("D = [\n");
      for (int i = 0; i < 3; ++i)
      {
	     MessageInterface::ShowMessage("        ");
         for (int j = 0; j < 3; ++j)
         {
    	    MessageInterface::ShowMessage("%le  ",D(i,j));
         }
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("          ]\n");

      MessageInterface::ShowMessage("Ddot = [\n");
      for (int i = 0; i < 3; ++i)
      {
	     MessageInterface::ShowMessage("        ");
         for (int j = 0; j < 3; ++j)
         {
    	    MessageInterface::ShowMessage("%le  ",Ddot(i,j));
         }
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("          ]\n");
   #endif
   
   //return D;
   return transformMatrix;
}

//-------------------------------------------------------------------------------
// bool PolyhedronGravityModel::Calculation(Rvector6 x, Rvector6& xdot, Rmatrix66& M)
//-------------------------------------------------------------------------------
/*
 * This function is used to calculate gravity at a given location
*/
//-------------------------------------------------------------------------------
bool PolyhedronGravityModel::Calculation(Rvector6 x, Rvector6& xdot, Rmatrix66& M)
{
   //MessageInterface::ShowMessage("Calculation... \n");
   // Get r and v:
   Rvector3 r,v;
   r.Set(x(0), x(1), x(2));
   v.Set(x(3), x(4), x(5));
#ifdef DEBUG_CALCULATION
   MessageInterface::ShowMessage("Position and velocity of spacecraft in asteroid's MJ2000Eq coordinate system:\n");  
   MessageInterface::ShowMessage("r = (%.15lf   %.15lf   %.15lf) km\n", r(0), r(1), r(2));
   MessageInterface::ShowMessage("v = (%.15lf   %.15lf   %.15lf) km/s\n", v(0), v(1), v(2));
#endif

   // Define variables:
   Rvector3 sumEdge(0.0, 0.0, 0.0);
   Rvector3 sumFace(0.0, 0.0, 0.0);
   Rmatrix33 sumEdgeA(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   Rmatrix33 sumFaceA(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   sumWf = 0.0;

   // Calculate face normals, edges, and vertices
   polybody->LoadBodyShape();
   if (firstcalculation)
   {
      //MessageInterface::ShowMessage(" Compute face normals, incenters, and edges\n");
      polybody->FaceNormals();
      polybody->Incenters();
      polybody->Edges();
	   firstcalculation = false;
   }

#ifdef DEBUG_CALCULATION
   MessageInterface::ShowMessage("edge size = %d     face size = %d\n", polybody->E.size(), polybody->facesList.size());
#endif
   // Specify coordinate transformation matrix:
   std::vector<Rmatrix33> transform = CalculateTransformationMatrix();
   Rmatrix33 D = transform[0];
   Rmatrix33 Ddot = transform[1];
   //Rmatrix33 D = CalculateTransformationMatrix_UsingIAUSimplified();
   r = D*r;                            // position in BodyFixed coordinate system
   Rvector3 v1 = Ddot*r + D*v;         // velocity in BodyFixed coordinate system

#ifdef DEBUG_CALCULATION
   MessageInterface::ShowMessage("Position and velocity of spacecraft in asteroid's BodyFixed coordinate system:\n");
   MessageInterface::ShowMessage("r = R*r          = (%.15lf   %.15lf   %.15lf)km \n", r(0), r(1), r(2));
   MessageInterface::ShowMessage("v = Rdot*r + R*v = (%.15lf   %.15lf   %.15lf)km/s \n", v1(0), v1(1), v1(2));
#endif

   // Sum the gravitational contributions of each edge:
   Rvector3 P1, P2, P1P2, n12, n21;
   Edge edge;
   Integer face1, face2;
   Rvector3 na, nb;
   Rvector3 na12, nb21;
   //Rvector3 a2b;
   Rmatrix33 Ee;
   Rvector3 re, R1, R2;
   Real e, r1, r2;
   Real Le;

   Rvector3 a2v, b2v;

   for (Integer i = polybody->E.size()-1; i >= 0; --i)
   {
      edge.vertex1 = polybody->E[i].vertex1;
      edge.vertex2 = polybody->E[i].vertex2;
      //MessageInterface::ShowMessage("Edge: %d   %d:  %d   %d   ", min(edge.vertex1,edge.vertex2), max(edge.vertex1,edge.vertex2), edge.vertex1, edge.vertex2);

      // Define edge unit vectors:
	   P1 = polybody->verticesList[edge.vertex1];					// the first vertex of edge e
	   P2 = polybody->verticesList[edge.vertex2];					// the second vertex of edge e
	   P1P2 = P2-P1;
	   n12 = P1P2; n12 = n12.Normalize();
	   n21 = -n12;
      //MessageInterface::ShowMessage("Normal vector n12: (%.15lf   %.15lf   %.15lf):   ", n12[0], n12[1], n12[2]); 

	  // Specify the attached faces:
	  // EdgeAttachments1(edge, face1, face2);
	  polybody->EdgeAttachments(i, face1, face2);
     //MessageInterface::ShowMessage("attachment face: %d   %d:   ", face1, face2); 
	  // Specify outward facing normals:
	  na = polybody->fn[face1];
	  nb = polybody->fn[face2];
     //MessageInterface::ShowMessage("na: (%.15lf  %.15lf  %.15lf)   ", na[0], na[1], na[2]);
     //MessageInterface::ShowMessage("nb: (%.15lf  %.15lf  %.15lf)   ", nb[0], nb[1], nb[2]);

	  // Calculate edge normal vectors:
//    na12 = [0 -n12(3) n12(2); n12(3) 0 -n12(1); -n12(2) n12(1) 0]*na;
//    nb21 = [0 -n21(3) n21(2); n21(3) 0 -n21(1); -n21(2) n21(1) 0]*nb;
      na12.Set(-n12(2)*na(1) + n12(1)*na(2),
    		    n12(2)*na(0) - n12(0)*na(2),
    		   -n12(1)*na(0) + n12(0)*na(1));
      nb21.Set(-n21(2)*nb(1) + n21(1)*nb(2),
    		    n21(2)*nb(0) - n21(0)*nb(2),
    		   -n21(1)*nb(0) + n21(0)*nb(1));
//	  MessageInterface::ShowMessage("face1 = %d: na12 = n12 x na = (%le,  %le,  %le)\n", face1, na12(0), na12(1), na12(2));
//	  MessageInterface::ShowMessage("face2 = %d: nb21 = n21 x nb = (%le,  %le,  %le)\n", face2, nb21(0), nb21(1), nb21(2));

	  // Ensure outward-pointing edge normals:
//      a2b = ici(2,:)-ici(1,:);                           // old MatLab code
     // a2b = polybody->ic[face2] - polybody->ic[face1];

     // if (a2b*na12<0.0)
     //     na12 = -na12;

     // if (-a2b*nb21<0.0)
     //     nb21 = -nb21;

      // New normal check
      // a2v = V(E(i,1),:)-ici(1,:);      // MatLab code
      a2v = P1 - polybody->ic[face1];
      if (a2v*na12<0.0)
         na12 = -na12;

      // b2v = V(E(i,1),:)-ici(2,:);     // MatLab code
      b2v = P1 - polybody->ic[face2];
      if (b2v*nb21<0.0)
           nb21 = -nb21;

//      MessageInterface::ShowMessage("After adjustment:\n");
//	  MessageInterface::ShowMessage("face1 = %d: na12 = n12 x na = (%le,  %le,  %le)\n", face1, na12(0), na12(1), na12(2));
//	  MessageInterface::ShowMessage("face2 = %d: nb21 = n21 x nb = (%le,  %le,  %le)\n", face2, nb21(0), nb21(1), nb21(2));

//      Ee = na*(na12') + nb*(nb21');
      Ee.Set(na(0)*na12(0)+nb(0)*nb21(0), na(0)*na12(1)+nb(0)*nb21(1), na(0)*na12(2)+nb(0)*nb21(2),
    		 na(1)*na12(0)+nb(1)*nb21(0), na(1)*na12(1)+nb(1)*nb21(1), na(1)*na12(2)+nb(1)*nb21(2),
    		 na(2)*na12(0)+nb(2)*nb21(0), na(2)*na12(1)+nb(2)*nb21(1), na(2)*na12(2)+nb(2)*nb21(2));
//	  MessageInterface::ShowMessage("Ee = (%le,  %le,  %le,\n", Ee(0,0), Ee(0,1), Ee(0,2));
//	  MessageInterface::ShowMessage("      %le,  %le,  %le,\n", Ee(1,0), Ee(1,1), Ee(1,2));
//	  MessageInterface::ShowMessage("      %le,  %le,  %le)\n", Ee(2,0), Ee(2,1), Ee(2,2));

	  // Vector from the field point to the edge:
//    re = V(E(i,1),:)'-r;
      re = polybody->verticesList[edge.vertex1] - r;
//      MessageInterface::ShowMessage("re = (%le,  %le,  %le)\n", re(0), re(1), re(2));

	  // Edge length:
      e = P1P2.Norm();
//      MessageInterface::ShowMessage("Edge length P1P2 = %le\n", e);

	  // Vectors from the field point to the edge endpoints:
//      R1 = V(E(i,1),:)'-r;
//      R2 = V(E(i,2),:)'-r;
      R1 = polybody->verticesList[edge.vertex1] - r;
      R2 = polybody->verticesList[edge.vertex2] - r;
      r1 = R1.Norm();
      r2 = R2.Norm();

	  // Calculate the logarithm expression, Le:
      Le = GmatMathUtil::Ln((r1+r2+e)/(r1+r2-e));
//      MessageInterface::ShowMessage("Le = %le\n", Le);

	  // Sum the edge gravity contributions:
      sumEdge = sumEdge + Ee*re*Le;
      //Rvector3 res = Ee*re*Le;
      //MessageInterface::ShowMessage("EdgeGravity: (%.15le   %.15le   %.15le)  ", res[0], res[1], res[2]);
      //MessageInterface::ShowMessage("SumEdgeGravity: (%.15le   %.15le   %.15le)\n", sumEdge[0], sumEdge[1], sumEdge[2]);

	  // Calculate variational terms (if requested):
//      if nargout > 1
          sumEdgeA = sumEdgeA + Ee*Le;
//      end

   }

#ifdef DEBUG_CALCULATION
   MessageInterface::ShowMessage("sumEdge = [ %.15le  %.15le  %.15le ]\n", sumEdge(0), sumEdge(1), sumEdge(2));
   MessageInterface::ShowMessage("sumEdgeA = [\n");
   for (int i = 0; i < 3; ++i)
   {
	  MessageInterface::ShowMessage("        ");
      for (int j = 0; j < 3; ++j)
      {
    	  MessageInterface::ShowMessage("%le  ",sumEdgeA(i,j));
      }
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("          ]\n");
#endif

   Rmatrix33 Ff;
   Rvector3 A, B, C, R3, cR23, n;
   Real r3, wf;

   for (Integer i = polybody->facesList.size()-1; i >= 0; --i)
   {
	  // Ff = (fn(i,:)')*(fn(i,:)')';
	  n = polybody->fn[i];
	  Ff.Set(n(0)*n(0), n(0)*n(1), n(0)*n(2),
			 n(1)*n(0), n(1)*n(1), n(1)*n(2),
			 n(2)*n(0), n(2)*n(1), n(2)*n(2));

	  // face vertices:
	  PolygonFace face = polybody->facesList[i];
	  A = polybody->verticesList[face[0]];
	  B = polybody->verticesList[face[1]];
	  C = polybody->verticesList[face[2]];

	  // Calculate vectors from field point to face vertices:
	  R1 = A-r;
	  R2 = B-r;
	  R3 = C-r;

	  r1 = R1.Norm();
	  r2 = R2.Norm();
	  r3 = R3.Norm();

	  cR23.Set(-R2(2)*R3(1) + R2(1)*R3(2),
			    R2(2)*R3(0) - R2(0)*R3(2),
			   -R2(1)*R3(0) + R2(0)*R3(1));

	  // Calculate the solid angle term, wf:
	  wf = 2*atan2(R1*cR23,
	        (r1*r2*r3+r1*R2*R3+r2*R3*R1+r3*R1*R2));

	  // Sum the face gravity contributions
	  sumFace = sumFace + Ff*R1*wf;
	  sumWf = sumWf + wf;

	  // Calculate variational terms (if requested)
//	    if nargout > 1
	  sumFaceA = sumFaceA + Ff*wf;
//	    end

   }
#ifdef DEBUG_CALCULATION
   MessageInterface::ShowMessage("sumFace = [ %.15le  %.15le  %.15le ]\n", sumFace(0), sumFace(1), sumFace(2));
   MessageInterface::ShowMessage("sumFaceA = [\n");
   for (int i = 0; i < 3; ++i)
   {
	  MessageInterface::ShowMessage("        ");
      for (int j = 0; j < 3; ++j)
      {
    	  MessageInterface::ShowMessage("%le  ",sumFaceA(i,j));
      }
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("          ]\n");
   MessageInterface::ShowMessage("sumWf = %le\n", sumWf);
#endif

   // Use the Laplacian to check if the field point is inside the polyhedron
//   if (GmatMathUtil::Abs(sumWf-4*pi) < 1.0e-6)
//       MessageInterface::ShowMessage("polydyn:InsidePoly','Field point is inside the polyhedron\n");

   // Acceleration at field point
   // bodyDensity's unit: kg/(m^3)     
   // Universal gravity constant's unit: km^3 / (kg x s^2)
   // if body shape is measured in km, then unit of a is km/s^2 
   Rvector3 a = (GmatPhysicalConstants::UNIVERSAL_GRAVITATIONAL_CONSTANT * 1.0e9 * bodyDensity) * (-sumEdge + sumFace);   // gravity vector in asteroid's BodyFixed coordinate system
#ifdef DEBUG_CALCULATION
   MessageInterface::ShowMessage("gravity vector in asteroid's BodyFixed coordinate system: g = (%.15le  %.15le  %.15le) km/s^2\n", a[0], a[1], a[2]); 
#endif

   Rvector3 aa = D.Transpose()*a;             // unit of acceleration aa is km/s^2                                        // gravity vector in asteroid's MJ2000Eq coordinate system

   // State vector derivative
   // Note that: 
   //    1. velocity v is in asteroid's MJ2000Eq
   //    2. assumption for spacecraft: only asteroid's gravity force applies to spacecraft  
   xdot.Set(v(0), v(1), v(2), aa(0), aa(1), aa(2));

   // Variational terms:
//   A = [zeros(3,3) eye(3,3);
//            D'*G*rho*(sumEdgeA - sumFaceA)*D zeros(3,3)];
   Rmatrix33 m1 = D.Transpose() * (GmatPhysicalConstants::UNIVERSAL_GRAVITATIONAL_CONSTANT * 1.0e9 *bodyDensity) * (sumEdgeA - sumFaceA)*D;

   for (int i = 0; i < 6; ++i)
	   for (int j = 0; j < 6; ++j)
		   M(i,j) = 0.0;
   M(0,3) = 1.0; M(1,4) = 1.0; M(2,5) = 1.0;
   for (int i = 1; i < 3; ++i)
	   for (int j = 0; j < 3; ++j)
		   M(i+3,j) = m1(i,j);

   return true;
}




//------------------------------------------------------------------------------
// Integer GetParameterCount() const
//------------------------------------------------------------------------------
Integer PolyhedronGravityModel::GetParameterCount() const
{
   return parameterCount;
}


// Access methods
//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string PolyhedronGravityModel::GetParameterText(const Integer id) const
{
   if (id >= GravityBaseParamCount && id < PolyhedronGravityModelParamCount)
	  return PARAMETER_TEXT[id - GravityBaseParamCount];
   else
      return GravityBase::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer PolyhedronGravityModel::GetParameterID(const std::string &str) const
{
   std::string alias = str;

   // Script document required two different names for the primary body
   // force descriptor
   if (alias == "Gravity")
      alias = "PrimaryBodies";

   for (int i = GravityBaseParamCount; i < PolyhedronGravityModelParamCount; i++)
   {
      if (alias == PARAMETER_TEXT[i - GravityBaseParamCount])
      {
         return i;
      }
   }

   return GravityBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType PolyhedronGravityModel::GetParameterType(const Integer id) const
{
   if (id >= GravityBaseParamCount && id < PolyhedronGravityModelParamCount)
      return PARAMETER_TYPE[id - GravityBaseParamCount];
   else
      return GravityBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string PolyhedronGravityModel::GetParameterTypeString(const Integer id) const
{
   if (id >= GravityBaseParamCount && id < PolyhedronGravityModelParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GravityBase::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
bool PolyhedronGravityModel::IsParameterReadOnly(const Integer id) const
{
   return GravityBase::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
bool PolyhedronGravityModel::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string PolyhedronGravityModel::GetStringParameter(const Integer id) const
{
   switch (id)
   {
      case CREATE_FORCE_BODY:
         return createForceBodyName;
      case SHAPE_FILENAME:
         return bodyShapeFilename;

      default:
         return GravityBase::GetStringParameter(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string PolyhedronGravityModel::GetStringParameter(const std::string &label) const
{
    return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool PolyhedronGravityModel::SetStringParameter(const Integer id, const std::string &value)
{

   switch (id)
   {
      case CREATE_FORCE_BODY:
         createForceBodyName = value;
		 this->SetBodyName(value);						// set theBody is the createForceBody
		 return true;
      case SHAPE_FILENAME:
         bodyShapeFilename = value;
         return true;
      default:
         return GravityBase::SetStringParameter(id, value);
    }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,const std::string &value)
//------------------------------------------------------------------------------
bool PolyhedronGravityModel::SetStringParameter(const std::string &label,
                                    const std::string &value)
{
    return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param id    Integer ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real PolyhedronGravityModel::GetRealParameter(const Integer id) const
{
   if (id == BODY_DENSITY)   return bodyDensity;                // unit: kg/m^3

   return GravityBase::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    id  Integer ID for the parameter
 * @param    val The new value for the parameter
 */
//------------------------------------------------------------------------------
Real PolyhedronGravityModel::SetRealParameter(const Integer id,
                                           const Real value)
{
   if (id == BODY_DENSITY)       return (bodyDensity = value);      // unit: kg/m^3

   return GravityBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Accessor method used to obtain a parameter value
 *
 * @param label    string ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real PolyhedronGravityModel::GetRealParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    label    string ID for the requested parameter
 * @param    value    The new value for the parameter
 */
//------------------------------------------------------------------------------
Real PolyhedronGravityModel::SetRealParameter(const std::string &label,
                                    const Real value)
{
   Integer id = GetParameterID(label);
   return SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// bool PointMassForce::GetDerivatives(Real * state, Real dt, Integer order)
//------------------------------------------------------------------------------
/**
 * Method used to apply the point mass force model to the state
 * 
 * This method sets the first derivative for 1 or more spacecraft.  The model
 * applies point mass gravity, with the gravitating point located at position
 * \f$(0, 0, 0)\f$.  A later implementation may allow for locations other than 
 * the origin.
 * 
 * The first derivatives map as follows:
 * 
 * \f[ {{d}\over{dt}} \pmatrix{\vec r \cr\vec v\cr} = 
 *     \pmatrix{\vec v \cr -{{GM\vec r}\over{r^3}}\cr} \f] 
 * 
 * @param state  The current state vector of a spacecraft (It is a 6-elemnets array) in MJ2000Eq coordinate system
 * @param dt     The time offset for the calculations
 * @param order  Order of the derivative being calculated
 */
//------------------------------------------------------------------------------
bool PolyhedronGravityModel::GetDerivatives(Real * state, Real dt, Integer order, 
      const Integer id)
{
   #ifdef DEBUG_DERIVATIVE
	MessageInterface::ShowMessage("----------------------------------------------------------------------------------\n");
	MessageInterface::ShowMessage("Start PolyhedronGravityModel::GetDerivatives()\n\n");

      MessageInterface::ShowMessage("Evaluating Polyhedron gravity model; "
         "state pointer = %p, time offset = %lf, order = %d, id = %d "
		 "epoch = %lf,  time = %lf\n", state, dt, order, id, theState->GetEpoch(), theState->GetEpoch()+dt/GmatTimeConstants::SECS_PER_DAY);
	  MessageInterface::ShowMessage("forceOrigin(%p) name = '%s'\n", forceOrigin, forceOrigin->GetName().c_str());
	  MessageInterface::ShowMessage("body(%p) name = '%s'\n", body, body->GetName().c_str());
   #endif

   if (fillCartesian || fillSTM || fillAMatrix)
   {
      if (order > 2)
         return false;
   
      if ((state == NULL) || (deriv == NULL) || (theState == NULL))
         // throw("Arrays not yet initialized -- exiting");
         return false;

	   // Get current location and orientation of the body generating gravity field: 
      if (theState->HasPrecisionTime())
         epoch = theState->GetEpochGT().GetMjd();
      else
         epoch = theState->GetEpoch();

      now = epoch + dt/GmatTimeConstants::SECS_PER_DAY;
	   bodyState = createForceBody->GetState(now);
	   bodyOrientation = createForceBody->GetOrientationParameters();

      #ifdef DEBUG_DERIVATIVE
	   MessageInterface::ShowMessage("orientation = %f,  %f,  %f,  %f,  %f,  %f\n", bodyOrientation[0], bodyOrientation[1], bodyOrientation[2], bodyOrientation[3], bodyOrientation[4], bodyOrientation[5]);
      #endif 

	   initialtime = createForceBody->GetRealParameter(createForceBody->GetParameterID("OrientationEpoch"));
   
	  // state of spacecraft in the body (asteroid) MJ2000Eq coordinate system:
	  Rvector6 x(state);					// notice that the input spacecraft state is the spacecarft state w/r/t the body				// spacecraftState - bodyState;

	  #ifdef DEBUG_DERIVATIVE
	     MessageInterface::ShowMessage("\n\n epoch = %lf,   dt = %lf,   now = epoch+dt = %lf  : unit = Julian day\n", epoch, dt/GmatTimeConstants::SECS_PER_DAY, now);
//		 MessageInterface::ShowMessage(" spacecraft state                      =   %lf,   %lf,   %lf,   %lf,   %lf,   %lf\n", state[0], state[1], state[2], state[3], state[4], state[5]);
//		 MessageInterface::ShowMessage(" state of the body ('%s')          =   %lf,   %lf,   %lf,   %lf,   %lf,   %lf\n", createForceBody->GetName().c_str(), bodyState[0], bodyState[1], bodyState[2], bodyState[3], bodyState[4], bodyState[5]);
		 MessageInterface::ShowMessage(" state of spacecraft in the bodyMJ2000Eq: x =   %le km,   %le km,   %le km,   %le km/s,   %le km/s,   %le km/s\n\n", x[0], x[1], x[2], x[3], x[4], x[5]);
      #endif

      // calculate xdot and M matrix:
	  Rvector6 xdot;
	  Rmatrix66 M;
	  
     Calculation(x, xdot, M);                // unit for x is km for position and km/s for velocity;    unit for xdot is Km/s and km/s^2
	  
	  for (int i=0; i < 6; ++i)
	     deriv[i] = xdot.GetElement(i);

     #ifdef DEBUG_DERIVATIVE
         MessageInterface::ShowMessage("\n xdot in the bodyMJ2000Eq =   %le km/s,   %le km/s,   %le km/s,   %le km/s^2,   %le km/s^2,   %le km/s^2\n\n", xdot[0], xdot[1], xdot[2], xdot[3], xdot[4], xdot[5]);
         MessageInterface::ShowMessage("End PolyhedronGravityModel::GetDerivatives()\n");
         MessageInterface::ShowMessage("----------------------------------------------------------------------------------\n");
     #endif
   }

   return true;
}


//------------------------------------------------------------------------------
// bool SetStart(Gmat::StateElementId id, Integer index, Integer quantity)
//------------------------------------------------------------------------------
/**
 * Sets indices for supported derivatives in the propagation state vector.
 *
 * Function used to set the start point and size information for the state
 * vector, so that the derivative information can be placed in the correct place
 * in the derivative vector.
 *
 * @param id State Element ID for the derivative type
 * @param index Starting index in the state vector for this type of derivative
 * @param quantity Number of objects that supply this type of data
 * @param quantity Number of rows/columns added to the STM (not yet used here)
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool PolyhedronGravityModel::SetStart(Gmat::StateElementId id, Integer index,
      Integer quantity, Integer sizeOfType)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage("PolyhedronGravityModel setting start data for id = "
            "%d to index %d; %d objects identified\n", id, index, quantity);
   #endif

   bool retval = false;

   switch (id)
   {
      case Gmat::CARTESIAN_STATE:
//         satCount       = quantity;
         cartesianCount = quantity;
         cartesianStart = index;
         fillCartesian  = true;
         retval         = true;
         break;

      default:
         break;
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool SupportsDerivative(Gmat::StateElementId id)
//------------------------------------------------------------------------------
/**
 * Function used to determine if the physical model supports derivative
 * information for a specified type.
 *
 * @param id State Element ID for the derivative type
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool PolyhedronGravityModel::SupportsDerivative(Gmat::StateElementId id)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage(
            "PolyhedronGravityModel checking for support for id %d\n", id);
   #endif

   if (id == Gmat::CARTESIAN_STATE)
      return true;

   return GravityBase::SupportsDerivative(id);
}

//------------------------------------------------------------------------------
// Real GetSolidAngle()
//------------------------------------------------------------------------------
/**
 * Returns the last computed value of the solid angle of the body.
 *
 * This value is usually the solid angle with respect to a state vector that was
 * passed into the derivative calculations.  If the derivatives have not been
 * calculated yet, the value is 0.0.  It is 4 pi when the state vector fell
 * inside of the body at the calculation point, and 0.0 (to numerical precision)
 * when outside of the body.
 *
 * @return The computed solid angle.
 */
//------------------------------------------------------------------------------
Real PolyhedronGravityModel::GetSolidAngle(Rvector3 &r, GmatEpoch time)
{
   #ifdef DEBUG_SOLID_ANGLE
      MessageInterface::ShowMessage("---------------------------- Solid Angle "
            "Calculation -----------------------------\n");
      MessageInterface::ShowMessage("Start PolyhedronGravityModel::"
            "GetSolidAngle()\n\n");

      MessageInterface::ShowMessage("Evaluating Polyhedron gravity solid angle; "
         "epoch = %lf,  r = [%lf %lf %lf]\n", 0.0, r[0], r[1], r[2]);
      MessageInterface::ShowMessage("body(%p) name = '%s', Poly file = %s\n", polybody,
           (polybody != NULL ? body->GetName().c_str() : "Body is NULL"),
           bodyShapeFilename.c_str());
   #endif

   // Specify coordinate transformation matrix:
   now = time;
   std::vector<Rmatrix33> transform = CalculateTransformationMatrix();
   Rmatrix33 D = transform[0];
   r = D*r;                            // position in BodyFixed coordinate system

   if (polybody == NULL)
      return 0.0;

   Rvector3 A, B, C, cR23, n;
   Rvector3 R1, R2, R3;
   Real r1, r2, r3;

   sumWf = 0.0;
   polybody->LoadBodyShape();
   polybody->FaceNormals();

   #ifdef DEBUG_SOLID_ANGLE
      MessageInterface::ShowMessage("   Shape data loaded: %d faces found,"
            " fn size = %d\n", polybody->facesList.size(), polybody->fn.size());
   #endif

   for (Integer i = polybody->facesList.size()-1; i >= 0; --i)
   {
     n = polybody->fn[i];

     // face vertices:
     PolygonFace face = polybody->facesList[i];
     A = polybody->verticesList[face[0]];
     B = polybody->verticesList[face[1]];
     C = polybody->verticesList[face[2]];

     // Calculate vectors from field point to face vertices:
     R1 = A-r;
     R2 = B-r;
     R3 = C-r;

     r1 = R1.Norm();
     r2 = R2.Norm();
     r3 = R3.Norm();

     cR23.Set(-R2(2)*R3(1) + R2(1)*R3(2),
             R2(2)*R3(0) - R2(0)*R3(2),
            -R2(1)*R3(0) + R2(0)*R3(1));

     // Calculate the solid angle term, wf:
     sumWf = sumWf + 2*atan2(R1*cR23,
           (r1*r2*r3+r1*R2*R3+r2*R3*R1+r3*R1*R2));
   }

   #ifdef DEBUG_SOLID_ANGLE
      MessageInterface::ShowMessage("SolidAngle = %le\n", sumWf);
   #endif

   return sumWf;
}

//------------------------------------------------------------------------------
// Real GetAltitude(Rvector3& r, GmatEpoch time)
//------------------------------------------------------------------------------
/**
 * Finds the distance to the closest polyhedron face
 *
 * @param r The spacecraft position
 * @param time The current epoch
 *
 * @return The height above the plane of r for the face closest to r
 */
//------------------------------------------------------------------------------
Real PolyhedronGravityModel::GetAltitude(Rvector3& r, GmatEpoch time)
{
   #ifdef DEBUG_SOLID_ANGLE
      MessageInterface::ShowMessage("---------------------------- Solid Angle "
            "Calculation -----------------------------\n");
      MessageInterface::ShowMessage("Start PolyhedronGravityModel::"
            "GetSolidAngle()\n\n");

      MessageInterface::ShowMessage("Evaluating Polyhedron gravity solid angle; "
         "epoch = %lf,  r = [%lf %lf %lf]\n", 0.0, r[0], r[1], r[2]);
      MessageInterface::ShowMessage("body(%p) name = '%s', Poly file = %s\n", polybody,
           (polybody != NULL ? body->GetName().c_str() : "Body is NULL"),
           bodyShapeFilename.c_str());
   #endif

   // Specify coordinate transformation matrix:
   now = time;
   std::vector<Rmatrix33> transform = CalculateTransformationMatrix();
   Rmatrix33 orient = transform[0];
   r = orient * r;                   // position in BodyFixed coordinate system

   if (polybody == NULL)
      return 0.0;

   Rvector3 A, B, C, D, cR23, n;
   Rvector3 R1, R2, R3, R;
   Real r1, r2, r3;

   Real alt, distance = 1.0e300;
   if (!isShapeLoaded)
   {
      polybody->LoadBodyShape();
      polybody->FaceNormals();
      isShapeLoaded = true;
   }

   #ifdef DEBUG_SOLID_ANGLE
      MessageInterface::ShowMessage("   Shape data loaded: %d faces found,"
            " fn size = %d\n", polybody->facesList.size(), polybody->fn.size());
   #endif

   Integer index = -1;
   for (Integer i = polybody->facesList.size()-1; i >= 0; --i)
   {
     n = polybody->fn[i];

     // face vertices:
     PolygonFace face = polybody->facesList[i];
     A = polybody->verticesList[face[0]];
     B = polybody->verticesList[face[1]];
     C = polybody->verticesList[face[2]];

     // Calculate the center of the face
     D = 1.0 / 3.0 * (A + B + C);
     // Vector from the face center to the input position
     R = r - D;

     alt = R.Norm();
     if (alt < distance)
     {
        distance = alt;
        index = i;
     }
   }

   if (index != -1)
   {
      // Compute the unit surface normal
      PolygonFace face = polybody->facesList[index];
      A = polybody->verticesList[face[0]];
      B = polybody->verticesList[face[1]];
      C = polybody->verticesList[face[2]];

      D[0] = A[1]*B[2]-B[1]*A[2] + B[1]*C[2]-C[1]*B[2] + C[1]*A[2]-A[1]*C[2];
      D[1] = A[2]*B[0]-B[2]*A[0] + B[2]*C[0]-C[2]*B[0] + C[2]*A[0]-A[2]*C[0];
      D[2] = A[0]*B[1]-B[0]*A[1] + B[0]*C[1]-C[0]*B[1] + C[0]*A[1]-A[0]*C[1];

      D /= D.Norm();

      // Dot it with r to get altitude
      alt = r * D;
   }
   else
      alt = 1.0e300;

   #ifdef DEBUG_SOLID_ANGLE
      MessageInterface::ShowMessage("Altitude = %le\n", alt);
   #endif

   return alt;
}
