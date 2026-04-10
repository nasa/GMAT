################################################
#  SIMPLE SOCKETS -- SimpleSockets.py          #
#  Author -- Michael Stark                     #
#  September 30, 2019                          #
################################################

#import sys for unknown exception handlers
import sys

import socket
class Server:
# server header

    def __init__(self):
       self.debug = False;
    
    def Initialize (self,serverName='localhost', port=5000):
        # get socket
        if self.debug:
            print ("*** Entering Server.Initialize()")
            print ("Server.Initialize - getting socket")
        self.s = socket.socket()
        if self.debug:
            print ("Server.Initialize - socket created")

        # bind it to the port
        self.port = port
        if self.debug:
            if serverName == 'localhost':
                print ("Server using 'localhost' for server")
            print ("Server name > ",serverName)
        serverIP = socket.gethostbyname(serverName)
        if self.debug:
            print ("Server.Initialize - binding server ", serverName,
                   " with IP address ",serverIP, "to port ",port)
        self.s.bind((serverName,port))
        if self.debug:
            print ("Server.Initialize - server bound to port")
            print ("Server.Initialize - listening for connection on port ",
                   port)
        self.s.listen(1)
        if self.debug:
            print ("Server.Initialize - connect received")
            print ("*** Exiting Server.Initialize()")
            print ()
    # end Initialize()

    def AcceptConnection (self):
        if self.debug:
            print("*** Entering Server.AcceptConnection()")

        # establish connection
        if self.debug:
            print("Server.AcceptConnection - accepting client connection")
        self.c, self.addr = self.s.accept()
        if self.debug:
            print("Server.AcceptConnection - accepted connection, addr=",
                  self.addr)
            print("***Exiting Server.AcceptConnection")
    # end AcceptConnection()

    def SendMessage (self, message):
        if self.debug:
            print ("*** Entering Server.SendMessage()")

        buf = bytes(message,'utf-8')
        if self.debug:
            print("Server.SendMessage - SENDING: ", message)
        self.c.send(buf)
        if self.debug:
            print("Message sent")
            print("*** Exiting Server.SendMessage()")
    # end SendMessage()

    def ReceiveMessage (self, maxbuf=1024):
        if self.debug:
            print ("*** Entering Server.ReceiveMessage()")

        buf = self.c.recv(maxbuf)
        message = buf.decode('utf-8')
        if self.debug:
            print ("Server.ReceiveMessage - returning ", message)
            print ("*** Exiting Server.ReceiveMessage()")
        return message

    # end ReceiveMessage()

    def Finalize (self):
       self.s.close()
    # end Finalize()
    

    def SetDebug(self):
        self.debug = True;

    def ClearDebug(self):
        self.debug = False;

# end class Server

####################################################################
class Client:
# client header

    def __init__(self):
       self.debug = False;

    def Initialize (self, serverName, port=5000):
        if self.debug:
            print ("*** Entering Client.Initialize()")
            if serverName == 'localhost':
                print ("Client is using 'localhost' option")
        #set up for connecting client to server
        if self.debug:
            clientName = socket.gethostname()
            print("client is ",clientName)
            print("server is ",serverName)
            print("client will connect through port ",port)
  
        #get server IP address from host name
        serverIP = socket.gethostbyname(serverName)
        if self.debug:
            clientIP = socket.gethostbyname(clientName)
            print("client IP address is ",clientIP)
            print("server IP address is ",serverIP)

        # get socket and connect to server
        self.s= socket.socket()
        if self.debug:
            print ("Socket created by client")
            print ("Connecting to server ", serverName, "through port", port)
        self.s.connect((serverIP,port))
        if self.debug:
            print ("Client successfully connected to server")
            print ("*** Exiting Client.Initialize()")

    # end Initialize()

    def SendMessage(self, message):
        if self.debug:
            print ("*** Entering Client.SendMessage()")
            print ("    Sending message:", message)
        
        buf = bytes (message,'utf-8')
        self.s.send(buf)

        if self.debug:
            print ("    Message sent")
            print ("*** Exiting Client.SendMessage()")

    # end SendMessage()

    def ReceiveMessage(self, maxbuf=1024):
        if self.debug:
            print("***Entering Client.ReceiveMessage()")

        buf = self.s.recv(maxbuf)
        if self.debug:
            print ("Client.ReceiveMessage - RECEIVED > ", buf)
        message = buf.decode('utf-8')
        if self.debug:
            print ("Client.ReceiveMessage - returning > ", message)
            print ("***Exiting Client.ReceiveMessage()")
        return message
    # end ReceiveMessage()

    def Finalize(self):
        if self.debug:
            print ("***Entering Client.Finalize()")
            print ("Client.Finalize() closing socket")

        try:        
            self.s.shutdown(socket.SHUT_RD)
            self.s.close()
        except OSError as err:
            errno,errmsg = err.args
            if errno == 57:
                # socket is not connected error
                print ("Server has shut down already")
                #print ("OS Error {0}: {1} has occurred".format(err))
            else:
                print ("OS Error: ",err.args, " has occurred")

        # when others => fire missiles
        except:
            print ("Unanticipated exception in Client.Finalize()") 
            print (sys.exc_info()[0])

        if self.debug:
            print ("Socket closed")
            print ("***Exiting Client.Finalize()")
    # end Finalize()

    # debug flags
    def SetDebug(self):
        self.debug = True;
   
    def ClearDebug(self):
        self.debug = False;

# end class Client

####################################################################
class ClientWithTimeout(Client):

    # directly inherits __init__(self)

    def Initialize (self, serverName, port=5000, timeout = 0.0):
        if self.debug:
            print ("*** Entering ClientWithTimeout.Initialize()")
            if serverName == 'localhost':
                print ("Client is using 'localhost' option")

        Client.Initialize(self, serverName, port)
        self.newData = False
        self.endOfData = False
        self.timeout = timeout
        if self.debug:
            print ("Initialize() calling socket to set timeout")
        self.s.settimeout(timeout)
        if self.debug:
            print ("Initialize() -- timeout has been set")
            print ("*** Exiting ClientWithTimeout.Initialize()")
    # end Initialize()

    # directly inherits SendMessage()

    def ReceiveMessage(self,maxbuf = 1024):
    # returns valid message if one is received, nothing otherwise.
    # internal flags are set depending on timeout and end of data status
        if self.debug:
            print("*** Entering ClientWithTimeout.ReceiveMessage")

        try:
            buf = self.s.recv(maxbuf)

        # timeout returns to caller with no message; caller will poll
        # whether there is new data
        except socket.timeout:
            self.newData = False
            if self.debug:
                print ("socket read timed out, newData = ",
                    self.newData)
            #endif
        # "when others => fire_missiles"
        except:
            print ("*** Exception in ClientWithTimeout.ReceiveMessage")
            print ("message = ",sys.exc_info()[0])
            sys.exit(1)

        # buffer contains null string indicating end of data, or a 
        # non-null text string. Caller polls to determine which it is.
        else:
            if len(buf) == 0:
                print("*** ClientWithTimeout: server stopped sending ***")
                self.endOfData = True
                self.newData = False

            else:
                # buffer has data
                self.newData = True;
                if self.debug:
                    print("newData=",self.newData," buffer = ",buf)
                message = buf.decode('utf-8')
                if self.debug:
                    print("Decoded message = ",message)
                return message

            # endif for null string

        # end try block
    
    # end ReceiveMessage()

    # Finalize()   is directly inherited
    # SetDebug()   is directly inherited
    # ClearDebug() is directly inherited

    def IsNewData(self):
        return self.newData
    # end IsNewData()

    def IsEndofData(self):
        return self.endOfData
    # end IsEndOfData()

# end class ClientWithTimeout
