#! /usr/bin/env python
"""
This script will test highgui's video reading functionality
for RAW UYVY .avi files
"""

# pixel format to check
FORMAT   = "UYVY"

# import check routine
import cvCaptureFromFile

# check video file of format FORMAT,
# the function also exits and returns
# 0,1 or 77 accordingly.

cvCaptureFromFile.video_ok(FORMAT)
