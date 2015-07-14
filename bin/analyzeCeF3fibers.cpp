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
  std::string inputFile = argv[1];
  std::string outputFile = argv[2];
  std::string numberOfEntries = argv[3];
  std::string inputTreeName = "H4tree";

  // Open input file and extract tree:
  std::cout<<"Input File: "<<inputFile<<std::endl;
  TFile* MyFile = TFile::Open(inputFile.c_str());
  setInputTree* DataTree = new setInputTree (MyFile, inputTreeName.c_str());
  if (DataTree->fChain == 0) return (-1);
  DataTree->Init();

  //---------output tree----------------
  TFile* outROOT = TFile::Open((std::string("output/")+outputFile).c_str(),"recreate");
  outROOT->cd();

  set<string> CH_hist_Set;

  Int_t maxTS = 1000;
  Int_t maxCH = 9;
  Int_t maxPlot = 100;

  Long64_t maxEvt = DataTree->fChain->GetEntries();
  maxEvt = ((Long64_t)std::atof(numberOfEntries.c_str()) > maxEvt || (Long64_t)std::atof(numberOfEntries.c_str()) <= 0)?
     maxEvt : (Long64_t)std::atof(numberOfEntries.c_str());

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

    // Loop over each digi sample:
    for (Int_t sample = 0; sample < DataTree->nDigiSamples; sample++) {

      if (DataTree->digiGroup[sample] != 0) continue;
      if (DataTree->digiSampleIndex[sample] >= maxTS) continue;

      TH1F* CH_hist = 0;
      std::string histName = inputFile.substr(inputFile.find("Run"),inputFile.find(".root")-inputFile.find("Run"));
      histName += "_Channel"+IntToString(DataTree->digiChannel[sample]);

      std::string histTitle = histName + ";Time Slice";
      CH_hist = (TH1F*)gDirectory->GetList()->FindObject(histName.c_str());
      if (!CH_hist){

        CH_hist = new TH1F(histName.c_str(), histTitle.c_str(), maxTS, 0, maxTS);
        CH_hist->GetYaxis()->SetTitle("Signal");
        CH_hist->Sumw2();
        CH_hist_Set.insert(histName);
    
      }

      CH_hist->AddBinContent(DataTree->digiSampleIndex[sample], DataTree->digiSampleValue[sample]/maxEvt);
      Signal[DataTree->digiChannel[sample]][DataTree->digiSampleIndex[sample]] = DataTree->digiSampleValue[sample];
      TimeSlice[DataTree->digiChannel[sample]][DataTree->digiSampleIndex[sample]] = DataTree->digiSampleIndex[sample];

    }// End loop over digi samples

    if ((Int_t)jentry < maxPlot)
      for (Int_t CH = 0; CH < maxCH; CH++){

        TGraph* CH_pulse = new TGraph(maxTS, &TimeSlice[CH][0], &Signal[CH][0]);
        CH_pulse->SetName(Form("CH%d",CH));
        mg[CH]->Add(CH_pulse);

      }// End loop over channels

  }// End loop over events

  for (set<string>::const_iterator itr = CH_hist_Set.begin(); itr != CH_hist_Set.end(); ++itr)
    gDirectory->GetList()->FindObject(itr->c_str())->Write();


  TCanvas* canvas[maxCH];
  for (Int_t CH = 0; CH < maxCH; CH++){

    canvas[CH] = new TCanvas(Form("CH%d",CH), Form("Channel %d Pulses (%d Events)", CH, (Int_t)maxEvt), 600, 400);
    mg[CH]->Draw("APL");
    mg[CH]->SetTitle(Form("CH%d (%d Events)",CH,(maxPlot < (Int_t)maxEvt)? maxPlot:(Int_t)maxEvt));
    mg[CH]->GetXaxis()->SetTitle("Time Slice");
    mg[CH]->GetYaxis()->SetTitle("Signal");
    canvas[CH]->Write();

  }// End loop over pulse plots

  //--------close everything-------------
  DataTree->fChain->Delete();
  outROOT->Close();

  return(0);

}// End main program
