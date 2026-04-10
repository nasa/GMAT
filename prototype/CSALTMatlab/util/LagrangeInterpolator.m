classdef LagrangeInterpolator < handle
    
    %  Author: S. Hughes.  steven.p.hughes@nasa.gov
    % 
    %  This class performs lagrange interpolation and differentiation using
    %  lagrange interpolating polynomials. 
    %
    %  Example Usage:  intepolate sin and exp functions and compute
    %  deriviatives at the points provided in the independent variables.
    %
    %      indVar  = [.1 .12 .13 .14 .15 .16 .17 .18]'
    %      interpValue = .125455;
    %      depVar1 = feval('sin',indVar);
    %      depVar2 = feval('exp',indVar);
    %      depVars = [depVar1 depVar2];
    %     
    %      lagInterp = LagrangeInterpolator();
    %      lagInterp.SetIndependentVariables(indVar);
    %      lagInterp.Interpolate(interpValue,depVars)
    %      lagInterp.Differentiate(depVars)
    
    properties  (SetAccess = 'protected')
        
        %   Column vector of independence variables.
        indVar
        %   The number of independent variables.  Order of interpolation
        %   will be 1-numPoints
        numPoints
        
    end
    
    methods
        
        %% Set the indepependent variables for the interpolator
        function SetIndependentVariables(this,indVar)
            
            %  Check that input is a column vector.
            [~,cols] = size(indVar);
            if  cols ~= 1
                errorMsg = 'Input must be a column vector';
                errorLoc  = ['LagrangeInterpolator:SetIndependent'...
                    'Variables'];
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            this.indVar    = indVar;
            this.numPoints = length(indVar);
            
        end
        
        %% Get the Lagrange products used in Lagrange Interpolation
        function lagProducts = GetLagrangeProducts(this,interpPoint)
            
            % Check that interpPoint is within range of indVar
            if interpPoint < this.indVar(1) || ...
                    interpPoint > this.indVar(this.numPoints)
                errorMsg = ['Requested interpolation point is not ' ...
                    'contained in vector of independent variables'];
                errorLoc = 'LagrangeInterpolator:GetLagrangeProducts';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            % Compute the Lagrange products
            lagProducts = ones(this.numPoints,1);
            for i = 1:this.numPoints
                for j = 1:this.numPoints
                    if i ~= j
                        lagProducts(i) = lagProducts(i)*(interpPoint - ...
                            this.indVar(j,1)) /(this.indVar(i,1) -...
                            this.indVar(j,1));
                    end
                end
            end
            
        end
        
        %%  Perform the interpolation
        function interpValues = Interpolate(this,interpPoint,funcValues)
            
            %  Check that the input is a column vector with correct number
            %  of rows
            [rows,cols] = size(funcValues);
            if rows ~= this.numPoints
                errorMsg = ['Number of rows in input must be the same' ...
                    'as the independent variable vector'];
                errorLoc = 'LagrangeInterpolator:Interpolate';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
                
            end
            
            %  Perform the interpolation
            interpValues = zeros(cols,1);
            lagProducts = GetLagrangeProducts(this,interpPoint);
            for funcIdx = 1:this.numPoints
                interpValues = interpValues + funcValues(funcIdx,:)'*...
                    lagProducts(funcIdx,1);
            end
            
        end
        
         function interpValues = InterpolateArray(this,interpPoints,funcValues)
             
             for rowIdx = 1:length(interpPoints)
                 interpValues(rowIdx,:) = this.Interpolate(interpPoints(rowIdx),funcValues);
             end
             
         end
        
        %%  Compute the differentiation matrix
        function D = GetDiffMatrix(this)
            
            %  This code was taken from GPOPS v2.4
            n = this.numPoints;
            D = zeros(n,n);
            for j = 1:n;
                for i = 1:n;
                    prod = 1;
                    sum = 0;
                    if j == i
                        for k = 1:n
                            if k~=i
                                sum = sum+1/(this.indVar(i)...
                                    -this.indVar(k));
                            end
                        end
                        D(i,j) = sum;
                    else
                        for k = 1:n
                            if (k~=i) && (k~=j)
                                prod = prod*(this.indVar(i)- ...
                                    this.indVar(k));
                            end
                        end
                        for k = 1:n
                            if k~=j
                                prod = prod/(this.indVar(j)-...
                                    this.indVar(k));
                            end
                        end
                        D(i,j) = prod;
                    end
                end
            end
            
        end
        
        %  Compute derivatives of polynomial at the points provided in the
        %  independent variable vector
        function diffValues = Differentiate(this,funcValues)
            
            %  Check that the input is a column vector with correct number
            %  of rows
            [rows,~] = size(funcValues);
            if rows ~= this.numPoints
                errorMsg = ['Number of rows in input must be the same' ...
                    'as the independent variable vector'];
                errorLoc = 'LagrangeInterpolator:Differentiate';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %   Perform the differentiation for each function 
            diffMat    = GetDiffMatrix(this);
            diffValues = diffMat*funcValues;

        end
        
    end
    
end