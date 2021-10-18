void Equalization()
{
//=========Macro generated from canvas: Equalization/Equalization
//=========  (Thu Oct 14 11:41:20 2021) by ROOT version 6.24/02
   TCanvas *Equalization = new TCanvas("Equalization", "Equalization",13,34,700,500);
   gStyle->SetOptFit(1);
   Equalization->SetHighLightColor(2);
   Equalization->Range(0,0,1,1);
   Equalization->SetFillColor(0);
   Equalization->SetBorderMode(0);
   Equalization->SetBorderSize(2);
   Equalization->SetFrameBorderMode(0);
   
   Double_t eq0_fx1001[8] = {
   0,
   1,
   2,
   3,
   4,
   5,
   6,
   7};
   Double_t eq0_fy1001[8] = {
   200.3629,
   359.1448,
   425.0144,
   301.3794,
   341.9208,
   371.8114,
   537.0831,
   390.1968};
   Double_t eq0_fex1001[8] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t eq0_fey1001[8] = {
   0.5754729,
   0.8262914,
   0.8755296,
   0.5558305,
   0.7359539,
   0.9329983,
   1.221907,
   1.764725};
   TGraphErrors *gre = new TGraphErrors(8,eq0_fx1001,eq0_fy1001,eq0_fex1001,eq0_fey1001);
   gre->SetName("eq0");
   gre->SetTitle("Charge peaks");
   gre->SetFillStyle(1000);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#ff0000");
   gre->SetLineColor(ci);
   
   TH1F *Graph_eq01001 = new TH1F("Graph_eq01001","Charge peaks",100,-1,8);
   Graph_eq01001->SetMinimum(165.9357);
   Graph_eq01001->SetMaximum(572.1568);
   Graph_eq01001->SetDirectory(0);
   Graph_eq01001->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_eq01001->SetLineColor(ci);
   Graph_eq01001->GetXaxis()->SetTitle("# Scintillator");
   Graph_eq01001->GetXaxis()->SetLabelFont(42);
   Graph_eq01001->GetXaxis()->SetTitleOffset(1);
   Graph_eq01001->GetXaxis()->SetTitleFont(42);
   Graph_eq01001->GetYaxis()->SetTitle("Charge (pC)");
   Graph_eq01001->GetYaxis()->SetLabelFont(42);
   Graph_eq01001->GetYaxis()->SetTitleFont(42);
   Graph_eq01001->GetZaxis()->SetLabelFont(42);
   Graph_eq01001->GetZaxis()->SetTitleOffset(1);
   Graph_eq01001->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_eq01001);
   
   gre->Draw("ap");
   
   Double_t eq1_fx1002[8] = {
   0,
   1,
   2,
   3,
   4,
   5,
   6,
   7};
   Double_t eq1_fy1002[8] = {
   319.6801,
   490.0146,
   352.3502,
   386.3161,
   368.5165,
   269.1376,
   397.3637,
   332.939};
   Double_t eq1_fex1002[8] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t eq1_fey1002[8] = {
   0.8497227,
   1.186766,
   0.7198019,
   1.343679,
   0.8212293,
   0.528964,
   0.8344413,
   1.185817};
   gre = new TGraphErrors(8,eq1_fx1002,eq1_fy1002,eq1_fex1002,eq1_fey1002);
   gre->SetName("eq1");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#00ff00");
   gre->SetLineColor(ci);
   gre->Draw("p");
   
   TLegend *leg = new TLegend(0,0,0,0,NULL,"brNDC");
   leg->SetBorderSize(1);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(1001);
   TLegendEntry *entry=leg->AddEntry("eq0","Side 0","lpf");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry=leg->AddEntry("eq1","Side 1","lpf");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   leg->Draw();
   Equalization->Modified();
   Equalization->cd();
   Equalization->SetSelected(Equalization);
}
