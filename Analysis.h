//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Sep 22 11:42:27 2021 by ROOT version 5.34/14
// from TTree CRT/CRT
// found on file: run156Ana.root
//////////////////////////////////////////////////////////
#define ROOT_CLASS root_class


#ifndef Analysis_h
#define Analysis_h

#define ROOT_CLASS root_class
#include "root_class.h"
#include "root_class.C"

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
      c->Print(Form("plots/%s.C", _name.Data()));
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
  virtual void CreateHistDict();
  virtual void CloseOutFile();
  virtual TH1F *GetHist(string name, int x, int y);
  pair<string, HistMatrix*> CreatePair(TString, int, int, int, TString, TString, TString, TString, int, double, double, TString, TString, int, double, double);
  void FillHists(unordered_map<string, double**> hist_array_map, int i, int j);
  void FillHists(unordered_map<string, pair<double**, double**>> hist_array_map, int i, int j);
  unordered_map<string, HistMatrix *> hist_dict;
};

#endif

Analysis::Analysis(TString _infileName, TFile *f, int window_close_handle, TTree *tree) : 
  _outfile(f), _window_close_handle(window_close_handle)
{
  CreateHistDict();
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


void fill_arrays(unordered_map<double**, double> arr_value_map, int i, int j){
  for(auto &pair: arr_value_map) pair.first[i][j] = pair.second;
}
