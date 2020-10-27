#include "Riostream.h"
#include <string>
#include <stdlib.h>
#include <iostream> 
#include <cstdio>
#include <cstdlib>
#include <math.h>

void lambdaFit()
{ // begin macro
  
  double x[9] = {1.4, 1.4, 9.48, 9.48, 429, 429, 1100, 1100, 1.4}; // krad/hr (dose rate)
  double dos[9] = {1680, 1680, 2950, 2950, 2950, 2950, 30000, 30000, 1680}; // krad (total dose)
  double y[9] = {5.91, 4.26, 7.99, 9.77, 14.95, 14.28, 20.23, 21.18, 7.28}; // decay constant
  double errY[9] = {1.66, 0.92, 0.58, 0.87, 2.26, 1.82, 1.21, 1.13, 2.06}; // error on decay constant

  double logx[9], logy[9], logerr[9];
  for (int i = 0; i < 9; i++)
    {
      logx[i] = log(x[i]);
      logy[i] = log(y[i]);
      logerr[i] = 0.434*log(errY[i])/y[i];
    }

  TCanvas *canv = new TCanvas("canvas", "canvas", 500, 500);
  canv->cd();
  //canv->SetLogy();
  //canv->SetLogx();
  canv->SetRightMargin(0.12);
  canv->SetTopMargin(0.12);
  //TGraphErrors *graph = new TGraphErrors(9, x, y, 0, errY);
  TGraphErrors *graph = new TGraphErrors(9, logx, logy, 0, logerr);
  TF1 *lambda = new TF1("lambda", "[0]+[1]*x", 0, 1100);

  graph->SetTitle("Decay constant vs dose rate");
  graph->GetXaxis()->SetTitle("Log dose rate (krad)");
  graph->GetYaxis()->SetTitle("Log decay constant D");
  graph->Draw("ap");
  graph->Fit("lambda");
  cout << "Fitted slope = " << graph->GetFunction("lambda")->GetParameter(1) << endl;
  cout << "Fitted intercept = " << graph->GetFunction("lambda")->GetParameter(0) << endl;

} // end macro
