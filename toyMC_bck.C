

#include "TH1.h"
#include "TSystem.h"
#include "TRandom.h"
#include "TFile.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>



int idbg=0;
int nexp = 1000;
float eff_b_true = 0.3;
float eff_l_true = 0.001;
float ave_n_jet1 = 200000;
float ave_n_jet2 = 50000;
float ave_frac_b1 = 0.1;
float err_ave_frac_b1 = 0.001;
float ave_frac_b2 = 0.1;
float err_ave_frac_b2 = 0.001;


void toyMC_bck()
{
  TH1F* hel = new TH1F("hel","measured eff light",1000,-0.1,1.);
  TH1F* hb = new TH1F("hb","measured eff b",1000,-0.1,1.);

  for(int i=0; i<nexp; i++) {  // loop over experiments

    float njet1 = gRandom->Gaus(ave_n_jet1,sqrt(ave_n_jet1));
    float fracb1 = gRandom->Gaus(ave_frac_b1,err_ave_frac_b1);
    float npass1a = (eff_b_true*fracb1+eff_l_true*(1-fracb1))*njet1;
    float npass1 = gRandom->Gaus(npass1a,sqrt(npass1a));
    float njet2 = gRandom->Gaus(ave_n_jet2,sqrt(ave_n_jet2));
    float fracb2 = gRandom->Gaus(ave_frac_b2,err_ave_frac_b2);
    float npass2a = (eff_b_true*fracb2+eff_l_true*(1-fracb2))*njet2;
    float npass2 = gRandom->Gaus(npass2a,sqrt(npass2a));

    float denoml = fracb1 * (1-fracb2) - fracb2* (1-fracb1);
    float denomb = (1-fracb1)*fracb2 - (1-fracb2)*fracb1;
    float tag1 = npass1/njet1;
    float tag2 = npass2/njet2;

    float el = (fracb1*tag2 - fracb2*tag1)/denoml;
    hel->Fill(el);
    float bb = ( (1-fracb1)*tag2-(1-fracb2)*tag1)/denomb;
    hb->Fill(bb);
      
    


  } // end loop over experiments

  TFile myfile("output.root","RECREATE");
  hel->Write();
  hb->Write();

}

//------------------------------------------------------------------------------
