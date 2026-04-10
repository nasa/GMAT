#ifndef OcfObj2_hpp
#define OcfObj2_hpp

#include "CsaltDriver.hpp"

class OcfObj2 : public OptimalControlFunction
{
public:
    OcfObj2(std::string funcName) : OptimalControlFunction(funcName) {
        setupOCF();
    }

    ~OcfObj2() {}

    virtual void setupOCF();
    virtual Rvector EvaluateFunctions();
    virtual void ScaleFunctionBounds();
};

#endif