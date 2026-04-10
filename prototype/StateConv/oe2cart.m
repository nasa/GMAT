%
%
%  oe2rv.m  Orbital Elements to cartesian state
%
%  cv = oe2rv(oe,GM_earth)
%			oe = [a e i om Om nu]
%			r,v  expressed in  IJK  frame
%
function cv = oe2rv(oe,GM_earth)
	a=oe(1); e=oe(2); i=oe(3); om=oe(4); Om=oe(5); nu=oe(6);
	p = a*(1-e*e);
	r = p/(1+e*cos(nu));
	rv = [r*cos(nu); r*sin(nu); 0];			% in PQW frame
   vv = sqrt(GM_earth/p)*[-sin(nu); e+cos(nu); 0];
%
%	now rotate
%
	cO = cos(Om);  sO = sin(Om);
	co = cos(om);  so = sin(om);
	ci = cos(i);   si = sin(i);
	R  = [cO*co-sO*so*ci  -cO*so-sO*co*ci  sO*si;
		  sO*co+cO*so*ci  -sO*so+cO*co*ci -cO*si;
		  so*si            co*si           ci];
	cv = [R*rv;R*vv];
