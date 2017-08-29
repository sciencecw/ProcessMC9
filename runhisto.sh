#!/bin/bash

#SumHistmass_X_d_800_mass_pi_d_5_tau_pi_d_5_7480X.root
#six     :  200,400,600,800,1000,1500,2000
#quartre :  1,2,5,10
#six     :  0p001 0p1 1 5 150 300
#           0p001 0p1 1 5 25 60 100 150 300
#albtosamples/SumHistmass_X_d_800_mass_pi_d_5_tau_pi_d_5_7480X.root


callfunc(){ 
    # here plotting general event variables
root -q -l -b "Overlay_model.C+(\"H_{t} before cut\",\"${dirname}_\",\"H_T\" ${argm},4)"    #HT plot
root -q -l -b "Overlay_model.C+(\"H_{t} HLT nm1\",\"${dirname}_\",\"hHThltnm1\" ${argm},4)"  # HTHLTnm1
    #sleep 1
# root -q -l -b "Overlay_model.C+(\"pt  before cut\",\"${dirname}_\",\"hpt\" ${argm},10)"    #pt plot
# root -q -l -b "Overlay_model.C+(\"pt1 before cut\",\"${dirname}_\",\"hpt1\"${argm},10,10.)"   #pt1 plot
# root -q -l -b "Overlay_model.C+(\"pt2 before cut\",\"${dirname}_\",\"hpt2\"${argm},10)"   #pt2 plot
# root -q -l -b "Overlay_model.C+(\"pt3 before cut\",\"${dirname}_\",\"hpt3\"${argm},10)"   #pt3 plot
# root -q -l -b "Overlay_model.C+(\"pt4 before cut\",\"${dirname}_\",\"hpt4\"${argm},10)"   #pt4 plot
# root -q -l -b "Overlay_model.C+(\"Ntrckpt1 before cut\",\"${dirname}_\",\"hbcut_ntrkpt1\"${argm})" #ntrkpt1 
# root -q -l -b "Overlay_model.C+(\"log(theta2D) before cut\",\"${dirname}_\",\"hbcut_theta2d\"${argm},5,30.)" #theta2d 
# root -q -l -b "Overlay_model.C+(\"ME_{T} before cut\",\"${dirname}_\",\"hmet\"${argm},5)" #met plot
# root -q -l -b "Overlay_model.C+(\"jet mass before cut\",\"${dirname}_\",\"hmass\"${argm},10)" #mass plot
# root -q -l -b "Overlay_model.C+(\"medIP before cut\",\"${dirname}_\",\"hbcut_medip\"${argm},10)" #medip plot (all kinds jets)
    # from here we plot QCD up+down vs Model dark jets for each variables
    #sleep 1
# root -q -l -b "Overlay_model_dk.C+(\"a3d \",\"${dirname}_\",\"ham2dfud\",\"ham2dfdk\"${argmdk},5)" #a3d plot
# root -q -l -b "Overlay_model_dk.C+(\"log(a3d) \",\"${dirname}_\",\"hlgam2dfud\",\"hlgam2dfdk\"${argmdk},5)" #a3d plot
# root -q -l -b "Overlay_model_dk.C+(\"ip2dsig\",\"${dirname}_\",\"hip2dsigud\",\"hip2dsigdk\"${argmdk},10)"  
# root -q -l -b "Overlay_model_dk.C+(\"pvz-trz\",\"${dirname}_\",\"hpvztrzud\",\"hpvztrzdk\"${argmdk},10)" #pvztrz 
# root -q -l -b "Overlay_model_dk.C(\"meanIP ud quarks vs dark quark\",\"${dirname}_\",\"hmedipud\",\"hmedipdk\"${argmdk},5)" #a3d plot
# root -q -l -b "Overlay_model_dk.C(\"theta2D ud quarks vs dark quark\",\"${dirname}_\",\"htheta2dud\",\"htheta2ddk\"${argmdk},5)" #a3d plot

    # these are Model down vs Model dark jets (note difference from above)
# root -q -l -b "Overlay_model_dk.C+(\"medIP\",\"${dirname}_\",\"hdjetmedip\",\"hdkjetmedip\"${argmdk})" #medipddk
# root -q -l -b "Overlay_model_dk.C+(\"log(medIP)\",\"${dirname}_\",\"hdjetlgmedip\",\"hdkjetlgmedip\"${argmdk},2)" #log medip 
  # Here comes plot of track variables
# root -q -l -b "Overlay_model_dk.C+(\"ahate\",\"${dirname}_\",\"hdjettrkahate\",\"hdkjettrkahate\"${argmdk},5)" 

}


#yh1000plots
pdfdirlow=yh1000plots_low_lifetime
pdfdir=yh1000plots_long_lifetime

if [ ! -d "${pdfdir}/" ]; then
  mkdir "${pdfdir}/"
fi


#for xd in 400 600 800 1000 1500 2000 

for xd in 1000
do 
#  for mp in 1 2 5 10
  for mp in 1 10
  do 
  	argm=""
  	dirname="Taudp_xd"${xd}"_mp"${mp}
    #legname="lifetime 0.001mm, lifetime 1mm,lifetime 25mm,lifetime 60mm,lifetime #100mm,lifetime 300mm"
    #legname="lifetime 0.001mm,lifetime 0.1mm,lifetime 1mm,lifetime 5mm,lifetime 25mm"
    legname="lifetime 5mm,lifetime 25mm,lifetime 60mm,lifetime 100mm,lifetime 300mm"
    legnamedk="dark jet lifetime 5mm,dark jet lifetime 25mm,dark jet lifetime 60mm,dark jet lifetime 100mm,dark jet lifetime 300mm"
    #legnamedk="dark jet lifetime 0.001mm,dark jet lifetime 1mm,dark jet lifetime #25mm,dark jet lifetime 60mm,dark jet lifetime 100mm,dark jet lifetime 300mm"
    #legnamedk="dark jet lifetime 0.001mm,dark jet lifetime 0.1mm, dark jet lifetime #1mm,dark jet lifetime 5mm,dark jet lifetime 25mm"
    #for tp in 0p001 1 25 60 100 300
    #for tp in 0p001 0p1 1 5 25 
    for tp in 5 25 60 100 300
    do
    	filename="yhsamplesXd1000/SumHistmass_X_d_${xd}_mass_pi_d_${mp}_tau_pi_d_${tp}_80X.root"
        argm=${argm}",\"${filename}\""
    done
    if [ ! -d "${pdfdir}/${dirname}" ]; then
    	mkdir "${pdfdir}/${dirname}"
    fi
    #argmdk=",\""${legnamedk}"\""${argm}
    #argm=",\"${legname}\"${argm}"
    argmdk=",\""${legnamedk}"\""${argm}",\"\""
    argm=",\"${legname}\"${argm},\"\""
	callfunc
    mv Fig_${dirname}*.pdf ${pdfdir}/${dirname}
  done
done

##for xd in 400 600 800 1000 1500 2000 
#for xd in 1000
#do 
##  for mp in 1 2 5 10
#  for mp in 1 10
#  do 
#    argm=""
#    dirname="Taudp_xd"${xd}"_mp"${mp}
#    legname="lifetime 0.001mm,lifetime 0.1mm,lifetime 1mm,lifetime 5mm,lifetime 25mm"
#    legnamedk="dark jet lifetime 0.001mm,dark jet lifetime 0.1mm, dark jet lifetime 1mm,dark jet lifetime 5mm,dark jet lifetime 25mm"
#    for tp in 0p001 0p1 1 5 25   
#    do
#      filename="yhsamplesXd1000/SumHistmass_X_d_${xd}_mass_pi_d_${mp}_tau_pi_d_${tp}_80X.root"
#        argm=${argm}",\"${filename}\""
#    done
#    if [ ! -d "${pdfdirlow}/${dirname}" ]; then
#      mkdir "${pdfdirlow}/${dirname}"
#    fi
#    #argmdk=",\""${legnamedk}"\""${argm}
#    #argm=",\"${legname}\"${argm}"
#    argmdk=",\""${legnamedk}"\""${argm}",\"\""
#    argm=",\"${legname}\"${argm},\"\""
#  callfunc
#    mv Fig_${dirname}*.pdf ${pdfdirlow}/${dirname}
#  done
#done





##for xd in 400 600 800 1000 1500 2000 
#for xd in 1000 
#do 
#  #for tp in 0p1 25 60 150
#  for tp in 25 60 
#  do 
#  	argm=""
#  	dirname="MPion_xd"${xd}"_tp"${tp}
#    legname="pion mass 1 GeV,pion mass 2GeV,pion mass 5GeV,pion mass 10GeV"
#    legnamedk="dark jets: pion mass 1 GeV,dark jets: pion mass 2GeV,dark jets: pion mass 5GeV,dark jets: pion mass 10GeV"
#    for mp in 1 2 5 10
#    do
#    	filename="yhsamplesXd1000/SumHistmass_X_d_${xd}_mass_pi_d_${mp}_tau_pi_d_${tp}_80X.root"
#        argm=${argm}",\"${filename}\""
#    done
#    if [ ! -d "${pdfdir}/${dirname}" ]; then
#    	mkdir "${pdfdir}/${dirname}"
#    fi
#    argmdk=",\""${legnamedk}"\""${argm}",\"\",\"\""
#    argm=",\"${legname}\"${argm},\"\",\"\""
#    callfunc
#    mv Fig_${dirname}*.pdf ${pdfdir}/${dirname}
#  done
#done




#for tp in 0p1 300
#do 
#  for mp in 1 10
#  do 
#  	argm=""
#  	dirname="Mmed_tp"${tp}"_mp"${mp}
#	for xd in 400 600 800 1000 1500 2000 
#    do
#    	filename="yhsamplesXd1000/SumHistmass_X_d_${xd}_mass_pi_d_${mp}_tau_pi_d_${tp}_80X.root"
#        argm=${argm}",\"${filename}\""
#    done
#    if [ ! -d "${pdfdir}/${dirname}" ]; then
#    	mkdir "${pdfdir}/${dirname}"
#    fi
#    argm=",\"${legname}\"${argm}"
#    argmdk=${argm}
#    callfunc
#    mv Fig_${dirname}*.pdf ${pdfdir}/${dirname}
#  done
#done



