ClearAll
maxIteration = 20;

for i = 1:maxIteration
   
    load(['Iteration',num2str(i),'_call_output_run1'])
    
    C1  = C;
    G1  = G;
    J1  = J;
    z1  = z;
    
    clear J G C z
    
    
    load(['Iteration',num2str(i),'_call_output_run2'])
    
    
    C2  = C;
    G2  = G;
    J2  = J;
    z2  = z;
    
    
    clear J G C z
    size(G2) 
    size(G1)
    disp(['Iteration ',num2str(i),' G error = ',num2str(max(abs(G2-G1)))])
    disp(['Iteration ',num2str(i),' C error = ',num2str(max(abs(C2-C1)))])
    disp(['Iteration ',num2str(i),' J error = ',num2str(max(max(abs(J2-J1))))])
    disp(['Iteration ',num2str(i),' z error = ',num2str(max(abs(z2-z1)))])
    
end