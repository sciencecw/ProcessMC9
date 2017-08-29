#include <iostream>
#include <iomanip>
#include <locale>

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TKey.h>
#include <TCollection.h>
#include <TObject.h>
#include <TList.h>

#include <fstream>
#include "vector"
#include "vector"
using std::vector;


#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

  
    // cuts
float DHTcut=800;
float Dpt1cut=300;
float Dpt2cut=100;
float Dpt3cut=100;
float Dpt4cut=50;
float Dalphacut=2;
float DmaxIPcut=0.03;
float Djetacut = 2.;
    // dont forget there is a hidden cut nalmostemergin<4!!!!!!!!!!!!!!!!!
int Dnemcut=2;
int Dntrk1=1;
    
float Dmetcut=0.;
float Dmasscut=2000;
float Dtheta2dcut=0.;
  

int EMJ16003(bool otfile, bool hasPre, const char* inputfilename,const char* outputfilename);

int EMJselect(bool otfile, bool hasPre, const char* inputfilename,const char* outputfilename,
	      float HTcut, float pt1cut, float pt2cut,float pt3cut, float pt4cut, float jetacut,float alphaMaxcut, float maxIPcut, float NemfracCut,float CemfracCut,
	      int ntrk1cut, int NemergingCut, bool blind,
	      float Dmetcut, float Dmass, float Dmasscut, float Dtheta2dcut
              );
std::vector<std::string> ListTH(std::string outdir,std::string binname,int htype);
void  HistNorm(std::string outdir, vector<double>& norm,int nbin,float* xsec, int* nfiles, std::string* binnames);
TH1F* HistMan(std::string outdir, float goalintlum,std::string thisHIST,vector<double>& histnorm, vector<double>& outnorm,int nbin,float* xsec, int* nfiles, std::string* binnames,bool donorm);

TH2F* HistMan2(std::string outdir, float goalintlum,std::string thisHIST,vector<double>& histnorm, vector<double>& outnorm,int nbin,float* xsec, int* nfiles, std::string* binnames,bool donorm);






void QCDhists(int imode, std::string outdir, int ibin, float goalintlum,  
int nbin, float* xsec, int* nfiles, std::string* binnames,std::string* filesnames,
std::string sumhistname,
 bool hasPre,bool donorm, bool blind, bool b16003,float themass) {

  std::string inputfile;
  std::string outputfile;
    // first make histograms for each file in each bin for the qcd sample
  if(imode==2) {
	for (int i=0;i<nbin;i++) nfiles[i]=1;
	sumhistname="Sum2"+sumhistname;
	std::cout<<"MODE2"<<std::endl;
	std::cout<<outdir<<std::endl;
	outdir+="Sum";
	std::cout<<outdir<<std::endl;
  }


  std::string bbname = outdir.c_str();
  std::cout<<"entering QCDhists: MODE "<<imode<<std::endl;
  std::cout<<"ibin "<<ibin<<" nbin "<<nbin<<" sumhistname "<<sumhistname<<std::endl;

  if(imode==0) {

    std::cout<<"making histograms for each file in each bin"<<std::endl;
    int i=ibin;
    std::ifstream inputconfig(filesnames[i]);
    std::cout<<"input config file is: "<<filesnames[i]<<std::endl;
    int linecounter = 0;
    while(std::getline(inputconfig,inputfile)) {
        
      std::cout<<"input file is "<<inputfile<<std::endl;
      outputfile=bbname+"histos"+binnames[i]+"_"+std::to_string(linecounter)+".root";
      std::cout<<"output file is "<<outputfile<<std::endl;

      int itmp;
      if(!b16003) {
	  itmp = EMJselect(true,hasPre,inputfile.c_str(),outputfile.c_str(),DHTcut, Dpt1cut,Dpt2cut,Dpt3cut,Dpt4cut,Djetacut,Dalphacut,DmaxIPcut,0.9,0.9,Dntrk1,Dnemcut,blind,
			       Dmetcut,themass,Dmasscut,Dtheta2dcut
                         );
      } else {
        itmp = EMJ16003(true,hasPre,inputfile.c_str(),outputfile.c_str());
      }
	  linecounter +=1;
    std::cout<<"passcounts: "<<itmp<<std::endl;
    }  // end of loop over files


  } else {  // other imodes

    // get normalization
    //  double norm[nbin];
  vector<double> norm(nbin);
  if(donorm) {
    HistNorm(bbname,norm,nbin,xsec,nfiles,binnames);  // this gives the total number of events in each bin before all selections using the eventCountPreTrigger histogram
  } else{
    for(int i=0;i<nbin;i++) norm[i]=1.;
  }
  for(int i=0;i<nbin;i++) {
    std::cout<<"total number events in bin "<<i<<" is "<<norm[i]<<std::endl;
  }
  TH1F* countclone = new TH1F("countclone","unnormalized count",20,0,20);
  for(int i=0;i<nbin;i++){
    countclone->AddBinContent(i+1,norm[i]);
  }
  
  TH1F* normhst = new TH1F("normhst","counts pretrigger by bin",nbin,0.,nbin);
  for(int i=0;i<nbin;i++){
    normhst->AddBinContent(i+1,norm[i]);
  }
   std::vector<std::string> histnames = ListTH(bbname,binnames[0],1);
   std::vector<std::string> histnames2 = ListTH(bbname,binnames[0],2);
   //std::vector<std::string> histnames  = (std::vector<std::string>) histnamesall[0];
  // std::vector<std::string> histnames2 = (std::vector<std::string>) histnamesall[1];
   
   int nhist = histnames.size();
   int nhist2 = histnames2.size();
   std::cout<<"num of TH1:"<< nhist<<std::endl;
   std::cout<<"num of TH2:"<< nhist2<<std::endl;

    //make and  output summed and renormalized histograms
  std::cout<<"normalizing histograms"<<std::endl;
  std::vector<TH1F*> vv(nhist);
  vector<double> outnorm(nbin);
  for(int i=0;i<nhist;i++) {
    std::cout<<" enering Histman with i = "<<i<<": "<<histnames[i]<<std::endl;
    vv[i]=HistMan(bbname,goalintlum,histnames[i],norm,outnorm,nbin,xsec,nfiles,binnames,donorm);
  }

  std::vector<TH2F*> vv2(nhist2);
  vector<double> outnorm2(nbin);
  for(int i=0;i<nhist2;i++) {
    std::cout<<" enering Histman2 with i = "<<i<<": "<<histnames2[i]<<std::endl;
    vv2[i]=HistMan2(bbname,goalintlum,histnames2[i],norm,outnorm2,nbin,xsec,nfiles,binnames,donorm);
  }

    // output total event count
  std::cout<<" initial event count before and after norm is"<<std::endl;
  double ttotal=0;
  for(int i=0;i<nbin;i++) {
    std::cout<<" bin "<<i<<" norm "<<norm[i]<<" times outnorm is "<<norm[i]*outnorm[i]<<std::endl;
    ttotal = ttotal + norm[i]*outnorm[i];
  }
  std::cout<<"total is "<<ttotal<<std::endl;;



  std::cout<<"outputting histograms"<<std::endl;
  outputfile=bbname+sumhistname;
  TFile out(outputfile.c_str(),"RECREATE");
   std::cout<<"Openning & writing to File: "<<outputfile<<std::endl;
  normhst->Write();
  countclone->Write();
  for(int i=0;i<nhist;i++) {
    vv[i]->Write();
  }
  for(int i=0;i<nhist2;i++) {
    vv2[i]->Write();
  }

  }

  std::cout<<"job finished succcessfully"<<std::endl;

    return;
}



std::vector<std::string> ListTH(std::string outdir,std::string binname,int htype) {
            std::string inputfile=outdir+"histos"+binname+"_0.root";
	std::vector<std::string> thlist={};
	TFile* fin = new TFile(inputfile.c_str()) ;

TList* list = fin->GetListOfKeys() ;
if (!list) { std::cout<<"<E> No keys found in file"<<std::endl; exit(1) ; }
TIter next(list) ; TKey* key ; TObject* obj ;
    
while (( key = (TKey*)next() )) {
  obj = key->ReadObj() ;
  if (    (strcmp(obj->IsA()->GetName(),"TProfile")!=0)
       && (!obj->InheritsFrom("TH2"))
 && (!obj->InheritsFrom("TH1")) 
     ) {
    std::cout<<"<W> Object "<<obj->GetName()<<" is not 1D or 2D histogram: will not be converted\n" ;
  }
if (obj->InheritsFrom("TH2")){
    if (htype==2){
	std::cout<<"TH2 Histo name: "<<obj->GetName()<<" title: "<< obj->GetTitle()<<std::endl;
   	thlist.push_back(std::string(obj->GetName())); 
    }
} else if (obj->InheritsFrom("TH1") && htype==1){
    std::cout<<"TH1 Histo name: "<<obj->GetName()<<" title: "<< obj->GetTitle()<<std::endl;
   thlist.push_back(std::string(obj->GetName())); 
}
}

return thlist;	
}

TH1F* HistMan(std::string outdir, float goalintlum,std::string thisHIST,vector<double>& norm,vector<double>& outnorm,int nbin,float* xsec, int* nfiles, std::string* binnames,bool donorm) {

    std::string inputfile;


    // now add up all the files for one bin
    vector<TH1F> sum(nbin);
    for(int i=0;i<nbin;i++) {  // for each bin
        for(int j=0;j<nfiles[i];j++) { //for each file for that bin
            inputfile=outdir+"histos"+binnames[i]+"_"+std::to_string(j)+".root";
            TFile* in = new TFile(inputfile.c_str());
    		std::cout<<"Openning File: "<<inputfile<<std::endl;
            if (!in->GetListOfKeys()->Contains(thisHIST.c_str())) return (new TH1F(thisHIST.c_str(),"dummy empty hist",10,0.,10.));

            if(j==0) {
                std::cout<<" adding up histos within a bin"<<std::endl;
                sum[i] = *(static_cast<TH1F*>(in->Get(thisHIST.c_str())->Clone()));
            } else {
                TH1F* tmp = static_cast<TH1F*>(in->Get(thisHIST.c_str())->Clone());
                sum[i].Add(tmp);
            }
            in->Close();
        }
    }

    if(donorm) {
        // reweight to int lum
        std::cout<<" reweighting to inst lum of "<<goalintlum<<" for each bin"<<std::endl;
        for(int i=0;i<nbin;i++) {
            // get total number of events before filter
            float ntotal = norm[i];
            std::cout<<" for bin "<<i<<" number of pretrigger events is "<<ntotal<<std::endl;
            float fileLum= ntotal/xsec[i];
            std::cout<<" equ lum for bin is "<<fileLum<<" fb-1"<<std::endl;
            outnorm[i] = goalintlum/fileLum;
            std::cout<<" scaling by a factor of "<<outnorm[i]<<std::endl;
            sum[i].Scale(outnorm[i]);
        }
    }

    //add the bins
    std::cout<<" adding bins"<<std::endl;
    TH1F* SUM=static_cast<TH1F*>((sum[0]).Clone());
    for(int i=1;i<nbin;i++) {
        SUM->Add(&sum[i]);
    }


    return SUM;
}

TH2F* HistMan2(std::string outdir, float goalintlum,std::string thisHIST,vector<double>& norm,vector<double>& outnorm,int nbin,float* xsec, int* nfiles, std::string* binnames,bool donorm) {

    std::string inputfile;


    // now add up all the files for one bin
    std::cout<<" adding up histos within a bin"<<std::endl;
    vector<TH2F> sum(nbin);
    for(int i=0;i<nbin;i++) {  // for each bin
        for(int j=0;j<nfiles[i];j++) { //for each file for that bin
            inputfile=outdir+"histos"+binnames[i]+"_"+std::to_string(j)+".root";
            TFile* in = new TFile(inputfile.c_str());
    		std::cout<<"Openning File: "<<inputfile<<std::endl;
            if(j==0) {
                sum[i] = *(static_cast<TH2F*>(in->Get(thisHIST.c_str())->Clone()));
            } else {
                TH2F* tmp = static_cast<TH2F*>(in->Get(thisHIST.c_str())->Clone());
                sum[i].Add(tmp);
            }
            in->Close();
        }
    }

    if(donorm) {
        // reweight to int lum
        std::cout<<" reweighting to inst lum of "<<goalintlum<<" for each bin"<<std::endl;
        for(int i=0;i<nbin;i++) {
            // get total number of events before filter
            float ntotal = norm[i];
            std::cout<<" for bin "<<i<<" number of pretrigger events is "<<ntotal<<std::endl;
            float fileLum= ntotal/xsec[i];
            std::cout<<" equ lum for bin is "<<fileLum<<" fb-1"<<std::endl;
            outnorm[i] = goalintlum/fileLum;
            std::cout<<" scaling by a factor of "<<outnorm[i]<<std::endl;
            sum[i].Scale(outnorm[i]);
        }
    }


    //add the bins
    std::cout<<" adding bins"<<std::endl;
    TH2F* SUM=static_cast<TH2F*>((sum[0]).Clone());
    for(int i=1;i<nbin;i++) {
        SUM->Add(&sum[i]);
    }


    return SUM;
}

void  HistNorm(std::string outdir, vector<double>& norm,int nbin,float* xsec, int* nfiles, std::string* binnames) {

    std::cout<<"entering HistNorm with nfiles "<<nfiles[0]<<std::endl; 

    std::string inputfile;
    TFile * in;

    // now add up all the files for one bin
    vector<TH1F> sum(nbin);
    for(int i=0;i<nbin;i++) {  // for each bin
        for(int j=0;j<nfiles[i];j++) { //for each file for that bin
            inputfile=outdir+"histos"+binnames[i]+"_"+std::to_string(j)+".root";
            std::cout<<i<<" "<<j<<" "<<inputfile<<std::endl;
            in = new TFile(inputfile.c_str());
            if(j==0) {
                sum[i] = *(static_cast<TH1F*>(in->Get("eventCountPreTrigger")->Clone()));
            } else {
                TH1F* tmp = static_cast<TH1F*>(in->Get("eventCountPreTrigger")->Clone());
                sum[i].Add(tmp);
            }
            in->Close();
        }
    }

    // reweight to int lum

    for(int i=0;i<nbin;i++) {
        // get total number of events before filter
        norm[i] = sum[i].GetBinContent(2);
        std::cout<<"norm "<<i<<" "<<norm[i]<<std::endl;
    }


    return;
}
