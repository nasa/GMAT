import numpy as np

#Currently passing indVar into functions, rather than having a global variable.
#indVar = []

#def SetIndependentVariables(newIndVar):
#  indVar = newIndVar

order = 4      # Polynomial degree of the interpolation
startIndex = 0 # Index for the points used in the interpolation

class InterpolatorException(Exception):
   def __init__(self,*args,**kwargs):
      Exception.__init__(self,*args,**kwargs)

#Get the Lagrange products used in Lagrange Interpolation
def GetLagrangeProducts(interpPoint, indVar):
   global order
   global startIndex

   #Check that interpPoint is within range of indVar
   if interpPoint < indVar[0] or interpPoint > indVar[len(indVar) - 1]:
      raise InterpolatorException("Error: Requested interpolation point is not contained in the vector of independent variables")
   
   if order > len(indVar):
      order = len(indVar)

   #Compute the Lagrange products
   #lagProducts = np.ones(len(indVar))
   lagProducts = np.ones(order+1)
   
   # Set the start index so that the point is centered, as much as possible, in the range
   i = 0
   startIndex = 0
   while (indVar[i] < interpPoint):
      if i > order / 2 :
         startIndex = startIndex + 1
      i = i + 1

   if (startIndex + order + 1 > len(indVar)):
      startIndex = len(indVar) - order - 1

   # print("Start index for ", interpPoint, " = ", startIndex)

   i = 0
   while i <= order:  # len(indVar):
      j = 0
      while j <= order:  # len(indVar):
         if i != j:
            lagProducts[i] = lagProducts[i]*(interpPoint - indVar[startIndex + j])/(indVar[startIndex + i] - indVar[startIndex + j])
         j += 1
      i += 1

   return lagProducts

#Perform the interpolation
def Interpolate(interpPoint, funcValues, indVar):
   global order
   global startIndex

   rows = order + 1  # len(funcValues)
   cols = len(funcValues[0])

   #Check that the input is a column vector with correct number of rows
   # if rows != len(indVar):
   if rows > len(indVar):
      raise InterpolatorException("Error: Number of rows in input must not be more than the size of the independent variable vector")

   #Perform the interpolation
   interpValues = np.zeros(cols)
   # print("Interpolating to ", interpPoint)
   lagProducts = GetLagrangeProducts(interpPoint, indVar)

   # print("LagrangeProducts:  ", lagProducts)

   # funcIdx = 0
   funcIdx = startIndex
   lagIndex = 0
   while funcIdx < startIndex + order + 1:  # len(indVar):
      # print("   Function values: ", funcValues[funcIdx])
      interpValues = interpValues + np.array(funcValues[funcIdx])*lagProducts[lagIndex]
      funcIdx += 1
      lagIndex += 1

   # print("Interpolation values: ", interpValues)
   return interpValues

def InterpolateArray(interpPoints, funcValues, indVar):
   interpValues = np.zeros((len(interpPoints), len(funcValues[0])))

   rowIdx = 0
   while rowIdx < len(interpPoints):
      interpValues[rowIdx] = Interpolate(interpPoints[rowIdx], funcValues, indVar)
      rowIdx += 1

   interpValues = interpValues.tolist()
   return interpValues
