#ifndef Driver_hpp
#define Driver_hpp

#include "CsaltDriver.hpp"
#include "PointObj.hpp"
#include "PathObj.hpp"

// Main driver class for CSALT
class Driver : public CsaltDriver
{
public:
    // Basic constructor.
    // Put problem name as an additional argument to base class
    Driver(std::string problemName) : CsaltDriver(problemName) {}

    // Basic destructor
    ~Driver() {}
protected:
    virtual void SetPointPathAndProperties();
    virtual void SetupPhases();
};

#endif