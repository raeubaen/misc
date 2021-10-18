#include <fstream>
#include <chrono>
#include "TApplication.h"
#include <TLine.h>
#include "Analysis.h"
#include <TSpline.h>
using namespace std;

double landau_mu[sideNum][scintNum], landau_mu_err[sideNum][scintNum];

// ------------------------------------------------------------ TO DO
// modificare is_mip (prima o poi) per raffinare il taglio
// sistemare il codice per essere pronti a equalizzare
// plot sigma/MPV
// asymm vds. Q

void Analysis::Loop(){
  if (fChain == 0) return;

  //Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nentries = 30;
  cout << "Number of events: " << nentries << endl;
  Long64_t nbytes = 0, nb = 0;
  int sideTmp, scintTmp;
  double Qtmp, Ttmp, Chi2tmp, Vtmp;
  double vp = 13; //cm/ns
  
  TFile *f = new TFile("splines4_tagger.root");
  TGraphErrors* gt = (TGraphErrors*)f->Get("gtempl");

  TSpline3 *sp3 = new TSpline3("sp3", gt);

  TGraph *meas, *fit;
  double wave_meas[100] = {0}, wave_fit[100] = {0}, time_arr[100] = {0};
  meas = new TGraph(100, time_arr, wave_meas);
  TCanvas *c = new TCanvas("caaa", "caaa");
  meas->Draw("AP");

  // LOOP OVER ENTRIES
  for (Long64_t jentry=0; jentry<nentries;jentry++) { //la parte interna al loop andrebbe messa una una funzione così come le parti prima e dopo, così la parte delicata sta in Loop nel .h
    Long64_t ientry = LoadTree(jentry);

    if (ientry < 0) break;

    nb = fChain->GetEntry(jentry);   
    nbytes += nb;

    // LOOP OVER HIT
    for(int hit=0; hit<nCry; hit++){
      sideTmp=iSide[hit];
      scintTmp = iScint[hit];

      if(Qval[hit] > 50 && scintTmp==4 && sideTmp==1 ){
        for(int i=0; i<100; i++){
          wave_meas[i] = wave[hit][i+50];

          time_arr[i] = time[i+50];
          wave_fit[i] = templFit[hit][0]*sp3->Eval(time[i+50] - templFit[hit][1])+ templFit[hit][2];
        }
        meas = new TGraph(100, time_arr, wave_meas);
        fit = new TGraph(100, time_arr, wave_fit);

        c->cd();
        meas->Draw("AP");
        fit->Draw("SAME");
      }
    }
  
  }

  CloseOutFile();

}

int main(int argc, char*argv[]){ 
  Analysis::Run(argc, argv, 1);
}
