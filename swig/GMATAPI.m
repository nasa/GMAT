classdef GMATAPI
    methods(Static)
        
		function val = Help(varargin)
            val = gmat.gmat.Help(varargin{:});
        end		

		function val = ShowObjectsForID(varargin)
            val = gmat.gmat.ShowObjectsForID(varargin{:});
        end	

		function val = ShowClassesForID(varargin)
            val = gmat.gmat.ShowClassesForID(varargin{:});
        end		

		function val = ShowObjects(varargin)
            val = gmat.gmat.ShowObjects(varargin{:});
        end		

		function val = ShowClasses(varargin)
            val = gmat.gmat.ShowClasses(varargin{:});
        end		

		function val = Exists(arg)
            val = gmat.gmat.LoadScript(arg);
        end			

		function val = GetSolarSystem()
            val = gmat.gmat.GetSolarSystem();
        end		

		function val = GetCommands(varargin)
            val = gmat.gmat.GetCommands(varargin{:});
        end		

		function val = GetNestedCommands(varargin)
            val = gmat.gmat.GetNestedCommands(varargin{:});
        end		

		function val = LoadScript(arg)
            val = gmat.gmat.LoadScript(arg);
        end		

		function val = LoadInclude(arg)
            val = gmat.gmat.LoadInclude(arg);
        end		

		function val = RunScript()
            val = gmat.gmat.RunScript();
        end		

		function val = Execute()
            val = gmat.gmat.Execute();
        end	
		
		function val = SaveScript(arg)
            val = gmat.gmat.SaveScript(arg);
        end		

		function val = GetRunSummary()
            val = gmat.gmat.GetRunSummary();
        end		

		function val = Setup(varargin)
            gmat.gmat.Setup(varargin{:});
        end		

		function val = Initialize(varargin)
            gmat.gmat.Initialize(varargin{:});
        end		

		function val = Update(varargin)
           gmat.gmat.Update(varargin{:});
        end		

		function val = Clear(varargin)
            val = gmat.gmat.Clear(varargin{:});
        end		

		function val = UseLogFile(varargin)
            gmat.gmat.UseLogFile(varargin{:});
        end		

		function val = EchoLogFile(varargin)
            gmat.gmat.EchoLogFile(varargin{:});
        end	
		
		function val = Construct(varargin)
            val = gmat.gmat.Construct(varargin{:});
            val = GMATAPI.SetClass(val);
        end
		
		function val = Command(varargin)
            val = gmat.gmat.Command(varargin{:});
            val = GMATAPI.SetClass(val);
        end
        
        function val = Copy(varargin)
            val = gmat.gmat.Copy(varargin{:});
            val = GMATAPI.SetClass(val);
        end
        
        function val = GetObject(arg)
            val = gmat.gmat.GetObject(arg);
            val = GMATAPI.SetClass(val);
        end
        
        function val = GetRuntimeObject(arg)
            val = gmat.gmat.GetRuntimeObject(arg);
            val = GMATAPI.SetClass(val);
        end
        
        function val = SetClass(val)
			if ~isempty(val)
				typestr = val.GetTypeName();
				if strcmp(typestr, 'ForceModel')
					typestr = 'ODEModel';
				end

				evalstr = strcat('gmat.', char(typestr), '.SetClass(val)');
				try
					val = eval(evalstr);
				catch
				end
			end
        end
    end
end
