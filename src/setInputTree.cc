#include "../interface/setInputTree.h"

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

setInputTree::setInputTree(TFile* inputFile, std::string inputTreeName)
{
  if(inputFile == 0) {
    TFile* f = new TFile("/lustre/hep/jfaulkne/ECAL_testbeam_June2015_CeF3/DataTree/Run2542_100GeV_Center_NormalAng.root");
    fChain = (TTree*) f -> Get("H4tree");
  }
  else fChain = (TTree*) inputFile -> Get(inputTreeName.c_str());
  Init();
}

setInputTree::~setInputTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t setInputTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
  if (!fChain)  return 0; 
   return fChain->GetEntry(entry);
}
Long64_t setInputTree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void setInputTree::Init()
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
   fChain->SetBranchAddress("spillNumber", &spillNumber, &b_spillNumber);
   fChain->SetBranchAddress("evtNumber", &evtNumber, &b_evtNumber);
   fChain->SetBranchAddress("evtTimeDist", &evtTimeDist, &b_evtTimeDist);
   fChain->SetBranchAddress("evtTimeStart", &evtTimeStart, &b_evtTimeStart);
   fChain->SetBranchAddress("nEvtTimes", &nEvtTimes, &b_nEvtTimes);
   fChain->SetBranchAddress("evtTime", evtTime, &b_evtTime); // [nEvtTimes]
   fChain->SetBranchAddress("evtTimeBoard", evtTimeBoard, &b_evtTimeBoard); // [nEvtTimes]
   fChain->SetBranchAddress("nAdcChannels", &nAdcChannels, &b_nAdcChannels);
   fChain->SetBranchAddress("adcBoard", adcBoard, &b_adcBoard); // [nAdcChannels]
   fChain->SetBranchAddress("adcChannel", adcChannel, &b_adcChannel); // [nAdcChannels]
   fChain->SetBranchAddress("adcData", adcData, &b_adcData); // [nAdcChannels]
   fChain->SetBranchAddress("nTdcChannels", &nTdcChannels, &b_nTdcChannels);
   fChain->SetBranchAddress("tdcBoard", tdcBoard, &b_tdcBoard); // [nTdcChannels]
   fChain->SetBranchAddress("tdcChannel", tdcChannel, &b_tdcChannel); // [nTdcChannels]
   fChain->SetBranchAddress("tdcData", tdcData, &b_tdcData); // [nTdcChannels]
   fChain->SetBranchAddress("nDigiSamples", &nDigiSamples, &b_nDigiSamples);
   fChain->SetBranchAddress("digiFrequency", digiFrequency, &b_digiFrequency); // [nDigiSamples]
   fChain->SetBranchAddress("digiGroup", digiGroup, &b_digiGroup); // [nDigiSamples]
   fChain->SetBranchAddress("digiChannel", digiChannel, &b_digiChannel); // [nDigiSamples]
   fChain->SetBranchAddress("digiSampleIndex", digiSampleIndex, &b_digiSampleIndex); // [nDigiSamples]
   fChain->SetBranchAddress("digiSampleValue", digiSampleValue, &b_digiSampleValue); // [nDigiSamples]
   fChain->SetBranchAddress("digiBoard", digiBoard, &b_digiBoard); // [nDigiSamples]
   fChain->SetBranchAddress("nScalerWords", &nScalerWords, &b_nScalerWords);
   fChain->SetBranchAddress("scalerWord", scalerWord, &b_scalerWord); // [nScalerWords]
   fChain->SetBranchAddress("scalerBoard", scalerBoard, &b_scalerBoard); // [nScalerWords]
   fChain->SetBranchAddress("nPatterns", &nPatterns, &b_nPatterns);
   fChain->SetBranchAddress("pattern", pattern, &b_pattern); // [nPatterns]
   fChain->SetBranchAddress("patternBoard", patternBoard, &b_patternBoard); // [nPatterns]
   fChain->SetBranchAddress("patternChannel", patternChannel, &b_patternChannel); // [nPatterns]
   fChain->SetBranchAddress("nTriggerWords", &nTriggerWords, &b_nTriggerWords);
   fChain->SetBranchAddress("triggerWords", triggerWords, &b_triggerWords); // [nTriggerWords]
   fChain->SetBranchAddress("triggerWordsBoard", &triggerWordsBoard, &b_triggerWordsBoard);

}

Bool_t setInputTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void setInputTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t setInputTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
