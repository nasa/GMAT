#ifndef CsaltDriver_hpp
#define CsaltDriver_hpp

#include "csalt.hpp"

class CsaltDriver {
public:
   CsaltDriver(const std::string &testName);
   CsaltDriver(const std::string &testName, const Integer snoptConsoleOutputLevel);
   virtual ~CsaltDriver();
   CsaltDriver& operator=(const CsaltDriver& driver);

   enum Verbosity
   {
      SILENT,
      BASIC,
      VERBOSE,
      VERBOSE_DEBUG
   };

   virtual Integer Run();
protected:
   /// Name for the test case, used to set file names and output identity
   std::string testName;
   /// Output verbosity setting: silent = 0, basic = 1, verbose = 2
   Verbosity   verbosity;

   Trajectory *traj;
   UserPathFunction *pathObject;
   UserPointFunction *pointObject;
   std::vector<OptimalControlFunction*> ocfObjects;
   Real costLowerBound;
   Real costUpperBound;
   Rvector majorOptimalityTolerances;
   IntegerArray majorIterationsLimits;
   IntegerArray totalIterationsLimits;
   Rvector feasibilityTolerances;
   Integer maxMeshRefinementCount;
   StringArray optimizationMode;
   std::vector<Phase*> phaseList;
   bool generateOptimizationOutput;
   std::string optimizationOutputFile;
   bool writeControlHistory;
   std::string controlHistoryFile;
   Integer snoptConsoleOutputLevel;

   // A useful value
   static const Real INF;

   // Required method that sets the path & point objects, & other run properties
   virtual void SetPointPathAndProperties() = 0;
   // Required method to set up the phase(s)
   virtual void SetupPhases() = 0;
};

#endif