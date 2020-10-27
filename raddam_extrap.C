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
#include "TPaletteAxis.h"

//CMSSW headers
#include "HEDarkening.C"

#define maxHEeta 14
#define maxHElay 19
#define maxHBeta 16
#define maxHBlay 17

using namespace std;

//dose map in MRad/fb-1 for HE from FLUKA
double towerDoseHE[maxHEeta][maxHElay] = {
    {0,0,0,0,3.0078e-06,1.2907e-06,6.9297e-07,5.8978e-07,3.0329e-07,2.5309e-07,0,0,0,0,0,0,0,0,0},
    {9.7545e-06,9.7545e-06,9.7545e-06,6.354e-06,5.4936e-06,3.9469e-06,1.4533e-06,1.2483e-06,8.0622e-07,3.8593e-07,3.0767e-07,1.551e-07,1.3541e-07,1.2756e-07,1.0562e-07,0,0,0,0},
    {2.423e-05,2.423e-05,2.423e-05,1.6219e-05,1.006e-05,7.4817e-06,4.6087e-06,2.9334e-06,1.8785e-06,1.8597e-06,1.1123e-06,5.4146e-07,1.8306e-07,1.4103e-07,1.4018e-07,8.2691e-08,5.4855e-08,0,0},
    {5.1453e-05,5.1453e-05,5.1453e-05,3.192e-05,2.2485e-05,1.5471e-05,9.7196e-06,5.2608e-06,3.6968e-06,2.2216e-06,1.2627e-06,8.4556e-07,5.3709e-07,4.1236e-07,4.1564e-07,2.0679e-07,1.2323e-07,5.2678e-08,9.6205e-08},
    {7.1658e-05,7.1658e-05,7.1658e-05,5.209e-05,3.7567e-05,2.3202e-05,1.3923e-05,9.1784e-06,7.1962e-06,4.0443e-06,2.2254e-06,1.6697e-06,1.1255e-06,8.7753e-07,4.6273e-07,1.7224e-07,1.5472e-07,7.6533e-08,5.8904e-08},
    {0.00010513,0.00010513,0.00010513,6.8076e-05,4.8024e-05,3.2941e-05,2.2905e-05,1.3165e-05,8.5492e-06,5.6401e-06,5.4964e-06,3.2693e-06,1.9466e-06,1.1398e-06,4.774e-07,5.2813e-07,3.6999e-07,3.1992e-07,1.8102e-07},
    {0.00016163,0.00016163,0.00016163,0.00011616,7.5282e-05,4.777e-05,3.3219e-05,2.3143e-05,1.4504e-05,8.6031e-06,8.9107e-06,5.0444e-06,2.3483e-06,1.6303e-06,1.5602e-06,1.1507e-06,5.6686e-07,5.8716e-07,4.7892e-07},
    {0.00024276,0.00024276,0.00024276,0.000158,0.00011011,7.6828e-05,5.1102e-05,3.83e-05,2.6519e-05,1.5143e-05,1.2381e-05,7.9172e-06,5.2049e-06,3.3424e-06,2.0094e-06,6.9646e-07,8.4341e-07,1.0813e-06,9.186e-07},
    {0.00035091,0.00035091,0.00035091,0.00026422,0.00019107,0.0001232,8.3939e-05,5.6385e-05,4.3727e-05,2.6196e-05,1.7212e-05,1.3704e-05,7.0876e-06,5.8282e-06,3.1596e-06,2.8218e-06,1.663e-06,1.6396e-06,1.7477e-06},
    {0.00064298,0.00064298,0.00064298,0.00045671,0.00029163,0.00020474,0.00013345,9.5852e-05,6.8091e-05,5.093e-05,3.739e-05,2.5318e-05,1.6084e-05,1.0943e-05,6.8101e-06,4.5743e-06,3.9845e-06,2.8978e-06,2.3335e-06},
    {0.0010035,0.0010035,0.0010035,0.00070371,0.00048652,0.00035907,0.00024411,0.00016002,0.00012549,8.5174e-05,5.9895e-05,4.0693e-05,2.6581e-05,1.824e-05,1.3063e-05,1.081e-05,8.3734e-06,6.8061e-06,6.786e-06},
    {0.0016469,0.0016469,0.0016469,0.0011721,0.000834,0.00057893,0.00040754,0.0002923,0.00020686,0.00014829,0.00011916,8.8023e-05,5.7005e-05,3.4656e-05,2.5798e-05,2.0287e-05,1.8604e-05,1.4054e-05,1.4382e-05},
    {0.0026729,0.0026729,0.0026729,0.0017819,0.0013143,0.00090031,0.00065935,0.00048808,0.00035408,0.00027706,0.00022755,0.00017984,0.00012401,9.5923e-05,8.0022e-05,6.1483e-05,5.7191e-05,5.0173e-05,5.7934e-05},
    {0.0062161,0.0062161,0.0062161,0.0035962,0.0027004,0.0019682,0.0016372,0.00133,0.0012069,0.00098556,0.00091189,0.00080247,0.00066273,0.0005789,0.00046604,0.00045916,0.00043642,0.00040108,0.00039779}
};

//dose map in MRad/fb-1 for HB from FLUKA
double towerDoseHB[maxHBeta][maxHBlay] = {
    {1.6355e-05,1.6355e-05,9.3005e-06,7.8386e-06,5.3218e-06,3.7286e-06,2.3433e-06,2.2456e-06,1.4385e-06,7.6475e-07,5.147e-07,4.9808e-07,3.6321e-07,2.5022e-07,1.7331e-07,7.9523e-08,3.938e-08},
    {1.3297e-05,1.3297e-05,1.0213e-05,7.7129e-06,4.9649e-06,3.4503e-06,1.7796e-06,1.915e-06,1.2789e-06,8.1349e-07,8.9615e-07,4.8704e-07,4.3468e-07,2.898e-07,1.4381e-07,5.5894e-08,2.7655e-08},
    {1.3804e-05,1.3804e-05,8.6797e-06,6.4666e-06,4.5343e-06,2.7496e-06,2.179e-06,1.6579e-06,1.0058e-06,6.9637e-07,7.8067e-07,3.7909e-07,3.3652e-07,1.9806e-07,1.2388e-07,4.7664e-08,3.5286e-08},
    {1.67e-05,1.67e-05,1.0139e-05,7.744e-06,5.3902e-06,3.8381e-06,1.7876e-06,1.8667e-06,9.3726e-07,5.715e-07,7.0929e-07,5.0335e-07,4.998e-07,3.1018e-07,2.4823e-07,7.2041e-08,4.1091e-08},
    {1.5285e-05,1.5285e-05,8.9625e-06,7.1812e-06,4.6201e-06,3.6905e-06,2.3599e-06,1.3698e-06,7.7528e-07,6.7453e-07,6.6782e-07,4.9568e-07,3.3382e-07,2.2436e-07,1.7383e-07,6.2093e-08,3.2185e-08},
    {1.5883e-05,1.5883e-05,1.0673e-05,7.0438e-06,4.6669e-06,2.9591e-06,1.9882e-06,1.3221e-06,1.1195e-06,1.1714e-06,1.3428e-06,7.3425e-07,4.6914e-07,3.2396e-07,2.942e-07,7.0385e-07,1.4283e-07},
    {1.5789e-05,1.5789e-05,1.0698e-05,7.4321e-06,4.7226e-06,3.5423e-06,2.0213e-06,1.5694e-06,1.2718e-06,7.2686e-07,4.7942e-07,4.1108e-07,2.7368e-07,1.4023e-07,6.7817e-08,1.4965e-07,3.0542e-08},
    {1.4952e-05,1.4952e-05,8.9343e-06,7.5356e-06,4.6129e-06,2.854e-06,1.8946e-06,1.3742e-06,1.148e-06,6.0089e-07,5.0596e-07,4.9365e-07,5.0855e-07,1.9237e-07,1.526e-07,9.4725e-08,6.7407e-08},
    {1.4296e-05,1.4296e-05,8.732e-06,6.2157e-06,4.7355e-06,3.8137e-06,2.1908e-06,1.5046e-06,1.0316e-06,5.8642e-07,6.581e-07,3.8427e-07,4.0988e-07,2.025e-07,1.1963e-07,1.0334e-07,1.204e-07},
    {1.6696e-05,1.6696e-05,1.0676e-05,7.4933e-06,5.3897e-06,3.5777e-06,2.0226e-06,1.4399e-06,1.1256e-06,7.6021e-07,6.4571e-07,3.4181e-07,2.7281e-07,2.6813e-07,1.1449e-07,1.733e-07,5.6048e-08},
    {1.6739e-05,1.6739e-05,1.0447e-05,7.5977e-06,4.5686e-06,3.763e-06,2.1208e-06,1.2894e-06,1.0678e-06,9.5998e-07,5.7535e-07,6.0283e-07,4.4754e-07,1.8153e-07,1.0956e-07,8.9769e-08,4.4927e-08},
    {1.8067e-05,1.8067e-05,1.1906e-05,8.8769e-06,5.5731e-06,3.6264e-06,2.7878e-06,1.6905e-06,1.4203e-06,8.0964e-07,1.0918e-06,7.8461e-07,7.4235e-07,3.2659e-07,1.08e-07,2.7512e-07,5.0994e-08},
    {1.779e-05,1.779e-05,1.1458e-05,8.8899e-06,6.1331e-06,4.0238e-06,2.7937e-06,1.9741e-06,1.3773e-06,9.6502e-07,9.3893e-07,1.0998e-06,7.5525e-07,4.3292e-07,2.1265e-07,1.2348e-07,1.0006e-07},
    {2.0796e-05,2.0796e-05,1.3636e-05,1.0875e-05,7.7367e-06,5.7647e-06,3.6716e-06,2.9422e-06,1.9517e-06,1.6208e-06,1.7653e-06,1.0563e-06,8.632e-07,5.8755e-07,4.0049e-07,4.5635e-07,3.4428e-07},
    {4.5252e-05,4.5252e-05,2.8284e-05,1.8258e-05,1.0742e-05,9.085e-06,5.0349e-06,3.857e-06,2.6583e-06,2.5269e-06,2.1112e-06,2.1939e-06,2.2058e-06,1.8829e-06,1.8675e-06,0,0},
    {6.9612e-05,6.9612e-05,4.9857e-05,2.5465e-05,1.4104e-05,1.0868e-05,1.56e-05,1.4835e-05,1.0083e-05,0,0,0,0,0,0,0,0}
};

//---------------------------------------------------------
//function to find trends of raddam decay constants vs dose
void raddam_trend(double lumi, bool do_mars=true, bool do_laser=false, bool do_print=false){
	//HEDarkening object for darkening weights
	HEDarkening darkening(0);
	
	/*
	//axis range based on lowest and highest log(dose)
	int nbin = 50;
	TH1F* h_dose = new TH1F("h_dose","",nbin,log10(darkening.towerDose[2][16]*lumi),log10(darkening.towerDose[13][0]*lumi));
	cout << "range [Mrad]: " << darkening.towerDose[2][16]*lumi << " to " << darkening.towerDose[13][0]*lumi << endl;
	
	//make axis work for dose with SetLogx - variable binning
	TAxis *xaxis = h_dose->GetXaxis();
	int bins = xaxis->GetNbins();
	
	Double_t xmin = xaxis->GetXmin();
	Double_t xmax = xaxis->GetXmax();
	Double_t width = (xmax - xmin)/bins;
	Double_t *new_bins = new Double_t[bins + 1];
	
	for (int b = 0; b <= bins; b++) {
		new_bins[b] = TMath::Power(10, xmin + b*width);
	}
	xaxis->Set(bins, new_bins); 
	*/
	
	//graph vectors
	vector<double> xval, yval;
	vector<double> logxval, logyval;
	
	//fill vectors
	for(int j = 0; j < maxHEeta; j++){
		for(int i = 0; i <= maxHElay; i++){
			double dose = darkening.towerDose[j][i];
			if(!do_mars) dose = towerDoseHE[j][i];
		
			if(do_laser && i!=2 && i!=8) continue; //use only layers 1 and 7
			if(dose==0) continue;
			if(i<2) continue; //skip layers 0 and -1
			/*
			double L0 = darkening.lumiscale[j][i];
			int bin = h_dose->GetXaxis()->FindBin(darkening.towerDose[j][i]*lumi);
			double xbin = h_dose->GetXaxis()->GetBinCenter(bin);
			xval.push_back(xbin);
			yval.push_back(L0);
			*/
			double L0 = darkening.lumiscale[j][i];
			xval.push_back(dose*lumi);
			logxval.push_back(log10(dose*lumi));
			yval.push_back(L0);
			logyval.push_back(log10(L0));
			//cout << log10(darkening.towerDose[j][i]*lumi) << "\t" << log10(L0) << endl;
			//cout << "lay " << i << ", L0 " << L0 << ", dose " << darkening.towerDose[j][i]*lumi << endl;
		}
	}

	//make graph
	TGraph* graph = new TGraph(xval.size(),&xval[0],&yval[0]);
	TGraph* loggraph = new TGraph(logxval.size(),&logxval[0],&logyval[0]);
	//cout << "# points = " << xval.size() << endl;
	//cout << "range [Mrad]: " << graph->GetXaxis()->GetXmin() << " to " << graph->GetXaxis()->GetXmax() << endl;
	
	/*
	//power law fit
	TF1* gfit = new TF1("powerlaw","[0]*x^[1]",graph->GetXaxis()->GetXmin(),graph->GetXaxis()->GetXmax());
	//TF1* gfit = new TF1("powerlaw","[0]*x^[1]*10^([2]*log10(x)*log10(x))",graph->GetXaxis()->GetXmin(),graph->GetXaxis()->GetXmax());
	TVirtualFitter::Fitter(graph)->SetMaxIterations(10000);
	//initial values
	double x1 = darkening.towerDose[2][16]*lumi;
	double y1 = darkening.lumiscale[2][16];
	double x2 = darkening.towerDose[13][0]*lumi;
	double y2 = darkening.lumiscale[13][0];
	double b = log10(y2/y1)/log10(x2/x1);
	double a = y1/pow(x1,b);
	gfit->SetParameters(a,b);
	//formatting
	gfit->SetLineColor(kRed);
	gfit->SetMarkerColor(kRed);
	gfit->SetLineWidth(2);
	//fit
	graph->Fit(gfit,"NRM");
	*/
	
	//linear fit to log-log data
	TF1* gfit = new TF1("linear","pol1",loggraph->GetXaxis()->GetXmin(),loggraph->GetXaxis()->GetXmax());
	TVirtualFitter::Fitter(graph)->SetMaxIterations(10000);
	//initial values
	double x1 = do_mars ? darkening.towerDose[2][16]*lumi : towerDoseHE[2][16]*lumi;
	double y1 = darkening.lumiscale[2][16];
	double x2 = do_mars ? darkening.towerDose[13][0]*lumi : towerDoseHE[13][0]*lumi;
	double y2 = darkening.lumiscale[13][0];
	double b = log10(y2/y1)/log10(x2/x1);
	double a = y1/pow(x1,b);
	gfit->SetParameters(a,b);
	//fit
	loggraph->Fit(gfit,"NRM");
	
	//formatting
	TF1* gline = new TF1("powerlaw","[0]*x^[1]",graph->GetXaxis()->GetXmin(),graph->GetXaxis()->GetXmax());
	gline->SetParameters(pow(10,(gfit->GetParameter(0))),gfit->GetParameter(1));
	gline->SetLineColor(kRed);
	gline->SetMarkerColor(kRed);
	gline->SetLineWidth(2);
	
	//fit info
	stringstream fitname[4];
	fitname[0] << "#lambda(D) = aD^{b}";
	fitname[1].precision(2);
	fitname[1] << "a = " << pow(10,(gfit->GetParameter(0))) << " #pm " << gfit->GetParError(0)*log(10)*pow(10,(gfit->GetParameter(0)));
	fitname[2].precision(3);
	fitname[2] << "b = " << gfit->GetParameter(1) << " #pm " << gfit->GetParError(1);
	fitname[3].precision(2);
	//fitname[3] << "#chi^{2} / ndf = " << gfit->GetChisquare() << " / " << gfit->GetNDF();
	fitname[3] << "#chi^{2} / ndf = " << graph->Chisquare(gline) << " / " << gfit->GetNDF();
	
	//pave for fit info
	TPaveText* pave = new TPaveText(0.6,0.6,0.9,0.85,"NDC");
	pave->SetFillColor(0);
	pave->SetBorderSize(0);
	pave->SetTextFont(42);
	pave->SetTextSize(0.05);
	pave->AddText((fitname[0].str()).c_str());
	pave->AddText((fitname[1].str()).c_str());
	pave->AddText((fitname[2].str()).c_str());
	pave->AddText((fitname[3].str()).c_str());
	
	//formatting
	graph->SetTitle("");
	stringstream sx;
	if(lumi==1) sx << "D, dose " << (do_mars ? "(MARS)" : "(FLUKA)") << " [Mrad/fb^{-1}]";
	else sx << "D, dose after " << lumi << " fb^{-1} " << (do_mars ? "(MARS)" : "(FLUKA)") << " [Mrad]";
	graph->GetXaxis()->SetTitle((sx.str()).c_str());
	graph->GetYaxis()->SetTitle("#lambda, decay constant [fb^{-1}]");
	
	//drawing
	TCanvas* can = new TCanvas("raddam_trend","raddam_trend",700,500);
	TPad* pad1 = new TPad("graph","",0,0,1,1);
	pad1->SetMargin(0.14,0.05,0.175,0.05);
	pad1->SetTicks(1,1);
	pad1->SetLogy();
	pad1->SetLogx();
	pad1->Draw();
	pad1->cd();	

	//more formatting
	graph->GetYaxis()->SetTitleOffset(0.85);
	graph->GetXaxis()->SetTitleOffset(0.95);
	graph->GetYaxis()->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
	graph->GetYaxis()->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
	graph->GetXaxis()->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
	graph->GetXaxis()->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
	graph->GetXaxis()->SetLabelOffset(1/(pad1->GetWh()*pad1->GetAbsHNDC()));
	graph->GetYaxis()->SetLabelOffset(1/(pad1->GetWh()*pad1->GetAbsHNDC()));
	graph->GetYaxis()->SetTickLength(12/(pad1->GetWh()*pad1->GetAbsHNDC()));
	graph->GetXaxis()->SetTickLength(12/(pad1->GetWh()*pad1->GetAbsHNDC()));
	
	graph->Draw("APZ");
	//gfit->Draw("same");
	gline->Draw("same");
	pave->Draw("same");
	
	if(do_print){
		stringstream outname;
		outname << "raddam_trend";
		if(do_mars) outname << "_mars";
		else outname << "_fluka";
		outname << "_lumi" << lumi;
		if(do_laser) outname << "_lay1,7";
		outname << ".png";
		can->Print((outname.str()).c_str(),"png");
	}
	
	//delete extra objects
	delete loggraph;
	delete gfit;
}

//----------------------------------------------------------
//function to make plot of signal loss for HB for given lumi
//using power law extrapolation from above (w/ FLUKA)
TH2F* signal_loss_HB(double lumi, double zcut=0., bool do_show=true, bool do_print=false, bool do_tower_eta=false, bool do_warning=true){
	gStyle->SetPalette(1); //rainbow
	
	//TF1 object for darkening weights
	TF1* gline = new TF1("powerlaw","[0]*x^[1]",TMath::MinElement(maxHBeta*maxHBlay,&towerDoseHB[0][0]),TMath::MaxElement(maxHBeta*maxHBlay,&towerDoseHB[0][0]));
	gline->SetParameters(2.464836,-0.534062);
	
	TH2F* hist;
	//if(do_tower_eta) hist = new TH2F("signal_loss_HB","",16,0,0.087*16,17,-0.5,16.5);
	//else hist = new TH2F("signal_loss_HB","",16,0.5,16.5,17,-0.5,16.5);
	hist = new TH2F("signal_loss_HB","",16,0.5,16.5,17,-0.5,16.5);

	for(int j = 0; j < maxHBeta; j++){
		//fill histo
		for(int i = 0; i < maxHBlay; i++){
			double dose = towerDoseHB[j][i];
			double sigloss = 0;
			if(dose>0){
				double decay = gline->Eval(dose);
				sigloss = exp(-lumi/decay);
			}
			int bin;
			//if(do_tower_eta) bin = hist->FindBin((j+0.5)*0.087,i);
			//else bin = hist->FindBin(j+1,i);
			bin = hist->FindBin(j+1,i);
			hist->SetBinContent(bin,sigloss); //be careful of eta and layer numbering
		}
	}

	//formatting
	hist->SetTitle("");
	if(do_tower_eta) hist->GetXaxis()->SetTitle("Tower #eta");
	else hist->GetXaxis()->SetTitle("Tower");
	hist->GetYaxis()->SetTitle("Layer");
	stringstream sz;
	sz << "relative signal after " << lumi << " fb^{-1}";
	string ztitle = sz.str();
	if(!do_warning) ztitle = "#splitline{" + ztitle + "}{(extrapolation from HE data)}";
	hist->GetZaxis()->SetTitle(ztitle.c_str());
	//hist->GetZaxis()->SetRangeUser(0.,1.);
	if(zcut<0.) zcut = 0.;
	else if(zcut>1.) zcut = 1.;
	hist->GetZaxis()->SetRangeUser(zcut,1.);
	
	//drawing
	TCanvas* can = new TCanvas("signal_loss_HB","signal_loss_HB",900,600);
	TPad* pad1 = new TPad("graph","",0,0,1,1);
	if(zcut>0.) pad1->SetMargin(0.085,0.25,0.125,0.05);
	else pad1->SetMargin(0.085,0.2,0.125,0.05);
	pad1->SetTicks(1,1);
	pad1->Draw();
	pad1->cd();

	//more formatting
	hist->GetYaxis()->SetTitleOffset(0.6);
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
	
	//pave for fit info
	TPaveText* pave;
	if(do_warning){
		pave = new TPaveText(0.45,0.4,0.45,0.8,"NDC");
		pave->SetFillColor(0);
		pave->SetBorderSize(0);
		pave->SetTextFont(42);
		pave->SetTextSize(0.08);
		pave->AddText("IMPROPER EXTRAPOLATION");
		pave->AddText("FROM AIR TO N_{2} ATMO");
		pave->AddText("USING COARSE DOSE MAP");
	}
	
	if(do_show){
		
		if(do_tower_eta) {
			//modification of label size due to ROOT bug in alphanumeric labels
			//see http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=18346&p=78118
			hist->GetXaxis()->SetLabelSize(32*1.500015/(pad1->GetWh()*pad1->GetAbsHNDC()));
			hist->GetXaxis()->SetTitle("Tower #eta");
			//hist->GetYaxis()->SetTitleOffset(1.025); //for precision(3)
			//hist->GetXaxis()->SetTitleOffset(1);
			hist->GetXaxis()->SetLabelOffset(0.005);
			Double_t tower_etas[] = {0.087*0.5, 0.087*1.5, 0.087*2.5, 0.087*3.5, 0.087*4.5, 0.087*5.5,
									  0.087*6.5, 0.087*7.5, 0.087*8.5, 0.087*9.5, 0.087*10.5,
									  0.087*11.5, 0.087*12.5, 0.087*13.5, 0.087*14.5, 0.087*15.5};
			for(int j = 0; j < maxHBeta; j++){
				if(j%2) continue;
				stringstream binname;
				//binname.precision(3);
				binname.precision(2);
				//binname.precision(1);
				binname << fixed << tower_etas[j];
				//cout << "bin " << j << ": " << binname.str() << endl;
				int xbin = hist->GetXaxis()->FindBin(j+1);
				hist->GetXaxis()->SetBinLabel(xbin,(binname.str()).c_str());
			}
			hist->GetXaxis()->LabelsOption("h");
		}
		
		if(!do_warning) hist->GetZaxis()->SetTitleOffset(0.9);
	
		//gStyle->SetPaintTextFormat(".2g"); //g is 'adaptive', uses shorter of e or f
		gStyle->SetPaintTextFormat(".2f"); //g is 'adaptive', uses shorter of e or f
		hist->Draw("COLZ TEXT");
		if(do_warning) pave->Draw("same");
		
		if(zcut>0.) {
			gPad->Update();
			TPaletteAxis *palette = (TPaletteAxis*)hist->GetListOfFunctions()->FindObject("palette");
			palette->SetTitleOffset(1.2);
		}
		
		if(do_print){
			stringstream outname;
			outname << "signal_loss_HB_lumi" << lumi;
			if(zcut!=0.) outname << "_cut" << zcut;
			if(do_tower_eta) outname << "_eta";
			outname << ".png";
			can->Print((outname.str()).c_str(),"png");
		}
	}
	
	return hist;
}

//-----------------------------------------------
//function to compare MARS and FLUKA doses for HE
void dose_ratio_HE(double zcut=1., bool do_show=true, bool do_print=false, bool do_tower_eta=false){
	gStyle->SetPalette(1); //rainbow
	
	//HEDarkening object for darkening weights
	HEDarkening darkening(0);
	
	TH2F* hist = new TH2F("dose_ratio_HE","",18,-0.5,17.5,14,-29.5,-15.5);

	for(int j = 0; j < maxHEeta; j++){
		//fill histo (negative ieta)
		for(int i = 0; i <= maxHElay; i++){
			int bin = hist->FindBin(i,-(j+16));
			double mars = darkening.towerDose[j][i];
			double fluka = towerDoseHE[j][i];
			double ratio;
			if(fluka==0) ratio = 0;
			else ratio = mars/fluka;
			hist->SetBinContent(bin,ratio); //be careful of eta and layer numbering
			//hist->SetBinContent(i+1,14-j,darkening.degradation(lumi,j+16,i)); //be careful of eta and layer numbering
		}
	}

	//formatting
	hist->SetTitle("");
	hist->GetXaxis()->SetTitle("Layer");
	hist->GetYaxis()->SetTitle("Tower");
	hist->GetZaxis()->SetTitle("MARS / FLUKA (HE)");
	//hist->GetZaxis()->SetRangeUser(0.,1.);
	if(zcut<1.) zcut = 1.;
	else if(zcut>7.) zcut = 7.;
	hist->GetZaxis()->SetRangeUser(zcut,7.);
	
	//drawing
	TCanvas* can = new TCanvas("dose_ratio","dose_ratio",900,600);
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
			for(int j = 0; j < maxHEeta; j++){
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
			outname << "dose_ratio_HE";
			if(zcut!=1.) outname << "_cut" << zcut;
			if(do_tower_eta) outname << "_eta";
			outname << ".png";
			can->Print((outname.str()).c_str(),"png");
		}
	}

}

//-----------------------------------------------
//function to compare MARS and FLUKA doses for HE
void dose_ratio_1D_HE(bool do_print=false){
	//HEDarkening object for darkening weights
	HEDarkening darkening(0);
	
	TH1F* hist = new TH1F("dose_ratio_1D_HE","",35,0.,7.);

	for(int j = 0; j < maxHEeta; j++){
		//fill histo (negative ieta)
		for(int i = 0; i <= maxHElay; i++){
			double mars = darkening.towerDose[j][i];
			double fluka = towerDoseHE[j][i];
			double ratio;
			if(fluka!=0) hist->Fill(mars/fluka);
		}
	}

	//formatting
	hist->SetTitle("");
	hist->GetXaxis()->SetTitle("MARS / FLUKA (HE)");
	
	//drawing
	TCanvas* can = new TCanvas("dose_ratio_1D","dose_ratio_1D",700,500);
	TPad* pad1 = new TPad("graph","",0,0,1,1);
	pad1->SetMargin(0.12,0.05,0.15,0.05);
	pad1->SetTicks(1,1);
	pad1->Draw();
	pad1->cd();

	//more formatting
	//hist->GetYaxis()->SetTitleOffset(0.75);
	//hist->GetXaxis()->SetTitleOffset(0.85);
	hist->SetLineWidth(2);
	hist->GetYaxis()->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetYaxis()->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetXaxis()->SetTitleSize(38/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetXaxis()->SetLabelSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetYaxis()->SetTickLength(12/(pad1->GetWh()*pad1->GetAbsHNDC()));
	hist->GetXaxis()->SetTickLength(12/(pad1->GetWh()*pad1->GetAbsHNDC()));
	
	hist->Draw("hist");
		
	if(do_print){
		stringstream outname;
		outname << "dose_ratio_1D_HE.png";
		can->Print((outname.str()).c_str(),"png");
	}

}


//-----------------------------------
//function to plot doses for HE tiles
void dose_map_HE(bool do_mars=false, bool do_print=false, bool do_tower_eta=false){
	gStyle->SetPalette(1); //rainbow
	
	//HEDarkening object for darkening weights
	HEDarkening darkening(0);
	
	TH2F* hist = new TH2F("dose_map_HE","",18,-0.5,17.5,14,-29.5,-15.5);

	for(int j = 0; j < maxHEeta; j++){
		//fill histo (negative ieta)
		for(int i = 0; i <= maxHElay; i++){
			int bin = hist->FindBin(i,-(j+16));
			double dose;
			if(do_mars) dose = darkening.towerDose[j][i];
			else dose = towerDoseHE[j][i];
			hist->SetBinContent(bin,dose); //be careful of eta and layer numbering
		}
	}

	//formatting
	hist->SetTitle("");
	hist->GetXaxis()->SetTitle("Layer");
	hist->GetYaxis()->SetTitle("Tower");
	if(do_mars) hist->GetZaxis()->SetTitle("Dose (MARS) [Mrad/fb^{-1}]");
	else hist->GetZaxis()->SetTitle("Dose (FLUKA) [Mrad/fb^{-1}]");
	//hist->GetZaxis()->SetRangeUser(0.,1.);
	hist->GetZaxis()->SetRangeUser(1e-9,1e-1);
	
	//drawing
	TCanvas* can = new TCanvas("dose_map_HE","dose_map_HE",900,600);
	TPad* pad1 = new TPad("graph","",0,0,1,1);
	if(do_tower_eta) pad1->SetMargin(0.13,0.15,0.125,0.05);
	else pad1->SetMargin(0.1,0.175,0.125,0.05);
	pad1->SetTicks(1,1);
	pad1->SetLogz();
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
		for(int j = 0; j < maxHEeta; j++){
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
	gStyle->SetPaintTextFormat(".0e"); //g is 'adaptive', uses shorter of e or f
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
		outname << "dose_map_HE";
		if(do_mars) outname << "_mars";
		else outname << "_fluka";
		if(do_tower_eta) outname << "_eta";
		outname << ".png";
		can->Print((outname.str()).c_str(),"png");
	}

}

//-----------------------------------------
//function to plot doses for HB tiles
//(sadly only FLUKA is available currently)
void dose_map_HB(bool do_mars=false, bool do_print=false, bool do_tower_eta=false){
	if(do_mars){
		cout << "No MARS dose map available for HB." << endl;
		return;
	}
	
	gStyle->SetPalette(1); //rainbow
	
	TH2F* hist;
	if(do_tower_eta) hist = new TH2F("dose_map_HB","",16,0,0.087*16,17,-0.5,16.5);
	else hist = new TH2F("dose_map_HB","",16,0.5,16.5,17,-0.5,16.5);

	for(int j = 0; j < maxHBeta; j++){
		//fill histo
		for(int i = 0; i < maxHBlay; i++){
			double dose = towerDoseHB[j][i];
			int bin;
			if(do_tower_eta) bin = hist->FindBin((j+0.5)*0.087,i);
			else bin = hist->FindBin(j+1,i);
			hist->SetBinContent(bin,dose); //be careful of eta and layer numbering
		}
	}

	//formatting
	hist->SetTitle("");
	if(do_tower_eta) hist->GetXaxis()->SetTitle("Tower #eta");
	else hist->GetXaxis()->SetTitle("Tower");
	hist->GetYaxis()->SetTitle("Layer");
	if(do_mars) hist->GetZaxis()->SetTitle("Dose (MARS) [Mrad/fb^{-1}]");
	else hist->GetZaxis()->SetTitle("Dose (FLUKA) [Mrad/fb^{-1}]");
	hist->GetZaxis()->SetRangeUser(1e-9,1e-1);
	
	//drawing
	TCanvas* can = new TCanvas("dose_map_HB","dose_map_HB",900,600);
	TPad* pad1 = new TPad("graph","",0,0,1,1);
	pad1->SetMargin(0.1,0.175,0.125,0.05);
	pad1->SetTicks(1,1);
	pad1->SetLogz();
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
	
	/*
	if(do_tower_eta) {
		//modification of label size due to ROOT bug in alphanumeric labels
		//see http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=18346&p=78118
		hist->GetXaxis()->SetLabelSize(32*1.500015/(pad1->GetWh()*pad1->GetAbsHNDC()));
		hist->GetXaxis()->SetTitle("Tower #eta");
		hist->GetZaxis()->SetTitleOffset(0.72);
		//hist->GetYaxis()->SetTitleOffset(1.025); //for precision(3)
		hist->GetXaxis()->SetTitleOffset(1);
		hist->GetXaxis()->SetLabelOffset(0.005);
		Double_t tower_etas[] = {0.087*0.5, 0.087*1.5, 0.087*2.5, 0.087*3.5, 0.087*4.5, 0.087*5.5,
								  0.087*6.5, 0.087*7.5, 0.087*8.5, 0.087*9.5, 0.087*10.5,
								  0.087*11.5, 0.087*12.5, 0.087*13.5, 0.087*14.5, 0.087*15.5};
		for(int j = 0; j < maxHBeta; j++){
			//if(j%2) continue;
			stringstream binname;
			//binname.precision(3);
			binname.precision(2);
			binname << fixed << tower_etas[j];
			//cout << "bin " << j << ": " << binname.str() << endl;
			int xbin = hist->GetXaxis()->FindBin(j+1);
			hist->GetXaxis()->SetBinLabel(xbin,(binname.str()).c_str());
		}
	}
	*/

	//gStyle->SetPaintTextFormat(".2g"); //g is 'adaptive', uses shorter of e or f
	gStyle->SetPaintTextFormat(".0e"); //g is 'adaptive', uses shorter of e or f
	hist->Draw("COLZ TEXT");
	
	if(do_print){
		stringstream outname;
		outname << "dose_map_HB";
		if(do_mars) outname << "_mars";
		else outname << "_fluka";
		if(do_tower_eta) outname << "_eta";
		outname << ".png";
		can->Print((outname.str()).c_str(),"png");
	}

}

//get decay constants for each tower and layer of HB
void getModelHB(){
	//TF1 object for darkening weights
	TF1* gline = new TF1("powerlaw","[0]*x^[1]",TMath::MinElement(maxHBeta*maxHBlay,&towerDoseHB[0][0]),TMath::MaxElement(maxHBeta*maxHBlay,&towerDoseHB[0][0]));
	gline->SetParameters(2.464836,-0.534062);
	
	//output decay constants
	string oname = "lumiscale_HB.txt";
	ofstream output(oname.c_str());
	if (!output) {
		std::cerr << "Cannot open the output file " << oname << "\n";
		return;
	}
	output << std::fixed;
	output << std::setprecision(1);
	output << "//scale parameter in fb-1 for exponential darkening from radiation damage for each tile" << endl;
	output << "//extrapolated from HB model" << endl;
	output << "double _lumiscale[" << maxHBeta << "][" << maxHBlay << "] = {" << endl;
	for(int ieta = 0; ieta < maxHBeta; ieta++){
		output << "    {";
		for(int lay = 0; lay < maxHBlay; lay++){
			double dose = towerDoseHB[ieta][lay];
			double decay = 0;
			if(dose>0){
				decay = gline->Eval(dose);
			}
			output << decay;
			
			if(lay<maxHBlay-1) output << ",";
		}
		output << "}";
		if(ieta<maxHBeta-1) output << ",";
		output << endl;
	}
	output << "};";
	
	output.close();

}

//--------------------------------------------
//function to plot decay constans for HE tiles
//(as fb-1 or Mrad)
void decay_constants_HE(bool do_mrad=true, bool do_mars=false, bool do_print=false, bool do_tower_eta=false){
	gStyle->SetPalette(1); //rainbow
	
	//HEDarkening object for darkening weights
	HEDarkening darkening(0);
	
	TH2F* hist = new TH2F("decay_constants_HE","",18,-0.5,17.5,14,-29.5,-15.5);

	for(int j = 0; j < maxHEeta; j++){
		//fill histo (negative ieta)
		for(int i = 0; i <= maxHElay; i++){
			int bin = hist->FindBin(i,-(j+16));
			double dose;
			if(do_mars) dose = darkening.towerDose[j][i];
			else dose = towerDoseHE[j][i];
			if(do_mrad) dose *= darkening.lumiscale[j][i];
			else dose = (dose==0) ? 0.0 : darkening.lumiscale[j][i];
			hist->SetBinContent(bin,dose); //be careful of eta and layer numbering
		}
	}

	//formatting
	hist->SetTitle("");
	hist->GetXaxis()->SetTitle("Layer");
	hist->GetYaxis()->SetTitle("Tower");
	if(!do_mrad) hist->GetZaxis()->SetTitle("Decay Constant [fb^{-1}]");
	else if(do_mars) hist->GetZaxis()->SetTitle("Decay Constant (MARS) [Mrad]");
	else hist->GetZaxis()->SetTitle("Decay Constant (FLUKA) [Mrad]");
	//hist->GetZaxis()->SetRangeUser(0.,1.);
	//hist->GetZaxis()->SetRangeUser(1e-9,1e-1);
	
	//drawing
	TCanvas* can = new TCanvas("decay_constants_HE","decay_constants_HE",900,600);
	TPad* pad1 = new TPad("graph","",0,0,1,1);
	if(do_tower_eta) pad1->SetMargin(0.13,0.15,0.125,0.05);
	else pad1->SetMargin(0.1,0.175,0.125,0.05);
	pad1->SetTicks(1,1);
	pad1->SetLogz();
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
		for(int j = 0; j < maxHEeta; j++){
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
	gStyle->SetPaintTextFormat(".2g"); //g is 'adaptive', uses shorter of e or f
	//gStyle->SetPaintTextFormat(".0e"); //g is 'adaptive', uses shorter of e or f
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
		outname << "decay_constants_HE";
		if(do_mrad) outname << "_mrad";
		else outname << "_fb-1";
		if(do_mars) outname << "_mars";
		else outname << "_fluka";
		if(do_tower_eta) outname << "_eta";
		outname << ".png";
		can->Print((outname.str()).c_str(),"png");
	}

}