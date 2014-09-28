import subprocess

#Get remotes string from git
remotesString = subprocess.check_output('git branch -r', universal_newlines=True)

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
            subprocess.check_call('git branch --track ' + local + ' origin/' + local)

