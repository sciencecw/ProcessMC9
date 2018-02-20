#include <iostream>
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <map>

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "vector"
#include "vector"
using std::vector;
#include "algorithm"

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TKey.h>
#include <TCollection.h>
#include <TObject.h>
#include <TMath.h>

struct Parmset {
  float HTcut;
  float pt1cut;
  float pt2cut;
  float pt3cut;
  float pt4cut;
  float jetacut; // always = 2

  //float alphaMaxcut; // should be gotten rid of
  float a3dcut;
  float ahatecut;
  float medIPcut;
  float NemfracCut; //always 0.9
  float CemfracCut; //always 0.9
  int   ntrk1cut;
  float pvz_cut;

  int   NemergingCut;
  float metcut;
  float mass;       //kak:850GeV?
  float masscut;
  float theta2dcut;
  //float ipsigcut;
  //float ipsigcut2; // cut for frac2DSig
// TODO:  to be added
  float pilecut;
  float pilecut2; //for fnpile
};

// function prototypes
/*int EMJcounter(bool otfile, bool hasPre, const char* inputfilename,const char* outputfilename, 
        const Parmset &ps);

void QCDhists(std::string outdir, int process, int icutset, int incutset, float goalintlum,
    int nbin, float* xsec, int* nfiles, std::string* binnames,std::string* aaname,
    std::string ohname, bool hasPre, Parmset &ps);

void  HistNorm(vector<double>& norm,int nbin,float* xsec, int* nfiles, 
    std::string* binnames, std::string outname);

TH1F* HistMan(float goalintlum,std::string thisHIST,vector<double>& histnorm, 
    vector<double>& outnorm,int nbin,float* xsec, int* nfiles, std::string* binnames,
    std::string outname);

TH2F* HistMan2(float goalintlum,std::string thisHIST,vector<double>& histnorm, 
    vector<double>& outnorm,int nbin,float* xsec, int* nfiles, std::string* binnames,
    std::string outname);
*/
void QCDhists(int imode, std::string outdir, int ibin, float goalintlum,  
int nbin, float* xsec, int* nfiles, std::string* binnames,std::string* filesnames,
std::string sumhistname,  bool hasPre,bool donorm, bool blind, const Parmset &ps);
int EMJselect(bool otfile, bool hasPre, const char* inputfilename,const char* outputfilename, const Parmset &ps, bool blind,bool isQCD);


//calculate invariant mass with various pairs
inline float masspair(vector<float> &e, vector<float> &px, vector<float> &py, 
                      vector<float> &pz, int ix1, int ix2) {
      return sqrt( pow((e[ix1]+e[ix2]),2) - pow((px[ix1]+px[ix2]),2)
        - pow((py[ix1]+py[ix2]),2) - pow((pz[ix1]+pz[ix2]),2));
}

inline float median(vector<float> &array) {
  int thesize = array.size();
  if (thesize % 2 == 0)
    {
      return (array[thesize/2]+array[thesize/2 - 1])/2.;
    }
  return (array[(thesize)/2]);
}

inline std::string decomment(std::string line) {

    std::string::size_type n = line.find("#");
    if(n!= std::string::npos)
      line.erase(n);
    return line;
}

inline void printParam(const Parmset &ps) {
  std::cout<<"HT cut        is "<<ps.HTcut<<std::endl;
  std::cout<<"pt1 cut       is "<<ps.pt1cut<<std::endl;
  std::cout<<"pt2 cut       is "<<ps.pt2cut<<std::endl;
  std::cout<<"pt3 cut       is "<<ps.pt3cut<<std::endl;
  std::cout<<"pt4 cut       is "<<ps.pt4cut<<std::endl;

  std::cout<<"a3d cut       is "<<ps.a3dcut<<std::endl;
  std::cout<<"median IP cut is "<<ps.medIPcut<<std::endl;
  std::cout<<"nTrk1 cut     is "<<ps.ntrk1cut<<std::endl;
  std::cout<<"PVz cut       is "<<ps.pvz_cut<<std::endl;
  std::cout<<"Nemerging cut is "<<ps.NemergingCut<<std::endl;

  std::cout<<"MET cut       is "<<ps.metcut<<std::endl;
  std::cout<<"central mass  is "<<ps.mass<<std::endl;
  std::cout<<"massRange cut is "<<ps.masscut<<std::endl;
  std::cout<<"theta2D cut   is "<<ps.theta2dcut<<std::endl;

  //std::cout<<"IPSig cut     is "<<ps.ipsigcut<<std::endl;
  //std::cout<<"a2DSig cut    is "<<ps.a2dsigcut<<std::endl;
  //std::cout<<"f2DSig cut    is "<<ps.f2dsigcut<<std::endl;
  std::cout<<"ahate cut     is "<<ps.ahatecut<<std::endl;
  std::cout<<"pile cut      is "<<ps.pilecut<<std::endl;
  std::cout<<"pile cut2     is "<<ps.pilecut2<<std::endl;
}
