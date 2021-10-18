void Equalization()
{
//=========Macro generated from canvas: Equalization/Equalization
//=========  (Mon Oct 18 11:11:32 2021) by ROOT version 6.24/02
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
   306.9569,
   370.9386,
   445.7729,
   310.5149,
   356.7408,
   383.213,
   558.4183,
   401.7216};
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
   1.379362,
   1.166167,
   2.247841,
   1.070612,
   1.499733,
   1.233895,
   1.938546,
   2.375666};
   TGraphErrors *gre = new TGraphErrors(8,eq0_fx1001,eq0_fy1001,eq0_fex1001,eq0_fey1001);
   gre->SetName("eq0");
   gre->SetTitle("Charge peaks");
   gre->SetFillStyle(1000);
   gre->SetMarkerStyle(21);
   gre->SetMarkerSize(1.5);
   
   TH1F *Graph_eq01001 = new TH1F("Graph_eq01001","Charge peaks",100,-1,8);
   Graph_eq01001->SetMinimum(280.0996);
   Graph_eq01001->SetMaximum(585.8348);
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
   382.1042,
   514.5368,
   360.2242,
   401.1601,
   394.3021,
   276.2614,
   419.5001,
   345.2698};
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
   1.844963,
   2.260441,
   1.272168,
   1.663998,
   1.432352,
   0.8279882,
   1.61001,
   1.960512};
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
   TLine *line = new TLine(0,389.2272,8,389.2272);
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
