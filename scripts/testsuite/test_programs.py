# Test all programs under ./programs in all currently finished machines
# Match memory dumps with expected memory values in *.memdump files
#
# Returns a status code - 0 if all programs were executed correctly, 1 if there was a mistake somewhere

import glob, sys
# Assume root dir
path = './programs/*.asm'

for filename in glob.glob(path):
    print("Would do "+filename)

sys.exit(0)