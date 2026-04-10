"""
A collection of test functions used to check some basic math in the Python 
interface
"""
import math


# RSS of the first size elements of arr
def mag(arr, size):
   rv = 0
   isize = int(size)
   
   if len(arr) < isize:
      raise ValueError("Error in inputs for mag: the array size must be >= the int(size)")
   
   for i in range(isize):
      rv = rv + arr[i]**2
      
   return math.sqrt(rv)
   
# Magnitude multiplied by a number
def magtimes(mult, arr, size):
   rv = mag(arr, size)
   return mult * rv
   
# vector cross product
def cross(a,b):
   if len(a) < 3 or len(b) < 3:
      raise ValueError("Error in inputs for cross: two array inputs of size >= 3 are required")
      
   c = [0.0,0.0,0.0]
   
   c[0] = a[1] * b[2] - a[2] * b[1] 
   c[1] = a[2] * b[0] - a[0] * b[2] 
   c[2] = a[0] * b[1] - a[1] * b[0] 

   return c
   
# vector dot product
def dot(a,b):
   if len(a) < 3 or len(b) < 3:
      raise ValueError("Error in inputs for dot: two array inputs of size >= 3 are required")
   c = a[0] * b[0] + a[1] * b[1] + a[2] * b[2]
   return c
   