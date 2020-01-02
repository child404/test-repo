#include "classes.h"
  
/* SpectrumData */

SpectrumData::SpectrumData () {
}

SpectrumData::SpectrumData (const string& filename, Int_t nlines_to_skip) {
  ParseFileWithData(filename, nlines_to_skip);
}

void 
SpectrumData::ParseFileWithData (const string& filename, Int_t nlines_to_skip=0) {
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

/* Сделать что-то с геттером GetSpectrumData класса */

/* Histogram */

Histogram::Histogram () {
}

Histogram::Histogram (const Data& data) {
  SetHistogram(data);
}

void 
Histogram::SetHistogram (const Data& data) {
  vector<Double_t> energy = data.GetXaxisData();
  vector<Double_t> intensity = data.GetYaxisData();

  Double_t nbins = energy.size() - 1;
  /* Почему мы берём nbins < energy.size() на единицу? */
  hist = new TH1F("hist", "hist", nbins, &energy[0]);
  for (Int_t i = 1; i <= nbins; i++) {
    hist->SetBinContent(i, intensity[i - 1]);  
  }
}

TH1F* Histogram::GetHistogram () const {
  return hist;
}

void 
Histogram::DrawHistogram () const {
  TCanvas *c2 = new TCanvas("c2", "Histogram", 1500, 500);
  // TCanvas *c2 = new TCanvas("c", "Hist", 10, 10, 1000, 900);
  hist->Draw("hist");
}

/* Background */

Background::Background () {}

Background::Background (TH1F *h) {
  SetBackground(h);
}

void 
Background::SetBackground (TH1F *h) {
  TSpectrum *s = new TSpectrum();
  // TH1 *histbg = s->Background(h, 20, "same");
  // background = (TH1F*)histbg;
  background = (TH1F*)s->Background(h, 20, "same");
}

TH1F* Background::GetBackground () const {
  return background;
}

/* CuttedHistogram */

CuttedHistogram::CuttedHistogram () {}

CuttedHistogram::CuttedHistogram (TH1F *h, Double_t peak_position, Int_t range) {
  SetCuttedHistogram(h, peak_position, range);
}

void 
CuttedHistogram::SetCuttedHistogram (TH1F *h, Double_t peak_position, Int_t range) {
  Int_t bin = h->GetXaxis()->FindBin(peak_position);
  Int_t cutted_bins = range * 2;
  /* Все равно теряем либо первый, либо последний бин (начинаем не с bin - range, а на единицу больше) */
  cutted_hist = new TH1F(
      "cutted_hist", "Gauss fit of 622 keV peak", cutted_bins, h->GetBinCenter(bin - range), h->GetBinCenter(bin + range)
    );
  for (Int_t i = 1; i <= cutted_bins; i++) {
    cutted_hist->SetBinContent(i, h->GetBinContent(bin - range + i));
  } 
}

TH1F* CuttedHistogram::GetCuttedHistogram () const {
  return cutted_hist;
}

/* PeakFinder */

PeakFinder::PeakFinder () {}

PeakFinder::PeakFinder (TH1F *h, Double_t resolution=0.1) {
  FindPeaks(h, resolution);
}

void 
PeakFinder::FindPeaks (TH1F *h, Double_t resolution=0.1) {
  TSpectrum *s = new TSpectrum();

  Int_t nfound = s->Search(h, 0.01, "", resolution);
  cout << "Found " << nfound << " peaks" << endl;
    
  Double_t *xpeaks = s->GetPositionX();
  /* Вектор состоит из координат пиков */
  peaks = Peaks{xpeaks, xpeaks + nfound};
}

vector<Double_t> PeakFinder::GetPeaks () const {
  return peaks;       
}

// ПЕРЕПИСАТЬ РЕАЛИЗАЦИЮ VALUES, ВЫНЕСТИ ОТДЕЛЬНО PROCESSPEAK

/* Values */

Values::Values () {}

Values::Values (TH1F* cutted_hist, const string& picname="") {
  ProcessPeak(cutted_hist, picname);
}

void 
Values::ProcessPeak (TH1F* cutted_hist, const string& picname="") {
  /* Определяем минимальное и максимальное значения графика по горизонтальной оси */
  Double_t xmin = cutted_hist->GetXaxis()->GetXmin();
  Double_t xmax = cutted_hist->GetXaxis()->GetXmax();
  //Double_t ymax = gPad->GetUymax();
  Double_t ymax = cutted_hist->GetMaximum();
  cout << "Y max value: " << ymax << endl;
  /* Составляем массив из параметров аппроксимации */
  Double_t params[5] = {ymax, (xmax+xmin)/2, 1.5, 3, 0};
  cout << "Peak position before fit: " << params[1] << endl;

  /* Создаем шаблон для графиков */
  TCanvas* canv = new TCanvas("canv", "title", 800, 600);
  /* Объявляем фунцкию аппроксимации */
  TF1 *f_total = new TF1("f_total", "gaus(0) + pol1(3)", xmin, xmax);
  /* Передаем параметры аппроксимации */
  f_total->SetParameters(params);
  /* Аппроксимируем наш пик заданной функцией */
  cutted_hist->Fit("f_total", "Q");
  /* Сохраняем изображение с пиком в файл picname */ 
  // Тут разобраться, почему не сохраняется аппроксимированный пик
  if (picname != "") {
    canv->SaveAs(picname.c_str());
  }

  /* Получаем уже подогнанные параметры аппроксимации */
  f_total->GetParameters(params);

  /* Создаем отдельно функции аппроксимации для пика (гаусс) 
  и фона (полином 1й степени) */
  TF1 *f_peak = new TF1("f_peak", "gaus(0)", xmin, xmax);
  TF1 *f_back = new TF1("f_back", "pol1(3)", xmin, xmax);

  /* Передаем параметры аппроксимации отдельным функциям */
  f_peak->SetParameters(&params[0]);
  f_back->SetParameters(&params[3]);
    
  /* Определяем Values нашего пика и заполняем его найденными значениями */
  peak = params[1];
  peak_error = f_total->GetParError(1);
  peak_area = GetArea(f_peak, &params[0]);
  peak_area_error = GetPeakAreaError(
      f_total, params, GetArea(f_peak, &params[0])
    );
  background = GetArea(f_back, params);

  /* Удаляем созданные объекты */
  delete canv, f_total, f_peak, f_back;
}

vector<Double_t> Values::GetValues () const {
  return vector<Double_t>{
      peak, peak_error, peak_area, peak_area_error, background
    };
}

Double_t Values::GetPeakArea () const {
  return peak_area;
}

Double_t Values::GetPeakAreaError () const {
  return peak_area_error;
}

void 
Values::Print () const {
  cout << "------******------" << endl;
  cout << "Peak: " << peak << " +- " 
                   << peak_error << endl;

  cout << "Peak area: " << peak_area << " +- " 
                        << peak_area_error << endl;

  cout << "Background area: " << background << endl;
  cout << "------******------" << endl;
}

Float_t Values::GetArea (TF1* f, Double_t* params) const {
  return f->Integral(params[1]-2*abs(params[2]), params[1]+2*abs(params[2]));
}

Float_t Values::GetPeakAreaError (const TF1* f, Double_t* params, Float_t peak_area) const {
  Float_t peak_sigma_error = f->GetParError(2);
  Float_t peak_magnitude_error = f->GetParError(0);

  return peak_area * sqrt(Sqr(peak_sigma_error)/Sqr(params[2]) + 
                          Sqr(peak_magnitude_error)/Sqr(params[0]));
}
