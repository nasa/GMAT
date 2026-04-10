// Common SWIG imports for GMAT
%include "std_string.i"

// Define needed for swig 4.1 and later
#define SWIGPY_SLICEOBJECT PyObject

// Apply array typemaps
%apply double[] {double *};
%apply int[] {int *};

ARRAYRETURN(double, Double, GmatBase, GetPropItem(const Integer), GetPropItemSize(arg2))
ARRAYRETURN(double, Double, GmatBase, GetEstimationParameterValue(const Integer), GetEstimationParameterSize(arg2))
ARRAYRETURN(double, Double, GmatState, GetState(), GetSize())
ARRAYRETURN(double, Double, GmatState, GetStateDot(), GetSize())
ARRAYRETURNSIZE(double, Double, Burn, GetDeltaVInertial(), 3)
ARRAYRETURNSIZE(double, Double, Burn, GetTotalAcceleration(), 3)
ARRAYRETURNSIZE(double, Double, Burn, GetTotalThrust(), 3)
ARRAYRETURN(double, Double, PhysicalModel, GetState(), GetDimension())
ARRAYRETURN(double, Double, PhysicalModel, GetStateDot(), GetDimension())
ARRAYRETURN(double, Double, PhysicalModel, GetJ2KState(), GetDimension())
ARRAYRETURN(double, Double, PhysicalModel, GetDerivativeArray(), GetDimension())
ARRAYRETURN(double, Double, PhysicalModel, GetJ2KDerivativeArray(), GetDimension())
ARRAYRETURN(double, Double, ODEModel, GetEstimationParameterValue(const Integer), GetEstimationParameterSize(arg2))
ARRAYRETURNSIZE(double, Double, GroundstationInterface, IsValidElevationAngle(const Rvector6 &), 3)
ARRAYRETURNSIZE(double, Double, AtmosphereModel, GetAngularVelocity(const Real), 3)
ARRAYRETURNSIZE(double, Double, PlanetaryEphem, GetPosVel(Integer, A1Mjd, bool), 6)
ARRAYRETURNSIZE(double, Double, PlanetaryEphem, GetPosVel(Integer, GmatTime, bool), 6)
ARRAYRETURNSIZE(double, Double, PlanetaryEphem, GetPosDelta(Integer, const GmatTime &, const GmatTime &, bool), 3)
ARRAYRETURNSIZE(double, Double, DeFile, GetPosVel(Integer, A1Mjd, bool), 6)
ARRAYRETURNSIZE(double, Double, DeFile, GetPosVel(Integer, GmatTime, bool), 6)
ARRAYRETURNSIZE(double, Double, DeFile, GetPosDelta(Integer, const GmatTime &, const GmatTime &, bool), 3)
ARRAYRETURN(double, Double, Spacecraft, GetPropItem(const Integer), GetPropItemSize(arg2))
ARRAYRETURN(double, Double, Spacecraft, GetEstimationParameterValue(const Integer), GetEstimationParameterSize(arg2))
ARRAYRETURN(double, Double, Propagator, AccessOutState(), GetDimension())
ARRAYRETURN(double, Double, Propagator, GetState(), GetDimension())
ARRAYRETURN(double, Double, Propagator, GetJ2KState(), GetDimension())

ARRAYRETURNSIZE(int, Int, PlanetaryEphem, GetStartDayAndYear(), 2)
ARRAYRETURNSIZE(int, Int, DeFile, GetStartDayAndYear(), 2)


EXCEPTION()

// Ignore nested exceptions classes
%ignore Date::TimeRangeError;
%ignore Date::LeapYearError;
%ignore ArrayTemplateExceptions;
%ignore ArrayTemplateExceptions::OutOfBounds;
%ignore ArrayTemplateExceptions::DimensionError;
%ignore ArrayTemplateExceptions::UnsizedArray;
%ignore ArrayTemplateExceptions::ArrayAlreadySized;
%ignore ArrayTemplateExceptions::IllegalSize;
%ignore GregorianDate::GregorianDateException;
%ignore RealUtilitiesExceptions;
%ignore RealUtilitiesExceptions::ArgumentError;
%ignore RealUtilitiesExceptions::IllegalTime;
%ignore TableTemplateExceptions;
%ignore TableTemplateExceptions::DimensionError;
%ignore TableTemplateExceptions::OutOfBounds;
%ignore TableTemplateExceptions::UnsizedTable;
%ignore TableTemplateExceptions::TableAlreadySized;
%ignore TableTemplateExceptions::IllegalSize;
%ignore Rmatrix::NotSquare;
%ignore Rmatrix::IsSingular;
%ignore Rmatrix::DivideByZero;
%ignore Rvector::ZeroVector;
// These exception(s) are in BodyFixedStateConverter
%ignore InvalidStateRepresentationException;
// These exception(s) are in TimeSystemConverter
%ignore UnimplementedException;
%ignore TimeFileException;
%ignore TimeFormatException;
%ignore InvalidTimeException;

// Ignore assignment operator
%ignore *::operator=;

%{
#include "GmatAPI.hpp"
%}


%include "std_vector.i"
%include "std_map.i"
// Template names taken from gmatdefs.hpp
VECTORCONVERT(double, double, RealArray)
VECTORCONVERT(int, int, IntegerArray)
VECTORCONVERT(unsigned int, long, UnsignedIntArray)
VECTORCONVERT(std::string, String, StringArray)
VECTORCONVERT(bool, boolean, BooleanArray)

VECTORCONVERT(GmatBase*, GmatBase, ObjectArray)
VECTORCONVERT(ElementWrapper*, ElementWrapper, WrapperArray)
VECTORCONVERT(Rvector6*, Rvector6, StateArray)
VECTORCONVERT(A1Mjd*, A1Mjd, EpochArray)
%template(IntegerMap) std::map<std::string, int>;
// Skip ColorMap as it is duplicated by ObjectTypeMap
%template(ObjectMap) std::map<std::string, GmatBase*>;
%template(WrapperMap) std::map<std::string, ElementWrapper*>;

// Skip ObjectTypeArray as it is duplicated by UnsignedIntArray
// Skip WrapperTypeArray as it is essentially an IntegerArray
%template(ObjectTypeMap) std::map<std::string, unsigned int>;
%template(ObjectTypeArrayMap) std::map<unsigned int, std::vector<std::string>>;

// Additional vectors used with no explicit typedef name
VECTORCONVERT(GmatCommand*, GmatCommand, GmatCommandArray)
VECTORCONVERT(GmatTime*, GmatTime, GmatTimeArray)
VECTORCONVERT(std::vector<double>, RealArray, RealArrayArray)
VECTORCONVERT(std::vector<int>, IntegerArray, IntegerArrayArray)
VECTORCONVERT(PhysicalModel*, PhysicalModel, PhysicalModelArray)
VECTORCONVERT(SpaceObject*, SpaceObject, SpaceObjectArray)

// Additional maps used with no explicit typedef name
%template(ObjectArrayMap) std::map<std::string, std::vector<GmatBase*>>;

// GmatUtil

%include "utildefs.hpp"
%ignore GmatRealConstants::INTEGER_UNDEFINED; // INTEGER_UNDEFINED is defined twice
%include "GmatConstants.hpp"
%include "BaseException.hpp"

%include "Date.hpp"
//%include "DateUtil.hpp"
// DOWNCAST(A1Date,Date) Cannot use dynamic_cast on Date
%include "A1Date.hpp"
%include "A1Mjd.hpp"
%include "AngleUtil.hpp"

%ignore ArrayTemplate::operator()(Integer); // Duplicate of operator []
%ignore ArrayTemplate::operator()(Integer) const; // Duplicate of operator []
%ignore ArrayTemplate::operator[]; // Ignore this so %extend will be used
%include "ArrayTemplate.hpp"
%template(ArrayTemplateReal) ArrayTemplate<Real>;
%extend ArrayTemplate<Real> {
#ifdef SWIGPYTHON
    Real __setitem__(Integer index, const Real& value) {
#else
    Real setitem(Integer index, const Real& value) {
#endif
        (*$self)[index] = value;
        return (*$self)[index];
    }
#ifdef SWIGPYTHON
    const Real __getitem__(Integer index) const {
#else
    const Real getitem(Integer index) const {
#endif
        return (*$self)[index];
    }
}

%include "AttitudeConversionUtility.hpp"
%rename(BodyFixedStateConvert) BodyFixedStateConverterUtil::Convert;
%include "BodyFixedStateConverter.hpp"
%include "CalculationUtilities.hpp"
%include "CCSDSEMReader.hpp"
%include "CCSDSEMSegment.hpp"
DOWNCAST(CCSDSAEMReader,CCSDSEMReader)
%include "CCSDSAEMReader.hpp"
DOWNCAST(CCSDSAEMSegment,CCSDSEMSegment)
%include "CCSDSAEMSegment.hpp"
DOWNCAST(CCSDSAEMEulerAngleSegment,CCSDSEMSegment)
%include "CCSDSAEMEulerAngleSegment.hpp"
DOWNCAST(CCSDSAEMQuaternionSegment,CCSDSEMSegment)
%include "CCSDSAEMQuaternionSegment.hpp"
%include "Code500EphemerisFile.hpp"
%include "ColorDatabase.hpp"
%include "ElapsedTime.hpp"
%include "EopFile.hpp"
%include "FileManager.hpp"
%include "Frozen.hpp"
%include "GmatDefaults.hpp"
%include "GmatGlobal.hpp"
%include "GmatTime.hpp"
%include "GregorianDate.hpp"
%ignore LeapSecsFileReader(const LeapSecsFileReader& lsfr); // No implementation
%include "LeapSecsFileReader.hpp"
// %include "Linear.hpp"
%include "OrbitDesignerTime.hpp"
%include "RealUtilities.hpp"
%include "RepeatGroundTrack.hpp"
%include "RepeatSunSync.hpp"
%include "RgbColor.hpp"

%ignore TableTemplate::operator(); // Ignore this so %extend will be used
%include "TableTemplate.hpp"
%template(TableTemplateReal) TableTemplate<Real>;
%extend TableTemplate<Real> {
#ifdef SWIGPYTHON
    Real __setitem__(Integer indices[2], const Real& value) {
        Integer r = indices[0];
        Integer c = indices[1];
#else
    Real setitem(Integer r, Integer c, const Real& value) {
#endif
        (*$self)(r,c) = value;
        return (*$self)(r,c);
    }
#ifdef SWIGPYTHON
    const Real __getitem__(Integer indices[2]) const {
        Integer r = indices[0];
        Integer c = indices[1];
#else
    const Real getitem(Integer r, Integer c) const {
#endif
        return (*$self)(r,c);
    }
}

%ignore operator>>(std::istream &, Rmatrix &);
%ignore operator<<(std::ostream &, const Rmatrix &);
DOWNCAST(Rmatrix,TableTemplate<Real>)
%include "Rmatrix.hpp"
DOWNCAST(Rmatrix33,TableTemplate<Real>)
%include "Rmatrix33.hpp"
DOWNCAST(Rmatrix66,TableTemplate<Real>)
%include "Rmatrix66.hpp"

%ignore operator>>(std::istream &, Rvector &);
%ignore operator<<(std::ostream &, const Rvector &);
%ignore operator<(const Element &e1, const Element &e2);
DOWNCAST(Rvector,ArrayTemplate<Real>)
%include "Rvector.hpp"
DOWNCAST(Rvector3,ArrayTemplate<Real>)
%include "Rvector3.hpp"
DOWNCAST(Rvector6,ArrayTemplate<Real>)
%include "Rvector6.hpp"

%include "StateConversionUtil.hpp"
%include "StringTokenizer.hpp"
%include "SunSync.hpp"
%include "TextParser.hpp"
%rename(TimeConvert) TimeConverterUtil::Convert;
%include "TimeSystemConverter.hpp"
%include "TimeTypes.hpp"
// DOWNCAST(UtcDate,Date) Cannot use dynamic_cast on Date
%include "UtcDate.hpp"

%include "Interpolator.hpp"
%include "BrentDekkerZero.hpp"
DOWNCAST(CubicSplineInterpolator,Interpolator)
%include "CubicSplineInterpolator.hpp"
DOWNCAST(HermiteInterpolator,Interpolator)
%include "HermiteInterpolator.hpp"
DOWNCAST(LagrangeInterpolator,Interpolator)
%include "LagrangeInterpolator.hpp"
DOWNCAST(LinearInterpolator,Interpolator)
%include "LinearInterpolator.hpp"
DOWNCAST(NotAKnotInterpolator,Interpolator)
%include "NotAKnotInterpolator.hpp"

// GmatBase

%include "gmatdefs.hpp"

%include "GmatBase.hpp"

%include "GmatState.hpp"
%include "Covariance.hpp"
%include "ElementWrapper.hpp"
%include "StateManager.hpp"
DOWNCAST(SpacePoint,GmatBase)
%include "SpacePoint.hpp"

DOWNCAST(Attitude,GmatBase)
%include "Attitude.hpp"
DOWNCAST(Kinematic,GmatBase)
%include "Kinematic.hpp"
DOWNCAST(CSFixed,GmatBase)
%include "CSFixed.hpp"
DOWNCAST(CCSDSAttitude,GmatBase)
%include "CCSDSAttitude.hpp"
DOWNCAST(NadirPointing,GmatBase)
%include "NadirPointing.hpp"
DOWNCAST(PrecessingSpinner,GmatBase)
%include "PrecessingSpinner.hpp"
DOWNCAST(SpiceAttitude,GmatBase)
%include "SpiceAttitude.hpp"
DOWNCAST(Spinner,GmatBase)
%include "Spinner.hpp"

DOWNCAST(Burn,GmatBase)
%include "Burn.hpp"
%include "ManeuverFrame.hpp"
DOWNCAST(FiniteBurn,GmatBase)
%include "FiniteBurn.hpp"
DOWNCAST(ImpulsiveBurn,GmatBase)
%include "ImpulsiveBurn.hpp"
DOWNCAST(InertialManeuverFrame,ManeuverFrame)
%include "InertialManeuverFrame.hpp"
%include "ManeuverFrameManager.hpp"
DOWNCAST(VnbManeuverFrame,ManeuverFrame)
%include "VnbManeuverFrame.hpp"

DOWNCAST(PhysicalModel,GmatBase)

// Omit some CSALT methods for now
%ignore PhysicalModel::GetMassJacobian();
%ignore PhysicalModel::GetTimeJacobian();

%include "PhysicalModel.hpp"
DOWNCAST(DragForce,GmatBase)
%include "DragForce.hpp"
 //%include "EventModel.hpp"
DOWNCAST(FiniteThrust,GmatBase)
%include "FiniteThrust.hpp"
DOWNCAST(GravityBase,GmatBase)
%include "GravityBase.hpp"
DOWNCAST(HarmonicField,GmatBase)
%include "HarmonicField.hpp"
DOWNCAST(GravityField,GmatBase)
%include "GravityField.hpp"
DOWNCAST(ODEModel,GmatBase)
%include "ODEModel.hpp"
DOWNCAST(PointMassForce,GmatBase)
%include "PointMassForce.hpp"
DOWNCAST(RelativisticCorrection,GmatBase)
%include "RelativisticCorrection.hpp"
DOWNCAST(SolarRadiationPressure,GmatBase)
%include "SolarRadiationPressure.hpp"

%include "Harmonic.hpp"
DOWNCAST(HarmonicGravity,Harmonic)
%include "HarmonicGravity.hpp"

DOWNCAST(MathNode,GmatBase)
%include "MathNode.hpp"
DOWNCAST(MathFunction,GmatBase)
%include "MathFunction.hpp"
DOWNCAST(Abs,GmatBase)
%include "Abs.hpp"
DOWNCAST(Acos,GmatBase)
%include "Acos.hpp"
DOWNCAST(Acosh,GmatBase)
%include "Acosh.hpp"
DOWNCAST(Add,GmatBase)
%include "Add.hpp"
DOWNCAST(Asin,GmatBase)
%include "Asin.hpp"
DOWNCAST(Asinh,GmatBase)
%include "Asinh.hpp"
DOWNCAST(Atan,GmatBase)
%include "Atan.hpp"
DOWNCAST(Atan2,GmatBase)
%include "Atan2.hpp"
DOWNCAST(Cos,GmatBase)
%include "Cos.hpp"
DOWNCAST(Cosh,GmatBase)
%include "Cosh.hpp"
DOWNCAST(DegToRad,GmatBase)
%include "DegToRad.hpp"
DOWNCAST(Determinant,GmatBase)
%include "Determinant.hpp"
DOWNCAST(Divide,GmatBase)
%include "Divide.hpp"
// DOWNCAST(Exp,GmatBase)
// %include "Exp.hpp"
DOWNCAST(FunctionRunner,GmatBase)
%include "FunctionRunner.hpp"
DOWNCAST(Inverse,GmatBase)
%include "Inverse.hpp"
DOWNCAST(Log,GmatBase)
%include "Log.hpp"
// DOWNCAST(Log10,GmatBase)
// %include "Log10.hpp"
DOWNCAST(MathElement,GmatBase)
%include "MathElement.hpp"
DOWNCAST(Multiply,GmatBase)
%include "Multiply.hpp"
DOWNCAST(Negate,GmatBase)
%include "Negate.hpp"
DOWNCAST(Norm,GmatBase)
%include "Norm.hpp"
DOWNCAST(Power,GmatBase)
%include "Power.hpp"
DOWNCAST(RadToDeg,GmatBase)
%include "RadToDeg.hpp"
DOWNCAST(Sin,GmatBase)
%include "Sin.hpp"
DOWNCAST(Sinh,GmatBase)
%include "Sinh.hpp"
// DOWNCAST(Sqrt,GmatBase)
// %include "Sqrt.hpp"
DOWNCAST(Subtract,GmatBase)
%include "Subtract.hpp"
DOWNCAST(Tan,GmatBase)
%include "Tan.hpp"
DOWNCAST(Tanh,GmatBase)
%include "Tanh.hpp"
DOWNCAST(Transpose,GmatBase)
%include "Transpose.hpp"

DOWNCAST(BodyFixedPoint,GmatBase)
%include "BodyFixedPoint.hpp"
DOWNCAST(GroundstationInterface,GmatBase)
%include "GroundstationInterface.hpp"

DOWNCAST(CoordinateBase,GmatBase)
%include "CoordinateBase.hpp"
DOWNCAST(CoordinateSystem,GmatBase)
%include "CoordinateSystem.hpp"
DOWNCAST(AxisSystem,GmatBase)
%include "AxisSystem.hpp"

DOWNCAST(DynamicAxes,GmatBase)
%include "DynamicAxes.hpp"
DOWNCAST(BodyFixedAxes,GmatBase)
%include "BodyFixedAxes.hpp"
DOWNCAST(EquatorAxes,GmatBase)
%include "EquatorAxes.hpp"
DOWNCAST(ITRFAxes,GmatBase)
%include "ITRFAxes.hpp"
DOWNCAST(LocalAlignedConstrainedAxes,GmatBase)
%include "LocalAlignedConstrainedAxes.hpp"
DOWNCAST(MeanOfDateAxes,GmatBase)
%include "MeanOfDateAxes.hpp"
DOWNCAST(MODEcAxes,GmatBase)
%include "MODEcAxes.hpp"
DOWNCAST(MODEqAxes,GmatBase)
%include "MODEqAxes.hpp"
DOWNCAST(TrueOfDateAxes,GmatBase)
%include "TrueOfDateAxes.hpp"
DOWNCAST(TODEcAxes,GmatBase)
%include "TODEcAxes.hpp"
DOWNCAST(TODEqAxes,GmatBase)
%include "TODEqAxes.hpp"
DOWNCAST(TEMEAxes,GmatBase)
%include "TEMEAxes.hpp"
DOWNCAST(TopocentricAxes,GmatBase)
%include "TopocentricAxes.hpp"
DOWNCAST(ObjectReferencedAxes,GmatBase)
%include "ObjectReferencedAxes.hpp"
DOWNCAST(BodySpinSunAxes,GmatBase)
%include "BodySpinSunAxes.hpp"
DOWNCAST(GeocentricSolarEclipticAxes,GmatBase)
%include "GeocentricSolarEclipticAxes.hpp"

DOWNCAST(InertialAxes,GmatBase)
%include "InertialAxes.hpp"
DOWNCAST(BodyInertialAxes,GmatBase)
%include "BodyInertialAxes.hpp"
DOWNCAST(ICRFAxes,GmatBase)
%include "ICRFAxes.hpp"
DOWNCAST(MJ2000EcAxes,GmatBase)
%include "MJ2000EcAxes.hpp"
DOWNCAST(MJ2000EqAxes,GmatBase)
%include "MJ2000EqAxes.hpp"
DOWNCAST(TOEEcAxes,GmatBase)
%include "TOEEcAxes.hpp"
DOWNCAST(TOEEqAxes,GmatBase)
%include "TOEEqAxes.hpp"

%include "IAUFile.hpp"
%include "ICRFFile.hpp"
%include "ItrfCoefficientsFile.hpp"

%include "CoordinateConverter.hpp"

DOWNCAST(CelestialBody,GmatBase)
%include "CelestialBody.hpp"
DOWNCAST(Asteroid,GmatBase)
%include "Asteroid.hpp"
DOWNCAST(Comet,GmatBase)
%include "Comet.hpp"
DOWNCAST(Moon,GmatBase)
%include "Moon.hpp"
DOWNCAST(Planet,GmatBase)
%include "Planet.hpp"
DOWNCAST(SpecialCelestialPoint,GmatBase)
%include "SpecialCelestialPoint.hpp"
DOWNCAST(Star,GmatBase)
%include "Star.hpp"
   
DOWNCAST(CalculatedPoint,GmatBase)
%include "CalculatedPoint.hpp"
DOWNCAST(Barycenter,GmatBase)
%include "Barycenter.hpp"
DOWNCAST(LibrationPoint,GmatBase)
%include "LibrationPoint.hpp"
   
DOWNCAST(AtmosphereModel,GmatBase)
%include "AtmosphereModel.hpp"
DOWNCAST(ExponentialAtmosphere,GmatBase)
%include "ExponentialAtmosphere.hpp"
DOWNCAST(JacchiaRobertsAtmosphere,GmatBase)
%include "JacchiaRobertsAtmosphere.hpp"
DOWNCAST(Msise90Atmosphere,GmatBase)
%include "Msise90Atmosphere.hpp"
DOWNCAST(SimpleExponentialAtmosphere,GmatBase)
%include "SimpleExponentialAtmosphere.hpp"

%include "PlanetaryEphem.hpp"
DOWNCAST(DeFile,PlanetaryEphem)
%include "DeFile.hpp"

%include "ShadowState.hpp"
%include "SolarFluxReader.hpp"

DOWNCAST(SolarSystem,GmatBase)
%include "SolarSystem.hpp"

DOWNCAST(GmatCommand,GmatBase)
%include "GmatCommand.hpp"
DOWNCAST(BranchCommand,GmatBase)
%include "BranchCommand.hpp"
DOWNCAST(ConditionalBranch,GmatBase)
%include "ConditionalBranch.hpp"
DOWNCAST(SolverBranchCommand,GmatBase)
%include "SolverBranchCommand.hpp"
DOWNCAST(ManageObject,GmatBase)
%include "ManageObject.hpp"
DOWNCAST(PropagationEnabledCommand,GmatBase)
%include "PropagationEnabledCommand.hpp"
DOWNCAST(SolverSequenceCommand,GmatBase)
%include "SolverSequenceCommand.hpp"
DOWNCAST(Achieve,GmatBase)
%include "Achieve.hpp"
DOWNCAST(Assignment,GmatBase)
%include "Assignment.hpp"
DOWNCAST(BeginFiniteBurn,GmatBase)
%include "BeginFiniteBurn.hpp"
DOWNCAST(BeginFunction,GmatBase)
%include "BeginFunction.hpp"
DOWNCAST(BeginMissionSequence,GmatBase)
%include "BeginMissionSequence.hpp"
DOWNCAST(BeginScript,GmatBase)
%include "BeginScript.hpp"
DOWNCAST(CallFunction,GmatBase)
%include "CallFunction.hpp"
DOWNCAST(CallBuiltinGmatFunction,GmatBase)
%include "CallBuiltinGmatFunction.hpp"
DOWNCAST(FindEvents,GmatBase)
%include "FindEvents.hpp"
%include "CommandUtil.hpp"
// DOWNCAST(ClearPlot,GmatBase)
// %include "ClearPlot.hpp"
DOWNCAST(Create,GmatBase)
%include "Create.hpp"
DOWNCAST(Else,GmatBase)
%include "Else.hpp"
DOWNCAST(EndFiniteBurn,GmatBase)
%include "EndFiniteBurn.hpp"
DOWNCAST(EndFor,GmatBase)
%include "EndFor.hpp"
DOWNCAST(EndFunction,GmatBase)
%include "EndFunction.hpp"
DOWNCAST(EndIf,GmatBase)
%include "EndIf.hpp"
DOWNCAST(EndOptimize,GmatBase)
%include "EndOptimize.hpp"
DOWNCAST(EndScript,GmatBase)
%include "EndScript.hpp"
DOWNCAST(EndTarget,GmatBase)
%include "EndTarget.hpp"
DOWNCAST(EndWhile,GmatBase)
%include "EndWhile.hpp"
DOWNCAST(For,GmatBase)
%include "For.hpp"
DOWNCAST(If,GmatBase)
%include "If.hpp"
DOWNCAST(Maneuver,GmatBase)
%include "Maneuver.hpp"
// DOWNCAST(MarkPoint,GmatBase)
// %include "MarkPoint.hpp"
DOWNCAST(Minimize,GmatBase)
%include "Minimize.hpp"
DOWNCAST(NonlinearConstraint,GmatBase)
%include "NonlinearConstraint.hpp"
DOWNCAST(NoOp,GmatBase)
%include "NoOp.hpp"
DOWNCAST(Optimize,GmatBase)
%include "Optimize.hpp"
// DOWNCAST(PenDown,GmatBase)
// %include "PenDown.hpp"
// DOWNCAST(PenUp,GmatBase)
// %include "PenUp.hpp"
DOWNCAST(Propagate,GmatBase)
%include "Propagate.hpp"
// DOWNCAST(PlotCommand,GmatBase)
// %include "PlotCommand.hpp"
DOWNCAST(RHSEquation,GmatBase)
%include "RHSEquation.hpp"
DOWNCAST(Report,GmatBase)
%include "Report.hpp"
DOWNCAST(RunSolver,GmatBase)
%include "RunSolver.hpp"
DOWNCAST(SaveMission,GmatBase)
%include "SaveMission.hpp"
DOWNCAST(Stop,GmatBase)
%include "Stop.hpp"
DOWNCAST(Target,GmatBase)
%include "Target.hpp"
DOWNCAST(Toggle,GmatBase)
%include "Toggle.hpp"
DOWNCAST(UpdateDynamicData,GmatBase)
%include "UpdateDynamicData.hpp"
DOWNCAST(Vary,GmatBase)
%include "Vary.hpp"
DOWNCAST(While,GmatBase)
%include "While.hpp"
DOWNCAST(Write,GmatBase)
%include "Write.hpp"

DOWNCAST(SpaceObject,GmatBase)
%include "SpaceObject.hpp"
DOWNCAST(Spacecraft,GmatBase)
%include "Spacecraft.hpp"

DOWNCAST(Solver,GmatBase)
%include "Solver.hpp"
DOWNCAST(Optimizer,GmatBase)
%include "Optimizer.hpp"
%include "DerivativeModel.hpp"
DOWNCAST(DifferentialCorrector,GmatBase)
%include "DifferentialCorrector.hpp"
DOWNCAST(ExternalOptimizer,GmatBase)
%include "ExternalOptimizer.hpp"
DOWNCAST(Gradient,DerivativeModel)
%include "Gradient.hpp"
DOWNCAST(InternalOptimizer,GmatBase)
%include "InternalOptimizer.hpp"
// DOWNCAST(ISolverListener,IChangeListener)
// %include "ISolverListener.hpp"
DOWNCAST(Jacobian,DerivativeModel)
%include "Jacobian.hpp"
%include "LineSearch.hpp"

DOWNCAST(PropSetup,GmatBase)
%include "PropSetup.hpp"
DOWNCAST(Propagator,GmatBase)
%include "Propagator.hpp"
DOWNCAST(Integrator,GmatBase)
%include "Integrator.hpp"
DOWNCAST(PredictorCorrector,GmatBase)
%include "PredictorCorrector.hpp"
DOWNCAST(RungeKutta,GmatBase)
%include "RungeKutta.hpp"
DOWNCAST(RungeKuttaNystrom,GmatBase)
%include "RungeKuttaNystrom.hpp"
DOWNCAST(AdamsBashforthMoulton,GmatBase)
%include "AdamsBashforthMoulton.hpp"
// DOWNCAST(Cowell,GmatBase)
//Non-creatable %include "Cowell.hpp"
DOWNCAST(DormandElMikkawyPrince68,GmatBase)
%include "DormandElMikkawyPrince68.hpp"
DOWNCAST(PrinceDormand45,GmatBase)
%include "PrinceDormand45.hpp"
DOWNCAST(PrinceDormand78,GmatBase)
%include "PrinceDormand78.hpp"
DOWNCAST(PropagationStateManager,StateManager)
%include "PropagationStateManager.hpp"
DOWNCAST(RungeKutta89,GmatBase)
%include "RungeKutta89.hpp"
DOWNCAST(RungeKuttaFehlberg56,GmatBase)
%include "RungeKuttaFehlberg56.hpp"

/*
// GmatParam::ODE_MODEL conflicts with Gmat::ODE_MODEL
%rename(GmatParam_ODE_MODEL) GmatParam::ODE_MODEL;
%include "Parameter.hpp"
%include "AlternateEquinoctialParameters.hpp"   
%include "AngularParameters.hpp"   
%include "Array.hpp"
%include "ArrayElementWrapper.hpp"
%include "ArrayWrapper.hpp"

//Non-creatable %include "AttitudeData.hpp"
%include "AttitudeParameters.hpp"   
//Non-creatable %include "AttitudeReal.hpp"
//Non-creatable %include "AttitudeRvector.hpp"
//Non-creatable %include "AttitudeString.hpp"
%include "BallisticMassParameters.hpp"
//Non-creatable %include "BallisticMassReal.hpp"
%include "BooleanWrapper.hpp"
 //Non-creatable %include "BplaneData.hpp"   
%include "BplaneParameters.hpp"   
//Non-creatable %include "BplaneReal.hpp"   
%include "BrouwerMeanLongParameters.hpp"   
%include "BrouwerMeanShortParameters.hpp"   
//Non-creatable %include "BurnData.hpp"   
%include "BurnParameters.hpp"   
%include "CartesianParameters.hpp"   
%include "DelaunayParameters.hpp"   
//Non-creatable %include "EnvData.hpp"   
//Non-creatable %include "EnvReal.hpp"   
%include "EquinoctialParameters.hpp"   
%include "ExpressionParser.hpp"   
//Non-creatable %include "HardwareReal.hpp"   
%include "HardwareParameters.hpp"   
%include "IncomingAsymptoteParameters.hpp"   
%include "KeplerianParameters.hpp"   
%include "ModEquinoctialParameters.hpp"   
%include "NumberWrapper.hpp"   
%include "ObjectWrapper.hpp"   
%include "ObjectPropertyWrapper.hpp"   
%include "OnOffWrapper.hpp"   
//Non-creatable %include "OrbitData.hpp"   
 //Non-creatable %include "OrbitReal.hpp"   
//Non-creatable %include "OrbitRmat33.hpp"   
//Non-creatable %include "OrbitRmat66.hpp"   
//Non-creatable %include "OrbitRvec6.hpp"   
%include "OrbitStmParameters.hpp"   
%include "OrbitalParameters.hpp"   
%include "OutgoingAsymptoteParameters.hpp"   
%include "ParameterDatabase.hpp"   
%include "ParameterInfo.hpp"   
%include "ParameterWrapper.hpp"   
//Non-creatable %include "PlanetData.hpp"   
%include "PlanetParameters.hpp"   
//Non-creatable %include "PlanetReal.hpp"   
%include "PlanetodeticParameters.hpp"   
//Non-creatable %include "RealVar.hpp"   
//Non-creatable %include "RefData.hpp"   
//Non-creatable %include "Rmat33Var.hpp"   
//Non-creatable %include "Rmat66Var.hpp"   
//Non-creatable %include "Rvec6Var.hpp"   
//Non-creatable %include "RvectorVar.hpp"   
//Non-creatable %include "SpacecraftData.hpp"   
%include "SphericalParameters.hpp"   
%include "StringObjectWrapper.hpp"   
//Non-creatable %include "TimeData.hpp"
%include "TimeParameters.hpp"
//Non-creatable %include "TimeReal.hpp"
//Non-creatable %include "TimeString.hpp"
%include "Variable.hpp"
%include "VariableWrapper.hpp"
*/

DOWNCAST(Hardware,GmatBase)
%include "Hardware.hpp"
DOWNCAST(Imager,GmatBase)
%include "Imager.hpp"
DOWNCAST(FuelTank,GmatBase)
%include "FuelTank.hpp"
DOWNCAST(PowerSystem,GmatBase)
%include "PowerSystem.hpp"
DOWNCAST(Thruster,GmatBase)
%include "Thruster.hpp"

DOWNCAST(ChemicalTank,GmatBase)
%include "ChemicalTank.hpp"
DOWNCAST(ChemicalThruster,GmatBase)
%include "ChemicalThruster.hpp"
DOWNCAST(ElectricTank,GmatBase)
%include "ElectricTank.hpp"
DOWNCAST(ElectricThruster,GmatBase)
%include "ElectricThruster.hpp"
DOWNCAST(NuclearPowerSystem,GmatBase)
%include "NuclearPowerSystem.hpp"
DOWNCAST(SolarPowerSystem,GmatBase)
%include "SolarPowerSystem.hpp"

//// SPICE accessors
%include "SpiceInterface.hpp"
DOWNCAST(SpiceKernelReader,SpiceInterface)
%include "SpiceKernelReader.hpp"
DOWNCAST(SpiceOrbitKernelReader,SpiceInterface)
%include "SpiceOrbitKernelReader.hpp"

DOWNCAST(EventLocator,GmatBase)
%include "EventLocator.hpp"
%include "LocatedEvent.hpp"

DOWNCAST(StopCondition,GmatBase)
%include "StopCondition.hpp"

DOWNCAST(Subscriber,GmatBase)
%include "Subscriber.hpp"
%include "EphemManager.hpp"
DOWNCAST(EphemerisFile,GmatBase)
%include "EphemerisFile.hpp"
DOWNCAST(ReportFile,GmatBase)
%include "ReportFile.hpp"

%include "Interpreter.hpp"
DOWNCAST(ScriptInterpreter,Interpreter)
%include "ScriptInterpreter.hpp"
%include "Validator.hpp"

%include "DynamicLibrary.hpp"

%include "ConfigManager.hpp"

%include "Publisher.hpp"
%include "Sandbox.hpp"

// API Functions Setup, Help, Create, etc
%ignore ProcessParameters; // Internal function - not (yet) exported on Windows, so no GMAT_API macro
%ignore ProcessUpdateParameters; // Internal function - not (yet) exported on Windows, so no GMAT_API macro
%include "APIFunctions.hpp"
%include "APIException.hpp"
%include "HelpSystem.hpp"

%include "Moderator.hpp"
