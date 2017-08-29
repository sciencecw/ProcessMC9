
#!/bin/bash

#while IFS='' read -r line || [[ -n "$line" ]]; do
#    echo "Text read from file: $line"
#done < models.txt

if [[ $# -eq 0 ]] ; then
    echo 'usage ./modelrun.sh [mode]'
    exit 1
fi

#for fullpath in /store/user/abelloni/EmJetAnalysis/Analysis-74xAODSIM_80xNTUPLES-v1/*/
#for fullpath in /store/user/yoshin/EmJetAnalysis/Analysis-20170811-v1/mass_X_d_1000_mass_pi_d_*_tau_pi_d*
#configs/models_80X.txt
while read -r folder
  #do folder=${fullpath%/} 
  #folder=${folder##*/}
  do yes | python control.py -s $folder -m$1 -n80X >modelrunoutput.log
  echo "python control.py -s $folder -m$1 -n80X"
  ./massjobs.sh
done < configs/models_80X.txt
