import numpy as np
from numpy import sin, cos, matmul
from numpy.linalg import norm
from numpy.matlib import identity

# for quaternion to roll, pitch & yaw computations
from math import asin, atan2

# module Attitude types is stripped down to what is needed for COSMOS - GMAT
# interface for Moon Express. This interface passes time (TBS representation),
# spacecraft attitude (represented by a quaternion), and
# angular velocty vector. This information will be used to define the 
# interface, and on the GMAT scripting side to propagate attitude kinematics 
# over short intervals

class Quaternion:

    def __init__ (self,q=[0,0,0,1]):
        self.q = np.array([ [q[0]], [q[1]], [q[2]], [q[3]] ])
    # end __init__

    def Set (self,q):
        if len(q) != 4:
            print ("raise exception here -- Set")
        else:
#            print ("self.q  = ",self.q)
#            print ("input q = ",q)
            self.q = np.array([ [q[0]], [q[1]], [q[2]], [q[3]] ])
#            print ("new self.q = ",self.q)
            self.Normalize()

    # end Set

    def Get(self):
        q = [float(self.q[0][0]),
             float(self.q[1][0]),
             float(self.q[2][0]),
             float(self.q[3][0]) ]
        return q
    # end Get

    def GetRPY(self):

        # get quaternion elements as scalars
        q1 = float(self.q[0][0])
        q2 = float(self.q[1][0])
        q3 = float(self.q[2][0])
        q4 = float(self.q[3][0])

        # compute DCM elements used in 321 rotation sequence
        a11 = q1*q1 - q2*q2 - q3*q3 + q4*q4
        a12 = 2.0*(q1*q2 + q3*q4)
        a13 = 2.0*(q1*q3 - q2*q4)
        a23 = 2.0*(q2*q3 + q1*q4)
        a33 = -q1*q1 - q2*q2 + q3*q3 + q4*q4

        # compute roll, pitch and yaw
        yaw   = atan2(a12,a11) # 1st rotation (3)
        pitch = asin (-a13)    # 2nd rotation (2)
        roll  = atan2(a23,a33) # 3rd rotation (1)

        return [roll, pitch, yaw]

    # end GetRPY

    def Normalize (self):
        # assumes length of quaternion has been validated in constructor
        length = norm(self.q)
        if (length != 1.0):
            self.q[0][0] = float(self.q[0][0]) /length
            self.q[1][0] = float(self.q[1][0]) /length
            self.q[2][0] = float(self.q[2][0]) /length
            self.q[3][0] = float(self.q[3][0]) /length
    # end Normalize(self)

# propagation functions
# Rate() for numeric integration
# Propagate() for analytic propagation for (small) time step

    def Rate (self, w):
        # w is angular velocity vector projected onto body frame axes
        # returns 4 vector representing dq/dt.
        # should only use with numeric integrators, use function Propagate for 
        # analytical solution over short time steps.
        if len(w) != 3:
            print ("raise exception here -- Rate")
            return 
        else:
            omega = OmegaMatrix(w)
            qdot = np.matmul(omega,self.q)
            return qdot
    # end Rate

    def Propagate (self, w, deltaT, update = False):
        # propagates attitude for delta seconds given angular velocity w
        # set Update to True if you want self to be updated, otherwise
        # the new quaternion is returned from the function
        # reference: GRO Dynamics Simulator Requirements and Mathematical
        #            Specifications, eqn. 5-1, p. 5-46

        # set basic parameters
        wMag = norm(w)
        I = identity(4)
        omega = OmegaMatrix(w)

        #compute rotation
        angle = wMag*deltaT/2.0
        rotation = cos(angle)*I + (1.0/wMag)*sin(angle)*omega

        #compute new quaternion and update if requested
        q = matmul(rotation,self.q)
        qnew = [ float(q[0]), float(q[1]), float(q[2]), float(q[3]) ]
        if update:
            self.Set(qnew)
        else:                           
            return qnew
    # end Propagate

    def ToString(self,precision = 8):
        qrow = self.q.reshape(1,4)
        q1 = float(qrow[0][0])
        q2 = float(qrow[0][1])
        q3 = float(qrow[0][2])
        q4 = float(qrow[0][3])
        s = '{0:10.8f} {1:10.8f} {2:10.8f} {3:10.8f} '.format(q1,q2,q3,q4)
        return s;
    # end ToString

# end Quaternion

# helper functions

#def Normalize (v3):
#    # v is a 3-vector, function returns corresponding unit vector
#    if len(v) != 3:
#        print ("raise exception here, normalize V3")
#    else:
#        norm = lp.norm(v)
#        return np.array([v[0]/norm,v[1]/norm,v[3]/norm])
# end Normalize(v3)

def OmegaMatrix (w):
    # computes skew matrix for quaternion rate computation
    if len(w) != 3:
        print ("raise exception here -- Rate")
        return
    else:
        return np.array([
                         [0.0,    w[2], -w[1], w[0]],
                         [-w[2],  0.0,   w[0], w[1]],
                         [ w[1], -w[0],  0.0,  w[2]],
                         [-w[0], -w[1], -w[2], 0.0 ]
                        ] )
    #end OmegaMatrix

#end OmegaMatrix
