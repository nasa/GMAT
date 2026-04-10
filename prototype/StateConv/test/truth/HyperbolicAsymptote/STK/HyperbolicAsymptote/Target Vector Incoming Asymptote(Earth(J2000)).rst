stk.v.10.0
WrittenBy    STK_v10.0.2

BEGIN ReportStyle

BEGIN ClassId
	Class		Satellite
END ClassId

BEGIN Header
	StyleType		0
	Date		Yes
	Name		Yes
	IsHidden		No
	DescShort		No
	DescLong		No
	YLog10		No
	Y2Log10		No
	YUseWholeNumbers		No
	Y2UseWholeNumbers		No
	VerticalGridLines		No
	HorizontalGridLines		No
	AnnotationType		Spaced
	NumAnnotations		3
	NumAngularAnnotations		5
	ShowYAnnotations		Yes
	AnnotationRotation		1
	BackgroundColor		#ffffff
	ForegroundColor		#000000
	ViewableDuration		3600.000000
	RealTimeMode		No
	DayLinesStatus		1
	LegendStatus		1
	LegendLocation		1
	TextPointSize		10.000000

BEGIN PostProcessor
	Destination	0
	Use	0
	Destination	1
	Use	0
	Destination	2
	Use	0
	Destination	3
	Use	0
END PostProcessor
	NumSections		2
END Header

BEGIN Section
	Name		Section 1
	ClassName		Satellite
	NameInTitle		No
	ExpandMethod		0
	PropMask		2
	ShowIntervals		No
	NumIntervals		0
	NumLines		1

BEGIN Line
	Name		Line 1
	NumElements		7

BEGIN Element
	Name		Time
	IsIndepVar		Yes
	IndepVarName		Time
	Title		Time
	NameInTitle		No
	Service		Astrogator
	Type		Cartesian Elems
	Element		Time
	SumAllowedMask		0
	SummaryOnly		No
	DataType		0
	UnitType		2
	LineStyle		0
	LineWidth		0
	LineColor		#000000
	PointStyle		0
	PointSize		0
	FillPattern		0
	FillColor		#000000
	PropMask		0
	UseScenUnits		Yes
END Element

BEGIN Element
	Name		Astrogator Values-Cartesian Elems-X(Earth(J2000))
	IsIndepVar		No
	IndepVarName		Time
	Title		X(Earth(J2000))
	NameInTitle		Yes
	Service		Astrogator
	Type		Cartesian Elems
	Element		X(Earth(J2000))
	Format		%.14e
	SumAllowedMask		1559
	SummaryOnly		No
	DataType		0
	UnitType		0
	LineStyle		0
	LineWidth		0
	LineColor		#000000
	PointStyle		0
	PointSize		0
	FillPattern		0
	FillColor		#000000
	PropMask		0
BEGIN Event
	UseEvent		No
	EventValue		0.000000
	Direction		Both
	CreateFile		No
END Event
	UseScenUnits		Yes
END Element

BEGIN Element
	Name		Astrogator Values-Cartesian Elems-Y(Earth(J2000))
	IsIndepVar		No
	IndepVarName		Time
	Title		Y(Earth(J2000))
	NameInTitle		Yes
	Service		Astrogator
	Type		Cartesian Elems
	Element		Y(Earth(J2000))
	Format		%.14e
	SumAllowedMask		1559
	SummaryOnly		No
	DataType		0
	UnitType		0
	LineStyle		0
	LineWidth		0
	LineColor		#000000
	PointStyle		0
	PointSize		0
	FillPattern		0
	FillColor		#000000
	PropMask		0
BEGIN Event
	UseEvent		No
	EventValue		0.000000
	Direction		Both
	CreateFile		No
END Event
	UseScenUnits		Yes
END Element

BEGIN Element
	Name		Astrogator Values-Cartesian Elems-Z(Earth(J2000))
	IsIndepVar		No
	IndepVarName		Time
	Title		Z(Earth(J2000))
	NameInTitle		Yes
	Service		Astrogator
	Type		Cartesian Elems
	Element		Z(Earth(J2000))
	Format		%.14e
	SumAllowedMask		1559
	SummaryOnly		No
	DataType		0
	UnitType		0
	LineStyle		0
	LineWidth		0
	LineColor		#000000
	PointStyle		0
	PointSize		0
	FillPattern		0
	FillColor		#000000
	PropMask		0
BEGIN Event
	UseEvent		No
	EventValue		0.000000
	Direction		Both
	CreateFile		No
END Event
	UseScenUnits		Yes
END Element

BEGIN Element
	Name		Astrogator Values-Cartesian Elems-Vx(Earth(J2000))
	IsIndepVar		No
	IndepVarName		Time
	Title		Vx(Earth(J2000))
	NameInTitle		Yes
	Service		Astrogator
	Type		Cartesian Elems
	Element		Vx(Earth(J2000))
	Format		%.14e
	SumAllowedMask		1559
	SummaryOnly		No
	DataType		0
	UnitType		4
	LineStyle		0
	LineWidth		0
	LineColor		#000000
	PointStyle		0
	PointSize		0
	FillPattern		0
	FillColor		#000000
	PropMask		0
BEGIN Event
	UseEvent		No
	EventValue		0.000000
	Direction		Both
	CreateFile		No
END Event
	UseScenUnits		Yes
END Element

BEGIN Element
	Name		Astrogator Values-Cartesian Elems-Vy(Earth(J2000))
	IsIndepVar		No
	IndepVarName		Time
	Title		Vy(Earth(J2000))
	NameInTitle		Yes
	Service		Astrogator
	Type		Cartesian Elems
	Element		Vy(Earth(J2000))
	Format		%.14e
	SumAllowedMask		1559
	SummaryOnly		No
	DataType		0
	UnitType		4
	LineStyle		0
	LineWidth		0
	LineColor		#000000
	PointStyle		0
	PointSize		0
	FillPattern		0
	FillColor		#000000
	PropMask		0
BEGIN Event
	UseEvent		No
	EventValue		0.000000
	Direction		Both
	CreateFile		No
END Event
	UseScenUnits		Yes
END Element

BEGIN Element
	Name		Astrogator Values-Cartesian Elems-Vz(Earth(J2000))
	IsIndepVar		No
	IndepVarName		Time
	Title		Vz(Earth(J2000))
	NameInTitle		Yes
	Service		Astrogator
	Type		Cartesian Elems
	Element		Vz(Earth(J2000))
	Format		%.14e
	SumAllowedMask		1559
	SummaryOnly		No
	DataType		0
	UnitType		4
	LineStyle		0
	LineWidth		0
	LineColor		#000000
	PointStyle		0
	PointSize		0
	FillPattern		0
	FillColor		#000000
	PropMask		0
BEGIN Event
	UseEvent		No
	EventValue		0.000000
	Direction		Both
	CreateFile		No
END Event
	UseScenUnits		Yes
END Element
END Line
END Section

BEGIN Section
	Name		Section 2
	ClassName		Satellite
	NameInTitle		No
	ExpandMethod		0
	PropMask		2
	ShowIntervals		No
	NumIntervals		0
	NumLines		1

BEGIN Line
	Name		Line 1
	NumElements		7

BEGIN Element
	Name		Time
	IsIndepVar		Yes
	IndepVarName		Time
	Title		Time
	NameInTitle		No
	Service		Astrogator
	Type		Target Vector
	Element		Time
	SumAllowedMask		0
	SummaryOnly		No
	DataType		0
	UnitType		2
	LineStyle		0
	LineWidth		0
	LineColor		#000000
	PointStyle		0
	PointSize		0
	FillPattern		0
	FillColor		#000000
	PropMask		0
	UseScenUnits		Yes
END Element

BEGIN Element
	Name		Astrogator Values-Keplerian Elems-Radius_Of_Periapsis
	IsIndepVar		No
	IndepVarName		Time
	Title		Radius_Of_Periapsis
	NameInTitle		Yes
	Service		Astrogator
	Type		Keplerian Elems
	Element		Radius_Of_Periapsis
	Format		%.14e
	SumAllowedMask		1559
	SummaryOnly		No
	DataType		0
	UnitType		0
	LineStyle		0
	LineWidth		0
	LineColor		#000000
	PointStyle		0
	PointSize		0
	FillPattern		0
	FillColor		#000000
	PropMask		0
BEGIN Event
	UseEvent		No
	EventValue		0.000000
	Direction		Both
	CreateFile		No
END Event
	UseScenUnits		Yes
END Element

BEGIN Element
	Name		Astrogator Values-Target Vector-C3_Energy
	IsIndepVar		No
	IndepVarName		Time
	Title		C3_Energy
	NameInTitle		Yes
	Service		Astrogator
	Type		Target Vector
	Element		C3_Energy
	Format		%.14e
	SumAllowedMask		1559
	SummaryOnly		No
	DataType		0
	UnitType		14
	LineStyle		0
	LineWidth		0
	LineColor		#000000
	PointStyle		0
	PointSize		0
	FillPattern		0
	FillColor		#000000
	PropMask		0
BEGIN Event
	UseEvent		No
	EventValue		0.000000
	Direction		Both
	CreateFile		No
END Event
	UseScenUnits		Yes
END Element

BEGIN Element
	Name		Astrogator Values-Target Vector-Incoming_Asymptote_RA(Earth(J2000))
	IsIndepVar		No
	IndepVarName		Time
	Title		Incoming_Asymptote_RA(Earth(J2000))
	NameInTitle		Yes
	Service		Astrogator
	Type		Target Vector
	Element		Incoming_Asymptote_RA(Earth(J2000))
	Format		%.14e
	SumAllowedMask		1559
	SummaryOnly		No
	DataType		0
	UnitType		3
	LineStyle		0
	LineWidth		0
	LineColor		#000000
	PointStyle		0
	PointSize		0
	FillPattern		0
	FillColor		#000000
	PropMask		0
BEGIN Event
	UseEvent		No
	EventValue		0.000000
	Direction		Both
	CreateFile		No
END Event
	UseScenUnits		No
BEGIN Units
		AngleUnit		Degrees
END Units
END Element

BEGIN Element
	Name		Astrogator Values-Target Vector-Incoming_Asymptote_Dec(Earth(J2000))
	IsIndepVar		No
	IndepVarName		Time
	Title		Incoming_Asymptote_Dec(Earth(J2000))
	NameInTitle		Yes
	Service		Astrogator
	Type		Target Vector
	Element		Incoming_Asymptote_Dec(Earth(J2000))
	Format		%.14e
	SumAllowedMask		1559
	SummaryOnly		No
	DataType		0
	UnitType		3
	LineStyle		0
	LineWidth		0
	LineColor		#000000
	PointStyle		0
	PointSize		0
	FillPattern		0
	FillColor		#000000
	PropMask		0
BEGIN Event
	UseEvent		No
	EventValue		0.000000
	Direction		Both
	CreateFile		No
END Event
	UseScenUnits		No
BEGIN Units
		AngleUnit		Degrees
END Units
END Element

BEGIN Element
	Name		Astrogator Values-Target Vector-Incoming_Vel_Az_at_Periapsis(Earth(J2000))
	IsIndepVar		No
	IndepVarName		Time
	Title		Incoming_Vel_Az_at_Periapsis(Earth(J2000))
	NameInTitle		Yes
	Service		Astrogator
	Type		Target Vector
	Element		Incoming_Vel_Az_at_Periapsis(Earth(J2000))
	Format		%.14e
	SumAllowedMask		1559
	SummaryOnly		No
	DataType		0
	UnitType		3
	LineStyle		0
	LineWidth		0
	LineColor		#000000
	PointStyle		0
	PointSize		0
	FillPattern		0
	FillColor		#000000
	PropMask		0
BEGIN Event
	UseEvent		No
	EventValue		0.000000
	Direction		Both
	CreateFile		No
END Event
	UseScenUnits		No
BEGIN Units
		AngleUnit		Degrees
END Units
END Element

BEGIN Element
	Name		Astrogator Values-Keplerian Elems-True_Anomaly
	IsIndepVar		No
	IndepVarName		Time
	Title		True_Anomaly
	NameInTitle		Yes
	Service		Astrogator
	Type		Keplerian Elems
	Element		True_Anomaly
	Format		%.14e
	SumAllowedMask		1559
	SummaryOnly		No
	DataType		0
	UnitType		3
	LineStyle		0
	LineWidth		0
	LineColor		#000000
	PointStyle		0
	PointSize		0
	FillPattern		0
	FillColor		#000000
	PropMask		0
BEGIN Event
	UseEvent		No
	EventValue		0.000000
	Direction		Both
	CreateFile		No
END Event
	UseScenUnits		No
BEGIN Units
		AngleUnit		Degrees
END Units
END Element
END Line
END Section

BEGIN LineAnnotations
END LineAnnotations
END ReportStyle

