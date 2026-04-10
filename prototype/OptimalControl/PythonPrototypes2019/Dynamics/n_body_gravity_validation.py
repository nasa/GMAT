# n_body_gravity_validation.py
#
# Validate equations and partial derivatives for n-body gravity accelerations.
#
# Revision history
#
# Noble Hatten; 10/18/2018; Created

# imports
import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian

class n_body_gravity_validation(object):
    def __init__(self):
        """constructor"""
        self.mu_sun = 1.327e11 # gravity parameter of sun (km^3/s^2)
        self.mu_third_body = 3.986e5 # gravity parameter of third body (km^3/s^2)
        self.mu_central_body = 3.986e5 # gravity parameter of central body (km^3/s^2)
        self.rmag_third_body = 150.0e6 # third body semimajor axis about sun (km); assume circular orbit for simplicity (Earth)
        self.rmag_central_body = 150.0e6 # central body semimajor axis about sun (km); assume circular orbit for simplicity (Earth)
        self.vmag_third_body = np.sqrt(self.mu_sun / self.rmag_third_body) # constant velocity magnitude of third body about sun (km/s)
        self.vmag_central_body = np.sqrt(self.mu_sun / self.rmag_central_body) # constant velocity magnitude of central body about sun (km/s)
        self.thetadot_third_body = np.sqrt(self.mu_sun / self.rmag_third_body**3) # angular rate of third body about sun (rad/s)
        self.thetadot_central_body = np.sqrt(self.mu_sun / self.rmag_central_body**3) # angular rate of central body about sun (rad/s)
        self.theta0_third_body = np.deg2rad(0.5) # initial angular position of third body (rad)
        self.theta0_central_body = np.deg2rad(0.0) # initial anuglar position of central body (rad)
        return

    def get_accel(self, t, r):
        """get gravitational acceleration acting on spacecraft due to central body and third body"""

        # central body
        rmag = np.linalg.norm(r)
        accel = (-self.mu_central_body / rmag**3) * r

        # third body (just assume 1 for now)
        # rk: position of third-body w.r.t. central body
        r_central_body = self.get_r_central_wrt_sun(t)
        r_third_body = self.get_r_third_body_wrt_sun(t)
        rk = r_third_body - r_central_body
        direct = (rk - r) / (np.linalg.norm(rk - r)**3)
        indirect = -rk / np.linalg.norm(rk)**3
        accel = accel + self.mu_third_body * (direct + indirect)
        return accel

    def get_d_accel_d_t(self, t, r):
        """get time derivative of gravitational acceleration acting on spacecraft due to central body and third body"""
        v_central_body = self.get_d_r_central_wrt_sun_d_t(t)
        v_third_body = self.get_d_r_third_body_wrt_sun_d_t(t)
        vk = v_third_body - v_central_body
        r_central_body = self.get_r_central_wrt_sun(t)
        r_third_body = self.get_r_third_body_wrt_sun(t)
        rk = r_third_body - r_central_body
        rkmag = np.linalg.norm(rk)
        rkhat = rk / rkmag
        rkmr = rk - r
        rkmrmag = np.linalg.norm(rkmr)
        rkmrhat = rkmr / rkmrmag
        eye = np.identity(3)
        d_accel_d_t = (1.0 / rkmrmag**3) * np.dot(eye - 3.0 * np.dot(rkmrhat, np.transpose(rkmrhat)), vk)
        d_accel_d_t = d_accel_d_t - (1.0 / rkmag**3) * np.dot(eye - 3.0 * np.dot(rkhat, np.transpose(rkhat)), vk)
        d_accel_d_t = self.mu_third_body * d_accel_d_t
        return d_accel_d_t

    def get_d_accel_d_r(self, t, r):
        """get position derivative of gravitational acceleration acting on spacecraft due to central body and third body"""
        rmag = np.linalg.norm(r)
        r_central_body = self.get_r_central_wrt_sun(t)
        r_third_body = self.get_r_third_body_wrt_sun(t)
        rk = r_third_body - r_central_body
        rkmr = rk - r
        rkmrmag = np.linalg.norm(rkmr)
        d_accel_d_r = -np.dot((self.mu_central_body/rmag**3) + (self.mu_third_body/rkmrmag**3), np.identity(3)) + 3.0 * ((self.mu_central_body/rmag**5) * np.dot(r, np.transpose(r)) + (self.mu_third_body/rkmrmag**5) * np.dot(rkmr, np.transpose(rkmr)))
        return d_accel_d_r

    def get_r_third_body_wrt_sun(self, t):
        """return position of third body relative to sun"""
        # assumes circular orbit
        theta = self.theta0_third_body + self.thetadot_third_body * t
        r_third_body_wrt_sun = self.rmag_third_body * np.array([[np.cos(theta)], [np.sin(theta)], [0.0]])
        return r_third_body_wrt_sun

    def get_d_r_third_body_wrt_sun_d_t(self, t): # checked
        """return time derivative of r_third_body_wrt_sun"""
        theta = self.theta0_third_body + self.thetadot_third_body * t
        d_r_third_body_wrt_sun_d_t = self.rmag_third_body * self.thetadot_third_body * np.array([[-np.sin(theta)], [np.cos(theta)], [0.0]])
        return d_r_third_body_wrt_sun_d_t

    def get_r_central_wrt_sun(self, t):
        """return position of central body relative to sun"""
        # assumes circular orbit
        theta = self.theta0_central_body + self.thetadot_central_body * t
        r_central_body_wrt_sun = self.rmag_central_body * np.array([[np.cos(theta)], [np.sin(theta)], [0.0]])
        return r_central_body_wrt_sun

    def get_d_r_central_wrt_sun_d_t(self, t): # checked
        """return time derivative of central body relative to sun"""
        theta = self.theta0_central_body + self.thetadot_central_body * t
        d_r_central_body_wrt_sun_d_t = self.rmag_central_body * self.thetadot_central_body * np.array([[-np.sin(theta)], [np.cos(theta)], [0.0]])
        return d_r_central_body_wrt_sun_d_t


if __name__ == "__main__":
    np.set_printoptions(precision=16)

    # time (s)
    t = 955.0

    # position vector w.r.t. central body (km)
    r = np.array([[6378.0 + 10000.0], [0.0], [0.0]])

    # create calculation object
    grav = n_body_gravity_validation()

    # calculate acceleration
    acc = grav.get_accel(t, r)

    # analytical derivatives
    d_acc_d_t = grav.get_d_accel_d_t(t, r)
    d_acc_d_r = grav.get_d_accel_d_r(t, r)

    # auto derivatives
    d_acc_d_t_fun = jacobian(grav.get_accel, 0)
    d_acc_d_r_fun = jacobian(grav.get_accel, 1)
    d_acc_d_t_ad = d_acc_d_t_fun(t, r)
    d_acc_d_r_ad = d_acc_d_r_fun(t, r)

    print('Time = ', t)
    print('Position = ', np.transpose(r[:,0]))
    print('Acceleration = ', np.transpose(acc[:,0]))
    print('')
    print('d acc / d r analytical = ', d_acc_d_r)
    print('')
    print('d acc / d r auto = ', d_acc_d_r_ad)
    print('')
    print('d acc / d t analytical = ', np.transpose(d_acc_d_t[:,0]))
    print('')
    print('d acc / d t auto = ', d_acc_d_t_ad)