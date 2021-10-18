void Equalization()
{
//=========Macro generated from canvas: Equalization/Equalization
//=========  (Mon Oct 18 10:22:32 2021) by ROOT version 6.24/02
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
   190.9921,
   359.6172,
   447.9759,
   309.4831,
   346.6725,
   372.8258,
   553.4983,
   393.143};
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
   0.8069812,
   0.8463871,
   1.756872,
   0.6925509,
   0.7690763,
   0.8717722,
   1.429395,
   2.415933};
   TGraphErrors *gre = new TGraphErrors(8,eq0_fx1001,eq0_fy1001,eq0_fex1001,eq0_fey1001);
   gre->SetName("eq0");
   gre->SetTitle("Charge peaks");
   gre->SetFillStyle(1000);
   gre->SetMarkerStyle(21);
   gre->SetMarkerSize(1.5);
   
   TH1F *Graph_eq01001 = new TH1F("Graph_eq01001","Charge peaks",100,-1,8);
   Graph_eq01001->SetMinimum(153.7108);
   Graph_eq01001->SetMaximum(591.4019);
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
   332.2993,
   502.9767,
   365.2239,
   397.0538,
   389.041,
   276.803,
   405.4192,
   338.3597};
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
   1.867124,
   1.618711,
   0.7766485,
   1.064049,
   1.216217,
   0.5799355,
   0.947996,
   1.335521};
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
   TLine *line = new TLine(0,373.8365,8,373.8365);
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
