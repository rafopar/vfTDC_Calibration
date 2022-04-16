/* 
 * File:   AnalyzeData1.cc
 * Author: rafopar
 *
 * Created on March 31, 2022, 3:17 PM
 */

#include <set>
#include <cstdlib>

// ===== Root headers =====
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>

// ===== Hipo headers =====
#include <reader.h>
#include <writer.h>
#include <dictionary.h>
#include <vector>


using namespace std;

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
        sprintf(inputFile, "../Data/vftdc_0%d_All.hipo", run);
    } else {
        std::cout << " *** please provide the run number..." << std::endl;
        std::cout<<"Exiting"<<endl;
        exit(0);
    }

    const int nOpen_Channels = 8;
    

    const int probSec = 6;
    const int probLayer = 1;
    const int probComponent = 1;
    const int probOrder = 2;
    const int probEdge = 0;

    const int nBins = 128; // Effectively it is 64 as the 1st bit is always is 0
    const int nEffectiveBins = 64; // As the 1st bit is always 0

    const int nMaxTDC = 66560;
    const int combineIntervals = 20; // will combine 20 TDC intervals together
    const int nTDCRegroupedIntervals = nMaxTDC / combineIntervals / 128;



    TFile file_out(Form("AnalyzeData1_%d_out.root", run), "Recreate");
    TH2D h_n_CAEN_VS_VF_TDCs1("h_n_CAEN_VS_VF_TDCs1", "", 11, -0.5, 10.5, 11., -0.5, 10.5);
    TH1D h_vdTDC_tdcRaw1("h_vdTDC_tdcRaw1", "", 66561, -0.5, 66560.5);
    TH1D h_vftdcbin1("h_vftdcbin1", "", 65, -0.5, 64.5);
    TH1D h_vftdcinterval1("h_vftdcinterval1", "", 521, -0.5, 520.5);

    TH1D h_vftdcbin_regrouped_[nTDCRegroupedIntervals];

    for (int i = 0; i < nTDCRegroupedIntervals; i++) {
        h_vftdcbin_regrouped_[i] = TH1D(Form("h_vftdcbin_regrouped_%d", i), "", 65, -0.5, 64.5);
    }

    TH1D h_countsInTDCBin_[nEffectiveBins];
    TH1D h_Intervals_[nEffectiveBins];

    for (int i = 0; i < nEffectiveBins; i++) {
        h_countsInTDCBin_[i] = TH1D(Form("h_countsInTDCBin_%d", i), "", 4000, 0., 2000);
        h_Intervals_[i] = TH1D(Form("h_Intervals_%d", i), "", 521, -0.5, 520.5);
    }

    int activeChannels[] = {1, 2, 5, 6, 9, 10, 13, 14 };
    std::set<int> s_activeChannels(activeChannels, activeChannels + nOpen_Channels);
    
    std::map<int, TH1D> m_hvfTDC_tdcRaw1; // The key of the map is the "component" in the (sec, layer, component)
    for( auto curComponent : s_activeChannels ){
        m_hvfTDC_tdcRaw1[curComponent] = TH1D(Form("hvfTDC_tdcRaw1_%d", curComponent), "", 66561, -0.5, 66560.5);
    }
    
    hipo::reader reader;
    reader.open(inputFile);

    hipo::dictionary factory;

    reader.readDictionary(factory);

    factory.show();

    hipo::event event;
    int evCounter = 0;

    hipo::bank bCAENTDC(factory.getSchema("FTOF::tdc"));
    hipo::bank bVFTDC(factory.getSchema("FTOF::vftdc"));

    const int nTestCounts = 250000;
    
    try {

        while (reader.next() == true) {
            reader.read(event);

            evCounter = evCounter + 1;

            //if( evCounter >= nTestCounts ){break;}
            
            //if( evCounter > 5000 ){break;}
            if (evCounter % 10000 == 0) {
                cout.flush() << "Processed " << evCounter << " events \r";
            }

            event.getStructure(bCAENTDC);
            event.getStructure(bVFTDC);

            int nCAENTDC = bCAENTDC.getRows();
            int nVFTDC = bVFTDC.getRows();

            h_n_CAEN_VS_VF_TDCs1.Fill(nVFTDC, nCAENTDC);

            //cout<<"Kuku"<<endl;
            for (int i = 0; i < nVFTDC; i++) {
                int sec = bVFTDC.getInt("sector", i);
                int layer = bVFTDC.getInt("layer", i);
                int component = bVFTDC.getInt("component", i);
                int order = bVFTDC.getInt("order", i);
                int tdc = bVFTDC.getInt("TDC", i);
                int edge = bVFTDC.getInt("edge", i);
                long int timestamp = bVFTDC.getLong("timestamp", i);

                
                if( edge != 0 ){
                    continue;
                }
                //cout<<"Sec = "<<sec<<"    layer = "<<layer<<"     component = "<<component<<"    order = "<<order<<"    tdc = "<<tdc<<"    edge = "<<edge<<endl;
                h_vdTDC_tdcRaw1.Fill(tdc);
                
                m_hvfTDC_tdcRaw1[component].Fill(tdc);

                int interval = tdc / nBins;
                int tdcBin = (tdc % nBins) / 2;
                int regroupedInterval = tdc / (combineIntervals * 128);

                h_Intervals_[tdcBin].Fill(interval);
                
                h_vftdcbin1.Fill(tdcBin);
                h_vftdcinterval1.Fill(interval);
                h_vftdcbin_regrouped_[regroupedInterval].Fill(tdcBin);
            }



        }
    } catch (const char msg) {
        cerr << msg << endl;
    }


    for (int interval = 0; interval < nTDCRegroupedIntervals; interval++) {
        for (int i = 0; i < nEffectiveBins; i++) {
            h_countsInTDCBin_[i].Fill( h_vftdcbin_regrouped_[interval].GetBinContent(i+1) );
        }
    }


    gDirectory->Write();

    return 0;
}

