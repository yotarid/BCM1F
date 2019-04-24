#ifndef BCM1FMONITORCOLLECTION_HH_
#define BCM1FMONITORCOLLECTION_HH_

// ROOT Includes 
#include <RQ_OBJECT.h>
#include <TH1I.h>
#include <TGraph.h>
#include <TFile.h>

// STL Includes 
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>

// Project includes 
#include "SimpleStandardEvent.hh"
#include "BCM1FMonitorHistos.hh"
#include "BaseCollection.hh"

using namespace std;

class BCM1FMonitorCollection : public BaseCollection {
  RQ_OBJECT("BCM1FMonitorCollection")
  
  protected:
    void fillHistograms(const SimpleStandardEvent &ev);
    bool histos_init;

  public:
    BCM1FMonitorCollection();
    virtual ~BCM1FMonitorCollection();
    void Reset();
    virtual void Write(TFile *file);
    virtual void Calculate(const unsigned int currentEventNumber);
    void bookHistograms(const SimpleStandardEvent &simpev);
    void setRootMonitor(RootMonitor *mon) { _mon = mon; }
    void Fill(const SimpleStandardEvent &simpev);
    BCM1FMonitorHistos *getBCM1FMonitorHistos();

  private:
    BCM1FMonitorHistos *mymonhistos;

};

#ifdef __CINT__
#pragma link C++ class BCM1FMonitorCollection - ;
#endif

#endif /* BCM1FMonitorCollection_HH_ */
