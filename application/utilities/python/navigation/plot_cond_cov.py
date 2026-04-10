
#
#   Plot and export filter and smoother covariance condition numbers
#
#   Generates plots of the log (base 10) of the filter, backward filter, and 
#   smoother covariance condition number.
#
#   By default a PDF copy of the plot image and a CSV-file of the plot data are
#   saved in the same directory as the MATFILE.
#
#   usage: plot_cond_cov.py [-h] [--no_plot] [--no_csv]
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


def load(filter_matfile, smoother_matfile):

    filmat = loadmat(filter_matfile, squeeze_me=True, struct_as_record=False)
    smomat = loadmat(smoother_matfile, squeeze_me=True, struct_as_record=False)
      
    try:
    
        fil = filmat['Filter']
        
    except KeyError:

        # This in case the user provided them in reverse order
        
        fil = smomat['Filter']
        smo = filmat['Smoother']
        bck = filmat['BackwardFilter']
        
    else:
    
        smo = smomat['Smoother']
        bck = smomat['BackwardFilter']
       
    #
    #   Compute condition numbers of forward filter, backward filter, and smoother
    #
              
    t_fwdfil_matlab = fil.EpochUTC[0,0:fil.State.shape[1]]
    t_bckfil_matlab = bck.EpochUTC[0,0:bck.State.shape[1]]
    t_smooth_matlab = smo.EpochUTC[0,0:smo.State.shape[1]]
        
    fwdfil_cond = np.array([np.linalg.cond(cov) for cov in fil.Covariance])
    bckfil_cond = np.array([np.linalg.cond(cov) for cov in bck.Covariance])
    smooth_cond = np.array([np.linalg.cond(cov) for cov in smo.Covariance])
           
    # Done-zo
    
    return [t_fwdfil_matlab, fwdfil_cond], [t_bckfil_matlab, bckfil_cond], \
        [t_smooth_matlab, smooth_cond]
    
    
def render(data, outdir, show_plot=True, save_plot=True, save_csv=True):

    [t_fwdfil_matlab, fwdfil_cond], [t_bckfil_matlab, bckfil_cond], \
        [t_smooth_matlab, smooth_cond] = data
    
    t_fwdfil = [datetime.fromordinal(int(tm)) + 
            timedelta(days=tm%1) - timedelta(days=366) for tm in t_fwdfil_matlab]

    t_bckfil = [datetime.fromordinal(int(tm)) + 
            timedelta(days=tm%1) - timedelta(days=366) for tm in t_bckfil_matlab]

    t_smooth = [datetime.fromordinal(int(tm)) + 
            timedelta(days=tm%1) - timedelta(days=366) for tm in t_smooth_matlab]

    #
    #   Plot log10 condition numbers
    #
        
    f = plt.figure()
    f.suptitle('log10(cond(covariance))')

    plt.plot_date(t_fwdfil, np.log10(fwdfil_cond), label='Forward Filter', 
        xdate=True, linestyle='solid', fmt='C0')
        
    plt.plot_date(t_bckfil, np.log10(bckfil_cond), label='Backward Filter', 
        xdate=True, linestyle='solid', fmt='C1')
        
    plt.plot_date(t_smooth, np.log10(smooth_cond), label='Smoother', 
        xdate=True, linestyle='solid', fmt='C2')

    plt.legend(loc='lower center', ncol=3, mode='expand')
    plt.xlabel('Time UTC')
    plt.ylabel('Unitless')

    f.autofmt_xdate()
 
    if save_plot:

        outfile = os.path.join(outdir, 'covariance_condition.pdf')
        f.savefig(outfile, bbox_inches='tight')
    
    if save_csv:
    
        outfile = os.path.join(outdir, 'covariance_condition.csv')
        csvfile = open(outfile, 'w', newline='')
        csv_writer = csv.writer(csvfile)
    
        csv_writer.writerow(['Time', 'Forward-Filter', 'Backward-Filter', 'Smoother'])

        outarray = np.array([t_fwdfil, fwdfil_cond, np.flip(bckfil_cond[1:]), smooth_cond], dtype=object)
        
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
