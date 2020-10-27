//STL headers
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cmath>

//ROOT headers
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH3.h"
#include "TH2.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TSpectrum.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TLine.h"
#include "TStyle.h"
#include "TMath.h"
#include "TGaxis.h"
#include "TExec.h"
#include "TVirtualFitter.h"
#include "TDirectory.h"
#include "TColor.h"

//CMSSW headers
#include "HEDarkening.C"

#define maxHDeta 14
#define maxHDlay 19

using namespace std;

int mode = 0;
void set_mode(int m) { mode = m; }

int phase = 1;
void set_phase(int p){ phase = p; }

//--------------------------------------------
//function to return depth segmentation scheme
//ref e.g. Geometry/HcalEventSetup/python/HcalRelabel_cfi.py, Geometry/HcalCommonData/data/Phase0/hcalRecNumbering.xml, Geometry/HcalCommonData/data/PhaseI/hcalRecNumbering.xml
int* get_depths(int ieta){
	//shift for endcap
	ieta += 16;
	//define depth segmentation
	int _depths[]     = {1,1,2,2,3,3,3,4,4,4,4,4,5,5,5,5,5,5,5};
	int _depths16[]   = {0,0,0,0,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0};
	int _depths17[]   = {1,1,2,2,3,3,3,4,4,4,4,4,5,5,5,0,0,0,0};
	int _depths18[]   = {1,1,2,2,3,3,3,4,4,4,4,4,5,5,5,5,5,0,0};
	int _depths0[]    = {1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2};
	int _depths0_16[] = {0,0,0,0,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0};
	int _depths0_17[] = {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0};
	int _depths0_18[] = {1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,0,0};
	int _depths0_27[] = {1,1,1,1,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3};
	int* depths = new int[maxHDlay];
	for(int i = 0; i < maxHDlay; i++){
		if(phase==1){
			if(ieta==16) depths[i] = _depths16[i];
			else if(ieta==17) depths[i] = _depths17[i];
			else if(ieta==18) depths[i] = _depths18[i];
			else depths[i] = _depths[i];
		}
		else if(phase==0){
			if(ieta==16) depths[i] = _depths0_16[i];
			else if(ieta==17) depths[i] = _depths0_17[i];
			else if(ieta==18) depths[i] = _depths0_18[i];
			else if(ieta>=27) depths[i] = _depths0_27[i];
			else depths[i] = _depths0[i];
		}
	}

/*	//figure out how many depths there are
	ndepths = 0;
	int maxdepth = 0;
	for(int i = 0; i < maxHDlay; i++){
		if(depths[i]>maxdepth){
			ndepths++;
			maxdepth++;
		}
	}
*/	
	return depths;
}

//just hardcoded for now...
int get_ndepths() { 
	if(phase==1) return 5;
	else if(phase==0) return 3;
}

//---------------------------------------------------------
//function to calculate correction factors for a given lumi
double** get_corrs(double lumi, double cutoff){
	//mean energy in each layer of each tower at 0 lumi
	Double_t Emean[maxHDeta][maxHDlay] = 
	{
      {0.000000,0.000000,0.001078,0.008848,0.014552,0.011611,0.008579,0.003211,0.002964,0.001775,0.001244,0.000194,0.000159,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000}, //tower 16
      {0.107991,0.110127,0.081192,0.050552,0.032968,0.022363,0.012158,0.009392,0.006228,0.003650,0.003512,0.001384,0.002693,0.000171,0.000012,0.000000,0.000000,0.000000,0.000000}, //tower 17
      {0.676505,0.490168,0.296676,0.171789,0.129949,0.082862,0.058659,0.044634,0.031707,0.019662,0.015764,0.009106,0.006647,0.004244,0.003099,0.002173,0.001148,0.001037,0.001255}, //tower 18
      {0.000000,1.722120,1.182322,0.777626,0.551815,0.381650,0.287366,0.209747,0.149539,0.095313,0.072690,0.052678,0.039654,0.026296,0.017126,0.010785,0.009125,0.006478,0.005883}, //tower 19
      {0.000000,2.253261,1.735958,1.188160,0.840946,0.598602,0.468244,0.302274,0.231664,0.155488,0.107260,0.071025,0.052279,0.040206,0.029258,0.016750,0.013793,0.010577,0.005453}, //tower 20
      {0.000000,2.531237,1.890250,1.299543,0.910114,0.669070,0.488073,0.362763,0.261798,0.177501,0.126352,0.089263,0.060376,0.045327,0.028015,0.021607,0.014022,0.010632,0.007277}, //tower 21
      {0.000000,2.864449,2.128851,1.430183,0.997971,0.742801,0.534812,0.386734,0.272178,0.202083,0.151007,0.106616,0.080018,0.055961,0.042164,0.026671,0.017066,0.010679,0.008012}, //tower 22
      {0.000000,3.245071,2.455721,1.654948,1.168912,0.847157,0.620316,0.450344,0.329651,0.237315,0.164873,0.111421,0.076739,0.058448,0.042908,0.026116,0.019845,0.012941,0.009722}, //tower 23
      {0.000000,3.523457,2.661981,1.771658,1.309808,0.945457,0.701592,0.484851,0.369293,0.265011,0.187915,0.131769,0.095761,0.066367,0.047637,0.034704,0.025890,0.019738,0.011549}, //tower 24
      {0.000000,3.927840,3.003811,2.029410,1.449478,1.099043,0.807025,0.585442,0.438498,0.318257,0.227604,0.153886,0.115857,0.088632,0.057335,0.040283,0.031698,0.022189,0.013614}, //tower 25
      {0.000000,4.351642,3.468444,2.368010,1.716175,1.279396,0.944951,0.700572,0.508232,0.371673,0.274277,0.195301,0.136719,0.100344,0.081408,0.057241,0.039744,0.027569,0.017390}, //tower 26
      {0.000000,3.315232,2.799176,1.928155,1.384991,1.040579,0.757341,0.574996,0.419020,0.306780,0.219979,0.160667,0.112313,0.084583,0.068486,0.052088,0.034637,0.023234,0.018230}, //tower 27
      {0.000000,3.960416,3.689421,2.514970,1.835407,1.372355,1.447654,1.087639,0.836911,0.652146,0.507556,0.416950,0.333755,0.283386,0.224336,0.191078,0.169798,0.144836,0.120804}, //tower 28
      {0.000000,1.530837,1.507162,0.977401,0.694320,0.543973,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000} //tower 29
    };
	
	//HEDarkening object for darkening weights
	HEDarkening darkening(mode);
	int ndepths = get_ndepths();

	//initialize 2D arrays of correction factors & mean sums [tower][depth]
	double **corrs = new double*[maxHDeta];
	double *meansums0 = new double[ndepths];
	double *meansumsL = new double[ndepths];
	for(int j = 0; j < maxHDeta; j++){
		corrs[j] = new double[ndepths];
		
		//reset sum(means(layer,0)) and sum(means(layer,lumi))
		for(int n = 0; n < ndepths; n++){
			meansums0[n] = 0;
			meansumsL[n] = 0;
		}
		
		//get depth seg for this tower
		int *depths = get_depths(j);
		//fill sum(means(layer,0)) and sum(means(layer,lumi))
		for(int i = 0; i < maxHDlay; i++){
			if(depths[i]==0) continue;
			meansums0[depths[i]-1] += Emean[j][i];
			meansumsL[depths[i]-1] += Emean[j][i]*(darkening.degradation(lumi,j+16,i-1)); //be careful of eta and layer numbering
		}
		
		//compute factors, w/ safety checks
		for(int n = 0; n < ndepths; n++){
			if(meansumsL[n]>0) corrs[j][n] = meansums0[n]/meansumsL[n];
			else corrs[j][n] = 1; //no energy, nothing to correct...
			
			//cutoff to avoid absurd factors
			if(cutoff > 1 && corrs[j][n] > cutoff) corrs[j][n] = cutoff;
			else if(corrs[j][n] < 1) corrs[j][n] = 1;
			
			//if(lumi==300 && n<3) {
			//	std::cout << "nval[" << j << "][" << n+1 << "]" << " = " << meansums0[n]
			//			  << " - " << "dval["<< j << "][" << n+1 << "]" << " = " << meansumsL[n] << "    corr = " << corrs[j][n] << std::endl;
			//}
		}
	}
	
	return corrs;
}

//------------------------------------------------------------
//function to compare correction factors vs. lumi for all etas
void comp_corrs(bool do_print=0, double cutoff=100){
	//lumi values are arbitrary now - whatever we want to plot
	Double_t lumis[] = {0, 100, 200, 300, 400, 500, 600, 700};
	int maxHDlumi = 8;
	Double_t ietas[] = {1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9};
	Int_t colors[] = {kSpring+8,kAzure+5,kPink-9,kRed+2,kYellow,kViolet+2,kGreen,kGreen+2,kOrange+7,kTeal,kMagenta,kYellow+2,kBlue,kRed};
	
	//get depth segmentation
	int ndepths = get_ndepths();

	//canvases, legends, paves
	TCanvas **can = new TCanvas*[ndepths];
	TPad **pad1 = new TPad*[ndepths];
	TPad **pad2 = new TPad*[ndepths];
	TLegend **leg = new TLegend*[ndepths];
	TPaveText **pave = new TPaveText*[ndepths];
	TLine **line = new TLine*[ndepths];
	
	//declare graphs & value arrays
	Double_t ***vals = new Double_t**[ndepths];
	TGraph ***graphs = new TGraph**[ndepths];
	Double_t *ymax = new Double_t[ndepths];
	Double_t *ymin = new Double_t[ndepths];
	
	//initialize arrays
	for(int n = 0; n < ndepths; n++){
		vals[n] = new Double_t*[maxHDeta];
		graphs[n] = new TGraph*[maxHDeta];
		ymax[n] = 0;
		ymin[n] = 2;
		for(int j = 0; j < maxHDeta; j++){
			vals[n][j] = new Double_t[maxHDlumi];
		}
	}
	
	//get correction factors
	for(int k = 0; k < maxHDlumi; k++){
		double **corrs = get_corrs(lumis[k],cutoff);
		for(int n = 0; n < ndepths; n++){
			for(int j = 0; j < maxHDeta; j++){
				vals[n][j][k] = corrs[j][n];
				//check extrema
				if(vals[n][j][k] > ymax[n]) ymax[n] = vals[n][j][k];
				if(vals[n][j][k] < ymin[n]) ymin[n] = vals[n][j][k];
			}
		}
	}
	
	//make graphs & plotting objects
	std::stringstream *dname = new std::stringstream[ndepths];
	std::stringstream *oname = new std::stringstream[ndepths];
	std::stringstream etaname[maxHDeta];
	
	for(int n = 0; n < ndepths; n++){
		dname[n] << "Depth " << n+1;
		
		can[n] = new TCanvas((dname[n].str()).c_str(),(dname[n].str()).c_str(),700,500);
		//pad1[n] = new TPad("graph","",0,0,1,1);
		//pad1[n]->SetMargin(0.1,0.05,0.15,0.05);
		pad1[n] = new TPad("graph","",0,0,0.85,1.0);
		pad1[n]->SetMargin(0.2,0.05,0.15,0.05);
		if(ymax[n]/ymin[n] > 100) pad1[n]->SetLogy(); //only use log scale if corrs span several orders of magnitude
		pad1[n]->SetTicks(1,1);
		pad1[n]->Draw();
		can[n]->cd();
		pad2[n] = new TPad("dmc","",0.82,0,1.0,1.0);
		//pad2[n]->SetMargin(0.1,0.1,0.25,0.05);
		pad2[n]->Draw();
		pad1[n]->cd();
		
		//leg[n] = new TLegend(0.5,0.55,0.65,0.85);
		leg[n] = new TLegend(0,0.075,1,0.825);
		leg[n]->SetFillColor(0);
		leg[n]->SetBorderSize(0);
		leg[n]->SetTextSize(0.2);
		leg[n]->SetTextFont(42);
		
		//pave[n] = new TPaveText(0.65,0.95,0.95,1.0,"NDC");
		pave[n] = new TPaveText(0,0.85,1,0.95,"NDC");
		pave[n]->AddText((dname[n].str()).c_str());
		pave[n]->SetFillColor(0);
		pave[n]->SetBorderSize(0);
		pave[n]->SetTextFont(42);
		pave[n]->SetTextSize(0.2);
		
		for(int j = 0; j < maxHDeta; j++){
			//only initialize names once
			if(n==0) etaname[j] << "i#eta = " << ietas[j]*10;
			
			//create & format graphs
			graphs[n][j] = new TGraph(maxHDlumi,lumis,vals[n][j]);
			graphs[n][j]->SetTitle("");
			graphs[n][j]->GetXaxis()->SetTitle("Luminosity [fb^{-1}]");
			graphs[n][j]->GetYaxis()->SetTitle("correction factor");
			graphs[n][j]->SetFillColor(0);
			graphs[n][j]->SetMarkerStyle(20);
			graphs[n][j]->SetMarkerColor(colors[j]);
			graphs[n][j]->SetMarkerSize(1.5);
			graphs[n][j]->SetLineColor(colors[j]);
			graphs[n][j]->GetYaxis()->SetRangeUser(ymin[n]*0.95,ymax[n]*1.05);
			graphs[n][j]->GetYaxis()->SetTitleSize(32/(pad1[n]->GetWh()*pad1[n]->GetAbsHNDC()));
			graphs[n][j]->GetYaxis()->SetLabelSize(28/(pad1[n]->GetWh()*pad1[n]->GetAbsHNDC()));
			graphs[n][j]->GetXaxis()->SetTitleSize(32/(pad1[n]->GetWh()*pad1[n]->GetAbsHNDC()));
			graphs[n][j]->GetXaxis()->SetLabelSize(28/(pad1[n]->GetWh()*pad1[n]->GetAbsHNDC()));
			graphs[n][j]->GetYaxis()->SetTickLength(12/(pad1[n]->GetWh()*pad1[n]->GetAbsHNDC()));
			graphs[n][j]->GetXaxis()->SetTickLength(12/(pad1[n]->GetWh()*pad1[n]->GetAbsHNDC()));	
			graphs[n][j]->GetXaxis()->SetNdivisions(507);	
			
			//add to legend
			leg[n]->AddEntry(graphs[n][j],(etaname[j].str()).c_str(),"p");
			
			//drawing
			if(j==0) graphs[n][j]->Draw("APZ");
			else graphs[n][j]->Draw("PZ same");
		}
		//cutoff line
		line[n] = new TLine(graphs[n][0]->GetXaxis()->GetXmin(),100,graphs[n][0]->GetXaxis()->GetXmax(),100);
		line[n]->SetLineStyle(2);
		line[n]->SetLineWidth(1);
		line[n]->SetLineColor(kBlack);
		line[n]->Draw("same");		
		
		pad2[n]->cd();
		pave[n]->Draw();
		leg[n]->Draw("same");
		
		if(do_print){
			oname[n] << "mean_corr_comp_depth_" << n+1;
			if(cutoff>0) oname[n] << "_cutoff" << cutoff;
			//oname[n] << ".png";
			oname[n] << "_zoom.png";
			can[n]->Print((oname[n].str()).c_str(),"png");
		}
	}
	
}

//---------------------------------------------------
//function to make plot of signal loss for given lumi
TH2F* signal_loss(double lumi, double zcut=0., bool do_show=true, bool do_print=false, bool do_tower_eta=false){
	gStyle->SetPalette(1); //rainbow
	
	//HEDarkening object for darkening weights
	HEDarkening darkening(mode);
	
	TH2F* hist = new TH2F("signal_loss","",18,-0.5,17.5,14,-29.5,-15.5);

	for(int j = 0; j < maxHDeta; j++){
		//fill histo (negative ieta)
		for(int i = 0; i <= maxHDlay; i++){
			int bin = hist->FindBin(i,-(j+16));
			double sigloss = darkening.degradation(lumi,j+16,i);
			if(darkening.towerDose[j][i+1]==0) sigloss = 0;
			hist->SetBinContent(bin,sigloss); //be careful of eta and layer numbering
			//hist->SetBinContent(i+1,14-j,darkening.degradation(lumi,j+16,i)); //be careful of eta and layer numbering
		}
	}

	//formatting
	hist->SetTitle("");
	hist->GetXaxis()->SetTitle("Layer");
	hist->GetYaxis()->SetTitle("Tower");
	stringstream sz;
	sz << "relative signal after " << lumi << " fb^{-1}";
	hist->GetZaxis()->SetTitle((sz.str()).c_str());
	//hist->GetZaxis()->SetRangeUser(0.,1.);
	if(zcut<0.) zcut = 0.;
	else if(zcut>1.) zcut = 1.;
	hist->GetZaxis()->SetRangeUser(zcut,1.);
	
	//drawing
	TCanvas* can = new TCanvas("signal_loss","signal_loss",900,600);
	TPad* pad1 = new TPad("graph","",0,0,1,1);
	if(do_tower_eta) pad1->SetMargin(0.13,0.15,0.125,0.05);
	else pad1->SetMargin(0.1,0.175,0.125,0.05);
	pad1->SetTicks(1,1);
	pad1->Draw();
	pad1->cd();

	//more formatting
	hist->GetYaxis()->SetTitleOffset(0.75);
	hist->GetZaxis()->SetTitleOffset(0.85);
	hist->GetXaxis()->SetTitleOffset(0.85);
	hist->GetZaxis()->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetZaxis()->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetYaxis()->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetYaxis()->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetXaxis()->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetXaxis()->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetYaxis()->SetTickLength(12/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetXaxis()->SetTickLength(12/(pad1->GetWh()*pad1->GetAbsHNDC()));
	
	if(do_show){
		if(do_tower_eta) {
			//modification of label size due to ROOT bug in alphanumeric labels
			//see http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=18346&p=78118
			hist->GetYaxis()->SetLabelSize(32*1.500015/(pad1->GetWh()*pad1->GetAbsHNDC()));
			hist->GetYaxis()->SetTitle("Tower #eta");
			hist->GetZaxis()->SetTitleOffset(0.72);
			//hist->GetYaxis()->SetTitleOffset(1.025); //for precision(3)
			hist->GetYaxis()->SetTitleOffset(1);
			hist->GetYaxis()->SetLabelOffset(0.005);
			Double_t tower_etas[] = {1.305,1.392,1.479,1.566,1.653,1.740,1.830,1.930,2.043,2.172,2.322,2.500,2.650,2.868,3.000};
			for(int j = 0; j < maxHDeta; j++){
				//if(j%2) continue;
				stringstream binname;
				//binname.precision(3);
				binname.precision(2);
				binname << fixed << (tower_etas[j]+tower_etas[j+1])/2;
				//cout << "bin " << j << ": " << binname.str() << endl;
				int ybin = hist->GetYaxis()->FindBin(-(j+16));
				hist->GetYaxis()->SetBinLabel(ybin,(binname.str()).c_str());
			}
		}
	
		//gStyle->SetPaintTextFormat(".2g"); //g is 'adaptive', uses shorter of e or f
		gStyle->SetPaintTextFormat(".2f"); //g is 'adaptive', uses shorter of e or f
		hist->Draw("COLZ TEXT");
		
		if(!do_tower_eta) {
			// Remove the current axis
			hist->GetYaxis()->SetLabelOffset(999);
			//hist->GetYaxis()->SetTickLength(0);
			
			// Redraw the new axis 
			gPad->Update();
		
			TGaxis *newaxis = new TGaxis(gPad->GetUxmin(), 
										gPad->GetUymax(),
										gPad->GetUxmin()-0.001,
										gPad->GetUymin(),
										-hist->GetYaxis()->GetXmax(),
										-hist->GetYaxis()->GetXmin(),
										510,"+");
			newaxis->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
			newaxis->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
			//newaxis->SetTickSize(12/(pad1->GetWh()*pad1->GetAbsHNDC()));
			newaxis->SetTickSize(0);
			newaxis->SetLabelOffset(-0.05);
			newaxis->SetLabelFont(42);
			newaxis->Draw();
		}
		
		if(do_print){
			stringstream outname;
			outname << "signal_loss_lumi" << lumi;
			if(zcut!=0.) outname << "_cut" << zcut;
			if(mode==1) outname << "_fingers";
			else if(mode==2) outname << "_powerlaw";
			if(do_tower_eta) outname << "_eta";
			outname << ".png";
			can->Print((outname.str()).c_str(),"png");
		}
	}
	
	return hist;
}

//------------------------------------------------------------------------
//function to compare signal loss plots
//ctype: 0 = diff, 1 = ratio, 2 = pct err
void comp_signal_loss(double lumi, int ctype, double zcut=0., bool do_print=false){
	TH2F* hist[2];
	
	set_mode(0);
	hist[1] = signal_loss(lumi,zcut,0);
	set_mode(2);
	hist[0] = signal_loss(lumi,zcut,0);

	if(ctype==0){
		//formatting
		stringstream sz;
		sz << "diff(rel. signal after " << lumi << " fb^{-1})";
		hist[0]->GetZaxis()->SetTitle((sz.str()).c_str());
		hist[1]->GetZaxis()->SetTitle((sz.str()).c_str());
		
		//take difference of histos
		hist[0]->Add(hist[1],-1);
		hist[0]->GetZaxis()->SetTitleOffset(0.925);
	}
	else if(ctype==1){
		//formatting
		stringstream sz;
		sz << "ratio(rel. signal after " << lumi << " fb^{-1})";
		hist[0]->GetZaxis()->SetTitle((sz.str()).c_str());
		hist[1]->GetZaxis()->SetTitle((sz.str()).c_str());
		
		//take ratio of histos
		hist[0]->Divide(hist[1]);
		
		//for some reason, divide does not update axis
		hist[0]->GetZaxis()->SetRangeUser(hist[0]->GetBinContent(hist[0]->GetMinimumBin()),hist[0]->GetBinContent(hist[0]->GetMaximumBin()));
	}
	else if(ctype==2){
		//formatting
		stringstream sz;
		sz << "%err(rel. signal after " << lumi << " fb^{-1})";
		hist[0]->GetZaxis()->SetTitle((sz.str()).c_str());
		hist[1]->GetZaxis()->SetTitle((sz.str()).c_str());

		//take pct err of histos
		hist[0]->Add(hist[1],-1);
		hist[0]->Divide(hist[1]);
	}
	else if(ctype==3){
		//formatting
		stringstream sz;
		sz << "rel. diff(rel. signal after " << lumi << " fb^{-1})";
		hist[0]->GetZaxis()->SetTitle((sz.str()).c_str());
		hist[1]->GetZaxis()->SetTitle((sz.str()).c_str());

		//take rel. diff. of histos 2(x-y)/(x+y)
		TH1F* numer = (TH1F*)hist[0]->Clone("numer");
		numer->Add(hist[1],-1);
		TH1F* denom = (TH1F*)hist[0]->Clone("denom");
		denom->Add(hist[1]);
		hist[0]->Divide(numer,denom,2);
	}	
	
	//drawing
	TCanvas* can = new TCanvas("signal_loss","signal_loss",900,600);
	TPad* pad1 = new TPad("graph","",0,0,1,1);
	pad1->SetMargin(0.1,0.175,0.125,0.05);
	pad1->SetTicks(1,1);
	pad1->Draw();
	pad1->cd();

	//draw
	gStyle->SetPaintTextFormat(".2f"); //g is 'adaptive', uses shorter of e or f
	hist[0]->Draw("COLZ TEXT");
	
	// Remove the current axis
	hist[0]->GetYaxis()->SetLabelOffset(999);
	//hist->GetYaxis()->SetTickLength(0);
	
	// Redraw the new axis 
	gPad->Update();
	TGaxis *newaxis = new TGaxis(gPad->GetUxmin(), 
								gPad->GetUymax(),
								gPad->GetUxmin()-0.001,
								gPad->GetUymin(),
								-hist[0]->GetYaxis()->GetXmax(),
								-hist[0]->GetYaxis()->GetXmin(),
								510,"+");
	newaxis->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
	newaxis->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
	//newaxis->SetTickSize(12/(pad1->GetWh()*pad1->GetAbsHNDC()));
	newaxis->SetTickSize(0);
	newaxis->SetLabelOffset(-0.05);
	newaxis->SetLabelFont(42);
	newaxis->Draw();
	
	if(do_print){
		stringstream outname;
		outname << "comp_signal_loss_lumi" << lumi;
		if(zcut!=0.) outname << "_cut" << zcut;
		if(ctype==0) outname << "_diff";
		else if(ctype==1) outname << "_ratio";
		else if(ctype==2) outname << "_pcterr";
		else if(ctype==3) outname << "_reldiff";
		outname << ".png";
		can->Print((outname.str()).c_str(),"png");
	}
}

//-----------------------------------------------------------------------
//function to find max dose when given a minimum relative signal and lumi
void max_dose(double lumi, double zcut=0.01, bool do_print=false){
	//HEDarkening object for darkening weights
	HEDarkening darkening(mode);
	
	vector<Double_t> x; //tower number
	vector<Double_t> y; //max dose in MRad
	Double_t yavg = 0;
	
	for(int j = 0; j < maxHDeta; j++){
		double yval = 0;
		for(int i = maxHDlay-3; i >= 0; i--){
			double sigloss = darkening.degradation(lumi,j+16,i);
			double dose = darkening.towerDose[j][i+1+1]; //dose for next layer
			if(sigloss < zcut && dose>0){
				//cout << j+16 << ": " << i << ", " << sigloss << endl;
				if(darkening.degradation(lumi,j+16,i+1) >= zcut) yval = dose*lumi;
				break;
			}
		}
		if(yval>0){
			x.push_back(j+16);
			y.push_back(yval);
			yavg += yval;
		}
	}
	yavg /= y.size();
	cout << "average: " << yavg << " Mrad" << endl;
	
	TGraph* g = new TGraph(x.size(),&x[0],&y[0]);
	g->SetTitle("");
	g->GetXaxis()->SetTitle("Tower");
	g->GetYaxis()->SetTitle("maximum dose [Mrad]");
	g->GetYaxis()->SetRangeUser(0.,1.5);

	//drawing
	TCanvas* can = new TCanvas("max_dose","max_dose",700,500);
	TPad* pad1 = new TPad("graph","",0,0,1,1);
	pad1->SetMargin(0.125,0.05,0.15,0.075);
	pad1->SetTicks(1,1);
	pad1->Draw();
	pad1->cd();	

	//more formatting
	g->GetYaxis()->SetTitleOffset(0.75);
	g->GetXaxis()->SetTitleOffset(0.85);
	g->GetYaxis()->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
	g->GetYaxis()->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
	g->GetXaxis()->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
	g->GetXaxis()->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
	g->GetYaxis()->SetTickLength(12/(pad1->GetWh()*pad1->GetAbsHNDC()));
	g->GetXaxis()->SetTickLength(12/(pad1->GetWh()*pad1->GetAbsHNDC()));

	g->Draw("AP");
	
	//info
	TPaveText* pave = new TPaveText(0.2,0.94,0.9,0.99,"NDC");
	pave->SetFillColor(0);
	pave->SetBorderSize(0);
	pave->SetTextFont(42);
	pave->SetTextSize(0.05);
	
	stringstream pname;
	pname << lumi << " fb^{-1}, minimum signal = " << zcut;
	pave->AddText((pname.str()).c_str());
	pave->Draw("same");
	
	if(do_print){
		stringstream outname;
		outname << "max_dose_lumi" << lumi << "_cut" << zcut;
		if(mode==1) outname << "_fingers";
		else if(mode==2) outname << "_powerlaw";
		outname << ".png";
		can->Print((outname.str()).c_str(),"png");
	}
	
}

//-------------------------------------------------------------
//function to make plot of recalibration factors for given lumi
void plot_recalib(double lumi, double cutoff=0., bool do_show=true, bool do_print=false, bool do_tower_eta=false){
	//gStyle->SetPalette(1); //rainbow
	
	//reversed rainbow palette Red->Violet
	Int_t ncolors = 50;
	Int_t* colors = new Int_t[ncolors];
	for (int i=0;i<ncolors;i++) colors[i] = 51+(ncolors-i);
	gStyle->SetPalette(ncolors,colors);
	
	int ndepths = get_ndepths();
	TH2F* hist = new TH2F("recalib","",18,-0.5,17.5,14,-29.5,-15.5);
	TH2F** tower_hist = new TH2F*[maxHDeta];
	double **corrs = get_corrs(lumi,cutoff);
	
	double zmax = 0;
	for(int j = 0; j < maxHDeta; j++){
		//figure out binning for this tower
		vector<double> xbins;
		vector<int> xdepths;
		int* depths = get_depths(j);
		int curr_depth = 0;
		for(int i = 0; i < maxHDlay; i++){
			if(depths[i]!=curr_depth){
				xbins.push_back((double)i-0.5);
				curr_depth = depths[i];
				xdepths.push_back(curr_depth);
			}
		}
		if(curr_depth != 0) xbins.push_back(17.5);
		
		//create tower histo
		stringstream hname;
		hname << "tower" << j+16;
		tower_hist[j] = new TH2F((hname.str()).c_str(),"",xbins.size()-1,&xbins[0],1,-(j+16)-0.5,-(j+16)+0.5);
		
		//fill histo
		for(int d = 0; d < xbins.size()-1; d++){
			if(corrs[j][xdepths[d]-1]==1.) tower_hist[j]->SetBinContent(d+1,1,0.0);
			else {
				tower_hist[j]->SetBinContent(d+1,1,corrs[j][xdepths[d]-1]);
				if(corrs[j][xdepths[d]-1]>zmax) zmax = corrs[j][xdepths[d]-1];
			}
		}
	}

	//formatting
	hist->SetTitle("");
	hist->GetXaxis()->SetTitle("Layer");
	hist->GetYaxis()->SetTitle("Tower");
	stringstream sz;
	sz << "recalibration factor after " << lumi << " fb^{-1}";
	hist->GetZaxis()->SetTitle((sz.str()).c_str());
	hist->GetZaxis()->SetRangeUser(0,zmax);
	for(int j = 0; j < maxHDeta; j++){
		tower_hist[j]->GetZaxis()->SetTitle((sz.str()).c_str());
		tower_hist[j]->GetZaxis()->SetRangeUser(0,zmax);
	}
	
	//drawing
	TCanvas* can = new TCanvas("plot_recalib","plot_recalib",900,600);
	TPad* pad1 = new TPad("graph","",0,0,1,1);
	if(do_tower_eta) pad1->SetMargin(0.13,0.15,0.125,0.05);
	else pad1->SetMargin(0.1,0.175,0.125,0.05);
	pad1->SetTicks(1,1);
	pad1->Draw();
	pad1->cd();

	//more formatting
	hist->GetYaxis()->SetTitleOffset(0.75);
	hist->GetZaxis()->SetTitleOffset(0.85);
	hist->GetXaxis()->SetTitleOffset(0.85);
	hist->GetZaxis()->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetZaxis()->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetYaxis()->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetYaxis()->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetXaxis()->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetXaxis()->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetYaxis()->SetTickLength(12/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetXaxis()->SetTickLength(12/(pad1->GetWh()*pad1->GetAbsHNDC()));
	TGaxis *newaxis = NULL;
	
	if(do_show){
		if(do_tower_eta) {
			//modification of label size due to ROOT bug in alphanumeric labels
			//see http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=18346&p=78118
			hist->GetYaxis()->SetLabelSize(32*1.500015/(pad1->GetWh()*pad1->GetAbsHNDC()));
			hist->GetYaxis()->SetTitle("Tower #eta");
			hist->GetZaxis()->SetTitleOffset(0.72);
			//hist->GetYaxis()->SetTitleOffset(1.025); //for precision(3)
			hist->GetYaxis()->SetTitleOffset(1);
			hist->GetYaxis()->SetLabelOffset(0.005);
			Double_t tower_etas[] = {1.305,1.392,1.479,1.566,1.653,1.740,1.830,1.930,2.043,2.172,2.322,2.500,2.650,2.868,3.000};
			for(int j = 0; j < maxHDeta; j++){
				//if(j%2) continue;
				stringstream binname;
				//binname.precision(3);
				binname.precision(2);
				binname << fixed << (tower_etas[j]+tower_etas[j+1])/2;
				//cout << "bin " << j << ": " << binname.str() << endl;
				int ybin = hist->GetYaxis()->FindBin(-(j+16));
				hist->GetYaxis()->SetBinLabel(ybin,(binname.str()).c_str());
			}
		}

		if(zmax<1000) gStyle->SetPaintTextFormat(".2f");
		else gStyle->SetPaintTextFormat(".2g"); //g is 'adaptive', uses shorter of e or f
		hist->Draw("COLZ TEXT");
		for(int j = 0; j < maxHDeta; j++){
			tower_hist[j]->Draw("COLZ TEXT SAME");
		}
		hist->Draw("sameaxis");
		
		if(!do_tower_eta) {
			// Remove the current axis
			hist->GetYaxis()->SetLabelOffset(999);
			//hist->GetYaxis()->SetTickLength(0);
			
			// Redraw the new axis 
			pad1->Update();
		
			newaxis = new TGaxis(pad1->GetUxmin(), 
								 pad1->GetUymax(),
								 pad1->GetUxmin()-0.001,
								 pad1->GetUymin(),
								 -hist->GetYaxis()->GetXmax(),
								 -hist->GetYaxis()->GetXmin(),
								 510,"+");
			newaxis->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
			newaxis->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
			//newaxis->SetTickSize(12/(pad1->GetWh()*pad1->GetAbsHNDC()));
			newaxis->SetTickSize(0);
			newaxis->SetLabelOffset(-0.05);
			newaxis->SetLabelFont(42);
			newaxis->Draw();
		}
		
		if(do_print){
			stringstream outname;
			outname << "plot_recalib_lumi" << lumi;
			outname << "_phase" << phase;
			if(cutoff>1) outname << "_cutoff" << cutoff;
			if(mode==1) outname << "_fingers";
			else if(mode==2) outname << "_powerlaw";
			if(do_tower_eta) outname << "_eta";
			outname << ".png";
			can->Print((outname.str()).c_str(),"png");
		}
	}
}