#ifndef PointObj_hpp
#define PointObj_hpp

#include "CsaltDriver.hpp"

// UserPointFunction base class has methods for setting constraints
// and bounds between each phase on the dynamics and their derivatives
class PointObj : public UserPointFunction
{
public:
    // Basic constructor - no additional inputs
    PointObj() : UserPointFunction() {}

    // Basic destructor
    ~PointObj() {}

    virtual void EvaluateFunctions();
    virtual void EvaluateJacobians();
};

#endif