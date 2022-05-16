/* 
 * File:   Measurement.h
 * Author: rafopar
 *
 * Created on May 10, 2022, 3:58 PM
 */

#ifndef MEASUREMENT_H
#define MEASUREMENT_H


struct tdcHit {
    int rawTDC;
    int interval;
    int tdcBin;
};

namespace vfTDCFuncs{
    bool CompareInterval( tdcHit*, tdcHit* );
}

class Measurement {
public:
    Measurement(std::vector<tdcHit*>&);
    Measurement(const Measurement& orig);
    virtual ~Measurement();
private:

protected:

    int fNmeasurements;
    std::vector<tdcHit*> f_v_Hits;
};

//________________________________________________________________________

class DoubleMeasure : public Measurement {
public:
    DoubleMeasure(std::vector<tdcHit*> & );
    
    int Get1stInterval();
    int Get1stTDCBin();
    int Get1stRawTDC();
    int Get2ndInterval();
    int Get2ndTDCBin();
    int Get2ndRawTDC();
    


private:
    int f1stInterval;
    int f2ndInterval;
    int f1stTDCBin;
    int f2ndTDCBin;
    int f1stRawTDC;
    int f2ndRawTDC;


    void AnalyzeMeasurement();
};


class TripleMeasure : public Measurement{
public:
    TripleMeasure( std::vector<tdcHit*> & );
    
    int Get1stInterval();
    int Get1stTDCBin();
    int Get1stRawTDC();
    int Get2ndInterval();
    int Get2ndTDCBin();
    int Get2ndRawTDC();
    int Get3rdInterval();
    int Get3rdTDCBin();
    int Get3rdRawTDC();
    
private:
    int f1stInterval;
    int f1stTDCBin;
    int f1stRawTDC;
    int f2ndInterval;
    int f2ndTDCBin;
    int f2ndRawTDC;
    int f3rdInterval;
    int f3rdTDCBin;
    int f3rdRawTDC;
    
    void AnalyzeMeasurement();
};

#endif /* MEASUREMENT_H */