/* 
 * File:   DrawPlots1.cc
 * Author: rafopar
 *
 * Created on April 4, 2022, 2:19 PM
 */

#include <cstdlib>

#include <TFile.h>

using namespace std;

/*
 * 
 */
void DrawPlots1() {

    gStyle->SetOptFit(1);

    const int run = 15906;

    TFile file_in(Form("AnalyzeData1_%d_out.root", run), "Read");
    const int nOpen_Channels = 8;
    int activeChannels[] = {1, 2, 5, 6, 9, 10, 13, 14};
    std::set<int> s_activeChannels(activeChannels, activeChannels + nOpen_Channels);

    std::map<int, TH1D*> m_hvfTDC_tdcRaw1; // The key of the map is the "component" in the (sec, layer, component)
    std::map<int, TH1D> m_h_vfTDCBin1;
    
    for( auto curCh : s_activeChannels ){
        m_h_vfTDCBin1[curCh] = TH1D(Form("h_vfTDCBin1_%d", curCh), "", 65, -0.5, 64.5 );
        m_h_vfTDCBin1[curCh].SetFillColor(4);
    }
    
    const int maxTDch = 66560;
    const int nTDCbins = 64;

    TH1D *h_vdTDC_tdcRaw1 = (TH1D*) file_in.Get("h_vdTDC_tdcRaw1");

    TFile file_out("OutHists.root", "Recreate");
    TH1D h_TDC_Counts_[nTDCbins];

    for (int i = 0; i < nTDCbins; i++) {

        int expectedCounts = h_vdTDC_tdcRaw1->GetBinContent(512 + 2 * i + 1);

        h_TDC_Counts_[i] = TH1D(Form("h_TDC_Counts_%d", i), "", 500, 0. * expectedCounts, 2.8 * expectedCounts);
    }

    for (int i = 0; i < h_vdTDC_tdcRaw1->GetNbinsX(); i++) {

        if ((i) % 2 != 0) {
            continue; // non event bins are empty so will skip those
        }

        int tdcBin = ((i + 1) % (2 * nTDCbins)) / 2;

        double countsInBin = h_vdTDC_tdcRaw1->GetBinContent(i + 1);
        if (countsInBin > 0.5) {
            h_TDC_Counts_[tdcBin].Fill(countsInBin);
        } else {
            cout << "Counts in Bin " << i + 1 << "   is " << countsInBin << endl;
        }

    }

    TCanvas *c1 = new TCanvas("c1", "", 3400, 600);
    c1->Divide(16, 4);

    for (int i = 0; i < nTDCbins; i++) {
        c1->cd(i + 1);
        h_TDC_Counts_[i].DrawCopy();
    }

    TCanvas *c2 = new TCanvas("c2", "", 1200, 1200);
    c2->cd();

    TF1 *f_Gaus = new TF1("f_Gaus", "[0]*TMath::Gaus(x, [1], [2])", 0, 18000);

    TLine *line_mean = new TLine();
    line_mean->SetLineColor(6);
    line_mean->SetLineStyle(6);
    TLine *line_StatErr = new TLine();
    line_StatErr->SetLineColor(4);
    TLine *line_FitErr = new TLine();
    line_FitErr->SetLineColor(2);
    c2->Print("Figs/TDC_Counts.pdf[");
    for (int i = 0; i < nTDCbins; i++) {

        /**
         * 
         * Let's find min and max bins of hists with some entrues, and set histogram ranges accordingly
         */


        int minBin = 1;
        int maxBin = h_TDC_Counts_[i].GetNbinsX();

        if (h_TDC_Counts_[i].GetEntries() > 0) {
            for (int ibin = 0; ibin < h_TDC_Counts_[i].GetNbinsX(); ibin++) {
                if (h_TDC_Counts_[i].GetBinContent(ibin + 3) > 0) {
                    minBin = ibin + 1;
                    break;
                }
            }

            for (int ibin = 0; ibin < h_TDC_Counts_[i].GetNbinsX(); ibin++) {
                if (h_TDC_Counts_[i].GetBinContent(h_TDC_Counts_[i].GetNbinsX() - ibin - 3) > 0) {
                    maxBin = h_TDC_Counts_[i].GetNbinsX() - ibin;
                    break;
                }

            }
        }

        h_TDC_Counts_[i].GetXaxis()->SetRange(minBin, maxBin);
        double xMin = h_TDC_Counts_[i].GetBinCenter(minBin);
        double xMax = h_TDC_Counts_[i].GetBinCenter(maxBin);
        f_Gaus->SetParameters(h_TDC_Counts_[i].GetMaximum(), h_TDC_Counts_[i].GetMean(), h_TDC_Counts_[i].GetRMS());
        h_TDC_Counts_[i].Fit(f_Gaus, "MeV", "", xMin, xMax);
        double statErr = sqrt(f_Gaus->GetParameter(1));
        double fitErr = f_Gaus->GetParameter(2);
        h_TDC_Counts_[i].DrawCopy();
        line_StatErr->DrawLine(f_Gaus->GetParameter(1) - statErr, 0., f_Gaus->GetParameter(1) - statErr, 0.5 * h_TDC_Counts_[i].GetMaximum());
        line_StatErr->DrawLine(f_Gaus->GetParameter(1) + statErr, 0., f_Gaus->GetParameter(1) + statErr, 0.5 * h_TDC_Counts_[i].GetMaximum());
        line_FitErr->DrawLine(f_Gaus->GetParameter(1) - fitErr, 0., f_Gaus->GetParameter(1) - fitErr, 0.5 * h_TDC_Counts_[i].GetMaximum());
        line_FitErr->DrawLine(f_Gaus->GetParameter(1) + fitErr, 0., f_Gaus->GetParameter(1) + fitErr, 0.5 * h_TDC_Counts_[i].GetMaximum());
        line_mean->DrawLine(f_Gaus->GetParameter(1), 0., f_Gaus->GetParameter(1), f_Gaus->GetParameter(0));
        c2->Print("Figs/TDC_Counts.pdf");
    }
    c2->Print("Figs/TDC_Counts.pdf]");


    /*
     * Now loop over multiple channels.
     */

    for (auto curCh : s_activeChannels) {

        c2->cd();
        c2->Print(Form("Figs/TDC_Counts_%d.pdf", curCh));

        m_hvfTDC_tdcRaw1[curCh] = (TH1D*) file_in.Get(Form("hvfTDC_tdcRaw1_%d", curCh));
        c1->cd();

        c1->Print(Form("Figs/Raw_TDC_Run_%d_Component%d.pdf[", run, curCh));
        m_hvfTDC_tdcRaw1[curCh]->Draw();

        c1->Print(Form("Figs/Raw_TDC_Run_%d_Component%d.pdf", run, curCh));

        TH1D h_TDC_Counts_[nTDCbins];

        for (int i = 0; i < nTDCbins; i++) {

            int expectedCounts = 0.5 * (m_hvfTDC_tdcRaw1[curCh]->GetBinContent(512 + 2 * i + 1) + m_hvfTDC_tdcRaw1[curCh]->GetBinContent(640 + 2 * i + 1));

            h_TDC_Counts_[i] = TH1D(Form("h_TDC_Counts_%d_%d", curCh, i), "", 500, 0. * expectedCounts, 2.8 * expectedCounts);
        }


        for (int i = 0; i < m_hvfTDC_tdcRaw1[curCh]->GetNbinsX(); i++) {

            if ((i) % 2 != 0) {
                continue; // non event bins are empty so will skip those
            }

            int tdcBin = ((i + 1) % (2 * nTDCbins)) / 2;
	
	    int binInHist = m_h_vfTDCBin1[curCh].FindBin(tdcBin);
            
            double countsInBin = m_hvfTDC_tdcRaw1[curCh]->GetBinContent(i + 1);
            m_h_vfTDCBin1[curCh].SetBinContent(binInHist, m_h_vfTDCBin1[curCh].GetBinContent(binInHist) + countsInBin );
            if (countsInBin > 0.5) {
                h_TDC_Counts_[tdcBin].Fill(countsInBin);
            } else {
                cout << "Counts in Bin " << i + 1 << "   is " << countsInBin << endl;
            }

        }
        
        m_h_vfTDCBin1[curCh].Draw();
        c1->Print(Form("Figs/Raw_TDC_Run_%d_Component%d.pdf", run, curCh));

        c1->Clear();
        c1->Divide(16, 4);

        for (int i = 0; i < nTDCbins; i++) {
            c1->cd(i + 1);
            h_TDC_Counts_[i].DrawCopy();
        }

        c1->Print(Form("Figs/Raw_TDC_Run_%d_Component%d.pdf", run, curCh));

        c2->cd();

        c2->Print(Form("Figs/TDC_Counts_%d.pdf[", curCh));

        for (int i = 0; i < nTDCbins; i++) {

//            cout<<"curCh = "<<curCh<<"    tdcBin = "<<i<<endl;
//            cin.ignore();
            
            /**
             * 
             * Let's find min and max bins of hists with some entrues, and set histogram ranges accordingly
             */

            int minBin = 1;
            int maxBin = h_TDC_Counts_[i].GetNbinsX();
            if (h_TDC_Counts_[i].GetEntries() > 0) {
                for (int ibin = 0; ibin < h_TDC_Counts_[i].GetNbinsX(); ibin++) {
                    if (h_TDC_Counts_[i].GetBinContent(ibin + 3) > 0) {
                        minBin = ibin + 1;
                        break;
                    }
                }

                for (int ibin = 0; ibin < h_TDC_Counts_[i].GetNbinsX(); ibin++) {
                    if (h_TDC_Counts_[i].GetBinContent(h_TDC_Counts_[i].GetNbinsX() - ibin - 3) > 0) {
                        maxBin = h_TDC_Counts_[i].GetNbinsX() - ibin;
                        break;
                    }

                }
            }

            h_TDC_Counts_[i].GetXaxis()->SetRange(minBin, maxBin);
            double xMin = h_TDC_Counts_[i].GetBinCenter(minBin);
            double xMax = h_TDC_Counts_[i].GetBinCenter(maxBin);
            f_Gaus->SetParameters(h_TDC_Counts_[i].GetMaximum(), h_TDC_Counts_[i].GetMean(), h_TDC_Counts_[i].GetRMS());
            h_TDC_Counts_[i].Fit(f_Gaus, "MeV", "", xMin, xMax);
            double statErr = sqrt(f_Gaus->GetParameter(1));
            double fitErr = f_Gaus->GetParameter(2);
            h_TDC_Counts_[i].DrawCopy();
            line_StatErr->DrawLine(f_Gaus->GetParameter(1) - statErr, 0., f_Gaus->GetParameter(1) - statErr, 0.5 * h_TDC_Counts_[i].GetMaximum());
            line_StatErr->DrawLine(f_Gaus->GetParameter(1) + statErr, 0., f_Gaus->GetParameter(1) + statErr, 0.5 * h_TDC_Counts_[i].GetMaximum());
            line_FitErr->DrawLine(f_Gaus->GetParameter(1) - fitErr, 0., f_Gaus->GetParameter(1) - fitErr, 0.5 * h_TDC_Counts_[i].GetMaximum());
            line_FitErr->DrawLine(f_Gaus->GetParameter(1) + fitErr, 0., f_Gaus->GetParameter(1) + fitErr, 0.5 * h_TDC_Counts_[i].GetMaximum());
            line_mean->DrawLine(f_Gaus->GetParameter(1), 0., f_Gaus->GetParameter(1), f_Gaus->GetParameter(0));

            c2->Print(Form("Figs/TDC_Counts_%d.pdf", curCh));

        }

        c2->Print(Form("Figs/TDC_Counts_%d.pdf]", curCh));
        c1->Print(Form("Figs/Raw_TDC_Run_%d_Component%d.pdf]", run, curCh));
    }




    gDirectory->Write();
}

