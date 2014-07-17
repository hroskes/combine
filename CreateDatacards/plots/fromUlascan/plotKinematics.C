#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include "TMath.h"
//#include "Math.h"
#include "QuantFuncMathCore.h"
#include "TLorentzVector.h"
#include "TPaveText.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TTree.h"
#include "TChain.h"
#include "TColor.h"
#include "TString.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "../setRates.C"

using namespace std;
using namespace ROOT::Math;

void plotKinematics(const int iKD, bool withSMDcut = false){

    const int nKDs = 8;
    TString KDlist[nKDs] = {
        "ZZMass", // 0
        "Z1Mass", // 1
        "Z2Mass", // 2
        "helcosthetaZ1", // 3
        "helcosthetaZ2", // 4
        "costhetastar", // 5
        "helphi", // 6
        "phistarZ1" // 7
    };
    TString KDlist_title[nKDs] = {
        "m_{4l} (GeV)", // 0
        "m_{1} (GeV)", // 1
        "m_{2} (GeV)", // 2
        "cos #theta_{1}", // 3
        "cos #theta_{2}", // 4
        "cos #theta*", // 5
        "#Phi", // 6
        "#Phi_{1}" // 7
    };
    if(iKD>=nKDs) assert(0);
    TString label_BSMHypo[1] = {
        "H(91.2)#rightarrowZZ#rightarrow4l" // 0
    };

    double rangeKD[nKDs][2] = { { 0 } };
    rangeKD[0][0] = 80;
    rangeKD[0][1] = 100;
    rangeKD[1][0] = 40;
    rangeKD[1][1] = 120;
    rangeKD[2][0] = 12;
    rangeKD[2][1] = 62;
    rangeKD[3][0] = -1;
    rangeKD[3][1] = 1;
    rangeKD[4][0] = -1;
    rangeKD[4][1] = 1;
    rangeKD[5][0] = -1;
    rangeKD[5][1] = 1;
    rangeKD[6][0] = -TMath::Pi();
    rangeKD[6][1] = TMath::Pi();
    rangeKD[7][0] = -TMath::Pi();
    rangeKD[7][1] = TMath::Pi();

    TH1D* hdata[6];
    TH1D* hsig[6];
    TH1D* hggzz[6];
    TH1D* hqqzz[6];
    TH1D* hz[6];
    int nbins=20;

    double plotWidth = (rangeKD[iKD][1]-rangeKD[iKD][0])/nbins;

    int order[6]={2,1,0,4,3,5};
    TString chan[6]={"ch1","ch2","ch3","ch4","ch5","ch6"};
    TString channameO[6]={"LHC_7TeV/4e/","LHC_7TeV/4mu/","LHC_7TeV/2mu2e/","LHC_8TeV/4mu/","LHC_8TeV/2mu2e/","LHC_8TeV/4e/"};
    int energy[6] = {7,7,7,8,8,8};
    TString flavor[6] = {"4e","4mu","2e2mu","4mu","2e2mu","4e"};
/*
    double normO[6]=    {    0.6951,    1.2439,    1.6662,    5.9471,    7.6807,    3.0898    }; 
    double normzO[6]=    {    0.6226,    0.2230,    1.0628,    1.1878,    4.2929,    2.7676    };
    double normqqO[6]=    {    0.8386,    1.7971,    2.2456,    7.6478,    8.8585,    2.9364    };
    double normggO[6]=    {    0.0341,    0.0625,    0.0741,    0.4131,    0.5005,    0.2041    };
*/
    double normO[6]=    { 0 }; 
    double normzO[6]=    { 0 };
    double normqqO[6]=    { 0 };
    double normggO[6]=    { 0 };

    for (int i = 0; i < 6; i++)
    {
//        normzO[i] = getSumOfWeights(energy[i],flavor[i],"Z") * luminosity[energy[i]-7];
//        normO[i] = /*getSumOfWeights(energy[i],flavor[i],"H") * luminosity[energy[i]-7]*/ normzO[i];
//        normqqO[i] = getSumOfWeights(energy[i],flavor[i],"TU") * luminosity[energy[i]-7];
//        normggO[i] = getSumOfWeights(energy[i],flavor[i],"gg") * luminosity[energy[i]-7];
//        cout << normO[i] << " " << normzO[i] << " " << normqqO[i] << " " << normggO[i] << endl;
    }

    double norm[6] = { 0 };
    double normgg[6] = { 0 };
    double normz[6] = { 0 };
    double normqq[6] = { 0 };

    double integral[6] = { 0 };
    double integralgg[6] = { 0 };
    double integralz[6] = { 0 };
    double integralqq[6] = { 0 };

    TString channame[6];
    TChain* tdata[6];
    TChain* tsig[6];
    TChain* tggzz[6];
    TChain* tqqzz[6];
    TChain* tz[6];

    TString coutput = "./plots_KD/plotContainer_";
    coutput.Append(KDlist[iKD]);
    if(withSMDcut) coutput.Append("_SignalEnriched");
    coutput.Append(".root");
    TFile* foutput = new TFile(coutput,"recreate");

// Grab the trees
    for (int j = 0; j < 6; j++){
        int idx = order[j];
        channame[j] = channameO[idx];
        norm[j] = normO[idx];
        normgg[j] = normggO[idx];
        normqq[j] = normqqO[idx];
        normz[j] = normzO[idx];

        TString file_tdata,file_tsig,file_tggzz,file_tqqzz,file_tz;
        file_tdata = getFilename(energy[j],flavor[j],"data");
        file_tsig  = getFilename(energy[j],flavor[j],"H");
        file_tggzz = getFilename(energy[j],flavor[j],"gg");
        file_tqqzz = getFilename(energy[j],flavor[j],"STU");
        file_tz    = getFilename(energy[j],flavor[j],"STU");
        tdata[j] = new TChain("SelectedTree");
        tsig[j] = new TChain("SelectedTree");
        tggzz[j] = new TChain("SelectedTree");
        tqqzz[j] = new TChain("SelectedTree");
        tz[j] = new TChain("SelectedTree");

        tdata[j]->Add(file_tdata);
        tsig [j]->Add(file_tsig);
        tggzz[j]->Add(file_tggzz);
        tqqzz[j]->Add(file_tqqzz);
        tz   [j]->Add(file_tz);

        TH1D *htempqqZZ=new TH1D("htqqZZ","",1,0.0,1000.);
        TH1D *htempggZZ=new TH1D("htggZZ","",1,0.0,1000.);
        TH1D *htempZ=new TH1D("htZ","",1,0.0,1000.);
        TH1D *htempH=new TH1D("htH","",1,0.0,1000.);
        tsig[j]->Draw("ZZMass>>htH","(ZZMass<100.0&&ZZMass>=80.0)*MC_weight");
        double sumH = htempH->GetSumOfWeights();
        integral[j] = sumH;

        tqqzz[j]->Draw("ZZMass>>htqqZZ","MC_weight_ZZQQBSTU[1]*MC_weight*(ZZMass<100&&ZZMass>=80)");
        integralqq[j] = htempqqZZ->GetSumOfWeights();
        tggzz[j]->Draw("ZZMass>>htggZZ","(MC_weight)*(ZZMass<100&&ZZMass>=80)");
        integralgg[j] = htempggZZ->GetSumOfWeights();
        tz[j]->Draw("ZZMass>>htZ","MC_weight_ZZQQBSTU[0]*MC_weight*(ZZMass<100&&ZZMass>=80)");
        integralz[j] = htempZ->GetSumOfWeights();

        delete htempH;
        delete htempqqZZ;
        delete htempggZZ;
        delete htempZ;
        TString cFill = KDlist[iKD];
        cFill.Append(">>");
        TString cFillAlt = cFill;
        cFillAlt.Append("+");

        TString cdata = chan[j];
        cdata.Append("_data");
        hdata[j] = new TH1D(cdata,"",nbins,rangeKD[iKD][0],rangeKD[iKD][1]);
        TString cFill_data = cFill;
        cFill_data.Append(cdata);
        if(!withSMDcut) tdata[j]->Draw(cFill_data,"(ZZMass<100.0&&ZZMass>=80.0)");
        else tdata[j]->Draw(cFill_data,"(ZZMass>=121.5&&ZZMass<130.5)");

        TString csig = chan[j];
        csig.Append("_sig");
        hsig[j] = new TH1D(csig,"",nbins,rangeKD[iKD][0],rangeKD[iKD][1]);
        TString cFill_sig = cFill;
        cFill_sig.Append(csig);
        tsig[j]->Draw(cFill_sig,"MC_weight * (ZZMass<100.0&&ZZMass>=80.0)");

        TString cggzz = chan[j];
        cggzz.Append("_ggzz");
        hggzz[j] = new TH1D(cggzz,"",nbins,rangeKD[iKD][0],rangeKD[iKD][1]);
        TString cFill_ggzz = cFill;
        TString cFill_ggzz_alt = cFillAlt;
        cFill_ggzz.Append(cggzz);
        cFill_ggzz_alt.Append(cggzz);
        if (!withSMDcut){
            tggzz[j]->Draw(cFill_ggzz, "(MC_weight)*(ZZMass<100.0&&ZZMass>=80.0)");
        }
        else{
            tggzz[j]->Draw(cFill_ggzz, "(MC_weight)*(ZZMass>=121.5&&ZZMass<130.5)");
        };

        TString cqqzz = chan[j];
        cqqzz.Append("_qqzz");
        hqqzz[j] = new TH1D(cqqzz,"",nbins,rangeKD[iKD][0],rangeKD[iKD][1]);
        TString cFill_qqzz = cFill;
        TString cFill_qqzz_alt = cFillAlt;
        cFill_qqzz.Append(cqqzz);
        cFill_qqzz_alt.Append(cqqzz);
        if (!withSMDcut){
            tqqzz[j]->Draw(cFill_qqzz, "(MC_weight_ZZQQBSTU[1]*MC_weight)*(ZZMass<100.0&&ZZMass>=80.0)");
        }
        else{
            tqqzz[j]->Draw(cFill_qqzz, "(MC_weight_ZZQQBSTU[1]*MC_weight)*(ZZMass>=121.5&&ZZMass<130.5)");
        }

        TString cz = chan[j];
        cz.Append("_z");
        hz[j] = new TH1D(cz,"",nbins,rangeKD[iKD][0],rangeKD[iKD][1]);
        TString cFill_z = cFill;
        cFill_z.Append(cz);
        if(!withSMDcut) tz[j]->Draw(cFill_z,"MC_weight_ZZQQBSTU[0]*MC_weight*(ZZMass<100.0&&ZZMass>=80.0)");
        else tz[j]->Draw(cFill_z,"MC_weight_ZZQQBSTU[0]*MC_weight*(ZZMass>=121.5&&ZZMass<130.5)");
        cout << chan[j] << endl;
/*
        cout << "BEFORE, Restricted:" << endl;
        cout << "H:    " << hsig[j]->Integral() << endl;
        cout << "ggZZ: " << hggzz[j]->Integral() << endl;
        cout << "qqZZ: " << hqqzz[j]->Integral() << endl;
        cout << "Z:    " << hz[j]->Integral() << endl;

        cout << "\nBEFORE, Full:" << endl;
        cout << "H:    " << integral[j] << endl;
        cout << "ggZZ: " << integralgg[j] << endl;
        cout << "qqZZ: " << integralqq[j] << endl;
        cout << "Z:    " << integralz[j] << endl;
*/

        hsig[j]->Scale(luminosity[energy[j]-7]);
        hggzz[j]->Scale(luminosity[energy[j]-7]);
        hqqzz[j]->Scale(luminosity[energy[j]-7]);
        hz[j]->Scale(luminosity[energy[j]-7]);
        hsig[j]->Scale(hz[j]->Integral() / hsig[j]->Integral());
/*
        cout << "AFTER:" << endl;
        cout << "H:    " << hsig[j]->Integral() << endl;
        cout << "ggZZ: " << hggzz[j]->Integral() << endl;
        cout << "qqZZ: " << hqqzz[j]->Integral() << endl;
        cout << "Z:    " << hz[j]->Integral() << endl;
*/
        delete tdata[j];
        delete tsig[j];
        delete tggzz[j];
        delete tqqzz[j];
        delete tz[j];
    };
    for (int j = 1; j < 6; j++){
        hdata[0]->Add(hdata[j]);
        hsig[0]->Add(hsig[j]);
        hggzz[0]->Add(hggzz[j]);
        hqqzz[0]->Add(hqqzz[j]);
        hz[0]->Add(hz[j]);

        delete hdata[j];
        delete hsig[j];
        delete hggzz[j];
        delete hqqzz[j];
        delete hz[j];
    };

    hqqzz[0]->Add(hggzz[0]);
    hz[0]->Add(hqqzz[0]);
    hsig[0]->Add(hqqzz[0]);

    TGraphAsymmErrors* tgdata;
    int ndata = 0;
    double xx_data[100];
    double xu_data[100];
    double xd_data[100];
    double yy_data[100];
    double yu_data[100];
    double yd_data[100];
    const double quant = (1.0-0.6827)/2.0;
    double max_data=0;

    for(int bin=1;bin<=hdata[0]->GetNbinsX();bin++){
        double bincenter = hdata[0]->GetBinCenter(bin);
        double bincontent = hdata[0]->GetBinContent(bin);

        if(bincontent>=0){
            xx_data[ndata] = bincenter;
            yy_data[ndata] = bincontent;
            xu_data[ndata] = 0;
            xd_data[ndata] = 0;
            yu_data[ndata] = ROOT::Math::chisquared_quantile_c(quant,2*(bincontent+1))/2.-bincontent;
            yd_data[ndata] = (bincontent==0)?0:(bincontent-ROOT::Math::chisquared_quantile_c(1-quant,2*bincontent)/2.);
            double high_data = yy_data[ndata]+yu_data[ndata];
            if (high_data > max_data) max_data = high_data;
            ndata++;
        };
    };
    cout << max_data << endl;
    tgdata = new TGraphAsymmErrors(ndata,xx_data,yy_data,xd_data,xu_data,yd_data,yu_data);
    tgdata->SetMarkerSize(1.2);
    tgdata->SetMarkerStyle(20);
    tgdata->SetMarkerColor(kBlack);
    tgdata->SetLineColor(kBlack);
    tgdata->SetLineWidth(1);
    gStyle->SetTitleFont(62, "t");
    gROOT->SetStyle(gStyle->GetName());
    gROOT->ForceStyle();

    TPaveText* pt = new TPaveText(0.15,0.93,0.85,1,"brNDC");
    pt->SetBorderSize(0);
    pt->SetFillStyle(0);
    pt->SetTextAlign(12);
    pt->SetTextFont(42);
    pt->SetTextSize(0.045);
    TText* text = pt->AddText(0.025,0.45,"#font[42]{CMS}");
    text->SetTextSize(0.044);
        text = pt->AddText(0.165, 0.45, "#font[42]{(preliminary)}");
        text->SetTextSize(0.0315);
    text = pt->AddText(0.537,0.45,"#font[42]{19.7 fb^{-1} (8 TeV) + 5.1 fb^{-1} (7 TeV)}");
    text->SetTextSize(0.0315);

    string strenrichedcut = "121.5<m_{4l}<130.5 GeV";

    for (int bin = 1; bin <= hsig[0]->GetNbinsX(); bin++){
        double bincontent = hsig[0]->GetBinContent(bin);
        double bincontent2 = hz[0]->GetBinContent(bin);
        if (bincontent2 > bincontent) bincontent = bincontent2;
        if (max_data < bincontent) max_data = bincontent;
    };
    cout << max_data << endl;

//  hsig[0]->SetFillColor(kOrange + 10);
//  hsig[0]->SetFillStyle(3001);
    hsig[0]->SetLineColor(kViolet);
    hsig[0]->SetLineWidth(2);
    hsig[0]->SetLineStyle(2);
//  hqqzz[0]->SetLineColor(TColor::GetColor("#000099"));
    hqqzz[0]->SetFillColor(kAzure-9);
    hqqzz[0]->SetFillStyle(1001);
    hqqzz[0]->SetLineWidth(2);
    hqqzz[0]->SetLineStyle(1);
    hz[0]->SetLineColor(kOrange + 10);
    hz[0]->SetLineWidth(2);
    hz[0]->SetLineStyle(1);

    foutput->cd();
    TString canvasname_2D = "cCompare_DataMC_AllTeV_";
    canvasname_2D.Append(KDlist[iKD]);
    if(withSMDcut) canvasname_2D.Append("_SignalEnriched");
    TCanvas* c2D = new TCanvas(canvasname_2D,"",8,30,800,800);
    c2D->cd();
    gStyle->SetOptStat(0);
    c2D->SetFillColor(0);
    c2D->SetBorderMode(0);
    c2D->SetBorderSize(2);
    c2D->SetTickx(1);
    c2D->SetTicky(1);
    c2D->SetLeftMargin(0.17);
    c2D->SetRightMargin(0.05);
    c2D->SetTopMargin(0.07);
    c2D->SetBottomMargin(0.13);
    c2D->SetFrameFillStyle(0);
    c2D->SetFrameBorderMode(0);
    c2D->SetFrameFillStyle(0);
    c2D->SetFrameBorderMode(0);

    TLegend *l2D = new TLegend(0.20,0.57,0.58,0.90);
    l2D->SetBorderSize(0);
    l2D->SetTextFont(42);
    l2D->SetTextSize(0.04);
    l2D->SetLineColor(1);
    l2D->SetLineStyle(1);
    l2D->SetLineWidth(1);
    l2D->SetFillColor(0);
    l2D->SetFillStyle(0);

    char yTitle[100];
    if (iKD >= 3){
        if (plotWidth >= 1) sprintf(yTitle, "Events / %.1f", plotWidth);
        else sprintf(yTitle, "Events / %.2f", plotWidth);
    }
    else{
        if (plotWidth >= 1) sprintf(yTitle, "Events / %.1f GeV", plotWidth);
        else sprintf(yTitle, "Events / %.2f GeV", plotWidth);
    };

    hsig[0]->GetXaxis()->SetTitle(KDlist_title[iKD]);
    hsig[0]->GetYaxis()->SetTitle(yTitle);
    hsig[0]->GetYaxis()->SetRangeUser(0,max_data*1.25);
    if(iKD!=0 && iKD!=1) hsig[0]->GetYaxis()->SetRangeUser(0,max_data*1.8);
    hsig[0]->GetXaxis()->SetNdivisions(505);
    hsig[0]->GetXaxis()->SetLabelFont(42);
    hsig[0]->GetXaxis()->SetLabelOffset(0.007);
    hsig[0]->GetXaxis()->SetLabelSize(0.05);
    hsig[0]->GetXaxis()->SetTitleSize(0.06);
    hsig[0]->GetXaxis()->SetTitleOffset(1);
    hsig[0]->GetXaxis()->SetTitleFont(42);
    hsig[0]->GetYaxis()->SetNdivisions(505);
    hsig[0]->GetYaxis()->SetLabelFont(42);
    hsig[0]->GetYaxis()->SetLabelOffset(0.007);
    hsig[0]->GetYaxis()->SetLabelSize(0.05);
    hsig[0]->GetYaxis()->SetTitleSize(0.06);
    hsig[0]->GetYaxis()->SetTitleOffset(1.1);
    hsig[0]->GetYaxis()->SetTitleFont(42);
    if (iKD == 4 || iKD == 6 || iKD == 13 || iKD == 15){
        hsig[0]->GetXaxis()->SetTitleSize(0.057);
    };

    l2D->AddEntry(tgdata,"Observed","p");
    l2D->AddEntry(hsig[0],label_BSMHypo[0],"l");
    l2D->AddEntry(hqqzz[0],"gg/q#bar{q}#rightarrowZZ/Z#gamma*/#gamma*#gamma*#rightarrow4l","f");
    l2D->AddEntry(hz[0],"q#bar{q}#rightarrowZ#rightarrow4l","f");

    hsig[0]->Draw("hist");
    hqqzz[0]->Draw("same");
    hz[0]->Draw("same");
    tgdata->Draw("e1psame");
    l2D->Draw("same");
    pt->Draw();
    TPaveText *pt10 = new TPaveText(0.56,0.87,0.95,0.93,"brNDC");
    pt10->SetBorderSize(0);
    pt10->SetTextAlign(12);
    pt10->SetTextSize(0.04);
    pt10->SetFillStyle(0);
    pt10->SetTextFont(42);
    /*TText* text10 = */pt10->AddText(0.01,0.01,strenrichedcut.c_str());
    if(withSMDcut) pt10->Draw();
    c2D->RedrawAxis();
    c2D->Modified();
    c2D->Update();
    tgdata->SetName("total_data");
    foutput->WriteTObject(tgdata);
    foutput->WriteTObject(hdata[0]);
    foutput->WriteTObject(hsig[0]);
    foutput->WriteTObject(hqqzz[0]);
    foutput->WriteTObject(hz[0]);
    foutput->WriteTObject(c2D);

    TString canvasname_2D_pdf=canvasname_2D;
    TString canvasname_2D_eps=canvasname_2D;
    TString canvasname_2D_png=canvasname_2D;
    TString canvasname_2D_root=canvasname_2D;
    TString canvasname_2D_c=canvasname_2D;
    canvasname_2D_pdf.Append(".pdf");
    canvasname_2D_eps.Append(".eps");
    canvasname_2D_png.Append(".png");
    canvasname_2D_root.Append(".root");
    canvasname_2D_c.Append(".C");
    c2D->SaveAs(canvasname_2D_pdf);
    c2D->SaveAs(canvasname_2D_eps);
    c2D->SaveAs(canvasname_2D_png);
    c2D->SaveAs(canvasname_2D_root);
    c2D->SaveAs(canvasname_2D_c);
    c2D->Close();

    foutput->Close();
}

void plotKinematics()
{
    for (int i = 0; i < 8; i++)
        plotKinematics(i);
}
