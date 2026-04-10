# -*- coding: utf-8 -*-
"""
A set of Python functions that have mixed input and output expectations

Created on Thu Sep  3 11:30:28 2015

@author: djc
"""

# Lowercases an input string
def toLower(s):
    if type(s) is not str:
        raise ValueError("Wrong input type: the parameter must be a string");
    return s.lower()

# Uppercases an input string
def toUpper(s):
    if type(s) is not str:
        raise ValueError("Wrong input type: the parameter must be a string");
    return s.upper()

# Combines 2 strings
def concat(a,b):
    if type(a) is not str or type(b) is not str:
        raise ValueError("Wrong input type: the parameters must be strings");
    return a + b

# Returns the starting location of b in a
def strFind(a, b):
    retval = -1
    
    if type(a) is not str or type(b) is not str:
        raise ValueError("Wrong input type: the parameters must be strings");
    
    if b in a:
        retval = a.find(b)
        
    return retval
