################################################
#  TEST DRIVER: SERVER -- Test-mjd.py          #
#  Author -- Michael Stark                     #
#  September 30, 2019                          #
################################################

from socket import gethostname
import SimpleSockets

from math import pi
from AttitudeTypes import Quaternion

import time as Clock
from time import sleep

debug = False ##### EDIT THIS TO BE TRUE TO TURN ON DEBUG

def Run(iterations=6):
    # initialize and connect
    print("**** Starting COSMOS Interface ****")
    local = input ("Enter 'y' to use localhost option >")
    if local == 'y':
        serverName = 'localhost'
        print ("Server using localhost option")
    else:
        serverName = gethostname()
        print ("Server getting hostname from socket module")

    if debug:
        print ("Sender initializing server")
    s = SimpleSockets.Server()
    s.Initialize(serverName,port=5000)
    if debug:
        print ("Sender accepting connection")
    s.AcceptConnection()

    # time parameters
    secPerDay = 86400.0
    # start telemetry with 9/30/2019 date & initial elapsed time & stepsize
    timeTag = 28756.0
    t = 0.0
    dt = 1

# initialize attitude kinematics
# initial quaternion is aligned with inertial reference frame
# angular velocity is in deg/sec around +X axis

    q = Quaternion()
    q0 = [0,0,0,1]
    q.Set(q0)
    w = [30*pi/180.0, 0.0, 0.0]

    # write initial attitude data
    message = BuildMessage(timeTag,q,w)
    print ("Sender sending initial attitude data:")
    print (message)
    s.SendMessage(message)
    print ("Message sent")

    # wait for client to signal that it has completed initialization
    message = s.ReceiveMessage()
    print ("Received message from client:")
    print (message)

    # start clock for synchronization outside main processing loop
    now  = Clock.time()
    next = now + dt;

    for k in range(0, iterations):
        t = t + dt
        timeTag = timeTag + dt/secPerDay
        q.Propagate(w,dt,update=True)
        qv = q.Get()
        print ("(k, time tag, elapsed time) = (", k, timeTag, t, ")")
        print ("build & send: ",timeTag,qv[0],qv[1],qv[2],qv[3],w[0],w[1],w[2])
        message = BuildMessage(timeTag,q,w)
        s.SendMessage (message)
        if debug:
            print ("Sender has sent message")
        now = Clock.time()
        if (k<=iterations):
           sleep(next-now)
        next = next + dt
    # end loop

    if debug:
        print ("Sender sending quit message and finalizing")
    s.SendMessage('q')
    message = s.ReceiveMessage()
    print ("Received message from client:")
    print (message)
    s.Finalize()
    print ("**** COSMOS Interface quitting now ****")
# end Run()

def BuildMessage (t, q, w):
    time = '{0: 15.9f} '.format(t)
    quaternion =  q.ToString()
    angvel = ' {0:18.14f} {1:18.14f} {2:18.14f} '.format(w[0],w[1],w[2])
    return time + quaternion + angvel

# end Build Message

# end CosmosInterface

################ MAIN ######################
if __name__ == "__main__":
   Run()
