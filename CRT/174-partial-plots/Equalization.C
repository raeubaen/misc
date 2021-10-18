void Equalization()
{
//=========Macro generated from canvas: Equalization/Equalization
//=========  (Mon Oct 18 10:23:33 2021) by ROOT version 6.24/02
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
   299.365,
   350.7311,
   421.2796,
   298.7802,
   339.0638,
   372.9599,
   540.8347,
   378.5813};
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
   2.129152,
   1.631598,
   2.735816,
   1.730752,
   1.758551,
   2.172599,
   3.18131,
   3.17669};
   TGraphErrors *gre = new TGraphErrors(8,eq0_fx1001,eq0_fy1001,eq0_fex1001,eq0_fey1001);
   gre->SetName("eq0");
   gre->SetTitle("Charge peaks");
   gre->SetFillStyle(1000);
   gre->SetMarkerStyle(21);
   gre->SetMarkerSize(1.5);
   
   TH1F *Graph_eq01001 = new TH1F("Graph_eq01001","Charge peaks",100,-1,8);
   Graph_eq01001->SetMinimum(272.3528);
   Graph_eq01001->SetMaximum(568.7126);
   Graph_eq01001->SetDirectory(0);
   Graph_eq01001->SetStats(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
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
   372.1184,
   488.1244,
   350.9829,
   385.8111,
   406.6811,
   270.3262,
   403.0992,
   329.5591};
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
   4.354814,
   2.244214,
   1.898608,
   4.518004,
   2.360206,
   1.38658,
   2.30822,
   2.700746};
   gre = new TGraphErrors(8,eq1_fx1002,eq1_fy1002,eq1_fex1002,eq1_fey1002);
   gre->SetName("eq1");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#0000ff");
   gre->SetLineColor(ci);

   ci = TColor::GetColor("#0000ff");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(21);
   gre->SetMarkerSize(1.5);
   gre->Draw("p");
   TLine *line = new TLine(0,375.5186,8,375.5186);
   line->Draw();
   
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
