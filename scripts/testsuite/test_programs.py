# Test all programs under ./programs in all currently finished machines
# Match memory dumps with expected memory values in *.memdump files
#
# Returns a status code - 0 if all programs were executed correctly, 1 if there was a mistake somewhere

import glob, sys, os.path, ntpath, subprocess
DEBUG = 'DEBUG=1' in sys.argv
DEBUG_SUFFIX = '_debug' if DEBUG else ''
# Assume root dir
path = './programs/'

# Machine call definitions, return status code
def Cpp_process(appPath, dumpTargetPath='', dumpAddress=0, dumpLength=0):
	p = subprocess.Popen(['machines/Cpp/bin/c_machine'+DEBUG_SUFFIX], stdout=subprocess.PIPE)
	p.wait()
	status = "OK" if p.returncode == 0 else "FAILED !!!" 
	print("Cpp"+DEBUG_SUFFIX+": "+status)
	if p.returncode != 0:
		print("Output was:")
		print(p.stdout.read())
	return p.returncode == 0

machines = {
	"Cpp": Cpp_process
}

runTests = []	# Tests to just run
compTests = []	# Tests to compare memdumps

for filename in glob.glob(path+"*.asm"):
	name, ext = os.path.splitext(ntpath.basename(filename))
	runTests.append(name)

for filename in glob.glob(path+"*.memdump"):
	name, ext = os.path.splitext(ntpath.basename(filename))
	if name in runTests:
		compTests.append(name)
		runTests.remove(name)

print("Will test "+str(len(runTests))+" programs")
print("Will "+str(len(compTests))+" dumps")

allOk = 1

print("-- Executing programs")
for name in runTests:
	print("# Testing "+name)
	for machine, execute in machines.iteritems():
		if(not execute('./programs/'+name+'.asm')):
			allOk = 0

sys.exit(0 if allOk else 1)