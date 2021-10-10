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
#include <TApplication.h>
#include <TH1.h>
#include <stdlib.h>
#include <TFrame.h>
#include <unordered_map>
#include <string>

using namespace std;
const int sideNum = 2;
const int scintNum = 8;

void default_pre_draw(TVirtualPad* pad, TH1* obj, int x, int y){
};

class HistMatrix{

  private:
    int _ndim, _nx, _ny;
    TH1 ***_h_mat;
    TString _xlabel, _ylabel, _name, _main_title;
    TFile *_outfile;
    int _window_close_handle;

    void _draw_single(TH1* obj){
      obj->SetFillStyle(0);
      if (_ndim == 1)
        obj->Draw();
      else
        obj->Draw("zcol");
      _outfile->cd();
      obj->Write();
    };

  public:

    HistMatrix(
      TFile *f,
      int window_close_handle,
      TString name,
      int ndim, 
      int nx, int ny,
      TString main_title,
      TString hist_title_format,
      TString xlabel, TString xunit,
      int xbins, double xmin, double xmax, 
      TString ylabel="", TString yunit="",
      int ybins=0, double ymin=0., double ymax=0.
    ): _name(name), _nx(nx), _ny(ny), _ndim(ndim), _outfile(f), _window_close_handle(window_close_handle), _main_title(main_title)
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
              _h_mat[y][x] = new TH1F(Form("%s_%i_%i", name.Data(), y, x), Form(hist_title_format.Data(), y, x), xbins, xmin, xmax);
            else
              _h_mat[y][x] = new TH1F(Form("%s_%i", name.Data(), x), Form(hist_title_format.Data(), x), xbins, xmin, xmax);
          }
          else if (ndim==2){
            if(ny!=1)
              _h_mat[y][x] = new TH2F(Form("%s_%i_%i", name.Data(), y, x), Form(hist_title_format.Data(), y, x), xbins, xmin, xmax, ybins, ymin, ymax);
            else
              _h_mat[y][x] = new TH2F(Form("%s_%i", name.Data(), x), Form(hist_title_format.Data(), x), xbins, xmin, xmax, ybins, ymin, ymax);
          }
          else {
            cout << "Error: only TH1F or TH2F supported" << endl;
            exit(-1);
          }
        }
      }
    }
  
    TH1*** GetArray(){ return _h_mat;};

    void (*pre_draw)(TVirtualPad* pad, TH1*, int, int) = &default_pre_draw; //by default no fit and no options

    void draw_all(){
      TCanvas *c = new TCanvas(_name, _name);
      if (_window_close_handle == 1){
        TRootCanvas *rc = (TRootCanvas *)c->GetCanvasImp();
        rc->Connect("CloseWindow()", "TApplication", gApplication, "Terminate()");
      }
      c->cd();
      c->SetFillStyle(0);
      TPad *padtitle = new TPad(Form("%s_titlepad", _name.Data()), _name.Data(), 0, 0.95, 1, 1, 21);
      padtitle->Draw();
      padtitle->cd();
      padtitle->SetFillStyle(0);
      auto tex = new TLatex(0.5,0.5, _main_title);
      tex->SetTextAlign(22);
      tex->SetTextSize(0.8);
      tex->Draw();

      c->cd();
      TPad *p = new TPad(Form("%s_graphpad", _name.Data()), _name.Data(), 0, 0, 1, 0.95, 22);
      p->Draw();
      p->SetFillStyle(0);
      p->GetFrame()->SetFillStyle(0);
      p->Divide(_nx, _ny);

      for (int y = 0; y < _ny; y++)
      {
        for(int x=0; x<_nx; x++){
          TVirtualPad* pad = p->cd(_nx*y + x + 1);

          pad->SetFillStyle(0);
          pad->SetFrameFillStyle(0);

          pad->SetLeftMargin(0.27);
          pad->SetRightMargin(0.2);

          TH1 *obj = _h_mat[y][x];
          obj->GetXaxis()->SetTitleSize(0.08);
          obj->GetYaxis()->SetTitleSize(0.08);
          obj->GetXaxis()->SetLabelSize(0.07);
          obj->GetYaxis()->SetLabelSize(0.07);
          obj->GetXaxis()->SetNdivisions(4);
          obj->GetYaxis()->SetNdivisions(8);
          obj->GetXaxis()->SetTitle(_xlabel.Data());
          obj->GetYaxis()->SetTitle(_ylabel.Data());

          obj->GetZaxis()->SetLabelSize(0.06);
          obj->GetZaxis()->SetLabelOffset(0.03);

          pad->SetBottomMargin(0.11);

          if(_ny==1){
            obj->GetXaxis()->SetLabelOffset(-0.04);
            obj->GetXaxis()->SetTitleOffset(0.2);
          }
          else{
            obj->GetXaxis()->SetLabelOffset(-0.02);
            obj->GetXaxis()->SetTitleOffset(0.6);
          }

          pre_draw(pad, obj, x, y);
          _draw_single(obj);
          pad->Update();

          TPaveStats *ps = (TPaveStats*)obj->FindObject("stats");
          if(ps == NULL) cout << "FOBDIJBKJBJHB" << endl;
          ps->SetName(Form("%s_mystats_%i_%i", _name.Data(), x,y ));
          TList *list = ps->GetListOfLines();
          TText *tconst = ps->GetLineWith("Constant");
          list->Remove(tconst);
          obj->SetStats(0);
          c->Modified();
        }
      }

      c->Update();
      c->Print(Form("%s.C", _name.Data()));
    };
};


class CRT_an
{
private:
  TFile *_outfile;
  int _window_close_handle;

public:
  TTree *fChain;  //!pointer to the analyzed TTree or TChain
  Int_t fCurrent; //!current Tree number in a TChain

  // Declaration of leaf types
  Int_t ntrig;
  Int_t evnum;
  Int_t nsample;
  Double_t time[1024]; //[nsample]
  Int_t nCry;
  Int_t iDAQ[16];           //[nCry]
  Int_t iScint[16];         //[nCry]
  Int_t iSide[16];          //[nCry]
  Int_t iMax[16];           //[nCry]
  Double_t Vmax[16];        //[nCry]
  Double_t Qval[16];        //[nCry]
  Double_t Tval[16];        //[nCry]
  Double_t pedL[16];        //[nCry]
  Double_t pedH[16];        //[nCry]
  Double_t wave[16][1024];  //[nCry]
  Double_t bline[16];       //[nCry]
  Double_t lognTime[16];    //[nCry]
  Double_t lognChi2[16];    //[nCry]
  Double_t templTime[16];   //[nCry]
  Double_t templChi2[16];   //[nCry]
  Double_t templFit[16][3]; //[nCry]
  Double_t templErr[16][3]; //[nCry]

  // List of branches
  TBranch *b_ntrig;     //!
  TBranch *b_evnum;     //!
  TBranch *b_nsample;   //!
  TBranch *b_time;      //!
  TBranch *b_nCry;      //!
  TBranch *b_iDAQ;      //!
  TBranch *b_iScint;    //!
  TBranch *b_iSide;     //!
  TBranch *b_iMax;      //!
  TBranch *b_Vmax;      //!
  TBranch *b_Qval;      //!
  TBranch *b_Tval;      //!
  TBranch *b_pedL;      //!
  TBranch *b_pedH;      //!
  TBranch *b_wave;      //!
  TBranch *b_bline;     //!
  TBranch *b_lognTime;  //!
  TBranch *b_lognChi2;  //!
  TBranch *b_templTime; //!
  TBranch *b_templChi2; //!
  TBranch *b_templFit;  //!
  TBranch *b_templErr;  //!

  unordered_map<string, HistMatrix *> hist_dict;

  CRT_an(TString infileName, TFile *f, int, TTree *tree = 0);
  virtual ~CRT_an();
  virtual Int_t Cut(Long64_t entry);
  virtual Int_t GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void Init(TTree *tree);
  virtual void Loop();
  virtual Bool_t Notify();
  virtual void Show(Long64_t entry = -1);
  virtual void CreateHistDict();
  virtual void CloseOutFile();
  virtual TH1F *GetHist(string name, int x, int y);
  pair<string, HistMatrix*> CreatePair(TString, int, int, int, TString, TString, TString, TString, int, double, double, TString, TString, int, double, double);

  void FillHists(unordered_map<string, double**> hist_array_map, int i, int j);
  void FillHists(unordered_map<string, pair<double**, double**>> hist_array_map, int i, int j);
};

#endif

#ifdef CRT_an_cxx
CRT_an::CRT_an(TString _infileName, TFile *f, int window_close_handle, TTree *tree) : 
  fChain(0), _outfile(f), _window_close_handle(window_close_handle)
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
  CreateHistDict();

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

void CRT_an::CloseOutFile(){
  _outfile->Close();
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

pair<string, HistMatrix *> CRT_an::CreatePair(
  TString name,
  int ndim,
  int nx, int ny,
  TString main_title,
  TString hist_title_format, TString xlabel, TString xunit,
  int xbins, double xmin, double xmax,
  TString ylabel = "", TString yunit = "",
  int ybins = 0, double ymin = 0., double ymax = 0.
) {
  if(ndim==1){
    return {
      name.Data(), new HistMatrix(_outfile, _window_close_handle, name.Data(), ndim, nx, ny, main_title, hist_title_format, xlabel, xunit, xbins, xmin,  xmax)
    };
  }
  else{
    return {
      name.Data(), new HistMatrix(_outfile, _window_close_handle, name.Data(), ndim, nx, ny, main_title, hist_title_format, xlabel, xunit, xbins, xmin,  xmax, ylabel, yunit, ybins, ymin, ymax)
    };
  }
};

#endif // #ifdef CRT_an_cxx
