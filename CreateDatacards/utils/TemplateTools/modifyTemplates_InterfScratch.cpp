#include <iostream>
#include <cmath>
#include <string>
#include "TChain.h"
#include "TString.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"


void modifySigTemplates(TString dir, TString sqrts = "7TeV", TString channame = "2e2mu"){
	TString cinputNominal = dir + "/" + sqrts + "/" + channame + "_templates_Nominal.root";
	TString cinputResUp = dir + "/" + sqrts + "/" + channame + "_templates_ResUp.root";
	TString cinputScaleUp = dir + "/" + sqrts + "/" + channame + "_templates_ScaleUp.root";
	TString cinputResDown = dir + "/" + sqrts + "/" + channame + "_templates_ResDown.root";
	TString cinputScaleDown = dir + "/" + sqrts + "/" + channame + "_templates_ScaleDown.root";
	TString cinput[5] = {
		cinputNominal,
		cinputResUp,
		cinputResDown,
		cinputScaleUp,
		cinputScaleDown
	};

	TString coutput = dir + "/" + sqrts + "/" + channame + "_templates_Modified_InterfScratched_Nominal_ScaleResUpDown.root";
	TFile* foutput = new TFile(coutput, "recreate");

	const int kNumTemplates=9;
	const int kNumSysts=5;
	const double EquivalenceCouplings[2][2] = { // PURE BSM TEMPLATE YIELD SHOULD CORRESPOND TO WHAT MC CONSTANTS ARE GIVEN FOR MIXTURE, NOT WHAT SHUFFLED TREES PREDICT
		{
			12046.01, // MC input T2
			12045.824 // Shuffled trees prediction T2
		},
		{
			1.638, // MC input T3
			1.638041 // Shuffled trees prediction T3
		}
	};
	const double XsecTemplates_Generated[kNumTemplates] = { // PUT THE SCALE FACTORS GIVEN TO MIXED TEMPLATES IN JSON FILES, COULD PUT 1 IF YOU DID NOT SCALE MIXED TEMPLATES BEFORE SUBTRACTION
		1,1,1, // T1, T2, T3
		3.968, 3.662, 3.678, // T4p, T5p, T6p
		0, 2, 2 // T7p, T8p, T9p
	};
	const double XsecTemplates[kNumTemplates] = { // PUT THE SCALE FACTORS YOU WANT
		1,1,1, // T1, T2, T3
		3.968030, 3.662325, 3.677925, // T4p, T5p, T6p
		0, 1.999900, 2.000040 // T7p, T8p, T9p
	};

	TFile* finput[kNumSysts];
	TH3F* hsig[kNumSysts * kNumTemplates];
	TH3F* hsig_scaleres[(kNumSysts-3) * kNumTemplates];
	for (int f = 0; f < kNumSysts; f++){
		finput[f] = new TFile(cinput[f], "read");
		cout << "Reading file: " << finput[f]->GetName() << "..." << endl;

		TString ctemplate_main = "T_3D_";
		for (int t = 0; t < kNumTemplates; t++){
			char tcode[2];
			if(t>2 && t!=6) sprintf(tcode, "%i%s", t + 1,"p");
			else sprintf(tcode, "%i", t + 1);
			TString ctemplate = ctemplate_main + tcode;
			hsig[9 * f + t] = (TH3F*)finput[f]->Get(ctemplate);
			cout << ctemplate << endl;
			cout << hsig[9 * f + t]->GetName() << endl;
		};
	};

	for (int f = 0; f < kNumSysts; f++){
		TString ctemplate_main = "T_3D_";
		for (int t = 0; t < kNumTemplates; t++){
			char tcode[2];
			sprintf(tcode, "%i", t + 1);
			TString ctemplate = ctemplate_main + tcode;
			TString ctemplate_scaleres = ctemplate + "_ScaleRes";
			if ((f - 1) % 2 == 0) ctemplate_scaleres = ctemplate_scaleres + "Up";
			if ((f - 1) % 2 == 1) ctemplate_scaleres = ctemplate_scaleres + "Down";
			if (f == 0) hsig[9 * f + t]->SetName(ctemplate);
			else hsig[9 * f + t]->SetName(ctemplate_scaleres);
			if (t == 6) continue;
			if (t == 1) hsig[9 * f + t]->Scale(pow(EquivalenceCouplings[0][0] / EquivalenceCouplings[0][1], 2));
			if (t == 2) hsig[9 * f + t]->Scale(pow(EquivalenceCouplings[1][0] / EquivalenceCouplings[1][1], 2));
			if (t > 2) hsig[9 * f + t]->Scale(XsecTemplates[t]/XsecTemplates_Generated[t]);
			if (t > 2 && !(t == 5 || t == 8)) hsig[9 * f + t]->Add(hsig[9 * f + 0], -1);
			if (t == 3 || t == 5 || t == 6 || t == 8){
				hsig[9 * f + t]->Add(hsig[9 * f + 1], -1);
			};
			if (t == 4 || t == 5 || t == 7 || t == 8){
				hsig[9 * f + t]->Add(hsig[9 * f + 2], -1);
			};
			if (t == 3 || t == 5) hsig[9 * f + t]->Scale(-1);
		};
	};

	for (int f = 0; f < kNumSysts; f++){
		TString ctemplate_main = "T_3D_";
		for (int t = 0; t < kNumTemplates; t++){
			char tcode[2];
			sprintf(tcode, "%i", t + 1);
			TString ctemplate = ctemplate_main + tcode;
			TString ctemplate_scaleres = ctemplate + "_ScaleRes";
			if ((f - 1) % 2 == 0) ctemplate_scaleres = ctemplate_scaleres + "Up";
			if ((f - 1) % 2 == 1) ctemplate_scaleres = ctemplate_scaleres + "Down";
//			if (f == 1 || f == 2){
			if (f == 1){
				hsig_scaleres[kNumTemplates * (f - 1) + t] = (TH3F*) hsig[kNumTemplates * f + t]->Clone(ctemplate_scaleres);
				hsig_scaleres[kNumTemplates * (f - 1) + t]->SetTitle(ctemplate_scaleres);
			};
			if (f == 2){
				hsig_scaleres[kNumTemplates * (f - 1) + t] = (TH3F*) hsig[kNumTemplates * (f-1) + t]->Clone(ctemplate_scaleres);
				hsig_scaleres[kNumTemplates * (f - 1) + t]->SetTitle(ctemplate_scaleres);
			};
//			if (f == 3 || f == 4){
			if (f == 3){
				hsig_scaleres[kNumTemplates * (f - 3) + t]->Add(hsig[kNumTemplates * f + t], 1.0);
			};
			if (f == 4){
				hsig_scaleres[kNumTemplates * (f - 3) + t]->Add(hsig[kNumTemplates * (f-1) + t], 1.0);
			};
		};
	};
	cout << "Integrals:\n"
		<< "Template\tNominal\tUp\tDown" << endl;
	for (int t = 0; t < kNumTemplates; t++){
		TString ccanvas = "validateSig_";
		char tcode[2];
		sprintf(tcode, "%i", t+1);
		ccanvas = ccanvas + tcode;

		foutput->WriteTObject(hsig[kNumTemplates * 0 + t]);
		cout << t+1 << '\t' << hsig[kNumTemplates * 0 + t]->Integral() << '\t';
		for (int f = 0; f < 1; f++){
			hsig_scaleres[kNumTemplates * f + t]->Add(hsig[kNumTemplates * 0 + t], -1.0);
			foutput->WriteTObject(hsig_scaleres[kNumTemplates * f + t]);
			cout << hsig_scaleres[kNumTemplates * f + t]->Integral() << '\t';
		};
		for (int f = 1; f < (kNumSysts-3); f++){
			hsig_scaleres[kNumTemplates * f + t]->Scale(-1.0);
			hsig_scaleres[kNumTemplates * f + t]->Add(hsig[kNumTemplates * 0 + t], 3.0);
			foutput->WriteTObject(hsig_scaleres[kNumTemplates * f + t]);
			cout << hsig_scaleres[kNumTemplates * f + t]->Integral() << '\t';
		};
		cout << endl;

		TString ccanvas_xy = ccanvas + "_xy";
		TCanvas* c = new TCanvas(ccanvas_xy,"",1600,1200);
		c->Divide(2,2);
		c->cd(1);
		hsig[kNumTemplates * 0 + t]->Project3D("xy")->Draw("colz");
		c->cd(2);
		hsig_scaleres[kNumTemplates * 0 + t]->Project3D("xy")->Draw("colz");
		c->cd(3);
		hsig_scaleres[kNumTemplates * 1 + t]->Project3D("xy")->Draw("colz");
		foutput->WriteTObject(c);
		delete c;
		TString ccanvas_xz = ccanvas + "_xz";
		c = new TCanvas(ccanvas_xz,"",1600,1200);
		c->Divide(2,2);
		c->cd(1);
		hsig[kNumTemplates * 0 + t]->Project3D("xz")->Draw("colz");
		c->cd(2);
		hsig_scaleres[kNumTemplates * 0 + t]->Project3D("xz")->Draw("colz");
		c->cd(3);
		hsig_scaleres[kNumTemplates * 1 + t]->Project3D("xz")->Draw("colz");
		foutput->WriteTObject(c);
		delete c;
		TString ccanvas_yz = ccanvas + "_yz";
		c = new TCanvas(ccanvas_yz,"",1600,1200);
		c->Divide(2,2);
		c->cd(1);
		hsig[kNumTemplates * 0 + t]->Project3D("yz")->Draw("colz");
		c->cd(2);
		hsig_scaleres[kNumTemplates * 0 + t]->Project3D("yz")->Draw("colz");
		c->cd(3);
		hsig_scaleres[kNumTemplates * 1 + t]->Project3D("yz")->Draw("colz");
		foutput->WriteTObject(c);
		delete c;
	};

	foutput->Close();
	for (int f = 0; f < kNumSysts; f++) finput[f]->Close();
}


void modifyBkgTemplates (TString dir, TString sqrts = "7TeV", TString channame = "2e2mu"){
	TString cinput = dir + "/" + sqrts + "/" + channame + "_templates_Nominal_bkg.root";
	TString coutput = dir + "/" + sqrts + "/" + channame + "_templates_Modified_Nominal_bkg.root";
	
	TFile* finput = new TFile(cinput,"read");
	TFile* foutput = new TFile(coutput,"recreate");
	TH3F* ggzz = (TH3F*) finput->Get("template_ggZZ");
	TH3F* qqzz = (TH3F*) finput->Get("template_qqZZ");
	TH3F* zx= (TH3F*) finput->Get("template_ZX");

	qqzz->Scale(1.0/qqzz->Integral());
	zx->Scale(1.0/zx->Integral());

	TH3F* zx_up = (TH3F*) qqzz->Clone("template_ZX_Up");
	TH3F* zx_down = (TH3F*) zx->Clone("template_ZX_Down");
	zx_up->SetNameTitle("template_ZX_Up","template_ZX_Up");
	zx_down->SetNameTitle("template_ZX_Down","template_ZX_Down");
	for (int binx = 1; binx <= zx_down->GetNbinsX(); binx++){
		for (int biny = 1; biny <= zx_down->GetNbinsY(); biny++){
			for (int binz = 1; binz <= zx_down->GetNbinsZ(); binz++){
				double bincontent_zx = zx->GetBinContent(binx, biny, binz);
				double bincontent_zxup = zx_up->GetBinContent(binx, biny, binz);
				double bincontent = 2.0*bincontent_zx - bincontent_zxup;
				if (bincontent <= 0) bincontent = 1.0e-40;
				zx_down->SetBinContent(binx, biny, binz,bincontent);
			};
		};
	};
	zx_down->Scale(1.0/zx_down->Integral());

	foutput->WriteTObject(ggzz);
	foutput->WriteTObject(qqzz);
	foutput->WriteTObject(zx);
	foutput->WriteTObject(zx_up);
	foutput->WriteTObject(zx_down);

	TString ccanvas = "validateBkg";
	TString ccanvas_xy = ccanvas + "_xy";
	TCanvas* c = new TCanvas(ccanvas_xy,"",1600,1200);
	c->Divide(2,2);
	c->cd(2);
	gStyle->SetPalette(1);
	zx->SetLineColor(1);
	zx->Project3D("xy")->Draw("colz");
	c->cd(1);
	qqzz->SetLineColor(2);
	qqzz->Project3D("xy")->Draw("colz");
	c->cd(3);
	zx_up->SetLineColor(4);
	zx_up->Project3D("xy")->Draw("colz");
	c->cd(4);
	zx_down->SetLineColor(4);
	zx_down->Project3D("xy")->Draw("colz");
	foutput->WriteTObject(c);
	delete c;
	TString ccanvas_xz = ccanvas + "_xz";
	c = new TCanvas(ccanvas_xz,"",1600,1200);
	c->Divide(2,2);
	c->cd(2);
	gStyle->SetPalette(1);
	zx->SetLineColor(1);
	zx->Project3D("xz")->Draw("colz");
	c->cd(1);
	qqzz->SetLineColor(2);
	qqzz->Project3D("xz")->Draw("colz");
	c->cd(3);
	zx_up->SetLineColor(4);
	zx_up->Project3D("xz")->Draw("colz");
	c->cd(4);
	zx_down->SetLineColor(4);
	zx_down->Project3D("xz")->Draw("colz");
	foutput->WriteTObject(c);
	delete c;
	TString ccanvas_yz = ccanvas + "_yz";
	c = new TCanvas(ccanvas_yz,"",1600,1200);
	c->Divide(2,2);
	c->cd(2);
	gStyle->SetPalette(1);
	zx->SetLineColor(1);
	zx->Project3D("yz")->Draw("colz");
	c->cd(1);
	qqzz->SetLineColor(2);
	qqzz->Project3D("yz")->Draw("colz");
	c->cd(3);
	zx_up->SetLineColor(4);
	zx_up->Project3D("yz")->Draw("colz");
	c->cd(4);
	zx_down->SetLineColor(4);
	zx_down->Project3D("yz")->Draw("colz");
	foutput->WriteTObject(c);
	delete c;

	foutput->Close();
	finput->Close();
}

void modifyTemplates_InterfScratch(TString dir, int processSig=1, int processBkg=1){
	TString channame[3] = { "4mu","4e","2e2mu" };
	TString sqrts[2] = { "7TeV","8TeV" };
	for (int s = 0; s < 2; s++){
		for (int f = 0; f < 3; f++){
			if(processBkg==1) modifyBkgTemplates(dir, sqrts[s], channame[f]);
			if(processSig==1) modifySigTemplates(dir, sqrts[s], channame[f]);
		};
	};
}