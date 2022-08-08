/* 
 * File:   AnalyzeAllChannels.cc
 * Author: rafopar
 *
 * Created on June 10, 2022, 4:40 PM
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

/*
 * 
 */
int main(int argc, char** argv) {

    char inputFile[256];
    int run;

    if (argc >= 2) {
        //sprintf(outputFile,"%s",argv[2]);
        run = atoi(argv[1]);
        //sprintf(inputFile, "../Decoded/000%d/vftdc_000%d_All.hipo", run, run);
        sprintf(inputFile, "../Decoded/0%d/vftdc_0%d_All.hipo", run, run);
    } else {
        std::cout << " *** please provide the run number..." << std::endl;
        std::cout << "Exiting" << endl;
        exit(0);
    }

    const int nBins = 128; // Effectively it is 64 as the 1st bit is always is 0    

    TFile file_out(Form("Analyze_AllChannels_%d.root", run), "Recreate");

    std::map<SLCO, TH1D > mh_tdcBin_lead1; // Just the tdc bin distribution of all leading edge hits, regardless of the interval,.
    std::map<SLCO, TH1D > mh_nMeasuredHits1;
    std::set<SLCO> s_ChannelsInTheFile;

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

//            if (evCounter >= nTestCounts) {
//                break;
//            }

            //if( evCounter > 5000 ){break;}
            if (evCounter % 100000 == 0) {
                cout.flush() << "Processed " << evCounter << " events \r";
            }

            event.getStructure(bVFTDC);

            int nVFTDC = bVFTDC.getRows();

            std::map<SLCO, vector<tdcHit> > m_v_leadHits;
            std::map<SLCO, vector< vector<tdcHit*> > > m_v_OrganizedLeadHits;

            std::set<SLCO> s_firedChannels;

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


                SLCO cur_SLCO(sec, layer, component, order);
                
                if (s_ChannelsInTheFile.find(cur_SLCO) == s_ChannelsInTheFile.end()) {
                    cout << "Kuku" << endl;
                    mh_tdcBin_lead1[cur_SLCO] = TH1D(Form("h_tdcBin_lead1_%d_%d_%d_%d", cur_SLCO.sector, cur_SLCO.layer, cur_SLCO.component, cur_SLCO.order), "", nBins + 1, -0.5, nBins + 0.5);
                    mh_nMeasuredHits1[cur_SLCO] = TH1D(Form("mh_nMeasuredHits1_%d_%d_%d_%d", cur_SLCO.sector, cur_SLCO.layer, cur_SLCO.component, cur_SLCO.order), "", 5, -0.5, 4.5);
                }

                mh_tdcBin_lead1[cur_SLCO].Fill(tdcBin);
                m_v_leadHits[cur_SLCO].push_back(curHit);
                s_firedChannels.insert(cur_SLCO);
                s_ChannelsInTheFile.insert(cur_SLCO);
            }


            for (SLCO ch : s_firedChannels) {

                vfTDCFuncs::GetOrganizedHits(m_v_leadHits[ch], m_v_OrganizedLeadHits[ch]);

                for (auto curVec : m_v_OrganizedLeadHits[ch]) {

                    mh_nMeasuredHits1[ch].Fill(curVec.size());

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