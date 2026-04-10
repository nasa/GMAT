import os
import sys
import tarfile
import struct
import platform

cspice_version = 'N0067'
swig_version = '4.4.1'
pcre_version = '10.47'
java_version = '11.0.5'
java_update = '10'
wx_build = True
wx_version = '3.2.9'
xerces_version = '3.2.2'
osx_min_version = '14.5'

#Change this to your Visual Studio version year and number. 
#Example VisualStudio 17 2022 is vs_version = 2022, vs_major_version = '17'
vs_version = 2022
vs_major_version = '17'

vc_major_version = '14'
vc_minor_version = '1'

gmat_path = os.path.dirname(os.path.dirname(os.path.abspath(__file__))) #Path to gmat folder
depends_path = gmat_path + '/depends'	#Path to depends folder
logs_path = depends_path + '/logs'	#Path to depends/logs folder

# Create path variables
bin_path = 		depends_path + '/bin'
f2c_path = 		depends_path + '/f2c'
cspice_path = 		depends_path + '/cspice'
swig_path = 		depends_path + '/swig'
java_path = 		depends_path + '/java'
wxWidgets_path = 	depends_path + '/wxWidgets'
xerces_path = 		depends_path + '/xerces'
sofa_path = 		depends_path + '/sofa'
tsplot_path = 		depends_path + '/tsPlot'

if sys.platform == 'darwin':
	cspice_path = cspice_path + '/macosx'
	java_path = java_path + '/macosx'
elif sys.platform == 'win32':
	cspice_path = cspice_path + '/windows'
	java_path = java_path + '/windows'
else:
	cspice_path = cspice_path + '/linux'
	java_path = java_path + '/linux'

# Set up filenames for downloaded files
pcre_filename = 'pcre2-' + pcre_version + '.tar.gz'

# Get number of cores for multithreaded compilation
nCores = str(os.cpu_count())
if nCores == 'None':
	nCores = '1'

# Load the Visual Studio path settings
def setup_windows():
	# 64-bit; change to x86 for 32-bit
	vs_arch = 'x86_amd64'
	vs_tools = 'vs' + vc_major_version + '0comntools'
	if vs_version >= 2022:
		vs_path_base = os.getenv("ProgramFiles") + '\\Microsoft Visual Studio\\' + str(vs_version)

		# Check which edition of Visual Studio is installed
		if os.path.exists(vs_path_base + '\\Enterprise'):
			vs_path = vs_path_base + '\\Enterprise\\Common7\\Tools'
		elif os.path.exists(vs_path_base + '\\Professional'):
			vs_path = vs_path_base + '\\Professional\\Common7\\Tools'
		elif os.path.exists(vs_path_base + '\\Community'):
			vs_path = vs_path_base + '\\Community\\Common7\\Tools'
		elif os.path.exists(vs_path_base + '\\WDExpress'):
			vs_path = vs_path_base + '\\WDExpress\\Common7\\Tools'
		else:
			sys.exit("Could not find suitable Visual Studio development environment.")
    
	if vs_version == 2017:
		vs_path_base = os.getenv("ProgramFiles(x86)") + '\\Microsoft Visual Studio\\' + str(vs_version)

		# Check which edition of Visual Studio is installed
		if os.path.exists(vs_path_base + '\\Enterprise'):
			vs_path = vs_path_base + '\\Enterprise\\Common7\\Tools'
		elif os.path.exists(vs_path_base + '\\Professional'):
			vs_path = vs_path_base + '\\Professional\\Common7\\Tools'
		elif os.path.exists(vs_path_base + '\\Community'):
			vs_path = vs_path_base + '\\Community\\Common7\\Tools'
		elif os.path.exists(vs_path_base + '\\WDExpress'):
			vs_path = vs_path_base + '\\WDExpress\\Common7\\Tools'
		else:
			sys.exit("Could not find suitable Visual Studio development environment.")
            
	syscall = vs_path + '\\..\\..\\VC\\Auxiliary\\Build\\vcvarsall.bat'
	if vs_version <= 2015:
		vs_path = vs_path = os.getenv(vs_tools)
		syscall = vs_path + '\\..\\..\\VC\\vcvarsall.bat'
	print('\"' + syscall + '\"' + " " + vs_arch + ' & set > vsEnvironment.txt')
	os.system('\"' + syscall + '\"' + " " + vs_arch + ' & set > vsEnvironment.txt')
	print('\"' + syscall + '\"' + " " + vs_arch + ' & set > vsEnvironment.txt')
	
	# Now parse the VC environment
	f = open('vsEnvironment.txt', 'r')
	env = f.read().splitlines()
	f.close()

	for line in env:
		pair = line.split('=', 1)
		# print ("--> Setting " + pair[0] + " to " + pair[1])
		os.environ[pair[0]] = pair[1]

	# and delete the temporary settings file
	os.remove('vsEnvironment.txt')

	# Add Cmake to path
	sys.path.append('C:/Program Files/CMake/bin')

def download_depends():
	wx_build = True
	# Download xerces if it doesn't already exist
	if not os.path.exists(xerces_path):
		os.chdir(depends_path)

		# Download and extract xerces
		print('\nDownloading Xerces-C ' + xerces_version + '...')
		os.system('curl https://archive.apache.org/dist/xerces/c/3/sources/xerces-c-' + xerces_version + '.tar.gz > xerces.tar.gz')
		tar = tarfile.open('xerces.tar.gz', 'r:gz')
		tar.extractall()
		tar.close()
		os.remove('xerces.tar.gz')

		# Rename the extracted xerces directory to be the proper path
		os.rename(depends_path + '/xerces-c-' + xerces_version, xerces_path)

	# Create directories and download wxWidgets if needed
	if not os.path.exists(wxWidgets_path + '/wxWidgets-' + wx_version) and wx_build:
		# Create & change directories
		if not os.path.exists(wxWidgets_path):
			os.mkdir(wxWidgets_path)
		os.chdir(wxWidgets_path)
	
		# Download wxWidgets source
		print('\nDownloading wxWidgets ' + wx_version + '...')
		os.system('curl -L https://github.com/wxWidgets/wxWidgets/releases/download/v' + wx_version + '/wxWidgets-' + wx_version + '.tar.bz2 > wxWidgets.tar.bz2')
		tar = tarfile.open('wxWidgets.tar.bz2', 'r:bz2') 
		tar.extractall()
		tar.close()
		os.remove('wxWidgets.tar.bz2')
	
		# Make sure wxWidgets was downloaded
		if not os.path.exists(wxWidgets_path + '/wxWidgets-' + wx_version):
			print('Error in wxWidgets-' + wx_version + ' download.')
			wx_build = False

	if not os.path.exists(cspice_path):
		# Create & change directories
		os.makedirs(depends_path + '/cspice', exist_ok=True)
		os.makedirs(cspice_path, exist_ok=True)
		os.chdir(cspice_path)

		if sys.platform == 'darwin':
			cspice_type = 'MacIntel_OSX_AppleC'
			if platform.machine() ==  'arm64':
				# https://naif.jpl.nasa.gov/pub/naif/misc/toolkit_N0067/C/MacM1_OSX_clang_64bit/packages
				cspice_type = 'MacM1_OSX_clang'
			# TODO: Will remove the line below after verifying the processor type.
			print("%s> cspice_type: %s <%s" %  ("-"*15,cspice_type,"-"*15))
		else:
			cspice_type = 'PC_Linux_GCC'
			
		# Determine 32 or 64 bit system
		if struct.calcsize("P") * 8 == 32:
			cspice_dir = 'cspice32'
			cspice_bit = '32bit'
		else:
			cspice_dir = 'cspice64'
			cspice_bit = '64bit'
			
		print('\nDownloading ' + cspice_bit + ' CSPICE ' + cspice_version + '...')
		if sys.platform == 'win32':
			# Download and extract Spice for Windows (32/64-bit)
			os.system('curl https://naif.jpl.nasa.gov/pub/naif/misc/toolkit_' + cspice_version + '/C/PC_Windows_VisualC_' + cspice_bit + '/packages/cspice.zip > cspice.zip')
			os.system(depends_path + '/bin/7za/7za.exe x cspice.zip > nul') 
			os.rename('cspice', cspice_dir)
			os.remove('cspice.zip')
		else:
			# Download and extract Spice for Mac/Linux (32/64-bit)
			os.system('curl https://naif.jpl.nasa.gov/pub/naif/misc/toolkit_"' + cspice_version + '"/C/"' + cspice_type + '"_' + cspice_bit + '/packages/cspice.tar.Z > cspice.tar.Z')
			os.system('gzip -d cspice.tar.Z')
			os.system('tar -xf cspice.tar')
			os.system('mv cspice ' + cspice_dir)
			os.remove('cspice.tar')

	if sys.platform == 'win32':
		swig_dir = swig_path + '/swigwin'
	else:
		swig_dir = swig_path + '/swig'

	if not os.path.exists(swig_dir):
		# Create & change directories
		os.makedirs(depends_path + '/swig', exist_ok=True)
		os.makedirs(swig_path, exist_ok=True)
		os.chdir(swig_path)

		print('\nDownloading SWIG ' + swig_version + '...')
		if sys.platform == 'win32':
			# Download and extract SWIG for Windows
			os.system('curl -L http://download.sourceforge.net/swig/swigwin-' + swig_version + '.zip > swig.zip')
			os.system(depends_path + '/bin/7za/7za.exe x swig.zip > nul')
			os.rename('swigwin-' + swig_version, 'swigwin')
			os.remove('swig.zip')
		else:
			# Download and extract SWIG for Mac/Linux
			os.system('curl -L http://download.sourceforge.net/swig/swig-' + swig_version + '.tar.gz > swig.tar.gz')
			os.system('gzip -d swig.tar.gz')
			os.system('tar -xf swig.tar')
			os.system('mv swig-' + swig_version + ' swig')
			os.remove('swig.tar')

			# [GMT-6892] Download PCRE2 into SWIG directory
			print('\nDownloading PCRE2 ' + pcre_version + ' for use with SWIG...')
			os.chdir(swig_dir)
			os.system('curl -L https://github.com/PCRE2Project/pcre2/releases/download/pcre2-' + pcre_version + '/' + pcre_filename + ' > ' + pcre_filename)

	if not os.path.exists(java_path):
		# Create & change directories
		os.makedirs(java_path, exist_ok=True)
		os.chdir(java_path)

		java_major_version = java_version.split('.')[0]
		java_full_version = java_version + '+' + java_update

		if sys.platform == 'darwin':
			java_os_name = 'mac'
		elif sys.platform == 'win32':
			java_os_name = 'windows'
		else:
			java_os_name = 'linux'

		java_base_url = 'https://github.com/AdoptOpenJDK/openjdk' + java_major_version + '-binaries/releases/download/jdk-' + java_full_version + '/'
		java_url = java_base_url + 'OpenJDK' + java_major_version + 'U-jdk_x64_' + java_os_name + '_hotspot_' + java_version + '_' + java_update

		print('\nDownloading Java JDK ' + java_full_version + '...')
		if sys.platform == 'win32':
			# Download and extract AdoptOpenJDK for Windows
			os.system('curl -L ' + java_url + '.zip > jdk.zip')
			os.system(depends_path + '/bin/7za/7za.exe x jdk.zip > nul')
			os.rename('jdk-' + java_full_version, 'jdk')
			os.remove('jdk.zip')
		else:
			# Download and extract AdoptOpenJDK for Mac/Linux
			os.system('curl -L ' + java_url + '.tar.gz > jdk.tar.gz')
			os.system('gzip -d jdk.tar.gz')
			os.system('tar -xf jdk.tar')
			os.system('mv jdk-' + java_full_version + ' jdk')
			os.remove('jdk.tar')


def build_xerces():
	print('\n********** Configuring Xerces-C++ ' + xerces_version + ' **********')

	# Windows-specific build
	if sys.platform == 'win32':
		xerces_outdir = xerces_path + '/windows-install'
		xerces_arch = 'x64'
		
		# Build Xerces if the directory doesn't already exist
		if not os.path.exists(xerces_outdir):
			os.makedirs(xerces_path + '/build/windows', exist_ok=True)
			os.chdir(xerces_path + '/build/windows')
			os.system('cmake -G "Visual Studio ' + vs_major_version + ' ' + str(vs_version) + '" ' + xerces_arch +' -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DBUILD_SHARED_LIBS:BOOL=OFF -Dtranscoder=windows -DCMAKE_INSTALL_PREFIX="' + xerces_outdir + '" "' + xerces_path + '"  > ' + logs_path + '\\xerces_cmake.log 2>&1')

			print('-- Compiling debug Xerces. This could take a while...')
			os.system('cmake --build . --config Debug --target install > ' + logs_path + '\\xerces_build_debug.log 2>&1')
			
			print('-- Compiling release Xerces. This could take a while...')
			os.system('cmake --build . --config Release --target install > ' + logs_path + '\\xerces_build_release.log 2>&1')
		else:
			print('-- Xerces already configured')
		return
		
	# Out-of-source xerces build/install locations
	if sys.platform == 'darwin':
		xerces_build_path = xerces_path + '/cocoa-build'
		xerces_install_path = xerces_path + '/cocoa-install'
	else:
		xerces_build_path = xerces_path + '/linux-build'
		xerces_install_path = xerces_path + '/linux-install'

	# Find a test file to check if xerces has already been installed
	xerces_test_file = xerces_install_path + '/lib/libxerces-c.a'

	# Build xerces if the test file doesn't already exist
	if os.path.exists(xerces_test_file):
		print('Xerces ' + xerces_version + ' already configured')
	else:
		os.mkdir(xerces_build_path)
		os.chdir(xerces_build_path)

		# For users who compile GMAT on multiple platforms side-by-side.
		# Running Windows configure.bat causes Mac/Linux configure scripts
		# to have missing permissions.
		os.system('chmod u+x ../configure')
		os.system('chmod u+x ../config/*')

		if sys.platform == 'darwin':
			# Xerces needs these flags on OSX
			OSXFLAGS = '-mmacosx-version-min=' + osx_min_version + ' -arch x86_64 -arch arm64'
			OSXLDFLAGS = ' LDFLAGS="-arch x86_64 -arch arm64" '
		else:
			OSXFLAGS = ''
			OSXLDFLAGS = ''

		COMMONXERCESFLAGS = '--disable-shared --disable-netaccessor-curl --disable-transcoder-icu --disable-msgloader-icu'

		print('Configuring Xerces ' + xerces_version + ' debug library. This could take a while...')
		COMMONCFLAGS = '-O0 -g -fPIC ' + OSXFLAGS
		os.system('../configure ' + COMMONXERCESFLAGS + OSXLDFLAGS + ' CFLAGS="' + COMMONCFLAGS + '" CXXFLAGS="' + COMMONCFLAGS + '" --prefix="' + xerces_install_path + '" > "' + logs_path + '/xerces_configure_debug.log" 2>&1')

		makeFlag = os.system('make -j' + nCores + ' > "' + logs_path + '/xerces_build_debug.log" 2>&1')
		if makeFlag == 0:
			os.system('make install > "' + logs_path + '/xerces_install_debug.log" 2>&1')
			os.rename(xerces_install_path + '/lib/libxerces-c.a', xerces_install_path + '/lib/libxerces-cd.a')
			os.system('make clean > /dev/null 2>&1')
		else:
			print('Xerces debug build failed. Fix errors and try again.')
			return

		print('Configuring Xerces ' + xerces_version + ' release library. This could take a while...')
		COMMONCFLAGS = '-O2 -fPIC ' + OSXFLAGS
		os.system('../configure ' + COMMONXERCESFLAGS + ' CFLAGS="' + COMMONCFLAGS + '" CXXFLAGS="' + COMMONCFLAGS + '" --prefix="' + xerces_install_path + '" > "' + logs_path + '/xerces_configure_release.log" 2>&1')

		makeFlag = os.system('make -j' + nCores + ' > "' + logs_path + '/xerces_build_release.log" 2>&1')
		if makeFlag == 0:
			os.system('make install > "' + logs_path + '/xerces_install_release.log" 2>&1')
			os.chdir('..')
			os.system('rm -Rf ' + xerces_build_path)
		else:
			print('Xerces release build failed. Fix errors and try again.')
			return

def build_wxWidgets():
	print('\n********** Configuring wxWidgets ' + wx_version + ' **********')

	# Windows-specific build
	if sys.platform == 'win32':
		# Determine 32 or 64 bit system
		if struct.calcsize("P") * 8 == 32:
			wxtype = '_'
			tgtcpu = ''
		else:
			wxtype = '_x64_'
			tgtcpu = 'TARGET_CPU=X64'
			
		# Generate filenames to download
		wx_path = 'wxWidgets/wxWidgets-' + wx_version
		
		# Download wxWidgets files if they don't already exist
		if not os.path.exists(wxWidgets_path + '/wxWidgets-' + wx_version + '/lib/vc' + wxtype + 'dll'):
			os.makedirs(wxWidgets_path + '/wxWidgets-' + wx_version, exist_ok=True)
			os.chdir(wxWidgets_path + '/wxWidgets-' + wx_version)
			
			os.chdir('build/msw')
			
			print('-- Compiling debug wxWidgets. This could take a while...')
			os.system('nmake -f makefile.vc OFFICIAL_BUILD=1 COMPILER_VERSION=' + vc_major_version + vc_minor_version + ' ' + tgtcpu + ' SHARED=1 BUILD=debug > ' + logs_path + '\\wxWidgets_build_debug.log 2>&1')
			
			print('-- Compiling release wxWidgets. This could take a while...')
			os.system('nmake -f makefile.vc OFFICIAL_BUILD=1 COMPILER_VERSION=' + vc_major_version + vc_minor_version + ' ' + tgtcpu + ' SHARED=1 BUILD=release > ' + logs_path + '\\wxWidgets_build_release.log 2>&1')
			
			os.chdir('../..')
			
			os.chdir('lib')
			os.rename('vc' + vc_major_version + vc_minor_version + wxtype + 'dll', 'vc' + wxtype + 'dll')
			
			os.chdir(depends_path)
			
			return
		else:
			print('-- wxWidgets already configured')
			return
	else:		
		# Set build path based on version
		wx_path = wxWidgets_path + '/wxWidgets-' + wx_version
	
		# OS-specific vars
		if sys.platform == 'darwin':
			wx_build_path = wx_path + '/cocoa-build'
			wx_install_path = wx_path + '/cocoa-install'

		else:
			wx_build_path = wx_path + '/gtk-build'
			wx_install_path = wx_path + '/gtk-install'
		
		# Find a test file to see if wxWidgets has already been configured
		wx_test_file = wx_install_path + '/bin/wx-config'
	
		# Build wxWidgets if the test file doesn't already exist
		# Note that according to 
		#   http://docs.wxwidgets.org/3.0/overview_debugging.html
		# debugging features "are always available by default", so
		# we don't build a separate debug version here.
		# IF a debug version is required in the future, then this
		# if/else block should be repeated with the --enable-debug flag
		# added to mac & linux versions of the wx ./configure command
		if os.path.exists(wx_test_file):
			print('wxWidgets ' + wx_version + ' already configured')
		else:
			os.makedirs(wx_build_path, exist_ok=True)
			os.chdir(wx_build_path)
	
			print('Configuring wxWidgets ' + wx_version + '. This could take a while...')
	
			if sys.platform == 'darwin':
				# wxWidgets needs these flags on OSX
				# NOTE on liblzma: The Mac build/test machine contains liblzma (via homebrew 'xz'), which conflicts with the wxWidgets build process
				OSXFLAGS = '--enable-universal_binary=x86_64,arm64 --with-osx_cocoa --without-liblzma --with-macosx-version-min=' + osx_min_version
			else:
				OSXFLAGS = ''
	
			os.system('../configure ' + OSXFLAGS + ' --enable-unicode --with-opengl --prefix="' + wx_install_path + '" > "' + logs_path + '/wxWidgets_configure.log" 2>&1')
	
			# Compile, install, and clean wxWidgets
			makeFlag = os.system('make -j' + nCores + ' > "' + logs_path + '/wxWidgets_build.log" 2>&1')
	
			if makeFlag == 0:
				os.system('make install > "' + logs_path + '/wxWidgets_install.log" 2>&1')
				os.chdir('..')
				os.system('rm -Rf "' + wx_build_path + '"')
			else:
				print('wxWidgets build failed. Fix errors and try again.')
				return

def build_cspice():
	print('\n********** Configuring CSPICE **********')

	# Windows-specific build
	if sys.platform == 'win32':
		cspice_type = 'PC_Linux_GCC'
			
		# Determine 32 or 64 bit system
		if struct.calcsize("P") * 8 == 32:
			cspice_dir = 'cspice32'
			cspice_bit = '32bit'
		else:
			cspice_dir = 'cspice64'
			cspice_bit = '64bit'
		
		# Build CSPICE if cspiced.lib does not already exist
		if not os.path.exists(cspice_path + '/' + cspice_dir + '/lib/cspiced.lib'):
			os.chdir(cspice_path + '/' + cspice_dir + '/src/cspice')
			print('-- Compiling debug CSPICE. This could take a while...')
			os.system('cl /c /DEBUG /Z7 /MP -D_COMPLEX_DEFINED -DMSDOS -DOMIT_BLANK_CC -DNON_ANSI_STDIO -DUIOLEN_int *.c > ' + logs_path + '\cspice_build_debug.log 2>&1')
			os.system('link -lib /out:..\..\lib\cspiced.lib *.obj >> ' + logs_path + '\cspice_build_debug.log 2>&1')
			
			os.system('del *.obj')
			
			print('-- Compiling release CSPICE. This could take a while...')
			os.system('cl /c /O2 /MP -D_COMPLEX_DEFINED -DMSDOS -DOMIT_BLANK_CC -DNON_ANSI_STDIO -DUIOLEN_int *.c > ' + logs_path + '\cspice_build_release.log 2>&1')
			os.system('link -lib /out:..\..\lib\cspice.lib *.obj >> ' + logs_path + '\cspice_build_release.log 2>&1')
			
			os.system('del *.obj')
			
			os.chdir(depends_path)
		else:
			print('-- CSPICE already configured')
			
		return
		
	if struct.calcsize("P") * 8 == 32:
		spice_path = cspice_path + '/cspice32'
		TKCOMPILEARCH = '-m32'
		os.system('export TKCOMPILEARCH="' + TKCOMPILEARCH + '"')
	else:
		spice_path = cspice_path + '/cspice64'
		TKCOMPILEARCH = '-m64'
		os.system('export TKCOMPILEARCH="' + TKCOMPILEARCH + '"')

	if sys.platform == 'darwin':
		OSXFLAGS = '-mmacosx-version-min=' + osx_min_version + ' -Wno-error=implicit-function-declaration -arch x86_64 -arch arm64' 
	else:
		OSXFLAGS = ''

	cspice_test_file = spice_path + '/lib/cspiced.a'

	if os.path.exists(cspice_test_file):
		print('CSPICE already configured')
	else:
		os.chdir(spice_path + '/src/cspice')

		#Compile debug CSPICE with integer uiolen [GMT-5044]
		print('Compiling CSPICE debug library. This could take a while...')
		os.environ["TKCOMPILEOPTIONS"] = TKCOMPILEARCH + ' -c -ansi ' + OSXFLAGS + ' -g -fPIC -DNON_UNIX_STDIO -DUIOLEN_int'
		makeFlag = os.system('./mkprodct.csh > "' + logs_path + '/cspice_build_debug.log" 2>&1')

		if makeFlag == 0:
			os.system('mv ../../lib/cspice.a ../../lib/cspiced.a')
		else:
			print('CSPICE debug build failed. Fix errors and try again.')

		#Compile release CSPICE with integer uiolen [GMT-5044]
		print('Compiling CSPICE release library. This could take a while...')
		os.environ["TKCOMPILEOPTIONS"] = TKCOMPILEARCH + ' -c -ansi ' + OSXFLAGS + ' -O2 -fPIC -DNON_UNIX_STDIO -DUIOLEN_int'
		makeFlag = os.system('./mkprodct.csh > "' + logs_path + '/cspice_build_release.log" 2>&1')

		if makeFlag != 0:
			print('CSPICE release build failed. Fix errors and try again.')

def build_swig():
	# Windows is pre-built
	if sys.platform == 'win32':
		return

	print('\n********** Configuring SWIG **********')

	# Out-of-source SWIG build/install locations
	swig_dir = swig_path + '/swig'
	if sys.platform == 'darwin':
		swig_build_path = swig_dir + '/cocoa-build'
		swig_install_path = swig_dir + '/cocoa-install'
	else:
		swig_build_path = swig_dir + '/linux-build'
		swig_install_path = swig_dir + '/linux-install'

	# Find a test file to check if SWIG has already been installed
	swig_test_file = swig_install_path + '/bin/swig'

	# Build SWIG if the test file doesn't already exist
	if os.path.exists(swig_test_file):
		print('SWIG ' + swig_version + ' already configured')
	else:
		os.makedirs(swig_build_path, exist_ok=True)
		os.chdir(swig_build_path)
		
		# [GMT-6892] Build static PCRE using SWIG-provided build script
		os.rename('../' + pcre_filename, './' + pcre_filename)
		os.system('../Tools/pcre-build.sh > "' + logs_path + '/pcre_build.log" 2>&1')

		# For users who compile GMAT on multiple platforms side-by-side.
		# Running Windows configure.bat causes Mac/Linux configure scripts
		# to have missing permissions.
		os.system('chmod u+x ../configure')

		print('Configuring SWIG ' + swig_version + ' tool. This could take a while...')
		os.system('../configure --prefix="' + swig_install_path + '" > "' + logs_path + '/swig_configure.log" 2>&1')

		makeFlag = os.system('make -j' + nCores + ' > "' + logs_path + '/swig_build.log" 2>&1')
		if makeFlag == 0:
			os.system('make install > "' + logs_path + '/swig_install.log" 2>&1')
			os.chdir('..')
			os.system('rm -Rf ' + swig_build_path)
		else:
			print('SWIG build failed. Fix errors and try again.')
			return

print('\n*** Configuring GMAT dependencies ***\n')

# Create log directory
if not os.path.exists(logs_path):
	os.mkdir(logs_path)

if sys.platform == 'win32':
	setup_windows()

download_depends()
build_xerces()
if wx_build:
	build_wxWidgets()
build_cspice()
build_swig()

print('\n*** Done configuring GMAT dependencies, please see status messages and logs for details ***\n')
