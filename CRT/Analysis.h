#ifndef Analysis_h
#define Analysis_h

// ----------------------------- modify when using a new root class (from tree->MakeClass)
#define ROOT_CLASS ana
#include "ana.h"
#include "ana.C"

#define TREE_NAME "CRT" // in ROOT_CLASS
// ------------------------------

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <vector>
#include <iostream>
#include <TApplication.h>
#include <TH1.h>
#include <stdlib.h>
#include <TFrame.h>
#include <unordered_map>
#include <string>

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
#include <TLegend.h>
#include <TGraphErrors.h>
#include <vector>
#include <TString.h>
#include <TTimer.h>
#include <TLatex.h>
#include <TGClient.h>
#include <TRootCanvas.h>
#include <TDatime.h>
#include <time.h>

using namespace std;

const int sideNum = 2;
const int scintNum = 8;

void default_pre_draw(TVirtualPad* pad, TH1* obj, int x, int y){};

class HistMatrix{

  private:
    int _ndim, _nx, _ny;
    TH1 ***_h_mat;
    TString _xlabel, _ylabel, _name, _main_title;
    TFile *_outfile;
    int _window_close_handle;

    void _draw_single(TH1* obj){
      obj->SetFillStyle(0);
      if (_ndim == 1){
        obj->Draw("same");
        obj->Draw("esame");
      }
      else
        obj->Draw("zcolsame");
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
              _h_mat[y][x] = new TH1F(Form("%s_%i",    name.Data(), x),    Form(hist_title_format.Data(), x),    xbins, xmin, xmax);
          }
          else if (ndim==2){
            if(ny!=1)
              _h_mat[y][x] = new TH2F(Form("%s_%i_%i", name.Data(), y, x), Form(hist_title_format.Data(), y, x), xbins, xmin, xmax, ybins, ymin, ymax);
            else
              _h_mat[y][x] = new TH2F(Form("%s_%i",    name.Data(), x),    Form(hist_title_format.Data(), x),    xbins, xmin, xmax, ybins, ymin, ymax);
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
          if(ps != nullptr){
            ps->SetName(Form("%s_mystats_%i_%i", _name.Data(), x,y ));
            TList *list = ps->GetListOfLines();
            TText *tconst = ps->GetLineWith("Constant");
            list->Remove(tconst);
            obj->SetStats(0);
          }
          c->Modified();
        }
      }

      c->Update();
      c->SaveAs(Form("plots/%s.root", _name.Data()));
      c->SaveAs(Form("plots/%s.C", _name.Data()));
    };
};


class Analysis: public ROOT_CLASS
{
private:
  TFile *_outfile;
  int _window_close_handle;

public:
  Analysis(TString infileName, TFile *f, int, TTree *tree = 0);
  virtual void Loop() override;
  virtual void CreateHistDict(vector <Double_t> v = {0.});
  virtual void CloseOutFile();
  virtual TH1F *GetHist(string name, int x, int y);
  pair<string, HistMatrix*> CreatePair(
    TString, int, int, int, TString, TString, 
    TString, TString, int, double, double, 
    TString, TString, int, double, double,
    void (*)(TVirtualPad* pad, TH1*, int, int)
  );
  void FillHists(unordered_map<string, double**> hist_array_map, int i, int j);
  void FillHists(unordered_map<string, pair<double**, double**>> hist_array_map, int i, int j);
  void FillArrays(unordered_map<double**, double> arr_value_map, int i, int j);
  static void Run(int argc, char *argv[], int window_close_handle);

  unordered_map<string, HistMatrix *> hist_dict;
};

#endif

TTree * GetTree(TString infileName){
  TTree *tree;
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(infileName.Data());
      if (!f || !f->IsOpen()) {
         f = new TFile(infileName.Data());
      }
      f->GetObject(TREE_NAME, tree);
  return tree;
}

Analysis::Analysis(TString infileName, TFile *f, int window_close_handle, TTree *tree) : 
  _outfile(f), _window_close_handle(window_close_handle), 
  ROOT_CLASS(GetTree(infileName))
{
  ;
};

void Analysis::CloseOutFile(){
  _outfile->Close();
}

TH1F* Analysis::GetHist(string name, int x, int y){
  return (TH1F*)hist_dict[name]->GetArray()[y][x];
}

pair<string, HistMatrix *> Analysis::CreatePair(
  TString name,
  int ndim,
  int nx, int ny,
  TString main_title,
  TString hist_title_format, TString xlabel, TString xunit,
  int xbins, double xmin, double xmax,
  TString ylabel = "", TString yunit = "",
  int ybins = 0, double ymin = 0., double ymax = 0.,
  void (*pre_draw)(TVirtualPad* pad, TH1*, int, int) = &default_pre_draw
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

void Analysis::FillHists(unordered_map<string, double**> hist_array_map, int i, int j){
  for(auto &pair: hist_array_map){
    TH1* hist = GetHist(pair.first, j, i);
    double value = pair.second[i][j];
    hist->Fill(value);
  }
}

void Analysis::FillHists(unordered_map<string, pair<double**, double**>> hist_array_map, int i, int j){
  for(auto &pair: hist_array_map){
    TH2* hist = (TH2*)GetHist(pair.first, j, i);
    double xvalue = pair.second.first[i][j];
    double yvalue = pair.second.second[i][j];
    hist->Fill(xvalue, yvalue);
  }
}

void Analysis::FillArrays(unordered_map<double**, double> arr_value_map, int i, int j){
  for(auto &pair: arr_value_map) pair.first[i][j] = pair.second;
}

void set_style(){
  gStyle->SetOptStat("emr"); //entries, mean and rms
  gStyle->SetTitleFontSize(0.12);
  gStyle->SetStatFont(63);
  gStyle->SetStatFontSize(10);
  gStyle->SetStatW(0.35);
  gStyle->SetStatX(0.95);
  gStyle->SetStatY(0.9);
  gStyle->SetTitleFontSize(0.1); 
  gROOT->ForceStyle();
  gStyle->SetOptFit(1);
}


#define QcutMin 80 //era 50
#define Qcutmax 100000 // praticamente inf
#define Vpeakmax 1800
#define Chi2max 10 //togliere selezione Chi2 dalle MIP per gli istogrammi di carica


int cut(double q, double v){
  if (q > QcutMin && q < Qcutmax && v < Vpeakmax) return 1; // 50-100
  else return 0;
}

int is_mip(
  double **Q, double **V, int isc
){
  int ismip = 1;
  double *Q_A = Q[0], *Q_B = Q[1];

  if( cut(Q_A[isc], V[0][isc]) && cut(Q_B[isc], V[1][isc]) ){
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

  else{
    ismip = 0;
  }

  return ismip;
}






void Analysis::Run(int argc, char *argv[], int window_close_handle){

  TApplication *myapp;
  if (argc != 3)
  {
    printf("Usage: %s [infile_name] [outfile_name]\n", argv[0]);
    exit(-1);
  }

  if (window_close_handle != -1) myapp = new TApplication("myapp", 0, 0);

  TFile *f = new TFile(argv[2], "RECREATE");


  Analysis *a = new Analysis(argv[1], f, window_close_handle);

  set_style();

  a->Loop();
  if (window_close_handle != -1)  myapp->Run(true);
}