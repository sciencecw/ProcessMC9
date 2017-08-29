# ProcessMC9


o compile:

make


o running is a multistep process and is done on condor

o Signal ModelA,B
  to run on the files
  ./control.py -s ModelA -m 0
  to gather the output
  ./control.py -s ModelA -m 1

o Quick QCD - less statistics 
	(resource estimate: 3 jobs, 20 mins, then 1job, 2mins)
  ./control.py -s QQCD -m 0; ./massjobs.sh 
  ./control.py -s QQCD -m 2; ./massjobs.sh 

o QCD - full statistics
	(resource estimate: 13jobs, 3.5hours, then 1job, 2mins)
  ./control.py -s QCD2 -m 0 ; ./massjobs.sh
  ./control.py -s QCD2 -m 2 ; ./massjobs.sh

o Run on other signal models, it searches for configs in the configs/ folder
  ./control.py -s "mass_X_d_1000_mass_pi_d_2_tau_pi_d_25" -m 0 -n 80X; ./massjobs.sh
  ./control.py -s "mass_X_d_1000_mass_pi_d_2_tau_pi_d_25" -m 1 -n 80X; ./massjobs.sh

o alternatively use modelrun.sh to submit the full list of models (preset to overwrite)
  ./modelrun.sh [0,1]

o control.py
  dirsuffix="_rerun": sets output folder suffix 

o plot-making: Overlay_model.C,Overlay_model_dk.C  
  root -l  "Overlay_model.C+(\"${title}\",\"${dirname}\",\"${objname}\",\"${file1}\"..,\"${file6}\")
    or 
  ./runhisto.sh (change paths accordingly)
