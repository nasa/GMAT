
#
#   Plot filter raw residuals
#
#   By default a PDF copy of the plot image and a CSV-file of the plot data are
#   saved in the same directory as the MATFILE.
#
#   usage: plot_raw_residuals.py [-h] [--no_plot] [--no_csv] matfile
#
#   matfile     (required) full path to filter MATLAB output file
#   --no_plot   (optional) don't show plot or create plot PDF files
#   --no_csv    (optional) don't create the CSV data files
#   -h, --help  (optional) show help message
#

from scipy.io import loadmat
from datetime import datetime, timedelta

import matplotlib.pyplot as plt
import pandas as pd
import csv, argparse, os


def load(matfile):

    ekf = loadmat(matfile, squeeze_me=True, struct_as_record=False)
      
    obs      = ekf['Observed']
    computed = ekf['Computed']

    t_matlab = obs.EpochUTC[0,:]

    residuals = pd.DataFrame.from_records(computed.PreUpdateResidual, 
        columns=['Residual-X', 'Residual-Y', 'Residual-Z'])

    residuals['EditFlag'] = pd.DataFrame(computed.MeasurementEditFlag)

    return t_matlab, residuals
    
    
def render(data, outdir, show_plot=True, save_plot=True, save_csv=True):

    t_matlab, residuals = data
    
    t = pd.DataFrame([datetime.fromordinal(int(tm)) + timedelta(days=tm%1) - timedelta(days=366) 
            for tm in t_matlab])
    
    #    
    #   Unedited (accepted) residuals
    #
    
    IACCEPT = residuals['EditFlag'].isin(['N'])

    t_accept = t.loc[IACCEPT]
    residual_accept = residuals.loc[IACCEPT]

    f = plt.figure()
    f.suptitle('Filter Raw Residuals')

    plt.plot(t_accept, residual_accept['Residual-X'], 'o', label='Residual-X')
    plt.plot(t_accept, residual_accept['Residual-Y'], 'o', label='Residual-Y')
    plt.plot(t_accept, residual_accept['Residual-Z'], 'o', label='Residual-Z')

    #
    #   Sigma-edited residuals
    #
    
    ISIG = residuals['EditFlag'].isin(['SIG'])

    t_sig = t.loc[ISIG]
    residual_sig = residuals.loc[ISIG]

    plt.plot(t_sig, residual_sig['Residual-X'], 'x', label='Residual-X-Edited', color='red')
    plt.plot(t_sig, residual_sig['Residual-Y'], 'x', label='Residual-Y-Edited', color='red')
    plt.plot(t_sig, residual_sig['Residual-Z'], 'x', label='Residual-Z-Edited', color='red')

    #   Legend and axes labels

    plt.legend(loc='lower center', ncol=4, mode='expand')
    plt.xlabel('Time UTC')
    plt.ylabel('Km')

    f.autofmt_xdate()
    
    #
    #   Display and save results, per user options
    #
    
    if save_plot:
            
        outfile = os.path.join(outdir, 'filter_raw_residuals.pdf')
        f.savefig(outfile, bbox_inches='tight')

    if save_csv:

        outfile = os.path.join(outdir, 'filter_raw_residuals.csv')

        outframe = residuals
        outframe['Time'] = t
        
        outframe.to_csv(outfile, index=False,
            columns=['Time', 'Residual-X', 'Residual-Y', 'Residual-Z', 'EditFlag'])

    if show_plot:
        plt.show()


if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    
    parser.add_argument('matfile', help='Name of GMAT output MATLAB file')
    parser.add_argument('--no_plot', 
        help='Don\'t show or save plot', action='store_false')
    parser.add_argument('--no_csv', 
        help='Don\'t save data to CSV file', action='store_false')
        
    args = parser.parse_args()
    outdir, _ = os.path.split(args.matfile)   
        
    data = load(args.matfile)   
    
    render(data, outdir, show_plot=args.no_plot, 
        save_plot=args.no_plot, save_csv=args.no_csv)
    