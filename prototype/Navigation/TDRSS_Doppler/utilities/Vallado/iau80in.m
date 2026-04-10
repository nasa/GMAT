% ----------------------------------------------------------------------------
%
%                           function iau80in
%
%  this function initializes the nutation matricies needed for reduction
%    calculations. the routine needs the filename of the files as input.
%
%  author        : david vallado                  719-573-2600   27 may 2002
%
%  revisions
%                -
%
%  inputs          description                    range / units
%    iar80       - integers for fk5 1980
%    rar80       - reals for fk5 1980             rad
%
%  outputs       :
%    none
%
%  locals        :
%    convrt      - conversion factor to degrees
%    i,j         - index
%
%  coupling      :
%    none        -
%
%  references    :
%
% [iar80,rar80] = iau80in();
% ----------------------------------------------------------------------------- }

function [iar80,rar80] = iau80in();

       % ------------------------  implementation   -------------------
       % 0.0001" to rad
       convrt= 0.0001 * pi / (180*3600.0);

       load nut80.dat;

       iar80 = nut80(:,1:5);
       rar80 = nut80(:,6:9);

       for i=1:106
           for j=1:4
               rar80(i,j)= rar80(i,j) * convrt;
           end
       end

