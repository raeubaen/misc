#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <TApplication.h>

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TF1.h>
#include <TTree.h>
#include <TFile.h>
#include <string>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <vector>
#include <TString.h>
#include "TROOT.h"
#include "TTimer.h"
#include "TLatex.h"
#include "TGClient.h"
#include "TRootCanvas.h"

using namespace std;


void gain_q_pde(TString holder, TString sipm, TString version){

    TTree *tree = new TTree();
    tree->ReadStream(cin, "V:G:sG:Q:sQ:PDE:sPDE", ',');

    const Long64_t trial = tree->GetEntries();
    double overvoltageErr[trial];
    std::fill_n(overvoltageErr, trial, 0.05); //50 mV

    double overvoltage[trial], Gain[trial], GainErr[trial], Charge[trial], ChargeErr[trial], PDE[trial], PDEErr[trial];

    TTreeReader myReader(tree);

    TTreeReaderValue<Float_t> rV(myReader, "V"), rG(myReader, "G"), rsG(myReader, "sG"), rQ(myReader, "Q"), rsQ(myReader, "sQ");
    TTreeReaderValue<Float_t> rPDE(myReader, "PDE"), rsPDE(myReader, "sPDE");

    int k = 0;
    while(myReader.Next()){
        overvoltage[k] = *rV;
        Gain[k] = *rG;
        GainErr[k] = *rsG;
        Charge[k] = *rQ;
        ChargeErr[k] = *rsQ;
        PDE[k] = *rPDE;
        PDEErr[k] = *rsPDE;
        k++;
    }

    for(k=0; k<trial; k++){
        cout << overvoltage[k] << "  " << Gain[k] << endl;
    }

    TGraphErrors * RGain_over= new TGraphErrors(trial, overvoltage, Gain ,overvoltageErr, GainErr);
    TGraphErrors * RCharge_over= new TGraphErrors(trial, overvoltage,Charge,overvoltageErr,ChargeErr);
    TGraphErrors * RPDE_over= new TGraphErrors(trial, overvoltage,PDE,overvoltageErr,PDEErr);

    TCanvas* canRQ = new TCanvas("canRQ","canRQ");
    canRQ->Connect("TCanvas", "Closed()", "TApplication", gApplication, "Terminate()");
    RCharge_over->SetMarkerStyle(20);
    RCharge_over->SetMarkerSize(1);
    RCharge_over->SetTitle("Charge/Charge(V_{op}) vs Overvoltage");
    RCharge_over->GetXaxis()->SetTitle("Overvoltage [V]");
    RCharge_over->GetYaxis()->SetTitle("Q/Q(V_{op})");
    RCharge_over->Draw("APE");
    TF1 *fuRQ = new TF1("fuRQ","pol1",6,11);
    fuRQ->SetParameter(0,1);
    fuRQ->SetParameter(1,0.2);
    fuRQ->ReleaseParameter(0);
    fuRQ->ReleaseParameter(1);
    RCharge_over->Fit("fuRQ","R");
    gPad->Update();

    TCanvas* canRPDE = new TCanvas("canRPDE","canRPDE");
    canRPDE->Connect("TCanvas", "Closed()", "TApplication", gApplication, "Terminate()");
    RPDE_over->SetMarkerStyle(20);
    RPDE_over->SetMarkerSize(1);
    RPDE_over->SetTitle("PDE/PDE(V_{op}) vs Overvoltage");
    RPDE_over->GetXaxis()->SetTitle("Overvoltage [V]");
    RPDE_over->GetYaxis()->SetTitle("PDE/PDE(V_{op})");
    RPDE_over->Draw("APE");
    TF1 *fuRPDE= new TF1("fuRPDE","[0]*(1-(1/( ([1]*x*(exp(-[2]/sqrt(x)))) * ([1]*x*(exp(-[2]/sqrt(x)))) ) ) )",6,11);
    fuRPDE->SetParameter(0,1);
    fuRPDE->SetParameter(1,0.2);
    fuRPDE->SetParameter(2,0.2);
    fuRPDE->ReleaseParameter(0);
    fuRPDE->ReleaseParameter(1);
    fuRPDE->ReleaseParameter(2);
    RPDE_over->Fit("fuRPDE","R");
    gPad->Update();

    TCanvas* canRG = new TCanvas("canRG","canRG");
    canRG->Connect("TCanvas", "Closed()", "TApplication", gApplication, "Terminate()");
    RGain_over->SetMarkerStyle(20);
    RGain_over->SetMarkerSize(1);
    RGain_over->Draw("APE");
    RGain_over->SetTitle("Gain/Gain(V_{op}) vs Overvoltage");
    RGain_over->GetXaxis()->SetTitle("Overvoltage [V]");
    RGain_over->GetYaxis()->SetTitle("G/G(V_{op})");
    TF1 *fuRG = new TF1("fuRG","pol1", 6,11);
    fuRG->SetParameter(0,1);
    fuRG->SetParameter(1,0.2);
    fuRG->ReleaseParameter(0);
    fuRG->ReleaseParameter(1);
    RGain_over->Fit("fuRG","R");
    gPad->Update();

    TCanvas *ra =new TCanvas("ra","ra");
    ra->Connect("TCanvas", "Closed()", "TApplication", gApplication, "Terminate()");

    ra->Divide(3,1);
    ra->cd(1);
    RGain_over->Draw("APE");
    RGain_over->SetTitle("Gain/Gain(V_{op}) vs Overvoltage");
    ra->cd(2);
    RCharge_over->Draw("APE");
    RCharge_over->SetTitle("Charge/Charge(V_{op}) vs Overvoltage");
    ra->cd(3);
    RPDE_over->Draw("APE");
    RPDE_over->SetTitle("PDE/PDE(V_{op}) vs Overvoltage");


    chrono::time_point<chrono::system_clock> end;

    std::chrono::milliseconds ms(30000);
    end = chrono::system_clock::now() + ms; // this is the end point

    while (chrono::system_clock::now() < end) { 
        gSystem->ProcessEvents(); 
        gSystem->Sleep(10);
    }

}