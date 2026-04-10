This directory contains unit tests for stateconv.

Prerequisites
=============

NOTE: These tests use the MATLAB xUnit Test Framework from the MATLAB
File Exchange. MATLAB R2013a ships with a built-in test framework, but
as of this writing this is very new. We should wait a while before
trying to switch.

Before running the test suite, you need to download the MATLAB xUnit
Test Framework and add it to your path.

Download from:
    http://www.mathworks.com/matlabcentral/fileexchange/22846-matlab-xunit-test-framework

Add to MATLAB path:

    >> addpath('/path/to/matlab_xunit/xunit');

Running tests
=============

To run all tests:

    >> runtests

To run tests in an individual suite (for example):

    >> runtests testMee

To run an individual test case within a suite (for example):

    >> runtests testMee:testCart2MeeCircularEqPro

To see all runtime options:

    >> help runtests
