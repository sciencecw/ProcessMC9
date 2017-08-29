  
#include "tdrstyle.C"
#include "CMS_lumi.C"
#include "TH1.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLegend.h"
#include "TFrame.h"
#include <iostream>
#include <string>
#include <sstream>


int dolog=1;
int idbg = 0;
void Overlay_model(string atitle, string figname, string hname, string legendname,
  string fname0, string fname1, string fname2, string fname3="", string fname4="", string fname5="", 
        int nrbin = 0, double upperlimit = 0) 
{ 
  TFile *fQ = TFile::Open("SumSum2SumHistQCD.root");
  TFile *fS[6];
  TH1F  *hS[6];
  fS[0] = TFile::Open(fname0.c_str());
  fS[1] = TFile::Open(fname1.c_str());
  fS[2] = TFile::Open(fname2.c_str());
 // if (fname3[0]!='\0')
   fS[3] = TFile::Open(fname3.c_str());
  //if (fname4[0]!='\0') 
    fS[4] = TFile::Open(fname4.c_str());
  //if (fname5[0]!='\0') 
    fS[5] = TFile::Open(fname5.c_str());

  //if (idbg) std::cout<<fname3<<" "<<fS[3]<<std::endl;
  //int iFarben[6] = {2,3,6,7,402,807};
  int iFarben[6] = {3,6,2,402,807,7};
  double farbStep = 50;
  if (fname5=="") farbStep=60.;
  if (fname4=="") farbStep=84;
  if (fname3=="") farbStep=100;
  for (int i=0;i<6;i++) iFarben[i]=TColor::GetColorPalette(i*farbStep);
 
  gStyle->SetOptStat(0);
 
  TString canvName = "Fig_";
  canvName += figname;
  canvName += hname;// "hptdp_A_B";
  
  int W = 800;
  int H = 600;
  TCanvas* canv = new TCanvas(canvName,canvName,50,50,W,H);
  if (dolog) canv->SetLogy();
  // references for T, B, L, R
  float T = 0.08*H;
  float B = 0.12*H; 
  float L = 0.12*W;
  float R = 0.04*W;
  
  //canv = new TCanvas(canvName,canvName,50,50,W,H);
  canv->SetFillColor(0);
  canv->SetBorderMode(0);
  canv->SetFrameFillStyle(0);
  canv->SetFrameBorderMode(0);
  canv->SetLeftMargin( L/W );
  canv->SetRightMargin( R/W );
  canv->SetTopMargin( T/H );
  canv->SetBottomMargin( B/H );
  canv->SetTickx(0);
  canv->SetTicky(0);


  TLatex latex;
  
  int n_ = 2;
  
  float x1_l = .88;
  //  float x1_l = 0.75;
  float y1_l = 0.9;
  
  float dx_l = 0.25;
  float dy_l = 0.20;
  float x0_l = x1_l-dx_l;
  float y0_l = y1_l-dy_l;
TLegend *lgd = new TLegend(x0_l,y0_l,x1_l, y1_l); 
lgd->SetBorderSize(0); lgd->SetTextSize(0.04); lgd->SetTextFont(62); lgd->SetFillColor(0);

if (idbg) std::cout<<"getting QCD"<<std::endl;
TH1F *Q_pt = static_cast<TH1F*>(fQ->Get(hname.c_str())->Clone());
Q_pt->SetDirectory(0);
if (nrbin!=0) Q_pt->Rebin(nrbin);
double aaQ = Q_pt->Integral();
if (idbg) std::cout<<" QCD entries is "<<aaQ<<std::endl;
Q_pt->Scale(1./aaQ);
double maxi = Q_pt->GetMaximum();


for (int i=0;i<6;i++){
 if (idbg) std::cout<<"getting "<<i<<"th "<<std::endl;
 if (fS[i]) hS[i] = static_cast<TH1F*>(fS[i]->Get(hname.c_str())->Clone());
 if (fS[i]) {
    hS[i]->SetDirectory(0);
    if (nrbin!=0) hS[i]->Rebin(nrbin);
    float total = hS[i]->Integral();
    if (idbg) std::cout<<"num entries: "<<total<<std::endl;
    hS[i]->Scale(1./total);
    maxi = std::max(maxi,hS[i]->GetMaximum());
  } else if (idbg) std::cout<<"no histo"<<std::endl;

}

if (upperlimit!=0) Q_pt->SetMaximum(upperlimit);
else Q_pt->SetMaximum(maxi*15);
Q_pt->GetYaxis()->SetTitle("percent ");  
Q_pt->GetYaxis()->SetTitleSize(0.04);  
Q_pt->GetXaxis()->SetTitle(atitle.c_str());  
Q_pt->GetXaxis()->SetTitleSize(0.04);  

Q_pt->SetLineWidth(3);
//Q_pt->SetFillColor(kBlue-10);
Q_pt->SetFillColor(18);
Q_pt->SetLineColor(12);
Q_pt->SetStats(0);
Q_pt->Draw("bar");

for (int i=0;i<6;i++){
  if (fS[i]){
  hS[i]->SetMaximum(maxi*15);
  hS[i]->GetYaxis()->SetTitle("percent ");  
  hS[i]->GetYaxis()->SetTitleSize(0.04);  
  //hS[i]->GetXaxis()->SetTitle(atitle.c_str());  
  hS[i]->GetXaxis()->SetTitleSize(0.04);  

  hS[i]->SetLineColor(iFarben[i]);
  hS[i]->SetLineWidth(3);
  hS[i]->SetStats(0);
  hS[i]->Draw("same");

}}
  
//set up stringstream for legend titles
stringstream ss(legendname); string token;


lgd->AddEntry(Q_pt, "QCD", "l" );
//lgd->AddEntry(hS[0], fname0.substr(20).c_str(), "l");
for (int i=0;i<6;i++){
    if (!getline(ss,token, ',')) token ="";
    if (fS[i]) lgd->AddEntry(hS[i], token.c_str(), "l");
}
//lgd->AddEntry(hS[1], fname1.substr(20).c_str(), "l");
//lgd->AddEntry(hS[2], fname2.substr(20).c_str(), "l");
//if (fS[3]) lgd->AddEntry(hS[3], fname3.substr(20).c_str(), "l");
//if (fS[4]) lgd->AddEntry(hS[4], fname4.substr(20).c_str(), "l");
//if (fS[5]) lgd->AddEntry(hS[5], fname5.substr(20).c_str(), "l");

    // Writing the lumi information and the CMS "logo"
   // second parameter in example_plot is iPos, which drives the position of the CMS logo in the plot
  // iPos=11 : top-left, left-aligned
  // iPos=33 : top-right, right-aligned
  // iPos=22 : center, centered
  // mode generally : 
  //   iPos = 10*(alignement 1/2/3) + position (1/2/3 = left/center/right)
   
  //writeExtraText = false;       // if extra text
  extraText  = "Preliminary";  // default extra text is "Preliminary"

  int iPeriod = 0;    // 1=7TeV, 2=8TeV, 3=7+8TeV, 7=7+8+13TeV, 0=free form (uses lumi_sqrtS)
   int iPos  = 11;
   //CMS_lumi( canv, iPeriod, iPos );
  
  canv->Update();
  canv->RedrawAxis();
  canv->GetFrame()->Draw();
  lgd->SetTextSize(0.03);
  lgd->Draw();

 
  if (dolog) {
    canv->Print(canvName+"_log.pdf",".pdf");
    //canv->Print(canvName+"_log.png",".png");
  } else{ 
    canv->Print(canvName+".pdf",".pdf");
    //canv->Print(canvName+".png",".png");
  }
  return;
}


