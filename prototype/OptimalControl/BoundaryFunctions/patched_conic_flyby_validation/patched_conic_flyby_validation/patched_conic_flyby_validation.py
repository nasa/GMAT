# patched_conic_flyby_validation.py
#
# Validate equations and partial derivatives for patched conic flyby constraints.
#
# Revision history
#
# Noble Hatten; 10/009/2018; Created

# imports
import autograd
import autograd.numpy as np
from autograd import grad
from autograd import jacobian

class patched_conic_flyby_validation(object):
    def __init__(self):
        """constructor"""
        self.mu_sun = 1.327e11 # gravity parameter of sun (km^3/s^2)
        self.mu_body = 3.986e5 # gravity parameter of flyby body (km^3/s^2)
        self.rmag_body = 150.0e6 # occulting body semimajor axis about sun (km); assume circular orbit for simplicity (Earth)
        self.thetadot_body = np.sqrt(self.mu_sun / self.rmag_body**3) # angular rate of occulting body about sun (rad/s)
        self.vmag_body = np.sqrt(self.mu_sun / self.rmag_body) # constant velocity magnitude of occulting body about sun (km/s)
        return

    def get_r_body_wrt_sun(self, t):
        """return position of flyby body relative to sun"""
        # assumes circular orbit
        theta = self.thetadot_body * t
        r_body_wrt_sun = self.rmag_body * np.array([np.cos(theta), np.sin(theta), 0.0])
        return r_body_wrt_sun

    def get_d_r_body_wrt_sun_d_t(self, t):
        """return time derivative of r_body_wrt_sun"""
        theta = self.thetadot_body * t
        d_r_body_wrt_sun_d_t = self.rmag_body * self.thetadot_body * np.array([-np.sin(theta), np.cos(theta), 0.0])
        return d_r_body_wrt_sun_d_t

    def get_d2_r_body_wrt_sun_d_t2(self, t):
        """return time derivative of r_body_wrt_sun"""
        theta = self.thetadot_body * t
        d_r2_body_wrt_sun_d_t2 = -self.rmag_body * self.thetadot_body**2 * np.array([np.cos(theta), np.sin(theta), 0.0])
        return d_r2_body_wrt_sun_d_t2

    def get_vinf(self, t, v):
        """return v infinity vector"""
        # v is relative to the central body, NOT the flyby body
        vb = self.get_d_r_body_wrt_sun_d_t(t)
        vinf = v - vb
        return vinf

    def get_vinf_mag(self, t, v):
        """return magnitude of v infinity vector"""
        # v is relative to the central body, NOT the flyby body
        vinf = self.get_vinf(t, v)
        vinfmag = np.linalg.norm(vinf)
        return vinfmag

    def get_delta(self, tm, tp, vm, vp):
        """return turn angle delta"""
        vinfm = self.get_vinf(tm, vm)
        vinfp = self.get_vinf(tp, vp)
        vinfmmag = self.get_vinf_mag(tm, vm)
        vinfpmag = self.get_vinf_mag(tp, vp)
        num = np.dot(vinfm, np.transpose(vinfp))
        den = vinfmmag * vinfpmag
        delta = np.arccos(num / den)
        return delta

    def get_rperi(self, tm, tp, vm, vp):
        """return periapsis distance about flyby body"""
        delta = self.get_delta(tm, tp, vm, vp)
        vinfp = self.get_vinf(tp, vp)
        vinfpmag = self.get_vinf_mag(tp, vp)
        rperi = (self.mu_body / vinfpmag**2) * ((1.0 / np.sin(delta / 2.0)) - 1.0)
        return rperi

    def get_rperi_derivs(self, tm, tp, vm, vp):
        """return derivatives of rperi"""
        # derivatives w.r.t. r and m are zero, so don't return
        vinfm = self.get_vinf(tm, vm)
        vinfp = self.get_vinf(tp, vp)
        vinfmmag = self.get_vinf_mag(tm, vm)
        vinfpmag = self.get_vinf_mag(tp, vp)
        rperi = self.get_rperi(tm, tp, vm, vp)
        delta = self.get_delta(tm, tp, vm, vp)
        vinfmhat = vinfm / vinfmmag
        vinfphat = vinfp / vinfpmag
        abm = self.get_d2_r_body_wrt_sun_d_t2(tm)
        abp = self.get_d2_r_body_wrt_sun_d_t2(tp)

        num = vinfmhat - np.dot(np.dot(vinfmhat, np.transpose(vinfphat)), vinfphat)
        den = np.sqrt(1.0 - np.dot(vinfmhat, np.transpose(vinfphat))**2)
        d_rperi_d_vp = -2.0 * (rperi / vinfpmag) * vinfphat + 0.5 * (self.mu_body / vinfpmag**3) * (1.0 / (np.tan(delta / 2.0) * np.sin(delta / 2.0))) * (num / den)
        d_rperi_d_tp = -np.dot(d_rperi_d_vp, abp)

        num = vinfphat - np.dot(np.dot(vinfphat, np.transpose(vinfmhat)), vinfmhat)
        den = np.sqrt(1.0 - np.dot(vinfmhat, np.transpose(vinfphat))**2)
        d_rperi_d_vm = 0.5 * (self.mu_body / (vinfpmag**2 * vinfmmag)) * (1.0 / (np.tan(delta / 2.0) * np.sin(delta / 2.0))) * (num / den)
        d_rperi_d_tm = -np.dot(d_rperi_d_vm, abm)
        return d_rperi_d_tm, d_rperi_d_tp, d_rperi_d_vm, d_rperi_d_vp

    def get_c_r(self, t, r):
        """return constraint on position vector"""
        rb = self.get_r_body_wrt_sun(t)
        c_r = r - rb
        return c_r

    def get_c_r_derivs(self, t, r):
        """return derivatives of constraint on position vector"""
        d_c_r_d_r = np.identity(3)
        vb = self.get_d_r_body_wrt_sun_d_t(t)
        d_c_r_d_t = -vb
        return d_c_r_d_t, d_c_r_d_r

    def get_c_vinf_mag(self, tm, tp, vm, vp):
        """return v infinity magnitude constraint"""
        vinfmmag = self.get_vinf_mag(tm, vm)
        vinfpmag = self.get_vinf_mag(tp, vp)
        c_vinf_mag = vinfpmag - vinfmmag
        return c_vinf_mag

    def get_c_vinf_mag_derivs(self, tm, tp, vm, vp):
        """return derivatives of v infinity magnitude constraint"""
        vinfm = self.get_vinf(tm, vm)
        vinfp = self.get_vinf(tp, vp)
        vinfmmag = self.get_vinf_mag(tm, vm)
        vinfpmag = self.get_vinf_mag(tp, vp)
        vinfmhat = vinfm / vinfmmag
        vinfphat = vinfp / vinfpmag
        abm = self.get_d2_r_body_wrt_sun_d_t2(tm)
        abp = self.get_d2_r_body_wrt_sun_d_t2(tp)

        d_vinf_mag_d_vm = -vinfmhat
        d_vinf_mag_d_vp = vinfphat

        d_vinf_mag_d_tm = np.dot(vinfmhat, abm)
        d_vinf_mag_d_tp = -np.dot(vinfphat, abp)
        return d_vinf_mag_d_tm, d_vinf_mag_d_tp, d_vinf_mag_d_vm, d_vinf_mag_d_vp

    def get_c_m(self, mm, mp):
        """return mass constraint"""
        c_m = mp - mm
        return c_m

    def get_c_t(self, tm, tp):
        """return time constraint"""
        c_t = tp - tm
        return c_t

if __name__ == "__main__":
    np.set_printoptions(precision=16)
    # define problem

    # times (s)
    tm = 955.0
    tp = 965.0

    # states w.r.t. flyby body
    rm = np.array([300.0, 600.0, 100.0]) # km
    vm = np.array([4.8, 9.2, 7.9]) # km/s
    mm = 1000.0 # kg

    rp = np.array([350.0, 700.0, 200.0]) # km
    vp = np.array([5.0, 10.2, 8.9]) # km/s
    mp = 990.0 # kg

    # create flyby constraint object
    o = patched_conic_flyby_validation()

    # states w.r.t. central body (body that flyby body orbits)
    rbm = o.get_r_body_wrt_sun(tm)
    vbm = o.get_d_r_body_wrt_sun_d_t(tm)
    rbp = o.get_r_body_wrt_sun(tp)
    vbp = o.get_d_r_body_wrt_sun_d_t(tp)

    rm = rm + rbm
    vm = vm + vbm
    rp = rp + rbp
    vp = vp + vbp

    # calculate constraint values and derivatives
    
    # position at tm
    c_rm = o.get_c_r(tm, rm)
    d_c_rm_d_tm, d_c_rm_d_rm = o.get_c_r_derivs(tm, rm)

    # position at tp
    c_rp = o.get_c_r(tp, rp)
    d_c_rp_d_tp, d_c_rp_d_rp = o.get_c_r_derivs(tp, rp)

    # v infinity magnitudes equal at tm and tp
    c_vinf_mag = o.get_c_vinf_mag(tm, tp, vm, vp)
    d_vinf_mag_d_tm, d_vinf_mag_d_tp, d_vinf_mag_d_vm, d_vinf_mag_d_vp = o.get_c_vinf_mag_derivs(tm, tp, vm, vp)
    d_vinf_mag_d_tm_fun = jacobian(o.get_c_vinf_mag, 0)
    d_vinf_mag_d_tp_fun = jacobian(o.get_c_vinf_mag, 1)
    d_vinf_mag_d_vm_fun = jacobian(o.get_c_vinf_mag, 2)
    d_vinf_mag_d_vp_fun = jacobian(o.get_c_vinf_mag, 3)
    d_vinf_mag_d_tm_ad = d_vinf_mag_d_tm_fun(tm, tp, vm, vp)
    d_vinf_mag_d_tp_ad = d_vinf_mag_d_tp_fun(tm, tp, vm, vp)
    d_vinf_mag_d_vm_ad = d_vinf_mag_d_vm_fun(tm, tp, vm, vp)
    d_vinf_mag_d_vp_ad = d_vinf_mag_d_vp_fun(tm, tp, vm, vp)

    # mass at tm and tp
    c_m = o.get_c_m(mm, mp)

    # time equal
    c_t = o.get_c_t(tm, tp)

    # periapsis
    c_rperi = o.get_rperi(tm, tp, vm, vp)
    d_rperi_d_tm, d_rperi_d_tp, d_rperi_d_vm, d_rperi_d_vp = o.get_rperi_derivs(tm, tp, vm, vp)
    d_rperi_d_tm_fun = jacobian(o.get_rperi, 0)
    d_rperi_d_tp_fun = jacobian(o.get_rperi, 1)
    d_rperi_d_vm_fun = jacobian(o.get_rperi, 2)
    d_rperi_d_vp_fun = jacobian(o.get_rperi, 3)
    d_rperi_d_tm_ad = d_rperi_d_tm_fun(tm, tp, vm, vp)
    d_rperi_d_tp_ad = d_rperi_d_tp_fun(tm, tp, vm, vp)
    d_rperi_d_vm_ad = d_rperi_d_vm_fun(tm, tp, vm, vp)
    d_rperi_d_vp_ad = d_rperi_d_vp_fun(tm, tp, vm, vp)

    print('RESULTS')
    print('=======')
    print('Scenario:')
    print('---------')
    print('t- = ', tm)
    print('t+ = ', tp)
    print('')
    print('rb- = ', rbm)
    print('rb+ = ', rbp)
    print('')
    print('vb- = ', vbm)
    print('vb+ = ', vbp)
    print('')
    print('r- = ', rm)
    print('r+ = ', rp)
    print('')
    print('v- = ', vm)
    print('v+ = ', vp)
    print('')
    print('m- = ', mm)
    print('m+ = ', mp)
    print('')
    print('Constraint Values:')
    print('------------------')
    print('r+ = ', c_rp)
    print('')
    print('r- = ', c_rm)
    print('')
    print('vinfmag =', c_vinf_mag)
    print('')
    print('m = ', c_m)
    print('')
    print('t = ', c_t)
    print('')
    print('rperi = ', c_rperi)
    print('')
    print('Derivatives:')
    print('------------')