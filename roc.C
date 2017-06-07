  
#include "tdrstyle.C"
#include "CMS_lumi.C"
#include "TH1.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TPaveLabel.h"
#include "TLatex.h"
#include "TFrame.h"


TGraph* makegraph(TFile* f1, char* name1, char* name2) {

  std::cout<<"getting first"<<std::endl;
 TH1F *A_pt = static_cast<TH1F*>(f1->Get(name1)->Clone());
 A_pt->SetDirectory(0);
  double aaA = A_pt->Integral();
std::cout<<" first entries is "<<aaA<<std::endl;
  A_pt->Scale(1./aaA);
  const int nbin = A_pt->GetNbinsX();
  Double_t eff1[nbin];
  for(int i=0;i<nbin;i++) {
    eff1[i]=0.;
    for(int j=0;j<i;j++) {
      eff1[i]+=A_pt->GetBinContent(j);
    }
  }

  std::cout<<"getting second"<<std::endl;
  TH1F *B_pt = static_cast<TH1F*>(f1->Get(name2)->Clone());
  std::cout<<"ha"<<std::endl;
  B_pt->SetDirectory(0);
  //  B_pt->Rebin(25);
  double aaB = B_pt->Integral();
std::cout<<" second entries is "<<aaB<<std::endl;
  B_pt->Scale(1/aaB);

  Double_t bck1[nbin];
  std::cout<<" bin center  bck  sig "<<std::endl;
  for(int i=0;i<nbin;i++) {
    bck1[i]=0.;
    for(int j=0;j<i;j++) {
      bck1[i]+=B_pt->GetBinContent(j);
    }
    std::cout<<A_pt->GetBinCenter(i)<<" "<<bck1[i]<<" "<<eff1[i]<<std::endl;
  }


  gr = new TGraph(nbin,bck1,eff1);

  delete A_pt;
  delete B_pt;

  return gr;
}



void roc() 
{ 

  //TFile *f1 = new TFile("SumHistsQCD.root");
  //TFile *f1 = new TFile("SumHistsWMCtSkim.root");  
  //TFile *f1 = new TFile("SumHistsWSkim.root");  
  //TFile *f1 = new TFile("SumHists80.root");
  TFile *f1 = new TFile("SumHistsModelA.root");  
  //TFile *f1 = new TFile("SumHistsModelB.root");  
  //TFile *f1 = new TFile("SumHistsDATA.root");  
  //TFile *f1 = new TFile("SumHists80.root");  

 
  gStyle->SetOptStat(0);
 

  TCanvas *c1 = new TCanvas("c1","roc curves",200,10,700,500);

  //c1->SetFillColor(42);
  //c1->SetGrid();


  
  char* aa1 = "hsum2Dfdk";
  char* bb1 = "hsum2Dfd";
  TGraph* gr1 = makegraph(f1,aa1,bb1);
  gr1->SetMarkerStyle(21);
  gr1->SetMarkerColor(0);
  gr1->SetTitle("roc curve");
  gr1->GetXaxis()->SetTitle("background efficiency");
  gr1->GetYaxis()->SetTitle("signa efficiency");
  gr1->Draw("ACP");

  
  char* aa2 = "hdkjetamo";
  char* bb2 = "hdjetamo";
  TGraph* gr2 = makegraph(f1,aa2,bb2);
  gr2->SetMarkerStyle(21);
  gr2->SetMarkerColor(1);
  gr2->Draw("psame");

  char* aa3 = "ham2dfdk";
  char* bb3 = "ham2dfd";
  TGraph* gr3 = makegraph(f1,aa3,bb3);
  gr3->SetMarkerStyle(21);
  gr3->SetMarkerColor(2);
  gr3->Draw("psame");
  

  TLatex *t = new TLatex();
  t->SetTextFont(32);

  t->SetTextSize(0.1);
  t->SetTextAlign(12);

  t->SetTextColor(0);
  t->DrawLatex(0.5,0.5,"fraction tracks ipsig<3");
  t->SetTextColor(1);
  t->DrawLatex(0.5,0.3,"alphaMax origin");
  t->SetTextColor(2);
  t->DrawLatex(0.5,0.2,"2D sig");
  


  
  c1->Update();
  c1->GetFrame()->SetFillColor(21);
  c1->GetFrame()->SetBorderSize(12);
  c1->Modified();
 

  return;
}


