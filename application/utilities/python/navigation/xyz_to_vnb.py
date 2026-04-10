
import numpy as np
from numpy.linalg import norm

def xyz_to_vnb(state):

    r = state[0:3]
    v = state[3:6]

    n = np.cross(r,v)
    b = np.cross(v,n)
    
    m_cart_vnb = np.array([v/norm(v), n/norm(n), b/norm(b)])
    
    zeros33 = np.zeros((3,3))
    
    rv = np.hstack((m_cart_vnb, zeros33))
    vr = np.hstack((zeros33, m_cart_vnb))       
    
    m_cart_vnb = np.vstack((rv, vr))
        
    return m_cart_vnb
    