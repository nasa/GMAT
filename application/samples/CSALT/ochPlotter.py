import os
import matplotlib.pyplot as plt

def getOchData(fname):
    if not os.path.isfile(fname):
        raise Exception("*.och file %s not found" %(fname))
    
    data = []
    indata = False
    with open(fname,'r') as fid:
        for line in fid:
            if "NUM_STATES" in line:
                nx = int(line.split('=')[-1])
            if "NUM_CONTROLS" in line:
                nu = int(line.split('=')[-1])
            if "DATA_START" in line:
                indata = True
                continue
            if "DATA_STOP" in line:
                indata = False
                continue
            if indata is True:
                lsplt = line.split()
                data.append([float(i) for i in lsplt])
    
    return nx, nu, list(zip(*data))

if __name__ == '__main__':
    # *.ocd filename
    fname = 'OrbitRaisingMultiPhase.och'
    
    # Get Data
    nx, nu, data = getOchData(fname)
    
    # Extract Data
    time = data[0]
    state = data[1:1+nx]
    control = data[1+nx:1+nx+nu]
    
    # Plot states
    for i in range(nx):
        plt.figure(i+1)
        plt.plot(time,state[i])
        plt.xlabel('time')
        plt.ylabel('state %s' %(i+1))
        plt.grid()
       
    # Plot controls
    for i in range(nu):
        plt.figure(i+nx+1)
        plt.plot(time,control[i])
        plt.xlabel('time')
        plt.ylabel('control %s' %(i+1))
        plt.grid()
    
    # Deploy plots
    plt.show()