  
#include "tdrstyle.C"
#include "CMS_lumi.C"
#include "TH1.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TPaveLabel.h"
#include "TLatex.h"
#include "TFrame.h"

int nrebin=20;

TGraph* makegraph(TFile* f1, char* name1, char* name2) {

 std::cout<<"getting first"<<std::endl;
 TH1F *A_pt = static_cast<TH1F*>(f1->Get(name1)->Clone());
 A_pt->SetDirectory(0);
 A_pt->Rebin(nrebin);
 const int nbin = A_pt->GetNbinsX();
 double aaA = A_pt->Integral();
 std::cout<<" first entries is "<<aaA<<std::endl;
 Double_t num[nbin];
 Double_t abin[nbin];
 for(int i=0;i<nbin;i++) {
   abin[i]= A_pt->GetBinCenter(i);
   num[i]=A_pt->GetBinContent(i);
 }

 std::cout<<"getting second"<<std::endl;
 TH1F *B_pt = static_cast<TH1F*>(f1->Get(name2)->Clone());
 B_pt->SetDirectory(0);
 B_pt->Rebin(nrebin);
 double aaB = B_pt->Integral();
 std::cout<<" second entries is "<<aaB<<std::endl;

Double_t denom[nbin];
for(int i=0;i<nbin;i++) {
  denom[i]=B_pt->GetBinContent(i);
}

 Double_t eff[nbin];
for(int i=0;i<nbin;i++) {
  eff[i]=0.;
  if(denom[i]>0) eff[i]=num[i]/denom[i];
  std::cout<<abin[i]<<" "<<num[i]<<" "<<denom[i]<<" "<<eff[i]<<std::endl;
}



 gr = new TGraph(nbin,abin,eff);

  delete A_pt;
  delete B_pt;

  return gr;
}



void fractions() 
{ 

  //TFile *f1 = new TFile("SumHistsQCD.root");
  TFile *f1 = new TFile("SumHistsQQCD.root");
  //TFile *f1 = new TFile("SumHistsWMCtSkim.root");  
  //TFile *f1 = new TFile("SumHistsWSkim.root");  
  //TFile *f1 = new TFile("SumHists80.root");
  //TFile *f1 = new TFile("SumHistsModelA.root");  
  //TFile *f1 = new TFile("save.root");  
  //TFile *f1 = new TFile("SumHistsModelB.root");  
  //TFile *f1 = new TFile("SumHistsDATA.root");  
  //TFile *f1 = new TFile("SumHists80.root");  

 
  gStyle->SetOptStat(0);
 

  TCanvas *c1 = new TCanvas("c1","roc curves",200,10,700,500);

  //c1->SetFillColor(42);
  //c1->SetGrid();


  
  char* aa1 = "hptgpre";
  char* bb1 = "hptallpre";
  TGraph* gr1 = makegraph(f1,aa1,bb1);
  gr1->SetMarkerStyle(21);
  gr1->SetMarkerColor(1);
  gr1->GetYaxis()->SetRangeUser(0.,1.);
  gr1->SetTitle("fraction");
  gr1->GetXaxis()->SetTitle("jet pt");
  gr1->GetYaxis()->SetTitle("flavor fraction");
  gr1->Draw("AP");

  
  char* aa2 = "hptudpre";
  char* bb2 = "hptallpre";
  TGraph* gr2 = makegraph(f1,aa2,bb2);
  gr2->SetMarkerStyle(21);
  gr2->SetMarkerColor(2);
  gr2->Draw("psame");
  


  char* aa3 = "hptbpre";
  char* bb3 = "hptallpre";
  TGraph* gr3 = makegraph(f1,aa3,bb3);
  gr3->SetMarkerStyle(21);
  gr3->SetMarkerColor(3);
  gr3->Draw("psame");


  char* aa4 = "hptgbbpre";
  char* bb4 = "hptallpre";
  TGraph* gr4 = makegraph(f1,aa4,bb4);
  gr4->SetMarkerStyle(21);
  gr4->SetMarkerColor(4);
  gr4->Draw("psame");


  char* aa5 = "hptspre";
  char* bb5 = "hptallpre";
  TGraph* gr5 = makegraph(f1,aa5,bb5);
  gr5->SetMarkerStyle(21);
  gr5->SetMarkerColor(5);
  gr5->Draw("psame");
  
  

  char* aa6 = "hptcpre";
  char* bb6 = "hptallpre";
  TGraph* gr6 = makegraph(f1,aa6,bb6);
  gr6->SetMarkerStyle(21);
  gr6->SetMarkerColor(6);
  gr6->Draw("psame");


  TLatex *t = new TLatex();
  t->SetTextFont(32);

  t->SetTextSize(0.05);
  t->SetTextAlign(12);

  t->SetTextColor(1);
  t->DrawLatex(10,0.5,"g ");
  t->SetTextColor(2);
  t->DrawLatex(10,0.45,"ud ");
  t->SetTextColor(3);
  t->DrawLatex(10,0.40,"b ");
  t->SetTextColor(4);
  t->DrawLatex(10,0.35,"g to b ");
  t->SetTextColor(5);
  t->DrawLatex(10,0.30,"s ");
  t->SetTextColor(6);
  t->DrawLatex(10,0.25,"c ");
  


  
  c1->Update();
  c1->GetFrame()->SetFillColor(18);
  c1->GetFrame()->SetBorderSize(12);
  c1->Modified();
 

  return;
}


