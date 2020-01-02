#pragma once

#include "mother_classes.h"

using Peaks = vector<Double_t>;
using Energy = vector<Double_t>;
using Intensity = vector<Double_t>;
using Efficiency = vector<Double_t>;

class EfficiencyCalibrationData : public Data {
public:
  EfficiencyCalibrationData ();

  EfficiencyCalibrationData (const string&, Int_t, Int_t);

  void ParseFileWithData (const string&, Int_t) override;

  void SetLogarithmicXaxisData ();

  void SetLogarithmicYaxisData ();

  void SetDistance (Int_t);

  Int_t GetDistance () const;

  void SetYerrors (const vector<Double_t>);

  vector<Double_t> GetYerrors () const; 

private:
  vector<Double_t> y_errors;
  Int_t distance;
};

/* Подумать насчета метода FitData */ 
class EfficiencyCalibrationDataFitter : public DataFitter {
public:
  EfficiencyCalibrationDataFitter ();
  // Вид апроксимирующей кривой: ln(y) = a + b*ln(x) с ln(y) и ln(x) по осям
  // На самом деле ищем зависимости y = a*pow(x, b)
  
  void FitData(const EfficiencyCalibrationData&, const string&);
};

class DecayCurveBuilder {
public:
  DecayCurveBuilder ();

  DecayCurveBuilder (
      const vector<Double_t>&, const vector<Double_t>&, const vector<Double_t>&);

  void SetDecayCurve (
      const vector<Double_t>&, const vector<Double_t>&, const vector<Double_t>&);

  TGraph* GetDecayCurve () const;

private:
  TGraph *decay_curve;
};
