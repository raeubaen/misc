//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Sep 22 11:42:27 2021 by ROOT version 5.34/14
// from TTree CRT/CRT
// found on file: run156Ana.root
//////////////////////////////////////////////////////////

#ifndef CRT_an_h
#define CRT_an_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <iostream>
#include <TH1.h>
#include <stdlib.h>
#include <unordered_map>
#include <string>

using namespace std;
const int sideNum = 2;
const int scintNum = 8;

void default_pre_draw(TH1* obj, int x, int y){};

class HistMatrix{

  private:
    int _ndim, _nx, _ny;
    TString _name;
    TH1 ***_h_mat;
    TString _xlabel, _ylabel;
    TFile *outfile;

    void _draw_single(TH1* obj){
      obj->GetXaxis()->SetTitle(_xlabel.Data());
      obj->GetYaxis()->SetTitle(_ylabel.Data());
      if (_ndim == 1) obj->Draw(); 
      else obj->Draw("zcol");
      outfile->cd();
      obj->Write();
    };

  public:

    HistMatrix(
      TFile *f,
      TString name,
      int ndim, 
      int nx, int ny, 
      TString title_format,
      TString xlabel, TString xunit,
      int xbins, int xmin, int xmax, 
      TString ylabel="", TString yunit="",
      int ybins=0, int ymin=0, int ymax=0
    ): _name(name), _nx(nx), _ny(ny), _ndim(ndim), outfile(f)
    {

      if (xunit.CompareTo("")){
        _xlabel = Form("%s (%s)", xlabel.Data(), xunit.Data());
      }
      else _xlabel = xlabel;

      if (ndim==1){
        _ylabel = Form("Events per %d %s", (int)((xmax-xmin)/(double)xbins), xunit.Data());
      }

      else{
        if (yunit.CompareTo("")){
        _ylabel = Form("%s (%s)", ylabel.Data(), yunit.Data());
      }
      else _ylabel = ylabel;
      }


      _h_mat = new TH1**[ny];
      for (int y = 0; y < ny; y++)
        _h_mat[y] = new TH1*[nx];

      for(int y=0; y<ny; y++){
        for(int x=0; x<nx; x++){
          if (ndim==1){
            if(ny!=1)
              _h_mat[y][x] = new TH1F(Form("%s_%i_%i", name.Data(), y, x), Form(title_format.Data(), y, x), xbins, xmin, xmax);
            else
              _h_mat[y][x] = new TH1F(Form("%s_%i", name.Data(), x), Form(title_format.Data(), x), xbins, xmin, xmax);

          }
          else if (ndim==2)
            if(ny!=1)
              _h_mat[y][x] = new TH2F(Form("%s_%i_%i", name.Data(), y, x), Form(title_format.Data(), y, x), xbins, xmin, xmax, ybins, ymin, ymax);
            else
              _h_mat[y][x] = new TH2F(Form("%s_%i", name.Data(), x), Form(title_format.Data(), x), xbins, xmin, xmax, ybins, ymin, ymax);
          else {
            cout << "Error: only TH1F or TH2F supported" << endl;
            exit(-1);
          }
        }
      }
    }
  
    TH1*** GetArray(){ return _h_mat;};

    void (*pre_draw)(TH1*, int, int) = &default_pre_draw; //by default no fit and no options

    void draw_all(){
      TCanvas *c = new TCanvas(_name, _name);
      c->Divide(_nx, _ny);
      for(int y=0; y<_ny; y++){
        for(int x=0; x<_nx; x++){
          c->cd(_nx*y + x + 1);
          pre_draw(_h_mat[y][x], x, y);
          _draw_single(_h_mat[y][x]);
        }
      }
    };

};


class CRT_an {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           ntrig;
   Int_t           evnum;
   Int_t           nsample;
   Double_t        time[1024];   //[nsample]
   Int_t           nCry;
   Int_t           iDAQ[16];   //[nCry]
   Int_t           iScint[16];   //[nCry]
   Int_t           iSide[16];   //[nCry]
   Int_t           iMax[16];   //[nCry]
   Double_t        Vmax[16];   //[nCry]
   Double_t        Qval[16];   //[nCry]
   Double_t        Tval[16];   //[nCry]
   Double_t        pedL[16];   //[nCry]
   Double_t        pedH[16];   //[nCry]
   Double_t        wave[16][1024];   //[nCry]
   Double_t        bline[16];   //[nCry]
   Double_t        lognTime[16];   //[nCry]
   Double_t        lognChi2[16];   //[nCry]
   Double_t        templTime[16];   //[nCry]
   Double_t        templChi2[16];   //[nCry]
   Double_t        templFit[16][3];   //[nCry]
   Double_t        templErr[16][3];   //[nCry]

   // List of branches
   TBranch        *b_ntrig;   //!
   TBranch        *b_evnum;   //!
   TBranch        *b_nsample;   //!
   TBranch        *b_time;   //!
   TBranch        *b_nCry;   //!
   TBranch        *b_iDAQ;   //!
   TBranch        *b_iScint;   //!
   TBranch        *b_iSide;   //!
   TBranch        *b_iMax;   //!
   TBranch        *b_Vmax;   //!
   TBranch        *b_Qval;   //!
   TBranch        *b_Tval;   //!
   TBranch        *b_pedL;   //!
   TBranch        *b_pedH;   //!
   TBranch        *b_wave;   //!
   TBranch        *b_bline;   //!
   TBranch        *b_lognTime;   //!
   TBranch        *b_lognChi2;   //!
   TBranch        *b_templTime;   //!
   TBranch        *b_templChi2;   //!
   TBranch        *b_templFit;   //!
   TBranch        *b_templErr;   //!

   unordered_map<string, HistMatrix* > hist_dict;

   CRT_an(TString infileName=TString(""), TTree *tree=0);
   virtual ~CRT_an();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(TFile *f);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   virtual void     CreateHistDict(TFile *f);
   virtual TH1F*    GetHist(string name, int x, int y);
};

#endif

#ifdef CRT_an_cxx
CRT_an::CRT_an(TString _infileName, TTree *tree) : fChain(0)
{
  // if parameter tree is not specified (or zero), connect the file
  // used to generate this class and read the Tree.

  if (tree == 0) {
    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(_infileName);
    if (!f || !f->IsOpen()) {
      f = new TFile(_infileName);
    }
    f->GetObject("CRT",tree);

  }
  Init(tree);
};

CRT_an::~CRT_an()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t CRT_an::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t CRT_an::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

Bool_t CRT_an::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void CRT_an::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}

Int_t CRT_an::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}


void CRT_an::Init(TTree *tree)
{
 
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("ntrig", &ntrig, &b_ntrig);
   fChain->SetBranchAddress("evnum", &evnum, &b_evnum);
   fChain->SetBranchAddress("nsample", &nsample, &b_nsample);
   fChain->SetBranchAddress("time", time, &b_time);
   fChain->SetBranchAddress("nCry", &nCry, &b_nCry);
   fChain->SetBranchAddress("iDAQ", iDAQ, &b_iDAQ);
   fChain->SetBranchAddress("iScint", iScint, &b_iScint);
   fChain->SetBranchAddress("iSide", iSide, &b_iSide);
   fChain->SetBranchAddress("iMax", iMax, &b_iMax);
   fChain->SetBranchAddress("Vmax", Vmax, &b_Vmax);
   fChain->SetBranchAddress("Qval", Qval, &b_Qval);
   fChain->SetBranchAddress("Tval", Tval, &b_Tval);
   fChain->SetBranchAddress("pedL", pedL, &b_pedL);
   fChain->SetBranchAddress("pedH", pedH, &b_pedH);
   fChain->SetBranchAddress("wave", wave, &b_wave);
   fChain->SetBranchAddress("bline", bline, &b_bline);
   fChain->SetBranchAddress("lognTime", lognTime, &b_lognTime);
   fChain->SetBranchAddress("lognChi2", lognChi2, &b_lognChi2);
   fChain->SetBranchAddress("templTime", templTime, &b_templTime);
   fChain->SetBranchAddress("templChi2", templChi2, &b_templChi2);
   fChain->SetBranchAddress("templFit", templFit, &b_templFit);
   fChain->SetBranchAddress("templErr", templErr, &b_templErr);
   Notify();
}


TH1F* CRT_an::GetHist(string name, int x, int y){
  return (TH1F*)hist_dict[name]->GetArray()[y][x];
}

#endif // #ifdef CRT_an_cxx
