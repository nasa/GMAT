function Status = CheckConstraintVariances(obj)
            %  Evaluate constraint feasiblity for all constraints
            %  This is currently only used for post processing of results
            %  to report which constraints are/are not satisfied at the
            %  solution
            
            %  Evaluate constraints initialize items
            constraintVec   = GetContraintVector(obj);
            if isa(constraintVec,'gradient')
                constraintVec = constraintVec.x;
            end
            numEventViolations  = 0;
            numdefectViolations = 0;
            numPathViolations   = 0;
            eventViolation  = false();
            defectViolation = false();
            pathViolation   = false();
            tol = 1e-5;  %  TODO.  Don't hard code this
            
            %  Check point (event) constraints for violation
            if obj.PointFunction.hasBoundaryFunctions;
                for conIdx = obj.algEventConStartIdx:obj.algEventConEndIdx
                    if constraintVec(conIdx) <= ...
                            obj.allConLowerBound(conIdx) - tol|| ...
                            constraintVec(conIdx) >= ...
                            obj.allConUpperBound(conIdx) + tol;
                        eventViolation = true();
                        numEventViolations = numEventViolations + 1;
                    end
                end
            end
            
            %  Check point (event) constraints for violation
            if obj.PathFunction.hasDynFunctions;
                for conIdx = obj.defectConStartIdx:obj.defectConEndIdx
                    if constraintVec(conIdx) <= ...
                            obj.allConLowerBound(conIdx) - tol || ...
                            constraintVec(conIdx) >= ...
                            obj.allConUpperBound(conIdx) + tol;
                        defectViolation = true();
                        numPathViolations = numPathViolations + 1;
                    end
                end
            end
            
            %  Check path constraints for violation
            if obj.PathFunction.hasAlgFunctions;
                for conIdx = obj.algPathConStartIdx:obj.algPathConEndIdx
                    if constraintVec(conIdx) <= ...
                            obj.allConLowerBound(conIdx) - tol|| ...
                            constraintVec(conIdx) >= ...
                            obj.allConUpperBound(conIdx) + tol;
                        pathViolation = true();
                        numdefectViolations = numdefectViolations + 1;
                    end
                end
            end
            Status.numEventViolations  = numEventViolations;
            Status.numdefectViolations = numdefectViolations;
            Status.numPathViolations   = numPathViolations;
            Status.eventViolation      = eventViolation;
            Status.defectViolation     = defectViolation;
            Status.pathViolation       = pathViolation;
        end
