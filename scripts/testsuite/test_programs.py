# Test all programs under ./programs in all currently finished machines
# Match memory dumps with expected memory values in *.memdump files
#
# Returns a status code - 0 if all programs were executed correctly, 1 if there was a mistake somewhere

import glob, sys, os.path, ntpath, subprocess, struct, filecmp
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

# End of machine call definitions
# Add your machine to the dictionary below
machines = {
	"Cpp": Cpp_process
}

# Returns basic information about the memory dump file
def memdump_read_location(dumpFilename):
	f = open(dumpFilename, 'rb')
	if not f:
		return [0, 0, 0]
	addr, _ = struct.unpack('i', f.read(4))
	length, _ = struct.unpack('i', f.read(4))

	return [1, addr, length]

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
print("Will test and compare "+str(len(compTests))+" programs with dumps")

allOk = 1

print("-- Executing programs")
for name in runTests:
	print("# Testing "+name)
	for machine, execute in machines.iteritems():
		if(not execute('./programs/'+name+'.asm')):
			allOk = 0

print("-- Executing programs with memdumps")
for name in compTests:
	print("# Executing "+name)
	status, addr, length = memdump_read_location('./programs/'+name+'.memdump')
	if not status:
		print("Failed to read memdump info for this test, continuing")
		allOk = 0
		continue
	print("Mem @ "+str(addr)+" len "+str(length))

	ndumpname = name+'__result.memdump'

	for machine, execute in machines.iteritems():
		if(not execute('./programs/'+name+'.asm', ndumpname, addr, length)):
			allOk = 0
			print("Cannot compare with expected memory dump (test failed)")
		else:
			if(not filecmp.cmp(ndumpname, './programs/'+name+'.memdump')):
				print(machine+": Dump is not the same as expected !!!")
				allOk = 0
			else:
				print(machine+": Dump ok")

sys.exit(0 if allOk else 1)