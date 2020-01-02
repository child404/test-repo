#include "eff_calib_main.h"

void WriteDataInFile (const Data& data, const string& filename="data.csv") {
  vector<Double_t> x = data.GetXaxisData(); 
  vector<Double_t> y = data.GetYaxisData();

  ofstream st;
  st.open(filename);
  st << "x" << "," << "y" << endl;

  for (size_t i = 0; i < x.size(); i++) {
    st << x[i] << "," << y[i] << endl;
  }

  st.close();
}

vector<Double_t> ParseFileWithDistances (const string& filename) {
  ifstream file;
  file.open(filename);

  vector<Double_t> result;
  string distance;
  while (getline(file, distance)) {
    result.push_back(stoi(distance));
  }

  return result;
}

DataStorage GetStorageOfData (const vector<Double_t>& distances, Int_t nlines_to_skip) {
  /* Название читаемого файла может не совпадать с реальным 
    названием (если расстояние не целочисленно) */
  // ДОБАВИТЬ СЧИТЫВАНИЕ ИМЁН ФАЙЛОВ ДЛЯ СЧИТЫВАНИЯ (ЛОЛ)
  DataStorage result;
  for (const auto& distance : distances) {
    EfficiencyCalibrationData data(to_string(Int_t(distance)) + ".txt", nlines_to_skip, distance);
    data.SetLogarithmicXaxisData();
    data.SetLogarithmicYaxisData();
    result[data.GetDistance()] = data;
  }

  return result;
}

map<Double_t, Parameters> GetParametersFromFit (
    const DataStorage& storage, const vector<Double_t>& fit_params) 
{
  /* Название сохраненной картинки может не совпадать с реальным 
    расстоянием (если оно не целочисленно) */
  map<Double_t, Parameters> result;
  for (const auto& item : storage) {
    EfficiencyCalibrationDataFitter *fitter = 
        new EfficiencyCalibrationDataFitter();
    Double_t distance = item.first;
    EfficiencyCalibrationData data = item.second;

    fitter->SetFitFunction("[1]*x+[0]", data.GetXmin(), data.GetXmax());
    // fitter->SetFitParameters(fit_params);
    fitter->FitData(data, to_string(Int_t(distance)) + ".png");

    result[distance] = fitter->GetFitParameters();
  }

  return result;
}

vector<Double_t> GetEfficienciesForAllDistances (
    const map<Double_t, Parameters>& distance_to_params, Double_t energy=622)
{
  vector<Double_t> efficiencies;
  for (const auto& item : distance_to_params) {
    Double_t a_value = item.second[0];
    Double_t b_value = item.second[1];
    efficiencies.push_back(exp(a_value) * pow(energy, b_value));
  }

  return efficiencies;
}

Double_t GetEfficienciesForMyDistances (
    const vector<Double_t>& fit_params, const Double_t* my_distances);
