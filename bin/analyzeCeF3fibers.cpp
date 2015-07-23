#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <iomanip>
#include <ctime>
#include <map>
#include <math.h>

#include "TROOT.h"
#include "TH2D.h"
#include "TFile.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TMultiGraph.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TChain.h"
#include "TProfile.h"
#include "TMath.h"
#include "TString.h"
#include "TClass.h"
#include "TApplication.h"
#include "TLorentzVector.h"

#include "../interface/setInputTree.h"
#include "../interface/tools.h"

using namespace std;

//*******MAIN*******************************************************************

int main (int argc, char** argv)
{

  // User input parameters: 
  string inputFile = argv[1];
  string outputFile = argv[2];
  string numberOfEntries = argv[3];
  string inputTreeName = "H4tree";

  // Open input file and extract tree:
  cout<<"Input File: "<<inputFile<<endl;
  TFile* MyFile = TFile::Open(inputFile.c_str());
  setInputTree* DataTree = new setInputTree (MyFile, inputTreeName.c_str());
  if (DataTree->fChain == 0) return (-1);
  DataTree->Init();

  //---------output tree----------------
  TFile* outROOT = TFile::Open((string("output/")+outputFile).c_str(),"recreate");
  outROOT->cd();

  set<string> CH_hist_Set, CH_hist_2d_Set, CH_hist_Pulse_Set, CH_hist_Integrals_Set;

  Int_t maxTS = 1000;
  Int_t maxCH = 9;
  Int_t maxPlot = 100;
  Int_t peaks_threshold = 3000;
  Int_t BKG_cut = 100;
  Int_t LDG_PK_Cut1 = 160;
  Int_t LDG_PK_Cut2 = 200;
  Int_t SCND_PK_Cut1 = 200;
  Int_t SCND_PK_Cut2 = maxTS;
  bool graph_pulses[maxCH]; for (Int_t CH = 0; CH < maxCH; CH++) graph_pulses[CH] = false;

  Long64_t maxEvt = DataTree->fChain->GetEntries();
  maxEvt = ((Long64_t)atof(numberOfEntries.c_str()) > maxEvt || (Long64_t)atof(numberOfEntries.c_str()) <= 0)?
     maxEvt : (Long64_t)atof(numberOfEntries.c_str());

  TMultiGraph* mg[maxCH];
  for (Int_t CH = 0; CH < maxCH; CH++) mg[CH] = new TMultiGraph();

  //---------start loop on events------------
  for (Long64_t jentry = 0; jentry < maxEvt; jentry++) {

    Long64_t iEntry = DataTree->LoadTree(jentry);
    if (iEntry < 0) break;
    int nb = DataTree->fChain->GetEntry(jentry);   

    if(iEntry % 1000 == 0)    
      cout << "Read entry: " << iEntry << endl;

    Double_t Signal[maxCH][maxTS];
    Double_t TimeSlice[maxCH][maxTS];

    Double_t Background_avg[maxCH];
    Double_t Peak_integral[maxCH][2];

    Int_t Npeaks[maxCH];
    bool Triggered[maxCH][2];
    for (Int_t CH = 0; CH < maxCH; CH++){
      Npeaks[CH] = 0; Triggered[CH][0] = false; Triggered[CH][1] = false;
      Background_avg[CH] = 0; Peak_integral[CH][0] = 0; Peak_integral[CH][1] = 0;
    }

    // Loop over each digi sample:
    for (Int_t sample = 0; sample < DataTree->nDigiSamples; sample++) {

      if (DataTree->digiGroup[sample] != 0) continue;
      if (DataTree->digiSampleIndex[sample] >= maxTS) continue;
      if (DataTree->digiSampleValue[sample] < peaks_threshold) Triggered[DataTree->digiChannel[sample]][0] = true;
      if (DataTree->digiSampleIndex[sample] < 200 && Triggered[DataTree->digiChannel[sample]][0])
        Triggered[DataTree->digiChannel[sample]][1] = true;

      TH1F* CH_hist = 0;
      string histName = inputFile.substr(inputFile.find("Run"),inputFile.find(".root")-inputFile.find("Run"));
      histName += "_Channel"+IntToString(DataTree->digiChannel[sample]);
      CH_hist = (TH1F*)gDirectory->GetList()->FindObject(histName.c_str());

      TH2D* CH_hist_2d = 0;
      string histName_2d = histName+"_2d";
      CH_hist_2d = (TH2D*)gDirectory->GetList()->FindObject(histName_2d.c_str());

      if (!CH_hist){

        string histTitle = histName + ";Time Slice";
        CH_hist = new TH1F(histName.c_str(), histTitle.c_str(), maxTS, 0, maxTS);
        CH_hist->GetYaxis()->SetTitle("Signal");
        CH_hist->Sumw2();
        CH_hist_Set.insert(histName);
    
      }

      if (!CH_hist_2d){

        CH_hist_2d = new TH2D(histName_2d.c_str(), histName_2d.c_str(), maxTS, 0, maxTS, maxPlot, 0, maxPlot);
        CH_hist_2d->GetZaxis()->SetTitle("Signal");
        CH_hist_2d->GetXaxis()->SetTitle("Time Slice");
        CH_hist_2d->GetYaxis()->SetTitle("Event");
        CH_hist_2d->Sumw2();
        CH_hist_2d_Set.insert(histName_2d);

      }

      CH_hist->AddBinContent(DataTree->digiSampleIndex[sample], DataTree->digiSampleValue[sample]/maxEvt);
      if ((Int_t)jentry < maxPlot)
        CH_hist_2d->SetBinContent(DataTree->digiSampleIndex[sample], (Int_t)jentry, DataTree->digiSampleValue[sample]);

      Signal[DataTree->digiChannel[sample]][DataTree->digiSampleIndex[sample]] = DataTree->digiSampleValue[sample];
      TimeSlice[DataTree->digiChannel[sample]][DataTree->digiSampleIndex[sample]] = DataTree->digiSampleIndex[sample];

      if (DataTree->digiSampleIndex[sample] < BKG_cut)
        Background_avg[DataTree->digiChannel[sample]] += DataTree->digiSampleValue[sample]/BKG_cut;

      else if (DataTree->digiSampleIndex[sample] > LDG_PK_Cut1 && DataTree->digiSampleIndex[sample] < LDG_PK_Cut2)
        Peak_integral[DataTree->digiChannel[sample]][0] +=
          (Background_avg[DataTree->digiChannel[sample]] - DataTree->digiSampleValue[sample]);

      else if (DataTree->digiSampleIndex[sample] > SCND_PK_Cut1 && DataTree->digiSampleIndex[sample] < SCND_PK_Cut2)
        Peak_integral[DataTree->digiChannel[sample]][1] += 
          (Background_avg[DataTree->digiChannel[sample]] - DataTree->digiSampleValue[sample]);

    }// End loop over digi samples

    for (Int_t CH = 0; CH < maxCH; CH++){

      TH1F* CH_hist = 0;
      string histName = inputFile.substr(inputFile.find("Run"),inputFile.find(".root")-inputFile.find("Run"));
      histName += "_Channel"+IntToString(CH)+"_Pulses";

      string histTitle = histName + ";# Pulses";
      CH_hist = (TH1F*)gDirectory->GetList()->FindObject(histName.c_str());
      if (!CH_hist){

        CH_hist = new TH1F(histName.c_str(), histTitle.c_str(), 3, 0, 3);
        CH_hist->GetYaxis()->SetTitle("# Events");
        CH_hist->Sumw2();
        CH_hist_Pulse_Set.insert(histName);

      }

      TH2D* CH_hist_Int = 0;
      string histName_Int = inputFile.substr(inputFile.find("Run"),inputFile.find(".root")-inputFile.find("Run"));
      histName_Int += "_Channel"+IntToString(CH)+"_Integrals";

      CH_hist_Int = (TH2D*)gDirectory->GetList()->FindObject(histName_Int.c_str());
      if (!CH_hist_Int){

        CH_hist_Int = new TH2D(histName_Int.c_str(), histName_Int.c_str(), 1000, 0, 500000, 1000, 0, 50000);
        CH_hist_Int->GetYaxis()->SetTitle("Leading Pulse Integral");
        CH_hist_Int->GetXaxis()->SetTitle("Second Pulse Integral");
        CH_hist_Int->Sumw2();
        CH_hist_Integrals_Set.insert(histName_Int);

      }

      if (Triggered[CH][0] && Triggered[CH][1]){

        Npeaks[CH] = 2;
        CH_hist_Int->Fill(Peak_integral[CH][1], Peak_integral[CH][0]);

      } else if (Triggered[CH][0] || Triggered[CH][1]) Npeaks[CH] = 1;

      CH_hist->Fill(Npeaks[CH]);

      if (CH_hist->GetBinContent(3) < maxPlot && Triggered[CH][1]){

        TGraph* CH_pulse = new TGraph(maxTS, &TimeSlice[CH][0], &Signal[CH][0]);
        CH_pulse->SetName(Form("CH%d",CH));
        mg[CH]->Add(CH_pulse);
        graph_pulses[CH] = true;

      }

    }// End loop over channels

  }// End loop over events

  TCanvas* canvas[maxCH];
  Int_t CH = 0;
  for (set<string>::const_iterator itr = CH_hist_Set.begin(); itr != CH_hist_Set.end(); ++itr)
    gDirectory->GetList()->FindObject(itr->c_str())->Write();
  for (set<string>::const_iterator itr = CH_hist_2d_Set.begin(); itr != CH_hist_2d_Set.end(); ++itr)
    gDirectory->GetList()->FindObject(itr->c_str())->Write();
  for (set<string>::const_iterator itr = CH_hist_Integrals_Set.begin(); itr != CH_hist_Integrals_Set.end(); ++itr)
    gDirectory->GetList()->FindObject(itr->c_str())->Write();
  for (set<string>::const_iterator itr = CH_hist_Pulse_Set.begin(); itr != CH_hist_Pulse_Set.end(); ++itr){

    TH1F* CH_hist = 0;
    CH_hist = (TH1F*)gDirectory->GetList()->FindObject(itr->c_str());
    if (graph_pulses[CH]){

      canvas[CH] = new TCanvas(Form("CH%d",CH), Form("Channel %d Pulses (%.0f Events)", CH,\
        (CH_hist->GetBinContent(3) > maxPlot)? ((maxPlot < (Int_t)maxEvt)? maxPlot:(Int_t)maxEvt) :
          CH_hist->GetBinContent(3)), 600, 400);
      mg[CH]->Draw("APL");
      mg[CH]->SetTitle(Form("CH%d (%.0f Events)",CH,(CH_hist->GetBinContent(3) > maxPlot)?
        ((maxPlot < (Int_t)maxEvt)? maxPlot:(Int_t)maxEvt) : 
          CH_hist->GetBinContent(3)));
      mg[CH]->GetXaxis()->SetTitle("Time Slice");
      mg[CH]->GetYaxis()->SetTitle("Signal");
      canvas[CH]->Write();

    }

    CH_hist->Write();
    CH++;

  }// End loop over pulse plots

  //--------close everything-------------
  DataTree->fChain->Delete();
  outROOT->Close();

  return(0);

}// End main program
