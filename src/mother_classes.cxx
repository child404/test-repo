#include "mother_classes.h"

/* Abstract class Data */

Data::Data () {}

Data::Data (const string& filename, Int_t nlines_to_skip) {
  ParseFileWithData(filename, nlines_to_skip);
}

void Data::ParseFileWithData (const string& filename, Int_t nlines_to_skip) {
  ifstream file;
  file.open(filename);
  SkipFirstNlines(file, nlines_to_skip);

  string line;
  while (getline(file, line)) {
    stringstream ss(line);
    string x_value, y_value;
    ss >> x_value >> y_value;
    x_axis_data.push_back(stod(x_value));
    y_axis_data.push_back(stod(y_value));
  }
}

void Data::SetXaxisData (const vector<Double_t>& x_axis_data) {
  this->x_axis_data = x_axis_data;	
}

void Data::SetYaxisData (const vector<Double_t>& y_axis_data) {
  this->y_axis_data = y_axis_data;	
}

vector<Double_t> Data::GetXaxisData () const {
  return x_axis_data;
}

vector<Double_t> Data::GetYaxisData () const {
  return y_axis_data;
}

Double_t Data::GetXmax () const {
  return *max_element(begin(x_axis_data), end(x_axis_data));
}

Double_t Data::GetXmin () const {
  return *min_element(begin(x_axis_data), end(x_axis_data));
}

void Data::DrawData (
    const string& lable, const string& x_axis_name, const string& y_axis_name) const 
{
  TCanvas *c1 = new TCanvas("c", "SpectrumDatas", 1000, 900);
  string title = lable + "; " + x_axis_name + "; " + y_axis_name;
    
  size_t points = x_axis_data.size();
  TGraph *g = new TGraph();
  for (Int_t i = 0; i < points; i++) {
    g->SetPoint(i, x_axis_data[i], y_axis_data[i]);
  }
  g->SetMarkerStyle(7);
  g->SetLineColor(kBlue);
  g->SetTitle(&title[0]);
  g->Draw("A*");
  string canvname = lable + ".png"; 
  c1->SaveAs(&canvname[0]);
}

void Data::SkipFirstNlines (ifstream& file, Int_t nlines) const {
  string line;
  for (Int_t i = 0; i < nlines; i++) {
    getline(file, line);
    line.clear();
  }
}

TGraph* GetGraphFromData (const Data& data) {
  TGraph *result = new TGraph();
  vector<Double_t> x_axis_data = data.GetXaxisData();
  vector<Double_t> y_axis_data = data.GetYaxisData();
  size_t points = x_axis_data.size();
  for (Int_t i = 0; i < points; i++) {
    result->SetPoint(i, x_axis_data[i], y_axis_data[i]);
  }

  return result;
}

/* DataFitter */

void DataFitter::SetFitFunction (
    const string& function_str="pol1(3)", Double_t xmin=0, Double_t xmax=1) 
{
  fit_function = new TF1("fit_function", &function_str[0], xmin, xmax);
  fit_function->SetParNames("a", "b", "c");
}

void DataFitter::SetFitParameters (const vector<Double_t>& fit_params) {
  this->fit_params = fit_params;
  fit_function->SetParameters(&fit_params[0]);
  //fit_function->SetParameters(fit_params[0], fit_params[1], fit_params[2]);
}

vector<Double_t> DataFitter::GetFitParameters () const {
  return fit_params;
}

TF1* DataFitter::GetFitFunction () const {
  return fit_function;
}

void DataFitter::FitData (const Data& data, const string& picname="") {
  /* Не совсем очевидное появление параметров функции */
  TCanvas *canv = new TCanvas("canv", "Some fit", 800, 600);

  TGraph *graph = GetGraphFromData(data);

  graph->SetMarkerStyle(22);

  graph->Fit("fit_function");
  graph->Draw("AP");

  if (picname != "") {
    canv->SaveAs(picname.c_str());
  }

  UpdateFitParameters();
}

void DataFitter::UpdateFitParameters () {
  Double_t *params = fit_function->GetParameters();
  Int_t nparams = fit_function->GetNpar();
  fit_params = vector<Double_t>{params, params + nparams};
}
