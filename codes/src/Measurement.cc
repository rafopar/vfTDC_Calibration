/* 
 * File:   Measurement.cc
 * Author: rafopar
 * 
 * Created on May 10, 2022, 3:58 PM
 */

#include <vector>
#include <iostream>
#include <bits/stdc++.h>
#include "Measurement.h"

#include <TMath.h>
using namespace std;

SLCO::SLCO(int asec, int alayer, int acomp, int aorder) : sector(asec), layer(alayer), component(acomp), order(aorder) {

}

bool SLCO::operator<(const SLCO& arg) const {

    if (this->sector == arg.sector) {

        if (this->layer == arg.layer) {

            if (this->component == arg.component) {

                return this->order < arg.order;

            } else {
                return this->component < arg.component;
            }

        } else {
            return this->layer < arg.layer;
        }

    } else {
        return this->sector < arg.sector;
    }
}

bool SLCO::operator==(const SLCO& arg) const {
    return this->sector == arg.sector && this->layer == arg.layer && this->component == arg.component && this->order == arg.order;
}

Measurement::Measurement(std::vector<tdcHit*> &v_Hits) {
    f_v_Hits = v_Hits;
    fNmeasurements = f_v_Hits.size();

}

Measurement::Measurement(const Measurement& orig) {
}

Measurement::~Measurement() {
}

//____________________________________________________________________________

DoubleMeasure::DoubleMeasure(std::vector<tdcHit*> &v_Hits) : Measurement(v_Hits) {
    if (fNmeasurements != 2) {
        std::cout << "This is a double measurement, but number of hits in the measurement is " << fNmeasurements << std::endl;
        std::cout << "Exiting..." << std::endl;
    }

    AnalyzeMeasurement();
}

void DoubleMeasure::AnalyzeMeasurement() {

    int firstInd = f_v_Hits.at(0)->interval < f_v_Hits.at(1)->interval ? 0 : 1;
    int secondInd = f_v_Hits.at(0)->interval < f_v_Hits.at(1)->interval ? 1 : 0;

    f1stInterval = f_v_Hits.at(firstInd)->interval;
    f1stTDCBin = f_v_Hits.at(firstInd)->tdcBin;
    f1stRawTDC = f_v_Hits.at(firstInd)->rawTDC;

    f2ndInterval = f_v_Hits.at(secondInd)->interval;
    f2ndTDCBin = f_v_Hits.at(secondInd)->tdcBin;
    f2ndRawTDC = f_v_Hits.at(secondInd)->rawTDC;
}

int DoubleMeasure::Get1stInterval() {
    return f1stInterval;
}

int DoubleMeasure::Get1stTDCBin() {
    return f1stTDCBin;
}

int DoubleMeasure::Get1stRawTDC() {
    return f1stRawTDC;
}

int DoubleMeasure::Get2ndInterval() {
    return f2ndInterval;
}

int DoubleMeasure::Get2ndTDCBin() {
    return f2ndTDCBin;
}

int DoubleMeasure::Get2ndRawTDC() {
    return f2ndRawTDC;
}


//____________________________________________________________________________

TripleMeasure::TripleMeasure(std::vector<tdcHit*> &v_Hits) : Measurement(v_Hits) {
    if (fNmeasurements != 3) {
        std::cout << "This is a triple measurement, but number of hits in the measurement is " << fNmeasurements << std::endl;
        std::cout << "Exiting..." << std::endl;
    }

    AnalyzeMeasurement();
}

void TripleMeasure::AnalyzeMeasurement() {

    sort(f_v_Hits.begin(), f_v_Hits.end(), vfTDCFuncs::CompareInterval);

    f1stInterval = f_v_Hits.at(0)->interval;
    f1stTDCBin = f_v_Hits.at(0)->tdcBin;
    f1stRawTDC = f_v_Hits.at(0)->rawTDC;
    f2ndInterval = f_v_Hits.at(1)->interval;
    f2ndTDCBin = f_v_Hits.at(1)->tdcBin;
    f2ndRawTDC = f_v_Hits.at(1)->rawTDC;
    f3rdInterval = f_v_Hits.at(2)->interval;
    f3rdTDCBin = f_v_Hits.at(2)->tdcBin;
    f3rdRawTDC = f_v_Hits.at(2)->rawTDC;
}

bool vfTDCFuncs::CompareInterval(tdcHit* h1, tdcHit* h2) {
    return (h1->interval < h2->interval);
}

void vfTDCFuncs::GetOrganizedHits(vector<tdcHit>& v_allHits, vector< vector<tdcHit*> > &v_organizedHits) {

    vector<tdcHit*> v_allHitsptr;

    for (int i = 0; i < v_allHits.size(); i++) {
        tdcHit* curHit = &v_allHits.at(i);
        v_allHitsptr.push_back(curHit);
        //cout << v_allHits.at(i).interval << "   ";
    }
    //cout << endl;

    vector<tdcHit*>::iterator it1;
    vector<tdcHit*>::iterator it2;

    for (it1 = v_allHitsptr.begin(); it1 != v_allHitsptr.end(); it1++) {
        vector<tdcHit*> v_curHits;
        v_curHits.push_back(*it1);

        //cout<<(*it1)->interval<<"   ";

        for (it2 = it1 + 1; it2 != v_allHitsptr.end(); it2++) {

            //cout<<(*it1)->interval<<"  ::  "<<(*it2)->interval<<endl;

            if (TMath::Abs((*it1)->interval - (*it2)->interval) < 3) {
                v_curHits.push_back(*it2);
                v_allHitsptr.erase(it2);
                it2--;
            }

        }

        v_organizedHits.push_back(v_curHits);
    }

}

double vfTDCFuncs::GetCalibratedTime(std::vector<double>& v_LUT, std::vector<double>& v_LUTErr, std::vector<tdcHit*>& v_hits) {

    if (v_hits.size() >= 2) {
        double t_1 = v_LUT.at(v_hits.at(0)->tdcBin);
        double t_2 = v_LUT.at(v_hits.at(1)->tdcBin);

        double value_1_4ns = v_hits.at(0)->rawTDC / 256;
        double value_2_4ns = v_hits.at(1)->rawTDC / 256;

        double value_1_2ns = v_hits.at(0)->interval % 2;
        double value_2_2ns = v_hits.at(1)->interval % 2;

        double time_1 = 4000 * value_1_4ns + 2000 * value_1_2ns + t_1;
        double time_2 = 4000 * value_2_4ns + 2000 * value_2_2ns + t_2;

        double time_1_err = v_LUTErr.at(v_hits.at(0)->tdcBin);
        double time_2_err = v_LUTErr.at(v_hits.at(1)->tdcBin);

        return (time_1 * time_2_err * time_2_err + time_2 * time_1_err * time_1_err) / (time_1_err * time_1_err + time_2_err * time_2_err);
    }else{
        double t_1 = v_LUT.at(v_hits.at(0)->tdcBin);
        double value_1_4ns = v_hits.at(0)->rawTDC / 256;
        double value_1_2ns = v_hits.at(0)->interval % 2;
        return 4000 * value_1_4ns + 2000 * value_1_2ns + t_1;
    }
}

std::vector <double> vfTDCFuncs::GetLUT(TH1D* h_tdcBin, int n_2Hit, int n_3Hit) {
    vector<double> v_LUT;

    TH1D *h_Cumul = (TH1D*) h_tdcBin->GetCumulative("h_Cumul");
    double t_Max = vfTDCFuncs::cycle4ns * (1 + double(n_3Hit) / (2. * double(n_2Hit + n_3Hit)));
    h_Cumul->Scale(t_Max / h_Cumul->GetMaximum());

    for (int i = 0; i < h_Cumul->GetNbinsX(); i++) {
        h_Cumul->SetBinContent(0, 0.);
        v_LUT.push_back(0.5 * (h_Cumul->GetBinContent(i) + h_Cumul->GetBinContent(i + 1)));
    }
    return v_LUT;
}

vector <double> vfTDCFuncs::GetLUTErr(TH1D* h_tdcBin, int n_2Hit, int n_3Hit) {

    vector<double> v_LUTErr;

    TH1D *h_Cumul = (TH1D*) h_tdcBin->GetCumulative("h_Cumul");
    //double t_Max = 4000*(1 + double(n_3Hit)/double(n_2Hit) );  // William mentions that this might be not correct
    double t_Max = vfTDCFuncs::cycle4ns * (1 + double(n_3Hit) / (2. * double(n_2Hit + n_3Hit))); // Instead he suggested to use this one
    h_Cumul->Scale(t_Max / h_Cumul->GetMaximum());

    for (int i = 0; i < h_Cumul->GetNbinsX(); i++) {
        h_Cumul->SetBinContent(0, 0.);
        v_LUTErr.push_back(0.5 * (h_Cumul->GetBinContent(i + 1) - h_Cumul->GetBinContent(i)));
    }
    return v_LUTErr;
}

int TripleMeasure::Get1stInterval() {
    return f1stInterval;
}

int TripleMeasure::Get1stTDCBin() {
    return f1stTDCBin;
}

int TripleMeasure::Get1stRawTDC() {
    return f1stRawTDC;
}

int TripleMeasure::Get2ndInterval() {
    return f2ndInterval;
}

int TripleMeasure::Get2ndTDCBin() {
    return f2ndTDCBin;
}

int TripleMeasure::Get2ndRawTDC() {
    return f2ndRawTDC;
}

int TripleMeasure::Get3rdInterval() {
    return f3rdInterval;
}

int TripleMeasure::Get3rdTDCBin() {
    return f3rdTDCBin;
}

int TripleMeasure::Get3rdRawTDC() {
    return f3rdRawTDC;
}