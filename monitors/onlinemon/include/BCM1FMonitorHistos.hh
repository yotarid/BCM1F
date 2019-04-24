#ifndef BCM1FMONITORHISTOS_HH_
#define BCM1FMONITORHISTOS_HH_

#include "TH1I.h"
#include "TGraph.h"
#include "TFile.h"
       
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <mutex>
#include "SimpleStandardEvent.hh"
#include "SimpleStandardPlane.hh"
#include "SimpleStandardHit.hh"
using namespace std;

class RootMonitor;

class BCM1FMonitorHistos {

  protected:
    TH1I **AmplitudeSpectrum;

    std::mutex m_mu;

  public:
    BCM1FMonitorHistos(const SimpleStandardEvent &ev);
    virtual ~BCM1FMonitorHistos();
    unsigned int FindPeak(const vector<SimpleStandardHit> &hits);
    void Fill(const SimpleStandardEvent &ev);
    void Write();
    void Reset();
    TH1I *getAmplitudeSpectrum(unsigned int i) const;
    
    unsigned int getNplanes() const;

    std::mutex* getMutexAmplitudeSpectrum(unsigned int i){return &m_mu;};
 

  private:
    unsigned int nplanes;
};

#ifdef __CINT__
#pragma link C++ class BCM1FMonitorHistos - ;
#endif

#endif /* BCM1FMONITORHISTOS_HH_ */
