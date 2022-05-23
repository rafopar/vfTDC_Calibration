/* 
 * File:   AnaMultiChannelDoubleMeasure.cc
 * Author: rafopar
 *
 * Created on May 17, 2022, 3:22 PM
 */

#include <cstdlib>
#include <set>
#include <iomanip>

// ===== Root headers =====
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>

// ===== Hipo headers =====
#include <reader.h>
#include <writer.h>
#include <dictionary.h>
#include <vector>

#include <Measurement.h>

using namespace std;

void GetOrganizedHits(vector<tdcHit>&, vector< vector<tdcHit*> >&);

/*
 * 
 */
int main(int argc, char** argv) {

    std::cout << " reading file example program (HIPO) " << __cplusplus << std::endl;

    char inputFile[256];
    int run;

    if (argc >= 2) {
        //sprintf(outputFile,"%s",argv[2]);
        run = atoi(argv[1]);
        sprintf(inputFile, "../Decoded/000%d/vftdc_000%d_All.hipo", run, run);
        //sprintf(inputFile, "../Decoded/0%d/vftdc_0%d_All.hipo", run, run);
    } else {
        std::cout << " *** please provide the run number..." << std::endl;
        std::cout << "Exiting" << endl;
        exit(0);
    }

    const int nOpen_Channels = 8;


    const int nBins = 128; // Effectively it is 64 as the 1st bit is always is 0
    const int nEffectiveBins = 64; // As the 1st bit is always 0

    const int nMaxTDC = 66560;
    int activeChannels[] = {1, 2, 5, 6, 9, 10, 13, 14};
    std::set<int> s_activeChannels(activeChannels, activeChannels + nOpen_Channels);

    
    TFile file_out(Form("Analyze_MultiChannelDoubleMeasure_%d.root", run), "Recreate");
    
    std::map<int, TH2D > mh_tdc1_vs_tdc2_DoubleMeasure1;
    std::map<int, TH2D > mh_tdc1_vs_tdc2_DoubleMeasure2;
    std::map<int, TH2D > mh_tdc1_vs_tdc2_DoubleMeasure3;
    std::map<int, TH2D > mh_tdc1_vs_tdc2_TripleMeasure1;
    std::map<int, TH2D > mh_tdc1_vs_tdc3_TripleMeasure1;
    std::map<int, TH2D > mh_tdc2_vs_tdc3_TripleMeasure1;
    std::map<int, TH2D > mh_tdc1_vs_tdc2_TripleMeasure2;
    std::map<int, TH2D > mh_tdc1_vs_tdc3_TripleMeasure2;
    std::map<int, TH2D > mh_tdc2_vs_tdc3_TripleMeasure2;
    std::map<int, TH2D > mh_tdc1_vs_tdc2_TripleMeasure3;
    std::map<int, TH2D > mh_tdc1_vs_tdc3_TripleMeasure3;
    std::map<int, TH2D > mh_tdc2_vs_tdc3_TripleMeasure3;

    
    for( int ch : s_activeChannels ){
        mh_tdc1_vs_tdc2_DoubleMeasure1[ch] = TH2D(Form("h_tdc1_vs_tdc2_DoubleMeasure1_%d", ch), "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);
        mh_tdc1_vs_tdc2_DoubleMeasure2[ch] = TH2D(Form("h_tdc1_vs_tdc2_DoubleMeasure2_%d", ch), "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);
        mh_tdc1_vs_tdc2_DoubleMeasure3[ch] = TH2D(Form("h_tdc1_vs_tdc2_DoubleMeasure3_%d", ch), "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);
        mh_tdc1_vs_tdc2_TripleMeasure1[ch] = TH2D(Form("h_tdc1_vs_tdc2_TripleMeasure1_%d", ch), "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);
        mh_tdc1_vs_tdc3_TripleMeasure1[ch] = TH2D(Form("h_tdc1_vs_tdc3_TripleMeasure1_%d", ch), "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);
        mh_tdc2_vs_tdc3_TripleMeasure1[ch] = TH2D(Form("h_tdc2_vs_tdc3_TripleMeasure1_%d", ch), "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);
        mh_tdc1_vs_tdc2_TripleMeasure2[ch] = TH2D(Form("h_tdc1_vs_tdc2_TripleMeasure2_%d", ch), "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);
        mh_tdc1_vs_tdc3_TripleMeasure2[ch] = TH2D(Form("h_tdc1_vs_tdc3_TripleMeasure2_%d", ch), "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);
        mh_tdc2_vs_tdc3_TripleMeasure2[ch] = TH2D(Form("h_tdc2_vs_tdc3_TripleMeasure2_%d", ch), "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);
        mh_tdc1_vs_tdc2_TripleMeasure3[ch] = TH2D(Form("h_tdc1_vs_tdc2_TripleMeasure3_%d", ch), "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);
        mh_tdc1_vs_tdc3_TripleMeasure3[ch] = TH2D(Form("h_tdc1_vs_tdc3_TripleMeasure3_%d", ch), "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);
        mh_tdc2_vs_tdc3_TripleMeasure3[ch] = TH2D(Form("h_tdc2_vs_tdc3_TripleMeasure3_%d", ch), "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);
    }

    hipo::reader reader;
    reader.open(inputFile);

    hipo::dictionary factory;

    reader.readDictionary(factory);

    factory.show();

    hipo::event event;
    int evCounter = 0;

    hipo::bank bVFTDC(factory.getSchema("FTOF::vftdc"));

    const int nTestCounts = 750000;

    try {

        while (reader.next() == true) {
            reader.read(event);

            evCounter = evCounter + 1;

            if (evCounter >= nTestCounts) {
                break;
            }

            //if( evCounter > 5000 ){break;}
            if (evCounter % 100000 == 0) {
                cout.flush() << "Processed " << evCounter << " events \r";
            }

            event.getStructure(bVFTDC);

            int nVFTDC = bVFTDC.getRows();

            std::map<int, vector<tdcHit> > m_v_leadHits;
            std::map<int, vector< vector<tdcHit*> > > m_v_OrganizedLeadHits;

            for (int ch : s_activeChannels) {
                m_v_leadHits[ch] = vector<tdcHit>();
                m_v_OrganizedLeadHits[ch] = vector< vector<tdcHit*> >();
            }

            vector<tdcHit> v_leadingedgeHits;
            vector<tdcHit> v_trailingedgeHits;

            for (int i = 0; i < nVFTDC; i++) {
                int sec = bVFTDC.getInt("sector", i);
                int layer = bVFTDC.getInt("layer", i);
                int component = bVFTDC.getInt("component", i);
                int order = bVFTDC.getInt("order", i);
                int tdc = bVFTDC.getInt("TDC", i);
                int edge = bVFTDC.getInt("edge", i);
                long int timestamp = bVFTDC.getLong("timestamp", i);
                int interval = tdc / nBins;
                int tdcBin = (tdc % nBins);

                tdcHit curHit;
                curHit.interval = interval;
                curHit.rawTDC = tdc;
                curHit.tdcBin = tdcBin;

                if (edge != 0) {
                    continue;
                }

                m_v_leadHits[component].push_back(curHit);

            }


            for (int ch : s_activeChannels) {

                if (m_v_leadHits[ch].size() == 0) {
                    continue;
                }
                GetOrganizedHits(m_v_leadHits[ch], m_v_OrganizedLeadHits[ch]);

                for (auto curVec : m_v_OrganizedLeadHits[ch]) {

                    if (curVec.size() == 2) {
                        DoubleMeasure doubleMes(curVec);
                        mh_tdc1_vs_tdc2_DoubleMeasure1[ch].Fill(doubleMes.Get1stTDCBin(), doubleMes.Get2ndTDCBin());
                        
                        if( doubleMes.Get1stInterval() %2 == 0 ){
                            mh_tdc1_vs_tdc2_DoubleMeasure2[ch].Fill(doubleMes.Get1stTDCBin(), doubleMes.Get2ndTDCBin());
                        }else{
                            mh_tdc1_vs_tdc2_DoubleMeasure3[ch].Fill(doubleMes.Get1stTDCBin(), doubleMes.Get2ndTDCBin());
                        }
                        
                        
                    }else if (curVec.size() == 3) {
                        TripleMeasure triMes(curVec);
                        
                        mh_tdc1_vs_tdc2_TripleMeasure1[ch].Fill(triMes.Get1stTDCBin(), triMes.Get2ndTDCBin() );
                        mh_tdc1_vs_tdc3_TripleMeasure1[ch].Fill(triMes.Get1stTDCBin(), triMes.Get3rdTDCBin() );
                        mh_tdc2_vs_tdc3_TripleMeasure1[ch].Fill(triMes.Get2ndTDCBin(), triMes.Get3rdTDCBin() );
                        
                        if( triMes.Get1stInterval()%2 == 0 ) {
                            mh_tdc1_vs_tdc2_TripleMeasure2[ch].Fill(triMes.Get1stTDCBin(), triMes.Get2ndTDCBin());
                            mh_tdc1_vs_tdc3_TripleMeasure2[ch].Fill(triMes.Get1stTDCBin(), triMes.Get3rdTDCBin());
                            mh_tdc2_vs_tdc3_TripleMeasure2[ch].Fill(triMes.Get2ndTDCBin(), triMes.Get3rdTDCBin());
                        }else {
                            mh_tdc1_vs_tdc2_TripleMeasure3[ch].Fill(triMes.Get1stTDCBin(), triMes.Get2ndTDCBin());
                            mh_tdc1_vs_tdc3_TripleMeasure3[ch].Fill(triMes.Get1stTDCBin(), triMes.Get3rdTDCBin());
                            mh_tdc2_vs_tdc3_TripleMeasure3[ch].Fill(triMes.Get2ndTDCBin(), triMes.Get3rdTDCBin());
                        }
                    }

                }

            }


        }


    } catch (const char msg) {
        cerr << msg << endl;
    }


    gDirectory->Write();
    file_out.Close();

    return 0;
}

void GetOrganizedHits(vector<tdcHit>& v_allHits, vector< vector<tdcHit*> > &v_organizedHits) {

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