#!/usr/bin/python3

import sys
if sys.version_info < (3, 0):
    sys.stdout.write("Sorry, requires Python 3.x - you have " + sys.version)
    sys.exit(1)

import subprocess

#Get remotes string from git
remotesString = subprocess.check_output('git branch -r', shell=True, universal_newlines=True)

#split into lines
remotesWithSpaces = remotesString.split('\n')

#remove the leading spaces
remotes = []
for r in remotesWithSpaces:
    remotes.append(r.lstrip())
    
#iterate list, branch and track so wanted
for remote in remotes:
    localList = remote.split('origin/')
    if len(localList) > 1:
        local = localList[1]
        if not local.startswith(('HEAD', 'master')):
            subprocess.check_call('git branch --track ' + local + ' origin/' + local, shell=True)

