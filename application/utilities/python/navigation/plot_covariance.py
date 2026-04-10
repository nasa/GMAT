
#
#   Plot and export filter or smoother position and velocity covariance
#
#   By default a PDF copy of the plot image and a CSV-file of the plot data are
#   saved in the same directory as the MATFILE.
#
#   usage: plot_covariance.py [-h] [--no_plot] [--no_csv] matfile
#
#   matfile     (required) full path to filter or smoother MATLAB output file
#   --no_plot   (optional) don't show plot or create plot PDF files
#   --no_csv    (optional) don't create the CSV data files
#   -h, --help  (optional) show help message
#

from scipy.io import loadmat
from datetime import datetime, timedelta

import matplotlib.pyplot as plt
import numpy as np
import csv, argparse, sys, os


def load(matfile):

    datadict = loadmat(matfile, squeeze_me=True, struct_as_record=False)
    
    if 'Filter' in datadict:
        runtype = 'Filter'
    elif 'Smoother' in datadict:
        runtype = 'Smoother'
    else:
        print('Error: Cannot find Filter or Smoother structure in', matfile)
        sys.exit()               
    
    run = datadict[runtype]

    t_matlab = run.EpochUTC[0,:]
    
    cov_pos_v = [cov[0,0] for cov in run.CovarianceVNB]
    cov_pos_n = [cov[1,1] for cov in run.CovarianceVNB]
    cov_pos_b = [cov[2,2] for cov in run.CovarianceVNB]

    cov_vel_v = [cov[3,3] for cov in run.CovarianceVNB]
    cov_vel_n = [cov[4,4] for cov in run.CovarianceVNB]
    cov_vel_b = [cov[5,5] for cov in run.CovarianceVNB]

    return t_matlab, runtype, \
        [cov_pos_v, cov_pos_n, cov_pos_b, cov_vel_v, cov_vel_n, cov_vel_b]
        
    
def render(data, outdir, show_plot=True, save_plot=True, save_csv=True):

    t_matlab, runtype, \
        [cov_pos_v, cov_pos_n, cov_pos_b, cov_vel_v, cov_vel_n, cov_vel_b] = data
    
    t = [datetime.fromordinal(int(tm)) + 
            timedelta(days=tm%1) - timedelta(days=366) for tm in t_matlab]

    #
    #   Plot position covariance
    #
    
    f_pos = plt.figure()
    f_pos.suptitle(runtype + ' 1-Sigma Position Uncertainty')

    plt.plot_date(t, np.sqrt(cov_pos_v)*1e3, label='Sigma-V', 
        xdate=True, linestyle='solid', fmt='C0')
    plt.plot_date(t, np.sqrt(cov_pos_n)*1e3, label='Sigma-N', 
        xdate=True, linestyle='solid', fmt='C1')
    plt.plot_date(t, np.sqrt(cov_pos_b)*1e3, label='Sigma-B', 
        xdate=True, linestyle='solid', fmt='C2')

    plt.legend(loc='lower center', ncol=3)
    plt.xlabel('Time UTC')
    plt.ylabel('meters')

    f_pos.autofmt_xdate()

    #
    #   Plot velocity covariance
    #
    
    f_vel = plt.figure()
    f_vel.suptitle(runtype + ' 1-Sigma Velocity Uncertainty')

    plt.plot_date(t, np.sqrt(cov_vel_v)*1e5, label='Sigma-V', 
        xdate=True, linestyle='solid', fmt='C0')
    plt.plot_date(t, np.sqrt(cov_vel_n)*1e5, label='Sigma-N', 
        xdate=True, linestyle='solid', fmt='C1')
    plt.plot_date(t, np.sqrt(cov_vel_b)*1e5, label='Sigma-B', 
        xdate=True, linestyle='solid', fmt='C2')

    plt.legend(loc='lower center', ncol=3)
    plt.xlabel('Time UTC')
    plt.ylabel('cm/sec')

    f_vel.autofmt_xdate()

    # Display and save results, per user options
    
    if save_plot:
            
        outfile = os.path.join(outdir, runtype + '_position_covariance.pdf')
        f_pos.savefig(outfile, bbox_inches='tight')

        outfile = os.path.join(outdir, runtype + '_velocity_covariance.pdf')
        f_vel.savefig(outfile, bbox_inches='tight')

    if save_csv:

        outfile = os.path.join(outdir, runtype + '_covariance.csv')
        csvfile = open(outfile, 'w', newline='')
        csv_writer = csv.writer(csvfile)

        csv_writer.writerow(['Time', 
            'Sigma-Pos-V (km)', 'Sigma-Pos-N (km)', 'Sigma-Pos-B (km)',
            'Sigma-Vel-V (km/sec)', 'Sigma-Vel-N (km/sec)', 'Sigma-Vel-B (km/sec)'])
            
        outarray = np.array([t, 
            np.sqrt(cov_pos_v), np.sqrt(cov_pos_n), np.sqrt(cov_pos_b),
            np.sqrt(cov_vel_v), np.sqrt(cov_vel_n), np.sqrt(cov_vel_b)])

        for row in outarray.transpose():
            csv_writer.writerow(row)
            
        csvfile.close()

    if show_plot:
        plt.show()


if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    
    parser.add_argument('matfile', 
        help='Name of GMAT output MATLAB file')
    parser.add_argument('--no_plot', 
        help='Don\'t show or save plot', action='store_false')
    parser.add_argument('--no_csv', 
        help='Don\'t save data to CSV file', action='store_false')
        
    args = parser.parse_args()
    outdir, _ = os.path.split(args.matfile)   
        
    data = load(args.matfile)   
    
    render(data, outdir, show_plot=args.no_plot, 
        save_plot=args.no_plot, save_csv=args.no_csv)
