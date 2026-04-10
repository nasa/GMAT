# -*- coding: utf-8 -*-
"""
Created on Tue Aug 29 11:36:41 2017

@author: djc
"""

import LagrangeInterpolator as lint;

x = [0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0]

dep = [[0.0, 0.0], [2.0, 0.01], [4.0, 0.04], [6.0, 0.09], [8.0, 0.16], [10.0,0.25], [12.0,0.36], [14.0,0.49], [16.0,0.64], [18.0,0.81], [20.0,1.0]]

for i in range(0, 10):
   val = i + 0.5
   ival = lint.Interpolate(val, dep, x)
   print(ival)
   