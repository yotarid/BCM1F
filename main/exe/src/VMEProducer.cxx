#include "eudaq/Configuration.hh"
#include "eudaq/Producer.hh"
#include "eudaq/Logger.hh"
#include "eudaq/RawDataEvent.hh"
#include "eudaq/Timer.hh"
#include "eudaq/Utils.hh"
#include "eudaq/OptionParser.hh"
#include "eudaq/ExampleHardware.hh"
#include <iostream>
#include <ostream>
#include <vector>

#include "eudaq/Status.hh"

//VME include 
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "adc_v1721_caen.hh"
#include "discr_v258b_caen.hh"
#include "RingBuffer.h"

#ifdef DIP_
#include "Dip.h"
#endif

//========== Global Objects ========================================
#define BLTSIZE 4096 // Words (32bit) per Block Transfer : Recom.: 16kB, Maximum: 60kB!
#define MAX_N_BOARDS 6
#define NCH 8
#define TOT_DATA 100000 
sigset_t *old_mask, *new_mask;



//A name to identify the raw data format of the events generated
static const std::string EVENT_TYPE = "VME";

class VMEProducer : public eudaq::Producer {

  public:
    VMEProducer(const std::string & name, const std::string & runcontrol)
      : eudaq::Producer(name, runcontrol),
      _n_run(0), _n_ev(0), _stopping(false), _done(false){}

    //Initialisation function : called when Initialise is pressed in the RunControl 
    virtual void OnInitialise(const eudaq::Configuration & init) {
      try{
        std::cout << "Reading: " << init.Name() << std::endl; 
        EUDAQ_INFO("Reading: "+init.Name());

        VME_link = init.Get("VMElink", 0);
        n_ADCs = init.Get("nADCs", 1); 
        
        std::cout << "Initialisation _____ VME_link = " << VME_link << std::endl;  
        std::cout << "Initialisation _____ n_ADCs = " << n_ADCs << std::endl;

        unsigned long baseaddress = init.Get("BaseAddress", 2290614272);

        for (unsigned int count = 0; count < n_ADCs; count++){
          Board_No[count] = count; 
          BaseAddress[count] = baseaddress; 
          ADC[count] = new ADC_V1721_CAEN(BaseAddress[count], "ADC_V1721_CAEN", VME_link, Board_No[count]); 
          ADC[count]->SW_Reset();
          std::cout << "Initialisation _____ ADC" << count << " BaseAddress = " << baseaddress << std::endl;
        }
        
        SetConnectionState(eudaq::ConnectionState::STATE_UNCONF, "Initialised (" + init.Name() + ")");
      }
      catch (...){
        std::cout << "Unknown exception" << std::endl;
        EUDAQ_ERROR("ERROR : Initilisation failed");
        SetConnectionState(eudaq::ConnectionState::STATE_ERROR, "Initialisation Error");
      }
    }

    //Configuration function : called when Configure is pressed in the RunControl
    virtual void OnConfigure( const eudaq::Configuration & config) {
      try{
        int retval;
        for (unsigned int count = 0; count < n_ADCs; count++){
          //Getting parameter values from config file 
          BuffOrg = config.Get("BuffOrg", 0xa);
          PostTrigNo = config.Get("PostTrigNo", 60);
          CustomSize = config.Get("CustomSize", 60);
          chMask = config.Get("ChMask", 0xff);
          Trigger_Mode = config.Get("TriggerMode", 0);
          //Congifuring VME
          ADC[count]->Set_Count_ALL_Trg(); // Count all triggers
          ADC[count]->Set_FrontPanel_PatternMode(); // To see LVDS Pattern in header 

          ADC[count]->Set_Buffer_Organisation(BuffOrg); 
          std::cout << "Configuration _____ BuffOrg = " << BuffOrg << std::endl;

          ADC[count]->Set_Custom_Size(CustomSize);
          std::cout << "Configuration _____ CustomSize = " << CustomSize << std::endl;

          ADC[count]->Set_Post_Trg(PostTrigNo);
          std::cout << "Configuration _____ PostTrgNo = " << PostTrigNo << std::endl;

          ADC[count]->Set_BLT_Event_Number(1); // One event per block transfer

          for (int i=0; i<NCH; i++) { //Enabling channels according to given channel mask
            if ( (chMask & (1<<i) ) == (1<<i) ){
              ADC[count]->Enable_Ch(i);
              std::cout << "Configuration _____ CH" << i << " enabled" << std::endl;
            }
            else{
              ADC[count]->Disable_Ch(i);
              std::cout << "Configuration _____ CH" << i << " disabled" << std::endl;
            }
          }
          
          switch (Trigger_Mode){ // Setting Trigger mode
            case 0:
              retval = Set_Trigger_External(); 
              break;
            default: 
              std::cout << "The requested mode does not exist or is not yet implemented" << std::endl;
              EUDAQ_INFO("The requested mode does not exist or is not yet implemented"); 
              break;
          }
          if (retval<0){
            std::cout << "ERROR : Could not set the requested trigger mode" << endl;
            EUDAQ_ERROR("ERROR : Could not set the requested trigger mode");
            SetConnectionState(eudaq::ConnectionState::STATE_ERROR, "Configuration Error");
          }    
        }
        SetConnectionState(eudaq::ConnectionState::STATE_CONF, "Configured (" + config.Name() + ")");
      }
      catch (...){
        std::cout << "Unknown exception" << std::endl;
        EUDAQ_ERROR("ERROR : Configuration failed");
        SetConnectionState(eudaq::ConnectionState::STATE_ERROR, "Configuration Error");
      }
    }

    //StartRun function : called when Start is pressed in the RunControl
    virtual void OnStartRun(unsigned param) {
      try{
        _n_run = param;
        _n_ev = 0;
        std::cout << "Start Run: " << _n_run << std::endl;
        eudaq::RawDataEvent bore(eudaq::RawDataEvent::BORE(EVENT_TYPE, _n_run));

        std::cout << "Start of acquisition loop ..." << std::endl;
        STOPPING = false;
        for (unsigned int count = 0; count < n_ADCs; count++){
          ADC[count]->Set_ACQ_RUN_START();
        }  

        SendEvent(bore);
        SetConnectionState(eudaq::ConnectionState::STATE_RUNNING, "Running");
      }
      catch (...){
        std::cout << "Unknown exception" << std::endl;
        SetConnectionState(eudaq::ConnectionState::STATE_ERROR, "Starting Error");
      }
    }

    //StopRun function : called when Stop is pressed in the RunControl
    virtual void OnStopRun(){
      try{
        _stopping = true;
        STOPPING = true;  

	std::cout << "... Stopping of acquisition loop" << std::endl; 
        for (unsigned int count = 0; count < n_ADCs; count++){
          ADC[count]->Set_ACQ_RUN_STOP();       
        }
        
        SendEvent(eudaq::RawDataEvent::EORE(EVENT_TYPE, _n_run, ++_n_ev));
 
        if (m_connectionstate.GetState() != eudaq::ConnectionState::STATE_ERROR){
          SetConnectionState(eudaq::ConnectionState::STATE_CONF);
        }
      }
      catch (...){
        std::cout << "Unknown exception" << std::endl; 
        SetConnectionState(eudaq::ConnectionState::STATE_ERROR, "Stopping Error");
      }
    }

    //Terminate : called when Terminate is pressed in the RunControl
    virtual void OnTerminate() {
      std::cout << "Terminating..." << std::endl;
      STOPPING = true;
      _done = true;
    }

    //ReadOut loop running in the main. It is adpated to the VME system.
    void ReadoutLoop() {
      try{
        while(!_done){

          if (GetConnectionState() != eudaq::ConnectionState::STATE_RUNNING){
            eudaq::mSleep(20);
            continue;
          } 
          
          //useful parameters to extract data from transfered blocks 
          bool ADC_comm = true;
          int ret=0, actual=0;
          unsigned int index=0;

          eudaq::RawDataEvent ev(EVENT_TYPE, _n_run, _n_ev);
      
          if(!STOPPING){ //if VME Started
            int retval = ADC[0]->Wait_4_Event(&STOPPING, 900000); //wait for event (trigger)
            if (retval==0){ //Data available 
              for (unsigned int nAdcBoard = 0; nAdcBoard < n_ADCs; nAdcBoard++){
                ADC_comm = VME_Read_Data(nAdcBoard);
                if (ADC_comm){ //Read Data OK ---> Extract data for each channel (see data structure in VME manual)
                  int lchmask = (0xff & TotalData[nAdcBoard][1]);
                  int NactiveChannels = 0;

                  for (int ich=0; ich<NCH; ich++){
                    if ( (lchmask & (1<<ich)) == (1<<ich) )
                      NactiveChannels++;
                  }
                  unsigned int DataSize = (TotalData[nAdcBoard][0] & 0xfffffff) - 4;
                  Nwords = int(DataSize/NactiveChannels);
                  Nsamples = Nwords*4;
                  int countch=0;
                   
                  for (int ich=0; ich<NCH; ich++){//Extract data of active channels
                    if ( (lchmask & (1<<ich)) == (1<<ich) ){
                      if (nAdcBoard==0){
                        unsigned int Buffer[Nsamples];   
                        unsigned int BufferSize = Nsamples;
                        memcpy(Buffer, &TotalData[nAdcBoard][4 + countch*Nwords], Nsamples);

                        ev.AddBlock(ich, Buffer, BufferSize); //Add data to RawDataEvent object
  
                        countch++;
                      }
                    }//end of if lchmask
                  }//end of ich<NCH
                }//end of if ADC_comm              
                else{
                  std::cout << " ... Acquisition stopped and communication lost " << std::endl;
                }
              }//end of nAdcBoard<n_ADCs
            }//end of if retval

            SendEvent(ev);
            _n_ev++;

            if (retval == -1){
              std::cout << " NO TRIGGER WITHIN THE PAST 15 min !!!!! " << std::endl;
            }
          }//end of if !STOPPING
          else{
            eudaq::mSleep(20);
            continue;
          }
        }//end of while !_done 
      }
      catch (...){
        std::cout << "Unknown exception" << std::endl;
        SetConnectionState(eudaq::ConnectionState::STATE_ERROR, "Error during running");
      }
    }

    //VME related functions (to simplify code writing) 
    int Set_Trigger_External(){

      int ret=0;
      for (unsigned int count = 0; count < n_ADCs; count++)
        {
          ret = ADC[count]->Disable_SW_Trg_Src();
          if (ret<0)
            return ret;
        }
      for (unsigned int count = 0; count < n_ADCs; count++)
        {
          ret = ADC[count]->Disable_Ch_Trg_Src(NCH);
          if (ret<0)
            return ret;
        }
      for (unsigned int count = 0; count < n_ADCs; count++)
        {
          ret = ADC[count]->Enable_External_Trg_Src();
          if (ret<0)
            return ret;
        }
      std::cout << " Trigger source set to external " << std::endl;
      return 0;  
    }   

    //VME Readout function
    bool VME_Read_Data(unsigned int nAdcBoard){
      int ret=0, actual=0;
      unsigned int index=0;
    
      pthread_sigmask(SIG_SETMASK, new_mask, old_mask);
      do {
        ret = ADC[nAdcBoard]->Read_MBLT64_T(BLTdata[nAdcBoard], BLTSIZE*sizeof(int), &actual);
        memcpy(&TotalData[nAdcBoard][index], BLTdata[nAdcBoard], actual);
        index += actual/sizeof(int);
      } while (ret==0); 
      pthread_sigmask(SIG_SETMASK, new_mask, old_mask);

      if (ret==-1){
        if (TriggerCounter > (0xffffff & TotalData[nAdcBoard][2]))
          TCOverflows++;
        TriggerCounter = 0xffffff & TotalData[nAdcBoard][2];
        return true;
      }
      return false;
    }


    private:
      unsigned _n_run, _n_ev;
      bool _stopping, _done;

      //---------- ADC configuration (settings in *.conf file, onverwrites default values!) ----------
      ADC_V1721_CAEN* ADC[MAX_N_BOARDS];
      unsigned int n_ADCs = 1;
      unsigned long BaseAddress[MAX_N_BOARDS];
      short VME_link = 0;
      short Board_No[MAX_N_BOARDS]; 
      int chMask = 0xff;
      int BuffOrg = 0x0a;
      int CustomSize = 0;
      int PostTrigNo = 10;

      //---------- Trigger mode and thresholds ------------------------------------------------------- 
      int Trigger_Mode = 0;
      int Trg_Thres_Ch[NCH]={0};
      unsigned int Time_ou_Thres_Ch[NCH]={0};
      int Coinc_Level = 0;
   
      //---------- Data processing -------------------------------------------------------------------
      bool STOPPING = true;
      unsigned int TriggerCounter = 0;
      unsigned int TCOverflows = 0;
      int Nsamples = 0;
      int Nwords = 0;
      unsigned int Nsignals = 0;
      unsigned long BLTdata[MAX_N_BOARDS][BLTSIZE];
      unsigned int TotalData[MAX_N_BOARDS][TOT_DATA];
};

int main(int /*argc*/, const char ** argv) {
  
  eudaq::OptionParser op("EUDAQ VME Producer", "1.0", "VME Producer");
  eudaq::Option<std::string> rctrl(op, "r", "runcontrol", "tcp://localhost:44000", "address", "Address of RunControl");
  eudaq::Option<std::string> level(op, "l", "log-level", "NONE", "level", "Minimum Log Level");
  eudaq::Option<std::string> name(op, "n", "name", "VME", "string", "Name of the Producer");

  try{
    op.Parse(argv);
    EUDAQ_LOG_LEVEL(level.Value());
    VMEProducer producer(name.Value(), rctrl.Value());
    EUDAQ_INFO("VME PRODUCER");
    producer.ReadoutLoop();
  }
  catch (...){
    std::cout << "Error when quitting" << std::endl;
    return op.HandleMainException();
  }

  return 0;
}












