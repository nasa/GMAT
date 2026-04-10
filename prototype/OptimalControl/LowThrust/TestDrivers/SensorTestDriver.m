
%%  A conic sensor
conicSensor = SimpleConicSensor;
conicSensor.boreSiteVector = [1 1 1];
conicSensor.coneHalfAngle = 5;
conicSensor.Initialize();
conicSensor.IsInFieldOfView([1 1.01 1])

%  A rectangular sensor
conicSensor = RectangularSensor;
conicSensor.boreSiteVector = [1 1 1];
conicSensor.fovWidth = 3;
conicSensor.fovHeight = 7;
conicSensor.Initialize();
conicSensor.IsInFieldOfView([1 1.01 1])