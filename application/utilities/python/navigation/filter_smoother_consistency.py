
#
#   Plot and export filter/smoother consistency tests
#
#   Generates filter/smoother consistency plots for position, velocity, and all 
#   solve-fors in the run.
#
#   By default a PDF copy of the plot image and a CSV-file of the plot data are
#   saved in the same directory as the MATFILE.
#
#   usage: filter_smoother_consistency.py [-h] [--no_plot] [--no_csv]
#                                     filter_matfile smoother_matfile
#
#   filter_matfile     (required) full path to filter MATLAB output file
#   smoother_matfile   (required) full path to smoother MATLAB output file
#   --no_plot          (optional) don't show plot or create plot PDF files
#   --no_csv           (optional) don't create the CSV data files
#   -h, --help         (optional) show help message
#

from scipy.io import loadmat
from datetime import datetime, timedelta

import matplotlib.pyplot as plt
import numpy as np
import csv, argparse, os

from xyz_to_vnb import xyz_to_vnb


def load(filter_matfile, smoother_matfile):

    filmat = loadmat(filter_matfile, squeeze_me=True, struct_as_record=False)
    smomat = loadmat(smoother_matfile, squeeze_me=True, struct_as_record=False)
      
    try:
    
        fil = filmat['Filter']
        
    except KeyError:

        # This in case the user provided them in reverse order
        
        fil = smomat['Filter']
        smo = filmat['Smoother']
        
    else:
    
        smo = smomat['Smoother']
    
    
    estimationConfig = filmat['EstimationConfig']
    
    #   Omit the last state, because you usually wind up trying to take the
    #   square root of a negative number there
    
    num_states = fil.State.shape[1] - 1

    t_matlab = fil.EpochUTC[0,0:num_states]
        
    #
    #   Compute position and velcity consistency
    #
        
    fil_cov_vnb = np.array([[cov[i,i] for i in range(6)] 
                                for cov in fil.CovarianceVNB])

    smo_cov_vnb = np.array([[cov[i,i] for i in range(6)] 
                                for cov in smo.CovarianceVNB])
                           
    posvel_consistency = np.zeros((6, num_states))
    
    for i in range(num_states):    
        
        rv_fil = fil.State[0:6,i]
        rv_smo = smo.State[0:6,i]
        
        #   Transform states to VNB
        
        m = xyz_to_vnb(rv_fil)
        
        rv_fil_vnb = np.dot(m, rv_fil)
        rv_smo_vnb = np.dot(m, rv_smo)

        #   Compute the consistency metric
        
        delta_posvel_vnb = rv_fil_vnb - rv_smo_vnb        
        delta_sigma_vnb  = np.sqrt(fil_cov_vnb[i,:] - smo_cov_vnb[i,:])
        
        posvel_consistency[:,i] = np.divide(delta_posvel_vnb, delta_sigma_vnb)
    
    #
    #   Compute consistency of any other solve-fors
    #
    
    num_params = len(estimationConfig.StateNames)
        
    param_name = []
    param_consistency = np.zeros((num_params-6, num_states))

    for j in range(6, num_params, 1):
    
        param_name.append(estimationConfig.StateNames[j])
    
        fil_param_sig = [cov[j,j] for cov in fil.Covariance]
        smo_param_sig = [cov[j,j] for cov in smo.Covariance]

        for i in range(num_states):    

            delta_param = fil.State[j,i] - smo.State[j,i]           
            param_consistency[j-6,i] = delta_param / np.sqrt(fil_param_sig[i] - smo_param_sig[i])
       
    # Done-zo
    
    return t_matlab, posvel_consistency, param_name, param_consistency
    
    
def render(data, outdir, show_plot=True, save_plot=True, save_csv=True):

    t_matlab, posvel_consistency, param_name, param_consistency = data
    
    t = [datetime.fromordinal(int(tm)) + 
            timedelta(days=tm%1) - timedelta(days=366) for tm in t_matlab]

    #
    #   Plot position consistency
    #
    
    pos_consistency = posvel_consistency[0:3,:]
    
    f = plt.figure()
    f.suptitle('Filter-Smoother Position Consistency')

    plt.plot_date(t, pos_consistency[0,:], label='Consistency-V', 
        xdate=True, linestyle='solid', fmt='C0')
    plt.plot_date(t, pos_consistency[1,:], label='Consistency-N', 
        xdate=True, linestyle='solid', fmt='C1')
    plt.plot_date(t, pos_consistency[2,:], label='Consistency-B', 
        xdate=True, linestyle='solid', fmt='C2')

    # Add 3-sigma indicator line

    sigma = 3
    plus_sigma  = [+sigma] * len(t)
    minus_sigma = [-sigma] * len(t)

    plt.plot_date(t,  plus_sigma, linestyle=(0,(5,5)), 
        alpha=0.5, linewidth=0.7, fmt='k')
    plt.plot_date(t, minus_sigma, linestyle=(0,(5,5)), 
        alpha=0.5, linewidth=0.7, fmt='k')

    plt.legend(loc='lower center', ncol=4, mode='expand')
    plt.xlabel('Time UTC')
    plt.ylabel('Unitless')

    f.autofmt_xdate()
 
    if save_plot:

        outfile = os.path.join(outdir, 'fs_position_consistency.pdf')
        f.savefig(outfile, bbox_inches='tight')
    
    if save_csv:
    
        outfile = os.path.join(outdir, 'fs_position_consistency.csv')
        csvfile = open(outfile, 'w', newline='')
        csv_writer = csv.writer(csvfile)
    
        csv_writer.writerow(['Time', 'Consistency-V', 'Consistency-N', 'Consistency-B'])
    
        outarray = np.array([t, 
            pos_consistency[0,:], pos_consistency[1,:], pos_consistency[2,:]])

        for row in outarray.transpose():
            csv_writer.writerow(row)
            
        csvfile.close()
            
    #
    #   Plot velocity consistency
    #
    
    vel_consistency = posvel_consistency[3:6,:]
    
    f = plt.figure()
    f.suptitle('Filter-Smoother Velocity Consistency')

    plt.plot_date(t, vel_consistency[0,:], label='Consistency-V', 
        xdate=True, linestyle='solid', fmt='C0')
    plt.plot_date(t, vel_consistency[1,:], label='Consistency-N', 
        xdate=True, linestyle='solid', fmt='C1')
    plt.plot_date(t, vel_consistency[2,:], label='Consistency-B', 
        xdate=True, linestyle='solid', fmt='C2')

    # Add 3-sigma indicator line

    sigma = 3
    plus_sigma  = [+sigma] * len(t)
    minus_sigma = [-sigma] * len(t)

    plt.plot_date(t,  plus_sigma, linestyle=(0,(5,5)), 
        alpha=0.5, linewidth=0.7, fmt='k')
    plt.plot_date(t, minus_sigma, linestyle=(0,(5,5)), 
        alpha=0.5, linewidth=0.7, fmt='k')

    plt.legend(loc='lower center', ncol=4, mode='expand')
    plt.xlabel('Time UTC')
    plt.ylabel('Unitless')

    f.autofmt_xdate()
 
    if save_plot:

        outfile = os.path.join(outdir, 'fs_velocity_consistency.pdf')
        f.savefig(outfile, bbox_inches='tight')
    
    if save_csv:
    
        outfile = os.path.join(outdir, 'fs_velocity_consistency.csv')
        csvfile = open(outfile, 'w', newline='')
        csv_writer = csv.writer(csvfile)
    
        csv_writer.writerow(['Time', 'Consistency-V', 'Consistency-N', 'Consistency-B'])
    
        outarray = np.array([t, 
            pos_consistency[0,:], pos_consistency[1,:], pos_consistency[2,:]])

        for row in outarray.transpose():
            csv_writer.writerow(row)
            
        csvfile.close()
 
    #
    #   Plot any other estimated parameters
    #
    
    for i in range(len(param_name)):
    
        param = param_name[i]

        f = plt.figure()
        f.suptitle('Filter-Smoother ' + param + ' Consistency')

        plt.plot_date(t, param_consistency[i,:], label=param, 
            xdate=True, linestyle='solid', fmt='C0')

        # Add 3-sigma indicator line

        sigma = 3
        plus_sigma  = [+sigma] * len(t)
        minus_sigma = [-sigma] * len(t)

        plt.plot_date(t,  plus_sigma, linestyle=(0,(5,5)), 
            alpha=0.5, linewidth=0.7, fmt='k')
        plt.plot_date(t, minus_sigma, linestyle=(0,(5,5)), 
            alpha=0.5, linewidth=0.7, fmt='k')

        plt.legend(loc='lower center', ncol=4, mode='expand')
        plt.xlabel('Time UTC')
        plt.ylabel('Unitless')

        f.autofmt_xdate()

        # Display and save results, per user options
        
        if save_plot:

            outfile = os.path.join(outdir, 'fs_' + param + '_consistency.pdf')
            f.savefig(outfile, bbox_inches='tight')
        
        if save_csv:
        
            outfile = os.path.join(outdir, 'fs_' + param + '_consistency.csv')
            csvfile = open(outfile, 'w', newline='')
            csv_writer = csv.writer(csvfile)
        
            csv_writer.writerow(['Time', param + ' Consistency'])
        
            outarray = np.array([t, param_consistency[i]])
            
            for row in outarray.transpose():
                csv_writer.writerow(row)
                
            csvfile.close()

    if show_plot:
        plt.show()


if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    
    parser.add_argument('filter_matfile', 
        help='Name of GMAT FILTER output MATLAB file')
    parser.add_argument('smoother_matfile', 
        help='Name of GMAT SMOOTHER output MATLAB file')

    parser.add_argument('--no_plot', 
        help='Don\'t show or save plot', action='store_false')
    parser.add_argument('--no_csv', 
        help='Don\'t save data to CSV file', action='store_false')
        
    args = parser.parse_args()
    outdir, _ = os.path.split(args.filter_matfile)   
        
    data = load(args.filter_matfile, args.smoother_matfile)   
    
    render(data, outdir, show_plot=args.no_plot, 
        save_plot=args.no_plot, save_csv=args.no_csv)
