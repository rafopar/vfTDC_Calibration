/* 
 * File:   MakeLUTAllChannels.cc
 * Author: rafopar
 *
 * Created on June 12, 2022, 7:00 PM
 */

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>

// ===== Hipo headers =====
#include <reader.h>
#include <writer.h>
#include <dictionary.h>
#include <vector>


#include <Measurement.h>
#include <set>


using namespace vfTDCFuncs;
using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    int run;

    char inputFile[256];
    char inpHipoFile[256];
    if (argc >= 2) {
        //sprintf(outputFile,"%s",argv[2]);
        run = atoi(argv[1]);
        sprintf(inputFile, "Analyze_AllChannels_%d.root", run);
        sprintf(inpHipoFile, "../Decoded/0%d/vftdc_0%d_All.hipo", run, run);
    } else {
        std::cout << " *** please provide the run number..." << std::endl;
        std::cout << "Exiting" << endl;
        exit(0);
    }

    const int nBins = 128; // Effectively it is 64 as the 1st bit is always is 0
    std::set<SLCO> s_ChannelsInTheFile;

    SLCO ch_1(6, 1, 10, 2);
    SLCO ch_2(6, 1, 10, 3);
    std::map<SLCO, vector<double> > mv_LUT;
    std::map<SLCO, vector<double> > mv_LUTErr;

    TFile file_in(inputFile, "Read");
    TFile file_out(Form("TimeResolStudiesAllCh_Run_%d.root", run), "Recreate");

    TList *objList = file_in.GetListOfKeys();

    for (int i = 0; i < objList->GetSize(); i++) {
        std::string histName = objList->At(i)->GetName();
        cout << histName << endl;

        string delim = "_";
        vector<std::string> nameParts;
        size_t pos = 0;

        while ((pos = histName.find(delim)) != string::npos) {
            nameParts.push_back(histName.substr(0, pos));
            histName.erase(0, pos + delim.length());
        }
        nameParts.push_back(histName);

        if (nameParts.at(1).compare("tdcBin") != 0) {
            continue;
        }
        int sec = atoi(nameParts.at(3).c_str());
        int layer = atoi(nameParts.at(4).c_str());
        int component = atoi(nameParts.at(5).c_str());
        int order = atoi(nameParts.at(6).c_str());

        SLCO cur_SLCO(sec, layer, component, order);
        s_ChannelsInTheFile.insert(cur_SLCO);

        TH1D *h_tdcDistr = (TH1D*) file_in.Get(Form("h_tdcBin_lead1_%d_%d_%d_%d", sec, layer, component, order));
        TH1D *h_nHits = (TH1D*) file_in.Get(Form("mh_nMeasuredHits1_%d_%d_%d_%d", sec, layer, component, order));

        int n2Hits = h_nHits->GetBinContent(3);
        int n3Hits = h_nHits->GetBinContent(4);

        //std::vector <double> v_LUT = vfTDCFuncs::GetLUT(h_tdcDistr, n2Hits, n3Hits);
        vector <double> v_LUT_Err = vfTDCFuncs::GetLUTErr(h_tdcDistr, n2Hits, n3Hits);
        mv_LUT[cur_SLCO] = vfTDCFuncs::GetLUT(h_tdcDistr, n2Hits, n3Hits);
        mv_LUTErr[cur_SLCO] = vfTDCFuncs::GetLUTErr(h_tdcDistr, n2Hits, n3Hits);

        ofstream out_LUT(Form("LUTs/LUTs_%d_%d_%d_%d_%d.dat", run, sec, layer, component, order));

        for (int ii = 0; ii < mv_LUT[cur_SLCO].size(); ii++) {
            out_LUT << setw(5) << ii << setw(14) << mv_LUT[cur_SLCO].at(ii) << setw(14) << mv_LUTErr[cur_SLCO].at(ii) << endl;
        }

        out_LUT.close();
    }

    TH2D *h_nHits_2_VS_1_1 = new TH2D("h_nHits_2_VS_1_1", "", 11, -0.5, 10.5, 11, -0.5, 10.5);
    TH1D *h_TImeDiff_1And2 = new TH1D("h_TImeDiff_1And2", "", 200, -25000, 25000.);
    //TH1D *h_TImeDiff_1And2 = new TH1D("h_TImeDiff_1And2", "", 800, 386000, 392000.);
    //TH1D *h_TImeDiff_1And2 = new TH1D("h_TImeDiff_1And2", "", 800, 382000, 386000.);
    TH2D *h_Time2_vs_1_1 = new TH2D("h_Time2_vs_1_1", "", 5000, 0., 1100000, 5000, 0., 1100000);
    TH2D *h_TimeDiff_vs_t1_1 = new TH2D("h_TimeDiff_vs_t1_1", "", 5000, 0., 1100000, 800, 386000, 392000);
    //TH2D *h_TimeDiff_vs_t1_1 = new TH2D("h_TimeDiff_vs_t1_1", "", 5000, 0., 1100000, 800, -2000, 2000);

    hipo::reader reader;
    reader.open(inpHipoFile);

    hipo::dictionary factory;

    reader.readDictionary(factory);

    factory.show();

    hipo::event event;
    int evCounter = 0;

    hipo::bank bVFTDC(factory.getSchema("FTOF::vftdc"));

    const int nTestCounts = 150000;
    try {

        while (reader.next() == true) {
            reader.read(event);

            evCounter = evCounter + 1;

            if (evCounter >= nTestCounts) {
                break;
            }

            if (evCounter % 100000 == 0) {
                cout.flush() << "Processed " << evCounter << " events \r";
            }

            event.getStructure(bVFTDC);
            int nVFTDC = bVFTDC.getRows();

            std::map<SLCO, vector<tdcHit> > m_v_leadHits;
            std::map<SLCO, vector< vector<tdcHit*> > > m_v_OrganizedLeadHits;


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
                curHit.timestamp = timestamp;

                if (edge != 0) {
                    continue;
                }

                SLCO cur_SLCO(sec, layer, component, order);

                m_v_leadHits[cur_SLCO].push_back(curHit);
            }

            for (SLCO ch : s_ChannelsInTheFile) {

                if (m_v_leadHits[ch].size() == 0) {
                    continue;
                }
                vfTDCFuncs::GetOrganizedHits(m_v_leadHits[ch], m_v_OrganizedLeadHits[ch]);
            }

            //cout << "Kuku ch_1 size is "<< m_v_OrganizedLeadHits[ch_1].size()<<"    ch_2 size is "<<m_v_OrganizedLeadHits[ch_2].size()<< endl;
            if (m_v_OrganizedLeadHits[ch_1].size() == 1 && m_v_OrganizedLeadHits[ch_2].size() == 1) {


                h_nHits_2_VS_1_1->Fill(m_v_OrganizedLeadHits[ch_1].at(0).size(), m_v_OrganizedLeadHits[ch_2].at(0).size());

                if (m_v_OrganizedLeadHits[ch_1].at(0).size() >= 2 && m_v_OrganizedLeadHits[ch_2].at(0).size() >= 2) {

                    double time_ch1 = vfTDCFuncs::GetCalibratedTime(mv_LUT[ch_1], mv_LUTErr[ch_1], m_v_OrganizedLeadHits[ch_1].at(0));
                    double time_ch2 = vfTDCFuncs::GetCalibratedTime(mv_LUT[ch_2], mv_LUTErr[ch_2], m_v_OrganizedLeadHits[ch_2].at(0));

                    //if (m_v_OrganizedLeadHits[ch_1].at(0).at(0)->timestamp != m_v_OrganizedLeadHits[ch_2].at(0).at(0)->timestamp) {
                    h_TImeDiff_1And2->Fill(time_ch1 - time_ch2);
                    h_Time2_vs_1_1->Fill(time_ch1, time_ch2);
                    h_TimeDiff_vs_t1_1->Fill(time_ch1, time_ch1 - time_ch2);
                    //}
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