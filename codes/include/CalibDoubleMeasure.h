/* 
 * File:   CalibDoubleMeasure.h
 * Author: rafopar
 *
 * Created on May 13, 2022, 4:13 PM
 */

#ifndef CALIBDOUBLEMEASURE_H
#define CALIBDOUBLEMEASURE_H

#include <vector>
#include <TH1D.h>
#include <TH2D.h>

class CalibDoubleMeasure {
public:
    CalibDoubleMeasure(TH2D*, TH2D*);
    CalibDoubleMeasure(const CalibDoubleMeasure& orig);
    virtual ~CalibDoubleMeasure();

    TH1D* GetTDCbins1(); // Returns the pointer to the tdc distribution in the 1st interval
    TH1D* GetTDCbins2(); // Returns the pointer to the tdc distribution in the 2nd interval
    TH1D* GetCumulative1(); // The cumulative of the 1st interval tdc distribution;
    TH1D* GetCumulative2(); // The cumulative of the 2nd interval tdc distribution;

    double GetTime(int, int); // Returns the time based on two measurements. First (Second) arguments is the tdc bin in the first (second) interval.
    double GetT1Time(int); // Returns the time based only on t1 bin
    double GetT2Time(int); // Returns the time based only on t2 bins
    double GetT1Sigma(int); // Returns the time based only on t1 bin
    double GetT2Sigma(int); // Returns the time based only on t2 bins

    static const double clockCycle;
private:


    TH2D fh_tdcBins_12; // histogram representing "tdcBin2 vs tdcBin1" from the double Measurement
    TH2D fh_tdcBins_13; // histogram representing "tdcBin3 vs tdcBin1" from the triple Measurement

    TH1D *fh_tdcBin1; // Distribution of tdc bin in the 1st interval
    TH1D *fh_tdcBin2; // Distribution of tdc bin in the 2nd interval

    TH1D *fh_Cumulative_t1; // Cumulative histogram of the 1st interval
    TH1D *fh_Cumulative_t2; // Cumulative histogram of the 2nd interval

    double ftMax;

    std::vector<double> f_LUT_1; // Lookup table for the 1st interval
    std::vector<double> f_LUTErr_1; // Lookup table Error for the 1st interval, i.e. bin width/time resolution
    std::vector<double> f_LUT_2; // Lookup table for the 2nd interval
    std::vector<double> f_LUTErr_2; // Lookup table Error for the 2nd interval, i.e. bin width/time resolution

};

#endif /* CALIBDOUBLEMEASURE_H */