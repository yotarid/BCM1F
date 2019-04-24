#ifndef EUDAQ_INCLUDED_DataCollector
#define EUDAQ_INCLUDED_DataCollector

#include <string>
#include <vector>
#include <list>

#include "eudaq/TransportServer.hh"
#include "eudaq/CommandReceiver.hh"
#include "eudaq/Event.hh"
#include "eudaq/FileWriter.hh"
#include "eudaq/Configuration.hh"
#include "eudaq/Utils.hh"
#include "eudaq/Platform.hh"
#include <memory>
#include <map>

namespace eudaq {

  /** Implements the functionality of the File Writer application.
   *
   */
  class DLLEXPORT DataCollector : public CommandReceiver {
  public:
    DataCollector(const std::string &name, const std::string &runcontrol,
                  const std::string &listenaddress,
                  const std::string &runnumberfile = "../data/runnumber.dat");

    virtual void OnConnect(const ConnectionInfo &id);
    virtual void OnDisconnect(const ConnectionInfo &id);
    virtual void OnServer();
    virtual void OnGetRun();
    virtual void OnInitialise(const Configuration &init);
    virtual void OnConfigure(const Configuration &param);
    virtual void OnPrepareRun(unsigned runnumber);
    virtual void OnStopRun();
    virtual void OnReceive(const ConnectionInfo &id, std::shared_ptr<Event> ev);
    virtual void OnCompleteEvent();
    virtual void OnStatus();
    virtual ~DataCollector();

    void DataThread();

  private:
    struct Info {
      std::shared_ptr<ConnectionInfo> id;
      std::list<std::shared_ptr<Event>> events;
    };

    const std::string
        m_runnumberfile; // path to the file containing the run number
    void DataHandler(TransportEvent &ev);
    size_t GetInfo(const ConnectionInfo &id);

    bool m_done, m_listening;
    TransportServer *m_dataserver; ///< Transport for receiving data packets
    std::unique_ptr<std::thread> m_thread;
    std::vector<Info> m_buffer;

    size_t m_itlu;       ///< Index of TLU in m_buffer vector, or -1 if no TLU
    size_t m_slow;     /// The number of slow producers registered
    unsigned m_runnumber, m_eventnumber;
    std::shared_ptr<FileWriter> m_writer;

    std::map<size_t, std::string> m_ireceived;    // <producer_num, producer_type>
                                // pairs of producers which sent event
    Configuration m_init;
    Configuration m_config;
    Time m_runstart;
  };
}

#endif // EUDAQ_INCLUDED_DataCollector
