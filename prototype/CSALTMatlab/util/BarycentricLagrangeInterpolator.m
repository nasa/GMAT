classdef BarycentricLagrangeInterpolator < handle    
    %  Author: S. Hughes, steven.p.hughes@nasa.gov
    %          Y. Kim,    youngkwangkim88@gmail.com
    % 
    %  This class performs barycentric lagrange interpolation. 
    %  differentiation is not done here.
    %  differentiation can be done by using collocD.m
    %
    %  note : barycentric interpolation is singular when an interp point is
    %  close to a data point but not equal to the data point. So, it is 
    %  necessary to apply the ordinary Lagrange interpolation for those 
    %  points. 
    properties  (SetAccess = 'protected')        
        %   Column vector of independence variables.
        %   only strictly increasing indVar is accepted
        indVar  
        %   range is the range of the indVar
        range
        %   The number of independent variables.  Order of interpolation
        %   will be 1-numPoints
        numIndVar
        % barycentric weights
        weightVec
    end
    
    methods       
        %% Set the indepependent variables for the interpolator
        function this=BarycentricLagrangeInterpolator(indVar)                      
            this.SetIndVar(indVar);            
            this.CalWeightVec();                       
        end
        function SetIndVar(this,indVar)
             %  Check that input is a column vector.
             %  Check that input is strictly increasing
            [numIndVar,numCols] = size(indVar);
            if  numCols ~= 1
                errorMsg = 'IndVar must be a column vector';
                errorLoc  = ['BarycenricLagrangeInterpolator'...
                    ':SetIndVar'];
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            if numIndVar < 2
                errorMsg = 'IndVar must have at least two elements';
                errorLoc  = ['BarycenricLagrangeInterpolator'...
                    ':SetIndVar'];
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            % check index fair from (1,2) to (end-1,end)                 
            for i = 2:numIndVar
                if (indVar(i) - indVar(i-1)) <= 0
                    errorMsg = 'data points are not strictly increasing';
                    errorLoc  = ['BarycenricLagrangeInterpolator'...
                        ':SetIndVar'];
                    ME = MException(errorLoc,errorMsg);
                    throw(ME);           
                end
            end
            this.indVar = indVar;
            this.numIndVar = numIndVar;            
            this.range = [indVar(1) indVar(end)];                                     
        end
        %% vectorized method for ptr array: GetNumPoints
        % this will not be used in C++ implementation
        function numPoints=GetNumPoints(this)
            numPtr=numel(this);
            numPoints=zeros(numPtr,1);
            for i=1:numPtr
                numPoints(i)=length(this(i).indVar);
            end
        end
        %% check whether the given interpPoints are valid
        function  ChkInterpPointsFeasibility(this,interpPoints)
            % one of interpPoints is equal to one of indVar elements,
            % the interpPoints are not feasible.
            % now the extrapolation is allowed.
             %  Check that input is a column vector.
             %  Check that input is strictly increasing
            [numInterpPoints,numCols] = size(interpPoints);
            if  numCols ~= 1
                errorMsg = 'InterpPoints must be a column vector';
                errorLoc  = ['BarycenricLagrangeInterpolator'...
                    ':SetIndVar'];
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            if numInterpPoints > 1
                for i = 2:numInterpPoints
                    if (interpPoints(i) - interpPoints(i-1)) <= 0
                        errorMsg = 'interpPoints are not strictly increasing.';
                        errorLoc  = ['BarycenricLagrangeInterpolator'...
                            ':ChkInterpPointsFeasibility'];
                        ME = MException(errorLoc,errorMsg);
                        throw(ME);           
                    end
                end   
            end            
            for interpPointIdx = 1:numInterpPoints
                for indVarIdx = 1:this.numIndVar
                    if interpPoints(interpPointIdx) == this.indVar(indVarIdx)
                        errorMsg = 'An interpPoint is identical to an indVar point.';
                        errorLoc  = ['BarycenricLagrangeInterpolator'...
                            ':ChkInterpPointsFeasibility'];
                        ME = MException(errorLoc,errorMsg);
                        throw(ME);     
                    end
                end
            end
                          
        end
        function ChkFuncValuesFeasibility(this,funcValues)
            [numFuncValues,numCols] = size(funcValues);
            if  numCols ~= 1
                errorMsg = 'Function Values must be a column vector';
                errorLoc  = ['BarycenricLagrangeInterpolator'...
                    ':IsFuncValuesFeasible'];
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            if numFuncValues ~= this.numIndVar
                errorMsg = ['The number of Function Values must be equal' ...
                    'to numIndVar'];
                errorLoc  = ['BarycenricLagrangeInterpolator'...
                    ':IsFuncValuesFeasible'];
                ME = MException(errorLoc,errorMsg);
                throw(ME);                
            end
        end
        %% Get the Lagrange products used in Lagrange Interpolation
        function barycentricLagMat = GetBarycentricMatrix(this,interpPoints)                            
            % Compute the barycentric form of Lagrange matrix
            numInterpPoints=numel(interpPoints);
            barycentricLagMat = zeros(numInterpPoints,this.numIndVar);
            normalizationVec =  zeros(numInterpPoints,1);
            for i = 1:this.numIndVar
                barycentricLagMat(:,i) = this.weightVec(i) ...
                    ./(interpPoints - this.indVar(i));  
                % vectorized subtraction & division !
                
                normalizationVec = normalizationVec + barycentricLagMat(:,i);                
            end
            for i= 1: numInterpPoints
                barycentricLagMat(i,:)=barycentricLagMat(i,:)./normalizationVec(i);
            end
        end
        function CalWeightVec(this)
            % weight vector is a column vector of length numPoints
            this.weightVec=ones(this.numIndVar,1);            
            for i=1:this.numIndVar
                for j=1:this.numIndVar
                    if i~=j
                        this.weightVec(i) = this.weightVec(i)/ ...
                            (this.indVar(i) - this.indVar(j));
                    end
                end
            end
        end
        %%  Perform the interpolation
        function interpValues = Interpolate(this,interpPoints,funcValues)
            %  Check that the input is a column vector with correct number
            %  of rows
            
            % this.IsFuncValuesFeasibility(funcValues);
            this.ChkInterpPointsFeasibility(interpPoints);
            
            %  Perform the interpolation
            barycentricLagMat = GetBarycentricMatrix(this,interpPoints);
            if isnan(barycentricLagMat)
                keyboard
            end
            interpValues = barycentricLagMat * funcValues;            
        end                
    end
    
end