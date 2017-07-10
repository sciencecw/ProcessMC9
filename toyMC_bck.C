

#include "TH1.h"
#include "TSystem.h"
#include "TRandom.h"
#include "TFile.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>



int idbg=0;
int nexp = 10000000;
float eff_b_true = 0.03;
float eff_l_true = 0.001;
float ave_n_jet1 = 200000;
float ave_n_jet2 = 50000;
float ave_frac_b1 = 0.005;
float err_ave_frac_b1 = 0.;
float ave_frac_b2 = 0.23;
float err_ave_frac_b2 = 0.0;


void toyMC_bck()
{
  TH1F* hel = new TH1F("hel","measured eff light",1000,-0.001,0.01);
  TH1F* hb = new TH1F("hb","measured eff b",1000,-0.1,0.06);
  TH2F* helveb = new TH2F("helveb","el v eb",1000,0.0004,0.0016,1000,0.02,0.04);
  TH1F* hprod = new TH1F("hprod","el * eb",1000,0.,0.00005);

  for(int i=0; i<nexp; i++) {  // loop over experiments

    //std::cout<<std::endl<<std::endl<<"Event="<<i<<std::endl;
    float err_j1 = sqrt(ave_n_jet1);;
    float njet1 = gRandom->Gaus(ave_n_jet1,err_j1);
    //std::cout<<"njet1="<<njet1<<std::endl;
    float fracb1 = gRandom->Gaus(ave_frac_b1,err_ave_frac_b1);
    //std::cout<<"fracb1="<<fracb1<<std::endl;
    float npass1a = (eff_b_true*fracb1+eff_l_true*(1-fracb1))*njet1;
    //std::cout<<"npass1a="<<npass1a<<std::endl;
    float err_p1=sqrt(npass1a);
    float npass1 = gRandom->Gaus(npass1a,err_p1);
    //std::cout<<"npass1="<<npass1<<std::endl;

    float err_j2 = sqrt(ave_n_jet2);; 
    float njet2 = gRandom->Gaus(ave_n_jet2,sqrt(ave_n_jet2));
    //std::cout<<"njet2="<<njet2<<std::endl;
    float fracb2 = gRandom->Gaus(ave_frac_b2,err_ave_frac_b2);
    //std::cout<<"fracb2="<<fracb2<<std::endl;
    float npass2a = (eff_b_true*fracb2+eff_l_true*(1-fracb2))*njet2;
    //std::cout<<"npass2a="<<npass2a<<std::endl;
    float err_p2=sqrt(npass2a);
    float npass2 = gRandom->Gaus(npass2a,err_p2);
    //std::cout<<"npass2="<<npass2<<std::endl;

    float denoml = fracb1 * (1-fracb2) - fracb2* (1-fracb1);
    //std::cout<<"denoml="<<denoml<<std::endl;
    float denomb = (1-fracb1)*fracb2 - (1-fracb2)*fracb1;
    //std::cout<<"denomb="<<denomb<<std::endl;
    float tag1 = npass1/njet1;
    //std::cout<<"tag1="<<tag1<<std::endl;
    float tag2 = npass2/njet2;
    //std::cout<<"tag2="<<tag2<<std::endl;

    float el = (fracb1*tag2 - fracb2*tag1)/denoml;
    //std::cout<<"el="<<el<<std::endl;
    hel->Fill(el);
    float bb = ( (1-fracb1)*tag2-(1-fracb2)*tag1)/denomb;
    //std::cout<<"bb="<<bb<<std::endl;
    hb->Fill(bb);
    helveb->Fill(el,bb);
    hprod->Fill(el*bb);


  } // end loop over experiments

  TFile myfile("output.root","RECREATE");
  hel->Write();
  hb->Write();
  helveb->Write();
  hprod->Write();
}

//------------------------------------------------------------------------------
