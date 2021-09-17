#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <TH2.h>
//#include <TStyle.h>
//#include <TCanvas.h>
//#include <TProfile.h>
#include <TF1.h>
#include <TFile.h>
#include <string>

using namespace std;

void fit_fee_final(){
  char  filein[150];
  char  fileq0[150];
  char  fileft[150];
  char  fileout[150];
  char  fileoutnew[150];
  char  fileroot[150];
  int   pos=0;
  float MaxVal=0.;
  float MyMean=0.;
  float Den   =0.;
  Double_t Xval = 0.;
  Double_t Yval = 0.;
  //Double_t par[3];
  Double_t errpar[3];
  char  HisName[10];
  char  HisTitle[20];

  cout << "Enter root input file:" << endl;
  cin >> filein;
  cout<<filein<<endl;
  cout << "Enter position:" << endl;
  cin >> pos;
  cout<<pos<<endl;
  cout << "Enter output file:" << endl;
  cin >> fileout;
  cout<<fileout<<endl;
  cout << "Enter root output file:" << endl;
  cin >> fileroot;
  cout<<fileroot<<endl;
  //cout << "Selected values: " << filein << " " << pos << " " << fileout << endl;
  TFile* f = new TFile(filein);

  TH1D *Q[4]; 
  Q[0] = (TH1D*)f->Get("Q0");
  Q[1] = (TH1D*)f->Get("Q1");
  Q[2] = (TH1D*)f->Get("Q2");
  Q[3] = (TH1D*)f->Get("Q3");

  TH1D *T[4];
  T[0] = (TH1D*)f->Get("timing0");
  T[1] = (TH1D*)f->Get("timing1");
  T[2] = (TH1D*)f->Get("timing2");
  T[3] = (TH1D*)f->Get("timing3");

  //fit****************************
  int binmax[4] = {0};
  double x0[4]  = {0.};

  for(int k=0; k<4; k++){
    binmax[k]     = Q[k]->GetMaximumBin();
    x0[k]         = Q[k]->GetXaxis()->GetBinCenter(binmax[k]);
  }
  

  Double_t SigVal_q0 = 50;
  Double_t FitMin_q[4];
  Double_t FitMax_q[4];
  double sigma_q[4] = {0.}, mean_q[4] = {0.}, Esigma[4]={0.}, Emean[4]={0.},chi2fin[4];
     
  TF1 **fitQ = new TF1*[4];

  for (int k=0; k<4; k++){
    FitMin_q[k] = x0[k]-1.8*SigVal_q0;
    FitMax_q[k] = x0[k]+1.8*SigVal_q0;
    fitQ[k]     = new TF1("Gaussian","gaus",FitMin_q[k],FitMax_q[k]);
    fitQ[k]->SetParameters(binmax[k],x0[k],SigVal_q0);

    
    Q[k]->Fit(fitQ[k],"R");
    sigma_q[k] = fitQ[k]->GetParameter(2);
    Esigma[k]  = fitQ[k]->GetParError(2); 
    mean_q[k]  = fitQ[k]->GetParameter(1); 
    Emean[k]   = fitQ[k]->GetParError(1);
    chi2fin[k] = fitQ[k]->GetChisquare()/fitQ[k]->GetNDF();
        
  }
 
  ofstream file;
  sprintf(fileoutnew,"%s",fileout);
     
  file.open (fileoutnew, ios::out | ios::app );
  cout.rdbuf(file.rdbuf());
  
  cout << pos ;
  for(int z=0; z<4 ; z++){
    cout<<" "<<mean_q[z] <<"  "<< Emean[z] << "  " << sigma_q[z] << "  "<< Esigma[z]<<"  "<<chi2fin[z];
  }
  cout<<" "<<endl;
     
  //
  // Save histos
  //

  file.close();

  //printf("file pointer: %p\n", &file);
  //printf("f pointer: %p\n", f);
  cout<<"inizio a salvare";
  TFile *OutputFile = new TFile(fileroot,"UPDATE");
  OutputFile->cd();

  for(int iloop=0 ; iloop<4; iloop++){
    sprintf(HisTitle,"Q%i pos %i",iloop,pos);
    sprintf(HisName,"Q%i_%i",iloop,pos);
    TH1D *Qnew=(TH1D*)Q[iloop]->Clone();
    Qnew->SetName(HisName);
    Qnew->SetTitle(HisTitle);
    if(Qnew->GetEntries()>0) Qnew->Write();

  }

  OutputFile->Close();
  f->Close();

  //printf("%p\n", OutputFile);
}

int main(){
  fit_fee_final();
}
