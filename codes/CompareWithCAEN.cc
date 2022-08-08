/* 
 * File:   CompareWithCAEN.cc
 * Author: rafopar
 *
 * Created on July 12, 2022, 4:27 PM
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
#include <set>

#include <Measurement.h>
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
        sprintf(inpHipoFile, "../Decoded/000%d/vftdc_000%d_All.hipo", run, run);
        //sprintf(inpHipoFile, "../Decoded/0%d/vftdc_0%d_All.hipo", run, run);
    } else {
        std::cout << " *** please provide the run number..." << std::endl;
        std::cout << "Exiting" << endl;
        exit(0);
    }

    int corr[6] = {3, 4, 5, 0, 1, 2};

    SLCO ch_1_CAEN(6, 1, 1, 2);
    SLCO ch_2_CAEN(6, 1, 5, 2);
    SLCO ch_1_VF(6, 1, 2, 2);
    SLCO ch_2_VF(6, 1, 6, 2);
    //=====
//        SLCO ch_1_CAEN(6, 1, 1, 2);
//        SLCO ch_2_CAEN(6, 1, 1, 3);
//        SLCO ch_1_VF(6, 1, 2, 2);
//        SLCO ch_2_VF(6, 1, 4, 2);

    //const double tdcconv = 0.023456;
    const double tdcconv = 0.0234375;
    //const double tdcconv = 0.024;
    const int nBins = 128; // Effectively it is 64 as the 1st bit is always is 0
    std::set<SLCO> s_ChannelsInTheFile;

    SLCO ch_1(6, 1, 10, 2);
    SLCO ch_2(6, 1, 10, 3);
    std::map<SLCO, vector<double> > mv_LUT;
    std::map<SLCO, vector<double> > mv_LUTErr;

    TFile file_in(inputFile, "Read");
    TFile file_out(Form("CompareWithCAEN_Run_%d.root", run), "Recreate");

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


    TH1D h_CaentDiff1("h_CaentDiff1", "", 1000, -3000., 3000.);
    //TH1D h_VFtDiff1("h_VFtDiff1", "", 800, -11700., -11300.);
    TH1D h_VFtDiff1("h_VFtDiff1", "", 800, 0., 1000.);
    TH1D h_CAEN_VF_tDiff("h_CAEN_VF_tDiff", "", 1600, -393200., -350000.);
    TH1D h_CAEN_VF_tDiff2("h_CAEN_VF_tDif2", "", 1600, 600000., 640000.);

    TH2D h_CAEN_VF_tDIFF_VS_tVF("h_CAEN_VF_tDIFF_VS_tVF", "", 100, 0., 1050000, 1600, -393200., -350000.);
    TH2D h_CAEN_VF_tDIFF_VS_tVF2("h_CAEN_VF_tDIFF_VS_tVF2", "", 100, 0., 1000000, 1600, 600000., 640000.);
//    TH2D h_CAEN_VF_tDIFF_VS_tVF("h_CAEN_VF_tDIFF_VS_tVF", "", 400, 400000., 1400000, 800, -344000., -338000.);
//    TH2D h_CAEN_VF_tDIFF_VS_tVF2("h_CAEN_VF_tDIFF_VS_tVF2", "", 400, 0., 1000000, 800, 676000., 682 000.);

    hipo::reader reader;
    reader.open(inpHipoFile);

    hipo::dictionary factory;

    reader.readDictionary(factory);

    factory.show();

    hipo::event event;
    int evCounter = 0;

    hipo::bank bVFTDC(factory.getSchema("FTOF::vftdc"));
    hipo::bank bFTOFTDC(factory.getSchema("FTOF::tdc"));

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
            event.getStructure(bFTOFTDC);
            int nVFTDC = bVFTDC.getRows();
            int nFTOFTDC = bFTOFTDC.getRows();

            std::map<SLCO, vector<tdcHit> > m_v_leadHits;
            std::map<SLCO, vector< vector<tdcHit*> > > m_v_OrganizedLeadHits;
            std::map<SLCO, vector< double > > m_v_CalibratedTimes;
            std::map<SLCO, vector< double > > m_v_CAENTimes;

            long int timestampVF = 0;

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
                timestampVF = timestamp;
                if (edge != 0) {
                    continue;
                }

                SLCO cur_SLCO(sec, layer, component, order);

                m_v_leadHits[cur_SLCO].push_back(curHit);
            }

            for (int i = 0; i < nFTOFTDC; i++) {
                int sec = bFTOFTDC.getInt("sector", i);
                int layer = bFTOFTDC.getInt("layer", i);
                int component = bFTOFTDC.getInt("component", i);
                int order = bFTOFTDC.getInt("order", i);

                int phase = timestampVF % 6;
                double tdc = bFTOFTDC.getInt("TDC", i) * tdcconv * 1000. - corr[phase] * 4000;
                SLCO cur_SLCO(sec, layer, component, order);
                m_v_CAENTimes[cur_SLCO].push_back(tdc);
            }

            for (SLCO ch : s_ChannelsInTheFile) {

                if (m_v_leadHits[ch].size() == 0) {
                    continue;
                }
                vfTDCFuncs::GetOrganizedHits(m_v_leadHits[ch], m_v_OrganizedLeadHits[ch]);

                for (vector<tdcHit*> v_cur_orgHits : m_v_OrganizedLeadHits[ch]) {
                    m_v_CalibratedTimes[ch].push_back(vfTDCFuncs::GetCalibratedTime(mv_LUT[ch], mv_LUTErr[ch], v_cur_orgHits));
                }

            }

            if (m_v_CAENTimes.find(ch_1_CAEN) != m_v_CAENTimes.end() && m_v_CAENTimes.find(ch_2_CAEN) != m_v_CAENTimes.end()) {
                h_CaentDiff1.Fill(m_v_CAENTimes[ch_1_CAEN].at(0) - m_v_CAENTimes[ch_2_CAEN].at(0));

            }

            if (m_v_CalibratedTimes.find(ch_1_VF) != m_v_CalibratedTimes.end() && m_v_CalibratedTimes.find(ch_2_VF) != m_v_CalibratedTimes.end()) {
                h_VFtDiff1.Fill(m_v_CalibratedTimes[ch_1_VF].at(0) - m_v_CalibratedTimes[ch_2_VF].at(0));
                //cout<<m_v_CalibratedTimes[ch_1_VF].at(0) - m_v_CalibratedTimes[ch_2_VF].at(0)<<endl;
            }

            if (m_v_CAENTimes.find(ch_1_CAEN) != m_v_CAENTimes.end() && m_v_CalibratedTimes.find(ch_1_VF) != m_v_CalibratedTimes.end()) {
                //cout << "CAEN - VF TIME = " << m_v_CAENTimes[ch_1_CAEN].at(0) - m_v_CalibratedTimes[ch_1_VF].at(0) << endl;
                h_CAEN_VF_tDiff.Fill(m_v_CAENTimes[ch_1_CAEN].at(0) - m_v_CalibratedTimes[ch_1_VF].at(0));
                h_CAEN_VF_tDiff2.Fill(m_v_CAENTimes[ch_1_CAEN].at(0) - m_v_CalibratedTimes[ch_1_VF].at(0));
                h_CAEN_VF_tDIFF_VS_tVF.Fill(m_v_CalibratedTimes[ch_1_VF].at(0), m_v_CAENTimes[ch_1_CAEN].at(0) - m_v_CalibratedTimes[ch_1_VF].at(0));
                h_CAEN_VF_tDIFF_VS_tVF2.Fill(m_v_CalibratedTimes[ch_1_VF].at(0), m_v_CAENTimes[ch_1_CAEN].at(0) - m_v_CalibratedTimes[ch_1_VF].at(0));
            }

        }

    } catch (const char msg) {
        cerr << msg << endl;
    }


    gDirectory->Write();

    return 0;
}