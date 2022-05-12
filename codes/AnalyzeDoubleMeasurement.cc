/* 
 * File:   AnalyzeDoubleMeasurement.cc
 * Author: rafopar
 *
 * Created on May 5, 2022, 4:56 PM
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

//struct tdcHit {
//    int rawTDC;
//    int interval;
//    int tdcBin;
//};


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
        sprintf(inputFile, "../Decoded/0%d/vftdc_0%d_All.hipo", run, run);
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

    TFile file_out(Form("Analyze_DoubleMeasurement_%d.root", run), "Recreate");

    std::map<int, TH1D> m_hvfTDC_tdcRaw_leading1; // The key of the map is the "component" in the (sec, layer, component)
    std::map<int, TH1D> m_hvfTDC_tdcRaw_trailing1; // The key of the map is the "component" in the (sec, layer, component)
    for (auto curComponent : s_activeChannels) {
        m_hvfTDC_tdcRaw_leading1[curComponent] = TH1D(Form("hvfTDC_tdcRaw_leading1_%d", curComponent), "", 66561, -0.5, 66560.5);
        m_hvfTDC_tdcRaw_trailing1[curComponent] = TH1D(Form("hvfTDC_tdcRaw_trailing1_%d", curComponent), "", 66561, -0.5, 66560.5);
    }

    TH2D h_nTrailingVSnLeadingHits1("h_nTrailingVSnLeadingHits1", "", 11, -0.5, 10.5, 11, -0.5, 10.5);
    TH1D h_IntervalDiff_3hits1("h_IntervalDiff_3hits1", "", 11, -5.5, 5.5);

    TH1D h_vfTDC_tdcRaw_leadingEdge1("h_vfTDC_tdcRaw_leadingEdge1", "", 66561, -0.5, 66560.5);
    TH1D h_vfTDC_tdcRaw_trailingEdge1("h_vfTDC_tdcRaw_trailingEdge1", "", 66561, -0.5, 66560.5);

    TH2D h_tdc1_vs_tdc2_DoubleMeasure1("h_tdc1_vs_tdc2_DoubleMeasure1", "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);
    TH2D h_tdc1_vs_tdc2_DoubleMeasure2("h_tdc1_vs_tdc2_DoubleMeasure2", "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);
    TH2D h_tdc1_vs_tdc2_DoubleMeasure3("h_tdc1_vs_tdc2_DoubleMeasure3", "", nBins + 1, -0.5, nBins + 0.5, nBins + 1, -0.5, nBins + 0.5);

    hipo::reader reader;
    reader.open(inputFile);

    hipo::dictionary factory;

    reader.readDictionary(factory);

    factory.show();

    hipo::event event;
    int evCounter = 0;

    hipo::bank bVFTDC(factory.getSchema("FTOF::vftdc"));

    const int nTestCounts = 25000000;

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

                edge == 0 ? v_leadingedgeHits.push_back(curHit) : v_trailingedgeHits.push_back(curHit);
                edge == 0 ? h_vfTDC_tdcRaw_leadingEdge1.Fill(curHit.rawTDC) : h_vfTDC_tdcRaw_trailingEdge1.Fill(curHit.rawTDC);
                edge == 0 ? m_hvfTDC_tdcRaw_leading1[component].Fill(curHit.rawTDC) : m_hvfTDC_tdcRaw_trailing1[component].Fill(curHit.rawTDC);


            }

            vector< vector<tdcHit*> > v_lead_OrganizedHits;


            int n_leadHits = v_leadingedgeHits.size();
            int n_trailHits = v_trailingedgeHits.size();
            h_nTrailingVSnLeadingHits1.Fill(n_leadHits, n_trailHits);

            GetOrganizedHits(v_leadingedgeHits, v_lead_OrganizedHits);
            //            if (n_leadHits > 2) {
            //                bVFTDC.show();
            //                cout << "Size of Organized Hits is " << v_lead_OrganizedHits.size() << endl;                
            //            }


            for (auto curVec : v_lead_OrganizedHits) {

                if (curVec.size() == 2) {

                    DoubleMeasure doubleMes(curVec);
                    h_tdc1_vs_tdc2_DoubleMeasure1.Fill(doubleMes.Get1stTDCBin(), doubleMes.Get2ndTDCBin());

                    if (doubleMes.Get1stInterval() % 2 == 0) {
                        h_tdc1_vs_tdc2_DoubleMeasure2.Fill(doubleMes.Get1stTDCBin(), doubleMes.Get2ndTDCBin());
                    } else {
                        h_tdc1_vs_tdc2_DoubleMeasure3.Fill(doubleMes.Get1stTDCBin(), doubleMes.Get2ndTDCBin());
                    }

                    //                    cout<<" ************************* "<<endl;
                    //                    cout<<"--- From the vector "<<curVec.at(0)->interval<<"     "<<curVec.at(1)->interval<<endl;
                    //                    DoubleMeasure doubleMes(curVec);
                    //                    cout<<"--- From the class "<<doubleMes.Get1stInterval()<<"     "<<doubleMes.Get2ndInterval()<<endl;

                }

            }

            if (n_leadHits == 3) {
                h_IntervalDiff_3hits1.Fill(v_leadingedgeHits.at(1).interval - v_leadingedgeHits.at(0).interval);
                h_IntervalDiff_3hits1.Fill(v_leadingedgeHits.at(2).interval - v_leadingedgeHits.at(1).interval);

                //                if( (v_leadingedgeHits.at(1).interval - v_leadingedgeHits.at(0).interval) != 1 ||
                //                        (v_leadingedgeHits.at(2).interval - v_leadingedgeHits.at(1).interval) != 1 ){
                //                    cout<<setw(15)<<v_leadingedgeHits.at(0).interval<<setw(15)<<v_leadingedgeHits.at(1).interval<<setw(15)
                //                            <<v_leadingedgeHits.at(2).interval<<endl;
                //                }


                // *********************** Making sure those three hits are in consecutive intervals ==================

                if (TMath::Abs(v_leadingedgeHits.at(1).interval - v_leadingedgeHits.at(0).interval) > 2 ||
                        TMath::Abs(v_leadingedgeHits.at(2).interval - v_leadingedgeHits.at(1).interval) > 2) {
                    continue;
                }

            } else if (n_leadHits == 2) {
                if (TMath::Abs(v_leadingedgeHits.at(1).interval - v_leadingedgeHits.at(0).interval) > 1) {
                    continue;
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