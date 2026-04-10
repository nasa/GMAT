# DecVecDerivatives.py

# Module (not a class) for holding functions for calculating the time derivatives of state representations.

# Revision history:
# 2019-03-06; Noble Hatten; Created

import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian
from enum import Enum # requires python 3.4+
from stateTypes import stateTypes

def DecVecTimeDerivatives(stateIn, inputType, inputAxes, inputOrigin):
    """
    Calculate time derivatives of decision vector
    """
    # @param stateIn Input state in inputType
    # @param inputType The type of the input state (stateTypes enum)
    # @param inputAxes The axes in which stateIn is written (string)
    # @param inputOrigin The origin of the axes in which stateIn is written (string)
    # @return partialDecVecPartialTime Partial derivative of stateIn w.r.t. time

    # What are examples of times when this would be non-zero?

    partialDecVecWrtTime = np.zeros(6)

    

    return partialDecVecWrtTime