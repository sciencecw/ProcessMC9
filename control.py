#!/usr/bin/env python

import subprocess
import os
import shutil
import sys
import optparse
#from optparse import OptionParser, OptionGroup

parser = optparse.OptionParser(usage="usage: python scan.py -s [QCD, QQCD, ModelA, ModelB] -m [0,1]",description="This is a script to set up and run a scan over many files in parallel")

parser.add_option ("-m","--mode", type="int", dest="mode", help="0=process 1=gather")

parser.add_option ("-s","--sample", type="string", dest="sample", help="Sample to process (e.g., QCD)")

parser.add_option ("-n","--ntuples", type="string", dest="ntuples", help="Ntuples to process (e.g. 74X, 80X)")


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

if options.ntuples=='80X':
   modellistpath='configs/models_80X.txt'
   modelparmspath="configs/modelparms_80X/parms_80X_"
if options.ntuples=='74X':
   modellistpath='configs/models_7480X.txt'
   modelparmspath="configs/modelparms_7480x/parms_7480x_"


#Set the output directory name and parameter file based on the sample you are running over
dirname=""
dirsuffix="_rerun"
prefix=""
nbin=""
if options.mode==0:
  print "running on files"
elif options.mode==1:
  print "gathering results"
elif options.mode==2:
  print "Quick gathering results "
else:
  print("mode not recognized!")
  parser.print_usage()
  exit(1)

isOtherModel=False
if options.sample=="QQCD":
  dirname="files_QQCD"+dirsuffix
  prefix = "QQCD"
  nbin=3
elif options.sample=="QCD":
  dirname="files_QCD"+dirsuffix
  prefix = "QCD"
  nbin=3
  print "slower than QCD2"
elif options.sample=="QCD2":
  dirname="files_QCD2"+dirsuffix
  prefix = "QCD2"
  nbin=13
elif options.sample=="ModelA":
  dirname="files_modA"+dirsuffix
  prefix = "modelA"
  nbin=1
elif options.sample=="ModelB":
  dirname="files_modB"+dirsuffix
  prefix = "modelB"
  nbin=1
else:
  with open(modellistpath) as myfile:
     if options.sample in myfile.read():
        print "othermodel: ", options.sample
        prefix=options.sample
        dirname="othermodels/files_"+prefix
        nbin=1
        isOtherModel=True
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
if options.mode==2:
   f.write("mergeTFileServiceHistograms -i "+dirname+"/histosQCD_HT500to700_*.root -o "+dirname+"/SumhistosQCD_HT500to700_0.root\n")
   f.write("mergeTFileServiceHistograms -i "+dirname+"/histosQCD_HT1000to1500*.root -o "+dirname+"/SumhistosQCD_HT1000to1500_0.root\n")
   f.write("mergeTFileServiceHistograms -i "+dirname+"/histosQCD_HT700to1000*.root -o "+dirname+"/SumhistosQCD_HT700to1000_0.root\n")
   f.write("mergeTFileServiceHistograms -i "+dirname+"/histosQCD_HT1500to2000*.root -o "+dirname+"/SumhistosQCD_HT1500to2000_0.root\n")
   f.write("mergeTFileServiceHistograms -i "+dirname+"/histosQCD_HT2000toInf*.root -o "+dirname+"/SumhistosQCD_HT2000toInf_0.root\n")
if options.mode>=1:
  nbin=1

print "number of bins is "+str(nbin)
for i in range(0,nbin):
  print "i="+str(i+1)
  parmsfile=hostarea+"configs/parms_"+prefix+".txt"
  if options.sample=="QCD2" and options.mode==0:
	parmsfile=hostarea+"configs/parms_QCD2b.txt"
  if isOtherModel: 
       parmsfile=hostarea+modelparmspath+prefix+".txt"
  name = "files-"+prefix+"-"+str(i+1)
  
  jdlfile = open(hostarea2+"condor-jobs-"+str(options.mode)+"_"+name+".jdl","w")
  jdlfile.write("universe = vanilla"+'\n')
  jdlfile.write("Executable = "+mainarea+"/condor-executable.sh"+'\n')
  jdlfile.write("should_transfer_files = NO"+'\n')
  #jdlfile.write("Requirements = TARGET.FileSystemDomain == \"privnet\""+'\n')
  jdlfile.write("Requirements = TARGET.FileSystemDomain == \"privnet\"&& TARGET.Machine =!= \"r510-0-6.privnet\""+'\n')
  jdlfile.write("Output = "+hostarea2+name+"_"+str(options.mode)+"_sce_$(cluster)_$(process).stdout"+'\n')
  jdlfile.write("Error = "+hostarea2+name+"_"+str(options.mode)+"_sce_$(cluster)_$(process).stderr"+'\n')
  jdlfile.write("Log = "+hostarea2+name+"_"+str(options.mode)+"_sce_$(cluster)_$(process).condor"+'\n')
  jdlfile.write("Arguments = "+name+" $(process) "+hostarea+" "+str(options.mode)+" "+str(i)+" "+parmsfile+" "+
  hostarea2+" "+'\n')
  jdlfile.write("priority=5\n")
  jdlfile.write("Queue 1"+'\n')
  jdlfile.close()

  f.write("condor_submit "+dirname+"/condor-jobs-"+str(options.mode)+"_"+name+'.jdl'+'\n')

f.close()



