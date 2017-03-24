#include <fstream>
#include <sstream>
#include <iostream>
#include "TSystem.h"
#include "Artus/Utility/interface/RoccoR2016.h"

const double CrystalBall2016::pi    = TMath::Pi();
const double CrystalBall2016::SPiO2 = sqrt(TMath::Pi()/2.0);
const double CrystalBall2016::S2    = sqrt(2.0);

int RocRes2016::getBin(double x, const int NN, const double *b) const{
    for(int i=0; i<NN; ++i) if(x<b[i+1]) return i;
    return NN-1;
}

RocRes2016::RocRes2016(){
    reset();
}

void RocRes2016::reset(){
    NETA=1;
    NTRK=1;
    NMIN=1;
    for(int H=0; H<NMAXETA; ++H){
	BETA[H]=0;
	kDat[H]=1.0;
	kRes[H]=1.0; //this is important
	for(int F=0; F<NMAXTRK+1; ++F){
	    ntrk[H][F]=0;
	    dtrk[H][F]=0;
	}
	for(int F=0; F<NMAXTRK; ++F){
	    width[H][F]=1;
	    alpha[H][F]=10;
	    power[H][F]=10;
	    cb[H][F].init(0.0, width[H][F], alpha[H][F], power[H][F]);
	}
    }
    BETA[NMAXETA]=0;
}

int RocRes2016::getEtaBin(double feta) const{
    return getBin(feta,NETA,BETA);
}

int RocRes2016::getNBinDT(double v, int H) const{
    return getBin(v,NTRK,dtrk[H]);
}

int RocRes2016::getNBinMC(double v, int H) const{
    return getBin(v,NTRK,ntrk[H]);
}

void RocRes2016::dumpParams(){

    std::cout << NMIN << std::endl;
    std::cout << NTRK << std::endl;
    std::cout << NETA << std::endl;
    for(int H=0; H<NETA+1; ++H) std::cout << BETA[H] << " ";
    std::cout << std::endl;
    for(int H=0; H<NETA; ++H){
	for(int F=0; F<NTRK; ++F){
		std::cout << Form("%8.4f %8.4f %8.4f | ", width[H][F], alpha[H][F], power[H][F]);
	}
	std::cout << std::endl;
    }
    for(int H=0; H<NETA; ++H){
	for(int F=0; F<NTRK+1; ++F){
		std::cout << Form("%8.4f %8.4f| ", ntrk[H][F], dtrk[H][F]);
	}
	std::cout << std::endl;
    }
    for(int H=0; H<NETA; ++H){
	for(int F=0; F<NTRK; ++F){
	    cb[H][F].init(0.0, width[H][F], alpha[H][F], power[H][F]);
	    std::cout << Form("%8.4f %8.4f %8.4f | ", rmsA[H][F], rmsB[H][F], rmsC[H][F]);
	}
	std::cout << std::endl;
    }
}


	
void RocRes2016::init(std::string filename){
    std::ifstream in(filename.c_str());
    char tag[4];
    int type, sys, mem, isdt, var, bin;	
    std::string s;
    while(std::getline(in, s)){
	std::stringstream ss(s); 
	if(s.substr(0,4)=="RMIN")       ss >> tag >> NMIN;
	else if(s.substr(0,4)=="RTRK")  ss >> tag >> NTRK;
	else if(s.substr(0,4)=="RETA")  {
	    ss >> tag >> NETA;
	    for(int i=0; i< NETA+1; ++i) ss >> BETA[i];
	}
	else if(s.substr(0,1)=="R")  {
	    ss >> tag >> type >> sys >> mem >> isdt >> var >> bin; 
	    if(var==0) for(int i=0; i<NTRK; ++i) ss >> rmsA[bin][i];  
	    if(var==1) for(int i=0; i<NTRK; ++i) ss >> rmsB[bin][i];  
	    if(var==2) for(int i=0; i<NTRK; ++i) {
		ss >> rmsC[bin][i];  
		rmsC[bin][i]/=100;
	    }
	    if(var==3) for(int i=0; i<NTRK; ++i) ss >> width[bin][i];  
	    if(var==4) for(int i=0; i<NTRK; ++i) ss >> alpha[bin][i];  
	    if(var==5) for(int i=0; i<NTRK; ++i) ss >> power[bin][i];  
	}
	else if(s.substr(0,1)=="T")  {
	    ss >> tag >> type >> sys >> mem >> isdt >> var >> bin; 
	    if(isdt==0) for(int i=0; i<NTRK+1; ++i) ss >> ntrk[bin][i];  
	    if(isdt==1) for(int i=0; i<NTRK+1; ++i) ss >> dtrk[bin][i];  
	}
	else if(s.substr(0,1)=="F")  {
	    ss >> tag >> type >> sys >> mem >> isdt >> var >> bin; 
	    if(var==0){
		if(isdt==0) for(int i=0; i<NETA; ++i) ss >> kRes[i];  
		if(isdt==1) for(int i=0; i<NETA; ++i) ss >> kDat[i];  
	    }
	}
    }

    for(int H=0; H<NETA; ++H){
	for(int F=0; F<NTRK; ++F){
	    cb[H][F].init(0.0, width[H][F], alpha[H][F], power[H][F]);
	}
    }
    in.close();
}

double RocRes2016::Sigma(double pt, int H, int F) const{
    double dpt=pt-45;
    return rmsA[H][F] + rmsB[H][F]*dpt + rmsC[H][F]*dpt*dpt;
}

double RocRes2016::getUrnd(int H, int F, double w) const{
    return ntrk[H][F]+(ntrk[H][F+1]-ntrk[H][F])*w; 
}

double RocRes2016::kSpread(double gpt, double rpt, double eta, int n, double w) const{
    int     H = getBin(fabs(eta), NETA, BETA);
    int     F = n>NMIN ? n-NMIN : 0;
    double  v = getUrnd(H, F, w);
    int     D = getBin(v, NTRK, dtrk[H]);
    double  kold = gpt / rpt;
    double  u = cb[H][F].cdf( (kold-1.0)/kRes[H]/Sigma(gpt,H,F) ); 
    double  knew = 1.0 + kDat[H]*Sigma(gpt,H,D)*cb[H][D].invcdf(u);
    if(knew<0) return 1.0;
    return kold/knew;
}

double RocRes2016::kSmear(double pt, double eta, TYPE type, double v, double u) const{
    int H = getBin(fabs(eta), NETA, BETA);
    int F = type==Data? getNBinDT(v, H) : getNBinMC(v, H);
    double K = type==Data ? kDat[H] : kRes[H]; 
    double x = K*Sigma(pt, H, F)*cb[H][F].invcdf(u);
    return 1.0/(1.0+x);
}

double RocRes2016::kSmear(double pt, double eta, TYPE type, double w, double u, int n) const{
    int H = getBin(fabs(eta), NETA, BETA);
    int F = n>NMIN ? n-NMIN : 0;
    if(type==Data) F = getNBinDT(getUrnd(H, F, w), H);
    double K = type==Data ? kDat[H] : kRes[H]; 
    double x = K*Sigma(pt, H, F)*cb[H][F].invcdf(u);
    return 1.0/(1.0+x);
}

double RocRes2016::kExtra(double pt, double eta, int n, double u, double w) const{
    int H = getBin(fabs(eta), NETA, BETA);
    int F = n>NMIN ? n-NMIN : 0;
    double  v = ntrk[H][F]+(ntrk[H][F+1]-ntrk[H][F])*w;
    int     D = getBin(v, NTRK, dtrk[H]);
    double RD = kDat[H]*Sigma(pt, H, D);
    double RM = kRes[H]*Sigma(pt, H, F);
    if(RD<=RM) return 1.0; 
    double r=cb[H][F].invcdf(u);
    if(fabs(r)>5) return 1.0; //protection against too large smearing
    double x = sqrt(RD*RD-RM*RM)*r;
    if(x<=-1) return 1.0;
    return 1.0/(1.0 + x); 
}


//-------------------------------------

const double RocOne2016::MPHI=-TMath::Pi();

int RocOne2016::getBin(double x, const int NN, const double *b) const{
    for(int i=0; i<NN; ++i) if(x<b[i+1]) return i;
    return NN-1;
}

int RocOne2016::getBin(double x, const int nmax, const double xmin, const double dx) const{
    int ibin=(x-xmin)/dx;
    if(ibin<0) return 0; 
    if(ibin>=nmax) return nmax-1;
    return ibin;
}

RocOne2016::RocOne2016(){
    reset();
}

RocOne2016::RocOne2016(std::string filename, int iTYPE, int iSYS, int iMEM){
    init(filename, iTYPE, iSYS, iMEM);
}


bool RocOne2016::checkSYS(int iSYS, int iMEM, int kSYS, int kMEM){
    if(iSYS==0 && iMEM==0)	      return true;
    if(iSYS==kSYS && iMEM==kMEM)      return true;
    return false;
}

bool RocOne2016::checkTIGHT(int iTYPE, int iSYS, int iMEM, int kTYPE, int kSYS, int kMEM){
    if(iTYPE!=kTYPE) return false;
    if(iSYS!=kSYS)   return false;
    if(iMEM!=kMEM)   return false;
    return true;
}

void RocOne2016::reset(){
    RR.reset();

    NETA=1;
    NPHI=1;
    DPHI=2*TMath::Pi()/NPHI;
    for(int H=0; H<NMAXETA; ++H){
	BETA[H]=0;
	D[MC][H]=1.0;
	D[DT][H]=1.0;
	for(int F=0; F<NMAXPHI; ++F){
	    for(int T=0; T<2; ++T){
		M[T][H][F]=1;
		A[T][H][F]=0;
	    }
	}
    }
    BETA[NMAXETA]=0;
}

void RocOne2016::init(std::string filename, int iTYPE, int iSYS, int iMEM){

    reset();

    RR.init(filename);

    std::ifstream in(filename.c_str());
    char tag[4];
    int type, sys, mem, isdt, var, bin;	

    bool initialized=false;

    std::string s;
    while(std::getline(in, s)){
	std::stringstream ss(s); 
	if(s.substr(0,4)=="CPHI")       {
	    ss >> tag >> NPHI;
	    DPHI=2*TMath::Pi()/NPHI;
	}
	else if(s.substr(0,4)=="CETA")  {
	    ss >> tag >> NETA;
	    for(int i=0; i< NETA+1; ++i) ss >> BETA[i];
	}
	else if(s.substr(0,1)=="C")  {
	    ss >> tag >> type >> sys >> mem >> isdt >> var >> bin; 
	    if(!checkTIGHT(type,sys,mem,iTYPE,iSYS,iMEM)) continue;
	    initialized=true;
	    if(var==0) for(int i=0; i<NPHI; ++i) { ss >> M[isdt][bin][i]; M[isdt][bin][i]/=100; M[isdt][bin][i]+=1.0;} 
	    if(var==1) for(int i=0; i<NPHI; ++i) { ss >> A[isdt][bin][i]; A[isdt][bin][i]/=100;} 

	}
	else if(s.substr(0,1)=="F")  {
	    ss >> tag >> type >> sys >> mem >> isdt >> var >> bin; 
	    if(var==1){
		for(int i=0; i<NETA; ++i) { 
		    ss >> D[isdt][i];  
		    D[isdt][i]/=10000;
		    D[isdt][i]+=1.0;
		}
	    }
	}
    }
    if(!initialized) std::cout << "Problem with input file: " << filename << std::endl;
    in.close();
}

double RocOne2016::kScaleDT(int Q, double pt, double eta, double phi) const{
    int H=getBin(eta, NETA, BETA);
    int F=getBin(phi, NPHI, MPHI, DPHI);
    double m=M[DT][H][F];
    double a=A[DT][H][F];
    double d=D[DT][H];

    double k=d/(m+Q*a*pt);
    return k;
}


double RocOne2016::kScaleMC(int Q, double pt, double eta, double phi, double kSMR) const{
    int H=getBin(eta, NETA, BETA);
    int F=getBin(phi, NPHI, MPHI, DPHI);
    double m=M[MC][H][F];
    double a=A[MC][H][F];
    double d=D[MC][H];
    double k=d/(m+Q*a*pt);
    return k*kSMR;
}

double RocOne2016::kScaleAndSmearMC(int Q, double pt, double eta, double phi, int n, double u, double w) const{
    double k=kScaleMC(Q, pt, eta, phi);
    return k*RR.kExtra(k*pt, eta, n, u, w);
}


double RocOne2016::kScaleFromGenMC(int Q, double pt, double eta, double phi, int n, double gt, double w) const{
    double k=kScaleMC(Q, pt, eta, phi);
    return k*RR.kSpread(gt, k*pt, eta, n, w);
}


double RocOne2016::kGenSmear(double pt, double eta, double v, double u, RocRes2016::TYPE TT) const{
    return RR.kSmear(pt, eta, TT, v, u);
}


//-------------------------------------


RoccoR2016::RoccoR2016(){}

RoccoR2016::RoccoR2016(std::string dirname){
    init(dirname);
}


void 
RoccoR2016::init(std::string dirname){

    std::string filename=Form("%s/config.txt", dirname.c_str());

    std::ifstream in(filename.c_str());
    std::string s;
    std::string tag;
    int si;
    int sn;
    while(std::getline(in, s)){
	std::stringstream ss(s); 
	ss >> tag >> si >> sn; 
	std::vector<RocOne2016> v;
	for(int m=0; m<sn; ++m){
	    std::string inputfile=Form("%s/%d.%d.txt", dirname.c_str(), si, m);
	    if(gSystem->AccessPathName(inputfile.c_str())) {
		std::cout << Form("Missing %8d %3d, using default instead...", si, m) << std::endl;
		v.push_back(RocOne2016(Form("%s/%d.%d.txt", dirname.c_str(),0,0),0,0,0));
	    }
	    else{
		v.push_back(RocOne2016(inputfile, 0, si, m));
	    }
	}
	RC.push_back(v);
    }

    in.close();
}

RoccoR2016::~RoccoR2016(){}



double RoccoR2016::kGenSmear(double pt, double eta, double v, double u, RocRes2016::TYPE TT, int s, int m) const{
    return RC[s][m].kGenSmear(pt, eta, v, u, TT);
}

double RoccoR2016::kScaleDT(int Q, double pt, double eta, double phi, int s, int m) const{
    return RC[s][m].kScaleDT(Q, pt, eta, phi);
}

double RoccoR2016::kScaleAndSmearMC(int Q, double pt, double eta, double phi, int n, double u, double w, int s, int m) const{
    return RC[s][m].kScaleAndSmearMC(Q, pt, eta, phi, n, u, w);
}

double RoccoR2016::kScaleFromGenMC(int Q, double pt, double eta, double phi, int n, double gt, double w, int s, int m) const{
    return RC[s][m].kScaleFromGenMC(Q, pt, eta, phi, n, gt, w);
}
