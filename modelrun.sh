
#!/bin/bash

#while IFS='' read -r line || [[ -n "$line" ]]; do
#    echo "Text read from file: $line"
#done < models.txt

for fullpath in /store/user/abelloni/EmJetAnalysis/Analysis-74xAODSIM_80xNTUPLES-v1/*/
  do folder=${fullpath%/} 
  folder=${folder##*/}
  configpath=${folder}_7480X_config.txt
  parmspath=parms_7480x_${folder}.txt
  cp parms_template.txt ${parmspath}
  fnum=`ls -1 ${fullpath}/*/*/*/*/ntuple_*.root | wc -l`
  echo ${folder} $fnum
  echo ${folder}>>models.txt
  sed -i 's/DIRNAME/'${folder}'_7480X/g' ${parmspath}
  sed -i 's/NUM/'${fnum}'/g' ${parmspath}
  ls -1 ${fullpath}/*/*/*/*/ntuple_*.root > ${configpath}
done
