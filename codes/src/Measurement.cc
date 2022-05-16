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

using namespace std;

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
    
    sort( f_v_Hits.begin(), f_v_Hits.end(), vfTDCFuncs::CompareInterval );
    
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


bool vfTDCFuncs::CompareInterval( tdcHit* h1, tdcHit* h2){
  return (h1->interval < h2->interval);
}

int TripleMeasure::Get1stInterval(){
    return f1stInterval;
}
int TripleMeasure::Get1stTDCBin(){
    return f1stTDCBin;
}
int TripleMeasure::Get1stRawTDC(){
    return f1stRawTDC;
}
int TripleMeasure::Get2ndInterval(){
    return f2ndInterval;
}
int TripleMeasure::Get2ndTDCBin(){
    return f2ndTDCBin;
}
int TripleMeasure::Get2ndRawTDC(){
    return f2ndRawTDC;
}
int TripleMeasure::Get3rdInterval(){
    return f3rdInterval;
}
int TripleMeasure::Get3rdTDCBin(){
    return f3rdTDCBin;
}
int TripleMeasure::Get3rdRawTDC(){
    return f3rdRawTDC;
}