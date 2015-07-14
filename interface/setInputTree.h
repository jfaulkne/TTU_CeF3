#ifndef setInputTree_h
#define setInputTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <iostream>
#include <TTree.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.
class setInputTree {


public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain


   // Declaration of leaf types
   UInt_t	runNumber;
   UInt_t	spillNumber;
   UInt_t	evtNumber;
   Int_t	evtTimeDist;
   Int_t	evtTimeStart;
   Int_t	nEvtTimes;
   Long64_t	evtTime[2]; // [nEvtTimes]
   Int_t	evtTimeBoard[2]; // [nEvtTimes]
   Int_t	nAdcChannels;
   Int_t	adcBoard[40]; // [nAdcChannels]
   Int_t	adcChannel[40]; // [nAdcChannels]
   Int_t	adcData[40]; // [nAdcChannels]
   Int_t	nTdcChannels;
   Int_t	tdcBoard[16]; // [nTdcChannels]
   Int_t	tdcChannel[16]; // [nTdcChannels]
   Int_t	tdcData[16]; // [nTdcChannels]
   Int_t	nDigiSamples;
   Int_t	digiFrequency[9216]; // [nDigiSamples]
   Int_t	digiGroup[9216]; // [nDigiSamples]
   Int_t	digiChannel[9216]; // [nDigiSamples]
   Int_t	digiSampleIndex[9216]; // [nDigiSamples]
   Float_t	digiSampleValue[9216]; // [nDigiSamples]
   Int_t	digiBoard[9216]; // [nDigiSamples]
   Int_t	nScalerWords;
   Int_t	scalerWord[8]; // [nScalerWords]
   Int_t	scalerBoard[8]; // [nScalerWords]
   Int_t	nPatterns;
   Int_t	pattern[8]; // [nPatterns]
   Int_t	patternBoard[8]; // [nPatterns]
   Int_t	patternChannel[8]; // [nPatterns]
   Int_t	nTriggerWords;
   Int_t	triggerWords[1]; // [nTriggerWords]
   Int_t	triggerWordsBoard[1]; // [nTriggerWords]


   // List of branches
   TBranch	*b_runNumber;
   TBranch	*b_spillNumber;
   TBranch	*b_evtNumber;
   TBranch	*b_evtTimeDist;
   TBranch	*b_evtTimeStart;
   TBranch	*b_nEvtTimes;
   TBranch	*b_evtTime; // [nEvtTimes]
   TBranch	*b_evtTimeBoard; // [nEvtTimes]
   TBranch	*b_nAdcChannels;
   TBranch      *b_adcBoard; // [nAdcChannels]
   TBranch      *b_adcChannel; // [nAdcChannels]
   TBranch      *b_adcData; // [nAdcChannels]
   TBranch      *b_nTdcChannels;
   TBranch      *b_tdcBoard; // [nTdcChannels]
   TBranch      *b_tdcChannel; // [nTdcChannels]
   TBranch      *b_tdcData; // [nTdcChannels]
   TBranch      *b_nDigiSamples;
   TBranch      *b_digiFrequency; // [nDigiSamples]
   TBranch      *b_digiGroup; // [nDigiSamples]
   TBranch      *b_digiChannel; // [nDigiSamples]
   TBranch      *b_digiSampleIndex; // [nDigiSamples]
   TBranch      *b_digiSampleValue; // [nDigiSamples]
   TBranch      *b_digiBoard; // [nDigiSamples]
   TBranch      *b_nScalerWords;
   TBranch      *b_scalerWord; // [nScalerWords]
   TBranch      *b_scalerBoard; // [nScalerWords]
   TBranch      *b_nPatterns;
   TBranch      *b_pattern; // [nPatterns]
   TBranch      *b_patternBoard; // [nPatterns]
   TBranch      *b_patternChannel; // [nPatterns]
   TBranch      *b_nTriggerWords;
   TBranch      *b_triggerWords; // [nTriggerWords]
   TBranch      *b_triggerWordsBoard;


   setInputTree(TFile* inputFile, std::string inputTree);
   virtual ~setInputTree();
   Int_t    Cut(Long64_t entry);
   Int_t    GetEntry(Long64_t entry);
   Long64_t LoadTree(Long64_t entry);
   void     Init();
   //   virtual void     Loop();
   Bool_t   Notify();
   void     Show(Long64_t entry = -1);
};

#endif 
