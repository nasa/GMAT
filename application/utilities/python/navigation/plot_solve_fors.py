
#
#   Plot and export all solve-fors in a filter or smoother run
#
#   By default a PDF copy of the plot image and a CSV-file of the plot data are
#   saved in the same directory as the MATFILE.
#
#   usage: plot_solve_fors.py [-h] [--no_plot] [--no_csv] matfile
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

    estimationConfig = datadict['EstimationConfig']
    nstates = len(estimationConfig.StateNames)
        
    if 'Filter' in datadict:
        runtype = 'Filter'
    elif 'Smoother' in datadict:
        runtype = 'Smoother'
    else:
        print('Error: Cannot find Filter or Smoother structure in', matfile)
        sys.exit()               
    
    run = datadict[runtype]

    t_matlab = run.EpochUTC[0,:]

    param_name = []
    param_vals = []
    param_sigs = []

    for i in range(6, nstates, 1):
    
        param_name.append(estimationConfig.StateNames[i])
        param_vals.append(run.State[i,:])
    
        param_cov = [cov[i,i] for cov in run.Covariance]
        param_sig = np.sqrt(param_cov)
        
        param_sigs.append(param_sig)
        
    return t_matlab, runtype, [param_name, param_vals, param_sigs]
        
    
def render(data, outdir, show_plot=True, save_plot=True, save_csv=True):

    t_matlab, runtype, [param_name, param_vals, param_sigs] = data

    t = [datetime.fromordinal(int(tm)) + 
            timedelta(days=tm%1) - timedelta(days=366) for tm in t_matlab]

    for i in range(len(param_name)):

        param = param_name[i]
        
        f = plt.figure()
        f.suptitle(runtype + ' ' + param + ' and 3-Sigma Uncertainty')

        plt.plot_date(t, param_vals[i], label=param, 
            xdate=True, linestyle='solid', marker=None)
            
        plus_sigma  = param_vals[i] + 3 * param_sigs[i]
        minus_sigma = param_vals[i] - 3 * param_sigs[i]

        plt.plot_date(t, plus_sigma, label=param + ' 3-sigma uncertainty', 
            xdate=True, linestyle='dashed', marker=None, color='green')
            
        plt.plot_date(t, minus_sigma, label=None, 
            xdate=True, linestyle='dashed', marker=None, color='green')

        plt.fill_between(t, plus_sigma, minus_sigma, 
            color='seagreen', alpha=0.1, label=None)

        plt.rc('grid', linestyle='dashed', color='gray', alpha=0.5)
        plt.grid()
        plt.legend(loc='lower center', ncol=3)
        plt.xlabel('Time UTC')

        f.autofmt_xdate()

        # Display and save results, per user options
        
        if save_plot:
            
            outfile = os.path.join(outdir, runtype + '_' + param + '.pdf')
            f.savefig(outfile, bbox_inches='tight')

        if save_csv:

            outfile = os.path.join(outdir, runtype + '_' + param + '.csv')
            csvfile = open(outfile, 'w', newline='')

            csv_writer = csv.writer(csvfile)
            
            csv_writer.writerow(['Time', param, 'Sigma-'+param])
            outarray = np.array([t, param_vals[i], param_sigs[i]])

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
    