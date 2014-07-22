#include "TCanvas.h"
#include "TLegend.h"
#include <iostream>
#include "TROOT.h"
#include "TStyle.h"
#include "TPaveText.h"
#include "TChain.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TLine.h"
using namespace std;


TString fileExpected = "/scratch0/hep/hroskes/CMSSW_6_1_1/src/HiggsAnalysis/HZZ4l_Combination/CreateDatacards/cards_HeshyExpected/HCG/higgsCombineobs_1D_fL1_PMF.MultiDimFit.mH91.2.root";
TString fileObserved = "/scratch0/hep/hroskes/CMSSW_6_1_1/src/HiggsAnalysis/HZZ4l_Combination/CreateDatacards/cards_HeshyObserved/HCG/higgsCombineobs_1D_fL1_PMF.MultiDimFit.mH91.2.root";

void makePlotScan(){
  gROOT->ProcessLine(".x tdrstyle.cc");
  gStyle->SetPadLeftMargin(0.16);
  gStyle->SetPadTopMargin(0.05);
  float mass = 125.6;

  TChain* t[2];
  for(int c=0;c<2;c++) t[c] = new TChain("limit");

  Float_t CMS_zz4l_fai1,CMS_zz4l_fai2,CMS_zz4l_phiai1,deltaNLL;
  double minX=0,minY=0,minLL=20;

  TH1F* h0 = new TH1F("h0","",201,-0.005,1+0.005); // 201 points. expected
  TH1F* h0p = new TH1F("h0","",201,-0.005,1+0.005); // 201 points, observed

  TGraph* gr[2];
  TH1F* hContainer[2] = {
      h0,h0p
  };
  double** x = new double* [2];
  double** y = new double* [2];

  for (int f = 0; f< 20; f++){
      int firstPoint = f * 10;
      int lastPoint = (f+1) * 10;

      char boh[200];
      //sprintf(boh, "./fL1_PMF/higgsCombine2D_exp_%i_%i.MultiDimFit.mH%.1f.root",firstPoint,lastPoint, mass);
//    sprintf(boh, "./fL1_PMF/higgsCombine2D_exp_%i_%i.MultiDimFit.mH%.1f.root",firstPoint,lastPoint, mass);
      //t[0]->Add(boh);
  };
  char cb0[200];
  //sprintf(cb0, "./fL1_PMF/higgsCombine2D_exp_%i_%i.MultiDimFit.mH%.1f.root",200,201, mass);
//sprintf(cb0, "./fL1_PMF/higgsCombine2D_exp_%i_%i.MultiDimFit.mH%.1f.root",200,201, mass);
/*  for (int f = 0; f< 20; f++){
      int firstPoint = f * 10;
      int lastPoint = (f+1) * 10;

      char boh[200];
      //sprintf(boh, "./fL1_PMF/higgsCombine2D_obs_%i_%i.MultiDimFit.mH%.1f.root",firstPoint,lastPoint, mass);
//    sprintf(boh, "./fL1_fa2fa3_fL10/higgsCombine2D_obs_%i_%i.MultiDimFit.mH%.1f.root",firstPoint,lastPoint, mass);
      //t[1]->Add(boh);
  };
  char cb0p[200];
  sprintf(cb0p, "./fL1_PMF/higgsCombine2D_obs_%i_%i.MultiDimFit.mH%.1f.root",200,201, mass);
//sprintf(cb0, "./fL1_fa2fa3_fL10/higgsCombine2D_exp_%i_%i.MultiDimFit.mH%.1f.root",200,201, mass);
  //t[1]->Add(cb0p);
*/
// Descending order
  //t[1]->Add("./Observed_Fits/higgsCombine2D_obs_1D_fL1_PMF.MultiDimFit.mH125.6.root");

  t[0]->Add(fileExpected);
  t[1]->Add(fileObserved);

  t[0]->SetBranchAddress("CMS_zz4l_fai1",&CMS_zz4l_fai1);
  t[0]->SetBranchAddress("deltaNLL",&deltaNLL);
  t[1]->SetBranchAddress("CMS_zz4l_fai1",&CMS_zz4l_fai1);
  t[1]->SetBranchAddress("deltaNLL",&deltaNLL);

  for (int ev = 0; ev < t[0]->GetEntries(); ev++){
      t[0]->GetEntry(ev);
      int binx = h0->GetXaxis()->FindBin(CMS_zz4l_fai1);
      double bincontent = h0->GetBinContent(binx);
      if (bincontent == 0 && deltaNLL >= 0) h0->SetBinContent(binx, 2 * deltaNLL);
  };

  for (int ev = 0; ev < t[1]->GetEntries(); ev++){
      t[1]->GetEntry(ev);
      int binx = h0p->GetXaxis()->FindBin(CMS_zz4l_fai1);
      double bincontent = h0p->GetBinContent(binx);
      if (bincontent == 0 && deltaNLL > 0) h0p->SetBinContent(binx, 2 * deltaNLL);
      if (deltaNLL == 0) cout << "Minimum at " << CMS_zz4l_fai1 << endl;
  };


  for (int c = 0; c < 2; c++){
      x[c] = new double[hContainer[c]->GetNbinsX()];
      y[c] = new double[hContainer[c]->GetNbinsX()];

//  for (int binx = 1; binx <= hContainer[c]->GetNbinsX(); binx++){
    for (int binx = 1; binx <= ( hContainer[c]->GetNbinsX() ) ; binx++){
        x[c][binx-1] = hContainer[c]->GetBinCenter(binx);
        y[c][binx-1] = hContainer[c]->GetBinContent(binx);
    };

    gr[c] = new TGraph(( hContainer[c]->GetNbinsX() ),x[c],y[c]);
    gr[c]->SetName(Form("tg_%i",c));
    gr[c]->SetTitle("");
    gr[c]->SetLineWidth(2);
    gr[c]->SetMarkerColor(0);
  };

    gr[0]->SetLineColor(kBlack);
    gr[0]->SetLineStyle(7);
    gr[1]->SetLineColor(kBlack);
    gr[1]->SetLineStyle(1);



  TCanvas *c1=new TCanvas("can_scan1D_fL1_fa2fa3_fL10","",800,800);
    c1->SetFillColor(0);
    c1->SetBorderMode(0);
    c1->SetBorderSize(2);
    c1->SetTickx(1);
    c1->SetTicky(1);
    c1->cd();
    c1->SetFrameFillStyle(0);
    c1->SetFrameBorderMode(0);
    c1->SetFrameFillStyle(0);
    c1->SetFrameBorderMode(0);

  gr[0]->GetXaxis()->SetTitle("f_{H}");
  gr[0]->GetYaxis()->SetTitle("-2 #Delta lnL");
  gr[0]->GetXaxis()->SetLabelSize(0.04);
  gr[0]->GetYaxis()->SetLabelSize(0.04);
  gr[0]->GetXaxis()->SetTitleSize(0.06);
  gr[0]->GetYaxis()->SetTitleSize(0.06);
  gr[0]->GetXaxis()->SetLabelFont(42);
  gr[0]->GetYaxis()->SetLabelFont(42);
  gr[0]->GetXaxis()->SetTitleFont(42);
  gr[0]->GetYaxis()->SetTitleFont(42);
  gr[0]->GetYaxis()->SetRangeUser(0., 100.);
  gr[0]->GetXaxis()->SetRangeUser(0,1);
  gr[0]->Draw("AL");
  if(t[1]->GetEntries()!=0) gr[1]->Draw("lsame");

  TLegend *leg = new TLegend(0.2,0.7,0.5,0.92);
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.03);

  leg->AddEntry(gr[1], "Observed","l");
  leg->AddEntry(gr[0], "Expected","l");
  leg->Draw();

  float lumi7TeV=5.1;
  float lumi8TeV=19.7;

//  TPaveText* pt = new TPaveText(0.15,0.94,0.85,1,"brNDC");
    TPaveText* pt = new TPaveText(0.15,0.955,0.96,1,"brNDC");

    pt->SetBorderSize(0);
    pt->SetFillStyle(0);
    pt->SetTextAlign(12);
    pt->SetTextFont(42);
    pt->SetTextSize(0.04);
    TText* text = pt->AddText(0.02,0.45,"#font[42]{CMS}");
    text->SetTextSize(0.044);
        text = pt->AddText(0.14, 0.45, "#font[42]{(preliminary)}");
        text->SetTextSize(0.0315);
    text = pt->AddText(0.48,0.45,"#font[42]{19.7 fb^{-1} (8 TeV) + 5.1 fb^{-1} (7 TeV)}");
    text->SetTextSize(0.0315);
    pt->Draw();


  TPaveText *oneSig = new TPaveText(0.18,0.16,0.28,0.19,"NDC");
  oneSig->SetFillColor(0);
  oneSig->SetFillStyle(0);
  oneSig->SetTextFont(42);
  oneSig->SetTextColor(kBlack);
  oneSig->SetBorderSize(0);
  oneSig->AddText("68\% CL"); 
  //oneSig->Draw();

  TPaveText *twoSig = new TPaveText(0.18,0.24,0.28,0.27,"NDC");
  twoSig->SetFillColor(0);
  twoSig->SetFillStyle(0);
  twoSig->SetTextFont(42);
  twoSig->SetTextColor(kBlack);
  twoSig->SetBorderSize(0);
  twoSig->AddText("95\% CL");
  //twoSig->Draw();

  TLine *l1=new TLine();
  l1->SetLineStyle(9);
  l1->SetLineWidth(1);
  l1->SetLineColor(kBlack);
  l1->DrawLine(0,1,1,1);
  l1->Draw("same");
  TLine *l2=new TLine();
  l2->SetLineStyle(9);
  l2->SetLineWidth(1);
  l2->SetLineColor(kBlue);
  l2->DrawLine(0,3.84,1,3.84);
  l2->Draw("same");
  leg->AddEntry(l1,"68\% CL","l");
  leg->AddEntry(l2,"95\% CL","l");

  c1->SaveAs("plotScan.eps");
  c1->SaveAs("plotScan.pdf");
  c1->SaveAs("plotScan.png");
  c1->SaveAs("plotScan.root");

  c1->SetLogy();
  c1->SaveAs("plotScan_log.eps");
  c1->SaveAs("plotScan_log.pdf");
  c1->SaveAs("plotScan_log.png");
  c1->SaveAs("plotScan_log.root");

/*
  for (int c = 0; c < 2; c++){
      delete gr[c];
      delete[] x[c];
      delete[] y[c];
      delete hContainer[c];
      delete t[c];
  }
  delete h0;
*/}
