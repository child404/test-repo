#pragma once
  
// C++ includes
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>

// ROOT includes
#include "TMath.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"

#include "operators.h"

using namespace std;

// Definition of mother classes of Data (abstract), DataParser, Graph and DataFitter
  
class Data {
public:
  Data ();

  Data (const string&, Int_t);

  virtual void ParseFileWithData (const string&, Int_t);

  void SetXaxisData (const vector<Double_t>&);

  void SetYaxisData (const vector<Double_t>&);

  vector<Double_t> GetXaxisData () const;

  vector<Double_t> GetYaxisData () const;

  Double_t GetXmax () const;

  Double_t GetXmin () const;

  void DrawData (
      const string&, const string&, const string&) const;

protected:
  void SkipFirstNlines (ifstream&, Int_t) const;

  vector<Double_t> x_axis_data;
  vector<Double_t> y_axis_data;
};

TGraph* GetGraphFromData (const Data&);

class DataFitter {
public:
  void SetFitFunction (
      const string&, Double_t, Double_t);

  void SetFitParameters (const vector<Double_t>&);

  vector<Double_t> GetFitParameters () const;

  TF1* GetFitFunction () const;

  void FitData (const Data&, const string&);

  void UpdateFitParameters (); 

private:
  TF1 *fit_function;
  vector<Double_t> fit_params;
};
