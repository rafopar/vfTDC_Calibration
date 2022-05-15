/* 
 * File:   TestDoubleCalib.cc
 * Author: rafopar
 *
 * Created on May 13, 2022, 10:53 PM
 */

#include <iostream>
#include <cstdlib>

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

#include <CalibDoubleMeasure.h>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    int run;

    char inputFile[256];
    if (argc >= 2) {
        //sprintf(outputFile,"%s",argv[2]);
        run = atoi(argv[1]);
        sprintf(inputFile, "Analyze_DoubleMeasurement_%d.root", run);
    } else {
        std::cout << " *** please provide the run number..." << std::endl;
        std::cout << "Exiting" << endl;
        exit(0);
    }
    
    
    TFile file_in(inputFile, "Read");
    TFile *file_out = new TFile(Form("TestCalib_out_%d.root", run), "Recreate");
    
    TH2D *h_tdc1_vs_tdc2_DoubleMeasure1 = (TH2D*)file_in.Get("h_tdc1_vs_tdc2_DoubleMeasure1");
    TH2D *h_tdc1_vs_tdc2_TripleMeasure1 = (TH2D*)file_in.Get("h_tdc1_vs_tdc2_TripleMeasure1");
    
    CalibDoubleMeasure clibDouble(h_tdc1_vs_tdc2_DoubleMeasure1, h_tdc1_vs_tdc2_TripleMeasure1);
    
    TH1D *h_tdcBin1 = (TH1D*)(clibDouble.GetTDCbins1())->Clone("h_tdcBin1");
    
    gDirectory->Write();

    return 0;
}

