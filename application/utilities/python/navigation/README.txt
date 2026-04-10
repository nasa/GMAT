The navigation subdirectory contains some utilities that are useful for 
analysing the output of Extended Kalman Filter runs, but these scripts require 
use of the GMAT MATLAB interface. These scripts were built to be used with the 
GPS_PosVec filter/smoother tutorial script and only currently work with the 
GPS_PosVec data type.

    export_measurements.py - Export run measurement and residual data to a CSV file
    filter_smoother_consistency.py -  Plot filter/smoother consistency tests
    plot_cond_cov.py - Plot filter and smoother covariance condition numbers
    plot_covariance.py - Plot filter or smoother position and velocity covariance
    plot_raw_residuals.py - Plot filter raw residuals
    plot_residual_ratios.py - Plot filter residual ratios
    plot_solve_fors.py - Plot all solve-fors in a filter or smoother run
    
The navigation subdirectory also contains scripts for conversion of CCSDS 
tracking data message (TDM), Deep Space Network TRK-2-34, and Universal Tracking
Data Format (UTDF) binary files to GMAT GMD format. At this time, these scripts 
only convert a small subset of data types and are intended as examples and not 
for general purpose or operational use.

The utdf_to_gmd.py script supports conversion of UTDF three-way RangeRate, TDRS 
one-way return Doppler, and TDRS DOWD to GMD. 

The tdm_to_gmd.py script supports conversion of CCSDS TDM DOPPLER_INTEGRATED to 
GMAT RangeRate, TRANSMIT_FREQUENCY to GMAT ramp records, RECEIVE_FREQUENCY to 
GMAT DSN_TCP, and RANGE to GMAT Range or DSN_SeqRange records. 

The trk234_to_gmd.py script supports conversion of DSN TRK-2-34 Total Count 
Phase (TCP), Pseudo-random Noise (PN) range, and ramp records to GMD format.