#ifndef OcfObj1_hpp
#define OcfObj1_hpp

#include "CsaltDriver.hpp"

// Optional class(es) to separate constraints and let CSALT take care of
// the order and indexing. Useful for multi-phase problems with many constraints
class OcfObj1 : public OptimalControlFunction
{
public:
    // Basic constructor where we set OptimalControlFunction parameters and constraint bounds
    OcfObj1(std::string funcName) : OptimalControlFunction(funcName) {
        setupOCF();
    }

    // Basic destructor
    ~OcfObj1() {}

    // Required method to setup the OCF
    virtual void setupOCF();
    // Required method to add constraints
    virtual Rvector EvaluateFunctions();
    // Required method to scale if desired
    virtual void ScaleFunctionBounds();
};

#endif