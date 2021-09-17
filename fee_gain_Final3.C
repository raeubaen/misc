#include <iostream>
#include <fstream>
#include <chrono>

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
#include "TROOT.h"
#include "TTimer.h"
#include "TLatex.h"
#include "TGClient.h"
#include "TRootCanvas.h"
#include <TDatime.h>
#include <time.h>

#define Qmax 500
#define Pmax 2000
using namespace std;

void fee_gain_Final3(TString h1,TString h2,TString HolderLabel, TString VoltageStep, int pos, int nchan, int timer, TString Version){

  //----------------------------------------------------------------------------
  //                                 Summary 
  //----------------------------------------------------------------------------

  cout<<"h1: "<<h1<<"\n"
      <<"h2: "<<h2<<"\n"
      <<"Holder Label: "<<HolderLabel<<"\n"
      <<"Wheel positions: "<<pos<<"\n"
      <<"Digitizer channels: "<<nchan<<"\n"
      <<"Display Timer: "<<timer<<endl;
  TDatime *datime = new TDatime();

  
  //----------------------------------------------------------------------------
  //                           Canvas Timer: ON - OFF              
  //----------------------------------------------------------------------------
  
  if(timer!=0 && timer !=1){
    cout<<"ERROR!! Invalid value of Display Timer. Insert 0 for OFF or 1 for ON"<<endl;
    exit(1);
  }

  gStyle->SetOptStat(10);
  gStyle->SetOptFit(111);

  //---------------------------------------------------------------------------
  //                                Definitions
  //---------------------------------------------------------------------------

  const int posmax = 12; // Wheel Positions
  const int nchanmax = 100; // Digitizer channels
  const int cdat = 21; // #Columns in <HolderLabel>.dat
  int ich = 0;
  int ipos = 0;
  
  if(pos>posmax){
    cout<<"ERROR!! Wrong number of wheel filters! Insert a number less than "<<posmax<<endl;
    exit(1);
  } // end if
  
  double mean[nchanmax][posmax]={0.,0.}, errm[nchanmax][posmax]={0.,0.},sig[nchanmax][posmax]={0.,0.},errs[nchanmax][posmax]={0.,0.},Qchi2[nchanmax][posmax]={0.,0.};
  double res[nchanmax][posmax]={0.,0.}, erres[nchanmax][posmax]={0.,0.}, Npe[nchanmax][posmax]={0.,0.}, errNpe[nchanmax][posmax]={0.,0.};
  double Ga[nchanmax]={0.}, Gaerr[nchanmax]={0.}, Chi2[nchanmax]= {0.}, ndof[nchanmax]={0.};
  TCanvas *gainAll= new TCanvas("gainAll","gainAll");
  //TCanvas *C[nchanmax];
  TGraphErrors *g[nchanmax];
  TGraphErrors *n[nchanmax];
  TPaveText *t[nchanmax];
  TPaveText *t2[nchanmax];
  TPad *pad1[nchanmax];
  TPaveText *pt[nchanmax];
  TString F_in = Form("/home/user/qa_wheel/specs/%s_%s.dat", HolderLabel.Data(), VoltageStep.Data());
  cout<<F_in.Data()<<endl;
  TFile *fileGain = new TFile(Form("/home/user/qa_wheel/specs/gainfile/Gain_%s_%s.root",HolderLabel.Data(), VoltageStep.Data()),"RECREATE");
  
  //---------------------------------------------------------------------------
  //                        Reading <HolderLabel>.dat
  //---------------------------------------------------------------------------
  
  double A[posmax][nchanmax]={0};
  ifstream dat;
  dat.open(F_in.Data(), ifstream::in);
  if(dat.is_open()){ 
    cout<<"File :"<< F_in.Data()<<"  open"<< endl;
    for(ipos=0; ipos<pos; ipos++){  // 0 - 8
      // for(ich=0; ich<(nchan*nchan)+1;ich++){ // 0 - 3
      for(ich=0; ich<cdat;ich++){ // 0 - 3
     	dat>>A[ipos][ich];
	
      } //end loop over wheel positions
    } // end loop over digitizer channels
  } // end if

  dat.close();
  
  for(int ipos=0;ipos<pos;ipos++){
    for(int ich=0;ich<nchan;ich++){
      int Bin = (ich*5);
      mean[ich][ipos] = A[ipos][Bin+1];
      errm[ich][ipos] = A[ipos][Bin+2];
      sig[ich][ipos]  = A[ipos][Bin+3];
      errs[ich][ipos] = A[ipos][Bin+4];
      Qchi2[ich][ipos]= A[ipos][Bin+5];
      
    } // end loop over digitizer channels
  } // end loop over wheel positions 

  //---------------------------------------------------------------------------
  //                       Getting Charge and Wave Profile
  //---------------------------------------------------------------------------

  TString F_root = Form("/home/user/qa_wheel/specs/%s_%s.root", HolderLabel.Data(), VoltageStep.Data());
  cout<<F_root.Data()<<endl;
  TFile* f = new TFile(F_root);
  TH1D *f_Q[nchanmax][posmax];
  TProfile *waveprof[nchanmax][posmax];
  
   
  for(ich=0; ich<nchan; ich++){
    for(ipos=pos-1; ipos>-1; ipos--){
      f_Q[ich][ipos]=(TH1D*)f->Get(Form("Q%i_%i",ich, ipos));
      f_Q[ich][ipos]->SetTitle(Form("Charge Q_{%i}: Filter %i",ich, ipos));
      f_Q[ich][ipos]->GetXaxis()->SetTitle(Form("Q_{%i} (pC)", ich));
      f_Q[ich][ipos]->GetYaxis()->SetTitle("Entries");
      f_Q[ich][ipos]->GetXaxis()->SetRangeUser(mean[ich][ipos]-5*sig[ich][ipos],mean[ich][ipos]+5*sig[ich][ipos]);
      f_Q[ich][ipos]->GetYaxis()->SetRangeUser(0,1.5* f_Q[ich][ipos]->GetMaximum());
      TString Fpos_root = Form("/home/user/qa_wheel/hist/%s/%s/%s_pos%ifull_hist.root", HolderLabel.Data(), VoltageStep.Data(), HolderLabel.Data(),ipos);
      TFile* fpos = new TFile(Fpos_root);
      waveprof[ich][ipos]=(TProfile*)fpos->Get(Form("wave_prof%i", ich));
      waveprof[ich][ipos]->GetXaxis()->SetRangeUser(0,600);

      //----------------------------------------------------------------------------
      //      Computing sigma/Q ± error: ATTENTION: scale factor of 100 to show y axis in %
      //----------------------------------------------------------------------------
      
      res[ich][ipos] = (sig[ich][ipos]/mean[ich][ipos])*100; 
      erres[ich][ipos] = (errs[ich][ipos]/mean[ich][ipos])*100;
    }// end loop over ipos


    
    ofstream meas;
    meas.open("/home/user/qa_wheel/database/measurement.dat", ofstream::out | ofstream::app);
      meas.precision(4);
      meas<<datime->GetDate()<<"  "<<Version.Data()<<"  " << VoltageStep.Data() << "  " <<h1<<"  0  ";
      for(ipos=0; ipos<pos; ipos++)meas<<mean[0][ipos]<<"  "<<errm[0][ipos]<<"  "<<sig[0][ipos]<<"  "<<errs[0][ipos]<<"  "<<Qchi2[0][ipos] << "  ";
      meas<<endl;
      meas<<datime->GetDate()<<"  "<<Version.Data()<<"  "<< VoltageStep.Data() << "  " <<h1<<"  1  ";
      for(ipos=0; ipos<pos; ipos++)meas<<mean[1][ipos]<<"  "<<errm[1][ipos]<<"  "<<sig[1][ipos]<<"  "<<errs[1][ipos]<<"  "<<Qchi2[1][ipos] << "  ";
      meas<<endl;
      meas<<datime->GetDate()<<"  "<<Version.Data()<<"  "<< VoltageStep.Data() << "  " <<h2<<"  0  ";
      for(ipos=0; ipos<pos; ipos++)meas<<mean[2][ipos]<<"  "<<errm[2][ipos]<<"  "<<sig[2][ipos]<<"  "<<errs[2][ipos]<<"  "<<Qchi2[2][ipos] << "  ";
      meas<<endl;
      meas<<datime->GetDate()<<"  "<<Version.Data()<<"  "<< VoltageStep.Data() << "  " <<h2<<"  1  ";
      for(ipos=0; ipos<pos; ipos++)meas<<mean[3][ipos]<<"  "<<errm[3][ipos]<<"  "<<sig[3][ipos]<<"  "<<errs[3][ipos]<<"  "<<Qchi2[3][ipos] << "  ";
      meas<<endl;
      
    //---------------------------------------------------------------------------
    //                            Creating Canvas
    //---------------------------------------------------------------------------
  
    //((TRootCanvas*)C[ich]->GetCanvasImp())->Resize(gClient->GetDisplayWidth()-20, gClient->GetDisplayHeight()-50);
  
    g[ich] = new TGraphErrors(pos,mean[ich],res[ich],errs[ich],erres[ich]);


    //g[ich]->Draw("ap");
    g[ich]->SetMarkerStyle(20);
    g[ich]->SetMarkerStyle(1);
    g[ich]->SetTitle(Form("Channel %i: #sigma/Q_{%i} vs Q_{%i}", ich,ich,ich));
    g[ich]->SetName(Form("Channel_%i",ich));
    g[ich]->GetXaxis()->SetTitle(Form("Q_{%i} [pC]", ich));
    g[ich]->GetYaxis()->SetTitle(Form("#sigma/Q_{%i} [%%]", ich));


    TF1 *fit = new TF1("fit", "sqrt([0]/x + [1]*[1]/(x*x) + [2]*[2])",0,1600);
    fit->SetParameter(0, 0.20);
    fit->SetParameter(1, 5.);
    fit->SetParameter(2, 0.01);
    g[ich]->Fit("fit","R");
    //gPad->Update();
    Ga[ich]=fit->GetParameter(0)/10000.; //ATTENTION: scale factor of 100
    Gaerr[ich]=fit->GetParError(0)/10000.;
    Chi2[ich]= fit->GetChisquare();
    ndof[ich] = fit->GetNDF();

/*
    t[ich] = new TPaveText(0.35,0.37,0.65,0.52,"NDC");
    t[ich]->SetTextSize(0.045);
    t[ich]->SetFillColor(0);
    t[ich]->SetBorderSize(0);
    t[ich]->SetTextAlign(10);
    t[ich]->AddText("\n");
    t[ich]->AddText("\n");
    t[ich]->AddText("\n");
    t[ich]->AddText("\n");
    t[ich]->AddText("\n");
    t[ich]->AddText("\n");
    t[ich]->AddText("\n");
    t[ich]->AddText("\n");
    t[ich]->AddText("Fit: #sqrt{#frac{p_{0}}{x} + #frac{p_{1}^{2}}{x^{2}} + p_{2}^{2}}");
    if(ich==0)t[ich]->SetTitle("%s_l",h1);
      if(ich==1)t[ich]->SetTitle("%s_r",h1);
      if(ich==2)t[ich]->SetTitle("%s_l",h2);
      if(ich==3)t[ich]->SetTitle("%s_r",h2);
    //t[ich]->Draw();*/
    
    fileGain->cd();
    g[ich]->Write();
    
    //---------------------------------------------------------------------------
    //                         Computing Npe ± error
    //---------------------------------------------------------------------------

    for (ipos=0; ipos<pos; ipos++){
      Npe[ich][ipos] = (mean[ich][ipos]/Ga[ich]);    
      errNpe[ich][ipos] = (Gaerr[ich]/Ga[ich])*Npe[ich][ipos];
    }
    /*
    for(int vp=0; vp<pos; vp++){
      C[ich]->cd(vpos[vp]);
      f_Q[ich][vp]->Draw("E");
    }
  
    C[ich]->cd(5);
    pad1[ich] = new TPad(Form("pad%i",ich),Form("pad%i",ich),0.05,0.50,0.95,0.95,21);
    TLatex latex;
    latex.SetTextSize(0.08);
     
     
    latex.DrawLatex(.1,.8,Form("Gain: %.2e #pm %.2e\n", Ga[ich]/(1*1.6e-7), Gaerr[ich]/(1*1.6e-7)));
     

     
    C[ich]->cd(9);
    for(ipos=0; ipos<pos; ipos++){
      waveprof[ich][ipos]->SetMarkerColor(ipos+1);
      if(ipos==0){waveprof[ich][ipos]->Draw();
      }else{waveprof[ich][ipos]->Draw("sames");
      }
    } // end loop over wave profile
*/
    //---------------------------------------------------------------------------
    //                               Print Gain
    //---------------------------------------------------------------------------

    cout.precision(4);
    cout<< "Channel : "<<ich<<"\t"<<"Gain: "<<Ga[ich]/(1*1.6e-7)<<" \u00b1 "<<Gaerr[ich]/(1*1.6e-7)<<endl;

    //if(ich==1) C[ich]->SaveAs(Form("/home/user/qa_wheel/specs/plot/Display_%s.eps",HolderLabel.Data()));
    
  } //end loop over Canvas

  fileGain->Close();

  //---------------------------------------------------------------------------
  //                             Filling Gain.dat
  //---------------------------------------------------------------------------
   
  ofstream gain;
  gain.open("/home/user/qa_wheel/database/Gain.dat", ofstream::out | ofstream::app);
  gain.precision(4);
  /*
    gain<<datime->GetDate()<<"  "<<h1<<"  0"<<"  "<<Chi2[0]/ndof[0]<<" "<<Ga[0]<<" "<<Gaerr[0]<<" "<<Ga[0]/(1*1.6e-7)<<" "<<Gaerr[0]/(1*1.6e-7)<<"  "<<"1"<<"  "<<Chi2[1]/ndof[1]<<" "<<Ga[1]<<" "<<Gaerr[1]<<" "<<Ga[1]/(1*1.6e-7)<<" "<<Gaerr[1]/(1*1.6e-7)<<endl;
    gain<<datime->GetDate()<<"  "<<h2<<"  0"<<"  "<<Chi2[2]/ndof[2]<<" "<<Ga[2]<<" "<<Gaerr[2]<<" "<<Ga[2]/(1*1.6e-7)<<" "<<Gaerr[2]/(1*1.6e-7)<<"  "<<"1"<<"  "<<Chi2[3]/ndof[3]<<" "<<Ga[3]<<" "<<Gaerr[3]<<" "<<Ga[3]/(1*1.6e-7)<<" "<<Gaerr[3]/(1*1.6e-7)<<endl;
  */
  gain << VoltageStep.Data() << "  " << datime->GetDate()<<"  " << h1 << "  " << Version.Data() << 
    "  0"<<"  " << mean[0][0]<<"  "<<sig[0][0]<< "  "  <<Ga[0]/(1*1.6e-7)<<"  "<<Gaerr[0]/(1*1.6e-7)<<
    "  1"<<"  " << mean[1][0]<<"  "<<sig[1][0] << "  " <<Ga[1]/(1*1.6e-7)<<"  "<<Gaerr[1]/(1*1.6e-7)<<endl;
  gain << VoltageStep.Data() << "  " << datime->GetDate()<<"  "<<h2<<"  "<<Version.Data()<<
    "  0"<<"  " << mean[2][0]<<"  "<<sig[2][0]<< "  " <<Ga[2]/(1*1.6e-7)<<"  "<<Gaerr[2]/(1*1.6e-7)<<
    "  1"<<"  " << mean[3][0]<<"  "<<sig[3][0]<< "  " <<Ga[3]/(1*1.6e-7)<<"  "<<Gaerr[3]/(1*1.6e-7)<<endl;
     
     
  gain.close();
     

     
  
  //---------------------------------------------------------------------------
  //                               Canvas Timer 
  //---------------------------------------------------------------------------

  
  /*
  cout<<h1<<" l "<<" "<<Chi2[0]/ndof[0]<<" "<<Ga[0]<<" "<<Gaerr[0]<<" "<<Ga[0]/(1*1.6e-7)<<" "<<Gaerr[0]/(1*1.6e-7)<<endl;
  cout<<h1<<" r "<<" "<<Chi2[1]/ndof[1]<<" "<<Ga[1]<<" "<<Gaerr[1]<<" "<<Ga[1]/(1*1.6e-7)<<" "<<Gaerr[1]/(1*1.6e-7)<<endl;
  cout<<h2<<" l "<<" "<<Chi2[2]/ndof[2]<<" "<<Ga[2]<<" "<<Gaerr[2]<<" "<<Ga[2]/(1*1.6e-7)<<" "<<Gaerr[2]/(1*1.6e-7)<<endl;
  cout<<h2<<" r "<<" "<<Chi2[3]/ndof[3]<<" "<<Ga[3]<<" "<<Gaerr[3]<<" "<<Ga[3]/(1*1.6e-7)<<" "<<Gaerr[3]/(1*1.6e-7)<<endl;
  */

  ((TRootCanvas*)gainAll->GetCanvasImp())->Resize(gClient->GetDisplayWidth()-20, gClient->GetDisplayHeight()-50);
  gainAll->cd();
  gainAll->Divide(4,3);
  TLatex latex1;
  for(int ic=1;ic<13;ic++){
    gainAll->cd(ic);
    if(ic<5){
      /*
      g[ic-1]->SetMarkerStyle(20);
      g[ic-1]->SetMarkerSize(0.7);*/
      g[ic-1]->Draw("AP");/*
      g[ic-1]->SetTitle(Form("Channel %i: #sigma/Q_{%i} vs Q_{%i}", ic-1,ic-1,ic-1));
      g[ic-1]->SetName(Form("Channel_%i",ic));
      g[ic-1]->GetXaxis()->SetTitle(Form("Q_{%i} [pC]", ic-1));
      g[ic-1]->GetYaxis()->SetTitle(Form("#sigma/Q_{%i} [%%]", ic-1));
      */
      latex1.SetTextSize(0.05);


      latex1.SetTextSize(0.05);
      // double stab =100.* TMath::Abs(mean[ich][0]-mean[ich][9])/mean[ich][0];
      //cout<<"AAAAAAA stability is "<<stab<<"% "<<mean[ich][0]<<"  "<<mean[ich][9]<<endl;
      
      // latex.DrawLatex(.2,.6,Form("  Stability is %5.2f %% \n",stab));
      if(ic-1==0) latex1.DrawLatex(157,10,Form(" %s_{0}: %.2e #pm %.2e\n",h1.Data(), Ga[ic-1]/(1*1.6e-7), Gaerr[ic-1]/(1*1.6e-7)));
      if(ic-1==1) latex1.DrawLatex(157,10,Form(" %s_{1}: %.2e #pm %.2e\n",h1.Data(), Ga[ic-1]/(1*1.6e-7), Gaerr[ic-1]/(1*1.6e-7)));
      if(ic-1==2) latex1.DrawLatex(157,10,Form(" %s_{0}: %.2e #pm %.2e\n",h2.Data(), Ga[ic-1]/(1*1.6e-7), Gaerr[ic-1]/(1*1.6e-7)));
      if(ic-1==3) latex1.DrawLatex(157,10,Form(" %s_{1}: %.2e #pm %.2e\n",h2.Data(), Ga[ic-1]/(1*1.6e-7), Gaerr[ic-1]/(1*1.6e-7)));

    }else if(ic>4 && ic<9){
      for(ipos=0; ipos<pos; ipos++){
        waveprof[ic-5][ipos]->SetMarkerColor(ipos+1);
        if(ipos==0){
          waveprof[ic-5][ipos]->Draw();
        } else{
          waveprof[ic-5][ipos]->Draw("sames");
        }
      }
    }
    else{
      for(ipos=pos-1; ipos>-1; ipos--){
        f_Q[ic-9][ipos]->SetMarkerColor(ipos+1);
        f_Q[ic-9][ipos]->GetXaxis()->SetRangeUser(0,Qmax);
        f_Q[ic-9][ipos]->GetYaxis()->SetRangeUser(0,Pmax);

        if(ipos==pos-1){
          f_Q[ic-9][ipos]->Draw();
        } else{
          f_Q[ic-9][ipos]->Draw("sames");
        }
      }
    }
  }

  chrono::time_point<chrono::system_clock> end;

  std::chrono::milliseconds ms(30000);
  end = chrono::system_clock::now() + ms; // this is the end point

  while (chrono::system_clock::now() < end) { 
    gSystem->ProcessEvents(); 
    gSystem->Sleep(10);
  }
} //end void






