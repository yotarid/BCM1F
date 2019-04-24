#include "../include/BCM1FMonitorCollection.hh"
#include "OnlineMon.hh"

BCM1FMonitorCollection::BCM1FMonitorCollection() : BaseCollection() {
  mymonhistos = NULL;
  histos_init = false;
  cout << "Initializing BCM1F Monitor Collection" << endl;
  CollectionType = BCM1FMONITOR_COLLECTION_TYPE;
}

BCM1FMonitorCollection::~BCM1FMonitorCollection() {

}

void BCM1FMonitorCollection::fillHistograms(const SimpleStandardEvent &ev){
  Fill(ev);
}

void BCM1FMonitorCollection::Reset() {
  if (mymonhistos != NULL)
    mymonhistos->Reset();
}

void BCM1FMonitorCollection::Write(TFile *file) {
  if (file == NULL) {
    cout << "BCM1FMonitorCollection::Write File pointer is NULL" << endl;
    exit(-1);
  }
  if (gDirectory != NULL) {
    gDirectory->mkdir("BCM1FMonitor");
    gDirectory->cd("BCM1FMonitor");
    mymonhistos->Write();
    gDirectory->cd("..");
  }
}

void BCM1FMonitorCollection::bookHistograms(const SimpleStandardEvent & /*simpev*/) {
  if (_mon != NULL) {
    cout << "BCM1FMonitorCollection:: Monitor running in online-mode " << endl;
    string mon_folder_name = "BCM1F Monitor";
    
    for (unsigned int i=0; i<mymonhistos->getNplanes(); i++) {
      stringstream namestring_spec;
      namestring_spec << mon_folder_name << "/Amplitude Spectrum" << i;
      _mon->getOnlineMon()->registerTreeItem(namestring_spec.str());
      _mon->getOnlineMon()->registerHisto(namestring_spec.str(),
                                          mymonhistos->getAmplitudeSpectrum(i));
      _mon->getOnlineMon()->registerMutex(namestring_spec.str(),
                                          mymonhistos->getMutexAmplitudeSpectrum(i)); 
    }
    _mon->getOnlineMon()->makeTreeItemSummary(mon_folder_name.c_str());
  }
}
  
BCM1FMonitorHistos *BCM1FMonitorCollection::getBCM1FMonitorHistos() {
  return mymonhistos;
}
 
void BCM1FMonitorCollection::Fill(const SimpleStandardEvent &simpev) {
  if (histos_init == false) {
    mymonhistos = new BCM1FMonitorHistos(simpev);
    if (mymonhistos == NULL) {
      cout << "BCM1FMonitorCollection:: Can't book histograms " << endl;
      exit(-1);
    }
    bookHistograms(simpev);
    histos_init = true;
  }
  mymonhistos->Fill(simpev);
}

void BCM1FMonitorCollection::Calculate(const unsigned int /*currentEventNumber*/) {}
