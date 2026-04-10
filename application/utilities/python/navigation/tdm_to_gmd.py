#TDM to GMD converter

import datetime
import os

#For arg parse
import argparse
from pathlib import Path
import math

TAIMJD_base = datetime.datetime(1941, 1, 5, 12)


class TdmReader:
    def __init__(self, in_path, leapSecondPath = None, applyRecieveDelay = True):
        self.in_path = in_path
        self.current_meta = {}
        self.data_blocks  = []
        self.header_data  = {}
        self.leapSecondData = None
        self.current_data_blocks = {}
        self.applyRecieveDelay = applyRecieveDelay
        self.leapSecondData = leap_second_file(path = leapSecondPath, default_data = leap_second_table)
        
        self.ignored_data_types = {}
        self.currentUTCformat = 0
        
        #The various data types currently supported, if adding more, add them here
        self.data_types = {
            "TRANSMIT_FREQ_1": TransmitFreqBlock,
            "TRANSMIT_FREQ_RATE_1": TransmitFreqBlock,
            "RECEIVE_FREQ_1": ReceiveFreqBlock,
            "DOPPLER_INTEGRATED" :  DopplerIntegratedBlock,
            "RANGE": RangeBlock
        }
        
    
    #Reads the tdm file and writes the data 
    def read_tdm(self):
        
        #Open the file
        with open(self.in_path, "r") as tdm_file:
            meta = False
            data = False
            
            #Read the tdm file line by line
            for line in tdm_file:
                #Ignore blank lines
                line = line.strip()
                if line == "":
                    continue

                #Read in data and convert
                elif data: 
                    if line == "DATA_STOP":
                        data = False
                        for i in self.data_types.keys():
                            if i == "TRANSMIT_FREQ_RATE_1":
                                continue
                            k = i.split("_")[0]
                            if len(self.current_data_blocks[k].data) > 0:
                                self.data_blocks.append(self.current_data_blocks[k])
                        self.current_data_blocks = {}
                    else:
                        l = line.split("=")
                        type = l[0].strip()
                        k = type.split("_")[0]
                        if type in self.data_types:
                            self.current_data_blocks[k].data.append(l)
                        else:
                            if not type in self.ignored_data_types:
                                self.ignored_data_types[type] = 0
                                print("WARNING: Data type \"" + type + "\" discovered. This data type is not supported by GMAT and will be ignored.\n")

                #Read in meta data as a dictionary
                elif meta: 
                    if line == "META_STOP":
                        meta = False
                    else:
                        l = line.split("=")
                        if len(l) == 2:
                            self.current_meta[l[0].strip()] = l[1].strip()

                #Find the start of meta        
                elif line == "META_START": 
                    meta = True
                    self.meta_is_valid = {}
                    self.current_meta = {}
                
                #find the start of data
                elif line == "DATA_START":
                    data = True
                    self.make_current_blocks()

                else:
                    l = line.split("=")
                    if len(l) == 1:
                        self.header_data[l[0].strip()] = ""
                    else:
                        self.header_data[l[0].strip()] = l[1].strip()

    
    
    def make_current_blocks(self):
        self.current_data_blocks = {}
        for i in self.data_types.keys():
            k = i.split("_")[0]
            self.current_data_blocks[k] = RawTDMBlock(self.current_meta)
    #Converts the tdm line to a gmd line
    def convert_data(self):
        #Go through once getting tranmit freq blocks to set up for Range blocks if required
        transmitIndices = []
        for index, block in enumerate(self.data_blocks):
            if isinstance(block, RawTDMBlock):  
                if block.get_data_type() == "TRANSMIT_FREQ_1" or block.get_data_type() == "TRANSMIT_FREQ_RATE_1":
                    self.data_blocks[index] = self.data_types[block.get_data_type()](block, leapSecondData = self.leapSecondData, applyRecieveDelay = self.applyRecieveDelay)
                    transmitIndices.append(index)
        
        #Go through again for the rest        
        for index, block in enumerate(self.data_blocks):
            if isinstance(block, RawTDMBlock):
                if block.get_data_type() in self.data_types:
                    if block.get_data_type() == "RANGE":
                        self.data_blocks[index] = RangeBlock(block, [self.data_blocks[i] for i in transmitIndices],leapSecondData = self.leapSecondData, applyRecieveDelay = self.applyRecieveDelay)
                    else:
                        self.data_blocks[index] = self.data_types[block.get_data_type()](block, leapSecondData = self.leapSecondData, applyRecieveDelay = self.applyRecieveDelay)
        
        #Remove not transformed blocks
        db = []
        for b in self.data_blocks:
            if not isinstance(b, RawTDMBlock):
                db.append(b)
        self.data_blocks = db
                
    def not_all_blocks_converted(self):
        retval = False
        for block in self.data_blocks:
            retval = retval or isinstance(block, RawTDMBlock)
        return retval        
    #writes the current data block
    def write_block(self):
        pass
        
    #Blocks is a list of lists, each inner list is a data block
    def merge_blocks(self, blocks):
        
        block_indexs = [0] * len(blocks)
        total_len = 0
        
        ret_data = []
        for b in blocks:
            total_len += len(b.data)
        
        for i in range(total_len):
            #The split here is inefficient, possibly find faster solution, maybe store epochs in blocks, or store lines as lists and join later...
            lowest = 2941535.0 #This will break in the year 10000...
            low_num = 0
            for b_num, current_index in enumerate(block_indexs):
                if current_index < len(blocks[b_num].data):
                    #The split here is inefficient, possibly find faster solution
                    if float(blocks[b_num].data[block_indexs[b_num]].split()[0]) < lowest:
                        lowest = float(blocks[b_num].data[block_indexs[b_num]].split()[0])
                        low_num = b_num
            
            ret_data.append(blocks[low_num].data[block_indexs[low_num]])
            block_indexs[low_num] += 1
            
        return "\n".join(ret_data)
    
    def get_rmp_table(self):
        transmitIndices = []
        for index, block in enumerate(self.data_blocks):
            if block.get_data_type() == "TRANSMIT_FREQ_1" or block.get_data_type() == "TRANSMIT_FREQ_RATE_1":
                transmitIndices.append(index)
        
        return self.merge_blocks([self.data_blocks[i] for i in transmitIndices])
            
        
    def get_gmd(self):
        nonTransmitIndices = []
        for index, block in enumerate(self.data_blocks):
            if block.get_data_type() != "TRANSMIT_FREQ_1" and block.get_data_type() != "TRANSMIT_FREQ_RATE_1":
                nonTransmitIndices.append(index)
        
        return self.merge_blocks([self.data_blocks[i] for i in nonTransmitIndices])
        
        
    def write_files(self, out_path, in_path):
        

        if out_path == "None":
            out_path = in_path
        
            out_path = out_path.split(".")
            addEnd = True
            if len(out_path) > 1:
                if out_path[-1] != "gmd":
                    out_path = ".".join(out_path[:-1])
                    out_path += ".gmd"
                else:
                    out_path = ".".join(out_path)
            else:
                out_path = ".".join(out_path)
        
        commentString = "% Created using GMAT python accessory: " + os.path.basename(__file__) + " on " + str(datetime.datetime.now()).split(".")[0] + "\n% Input file used: " + in_path +  "\n\n"
        rmp = self.get_rmp_table()
        
        if rmp != "":
            try:
            
                out = out_path.split(".")
                if out[-1] == "gmd":
                    out = ".".join(out[:-1])
                else:
                    out = ".".join(out)
                    
                out = out + ".rmp"

                
                r = open(out, "w")
                
                rmp = commentString + rmp
                r.write(rmp)
                print("Wrote .rmp file to \"" + out + "\"")
                r.close()
            except:
                print("ERROR: failed to write file \"" + out + "\"")
                print("Please specify a valid path using the \"--output\" flag")
        
        gmd = self.get_gmd()
        if gmd != "":
            try:
                out = out_path
                g = open(out, "w")
                gmd = commentString + gmd
                g.write(gmd)
                print("Wrote .gmd file to \"" + out + "\"")
                g.close()
            except:
                print("ERROR: failed to write file \"" + out + "\"")
                print("Please specify a valid path using the \"--output\" flag")



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
                epoch = datetime.datetime.strptime(strpdat, "%Y %b %d")
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
                epoch = datetime.datetime.strptime(strpdat, "%Y %b %d")
                secs = float(dat[6])
                self.leapSecs.append({"epoch":epoch, "mjd":mjd, "leapSecs":secs})
        except:
            raise Exception("ERROR: Failed to parse default leap second data, please specify a valid leap second file.")
            
            
    def getLeapSecondDelta(self, epoch):
        try:
            for i in reversed(range(len(self.leapSecs))):
                if epoch > self.leapSecs[i]["epoch"]:
                    return datetime.timedelta(seconds=self.leapSecs[i]["leapSecs"])
        except:
            print("WARNING: Failed to load leap second data, please specify a valid leap second file. No leap second data will be used for this run")
            self.leapSecs = []
        return datetime.timedelta(seconds=0)
        
class RawTDMBlock:
    def __init__(self, meta):
        self.meta = meta
        self.data = []

    def get_data_type(self):
        return self.data[0][0].strip()
        
        
    
class TdmBlock:
    def __init__(self, RawTDMBlock, leapSecondData = None, applyRecieveDelay = True):
        self.meta = RawTDMBlock.meta
        self.meta_is_valid = False
        self.meta_is_invalid = False
        self.data = []
        self.leapSecondData = leapSecondData
        self.bands = {"S":"1", "X":"2", "Ka":"3", "Ku":"4", "L":"5"}
        self.applyRecieveDelay = applyRecieveDelay
        #Multiple different acceptable UTC time formats in TDMs...
        self.UTC_formats =['%Y-%m-%dT%H:%M:%S', '%Y-%jT%H:%M:%S']
        self.currentUTCformat = 0
        
        self.includeTransmitDelay = False
        self.allowed_time_systems = ["UTC"]
    
    def get_time_as_datetime(self, time):
        
        
        date = None
        milliseconds = 0
        if self.meta["TIME_SYSTEM"] == "UTC":
            t = time.split(".")
            time = t[0]
            milliseconds = 0
            if len(t) == 2:
                ms = t[1]
                if ms[-1] == "Z":
                    ms = ms[:-1]
                ms = "0." + ms
                milliseconds = float(ms)
            for i in range(len(self.UTC_formats)): 
                try:
                    date = datetime.datetime.strptime(time, self.UTC_formats[self.currentUTCformat])
                except:
                    self.currentUTCformat = (self.currentUTCformat + 1) % 3
        
        if not date:
            raise Exception("ERROR: Time format error, invalid input time format for time: " + time) 
        
        date = date + datetime.timedelta(milliseconds = 0)
        
        if self.leapSecondData:
            date = date + self.leapSecondData.getLeapSecondDelta(date)

        delay = self.get_time_delay(date)

        milliseconds += delay

        m = str(milliseconds)
        msplit = m.split(".")
        s = msplit[0]
        ms = "0." + msplit[1]
        if s[0] == "-":
            ms = "-" + ms
        secs = float(s)
        milliseconds = float(ms)
        date = date + datetime.timedelta(seconds = secs)
        
        return date, milliseconds
        
    
    def convert_time_to_modjulian(self, time):
        date, milliseconds = self.get_time_as_datetime(time)
        delta = (date - TAIMJD_base)
        diffDays = delta.days
        diffHours = abs(date.hour - TAIMJD_base.hour)
        diffMins = abs(date.minute - TAIMJD_base.minute)
        diffSecs = abs(date.second - TAIMJD_base.second)
        fractDiff = 0.5 + (diffHours / 24.0) + (diffMins / 1440.0) + ((diffSecs + milliseconds) / 86400.0)
        
        return ((date - TAIMJD_base).total_seconds() + milliseconds) / 86400.0
    
    
    
    def get_time_delay(self, date):
        delay = 0.0
        
        if self.includeTransmitDelay == True:
            if "TRANSMIT_DELAY_1" in self.meta:
                delay -= float(self.meta["TRANSMIT_DELAY_1"])

        if self.applyRecieveDelay == True: 
            if "RECEIVE_DELAY_1" in self.meta:
                delay -= float(self.meta["RECEIVE_DELAY_1"])
        
        return delay
            
    def check_meta_validity(self):
        exc = ""
        if self.meta_is_invalid:
            return
        if not "START_TIME" in self.meta:
            exc += "META ERROR: Invalid meta data, all blocks must include START_TIME.\n"
        
        if not "STOP_TIME" in self.meta:
            exc += "META ERROR: Invalid meta data, all blocks must include STOP_TIME.\n"
        
        if "TIME_SYSTEM" in self.meta:
            if not self.meta["TIME_SYSTEM"] in self.allowed_time_systems:
                exc += "META ERROR: Invalid meta data, all blocks must include a valid time system.\n Allowed time systems are: " + str(self.allowed_time_systems) 
        else:
            exc += "META ERROR: Invalid meta data, all blocks must include a TIME_SYSTEM.\n" 
        
        
        if not "PARTICIPANT_1" in self.meta:
            exc += "META ERROR: Invalid meta data, all blocks must include a PARTICIPANT_1.\n"
        
        if not "PARTICIPANT_2" in self.meta:
            exc += "META ERROR: Invalid meta data, all blocks must include a PARTICIPANT_2.\n"
            
        
        if exc != "":
            raise Exception(exc)
        
    def convert_line(self, values):
        raise Exception("ERROR: Must call convert_line in child class.")

    def get_data_type(self):
        raise Exception("ERROR: Must call get_data_type in child class.")
    
    def get_block_text(self):
        return "\n".join(self.data)
        
    


   
class DopplerIntegratedBlock(TdmBlock):
    def __init__(self, RawTDMBlock, leapSecondData = None, applyRecieveDelay = True):
        super().__init__(RawTDMBlock, leapSecondData = leapSecondData, applyRecieveDelay = applyRecieveDelay)
        
        
        for line in RawTDMBlock.data:
            self.convert_line(line[1].strip().split())
        
        
    def check_meta_validity(self):
        super().check_meta_validity()
        if self.meta_is_invalid:
            return
        exc = ""
        
        metaErrorHeader = "META ERROR: Invalid meta data for data type \"" + self.get_data_type() + "\": "
        
        
        if not "INTEGRATION_INTERVAL" in self.meta:
            exc += metaErrorHeader + "\" \"INTEGRATION_INTERVAL\" is a required value.\n" 
            
        int_refs = ["START", "MIDDLE", "END"]
        
        if "INTEGRATION_REF" in self.meta:
            try:
                assert self.meta["INTEGRATION_REF"].strip() in int_refs
            except:
                exc += metaErrorHeader + "\" , \"INTEGRATION_REF\"  must be one of the following values:\n     " + str(int_refs) + "\n" 
        else:
            exc += metaErrorHeader + "\" \"INTEGRATION_REF\" is a required value.\n" 
            
            
        if "TRANSMIT_BAND" in self.meta and "RECEIVE_BAND" in self.meta:
            try:
                assert self.meta["TRANSMIT_BAND"].strip() == self.meta["RECEIVE_BAND"].strip()
                assert self.meta["TRANSMIT_BAND"].strip() in self.bands
            except:
                exc +=  metaErrorHeader + "\"TRANSMIT_BAND\" and \"RECEIVE_BAND\" must be the same and must be one of the following values:\n     " + str(self.bands.keys()) + "\n" 
        else:
            exc += metaErrorHeader + "\"TRANSMIT_BAND\" and \"RECEIVE_BAND\" are required.\n" 
        
        if not "INTEGRATION_INTERVAL" in self.meta:
            exc += metaErrorHeader +  "\"INTEGRATION_INTERVAL\" is required.\n"
        
        
        if "PATH" in self.meta:
            try:
                p = self.meta["PATH"].split(",")
                if len(p) != 3:
                    self.meta_is_invalid = True 
                    if exc == "":
                        print("WARNING: Invalid meta data for data type \"" + self.get_data_type() + "\": a path that is not 2-way was detected. This block will be ignored. \n         Path found: " + self.meta["PATH"] + "\n")
                        return
                assert p[0].strip() == "1"
                assert p[1].strip() == "2"
                assert p[2].strip() == "1"
            except:
                exc += metaErrorHeader +  "\" PATH must be \"1,2,1\".\n" 
        else:
            exc += "META ERROR: Invalid meta data, all blocks must include a PATH.\n" 
            
        
        if exc != "":
            raise Exception(exc)
            
        #If meta is valid, do this:
        self.meta_is_valid = True
        
    def get_data_type(self):
        return "DOPPLER_INTEGRATED"
        
    #23430.503148148146    RangeRate    9012    GDS    LEOSat    1    10               -11.61467029
    def convert_line(self, values):
        #Check to make sure meta data is valid, do this only once per meta block and key
        if not self.meta_is_valid:
            self.check_meta_validity()
        if self.meta_is_invalid:
            return
        #1
        epoch = self.convert_time_to_modjulian(values[0])
        out_line = "{:.20f}".format(epoch) + "    "
        #2
        out_line += "RangeRate    "
        #3 9012 for now, might add 9030 if wanted later...
        out_line += "   9012    "
        #4
        out_line += self.meta["PARTICIPANT_1"] + "    "
        #5
        out_line += self.meta["PARTICIPANT_2"] + "    "
        #6
        out_line += self.bands[self.meta["TRANSMIT_BAND"]] + "    "
        #7
        out_line += self.meta["INTEGRATION_INTERVAL"] + "    "
        #8 
        out_line += " " * (20 - len(values[1]))
        out_line += values[1]
        
        self.data.append(out_line)
        
        
    def convert_time_to_modjulian(self, epoch):
        e = super().convert_time_to_modjulian(epoch)
        if self.meta["INTEGRATION_REF"] == "END":
            return e
        elif self.meta["INTEGRATION_REF"] == "MIDDLE":
            return e + ((float(self.meta["INTEGRATION_INTERVAL"]) / 86400.0) / 2.0)
        elif self.meta["INTEGRATION_REF"] == "START":
            return e + (float(self.meta["INTEGRATION_INTERVAL"]) / 86400.0)
        else:
            # Will not occur as we check value in check_meta_validity
            raise Exception("ERROR: INTEGRATION_REF must be START, MIDDLE, or END\n")
        
    
        
        
class TransmitFreqBlock(TdmBlock):
    def __init__(self, RawTDMBlock, leapSecondData = None, applyRecieveDelay = True):
        super().__init__(RawTDMBlock, leapSecondData = leapSecondData, applyRecieveDelay = applyRecieveDelay)
        
        self.rmpRates = []
        self.freqRates = []
        self.lastCheckedEpochIndex = 0
        self.lastCheckedFreqEpochIndex = 0
        
        for line in RawTDMBlock.data:
            if line[0].strip() == "TRANSMIT_FREQ_1":
                values = line[1].strip().split()
                epoch = self.convert_time_to_modjulian(values[0])
                self.freqRates.append([epoch, float(values[1])])
            
            if line[0].strip() == "TRANSMIT_FREQ_RATE_1":
                values = line[1].strip().split()
                epoch = self.convert_time_to_modjulian(values[0])
                self.rmpRates.append([epoch, float(values[1])])
        
        for line in RawTDMBlock.data:
            if line[0].strip() == "TRANSMIT_FREQ_RATE_1":
                self.convert_line(line[0].strip(), line[1].strip().split())
        
        for line in RawTDMBlock.data:
            if line[0].strip() == "TRANSMIT_FREQ_1":
                self.convert_line(line[0].strip(), line[1].strip().split())
        
    def get_data_type(self):
        return "TRANSMIT_FREQ_1"
        
    def check_meta_validity(self):
        super().check_meta_validity()
        
        exc = ""
        
        if "TRANSMIT_BAND" in self.meta and "RECEIVE_BAND" in self.meta:
            try:
                assert self.meta["TRANSMIT_BAND"].strip() == self.meta["RECEIVE_BAND"].strip()
                assert self.meta["TRANSMIT_BAND"].strip() in self.bands
            except:
                exc += "META ERROR: Invalid meta data for data type \"" + self.get_data_type() + "\", \"TRANSMIT_BAND\" and \"RECEIVE_BAND\" must be the same and must be one of the following values:\n     " + str(self.bands.keys()) + "\n" 
        
        if "PATH" in self.meta:
            try:
                p = self.meta["PATH"].split(",")
                if len(p) == 2:
                    assert p[0].strip() == "1"
                    assert p[1].strip() == "2"
                elif len(p) == 3:
                    assert p[0].strip() == "1"
                    assert p[1].strip() == "2"
                    assert p[2].strip() == "1"
                else:
                    raise Exception("")
            except:
                exc += "META ERROR: Invalid meta data, for data type \"" + self.get_data_type() + "\" PATH must be \"1,2\" or \"1,2,1\".\n" 
        else:
            exc += "META ERROR: Invalid meta data, all blocks must include a PATH.\n" 

        
        if exc != "":
            raise Exception(exc)
            
        #If meta is valid, do this:
        self.meta_is_valid = True
    
    def convert_line(self, key, values):
        if not self.meta_is_valid:
            self.check_meta_validity()
        
        if key == "TRANSMIT_FREQ_RATE_1":
            epoch = self.convert_time_to_modjulian(values[0])
            self.rmpRates.append([epoch, float(values[1])])
        
        if key == "TRANSMIT_FREQ_1":
            #1
            epoch = self.convert_time_to_modjulian(values[0])
            out_line = "{:.20f}".format(epoch) + "    "
            #2
            out_line += self.meta["PARTICIPANT_1"] + "    "
            #3
            out_line += self.meta["PARTICIPANT_2"] + "    "
            #4
            out_line += self.bands[self.meta["TRANSMIT_BAND"]] + "    "
            #5  6 (invalid/unknown)
            out_line += "6    "
            #6
            out_line += values[1] + "    "
            #7
            out_line += str(self.getRMPRateFrequency(epoch, forPrint = True)) + "    "

            self.data.append(out_line)
            
    
    def getFrequency(self, epoch):
        return self.getFreqRateFrequency(epoch) + self.getRMPRateFrequency(epoch)
    
    def getRMPRateFrequency(self, epoch, forPrint = False):
        
        index = self.lastCheckedEpochIndex
        if index + 1 < len(self.rmpRates):
            if epoch < self.rmpRates[index + 1][0]:
                if epoch >= self.rmpRates[index][0]:
                    dt = (epoch - self.rmpRates[index][0]) * 86400
                    self.lastCheckedEpochIndex = index
                    rmpr = self.rmpRates[index][1]
                    if forPrint == True:
                        return rmpr
                    else:
                        return rmpr * dt
                else:
                    if index - 1 < 0:
                        return 0.0
                    else: 
                        self.lastCheckedEpochIndex = index - 1 
                        return self.getRMPRateFrequency(epoch, forPrint = forPrint)
            else:
                self.lastCheckedEpochIndex = index + 1
                return self.getRMPRateFrequency(epoch, forPrint = forPrint)
        
        else:
            if epoch >= self.rmpRates[index][0]:
                dt = (epoch - self.rmpRates[index][0]) * 86400
                self.lastCheckedEpochIndex = index
                rmpr = self.rmpRates[index][1]
                if forPrint == True:
                    return rmpr
                else:
                    return rmpr * dt
            else:
                self.lastCheckedEpochIndex = 0
                return self.getRMPRateFrequency(epoch, forPrint = forPrint)
                
                
    def getFreqRateFrequency(self, epoch):
        index = self.lastCheckedFreqEpochIndex
        if index + 1 < len(self.freqRates):
            if epoch < self.freqRates[index + 1][0]:
                if epoch >= self.freqRates[index][0]:
                    self.lastCheckedFreqEpochIndex = index
                    return self.freqRates[index][1]
                else:
                    if index - 1 < 0:
                        return 0.0
                    else: 
                        self.lastCheckedFreqEpochIndex = index - 1 
                        return self.getFreqRateFrequency(epoch)
            else:
                self.lastCheckedFreqEpochIndex = index + 1
                return self.getFreqRateFrequency(epoch)
        
        else:
            if epoch >= self.freqRates[index][0]:
                self.lastCheckedFreqEpochIndex = index
                return self.freqRates[index][1]
            else:
                self.lastCheckedFreqEpochIndex = 0
                return self.getFreqRateFrequency(epoch)

class ReceiveFreqBlock(TdmBlock):
    def __init__(self, RawTDMBlock, leapSecondData = None, applyRecieveDelay = True):
        super().__init__(RawTDMBlock, leapSecondData = leapSecondData, applyRecieveDelay = applyRecieveDelay)
        
        for line in RawTDMBlock.data:
            self.convert_line(line[0].strip(), line[1].strip().split())
    
    
    def check_meta_validity(self):
        if self.meta_is_invalid:
            return
        
        super().check_meta_validity()
        
        
        metaErrorHeader = "META ERROR: Invalid meta data for data type \"" + self.get_data_type() + "\": "
        
        exc = ""

        if not "TIMETAG_REF" in self.meta:
            exc += metaErrorHeader + " \"TIMETAG_REF\" is a required value.\n" 
            
        if not "INTEGRATION_INTERVAL" in self.meta:
            exc += metaErrorHeader + " \"INTEGRATION_INTERVAL\" is a required value.\n" 
            
        int_refs = ["START", "MIDDLE", "END"]
        
        if "INTEGRATION_REF" in self.meta:
            try:
                assert self.meta["INTEGRATION_REF"].strip() in int_refs
            except:
                exc += metaErrorHeader + " \"INTEGRATION_REF\"  must be one of the following values:\n     " + str(int_refs) + "\n" 
        else:
            exc += metaErrorHeader + " \"INTEGRATION_REF\" is a required value.\n" 
        
        if "TRANSMIT_BAND" in self.meta and "RECEIVE_BAND" in self.meta:
            try:
                assert self.meta["TRANSMIT_BAND"].strip() == self.meta["RECEIVE_BAND"].strip()
                assert self.meta["TRANSMIT_BAND"].strip() in self.bands
            except:
                exc += metaErrorHeader + " \"TRANSMIT_BAND\" and \"RECEIVE_BAND\" must be the same and must be one of the following values:\n     " + str(self.bands.keys()) + "\n" 
        else:
            exc += metaErrorHeader + " \"TRANSMIT_BAND\" and \"RECEIVE_BAND\" are required.\n" 
        
        if "PATH" in self.meta:
            try:
                p = self.meta["PATH"].split(",")
                if len(p) != 3:
                    self.meta_is_invalid = True
                    if exc == "":
                        print("WARNING: Invalid meta data for data type \"" + self.get_data_type() + "\": a path that is not 2-way was detected. This block will be ignored. \n         Path found: " + self.meta["PATH"]  + "\n")
                        return
                else:
                    assert p[0].strip() == "1"
                    assert p[1].strip() == "2"
                    assert p[2].strip() == "1"
            except:
                exc += metaErrorHeader + "\" PATH must be \"1,2,1\".\n" 
        else:
            exc += metaErrorHeader + " all blocks must include a PATH.\n" 
        
        
        
        if exc != "":
            raise Exception(exc)
            
        #If meta is valid, do this:
        self.meta_is_valid = True
        
        
    def convert_line(self, key, values):
        #"RECEIVE_FREQ_1"
        if not self.meta_is_valid:
            self.check_meta_validity()
        
        if self.meta_is_invalid:
            return
        
        if key == "RECEIVE_FREQ_1":
            #1
            epoch = self.convert_time_to_modjulian(values[0])
            out_line = "{:.20f}".format(epoch) + "    "
            #2
            out_line += "DSN_TCP    "
            #3 Observation type index number For now 9006 for ground to space
            out_line += "9006    "
            #4
            out_line += "{" + self.meta["PARTICIPANT_1"] + "    "
            #5
            out_line += self.meta["PARTICIPANT_2"] + "    "
            out_line += self.meta["PARTICIPANT_1"] + "}    "
            #6
            out_line += self.bands[self.meta["TRANSMIT_BAND"]] + "    "
            #7
            out_line += self.meta["INTEGRATION_INTERVAL"] + "    "
            #8
            freqOffset = 0.0
            if "FREQ_OFFSET" in self.meta:
                freqOffset = self.meta["FREQ_OFFSET"]
            out_line += "-" + str(float(values[1]) + float(freqOffset)) + "    "
            
            
            self.data.append(out_line)
        
    def get_data_type(self):
        return "RECEIVE_FREQ_1"
        
    def convert_time_to_modjulian(self, epoch):
        e = super().convert_time_to_modjulian(epoch)
        if self.meta["INTEGRATION_REF"] == "END":
            return e
        elif self.meta["INTEGRATION_REF"] == "MIDDLE":
            return e + ((float(self.meta["INTEGRATION_INTERVAL"]) / 86400) / 2)
        elif self.meta["INTEGRATION_REF"] == "START":
            return e + (float(self.meta["INTEGRATION_INTERVAL"]) / 86400)
        else:
            # Will not occur as we check value in check_meta_validity
            raise Exception("ERROR: INTEGRATION_REF must be START, MIDDLE, or END\n")
            
            
class RangeBlock(TdmBlock):
    def __init__(self, RawTDMBlock, transmitBlocks, leapSecondData = None, applyRecieveDelay = True):
        super().__init__(RawTDMBlock, leapSecondData = leapSecondData, applyRecieveDelay = applyRecieveDelay)
        self.transmitBlocks = transmitBlocks
        
        for line in RawTDMBlock.data:
            self.convert_line(line[0].strip(), line[1].strip().split())

    def get_data_type(self):
        return "RANGE"
        
    def check_meta_validity(self):
        super().check_meta_validity()
        if self.meta_is_invalid:
            return
        metaErrorHeader = "META ERROR: Invalid meta data for data type \"" + self.get_data_type() + "\": "
        
        range_units_allowed_values = ["S", "RU", "KM"]
        
        exc = ""
        
        if "RANGE_UNITS" in self.meta:
            try:
                assert self.meta["RANGE_UNITS"] in range_units_allowed_values
            except:
                exc += metaErrorHeader + " RANGE_UNITS value of " + self.meta["RANGE_UNITS"] + " is not allowed. Allowed values are: " + str(range_units_allowed_values) + "\n"
        else:
            exc += metaErrorHeader + " \"RANGE_UNITS\" is a required value.\n" 
        
        if "CORRECTIONS_APPLIED" in self.meta:
            try:
                assert self.meta["CORRECTIONS_APPLIED"] == "NO" or self.meta["CORRECTIONS_APPLIED"] == "YES"
            except:
                exc += metaErrorHeader + "\" , CORRECTIONS_APPLIED value of " + self.meta["CORRECTIONS_APPLIED"] + " is not allowed. Allowed values are: YES, NO\n" 
            
            if not "CORRECTION_RANGE" in self.meta and self.meta["CORRECTIONS_APPLIED"] == "NO":
                self.meta["CORRECTION_RANGE"] = 0
        else:
            exc += metaErrorHeader + "\" \"CORRECTIONS_APPLIED\" is a required value.\n" 
        
        if not "RANGE_MODULUS" in self.meta:
            self.meta["RANGE_MODULUS"] = 0
        elif float(self.meta["RANGE_MODULUS"]) != 0.0:
            if self.meta["RANGE_UNITS"] != "RU":
                exc += metaErrorHeader + "\" \"RANGE_MODULUS\" must be 0 if \"RANGE_UNITS\" is not \"RU\".\n" 
            #exc += metaErrorHeader + "\" \"RANGE_MODULUS\" is a required value.\n" 

        if "TRANSMIT_BAND" in self.meta and "RECEIVE_BAND" in self.meta:
            try:
                assert self.meta["TRANSMIT_BAND"].strip() == self.meta["RECEIVE_BAND"].strip()
                assert self.meta["TRANSMIT_BAND"].strip() in self.bands
            except:
                exc += metaErrorHeader + " \"TRANSMIT_BAND\" and \"RECEIVE_BAND\" must be the same and must be one of the following values:\n     " + str(self.bands.keys()) + "\n" 
        else:
            exc += metaErrorHeader + " \"TRANSMIT_BAND\" and \"RECEIVE_BAND\" are required.\n" 
        
        if "PATH" in self.meta:
            try:
                p = self.meta["PATH"].split(",")
                if len(p) != 3:
                    self.meta_is_invalid = True 
                    if exc == "":
                        print("WARNING: Invalid meta data for data type \"" + self.get_data_type() + "\": a path that is not 2-way was detected. This block will be ignored. \n          Path found: " + self.meta["PATH"] + "\n")
                        return
                assert p[0].strip() == "1"
                assert p[1].strip() == "2"
                assert p[2].strip() == "1"
            except:
                exc += metaErrorHeader + "\" PATH must be \"1,2,1\".\n" 
        else:
            exc += metaErrorHeader + " all blocks must include a PATH.\n" 
            
        
        if exc != "":
            raise Exception(exc)
            
        #If meta is valid, do this:
        self.meta_is_valid = True
        
        
    def convert_line(self, key, values):
        #RANGE
        
        if not self.meta_is_valid:
            self.check_meta_validity()
        if self.meta_is_invalid:
            return
        if key == "RANGE":
            #1
            epoch = self.convert_time_to_modjulian(values[0])
            out_line = "{:.20f}".format(epoch) + "    "
            #2 + #3
            if float(self.meta["RANGE_MODULUS"]) != 0.0:
                out_line += "DSN_SeqRange    "
                out_line += "9004    "
            else:
                out_line += "Range    "
                out_line += "       9002    "
            #4
            out_line += self.meta["PARTICIPANT_1"] + "    "
            #5
            out_line += self.meta["PARTICIPANT_2"] + "    "
            #6
            rangeVal = float(values[1])
            if self.meta["CORRECTIONS_APPLIED"] == "NO":
                rangeVal = rangeVal - float(self.meta["CORRECTION_RANGE"])
            #Convert from Seconds to KM
            if self.meta["RANGE_UNITS"] == "S":
                #Convert to km by multipling by 299792.458 km/sec
                rangeVal = rangeVal * 299792.458
            elif self.meta["RANGE_UNITS"] == "RU":
                if ("RECEIVE_DELAY_1" in self.meta or "TRANSMIT_DELAY_1" in self.meta) and float(self.meta["RANGE_MODULUS"]) != 0.0:
                    delay = 0.0
                    if "TRANSMIT_DELAY_1" in self.meta:
                        delay += float(self.meta["TRANSMIT_DELAY_1"])
                    if "RECEIVE_DELAY_1" in self.meta:
                        delay += float(self.meta["RECEIVE_DELAY_1"])
                    if delay != 0.0:
                        ft = self.get_transmit_frequency(self.convert_time_to_modjulian(values[0]))
                        k = self.getK(self.meta["TRANSMIT_BAND"])
                        #Range Observable = RANGE – RECEIVE_DELAY_1 * Ft * K
                        rangeVal = rangeVal - (delay * ft * k) 
            
            out_line += "{:.20f}".format(rangeVal) + "    "
            
            #Ambiguous range has extra values...
            if float(self.meta["RANGE_MODULUS"]) != 0.0:
                #7
                out_line += self.bands[self.meta["TRANSMIT_BAND"]] + "    "
                #8
                self.includeTransmitDelay = True
                freqVal = self.get_transmit_frequency(self.convert_time_to_modjulian(values[0]))
                out_line += "{:.12f}".format(freqVal) + "    "
                self.includeTransmitDelay = False
                #9
                out_line += self.meta["RANGE_MODULUS"] + "    "
            
            self.data.append(out_line)
            
            
            
    def getK(self, band):
        bandRatios = {"S":1/2, "X":221/1498, "Ka":1/2 , "Ku":1/2 , "L":1}
        return bandRatios[band]
    
    def get_transmit_frequency(self, epoch):
        for b in self.transmitBlocks:
            if epoch >= self.convert_time_to_modjulian(b.meta["START_TIME"]) and epoch <= self.convert_time_to_modjulian(b.meta["STOP_TIME"]):
                return b.getFrequency(epoch)
        errmsg = "ERROR: No transmit frequency available for epoch: " +  str(epoch) +  " a transmit frequency must be provided for all range data"
        raise Exception(errmsg)
        
def main():

    parser = argparse.ArgumentParser(description='TDM to GMD Converter')
    parser.add_argument('-i', '--input', required = True, type=Path, metavar='IN_PATH', dest='in_path', help='Input file')
    parser.add_argument('-o', '--output',  type=Path, metavar='OUT_PATH', dest='out_path', help='Output path')
    parser.add_argument('-l', '--LeapSecondFile',  type=Path, metavar='LEAP_PATH', dest='leap_path', help='Leap second file path')
    parser.add_argument('-apply_receive_delay_to_timetag', metavar='True|False',  dest="apply_receive_delay_to_timetag", help = "True|False Whether or not to apply the recieve delay to the time tag, this is included in observation calculations regardless. Default: True")
    
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
    
    applyRecieveDelay = args.apply_receive_delay_to_timetag
    
    if applyRecieveDelay == None:
        applyRecieveDelay = True
    elif applyRecieveDelay.lower() == "true":
        applyRecieveDelay = True
    elif applyRecieveDelay.lower() == "false":
        applyRecieveDelay = False
    else:
        print("ERROR: -apply_receive_delay_to_timetag can only be True or False")
        return


    tdm = TdmReader(args.in_path.__str__(), leapSecondPath = leapSecPath, applyRecieveDelay = applyRecieveDelay)
    tdm.read_tdm()
    
    tdm.convert_data()
    

    
    tdm.write_files(args.out_path.__str__(), args.in_path.__str__())
    
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