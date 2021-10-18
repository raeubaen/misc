#include <fstream>
#include <chrono>
#include "TApplication.h"
#include <TLine.h>
#include "Analysis.h"

using namespace std;

double landau_mu[sideNum][scintNum], landau_mu_err[sideNum][scintNum];

// ------------------------------------------------------------ TO DO
// modificare is_mip (prima o poi) per raffinare il taglio
// sistemare il codice per essere pronti a equalizzare
// plot sigma/MPV
// asymm vds. Q

void time_pre_draw(TVirtualPad* pad, TH1 *hist, int x, int y)
{
  double tpeak = hist->GetBinCenter(hist->GetMaximumBin());
  double tmax = tpeak + 50, tmin = tpeak - 50;
  TF1 l = TF1("logn", "[0]*ROOT::Math::lognormal_pdf(x,log([1]),log([2]))", tmin, tmax);
  hist->Fit(&l, "R");
}

void z_pre_draw(TVirtualPad* pad, TH1 *obj, int x, int y){

  TF1 l = TF1("l", "gaus", -20, 20);
  obj->Fit(&l, "R");

  obj->SetTitle(Form("Scint %i", 2*y+x));
  pad->SetTopMargin(0.2);
  pad->SetBottomMargin(0.3);
  obj->GetXaxis()->SetLabelOffset(0.05);
  obj->GetXaxis()->SetTitleOffset(1);
  //pad->SetLogy();
}

void big_graph_pre_draw(TVirtualPad* pad, TH1 *obj, int x, int y){
  obj->GetXaxis()->SetLabelOffset();
  obj->GetXaxis()->SetTitleOffset(0.8);
  pad->SetLeftMargin(0.15);
  obj->GetXaxis()->SetLabelSize(0.05);
  obj->GetYaxis()->SetLabelSize(0.05);
  obj->GetYaxis()->SetTitleSize(0.06);
  obj->GetYaxis()->SetTitleSize(0.06);
  pad->SetBottomMargin(0.2);
}

void QH2_pre_draw(TVirtualPad* pad, TH1 *obj, int x, int y){
  pad->SetTopMargin(0.15);
  pad->SetBottomMargin(0.2);
  TAxis *xa = obj->GetXaxis(), *ya = obj->GetYaxis();
  TLine *l = new TLine(xa->GetXmin(), ya->GetXmin(), xa->GetXmax(), ya->GetXmax());
  xa->SetLabelOffset(0);
  xa->SetTitleOffset(0.8);
  xa->SetLabelSize(0.05);
  ya->SetLabelSize(0.05);
  ya->SetTitleSize(0.06);
  ya->SetTitleSize(0.06);
  obj->SetTitle(Form("Scint %i", 4*y+x));
  obj->Draw("zcol");
  l->Draw("same");
}

void QSharing_pre_draw(TVirtualPad* pad, TH1 *obj, int x, int y){
  pad->SetTopMargin(0.15);
  pad->SetBottomMargin(0.2);

  obj->GetXaxis()->SetLabelOffset(0);
  obj->GetXaxis()->SetTitleOffset(0.8);
  obj->GetXaxis()->SetLabelSize(0.05);
  obj->GetYaxis()->SetLabelSize(0.05);
  obj->GetYaxis()->SetTitleSize(0.06);
  obj->GetYaxis()->SetTitleSize(0.06);
  obj->SetTitle(Form("Scint %i", 3*y+x +1 ));
}

void charge_pre_draw(TVirtualPad* pad, TH1 *hist, int x, int y)
{
  double qpeak = hist->GetBinCenter(hist->GetMaximumBin());
  double qmax = qpeak + 150, qmin = qpeak - 50;
  TF1 l = TF1("l", "landau", qmin, qmax);
  hist->Fit(&l, "R");
  if (TString(hist->GetName()).Contains("Qmip"))
  {
    landau_mu[y][x] = l.GetParameter(1);
    landau_mu_err[y][x] = l.GetParError(1);
  }
}

void asymm_pre_draw(TVirtualPad* pad, TH1 *hist, int x, int y){
  hist->GetXaxis()->SetTitleOffset(0.61);
}


// Histograms to create
void Analysis::CreateHistDict(vector <Double_t> pars){ 

  long nentries = (long)pars[0];
  double Q_min=50, Q_max=1550, Z_min = -30, Z_max=30, T_min = -25, T_max = 25;
  int Q_bins = 75, T_bins = 50, Z_bins = 50; //Z_bins vede essere messo a 100 

  hist_dict = {
    CreatePair("TotQperside",  1, 2, 1, "Total Charge on each side","Side %i",                  "Q",        "pC", Q_bins,   Q_min, Q_max),

    CreatePair("TotQperscint", 1, 8, 1, "Total Charge on each scint.", "Scint %i",              "Q",        "pC", Q_bins,   Q_min, Q_max),
    CreatePair("Tsummip",      1, 8, 1, "Time sum (MIP)","Scint %i",                            "T",        "ns", T_bins,   -100,  100),
    CreatePair("SideAsymm",    1, 8, 1, "L/R charge asymmetry", "Scint %i",  "#frac{Q[A]-Q[B]}{Q[A]+Q[B]}",  "",  400,      -1,    1),

    CreatePair("Zmip",         1, 2, 4, "Z (MIP)", "%i %i",                                     "Z",        "cm", Z_bins,   -25,   25),

    CreatePair("Pedestal",     1, 8, 2, "Pedestal", "Side %i - Scint %i",                       "Pedestal", "pC", 100,      -50,   50),
    CreatePair("Qmip",         1, 8, 2, "Charge (MIP)", "Side %i - Scint %i",                   "Q",        "pC", Q_bins,   Q_min, Q_max),
    CreatePair("Tmip",         1, 8, 2, "T-t0 (MIP)", "Side %i - Scint %i",                     "T",        "ns", T_bins,   T_min, T_max),
  //CreatePair("TnoCut",       1, 8, 2, "T-t0 (Q > 2pc)", "Side %i - Scint %i",                 "T",        "ns", T_bins,   T_min, T_max),
    CreatePair("Chi2",         1, 8, 2, "Chi2", "Side %i - Scint %i",                           "Chi2",     "",   100,      0,     15),
    CreatePair("QnoCut",       1, 8, 2, "Charge (> 2 pC)", "Side %i - Scint %i",                "Q",        "pC", Q_bins,   0,     1500),

    CreatePair("ZScintmip",    2, 1, 1, "Z vs. Scint (MIP)","",                                 "Z",        "cm", Z_bins*5, Z_min, Z_max, "#Scint.", "",   8,      0,         8),

    CreatePair("QSharing",     2, 3, 2, "Q[i] vs. Q[i+1]+Q[i-1]", "Scint %i",                   "Q[i]",     "pC", Q_bins,   0,     Q_max, "Q[i+1]+Q[i-1]", "pC", Q_bins, 0,   Q_max),
    CreatePair("QABnoCut",     2, 4, 2, "Q_A vs. Q_B (> 2 pC)", "Scint %i",                     "Q[A]",     "pC", Q_bins,   0,     1500,  "Q[B]",    "pC", Q_bins, 0,         1500),
    CreatePair("QABmip",       2, 4, 2, "Q_A vs. Q_B (MIP)", "Scint %i",                        "Q[A]",     "pC", Q_bins,   Q_min, Q_max, "Q[B]",    "pC", Q_bins, Q_min,     Q_max),
   
    CreatePair("QTmip",        2, 8, 2, "Charge vs. T-t0 (MIP)", "Side %i - Scint %i",          "Q",        "pC", Q_bins,   Q_min, Q_max, "T",       "ns", T_bins, T_min,     T_max),
    CreatePair("QChi2mip",     2, 8, 2, "Chi2 vs. Charge(MIP)", "Side %i - Scint. %i",          "Q",        "pC", Q_bins,   Q_min, Q_max, "Chi2",    "",   500,    0,         500),    
    CreatePair("QScalemip",    2, 8, 2, "Templ. scale fact. vs. Q (MIP)", "Side %i - Scint %i", "Q",        "pC", Q_bins,   Q_min, Q_max, "Scale",   "",   Q_bins, Q_min*0.6, Q_max*0.6),
    CreatePair("PedChi2",      2, 8, 2, "Pedestal vs templ. Chi2","Side %i - Scint %i",         "Pedestal", "pC", 100,      -50,   50,    "Chi2",    "",   500,    0,         500),        
    CreatePair("PedBaseline",  2, 8, 2, "Pedestal vs baseline","Side %i - Scint %i",            "Pedestal", "pC", 100,      -50,   50,    "Baseline","mV", 400,    -20,       20),
  //CreatePair("PedNevent",    2, 8, 2, "Pedestal vs. Ev. Num.", "Side %i - Scint %i",          "Pedestal", "pC", 300,      -50,   50,    "Evnum",   nentries,     0,         nentries),
    
    CreatePair("Qzmip",        2, 8, 1, "Z vs. Charge (MIP)", "Scint. %i",                      "Q",        "pC", Q_bins,   Q_min, Q_max, "Z",       "cm", Z_bins*4, Z_min,     Z_max), //fare TProfile

  };
}

double** matrix_from_csv(TString filename, int nrow=sideNum, int ncol=scintNum){
  ifstream inf(filename.Data());
  TString temp;
  double** arr = new double*[nrow];
  for(int r=0; inf >> temp; r++){
    arr[r] = new double[ncol];
    for(int c=0; c<ncol; c++){
      TObjString * tempobj = (TObjString *)(temp.Tokenize(",")->At(c));
      arr[r][c] = atof(tempobj->GetString());
    }
  }
  return arr;
}


void Analysis::Loop(){
  if (fChain == 0) return;

  double **time_off = matrix_from_csv("time_off.csv");
  //double **q_peaks = matrix_from_csv("q_peaks.csv");

  Long64_t nentries = fChain->GetEntriesFast();
  cout << "Number of events: " << nentries << endl;
  Long64_t nbytes = 0, nb = 0;
  int sideTmp, scintTmp;
  double Qtmp, Ttmp, Chi2tmp, Vtmp;
  double vp = 13; //cm/ns

  CreateHistDict({(Double_t)nentries});

  // LOOP OVER ENTRIES
  for (Long64_t jentry=0; jentry<nentries;jentry++) { //la parte interna al loop andrebbe messa una una funzione così come le parti prima e dopo, così la parte delicata sta in Loop nel .h
    Long64_t ientry = LoadTree(jentry);
    if (jentry%500 == 0) 
      cout << Form("Processing event n.%lld of %lld: %i%%", jentry, nentries, (int)((double)jentry/nentries * 100)) << endl;
    if (ientry < 0) break;

    nb = fChain->GetEntry(jentry);   
    nbytes += nb;

    double **Q, **T, **V, **Chi2, **Ped, **Scale, **Baseline;
    
    list<double ***> arr_list = {&Q, &T, &V, &Chi2, &Ped, &Scale, &Baseline};

    for(double*** &arr: arr_list) {
      *arr = new double*[sideNum];
      for(int i = 0; i<sideNum; i++){
        (*arr)[i] = new double[scintNum]();
      } 
    }    

    // LOOP OVER HITS
    for(int hit=0; hit<nCry; hit++){
      sideTmp=iSide[hit];
      scintTmp = iScint[hit];

      FillArrays({
        {Q, Qval[hit]}, {Chi2, templChi2[hit]}, {T, templTime[hit] - time_off[sideTmp][scintTmp]},
        {V, Vmax[hit]}, {Ped, pedL[hit]}, {Scale, templFit[hit][0]}, {Baseline, templFit[hit][2]},
      }, sideTmp, scintTmp);

      // forse è più veloce se filla qui gli istogrammi per cui non va applicata la selezione mip
    }
  
    // Looping over scintillators
    double Q_sum_tmp[2] = {0.};
    for(int isc = 0; isc <scintNum; isc++){

      for(int isd = 0; isd < sideNum; isd++){
        Q_sum_tmp[isd] += Q[isd][isc];
        //GetHist("PedNevent", isc, isd)->Fill(Ped[isd][isc], jentry);
      }
      
      GetHist("TotQperscint", isc, 0)->Fill(Q[0][isc] + Q[1][isc]);    


      if( Q[1][isc]>0. && Q[0][isc]>0.){ //if Q > 2pC in both sides (refers to Qcut in analysis_CRT.C)

        GetHist("QABnoCut", isc%4, isc/4)->Fill( Q[0][isc], Q[1][isc]);
        GetHist("SideAsymm", isc, 0)->Fill( (Q[0][isc] - Q[1][isc]) / (Q[0][isc] + Q[1][isc]) );
        if((isc+1)%8 > 1) //not 0 or 7
          GetHist("QSharing", (isc-1)%3, (isc-1)/3)->Fill(  Q[0][isc] + Q[1][isc] , Q[0][isc+1] + Q[1][isc+1] + Q[0][isc-1] + Q[1][isc-1]);

        for(int isd = 0; isd < sideNum; isd++){
          FillHists({
            {"QnoCut", Q}, //{"TnoCut", T}, 
          }, isd, isc);
          FillHists({{"PedBaseline", {Ped, Baseline}}}, isd, isc);
        }

        if (is_mip(Q, V, isc)){ 
          GetHist("Zmip", isc%2, isc/2)->Fill((T[0][isc]-T[1][isc])*vp/2);

          if (Chi2[0][isc] < Chi2max && Chi2[1][isc] < Chi2max) {
            for(int isd = 0; isd < sideNum; isd++){
              FillHists({
                {"Tmip", T}, {"Pedestal", Ped}, {"Chi2", Chi2}
              }, isd, isc);
              FillHists({
               {"QTmip", {Q, T}}, {"QScalemip", {Q, Scale}}, 
              }, isd, isc);
            }

            GetHist("Tsummip", isc, 0)->Fill(T[0][isc] + T[1][isc]);
            GetHist("Qzmip", isc, 0)->Fill(Q[0][isc] + Q[1][isc], (T[0][isc] - T[1][isc])*vp/2);
            GetHist("ZScintmip", 0, 0)->Fill((T[0][isc] - T[1][isc])*vp/2, isc);
          }

          for(int isd = 0; isd < sideNum; isd++){

            GetHist("Qmip", isc, isd)->Fill(Q[isd][isc]);
            GetHist("QChi2mip", isc, isd)->Fill(Q[isd][isc], Chi2[isd][isc]);
            GetHist("PedChi2", isc, isd)->Fill(Ped[isd][isc], Chi2[isd][isc]);
          }

          GetHist("QABmip", isc%4, isc/4)->Fill( Q[0][isc], Q[1][isc]);	
          
        }
      }
    }

    for(int isd = 0; isd < sideNum; isd++) GetHist("TotQperside", isd, 0)->Fill(Q_sum_tmp[isd]);
  }
  
  // si può passare come argomento all'inizializzazione (lasciando il default se non specificato)
  hist_dict["QnoCut"]->pre_draw = &charge_pre_draw;
  hist_dict["Qmip"]->pre_draw = &charge_pre_draw;
  hist_dict["Zmip"]->pre_draw = &z_pre_draw;
  hist_dict["QABmip"]->pre_draw = &QH2_pre_draw;
  hist_dict["QABnoCut"]->pre_draw = &QH2_pre_draw;
  hist_dict["QSharing"]->pre_draw = &QSharing_pre_draw;
  hist_dict["ZScintmip"]->pre_draw = &big_graph_pre_draw;
  hist_dict["TotQperside"]->pre_draw = &big_graph_pre_draw;
  hist_dict["SideAsymm"]->pre_draw = &asymm_pre_draw;


  for (auto& hist_matrix: hist_dict) {
    hist_matrix.second->draw_all();
  }

  CloseOutFile();

  ofstream outf("landau.csv");
  outf << "#mua, smua, mub, smub" << endl;
  for (int isc=0; isc<scintNum; isc++){
    for (int isd=0; isd<sideNum; isd++){
      outf << landau_mu[isd][isc] << " " << landau_mu_err[isd][isc];
      if(isd!=sideNum-1) outf << " ";
      else outf << endl;
    }
  }
  outf.close();


  TCanvas *c = new TCanvas("Equalization", "Equalization");
  c->cd();
  double scint[8] = {0, 1, 2, 3, 4, 5, 6, 7}, scint_err[8] = {0};

  
  double mean = 0, wsum=0, w;
  for(int isd=0; isd<sideNum; isd++){
    for(int isc=0; isc<scintNum; isc++){
      w = 1; // /(landau_mu_err[isd][isc]*landau_mu_err[isd][isc]);
      mean += w*landau_mu[isd][isc];
      wsum += w;
    }
  }
  mean /= wsum;

  TGraphErrors *eq0 = new TGraphErrors(8, scint, landau_mu[0], scint_err, landau_mu_err[0]);
  eq0->SetName("eq0");
  eq0->SetTitle("Charge peaks");
  eq0->GetXaxis()->SetTitle("# Scintillator");
  eq0->GetYaxis()->SetTitle("Charge (pC)");
  eq0->GetXaxis()->SetLimits(-1, 8);

  eq0->SetLineColor(kBlack);
  eq0->SetMarkerColor(kBlack);
  eq0->SetMarkerSize(1.5);
  eq0->SetMarkerStyle(21);

  eq0->Draw("ap");


  TGraphErrors *eq1 = new TGraphErrors(8, scint, landau_mu[1], scint_err, landau_mu_err[1]);
  eq1->SetName("eq1");
  eq1->SetLineColor(kBlue);

  eq1->SetMarkerColor(kBlue);
  eq1->SetMarkerSize(1.5);
  eq1->SetMarkerStyle(21);

  eq1->Draw("p");

  TLine *l = new TLine(0, mean, 8, mean);
  l->Draw("same");
  auto legend = new TLegend(0.1,0.7,0.48,0.9);
  legend->AddEntry("eq0","Side 0");
  legend->AddEntry("eq1","Side 1");
  legend->Draw();
  c->SaveAs("plots/Equalization.root");
  c->SaveAs("plots/Equalization.C");

  cout << "Press Ctrl+C to quit" << endl;

}

int main(int argc, char*argv[]){ 
  Analysis::Run(argc, argv, 1);
}
