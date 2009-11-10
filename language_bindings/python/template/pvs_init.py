#!/usr/bin/python
# pvs_init.py pvserver template 
pvslib_path = '/opt/pvb/language_bindings/python/mt'
pvs_path    = '.'
#################################################
print 'pvs_init.py starting'
#import os, sys, time, string
import sys
sys.path.insert(0, pvslib_path)
sys.path.insert(0, pvs_path)
