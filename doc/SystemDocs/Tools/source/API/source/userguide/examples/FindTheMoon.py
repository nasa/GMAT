import gmatpy as gmat

theScript = "ToLuna"
gmat.LoadScript(theScript + ".script")

burn = gmat.GetObject("TOI")
Time = gmat.GetObject("LeoTime")
start = gmat.GetObject("StartEpoch")

print()

closest = 1000000000.0
dt = 4000
dv = 2.9
startTime = 0.0

for i in range(10) :
   for j in range(20) :
      for k in range(25) :
         StartEpoch = k / 2.0
         start.SetRealParameter("Value", StartEpoch / 24) 
         Time.SetRealParameter("Value", 1500.0 + i * 100) 
         burn.SetRealParameter("Element1", 3.0 + j * 0.01) 
         gmat.RunScript()
         MoonDistance = gmat.GetRuntimeObject("MoonDistance")
         theRange = MoonDistance.GetRealParameter("Value")
         if closest > theRange :
            closest = theRange
            dt = Time.GetRealParameter("Value")
            dv = burn.GetRealParameter("Element1")
            startTime = start.GetRealParameter("Value")
            print(closest, "Launch ", startTime, " Coast time: ", dt, "   Delta-V: ", dv, "   Moon Distance: ", theRange)

theSolution = theScript + "_solution.script"
print("\nSaving solution to \n")
start.SetRealParameter("Value", startTime) 
Time.SetRealParameter("Value", dt) 
burn.SetRealParameter("Element1", dv) 
gmat.SaveScript(theSolution)
