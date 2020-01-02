#pragma once

// C++ includes

#include <utility>
#include <map>

#include "classes.h"

using PeakToValues = map<Double_t, Values>;

enum class PeakFinderOptions {
  inHistogram,
  inCuttedHistogram,
  noSelectedOption
};

enum class SetBackgroundOption {
  ofHistogram,
  ofCuttedHistogram,
  noSelectedOption
};

// Definition of DataAnalyser and StorageForValues

class DataAnalyser {
public:
  void SetSpectrumData (const string&, Int_t);

  void SetHistogram ();

  void SetBackground (
      SetBackgroundOption option);

  void SetCuttedHistogram (Double_t, Int_t);

  void FindPeaks (
      Double_t, PeakFinderOptions option);

  void ProcessValues (const string&);

  pair<Energy, Intensity> GetSpectrum () const;

  TH1F* GetHistogram () const;

  TH1F* GetBackground () const;

  TH1F* GetCuttedHistogram () const;

  Peaks GetPeaks () const;

  Values GetValues () const;

  void PrintValues () const;

private:
  SpectrumData spec;
  Histogram hist;
  Background bg;
  CuttedHistogram ch;
  PeakFinder pf;
  Values values;
};

class StorageForValues {
public:
  StorageForValues ();

  StorageForValues (class DataAnalyser*);

  void PeaksIntegral (class DataAnalyser*);

  PeakToValues GetPeakToValues () const;

  void Print () const;

private:
  PeakToValues peak_to_values;
};
