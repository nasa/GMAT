# -*- coding: utf-8 -*-
"""
Created on Fri May 11 08:55:52 2018
TRK-2-34 Reader

@author: jmcrensh and pcandell
"""
from datetime import datetime, timedelta
import os

#For arg parse
import argparse
from pathlib import Path
import math
#import pandas
import struct

speedOfLight  = 299792.458
frequencyBias = 2.4e08

TAIMJD_base = datetime(1941, 1, 5, 12)
baseSoy = (TAIMJD_base.timetuple().tm_yday * 86400.0) +  (TAIMJD_base.hour * 3600.0) + (TAIMJD_base.minute * 60.0) + TAIMJD_base.second

def read_sfdu(fin):
    rawsfdu = fin.read(20)
    sfdu = {}
    sfdu['control_auth_id'] = (b''.join(struct.unpack_from('cccc', rawsfdu, offset=0))).decode("utf-8")
    sfdu['sfdu_version_id'] = (struct.unpack_from('c', rawsfdu, offset=4)[0]).decode("utf-8")
    sfdu['sfdu_class_id'] = (struct.unpack_from('c', rawsfdu, offset=5)[0]).decode("utf-8")
    sfdu['reserve2'] = (b''.join(struct.unpack_from('cc', rawsfdu, offset=6))).decode("utf-8")
    sfdu['data_description_id'] = (b''.join(struct.unpack_from('cccc', rawsfdu, offset=8))).decode("utf-8")
    sfdu['sfdu_length'] = struct.unpack_from('>Q', rawsfdu, offset=12)[0]
    
    return sfdu

def read_aggregation_chdo_label(fin):
    rawaggr = fin.read(4)
    aggregation_chdo_label = {}
    aggregation_chdo_label['chdo_type'] = struct.unpack_from('>H', rawaggr, offset=0)[0]
    aggregation_chdo_label['chdo_length'] = struct.unpack_from('>H', rawaggr, offset=2)[0]

    return aggregation_chdo_label

def read_primary_chdo(fin):
    rawchdo = fin.read(8)
    chdo = {}
    chdo['chdo_type'] = struct.unpack_from('>H', rawchdo, offset=0)[0]
    chdo['chdo_length'] = struct.unpack_from('>H', rawchdo, offset=2)[0]
    chdo['mjr_data_class'] = struct.unpack_from('>B', rawchdo, offset=4)[0]
    chdo['mnr_data_class'] = struct.unpack_from('>B', rawchdo, offset=5)[0]
    chdo['mission_id'] = struct.unpack_from('>B', rawchdo, offset=6)[0]
    chdo['format_code'] = struct.unpack_from('>B', rawchdo, offset=7)[0]
    
    return chdo

def read_chdo132(fin):
    rawchdo132 = fin.read(66)
    chdo132 = {}
    # chdo132['chdo_type'] = struct.unpack_from('>H', rawchdo132, offset=0)[0]
    # chdo132['chdo_length'] = struct.unpack_from('>H', rawchdo132, offset=2)[0]
    chdo132['orig_id'] = struct.unpack_from('>B', rawchdo132, offset=4-4)[0]
    chdo132['last_modifier_id'] = struct.unpack_from('>B', rawchdo132, offset=5-4)[0]
    chdo132['reserve1'] = struct.unpack_from('>B', rawchdo132, offset=6-4)[0]
    chdo132['scft_id'] = struct.unpack_from('>B', rawchdo132, offset=7-4)[0]
    chdo132['upl_rec_seq_num'] = struct.unpack_from('>I', rawchdo132, offset=8-4)[0]
    chdo132['rec_seq_num'] = struct.unpack_from('>I', rawchdo132, offset=12-4)[0]
    chdo132['year'] = struct.unpack_from('>H', rawchdo132, offset=16-4)[0]
    chdo132['doy'] = struct.unpack_from('>H', rawchdo132, offset=18-4)[0]
    chdo132['sec'] = struct.unpack_from('>d', rawchdo132, offset=20-4)[0]
    chdo132['rct_day'] = struct.unpack_from('>H', rawchdo132, offset=28-4)[0]
    chdo132['rct_msec'] = struct.unpack_from('>I', rawchdo132, offset=30-4)[0]
    chdo132['ul_dss_id'] = struct.unpack_from('>B', rawchdo132, offset=34-4)[0]
    chdo132['ul_band'] = struct.unpack_from('>B', rawchdo132, offset=35-4)[0]
    chdo132['ul_assembly_num'] = struct.unpack_from('>B', rawchdo132, offset=36-4)[0]          
    chdo132['transmit_num'] = struct.unpack_from('>B', rawchdo132, offset=37-4)[0]
    chdo132['transmit_stat'] = struct.unpack_from('>B', rawchdo132, offset=38-4)[0]
    chdo132['transmit_mode'] = struct.unpack_from('>B', rawchdo132, offset=39-4)[0]
    chdo132['cmd_modul_stat'] = struct.unpack_from('>B', rawchdo132, offset=40-4)[0]
    chdo132['rng_modul_stat'] = struct.unpack_from('>B', rawchdo132, offset=41-4)[0]
    chdo132['fts_vld_flag'] = struct.unpack_from('>B', rawchdo132, offset=42-4)[0]
    chdo132['ul_software_version'] = struct.unpack_from('>B', rawchdo132, offset=43-4)[0]
    chdo132['transmit_time_tag_delay'] = struct.unpack_from('>d', rawchdo132, offset=44-4)[0]           
    chdo132['ul_zheight_corr'] = struct.unpack_from('>f', rawchdo132, offset=52-4)[0]
    chdo132['mod_day'] = struct.unpack_from('>H', rawchdo132, offset=56-4)[0]
    chdo132['mod_msec'] = struct.unpack_from('>I', rawchdo132, offset=58-4)[0]
    chdo132['version_num'] = struct.unpack_from('>B', rawchdo132, offset=62-4)[0]
    chdo132['sub_version_num'] = struct.unpack_from('>B', rawchdo132, offset=63-4)[0]
    chdo132['sub_sub_version_num'] = struct.unpack_from('>B', rawchdo132, offset=64-4)[0]
    chdo132['reserve1b'] = struct.unpack_from('>B', rawchdo132, offset=65-4)[0]
    chdo132['reserve4'] = struct.unpack_from('>I', rawchdo132, offset=66-4)[0]
    
    return chdo132

def read_chdo134(fin):
    rawchdo134 = fin.read(124)
    chdo134 = {}
    # chdo134['chdo_type'] = struct.unpack_from('>H', rawchdo134, offset=0)[0]
    # chdo134['chdo_length'] = struct.unpack_from('>H', rawchdo134, offset=2)[0]
    chdo134['orig_id'] = struct.unpack_from('>B', rawchdo134, offset=4-4)[0]
    chdo134['last_modifier_id'] = struct.unpack_from('>B', rawchdo134, offset=5-4)[0]
    chdo134['reserve1'] = struct.unpack_from('>B', rawchdo134, offset=6-4)[0]
    chdo134['scft_id'] = struct.unpack_from('>B', rawchdo134, offset=7-4)[0]
    chdo134['rec_seq_num'] = struct.unpack_from('>I', rawchdo134, offset=8)[0]
    chdo134['year'] = struct.unpack_from('>H', rawchdo134, offset=12-4)[0]
    chdo134['doy'] = struct.unpack_from('>H', rawchdo134, offset=14-4)[0]
    chdo134['sec'] = struct.unpack_from('>d', rawchdo134, offset=16-4)[0]
    chdo134['rct_day'] = struct.unpack_from('>H', rawchdo134, offset=24-4)[0]
    chdo134['rct_msec'] = struct.unpack_from('>I', rawchdo134, offset=26-4)[0]
    chdo134['stn_stream_src'] = struct.unpack_from('>B', rawchdo134, offset=30-4)[0]
    chdo134['ul_band'] = struct.unpack_from('>B', rawchdo134, offset=31-4)[0]
    chdo134['ul_assembly_num'] = struct.unpack_from('>B', rawchdo134, offset=32-4)[0]          
    chdo134['transmit_num'] = struct.unpack_from('>B', rawchdo134, offset=33-4)[0]
    chdo134['transmit_stat'] = struct.unpack_from('>B', rawchdo134, offset=34-4)[0]
    chdo134['transmit_mode'] = struct.unpack_from('>B', rawchdo134, offset=35-4)[0]
    chdo134['cmd_modul_stat'] = struct.unpack_from('>B', rawchdo134, offset=36-4)[0]
    chdo134['rng_modul_stat'] = struct.unpack_from('>B', rawchdo134, offset=37-4)[0]
    chdo134['transmit_time_tag_delay'] = struct.unpack_from('>d', rawchdo134, offset=38-4)[0]           
    chdo134['ul_zheight_corr'] = struct.unpack_from('>f', rawchdo134, offset=46-4)[0]
    chdo134['dl_dss_id'] = struct.unpack_from('>B', rawchdo134, offset=50-4)[0]
    chdo134['dl_software_version'] = struct.unpack_from('>B', rawchdo134, offset=51-4)[0]
    chdo134['dl_chan_num'] = struct.unpack_from('>B', rawchdo134, offset=52-4)[0]
    chdo134['prdx_mode'] = struct.unpack_from('>B', rawchdo134, offset=53-4)[0]
    chdo134['ul_prdx_stn'] = struct.unpack_from('>B', rawchdo134, offset=54-4)[0]
    chdo134['ul_band_dl'] = struct.unpack_from('>B', rawchdo134, offset=55-4)[0]
    chdo134['array_delay'] = struct.unpack_from('>d', rawchdo134, offset=56-4)[0]
    chdo134['fts_vld_flag'] = struct.unpack_from('>B', rawchdo134, offset=64-4)[0]
    chdo134['carr_lock_stat'] = struct.unpack_from('>B', rawchdo134, offset=65-4)[0]
    chdo134['array_flag'] = struct.unpack_from('>B', rawchdo134, offset=66-4)[0]
    chdo134['lna_num'] = struct.unpack_from('>B', rawchdo134, offset=67-4)[0]
    chdo134['rcv_time_tag_delay'] = struct.unpack_from('>d', rawchdo134, offset=68-4)[0]
    chdo134['dl_zheight_corr'] = struct.unpack_from('>f', rawchdo134, offset=76-4)[0]
    chdo134['vld_ul_stn'] = struct.unpack_from('>B', rawchdo134, offset=80-4)[0]
    chdo134['vld_dop_mode'] = struct.unpack_from('>B', rawchdo134, offset=81-4)[0]
    chdo134['vld_scft_coh'] = struct.unpack_from('>B', rawchdo134, offset=82-4)[0]
    chdo134['vld_dl_band'] = struct.unpack_from('>B', rawchdo134, offset=83-4)[0]
    chdo134['scft_transpd_lock'] = struct.unpack_from('>B', rawchdo134, offset=84-4)[0]
    chdo134['scft_transpd_num'] = struct.unpack_from('>B', rawchdo134, offset=85-4)[0]
    chdo134['reserve2'] = struct.unpack_from('>H', rawchdo134, offset=86-4)[0]
    chdo134['scft_osc_freq'] = struct.unpack_from('>d', rawchdo134, offset=88-4)[0]
    chdo134['scft_transpd_delay'] = struct.unpack_from('>d', rawchdo134, offset=96-4)[0]
    chdo134['scft_transpd_turn_num'] = struct.unpack_from('>I', rawchdo134, offset=104-4)[0]
    chdo134['scft_transpd_turn_den'] = struct.unpack_from('>I', rawchdo134, offset=108-4)[0]
    chdo134['scft_twnc_stat'] = struct.unpack_from('>B', rawchdo134, offset=112-4)[0]
    chdo134['scft_osc_type'] = struct.unpack_from('>B', rawchdo134, offset=113-4)[0]
    chdo134['mod_day'] = struct.unpack_from('>H', rawchdo134, offset=114-4)[0]
    chdo134['mod_msec'] = struct.unpack_from('>I', rawchdo134, offset=116-4)[0]
    chdo134['cnt_time'] = struct.unpack_from('>f', rawchdo134, offset=120-4)[0]
    chdo134['version_num'] = struct.unpack_from('>B', rawchdo134, offset=124-4)[0]
    chdo134['sub_version_num'] = struct.unpack_from('>B', rawchdo134, offset=125-4)[0]
    chdo134['sub_sub_version_num'] = struct.unpack_from('>B', rawchdo134, offset=126-4)[0]
    chdo134['lna_corr_value'] = struct.unpack_from('>B', rawchdo134, offset=127-4)[0]
    
    return chdo134

def read_chdo133(fin):
    rawchdo133 = fin.read(110)
    chdo133 = {}
    # chdo133['chdo_type'] = struct.unpack_from('>H', rawchdo133, offset=0)[0]
    # chdo133['chdo_length'] = struct.unpack_from('>H', rawchdo133, offset=2)[0]
    chdo133['orig_id'] = struct.unpack_from('>B', rawchdo133, offset=4-4)[0]
    chdo133['last_modifier_id'] = struct.unpack_from('>B', rawchdo133, offset=5-4)[0]
    chdo133['reserve1'] = struct.unpack_from('>B', rawchdo133, offset=6-4)[0]
    chdo133['scft_id'] = struct.unpack_from('>B', rawchdo133, offset=7-4)[0]
    chdo133['dtt_rec_seq_num'] = struct.unpack_from('>I', rawchdo133, offset=8-4)[0]
    chdo133['rec_seq_num'] = struct.unpack_from('>I', rawchdo133, offset=12-4)[0]
    chdo133['year'] = struct.unpack_from('>H', rawchdo133, offset=16-4)[0]
    chdo133['doy'] = struct.unpack_from('>H', rawchdo133, offset=18-4)[0]
    chdo133['sec'] = struct.unpack_from('>d', rawchdo133, offset=20-4)[0]
    chdo133['rct_day'] = struct.unpack_from('>H', rawchdo133, offset=28-4)[0]
    chdo133['rct_msec'] = struct.unpack_from('>I', rawchdo133, offset=30-4)[0]
    chdo133['dl_dss_id'] = struct.unpack_from('>B', rawchdo133, offset=34-4)[0]
    chdo133['dl_software_version'] = struct.unpack_from('>B', rawchdo133, offset=35-4)[0]
    chdo133['dl_chan_num'] = struct.unpack_from('>B', rawchdo133, offset=36-4)[0]
    chdo133['prdx_mode'] = struct.unpack_from('>B', rawchdo133, offset=37-4)[0]
    chdo133['ul_prdx_stn'] = struct.unpack_from('>B', rawchdo133, offset=38-4)[0]
    chdo133['ul_band_dl'] = struct.unpack_from('>B', rawchdo133, offset=39-4)[0]
    chdo133['array_delay'] = struct.unpack_from('>d', rawchdo133, offset=40-4)[0]
    chdo133['fts_vld_flag'] = struct.unpack_from('>B', rawchdo133, offset=48-4)[0]
    chdo133['carr_lock_stat'] = struct.unpack_from('>B', rawchdo133, offset=49-4)[0]
    chdo133['array_flag'] = struct.unpack_from('>B', rawchdo133, offset=50-4)[0]
    chdo133['polarization'] = struct.unpack_from('>B', rawchdo133, offset=51-4)[0]
    chdo133['diplexr_stat'] = struct.unpack_from('>B', rawchdo133, offset=52-4)[0]
    chdo133['lna_num'] = struct.unpack_from('>B', rawchdo133, offset=53-4)[0]
    chdo133['rf_if_chan_num'] = struct.unpack_from('>B', rawchdo133, offset=54-4)[0]
    chdo133['if_num'] = struct.unpack_from('>B', rawchdo133, offset=55-4)[0]
    chdo133['rcv_time_tag_delay'] = struct.unpack_from('>d', rawchdo133, offset=56-4)[0]
    chdo133['dl_zheight_corr'] = struct.unpack_from('>f', rawchdo133, offset=64-4)[0]
    chdo133['vld_ul_stn'] = struct.unpack_from('>B', rawchdo133, offset=68-4)[0]
    chdo133['vld_dop_mode'] = struct.unpack_from('>B', rawchdo133, offset=69-4)[0]
    chdo133['vld_scft_coh'] = struct.unpack_from('>B', rawchdo133, offset=70-4)[0]
    chdo133['scft_transpd_lock'] = struct.unpack_from('>B', rawchdo133, offset=71-4)[0]
    chdo133['scft_transpd_num'] = struct.unpack_from('>B', rawchdo133, offset=72-4)[0]
    chdo133['dl_software_version'] = struct.unpack_from('>B', rawchdo133, offset=73-4)[0]
    chdo133['scft_osc_freq'] = struct.unpack_from('>d', rawchdo133, offset=74-4)[0]
    chdo133['scft_transpd_delay'] = struct.unpack_from('>d', rawchdo133, offset=82-4)[0]
    chdo133['scft_transpd_turn_num'] = struct.unpack_from('>I', rawchdo133, offset=90-4)[0]
    chdo133['scft_transpd_turn_den'] = struct.unpack_from('>I', rawchdo133, offset=94-4)[0]
    chdo133['scft_twnc_stat'] = struct.unpack_from('>B', rawchdo133, offset=98-4)[0]
    chdo133['scft_osc_type'] = struct.unpack_from('>B', rawchdo133, offset=99-4)[0]
    chdo133['mod_day'] = struct.unpack_from('>H', rawchdo133, offset=100-4)[0]
    chdo133['mod_msec'] = struct.unpack_from('>I', rawchdo133, offset=102-4)[0]
    chdo133['version_num'] = struct.unpack_from('>B', rawchdo133, offset=106-4)[0]
    chdo133['sub_version_num'] = struct.unpack_from('>B', rawchdo133, offset=107-4)[0]
    chdo133['sub_sub_version_num'] = struct.unpack_from('>B', rawchdo133, offset=108-4)[0]
    chdo133['lna_corr_value'] = struct.unpack_from('>B', rawchdo133, offset=109-4)[0]
    chdo133['reserve4'] = struct.unpack_from('>I', rawchdo133, offset=110-4)[0]
    
    return chdo133

def read_secondary_chdo(fin):
    rawchdo2_head = fin.read(4)
    # chdo2 = {}
    chdo_type = struct.unpack_from('>H', rawchdo2_head, offset=0)[0]
    chdo_length = struct.unpack_from('>H', rawchdo2_head, offset=2)[0]
    if chdo_type == 132:
        chdo2 = read_chdo132(fin)
    elif chdo_type == 133:
        chdo2 = read_chdo133(fin)
    elif chdo_type == 134:
        chdo2 = read_chdo134(fin)
    else:
        chdo2['rawbytes'] = read_bytes(fin, chdo_length)
   
    chdo2['chdo_type'] = chdo_type
    chdo2['chdo_length'] = chdo_length 
   
    return chdo2

def read_type7(fin):
    rawtype7 = fin.read(186)
    type7 = {}
    # type7['chdo_type'] = struct.unpack_from('>H', rawtype7, offset=0)[0]
    # type7['chdo_length'] = struct.unpack_from('>H', rawtype7, offset=2)[0]
    type7['ul_stn_cal'] = struct.unpack_from('>d', rawtype7, offset=-4+4)[0]
    type7['dl_stn_cal'] = struct.unpack_from('>d', rawtype7, offset=-4+12)[0]
    type7['meas_rng'] = struct.unpack_from('>d', rawtype7, offset=-4+20)[0]
    type7['rng_obs'] = struct.unpack_from('>d', rawtype7, offset=-4+28)[0]
    type7['rng_obs_dl'] = struct.unpack_from('>d', rawtype7, offset=-4+36)[0]
    type7['clock_waveform'] = struct.unpack_from('>B', rawtype7, offset=-4+44)[0]
    type7['chop_start_num'] = struct.unpack_from('>B', rawtype7, offset=-4+45)[0]
    type7['figure_merit'] = struct.unpack_from('>f', rawtype7, offset=-4+46)[0]
    type7['drvid'] = struct.unpack_from('>d', rawtype7, offset=-4+50)[0]
    type7['rtlt'] = struct.unpack_from('>f', rawtype7, offset=-4+58)[0]
    type7['prn0'] = struct.unpack_from('>f', rawtype7, offset=-4+62)[0]
    type7['transmit_pwr'] = struct.unpack_from('>f', rawtype7, offset=-4+66)[0]
    type7['invert'] = struct.unpack_from('>B', rawtype7, offset=-4+70)[0]
    type7['correl_type'] = struct.unpack_from('>B', rawtype7, offset=-4+71)[0]
    type7['t1'] = struct.unpack_from('>H', rawtype7, offset=-4+72)[0]
    type7['t2'] = struct.unpack_from('>H', rawtype7, offset=-4+74)[0]
    type7['t3'] = struct.unpack_from('>H', rawtype7, offset=-4+76)[0]
    type7['first_comp_num'] = struct.unpack_from('>B', rawtype7, offset=-4+78)[0]
    type7['last_comp_num'] = struct.unpack_from('>B', rawtype7, offset=-4+79)[0]
    type7['chop_comp_num'] = struct.unpack_from('>B', rawtype7, offset=-4+80)[0]
    type7['num_drvid'] = struct.unpack_from('>B', rawtype7, offset=-4+81)[0]
    type7['transmit_inphs_time'] = struct.unpack_from('>f', rawtype7, offset=-4+82)[0]
    type7['rcv_inphs_time'] = struct.unpack_from('>f', rawtype7, offset=-4+86)[0]
    type7['carr_sup_rng_modul'] = struct.unpack_from('>f', rawtype7, offset=-4+90)[0]
    type7['exc_scalar_num'] = struct.unpack_from('>I', rawtype7, offset=-4+94)[0]
    type7['exc_scalar_den'] = struct.unpack_from('>I', rawtype7, offset=-4+98)[0]
    type7['rng_cycle_time'] = struct.unpack_from('>d', rawtype7, offset=-4+102)[0]
    type7['rng_modulo'] = struct.unpack_from('>I', rawtype7, offset=-4+110)[0]
    type7['inphs_correl'] = struct.unpack_from('>f', rawtype7, offset=-4+114)[0]
    type7['quad_phs_correl'] = struct.unpack_from('>f', rawtype7, offset=-4+118)[0]
    type7['ul_freq'] = struct.unpack_from('>d', rawtype7, offset=-4+122)[0]
    type7['rng_type'] = struct.unpack_from('>B', rawtype7, offset=-4+130)[0]
    type7['fabricated_ul_flag'] = struct.unpack_from('>B', rawtype7, offset=-4+131)[0]
    type7['rng_noise'] = struct.unpack_from('>f', rawtype7, offset=-4+132)[0]
    type7['rng_prefit_resid'] = struct.unpack_from('>d', rawtype7, offset=-4+136)[0]
    type7['rng_dl_prefit_resid'] = struct.unpack_from('>d', rawtype7, offset=-4+144)[0]
    type7['rng_prefit_resid_vld_flag'] = struct.unpack_from('>B', rawtype7, offset=-4+152)[0]
    type7['rng_dl_prefit_resid_vld_flag'] = struct.unpack_from('>B', rawtype7, offset=-4+153)[0]
    type7['rng_resid_tol_value'] = struct.unpack_from('>f', rawtype7, offset=-4+154)[0]
    type7['drvid_tol_value'] = struct.unpack_from('>f', rawtype7, offset=-4+158)[0]
    type7['prn0_resid_tol_value'] = struct.unpack_from('>f', rawtype7, offset=-4+162)[0]
    type7['rng_sigma_tol_value'] = struct.unpack_from('>f', rawtype7, offset=-4+166)[0]
    type7['fom_tol_value'] = struct.unpack_from('>f', rawtype7, offset=-4+170)[0]
    type7['rng_resid_tol_flag'] = struct.unpack_from('>B', rawtype7, offset=-4+174)[0]
    type7['drvid_tol_flag'] = struct.unpack_from('>B', rawtype7, offset=-4+175)[0]
    type7['prn0_resid_tol_flag'] = struct.unpack_from('>B', rawtype7, offset=-4+176)[0]
    type7['rng_sigma_tol_flag'] = struct.unpack_from('>B', rawtype7, offset=-4+177)[0]
    type7['rng_vld_flag'] = struct.unpack_from('>B', rawtype7, offset=-4+178)[0]
    type7['rng_config_flag'] = struct.unpack_from('>B', rawtype7, offset=-4+179)[0]
    type7['stn_cal_corr_flag'] = struct.unpack_from('>B', rawtype7, offset=-4+180)[0]
    type7['rng_chan_num'] = struct.unpack_from('>B', rawtype7, offset=-4+181)[0]
    type7['time_tag_corr_flag'] = struct.unpack_from('>B', rawtype7, offset=-4+182)[0]
    type7['type_time_corr_flag'] = struct.unpack_from('>B', rawtype7, offset=-4+183)[0]
    type7['reserve6'] = 65536*struct.unpack_from('>HI', rawtype7, offset=-4+184)[0] +\
         struct.unpack_from('>HI', rawtype7, offset=-4+184)[1]
    
    return type7

def read_type9(fin):
    rawtype9 = fin.read(38)
    type9 = {}
    # type9['chdo_type'] = struct.unpack_from('>H', rawtype9, offset=0)[0]
    # type9['chdo_length'] = struct.unpack_from('>H', rawtype9, offset=2)[0]
    type9['ul_hi_phs_cycles'] = struct.unpack_from('>I', rawtype9, offset=-4+4)[0]
    type9['ul_lo_phs_cycles'] = struct.unpack_from('>I', rawtype9, offset=-4+8)[0]
    type9['ul_frac_phs_cycles'] = struct.unpack_from('>I', rawtype9, offset=-4+12)[0]
    type9['ramp_freq'] = struct.unpack_from('>d', rawtype9, offset=-4+16)[0]
    type9['ramp_rate'] = struct.unpack_from('>d', rawtype9, offset=-4+24)[0]
    type9['ramp_type'] = struct.unpack_from('>B', rawtype9, offset=-4+32)[0]
    type9['fabricated_sfdu_flag'] = struct.unpack_from('>B', rawtype9, offset=-4+33)[0]
    type9['reserve8'] = struct.unpack_from('>Q', rawtype9, offset=-4+34)[0]
    
    return type9

def read_type0(fin):
    
    rawtype0 = fin.read(76)
    type0 = {}
    # type0['chdo_type'] = struct.unpack_from('>H', rawtype0, offset=0)[0]
    # type0['chdo_length'] = struct.unpack_from('>H', rawtype0, offset=2)[0]
    type0['ul_hi_phs_cycles'] = struct.unpack_from('>I', rawtype0, offset=-4+4)[0]
    type0['ul_lo_phs_cycles'] = struct.unpack_from('>I', rawtype0, offset=-4+8)[0]
    type0['ul_frac_phs_cycles'] = struct.unpack_from('>I', rawtype0, offset=-4+12)[0]
    type0['ramp_freq'] = struct.unpack_from('>d', rawtype0, offset=-4+16)[0]
    type0['ramp_rate'] = struct.unpack_from('>d', rawtype0, offset=-4+24)[0]
    type0['transmit_switch_stat'] = struct.unpack_from('>B', rawtype0, offset=-4+32)[0]
    type0['ramp_type'] = struct.unpack_from('>B', rawtype0, offset=-4+33)[0]
    type0['transmit_op_power'] = struct.unpack_from('>f', rawtype0, offset=-4+34)[0]
    type0['sup_data_id'] = (b''.join(struct.unpack_from('cccccccc', rawtype0, offset=-4+38))).decode("utf-8")
    type0['sup_data_rev'] = (b''.join(struct.unpack_from('cccccccc', rawtype0, offset=-4+46))).decode("utf-8")
    type0['prdx_time_offset'] = struct.unpack_from('>d', rawtype0, offset=-4+54)[0]
    type0['prdx_freq_offset'] = struct.unpack_from('>d', rawtype0, offset=-4+62)[0]
    type0['time_tag_corr_flag'] = struct.unpack_from('>B', rawtype0, offset=-4+70)[0]
    type0['type_time_corr_flag'] = struct.unpack_from('>B', rawtype0, offset=-4+71)[0]
    type0['fabricated_sfdu_flag'] = struct.unpack_from('>B', rawtype0, offset=-4+72)[0]
    type0['reserve1'] = struct.unpack_from('>B', rawtype0, offset=-4+73)[0]
    type0['reserve6'] = 65536*struct.unpack_from('>HI', rawtype0, offset=-4+74)[0] +\
         struct.unpack_from('>HI', rawtype0, offset=-4+74)[1]
    
    return type0

def read_type1(fin):
    
    rawtype1 = fin.read(228)
    type1 = {}
    # type1['chdo_type'] = struct.unpack_from('>H', rawtype1, offset=0)[0]
    # type1['chdo_length'] = struct.unpack_from('>H', rawtype1, offset=2)[0]
    type1['carr_loop_bw'] = struct.unpack_from('>f', rawtype1, offset=-4+4)[0]
    type1['pcn0'] = struct.unpack_from('>f', rawtype1, offset=-4+8)[0]
    type1['pcn0_resid'] = struct.unpack_from('>f', rawtype1, offset=-4+12)[0]
    type1['pdn0'] = struct.unpack_from('>f', rawtype1, offset=-4+16)[0]
    type1['pdn0_resid'] = struct.unpack_from('>f', rawtype1, offset=-4+20)[0]
    type1['system_noise_temp'] = struct.unpack_from('>f', rawtype1, offset=-4+24)[0]
    type1['phs_hi_0'] = struct.unpack_from('>I', rawtype1, offset=-4+28)[0]
    type1['phs_lo_0'] = struct.unpack_from('>I', rawtype1, offset=-4+32)[0]
    type1['phs_frac_0'] = struct.unpack_from('>I', rawtype1, offset=-4+36)[0]
    type1['phs_hi_1'] = struct.unpack_from('>I', rawtype1, offset=-4+40)[0]
    type1['phs_lo_1'] = struct.unpack_from('>I', rawtype1, offset=-4+44)[0]
    type1['phs_frac_1'] = struct.unpack_from('>I', rawtype1, offset=-4+48)[0]
    type1['phs_hi_2'] = struct.unpack_from('>I', rawtype1, offset=-4+52)[0]
    type1['phs_lo_2'] = struct.unpack_from('>I', rawtype1, offset=-4+56)[0]
    type1['phs_frac_2'] = struct.unpack_from('>I', rawtype1, offset=-4+60)[0]
    type1['phs_hi_3'] = struct.unpack_from('>I', rawtype1, offset=-4+64)[0]
    type1['phs_lo_3'] = struct.unpack_from('>I', rawtype1, offset=-4+68)[0]
    type1['phs_frac_3'] = struct.unpack_from('>I', rawtype1, offset=-4+72)[0]
    type1['phs_hi_4'] = struct.unpack_from('>I', rawtype1, offset=-4+76)[0]
    type1['phs_lo_4'] = struct.unpack_from('>I', rawtype1, offset=-4+80)[0]
    type1['phs_frac_4'] = struct.unpack_from('>I', rawtype1, offset=-4+84)[0]
    type1['phs_hi_5'] = struct.unpack_from('>I', rawtype1, offset=-4+88)[0]
    type1['phs_lo_5'] = struct.unpack_from('>I', rawtype1, offset=-4+92)[0]
    type1['phs_frac_5'] = struct.unpack_from('>I', rawtype1, offset=-4+96)[0]
    type1['phs_hi_6'] = struct.unpack_from('>I', rawtype1, offset=-4+100)[0]
    type1['phs_lo_6'] = struct.unpack_from('>I', rawtype1, offset=-4+104)[0]
    type1['phs_frac_6'] = struct.unpack_from('>I', rawtype1, offset=-4+108)[0]
    type1['phs_hi_7'] = struct.unpack_from('>I', rawtype1, offset=-4+112)[0]
    type1['phs_lo_7'] = struct.unpack_from('>I', rawtype1, offset=-4+116)[0]
    type1['phs_frac_7'] = struct.unpack_from('>I', rawtype1, offset=-4+120)[0]
    type1['phs_hi_8'] = struct.unpack_from('>I', rawtype1, offset=-4+124)[0]
    type1['phs_lo_8'] = struct.unpack_from('>I', rawtype1, offset=-4+128)[0]
    type1['phs_frac_8'] = struct.unpack_from('>I', rawtype1, offset=-4+132)[0]
    type1['phs_hi_9'] = struct.unpack_from('>I', rawtype1, offset=-4+136)[0]
    type1['phs_lo_9'] = struct.unpack_from('>I', rawtype1, offset=-4+140)[0]
    type1['phs_frac_9'] = struct.unpack_from('>I', rawtype1, offset=-4+144)[0]
    type1['phs_hi_avg'] = struct.unpack_from('>I', rawtype1, offset=-4+148)[0]
    type1['phs_lo_avg'] = struct.unpack_from('>I', rawtype1, offset=-4+152)[0]
    type1['phs_frac_avg'] = struct.unpack_from('>I', rawtype1, offset=-4+156)[0]
    type1['dl_freq'] = struct.unpack_from('>d', rawtype1, offset=-4+160)[0]
    type1['dop_resid'] = struct.unpack_from('>f', rawtype1, offset=-4+168)[0]
    type1['dop_noise'] = struct.unpack_from('>f', rawtype1, offset=-4+172)[0]
    type1['slipped_cycles'] = struct.unpack_from('>i', rawtype1, offset=-4+176)[0]
    type1['carr_loop_type'] = struct.unpack_from('>B', rawtype1, offset=-4+180)[0]
    type1['snt_flag'] = struct.unpack_from('>B', rawtype1, offset=-4+181)[0]
    type1['carr_resid_wt'] = struct.unpack_from('>f', rawtype1, offset=-4+182)[0]
    type1['sup_data_id'] = (b''.join(struct.unpack_from('cccccccc', rawtype1, offset=-4+186))).decode("utf-8")#, "ignore")
    type1['sup_data_rev'] = (b''.join(struct.unpack_from('cccccccc', rawtype1, offset=-4+194))).decode("utf-8")#, "ignore")
    type1['prdx_time_offset'] = struct.unpack_from('>d', rawtype1, offset=-4+202)[0]
    type1['prdx_freq_offset'] = struct.unpack_from('>d', rawtype1, offset=-4+210)[0]
    type1['carr_resid_tol_flag'] = struct.unpack_from('>B', rawtype1, offset=-4+218)[0]
    type1['time_tag_corr_flag'] = struct.unpack_from('>B', rawtype1, offset=-4+219)[0]
    type1['type_time_corr_flag'] = struct.unpack_from('>B', rawtype1, offset=-4+220)[0]
    type1['dop_mode_corr_flag'] = struct.unpack_from('>B', rawtype1, offset=-4+221)[0]
    type1['ul_stn_corr_flag'] = struct.unpack_from('>B', rawtype1, offset=-4+222)[0]
    type1['reserve1'] = struct.unpack_from('>B', rawtype1, offset=-4+223)[0]
    type1['reserve8'] = struct.unpack_from('>Q', rawtype1, offset=-4+224)[0]
    
    return type1

def read_type2(fin):
    rawtype2 = fin.read(108)
    
    return rawtype2

def read_type4(fin):
    
    rawtype4 = fin.read(190)
    type4 = {}
    # type4['chdo_type'] = struct.unpack_from('>H', rawtype4, offset=0)[0]
    # type4['chdo_length'] = struct.unpack_from('>H', rawtype4, offset=2)[0]
    type4['stn_cal'] = struct.unpack_from('>d', rawtype4, offset=-4+4)[0]
    type4['ul_stn_cal'] = struct.unpack_from('>d', rawtype4, offset=-4+12)[0]
    type4['ul_cal_freq'] = struct.unpack_from('>d', rawtype4, offset=-4+20)[0]
    type4['cal_std_dev'] = struct.unpack_from('>f', rawtype4, offset=-4+28)[0]
    type4['cal_pts'] = struct.unpack_from('>H', rawtype4, offset=-4+32)[0]
    type4['ul_rng_phs'] = struct.unpack_from('>d', rawtype4, offset=-4+34)[0]
    type4['state_subcode1'] = struct.unpack_from('>B', rawtype4, offset=-4+42)[0]
    type4['state_subcode2'] = struct.unpack_from('>B', rawtype4, offset=-4+43)[0]
    type4['state_subcode3'] = struct.unpack_from('>B', rawtype4, offset=-4+44)[0]
    type4['state_subcode4'] = struct.unpack_from('>B', rawtype4, offset=-4+45)[0]
    type4['state_subcode5'] = struct.unpack_from('>B', rawtype4, offset=-4+46)[0]
    type4['state_subcode6'] = struct.unpack_from('>B', rawtype4, offset=-4+47)[0]
    type4['pn_clk_phs'] = struct.unpack_from('>d', rawtype4, offset=-4+48)[0]
    type4['transmit_switch_stat'] = struct.unpack_from('>B', rawtype4, offset=-4+56)[0]
    type4['invert'] = struct.unpack_from('>B', rawtype4, offset=-4+57)[0]
    type4['transmit_op_pwr'] = struct.unpack_from('>f', rawtype4, offset=-4+58)[0]
    type4['template_id'] = (b''.join(struct.unpack_from('cccccccccccccccccccccc', rawtype4, offset=-4+62))).decode("utf-8")
    type4['clk_divider'] = struct.unpack_from('>B', rawtype4, offset=-4+84)[0]
    type4['len_subcode1'] = struct.unpack_from('>B', rawtype4, offset=-4+85)[0]
    type4['len_subcode2'] = struct.unpack_from('>B', rawtype4, offset=-4+86)[0]
    type4['len_subcode3'] = struct.unpack_from('>B', rawtype4, offset=-4+87)[0]
    type4['len_subcode4'] = struct.unpack_from('>B', rawtype4, offset=-4+88)[0]
    type4['len_subcode5'] = struct.unpack_from('>B', rawtype4, offset=-4+89)[0]
    type4['len_subcode6'] = struct.unpack_from('>B', rawtype4, offset=-4+90)[0]
    type4['op_subcode1'] = struct.unpack_from('>B', rawtype4, offset=-4+91)[0]
    type4['op_subcode2'] = struct.unpack_from('>B', rawtype4, offset=-4+92)[0]
    type4['op_subcode3'] = struct.unpack_from('>B', rawtype4, offset=-4+93)[0]
    type4['op_subcode4'] = struct.unpack_from('>B', rawtype4, offset=-4+94)[0]
    type4['op_subcode5'] = struct.unpack_from('>B', rawtype4, offset=-4+95)[0]
    type4['def_subcode1'] = struct.unpack_from('>Q', rawtype4, offset=-4+96)[0]
    type4['def_subcode2'] = struct.unpack_from('>Q', rawtype4, offset=-4+104)[0]
    type4['def_subcode3'] = struct.unpack_from('>Q', rawtype4, offset=-4+112)[0]
    type4['def_subcode4'] = struct.unpack_from('>Q', rawtype4, offset=-4+120)[0]
    type4['def_subcode5'] = struct.unpack_from('>Q', rawtype4, offset=-4+128)[0]
    type4['def_subcode6'] = struct.unpack_from('>Q', rawtype4, offset=-4+136)[0]
    type4['pn_code_length'] = struct.unpack_from('>I', rawtype4, offset=-4+144)[0]
    type4['transmit_inphs_time_year'] = struct.unpack_from('>H', rawtype4, offset=-4+148)[0]
    type4['transmit_inphs_time_doy'] = struct.unpack_from('>H', rawtype4, offset=-4+150)[0]
    type4['transmit_inphs_time_sec'] = struct.unpack_from('>d', rawtype4, offset=-4+152)[0]
    type4['carr_sup_rng_modul'] = struct.unpack_from('>f', rawtype4, offset=-4+160)[0]
    type4['rng_modul_amp'] = struct.unpack_from('>H', rawtype4, offset=-4+164)[0]
    type4['exc_scalar_num'] = struct.unpack_from('>I', rawtype4, offset=-4+166)[0]
    type4['exc_scalar_den'] = struct.unpack_from('>I', rawtype4, offset=-4+170)[0]
    type4['rng_cycle_time'] = struct.unpack_from('>d', rawtype4, offset=-4+174)[0]
    type4['clock_waveform'] = struct.unpack_from('>B', rawtype4, offset=-4+182)[0]
    type4['rng_meas_type'] = struct.unpack_from('>B', rawtype4, offset=-4+183)[0]
    type4['time_tag_corr_flag'] = struct.unpack_from('>B', rawtype4, offset=-4+184)[0]
    type4['type_time_corr_flag'] = struct.unpack_from('>B', rawtype4, offset=-4+185)[0]
    type4['fabricated_sfdu_flag'] = struct.unpack_from('>B', rawtype4, offset=-4+186)[0]
    type4['op_subcode6'] = struct.unpack_from('>B', rawtype4, offset=-4+187)[0]
    type4['ccsds_k'] = struct.unpack_from('>B', rawtype4, offset=-4+188)[0]
    type4['ccsds_l'] = struct.unpack_from('>B', rawtype4, offset=-4+189)[0]
    type4['ul_rng_modulo'] = struct.unpack_from('>I', rawtype4, offset=-4+190)[0]
    
    return type4

def read_type5(fin):
    
    rawtype5 = fin.read(258)
    type5 = {}
    # type5['chdo_type'] = struct.unpack_from('>H', rawtype5, offset=0)[0]
    # type5['chdo_length'] = struct.unpack_from('>H', rawtype5, offset=2)[0]
    type5['stn_cal'] = struct.unpack_from('>d', rawtype5, offset=-4+4)[0]
    type5['dl_stn_cal'] = struct.unpack_from('>d', rawtype5, offset=-4+12)[0]
    type5['dl_cal_freq'] = struct.unpack_from('>d', rawtype5, offset=-4+20)[0]
    type5['cal_std_dev'] = struct.unpack_from('>f', rawtype5, offset=-4+28)[0]
    type5['cal_pts'] = struct.unpack_from('>H', rawtype5, offset=-4+32)[0]
    type5['dl_rng_phs'] = struct.unpack_from('>d', rawtype5, offset=-4+34)[0]
    type5['figure_merit'] = struct.unpack_from('>f', rawtype5, offset=-4+42)[0]
    type5['rng_resid'] = struct.unpack_from('>d', rawtype5, offset=-4+46)[0]
    type5['drvid'] = struct.unpack_from('>d', rawtype5, offset=-4+54)[0]
    type5['rtlt'] = struct.unpack_from('>f', rawtype5, offset=-4+62)[0]
    type5['pcn0'] = struct.unpack_from('>f', rawtype5, offset=-4+66)[0]
    type5['pcn0_resid'] = struct.unpack_from('>f', rawtype5, offset=-4+70)[0]
    type5['pdn0'] = struct.unpack_from('>f', rawtype5, offset=-4+74)[0]
    type5['pdn0_resid'] = struct.unpack_from('>f', rawtype5, offset=-4+78)[0]
    type5['prn0'] = struct.unpack_from('>f', rawtype5, offset=-4+82)[0]
    type5['prn0_resid'] = struct.unpack_from('>f', rawtype5, offset=-4+86)[0]
    type5['system_noise_temp'] = struct.unpack_from('>f', rawtype5, offset=-4+90)[0]
    type5['state_subcode1'] = struct.unpack_from('>B', rawtype5, offset=-4+94)[0]
    type5['state_subcode2'] = struct.unpack_from('>B', rawtype5, offset=-4+95)[0]
    type5['state_subcode3'] = struct.unpack_from('>B', rawtype5, offset=-4+96)[0]
    type5['state_subcode4'] = struct.unpack_from('>B', rawtype5, offset=-4+97)[0]
    type5['state_subcode5'] = struct.unpack_from('>B', rawtype5, offset=-4+98)[0]
    type5['state_subcode6'] = struct.unpack_from('>B', rawtype5, offset=-4+99)[0]
    type5['pn_clk_phs'] = struct.unpack_from('>d', rawtype5, offset=-4+100)[0]
    type5['carr_loop_type'] = struct.unpack_from('>B', rawtype5, offset=-4+108)[0]
    type5['snt_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+109)[0]
    type5['carr_resid_wt'] = struct.unpack_from('>f', rawtype5, offset=-4+110)[0]
    type5['template_id'] = (b''.join(struct.unpack_from('cccccccccccccccccccc', rawtype5, offset=-4+114))).decode("utf-8")
    type5['invert'] = struct.unpack_from('>B', rawtype5, offset=-4+134)[0]
    type5['correl_type'] = struct.unpack_from('>B', rawtype5, offset=-4+135)[0]
    type5['int_time'] = struct.unpack_from('>I', rawtype5, offset=-4+136)[0]
    type5['clk_divider'] = struct.unpack_from('>B', rawtype5, offset=-4+140)[0]
    type5['len_subcode1'] = struct.unpack_from('>B', rawtype5, offset=-4+141)[0]
    type5['len_subcode2'] = struct.unpack_from('>B', rawtype5, offset=-4+142)[0]
    type5['len_subcode3'] = struct.unpack_from('>B', rawtype5, offset=-4+143)[0]
    type5['len_subcode4'] = struct.unpack_from('>B', rawtype5, offset=-4+144)[0]
    type5['len_subcode5'] = struct.unpack_from('>B', rawtype5, offset=-4+145)[0]
    type5['len_subcode6'] = struct.unpack_from('>B', rawtype5, offset=-4+146)[0]
    type5['op_subcode1'] = struct.unpack_from('>B', rawtype5, offset=-4+147)[0]
    type5['op_subcode2'] = struct.unpack_from('>B', rawtype5, offset=-4+148)[0]
    type5['op_subcode3'] = struct.unpack_from('>B', rawtype5, offset=-4+149)[0]
    type5['op_subcode4'] = struct.unpack_from('>B', rawtype5, offset=-4+150)[0]
    type5['op_subcode5'] = struct.unpack_from('>B', rawtype5, offset=-4+151)[0]
    type5['def_subcode1'] = struct.unpack_from('>Q', rawtype5, offset=-4+152)[0]
    type5['def_subcode2'] = struct.unpack_from('>Q', rawtype5, offset=-4+160)[0]
    type5['def_subcode3'] = struct.unpack_from('>Q', rawtype5, offset=-4+168)[0]
    type5['def_subcode4'] = struct.unpack_from('>Q', rawtype5, offset=-4+176)[0]
    type5['def_subcode5'] = struct.unpack_from('>Q', rawtype5, offset=-4+184)[0]
    type5['def_subcode6'] = struct.unpack_from('>Q', rawtype5, offset=-4+192)[0]
    type5['pn_code_length'] = struct.unpack_from('>I', rawtype5, offset=-4+200)[0]
    type5['rcv_inphs_time_year'] = struct.unpack_from('>H', rawtype5, offset=-4+204)[0]
    type5['rcv_inphs_time_doy'] = struct.unpack_from('>H', rawtype5, offset=-4+206)[0]
    type5['rcv_inphs_time_sec'] = struct.unpack_from('>d', rawtype5, offset=-4+208)[0]
    type5['exc_scalar_num'] = struct.unpack_from('>I', rawtype5, offset=-4+216)[0]
    type5['exc_scalar_den'] = struct.unpack_from('>I', rawtype5, offset=-4+220)[0]
    type5['rng_cycle_time'] = struct.unpack_from('>d', rawtype5, offset=-4+224)[0]
    type5['inphs_correl'] = struct.unpack_from('>f', rawtype5, offset=-4+232)[0]
    type5['quad_phs_correl'] = struct.unpack_from('>f', rawtype5, offset=-4+236)[0]
    type5['metrics_vld_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+240)[0]
    type5['correl_vld_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+241)[0]
    type5['rng_resid_tol_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+242)[0]
    type5['drvid_tol_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+243)[0]
    type5['prn0_resid_tol_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+244)[0]
    type5['rng_sigma_tol_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+245)[0]
    type5['rng_vld_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+246)[0]
    type5['rng_config_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+247)[0]
    type5['rng_hw_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+248)[0]
    type5['rng_meas_type'] = struct.unpack_from('>B', rawtype5, offset=-4+249)[0]
    type5['time_tag_corr_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+250)[0]
    type5['type_time_corr_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+251)[0]
    type5['dop_mode_corr_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+252)[0]
    type5['ul_stn_corr_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+253)[0]
    type5['stn_cal_corr_flag'] = struct.unpack_from('>B', rawtype5, offset=-4+254)[0]
    type5['op_subcode6'] = struct.unpack_from('>B', rawtype5, offset=-4+255)[0]
    type5['ccsds_k'] = struct.unpack_from('>B', rawtype5, offset=-4+256)[0]
    type5['ccsds_l'] = struct.unpack_from('>B', rawtype5, offset=-4+257)[0]
    type5['dl_rng_modulo'] = struct.unpack_from('>I', rawtype5, offset=-4+258)[0]
    
    return type5

# def read_type_ni(fin): #not implemented
#     rawtype_ni_head = fin.read(4)
#     type_ni = {}
#     type_ni['chdo_type'] = struct.unpack_from('>H', rawtype_ni_head, offset=0)[0]
#     type_ni['chdo_length'] = struct.unpack_from('>H', rawtype_ni_head, offset=2)[0]
    
#     rawtype_ni_body = fin.read(type_ni['chdo_length']) 
#     type_ni['rawbytes'] = rawtype_ni_body
         
#     return type_ni

def read_type17(fin):
    rawtype17 = fin.read(72)
    type17 = {}
    # type17['chdo_type'] = struct.unpack_from('>H', rawtype17, offset=0)[0]
    # type17['chdo_length'] = struct.unpack_from('>H', rawtype17, offset=2)[0]
    type17['ref_rcv_type'] = struct.unpack_from('>B', rawtype17, offset=-4+4)[0]
    type17['fabricated_ul_flag'] = struct.unpack_from('>B', rawtype17, offset=-4+5)[0]
    type17['total_cnt_phs_prefit_resid_tol_value'] = struct.unpack_from('>f', rawtype17, offset=-4+6)[0]
    type17['reserve2'] = struct.unpack_from('>H', rawtype17, offset=-4+10)[0]
    type17['dop_noise'] = struct.unpack_from('>f', rawtype17, offset=-4+12)[0]
    type17['delta_ff'] = struct.unpack_from('>d', rawtype17, offset=-4+16)[0]
    type17['rcv_sig_lvl'] = struct.unpack_from('>f', rawtype17, offset=-4+24)[0]
    type17['num_obs'] = struct.unpack_from('>H', rawtype17, offset=-4+28)[0]
    type17['obs_cnt_time'] = struct.unpack_from('>f', rawtype17, offset=-4+30)[0]
    type17['total_cnt_phs_st_year'] = struct.unpack_from('>H', rawtype17, offset=-4+34)[0]
    type17['total_cnt_phs_st_doy'] = struct.unpack_from('>H', rawtype17, offset=-4+36)[0]
    type17['total_cnt_phs_st_sec'] = struct.unpack_from('>d', rawtype17, offset=-4+38)[0]
    type17['total_cnt_phs_obs_hi'] = struct.unpack_from('>I', rawtype17, offset=-4+46)[0]
    type17['total_cnt_phs_obs_lo'] = struct.unpack_from('>I', rawtype17, offset=-4+50)[0]
    type17['total_cnt_phs_obs_frac'] = struct.unpack_from('>I', rawtype17, offset=-4+54)[0]
    type17['total_cnt_phs_prefit_resid'] = struct.unpack_from('>f', rawtype17, offset=-4+58)[0]
    type17['total_cnt_phs_prefit_resid_vld_flag'] = struct.unpack_from('>B', rawtype17, offset=-4+62)[0]
    type17['total_cnt_phs_prefit_resid_tol_flag'] = struct.unpack_from('>B', rawtype17, offset=-4+63)[0]
    type17['carr_resid_wt'] = struct.unpack_from('>f', rawtype17, offset=-4+64)[0]
    type17['reserve8'] = struct.unpack_from('>Q', rawtype17, offset=-4+68)[0]

    return type17

def read_type11(fin):
    rawtype11 = fin.read(38)
    type11 = {}
    # type11['chdo_type'] = struct.unpack_from('>H', rawtype11, offset=0)[0]
    # type11['chdo_length'] = struct.unpack_from('>H', rawtype11, offset=2)[0]
    type11['drvid_type'] = struct.unpack_from('>B', rawtype11, offset=-4+4)[0]
    type11['drvid_pts'] = struct.unpack_from('>B', rawtype11, offset=-4+5)[0]
    type11['drvid'] = struct.unpack_from('>d', rawtype11, offset=-4+6)[0]
    type11['prn0'] = struct.unpack_from('>f', rawtype11, offset=-4+14)[0]
    type11['drvid_noise'] = struct.unpack_from('>f', rawtype11, offset=-4+18)[0]
    type11['drvid_tol_value'] = struct.unpack_from('>f', rawtype11, offset=-4+22)[0]
    type11['prn0_resid_tol_value'] = struct.unpack_from('>f', rawtype11, offset=-4+26)[0]
    type11['reserve1'] = struct.unpack_from('>B', rawtype11, offset=-4+30)[0]
    type11['drvid_tol_flag'] = struct.unpack_from('>B', rawtype11, offset=-4+31)[0]
    type11['prn0_resid_tol_flag'] = struct.unpack_from('>B', rawtype11, offset=-4+32)[0]
    type11['drvid_noise_pts'] = struct.unpack_from('>B', rawtype11, offset=-4+33)[0]
    type11['reserve8'] = struct.unpack_from('>Q', rawtype11, offset=-4+34)[0]

    return type11

def read_type14(fin):
    
    rawtype14 = fin.read(204)
    type14 = {}
    # type14['chdo_type'] = struct.unpack_from('>H', rawtype14, offset=0)[0]
    # type14['chdo_length'] = struct.unpack_from('>H', rawtype14, offset=2)[0]
    type14['ul_stn_cal'] = struct.unpack_from('>d', rawtype14, offset=-4+4)[0]
    type14['dl_stn_cal'] = struct.unpack_from('>d', rawtype14, offset=-4+12)[0]
    type14['meas_rng'] = struct.unpack_from('>d', rawtype14, offset=-4+20)[0]
    type14['rng_obs_dl'] = struct.unpack_from('>d', rawtype14, offset=-4+28)[0]
    type14['figure_merit'] = struct.unpack_from('>f', rawtype14, offset=-4+36)[0]
    type14['drvid'] = struct.unpack_from('>d', rawtype14, offset=-4+40)[0]
    type14['rtlt'] = struct.unpack_from('>f', rawtype14, offset=-4+48)[0]
    type14['pcn0'] = struct.unpack_from('>f', rawtype14, offset=-4+52)[0]
    type14['transmit_pwr'] = struct.unpack_from('>f', rawtype14, offset=-4+56)[0]
    type14['invert'] = struct.unpack_from('>B', rawtype14, offset=-4+60)[0]
    type14['correl_type'] = struct.unpack_from('>B', rawtype14, offset=-4+61)[0]
    type14['clk_divider'] = struct.unpack_from('>B', rawtype14, offset=-4+62)[0]
    type14['len_subcode1'] = struct.unpack_from('>B', rawtype14, offset=-4+63)[0]
    type14['len_subcode2'] = struct.unpack_from('>B', rawtype14, offset=-4+64)[0]
    type14['len_subcode3'] = struct.unpack_from('>B', rawtype14, offset=-4+65)[0]
    type14['len_subcode4'] = struct.unpack_from('>B', rawtype14, offset=-4+66)[0]
    type14['len_subcode5'] = struct.unpack_from('>B', rawtype14, offset=-4+67)[0]
    type14['len_subcode6'] = struct.unpack_from('>B', rawtype14, offset=-4+68)[0]
    type14['op_subcode1'] = struct.unpack_from('>B', rawtype14, offset=-4+69)[0]
    type14['op_subcode2'] = struct.unpack_from('>B', rawtype14, offset=-4+70)[0]
    type14['op_subcode3'] = struct.unpack_from('>B', rawtype14, offset=-4+71)[0]
    type14['op_subcode4'] = struct.unpack_from('>B', rawtype14, offset=-4+72)[0]
    type14['op_subcode5'] = struct.unpack_from('>B', rawtype14, offset=-4+73)[0]
    type14['def_subcode1'] = struct.unpack_from('>Q', rawtype14, offset=-4+74)[0]
    type14['def_subcode2'] = struct.unpack_from('>Q', rawtype14, offset=-4+82)[0]
    type14['def_subcode3'] = struct.unpack_from('>Q', rawtype14, offset=-4+90)[0]
    type14['def_subcode4'] = struct.unpack_from('>Q', rawtype14, offset=-4+98)[0]
    type14['def_subcode5'] = struct.unpack_from('>Q', rawtype14, offset=-4+106)[0]
    type14['def_subcode6'] = struct.unpack_from('>Q', rawtype14, offset=-4+114)[0]
    type14['pn_code_length'] = struct.unpack_from('>I', rawtype14, offset=-4+122)[0]
    type14['transmit_inphs_time'] = struct.unpack_from('>f', rawtype14, offset=-4+126)[0]
    type14['rcv_inphs_time'] = struct.unpack_from('>f', rawtype14, offset=-4+130)[0]
    type14['carr_sup_rng_modul'] = struct.unpack_from('>f', rawtype14, offset=-4+134)[0]
    type14['exc_scalar_num'] = struct.unpack_from('>I', rawtype14, offset=-4+138)[0]
    type14['exc_scalar_den'] = struct.unpack_from('>I', rawtype14, offset=-4+142)[0]
    type14['rng_cycle_time'] = struct.unpack_from('>d', rawtype14, offset=-4+146)[0]
    type14['rng_modulo'] = struct.unpack_from('>I', rawtype14, offset=-4+154)[0]
    type14['rng_type'] = struct.unpack_from('>B', rawtype14, offset=-4+158)[0]
    type14['fabricated_ul_flag'] = struct.unpack_from('>B', rawtype14, offset=-4+159)[0]
    type14['rng_noise'] = struct.unpack_from('>f', rawtype14, offset=-4+160)[0]
    type14['rng_obs'] = struct.unpack_from('>d', rawtype14, offset=-4+164)[0]
    type14['rng_dl_prefit_resid_vld_flag'] = struct.unpack_from('>B', rawtype14, offset=-4+172)[0]
    type14['clock_waveform'] = struct.unpack_from('>B', rawtype14, offset=-4+173)[0]
    type14['rng_resid_tol_value'] = struct.unpack_from('>f', rawtype14, offset=-4+174)[0]
    type14['drvid_tol_value'] = struct.unpack_from('>f', rawtype14, offset=-4+178)[0]
    type14['prn0_resid_tol_value'] = struct.unpack_from('>f', rawtype14, offset=-4+182)[0]
    type14['rng_resid_tol_value'] = struct.unpack_from('>f', rawtype14, offset=-4+186)[0]
    type14['fom_tol_value'] = struct.unpack_from('>f', rawtype14, offset=-4+190)[0]
    type14['rng_resid_tol_flag'] = struct.unpack_from('>B', rawtype14, offset=-4+194)[0]
    type14['drvid_tol_flag'] = struct.unpack_from('>B', rawtype14, offset=-4+195)[0]
    type14['prn0_resid_tol_flag'] = struct.unpack_from('>B', rawtype14, offset=-4+196)[0]
    type14['rng_sigma_tol_flag'] = struct.unpack_from('>B', rawtype14, offset=-4+197)[0]
    type14['rng_vld_flag'] = struct.unpack_from('>B', rawtype14, offset=-4+198)[0]
    type14['rng_config_flag'] = struct.unpack_from('>B', rawtype14, offset=-4+199)[0]
    type14['stn_cal_corr_flag'] = struct.unpack_from('>B', rawtype14, offset=-4+200)[0]
    type14['op_subcode6'] = struct.unpack_from('>B', rawtype14, offset=-4+201)[0]
    type14['ccsds_k'] = struct.unpack_from('>B', rawtype14, offset=-4+202)[0]
    type14['ccsds_l'] = struct.unpack_from('>B', rawtype14, offset=-4+203)[0]
    type14['rng_dl_prefit_resid'] = struct.unpack_from('>f', rawtype14, offset=-4+204)[0]
    
    return type14


def read_type16(fin):
    rawtype16 = fin.read(56)
    type16 = {}
    # type16['chdo_type'] = struct.unpack_from('>H', rawtype16, offset=0)[0]
    # type16['chdo_length'] = struct.unpack_from('>H', rawtype16, offset=2)[0]
    type16['ref_rcv_type'] = struct.unpack_from('>B', rawtype16, offset=-4+4)[0]
    type16['fabricated_ul_flag'] = struct.unpack_from('>B', rawtype16, offset=-4+5)[0]
    type16['carr_prefit_resid_tol_value'] = struct.unpack_from('>f', rawtype16, offset=-4+6)[0]
    type16['reserve2'] = struct.unpack_from('>H', rawtype16, offset=-4+10)[0]
    type16['dop_noise'] = struct.unpack_from('>f', rawtype16, offset=-4+12)[0]
    type16['delta_ff'] = struct.unpack_from('>d', rawtype16, offset=-4+16)[0]
    type16['rcv_sig_lvl'] = struct.unpack_from('>f', rawtype16, offset=-4+24)[0]
    type16['num_obs'] = struct.unpack_from('>H', rawtype16, offset=-4+28)[0]
    type16['obs_cnt_time'] = struct.unpack_from('>f', rawtype16, offset=-4+30)[0]
    type16['rcv_carr_obs'] = struct.unpack_from('>d', rawtype16, offset=-4+34)[0]
    
    type16['carr_prefit_resid'] = struct.unpack_from('>f', rawtype16, offset=-4+42)[0]
    type16['carr_prefit_resid_vld_flag'] = struct.unpack_from('>B', rawtype16, offset=-4+46)[0]
    type16['carr_prefit_resid_tol_flag'] = struct.unpack_from('>B', rawtype16, offset=-4+47)[0]
    type16['carr_resid_wt'] = struct.unpack_from('>f', rawtype16, offset=-4+48)[0]
    type16['reserve8'] = struct.unpack_from('>Q', rawtype16, offset=-4+52)[0]

    return type16

# def read_chdo2(fin, num_bytes):
#     rawchdo2 = fin.read(4)
#     # rawchdo2 = fin.read(num_bytes)
#     chdo2 = {}
#     chdo2['chdo_type'] = struct.unpack_from('>H', rawchdo2, offset=0)[0]
#     chdo2['chdo_length'] = struct.unpack_from('>H', rawchdo2, offset=2)[0]
#     # rawchdo_body = fin.read(chdo2['chdo_length']) 
#     # chdo2['rawbytes'] = rawchdo2[4:]#rawchdo_body
#     # print(num_bytes)
#     # print(int(chdo2['chdo_length']))
#     chdo2['rawbytes'] = read_bytes(fin, chdo2['chdo_length'])
#     chdo2['extra'] = read_bytes(fin, num_bytes - chdo2['chdo_length'] - 4)
   
#     return chdo2

def read_data_type(fin, format_code):
    type_head = fin.read(4)
    
    chdo_type = struct.unpack_from('>H', type_head, offset=0)[0]
    chdo_length = struct.unpack_from('>H', type_head, offset=2)[0]
    
    if format_code == 0:
        type_data = read_type0(fin)
    elif format_code == 1:
        type_data = read_type1(fin)
    # elif format_code == 2:
        # type_data = read_type2(fin)
    elif format_code == 4:
        type_data = read_type4(fin)
    elif format_code == 5:
        type_data = read_type5(fin)
    elif format_code == 7:
        type_data = read_type7(fin)
    elif format_code == 9:
        type_data = read_type9(fin)
    elif format_code == 11:
        type_data = read_type11(fin)
    elif format_code == 14:
        type_data = read_type14(fin)
    elif format_code == 16:
        type_data = read_type16(fin)
    elif format_code == 17:
        type_data = read_type17(fin)
    else:
        type_data = {}
        type_data['rawbytes'] = read_bytes(fin, chdo_length)
   
    type_data['chdo_type'] = chdo_type
    type_data['chdo_length'] = chdo_length 
   
    return type_data

# def read_chdo2_new(fin, num_bytes):
#     rawchdo2_head = fin.read(4)
#     chdo2 = {}
#     chdo2['chdo_type'] = struct.unpack_from('>H', rawchdo2_head, offset=0)[0]
#     chdo2['chdo_length'] = struct.unpack_from('>H', rawchdo2_head, offset=2)[0]
    
#     if chdo2['chdo_type'] == 133:
#         rawchdo_body = fin.read(int(chdo2['chdo_length'])) 
    
    
#     chdo2['rawbytes'] = rawchdo2[4:]#rawchdo_body
    
#     rawchdo2 = fin.read(num_bytes)
   
#     return chdo2

def header_read(fin, bad_bytes=667):
    
    
    # hread = fin.read(667)
    # hread = fin.read(160500)
    hread = fin.read(bad_bytes)
    
    return hread
    
def read_bytes(fin, num_bytes):    
    bytes_read = fin.read(num_bytes)
    
    return bytes_read

# """
#     Collect All TRK-2-34 Frames into one array and output the rest into a separate binary
# """
# def read_all_trk234_frames_v2(filein, fileout):
#     trk234_data = []
#     bytesread = 0
#     num = 0
    
#     index = get_trk234_index(filein)
    
#     with open(filein, "rb") as fin:
        
#         with open(fileout, "wb") as fout:
        
#             fout.write(header_read(fin, index[num]))
#             bytesread = bytesread + index[num]
#             # num = num + 1
        
#             try:
#                 sfdu = read_sfdu(fin)
#                 bytesread = bytesread + 20
                
#             except:
#                 sfdu = {}
#                 sfdu['control_auth_id']=0
            
#             while (sfdu['control_auth_id']=='NJPL'):
#                 this_frame = {}
#                 this_frame['sfdu'] = sfdu.copy()
                
#                 aggregation = read_aggregation_chdo_label(fin)
#                 this_frame['aggregation'] = aggregation
                
#                 bytesread = bytesread + 4
                
#                 chdo = read_primary_chdo(fin)
#                 this_frame['chdo'] = chdo
                
#                 bytesread = bytesread + 8
                
#                 type_sizes = {
#                     0 : 162,
#                     1 : 358,
#                     2 : 194,
#                     3 : 304,
#                     4 : 276,
#                     5 : 388,
#                     6 : 200,
#                     7 : 330,
#                     8 : 178,
#                     9 : 124,
#                     10 : 204,
#                     11 : 182,
#                     12 : 164,
#                     13 : 160,
#                     14 : 348,
#                     15 : 194,
#                     16 : 200,
#                     17 : 216,
#                     }
                
                
#                 if type_sizes[chdo['format_code']] == this_frame['sfdu']['sfdu_length']:
#                     format_code = chdo['format_code']
#                 else:
#                     format_code = 999
                
#                 if format_code==9:
#                     chdo132 = read_chdo132(fin)
#                     this_frame['chdo132'] = chdo132
                    
#                     bytesread = bytesread + 70
                    
#                     type9 = read_type9(fin)
#                     this_frame['type9'] = type9
                    
#                     bytesread = bytesread + 42
                    
#                 elif format_code==7:
#                     chdo134 = read_chdo134(fin)
#                     this_frame['chdo134'] = chdo134
                    
#                     bytesread = bytesread + 128
                    
#                     type7 = read_type7(fin)
#                     this_frame['type7'] = type7
                    
#                     bytesread = bytesread + 190
    
#                 elif format_code==17:
#                     chdo134 = read_chdo134(fin)
#                     this_frame['chdo134'] = chdo134
                    
#                     bytesread = bytesread + 128
                    
#                     type17 = read_type17(fin)
#                     this_frame['type17'] = type17
                    
#                     bytesread = bytesread + 76


#                 elif format_code==0:
#                     chdo132 = read_chdo132(fin)
#                     this_frame['chdo132'] = chdo132
                    
#                     bytesread = bytesread + 70
                    
#                     type0 = read_type0(fin)
#                     this_frame['type0'] = type0
                    
#                     bytesread = bytesread + 80
                    
#                 elif format_code==1:
#                     chdo133 = read_chdo133(fin)
#                     this_frame['chdo133'] = chdo133
                    
#                     bytesread = bytesread + 114
                    
#                     type1 = read_type1(fin)
#                     this_frame['type1'] = type1
                    
#                     bytesread = bytesread + 232
                    
#                 elif format_code==4:
#                     chdo132 = read_chdo132(fin)
#                     this_frame['chdo132'] = chdo132
                    
#                     bytesread = bytesread + 70
                    
#                     type4 = read_type4(fin)
#                     this_frame['type4'] = type4
                    
#                     bytesread = bytesread + 194
                    
#                 elif format_code==5:
#                     chdo133 = read_chdo133(fin)
#                     this_frame['chdo133'] = chdo133
                    
#                     bytesread = bytesread + 114
                    
#                     type5 = read_type5(fin)
#                     this_frame['type5'] = type5
                    
#                     bytesread = bytesread + 262
                
#                 elif format_code==11:
#                     chdo134 = read_chdo134(fin)
#                     this_frame['chdo134'] = chdo134
                    
#                     bytesread = bytesread + 128
                    
#                     type11 = read_type11(fin)
#                     this_frame['type11'] = type11
                
#                     bytesread = bytesread + 42
                    
#                 elif format_code==14:
#                     chdo134 = read_chdo134(fin)
#                     this_frame['chdo134'] = chdo134
                    
#                     bytesread = bytesread + 128
                    
#                     type14 = read_type14(fin)
#                     this_frame['type14'] = type14
                
#                     bytesread = bytesread + 208
                    
#                 elif format_code==16:
#                     chdo134 = read_chdo134(fin)
#                     this_frame['chdo134'] = chdo134
                    
#                     bytesread = bytesread + 128
                    
#                     type16 = read_type16(fin)
#                     this_frame['type16'] = type16
                
#                     bytesread = bytesread + 60
                
#                 else:
#                     #data not used
#                     # chdo2 = read_bytes(fin, aggregation['chdo_length'] - 8) #read secondary CHDO
#                     # chdo2 = read_chdo2(fin) #read secondary CHDO
#                     chdo2 = read_chdo2(fin, aggregation['chdo_length'] - 8) #read secondary CHDO
#                     this_frame['chdo2'] = chdo2
                    
#                     bytesread = bytesread + aggregation['chdo_length'] - 8
                    
#                     type_ni = read_type_ni(fin) #read data type bytes
#                     this_frame['type_ni'] = type_ni
                    
#                     bytesread = bytesread + 4
    
#                 trk234_data.append(this_frame)
                
#                 print(bytesread)
#                 print(index[num])
                
#                 if bytesread > index[num]:
#                     num = num + 1
                    
#                 print(bytesread)
#                 print(index[num])
    
#                 try:
#                     if bytesread < index[num]:
#                         fout.write(header_read(fin, index[num] - bytesread))
#                         bytesread = index[num]
#                         num = num + 1
                        
#                         print("saved junk")
                        
#                     sfdu = read_sfdu(fin)
#                     bytesread = bytesread + 20
#                     print(sfdu['control_auth_id'])
#                     print(sfdu['control_auth_id'] == "NJPL")
#                 except:
#                     sfdu['control_auth_id']=0

    
#     return trk234_data

def get_trk234_index(file_in, pattern='NJPL'):
    
    index = []
    
    with open(filein, "rb") as fin:
        
        bytedata = fin.read()
            
        for i in range(len(bytedata)):
        
            try:
            
                id = (b''.join(struct.unpack_from('cccc', bytedata, offset=i))).decode("utf-8")
                
            except:
                
                id = "junk"
                
            if id == pattern:
                
                index.append(i)
                
        return index

"""
    Collect All TRK-2-34 Frames into one array
"""
def read_all_trk234_frames(filein, hflag=False):
    trk234_data = []
    
    dict_chdo_name = {
                    132 : "chdo132",
                    133 : "chdo133",
                    134 : "chdo134"
                    }
    
    dict_data_type = {
                    0 : "type0",
                    1 : "type1",
                    2 : "type2",
                    3 : "type3",
                    4 : "type4",
                    5: "type5",
                    7 : "type7",
                    9 : "type9",
                    11 : "type11",
                    14 : "type14",
                    16 : "type16",
                    17 : "type17",
                    999 : "type_ni"
                    }
    
    type_sizes = {
                    0 : 162,
                    1 : 358,
                    2 : 194,
                    3 : 304,
                    4 : 276,
                    5 : 388,
                    6 : 200,
                    7 : 330,
                    8 : 178,
                    9 : 124,
                    10 : 204,
                    11 : 182,
                    12 : 164,
                    13 : 160,
                    14 : 348,
                    15 : 194,
                    16 : 200,
                    17 : 216,
                    }
    
    with open(filein, "rb") as fin:
        
        try:
            if hflag: header_read(fin)
            sfdu = read_sfdu(fin)
        except:
            sfdu = {}
            sfdu['control_auth_id']=0
        
        while (sfdu['control_auth_id']=='NJPL'):
            this_frame = {}
            this_frame['sfdu'] = sfdu.copy()
            
            aggregation = read_aggregation_chdo_label(fin)
            this_frame['aggregation'] = aggregation
            
            chdo = read_primary_chdo(fin)
            this_frame['chdo'] = chdo
                
            if type_sizes[chdo['format_code']] == this_frame['sfdu']['sfdu_length']:
                format_code = chdo['format_code']
            else:
                format_code = 999
            
            
            chdo2 = read_secondary_chdo(fin) #read secondary CHDO
                
            try:
                chdo_name = dict_chdo_name[chdo2["chdo_type"]]
            except:
                chdo_name = "chdo2"
                    
            this_frame[chdo_name] = chdo2
            
            if format_code==999:
                excess = aggregation['chdo_length'] - 8 - chdo2['chdo_length'] - 4
                if excess:
                    if excess > 3:
                        raw_exc = fin.read(4)
                        exc = {}
                        exc['excess_type'] = struct.unpack_from('>H', raw_exc, offset=0)[0]
                        exc['excess_length'] = struct.unpack_from('>H', raw_exc, offset=2)[0]
                        if excess == 76:
                            # print("I'm here")
                            this_frame['excess'] = read_type17(fin)
                            this_frame['excess']['type'] = exc['excess_type']
                            this_frame['excess']['length'] = exc['excess_length']
                        else:
                            exc['rawbytes'] = read_bytes(fin, excess-4)
                        # print(excess-4)
                            this_frame['excess'] = exc
                    else:
                        this_frame['excess'] = read_bytes(fin, excess)
            
            
            data_type = read_data_type(fin, format_code)
            

            data_type_name = dict_data_type[format_code]
            this_frame[data_type_name] = data_type
            
            trk234_data.append(this_frame)

            try:
                sfdu = read_sfdu(fin)
            except:
                sfdu['control_auth_id']=0
    
    return trk234_data
       
class leap_second_file:
    def __init__(self, path = None, default_data = None):
        self.path = path
        self.leapSecs = []
        self.default_data = default_data
        if path:
            self.read_in_file()
        else:
            self.read_default_leap_data()
        
    def read_in_file(self):
        try:
            f = open(self.path, "r")
            for x in f:
                dat = x[1:-1].split()
                #JD to MJD
                mjd = float(dat[4]) - 2400000.5
                strpdat = dat[0] + " " + dat[1] + " " +  dat[2]
                epoch = datetime.strptime(strpdat, "%Y %b %d")
                secs = float(dat[6])
                self.leapSecs.append({"epoch":epoch, "mjd":mjd, "leapSecs":secs})
        except:
            raise Exception("ERROR: Failed to load leap second data, please specify a valid leap second file.")
    
    def read_default_leap_data(self):
        try:
            f = self.default_data
            for x in f.split("\n"):
                dat = x[:-1].split()
                #JD to MJD
                mjd = float(dat[4]) - 2400000.5
                strpdat = dat[0] + " " + dat[1] + " " +  dat[2]
                epoch = datetime.strptime(strpdat, "%Y %b %d")
                secs = float(dat[6])
                self.leapSecs.append({"epoch":epoch, "mjd":mjd, "leapSecs":secs})
        except:
            raise Exception("ERROR: Failed to parse default leap second data, please specify a valid leap second file.")
            
    def getLeapSecondDelta(self, epoch):
        try:
            for i in reversed(range(len(self.leapSecs))):
                if epoch > self.leapSecs[i]["epoch"]:
                    return timedelta(seconds=self.leapSecs[i]["leapSecs"])
        except:
            print("WARNING: Failed to load leap second data, please specify a valid leap second file. No leap second data will be used for this run")
            self.leapSecs = []
        return datetime.timedelta(seconds=0)
        
def convert_time_to_modjulian(date, milliseconds):
        delta = (date - TAIMJD_base)
        diffDays = delta.days
        diffHours = abs(date.hour - TAIMJD_base.hour)
        diffMins = abs(date.minute - TAIMJD_base.minute)
        diffSecs = abs(date.second - TAIMJD_base.second)
        fractDiff = 0.5 + (diffHours / 24.0) + (diffMins / 1440.0) + ((diffSecs + milliseconds) / 86400.0)
        
        return ((date - TAIMJD_base).total_seconds() + milliseconds) / 86400.0


def convert_time(year, dayOfYear, secondsOfDay, leapFile):
    milliseconds = secondsOfDay - math.floor(secondsOfDay)
    hours = math.floor(secondsOfDay/3600.0)
    minutes = math.floor((secondsOfDay - hours*3600)/60.0)
    seconds = secondsOfDay - hours*3600.0 - minutes*60.0
            
    timeStr = str(year) + "-" + str(dayOfYear) + "-" + str(hours) + "-" + str(minutes) + "-" + str(int(seconds))
            
    dnum   = datetime.strptime(timeStr, "%Y-%j-%H-%M-%S")
    dnum   = dnum + leapFile.getLeapSecondDelta(dnum)
    return convert_time_to_modjulian(dnum, milliseconds)


def write_gmd(data, outfile, leapFile, addOptions = False):
    fout = open(outfile, 'w')
    last_mj = -1
    last_phaseHi = 0
    last_phaseLo = 0
    last_phaseFrac = 0
    old_dl_data_17 = {}
    old_dl_data_14 = {}
    
    
    str1 = "DSN_TCP"
    str2 = "DSN_SeqRange"
    str3 = "DSN_PNRange"
    
    rampData = {}
    
    #loop through all observations
    for obs in data:
        if obs["chdo"]["format_code"] == 9:
            ramp_freq = obs["type9"]["ramp_freq"]
            ramp_rate = obs["type9"]["ramp_rate"]
            rmp_mj = convert_time(obs["chdo132"]["year"], obs["chdo132"]["doy"], obs["chdo132"]["sec"], leapFile)
            sta_id      = obs["chdo132"]["ul_dss_id"]
            sc_id       = obs["chdo132"]["scft_id"]
            
            rampData[sta_id+sc_id] = [rmp_mj, ramp_rate, ramp_freq]
        if obs["chdo"]["format_code"] == 17:
            #Get measurement time
            mj = convert_time(obs["chdo134"]["year"], obs["chdo134"]["doy"], obs["chdo134"]["sec"], leapFile)
            
            #Get ids and path order
            sta_id      = obs["chdo134"]["ul_prdx_stn"]
            sc_id       = obs["chdo134"]["scft_id"]
            st_dl_id   = obs["chdo134"]["dl_dss_id"]
            
                
            #uplink band
            ul_band = obs["chdo134"]["ul_band"]
        
        
            phaseHi = obs["type17"]["total_cnt_phs_obs_hi"]
            phaseLo = obs["type17"]["total_cnt_phs_obs_lo"]
            phaseFrac = obs["type17"]["total_cnt_phs_obs_frac"]
            
            if st_dl_id in old_dl_data_17:
                last_mj = old_dl_data_17[st_dl_id][-1][0]
                last_phaseHi = old_dl_data_17[st_dl_id][-1][1]
                last_phaseLo = old_dl_data_17[st_dl_id][-1][2]
                last_phaseFrac = old_dl_data_17[st_dl_id][-1][3]
                old_dl_data_17[st_dl_id].append([mj,phaseHi,phaseLo,phaseFrac])
            else:
                last_mj = -1
                last_phaseHi = 0
                last_phaseLo = 0
                last_phaseFrac = 0
                old_dl_data_17[st_dl_id] = [[mj,phaseHi,phaseLo,phaseFrac]]
            
            dt = (mj-last_mj)*86400
            tol = 1e-6
            obs_cnt_time = obs["type17"]["obs_cnt_time"]
            
            #dt needs to match observation count time
            if abs(dt - obs_cnt_time) < tol and sta_id != 0:
                
                dt = obs_cnt_time
                
                ddoppler1 = (phaseHi-last_phaseHi)*2.0**32
                ddoppler2 = phaseLo-last_phaseLo;
                ddoppler3 = (phaseFrac-last_phaseFrac)/2.0**32
            
                ddopplerWhole = ddoppler1+ddoppler2
                ddopplerFrac = ddoppler3
                
                d1 = ddopplerWhole/dt
                d2 = ddopplerFrac/dt
                
                dopplerMeas_Hz = -(d1+d2)
                
                epoch = "{:.22f}".format(mj)
                dMeas = "{:.16e}".format(abs(dopplerMeas_Hz))
                sign = " "
                if dopplerMeas_Hz < 0:
                    sign = "-"
                #write the obs line:
                fout.write(f"{epoch}    {str1}    9006    {{ {sta_id}    {sc_id}    {st_dl_id} }}    {ul_band}    {dt}    {sign}{dMeas}\n")  
        
        
        #PN Range
        elif obs["chdo"]["format_code"] == 14:
            #Get measurement time
            mj = convert_time(obs["chdo134"]["year"], obs["chdo134"]["doy"], obs["chdo134"]["sec"], leapFile)
            
            #Get ids and path order
            sta_id      = obs["chdo134"]["ul_prdx_stn"]
            sc_id       = obs["chdo134"]["scft_id"]
            st_dl_id    = obs["chdo134"]["dl_dss_id"]
            
            #uplink band
            ul_band = obs["chdo134"]["ul_band"]
            
            freq = 0
            rampId = sta_id + sc_id
            if rampId in rampData:
                #rmp_freq + (rmp_rate*((mj-rmp_mj)*86400))
                freq = rampData[rampId][2] + (rampData[rampId][1]*((mj-rampData[rampId][0])*86400))
            else:
                #invalid measurement
                meas_rng = -1
                continue
        
            if sta_id != 0:
                
                meas_rng = obs["type14"]["meas_rng"]
                if meas_rng <= -1:
                    #invalid measurement
                    continue
                
                
                # convert to/from sec/RU based on band
                vld_dl_band = obs["chdo134"]["vld_dl_band"]
                if (vld_dl_band == 1):    
                   #S-band
                   RU_to_sec = 0.5 
                 
                elif (vld_dl_band == 2):
                   # X-band 
                   RU_to_sec = 221.0/1498.0 
                
                #elif (vld_dl_band == 3):
                   # C-band 
                   #RU_to_sec = (1024*79*228)/(109440*681)                
                else:
                   #invalid range for invalid frequency band 
                   meas_rng = -1
                   continue
  
                #calculate zHeight
                zHeight = 0
                ul_zheight_corr = obs["chdo134"]["ul_zheight_corr"]
                dl_zheight_corr = obs["chdo134"]["dl_zheight_corr"]
                
                if (ul_zheight_corr == 0 or ul_zheight_corr == -1):
                    meas_rng = -1
                    continue
                if (dl_zheight_corr == 0 or dl_zheight_corr == -1):
                    meas_rng = -1
                    continue
                 
                #convert to RU
                ul_zheight_RU = ul_zheight_corr * freq * RU_to_sec
                dl_zheight_RU = dl_zheight_corr * freq * RU_to_sec
                
                #calculate station calibration
                ul_sta_cal = obs["type14"]["ul_stn_cal"]
                dl_sta_cal = obs["type14"]["dl_stn_cal"] 
                
                if ul_sta_cal == 0 or ul_sta_cal == -1:
                    meas_rng = -1
                    continue
                if dl_sta_cal == 0 or dl_sta_cal == -1:
                    meas_rng = -1
                    continue
                    
                #Convert to 
                ul_sta_secs =  ul_sta_cal / (freq * RU_to_sec)
                dl_sta_secs =  dl_sta_cal / (freq * RU_to_sec)
                
                
                transmit_delay = 0.0
                receive_delay  = 0.0
                
                #final range observation
                rangeObs = 0
                if(addOptions):
                    rangeObs = meas_rng
                    transmit_delay =  ul_sta_secs + ul_zheight_corr
                    receive_delay  =  dl_sta_secs + dl_zheight_corr
                else:
                    rangeObs = meas_rng - (ul_zheight_RU + dl_zheight_RU + ul_sta_cal + dl_sta_cal)
                    transmit_delay = 0.0
                    receive_delay  = 0.0
                
                
                #Format numbers to strings
                epoch = "{:.22f}".format(mj)
                
                rangeModulo = obs["type14"]["rng_modulo"]
                
                #rangeObs
                rO = "{:.16e}".format(rangeObs)
                #rangeModulo
                rM = "{:.16e}".format(rangeModulo)
                #freq
                f  = "{:.16e}".format(freq)
                #transmit_delay
                tD = "{:.16e}".format(transmit_delay)
                #receive_delay
                rD = "{:.16e}".format(receive_delay)
                
                #print line
                if meas_rng > -1:
                    fout.write(f"{epoch}    {str3}    9033    {{ {sta_id}    {sc_id}    {st_dl_id} }}    {rO}    {tD}    {rD}    {ul_band}    {vld_dl_band}    {f}    {rM}\n")    



def write_rmp(data, outfile, leapFile):
    fout = open(outfile, 'w')
    
    #loop through all observations
    for obs in data:
        if obs["chdo"]["format_code"] == 9:
            #Get measurement time
            mj = convert_time(obs["chdo132"]["year"], obs["chdo132"]["doy"], obs["chdo132"]["sec"], leapFile)
            
            #Get ids and path order
            sta_id      = obs["chdo132"]["ul_dss_id"]
            sc_id       = obs["chdo132"]["scft_id"]
            
            ramp_freq = obs["type9"]["ramp_freq"]
            ramp_rate = obs["type9"]["ramp_rate"]
            ramp_type = obs["type9"]["ramp_type"]
            #uplink band
            ul_band = obs["chdo132"]["ul_band"]
            
            sign1 = " "
            if ramp_freq < 0:
                sign1 = "-"
            
            sign2 = " "
            if ramp_rate < 0:
                sign2 = "-"   
            
            ramp_freq = "{:.12e}".format(abs(ramp_freq))
            ramp_rate = "{:.12e}".format(abs(ramp_rate))
            
            epoch = "{:.15f}".format(mj)
            #write the obs line:
            fout.write(f"{epoch}  {sta_id} {sc_id} {ul_band} {ramp_type} {sign1}{ramp_freq} {sign2}{ramp_rate}\n")

def main():

    parser = argparse.ArgumentParser(description='trk234 to GMD Converter')
    parser.add_argument('-i', '--input', required = True, type=Path, metavar='IN_PATH', dest='in_path', help='Input file')
    parser.add_argument('-o', '--output',  type=Path, metavar='OUT_PATH', dest='out_path', help='Output path without file extension')
    parser.add_argument('-l', '--LeapSecondFile',  type=Path, metavar='LEAP_PATH', dest='leap_path', help='Leap second file path')
    parser.add_argument('-add_corrections_to_obs', '--add_corrections_to_obs',  action=argparse.BooleanOptionalAction, metavar='ADD_OPTS', dest='add_opts', help='Add options to ranging. See users guide for more details')
    
    args = parser.parse_args()
    
    print()
    print("Beginning run...")
    print()
    if not args.in_path:
        print("ERROR: must specify input file path with --input tag")
        return
    
    leapSecPath = args.leap_path.__str__()
    
    if leapSecPath == "None":
        leapSecPath = None
        print("WARNING: No leap second file path was found, a default leap second file will be used. This file contains all leap seconds up to and including the leap second on 2017 JAN 1. Add custom leap second data by specifying a path to a tai-utc.dat file with the \"--LeapSecondFile\" tag\n")
    
    add_options = True
    if(args.add_opts):
        add_options = False

    leapFile = leap_second_file(path = leapSecPath, default_data = leap_second_table)
    
    trk234 = read_all_trk234_frames(args.in_path.__str__())
    
    type9  = []
    type17 = []
    for item in trk234:
        if 'type17' in item: type17.append(item)
    
    for item in trk234:
        if 'type9' in item: type9.append(item)
    
    out_path = args.out_path.__str__()
    if out_path == "None":
        out_path = args.in_path.__str__()
    
        out_path = out_path.split(".")
        addEnd = True
        if len(out_path) > 1:
            if out_path[-1] == "gmd":
                out_path = ".".join(out_path[:-1])
            else:
                out_path = ".".join(out_path)
        else:
            out_path = ".".join(out_path)

    write_gmd(trk234, out_path + ".gmd",  leapFile, add_options)
    write_rmp(trk234, out_path + ".rmp",  leapFile)
    
    print()
    print("Run finished")

leap_second_table = """1961 JAN  1 =JD 2437300.5  TAI-UTC=   1.4228180 S + (MJD - 37300.) X 0.001296 S
                     1961 AUG  1 =JD 2437512.5  TAI-UTC=   1.3728180 S + (MJD - 37300.) X 0.001296 S
                     1962 JAN  1 =JD 2437665.5  TAI-UTC=   1.8458580 S + (MJD - 37665.) X 0.0011232S
                     1963 NOV  1 =JD 2438334.5  TAI-UTC=   1.9458580 S + (MJD - 37665.) X 0.0011232S
                     1964 JAN  1 =JD 2438395.5  TAI-UTC=   3.2401300 S + (MJD - 38761.) X 0.001296 S
                     1964 APR  1 =JD 2438486.5  TAI-UTC=   3.3401300 S + (MJD - 38761.) X 0.001296 S
                     1964 SEP  1 =JD 2438639.5  TAI-UTC=   3.4401300 S + (MJD - 38761.) X 0.001296 S
                     1965 JAN  1 =JD 2438761.5  TAI-UTC=   3.5401300 S + (MJD - 38761.) X 0.001296 S
                     1965 MAR  1 =JD 2438820.5  TAI-UTC=   3.6401300 S + (MJD - 38761.) X 0.001296 S
                     1965 JUL  1 =JD 2438942.5  TAI-UTC=   3.7401300 S + (MJD - 38761.) X 0.001296 S
                     1965 SEP  1 =JD 2439004.5  TAI-UTC=   3.8401300 S + (MJD - 38761.) X 0.001296 S
                     1966 JAN  1 =JD 2439126.5  TAI-UTC=   4.3131700 S + (MJD - 39126.) X 0.002592 S
                     1968 FEB  1 =JD 2439887.5  TAI-UTC=   4.2131700 S + (MJD - 39126.) X 0.002592 S
                     1972 JAN  1 =JD 2441317.5  TAI-UTC=  10.0       S + (MJD - 41317.) X 0.0      S
                     1972 JUL  1 =JD 2441499.5  TAI-UTC=  11.0       S + (MJD - 41317.) X 0.0      S
                     1973 JAN  1 =JD 2441683.5  TAI-UTC=  12.0       S + (MJD - 41317.) X 0.0      S
                     1974 JAN  1 =JD 2442048.5  TAI-UTC=  13.0       S + (MJD - 41317.) X 0.0      S
                     1975 JAN  1 =JD 2442413.5  TAI-UTC=  14.0       S + (MJD - 41317.) X 0.0      S
                     1976 JAN  1 =JD 2442778.5  TAI-UTC=  15.0       S + (MJD - 41317.) X 0.0      S
                     1977 JAN  1 =JD 2443144.5  TAI-UTC=  16.0       S + (MJD - 41317.) X 0.0      S
                     1978 JAN  1 =JD 2443509.5  TAI-UTC=  17.0       S + (MJD - 41317.) X 0.0      S
                     1979 JAN  1 =JD 2443874.5  TAI-UTC=  18.0       S + (MJD - 41317.) X 0.0      S
                     1980 JAN  1 =JD 2444239.5  TAI-UTC=  19.0       S + (MJD - 41317.) X 0.0      S
                     1981 JUL  1 =JD 2444786.5  TAI-UTC=  20.0       S + (MJD - 41317.) X 0.0      S
                     1982 JUL  1 =JD 2445151.5  TAI-UTC=  21.0       S + (MJD - 41317.) X 0.0      S
                     1983 JUL  1 =JD 2445516.5  TAI-UTC=  22.0       S + (MJD - 41317.) X 0.0      S
                     1985 JUL  1 =JD 2446247.5  TAI-UTC=  23.0       S + (MJD - 41317.) X 0.0      S
                     1988 JAN  1 =JD 2447161.5  TAI-UTC=  24.0       S + (MJD - 41317.) X 0.0      S
                     1990 JAN  1 =JD 2447892.5  TAI-UTC=  25.0       S + (MJD - 41317.) X 0.0      S
                     1991 JAN  1 =JD 2448257.5  TAI-UTC=  26.0       S + (MJD - 41317.) X 0.0      S
                     1992 JUL  1 =JD 2448804.5  TAI-UTC=  27.0       S + (MJD - 41317.) X 0.0      S
                     1993 JUL  1 =JD 2449169.5  TAI-UTC=  28.0       S + (MJD - 41317.) X 0.0      S
                     1994 JUL  1 =JD 2449534.5  TAI-UTC=  29.0       S + (MJD - 41317.) X 0.0      S
                     1996 JAN  1 =JD 2450083.5  TAI-UTC=  30.0       S + (MJD - 41317.) X 0.0      S
                     1997 JUL  1 =JD 2450630.5  TAI-UTC=  31.0       S + (MJD - 41317.) X 0.0      S
                     1999 JAN  1 =JD 2451179.5  TAI-UTC=  32.0       S + (MJD - 41317.) X 0.0      S
                     2006 JAN  1 =JD 2453736.5  TAI-UTC=  33.0       S + (MJD - 41317.) X 0.0      S
                     2009 JAN  1 =JD 2454832.5  TAI-UTC=  34.0       S + (MJD - 41317.) X 0.0      S
                     2012 JUL  1 =JD 2456109.5  TAI-UTC=  35.0       S + (MJD - 41317.) X 0.0      S
                     2015 JUL  1 =JD 2457204.5  TAI-UTC=  36.0       S + (MJD - 41317.) X 0.0      S
                     2017 JAN  1 =JD 2457754.5  TAI-UTC=  37.0       S + (MJD - 41317.) X 0.0      S"""
    
    
if __name__ == '__main__' : exit(main())