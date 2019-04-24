#include "eudaq/DataConverterPlugin.hh"
#include "eudaq/StandardEvent.hh"
#include "eudaq/Utils.hh"

// All LCIO-specific parts are put in conditional compilation blocks
// so that the other parts may still be used if LCIO is not available.
#if USE_LCIO
#include "IMPL/LCEventImpl.h"
#include "IMPL/TrackerRawDataImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "lcio.h"
#endif

namespace eudaq {

  // The event type for which this converter plugin will be registered
  // Should match your actual event type (from the Producer)
  static const char *EVENT_TYPE = "VME";

  // Declare a new class that inherites from DataConverterPlugin
  class VMEConverterPlugin : public DataConverterPlugin {

  public:
    // This is called once at the beginning of each run
    // You may extract information from the BORE and/or configuration
    // and store it in member variables to use during the decoding later
    virtual void Initialize(const Event &bore, const Configuration &cnf) {
      m_VMEparam = bore.GetTag("VME", 0);
#ifndef WIN32 //some linux Stuff //$$change
  (void)cnf; // just to suppress a warning about the unused parameter cnf
#endif
    }

    // Thsi should return the trigger ID (as provided by the TLU)
    // if it was read out, otherwise it can either return (unsigned) -1 
    // or be left undefined as there is already a default version
    virtual unsigned GetTriggerID(const Event &ev) const {
      static const unsigned TRIGGER_OFFSET = 8;
      // Make syre the eventis of class RawDataEvent 
      if (const RawDataEvent *rev = dynamic_cast<const RawDataEvent *>(&ev)) {
        // Make sure we have at least one block of data and it's large enough
        if (rev->NumBlocks() > 0 && rev->GetBlock(0).size() >= (TRIGGER_OFFSET + sizeof(short))) {
          // Read a little-endian unsigned short from offset TRIGGER_OFFSET
          return getlittleendian<unsigned short>(&rev->GetBlock(0)[TRIGGER_OFFSET]);
        }
      }
      // If we are unable to extract the Trigger ID, signal with (unsigned)-1
      return (unsigned)-1; 
    }

    // Here, the data from the RawDataEvent is extracted into a StandardEvent
    // The return value indicates wether the conersion was successful
    virtual bool GetStandardSubEvent(StandardEvent &sev, const Event &ev) const {
      if (ev.IsBORE() || ev.IsEORE()){
        // Nothing to do
        return true;
      } 
      
      // If we get here there must be data event
      const RawDataEvent &rev = dynamic_cast<const RawDataEvent &>(ev);
      // If the event is type is used for different sensors
      // they can be differetiated here
      std::string sensortype = "VME";
      
      //Add each channel data to the plane
      for (size_t ich=0; ich < rev.NumBlocks(); ich++){
        // Create a StrandardPlane representing one sensor plane
        StandardPlane plane(ich , EVENT_TYPE, sensortype);
        // Set the number of pixels
        int width = 1, height = 1;
        plane.SetSizeRaw(width, height);
        for (size_t sample=0; sample < rev.GetBlock(ich).size(); sample++){
          plane.PushPixel(width, height, rev.GetBlock(ich).at(sample));
        }  
        // Set the trigger ID
        plane.SetTLUEvent(GetTriggerID(ev));
        // Add the plane to the StandarEvent
        sev.AddPlane(plane);
        // Indicate that data was successfully converted
      }
      return true;
    }

#if USE_LCIO
    // This is where the conversion to LCIO is done
    virtual lcio::LCEvent *GetLCIOEvent(const Event * /*ev*/) const {
      return 0;
    }
#endif

    private:
      // The constructor can be private, only one static instance is created
      // The DataConverterPlugin constructor must be passed the event type
      // in order to register this converter for the corresponding conversions
      // Member variables should also be initialized to default values here.
      VMEConverterPlugin() : DataConverterPlugin(EVENT_TYPE), m_VMEparam(0) {} 

      // Informatin extracted in Initialize() can be stored here
      unsigned m_VMEparam;

      // The single instance of this converter plugin
      static VMEConverterPlugin m_instance;
  }; // class VMEConverterPlugin
  
  // Instantiate the converter plugin instance
  VMEConverterPlugin VMEConverterPlugin::m_instance;

}// namespace eudaq
