# --------------------------------
# Display one or more trajectories
# --------------------------------
#
# # Example beginning and end times for the plot.
# beginTime = M.Epoch( "24-JUL-2014 01:00:00 UTC" )
# endTime = M.Epoch( "02-AUG-2014 12:00:00 UTC" )

import Monte as M
from mpy.units import *

import mpylab
import matplotlib


def ShowTrajectory(theBoa, theSats, beginTime, endTime, show):

	fig, ax = mpylab.subplots()

	colors = ['red','blue','green','purple','black']
	index = 0
	dates = M.Epoch.range( beginTime, endTime, 60 * sec )

	for scName in theSats:
		# Get trajectory reference
		queryTransfer = M.TrajQuery( theBoa, scName, 'Earth', 'EME2000' )

		# Sample the orbit every 60 seconds for the plot
		statesTransfer = []
		for date in dates:
		   stateData = queryTransfer.state( date )
		   statesTransfer.append( stateData )

		xTransfer = [ state.pos()[0] for state in statesTransfer ]
		yTransfer = [ state.pos()[1] for state in statesTransfer ]

		# Plot the reference orbit (looking down the EME2000 Z-axis)
		ax.plot( xTransfer, yTransfer, color = colors[index], label = scName )

	# Add Earth for scale
	c = matplotlib.patches.Circle( (0, 0), 6378.0, color = 'lightgrey' )
	ax.add_patch( c )
	ax.annotate( "Earth", (0, 0) )
	if show == True:
		mpylab.show()
