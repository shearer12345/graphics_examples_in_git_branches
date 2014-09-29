#!/usr/bin/python3

import sys
if sys.version_info < (3, 0):
    sys.stdout.write("Sorry, requires Python 3.x - you have " + sys.version)
    sys.exit(1)

import subprocess

#checkout master
subprocess.check_call('git checkout master')

#Get remotes string and local string from git
remotesString = subprocess.check_output('git branch -r', shell=True, universal_newlines=True)
localsString = subprocess.check_output('git branch -l', shell=True, universal_newlines=True)

#split into lines
remotesWithSpaces = remotesString.split('\n')
localsWithSpaces = localsString.split('\n')

#remove the leading spaces
remotes = []
for r in remotesWithSpaces:
    remotes.append(r.lstrip())

locals= []
for l in localsWithSpaces:
    t = l.lstrip().lstrip('*').lstrip()
    if len(t) > 0:
        locals.append(t)

#iterate list, branch and track so wanted
for remote in remotes:
    localsToAddList = remote.split('origin/')
    if len(localsToAddList) > 1:
        toAdd = localsToAddList[1]
        if not toAdd.startswith(('HEAD', 'master')):
            if not toAdd.startswith(tuple(locals)):
                subprocess.check_call('git branch --track ' + toAdd  + ' origin/' + toAdd, shell=True)


#iterate locals and update them
for local in locals:
    subprocess.check_call('git checkout ' + local)
    subprocess.check_call('git pull')

subprocess.check_call('git checkout master')
