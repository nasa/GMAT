#ifndef PathObj_hpp
#define PathObj_hpp

#include "CsaltDriver.hpp"

// UserPathFunction base class has methods for setting up the system
// dynamics and its derivatives as well as any path constraints
class PathObj : public UserPathFunction
{
public:
    // Basic constructor - no additional inputs
    PathObj() : UserPathFunction() {}

    // Basic destructor
    ~PathObj() {}

    virtual void EvaluateFunctions();
    virtual void EvaluateJacobians();
};

#endif