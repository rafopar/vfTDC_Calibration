/* 
 * File:   CalcLUT.cc
 * Author: rafopar
 *
 * Created on April 19, 2022, 10:01 PM
 */

/**
 * LUT stands for LookUp Table
 */

#include <cstdlib>
#include <iostream>

#include <TF1.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TGraph.h>
#include <TCanvas.h>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    int run;

    if (argc >= 2) {
        run = atoi(argv[1]);
    } else {
        cout << "Please specify the run number " << endl;
    }

    TCanvas *c1 = new TCanvas("c1", "", 950, 950);
    c1->SetLeftMargin(0.15);

    TFile file_in(Form("AnalyzeData1_%d_out.root", run));

    TH1D *h_intervDiffMultiHit1 = (TH1D*) file_in.Get("h_intervDiffMultiHit1");

    TH2D *h_tdcBin_1vs2_1 = (TH2D*)file_in.Get("h_tdcBin_1vs2_1");
    int b_x1 = h_tdcBin_1vs2_1->GetNbinsX()/2;
    int b_x2 = h_tdcBin_1vs2_1->GetNbinsX();
    int b_y1 = 1;
    int b_y2 = h_tdcBin_1vs2_1->GetNbinsY()/2;
    
    int nDoubleHits = h_tdcBin_1vs2_1->Integral(b_x1, b_x2, b_y1, b_y2);
    
    TH1D *h_n_vfTDC_leadEdge1 = (TH1D*) file_in.Get("h_n_vfTDC_leadEdge1");

    int bin_1InterDiff = h_intervDiffMultiHit1->FindBin(1); // The bin number, when the iterval difference is 1
    //int nDoubleHits = h_intervDiffMultiHit1->GetBinContent(bin_1InterDiff); // The number of events with double hits, when hits are in sequential intervals

    int binSingleHits = h_n_vfTDC_leadEdge1->FindBin(1);
    int nSingleHits = h_n_vfTDC_leadEdge1->GetBinContent(binSingleHits);

    const double interval_width = 2000.; // ns
    const double t_max = interval_width + interval_width * double(nDoubleHits) / double(nSingleHits);
    cout << "tMax is " << t_max << endl;

    TH1D *h_vftdcbin_odd1 = (TH1D*) file_in.Get("h_vftdcbin_odd1");
    TH1D *h_vftdcbin_even1 = (TH1D*) file_in.Get("h_vftdcbin_even1");

    h_vftdcbin_odd1->SetTitle("; TDC Bin; counts");
    h_vftdcbin_odd1->SetFillColor(4);
    h_vftdcbin_odd1->Draw();
    c1->Print(Form("Figs/tdcBin_Odd_Distr_%d.pdf", run));
    c1->Print(Form("Figs/tdcBin_Odd_Distr_%d.png", run));
    c1->Print(Form("Figs/tdcBin_Odd_Distr_%d.root", run));
    h_vftdcbin_even1->SetTitle("; TDC Bin; counts");
    h_vftdcbin_even1->SetFillColor(4);
    h_vftdcbin_even1->Draw();
    c1->Print(Form("Figs/tdcBin_Even_Distr_%d.pdf", run));
    c1->Print(Form("Figs/tdcBin_Even_Distr_%d.png", run));
    c1->Print(Form("Figs/tdcBin_Even_Distr_%d.root", run));

    TFile file_tmp("tmp.root", "Recreate");
    h_vftdcbin_odd1->Write();
    file_tmp.Close();

    TH1D *h_cumulative_odd1 = (TH1D*) h_vftdcbin_odd1->GetCumulative();
    h_cumulative_odd1->SetTitle("; tdc bin; time [ps]");

    h_cumulative_odd1->Scale(t_max / h_cumulative_odd1->GetMaximum());
    h_cumulative_odd1->SetStats(0);
    h_cumulative_odd1->Draw();
    const int MaxTDC = 63;
    int maxTDCBin = h_cumulative_odd1->FindBin(MaxTDC);
    //    double y2 = 0.5*( h_cumulative_odd1->GetBinContent(maxTDCBin - 1) + h_cumulative_odd1->GetBinContent(maxTDCBin) );
    //    double y1 = 0.5*h_cumulative_odd1->GetBinContent(1);
    //    double x1 = 0;
    //    double x2 = MaxTDC;

    double x1 = h_cumulative_odd1->GetBinLowEdge(1);
    double x2 = h_cumulative_odd1->GetBinLowEdge(maxTDCBin) + h_cumulative_odd1->GetBinWidth(maxTDCBin);
    double y1 = 0.;
    double y2 = h_cumulative_odd1->GetMaximum();

    double offset = y2 / (2. * MaxTDC);
    double slope = (y2 - y1) / (x2 - x1);

    cout << "The ideal slope is " << slope << endl;

    TF1 *f_lin1 = new TF1("f_lin1", "[0] + [1]*x", -2., 65);
    f_lin1->SetParameters(offset, slope);
    f_lin1->Draw("Same");

    c1->Print(Form("Figs/odd_Cumulative_%d.pdf", run));
    c1->Print(Form("Figs/odd_Cumulative_%d.png", run));
    c1->Print(Form("Figs/odd_Cumulative_%d.root", run));

    TH1D *h_LUT_odd1 = (TH1D*) h_cumulative_odd1->Clone("h_LUT_odd1");
    h_LUT_odd1->SetTitle("; TDC bin; correction [ps]");
    h_LUT_odd1->Reset();
    for (int i = 0; i < h_LUT_odd1->GetNbinsX(); i++) {
        double x = h_LUT_odd1->GetBinCenter(i + 1);
        double funcVal = f_lin1->Eval(x);
        double histVal = 0.5 * (h_cumulative_odd1->GetBinContent(i) + h_cumulative_odd1->GetBinContent(i + 1));
        histVal = i > 0 ? histVal : 0.5 * h_cumulative_odd1->GetBinContent(1);
        h_LUT_odd1->SetBinContent(i + 1, histVal - funcVal);
    }

    h_LUT_odd1->Draw();
    c1->Print(Form("Figs/LUT_odd1_%d.pdf", run));
    c1->Print(Form("Figs/LUT_odd1_%d.png", run));
    c1->Print(Form("Figs/LUT_odd1_%d.root", run));
    
    h_vftdcbin_odd1->Scale(t_max/h_vftdcbin_odd1->Integral() );
    h_vftdcbin_odd1->Draw("hist");
    c1->Print(Form("Figs/binWidths_odd1_%d.pdf", run));
    c1->Print(Form("Figs/binWidths_odd1_%d.png", run));
    c1->Print(Form("Figs/binWidths_odd1_%d.root", run));

    
    
    TGraph *gr_LUT = new TGraph();
    gr_LUT->SetMarkerColor(4);
    gr_LUT->SetMarkerStyle(20);
    gr_LUT->SetMinimum(0);
    gr_LUT->SetTitle("; tdc bin; time [ps]");
    
    for( int i = 0; i < h_cumulative_odd1->GetNbinsX(); i++ ){
        
        double x = i+1;
        double y = 0.5*( h_cumulative_odd1->GetBinContent(i) + h_cumulative_odd1->GetBinContent(i+1) );
        y = i==0 ? 0.5*( h_cumulative_odd1->GetBinContent(i+1) ) : y;
        
        gr_LUT->SetPoint(i, x, y);       
    }
    
    gr_LUT->Draw("AP");
    c1->Print(Form("Figs/gr_LUT_odd_%d.pdf", run));
    c1->Print(Form("Figs/gr_LUT_odd_%d.png", run));
    c1->Print(Form("Figs/gr_LUT_odd_%d.root", run));
    
    return 0;
}

