
#
#   Plot filter residual ratios
#
#   By default a PDF copy of the plot image and a CSV-file of the plot data are
#   saved in the same directory as the MATFILE.
#
#   usage: plot_residual_ratios.py [-h] [--no_plot] [--no_csv] matfile
#
#   matfile     (required) full path to filter MATLAB output file
#   --no_plot   (optional) don't show plot or create plot PDF files
#   --no_csv    (optional) don't create the CSV data files
#   -h, --help  (optional) show help message
#

from scipy.io import loadmat
from datetime import datetime, timedelta

import matplotlib.pyplot as plt
import numpy as np
import csv, argparse, os


def load(matfile):
    
    ekf = loadmat(matfile, squeeze_me=True, struct_as_record=False)
      
    obs  = ekf['Observed']
    comp = ekf['Computed']

    t_matlab = obs.EpochUTC[0,:]

    scaled_residual_x = [vec[0] for vec in comp.ScaledResidual]
    scaled_residual_y = [vec[1] for vec in comp.ScaledResidual]
    scaled_residual_z = [vec[2] for vec in comp.ScaledResidual]

    return t_matlab, [scaled_residual_x, scaled_residual_y, scaled_residual_z]

    
def render(data, outdir, show_plot=True, save_plot=True, save_csv=True):

    t_matlab, [scaled_residual_x, scaled_residual_y, scaled_residual_z] = data

    t = [datetime.fromordinal(int(tm)) + timedelta(days=tm%1) - timedelta(days=366) 
            for tm in t_matlab]

    # Display and save results, per user options
    
    f = plt.figure()
    f.suptitle('Filter Scaled Residuals')

    plt.plot(t, scaled_residual_x, 'x', label='PosVec-X')
    plt.plot(t, scaled_residual_y, 'x', label='PosVec-Y')
    plt.plot(t, scaled_residual_z, 'x', label='PosVec-Z')

    # Add scaled residual threshold

    sigma = 3
    plus_sigma  = [+sigma] * len(t)
    minus_sigma = [-sigma] * len(t)

    plt.fill_between(t, plus_sigma, minus_sigma, 
        color='seagreen', alpha=0.1, label='Accepted')

    plt.legend(loc='lower center', ncol=4, mode='expand')
    plt.xlabel('Time UTC')
    plt.ylabel('Unitless')

    f.autofmt_xdate()

    # Display and save results, per user options
    
    if save_plot:
            
        outfile = os.path.join(outdir, 'filter_scaled_residuals.pdf')
        f.savefig(outfile, bbox_inches='tight')
        
    if save_csv:
    
        outfile = os.path.join(outdir, 'filter_scaled_residuals.csv')
        csvfile = open(outfile, 'w', newline='')
        csv_writer = csv.writer(csvfile)

        csv_writer.writerow(['Time', 'ScaledResidual-X', 'ScaledResidual-Y', 'ScaledResidual-Z'])
        outarray = np.array([t, scaled_residual_x, scaled_residual_y, scaled_residual_z])

        for row in outarray.transpose():
            csv_writer.writerow(row)
    
        csvfile.close()

    if show_plot:
        plt.show()
    

if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    
    parser.add_argument('matfile', help='Name of GMAT output MATLAB file')
    parser.add_argument('--no_plot', help='Don\'t show or save plot', action='store_false')
    parser.add_argument('--no_csv', help='Don\'t save data to CSV file', action='store_false')
        
    args = parser.parse_args()
    outdir, _ = os.path.split(args.matfile)   
        
    data = load(args.matfile)   
    
    render(data, outdir, show_plot=args.no_plot, 
        save_plot=args.no_plot, save_csv=args.no_csv)
    