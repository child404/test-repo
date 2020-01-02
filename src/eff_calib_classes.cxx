#include "eff_calib_classes.h"

/* EfficiencyCalibrationData */

EfficiencyCalibrationData::EfficiencyCalibrationData () 
  : Data() {
}

EfficiencyCalibrationData::EfficiencyCalibrationData (
    const string& filename, Int_t nlines_to_skip, Int_t d) : distance(d)
{
  ParseFileWithData(filename, nlines_to_skip);
}

void EfficiencyCalibrationData::ParseFileWithData (const string& filename, Int_t nlines_to_skip=0) {
  ifstream file;
  file.open(filename);
  SkipFirstNlines(file, nlines_to_skip);

  string line;
  while (getline(file, line)) {
    stringstream ss(line);
    string x_value, y_value, y_error;
    ss >> x_value >> y_value >> y_error;
    x_axis_data.push_back(stod(x_value));
    y_axis_data.push_back(stod(y_value));
    y_errors.push_back(stod(y_error));
  }
}

void EfficiencyCalibrationData::SetLogarithmicXaxisData () {
  size_t points = x_axis_data.size();
  for (Int_t i = 0; i < points; i++) {
    x_axis_data[i] = log(x_axis_data[i]);
  }
}

void EfficiencyCalibrationData::SetLogarithmicYaxisData () {
  size_t points = y_axis_data.size();
  for (Int_t i = 0; i < points; i++) {
    y_axis_data[i] = log(y_axis_data[i]);
  }
}

void EfficiencyCalibrationData::SetDistance (Int_t distance) {
  this->distance = distance;
}

Int_t EfficiencyCalibrationData::GetDistance () const {
  return distance;
}

void EfficiencyCalibrationData::SetYerrors (const vector<Double_t> y_errors) {
  this->y_errors = y_errors;
}

vector<Double_t> EfficiencyCalibrationData::GetYerrors () const {
  return y_errors;
}

/* EfficiencyCalibrationDataFitter */

EfficiencyCalibrationDataFitter::EfficiencyCalibrationDataFitter () 
  : DataFitter() {
}

void EfficiencyCalibrationDataFitter::FitData(const EfficiencyCalibrationData& data, const string& picname="") {
  TCanvas *canvas = new TCanvas("canvas","Efficiency calibration",800,600);

  TGraphErrors *graph = new TGraphErrors(data.GetXaxisData().size(),data.GetXaxisData().data(),
                                         data.GetYaxisData().data(),0,data.GetYerrors().data());
  graph->SetMarkerStyle(22);
  graph->Fit("fit_function");
  graph->Draw("A*");

  if (picname != "") {
    canvas->SaveAs(picname.c_str());
  }

  UpdateFitParameters();
}

/* DecayCurveBuilder */

DecayCurveBuilder::DecayCurveBuilder () {}

DecayCurveBuilder::DecayCurveBuilder (
    const vector<Double_t>& time, const vector<Double_t>& eff, const vector<Double_t>& areas_of_peaks) 
{
  SetDecayCurve(time, eff, areas_of_peaks);
}

void DecayCurveBuilder::SetDecayCurve (
    const vector<Double_t>& time, const vector<Double_t>& eff, const vector<Double_t>& areas_of_peaks) 
{
  size_t points = time.size();
  for (Int_t i = 0; i < points; i++) {
    decay_curve->SetPoint(i, time[i], areas_of_peaks[i] / time[i] / eff[i]);
  }
}

TGraph* DecayCurveBuilder::GetDecayCurve () const {
  return decay_curve;
}
