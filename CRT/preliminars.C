#define CRT_an_cxx
#include <fstream>
#include <chrono>
#include "TApplication.h"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <TObjString.h>
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TF1.h>
#include <TFile.h>
#include <string>
#include <TLegend.h>
#include <TGraphErrors.h>
#include <vector>
#include <TString.h>
#include <TH1.h>
#include "TROOT.h"
#include "TTimer.h"
#include "TLatex.h"
#include "TGClient.h"
#include "TRootCanvas.h"
#include <TDatime.h>
#include <time.h>

#include "CRT.h"

#define QcutMin 50
#define Qcutmax 1000

using namespace std;

double time_mu[sideNum][scintNum];

// Histograms to create
void CRT_an::CreateHistDict(){
  // si potrebbe fare con un un dataframe pandas da csv modificabile da GUI (pandasgui) che parte in python prima dell'eseguibile C++
  // in alternativa si può modificare il file tables.C (github) e fare la stessa cosa dalla gui di root

  double  T_min = 150, T_max = 350;
  int T_bins = 50;

  hist_dict = {
      CreatePair("Tmip_off",            1, 8, 2, "Time (MIP) w/o offset subtr.", "Side %i - Scint. %i",           "T", "ns", T_bins, T_min, T_max),
      CreatePair("TnoCut_off",          1, 8, 2, "Time (Q > 2pc) w/o offset subtr.", "Side %i - Scint. %i",       "T", "ns", T_bins, T_min, T_max)
  };
}

void time_pre_draw(TVirtualPad* pad, TH1 *hist, int x, int y)
{
  hist->Draw();
  double tpeak = hist->GetBinCenter(hist->GetMaximumBin());
  double tmax = tpeak + 15, tmin = tpeak - 15;
  //TF1 l = TF1("logn", "[0]*ROOT::Math::lognormal_pdf(x,log([1]),log([2]))", tmin, tmax);
  TF1 l = TF1("g", "gaus", tmin, tmax);
  l.SetParameter(1, tpeak);
  l.SetParameter(2, 2);
  hist->Fit(&l, "R");

  time_mu[y][x] = l.GetParameter(1);
}

int charge_cut(double q){
  if (q > QcutMin && q < Qcutmax) return 1;
}

int is_mip(double Q[sideNum][scintNum], int isc){
  int ismip = 1;
  double *Q_A = Q[0], *Q_B = Q[1];

  if( charge_cut(Q_A[isc]) && charge_cut(Q_B[isc])){
    if (isc > 0){
      if(Q_A[isc-1] > 40 || Q_B[isc-1] > 40){
      	ismip = 0;
      }
    }
    if (isc < scintNum - 1){
      if(Q_A[isc+1] > 40 || Q_B[isc+1] > 40){
	      ismip = 0;
      }
    }
  }
  else ismip = 0;
  return ismip;
}

void CRT_an::Loop(){
  if (fChain == 0) return;

  Long64_t nentries = fChain->GetEntriesFast();
  gStyle->SetOptFit(1);
  Long64_t nbytes = 0, nb = 0;
  int sideTmp, scintTmp;
  double Qtmp, Ttmp, Chi2tmp;
  double vp = 13; //cm/ns

  // LOOP OVER ENTRIES
  for (Long64_t jentry=0; jentry<nentries;jentry++) { 
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   
    nbytes += nb;

    double Q[sideNum][scintNum] = {0.};
    double T[sideNum][scintNum] = {0.};

    // LOOP OVER HITS
    for(int hit=0; hit<nCry; hit++){
      sideTmp=iSide[hit];
      scintTmp = iScint[hit];

      Qtmp = Qval[hit];
      Ttmp = templTime[hit]; 

      // Saving data for each side and each scintillator
      Q[sideTmp][scintTmp]=Qtmp;
      T[sideTmp][scintTmp]= Ttmp;
      
      GetHist("TnoCut_off", scintTmp, sideTmp)->Fill(Ttmp);
    }
  
    // Looping over scintillators
    for(int isc = 0; isc <scintNum; isc++){

      if( Q[1][isc]>0. && Q[0][isc]>0.){ //if there is > 2pC in both sides (refers to Qcut in analysis_CRT.C)

        if (is_mip(Q, isc)){
          for(int isd = 0; isd < sideNum; isd++){
            GetHist("Tmip_off", isc, isd)->Fill(T[isd][isc]);
          }
        }
      }
    }
  }
  
  hist_dict["TnoCut_off"]->pre_draw = &time_pre_draw;
  hist_dict["Tmip_off"]->pre_draw = &time_pre_draw;

  for (auto& hist_matrix: hist_dict) {
    hist_matrix.second->draw_all();
  }

  CloseOutFile();

  ofstream outf("time_off.csv");
  for (int isd = 0; isd < sideNum; isd++)
  {
    for (int isc = 0; isc < scintNum; isc++)
    {
      outf << time_mu[isd][isc];
      if (isc != scintNum - 1)
        outf << ",";
      else
        outf << endl;
    }
  }
  outf.close();

  cout << "Press Ctrl+C to quit" << endl;
}


int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Usage: %s [infile_name] [outfile_name]\n", argv[0]);
    exit(-1);
  }
  TApplication* myapp = new TApplication("myapp", 0, 0);

  TFile *f = new TFile(argv[2], "RECREATE");

  int window_close_handle = 1; //closes app if a canvas is closed

  CRT_an *a = new CRT_an(argv[1], f, window_close_handle);

  gStyle->SetOptStat("emr"); //entries, mean and rms
  gStyle->SetTitleFontSize(0.12);
  gStyle->SetStatFont(63);
  gStyle->SetStatFontSize(10);
  gStyle->SetStatW(0.4);
  gStyle->SetTitleFontSize(0.1); 
  gROOT->ForceStyle();

  a->Loop();
  myapp->Run(true);
}
