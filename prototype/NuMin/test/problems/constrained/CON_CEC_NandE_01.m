function [ci,ce,Ji,Je]= CON_CEC_NandE_01(x)
	ce = x(1)^2 + 3*x(2)^2 - 4;
	Je = [2*x(1);6*x(2);0];
	Ji = [];
	ci = [];