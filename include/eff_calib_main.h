#pragma once

// C++ includes

#include <fstream>
#include <map>

#include "eff_calib_classes.h"

using DataStorage = map<Double_t, EfficiencyCalibrationData>;
using Parameters = vector<Double_t>;

void WriteDataInFile (const Data&, const string&);

vector<Double_t> ParseFileWithDistances (const string&);

DataStorage GetStorageOfData (const vector<Double_t>&, Int_t);

map<Double_t, Parameters> GetParametersFromFit (const DataStorage&, const vector<Double_t>&);

vector<Double_t> GetEfficiencies (const map<Double_t, Parameters>&, Double_t);

vector<Double_t> GetEfficienciesForAllDistances (
    const map<Double_t, Parameters>&, Double_t);

vector<Double_t> GetEfficienciesForMyDistances (
    const vector<Double_t>&, const vector<Double_t>&);
