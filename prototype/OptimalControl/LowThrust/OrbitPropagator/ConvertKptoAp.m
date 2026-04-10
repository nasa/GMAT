function [Ap] = ConvertKptoAp(Kp)

Ap = fsolve(@ErrorFunc,15,[],Kp);

function error = ErrorFunc(Ap,Kp)

error = 28*Kp+0.03*exp(Kp) - Ap -100*(1 - exp(-0.008*Ap));