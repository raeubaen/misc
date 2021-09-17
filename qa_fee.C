#define qa_fee_cxx
#include "qa_fee.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TProfile.h>
#include <string>
#include <stdlib.h>
#include <iostream>

void qa_fee::Loop(TString HistoFile)
{
//   In a ROOT session, you can do:
//      Root > .L qa_fee.C
//      Root > qa_fee t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   cout << "Selected output file: " << HistoFile << endl;
   TFile *hfile=new TFile(HistoFile,"RECREATE");

   //***********************************************
   // Booking
   //***********************************************

   BookHist();

   //***********************************************
   // Fill timing histos
   //***********************************************

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      timing0->Fill(tmean0);
      timing1->Fill(tmean1);
      timing2->Fill(tmean2);
      timing3->Fill(tmean3);

      for(int t=0;t<1024;t++){
	wave_prof0->Fill(t,onda0[t]);
	wave_prof1->Fill(t,onda1[t]);
	wave_prof2->Fill(t,onda2[t]);
	wave_prof3->Fill(t,onda3[t]);
      }
   }

 //***********************************************
   // Find peak for timing
   //***********************************************
   
   TAxis *Xaxis = timing0->GetXaxis();

   //Double_t MinVal  = Q1->GetXaxis()->GetXmin();
   //Double_t MaxVal  = Q1->GetXaxis()->GetXmax();

   Int_t    Nbins   = timing0->GetXaxis()->GetNbins();
   
   Int_t Xmax = -10.;
   Int_t Ymax = -10.;
   Double_t Xval = 0.;
   Double_t Yval = 0.;
   
   for (Int_t i=1;i<Nbins;i++){
     Xval = Xaxis->GetBinCenter(i);
     Yval = timing1->GetBinContent(i);
     
     if( Yval>Ymax ){
       Xmax = Xval;
       Ymax = Yval;
     }
   }
   cout<<"xmax: "<<Xmax <<endl;

   Double_t SigVal = 10;
   Double_t FitMin = Xmax-2.*SigVal;
   Double_t FitMax = Xmax+2.*SigVal;
   TF1 *gfit0 = new TF1("Gaussian","gaus",FitMin,FitMax);
   gfit0->SetParameters(Ymax,Xmax,SigVal);
   //func->SetParNames("Constant","Mean","Sigma");
   timing0->Fit(gfit0,"R");
   
   Double_t par[3];
   //Double_t errpar[3];
   gfit0->GetParameters(par);
   
   TF1 *gfit1 = new TF1("Gaussian","gaus",par[1]-2*par[2],par[1]+2*par[2]);
   gfit1->SetParameters(par[0],par[1],par[2]);
   timing0->Fit(gfit1,"R");
   
   Double_t TimeMin = par[1]-2.*par[2];
   Double_t TimeMax = par[1]+2.*par[2];

   timing1->Fit(gfit1,"R");
   timing2->Fit(gfit1,"R");
   timing3->Fit(gfit1,"R");

   cout<<"Tmax: "<<TimeMax << " ; Tmin: "<<TimeMin<<endl;
   
   //***********end find timing peak***********//

   //***********************************************
   // Fill charge histos
   //***********************************************
   for (Long64_t jentry=0; jentry<nentries; jentry++) {
     Long64_t ientry = LoadTree(jentry);
     if (ientry < 0) break;
     nb = fChain->GetEntry(jentry);   nbytes += nb;
     //printf(" That's it the average timing for charge %f \n", charge0);
     if(ntrig>1000)Q0->Fill(charge0);
     if(ntrig>1000)Q1->Fill(charge1);
     if(ntrig>1000)Q2->Fill(charge2);
     if(ntrig>1000)Q3->Fill(charge3);
   } // End loop on events

   int binmax[4] = {0};
   binmax[0]     = Q0->GetMaximumBin();
   binmax[1]     = Q1->GetMaximumBin();
   binmax[2]     = Q2->GetMaximumBin();
   binmax[3]     = Q3->GetMaximumBin();

   double x0[4]  = {0.};
   x0[0]         = Q0->GetXaxis()->GetBinCenter(binmax[0]);
   x0[1]         = Q1->GetXaxis()->GetBinCenter(binmax[1]);
   x0[2]         = Q2->GetXaxis()->GetBinCenter(binmax[2]);
   x0[3]         = Q3->GetXaxis()->GetBinCenter(binmax[3]);
  
   Double_t SigVal_q0 = 50;
   Double_t FitMin_q[4];
   Double_t FitMax_q[4];
   double sigma_q[4] = {0.}, mean_q[4] = {0.};

   TF1 **fitQ = new TF1*[4];

   for (int k=0; k<4; k++){
     FitMin_q[k] = x0[k]-1.8*SigVal_q0;
     FitMax_q[k] = x0[k]+1.8*SigVal_q0;
     fitQ[k]     = new TF1("Gaussian","gaus",FitMin_q[k],FitMax_q[k]);
     fitQ[k]->SetParameters(binmax[k],x0[k],SigVal_q0);
     //func->SetParNames("Constant","Mean","Sigma");
   }
   Q0->Fit(fitQ[0],"R");
   sigma_q[0] = fitQ[0]->GetParameter(2);
   mean_q[0]  = fitQ[0]->GetParameter(1); 
   Q1->Fit(fitQ[1],"R");
   sigma_q[1] = fitQ[1]->GetParameter(2);
   mean_q[1]  = fitQ[1]->GetParameter(1); 
   Q2->Fit(fitQ[2],"R");
   sigma_q[2] = fitQ[2]->GetParameter(2);
   mean_q[2]  = fitQ[2]->GetParameter(1); 
   Q3->Fit(fitQ[3],"R");
   sigma_q[3] = fitQ[3]->GetParameter(2);
   mean_q[3]  = fitQ[3]->GetParameter(1); 

   cout << "Writing histos" << endl;
   hfile->Write();
   hfile->Close();

  
}
