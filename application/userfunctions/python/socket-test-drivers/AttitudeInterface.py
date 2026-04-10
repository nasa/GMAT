################################################
#  ATTITUDE INTERFACE -- AttitudeInterface.py  #
#  Author -- Michael Stark                     #
#  September 30, 2019                          #
################################################

import sys
# on MacOS, one needs to explicitly point to site-packages in code, apps opened
# by clicking on icon don't run .profile or .bash_profile, where path is
# generally set. In this
if sys.platform == 'darwin':
   sys.path.append("/Library/Frameworks/Python.framework/Versions/3.6/lib/python3.6/site-packages/")

import SimpleSockets
from AttitudeTypes import Quaternion # for propagation in Get(self,t)

# debug output flag, run with python driver, output not visible for GMAT runs
debug    = False
echoRPY  = False
echoW    = False

receiver = SimpleSockets.ClientWithTimeout()
attitude = []
quit =  False

################################################################################
#  Initialize() & Finalize() operations; set up and shut down socket           #
################################################################################

def Initialize (serverName='localhost', port = 5000, timeout=0.0):
    global receiver
    receiver.Initialize(serverName,int(port),timeout)
# end Initialize()

def Finalize():
    global receiver
    receiver.Finalize()
# end Finalize()

################################################################################
#  Read() and Write() operations, interface to socket receive and send         #
#     Read()  has the logic to parse the message sent by the server            #
#     Write() simply sends a message string to the server, is intended for     #
#             acknowledgements when they are needed                            #
################################################################################

def Read():
# computes list of 8 real numbers representing attitude at a given time
# and stores the value
# list = [t, q1, q2, q3, q4, wx, wy, wz]

    global receiver, attitude, quit, debug
    if debug:
        print("*** Entering AttitudeInterface.Read() ***")

    message = receiver.ReceiveMessage()
    if debug:
        print ("Message read = ",message)

    if NewData():
        if (message == 'q') or (message == 'quit') or EndOfData():
            if debug:
                print("Attitude Interface detects quit message: ",message)
            quit = True
        else:
            list = message.split()
            length = len(list)
            if debug:
               print ("    list =",list," length=",length)
            if length == 8:
                attitude = []
                for i in range(length):
                    attitude.append(float(list[i]))
                if debug:
                    print ("    attitude =", attitude)

            else:
                print ("** AttitudeInterface: attitude vector length /= 8")
                sys.exit(1)
            # end if
        # end if quitting time
    else:
        # no new data
        pass
    #end if

    if debug:
        print("Newdata=",NewData(),", endOfData=",EndOfData() )
        print (" *** Exiting AttitudeInterface.Read() ***")

# end Read()

def Write(message):
    if debug:
        print ("*** Entering AttitudeInterface.Write() ***")
        print ("   Sending Message >", message)

    receiver.SendMessage(message)

    if debug:
        print ("    Message sent")
        print ("*** Exiting AttitudeInterface.Write() ***")

# end Write()

################################################################################
#  State functions returning state components or computed roll, pitch & yaw    #
################################################################################

def State():
   # returns stored attitude, attitude = [t, q1, q2, q3, q4, wx, wy, wz]
   return attitude
# end Get()

def StateT():
   return attitude[0]
# end GetT()

def StateQ():
   return attitude[1:5]
# end GetQ()

def StateRPY():
   return Quaternion(attitude[1:5]).GetRPY()
# end GetRPY()

def StateW():
   return attitude[5:8]
# end GetW()

################################################################################
#  Get functions that propagate state to requested time                        #
################################################################################

def Get(t):
    # returns quaternion and angular velocity at time t
    if t == attitude[0]:
        # matches time tag on last message
        if debug:
            print ("returning stored attitude tagged with time",t)
        retval = attitude[1:5] + attitude[5:8]
    else:
        # need to propagate
        t0 = attitude[0]
        q0 = Quaternion(attitude[1:5])
        w  = attitude[5:8]
        if debug:
            print("return value is propagated from",t0," to ",t)
        # the "+" below indicates array concatenation
        dt = (t-t0)*86400
        retval = q0.Propagate(w,dt) + w
    #end if
    return retval

# end Get(t)

def GetQ(t):
# returns state quaternion at time t
    if t == attitude[0]:
        # matches time tag on last message
        if debug:
            print ("returning stored quaternion tagged with time ",t)
        retval = attitude[1:5]
    else:
        # need to propagate
        t0 = attitude[0]
        q0 = Quaternion(attitude[1:5])
        w  = attitude[5:8]
        if debug:
            print("return value is propagated from ",t0," to ",t)
        dt = (t-t0)*86400
        retval = q0.Propagate(w,dt)
    #end if

    return retval

# end GetQ(t)

def GetW(t):
    # angular velocity is constant until next record is read from socket
    retval = attitude[5:8]
    if echoW:
       print ("returning W=",retval)
    return retval

# end GetW(t)

def GetRPY(t):
    # returns roll, pitch & yaw (321 Euler sequence) at time t
    if attitude[0] == t:
        # return roll, pitch & yaw equivalent to stored quaternion
        if debug:
            print ("computing roll, pitch and yaw from stored quaternion")
        retval = StateRPY()

    else:
        t0 = attitude[0]
        q0 = Quaternion(attitude[1:5])
        w  = attitude[5:8]
        if debug:
            line = "rpy propatation from {0:13.7f} to {0:13.7f}"
            print(line.format(t0,t))
        dt = (t-t0)*86400
        q = Quaternion(q0.Propagate(w, dt)) # can change to simplify
        retval = q.GetRPY()

    # end if

    if echoRPY:
        print("returning RPY=",retval)

    return retval

# end GetRPY(t)

################################################################################
#                               BOOLEAN functions                              #
################################################################################

def TimeToQuit():
    if debug:
        print ("TimeToQuit() returning ",quit)
    return quit
# end TimeToQuit()

def EndOfData():
    global receiver, debug
    isEOD = receiver.IsEndofData()
    if debug:
       print ("EndOfData(): end of data is",isEOD)
    return isEOD
# end EndOfData()


def NewData():
    global receiver, debug
    isNew = receiver.IsNewData()
    if debug:
       print ("NewData(): It is ",isNew," that data is new")
    return isNew
# end NewData()

def SetDebug():
   global debug, echoRPY, echoW
   debug    = True
   echoRPY  = True
   echoW    = True

def ClearDebug():
   global debug, echoRPY, echoW
   debug    = False
   echoRPY  = False
   echoW    = False
