function UnitTest_LineSegmentIntersect()

is_show = false;
XY1 = [
    % Random lines
    0.87572400256463         0.597302079801353         0.280397005689543        0.0451518760404848
    0.393098592724503          0.65917606662187          0.25942744797746         0.240645885287937
    0.458066938000918         0.580006430551879         0.547095959676547       0.00885503009860023
    0.208245204973289         0.909951610123007         0.541268467485969         0.671593534312873
    0.757273162983819         0.636006152111755         0.788113184048992         0.904813230017362
    0.546692028178502         0.525561251235387         0.869605555315031          0.57241631161166
    0.357388192967744         0.259623080110499         0.787540530159212         0.155461290328972
    0.700997789695237        0.0511708673569459         0.969427855895745         0.502367433727028
    0.109222496688655          0.73195012915998         0.180468109688552         0.567732878272974
    0.00661076596420229         0.164291390918458         0.930598962817579         0.188271775739867
    % Parallel lines
    0.25 0.25 0.75 0.25
    0.75 0.25 0.75 0.75
    0.75 0.75 0.25 0.75
    0.25 0.75 0.25 0.25
    % Coincident lines
    0.5 0.5 0.75 0.75
    0.4 0.4 0.75 0.75
    ];

XY2 = [
    % Random lines
    0.32419646346704         0.106872780238886         0.287584055249314         0.905531348068602
    0.716037342838811         0.732149758734352         0.816712735890955         0.417320883626846
    0.552928708571216         0.970522786956106         0.450528094868533         0.154058327516862
    0.142281739817257         0.608885538697176         0.806632520363243         0.539999263770361
    0.380363559412667         0.719665612178919         0.790172935024334         0.937091367960309
    0.396569245211042         0.302752325729782         0.282957873186578         0.660955064915145
    0.576741695694105         0.459022394078519        0.0683092386250433         0.394657441563815
    0.0194015027111201        0.0480286131823641        0.0549307282223953         0.258990542158967
    0.577580061269445         0.385352105175205         0.637522639339998         0.847912159970545
    0.932196092808549         0.361716380248139           0.4242855812171         0.945056074939906
    % Parallel lines
    0.27 0.27 0.77 0.27
    0.77 0.27 0.77 0.77
    0.77 0.77 0.27 0.77
    0.27 0.77 0.27 0.27
    % Coincident lines
    0.5 0.5 0.77 0.77
    0.44 0.44 0.75 0.75
    ];

CppSyntax('XY1',XY1);
CppSyntax('XY2',XY2);

out = lineSegmentIntersect(XY1,XY2);

CppLineIntersectOutput
out.intAdjacencyMatrix - intAdjacencyMatrixGMAT
out.intMatrixX - intMatrixXGMAT
out.intMatrixY - intMatrixYGMAT
out.intNormalizedDistance1To2 - intNormalizedDistance1To2GMAT
out.intNormalizedDistance2To1 - intNormalizedDistance2To1GMAT
out.parAdjacencyMatrix - parAdjacencyMatrixGMAT
out.coincAdjacencyMatrix - coincAdjacencyMatrixGMAT

out.intAdjacencyMatrix;
CppSyntax('intAdjacencyMatrixTruth',out.intAdjacencyMatrix);
out.intMatrixX;
CppSyntax('intMatrixXTruth',out.intMatrixX);
out.intMatrixY;
CppSyntax('intMatrixYTruth',out.intMatrixY);
out.intNormalizedDistance1To2;
CppSyntax('intNormalizedDistance1To2Truth',out.intNormalizedDistance1To2);
out.intNormalizedDistance2To1;
CppSyntax('intNormalizedDistance2To1Truth',out.intNormalizedDistance2To1);
out.parAdjacencyMatrix;
CppSyntax('parAdjacencyMatrixTruth',out.parAdjacencyMatrix);
out.coincAdjacencyMatrix;
CppSyntax('coincAdjacencyMatrixTruth',out.coincAdjacencyMatrix);

if is_show
    
    % Show the intersection points.
    
    figure('Position',[10 100 500 500],'Renderer','zbuffer');
    
    axes_properties.box             = 'on';
    axes_properties.XLim            = [0 1];
    axes_properties.YLim            = [0 1];
    axes_properties.DataAspectRatio = [1 1 1];
    axes_properties.NextPlot        = 'add';
    
    axes(axes_properties,'parent',gcf);
    
    line([XY1(:,1)';XY1(:,3)'],[XY1(:,2)';XY1(:,4)'],'Color','r');
    line([XY2(:,1)';XY2(:,3)'],[XY2(:,2)';XY2(:,4)'],'Color','k');
    
    scatter(out.intMatrixX(:),out.intMatrixY(:),[],'r');
    
    title('Intersection Points');
    return
    % Show the parallel lines.
    
    figure('Position',[20 90 500 500],'Renderer','zbuffer');
    
    
    axes(axes_properties,'parent',gcf);
    
    [I,J] = find(out.parAdjacencyMatrix);
    
    hL1 = line([XY1(I,1)';XY1(I,3)'],[XY1(I,2)';XY1(I,4)'],'Color','r');
    
    title('Parallel Line Segments');
    
    % Show the coincident lines.
    
    figure('Position',[30 80 500 500],'Renderer','zbuffer');
    
    axes(axes_properties,'parent',gcf);
    
    [I,J] = find(out.coincAdjacencyMatrix);
    
    line([XY1(I,1)';XY1(I,3)'],[XY1(I,2)';XY1(I,4)'],'Color','r');
    line([XY2(J,1)';XY2(J,3)'],[XY2(J,2)';XY2(J,4)'],'Color','k');
    
    title('Coincident Line Segments');
    
end

end

function CppSyntax(arrayName,arrayData)

numRows = size(arrayData,1);
numCols = size(arrayData,2);
disp([arrayName ' = new Rmatrix(' num2str(numRows) ',' num2str(numCols) ');']);
for rowIdx = 1:numRows;
    for colIdx = 1:numCols
        lhsString = [arrayName '[' num2str(rowIdx-1) ',' num2str(colIdx-1) ']'];
        rhsString = num2str(arrayData(rowIdx,colIdx),16);
        disp([lhsString '=' rhsString ';']);
    end
end
end