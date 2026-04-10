
#
#   Export run measurement and residual data to a CSV file.
#
#   The CSV file is created in the same directory as the MATFILE.
#
#   usage: export_measurements.py [-h] matfile
#
#   matfile     (required) full path to filter MATLAB output file
#   -h, --help  (optional) show help message
#

from scipy.io import loadmat
from datetime import datetime, timedelta

import pandas as pd
import os, sys, argparse


def load(matfile):

    ekf = loadmat(matfile, squeeze_me=True, struct_as_record=False)
  
    observed = ekf['Observed']
    computed = ekf['Computed']

    t_matlab     = observed.EpochUTC[0,:]
    participants = pd.DataFrame(observed.Participants, columns=['Participants'])
    meas_type    = pd.DataFrame(observed.MeasurementType, columns=['MeasurementType'])
    meas         = pd.DataFrame.from_records(observed.Measurement, columns=['Observed-X', 'Observed-Y', 'Observed-Z'])
    resid        = pd.DataFrame.from_records(computed.PreUpdateResidual, columns=['Residual-X', 'Residual-Y', 'Residual-Z'])
    scaled_resid = pd.DataFrame.from_records(computed.ScaledResidual, columns=['ScaledResidual-X', 'ScaledResidual-Y', 'ScaledResidual-Z'])
    edit_flag    = pd.DataFrame(computed.MeasurementEditFlag, columns=['MeasurementEditFlag'])

    t = pd.DataFrame([datetime.fromordinal(int(tm)) + timedelta(days=tm%1) - timedelta(days=366) 
            for tm in t_matlab], columns=['Epoch-UTC'])

    outframe = pd.concat((t, participants, meas_type, meas, resid, 
        scaled_resid, edit_flag), axis=1)
        
    return outframe
    
    
def render(data, outdir):

    outfile = os.path.join(outdir, 'filter_measurements.csv')
    csvfile = open(outfile, 'w', newline='')
    
    data.to_csv(csvfile, index=False)


if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    
    parser.add_argument('matfile', help='Name of GMAT FILTER output MATLAB file')
        
    args = parser.parse_args()
    outdir, _ = os.path.split(args.matfile)   
        
    data = load(args.matfile)   
    
    render(data, outdir)
       