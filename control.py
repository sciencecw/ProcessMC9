#! /usr/bin/env/python

import subprocess
import os
import shutil
import sys
import optparse
#from optparse import OptionParser, OptionGroup

parser = optparse.OptionParser(usage="usage: python scan.py -s [QCD, QQCD, ModelA, ModelB] -m [0,1]",description="This is a script to set up and run a scan over many files in parallel")

parser.add_option ("-m","--mode", type="int", dest="mode", help="0=process 1=gather")

parser.add_option ("-s","--sample", type="string", dest="sample", help="Sample to process (e.g., QCD)")


(options, args) = parser.parse_args()
if len(args) < 0:
  print "Too few arguments"
  parser.print_usage()
  exit(1)

print options.mode
print options.sample

mainarea = os.getcwd()
hostarea = os.getcwd()
hostarea+="/"


#Set the output directory name and parameter file based on the sample you are running over
dirname=""
prefix=""
nbin=""
if options.mode==0:
  print "running on files"
elif options.mode==1:
  print "gathering results"
else:
  print("mode not recognized!")
  parser.print_usage()
  exit(1)


if options.sample=="QQCD":
  dirname="files_QQCD"
  prefix = "QQCD"
  nbin=2
elif options.sample=="QCD":
  dirname="files_QCD"
  prefix = "QCD"
  nbin=4
elif options.sample=="ModelA":
  dirname="files_modA"
  prefix = "modelA"
  nbin=1
elif options.sample=="ModelB":
  dirname="files_modB"
  prefix = "modelB"
  nbin=1
else:
  print("Input sample not recognized!")
  parser.print_usage()
  exit(1)

#Create output directory; ask what to do if it exists already
if options.mode==0:
  print('Checking if directory {} exists...'.format(dirname))
  if os.path.exists(hostarea+dirname):
    userreply = raw_input("Output directory already exists! Are you sure you want to overwrite all the contents? (y/n): ")
    if (userreply == "y"):
      print("Answer = yes. Old output directory will be deleted and a new empty one will be created.")
      shutil.rmtree(hostarea+dirname)
      os.makedirs(hostarea+dirname)
    elif (userreply == "n"):
      print("Answer = no. Exiting scan.py.")
      exit() 
    else:
      print("Invalid input. Exiting scan.py anyway.")
      exit()
  else:
    os.makedirs(hostarea+dirname)

hostarea2=hostarea+dirname+"/"

#write the .jdl file and the bash script to submit the condor job
f = open("massjobs.sh",'w')

print "number of bins is "+str(nbin)
for i in range(0,nbin):
  print "i="+str(i+1)
  parmsfile=hostarea+"configs/parms_"+prefix+".txt"
  name = "files-"+prefix+"-"+str(i+1)
  
  jdlfile = open(hostarea2+"condor-jobs-"+name+".jdl","w")
  jdlfile.write("universe = vanilla"+'\n')
  jdlfile.write("Executable = "+mainarea+"/condor-executable.sh"+'\n')
  jdlfile.write("should_transfer_files = NO"+'\n')
  jdlfile.write("Requirements = TARGET.FileSystemDomain == \"privnet\""+'\n')
  jdlfile.write("Output = "+hostarea2+name+"_sce_$(cluster)_$(process).stdout"+'\n')
  jdlfile.write("Error = "+hostarea2+name+"_sce_$(cluster)_$(process).stderr"+'\n')
  jdlfile.write("Log = "+hostarea2+name+"_sce_$(cluster)_$(process).condor"+'\n')
  jdlfile.write("Arguments = "+name+" $(process) "+hostarea+" "+str(options.mode)+" "+str(i)+" "+parmsfile+" "+
  hostarea2+" "+'\n')
  jdlfile.write("Queue 1"+'\n')
  jdlfile.close()

  f.write("condor_submit "+dirname+"/condor-jobs-"+name+'.jdl'+'\n')

f.close()



