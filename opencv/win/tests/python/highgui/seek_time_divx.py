#! /usr/bin/env python

"""
This script checks HighGUI's time seeking functionality
on a DivX-compressed .avi file.
"""

# name if this test and it's requirements
TESTNAME = "seek_time_divx"
REQUIRED = []
ERRORS=[0.084,0.081,0.085,0.083,0.085,0.083,0.085,0.085,0.084,0.084,0.087,0.086,0.088,0.086,0.087,0.086,0.086,0.086,0.087,0.086,0.089,0.087,0.090,0.089,0.089,0.088,0.089,0.089,0.089]

# needed for sys.exit(int), .works file handling and check routine
import sys
import works
import seek_test

# check requirements and delete old flag file, if it exists
if not works.check_files(REQUIRED,TESTNAME):
	sys.exit(77)

# name of file we check here
FILENAME='divx.avi'

# run check routine
result=seek_test.seek_time_ok(FILENAME,ERRORS)

# create flag file for following tests
works.set_file(TESTNAME)

 # return result of test routine
sys.exit(result)
