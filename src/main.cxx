#include "data_analyser.h"
#include "eff_calib_main.h"
#include "TApplication.h"

Double_t GetPeakAreaFromAnalyser (const string& filename, Double_t peak, Double_t range) {
  cout << filename[0] << ": " << endl;
  class DataAnalyser *analyser = new class DataAnalyser();
  analyser->SetSpectrumData(filename, 4);
  analyser->SetHistogram();
  // analyser->FindPeaks();
  // analyser->SetBackground();
  analyser->SetCuttedHistogram(peak, range);
  analyser->ProcessValues("pic" + string(1, filename[0]) + "spec.png");
  analyser->PrintValues();

  return analyser->GetValues().GetPeakArea();
}

void Do () {
  class DataAnalyser *analyser = new class DataAnalyser();
  analyser->SetSpectrumData("1spec.txt", 4);
  analyser->SetHistogram();
  analyser->GetHistogram()->Draw();
  // analyser->FindPeaks(0.1, PeakFinderOptions::inCuttedHistogram);
  // cout << analyser->GetPeaks() << endl;
  // analyser->ProcessValues("channels.png");
  // analyser->PrintValues();
}

vector<Double_t> GetEffForMyDist (const string& filename, const vector<Double_t>& energies) {
  vector<Double_t> distances = ParseFileWithDistances(filename);
  Int_t nlines_to_skip = 5;
  DataStorage storage = GetStorageOfData(distances, nlines_to_skip);
  
  vector<Double_t> fit_params = {15, -1};
  map<Double_t, Parameters> distance_to_params = GetParametersFromFit(storage, fit_params);

  Data data;
  data.SetXaxisData(distances);
  // Сделать вектор энергий и, возможно, убрать первое расстояние
  data.SetYaxisData(GetEfficienciesForAllDistances(distance_to_params, energies[0]));
  WriteDataInFile(data, "data.csv");

  DataFitter *fitter = new DataFitter();
  fitter->SetFitFunction("pol2(0)", data.GetXmin(), data.GetXmax());
  fitter->FitData(data, "eff_from_dist.png");

  vector<Double_t> dist_to_eff_params = fitter->GetFitParameters();

  Double_t found_efficiency = dist_to_eff_params[0] * TMath::Exp(-dist_to_eff_params[1]*70.4) + dist_to_eff_params[2];

  // Тут тоже взять найденные энергии 
  cout << "Efficiency found: " << found_efficiency << endl;
  Double_t eff_for_40_dist = exp(distance_to_params[40][0]) * pow(energies[4], distance_to_params[40][1]);
  Double_t eff_for_30_dist = exp(distance_to_params[30][0]) * pow(energies[5], distance_to_params[30][1]);
  
 // return vector<Double_t>{found_efficiency, found_efficiency, found_efficiency, found_efficiency, eff_for_40_dist, eff_for_30_dist};
  delete fitter;
  
  return vector<Double_t>{found_efficiency, found_efficiency, found_efficiency, found_efficiency, eff_for_40_dist, eff_for_30_dist};
}

vector<Double_t> GetPeakPositions (Double_t peak_pos, const vector<string>& names) {
  vector<Double_t> peaks;
  for (const auto& name : names) {
    class DataAnalyser *analyser = new class DataAnalyser();
    analyser->SetSpectrumData(name, 4);
    analyser->SetHistogram();
    analyser->SetCuttedHistogram(peak_pos, 20);
    analyser->FindPeaks(0.1, PeakFinderOptions::inCuttedHistogram);  
    Double_t peak = analyser->GetPeaks()[0];
    peaks.push_back(peak);
    delete analyser;
  }

  return peaks;
}

vector<Double_t> GetIntensities (const vector<Double_t>& areas, const vector<Double_t>& time_intervals, const vector<Double_t>& efficiencies) {
  vector<Double_t> intensities;
  for (size_t i = 0; i < areas.size(); i++) {
    intensities.push_back(areas[i] / time_intervals[i] / efficiencies[i]);
  }

  return intensities;
}

int main (int argc, char* argv[]) {
  TApplication* app = new TApplication("app", &argc, argv);
  // gROOT->Reset();

  Double_t peak_pos, range, a, b, c;
  cout << "Type peak position: ";
  cin >> peak_pos;
  cout << "Type peak range: ";
  cin >> range;
  cout << "Type decay curve fit parameters: ";
  cin >> a >> b >> c;

  vector<string> names = {
         "1spec.txt", "2spec.txt", "3spec.txt", "4spec.txt", "5spec.txt", "6spec.txt"
     };

  TCanvas *canvas = new TCanvas("canvas", "Spectrum", 1300, 900);
  Data testdata(names[0], 0);
  Histogram hist(testdata);
  CuttedHistogram cuttedhist(hist.GetHistogram(), 622, 120);
  TH1F *ch = cuttedhist.GetCuttedHistogram();
  ch->SetXTitle("Energy, keV");
  ch->SetYTitle("Intensity");
  ch->SetTitle("Spectrum");
  ch->Draw();
  canvas->SaveAs("spectrum.png");

  vector<Double_t> peaks = GetPeakPositions(peak_pos, names);

  vector<Double_t> areas;
  for (size_t i = 0; i < names.size(); i++) {
    areas.push_back(GetPeakAreaFromAnalyser(names[i], peaks[i], range));
  }

  vector<Double_t> time_intervals = {600, 1800, 3000, 4800, 6600, 8400};

  //vector<Double_t> intensities = GetIntensities(areas, {600, 1200, 1200, 1800, 1800, 1800}, GetEffForMyDist("distances", peaks)); 
  vector<Double_t> intensities = GetIntensities(areas, time_intervals, GetEffForMyDist("distances", peaks)); 

  //cout << GetEffForMyDist("distances") << endl;
  Data data;
  data.SetXaxisData(time_intervals);
  data.SetYaxisData(intensities);
  WriteDataInFile(data, "decay_curve_data.csv");
  //data.DrawData("decaycurve", "time", "intensity");
  DataFitter *fitter = new DataFitter();
  vector<Double_t> params = {a, b, c};
  fitter->SetFitFunction("[0]*TMath::Exp([1]*x)+[2]", data.GetXmin(), data.GetXmax());
  fitter->SetFitParameters(params);
  fitter->FitData(data, "fitted_data.png");

  //Do();

  cout << "HELLO" << endl;

  app->Run();

  return 0;
}
