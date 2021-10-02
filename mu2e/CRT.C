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
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TF1.h>
#include <TFile.h>
#include <string>
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


// Histograms to create
void CRT_an::CreateHistDict(TFile *f){
  // si potrebbe fare con un un dataframe pandas da csv modificabile da GUI (pandasgui) che parte in python prima dell'eseguibile C++
  // in alternativa si può modificare il file tables.C (github) e fare la stessa cosa dalla gui di root
  hist_dict = {
      {"QnoCut",          new HistMatrix(f, "QnoCut", 1, 8, 2, "Charge (> 2 pC) - Side %i - Scint. %i", "Q", "pC", 75, 0, 1500)},
      {"Qmip",            new HistMatrix(f, "Qmip", 1, 8, 2, "Charge (MIP) - Side %i - Scint. %i", "Q", "pC", 75, 0, 1500)},
      {"QTmip",           new HistMatrix(f, "QTmip", 2, 8, 2, "Time vs. Charge (MIP) - Side %i - Scint %i", "Time", "ns", 50, 200, 400, "Q", "pC", 75, 50, 1550)},
      {"TotalQeachside",  new HistMatrix(f, "TotalQeachside", 1, 2, 1, "Total Charge on side %i", "Q", "pC", 75, 50, 1550)},
      {"TotalQeachscint", new HistMatrix(f, "TotalQeachscint""TotalQeachscint", 1, 8, 1, "Total Charge on scint. %i", "Q", "pC", 75, 50, 1550)},
      {"QChi2mip",        new HistMatrix(f, "QChi2mip", 2, 8, 2, "Template Chi2 vs. Charge(MIP) - Side %i - Scint. %i", "Chi2", "", 100, 0, 200, "Q", "pC", 75, 50, 1550)},
      {"Qzmip",           new HistMatrix(f, "Qzmip", 2, 8, 1, "Z vs. Charge (MIP) - Side %i - Scint. %i", "Z", "cm", 100, -1000, 1000, "Q", "pC", 75, 50, 1550)},
      {"QABnoCut",        new HistMatrix(f, "QABnoCut", 2, 8, 1, "Charge A vs. Charge B", "Q", "pC", 75, 0, 1500, "Q", "pC", 75, 0, 1500)},
      {"QABmip",          new HistMatrix(f, "QABmip", 2, 8, 1, "Charge A vs. Charge B (MIP)", "Q", "pC", 75, 0, 1500, "Q", "pC", 75, 0, 1500)},
      {"Zmip",            new HistMatrix(f, "Zmip", 1, 8, 1, "Z (MIP)", "Z", "cm", 100, -3000, 3000)},
      {"Tsummip",         new HistMatrix(f, "Tsummip", 1, 8, 1, "Time sum (MIP)", "Time", "ns", 100, 0, 1000)}
  };
}


void charge_pre_draw(TH1* hist, int x, int y){
      double qpeak = hist->GetBinCenter(hist->GetMaximumBin());
      double qmax = qpeak + 150, qmin = qpeak - 50;
      TF1 l = TF1("l", "landau", qmin, qmax);
      hist->Fit(&l, "R");
      double landau_mu = l.GetParameter(0);
      double landau_mu_err = l.GetParError(0);
      cout << landau_mu << "  " << landau_mu_err << endl;
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


void CRT_an::Loop(TFile *outfile){
  if (fChain == 0) return;

  CreateHistDict(outfile);

  Long64_t nentries = fChain->GetEntriesFast();
  gStyle->SetOptFit(1);
  gStyle->SetTitleSize(0, "t");
  Long64_t nbytes = 0, nb = 0;
  int sideTmp, scintTmp, canvas_index;
  double Qtmp, Vtmp, Ttmp, Chi2tmp, qpeak, qmin, qmax, landau_mu, landau_mu_err;
  double vp = 13;
  TH1F *histTmp;
  TF1 *l;

  for (Long64_t jentry=0; jentry<nentries;jentry++) { 
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   
    nbytes += nb;

    double Q[sideNum][scintNum] = {0.};
    double T[sideNum][scintNum] = {0.};
    double Chi2[sideNum][scintNum] = {0.};

    for(int hit=0; hit<nCry; hit++){
      sideTmp=iSide[hit];
      scintTmp = iScint[hit];

      Qtmp = Qval[hit];
      Chi2tmp = templChi2[hit];
      Ttmp = templTime[hit];

      Q[sideTmp][scintTmp]=Qtmp;
      Chi2[sideTmp][scintTmp]=Chi2tmp;
      T[sideTmp][scintTmp]=Ttmp;
      
      GetHist("QnoCut", scintTmp, sideTmp)->Fill(Qtmp);
    }
  
    double Q_sum_tmp[2] = {0.};
    for(int isc = 0; isc <scintNum; isc++){

      for(int isd = 0; isd < sideNum; isd++) Q_sum_tmp[isd] += Q[isd][isc];
      
      GetHist("TotalQeachscint", isc, 0)->Fill(Q[0][isc] + Q[1][isc]);    

      if( Q[1][isc]>0. && Q[0][isc]>0.){ //if there is > 2pC in both sides (refers to Qcut in analysis_CRT.C)

        GetHist("QABnoCut", isc, 0)->Fill( Q[0][isc], Q[1][isc]);

        if (is_mip(Q, isc)){
          for(int isd = 0; isd < sideNum; isd++){
            GetHist("Qmip", isc, isd)->Fill(Q[isd][isc]);
            GetHist("QChi2mip", isc, isd)->Fill(Q[isd][isc], Chi2[isd][isc]);
            GetHist("QTmip", isc, isd)->Fill(Q[isd][isc], T[isd][isc]);	
          } 

          GetHist("QABmip", isc, 0)->Fill( Q[0][isc], Q[1][isc]);	

          GetHist("Zmip", isc, 0)->Fill((T[0][isc]-T[1][isc])*vp/2);

          GetHist("Qzmip", isc, 0)->Fill(Q[0][isc] + Q[1][isc], (T[0][isc] - T[1][isc])*vp/2);

          GetHist("Tsummip", isc, 0)->Fill(T[0][isc] + T[1][isc]);

        }
      }
    }

    for(int isd = 0; isd < sideNum; isd++) GetHist("TotalQeachside", isd, 0)->Fill(Q_sum_tmp[isd]);
  }
  
  hist_dict["QnoCut"]->pre_draw = &charge_pre_draw;
  hist_dict["Qmip"]->pre_draw = &charge_pre_draw;

  for (auto& hist_matrix: hist_dict) {
    hist_matrix.second->draw_all();
  }

  outfile->Close();
  cout << "Press Ctrl+C to quit" << endl;

}

  int main(int argc, char*argv[]){
    if(argc != 2){
      printf("Usage: %s [file_name]\n", argv[0]);
      exit(-1);
    }
    TApplication *myapp = new TApplication("myapp", 0, 0);

    TFile *f = new TFile("CRT_hists.root","RECREATE");

    CRT_an *a = new CRT_an(argv[1]);

    a->Loop(f);
    myapp->Run(true);
  }
