Unit testing on Windows using GCC compiler:
1. Copy BuildEnv.mk from ./build/windows to unit test directory.
1. Modify BuildEnv.mk to point to GMAT base location.
2. In each unit test makefile, include ../BuildEnv.mk.
