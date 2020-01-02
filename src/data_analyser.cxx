#include "data_analyser.h"

/* DataAnalyser */

void DataAnalyser::SetSpectrumData (const string& filename, Int_t nlines_to_skip) {
  /* Для второго файла classes2.C */
  spec = SpectrumData(filename, nlines_to_skip);
}

void DataAnalyser::SetHistogram () {
  hist = Histogram(spec);
}

void DataAnalyser::SetBackground (
  SetBackgroundOption option=SetBackgroundOption::noSelectedOption) 
{
  switch (option) {
  case SetBackgroundOption::ofHistogram:
    bg = Background(hist.GetHistogram());
    break;
  case SetBackgroundOption::ofCuttedHistogram:
    bg = Background(ch.GetCuttedHistogram());
    break;
  default:
    cout << "No option was selected" << endl;
  }
}

void DataAnalyser::SetCuttedHistogram (Double_t peak_position, Int_t range) {
  /* Должна быть связаня с ProcessValues и наоборот */
  ch = CuttedHistogram(hist.GetHistogram(), peak_position, range);
}

void DataAnalyser::FindPeaks (Double_t resolution=0.1, PeakFinderOptions option=PeakFinderOptions::noSelectedOption) {
  switch (option) {
  case PeakFinderOptions::inHistogram:
    pf = PeakFinder(hist.GetHistogram(), resolution);
    break;
  case PeakFinderOptions::inCuttedHistogram:
    pf = PeakFinder(ch.GetCuttedHistogram(), resolution);
    break;
  default:
    cout << "No option was selected\n";
  }
}

void DataAnalyser::ProcessValues (const string& picname="") {
  values.ProcessPeak(ch.GetCuttedHistogram(), picname);
}

pair<Energy, Intensity> DataAnalyser::GetSpectrum () const {
  return make_pair(spec.GetXaxisData(), spec.GetYaxisData());
}

TH1F* DataAnalyser::GetHistogram () const {
  return hist.GetHistogram();
}

TH1F* DataAnalyser::GetBackground () const {
  return bg.GetBackground();
}

TH1F* DataAnalyser::GetCuttedHistogram () const {
  return ch.GetCuttedHistogram();
}

Peaks DataAnalyser::GetPeaks () const {
  return pf.GetPeaks();
}

Values DataAnalyser::GetValues () const {
  return values;
}

void DataAnalyser::PrintValues () const {
  values.Print();
}

/* StorageForValues */

StorageForValues::StorageForValues () {}

StorageForValues::StorageForValues (class DataAnalyser* analyser) {
  PeaksIntegral(analyser);
}

void StorageForValues::PeaksIntegral (class DataAnalyser* analyser) {
  // Анализатор может не знать, какие пики ему брать
  Peaks peaks = analyser->GetPeaks();

  for (const auto& peak : peaks) {
    analyser->SetCuttedHistogram(peak, 100);
    analyser->ProcessValues(to_string(peak));
    peak_to_values[peak] = analyser->GetValues();
  }
}

PeakToValues StorageForValues::GetPeakToValues () const {
  return peak_to_values;
}

void StorageForValues::Print () const {
  // Переписать
  for (const auto& item : peak_to_values) {
    cout << item.first << ":\n";
    item.second.Print(); 
  }
  cout << endl;
}
