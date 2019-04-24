#include "../include/BCM1FMonitorHistos.hh"

BCM1FMonitorHistos::BCM1FMonitorHistos(const SimpleStandardEvent &ev) {
  nplanes = ev.getNPlanes();

  AmplitudeSpectrum = new TH1I *[nplanes];

  for (unsigned int i=0; i<nplanes; i++) {
    stringstream ch;
    stringstream histolabel;
    
    string name = ev.getPlane(i).getName();
    if (!name.compare("VME")){
      histolabel << "Amplitude Spectrum " << name << i;
      AmplitudeSpectrum[i] = new TH1I(histolabel.str().c_str(), histolabel.str().c_str(), 1000, 0, 300); 

      AmplitudeSpectrum[i]->SetLineColor(i+1);
      AmplitudeSpectrum[i]->SetMarkerColor(i+1);
      

#ifdef EUDAQ_LIB_ROOT6
      AmplitudeSpectrum[i]->SetCanExtend(TH1::kAllAxes);
#else
      AmplitudeSpectrum[i]->SetBit(TH1::kCanRebin);
#endif
    }
  }
}

BCM1FMonitorHistos::~BCM1FMonitorHistos() {

}

unsigned int FindPeak(const vector<SimpleStandardHit> &hits) {
  unsigned int max = hits.at(0).getTOT();
  for (size_t samples=1; samples<hits.size(); samples++) {
    if (hits.at(sample).getTOT() < max) { //VME signal has negative polarity
      max = hits.at(sample).getTOT();
    }
  }
  return max;
}

void BCM1FMonitorHistos::Fill(const SimpleStandardEvent &ev) {
 
  std::lock_guard<std::mutex> lck(m_mu);
  for (unsigned int ch=0; ch<ev.getNPlanes(); ch++) {
    string name = ev.getPlane(ch).getName();
    if (!name.compare("VME")){
      AmplitudeSpectruM[ch]->Fill(FindPeak(ev.getPlane(ch).getHits()));
    }
    //size_t TotSamples = ev.getPlane(ch).getNHits();
    //for (size_t sample=1; sample<TotSamples; sample++) {
    //AmplitudeSpectrum[ch]->Fill(ev.getPlane(ch).getHits().at(sample).getTOT()); 
    //}
  }
}

void BCM1FMonitorHistos::Write() {
  for (unsigned int i=0; i<nplanes; i++) {
    AmplitudeSpectrum[i]->Write();
  }
}

TH1I *BCM1FMonitorHistos::getAmplitudeSpectrum(unsigned int i) const{
  return AmplitudeSpectrum[i];
}

void BCM1FMonitorHistos::Reset() {
  for (unsigned int i=0; i<nplanes; i++) { 
    AmplitudeSpectrum[i]->Reset();
  }

}

unsigned int BCM1FMonitorHistos::getNplanes() const {
  return nplanes;
}

