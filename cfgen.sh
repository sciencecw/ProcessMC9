#!/bin/bash

# create config/parms files for alternative models

#workdir=$PWD
#echo $workdir
rm -f configs/models_80X.txt
for fullpath in /store/user/yoshin/EmJetAnalysis/Analysis-20170823-v0/mass_X_d_1000_mass_pi_d_*_tau_pi_d*
#for fullpath in /store/user/yoshin/EmJetAnalysis/Analysis-20170811-v1/mass_X_d_1000_mass_pi_d_*_tau_pi_d*
#cd /store/user/abelloni/EmJetAnalysis/Analysis-74xAODSIM_80xNTUPLES-v1
#for fullpath in /store/user/abelloni/EmJetAnalysis/Analysis-74xAODSIM_80xNTUPLES-v1/*/
  do folder=${fullpath%/} 
  folder=${folder##*/}
  configpath=configs/model_80X/${folder}_80X_config.txt
  parmspath=configs/modelparms_80X/parms_80X_${folder}.txt
  cp configs/parms_template.txt ${parmspath}
  fnum=`ls -1 ${fullpath}/*/*/*/*/ntuple_*.root | wc -l`
  echo ${folder} $fnum
  echo ${folder}>>configs/models_80X.txt
  sed -i 's/DIRNAME/'${folder}'_80X/g' ${parmspath}
  sed -i 's/NUM/'${fnum}'/g' ${parmspath}
  ls -1 ${fullpath}/*/*/*/*/ntuple_*.root > ${configpath}
done
