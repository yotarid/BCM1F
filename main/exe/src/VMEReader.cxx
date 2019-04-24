#include "eudaq/FileReader.hh"
#include "eudaq/PluginManager.hh"
#include "eudaq/OptionParser.hh"
#include <iostream>
#include <fstream>
static const std::string EVENT_TYPE = "VME";

int main(int /*argc*/, const char ** argv) {
  eudaq::OptionParser op("VME File Reader", "1.0", "VME Raw Data File Reader", 1);
  eudaq::OptionFlag doraw(op, "r", "raw", "Display raw data from events");
  eudaq::OptionFlag docon(op, "c", "converted", "Display converted events");
  try{
    op.Parse(argv);
    for (size_t i=0; i<op.NumArgs(); i++) {
      eudaq::FileReader reader(op.GetArg(i));

      std::cout << "Opened file:" << reader.Filename() << std::endl;

      if (docon.IsSet()) {
        eudaq::PluginManager::Initialize(reader.GetDetectorEvent());
      }
     
      //CSV file test
      std::ofstream rawFile;
      rawFile.open("/afs/cern.ch/user/y/yotarid/BCM1F/eudaq/data/rawtocsv.csv");

      std::ofstream stdevFile;
      stdevFile.open("/afs/cern.ch/user/y/yotarid/BCM1F/eudaq/data/stdevtocsv.csv");

      while (reader.NextEvent()) {
        if (reader.GetDetectorEvent().IsEORE()) {
          std::cout << "End of run detected" << std::endl;
          break;
        }
      
        if (doraw.IsSet()) {
          try {
            const eudaq::RawDataEvent &rev = reader.GetDetectorEvent().GetRawSubEvent(EVENT_TYPE); 
            for (size_t ich=0; ich < rev.NumBlocks(); ich++) {
              //std::cout << " CHANNEL " << ich << std::endl;
              rawFile << "CH" << ich << ",";
              for (size_t sample=0; sample < rev.GetBlock(ich).size(); sample++) {
                //std::cout << (unsigned int)rev.GetBlock(ich).at(sample) << std::endl;
                rawFile << (unsigned int)rev.GetBlock(ich).at(sample) << ","; 
              }           
              rawFile << "\n";
            }
            std::cout << rev << std::endl;
          }
          catch (const eudaq::Exception &) {
            std::cout << "No " << EVENT_TYPE << " subevent in event " << reader.GetDetectorEvent().GetEventNumber() << std::endl;
          }
        }

        if (docon.IsSet()) {
          eudaq::StandardEvent sev = eudaq::PluginManager::ConvertToStandard(reader.GetDetectorEvent());
          size_t NumSens = sev.NumPlanes();
          for (size_t ich=0; ich < NumSens; ich++){
            eudaq::StandardPlane sens = sev.GetPlane(ich); 
            //std::cout << "CHANNEL " << ich << std::endl;
            size_t TotSamples = sens.HitPixels(0);

            stdevFile << "CH" << ich << ",";
            for (size_t sample=2; sample < TotSamples; sample++){// start from 2 because 2 first samples = 0 
              //std::cout << sens.GetPixel(sample, 0) << std::endl;
              stdevFile << sens.GetPixel(sample,0) << ","; 
            }
            stdevFile << "\n";
          } 

          std::cout << sev << std::endl;
        }
      } 
     rawFile.close(); 
     stdevFile.close(); 
    }
  }
  catch (...) {
    return op.HandleMainException();
  }
  return 0;
}
