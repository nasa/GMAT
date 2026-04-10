import pandas as pd
from pandas import ExcelWriter
from pandas import ExcelFile

def readExcelToPandas(fileName, sheetName, dtype=None):
    """
    read in excel sheet to pandas data frame
    fileName Name of .xlsx file
    sheetName Name of sheet within fileName
    df The data frame
    """
    df = pd.read_excel(fileName, sheet_name = sheetName, dtype=dtype)
    return df

def getColumnsOfData(df, columnsToSave = []):
    """
    save columns of data from pandas data frame and return as list of lists
    df The pandas data frame
    columnsToSave Names of column headers to add to the list. If empty, all columns are put into list (default behavior).
    retData The list of lists
    """
    retData = []
    if columnsToSave == []:
        columns = df.columns
    else:
        columns = columnsToSave

    for column in columns:
        retData.append(df[column])
    return retData

def writeTSTTable2Column(dataToWrite, outputFile, append, headers, columnWidths, caption, label, introText):
    """
    Take in 2 lists of data (plus metadata) and write out a 2-column RST table from it.
    dataToWrite list of 2 lists. Each list contains a column of data
    outputFile The file to write to
    columnWidths List of column widths for RST table (converted to integers internally with int()) (needs to be 2 elements)
    caption Table caption
    label Table label (without the leading _)
    introText Text written to outputFile prior to writing out the table
    """
    if append == 1:
        f = open(outputFile, "a+")
    else:
        f = open(outputFile, "w+")

    # write introductory text
    f.write(introText)

    # write label
    text = '.. {0}:\n'.format(label)
    f.write(text)

    # start table and write caption
    text = '.. list-table:: {0}\n'.format(caption)
    f.write(text)

    # write table widths
    text = '    :widths:'
    f.write(text)

    for width in columnWidths:
        text = ' {0}'.format(int(width))
        f.write(text)
    f.write('\n')

    # write number of header rows (assumed to be 0)
    text = '    :header-rows: 0\n\n'
    f.write(text)

    # not writing any headers

    # write the data
    rows = len(column1)
    for i in range(rows):
        for j in range(len(dataToWrite)):
            if j == 0:
                text = '    * - '
            else:
                text = '      - '
            f.write(text)
            text = '{0}'.format(dataToWrite[j][i])
            text = text.strip() # strip leading/trailing spaces
            text = text.strip('\n') # strip leading/trailing new lines
            # find intermediate new lines
            textSplit = text.split('\n')
            text = ''
            if len(textSplit) > 1:
                for i in range(len(textSplit) - 1):
                    t = textSplit[i]
                    text = text + t + '\n\n' + '        '
                text = text + textSplit[-1]
            else:
                text = textSplit[0]
            if text == 'nan':
                # just skip it, pandas puts in nan for empty cell
                #text = '\n'
                text = 'N/A\n' # put an N/A in empty cells
            else:
                text = text + '\n'
            f.write(text)
    # skip a line
    f.write('\n')
    f.close()

    return

def writeColumnsToRST(dataToWrite, outputFile, rowStop, headers, headerWidths, caption, label, introText):
    """
    dataToWrite List of lists of columns of data to write to table (retData from getColumnsOfData)
    outputFile The file to write to
    rowStop last row to print; if rowStop > number of rows, all rows are printed
    headers List of strings of column headers
    headerWidths List of column widths for RST table (converted to integers internally with int())
    caption Table caption
    label Table label (without the leading _)
    introText Text written to outputFile prior to writing out the table
    """

    f = open(outputFile, "w")

    # write introductory text
    f.write(introText)

    # write label
    text = '.. {0}:\n'.format(label)
    f.write(text)

    # start table and write caption
    text = '.. list-table:: {0}\n'.format(caption)
    f.write(text)

    # write table widths
    text = '    :widths:'
    f.write(text)

    for width in headerWidths:
        text = ' {0}'.format(int(width))
        f.write(text)
    f.write('\n')

    # write number of header rows (assumed to be 1)
    text = '    :header-rows: 1\n\n'
    f.write(text)

    rows = min(rowStop, len(dataToWrite[0]))
    for i in range(rows):
        if i == 0:
            # write headers
            for j in range(len(dataToWrite)):
                if j == 0:
                    text = '    * - '
                else:
                    text = '      - '
                f.write(text)

                text = "{0}".format(headers[j])
                text = text.strip() # strip leading/trailing spaces
                text = text.strip('\n') # strip leading/trailing new lines

                # find intermediate new lines
                textSplit = text.split('\n')
                text = ''
                if len(textSplit) > 1:
                    for i in range(len(textSplit) - 1):
                        t = textSplit[i]
                        text = t + '\n' + '        '
                    text = text + textSplit[-1]
                else:
                    text = textSplit[0]
                if text == 'nan':
                    # just skip it, pandas put in nan for empty cell
                    #text = '\n'
                    text = 'N/A\n' # put N/A in empty cells
                else:
                    text = text + '\n'
                f.write(text)

        for j in range(len(dataToWrite)):
            if j == 0:
                text = '    * - '
            else:
                text = '      - '
            f.write(text)
            text = '{0}'.format(dataToWrite[j][i])
            text = text.strip() # strip leading/trailing spaces
            text = text.strip('\n') # strip leading/trailing new lines
            # find intermediate new lines
            textSplit = text.split('\n')
            text = ''
            if len(textSplit) > 1:
                for i in range(len(textSplit) - 1):
                    t = textSplit[i]
                    text = text + t + '\n\n' + '        '
                text = text + textSplit[-1]
            else:
                text = textSplit[0]
            if text == 'nan':
                # just skip it, pandas puts in nan for empty cell
                text = '\n'
            else:
                text = text + '\n'
            f.write(text)
    f.close()
    return


if __name__ == "__main__":
    """
    To create RST file containing table for UI Spec:
        Set tablesToWrite = 'UISpec'
        Set fileName under (tablesToWrite == 'UISpec') to be complete path to User_Interface_Spec.xlsx
        Set rowStop under (tablesToWrite == 'UISpec') to be one less than the last row of the Excel sheet you want to write
            E.g., if the last row of data you want added to the RST file is in row 82, set rowStop = 81
        Set outputFile under (writeType == 1) to be the complete path to gmat-git//doc//SystemDocs//Tools//source//CSALT//source//csalt-UserInterfaceSpec-smallTables.rst
        Execute main.
        Read notes in csalt_doc_to_latex_manual_edits.txt in order to make a PDF from RST-generated LaTeX.
        
    """
    #tablesToWrite = 'UISpec'
    tablesToWrite = 'EMTGSpacecraftSpec'

    if tablesToWrite == 'UISpec':
        fileName = 'C://GMAT//User_Interface_Spec.xlsx'
        sheetName = 'UnderDevelopment'
        writeType = 1 # 0: one enormous table; 1: a bunch of smaller tables
    
        df = readExcelToPandas(fileName, sheetName, dtype=object)

        columnsToWrite = ["Resource/\nCommand Name", "Field Name", "Data Type", 
                          "Default Value", "Allowed Values", "Description", 
                          "Notes", "Units", "Field Couplings", "Access", "Interfaces"]
        headers = ["Resource/Command Name", "Field Name", "Data Type", 
                          "Default Value", "Allowed Values", "Description", 
                          "Notes", "Units", "Field Couplings", "Access", "Interfaces"]
        retData = getColumnsOfData(df, columnsToWrite)
        rowStop = 81
        if (writeType == 0):
            outputFile = "C://GMAT//Master_OutOfSourceBuild_Repo//gmat-git//doc//SystemDocs//Tools//source//CSALT//source//csalt-UserInterfaceSpec.rst"
            numHeaders = len(headers)
            headerWidthSingle = 100./numHeaders
            headerWidths = []
            for i in range(numHeaders):
                headerWidths.append(headerWidthSingle)

            caption = 'GMAT Optimal Control User Interface Specification'
            label = '_GMATOC_UISpec'

            introText = 'User Interface Specification\n'
            introText += '===============================\n\n'
            introText += 'This section describes the GMAT Optimal Control user interface. The user interface specification is given in :numref:`{0}`.\n\n'.format(label.lstrip('_'))
            writeColumnsToRST(retData, outputFile, rowStop, headers, headerWidths, caption, label, introText)
        elif (writeType == 1):
            outputFile = "C://GMAT//Master_OutOfSourceBuild_Repo//gmat-git//doc//SystemDocs//Tools//source//CSALT//source//csalt-UserInterfaceSpec-smallTables.rst"

            # get the data we want for this particular table
            column1 = headers
        
            # loop through all data fields and create tables
            for i in range(min(len(retData[0]), rowStop)):
                column2 = []
                for j in range(len(columnsToWrite)):
                    column2.append(str(retData[j][i]).strip('\n'))
                dataToWrite = [column1, column2]
                curSection = column2[0] # the current resource name

                # capture case where the resource name is OptimalControlFunction:<Function Name>
                colonLocation = curSection.find(':')
                if colonLocation > -1:
                    # found a :, which means we need to deal with it differently
                    ocFunc = 1
                else:
                    ocFunc = 0

                if (i > 0):
                    append = 1
                    introText = '|\n\n' # inserts blanks space between tables
                else:
                    append = 0
                    introText = '.. _Sec_GMATOC_UISpec:\n\n'
                    introText += 'User Interface Specification\n'
                    introText += '===============================================\n\n'
                    introText += 'This section describes the GMAT Optimal Control user interface. The user interface specification is given in the tables in this section and are organized by Resource name.\n\n'
                    oldSection = '' # force writing subsection header

                if (curSection == oldSection):
                    # don't need to create new section
                    pass
                else:
                    # create a new section
                    if ocFunc == 1:
                        headerResource = curSection[0:colonLocation]
                        headerFunction = curSection[colonLocation+1:]
                        introText += '{0} for Function Field = {1}\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n'.format(headerResource, headerFunction)
                    else:
                        introText += '{0}\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n'.format(curSection)
                oldSection = curSection

                # always 2 columns, so columnWidths is easy
                columnWidths = [50, 50]
                if ocFunc == 1:
                    caption = 'User interface specification for {0}.{1} for {0}.Function = {2}.'.format(headerResource, column2[1], headerFunction)
                    label = '_GMATOC_UISpec_{0}_{1}_{2}'.format(headerResource, headerFunction, column2[1])
                else:
                    caption = 'User interface specification for {0}.{1}.'.format(column2[0], column2[1])
                    label = '_GMATOC_UISpec_{0}_{1}'.format(column2[0], column2[1])
            
                writeTSTTable2Column(dataToWrite, outputFile, append, headers, columnWidths, caption, label, introText)
    elif tablesToWrite == 'EMTGSpacecraftSpec':
        fileName = 'C://GMAT//EMTGSpacecraft_File_Specification.xlsx'
        sheetNames = ['Spacecraft Block', 'Stage Block', 'Power Library Block', 'Propulsion Library Block']
        rowStop = 200 # big number, write everything
        writeType = 1 # 0: one enormous table; 1: a bunch of smaller tables
        columnsToWrite = ['Line number', 'Line name', 'Variable number', 'Variable description', 'Data type', 'Units', 'Value restrictions', 'Other']
        headers = ['Line number', 'Line name', 'Variable number', 'Variable description', 'Data type', 'Units', 'Value restrictions', 'Other']
        
        if writeType == 0:
            sheetName = sheetNames[0] # need to make this a loop eventually
            df = readExcelToPandas(fileName, sheetName, dtype=object)
            retData = getColumnsOfData(df, columnsToWrite)
            outputFile = "C://GMAT//Master_OutOfSourceBuild_Repo//gmat-git//doc//SystemDocs//Tools//source//CSALT//source//csalt-EMTGSpacecraftFileSpec.rst"
            numHeaders = len(headers)
            headerWidthSingle = 100./numHeaders
            headerWidths = []
            for i in range(numHeaders):
                headerWidths.append(headerWidthSingle)

            caption = 'EMTG Spacecraft File Specification'
            label = '_GMATOC_EMTGSpacecraftFileSpec'

            introText = 'EMTG Spacecraft File Specification\n'
            introText += '===============================\n\n'
            introText += 'This section describes the contents of an EMTG spacecraft file. The description is given in :numref:`{0}`.\n\n'.format(label.lstrip('_'))
            writeColumnsToRST(retData, outputFile, rowStop, headers, headerWidths, caption, label, introText)
        elif writeType == 1:
            outputFile = "C://GMAT//Master_OutOfSourceBuild_Repo//gmat-git//doc//SystemDocs//Tools//source//CSALT//source//csalt-EMTGSpacecraftFileSpec-smallTables.rst"
            # loop through sheets
            for iSheet in range(len(sheetNames)):
                if iSheet == 0 or iSheet == 1:
                    rowStop = 200 # all the rows
                elif iSheet == 2:
                    rowStop = 17 # don't get the "repeat" lines
                elif iSheet == 3:
                    rowStop = 27 # don't get the "repeat" lines
                # header for the entire section
                if (iSheet == 0):
                    append = 0
                    introText = ''
                    introText += '.. _EMTGSpacecraftFileSpec-label:\n\n'
                    introText += 'EMTG Spacecraft File Specification\n'
                    introText += '===============================================\n\n'
                    introText += 'This section describes the contents of an EMTG spacecraft file.'
                    introText += ' The descriptions are given in the tables in this section'
                    introText += ' and are organized by the spacecraft file block, the line number within a block, and the variable number within a line.'
                    introText += ' It is highly recommended that this file specification be used in conjunction with :ref:`Sec_GMATOC_FormatOfEMTGSpacecraftFile`'
                    introText += ' and the example .emtg\_spacecraft'
                    introText += ' files provided in gmat/data/emtg/.\n\n'
                    introText += 'Several symbols and acronyms are used in the tables in this section: :math:`P` is power;'
                    introText += ' :math:`P_0` is base power delivered by a solar array;'
                    introText += ' :math:`r_s` is the distance from the spacecraft to the Sun;'
                    introText += ' Isp is specific impulse; CSI is constant specific impulse; and VSI is variable specific impulse.\n\n'
                    oldSection = '' # force writing subsection header
                else:
                    append = 1
                    introText = '|\n\n' # inserts blanks space between tables

                sheetName = sheetNames[iSheet]
                df = readExcelToPandas(fileName, sheetName, dtype=object)
                retData = getColumnsOfData(df, columnsToWrite)
                curSection = sheetName # the current sheet name

                # get the data we want for this particular table
                column1 = headers
        
                # loop through all data fields and create tables
                for i in range(min(len(retData[0]), rowStop)):
                    column2 = []
                    for j in range(len(columnsToWrite)):
                        column2.append(str(retData[j][i]).strip('\n'))
                    dataToWrite = [column1, column2]
                    
                    if (iSheet == 0 and i == 0):
                        append = 0
                    else:
                        append = 1
                    if (i > 0):
                        introText = '|\n\n' # inserts blanks space between tables

                    if (curSection == oldSection):
                        # don't need to create new section
                        pass
                    else:
                        # create a new section
                        introText += '{0}\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n'.format(curSection)
                        introText += 'This section describes the contents of the {0} of an EMTG spacecraft file.\n\n'.format(curSection)
                        if (curSection == 'Power Library Block'):
                            introText += '\n\nMultiple power systems may be added to the power library by adding'
                            introText += ' additional lines that contain all elements specified in line 1.\n\n'
                        elif (curSection == 'Propulsion Library Block'):
                            introText += '\n\nMultiple propulsion systems may be added to the propulsion library by adding'
                            introText += ' additional lines that contain all elements specified in line 1.\n\n'
                    oldSection = curSection

                    # always 2 columns, so columnWidths is easy
                    columnWidths = [50, 50]
                    caption = 'EMTG spacecraft file specification for {0}, line {1}, entry {2}.'.format(sheetName, column2[0], column2[2])
                    label = '_GMATOC_EMTGSpacecraftFileSpec_Block_{0}_Line{1}_Entry{2}'.format(sheetName.split(' ')[0], column2[0], column2[2])
            
                    writeTSTTable2Column(dataToWrite, outputFile, append, headers, columnWidths, caption, label, introText)

    print('done')

