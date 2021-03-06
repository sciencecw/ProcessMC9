/*#include <iostream>
#include <iomanip>
#include <locale>

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "vector"
#include "vector"
using std::vector;
#include "list"
#include "algorithm"

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
*/

#include "paramSet.h"

TTree          *fChain;   //!pointer to the analyzed TTree or TChain               
Int_t           fCurrent; //!current Tree number in a TChain                       


int const  iDBG=0;
int const iSDBG=1;
/*//float pilecut=1.5;
//float pilecut2=1.5;
// temporary for a3d related cuts
float a3dcut=0.1;
float ahatecut=8;
*/
//float pilecut=100000;
//float pilecut2=10000;
float deltaz=0.01;
float Njetcut=4;
float alphaMaxcut=0.03; //for plotting EJ vs nEJ plots only

//Used for overflow
float epsilon=0.0000001;
double OverFlow(Double_t value, TH1* ahist) {
   double a = std::min(value, ((ahist->GetXaxis())->GetXmax())-epsilon);
   return std::max(a,((ahist->GetXaxis())->GetXmin()));
}


float DeltaR(float eta1, float phi1, float eta2, float phi2) {

  float dR=0.;
  float deta = std::fabs(eta1-eta2);
  float dphi = std::fabs(phi1-phi2);
  if(dphi>3.14159) dphi = 2.*3.14159-dphi;
  dR=std::sqrt(deta*deta+dphi*dphi);

  return dR;
}



void BinLogX(TH1*h) 
{

   TAxis *axis = h->GetXaxis(); 
   int bins = axis->GetNbins();

   Axis_t from = axis->GetXmin();
   Axis_t to = axis->GetXmax();
   Axis_t width = (to - from) / bins;
   Axis_t *new_bins = new Axis_t[bins + 1];

   for (int i = 0; i <= bins; i++) {
     new_bins[i] = TMath::Power(10, from + i * width);
   } 
   axis->Set(bins, new_bins); 
   delete new_bins; 
}

int EMJselect(bool otfile, bool hasPre, const char* inputfilename,const char* outputfilename, const Parmset &ps, bool blind, bool isQCD
) {
  // "ntuple.root", "histos.root"
  // suggest cuts 1000., 400.,200.,125.,50.,0.2,0.9,0.9,0,1
  // right now this code hard wires the jet pT cut and requires emerging jets to have at least
  // one track with pT> 1 GeV

  int npass=0;
  TFile *f = new TFile(inputfilename);
  // get histogram of events before trigger
  TH1F *eventCountPreTrigger;

  if(hasPre) {
    if(otfile) eventCountPreTrigger = static_cast<TH1F*>(f->Get("eventCountPreTrigger/eventCountPreTrigger")->Clone());
  } else {
    if(otfile)  eventCountPreTrigger = new TH1F("eventCountPreTrigger","haha",2,0.,2.);
  }



  TTree *tt = (TTree*)f->Get("emJetAnalyzer/emJetTree");

  Int_t nVtx, event, lumi, run, nTrueInt, nTracks;
  Bool_t hltbit;
  Float_t met_pt, met_phi;

  //pv
  //float pv_x,pv_y,pv_z;
  vector<int> *pv_index=new vector<int>;
  vector<float> *pv_x = 0;
  vector<float> *pv_y = 0;
  vector<float> *pv_z = 0;


  // gen particles
  vector<int> *gp_index=new vector<int>;
  vector<int> *gp_pdgId=new vector<int>;
  vector<float> *gp_pt = new vector<float>;
  vector<float> *gp_eta = new vector<float>;
  vector<float> *gp_phi = new vector<float>;
  vector<int> *gp_charge = new vector<int>;
  vector<int> *gp_status = new vector<int>;
  vector<float> *gp_vx = new vector<float>;
  vector<float> *gp_vy = new vector<float>;
  vector<float> *gp_vz = new vector<float>;

  // jet
  vector<int> *jet_index=new vector<int>;
  vector<int> *jet_source=new vector<int>;
  vector<float> *jet_pt = new vector<float>;
  vector<float> *jet_eta = new vector<float>;
  vector<float> *jet_phi = new vector<float>;
  vector<float> *jet_alphaMax = new vector<float>;
  vector<float> *jet_theta2D = new vector<float>;
  vector<float> *jet_cef = new vector<float>;
  vector<float> *jet_nef = new vector<float>;
  vector<float> *jet_chf = new vector<float>;
  vector<float> *jet_nhf = new vector<float>;
  //  vector<float> *jet_phf = new vector<float>;
  vector<vector<float> > *track_pt = 0;
  vector<vector<float> > *track_eta = 0;
  vector<vector<float> > *track_phi = 0;
  vector<vector<float> > *track_pvWeight =0;
  vector<vector<int> > *track_source = 0;
  vector<vector<int> > *track_quality = 0;
  vector<vector<int> > *track_index = 0;
  vector<vector<int> > *track_jet_index = 0;
  vector<vector<int> > *track_algo = 0;
  vector<vector<float> > *track_ipZ =0;
  vector<vector<float> > *track_ipXY = 0;
  vector<vector<float> > *track_ipXYSig = 0;
  vector<vector<float> > *track_ip3DSig = 0;
  vector<vector<float> > *track_ref_x =0;
  vector<vector<float> > *track_ref_y =0;
  vector<vector<float> > *track_ref_z =0;
  vector<vector<int> > *track_nMissInnerHits = 0;
  vector<vector<int> > *track_nMissInnerTrkLayers = 0;
  vector<vector<int> > *track_nMissOuterTrkLayers = 0;
  vector<vector<int> > *track_nMissInnerPxlLayers = 0;
  vector<vector<int> > *track_nPxlLayers = 0;
  vector<vector<int> > *track_nHits = 0;


  
  

  //get event count pre trigger



  //for ntuple
  // gen particles
  tt->SetBranchAddress("gp_index",&gp_index);
  tt->SetBranchAddress("gp_pdgId",&gp_pdgId);
  tt->SetBranchAddress("gp_pt",&gp_pt);
  tt->SetBranchAddress("gp_eta",&gp_eta);
  tt->SetBranchAddress("gp_phi",&gp_phi);
  tt->SetBranchAddress("gp_charge",&gp_charge);
  tt->SetBranchAddress("gp_status",&gp_status);
  tt->SetBranchAddress("gp_vx",&gp_vx);
  tt->SetBranchAddress("gp_vy",&gp_vy);
  tt->SetBranchAddress("gp_vz",&gp_vz);


  // vertices
  tt->SetBranchAddress("nVtx",&nVtx);
  tt->SetBranchAddress("nTrueInt",&nTrueInt);
  tt->SetBranchAddress("nTracks",&nTracks);
  tt->SetBranchAddress("pv_x",&pv_x);
  tt->SetBranchAddress("pv_y",&pv_y);
  tt->SetBranchAddress("pv_z",&pv_z);
  tt->SetBranchAddress("pv_index",&pv_index);
//  tt->SetBranchAddress("pv_indexInColl",&pv_indexInColl);

  // general event information
  tt->SetBranchAddress("event",&event);
  tt->SetBranchAddress("lumi",&lumi);
  tt->SetBranchAddress("run",&run);
  tt->SetBranchAddress("met_pt",&met_pt);
  tt->SetBranchAddress("met_phi",&met_phi);
  tt->SetBranchAddress("HLT_PFHT900",&hltbit);

  //jets
  tt->SetBranchAddress("jet_index",&jet_index);
  tt->SetBranchAddress("jet_source",&jet_source);
  tt->SetBranchAddress("jet_pt",&jet_pt);
  tt->SetBranchAddress("jet_eta",&jet_eta);
  tt->SetBranchAddress("jet_phi",&jet_phi);
  tt->SetBranchAddress("jet_cef",&jet_cef);
  tt->SetBranchAddress("jet_nef",&jet_nef);
  tt->SetBranchAddress("jet_chf",&jet_chf);
  tt->SetBranchAddress("jet_nhf",&jet_nhf);
  //  tt->SetBranchAddress("jet_phf",&jet_phf);
  tt->SetBranchAddress("jet_alphaMax",&jet_alphaMax);
  tt->SetBranchAddress("jet_theta2D",&jet_theta2D);
  tt->SetBranchAddress("track_pt",&track_pt);
  tt->SetBranchAddress("track_eta",&track_eta);
  tt->SetBranchAddress("track_phi",&track_phi);
  tt->SetBranchAddress("track_pvWeight",&track_pvWeight);
  tt->SetBranchAddress("track_source",&track_source);
  tt->SetBranchAddress("track_quality",&track_quality);
  tt->SetBranchAddress("track_index",&track_index);
  tt->SetBranchAddress("track_jet_index",&track_jet_index);
  tt->SetBranchAddress("track_algo",&track_algo);
  tt->SetBranchAddress("track_ipXY",&track_ipXY);
  tt->SetBranchAddress("track_ipZ",&track_ipZ);
  tt->SetBranchAddress("track_ref_x",&track_ref_x);
  tt->SetBranchAddress("track_ref_y",&track_ref_y);
  tt->SetBranchAddress("track_ref_z",&track_ref_z);
  tt->SetBranchAddress("track_ipXYSig",&track_ipXYSig);
  tt->SetBranchAddress("track_ip3DSig",&track_ip3DSig);
  tt->SetBranchAddress("track_nMissInnerHits",&track_nMissInnerHits);
  tt->SetBranchAddress("track_nMissInnerPxlLayers",&track_nMissInnerPxlLayers);
  tt->SetBranchAddress("track_nMissOuterTrkLayers",&track_nMissOuterTrkLayers);
  tt->SetBranchAddress("track_nMissInnerTrkLayers",&track_nMissInnerTrkLayers);
  tt->SetBranchAddress("track_nPxlLayers",&track_nPxlLayers);
  tt->SetBranchAddress("track_nHits",&track_nHits);
  tt->SetBranchAddress("track_ipZ",&track_ipZ);

  

  

  // create a histogramsi
  TH1F *acount,*count,*hjetcut,*hjetcuta,*hjetchf,*h_naemg, *h_nemg,*hnjet,*hpt,*heta,*heta2,*halpha,*H_T,*H_T2,*H_T3,*H_T4,*hbcut_ntrkpt1,*hacut_ntrkpt1,*hbcut_nef,*hacut_nef,*hbcut_cef,*hacut_cef,*hbcut_alphamax,*hacut_alphamax,*hbcut_theta2d,*hbcut_medip,*hmetnm1,*hmet,*hmassnm1,*htheta2D1nm1,*htheta2D2nm1,*htheta2D3nm1,*htheta2D4nm1,*hHTnm1,*hHThltnm1,*hnHitsnm1,*hntrk1nm1,*hlgmedipnm1,*hlgmedipdknm1,*hlgmedipdnm1,* hmedipnm1,*hpt1nm1,*hpt2nm1,*hpt3nm1,*hpt4nm1,*halphanm1,*hnemnm1,*hpt1,*hpt2,*hpt3,*hpt4,*hipXYEJ,*hipXYnEJ,*htvw,*htvwEJ,*hnmedipnm1,*hnlgmedipnm1,*hn2medipnm1,*hjptfrb,*hjptfra1,*hjptfra2,*hjptfrbc,*hjptfra1c,*hjptfra2c,*hjptb,*hjpta,*hHTko,*hpt1ko,*hpt2ko,*hpt3ko,*hpt4ko,*hipXYSigEJ,*hipXYSignEJ,*hmedipXYEJ,*hmedipXYnEJ,*hmeanipXYEJ,*hmeanipXYnEJ,*hmass,
    *hdkjetmeanip,*hdkjetntr,*hdkjetlgmedip,*hdkjetmedip,*hdkjettrkip,*hdkjettrkips,*hdkjettrkw,*hdkjettrgip,*hdkjettrkdr,*ham2dfd,*ham2dfdk,*hdkjettrk3dsig_sgn,*hdjettrk3dsig_sgn,*hdkjettrk3dsig,*hdjettrk3dsig,*hdkjettrkahate,*hdjettrkahate,*hdzjpre,*hdzjfinal,
    *hdjetmeanip,*hdjetntr,*hdjetmedip,*hdjetlgmedip,*hdjettrkip,*hdjettrkips,*hdjettrkw,*hdjettrgip,*hdjettrkdr,*hmeanz,*hmeanzfa,*hmeanzpa,*hmeanzdk,*hmeanzd,*h2dpa,*h2dfa,*hntrkpt1zmpa,*hntrkpt1zmfa,*hbigb,*hpvpre,*hpvfinal,*hdzpre,*hdzfinal,*hmeanzpre,*hmeanzfinal,
    *hnvtxpre,*hnvtxfinal,*hntrkpre,*hntrkfinal,*hjetptfrpre,*hjetptfrfinal,
    *hjntrkpre,*hjntrkfinal,*hfpilepre,*hfpilefinal,*hptmaxpre,*hptmaxfinal,*hsum2Dfpre,*hsum2Dffinal,*hsum2Dfd,*hsum2Dfdk,
    *hptallpre,*hptudpre,*hptcpre,*hptspre,*hptgbbpre,*hptbpre,*hptgpre,
    *hptallfinal,*hptudfinal,*hptcfinal,*hptsfinal,*hptgbbfinal,*hptbfinal,*hptgfinal,
    *hptallfinal2,*hptudfinal2,*hptcfinal2,*hptsfinal2,*hptgbbfinal2,*hptbfinal2,*hptgfinal2,
    *hptallpree,*hptudpree,*hptcpree,*hptspree,*hptgbbpree,*hptbpree,*hptgpree,
    *hptallfinale,*hptudfinale,*hptcfinale,*hptsfinale,*hptgbbfinale,*hptbfinale,*hptgfinale,
    *ham2dfb,*ham2dfgbb,*ham2dfg,*ham2dfud,
    *ham2dfbpt1,*ham2dfbpt2,*ham2dfbpt3,
    *ham2dfudpt1,*ham2dfudpt2,*ham2dfudpt3,
    *hpvztrz,*hip2dsig,
    *htheta2dd, *htheta2db, *htheta2dud, *htheta2ddk, 
    *hmedipd, *hmedipb, *hmedipud, *hmedipdk
   , *hlgam2dfud, *hlgam2dfdk, *hpvztrzd  , *hip2dsigd , *hpvztrzdk , *hip2dsigdk, *hpvztrzud , *hip2dsigud, *hpvztrzb  , *hip2dsigb 
,*hmetad  ,*hmetab  ,*hmetaud ,*hmetadk,*hhasbfinal 
   ,* halphadknm1 ,* halphadnm1 
,* hip2dd    ,* hip2ddk   ,* hip2dud   ,* hip2db    ,* hmedsipd  ,* hmedsipb  ,* hmedsipud ,* hmedsipdk ;
  TH1F *hmzamd,*hmznamd,*h2damd,*h2dnamd;

  TH2F *aMip,*haMvjpt,*haMvHT,*haMvnvtx,
    *adkwvd0,*h2ipvz,*adkwviz,
    *adwvd0,*adwviz,*adk2Dr0,*ad2Dr0,*hdkipphi,*hdipphi,*hht4
    ,*h2ipvzd, *h2ipvzb, *h2ipvzud, *h2ipvzdk
    ,*hmipahated,*hmipahateud,*hmipahatedk,*hmipahateb
,*hmipa3dd  ,*hmipa3db  ,*hmipa3dud ,*hmipa3ddk 
,*hntrk1a3dd  ,*hntrk1a3db  ,*hntrk1a3dud ,*hntrk1a3ddk 
,*hmipmetad  ,*hmipmetab  ,*hmipmetaud ,*hmipmetadk 
,*hmipipsigd ,*hmipipsigb ,*hmipipsigud ,*hmipipsigdk
,*hmipmetd  ,*hmipmetb  ,*hmipmetud ,*hmipmetdk ,*hmipmassd ,*hmipmassb ,*hmipmassud ,*hmipmassdk ;

  acount = new TH1F("acount","counts",20,0.,20.);
  count = new TH1F("count","counts",3,0,3);
  count->SetStats(0);
  count->SetCanExtend(TH1::kAllAxes);

  // 1d
  hnjet = new TH1F("hnjet","Jet multiplicity",20,0.,20);
  hjetcut = new TH1F("hjetcut","jetcut counts",20,0.,20.);
  hjetcuta = new TH1F("hjetcuta","jetcut counts",20,0.,20.);
  hjetchf = new TH1F("hjetchf","jet charged hadron fr",20,0.,1.2);
  h_nemg = new TH1F("h_nemg","number of emerging jets",10,0.,10.);
  h_naemg = new TH1F("h_naemg","number of almost emerging jets",10,0.,10.);
  hpt = new TH1F("hpt","jet pt distribution",200,0.,1000.);
  heta   = new TH1F("heta","jet eta distribution",100,-4.,4.);
  heta2   = new TH1F("heta2","jet eta distribution first 4 jets",100,-4.,4.);
  halpha   = new TH1F("halpha","jet alphaMax distribution",100,0.,1.5);
  //haMgj   = new TH1F("haMgj","jet alphaMax distribution, good jets",100,0.,1.5);
  H_T      = new TH1F("H_T"," HT distribution before cut", 100,0.,5000.);
  H_T2      = new TH1F("H_T2"," HT distribution after cut", 100,0.,5000.);
  H_T3      = new TH1F("H_T3"," HT distribution at end", 100,0.,5000.);
  H_T4      = new TH1F("H_T4"," HT distribution test", 100,0.,5000.);
  hpt1 = new TH1F("hpt1"," pT of leading jet",200,0.,1000.);
  hpt2 = new TH1F("hpt2"," pT of second jet",200,0.,1000.);
  hpt3 = new TH1F("hpt3"," pT of third jet",200,0.,1000.);
  hpt4 = new TH1F("hpt4"," pT of fourth jet",200,0.,1000.);
  hbcut_ntrkpt1 = new TH1F("hbcut_ntrkpt1","number tracks pt>1 before cut",20,0.,20.);
  hacut_ntrkpt1 = new TH1F("hacut_ntrkpt1","number tracks pt>1 after cut",20,0.,20.);
  hbcut_nef = new TH1F("hbcut_nef","neutral em fraction before cut",10,0.,1.2);
  hacut_nef = new TH1F("hacut_nef","neutral em fraction after cut",10,0.,1.2);
  hbcut_cef = new TH1F("hbcut_cef","charged em fraction before cut",50,0.,1.2);
  hacut_cef = new TH1F("hacut_cef","charged em fraction after cut",50,0.,1.2);
  hbcut_alphamax = new TH1F("hbcut_alphamax","alphamax before cut",50,0.,1.5);
  hacut_alphamax = new TH1F("hacut_alphamax","alphamax after cut",50,0.,1.5);
  hbcut_medip = new TH1F("hbcut_medip","medIP before cut",200,0.,0.5);
  hbcut_theta2d = new TH1F("hbcut_theta2d","log theta2d before cut",50,-3.5,0.5);
  hHTnm1 = new TH1F("hHTnm1","HT n-1 without PFHLT800",100,0.,5000.);
  hHThltnm1 = new TH1F("hHThltnm1","HT n-1",100,0,5000);
  hmassnm1 = new TH1F("hmassnm1","mass n-1",100,-10.,5000.);
  htheta2D1nm1 = new TH1F("htheta2D1nm1","log10 theta2D jet1 n-1",50,-3.5,0.5);
  htheta2D2nm1 = new TH1F("htheta2D2nm1","log10 theta2D jet2 n-1",50,-3.5,0.5);
  htheta2D3nm1 = new TH1F("htheta2D3nm1","log10 theta2D jet3 n-1",50,-3.5,0.5);
  htheta2D4nm1 = new TH1F("htheta2D4nm1","log10 theta2D jet4 n-1",50,-3.5,0.5);
  hmetnm1 = new TH1F("hmetnm1","MET n-1",100,0.,300.);
  hmet = new TH1F("hmet","MET",100,0.,500.);
  hpt1nm1 = new TH1F("hpt1nm1","pt1 n-1",20,0.,1000.);
  hpt2nm1 = new TH1F("hpt2nm1","pt2 n-1",20,0.,1000.);
  hpt3nm1 = new TH1F("hpt3nm1","pt3 n-1",20,0.,1000.);
  hpt4nm1 = new TH1F("hpt4nm1","pt4 n-1",20,0.,1000.);
  halphanm1 = new TH1F("halphanm1","alpha max n-1",200,0.,1.);
  halphadnm1 = new TH1F("halphadnm1","alpha3d n-1",200,0.,1.);
  halphadknm1 = new TH1F("halphadknm1","alpha3d n-1",200,0.,1.);
  hmedipnm1 = new TH1F("hmedipnm1","medIP n-1",200,0.,10.);
  hlgmedipnm1 = new TH1F("hlgmedipnm1","log medIP n-1",80,-5.,3.);
  hlgmedipdknm1 = new TH1F("hlgmedipdknm1","log(medIP) n-1 dark jets",80,-5.,3.);
  hlgmedipdnm1 = new TH1F("hlgmedipdnm1","log(medIP) n-1 dark jets",80,-5.,3.);
  hnmedipnm1 = new TH1F("hnmedipnm1","new 2 medIP n-1",200,0.,10.);
  hnlgmedipnm1 = new TH1F("hnlgmedipnm1","new 2 medIP n-1",80,-5.,3.);
  hn2medipnm1 = new TH1F("hn2medipnm1","new 1 medIP n-1",200,0.,10.);
  hnHitsnm1 = new TH1F("hnHitsnm1","number Hits n-1",40,0.,40.);
  hntrk1nm1 = new TH1F("hntrk1nm1","number tracks pt>1 n-1",50,0.,50.);
  hnemnm1 = new TH1F("hnemnm1","N emerging jets n-1",10,0.,10.);
  hipXYEJ = new TH1F("hipXYEJ","impact parameter  tracks of emerging jets",300,0,0.2);
  hipXYnEJ = new TH1F("hipXYnEJ","impact parameter  tracks of not emerging jets",300,0.,0.2);
  htvw = new TH1F("htvw","track vertex weight ",150,-5.,1.);
  htvwEJ= new TH1F("htvwEJ","track vertex weight Emerging Jets ",15,-5.,10.);
  hipXYSigEJ = new TH1F("hipXYSigEJ","ip sig emerging jets",100,0.,10.);
  hipXYSignEJ = new TH1F("hipXYSignEJ","ip sig not emerging jets",100,0.,10.);
  hmedipXYEJ = new TH1F("hmedipXYEJ","med ip emerging jets",100,0.,2.);
  hmedipXYnEJ = new TH1F("hmedipXYnEJ","med ip not emerging jets",100,0.,2.);
  hmeanipXYEJ = new TH1F("hmeanipXYEJ","mean ip emerging jets",200,0.,2.);
  hmeanipXYnEJ = new TH1F("hmeanipXYnEJ","mean ip not emerging jets",200,0.,2.);
  hjptb = new TH1F("hjptb"," pT of basic jet",100,0.,1000.);
  hjpta = new TH1F("hjpta"," pT of emergng jets",100,0.,1000.);
  hjptfrb = new TH1F("hjptfrb"," pT of basic jets passing kine selection and n<4",100,0.,1000.);
  hjptfra1 = new TH1F("hjptfra1"," pT of basic jets passing kine, almost selection and n<4",100,0.,1000.);
  hjptfra2 = new TH1F("hjptfra2"," pT of basic jets passing kine, almost, and emerging selection and n<4",100,0.,1000.);
  hjptfrbc = new TH1F("hjptfrbc"," pT of basic jets passing kine selection",100,0.,1000.);
  hjptfra1c = new TH1F("hjptfra1c"," pT of basic jets passing kine, almost selection",100,0.,1000.);
  hjptfra2c = new TH1F("hjptfra2c"," pT of basic jets passing kine, almost, and emerging selection",100,0.,1000.);
  hHTko      = new TH1F("hHTko"," HT distribution test kine cuts", 100,0.,5000.);
  hpt1ko = new TH1F("hpt1ko"," pT of leading jet kine cuts",200,0.,1000.);
  hpt2ko = new TH1F("hpt2ko"," pT of second jet kine cuts",200,0.,1000.);
  hpt3ko = new TH1F("hpt3ko"," pT of third jet kine cuts",200,0.,1000.);
  hpt4ko = new TH1F("hpt4ko"," pT of fourth jet kine cuts",200,0.,1000.);
  hmass = new TH1F("hmass","mass emerging and non",100,-10.,2000.);

  /*hdkjetam = new TH1F("hdkjetam","alphamax dark quark jets ",100,0.,1.);
  hdkjetamo = new TH1F("hdkjetamo","old alphamax dark quark jets ",100,0.,1.);
  hdjetamo = new TH1F("hdjetamo","old alphamax down quark jets ",100,0.,1.);
  hdkjetam2d = new TH1F("hdkjetam2d","alphamax2d dark quark jets ",100,0.,1.);
  */
  ham2dfd = new TH1F("ham2dfd","a3d down quark jets ",100,0.,1.);
  ham2dfb = new TH1F("ham2dfb","a3d b quark jets ",100,0.,1.);
  ham2dfgbb = new TH1F("ham2dfgbb","a3d glue to bb quark jets ",100,0.,1.);
  ham2dfg = new TH1F("ham2dfg","a3d  glu jets ",100,0.,1.);
  ham2dfud = new TH1F("ham2dfud","a3d u,d quark jets ",100,0.,1.);
  ham2dfdk = new TH1F("ham2dfdk","a3d dark quark jets ",100,0.,1.);
  hlgam2dfdk = new TH1F("hlgam2dfdk","log(a3d) dark quark jets ",100,-4.,0.);
  hlgam2dfud = new TH1F("hlgam2dfud","log(a3d) down quark jets ",100,-4.,0.);

  ham2dfbpt1 = new TH1F("ham2dfbpt1","a3d b quark jets pt 100-300 ",100,0.,1.);
  ham2dfbpt2 = new TH1F("ham2dfbpt2","a3d b quark jets pt 300-600 ",100,0.,1.);
  ham2dfbpt3 = new TH1F("ham2dfbpt3","a3d b quark jets pt >600 ",100,0.,1.);
  ham2dfudpt1 = new TH1F("ham2dfudpt1","a3d ud quark jets pt 100-300 ",100,0.,1.);
  ham2dfudpt2 = new TH1F("ham2dfudpt2","a3d ud quark jets pt 300-600 ",100,0.,1.);
  ham2dfudpt3 = new TH1F("ham2dfudpt3","a3d ud quark jets pt >600 ",100,0.,1.);

htheta2dd = new TH1F("htheta2dd","log theta2d down quark jets ",100,-3.5,0.5);
htheta2db = new TH1F("htheta2db","log theta2d b quark jets ",100,-3.5,0.5);
htheta2dud = new TH1F("htheta2dud","log theta2d u,d quark jets ",100,-3.5,0.5);
htheta2ddk = new TH1F("htheta2ddk","log theta2d dark quark jets ",100,-3.5,0.5);

   hpvztrzd  = new TH1F("hpvztrzd","|PVz - track_ref_z| down quark;pvz-trz",400,-5,5);
   hip2dsigd = new TH1F("hip2dsigd","ip2dsig down quark; ip2dsig",400,-80,80);
   hpvztrzdk = new TH1F("hpvztrzdk","|PVz - track_ref_z| dark quark;pvz-trz",400,-5,5);
   hip2dsigdk= new TH1F("hip2dsigdk","ip2dsig dark quark; ip2dsig",400,-80,80);
   hpvztrzud = new TH1F("hpvztrzud","|PVz - track_ref_z| ud quark;pvz-trz",400,-5,5);
   hip2dsigud= new TH1F("hip2dsigud","ip2dsig ud quark; ip2dsig",400,-80,80);
   hpvztrzb  = new TH1F("hpvztrzb","|PVz - track_ref_z| b quark;pvz-trz",400,-5,5);
   hip2dsigb = new TH1F("hip2dsigb","ip2dsig b quark; ip2dsig",400,-80,80);

   hip2dd    = new TH1F("hip2dd" ,"ip2d down quark; ip2d",400,-2,2);
   hip2ddk   = new TH1F("hip2ddk","ip2d dark quark; ip2d",400,-2,2);
   hip2dud   = new TH1F("hip2dud","ip2d ud quark  ; ip2d",400,-2,2);
   hip2db    = new TH1F("hip2db" ,"ip2d b quark   ; ip2d",400,-2,2);
   hmedsipd  = new TH1F("hmedsipd", "median signed 2DIP down quark jets ",100,-1,1);
   hmedsipb  = new TH1F("hmedsipb", "median signed 2DIP b quark jets "   ,100,-1,1);
   hmedsipud = new TH1F("hmedsipud","median signed 2DIP u,d quark jets " ,100,-1,1);
   hmedsipdk = new TH1F("hmedsipdk","median signed 2DIP dark quark jets ",100,-1,1);

hmedipd = new TH1F("hmedipd","median 2DIP down quark jets ",100,0.,10.);
hmedipb = new TH1F("hmedipb","median 2DIP b quark jets ",100,0.,10.);
hmedipud = new TH1F("hmedipud","median 2DIP u,d quark jets ",100,0.,10.);
hmedipdk = new TH1F("hmedipdk","median 2DIP dark quark jets ",100,0.,10.);

hmetad  = new TH1F("hmetad" ,"met jet-component down quark jets ",100,-1000.,1000.);
hmetab  = new TH1F("hmetab" ,"met jet-component b quark jets "   ,100,-1000.,1000.);
hmetaud = new TH1F("hmetaud","met jet-component u,d quark jets " ,100,-1000.,1000.);
hmetadk = new TH1F("hmetadk","met jet-component dark quark jets ",100,-1000.,1000.);


  hdjettrkahate = new TH1F("hdjettrkahate","ahate in down quark jets",300,-2,4);
  hdkjettrkahate = new TH1F("hdkjettrkahate","ahate in dark quark jets",300,-2,4);
  hdjettrk3dsig = new TH1F("hdjettrk3dsig","3dsig for down quark jets",300,-2,4);
  hdkjettrk3dsig = new TH1F("hdkjettrk3dsig","3dsig for dark quark jets",300,-2,4);
  BinLogX(hdjettrk3dsig);
  BinLogX(hdjettrkahate);
  BinLogX(hdkjettrkahate);
  BinLogX(hdkjettrk3dsig);
  hdjettrk3dsig_sgn = new TH1F("hdjettrk3dsig_sgn","3dsig for down quark jets",300,-300,300);
  hdkjettrk3dsig_sgn = new TH1F("hdkjettrk3dsig_sgn","3dsig for dark quark jets",300,-300,300);


  hdkjetmeanip = new TH1F("hdkjetmeanip","mean ip dark quark jets",100,0.,10.);
  hdkjetmedip = new TH1F("hdkjetmedip","med ip dark quark jets",100,0.,10.);
  hdkjetlgmedip = new TH1F("hdkjetlgmedip","log med ip dark quark jets",80,-5.,3.);
  hdkjetntr = new TH1F("hdkjetntr","number tracks pt>1 dark quark jets",100,0.,50.);
  hdkjettrkip = new TH1F("hdkjettrkip","2d ip tracks in dark quark jets",100,0.,2.);
  hdkjettrkips = new TH1F("hdkjettrkips","2d ip sig tracks in dark quark jets",100,-5.,5.);
  hdkjettrkw = new TH1F("hdkjettrkw","track pv weight in dark quark jets",100,-1.2,1.2);
  hdkjettrgip = new TH1F("hdkjettrgip","gen r0 charged part in dark quark jets",100,0.,5.);
  hdkjettrkdr= new TH1F("hdkjettrkdr","gen trk dr charged part in dark quark jets",100,0.,1.);

  //hdjetam = new TH1F("hdjetam","alphamax down quark jets ",100,0.,1.);
  //hdjetam2d = new TH1F("hdjetam2d","alphamax2d down quark jets ",100,0.,1.);
  hdjetmeanip = new TH1F("hdjetmeanip","mean ip down quark jets",100,0.,10.);
  hdjetmedip = new TH1F("hdjetmedip","med ip down quark jets",100,0.,10.);
  hdjetlgmedip = new TH1F("hdjetlgmedip","log med ip down quark jets",80,-5.,3.);
  hdjetntr = new TH1F("hdjetntr","number tracks pt>1 down quark jets",100,0.,50.);
  hdjettrkip = new TH1F("hdjettrkip","2d ip tracks in down quark jets",100,0.,2.);
  hdjettrkips = new TH1F("hdjettrkips","2d ip sig tracks in down quark jets",100,-5.,5.);
  hdjettrkw = new TH1F("hdjettrkw","track pv weight in down quark jets",100,-1.2,1.2);
  hdjettrgip = new TH1F("hdjettrgip","gen r0 charged part in down quark jets",100,0.,5.);
  hdjettrkdr = new TH1F("hdjettrkdr","gen trk dr charged part in down quark jets",100,0.,1.);

  hmeanz = new TH1F("hmeanz","diff pvz and mean jet z",100,-.5,.5);
  hmeanzd = new TH1F("hmeanzd","diff pvz and mean jet z down quarks",100,-5.,5.);
  hmeanzdk = new TH1F("hmeanzdk","diff pvz and mean jet z dark quarks",100,-5.,5.);

  hmeanzfa = new TH1F("hmeanzfa","diff pvz and mean jet z failling almost emerging",500,-5.,5.);
  hmeanzpa = new TH1F("hmeanzpa","diff pvz and mean jet z pass almost emerging",500,-5.,5.);


  hmeanzpre = new TH1F("hmeanzpre","diff pvz and mean jet z preselection",500,-5.,5.);
  hmeanzfinal = new TH1F("hmeanzfinal","diff pvz and mean jet z pass final selection",500,-5.,5.);

  hdzjpre = new TH1F("hdzjpre","max diff median z 4 jets preselection",250,0.,20.);
  hdzjfinal = new TH1F("hdzjfinal","max diff median z 4 jets final selection" ,250,0.,20.);

  hsum2Dfpre = new TH1F("hsum2Dfpre","fract track  ahate  preselection",100,0.,1.1); 
  hsum2Dffinal = new TH1F("hsum2Dffinal","fract track ahate final selection",100,0.,1.1);
  hsum2Dfd = new TH1F("hsum2Dfd","fract track ahate down quarks",100,0.,1.1);
  hsum2Dfdk = new TH1F("hsum2Dfdk","fract track ahate dark quarks",100,0.,1.1);

  hntrkpt1zmpa = new TH1F("hntrkpt1zmpa","number tracks in jet matching pv pass almost",30,0.,30.);
  hntrkpt1zmfa = new TH1F("hntrkpt1zmfa","number tracks in jet matching pv failing almost",30,0.,30.);

  h2dpa = new TH1F("h2dpa","alpha2d z pass almost emerging",100,0.,1.);
  h2dfa = new TH1F("h2dfa","alpha2d z fail almost emerging",100,0.,1.);
  hmzamd = new TH1F("hmzamd","meanz down quarks almost emerging",100,-7.,7.);
  hmznamd = new TH1F("hmznamd","meanz down quarks not almost emerging",100,-7.,7.);
  h2damd = new TH1F("h2damd","alpha2d down quarks almost emerging",100,0.,1.);
  h2dnamd = new TH1F("h2dnamd","alpha2d down quarks not almost emerging",100,0.,1.);

  hbigb = new TH1F("hbigb","delta R emerging jet and nearest big b",100,-2,5.);
hhasbfinal = new TH1F("hhasbfinal","B jet",5,0,5);
  hpvpre = new TH1F("hpvpre","pv z preselection",100,-30.,30.);
  hpvfinal = new TH1F("hpvfinal","pv z final",100,-30.,30.);

  hdzpre = new TH1F("hdzpre","delta z pv track  preselection",400,-0.1,0.1);
  hdzfinal = new TH1F("hdzfinal","delta z pv track final",400,-0.1,0.1);


  hnvtxpre = new TH1F("hnvtxpre","nvtx  preselection",50,-0.,100.);
  hnvtxfinal = new TH1F("hnvtxfinal","nvtx final",50,0.,100.);

  hntrkpre = new TH1F("hntrkpre","ntrk  preselection",100,0.,3000.);
  hntrkfinal = new TH1F("hntrkfinal","nntrk final",100,0.,3000.);


  hjntrkpre = new TH1F("hjntrkpre","jet ntrk  preselection",100,0.,100.);
  hjntrkfinal = new TH1F("hjntrkfinal","jet nntrk final",100,0.,100.);


  hjetptfrpre = new TH1F("hjetptfrpre","fract jet pt leading track  preselection",100,0.,1.5);
  hjetptfrfinal = new TH1F("hjetptfrfinal","fract jet pt leading track final",100,0.,1.5);

  hfpilepre = new TH1F("hfpilepre","fract jet pt pileup  preselection",100,0.,1.5);
  hfpilefinal = new TH1F("hfpilefinal","fract jet pt pileup  final selection",100,0.,1.5);


  hptmaxpre = new TH1F("hptmaxpre","pt highest pt track preselection",400,0.,400.);
  hptmaxfinal = new TH1F("hptmaxfinal","pt highest pt track final selection",400,0.,400.);

  hptallpre = new TH1F("hptallpre","pt all jets passing preselection",500,0.,1500.);
  hptudpre = new TH1F("hptudpre","pt ud jets passing preselection",500,0.,1500.);
  hptspre = new TH1F("hptspre","pt s jets passing preselection",500,0.,1500.);
  hptcpre = new TH1F("hptcpre","pt c jets passing preselection",500,0.,1500.);
  hptbpre = new TH1F("hptbpre","pt b jets passing preselection",500,0.,1500.);
  hptgpre = new TH1F("hptgpre","pt g jets passing preselection",500,0.,1500.);
  hptgbbpre = new TH1F("hptgbbpre","pt gbb jets passing preselection",500,0.,1500.);

  hptallpree = new TH1F("hptallpree","pt allemerging jets passing preselection",500,0.,1500.);
  hptudpree = new TH1F("hptudpree","pt ud emerging jets passing preselection",500,0.,1500.);
  hptspree = new TH1F("hptspree","pt s emerging jets passing preselection",500,0.,1500.);
  hptcpree = new TH1F("hptcpree","pt c emerging jets passing preselection",500,0.,1500.);
  hptbpree = new TH1F("hptbpree","pt b emerging jets passing preselection",500,0.,1500.);
  hptgpree = new TH1F("hptgpree","pt g emerging jets passing preselection",500,0.,1500.);
  hptgbbpree = new TH1F("hptgbbpree","pt gbb emerging jets passing preselection",500,0.,1500.);


  hptallfinal = new TH1F("hptallfinal","pt all jets passing 1 emerging",500,0.,1500.);
  hptudfinal = new TH1F("hptudfinal","pt ud jets passing 1 emerging",500,0.,1500.);
  hptsfinal = new TH1F("hptsfinal","pt s jets passing 1 emerging",500,0.,1500.);
  hptcfinal = new TH1F("hptcfinal","pt c jets passing 1 emerging",500,0.,1500.);
  hptbfinal = new TH1F("hptbfinal","pt b jets passing 1 emerging",500,0.,1500.);
  hptgfinal = new TH1F("hptgfinal","pt g jets passing 1 emerging",500,0.,1500.);
  hptgbbfinal = new TH1F("hptgbbfinal","pt gbb jets 1 emerging",500,0.,1500.);

  hptallfinal2 = new TH1F("hptallfinal2","pt all jets passing 2 emerging",500,0.,1500.);
  hptudfinal2 = new TH1F("hptudfinal2","pt ud jets passing 2 emerging",500,0.,1500.);
  hptsfinal2 = new TH1F("hptsfinal2","pt s jets passing 2 emerging",500,0.,1500.);
  hptcfinal2 = new TH1F("hptcfinal2","pt c jets passing 2 emerging",500,0.,1500.);
  hptbfinal2 = new TH1F("hptbfinal2","pt b jets passing 2 emerging",500,0.,1500.);
  hptgfinal2 = new TH1F("hptgfinal2","pt g jets passing 2 emerging",500,0.,1500.);
  hptgbbfinal2 = new TH1F("hptgbbfinal2","pt gbb jets passing 2 emerging",500,0.,1500.);

  hptallfinale = new TH1F("hptallfinale","pt allemerging jets passing 1 emerging",500,0.,1500.);
  hptudfinale = new TH1F("hptudfinale","pt ud emerging jets passing 1 emerging",500,0.,1500.);
  hptsfinale = new TH1F("hptsfinale","pt s emerging jets passing 1 emerging",500,0.,1500.);
  hptcfinale = new TH1F("hptcfinale","pt c emerging jets passing 1 emerging",500,0.,1500.);
  hptbfinale = new TH1F("hptbfinale","pt b emerging jets passing 1 emerging",500,0.,1500.);
  hptgfinale = new TH1F("hptgfinale","pt g emerging jets passing 1 emerging",500,0.,1500.);
  hptgbbfinale = new TH1F("hptgbbfinale","pt gbb emerging jets passing 1 emerging",500,0.,1500.);
   hpvztrz = new TH1F("hpvztrz","|PVz - track_ref_z|",400,-2,2);
   hip2dsig = new TH1F("hip2dsig","ip2dsig",400,-40,40);


  //2d
  aMip = new TH2F("aMip"," alpha Max versus max IP n-1 plot",100,0.,1.,100,0.,10.);
  haMvjpt = new TH2F("haMvjpt"," alpha Max versus jet pT ",100,0.,1.,100,0.,700.);
  haMvHT = new TH2F("haMvHT"," alpha Max versus HT ",100,0.,1.,100,0.,2500.);
  haMvnvtx = new TH2F("haMvnvtx"," alpha Max versus nvtx ",40,0.,1.,100,0.,40.);

  /*aMbh = new TH2F("aMbh"," alpha Max versus alphaMax by hand",100,0.,1.2,100,0.,1.2);
  aMbh2D = new TH2F("aMbh2D"," alpha2D versus alphaMax by hand",100,0.,1.2,100,0.,1.2);
  aMbh2Dd = new TH2F("aMbh2Dd"," alpha2D versus alphaMax by hand down quarks",100,0.,1.2,100,0.,1.2);
  aMbh2Ddk = new TH2F("aMbh2Ddk"," alpha2D versus alphaMax by hand dark quarks",100,0.,1.2,100,0.,1.2);
  aMbh2Daem = new TH2F("aMbh2Daem"," alpha2D versus alphaMax by hand failling almost emerging",100,0.,1.2,100,0.,1.2);
  aMmzd = new TH2F("aMmzd"," meanz-pv versus alphaMax by hand down quarks",100,0.,1.2,100,0.,3.0);
  aMmzdk = new TH2F("aMmzdk"," manz-pv versus alphaMax by hand dark quarks",100,0.,1.2,100,0.,3.);
*/
h2ipvzd =  new TH2F("h2ipvzd","ip2dsig vs pvz-track_ref_z down jets" ,100,-50,50,100,-.5,.5);
h2ipvzb =  new TH2F("h2ipvzb","ip2dsig vs pvz-track_ref_z b jets"    ,100,-50,50,100,-.5,.5);
h2ipvzud = new TH2F("h2ipvzud","ip2dsig vs pvz-track_ref_z u,d jets" ,100,-50,50,100,-.5,.5);
h2ipvzdk = new TH2F("h2ipvzdk","ip2dsig vs pvz-track_ref_z dark jets",100,-50,50,100,-.5,.5);

  adkwvd0 = new TH2F("adkwvd0","weight versus ip dark quark jets",100,-1.2,1.2,100,0.,6.);
  adwvd0 = new TH2F("adwvd0","weight versus ip down quark jets",100,-1.2,1.2,100,0.,6.);
  adkwviz = new TH2F("adkwviz","weight versus 3Dip dark quark jets",100,-1.2,1.2,100,0.,6.);
  adwviz = new TH2F("adwviz","weight versus 3Dip down quark jets",100,-1.2,1.2,100,0.,6.);
  adk2Dr0 = new TH2F("adk2Dr0"," 2dIP reco vs 2D matched gen part creation pont tracks dark jets",100,0.,2.,100,0.,0.4);
  ad2Dr0 = new TH2F("ad2Dr0"," 2dIP reco vs 2D matched gen part creation pont tracks down jets",100,0.,2.,100,0.,0.4);
  hdkipphi = new TH2F("hdkipphi","2Dip v phi dark quarks",100,-3.2,3.2,100,0.,0.4);
  hdipphi = new TH2F("hdipphi","2Dip v phi down quarks",100,-3.2,3.2,100,0.,0.4);
  h2ipvz = new TH2F("h2ipvz","ip2dsig vs pvz-track_ref_z",100,-1000,1000,100,-10,10);
  hht4 = new TH2F("hht4","HT of all jets vs HT of leading 4 jets",250,0,2500,250,0,2500);


hmipahated  = new TH2F("hmipahated","median 2DIP vs ahate down quark jets " ,100,-4,1.5,200,0,50);
hmipahateb  = new TH2F("hmipahateb","median 2DIP vs ahate b quark jets "    ,100,-4,1.5,200,0,50);
hmipahateud = new TH2F("hmipahateud","median 2DIP vs ahate u,d quark jets " ,100,-4,1.5,200,0,50);
hmipahatedk = new TH2F("hmipahatedk","median 2DIP vs ahate dark quark jets ",100,-4,1.5,200,0,50);

hmipipsigd  = new TH2F("hmipipsigd","median 2DIP vs ipsig down quark jets " ,100,-4,1.5,100,0,10);
hmipipsigb  = new TH2F("hmipipsigb","median 2DIP vs ipsig b quark jets "    ,100,-4,1.5,100,0,10);
hmipipsigud = new TH2F("hmipipsigud","median 2DIP vs ipsig u,d quark jets " ,100,-4,1.5,100,0,10);
hmipipsigdk = new TH2F("hmipipsigdk","median 2DIP vs ipsig dark quark jets ",100,-4,1.5,100,0,10);

hmipa3dd  = new TH2F("hmipa3dd", "log median 2DIP vs a3d down quark jets "  ,100,-4,1.5,100,0,1);
hmipa3db  = new TH2F("hmipa3db", "log median 2DIP vs a3d b quark jets "     ,100,-4,1.5,100,0,1);
hmipa3dud = new TH2F("hmipa3dud","log median 2DIP vs a3d u,d quark jets "   ,100,-4,1.5,100,0,1);
hmipa3ddk = new TH2F("hmipa3ddk","log median 2DIP vs a3d dark quark jets "  ,100,-4,1.5,100,0,1);

hmipmetd  = new TH2F("hmipmetd", "log median 2DIP vs met down quark jets "  ,100,-4,1.5,100,0,1000);
hmipmetb  = new TH2F("hmipmetb", "log median 2DIP vs met b quark jets "     ,100,-4,1.5,100,0,1000);
hmipmetud = new TH2F("hmipmetud","log median 2DIP vs met u,d quark jets "   ,100,-4,1.5,100,0,1000);
hmipmetdk = new TH2F("hmipmetdk","log median 2DIP vs met dark quark jets "  ,100,-4,1.5,100,0,1000);
hmipmetad  = new TH2F("hmipmetad", "log median 2DIP vs met in jet direction down quark jets "  ,100,-4,1.5,100,-1000,1000);
hmipmetab  = new TH2F("hmipmetab", "log median 2DIP vs met in jet direction b quark jets "     ,100,-4,1.5,100,-1000,1000);
hmipmetaud = new TH2F("hmipmetaud","log median 2DIP vs met in jet direction u,d quark jets "   ,100,-4,1.5,100,-1000,1000);
hmipmetadk = new TH2F("hmipmetadk","log median 2DIP vs met in jet direction dark quark jets "  ,100,-4,1.5,100,-1000,1000);

hmipmassd  = new TH2F("hmipmassd", "log median 2DIP vs mass down quark jets "  ,100,-4,1.5,100,0,2000);
hmipmassb  = new TH2F("hmipmassb", "log median 2DIP vs mass b quark jets "     ,100,-4,1.5,100,0,2000);
hmipmassud = new TH2F("hmipmassud","log median 2DIP vs mass u,d quark jets "   ,100,-4,1.5,100,0,2000);
hmipmassdk = new TH2F("hmipmassdk","log median 2DIP vs mass dark quark jets "  ,100,-4,1.5,100,0,2000);

hntrk1a3dd  = new TH2F("hntrk1a3dd", "Ntrk>1GeV vs a3d down quark jets "  ,20,0,20,100,0,1);
hntrk1a3db  = new TH2F("hntrk1a3db", "Ntrk>1GeV vs a3d b quark jets "     ,20,0,20,100,0,1);
hntrk1a3dud = new TH2F("hntrk1a3dud","Ntrk>1GeV vs a3d u,d quark jets "   ,20,0,20,100,0,1);
hntrk1a3ddk = new TH2F("hntrk1a3ddk","Ntrk>1GeV vs a3d dark quark jets "  ,20,0,20,100,0,1);

  //read all entries and fill the histograms
  Int_t nentries = (Int_t)tt->GetEntries();


  // loop over events
  for (Int_t i=0; i<nentries; i++) {
 
    if(!hasPre) eventCountPreTrigger->Fill(1.5); 
    
    if(otfile) count->Fill("All",1);  // count number of events
    if(otfile) acount->Fill(0.5);
    tt->GetEntry(i);




    if(iDBG>2) std::cout<<"***run event lumi "<<run<<" "<<event<<" "<<lumi<<std::endl;

    if(iDBG>2) {
      std::cout<<" number of vertex is "<<nVtx<<std::endl;
      std::cout<<"pv position is "<<pv_x->at(0)<<","<<pv_y->at(0)<<","<<pv_z->at(0)<<std::endl;
    }

    // make some basic plots on all events before any selections




    // gen particles
    // especially find the first dark quark and dark anti quark
    // assume the following particle is d or dbar
    int firstdkq=0;
    int firstadkq=0;
    int firstdq=0;
    int firstadq=0;

    std::vector<int> bigbs;
    std::vector<int> ts;

    int NNNgp = (*gp_index).size();
    if(iDBG>2) std::cout<<" gen particle id pt eta phi"<<std::endl;
    for(Int_t j=1; j<NNNgp-1; j++) {
      //	if(iDBG>2) std::cout<<"    "<<(*gp_pdgId)[j]<<" "<<(*gp_pt)[j]<<" "<<(*gp_eta)[j]<<" "<<(*gp_phi)[j]<<std::endl;

      // for background, find high pt b's
      if(abs(gp_pdgId->at(j))==5) {
	if(gp_pt->at(j)>50) {
	  if(fabs(gp_eta->at(j))<4) {
	  if(int(bigbs.size())==0) {
	      bigbs.push_back(j);
	    } else {
	      int anewone=0;
	      for(int k=0;k<int(bigbs.size());k++ ) {
	        float ggg = DeltaR(gp_eta->at(j),gp_phi->at(j),gp_eta->at(bigbs[k]),gp_phi->at(bigbs[k]));
	        if(ggg<0.4) {
		  anewone=1;
	        }
	      }
	      if(anewone==0) bigbs.push_back(j);
	    }
          if(iDBG>2) std::cout<<" BIG B   "<<(*gp_pdgId)[j]<<" "<<(*gp_pt)[j]<<" "<<(*gp_eta)[j]<<" "<<(*gp_phi)[j]<<" current size is "<<int(bigbs.size())<<std::endl;

	  }}
      }

      // find ts
      if(abs(gp_pdgId->at(j))==6) {
        if(int(ts.size())==0) {
	  ts.push_back(j);
	} else {
	  int anewone=0;
	  for(int k=0;k<int(ts.size());k++ ) {
	    float ggg = DeltaR(gp_eta->at(j),gp_phi->at(j),gp_eta->at(ts[k]),gp_phi->at(ts[k]));
	    if(ggg<0.4) {
		  anewone=1;
	    }
	  }
	  if(anewone==0) ts.push_back(j);
	}
          if(iDBG>2) std::cout<<" top   "<<(*gp_pdgId)[j]<<" "<<(*gp_pt)[j]<<" "<<(*gp_eta)[j]<<" "<<(*gp_phi)[j]<<" current size is "<<int(ts.size())<<std::endl;

      }
      


      // for signal, find the daughters
      if(((*gp_pdgId)[j]==4900101)&&(firstdkq==0)
           &&( ((*gp_pdgId)[j+1]==1)||((*gp_pdgId)[j-1]==1)) ) {
	firstdkq=j;
	firstdq=j+1;
	if((*gp_pdgId)[j+1]!=1) firstdq=j-1;
	if(iDBG>2) std::cout<<"    match "<<(*gp_pdgId)[firstdkq]<<" "<<(*gp_pt)[firstdkq]<<" "<<(*gp_eta)[firstdkq]<<" "<<(*gp_phi)[firstdkq]<<std::endl;
	if(iDBG>2) std::cout<<"    match "<<(*gp_pdgId)[firstdq]<<" "<<(*gp_pt)[firstdq]<<" "<<(*gp_eta)[firstdq]<<" "<<(*gp_phi)[firstdq]<<std::endl;
      }
      if(((*gp_pdgId)[j]==-4900101)&&(firstadkq==0)
	 &&( ((*gp_pdgId)[j+1]==-1)||((*gp_pdgId)[j-1]==-1)) ) {
	firstadkq=j;
	firstadq=j+1;
	if((*gp_pdgId)[j+1]!=-1) firstadq=j-1;
	if(iDBG>2) std::cout<<"    match "<<(*gp_pdgId)[firstadkq]<<" "<<(*gp_pt)[firstadkq]<<" "<<(*gp_eta)[firstadkq]<<" "<<(*gp_phi)[firstadkq]<<std::endl;
	if(iDBG>2) std::cout<<"    match "<<(*gp_pdgId)[firstadq]<<" "<<(*gp_pt)[firstadq]<<" "<<(*gp_eta)[firstadq]<<" "<<(*gp_phi)[firstadq]<<std::endl;

      }
    }
    if(iDBG>2) {std::cout<<std::endl<<std::endl;
    if(firstdkq==0) std::cout<<" first dark quark not found"<<std::endl;
    if(firstdq==0) std::cout<<" first down quark not found"<<std::endl;
    if(firstadkq==0) std::cout<<" first anti dark quark not found"<<std::endl;
    if(firstadq==0) std::cout<<" first anti down quark not found"<<std::endl;
}



    // jets
    int NNNjet = (*jet_index).size();
    vector<int> jet_ntrkpt1(NNNjet);
    vector<int> jet_ntrkpt1zm(NNNjet);
    vector<float> jet_meanip(NNNjet);
    vector<double> jet_fpt(NNNjet);
    vector<double> jet_ptmax(NNNjet);
    vector<double> jet_fnpile(NNNjet);
    vector<float> AM(NNNjet);
    vector<float> r0(NNNjet);
    vector<float> r1(NNNjet);
    vector<float> rmed(NNNjet);
    vector<float> rsmed(NNNjet);
    vector<int> jntrack(NNNjet);
    vector<int> jntrackip(NNNjet);
    vector<float> jet_e(NNNjet);
    vector<float> jet_theta(NNNjet);
    vector<float> jet_px(NNNjet);
    vector<float> jet_py(NNNjet);
    vector<float> jet_pz(NNNjet);
    vector<float> amaxbyhand(NNNjet);
    vector<float> amax2D(NNNjet);
    vector<float> a3d(NNNjet); //a3d
    vector<float> jet_met(NNNjet); //met in jet direction
    vector<float> amax2Df2(NNNjet);
    vector<float> jet_meanz(NNNjet);
    vector<int> jet_pid_maxEt(NNNjet);
    vector<float> jet_maxET_part(NNNjet);
      vector<bool> matchdkq(NNNjet);;
      vector<bool> matchdq(NNNjet);

    if(otfile) hnjet->Fill(NNNjet+0.5);
    if(iDBG>2) std::cout<<std::endl<<" number of jets is "<<NNNjet<<std::endl;

    for(Int_t j=0; j<NNNjet; j++) {
      if(iDBG>2) std::cout<<"jet j = "<<j<<std::endl;
      jet_theta[j]=2.*atan(exp(-jet_eta->at(j)));
      jet_e[j]=(*jet_pt)[j]/sin(jet_theta[j]);
      jet_px[j]=(*jet_pt)[j]*cos(jet_phi->at(j));
      jet_py[j]=(*jet_pt)[j]*sin(jet_phi->at(j));
      jet_pz[j]=(*jet_pt)[j]/tan(jet_theta[j]);
      jet_met[j]=met_pt*cos((*jet_phi)[j]-met_phi);
				
      if(otfile) hpt->Fill(OverFlow(jet_pt->at(j),hpt));
      if(otfile) heta->Fill(OverFlow(jet_eta->at(j),heta));
      if(otfile) hjetchf->Fill(OverFlow(jet_chf->at(j),hjetchf));
      if(otfile) if(j<4) heta2->Fill(OverFlow(jet_eta->at(j),heta2));



      //      calculate  track variable associated with jet
      jet_ntrkpt1[j]=0;
      jet_ntrkpt1zm[j]=0;
      AM[j]=0;
      amax2D[j]=0.;
      amax2Df2[j]=0.;
      amaxbyhand[j]=0.;
      jet_meanip[j]=0.;
      jet_fpt[j]=0.;
      jet_ptmax[j]=0.;
      jet_fnpile[j]=0.;
      if(r0.size()>0) r0[j]=0.;
      if(r1.size()>0) r1[j]=0.;
      vector<float> track_pts = track_pt->at(j);
      vector<float> track_etas = track_eta->at(j);
      vector<float> track_phis = track_phi->at(j);
      vector<float> track_pvWeights = track_pvWeight->at(j);
      vector<int> track_sources = track_source->at(j);
      vector<int> track_qualitys = track_quality->at(j);
      vector<float> track_ipXYs = track_ipXY->at(j);
      vector<float> track_ipZs = track_ipZ->at(j);
      vector<float> track_ipXYSigs = track_ipXYSig->at(j);
      vector<float> sort_ip;
      vector<float> sort_sip;// signed IP2D
      vector<float> track_ref_xs = track_ref_x->at(j);
      vector<float> track_ref_ys = track_ref_y->at(j);
      vector<float> track_ref_zs = track_ref_z->at(j);
      jntrack[j]=0;
      jntrackip[j]=0;
      jet_meanz[j]=0.;
      if(iDBG>2) std::cout<<"  with tracks "<<std::endl;
      if(iDBG>2) std::cout<<" #     pt     eta   phi    weight  ipxy    ipxysig     "<<std::endl;


      double sumpt=0.;
      double sumpile=0.;
      double sumptallf=0.;
      double sumallf=0.;
      double sumptall=0.;
      double sumptallnz=0.;
      double sumpt2D=0.;
      double sumpt2Df=0.;
      double sum2Df=0.;
      float tracks_srczero = 0.; // for counting number of tracks src 0
      float ptmaxtrk=0.;
      vector<float> forzs;
      if(iDBG>2) std::cout<<" for jet "<<j<<std::endl;
      for (unsigned itrack=0; itrack<track_pts.size(); itrack++) {
	if((track_sources[itrack]==0)&&((track_qualitys[itrack]&4)>0)) {
	  sumptallnz+=track_pts[itrack];
	  float ahate1 = pv_z->at(0)-track_ref_zs[itrack];
	  float ahate2=track_ipXYSigs[itrack];
  	  hpvztrz->Fill(OverFlow(ahate1,hpvztrz));
  	  hip2dsig->Fill(OverFlow(ahate2,hip2dsig)); 
	  h2ipvz->Fill(ahate2,ahate1);
	  if (fabs(pv_z->at(0)-track_ref_zs[itrack])<ps.pilecut) {  // effectively turned off due to large cut value
	    tracks_srczero += 1.0;
	    if(track_pts[itrack]>ptmaxtrk) {
	      ptmaxtrk=track_pts[itrack];
	    }
	    if(iDBG>2)     std::cout<<"  "<<itrack<<" "<<track_pts[itrack]<<" "<<track_etas[itrack]<<" "<<track_phis[itrack]<<" "<<track_pvWeights[itrack]<<" "<<track_ipXYs[itrack]<<" "<<track_ipZs[itrack]<<std::endl;

	    if(fabs(track_ipXYs[itrack])<0.08) {
	      jntrackip[j]++;
	      forzs.push_back(track_ref_zs[itrack]);
	    }
	    sort_ip.push_back(fabs(track_ipXYs[itrack]));
	    sort_sip.push_back(track_ipXYs[itrack]);
	    sumptall+=track_pts[itrack];
	    sumptallf+=track_pts[itrack];
	    sumallf+=1.;
	    ahate1=ahate1/deltaz;  // 0.01 is guess on width?
	    float ahate =ahate1*ahate1+ahate2*ahate2;
	    ahate=sqrt(ahate);
	    //float ahate = fabs(ahate1)+abs(ahate2);
	    //ahate=ahate/1.6;
	    //if(fabs(track_ipXYSigs[itrack])<4) sumpt2Df+=track_pts[itrack];
	    if(ahate<ps.ahatecut) sumpt2Df+=track_pts[itrack]; //a3d calculation
	    if(fabs(track_ipXYSigs[itrack])<2) sum2Df+=1.;
	    // if(fabs(pv_z->at(0)-track_ref_zs[itrack])<ps.pilecut2) sumpile+=track_pts[itrack];
 	    if(track_pvWeights[itrack]>0) sumpt+=track_pts[itrack];
	    if(fabs(track_ipXYs[itrack])<0.08) sumpt2D+=track_pts[itrack];
	    if(otfile) htvw->Fill(OverFlow(track_pvWeights[itrack],htvw));
	    if(track_pts[itrack]>1) {
	      jet_ntrkpt1[j]+=1;
	      if(fabs(track_ref_zs[itrack]-pv_z->at(0))<5.) {
	        jet_ntrkpt1zm[j]+=1;
	      }
	    }
	    jet_meanip[j]=jet_meanip[j]+fabs(track_ipXYs[itrack]);
	    jntrack[j]++;
	    if(iDBG>2) {
	      std::cout
              <<itrack
              <<std::setw(8)<<std::setprecision(3)<<track_pts[itrack]
              <<std::setw(8)<<std::setprecision(3)<<track_etas[itrack]
              <<std::setw(8)<<std::setprecision(3)<<track_phis[itrack]
              <<std::setw(8)<<std::setprecision(3)<<track_pvWeights[itrack]
              <<std::setw(9)<<std::setprecision(3)<<track_ipXYs[itrack]
              <<std::setw(8)<<std::setprecision(3)<<track_ipXYSigs[itrack]
		     <<std::endl;
	    }
	  }  //pileupcut
	} //source and quality
      }
      if(sumptall>0) AM[j]=sumpt/sumptall;
      if(sumptallnz>0) jet_fnpile[j]=sumpile/sumptallnz;
      jet_fpt[j]=ptmaxtrk/jet_pt->at(j);
      jet_ptmax[j]=ptmaxtrk;
      if(sumptall>0) amaxbyhand[j]=sumpt/sumptall;
      if(sumptall>0) amax2D[j]=sumpt2D/sumptall;
      if(sumptallf>0) a3d[j]=sumpt2Df/sumptallf;//a3d
      if(sumallf>0) amax2Df2[j]=sum2Df/sumallf;
      if(otfile) halpha->Fill(OverFlow(AM[j],halpha));
      //float atmp = jntrack[j];
      if(jntrack[j]>0) jet_meanip[j]=jet_meanip[j]/tracks_srczero;

      //float atmp2 = jntrackip[j];
      //if(tracksFz>0) jet_meanz[j]=jet_meanz[j]/tracksFz;
      int itmp = forzs.size();
      if(itmp>0) {
	std::sort(forzs.begin(),forzs.end());
	int itmp2=itmp/2;
	jet_meanz[j]=forzs[itmp2];
      }

      std::sort(sort_ip.begin(), sort_ip.end());
      std::sort(sort_sip.begin(), sort_sip.end());
      std::reverse(sort_ip.begin(),sort_ip.end());
      if(sort_ip.size()>0) r0[j]=sort_ip[0];
      if(sort_ip.size()>1) r1[j]=sort_ip[1];
      if(sort_ip.size()>0) rmed[j] = median(sort_ip);  
      if(sort_sip.size()>0) rsmed[j] = median(sort_sip);  


      // recalculate jetmeanz throwing out outlyers
      /*
      float atmp = jet_meanz[j];
      if(iDBG>0) std::cout<<"atmp is "<<atmp<<std::endl;
      jet_meanz[j]=0.;
      tracksFz=0.;
      for (unsigned itrack=0; itrack<track_pts.size(); itrack++) {
	if((track_sources[itrack]==0)&&((track_qualitys[itrack]&4)>0)) {
	  if(fabs(track_ref_zs[itrack]-atmp)<5) {
	  if(fabs(track_ipXYs[itrack])<0.08) {
	    //jet_meanz[j]+=(track_pts[itrack])*track_ref_zs[itrack];
	    //tracksFz+=track_pts[itrack];
	      jet_meanz[j]+=track_ref_zs[itrack];
	      tracksFz+=1;
	      if(iDBG>2) std::cout<<"itrack meanz tracksFz "<<itrack<<" "<<jet_meanz[j]<<" "<<tracksFz<<std::endl;
	  }
	}
	}
      }
      if(tracksFz>0) jet_meanz[j]=jet_meanz[j]/tracksFz;
      */

      if(iDBG>2) {
	std::cout<<"mean max are "<<jet_meanip[j]<<" "<<r0[j]<<std::endl;
	std::cout<<"   jet "<<j<<" "<<jet_pt->at(j)<<" "<<jet_eta->at(j)<<" "<<jet_phi->at(j)<<" "<<AM[j]<<std::endl;
      }
      if(iDBG>2) std::cout<<" pt eta phi alphamax ntrack r0 AM AM2D AM2Df"<<std::endl;
      if(iDBG>2) {
	std::cout<<"   jet "<<j<<" "<<jet_pt->at(j)<<" "<<jet_eta->at(j)<<" "<<jet_phi->at(j)<<" "<<AM[j]<<" "<<r0[j]<<" "<<AM[j]<<" "<<amax2D[j]<<" "<<a3d[j]<<std::endl;
      }

      // calculate some gen particle information for jet
      int NNNgp = (*gp_index).size();
      int igenmax=-1;
      float etgenmax=0.;
      for(Int_t igen=1; igen<NNNgp; igen++) {
        if((abs(gp_pdgId->at(igen))<6)||(abs(gp_pdgId->at(igen))==21)) {  // quark or gluon
	  if(DeltaR(jet_eta->at(j),jet_phi->at(j),gp_eta->at(igen),gp_phi->at(igen))<0.4) {
	    if(gp_pt->at(igen)>etgenmax) {
	      igenmax=igen;
	      etgenmax=gp_pt->at(igen);
	    }
	  }
	}
      }
      // fix glue to bbbar
      float igenmax2=-1;
      float etgenmax2=0.;
      if(igenmax>0) {
	if(abs(gp_pdgId->at(igenmax)==21)) {
          for(Int_t igen=1; igen<NNNgp; igen++) {
            if((abs(gp_pdgId->at(igen))==5)&&(gp_pt->at(igen)>10.)) {  // b
	      if(DeltaR(jet_eta->at(j),jet_phi->at(j),gp_eta->at(igen),gp_phi->at(igen))<0.4) {
	        if(gp_pt->at(igen)>etgenmax2) {
	          igenmax2=1;
	          etgenmax2=gp_pt->at(igen);
	        }
	      }
	    }
	  }
	}
      }

      jet_pid_maxEt[j]=0;
      jet_maxET_part[j]=0;
      if(igenmax>-1) {
	int ipid = gp_pdgId->at(igenmax);
	if(abs(ipid)<6) {
	  jet_pid_maxEt[j]=gp_pdgId->at(igenmax);
	  jet_maxET_part[j] = etgenmax;
	} else {
	  if(igenmax2==-1) {
	    jet_pid_maxEt[j]=7;
	    jet_maxET_part[j] = etgenmax;
	  } else {
	    jet_pid_maxEt[j]=8;
	    jet_maxET_part[j] = etgenmax;

	  }
	}
      }



     }  // end of loop over jets



      //now see which jets are emerging
    if(iDBG>2) std::cout<<" in event "<<event<<" number of jets is "<<NNNjet<<std::endl;
    vector<bool> emerging(NNNjet);
    vector<bool> almostemerging(NNNjet);
    vector<bool> almostemerging2(NNNjet);
    vector<bool> basicjet(NNNjet);
      for( int i=0;i<4;i++) {
	  emerging[i]=false;
	  almostemerging[i]=false;
	  almostemerging2[i]=false;
	  basicjet[i]=false;
	}
      int nbasicjet=0.;
      int nemerging=0;
      int nalmostemerging=0;
      int nalmostemerging2=0;
      for(int ij=0;ij<NNNjet;ij++) {
	
        vector<float> track_ipXYs = track_ipXY->at(ij);
        vector<float> track_ipXYSigs = track_ipXYSig->at(ij);
        vector<int> track_sources = track_source->at(ij);
        vector<int> track_qualitys = track_quality->at(ij);
      vector<float> track_pvWeights = track_pvWeight->at(ij);
      vector<float> track_ref_zs = track_ref_z->at(ij);
	if(otfile) hjetcut->Fill(0.5);
	if(otfile) hjetcuta->Fill("All",1);
	if(fabs(jet_eta->at(ij))<ps.jetacut) { // jet eta cut
	    if(otfile) hjetcut->Fill(1.5);
	    if(otfile) hjetcuta->Fill("Eta",1);
	    if(otfile) hbcut_nef->Fill(OverFlow(jet_nef->at(ij),hbcut_nef));
	    if(jet_nef->at(ij)<ps.NemfracCut) {  // neutral fraction
	        if(otfile) hacut_nef->Fill(OverFlow(jet_nef->at(ij),hacut_nef));
	        if(otfile) hjetcut->Fill(2.5);
	        if(otfile) hjetcuta->Fill("nef",1);
	        if(otfile) hbcut_ntrkpt1->Fill(OverFlow(jet_ntrkpt1[ij],hbcut_ntrkpt1));
	    if(jet_ntrkpt1[ij]>ps.ntrk1cut) {  // tracks pt>1
	      if(otfile) hacut_ntrkpt1->Fill(OverFlow(jet_ntrkpt1[ij],hacut_ntrkpt1));
	      if(otfile) hjetcut->Fill(3.5);
	if(otfile) hjetcuta->Fill("ntrk1",1);

	      if(otfile) hbcut_cef->Fill(OverFlow(jet_cef->at(ij),hbcut_cef));
	      if(jet_cef->at(ij)<ps.CemfracCut) {  //charged fraction
	        if(otfile) hacut_cef->Fill(OverFlow(jet_cef->at(ij),hacut_cef));
	        if(otfile) hjetcut->Fill(4.5);
		if(otfile) hjetcuta->Fill("cef",1);

		if(jet_fpt[ij]<0.6) {
		//if(jet_fpt[ij]<10000.) 
	        if(otfile) hjetcut->Fill(5.5);
		if(otfile) hjetcuta->Fill("fpt",1);

		//if(jet_fnpile[ij]>0.4) 
		if (true)
		{	
		  //if(jet_ptmax[ij]<10000) 
		  //if(jet_ptmax[ij]<100000000) 
		if (true){
	 	    basicjet[ij]=true;
		    if(ij<Njetcut) nbasicjet+=1;


	            if(otfile) hbcut_alphamax->Fill(OverFlow(AM[ij],hbcut_alphamax));
		if (true){    
		//if(AM[ij]<alphaMaxcut)  // alpha max

	              //if(otfile) hacut_alphamax->Fill(OverFlow(AM[ij],hacut_alphamax));
	              //if(otfile) hjetcut->Fill(6.5);
			//if(otfile) hjetcuta->Fill("amax",1);
		      if(iDBG>2) {
		      if(ij<Njetcut) {
		          std::cout<<" an almost emerging jet "<<ij<<std::endl;
		          std::cout<<" with r0 of "<<r0[ij]<<std::endl;
		          std::cout<<" and pt of "<<jet_pt->at(ij)<<std::endl;
		        }
		      }

		    }

		    if(a3d[ij]<ps.a3dcut) { //a3d
		      almostemerging[ij]=true;
			if(otfile) hjetcuta->Fill("a3d",1);
		    if (true){ 
		      //if(amax2Df2[ij]<0.3) 
		      if(ij<Njetcut) nalmostemerging=nalmostemerging+1;
		      //almostemerging2[ij]=true;
		      //if(ij<Njetcut) nalmostemerging2=nalmostemerging2+1;


	              if(otfile) hbcut_medip->Fill(OverFlow(rmed[ij],hbcut_medip));
		      if(rmed[ij]>ps.medIPcut) { // med IP cut
                        if(otfile) hjetcut->Fill(7.5);
			if(otfile) hjetcuta->Fill("medIP",1);
	                if(otfile) hbcut_theta2d->Fill(OverFlow(log10(jet_theta2D->at(ij)),hbcut_theta2d));
		        if(jet_theta2D->at(ij)>ps.theta2dcut) {
			  if(otfile) hjetcuta->Fill("theta2d",1);
                          if(otfile) hjetcut->Fill(8.5);

	                  emerging[ij]=true;
	                  if(ij<Njetcut) nemerging+=1.; //nemerging cut???
		          if(iDBG>2) {
		            if(ij<Njetcut) {
		              std::cout<<" an emerging jet "<<ij<<std::endl;
		              std::cout<<" with r0 of "<<r0[ij]<<std::endl;
		              std::cout<<" and pt of "<<jet_pt->at(ij)<<std::endl;
		            }
		          }
		// look at tracks in the emerging jets
		          if(otfile) hmedipXYEJ->Fill(OverFlow(rmed[ij],hmedipXYEJ));
		          if(otfile) hmeanipXYEJ->Fill(OverFlow(jet_meanip[ij],hmeanipXYEJ));
		          if(jet_meanip[ij]>r0[ij]) std::cout<<"DANGER DANGER"<<std::endl;
                          for (unsigned itrack=0; itrack<track_ipXYs.size(); itrack++) {
	                    if((track_sources[itrack]==0)&&((track_qualitys[itrack]&4)>0)&& (fabs(pv_z->at(0)-track_ref_zs[itrack])<ps.pilecut)) {
		              if(otfile) hipXYEJ->Fill(OverFlow(track_ipXYs[itrack],hipXYEJ));
		              if(otfile) hipXYSigEJ->Fill(OverFlow(track_ipXYSigs[itrack],hipXYSigEJ));
		              if(otfile) htvwEJ->Fill(OverFlow(track_pvWeights[itrack],htvwEJ));
			    }
			  }  // 
			} //theta 2D
		      }//mean ip
		    }} //alpha
		    }}  //end of basic jet selection
		}
	      }
	    }
          }
        }
	if(!emerging[ij]) {
	  if(otfile) hmedipXYnEJ->Fill(OverFlow(rmed[ij],hmedipXYnEJ));
	  if(otfile) hmeanipXYnEJ->Fill(OverFlow(jet_meanip[ij],hmeanipXYnEJ));
                for (unsigned itrack=0; itrack<track_ipXYs.size(); itrack++) {
	          if((track_sources[itrack]==0)&&((track_qualitys[itrack]&4)>0)&& (fabs(pv_z->at(0)-track_ref_zs[itrack])<ps.pilecut)) {
		    if(otfile) hipXYnEJ->Fill(OverFlow(track_ipXYs[itrack],hipXYnEJ));
		    if(otfile) hipXYSignEJ->Fill(OverFlow(track_ipXYSigs[itrack],hipXYSignEJ));
	           }
                }

	}
	if(iDBG>2) std::cout<<"event pt alphaM cef nef ntrkpt1 r0 emerging  almost almost2"<<event<<" "<<jet_pt->at(ij)<<" "<<AM[ij]<<" "<<jet_cef->at(ij)<<" "<<jet_nef->at(ij)<<" "<<jet_ntrkpt1[ij]<<" "<<r0[ij]<<" "<<emerging[ij]<<" "<<almostemerging[ij]<<" "<<almostemerging2[ij]<<std::endl;
      }
      if(otfile) h_nemg->Fill(OverFlow(nemerging,h_nemg));
      if(otfile) h_naemg->Fill(OverFlow(nalmostemerging,h_naemg));





      // *************************************************************
    // now start the event selections
      // *************************************************************

      //vertex cuts

      bool PVPT0=true;
      //if(pv_indexInColl!=0) PVPT0=false;
      if((*pv_index).at(0)>0) PVPT0=false;


      bool PVZ=true;
      //if(fabs(pv_z->at(0))>15) PVZ=false;
      if(fabs(pv_z->at(0))>ps.pvz_cut) PVZ=false;

    // require at least 4 jets
    bool C4jet=true;
    if(nbasicjet<Njetcut) C4jet=false;
    // HT
    double HT=0.;
    for(int i=0;i<std::min(NNNjet,4);i++) {
      HT+=jet_pt->at(i);
    }
    double HTall=0;
    for (int i=0;i<NNNjet;i++){HTall+=jet_pt->at(i);}
    


    //hltbit=1;
    if(otfile) H_T->Fill(OverFlow(HT,H_T));
    if(otfile&&hltbit) hht4->Fill(HTall,HT);
    if(otfile&&NNNjet>0&&hltbit) hpt1->Fill(OverFlow(jet_pt->at(0),hpt1));
    if(otfile&&NNNjet>1&&hltbit) hpt2->Fill(OverFlow(jet_pt->at(1),hpt2));
    if(otfile&&NNNjet>2&&hltbit) hpt3->Fill(OverFlow(jet_pt->at(2),hpt3));
    if(otfile&&NNNjet>3&&hltbit) hpt4->Fill(OverFlow(jet_pt->at(3),hpt4));
    bool CHT=false;
    if(hltbit&&(HT>ps.HTcut)) CHT=true;
    // jet pt
    bool Cpt1=false;
    bool Cpt2=false;
    bool Cpt3=false;
    bool Cpt4=false;
    if(NNNjet>0) {
      if((jet_pt->at(0)>ps.pt1cut)&&(fabs(jet_eta->at(0))<ps.jetacut)) Cpt1=true;}
    if(NNNjet>1) {
      if((jet_pt->at(1)>ps.pt2cut)&&(fabs(jet_eta->at(1))<ps.jetacut)) Cpt2=true;}
    if(NNNjet>2) {
      if((jet_pt->at(2)>ps.pt3cut)&&(fabs(jet_eta->at(2))<ps.jetacut)) Cpt3=true;}
    if(NNNjet>3) {
      if((jet_pt->at(3)>ps.pt4cut)&&(fabs(jet_eta->at(3))<ps.jetacut)) Cpt4=true;}
    // number emerging jets
    bool Cnem = true;
    if(nemerging<ps.NemergingCut) Cnem=false;

    // almost emerging
    bool Canem =true;
    if(nalmostemerging>=4) Canem=false;
    
     
     double amass=0;
     if(NNNjet>3) {
       int ie1 =-1;
       int ie2=-1;
       int id1=-1;
       int id2=-1;

       for(int i5=0;i5<4;i5++) {
	 if(emerging[i5]) {
	   if(ie1==-1) {ie1=i5;}
	   else {ie2=i5;}
	 } else {
	   if(id1==-1) {id1=i5;}
	   else {id2=i5;}
	 }
       }
       if(iDBG>2) {
	 std::cout<<"ie1 ie2 id1 id2 are "<<ie1<<" "<<ie2<<" "<<id1<<" "<<id2<<std::endl;
       }
       if(ie1>-1&&ie2>-1&&id1>-1&&id2>-1) {
	      float mass1 = sqrt(
				 pow((jet_e[ie1]+jet_e[id1]),2) -
				 pow((jet_px[ie1]+jet_px[id1]),2) -
				 pow((jet_py[ie1]+jet_py[id1]),2) -
				 pow((jet_pz[ie1]+jet_pz[id1]),2)
				 );
	      float mass2 = sqrt(
				 pow((jet_e[ie2]+jet_e[id2]),2) -
				 pow((jet_px[ie2]+jet_px[id2]),2) -
				 pow((jet_py[ie2]+jet_py[id2]),2) -
				 pow((jet_pz[ie2]+jet_pz[id2]),2)
				 );
	      float mass3 = sqrt(
				 pow((jet_e[ie1]+jet_e[id2]),2) -
				 pow((jet_px[ie1]+jet_px[id2]),2) -
				 pow((jet_py[ie1]+jet_py[id2]),2) -
				 pow((jet_pz[ie1]+jet_pz[id2]),2)
				 );
	      float mass4 = sqrt(
				 pow((jet_e[ie2]+jet_e[id1]),2) -
				 pow((jet_px[ie2]+jet_px[id1]),2) -
				 pow((jet_py[ie2]+jet_py[id1]),2) -
				 pow((jet_pz[ie2]+jet_pz[id1]),2)
				 );
	      if(fabs(mass1-mass2)<fabs(mass3-mass4)) {
                amass=(mass1+mass2)/2.;
	      } else {
                amass=(mass3+mass4)/2.;
	      }
	      if(iDBG>2) std::cout<<"mass1 mass2 mass3 mass4 amass "<<mass1<<" "<<mass2<<" "<<mass3<<" "<<mass4<<" "<<amass<<std::endl;
       }
     }
	  hmass->Fill(OverFlow(amass,hmass));

    bool Cmass = false;
    if(fabs(amass-ps.mass)<ps.masscut) Cmass=true;

    bool Cmet = false;
    if(met_pt>ps.metcut) Cmet = true;


    //blind
    if(blind) {
      Cnem=false;
      Canem=false;
    }

    // do some plots
    if(otfile) {

      // kine only plots
      if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4) {
	hHTko->Fill(OverFlow(HT,hHTko));
	hpt1ko->Fill(OverFlow(jet_pt->at(0),hpt1ko));
	hpt2ko->Fill(OverFlow(jet_pt->at(1),hpt2ko));
	hpt3ko->Fill(OverFlow(jet_pt->at(2),hpt3ko));
	hpt4ko->Fill(OverFlow(jet_pt->at(3),hpt4ko));

      }
      if (PVZ&&PVPT0&&C4jet&&hltbit){
      // jet plots
      float dr;
      for(int i=0;i<NNNjet;i++) {
	
        vector<float> track_pts = track_pt->at(i);
        vector<float> track_etas = track_eta->at(i);
        vector<float> track_phis = track_phi->at(i);
        vector<float> track_ipXYs = track_ipXY->at(i);
        vector<float> track_ipZs = track_ipZ->at(i);
        vector<float> track_ipXYSigs = track_ipXYSig->at(i);
        vector<float> track_ip3DSigs= track_ip3DSig->at(i);
        vector<int> track_sources = track_source->at(i);
        vector<int> track_qualitys = track_quality->at(i);
        vector<float> track_pvWeights = track_pvWeight->at(i);
        vector<float> track_ref_zs = track_ref_z->at(i);


	    matchdkq[i]=false;
	    matchdq[i]=false;
	
	if(basicjet[i]) {
	  if(jet_pt->at(i)>50 ) {
	    if(firstdkq>0) {
              dr=DeltaR(jet_eta->at(i),jet_phi->at(i),gp_eta->at(firstdkq),gp_phi->at(firstdkq));
	      if(dr<0.4) {
		matchdkq[i]=true;
	        if(iDBG>2) std::cout<<" matched jet "<<i<<" with dark quark dR="<<dr<<std::endl;
	       }
	    }


	    if(firstadkq>0) {
	      dr=DeltaR(jet_eta->at(i),jet_phi->at(i),gp_eta->at(firstadkq),gp_phi->at(firstadkq));
	      if(dr<0.4) {
		matchdkq[i]=true;
	        if(iDBG>2) std::cout<<" matched jet "<<i<<" anti with dark quark dr="<<dr<<std::endl;
	      }
	    }


	    if(firstdq>0) {
	      dr=DeltaR(jet_eta->at(i),jet_phi->at(i),gp_eta->at(firstdq),gp_phi->at(firstdq));
	      if(dr<0.4) {
		matchdq[i]=true;
	      if(iDBG>2) std::cout<<" matched jet "<<i<<" with down quark dr="<<dr<<std::endl;
	      }
	    }


	    if(firstadq>0) {
	      dr=DeltaR(jet_eta->at(i),jet_phi->at(i),gp_eta->at(firstadq),gp_phi->at(firstadq));
	      if(dr<0.4) {
		matchdq[i]=true;
	        if(iDBG>2) std::cout<<" matched jet "<<i<<" with antidown quark dr="<<dr<<std::endl;
	        std::cout<<" matched jet "<<i<<" with antidown quark dr="<<dr<<std::endl;
	      }
	    }
	    if(iDBG>2) std::cout<<" matchdkq matchdq are "<<matchdkq[i]<<" "<<matchdq[i]<<std::endl;

	    //if(matchdkq[i]&&matchdq[i]) std::cout<<"danger danger match both dark and down quark"<<std::endl;


	    /*haMgj->Fill(OverFlow(AM[i],haMgj));
	    if(iDBG>2) {
	    if(AM[i]<0.015) {
	      std::cout<<"CHECK"<<std::endl;
	      std::cout<<"alpha max is "<<AM[i]<<std::endl;
	      std::cout<<"jet pt is "<<jet_pt->at(i)<<std::endl;  
	      std::cout<<"jet eta is "<<jet_eta->at(i)<<std::endl;  
	      std::cout<<"jet phi is "<<jet_phi->at(i)<<std::endl;  
	      std::cout<<" number tracks in jet is "<<jntrack[i]<<std::endl;
	      std::cout<<" number true interactions is "<<nTrueInt<<std::endl;
	      std::cout<<" total number tracks is "<<nTracks<<std::endl;
	    }
	    }*/

	    /*haMvjpt->Fill(AM[i],jet_pt->at(i));
	    haMvHT->Fill(AM[i],HT);
	    haMvnvtx->Fill(AM[i],nVtx);
	    hjptb->Fill(OverFlow(jet_pt->at(i),hjptb));
	    if(emerging[i]) {
	      hjpta->Fill(OverFlow(jet_pt->at(i),hjpta));
	    }

	    if(iDBG>2) {
	      std::cout<<" alphamax alphamax by hand alpha2d are "<<AM[i]<<" "<<amaxbyhand[i]<<" "<<amax2D[i]<<std::endl;
	    }
	    aMbh->Fill(AM[i],amaxbyhand[i]);
*/

	    // plots for dark and down quark jets  WILL ROBINSON
	    if(matchdkq[i]&&(!matchdq[i])) {  // dark quark jet
	      //hdkjetam->Fill(OverFlow(AM[i],hdkjetam));
	      hsum2Dfdk->Fill(OverFlow(amax2Df2[i],hsum2Dfdk));
	      //hdkjetamo->Fill(jet_alphaMax->at(i));
	      ham2dfdk->Fill(OverFlow(a3d[i],ham2dfdk));
	      if (a3d[i]>0) hlgam2dfdk->Fill(OverFlow(log10(a3d[i]),hlgam2dfdk));
	      else hlgam2dfdk->Fill(-3.9);
       	      htheta2ddk->Fill(OverFlow(log(jet_theta2D->at(i)),htheta2ddk));
	      hmedipdk->Fill(OverFlow(rmed[i],hmedipdk));
	      hmedsipdk->Fill(OverFlow(rsmed[i],hmedsipdk));
	      //hdkjetam2d->Fill(OverFlow(amax2D[i],hdkjetam2d));
	      //aMbh2Ddk->Fill(AM[i],amax2D[i]);
	      float why1=jet_meanz[i]-pv_z->at(0);
	      //aMmzdk->Fill(AM[i],fabs(why1));
	      //if(almostemerging2[i]) {}
	      if(almostemerging[i]) {
		hmzamd->Fill(OverFlow(why1,hmzamd));
		h2damd->Fill(OverFlow(amax2D[i],h2damd));
	      } else {
		hmznamd->Fill(OverFlow(why1,hmznamd));
		h2dnamd->Fill(OverFlow(amax2D[i],h2dnamd));
	      }
	      hmeanzdk->Fill(OverFlow(why1,hmeanzdk));
	      hdkjetmeanip->Fill(OverFlow(jet_meanip[i],hdkjetmeanip));
	      hdkjetntr->Fill(OverFlow(jet_ntrkpt1[i],hdkjetntr));
	      hdkjetmedip->Fill(OverFlow(rmed[i],hdkjetmedip));
	      hdkjetlgmedip->Fill(OverFlow(log10(rmed[i]),hdkjetlgmedip));
	      hmipa3ddk->Fill(log10(rmed[i]),a3d[i]);
	      hntrk1a3ddk->Fill(jet_ntrkpt1[i]>=20?19.5:jet_ntrkpt1[i],a3d[i]>=1?0.999:a3d[i]);
	      hmipmetdk->Fill(log10(rmed[i]),met_pt);
	      hmipmetadk->Fill(log10(rmed[i]),jet_met[i]);
	      hmetadk->Fill(OverFlow(jet_met[i],hmetadk));
	      hmipmassdk->Fill(log10(rmed[i]),amass);
	      
              for (unsigned itrack=0; itrack<track_pts.size(); itrack++) {
	        if((track_sources[itrack]==0)&((track_qualitys[itrack]&4)>0)) {
		  float ahate1 =(pv_z->at(0)-track_ref_zs[itrack]);
		  float ahate2=track_ipXYSigs[itrack];
		  hpvztrzdk->Fill(OverFlow(ahate1,hpvztrzdk));
		  hip2dsigdk->Fill(OverFlow(ahate2,hip2dsigdk));
		  hip2ddk->Fill(OverFlow(track_ipXYs[itrack],hip2ddk));
 		  h2ipvzdk->Fill(ahate2,ahate1);
		  ahate1=ahate1/deltaz;
		  float ahate =sqrt(ahate1*ahate1+ahate2*ahate2);
	          if(fabs(pv_z->at(0)-track_ref_zs[itrack])<ps.pilecut) 
		  {
		  hdkjettrkahate->Fill(OverFlow(ahate,hdkjettrkahate));
		  hdkjettrk3dsig->Fill(OverFlow(fabs(track_ip3DSigs[itrack]),hdkjettrk3dsig));
		  hdkjettrk3dsig_sgn->Fill(OverFlow(track_ip3DSigs[itrack],hdkjettrk3dsig_sgn));
		    hdkjettrkip->Fill(OverFlow(track_ipXYs[itrack],hdkjettrkip));
		    hdkjettrkips->Fill(OverFlow(track_ipXYSigs[itrack],hdkjettrkips));
		    hdkjettrkw->Fill(OverFlow(track_pvWeights[itrack],hdkjettrkw));
		    hmipahatedk->Fill(log10(rmed[i]),ahate);
		    hmipipsigdk->Fill(log10(rmed[i]),track_ipXYSigs[itrack]);
		    hdkipphi->Fill(track_phis[itrack],track_ipXYs[itrack]);
		    adkwvd0->Fill(track_pvWeights[itrack],track_ipXYs[itrack]);
		    adkwviz->Fill(track_pvWeights[itrack],track_ipZs[itrack]);
		    //find gen particle that best matches
		    float dR=99999.;
		    int ipnt=0;
                    for(unsigned int j=0; j<(*gp_index).size(); j++) {
		      if((*gp_status)[j]==1 ) { //stable
		        if((*gp_charge)[j]!=0) {  //charged
                          dr=DeltaR((*gp_eta)[j],(*gp_phi)[j],track_etas[itrack],track_phis[itrack]);
		          if(dr<dR) { dR=dr; ipnt=j;}
		        }
		      }
		    }  // end loop gen part
		    if(iDBG>2) std::cout<<" track "<<itrack<<" matches gen part "<<ipnt<<std::endl;
		    if(ipnt>0) {
		      hdkjettrkdr->Fill(OverFlow(dR,hdkjettrkdr));
		      if(dR<0.01) {
		        float aaa =sqrt(
                         (*gp_vx)[ipnt]*(*gp_vx)[ipnt]
		        +(*gp_vy)[ipnt]*(*gp_vy)[ipnt]
		       			   );

		        adk2Dr0->Fill(aaa,track_ipXYs[itrack]);
		        hdkjettrgip->Fill(OverFlow(aaa,hdkjettrgip));
		      }
		    }
		}}// end track source
	      }  // end loop over tracks
	    }  // end match dark quark
	   
	    if(matchdq[i]&&(!matchdkq[i])) {  // down quark jet
	      //hdjetam->Fill(OverFlow(AM[i],hdjetam));
	      hsum2Dfd->Fill(OverFlow(amax2Df2[i],hsum2Dfd));
	      //hdjetamo->Fill(OverFlow(jet_alphaMax->at(i),hdjetamo));
	      ham2dfd->Fill(OverFlow(a3d[i],ham2dfd));
	      htheta2dd ->Fill(OverFlow(log(jet_theta2D->at(i)),htheta2dd ));
	      hmedipd ->Fill(OverFlow(rmed[i],hmedipd ));
	      hmedsipd ->Fill(OverFlow(rsmed[i],hmedsipd ));
	      //hdjetam2d->Fill(OverFlow(amax2D[i],hdjetam2d));
	      //aMbh2Dd->Fill(AM[i],amax2D[i]);
	      //float why1=jet_meanz[i]-pv_z->at(0);
	      //aMmzd->Fill(AM[i],fabs(why1));
	      hmeanzd->Fill(OverFlow(jet_meanz[i]-pv_z->at(0),hmeanzd));
	      hdjetmeanip->Fill(OverFlow(jet_meanip[i],hdjetmeanip));
	      hdjetntr->Fill(OverFlow(jet_ntrkpt1[i],hdjetntr));
	      hdjetmedip->Fill(OverFlow(rmed[i],hdjetmedip));
	      hdjetlgmedip->Fill(OverFlow(log10(rmed[i]),hdjetlgmedip));
	      hmipa3dd->Fill(log10(rmed[i]),a3d[i]);
	      hntrk1a3dd->Fill(jet_ntrkpt1[i]>=20?19.5:jet_ntrkpt1[i],a3d[i]>=1?0.999:a3d[i]);
	      hmipmetd->Fill(log10(rmed[i]),met_pt);
	      hmipmetad->Fill(log10(rmed[i]),jet_met[i]);
	      hmetad->Fill(OverFlow(jet_met[i],hmetad));
	      hmipmassd->Fill(log10(rmed[i]),amass);
	      
              for (unsigned itrack=0; itrack<track_pts.size(); itrack++) {
	        if((track_sources[itrack]==0)&&((track_qualitys[itrack]&4)>0) ) {
		  float ahate1 = (pv_z->at(0)-track_ref_zs[itrack]);
		  float ahate2=track_ipXYSigs[itrack];
    		  h2ipvzd->Fill(ahate2,ahate1);
		  hpvztrzd->Fill(OverFlow(ahate1,hpvztrzd));
		  hip2dsigd->Fill(OverFlow(ahate2,hip2dsigd));
		  hip2dd->Fill(OverFlow(track_ipXYs[itrack],hip2dd));
		  ahate1=ahate1/deltaz;
		  float ahate =sqrt(ahate1*ahate1+ahate2*ahate2);
		  if(fabs(pv_z->at(0)-track_ref_zs[itrack])<ps.pilecut){
		  hdjettrkahate->Fill(OverFlow(ahate,hdjettrkahate));
		  hdjettrk3dsig->Fill(OverFlow(fabs(track_ip3DSigs[itrack]),hdjettrk3dsig));
		  hdjettrk3dsig_sgn->Fill(OverFlow(track_ip3DSigs[itrack],hdjettrk3dsig_sgn));
		  hmipahated->Fill(log10(rmed[i]),ahate);
		  hmipipsigd->Fill(log10(rmed[i]),track_ipXYSigs[itrack]);
		  hdjettrkip->Fill(OverFlow(track_ipXYs[itrack],hdjettrkip));
		  hdjettrkips->Fill(OverFlow(track_ipXYSigs[itrack],hdjettrkips));
		  hdjettrkw->Fill(OverFlow(track_pvWeights[itrack],hdjettrkw));
		  adwvd0->Fill(track_pvWeights[itrack],track_ipXYs[itrack]);
		  adwviz->Fill(track_pvWeights[itrack],track_ipZs[itrack]);
		  hdipphi->Fill(track_phis[itrack],track_ipXYs[itrack]);

		  //find gen particle that best matches
		  float dR=99999.;
		  int ipnt=0;
                  for(unsigned int j=0; j<(*gp_index).size(); j++) {
		    if((*gp_status)[j]==1 ) { //stable
		      if((*gp_charge)[j]!=0) {  //charged
                        dr=DeltaR((*gp_eta)[j],(*gp_phi)[j],track_etas[itrack],track_phis[itrack]);
			if(dr<dR) {
			  dR=dr;
			  ipnt=j;
			}
		      }
		    }
		  }  // end loop gen part
		  if(iDBG>2) std::cout<<" track "<<itrack<<" matches gen part "<<ipnt<<std::endl;

		  if(ipnt>0) {
		    hdjettrkdr->Fill(OverFlow(dR,hdjettrkdr));
		    if(dR<0.01) {
                      float aaa = sqrt(
                       (*gp_vx)[ipnt]*(*gp_vx)[ipnt]
		       +(*gp_vy)[ipnt]*(*gp_vy)[ipnt]);

		      ad2Dr0->Fill(aaa,track_ipXYs[itrack]);
		      hdjettrgip->Fill(OverFlow(aaa,hdjettrgip));
		    }
		  }
		}
		}  //end track source
	      
	      }  // end loop over tracks
	    }  // if matches down quark
	  }  // end pT 50
	}  // end basic jet
        }
      }

      //N-1 plots
      if(PVZ&&PVPT0&&C4jet&&Cpt1&&Cpt2&&Cpt3&&Cpt4&&Cnem&&Canem&&Cmass&&Cmet) hHTnm1->Fill(OverFlow(HT,hHTnm1));
      if(hltbit&&PVZ&&PVPT0&&C4jet&&Cpt1&&Cpt2&&Cpt3&&Cpt4&&Cnem&&Canem&&Cmass&&Cmet) hHThltnm1->Fill(OverFlow(HT,hHTnm1));
      if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt2&&Cpt3&&Cpt4&&Cnem&&Canem&&Cmass&&Cmet) hpt1nm1->Fill(OverFlow(jet_pt->at(0),hpt1nm1));
      if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt3&&Cpt4&&Cnem&&Canem&&Cmass&&Cmet) hpt2nm1->Fill(OverFlow(jet_pt->at(1),hpt2nm1));
      if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt4&&Cnem&&Canem&&Cmass&&Cmet) hpt3nm1->Fill(OverFlow(jet_pt->at(2),hpt2nm1));
      if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cnem&&Canem&&Cmass&&Cmet) hpt4nm1->Fill(OverFlow(jet_pt->at(3),hpt4nm1));
      if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4&&Canem&&Cmass&&Cmet) hnemnm1->Fill(OverFlow(nemerging,hnemnm1));
      if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4&&Cnem&&Canem&&Cmet) hmassnm1->Fill(OverFlow(amass,hmassnm1));
      if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4&&Cnem&&Canem&&Cmass) hmetnm1->Fill(OverFlow(met_pt,hmetnm1));
      /*if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4&&Cnem&&Canem&&Cmass&&Cmet)
        {
	  htheta2D1nm1->Fill(OverFlow(log10(jet_theta2D->at(0)),htheta2D1nm1));
	  htheta2D2nm1->Fill(OverFlow(log10(jet_theta2D->at(1)),htheta2D2nm1));
	  htheta2D3nm1->Fill(OverFlow(log10(jet_theta2D->at(2)),htheta2D3nm1));
	  htheta2D4nm1->Fill(OverFlow(log10(jet_theta2D->at(3)),htheta2D4nm1));
        }*/


      if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4&&Cmet) {
        for(int i=0;i<4;i++) {
	  if(basicjet[i]) {
    	     /*if(!matchdkq[i]&&(a3d[i]<0.001||(a3d[i]<0.2&&a3d[i]>0.15))) { //not dark but completely emerging jet
		if (a3d[i]>0.05) std::cout<<"OK";
		if (a3d[i]<0.001) std::cout<<"DANGER";
                std::cout<<"jet pt is "<<jet_pt->at(i)
	    	     <<" a3d "<<a3d[i]
		     <<" pid "<< jet_pid_maxEt[i]
	    	     <<" ntrkpt1 "<<jet_ntrkpt1[i]
	    	     <<" medip "<<rmed[i]
	    	     <<" ip max "<<r0[i]
	    	     <<" jmet "<<jet_met[i]
                <<std::endl;
              vector<float> track_pts = track_pt->at(i);
              vector<int> track_sources = track_source->at(i);
              vector<int> track_qualitys = track_quality->at(i);
              vector<float> track_ipXYs = track_ipXY->at(i);
              vector<float> track_ipZs = track_ipZ->at(i);
              vector<float> track_ipXYSigs = track_ipXYSig->at(i);
              vector<float> track_pvWeights = track_pvWeight->at(i);
              vector<int> track_nMissInnerHitss = track_nMissInnerHits->at(i);
              vector<int> track_nMissInnerPxlLayerss = track_nMissInnerPxlLayers->at(i);
              vector<int> track_nPxlLayerss = track_nPxlLayers->at(i);
              vector<int> track_nHitss = track_nHits->at(i);
              vector<float> track_ref_zs = track_ref_z->at(i);
              for (unsigned itrack=0; itrack<track_pts.size(); itrack++) {
	        if((track_sources[itrack]==0)&&((track_qualitys[itrack]&4)>0)&& (fabs(pv_z->at(0)-track_ref_zs[itrack])<ps.pilecut)) {
			if (0) std::cout<<
                         "    trkpt "<<std::setw(5)<<std::setprecision(4)<<track_pts[itrack]
			 <<" pvz-trz "<<std::setw(5)<<std::setprecision(4)<<(pv_z->at(0)-track_ref_zs[itrack])/0.01
			 <<" ipxysig "<<std::setw(8)<<std::setprecision(4)<<track_ipXYSigs[itrack]
			 //<<" ipz "<<track_ipZs[itrack]
			 <<" ipxy "   <<std::setw(8)<<std::setprecision(4)<<track_ipXYs[itrack]
			 <<" trkq "   <<(track_qualitys[itrack]&4)
			 <<" mihits" <<track_nMissInnerHitss[itrack]
			 <<" miplayr" <<track_nMissInnerPxlLayerss[itrack]
			 <<" playr"   <<track_nPxlLayerss[itrack]
		 	 <<" nHits"   <<track_nHitss[itrack]
			 <<std::endl;
	        }
              }
	     }*/
             if(rmed[i]>ps.medIPcut) { // med IP cut
	         if(matchdkq[i]&&(!matchdq[i])) {  // dark quark jet
	             halphadknm1->Fill(OverFlow(a3d[i],halphadknm1));
	         } else {  // other quark jet
	             halphadnm1->Fill(OverFlow(a3d[i],halphadnm1));
 		 }
   	     }
	    //aMip->Fill(AM[i],rmed[i]);
	    hntrk1nm1->Fill(OverFlow(jet_ntrkpt1[i],hntrk1nm1));
	    //if((AM[i]<alphaMaxcut)) 
	    if(a3d[i]<ps.a3dcut){
	      hmedipnm1->Fill(OverFlow(rmed[i],hmedipnm1));
	      hlgmedipnm1->Fill(OverFlow(log10(rmed[i]),hlgmedipnm1));
	      if(matchdkq[i]&&(!matchdq[i])) {  // dark quark jet
	         hlgmedipdknm1->Fill(OverFlow(log10(rmed[i]),hlgmedipdknm1));
	      } else  {  // down quark jet
	         hlgmedipdnm1->Fill(OverFlow(log10(rmed[i]),hlgmedipdnm1));
	      }

	    }  // alphamax cuut
	  }  //basic jet
	}//loop over jets
      } // select events


      if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4) {
        for(int i=0;i<4;i++) {
	    //aMbh2D->Fill(amax2D[i],amaxbyhand[i]);
	    hmeanz->Fill(OverFlow(jet_meanz[i]-pv_z->at(0),hmeanz));
        }
      }


      // plots for events that fail almost energing
    if(C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4&&Cnem&&(!Canem)) {
      for(int i=0;i<4;i++) {
	    //aMbh2Daem->Fill(amax2D[i],amaxbyhand[i]);
	    hmeanzfa->Fill(OverFlow(jet_meanz[i]-pv_z->at(0),hmeanzfa));
	    hntrkpt1zmfa->Fill(OverFlow(jet_ntrkpt1zm[i],hntrkpt1zmfa));
	    h2dfa->Fill(OverFlow(amax2D[i],h2dfa));
	    if(iDBG>0) {
	      std::cout<<" jet meanz pvz are "<<jet_meanz[i]<<" "<<pv_z->at(0)<<std::endl;
	      std::cout<<" jet ntrkpt1zm is "<<jet_ntrkpt1zm[i]<<std::endl;
	      std::cout<<"FAIL CANEM"<<std::endl;
	      std::cout<<"pvz pvpt0 are "<<PVZ<<" "<<PVPT0<<std::endl;
	    }
	//aMbh2Daem->Fill(amax2D[i],amaxbyhand[i]);
      }

    }  // end plots for events that fail almost emerging

    // similar plots for those that pass almost enmering
    if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4&&Cnem&&(Canem)&&Cmass&&Cmet) {
      for(int i=0;i<4;i++) {
	hmeanzpa->Fill(OverFlow(jet_meanz[i]-pv_z->at(0),hmeanzpa));
	hntrkpt1zmpa->Fill(OverFlow(jet_ntrkpt1zm[i],hntrkpt1zmpa));
	h2dpa->Fill(OverFlow(amax2D[i],h2dpa));
	    if(iDBG>1) {
	      std::cout<<" jet meanz pvz are "<<jet_meanz[i]<<" "<<pv_z->at(0)<<std::endl;
	      if(fabs(jet_meanz[i]-pv_z->at(0))>5) std::cout<<"danger"<<std::endl;
	    }
        }
    }

    if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4&&nalmostemerging2>=2) {
      for(int i=0;i<4;i++) {
	if(almostemerging[i]) {
	    hnmedipnm1->Fill(OverFlow(rmed[i],hnmedipnm1));
	    hnlgmedipnm1->Fill(OverFlow(log10(rmed[i]),hnlgmedipnm1));
	}
      }
    }
/*
    if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4&&nalmostemerging2>=1) {
      for(int i=0;i<4;i++) {
	if(almostemerging[i]) {
	    hn2medipnm1->Fill(OverFlow(rmed[i],hn2medipnm1));
	}
      }
    }

*/
    // debug spew
    if(iDBG>2) std::cout<<"pvz pvpt0 c4jet cht cpt1 cpt2 cpt3 cpt4 cnem "<<PVZ<<" "<<PVPT0<<" "<<C4jet<<" "<<CHT<<" "<<Cpt1<<" "<<Cpt2<<" "<<Cpt3<<" "<<Cpt4<<" "<<Cnem<<std::endl;





    // preselection plots
    if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4) {
      hmet->Fill(OverFlow(met_pt,hmet));
      hpvpre->Fill(OverFlow(pv_z->at(0),hpvpre));
      hnvtxpre->Fill(OverFlow(nVtx,hnvtxpre));
      hntrkpre->Fill(OverFlow(nTracks,hntrkpre));
      float aab=0.;
      for(int i=0;i<4;i++) {
              vector<float> track_pts = track_pt->at(i);
              vector<int> track_sources = track_source->at(i);
              vector<int> track_qualitys = track_quality->at(i);
              vector<float> track_ipXYs = track_ipXY->at(i);
              vector<float> track_ipZs = track_ipZ->at(i);
              vector<float> track_ipXYSigs = track_ipXYSig->at(i);
              vector<float> track_ref_zs = track_ref_z->at(i);
              vector<float> track_ip3DSigs= track_ip3DSig->at(i);
	hjetptfrpre->Fill(std::min(jet_fpt[i],1.2));
	hptallpre->Fill(OverFlow(jet_pt->at(i),hptallpre));
	hmeanzpre->Fill(OverFlow(jet_meanz[i]-pv_z->at(0),hmeanzpre));
	if(jet_pid_maxEt[i]==1) hptudpre->Fill(OverFlow(jet_pt->at(i),hptudpre));
	if(jet_pid_maxEt[i]==2) hptudpre->Fill(OverFlow(jet_pt->at(i),hptudpre));
	if(jet_pid_maxEt[i]==3) hptspre->Fill(OverFlow(jet_pt->at(i),hptspre));
	if(jet_pid_maxEt[i]==4) hptcpre->Fill(OverFlow(jet_pt->at(i),hptcpre));
	if(jet_pid_maxEt[i]==5) hptbpre->Fill(OverFlow(jet_pt->at(i),hptbpre));
	if(jet_pid_maxEt[i]==7) hptgpre->Fill(OverFlow(jet_pt->at(i),hptgpre));
	if(jet_pid_maxEt[i]==8) hptgbbpre->Fill(OverFlow(jet_pt->at(i),hptgbbpre));

	if(jet_pid_maxEt[i]==5){
	 	htheta2db ->Fill(OverFlow(log(jet_theta2D->at(i)),htheta2db ));
		hmedipb ->Fill(OverFlow(rmed[i],hmedipb ));
		hmedsipb ->Fill(OverFlow(rmed[i],hmedsipb ));
		ham2dfb->Fill(OverFlow(a3d[i],ham2dfb));
	        hmipa3db->Fill(log10(rmed[i]),a3d[i]);
	        hntrk1a3db->Fill(jet_ntrkpt1[i]>=20?19.5:jet_ntrkpt1[i],a3d[i]>=1?0.999:a3d[i]);
	        hmipmetb->Fill(log10(rmed[i]),met_pt);
	        hmipmetab->Fill(log10(rmed[i]),jet_met[i]);
	        hmetab->Fill(OverFlow(jet_met[i],hmetab));
	        hmipmassb->Fill(log10(rmed[i]),amass);
		for (unsigned itrack=0; itrack<track_pts.size(); itrack++) {
               	if((track_sources[itrack]==0)&&((track_qualitys[itrack]&4)>0)&& 
				(fabs(pv_z->at(0)-track_ref_zs[itrack])<ps.pilecut)) {
                  float ahate1=pv_z->at(0)-track_ref_zs[itrack];
                  float ahate2=track_ipXYSigs[itrack];
                  h2ipvzb->Fill(ahate2,ahate1);
		  hpvztrzb->Fill(OverFlow(ahate1,hpvztrzb));
		  hip2dsigb->Fill(OverFlow(ahate2,hip2dsigb));
		  hip2db->Fill(OverFlow(track_ipXYs[itrack],hip2db));
		  ahate1=ahate1/deltaz;
		  float ahate =sqrt(ahate1*ahate1+ahate2*ahate2);
		  hmipahateb->Fill(log10(rmed[i]),ahate);
		  hmipipsigb->Fill(log10(rmed[i]),track_ipXYSigs[itrack]);
  		}}
	}
	if(jet_pid_maxEt[i]==8) ham2dfgbb->Fill(OverFlow(a3d[i],ham2dfgbb));
	if(jet_pid_maxEt[i]==7) ham2dfg->Fill(OverFlow(a3d[i],ham2dfg));
	if(jet_pid_maxEt[i]==1||jet_pid_maxEt[i]==2) {
		ham2dfud->Fill(OverFlow(a3d[i],ham2dfud));
		htheta2dud->Fill(OverFlow(log(jet_theta2D->at(i)),htheta2dud));
		hmedipud->Fill(OverFlow(rmed[i],hmedipud));
		hmedsipud->Fill(OverFlow(rmed[i],hmedsipud));
		if (a3d[i]>0)hlgam2dfud->Fill(OverFlow(log10(a3d[i]),hlgam2dfud));
	        else hlgam2dfud->Fill(-3.9);
	        hmipa3dud->Fill(log10(rmed[i]),a3d[i]);
	        hntrk1a3dud->Fill(jet_ntrkpt1[i]>=20?19.5:jet_ntrkpt1[i],a3d[i]>=1?0.999:a3d[i]);
	        hmipmetud->Fill(log10(rmed[i]),met_pt);
	        hmipmetaud->Fill(log10(rmed[i]),jet_met[i]);
	        hmetaud->Fill(OverFlow(jet_met[i],hmetaud));
	        hmipmassud->Fill(log10(rmed[i]),amass);

		for (unsigned itrack=0; itrack<track_pts.size(); itrack++) {
               	if((track_sources[itrack]==0)&&((track_qualitys[itrack]&4)>0)){
                  float ahate1=pv_z->at(0)-track_ref_zs[itrack];
                  float ahate2=track_ipXYSigs[itrack];
                  h2ipvzud->Fill(ahate2,ahate1);
		  hpvztrzud->Fill(OverFlow(ahate1,hpvztrzud));
		  //ip2dsigud->Fill(OverFlow(ahate2,hip2dsigud));
		  hip2dud->Fill(OverFlow(track_ipXYs[itrack],hip2dud));
		  ahate1=ahate1/deltaz;
		  float ahate =sqrt(ahate1*ahate1+ahate2*ahate2);
		  if (isQCD && fabs(pv_z->at(0)-track_ref_zs[itrack])<ps.pilecut) {
			  hdjettrkahate->Fill(OverFlow(ahate,hdjettrkahate));
		  	  hdjettrk3dsig->Fill(OverFlow(fabs(track_ip3DSigs[itrack]),hdjettrk3dsig));
		  	  hdjettrk3dsig_sgn->Fill(OverFlow(track_ip3DSigs[itrack],hdjettrk3dsig_sgn));
		  }
		  hmipahateud->Fill(log10(rmed[i]),ahate);
		  hmipipsigud->Fill(log10(rmed[i]),track_ipXYSigs[itrack]);
  		}}
	}

	if(jet_pid_maxEt[i]==5) {
	  if((jet_pt->at(i)>100)&&(jet_pt->at(i)<300)) ham2dfbpt1->Fill(OverFlow(a3d[i],ham2dfbpt1));
	  if((jet_pt->at(i)>300)&&(jet_pt->at(i)<600)) ham2dfbpt2->Fill(OverFlow(a3d[i],ham2dfbpt2));
	  if((jet_pt->at(i)>600)) ham2dfbpt3->Fill(OverFlow(a3d[i],ham2dfbpt3));
	}

	if((jet_pid_maxEt[i]==1)||(jet_pid_maxEt[i]==2)) {
	  if((jet_pt->at(i)>100)&&(jet_pt->at(i)<300)) ham2dfudpt1->Fill(OverFlow(a3d[i],ham2dfudpt1));
	  if((jet_pt->at(i)>300)&&(jet_pt->at(i)<600)) ham2dfudpt2->Fill(OverFlow(a3d[i],ham2dfudpt2));
	  if((jet_pt->at(i)>600)) ham2dfudpt3->Fill(OverFlow(a3d[i],ham2dfudpt3));
	}


	if(emerging[i]) {
	  hptallpree->Fill(OverFlow(jet_pt->at(i),hptallpree));
	  if(jet_pid_maxEt[i]==1) hptudpree->Fill(OverFlow(jet_pt->at(i),hptudpree));
	  if(jet_pid_maxEt[i]==2) hptudpree->Fill(OverFlow(jet_pt->at(i),hptudpree));
	  if(jet_pid_maxEt[i]==3) hptspree->Fill(OverFlow(jet_pt->at(i),hptspree));
	  if(jet_pid_maxEt[i]==4) hptcpree->Fill(OverFlow(jet_pt->at(i),hptcpree));
	  if(jet_pid_maxEt[i]==5) hptbpree->Fill(OverFlow(jet_pt->at(i),hptbpree));
	  if(jet_pid_maxEt[i]==7) hptgpree->Fill(OverFlow(jet_pt->at(i),hptgpree));
	  if(jet_pid_maxEt[i]==8) hptgbbpree->Fill(OverFlow(jet_pt->at(i),hptgbbpree));
	}

	hsum2Dfpre->Fill(OverFlow(amax2Df2[i],hsum2Dfpre));
	hjntrkpre->Fill(OverFlow(jet_ntrkpt1[i],hjntrkpre));
	hfpilepre->Fill(OverFlow(jet_fnpile[i],hfpilepre));
	hptmaxpre->Fill(std::min(jet_ptmax[i],399.5));
        for (unsigned itrack=0; itrack<track_ref_zs.size(); itrack++) {
	  if((track_sources[itrack]==0)&&((track_qualitys[itrack]&4)>0)&& (fabs(pv_z->at(0)-track_ref_zs[itrack])<ps.pilecut)) {
	    hdzpre->Fill(OverFlow(pv_z->at(0)-track_ref_zs[itrack],hdzpre));
	  }
	}
	for(int j=i+1;j<4;j++) {
	  if(fabs(jet_meanz[i]-jet_meanz[j])>aab) {
	    aab=fabs(jet_meanz[i]-jet_meanz[j]);
	  }
	} 
      }
      hdzjpre->Fill(OverFlow(aab,hdzjpre));

    }  //end preselection plots


    //final selection spew for debug  all cuts except nemerging, separate spew
    // for 1 and 2 emerging
    if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4&&Canem&&Cmass&&Cmet) {
      if(nemerging>0) {  // some debug spew, not a cut

	for(int i=0;i<4;i++) {
	  hptallfinal->Fill(OverFlow(jet_pt->at(i),hptallfinal));
	  if(jet_pid_maxEt[i]==1) hptudfinal->Fill(OverFlow(jet_pt->at(i),hptudfinal));
	  if(jet_pid_maxEt[i]==2) hptudfinal->Fill(OverFlow(jet_pt->at(i),hptudfinal));
	  if(jet_pid_maxEt[i]==3) hptsfinal->Fill(OverFlow(jet_pt->at(i),hptsfinal));
	  if(jet_pid_maxEt[i]==4) hptcfinal->Fill(OverFlow(jet_pt->at(i),hptcfinal));
	  if(jet_pid_maxEt[i]==5) hptbfinal->Fill(OverFlow(jet_pt->at(i),hptbfinal));
	  if(jet_pid_maxEt[i]==7) hptgfinal->Fill(OverFlow(jet_pt->at(i),hptgfinal));
	  if(jet_pid_maxEt[i]==8) hptgbbfinal->Fill(OverFlow(jet_pt->at(i),hptgbbfinal));
	  if(emerging[i]) {
	    hptallfinale->Fill(OverFlow(jet_pt->at(i),hptallfinale));
	    if(jet_pid_maxEt[i]==1) hptudfinale->Fill(OverFlow(jet_pt->at(i),hptudfinale));
	    if(jet_pid_maxEt[i]==2) hptudfinale->Fill(OverFlow(jet_pt->at(i),hptudfinale));
	    if(jet_pid_maxEt[i]==3) hptsfinale->Fill(OverFlow(jet_pt->at(i),hptsfinale));
	    if(jet_pid_maxEt[i]==4) hptcfinale->Fill(OverFlow(jet_pt->at(i),hptcfinale));
	    if(jet_pid_maxEt[i]==5) hptbfinale->Fill(OverFlow(jet_pt->at(i),hptbfinale));
	    if(jet_pid_maxEt[i]==7) hptgfinale->Fill(OverFlow(jet_pt->at(i),hptgfinale));
	    if(jet_pid_maxEt[i]==8) hptgbbfinale->Fill(OverFlow(jet_pt->at(i),hptgbbfinale));
	  }
	}

	if(nemerging>1) {
	  for(int i=0;i<4;i++) {
	    hptallfinal2->Fill(OverFlow(jet_pt->at(i),hptallfinal2));
	    if(jet_pid_maxEt[i]==1) hptudfinal2->Fill(OverFlow(jet_pt->at(i),hptudfinal2));
	    if(jet_pid_maxEt[i]==2) hptudfinal2->Fill(OverFlow(jet_pt->at(i),hptudfinal2));
	    if(jet_pid_maxEt[i]==3) hptsfinal2->Fill(OverFlow(jet_pt->at(i),hptsfinal2));
	    if(jet_pid_maxEt[i]==4) hptcfinal2->Fill(OverFlow(jet_pt->at(i),hptcfinal2));
	    if(jet_pid_maxEt[i]==5) hptbfinal2->Fill(OverFlow(jet_pt->at(i),hptbfinal2));
	    if(jet_pid_maxEt[i]==7) hptgfinal2->Fill(OverFlow(jet_pt->at(i),hptgfinal2));
	    if(jet_pid_maxEt[i]==8) hptgbbfinal2->Fill(OverFlow(jet_pt->at(i),hptgbbfinal2));
	  }
	}

	if(iDBG>0) if(nemerging>1) std::cout<<"HAS "<<nemerging<<"  EMERGING"<<std::endl;

	// see if matches a ba
	for(int i=0;i<4;i++) {
	  if(emerging[i]) {
	    float bdr=1000.;
	    for(unsigned int k=0;k<bigbs.size();k++) {
	      float bdt = DeltaR(jet_eta->at(i),jet_phi->at(i),gp_eta->at(bigbs[k]),gp_phi->at(bigbs[k]));
	      if(iDBG>1) std::cout<<"i k bdt are "<<i<<" "<<k<<" "<<bdt<<std::endl;
	      if(bdt<bdr) bdr=bdt;
	    }
	    if(bdr>10) bdr=-1;
	    hbigb->Fill(OverFlow(bdr,hbigb));
	    if(iDBG>0) {
	      std::cout<<"bdr is "<<bdr<<std::endl;
	      if(abs(bdr)<0.4) std::cout<<" jet "<<i<<" tagged as b"<<std::endl;
	    }
	  }  //end emerging
	}  //end loop over jets



	if(iDBG>0) {
	  if(nemerging>1) std::cout<<"passing2 run lumi event filename is "<<run<<" "<<lumi<<" "<<event<<" "<<inputfilename<<std::endl;
	  else  std::cout<<"passing1 run lumi event filename is "<<run<<" "<<lumi<<" "<<event<<" "<<inputfilename<<std::endl;
	}
	if(iDBG>0) {
	  if(!Canem) std::cout<<"FAILS ALMOST EMERGING"<<std::endl;
	}
	if(iDBG>0) std::cout<<"pv position is "<<pv_x->at(0)<<","<<pv_y->at(0)<<","<<pv_z->at(0)<<std::endl;
	if(iDBG>0) std::cout<<" pv ntracks is "<<nTracks<<std::endl;
	if(iDBG>0) std::cout<<" number of vertices is "<<nVtx<<std::endl;

	if(iDBG>0) std::cout<<"     pt    eta    phi   nef    cfe   ntrkpt1 alphamax    r0    amax2d    amax2df  meanz jet_fpt jet_fnpile  2dipfrac  pid   partet"<<std::endl;
	for(int i=0;i<4;i++) {
	  if(AM[i]<0.002&&iDBG>0) std::cout<<"BAD BAD CAT"<<std::endl; 
	  if(iDBG>0){std::cout
            <<std::setw(8)<<std::setprecision(3)<<jet_pt->at(i)
            <<std::setw(8)<<std::setprecision(3)<<jet_eta->at(i)
            <<std::setw(8)<<std::setprecision(3)<<jet_phi->at(i)
            <<std::setw(8)<<std::setprecision(3)<<jet_nef->at(i)
	    <<std::setw(8)<<std::setprecision(3)<<jet_cef->at(i)
	    <<std::setw(8)<<std::setprecision(3)<<jet_ntrkpt1[i]
	    <<std::setw(8)<<std::setprecision(3)<<AM[i]
	    <<std::setw(8)<<std::setprecision(3)<<r0[i]
	    <<std::setw(8)<<std::setprecision(3)<<amax2D[i]
	    <<std::setw(8)<<std::setprecision(3)<<a3d[i]
	    <<std::setw(8)<<std::setprecision(3)<<jet_meanz[i]
	    <<std::setw(8)<<std::setprecision(3)<<jet_fpt[i]
	    <<std::setw(8)<<std::setprecision(3)<<jet_fnpile[i]
	    <<std::setw(8)<<std::setprecision(3)<<amax2Df2[i]
	    <<std::setw(8)<<std::setprecision(3)<<jet_pid_maxEt[i]
	    <<std::setw(8)<<std::setprecision(3)<<jet_maxET_part[i]
	    <<std::endl;}
	}  
	if(iDBG>0) {
	  std::cout<<"this event has "<<bigbs.size()<<" big bs"<<std::endl;
	      	      std::cout<<"this event has "<<ts.size()<<" topss"<<std::endl;
	  for(unsigned int k=0;k<bigbs.size();k++) {
	    std::cout<<"b pt eta phi "<<gp_pt->at(bigbs[k])<<" "<<gp_eta->at(bigbs[k])<<" "<<gp_phi->at(bigbs[k])<<std::endl;
	  }
	}
	    
	if(iDBG>0) {
	  for(int i=0;i<4;i++) {
	    std::cout<<" for jet "<<i<<std::endl;
            std::cout<<"  with tracks "<<std::endl;
            std::cout<<" #     pt     eta   phi    weight  ipxy    ipxysig  refx    refy    refz   quality  algo nmissinner nmissouter"<<std::endl;
            vector<float> track_pts = track_pt->at(i);
            vector<float> track_etas = track_eta->at(i);
            vector<float> track_phis = track_phi->at(i);
            vector<float> track_pvWeights = track_pvWeight->at(i);
            vector<int> track_sources = track_source->at(i);
            vector<int> track_qualitys = track_quality->at(i);
            vector<float> track_ipXYs = track_ipXY->at(i);
            vector<float> track_ipZs = track_ipZ->at(i);
            vector<float> track_ipXYSigs = track_ipXYSig->at(i);
            vector<float> track_ref_xs = track_ref_x->at(i);
            vector<float> track_ref_ys = track_ref_y->at(i);
            vector<float> track_ref_zs = track_ref_z->at(i);
            vector<int> track_algos = track_algo->at(i);
	    vector<int> track_nMissInnerTrkLayerss = track_nMissInnerTrkLayers->at(i);
	    vector<int> track_nMissOuterTrkLayerss = track_nMissOuterTrkLayers->at(i);

            for (unsigned itrack=0; itrack<track_pts.size(); itrack++) {
	      if((track_sources[itrack]==0)&&((track_qualitys[itrack]&4)>0)&& (fabs(pv_z->at(0)-track_ref_zs[itrack])<ps.pilecut)) {
	        std::cout
                <<itrack
                <<std::setw(8)<<std::setprecision(3)<<track_pts[itrack]
                <<std::setw(8)<<std::setprecision(3)<<track_etas[itrack]
                <<std::setw(8)<<std::setprecision(3)<<track_phis[itrack]
                <<std::setw(8)<<std::setprecision(3)<<track_pvWeights[itrack]
                <<std::setw(9)<<std::setprecision(3)<<track_ipXYs[itrack]
                <<std::setw(8)<<std::setprecision(3)<<track_ipXYSigs[itrack]
                <<std::setw(8)<<std::setprecision(3)<<track_ref_xs[itrack]
                <<std::setw(8)<<std::setprecision(3)<<track_ref_ys[itrack]
                <<std::setw(8)<<std::setprecision(3)<<track_ref_zs[itrack]
                <<std::setw(8)<<std::setprecision(3)<<track_qualitys[itrack]
                <<std::setw(8)<<std::setprecision(3)<<track_algos[itrack]
                <<std::setw(8)<<std::setprecision(3)<<track_nMissInnerTrkLayerss[itrack]
                <<std::setw(8)<<std::setprecision(3)<<track_nMissOuterTrkLayerss[itrack]
		     <<std::endl;
	      }
	    }// loop over tracks
	  }  // loop over jets
	} // iDBG

	// some plots for events that pass all cuts
	if(nemerging>=ps.NemergingCut) {
	  H_T3->Fill(OverFlow(HT,H_T3));   
	  hpvfinal->Fill(OverFlow(pv_z->at(0),hpvfinal));
          hnvtxfinal->Fill(OverFlow(nVtx,hnvtxfinal));
          hntrkfinal->Fill(OverFlow(nTracks,hntrkfinal));
	  bool hasBjet = false;
	  float aab=0.;
	  for(int i=0;i<4;i++) {
	    if(emerging[i]) {
 		if (jet_pid_maxEt[i]==5||jet_pid_maxEt[i]==8) hasBjet = true;
	      hmeanzfinal->Fill(OverFlow(jet_meanz[i]-pv_z->at(0),hmeanzfinal));
	      hjetptfrfinal->Fill(std::min(jet_fpt[i],1.2));
	      hsum2Dffinal->Fill(OverFlow(amax2Df2[i],hsum2Dffinal));
	      hjntrkfinal->Fill(OverFlow(jet_ntrkpt1[i],hjntrkfinal));
	      hfpilefinal->Fill(OverFlow(jet_fnpile[i],hfpilefinal));
	      hptmaxfinal->Fill(std::min(jet_ptmax[i],399.5));
              vector<int> track_sources = track_source->at(i);
              vector<int> track_qualitys = track_quality->at(i);
              vector<float> track_ref_zs = track_ref_z->at(i);
              for (unsigned itrack=0; itrack<track_ref_zs.size(); itrack++) {
	        if((track_sources[itrack]==0)&&((track_qualitys[itrack]&4)>0)&& (fabs(pv_z->at(0)-track_ref_zs[itrack])<ps.pilecut)) {
	          hdzfinal->Fill(OverFlow(pv_z->at(0)-track_ref_zs[itrack],hdzfinal));
	        }
	      }
	    }  // end emerging
	    for(int j=i+1;j<4;j++) {
	      if(fabs(jet_meanz[i]-jet_meanz[j])>aab) {
	        aab=fabs(jet_meanz[i]-jet_meanz[j]);
	      }
	    } 
          } //end loop over jets
	if (hasBjet) hhasbfinal->Fill(1);
	else hhasbfinal->Fill(0);
          hdzjfinal->Fill(OverFlow(aab,hdzjfinal));
        if(iDBG>0) std::cout<<"npass  event is "<<npass<<" "<<event<<std::endl;
        if(iDBG>0) std::cout<<"nemerging nalmostemerging almostemerging2 "<<nemerging<<" "<<nalmostemerging<<" "<<nalmostemerging2<<std::endl;

	}  //end nemerging > nemerging cut



      }  // nemerging>0
    }  //selection besides on nemerging

      // make plots for fake rate studes
/*    if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4&&Canem) {
      for(Int_t j=0; j<NNNjet; j++) {
	if(basicjet[j]) {
	  hjptfrb->Fill(OverFlow(jet_pt->at(j),hjptfrb));
	  if(almostemerging2[j]){
	    hjptfra1->Fill(OverFlow(jet_pt->at(j),hjptfra1));
	    if(emerging[j]) {
	      hjptfra2->Fill(OverFlow(jet_pt->at(j),hjptfra2));
	    }
	  }
	}
      }
    }*/
      // check without Canem
    if(PVZ&&PVPT0&&C4jet&&CHT&&Cpt1&&Cpt2&&Cpt3&&Cpt4) {
      for(Int_t j=0; j<NNNjet; j++) {
	if(basicjet[j]) {
	  hjptfrbc->Fill(OverFlow(jet_pt->at(j),hjptfrbc));
	  if(almostemerging2[j]){
	    hjptfra1c->Fill(OverFlow(jet_pt->at(j),hjptfra1c));
	    if(emerging[j]) {
	      hjptfra2c->Fill(OverFlow(jet_pt->at(j),hjptfra2c));
	    }
	  }
	}
      }
    }

    }  // close if (otfile)
    // apply cuts sequentially



    if(PVPT0) {
    if(otfile) count->Fill("pv 0",1);
    if(otfile) acount->Fill(1.5);

    if(PVZ) {
    if(otfile) count->Fill("pv z",1);
    if(otfile) acount->Fill(2.5);

    if(C4jet) {
    if(otfile) count->Fill("4 jets",1);
    if(otfile) acount->Fill(3.5);

    // calculate HT and require it greater than some cut value
    if(CHT) {
    if(otfile) count->Fill("HT",1);
    if(otfile) acount->Fill(4.5);
    if(otfile) H_T2->Fill(HT);

    // do pT cuts on jets  
    if(Cpt1) {
    if(otfile) count->Fill("jet pt1",1);
    if(otfile) acount->Fill(5.5);


    if(Cpt2) {
    if(otfile) count->Fill("jet pt2",1);
    if(otfile) acount->Fill(6.5);


    if(Cpt3) {
    if(otfile) count->Fill("jet pt3",1);
    if(otfile) acount->Fill(7.5);


    if(Cpt4) {
    if(otfile) count->Fill("jet pt4",1);
    if(otfile) acount->Fill(8.5);





    if(Cnem) {  // emerging
    if(otfile) count->Fill("emerging",1);
    if(otfile) acount->Fill(9.5);

    if(Canem) {   // almost emerging
    //if (true){
    if(otfile) count->Fill("almostemerging",1);
    if(otfile) acount->Fill(10.5);

    if(Cmet) {
    if(otfile) count->Fill("MET",1);
    if(otfile) acount->Fill(11.5);

    if(Cmass) {
    if(otfile) count->Fill("mass",1);
    if(otfile) acount->Fill(12.5);

            npass+=1;

    }}}}}}}}}}}}




  }  // end of loop over events

  if(otfile) {
    TFile myfile(outputfilename,"RECREATE");
    count->LabelsDeflate();
    count->LabelsOption("v");
  //  count->LabelsOption("a");

    eventCountPreTrigger->Write();
    hnjet->Write();
    acount->Write();
    count->Write();
    hjetcut->Write();
    hjetcuta->Write();
    hpt->Write();
    heta->Write();
    heta2->Write();
    halpha->Write();
    //haMgj->Write();
    H_T->Write();
    H_T2->Write();
    H_T3->Write();
    H_T4->Write();
    hpt1->Write();
    hpt2->Write();
    hpt3->Write();
    hpt4->Write();
    h_nemg->Write();
    h_naemg->Write();
    hjetchf->Write();
    hbcut_ntrkpt1->Write();
    hacut_ntrkpt1->Write();
    hbcut_nef->Write();
    hacut_nef->Write();
    hbcut_cef->Write();
    hacut_cef->Write();
    hbcut_alphamax->Write();
    hacut_alphamax->Write();
    hbcut_medip->Write();
    hbcut_theta2d->Write();
    hHTnm1->Write();
    hHThltnm1->Write();
    hmassnm1->Write();
    htheta2D1nm1->Write();
    htheta2D2nm1->Write();
    htheta2D3nm1->Write();
    htheta2D4nm1->Write();
    hmetnm1->Write();
    hmet->Write();
    hpt1nm1->Write();
    hpt2nm1->Write();
    hpt3nm1->Write();
    hpt4nm1->Write();
    halphanm1->Write();
    halphadknm1->Write();
    halphadnm1->Write();
    hmedipnm1->Write();
    hlgmedipnm1->Write();
    hlgmedipdknm1->Write();
    hlgmedipdnm1 ->Write();
    hnmedipnm1->Write();
    hn2medipnm1->Write();
    hnHitsnm1->Write();
    hntrk1nm1->Write();
    hnemnm1->Write();
    hipXYEJ->Write();
    hipXYnEJ->Write();
    htvw->Write();
    htvwEJ->Write();
    hipXYSigEJ->Write();
    hipXYSignEJ->Write();
    hmedipXYEJ->Write();
    hmedipXYnEJ->Write();
    hmeanipXYEJ->Write();
    hmeanipXYnEJ->Write();
    hjptb->Write();
    hjpta->Write();
    hjptfrb->Write();
    hjptfra1->Write();
    hjptfra2->Write();
    hjptfrbc->Write();
    hjptfra1c->Write();
    hjptfra2c->Write();

    hpt1ko->Write();
    hpt2ko->Write();
    hpt3ko->Write();
    hpt4ko->Write();
    hmass->Write();

    hHTko->Write();

    /*hdkjetam->Write();
    hdkjetamo->Write();
    hdkjetam2d->Write();*/
    hdkjetmeanip->Write();
    hdkjetntr->Write();
    hdkjetmedip->Write();
    hdkjetlgmedip->Write();
    hdkjettrkip->Write();
    hdkjettrkips->Write();
    hdkjettrkw->Write();
    hdkjettrgip->Write();
    hdkjettrkdr->Write();
    ham2dfd->Write();
    ham2dfdk->Write();
    ham2dfb->Write();
    ham2dfgbb->Write();
    ham2dfg->Write();
    ham2dfud->Write();
    ham2dfbpt1->Write();
    ham2dfbpt2->Write();
    ham2dfbpt3->Write();
    ham2dfudpt1->Write();
    ham2dfudpt2->Write();
    ham2dfudpt3->Write();
    hlgam2dfud->Write();
hlgam2dfdk->Write();
hpvztrzd->Write();
hip2dsigd->Write();
hpvztrzdk->Write();
hip2dsigdk->Write();
hpvztrzud->Write();
hip2dsigud->Write();
hpvztrzb->Write();
hip2dsigb->Write();


    //hdjetam->Write();
    //hdjetam2d->Write();
    //hdjetamo->Write();
    hdjetmeanip->Write();
    hdjetntr->Write();
    hdjetlgmedip->Write();
    hdjetmedip->Write();
    hdjettrkip->Write();
    hdjettrkips->Write();
    hdjettrkw->Write();
    hdjettrgip->Write();
    hdjettrkdr->Write();
    hmeanz->Write();
    hmeanzd->Write();
    hmeanzdk->Write();
    hmeanzfa->Write();
    hmeanzpa->Write();
    hntrkpt1zmpa->Write();
    hntrkpt1zmfa->Write();
    h2dpa->Write();
    h2dfa->Write();
    hmzamd->Write();
    h2damd->Write();
    hmznamd->Write();
    h2dnamd->Write();
    hbigb->Write();
    hpvpre->Write();
    hpvfinal->Write();
    hnvtxpre->Write();
    hnvtxfinal->Write();
    hntrkpre->Write();
    hntrkfinal->Write();
    hjetptfrpre->Write();
    hjetptfrfinal->Write();
    hjntrkpre->Write();
    hjntrkfinal->Write();
    hdzpre->Write();
    hdzfinal->Write();
    hfpilepre->Write();
    hfpilefinal->Write();
    hptmaxpre->Write();
    hptmaxfinal->Write();
    hdzjpre->Write();
    hdzjfinal->Write();
    hsum2Dfpre->Write();
    hsum2Dffinal->Write();
    hsum2Dfd->Write();
    hsum2Dfdk->Write();

    hptallpre->Write();
    hptudpre->Write();
    hptspre->Write();
    hptcpre->Write();
    hptbpre->Write();
    hptgpre->Write();
    hptgbbpre->Write();
    hptallpree->Write();
    hptudpree->Write();
    hptspree->Write();
    hptcpree->Write();
    hptbpree->Write();
    hptgpree->Write();
    hptgbbpree->Write();


    hptallfinal->Write();
    hptudfinal->Write();
    hptsfinal->Write();
    hptcfinal->Write();
    hptbfinal->Write();
    hptgfinal->Write();
    hptgbbfinal->Write();
    hptallfinal2->Write();
    hptudfinal2->Write();
    hptsfinal2->Write();
    hptcfinal2->Write();
    hptbfinal2->Write();
    hptgfinal2->Write();
    hptgbbfinal2->Write();

    hptallfinale->Write();
    hptudfinale->Write();
    hptsfinale->Write();
    hptcfinale->Write();
    hptbfinale->Write();
    hptgfinale->Write();
    hptgbbfinale->Write();

    hmeanzpre->Write();
    hmeanzfinal->Write();
	hpvztrz->Write();
	hip2dsig->Write();

    //2d
    aMip->Write();
    haMvjpt->Write();

    haMvHT->Write();
    haMvnvtx->Write();
  /*  aMbh->Write();
    aMbh2D->Write();
    aMbh2Dd->Write();
    aMbh2Ddk->Write();
    aMbh2Daem->Write();
    aMmzd->Write();
    aMmzdk->Write();
*/
    adkwvd0->Write();
    adwvd0->Write();
    adkwviz->Write();
    adwviz->Write();

    hdjettrkahate  ->Write(); 
    hdkjettrkahate ->Write(); 
    hdjettrk3dsig  ->Write(); 
    hdkjettrk3dsig  ->Write(); 
    hdjettrk3dsig_sgn->Write(); 
    hdkjettrk3dsig_sgn->Write(); 
hnlgmedipnm1->Write();
    adk2Dr0->Write();
    ad2Dr0->Write();

    hdkipphi->Write();
    hdipphi->Write();
    h2ipvz->Write();

htheta2dd->Write();
htheta2db->Write();
htheta2dud->Write();
htheta2ddk->Write();
hmedipd->Write();
hmedipb->Write();
hmedipud->Write();
hmedipdk->Write();
hmipahated->Write();
hmipahatedk->Write();
hmipahateud->Write();
hmipahateb->Write();
hmipa3dd ->Write(); 
hmipa3db ->Write(); 
hmipa3dud->Write(); 
hmipa3ddk->Write(); 
hntrk1a3dd ->Write(); 
hntrk1a3db ->Write(); 
hntrk1a3dud->Write(); 
hntrk1a3ddk->Write(); 


hht4->Write();
h2ipvzd->Write();
h2ipvzb->Write();
h2ipvzud->Write();
h2ipvzdk->Write();
hmipmetd  ->Write();
hmipmetb  ->Write();
hmipmetud ->Write();
hmipmetdk ->Write();
hmipmassd ->Write();
hmipmassb ->Write();
hmipmassud->Write();
hmipmassdk->Write();

hmipmetad ->Write(); 
hmipmetab ->Write(); 
hmipmetaud->Write(); 
hmipmetadk->Write(); 
hmetad ->Write(); 
hmetab ->Write(); 
hmetaud->Write(); 
hmetadk->Write(); 
hmipipsigd ->Write();
hmipipsigb ->Write();
hmipipsigud->Write();
hmipipsigdk->Write();
hhasbfinal->Write();
hip2dd   ->Write(); 
hip2ddk  ->Write(); 
hip2dud  ->Write(); 
hip2db   ->Write(); 
hmedsipd ->Write(); 
hmedsipb ->Write(); 
hmedsipud->Write(); 
hmedsipdk->Write(); 
    myfile.Close();
  }

  tt->ResetBranchAddresses();
  
  delete jet_index;
  delete jet_source;
  delete jet_pt;
  delete jet_eta;
  delete jet_phi;
  delete jet_alphaMax;
  delete jet_cef;
  delete jet_nef;
  delete jet_chf;
  //  delete jet_phf;
  delete track_pt;
  delete track_eta;
  delete track_source;
  delete track_quality;
  delete track_index;
  delete track_jet_index;
  delete track_algo;
  delete track_ipZ;
  delete track_ipXY;
  delete track_ip3DSig;
  delete track_ipXYSig;
  delete track_ref_x;  
  delete track_ref_y;  
  delete track_ref_z;  

delete  acount;delete  count;delete  hjetcut;delete  hjetcuta;delete  hjetchf;delete  h_nemg;delete h_naemg;delete  hnjet;delete  hpt;delete  heta;delete  heta2;delete  halpha;delete  H_T;delete  H_T2;delete  H_T3;delete  H_T4;delete  hbcut_ntrkpt1;delete  hacut_ntrkpt1;delete  hbcut_nef;delete  hacut_nef;delete  hbcut_cef;delete  hacut_cef;delete  hbcut_alphamax;delete  hacut_alphamax;delete  hbcut_theta2d;delete  hbcut_medip;delete  hmetnm1;delete  hmet;delete  hmassnm1;delete  htheta2D1nm1;delete  htheta2D2nm1;delete  htheta2D3nm1;delete  htheta2D4nm1;delete  hHTnm1;delete  hHThltnm1;delete  hnHitsnm1;delete  hntrk1nm1;delete  hlgmedipnm1;delete  hmedipnm1;delete  hpt1nm1;delete  hpt2nm1;delete  hpt3nm1;delete  hpt4nm1;delete  halphanm1;delete  hnemnm1;delete  hpt1;delete  hpt2;delete  hpt3;delete  hpt4;delete  hipXYEJ;delete  hipXYnEJ;delete  htvw;delete  htvwEJ;delete  hnmedipnm1;delete  hnlgmedipnm1;delete  hn2medipnm1;delete  hjptfrb;delete  hjptfra1;delete  hjptfra2;delete  hjptfrbc;delete  hjptfra1c;delete  hjptfra2c;delete  hjptb;delete  hjpta;delete  hHTko;delete  hpt1ko;delete  hpt2ko;delete  hpt3ko;delete  hpt4ko;delete  hipXYSigEJ;delete  hipXYSignEJ;delete  hmedipXYEJ;delete  hmedipXYnEJ;delete  hmeanipXYEJ;delete  hmeanipXYnEJ;delete  hmass;
    delete  hdkjetmeanip;delete  hdkjetntr;delete  hdkjetlgmedip;delete  hdkjetmedip;delete  hdkjettrkip;delete  hdkjettrkips;delete  hdkjettrkw;delete  hdkjettrgip;delete  hdkjettrkdr;delete  ham2dfd;delete  ham2dfdk;delete  hdkjettrkahate;delete  hdjettrkahate;delete hdjettrk3dsig; delete hdkjettrk3dsig; delete hdjettrk3dsig_sgn; delete hdkjettrk3dsig_sgn;delete  hdzjpre;delete  hdzjfinal;
    delete  hdjetmeanip;delete  hdjetntr;delete  hdjetmedip;delete  hdjetlgmedip;delete  hdjettrkip;delete  hdjettrkips;delete  hdjettrkw;delete  hdjettrgip;delete  hdjettrkdr;delete  hmeanz;delete  hmeanzfa;delete  hmeanzpa;delete  hmeanzdk;delete  hmeanzd;delete  h2dpa;delete  h2dfa;delete  hntrkpt1zmpa;delete  hntrkpt1zmfa;delete  hbigb;delete  hpvpre;delete  hpvfinal;delete  hdzpre;delete  hdzfinal;delete  hmeanzpre;delete  hmeanzfinal;
    delete  hnvtxpre;delete  hnvtxfinal;delete  hntrkpre;delete  hntrkfinal;delete  hjetptfrpre;delete  hjetptfrfinal;
    delete  hjntrkpre;delete  hjntrkfinal;delete  hfpilepre;delete  hfpilefinal;delete  hptmaxpre;delete  hptmaxfinal;delete  hsum2Dfpre;delete  hsum2Dffinal;delete  hsum2Dfd;delete  hsum2Dfdk;
    delete  hptallpre;delete  hptudpre;delete  hptcpre;delete  hptspre;delete  hptgbbpre;delete  hptbpre;delete  hptgpre;
    delete  hptallfinal;delete  hptudfinal;delete  hptcfinal;delete  hptsfinal;delete  hptgbbfinal;delete  hptbfinal;delete  hptgfinal;
    delete  hptallfinal2;delete  hptudfinal2;delete  hptcfinal2;delete  hptsfinal2;delete  hptgbbfinal2;delete  hptbfinal2;delete  hptgfinal2;
    delete  hptallpree;delete  hptudpree;delete  hptcpree;delete  hptspree;delete  hptgbbpree;delete  hptbpree;delete  hptgpree;
    delete  hptallfinale;delete  hptudfinale;delete  hptcfinale;delete  hptsfinale;delete  hptgbbfinale;delete  hptbfinale;delete  hptgfinale;
    delete  ham2dfb;delete  ham2dfgbb;delete  ham2dfg;delete  ham2dfud;
    delete  ham2dfbpt1;delete  ham2dfbpt2;delete  ham2dfbpt3;
    delete  ham2dfudpt1;delete  ham2dfudpt2;delete  ham2dfudpt3;
    delete  hpvztrz;delete  hip2dsig;
    delete  htheta2dd; delete  htheta2db; delete  htheta2dud; delete  htheta2ddk; 
    delete  hmedipd; delete  hmedipb; delete  hmedipud; delete  hmedipdk ; delete  hlgam2dfud; delete  hlgam2dfdk; delete  hpvztrzd  ; delete  hip2dsigd ; delete  hpvztrzdk ; delete  hip2dsigdk; delete  hpvztrzud ; delete  hip2dsigud; delete  hpvztrzb  ; delete  hip2dsigb;
delete  hmipa3dd  ;delete  hmipa3db  ;delete  hmipa3dud ;delete  hmipa3ddk;
delete hntrk1a3dd ; delete hntrk1a3db ; delete hntrk1a3dud; delete hntrk1a3ddk; 
delete hip2dd   ; delete hip2ddk  ; delete hip2dud  ; delete hip2db   ; 
delete hmedsipd ; delete hmedsipb ; delete hmedsipud; delete hmedsipdk; 

delete hmipmetd  ;
delete hmipmetb  ;
delete hmipmetud ;
delete hmipmetdk ;
delete hmipmassd ;
delete hmipmassb ;
delete hmipmassud;
delete hmipmassdk;
delete hmipmetad  ;
delete hmipmetab  ;
delete hmipmetaud ;
delete hmipmetadk ;
delete hmetad  ;
delete hmetab  ;
delete hmetaud ;
delete hmetadk ;
delete hlgmedipdknm1; delete hlgmedipdnm1;
   delete halphadknm1;
   delete halphadnm1;
delete hmipipsigd ;
delete hmipipsigb ;
delete hmipipsigud;
delete hmipipsigdk;
delete hhasbfinal;
  f->Close();
  


  return npass;
}
