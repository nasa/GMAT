GMAT's Generic Type
=====================

The Generic type in GMAT is implemented using the variant type, either from the
C++17 standard if available, or from the boost variant implementation when the
compiler does not conform to the C++17 standard.  For C++17 enabled compilers, 
users can control selection of the variant implementation using the Cmake 
GMAT_USE_BOOST_VARIANT setting in the GMAT Cmake configuration.  

.. note:: 
   At this writing, XCode 10 contains a partial implementaiton of C++17, but
   does not support features needed for standards conforming access to the 
   variant data.  The Cmake setup should toggle the Boost variant implementation
   automatically on OSX when using XCode 10. 

The Generic type is defined in the include file for GMAT utility code, 
utildefs.hpp.  The implementation supports integer, real, and string data as a 
single type.  Developers identify fields that use this type as having type 
GENERIC_TYPE when setting up the field specification for a class.

The Generic type in GMAT provides a storage containing for mixed type data on a 
field.  The class implementation is responsible for interpreting and using the 
stored data.

Code Support for Generics
--------------------------

The type definition for Generic fields, in utildefs.hpp, is, at this writing,

.. code-block:: c++
   :caption: utildefs.hpp: Generic definition

   // Generic container; Changes here require changes in GmatBase::GetGenericType()
   typedef Variant<Real, Integer, std::string, RealArray, IntegerArray,
                StringArray> Generic;

The container may grow over time based on needs encountered as we use it, so 
interested readers should check the definition in the header file for the 
current list of contained types.

The GmatBase class has seven methods available for use in support for Generic 
data fields:

.. code-block:: c++
   :caption: GmatBase.hpp: Support methods for Generic Fields
   :linenos:

   virtual Integer      CheckGenericTypeSupport(const Integer id,
                                                const Integer genType,
                                                const std::string &setting = "");
   virtual Generic      GetGenericParameter(const Integer id) const;
   virtual Generic      SetGenericParameter(const Integer id,
                                            const Generic value);

   virtual Integer      CheckGenericTypeSupport(const std::string &label,
                                                const Integer genType,
                                                const std::string &setting = "");
   virtual Generic      GetGenericParameter(const std::string &label) const;
   virtual Generic      SetGenericParameter(const std::string &label,
                                            const Generic value);

   UnsignedInt          GetType(const Generic &forGeneric);

The ``GetGenericParameter`` and ``SetGenericParameter`` methods here behave similarly to 
the methods for the native Real, Integer, String, and similar types.  Generics
contain data of a set type when they are loaded.  The ``GetType`` method retrieves 
the type of the data currently contained in a Generic.

The new ``CheckGenericTypeSupport`` method provides support for testing the Generic 
container for type compatibility with the contained data.  As an example, a 
class may need a Generic field that can be either an Integer or a String.  If a 
user tries to set that field using a Real value, the programmer can use their 
overridden ``CheckGenericTypeSupport`` method as a trap to throw an exception.

.. note:: TBD: Is ``CheckGenericTypeSupport`` actually useful?  I'm leaving it in 
   place for now, with a GmatBase implementation accepting everything a Generic
   supports.

Example Usage
--------------

One use case for the Generic data type is a field that may be input as either a
real number or a string.  An example of this usage is epoch data.  In GMAT, most
classes treat the epoch field as a string in the interrpeter subsystem, and 
assign it to a field with type ``TIME_TYPE`` or ``GMATTIME_TYPE``.  An alternative is to
implement the field as a Generic, simpifying the parsing of the field and 
placing the burden of interpretation on the class that implements the field.  

In order to show this use case, consider a hypothetical GroundPoint class that 
represents a point of a celestial body for the purposes of sensor acquisition.  
The point needs to be able to report its position with respect to its body.  An
actual implementation would specify the position data ina  body-fixed, and 
therefore time independent, coordinate system.  For the purposes of example 
code, we'll specify it in body fixed MJ2000 equatorial coordinates instead.  The
class header would looks something like

.. code-block:: c++
   :caption: BodyPoint.hpp: Header for a Generic Field, "Epoch"
   :linenos:

   class BODYPOINT_API BodyPoint : public SpacePoint
   {
   public:
      ...
   virtual Generic      GetGenericParameter(const Integer id) const;
   virtual Generic      SetGenericParameter(const Integer id,
                                            const Generic value);
   virtual Generic      GetGenericParameter(const std::string &label) const;
   virtual Generic      SetGenericParameter(const std::string &label,
                                            const Generic value);
      ...
   protected:
      /// Name of the body with the BodyPoint
      std::string originName;
      /// The body, set from the Solar Systems
      CelestialBody theOrigin;
      /// Location of the point on the body
      Rvector3 Location;
      /// Format of the input epoch
      std::string epochFormat;
      /// The container for the epoch of teh location data, from the script
      Generic epoch;
      /// The epoch, in A.1 mod Julian format, of the location 
      GmatEpoch theEpoch;
      ...
      enum
      {
         ORIGIN = SpacePointParamCount,
         LOCATION,
         EPOCH_FORMAT,
         EPOCH
      };
      /// Parameter types
      static const Gmat::ParameterType PARAMETER_TYPE[BodyPointParamCount- SpacePointParamCount];
      /// Parameter labels
      static const std::string PARAMETER_LABEL[BodyPointParamCount- SpacePointParamCount];

   }

The corresponding definitions at the top of the implementation file set the type 
and script strings:

.. code-block:: c++
   :caption: BodyPoint.cpp: Header for a Generic Field, "Epoch"
   :linenos:

   const std::string
   BodyPoint::PARAMETER_TEXT[SpacePointParamCount - GmatBaseParamCount] =
   {
      "Origin"
      "Location",
      "EpochFormat",
      "Epoch"
   };

   const Gmat::ParameterType
   BodyPoint::PARAMETER_TYPE[SpacePointParamCount - GmatBaseParamCount] =
   {
      Gmat::STRING_TYPE,       // "Origin"
      Gmat::RVECTOR_TYPE,      // "Location"
      Gmat::STRING_TYPE,       // "EpochFormat"
      Gmat::GENERIC_TYPE,      // "Epoch"
   };

Once the Parameter accessor code is in place, this setup enables the following 
scripting:

.. code-block:: matlab
   :caption: GMAT Scripting for a Generic Epoch
   :linenos:

   Create BodyPoint gsfc;
   gsfc.Origin = Earth;
   gsfc.Location = [1129.37, -4834.05, 3991.61];     % Not the real location!
   gsfc.EpochFormat = UTCGregorian;
   gsfc.Epoch = '01 Jan 2000 12:00:00.000'

   Create BodyPoint jpl;
   jpl.Origin = Earth;
   jpl.Location = [-2493.21, -4655.68, 3565.10];     % Not the real location!
   jpl.EpochFormat = UTCModJulian;
   jpl.Epoch = 21545.0

Note the treatment of epoch data here: the reference epoch for the gsfc 
BodyPoint is a string (line 5), while the epoch for the jpl BodyPoint is a real 
number (line 11).  Parsing handles mapping these options into the Generic 
container.

The Get and Set methods for the epoch are similar to code used for other fields
in GMAT: 

.. code-block:: c++
   :caption: Get and Set Methods for a Generic Field
   :linenos:

   Generic BodyPoint::GetGenericParameter(const Integer id) const
   {
      if (id == EPOCH)
         return epoch;
      return SpacePoint::GetGenericParameter(id);
   }

   Generic BodyPoint::SetGenericParameter(const Integer id, const Generic value)
   {
      if (id == EPOCH)
      {
         epoch = value;
         return epoch;
      }
      return SpacePoint::SetGenericParameter(id, value);
   }

   Generic BodyPoint::GetGenericParameter(const std::string &label) const
   {
      return GetGenericParameter(GetParameterID(label));
   }

   Generic BodyPoint::SetGenericParameter(const std::string &label, const Generic value)
   {
      return SetGenericParameter(GetParameterID(label), value);
   }

The underlying code needs to take the generic and convert it into a form that 
can be used in a BodyPoint object.  The place this happens will depend on the 
class that uses the Generic.  For this example, the epoch is needed during 
initialization, so the Generic is set in the Initialize method:

.. code-block:: c++
   :caption: Use of a Generic
   :linenos:

   void BodyPoint::Initialize()
   {
      ...
      UnsignedInt gType = GetType(epoch);
      if (gType == Gmat::STRING_TYPE)
         theTimeConverter->Convert(epochFormat, 0.0, epoch.VarGet<std::string>, 
               "A1ModJulian", theEpoch, "");
      else
      {
         Real val;

         val = (gType == Gmat::REAL_TYPE ? epoch.VarGet<Real> : epoch.VarGet<Integer>)

         theTimeConverter->Convert(epochFormat, val, "", 
               "A1ModJulian", theEpoch, "");

      }
      ...
   }

Generic fields by themselves are of limited usefulness.  However, Generic inputs
to scripted object methods, the subject of :ref:`ObjectMethods`, provide a 
powerful tool for enhancing GMAT scripting.
