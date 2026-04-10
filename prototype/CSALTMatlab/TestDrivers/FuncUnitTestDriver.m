
%  User Path Function
% func = UserPathFunction();
% func.SetState(1)
% func.SetControl(1)
% func.SetTime(0)
% func.SetStateControlTime(1,1,0)
% func.functionName = 'PathFunction'
% func.Initialize();


func = UserPointFunction();
func.SetInitialState([1 0 0 1 1]');
func.SetFinalState([1.5 pi/2 0 1 1]');
func.SetInitialControl([0 1]');
func.functionName = 'OrbitRaisingPointFunction';
func.Initialize();
func