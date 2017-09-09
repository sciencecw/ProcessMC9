/*#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include <iomanip>
#include <locale>

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
// global variables
*/

#include "paramSet.h"

// for codes like EMJselect
float goalintlum=20.;
int nbin;
int ibin;
float* xsec;
int* nfiles;
std::string* binnames;
std::string* filesname;
std::string sumhist;
bool hasPre;
bool donorm=true;
bool blind=false;
bool b16003=false;
float themass=1000.;


/*void  QCDhists(int imode, std::string outdir, int ibin, float goalintlum,
int nbin, float* xsec, int* nfiles, std::string* binnames,
std::string* aaname,std::string sumhist,
bool hasPre,bool donorm, bool blind, bool b16003,float themass);

std::string decomment(std::string line) {

    std::string::size_type n = line.find("#");
    if(n!= std::string::npos)
      line.erase(n);
    return line;
}
*/

int main(int argc, char *argv[])
{ 

  int imode = (int)atoi((argv[1]));
  int ibin = (int)atoi((argv[2]));
  char* parmfile = (argv[3]); 
  char* outdir = (argv[4]); 

  std::cout<<"bin is "<<ibin<<std::endl;
  std::cout<<"input file is "<<parmfile<<std::endl;
  Parmset ps;
  ps.HTcut=1000;
  ps.pt1cut=300;
  ps.pt2cut=300;
  ps.pt3cut=250;
  ps.pt4cut=150;
  ps.jetacut=2; // always = 2

  ps.a3dcut=0.25;
  ps.ahatecut=10;
  ps.medIPcut=0.05;
  ps.NemfracCut=0.9; //always 0.9
  ps.CemfracCut=0.9; //always 0.9
  ps.ntrk1cut=1;
  ps.pvz_cut=50;

  ps.NemergingCut=2;
  ps.metcut=0;
  ps.mass=800;       
  ps.masscut=300;
  ps.theta2dcut=0;
  ps.pilecut=1000;
  ps.pilecut2=1000;

 // ipsigcut=2;

  printParam(ps); 

  std::ifstream infile;
  std::string line;
  infile.open(parmfile);
  if(infile.is_open()) {
    
    getline(infile,line);
    std::istringstream(decomment(line))>>hasPre;
    if(!hasPre) std::cout<<" unable to normalize since missing hasPre"<<std::endl;

    getline(infile,line);
    std::istringstream(decomment(line))>>nbin;
    std::cout<<" assuming generated in "<<nbin<<" HT bins"<<std::endl;

    xsec = new float[nbin];
    getline(infile,line);
    std::cout<<line<<std::endl;
    std::istringstream aline(decomment(line));
    for(int i=0;i<nbin;i++) aline>>xsec[i];
    for(int i=0;i<nbin;i++) std::cout<<"  xsec["<<i<<"]="<<xsec[i]<<std::endl;

    nfiles = new int[nbin];
    getline(infile,line);
    aline.str(decomment(line).c_str());
    std::cout<<line<<std::endl;
    for(int i=0;i<nbin;i++) aline>>nfiles[i];
    for(int i=0;i<nbin;i++) std::cout<<"  nfiles["<<i<<"]="<<nfiles[i]<<std::endl;
    
    binnames = new std::string[nbin];
    getline(infile,line);
    aline.str(decomment(line).c_str());
    std::cout<<line<<std::endl;
    for(int i=0;i<nbin;i++) aline>>binnames[i];
    for(int i=0;i<nbin;i++) std::cout<<"  binnames["<<i<<"]="<<binnames[i]<<std::endl;


    filesname = new std::string[nbin];
    getline(infile,line);
    aline.str(decomment(line).c_str());
    std::cout<<line<<std::endl;
    for(int i=0;i<nbin;i++) aline>>filesname[i];
    for(int i=0;i<nbin;i++) std::cout<<" filesname["<<i<<"]="<<filesname[i]<<std::endl;

    //std::string sumhist_temp;
    getline(infile,line);
    aline.str(decomment(line).c_str());
    std::cout<<line<<std::endl;
    aline>>sumhist;
    std::cout<<"sumhist is "<<sumhist<<std::endl;
    sumhist=sumhist+".root";
    std::cout<<" output histogram is "<<sumhist<<std::endl;



  }
  std::string odir(outdir);
  QCDhists(imode,odir,ibin,goalintlum,
nbin,xsec,nfiles,binnames, filesname,
sumhist, hasPre, donorm, blind, ps);



}
