/* 
 * File:   CalibDoubleMeasure.cc
 * Author: rafopar
 * 
 * Created on May 13, 2022, 4:13 PM
 */

#include <iostream>
#include "CalibDoubleMeasure.h"


const double CalibDoubleMeasure::clockCycle = 2000.;

CalibDoubleMeasure::CalibDoubleMeasure(TH2D *h_12, TH2D *h_13) {
    fh_tdcBins_12 = *h_12;
    fh_tdcBins_13 = *h_13; // We need the h_13 only to determine the t_max

    fh_tdcBin1 = fh_tdcBins_12.ProjectionX("fh_tdcBin1", 1, fh_tdcBins_12.GetNbinsY());
    fh_tdcBin1->SetTitle("fh_tdcBin1");

    fh_tdcBin2 = fh_tdcBins_12.ProjectionY("fh_tdcBin2", 1, fh_tdcBins_12.GetNbinsX());
    fh_tdcBin2->SetTitle("fh_tdcBin2");

    fh_Cumulative_t1 = (TH1D*) fh_tdcBin1->GetCumulative("fh_Cumulative_t1");
    fh_Cumulative_t1->SetTitle("fh_Cumulative_t1");

    fh_Cumulative_t2 = (TH1D*) fh_tdcBin2->GetCumulative("fh_Cumulative_t1");
    fh_Cumulative_t2->SetTitle("fh_Cumulative_t2");

    // ================= Let's calculate the tMax ===================
    ftMax = clockCycle * (1 + h_13->GetEntries() / h_12->GetEntries());

    fh_Cumulative_t1->Scale(ftMax / fh_Cumulative_t1->GetMaximum());
    fh_Cumulative_t2->Scale(ftMax / fh_Cumulative_t2->GetMaximum());

    for (int i = 0; i < fh_Cumulative_t1->GetNbinsX(); i++) {
        if (i == 0) {
            f_LUT_1.push_back((0 + fh_Cumulative_t1->GetBinContent(i + 1)) / 2.);
            f_LUTErr_1.push_back(fh_Cumulative_t1->GetBinContent(i + 1) != 0 ? 0.5 * (fh_Cumulative_t1->GetBinContent(i + 1) - 0) : 10000);
        } else {
            f_LUT_1.push_back((fh_Cumulative_t1->GetBinContent(i) + fh_Cumulative_t1->GetBinContent(i + 1)) / 2.);
            f_LUTErr_1.push_back(fh_Cumulative_t1->GetBinContent(i + 1) != 0 ? 0.5 * (fh_Cumulative_t1->GetBinContent(i + 1) - fh_Cumulative_t1->GetBinContent(i)) : 10000);
        }

    }

    for (int i = 0; i < fh_Cumulative_t2->GetNbinsX(); i++) {
        if (i == 0) {
            f_LUT_2.push_back((0 + fh_Cumulative_t2->GetBinContent(i + 1)) / 2.);
            f_LUTErr_2.push_back(fh_Cumulative_t2->GetBinContent(i + 1) != 0 ? 0.5 * (fh_Cumulative_t2->GetBinContent(i + 1) - 0) : 10000);
        } else {
            f_LUT_2.push_back((fh_Cumulative_t2->GetBinContent(i) + fh_Cumulative_t2->GetBinContent(i + 1)) / 2.);
            f_LUTErr_2.push_back(fh_Cumulative_t2->GetBinContent(i + 1) != 0 ? 0.5 * (fh_Cumulative_t2->GetBinContent(i + 1) - fh_Cumulative_t2->GetBinContent(i)) : 10000);
        }

    }

}

double CalibDoubleMeasure::GetTime(int tbin1, int tbin2){

    if( tbin1 < 0 || tbin1 >= f_LUT_1.size() || tbin2 < 0 || tbin2 >= f_LUT_2.size() ){
        std::cout<<"Either tbin1 or tbin2 are out of range. Exiting"<<std::endl;
        exit(1);
    }
    
    double t1 = f_LUT_1.at(tbin1);
    double s1 = f_LUTErr_1.at(tbin1);
    double t2 = f_LUT_2.at(tbin2);
    double s2 = f_LUTErr_2.at(tbin2);
    
    return ( t1*s2*s2 + t2*s1*s1 )/(s1*s1 + s2*s2);
}

double CalibDoubleMeasure::GetT1Time(int tbin){
    if( tbin < 0 || tbin >= f_LUT_1.size() ){
        std::cout<<" tbin1 is out of range. Exiting"<<std::endl;
        exit(1);
    }
    
    return f_LUT_1.at(tbin);
}

double CalibDoubleMeasure::GetT2Time(int tbin){
    if( tbin < 0 || tbin >= f_LUT_2.size() ){
        std::cout<<" tbin2 is out of range. Exiting"<<std::endl;
        exit(1);
    }
    
    return f_LUT_2.at(tbin);
}

double CalibDoubleMeasure::GetT1Sigma(int tbin){
    if( tbin < 0 || tbin >= f_LUT_1.size() ){
        std::cout<<" tbin1 is out of range. Exiting"<<std::endl;
        exit(1);
    }
    
    return f_LUTErr_1.at(tbin);
}

double CalibDoubleMeasure::GetT2Sigma(int tbin){
    if( tbin < 0 || tbin >= f_LUT_2.size() ){
        std::cout<<" tbin2 is out of range. Exiting"<<std::endl;
        exit(1);
    }
    
    return f_LUTErr_2.at(tbin);
}


CalibDoubleMeasure::CalibDoubleMeasure(const CalibDoubleMeasure& orig) {
}

TH1D* CalibDoubleMeasure::GetTDCbins1() {
    return fh_tdcBin1;
}

TH1D* CalibDoubleMeasure::GetTDCbins2() {
    return fh_tdcBin2;
}

TH1D* CalibDoubleMeasure::GetCumulative1() {
    return fh_Cumulative_t1;
}

TH1D* CalibDoubleMeasure::GetCumulative2() {
    return fh_Cumulative_t2;
}

CalibDoubleMeasure::~CalibDoubleMeasure() {
    delete fh_tdcBin1;
    delete fh_tdcBin2;
    delete fh_Cumulative_t1;
    delete fh_Cumulative_t2;
}