# -*- coding: utf-8 -*-
#TDM to GMD converter

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

class UtdfReader:
    """


    @author: jmcrensh
    """
    def read_utdf_frame(self, bytedata):
        
        #Dictionary definitions
        antsizes = {
                0 : 1.0,
                1 : 3.9,
                2 : 4.3,
                3 : 9.0, #or '10-m S-band antennas'
                4 : 12.0,#also 11-m for TDRS TTC
                5 : 26.0,
                6 : 'TDRSS ground antenna',
                7 : 6,
                8 : 7.3,
                9 : 8.0,
                15 : 'Unknown'
                }
        
        antmounts = {
                0 : 'AZ-EL',
                1 : 'X-Y (+X South)',
                2 : 'X-Y (+X East)',
                3 : 'RA-DEC',
                4 : 'HR-DEC',
                12 : 'Unknown',
                15 : 'Unknown'
                }
        
        tdrs_config = {
                0 : 'Spare1',
                1 : 'return-only',
                2 : 'forward-return',
                3 : 'Spare2'
                }
        
        tdrs_id = {
                0	: 'not used',
                1 : 1300,
                2	: 1301,
                3 : 1302,
                4 : 1303,
                5 : 1304,
                6 : 1305,
                7 : 1306,
                8 : 1307,
                9 : 1308,
                10 : 1309,
                11 : 1410,
                12 : 1411,
                13 : 1412,
                14 : 1413
                }
        
        transtype = {
                0 : 'Test',
                2 : 'Simulated',
                3 : 'Resubmit',
                4 : 'Real-Time', # Normal Service for TDRS
                5 : 'Playback'
                }
        
        trackertype = {
                0 : 'C-Band Pulse Track',
                1 : 'SRE (S-band and VHF) or RER',
                2 : 'X-Y angles only (data acquisition antenna)',
                3 : 'Unknown',
                4 : 'SGLS (AFSCF S-band trackers)',
                6 : 'TDRSS',
                7 : 'STGT/WSGTU',
                8 : 'TDRSS TT&C'
                }
        
        trackingmode = {
                0 : 'Autotrack',
                1 : 'Program Track',
                2 : 'Manual',
                3 : 'Slaved'
                }
        
        cbandbeaconskin = {
                0 : 'Beacon',
                1 : 'Skin'
                }
        
        paths = {
                0 : 'not used',
                1 : '1-way',
                2 : '2-way',
                3 : '3-way'
                }
        
        majortone = {
                0 : 'not used',
                1 : '20 kHz',
                2 : '100 kHz',
                3 : '500 kHz'
                }
        
        
        tdrs_user_bit_rate = {
                0 : '5000 b/sec < BR',
                1 : '1000 b/sec < BR <= 5000 b/sec',
                2 : '500 b/sec < BR <= 1000 b/sec',
                3 : 'BR <=  500 b/sec'
                }
        
        SA_strings = {
                False : 'primary',
                True : 'secondary'
                }
        
        #Extraction of encoded bytes
        byte45 = struct.unpack_from('>B', bytedata, offset=44)[0]
        byte47 = struct.unpack_from('>B', bytedata, offset=46)[0]    
        byte49 = struct.unpack_from('>B', bytedata, offset=48)[0]
        byte50 = struct.unpack_from('>B', bytedata, offset=49)[0]
        byte51 = struct.unpack_from('>B', bytedata, offset=50)[0]
        byte52 = struct.unpack_from('>B', bytedata, offset=51)[0]
        byte53 = struct.unpack_from('>B', bytedata, offset=52)[0]
        #For TDRS
        byte55 = struct.unpack_from('>B', bytedata, offset=54)[0]
        byte56 = struct.unpack_from('>B', bytedata, offset=55)[0]
        byte69 = struct.unpack_from('>B', bytedata, offset=68)[0]
        
        frame = {}
        
        #('Tracker Type')
        try:
            frame['tracker_type'] = trackertype[byte53 >> 4]
        except:
    #        print(byte53 >> 4)
            raise NameError('UnknownTrackerType')
        
        is_TDRS =  frame['tracker_type'] == 'TDRSS' or frame['tracker_type'] == 'STGT/WSGTU'
        is_TDRS_TTC = frame['tracker_type'] == 'TDRSS TT&C'


        frame['header1'] = struct.unpack_from('>B', bytedata, offset=0)[0]
        frame['header2'] = struct.unpack_from('>B', bytedata, offset=1)[0]
        frame['header3'] = struct.unpack_from('>B', bytedata, offset=2)[0]

        # print(struct.unpack_from(b'cc', bytedata, offset=3))
        frame['TRK_Router'] = b''.join(struct.unpack_from('cc', bytedata, offset=3)).decode("utf-8")
        
        frame['year'] = struct.unpack_from('>B', bytedata, offset=5)[0]

        frame['SIC'] = struct.unpack_from('>H', bytedata, offset=6)[0]
            
        frame['VIC'] = struct.unpack_from('>H', bytedata, offset=8)[0]
                
        #('Seconds of Year:')
        frame['seconds'] = struct.unpack_from('>I', bytedata, offset=10)[0]
            
        #('Microseconds of Second:')
        frame['microseconds'] = struct.unpack_from('>I', bytedata, offset=14)[0]
            
        #('Angle 1 (AZ / X):')
        frame['angle1'] = struct.unpack_from('>I', bytedata,
                                        offset=18)[0]*360.0/(2**32)
            
        #('Angle 2 (EL / Y):')
        frame['angle2'] = struct.unpack_from('>I', bytedata,
                                        offset=22)[0]*360.0/(2**32)
            
        #('RTLT nsec:')
        frame['rtlt'] = (float(struct.unpack_from('>H', bytedata,
            offset=26)[0]*(2**32) + struct.unpack_from('>I',
                    bytedata, offset=28)[0])/256.0)
                
            #('Doppler Count (mCycles for S-Band and VHF, Cycles/250 for X-Band):')
        frame['cycle_count'] = (struct.unpack_from('>H', bytedata,
        offset=32)[0]*(16**8) + struct.unpack_from('>I',
        bytedata, offset=34)[0])
        #('AGC dBm:')
        frame['AGC'] = struct.unpack_from('>H', bytedata,
        offset=38)[0]*(-150.0)/(2**13) -50.0
            
        #('Transmit Frequency Hz (multiples of 10)')
        frame['transmit_frequency'] = struct.unpack_from('>I', bytedata,
            offset=40)[0]*10.0
            
        #('Tx Antenna Diameter meters')
        try:
            frame['transmit_antenna_size'] = antsizes[byte45 >> 4]
        except:
    #        print(byte45 >> 4)
            raise NameError('UnknownTxAntSize')   
            
        #('Tx Antenna Mount')
        try:
            frame['transmit_antenna_mount'] = antmounts[byte45 & 0b00001111]
        except:
            raise NameError('UnknownMount')
            
                #print('Tx PAD ID')
        frame['transmit_pad_id'] = struct.unpack_from('>B', bytedata, offset=45)[0]
            
                
        #('Rx Antenna Diameter meters')
        try:
            frame['receive_antenna_size'] = antsizes[byte47 >> 4]
        except:
            raise NameError('UnknownRxAntSize') 
            
        #('Rx Antenna Mount')
        try:
            frame['receive_antenna_mount'] = antmounts[byte47 & 15]
        except:
            print(byte47 & 15)
            raise NameError('UnknownMount')
            
                #print('Rx PAD ID')
        frame['receive_pad_id'] = struct.unpack_from('>B', bytedata, offset=47)[0]
                
        #('Frequency Band')
        try:
            frame['Frequency Band'] = byte52 >> 4 #Use bands for further data retrieval
        except:
    #        print(byte52 >> 4)
            raise NameError('UnknownBand')
            

        #('Transmission Type')
        try:
            frame['Transmission Type'] = transtype[byte52 & 15]
        except:
            raise NameError('UnknownTransmissionType')        
                
    #    #('Tracker Type') #Done at top
    #    frame['tracker_type'] = trackertype[byte53 >> 4]
            
        #print('Last Frame')
        frame['last_frame'] = bool(byte53 & 8)
        
        #print('Transmission Rate (seconds)')
        transnum = ((byte53 & 3) << 8) + struct.unpack_from('>B', bytedata, offset=53)[0]
        if (byte53 & 4):
            transrate = 1.0/((~transnum & 1023) + 1)
        else:
            transrate = float(transnum)
                
        frame['transmission_rate'] = transrate

        
        frame['valid_angle'] = bool(byte51 & 0b00000100)
        frame['valid_range'] = bool(byte51 & 0b00000001)


        frame['angle_corrections_applied'] = bool(byte51 & 0b00001000)
        # If not for TDRS-TTC or TDRS):
        frame['angle_corrections_applied'] = bool(byte51 & 0b00001000)
        frame['sidelobe'] = bool(byte51 & 0b10000000)            
        frame['destruct_range_rate'] = bool(byte51 & 0b01000000)
        frame['refraction_corrected_range_range-rate'] = bool(byte51 & 0b00100000)
        frame['refraction_corrected_angles'] = bool(byte51 & 0b00010000)
        frame['valid_range-rate'] = bool(byte51 & 0b00000010)
        
        frame['signal_path'] = paths[(byte50 & 48) >> 4] #1-way, 2-way...
        
        if is_TDRS:
            frame['forward_TDRS_ID'] = tdrs_id[byte49 >> 4]
            frame['return_TDRS_ID'] = tdrs_id[byte49 & 15]
            frame['MA_return_ID'] = byte50 >> 3
            frame['TDRS_ground_transponder_track'] = bool(byte50 & 0b00000100)
            frame['TDRS_tracking_configuration'] = tdrs_config[byte50 & 3]
            
            frame['TDRS_orienation_valid'] = bool(byte55 & 0b10000000)
            frame['TDRS_angles_valid'] = bool(byte55 & 0b01000000)
            
            frame['forward_link'] = (byte55 & 0b00111000) >> 3
            frame['return_link'] = byte55 & 0b00000111
            
            frame['TDRS_user_bit_rate'] = tdrs_user_bit_rate[(byte56 & 0b11000000) >> 6]
            
            #if 1309 user SIC not from 1310 to 1372
            frame['TDRS_ground_transponder'] = (byte56 & 0b00111111)
            
            frame['TDRS_yaw'] = struct.unpack_from('>H', bytedata,
                                        offset=56)[0]*360.0/(2**16)
            frame['TDRS_roll'] = struct.unpack_from('>H', bytedata,
                                        offset=58)[0]*360.0/(2**16)
            frame['TDRS_pitch'] = struct.unpack_from('>H', bytedata,
                                        offset=60)[0]*360.0/(2**16)
            tdrs_azimuth = (struct.unpack_from('>I', bytedata,
                offset=61)[0] & 0b00000000111111111111111111111111)# * 90.0/(2**23)
            if tdrs_azimuth > 0b011111111111111111111111:
                frame['TDRS_azimuth'] = -(~tdrs_azimuth & 0b011111111111111111111111) * 90.0/(2**23)
            else:
                frame['TDRS_azimuth'] = tdrs_azimuth * 90.0/(2**23)
            
            tdrs_elevation = (struct.unpack_from('>H', bytedata,
                offset=64)[0] & 0b00000000111111111111111111111111)# * 90.0/(2**23)
            if tdrs_elevation > 0b011111111111111111111111:
                frame['TDRS_elevation'] = -(~tdrs_elevation & 0b011111111111111111111111) * 90.0/(2**23)
            else:
                frame['TDRS_elevation'] = tdrs_elevation * 90.0/(2**23)
            
            frame['doppler_compensation'] = not bool(byte69 & 0b10000000)
            frame['receiver_PN_lock'] = bool(byte69 & 0b01000000)
            frame['receiver_carrier_lock'] = bool(byte69 & 0b00100000)
            
            frame['sglt_string'] = (byte69 & 0b00011000) >> 3 #TODO: what to do with this? Station-Equipment chain used
            
            frame['SA_string'] = SA_strings[bool(byte69 & 0b00000100)]
            
            frame['SMAR_downlink_frequency'] = struct.unpack_from('B',
                bytedata, offset=69)[0]
            
        else:        
            #print('Tracking Mype')
            frame['antenna_tracking_mode'] = trackingmode[(byte50 & 12) >> 2]
            
            if is_TDRS_TTC: #if TDRS-TTC
                frame['primary_uplink'] = bool(byte49 & 8)
                frame['primary_downlink'] = bool(byte49 & 4)
                
            elif frame['tracker_type'] == 'C-Band Pulse Track': #If C-Band          
                frame['C-Band_beacon_or_skin'] = cbandbeaconskin[byte50 & 3]
                
            elif frame['tracker_type'] == 'SRE (S-band and VHF) or RER':
                frame['lowest_range_tone_is_10Hz'] = bool(((byte50 & 192) >> 6) == 1)
                frame['major_range_tone'] = majortone[byte49 & 3]
 
                if frame['Frequency Band'] != 'VHF': 
                    frame['coherent'] = not bool(byte50 & 1)
                    frame['primary'] = bool(byte50 & 2)
                    frame['tracking_receiver_number'] = (byte49 & 28) >> 2
                    frame['ranging_receiver_number'] = (byte49 & 224) >> 5
                            
        return frame


    def read_all_utdf_frames(self, filein):
        
        utdf_data = []
        
        with open(filein, "rb") as fin:
            bytedata = fin.read(75)
            while bytedata:
                utdf_data.append(self.read_utdf_frame(bytedata))
                bytedata = fin.read(75)
                
        return utdf_data        


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
        return timedelta(seconds=0)
    

class GMDWriter:

    def __init__(self, utdfData, outFilePath, leapSecondPath = None, writeDowd = True, writeRtn = False):
        self.utdfData = utdfData
        self.measIndex = 0
        self.measCount = len(utdfData)
        self.measLine = {}
        self.oldDoppler_list = {}
        self.oldTime_list    = {}
        self.oldTracker_list = {}
        self.oldTrackerType_list = {}
        self.rcvPadID_list   =[]
        self.recNum     = 0
        self.maxTimeBetweenPasses = 61
        self.numRcvPad = 0
        self.leapSecondData = leap_second_file(path = leapSecondPath, default_data = leap_second_table)
        self.outFilePath = outFilePath
        self.outFile = open(self.outFilePath, "w") 
        self.writeRtn = writeRtn
        self.writeDowd = writeDowd
        self.oneWayMeasByEpoch = {}
        self.lastOutDowdPath = []
        self.printOneWayAssumeWarn = True
        self.printOneWayXmitNonZero = True
    
    def convert_time_to_modjulian(self, date, microseconds):
        delta = (date - TAIMJD_base)
        diffDays = delta.days
        diffHours = abs(date.hour - TAIMJD_base.hour)
        diffMins = abs(date.minute - TAIMJD_base.minute)
        diffSecs = abs(date.second - TAIMJD_base.second)
        fractDiff = 0.5 + (diffHours / 24.0) + (diffMins / 1440.0) + ((diffSecs + microseconds) / 86400.0)
        
        return ((date - TAIMJD_base).total_seconds() + microseconds) / 86400.0
    
    

    #Processes a line of utdf data and writes it to file
    def processUtdfMeas(self):
        utdfDataLine = self.utdfData[self.measIndex]
        

                # exit if the sync codes are invalid 
        if (utdfDataLine["header1"] != 13 or utdfDataLine["header2"] != 10 or utdfDataLine["header3"] != 1):
            return
        
        # Bytes 4-5 = AA for TDRS UTDF, DD for UTDF (is this always true?)
        trackingDataRouter = utdfDataLine["TRK_Router"]
        
        if (trackingDataRouter == "AA"):
            tdrsUTDFData = 1
        else:
            tdrsUTDFData = 0
            
        
        
        year = utdfDataLine["year"]
        secondsOfYear = utdfDataLine["seconds"]
        microSeconds = utdfDataLine["microseconds"]
        dayOfYear = math.floor(secondsOfYear / 86400.0)
        secondsOfDay = secondsOfYear - dayOfYear*86400.0
        hours = math.floor(secondsOfDay/3600.0)
        minutes = math.floor((secondsOfDay - hours*3600)/60.0)
        seconds = secondsOfYear - dayOfYear*86400.0 - hours*3600.0 - minutes*60.0
            
        newTime = secondsOfYear + microSeconds/1e06

        y = abs(year - TAIMJD_base.year)

        baseSoy = (TAIMJD_base.timetuple().tm_yday * 86400.0) +  (TAIMJD_base.hour * 3600.0) + (TAIMJD_base.minute * 60.0) + TAIMJD_base.second
        soy = secondsOfYear - baseSoy

        
        angle1 = utdfDataLine["angle1"]
        #angle1Deg = angle1 * 360 / 2^32
        angle1Rad = math.pi/180* angle1
        

        angle2 = utdfDataLine["angle2"]
        #angle2Deg = angle2 * 360 / 2^32
        angle2Rad = math.pi/180* angle2
        
        #Round Trip Light Time   
        RTLT = utdfDataLine["rtlt"]
        
        # math.piece together the 6 bytes
        #RTLT = 0
        #for i in range(1, 7):
            #RTLT += R[i-1] << (48 - i * 8)
        
        #fprintf("RTLT = #21.3f\n",RTLT)
        
        # Bytes 33-36 = Bias Doppler      
        D = utdfDataLine["cycle_count"]
        #fprintf ("D = #d\n",D)
        newDoppler = D
        # math.piece together the 6 bytes
        #newDoppler = 0
        #for i in range(1, 7):
            #newDoppler += D[i-1] << (48 - i * 8)
        

        # Bytes 39-40 = AGC
        if "AGC" in utdfDataLine:
            AGC = utdfDataLine["AGC"]
        
        # Bytes 41-44 = Transmit Frequency in 10s of Hz
        transmitFrequencyHz = utdfDataLine["transmit_frequency"]
        frequencyNode4 = transmitFrequencyHz

        # Byte 45 MSD = xmit antenna size 
        xmitAntennaSize = utdfDataLine["transmit_antenna_size"]

        # Byte 45 LSD = xmit antenna geometry 
        xmitAntennaGeometry = utdfDataLine['transmit_antenna_mount']
        
        # Byte 46 xmit Pad ID
        xmitPadID = utdfDataLine["transmit_pad_id"]   
        
        # Byte 47 MSD = rcv antenna size 
        rcvAntennaSize = utdfDataLine["receive_antenna_size"]
        #fprintf ("rcvAntennaSize = #d\n",rcvAntennaSize) 

        # Byte 47 LSD = rcv antenna geometry 
        rcvAntennaGeometry = utdfDataLine["receive_antenna_mount"]
        #fprintf ("rcvAntennaGeometry = #d\n",rcvAntennaGeometry) 
        
        # Byte 48 rcv Pad ID
        rcvPadID = utdfDataLine["receive_pad_id"]
        
        # check if this station has already been defined
        found_station = 0    
        
        for i in self.rcvPadID_list:
            if (rcvPadID == i):
                found_station = 1
                this_rcvPadID = i
            
        
        
        # if not, add it to the list
        if (found_station == 0):
            self.rcvPadID_list.append(rcvPadID)
            this_rcvPadID              = rcvPadID
            self.oldTime_list[this_rcvPadID]      = 0
            self.oldDoppler_list[this_rcvPadID]      = 0
            self.oldTrackerType_list[this_rcvPadID] = 0

            
        
        # set the saved values for this tracker
        oldTime    = self.oldTime_list[this_rcvPadID]
        oldDoppler = self.oldDoppler_list[this_rcvPadID]
        oldTracker = self.oldTrackerType_list[this_rcvPadID]
        
        # Byte 49 MSD = forward link TDRS ID 
        #forwardLinkTDRSID = utdfDataLine["forward_TDRS_ID"]
        
        # Byte 49 LSD = return link TDRS ID
        if "return_TDRS_ID" in utdfDataLine:
            returnLinkTDRSID = utdfDataLine["return_TDRS_ID"]
        
            returnLinkTDRSSIC = returnLinkTDRSID

            # Byte 50 Bits 4-8 = MA return link ID 
            MAreturnLinkID = utdfDataLine["MA_return_ID"]
        
            # Byte 50 Bit 3 = return link TDRS ID 
            trackingDataOnly = utdfDataLine["TDRS_tracking_configuration"]
        
            # Byte 50 Bit2 1-2 = tracking service configuration 
            trackingServiceConfiguration = utdfDataLine["TDRS_ground_transponder_track"]
            #set range validity flags to zero 
            # if trackingServiceConfiguration indicates 1-way data
            if (trackingServiceConfiguration == 1):
                rangeValid     = 0
                oneWay = 1
                #rangeRateValid = 0
            else:
                oneWay = 0
        
            # Byte 51 Data validity bits
    
            # Byte 51 bit 8 = sidelobe
        sidelobe = utdfDataLine["sidelobe"]
        
        # Byte 51 bit 7 = destruct range rate
        destructRangeRate = utdfDataLine["destruct_range_rate"]
        
        # Byte 51 bit 6 = refraction correction to Range, RangeRate
        refractionCorrectionRange = utdfDataLine["refraction_corrected_range_range-rate"]
        
        # Byte 51 bit 5 = refraction correction to Angles
        refractionCorrectionAngle = utdfDataLine["refraction_corrected_angles"]
        
        # Byte 51 bit 4 = angle data correction
        angleCorrection = utdfDataLine["angle_corrections_applied"]

        # Byte 51 bit 3 = angle valid
        angleValid = utdfDataLine["valid_angle"]
        
        # Byte 51 bit 2 = RangeRate valid
        rangeRateValid = utdfDataLine["valid_range-rate"]
        
        # Byte 51 bit 1 = Range valid
        rangeValid = utdfDataLine["valid_range"]
        #fprintf ("rangeValid = #d\n",rangeValid)
        
        
        

        # Byte 52 MSD = frequency band
        frequencyBand = utdfDataLine["Frequency Band"]
        #fprintf ("frequencyBand = #x\n",frequencyBand) 
        
        #               UTDF Flag   GMD Flag
        #   S-Band      3           1
        #   X-Band      5           2
        #   K-Band      6           3
        #   Undefined               0
        
        if (frequencyBand == 3):
            outputFreqBand = 1
        elif (frequencyBand == 5):
            outputFreqBand = 2
        elif (frequencyBand == 6):
            outputFreqBand = 3
        else:
            outputFreqBand = 0  
            
        
        # Byte 52 LSD = data transmission type
        dataTransmissionType = utdfDataLine["Transmission Type"]
            
        # Byte 53 MSD = tracker type
        trackerType = utdfDataLine["tracker_type"]
        #fprintf ("trackerType = #x\n",trackerType) 
        
        if (trackerType == 6): 
            TDRSK = 1
        else:
            TDRSK = 0
                
        # Byte 53 bit 4 = last frame of data
        lastFrameOfData = utdfDataLine["last_frame"]
        #fprintf ("lastFrameOfData = #x\n",lastFrameOfData) 
        
        # Byte 53 bit 3 = transmission rate bit
        # Byte 53 bit 2 = transmission rate bit
        # Byte 53 bit 1 = transmission rate bit
        transmitRate = utdfDataLine["transmission_rate"]
        
        # Byte 54 = transmission bits
        #byte54 = fread(fid,1,"uint8")
        
        if (tdrsUTDFData == 1):
            # Byte 55 Bit 8 = TDRS orientation data validity
            tdrsOrientationDataValidity = utdfDataLine["TDRS_orienation_valid"]
            
            # Byte 55 Bit 7 = TDRS return-link orientation data validity
            tdrsReturnLinkOrientationDataValidity = utdfDataLine["TDRS_angles_valid"]
            
            # Byte 55 Bits 4-6 = Forward-Link
            if "forward_link" in utdfDataLine:
                forwardLink = utdfDataLine["forward_link"]
            else:
                forwardLink = 0
            
            if (forwardLink == 1):
                forwardService = "SA1"
            elif (forwardLink == 3):
                forwardService = "MA"
            elif (forwardLink == 6):	
                forwardService = "SA2"
            else:
                forwardService = "NA"	
        

        # Byte 55 Bits 1-3 = Return-Link
        if "return_link" in utdfDataLine:
            returnLink = utdfDataLine["return_link"]
        else:
            returnLink = 0 
    

        if (returnLink == 1):
            returnService = "SA1"
        elif (returnLink == 3):
            returnService = "MA"
        elif (returnLink == 6):
            returnService = "SA2"
        else:
            returnService = "NA"	
        

        # Byte 56 Bits 7-8 = User Bit Rate
        userBitRate = 0
        groundBasedTransponderID = 0
        if "TDRS_user_bit_rate" in utdfDataLine:
            userBitRate = utdfDataLine["TDRS_user_bit_rate"]
            groundBasedTransponderID = utdfDataLine["TDRS_ground_transponder"]

            # Bytes 57-58 = TDRS Orientation Yaw angle
            yaw = utdfDataLine["TDRS_yaw"]
            yawRad = math.pi/180* yaw
        
            # Bytes 59-60 = TDRS Orientation Roll angle
            roll = utdfDataLine["TDRS_roll"]
            rollRad = math.pi/180* roll
        
            # Bytes 61-62 = TDRS Orientation math.pitch angle
            pitch = utdfDataLine["TDRS_pitch"]
            pitchRad = math.pi/180* pitch
        
            # Bytes 63-65 = TDRS Return-Link Orientation Azimuth Angle
            az = utdfDataLine["TDRS_azimuth"]
            #fprintf ("az = #12.6f\n",az)
        
            a1 = az
            azimuth = az

            # Bytes 66-68 = TDRS Return-Link Orientation Elevation Angle
            elevation = utdfDataLine["TDRS_elevation"]
        
            # Byte 69 Bit 8 = Doppler compensation
            dopplerCompensation = utdfDataLine["doppler_compensation"]
        
            # Byte 69 Bit 7 = PN lock
            PNLock = utdfDataLine["receiver_PN_lock"]
        
            # Byte 69 Bit 6 = carrier lock
            carrierLock = utdfDataLine["receiver_carrier_lock"]
        
            # Byte 69 Bit 4-5 = SGLT
            SGLT = utdfDataLine["sglt_string"]
            
        
            # Byte 69 Bit 3 = SA equipment chain
            SAEquipmentChain = utdfDataLine["SA_string"]
        
            # Byte 70 = SMAR downlink frequency ID
            SMARDownlinkFrequencyID = utdfDataLine["SMAR_downlink_frequency"]
    

        # compute the change in the Doppler counts and the change in time
        deltaDoppler = newDoppler - oldDoppler
        deltaTime    = newTime    - oldTime
    
        # invalidate 1st Doppler measurement in a pass
        if (deltaTime > self.maxTimeBetweenPasses):
            rangeRateValid = 0
        
        
        # invalidate 1st Doppler measurement if data has switched from UTDF to TDRS UTDF
        if (oldTracker != trackerType):
            rangeRateValid = 0
        
        
        # some constants
        c  = speedOfLight
        fT = transmitFrequencyHz
        B  = frequencyBias
        
        # set K and M based on frequency band
        # S-Band
        if (frequencyBand == 3) or (frequencyBand == 0):
            K = 240/221
            M = 1000
        # X-Band  
        elif (frequencyBand == 5):
            K = 880/749
            M = 250
        # Ku-Band  
        elif (frequencyBand == 6):
            K = 1/2
            M = 100   
        else:
            K = 1
            M = 1   
        
        #print(xmitPadID)
        if tdrsUTDFData == 0:
            if utdfDataLine["signal_path"] == "1-way":
                K = 1 #Turn around ratio 1/1 for one way
            
        #fprintf("K = #12.6f\n",K)
        
        # compute the range rate in km/sec
        
        if fT == 0.0 or K == 0 or M == 0 or deltaTime == 0:
            rangeRateValid = 0
            return
        
        rangeRate = -c/(fT*K*M) * ((deltaDoppler / deltaTime) - B) 
        observedAverageDoppler = (1.0/M) * ((deltaDoppler / deltaTime) - B)
        observedAverageDopplerHz = 1.0e06 * observedAverageDoppler
        
        # convert RTLT in 1/256 nsec into two-way range in km
        range = c * (RTLT /256)/1.0e9
        #fprintf ("range = #21.6f\n",range)
        
        # save the Doppler counts and time
        self.oldTime_list[this_rcvPadID]     = newTime    
        self.oldDoppler_list[this_rcvPadID]     = newDoppler
        self.oldTrackerType_list[this_rcvPadID] = trackerType
        
        # format the output time to taimjd
        year = year + 2000
        dayOfYear = dayOfYear + 1
        timeStr = str(year) + "-" + str(dayOfYear) + "-" + str(hours) + "-" + str(minutes) + "-" + str(int(seconds))

        dnum   = datetime.strptime(timeStr, "%Y-%j-%H-%M-%S")
        dnum   = dnum + self.leapSecondData.getLeapSecondDelta(dnum)
        mj     = self.convert_time_to_modjulian(dnum, microSeconds)
        
        SIC = utdfDataLine["SIC"]


        if ((rangeRateValid > 0) and (abs(rangeRate) != float("inf"))):
            if (self.recNum > 1):
                # Write to the TDRS UTDF output file
                if (tdrsUTDFData == 1):
                    if (groundBasedTransponderID == 0):
                        # one-way return Doppler  
                        if (oneWay == 1):
                            if(self.writeRtn):
                                self.outFile.write("%5.15f    SN_Doppler_Rtn  9026    { %s    %s    %s }   %0.12e    %s    %s   %s    %s   %9.6f   %15.6f\n" % (mj,SIC,returnLinkTDRSSIC,rcvPadID,frequencyNode4,outputFreqBand,returnService,TDRSK,SMARDownlinkFrequencyID,deltaTime,observedAverageDoppler));  
                            if(self.writeDowd):
                                oneWayMeas  = [SIC,returnLinkTDRSSIC,rcvPadID,frequencyNode4,outputFreqBand,returnService,TDRSK,SMARDownlinkFrequencyID,deltaTime,observedAverageDoppler]
                                if not mj in self.oneWayMeasByEpoch:
                                    self.oneWayMeasByEpoch[mj] = []
                                else:
                                    #Check to make sure no duplicate TDRS at a given epoch
                                    for i in self.oneWayMeasByEpoch[mj]:
                                        if i[1] == oneWayMeas[1]:
                                            return
                                self.oneWayMeasByEpoch[mj].append(oneWayMeas)
                                #self.outFile.write("%5.15f    SN_DOWD  9028    { %s    %s    %s    %s    %s }   %0.12e    %s    %s   %s    %s   %9.6f   %15.6f\n" % (mj,SIC,returnLinkTDRSSIC,rcvPadID,frequencyNode4,outputFreqBand,returnService,TDRSK,SMARDownlinkFrequencyID,deltaTime,observedAverageDoppler));  
                #Write RangeRate
                elif tdrsUTDFData == 0:
                    if self.printOneWayXmitNonZero:
                        if utdfDataLine['signal_path'] == "1-way" and xmitPadID != 0:
                            print("WARNING: A UTDF data line was found where the signal path is 1-way but the Pad ID (xmit) is not set to 0.")
                            print("     At Epoch: Year: ", utdfDataLine["year"], " Seconds Of Year: ", utdfDataLine["seconds"], " Microseconds: ", utdfDataLine["microseconds"]) 
                            print("     Pad ID (xmit) found: ", xmitPadID)
                            print("     This data will be treated as 1-way data. This warning will only be printed once per run.\n")
                            self.printOneWayXmitNonZero = False
                    if utdfDataLine['signal_path'] == "1-way":
                            #GN One Way
                        self.outFile.write("%5.15f    RangeRate  9012    { %s    %s }    %s   %9.6f   %15.15e\n" % (mj, SIC,rcvPadID,outputFreqBand,deltaTime,rangeRate));
                    else: 
                        #Two Way or Three Way
                        self.outFile.write("%5.15f    RangeRate  9012    { %s    %s    %s }    %s   %9.6f   %15.15e\n" % (mj,xmitPadID,SIC,rcvPadID,outputFreqBand,deltaTime,rangeRate));
    
    def writeAllData(self):
        for i in self.utdfData:
            self.processUtdfMeas()
            self.measIndex += 1
            self.recNum += 1
        if self.writeDowd:
            self.writeDowdData()
    
    
    
    def writeDowdData(self):
        for epoch in self.oneWayMeasByEpoch:
            numMeasAtEpoch = len(self.oneWayMeasByEpoch[epoch])
            for i in range(numMeasAtEpoch):
                 for j in range(i+1, numMeasAtEpoch):
                    m1 = self.oneWayMeasByEpoch[epoch][i]
                    m2 = self.oneWayMeasByEpoch[epoch][j]
                    #fixes a bug where occasionally the dowd record path was reveresed.
                    #This fix could cause issues if a DOWD record happens to want the exact reversed path as the one before. This is doubtful and would be very very rare. 
                    if self.lastOutDowdPath == [m1[0],m2[1],m2[2],m1[1], m1[2]]:
                        m1 = self.oneWayMeasByEpoch[epoch][j]
                        m2 = self.oneWayMeasByEpoch[epoch][i]
                    self.lastOutDowdPath = [m1[0],m1[1],m1[2],m2[1], m2[2]]
                    #mj,SIC,returnLinkTDRSSIC,rcvPadID,returnLinkTDRSSIC2,rcvPadID2,frequencyNode4,outputFreqBand,returnService,TDRSK,SMARDownlinkFrequencyID,deltaTime,frequencyNode42,outputFreqBand2,returnService2,TDRSK2,SMARDownlinkFrequencyID2,deltaTime2,observedDiffAverageDoppler     
                    self.outFile.write("%5.15f    SN_DOWD  9028    { %s    %s    %s    %s    %s }   %0.12e    %s    %s   %s    %s   %9.6f   %0.12e    %s    %s   %s    %s   %9.6f   %0.12e\n" % (epoch, m1[0],m1[1],m1[2],m2[1], m2[2], m1[3],m1[4],m1[5],m1[6],m1[7],m1[8], m2[3],m2[4],m2[5],m2[6],m2[7],m2[8], m2[9]-m1[9]));  
                                        

def main():

    parser = argparse.ArgumentParser(description='UTDF to GMD Converter')
    parser.add_argument('-i', '--input', required = True, type=Path, metavar='IN_PATH', dest='in_path', help='Input file')
    parser.add_argument('-o', '--output',  type=Path, metavar='OUT_PATH', dest='out_path', help='Output path')
    parser.add_argument('-l', '--LeapSecondFile',  type=Path, metavar='LEAP_PATH', dest='leap_path', help='Leap second file path')
    parser.add_argument('-dowd', '--CreateDowdData',  action='store_true', dest='DOWD', help='Write out DOWD data')
    parser.add_argument('-rtn', '--CreateSnDopplerRtnData',  action='store_true', dest='Rtn', help='Write out Sn_Doppler_Rtn data')
    #parser.add_argument('-apply_receive_delay_to_timetag', metavar='True|False',  dest="apply_receive_delay_to_timetag", help = "True|False Whether or not to apply the recieve delay to the time tag, this is included in observation calculations regardless. Default: True")
    
    args = parser.parse_args()

    print()
    print("Beginning run...")
    print()

    if not args.in_path:
        print("ERROR: must specify input file path with --input tag")
        return
    if not args.out_path:
        print("ERROR: must specify output file path with --output tag")
        return
    leapSecPath = args.leap_path.__str__()
    

    utdfReader = UtdfReader()

    utdf_all = utdfReader.read_all_utdf_frames(args.in_path.__str__())
    if leapSecPath == "None":
        leapSecPath = None
        print("WARNING: No leap second file path was found, a default leap second file will be used. This file contains all leap seconds up to and including the leap second on 2017 JAN 1. Add custom leap second data by specifying a path to a tai-utc.dat file with the \"--LeapSecondFile\" tag\n")

    dowd = False
    if args.DOWD != None:
        dowd = args.DOWD

    rtn = False
    if args.Rtn != None:
        rtn = args.Rtn

    if(rtn == False):
        print("WARNING: Sn_Doppler_Rtn writing is turned off so no Sn_Doppler_Rtn data will be written. To enable Sn_Doppler_Rtn writing, use the flag -rtn\n")
    if(dowd == False):
        print("WARNING: DOWD writing is turned off so no DOWD data will be written. To enable DOWD writing, use the flag -dowd\n")
    writer = GMDWriter(utdf_all, args.out_path.__str__(), leapSecondPath = leapSecPath, writeDowd=dowd, writeRtn=rtn )
    writer.writeAllData()


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