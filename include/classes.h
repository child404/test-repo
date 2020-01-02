#pragma once

// ROOT includes

#include "TH1F.h"
#include "TH1.h"
#include "TSpectrum.h"
#include "functions.h"

#include "mother_classes.h"

using Peaks = vector<Double_t>;
using Energy = vector<Double_t>;
using Intensity = vector<Double_t>;

/* Definition of classes: SpectrumData, SpectrumDataParser, 
 Histogram, Background, CuttedHistogram, PeakFinder and Values */

class SpectrumData : public Data {
public:
  SpectrumData ();

  SpectrumData (const string&, Int_t);

  void ParseFileWithData (const string&, Int_t) override;

};

class Histogram {
public:
  Histogram ();

  Histogram (const Data&);

  void SetHistogram (const Data&);

  TH1F* GetHistogram () const;

  void DrawHistogram () const;

  // void DrawHistWithBackground () const {
  //     TCanvas *c3 = new TCanvas("c", "c", 10, 10, 1000, 900);
  //     hist->Draw("hist");
  //     if (background.GetBackground()) { 
  //         c3->Update();
  //     }
  // }

private:
  TH1F *hist;
  // Background background;
  // CuttedHist cutted_hist;
  // vector<Double_t> peaks;
};

class Background {
public:
  Background ();

  Background (TH1F*);

  void SetBackground (TH1F*);

  TH1F* GetBackground () const;

private:
  TH1F *background;
};

class CuttedHistogram {
public:
  CuttedHistogram ();

  CuttedHistogram (TH1F*, Double_t, Int_t);

  void SetCuttedHistogram (TH1F*, Double_t, Int_t);

  TH1F* GetCuttedHistogram () const;

private:
  TH1F *cutted_hist;
};

class PeakFinder {
public:
  PeakFinder ();

  PeakFinder (TH1F*, Double_t);

  void FindPeaks (TH1F*, Double_t);

  vector<Double_t> GetPeaks () const;

private:
  Peaks peaks;
};

class Values {
public:
  Values ();

  Values (TH1F*, const string&);

  void ProcessPeak (TH1F*, const string&);

  vector<Double_t> GetValues () const;

  Double_t GetPeakArea () const;

  Double_t GetPeakAreaError () const;

  void Print () const;

private:
  Float_t GetArea (TF1*, Double_t*) const;

  Float_t GetPeakAreaError (const TF1*, Double_t*, Float_t) const;

  Double_t peak;
  Double_t peak_error;
  Double_t peak_area;
  Double_t peak_area_error;
  Double_t background;
};
