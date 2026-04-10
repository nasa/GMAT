.. _JavaExamples:

API Examples in Java
====================

:ref:`DesignExamples` shows Python scripting for four common GMAT API use cases.
This section shows those same use cases in Java.

Time System Conversion
----------------------

.. code-block:: java
   :caption: Time System Conversion in Java
   :linenos:

   import gmat.*;

   public class TimeConvNew {

       public static void main(String[] args) {

           // Get the converter
           TimeSystemConverter timeConverter = gmat.theTimeSystemConverter;
           
           // Convert an epoch
           double UTCepoch = 21738.22145;
           double TAIepoch = timeConverter.Convert(UTCepoch, UTC, TAI);
     }
   }


Coordinate System Conversion
----------------------------

.. code-block:: java
   :caption: Coordinate Conversion in Java
   :linenos:

   import gmat.*;

   public class CoordConvNew {

       public static void main(String[] args) {
            
           // Initialize GMAT
           gmat.Setup("MyCustomStartupFile.txt");
            
           // Setup the GMAT data structures for the conversion
           A1Mjd mjd = new A1Mjd(22326.977184);
           Rvector6 rvIn = new Rvector6(6988.427, 1073.884, 2247.333, 0.019982, 7.226988, -1.554962);
           Rvector6 rvOut = new Rvector6(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
           
           // Create the converter
           CoordinateConverter csConverter = new CoordinateConverter();
           
           // Create the input and output coordinate systems
           CoordinateSystem eci = gmat.Construct("CoordinateSystem",
                   "ECI", "Earth", "MJ2000Eq");
           CoordinateSystem ecef = gmat.Construct("CoordinateSystem",
                   "ECEF", "Earth", "BodyFixed");
           
           csConverter.Convert(mjd, rvIn, eci, rvOut, ecef);
     }
   }


Force Modeling
--------------

.. code-block:: java
   :caption: Force Model Creation and Use in Java
   :linenos:

   import gmat.*;

   public class ForceModelNew {

     public static void main(String[] args) 
     {
       ODEModel dynamics = gmat.Construct("ODEModel","FM");

       PointMassForce epm = gmat.Construct("PointMassForce","EPM");
       dynamics.AddForce(epm);

       gmat.Initialize();
           
       dynamics.GetDerivatives(state, dt);
       double[] derivatives = dynamics.GetDerivativeArray();
     }
   }


Propagation
-----------

.. code-block:: java
   :caption: Propagation in Java
   :linenos:

   import gmat.*;

   public class PropExampleNew {

       public static void main(String[] args) {

           // Setup the state for propagation
           double[] state = {7000.0, 0.0, 1000.0, 0.0, 8.0, -0.25};
           // Setup a Earth/Sun/Moon force model
           // note: Use Moderator for the forces and Python memory management won't seg fault
           ODEModel dynamics = gmat.Construct("ODEModel", "Forces");
           
           PhysicalModel epm = gmat.Construct("PointMassForce", "EarthPointMass");
           PhysicalModel spm = gmat.Construct("PointMassForce", "SunPointMass");
           PhysicalModel mpm = gmat.Construct("PointMassForce", "MoonPointMass");
           spm.SetStringParameter("BodyName", "Sun");
           mpm.SetStringParameter("BodyName", "Luna");
           
           dynamics.AddForce(epm);
           dynamics.AddForce(spm);
           dynamics.AddForce(mpm);
           
           // Propagator configuration
           PrinceDormand78 prop = new PrinceDormand78("Propagator");
           prop.SetPhysicalModel(dynamics);
           
           gmat.Initialize();
           
           // Set the propagation state
           dynamics.SetState(state);
           
           for (int i = 0; i < 10; i++) {
               prop.Step(60.0);
           }
     }
   }
