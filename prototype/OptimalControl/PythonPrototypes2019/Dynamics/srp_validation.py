# srp_validation.py
#
# Validate equations and partial derivatives for SRP.
#
# Revision history
#
# Noble Hatten; 10/05/2018; Created
# Noble Hatten; 10/19/2018; Revamped to do entire srp acceleration, not just shadow function

# imports
import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian

class srp_validation(object):
    def __init__(self):
        """constructor"""
        self.mu_sun = 1.327e11 # gravity parameter of sun (km^3/s^2)
        #self.rad_sun = 695508. # radius of sun (km)       
        #self.rad_body = 6378. # radius of occulting body (km) (Earth)       
        self.rmag_body = 150.0e6 # occulting body semimajor axis about sun (km); assume circular orbit for simplicity (Earth)
        self.rmag_central_body = 100.0e6 # central body semimajor axis about sun (km); assume circular orbit for simplicity (Venus)
        self.vmag_body = np.sqrt(self.mu_sun / self.rmag_body) # constant velocity magnitude of occulting body about sun (km/s)
        self.vmag_central_body = np.sqrt(self.mu_sun / self.rmag_central_body) # constant velocity magnitude of occulting body about sun (km/s)
        self.thetadot_body = np.sqrt(self.mu_sun / self.rmag_body**3) # angular rate of occulting body about sun (rad/s)
        self.thetadot_central_body = np.sqrt(self.mu_sun / self.rmag_central_body**3) # angular rate of central body about sun (rad/s)
        #self.P_s = 1367.0 # solar flux at 1 AU in W/m^2 (units don't match, but it doesn't matter because it is a constant, so derivatives are zero)
        self.P_s = 4.5598211813587384e-006 # solar flux pressure at 1 AU in N/m^2
        #self.C_r = 1.1 # coefficient of reflectivity, assumed constant
        #self.Area = 0.1 # spacecraft SRP area, assumed constant (units don't matter for checking derivatives because constant)
        #self.mass = 1.0 # spacecraft mass, assumed constant (units don't matter for checking derivatives because constant)

        self.rad_sun = 695990.0 # radius of sun (km) (GMAT)
        self.rad_body = 6051.8999999999996 # radius of occulting body (km) (GMAT)
        self.C_r = 1.8 # coefficient of reflectivity, assumed constant (GMAT)
        self.Area = 0.001 # spacecraft SRP area, assumed constant (units don't matter for checking derivatives because constant) (GMAT)
        self.mass = 1.0 # spacecraft mass, assumed constant (units don't matter for checking derivatives because constant) (GMAT)
        self.oneAu = 149597871.0  # one AU in km
        return

    def get_a_srp(self, t, r):
        """return srp acceleration"""
        shadow = self.get_shadow(t, r)
        nu = 1.0 - shadow
        rs = self.get_r_sun(t)
        rsmag = np.linalg.norm(rs)
        rvs = r - rs 
        rvsmag = np.linalg.norm(rvs)
        #a_srp = nu * self.P_s * rsmag**2 * self.C_r * self.Area / self.mass / rvsmag**3 * rvs
        a_srp = nu * self.P_s * self.oneAu**2 * self.C_r * self.Area / self.mass / rvsmag**3 * rvs
        return a_srp

    def get_d_a_srp_d_x(self, t, r):
        """return derivatives of srp acceleration"""
        shadow = self.get_shadow(t, r)
        nu = 1.0 - shadow
        rs = self.get_r_sun(t)
        rsmag = np.linalg.norm(rs)
        rvs = r - rs 
        rvsmag = np.linalg.norm(rvs)
        coef1 = nu * self.P_s * self.oneAu**2 * self.C_r * self.Area / self.mass / rvsmag**3
        coef2 = self.P_s * self.oneAu**2 * self.C_r * self.Area / self.mass / rvsmag**3 * rvs
        eye = np.identity(3)
        d_shadow_d_t, d_shadow_d_r = self.get_d_shadow_d_x(t, r)
        d_nu_d_t = -d_shadow_d_t
        d_nu_d_r = -d_shadow_d_r
        v_sun = self.get_d_r_sun_d_t(t)

        d_a_srp_d_r = coef1 * (eye - 3.0 / rvsmag**2 * np.dot(rvs, np.transpose(rvs))) + coef2 * d_nu_d_r
        d_a_srp_d_t = coef1 * np.dot(eye - 3.0 / rvsmag**2 * np.dot(rvs, np.transpose(rvs)), -v_sun) + coef2 * d_nu_d_t
        return d_a_srp_d_t, d_a_srp_d_r

    def get_shadow(self, t, r):
        """return value of shadow function"""
        dprime = self.get_dprime(t, r)
        rad_sun_app = self.get_rad_sun_app(t, r)
        rad_body_app = self.get_rad_body_app(t, r)
        if (dprime >= (rad_sun_app + rad_body_app)):
            shadow = 0. # no shadow
        elif (dprime <= (rad_body_app - rad_sun_app)):
            shadow = 1.0 # full shadow
        elif (dprime > np.absolute(rad_sun_app - rad_body_app) and dprime < (rad_sun_app + rad_body_app)): # partial shadow case 1
            A = self.get_A(t, r)
            shadow = A / (np.pi * rad_sun_app**2)
        else: # partial shadow case 2
            shadow = rad_body_app**2 / rad_sun_app**2
        return shadow

    def get_d_shadow_d_x(self, t, r):
        """return derivatives w.r.t. time and positions"""
        dprime = self.get_dprime(t, r)
        rad_sun_app = self.get_rad_sun_app(t, r)
        rad_body_app = self.get_rad_body_app(t, r)
        #print('')
        #print('Conditional: ', np.absolute(rad_sun_app - rad_body_app), ' < ', dprime[0,0], ' < ', (rad_sun_app + rad_body_app))
        #print('R_B_prime = ', rad_body_app)
        #print('R_Sun_prime = ', rad_sun_app)
        #print('')
        if (dprime >= (rad_sun_app + rad_body_app)):
            print('No shadow')
            d_shadow_d_t = 0. # no shadow
            d_shadow_d_r = np.array([0.0, 0.0, 0.0])
        elif (dprime <= (rad_body_app - rad_sun_app)):
            print('Full shadow')
            d_shadow_d_t = 0. # full shadow
            d_shadow_d_r = np.array([0.0, 0.0, 0.0]) 
        elif (dprime > np.absolute(rad_sun_app - rad_body_app) and dprime < (rad_sun_app + rad_body_app)): # partial shadow case 1
            print('Partial Shadow case 1')
            A = self.get_A(t, r)
            d_A_d_t, d_A_d_r = self.get_d_A_d_x(t, r)
            d_rad_sun_app_d_t, d_rad_sun_app_d_r = self.get_d_rad_sun_app_d_x(t, r)
            d_shadow_d_t = (1.0 / np.pi) * ((1.0 / rad_sun_app**2) * d_A_d_t - ((2.0 * A) / rad_sun_app**3) * d_rad_sun_app_d_t)
            d_shadow_d_r = (1.0 / np.pi) * ((1.0 / rad_sun_app**2) * d_A_d_r - ((2.0 * A) / rad_sun_app**3) * d_rad_sun_app_d_r)
        else: # partial shadow case 2
            print('Partial shadow case 2')
            d_rad_sun_app_d_t, d_rad_sun_app_d_r = self.get_d_rad_sun_app_d_x(t, r)
            d_rad_body_app_d_t, d_rad_body_app_d_r = self.get_d_rad_body_app_d_x(t, r)
            d_shadow_d_t = ((2.0 * rad_body_app) / rad_sun_app**2) * d_rad_body_app_d_t - ((2.0 * rad_body_app**2) / rad_sun_app**3) * d_rad_sun_app_d_t
            d_shadow_d_r = ((2.0 * rad_body_app) / rad_sun_app**2) * d_rad_body_app_d_r - ((2.0 * rad_body_app**2) / rad_sun_app**3) * d_rad_sun_app_d_r
        return d_shadow_d_t, d_shadow_d_r

    def get_r_body_wrt_sun(self, t):
        """return position of occulting body relative to sun"""
        # assumes circular orbit
        theta = self.thetadot_body * t
        r_body_wrt_sun = self.rmag_body * np.array([[np.cos(theta)], [np.sin(theta)], [0.0]])
        r_body_wrt_sun = np.array([[-4.420918226163e+007], [-9.138079337812e+007], [-3.831724992881e+007]]) # for comparing directly with gmat; central body is the occulting body
        return r_body_wrt_sun

    def get_d_r_body_wrt_sun_d_t(self, t): # checked
        """return time derivative of r_body_wrt_sun"""
        theta = self.thetadot_body * t
        d_r_body_wrt_sun_d_t = self.rmag_body * self.thetadot_body * np.array([[-np.sin(theta)], [np.cos(theta)], [0.0]])
        d_r_body_wrt_sun_d_t = np.array([[3.174243467928e+001], [-1.242724260029e+001], [7.600024592189e+000]]) # for comparing directly with gmat; central body is the occulting body
        return d_r_body_wrt_sun_d_t

    def get_r_central_wrt_sun(self, t):
        """return position of central body relative to sun"""
        # assumes circular orbit
        theta = self.thetadot_central_body * t
        r_central_body_wrt_sun = self.rmag_central_body * np.array([[np.cos(theta)], [np.sin(theta)], [0.0]])
        r_central_body_wrt_sun = np.array([[-4.420918226163e+007], [-9.138079337812e+007], [-3.831724992881e+007]]) # for comparing directly with gmat; central body is the occulting body
        return r_central_body_wrt_sun

    def get_d_r_central_wrt_sun_d_t(self, t): # checked
        """return time derivative of central body relative to sun"""
        theta = self.thetadot_central_body * t
        d_r_central_body_wrt_sun_d_t = self.rmag_central_body * self.thetadot_central_body * np.array([[-np.sin(theta)], [np.cos(theta)], [0.0]])
        d_r_central_body_wrt_sun_d_t = np.array([[3.174243467928e+001], [-1.242724260029e+001], [7.600024592189e+000]]) # for comparing directly with gmat; central body is the occulting body
        return d_r_central_body_wrt_sun_d_t

    def get_r_sun(self, t):
        """return position of sun relative to central body"""
        r_central_body_wrt_sun = self.get_r_central_wrt_sun(t)
        r_sun = - r_central_body_wrt_sun
        r_sun = np.array([[4.42091822616308110e+007], [9.13807933781233580e+007], [3.83172499288109910e+007]]) # for comparing directly with gmat
        return r_sun

    def get_d_r_sun_d_t(self, t): # checked
        """return time derivative of position of sun relative to central body"""
        d_r_central_body_wrt_sun_d_t = self.get_d_r_central_wrt_sun_d_t(t)
        d_r_sun_d_t = - d_r_central_body_wrt_sun_d_t
        d_r_sun_d_t = np.array([[-3.17424346792787690e+001], [1.24272426002935640e+001], [7.60002459218929260e+000]]) # for comparing directly with gmat
        return d_r_sun_d_t

    def get_r_body(self, t):
        """return position of occulting body relative to central body"""
        r_body_wrt_sun = self.get_r_body_wrt_sun(t)
        r_central_body_wrt_sun = self.get_r_central_wrt_sun(t)
        r_body = r_body_wrt_sun - r_central_body_wrt_sun
        return r_body

    def get_d_r_body_d_t(self, t): # checked
        """return time derivative of position of occulting body relative to central body"""
        d_r_body_wrt_sun_d_t = self.get_d_r_body_wrt_sun_d_t(t)
        d_r_central_body_wrt_sun_d_t = self.get_d_r_central_wrt_sun_d_t(t)
        d_r_body_d_t = d_r_body_wrt_sun_d_t - d_r_central_body_wrt_sun_d_t
        return d_r_body_d_t

    def get_d(self, t, r):
        """return vector from spacecraft to sun"""
        r_sun = self.get_r_sun(t)
        d = r_sun - r
        return d

    def get_d_d_d_x(self, t, r): # checked
        """return derivatives of vector from spacecraft to sun"""
        d_r_sun_d_t = self.get_d_r_sun_d_t(t)
        d_d_d_t = d_r_sun_d_t
        d_d_d_r = -np.identity(3)
        return d_d_d_t, d_d_d_r

    def get_s(self, t, r):
        """return vector from occulting body to spacecraft"""
        r_body = self.get_r_body(t)
        s = r - r_body
        return s

    def get_d_s_d_x(self, t, r): # checked
        """return derivatives of vector from occulting body to spacecraft"""
        d_r_body_d_t = self.get_d_r_body_d_t(t)
        d_s_d_t = -d_r_body_d_t
        d_s_d_r = np.identity(3)
        return d_s_d_t, d_s_d_r

    def get_s_(self, t):
        """return vector from occulting body to sun"""
        # Don't think this is used anywhere
        r_sun = self.get_r_sun(t)
        r_body = self.get_r_body(t)
        s_ = r_sun - r_body
        return s_

    def get_rad_sun_app(self, t, r):
        """return apparent radius of sun"""
        d = self.get_d(t, r)
        rad_sun_app = np.arcsin(self.rad_sun / np.linalg.norm(d))
        return rad_sun_app

    def get_d_rad_sun_app_d_x(self, t, r): # checked
        """return derivatives of apparent radius of sun"""
        d = self.get_d(t, r)
        dmag = np.linalg.norm(d)
        d_d_d_t, d_d_d_r = self.get_d_d_d_x(t, r)
        d_rad_sun_app_d_x = ((-self.rad_sun) / (dmag**3 * np.sqrt(1.0 - self.rad_sun**2 / dmag**2))) * np.transpose(d)
        d_rad_sun_app_d_t = np.dot(d_rad_sun_app_d_x, d_d_d_t)
        d_rad_sun_app_d_r = np.dot(d_rad_sun_app_d_x, d_d_d_r)
        return d_rad_sun_app_d_t, d_rad_sun_app_d_r

    def get_rad_body_app(self, t, r):
        """return apparent radius of occulting body"""
        s = self.get_s(t, r)
        rad_body_app = np.arcsin(self.rad_body / np.linalg.norm(s))
        return rad_body_app

    def get_d_rad_body_app_d_x(self, t, r): # checked
        """return derivatives of apparent radius of occulting body"""
        s = self.get_s(t, r)
        smag = np.linalg.norm(s)
        d_s_d_t, d_s_d_r = self.get_d_s_d_x(t, r)
        d_rad_body_app_d_x = ((-self.rad_body) / (smag**3 * np.sqrt(1.0 - self.rad_body**2 / smag**2))) * np.transpose(s)
        d_rad_body_app_d_t = np.dot(d_rad_body_app_d_x, d_s_d_t)
        d_rad_body_app_d_r = np.dot(d_rad_body_app_d_x, d_s_d_r)
        return d_rad_body_app_d_t, d_rad_body_app_d_r

    def get_dprime(self, t, r):
        """return apparent separation of sun and occulting body"""
        s = self.get_s(t, r)
        d = self.get_d(t, r)
        num = - np.dot(np.transpose(s), d)
        den = np.linalg.norm(s) * np.linalg.norm(d)
        dprime = np.arccos(num / den)
        return dprime

    def get_d_dprime_d_x(self, t, r): # checked
        """return derivatives of apparent separation of sun and occulting body"""
        s = self.get_s(t, r)
        d = self.get_d(t, r)
        s_cross_d = np.cross(s[:,0], d[:,0])
        crossmag = np.linalg.norm(s_cross_d)
        minus_s_dot_d = np.dot(-np.transpose(s), d)
        factor = 1.0 / (crossmag**2 + minus_s_dot_d**2)
        d_d_d_t, d_d_d_r = self.get_d_d_d_x(t, r)
        d_s_d_t, d_s_d_r = self.get_d_s_d_x(t, r)
        v_sun = self.get_d_r_sun_d_t(t)
        v_b = self.get_d_r_body_d_t(t)
        d_cross_d_t = np.array([[-v_b[1,0] * d[2,0] + s[1,0] * v_sun[2,0] + v_b[2,0] * d[1,0] - s[2,0] * v_sun[1]],
                                   [-v_b[2,0] * d[0,0] + s[2,0] * v_sun[0,0] + v_b[0,0] * d[2,0] - s[0,0] * v_sun[2]],
                                   [-v_b[0,0] * d[1,0] + s[0,0] * v_sun[1,0] + v_b[1,0] * d[0,0] - s[1,0] * v_sun[0,0]]])
        d_crossmag_d_t = np.dot((s_cross_d / crossmag), d_cross_d_t)
        d_minus_s_dot_d_d_t = -(np.dot(np.transpose(s[:,0]), d_d_d_t) + np.dot(np.transpose(d), d_s_d_t[:,0]))
        d_cross_d_r = np.array([[0.0, d[2,0] + s[2,0], -d[1,0] - s[1,0]],
                                  [-d[2,0] - s[2,0], 0.0, d[0,0] + s[0,0]],
                                  [d[1,0] + s[1,0], -d[0,0] - s[0,0], 0.0]])
        d_crossmag_d_r = np.dot((s_cross_d / crossmag), d_cross_d_r)
        d_minus_s_dot_d_d_r = -(np.dot(np.transpose(s), d_d_d_r) + np.dot(np.transpose(d), d_s_d_r))
        d_dprime_d_t = factor * (minus_s_dot_d * d_crossmag_d_t - crossmag * d_minus_s_dot_d_d_t)
        d_dprime_d_r = factor * (minus_s_dot_d * d_crossmag_d_r - crossmag * d_minus_s_dot_d_d_r)
        return d_dprime_d_t, d_dprime_d_r

    def get_A(self, t, r):
        """return area of overlap of 2 apparent disks"""
        rad_sun_app = self.get_rad_sun_app(t, r)
        rad_body_app = self.get_rad_body_app(t, r)
        dprime = self.get_dprime(t, r)
        c1 = self.get_c1(t, r)
        c2 = self.get_c2(t, r)
        #A_ = rad_sun_app**2 * np.arccos(c1 / rad_sun_app) + rad_body_app**2 * np.arccos((dprime - c1) / rad_body_app) - dprime * c2
        A = rad_sun_app**2 * np.arccos(c1 / rad_sun_app) + rad_body_app**2 * np.arctan2(c2, dprime - c1) - dprime * c2
        return A

    def get_d_A_d_x(self, t, r): # exactly correct if using atan2 to get A; nearly correct if using acos
        """return derivatives of area of overlap of 2 apparent disks"""
        rad_sun_app = self.get_rad_sun_app(t, r)
        d_rad_sun_app_d_t, d_rad_sun_app_d_r = self.get_d_rad_sun_app_d_x(t, r)
        rad_body_app = self.get_rad_body_app(t, r)
        d_rad_body_app_d_t, d_rad_body_app_d_r = self.get_d_rad_body_app_d_x(t, r)
        dprime = self.get_dprime(t, r)
        d_dprime_d_t, d_dprime_d_r = self.get_d_dprime_d_x(t, r)
        c1 = self.get_c1(t, r)
        d_c1_d_t, d_c1_d_r = self.get_d_c1_d_x(t, r)
        c2 = self.get_c2(t, r)
        d_c2_d_t, d_c2_d_r = self.get_d_c2_d_x(t, r)
        # frac = c1 / rad_sun_app
        d_frac_d_t = (1.0 / rad_sun_app) * d_c1_d_t - (c1 / rad_sun_app**2) * d_rad_sun_app_d_t
        d_frac_d_r = (1.0 / rad_sun_app) * d_c1_d_r - (c1 / rad_sun_app**2) * d_rad_sun_app_d_r
        # atan = atan2(c2, dprime-c1)
        d_atan_d_t = (1.0 / (c2**2 + (dprime - c1)**2)) * ((dprime - c1) * d_c2_d_t - c2 * (d_dprime_d_t - d_c1_d_t))
        d_atan_d_r = (1.0 / (c2**2 + (dprime - c1)**2)) * ((dprime - c1) * d_c2_d_r - c2 * (d_dprime_d_r - d_c1_d_r))
        
        d_A_d_t = 2.0 * rad_sun_app * np.arccos(c1 / rad_sun_app) * d_rad_sun_app_d_t - (rad_sun_app**2 / np.sqrt(1.0 - c1**2 / rad_sun_app**2)) * d_frac_d_t + 2.0 * rad_body_app * np.arctan2(c2, dprime-c1) * d_rad_body_app_d_t + rad_body_app**2 * d_atan_d_t - dprime * d_c2_d_t - c2 * d_dprime_d_t
        d_A_d_r = 2.0 * rad_sun_app * np.arccos(c1 / rad_sun_app) * d_rad_sun_app_d_r - (rad_sun_app**2 / np.sqrt(1.0 - c1**2 / rad_sun_app**2)) * d_frac_d_r + 2.0 * rad_body_app * np.arctan2(c2, dprime-c1) * d_rad_body_app_d_r + rad_body_app**2 * d_atan_d_r - dprime * d_c2_d_r - c2 * d_dprime_d_r
        return d_A_d_t, d_A_d_r

    def get_c1(self, t, r):
        """return c1"""
        rad_sun_app = self.get_rad_sun_app(t, r)
        rad_body_app = self.get_rad_body_app(t, r)
        dprime = self.get_dprime(t, r)
        num = dprime**2 + rad_sun_app**2 - rad_body_app**2
        den = 2. * dprime
        c1 = num / den
        return c1

    def get_d_c1_d_x(self, t, r): # checked
        """return derivatives of c1"""
        rad_sun_app = self.get_rad_sun_app(t, r)
        d_rad_sun_app_d_t, d_rad_sun_app_d_r = self.get_d_rad_sun_app_d_x(t, r)
        rad_body_app = self.get_rad_body_app(t, r)
        d_rad_body_app_d_t, d_rad_body_app_d_r = self.get_d_rad_body_app_d_x(t, r)
        dprime = self.get_dprime(t, r)
        d_dprime_d_t, d_dprime_d_r = self.get_d_dprime_d_x(t, r)
        d_c1_d_t = 0.5 * ((-1.0/dprime**2) * (dprime**2 + rad_sun_app**2 - rad_body_app**2) * d_dprime_d_t + 
                          (2.0 / dprime) * (dprime * d_dprime_d_t + rad_sun_app * d_rad_sun_app_d_t - rad_body_app * d_rad_body_app_d_t))
        d_c1_d_r = 0.5 * ((-1.0/dprime**2) * (dprime**2 + rad_sun_app**2 - rad_body_app**2) * d_dprime_d_r + 
                          (2.0 / dprime) * (dprime * d_dprime_d_r + rad_sun_app * d_rad_sun_app_d_r - rad_body_app * d_rad_body_app_d_r))
        return d_c1_d_t, d_c1_d_r

    def get_c2(self, t, r):
        """return c2"""
        rad_sun_app = self.get_rad_sun_app(t, r)
        c1 = self.get_c1(t, r)
        c2 = np.sqrt(rad_sun_app**2 - c1**2)
        return c2

    def get_d_c2_d_x(self, t, r): # checked
        """return derivatives of c2"""
        rad_sun_app = self.get_rad_sun_app(t, r)
        d_rad_sun_app_d_t, d_rad_sun_app_d_r = self.get_d_rad_sun_app_d_x(t, r)
        c1 = self.get_c1(t, r)
        d_c1_d_t, d_c1_d_r = self.get_d_c1_d_x(t, r)
        c2 = self.get_c2(t, r)
        d_c2_d_t = (1.0 / c2) * (rad_sun_app * d_rad_sun_app_d_t - c1 * d_c1_d_t)
        d_c2_d_r = (1.0 / c2) * (rad_sun_app * d_rad_sun_app_d_r - c1 * d_c1_d_r)
        return d_c2_d_t, d_c2_d_r

if __name__ == "__main__":
    np.set_printoptions(precision=16)
    # define problem

    # time (s)
    t = 955.0

    # position vector w.r.t. central body (km)
    #r = np.array([[50.e6 + 6378.0], [10000000.0], [0.0]]) # no shadow
    #r = np.array([[50.e6 + 6378.0 + 100.0], [10.0], [-10.0]]) # partial shadow case 1
    #r = np.array([[50.e6 + 6378.0 + 10000000.0], [0.0], [0.0]]) # partial shadow case 2

    r = np.array([[3675.0], [-6100.0], [0.0]]) # GMAT checking

    # create calculation object
    srp = srp_validation()

    # calculate shadow function
    p = srp.get_shadow(t, r)

    # calculate acceleration
    a = srp.get_a_srp(t, r)

    # calculate analytical derivatives
    d_p_d_t, d_p_d_r = srp.get_d_shadow_d_x(t, r)

    d_a_d_t, d_a_d_r = srp.get_d_a_srp_d_x(t, r)
    

    # calculate automatic differentiation
    #d_p_d_t_fun = jacobian(srp.get_shadow, 0)
    #d_p_d_r_fun = jacobian(srp.get_shadow, 1)
    #d_p_d_t_ad = d_p_d_t_fun(t, r)
    #d_p_d_r_ad = d_p_d_r_fun(t, r)

    #d_a_d_t_fun = jacobian(srp.get_a_srp, 0)
    #d_a_d_r_fun = jacobian(srp.get_a_srp, 1)
    #d_a_d_t_ad = d_a_d_t_fun(t, r)
    #d_a_d_r_ad = d_a_d_r_fun(t, r)

    # check intermediate results
    #val = srp.get_A(t, r)
    #dvaldt, dvaldr = srp.get_d_A_d_x(t, r)
    #dvaldt_fun = jacobian(srp.get_A, 0)
    #dvaldr_fun = jacobian(srp.get_A, 1)
    #dvaldt_ad = dvaldt_fun(t, r)
    #dvaldr_ad = dvaldr_fun(t, r)

    # print results
    #print('Time = ', t)
    #print('Position = ', np.transpose(r[:,0]))
    #print('value = ', val)
    #print('d [value] / d [t] analytical = ', dvaldt)
    #print('d [value] / d [t] AD = ', dvaldt_ad)
    #print('d [value] / d [r] analytical = ', dvaldr)
    #print('d [value] / d [r] AD = ', dvaldr_ad)

    print('Time (s) = ', t)
    print('Position (km) = ', np.transpose(r[:,0]))
    print('Shadow = ', p)
    print('d [Shadow] / d [t] analytical (1/s) = ', d_p_d_t)    
    #print('d [Shadow] / d [t] AD = ', d_p_d_t_ad)
    print('d [Shadow] / d [r] analytical (1/km) = ', d_p_d_r)
    #print('d [Shadow] / d [r] AD = ', np.transpose(d_p_d_r_ad[:,0]))
    print('')
    print('')
    print('')
    print('')
    print('')
    print('acc (km/s^2) = ', np.transpose(a[:,0]) / 1000.)
    print('')
    print('d [Acc] / d [t] analytical (km/s^3) = ', d_a_d_t / 1000.)
    print('')
    #print('d [Acc] / d [t] AD = ', d_a_d_t_ad)
    print('')
    print('d [Acc] / d [r] analytical (1/s^2) = ', d_a_d_r)
    print('')
    #print('d [Acc] / d [r] AD = ', d_a_d_r_ad)





    #x = np.array([0.,10.,20.,30.,40.,50.,60.,70.,80.,90.,100.,110.,120.,130.,140.,150.,160.,170.,180.,190.,200.])
    #y = np.array([4455.,3590.,2900.,2360.,1930.,1590.,1320.,1100.,925.,775.,655.,555.,470.,400.,340.,285.,240.,205.,170.,140.,115])
    #z = np.polyfit(x,y,5)