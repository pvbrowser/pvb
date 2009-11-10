//#############################################################################
// Author: pvbrowser 2007
//
// This is a template for a daemon implementing your own custom data acquisition.
// From pvserver you can access this data acquisition with the class rlDataAcquisition.
//
// There are variables (strings) and values (also strings) which are stored in the shared memory.
// You will have to read the variables from the real world and store them in shared mmemory.
// Thus the pvserver can read them with rlDataAcquisition.
//
// Also there is a mailbox.
// pvserver will send variables to this mailbox and you have to output them to the real world.
//
// The variables you have to poll are contained in the dataacquisition.itemlist file
//
// search for "TODO" to find out what you will have to add to this template
//#############################################################################
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "rldataacquisitionprovider.h"
#include "rlmailbox.h"
#include "rlthread.h"

#define MAX_PATH_LENGTH 1024

typedef struct
{
  rlMailbox                 *mbx;
  rlDataAcquisitionProvider *provider;
  int                        running;                    // running = 1 as long as the provider is running
  char                       itemlist[MAX_PATH_LENGTH];  // name of the itemlist
  char                       shmname[MAX_PATH_LENGTH];   // name of the shared memory
  char                       mbxname[MAX_PATH_LENGTH];   // name of the mailbox
  int                        sleep;                      // interval in which data is polled
  int                        max_name_length;            // max length of a value in the shared memory
  int                        debug;                      // switch that may be used for debugging purpose
  long                       shmsize;                    // total size of the shared memory
                                                         // TODO: you may add your own parameters in here
} PV_DAQ_PARAM;

//### this is the main subroutine that is polling the data ####################
static void pollData(PV_DAQ_PARAM *daq, rlThread *thread)
{
  if(daq == NULL || thread == NULL) return;
  const char *cptr;
  static int  itest = 0;
  rlDataAcquisitionProvider *provider = daq->provider;

  // poll data forever
  while(daq->running)
  {
    cptr = provider->firstItem();  // get first item
    while(cptr != NULL)            // loop through itemlist
    {
      // thread->lock();           // if you have critical sections, you may lock the thread
                                   // TODO: replace this test with something meaningfull
                                   printf("setIntValue variablename=%s value=%d\n", cptr, itest);
                                   provider->setIntValue(cptr, itest);
                                   itest += 1;
                                   printf("stored value=%d\n", provider->intValue(cptr));
      // thread->unlock();         // if you have critical sections, you may lock the thread
      cptr = provider->nextItem(); // get next item
    }
                                   if(itest > 256*256) break; // this only for testing
    rlsleep(daq->sleep);           // sleep for cycle time
  }  

  daq->running = 0;
}

//### this is the thread that is reading the mailbox ##########################
static void *mbxReaderThread(void *arg)
{
  int  ret;
  char buf[1024], *cptr, *variablename, *value;
  THREAD_PARAM *p    = (THREAD_PARAM *) arg;
  PV_DAQ_PARAM *daq  = (PV_DAQ_PARAM *) p->user;

  if(daq->debug) printf("mbxReaderThread starting\n");

  // read mbx until it is empty
  daq->mbx->clear();

  // wait for commands from clients
  while(daq->running)
  {
    ret = daq->mbx->read(buf,sizeof(buf));
    if(ret < 0) break;             // terminate if mailbox read fails
    cptr = strchr(buf,'\n');       // parse buf
    if(cptr != NULL) *cptr = '\0'; // eliminate newline
    cptr = strchr(buf,',');        // get second parameter
    if(cptr != NULL)
    {
      *cptr = '\0';
      cptr++;
      variablename = &buf[0];      // this is the name of your variable
      value        = cptr;         // this is the value of your variable
      // p->thread->lock();        // if you have critical sections, you may lock the thread
      printf("TODO: send your variables to the real world here. variablename=%s value=%s\n", variablename, value);
      // p->thread->unlock();
    }  
  }

  if(daq->debug) printf("mbxReaderThread terminating\n");
  daq->running = 0;
  return NULL;
}

// ### this is the main worker subroutine #####################################
static int run(PV_DAQ_PARAM *daq)
{
  if(daq == NULL) return -1;
  rlThread thread;

  // create instance of provider and mbx
  daq->provider = new rlDataAcquisitionProvider(daq->max_name_length, daq->shmname, daq->shmsize);
  if(daq->provider->shmStatus() == rlDataAcquisitionProvider::DAQ_PROVIDER_ERROR)
  {
    printf("error shared memory\n");
    return -1;
  }
  daq->mbx = new rlMailbox(daq->mbxname);

  // read the itemlist
  if(daq->provider->readItemList(daq->itemlist) != 0)
  {
    // cleanup
    delete daq->provider;
    delete daq->mbx;
    return -1;
  }

  daq->running = 1;

  // create mbxReaderThread
  thread.create(mbxReaderThread, daq);

  // poll data
  pollData(daq, &thread);

  // cleanup
  delete daq->provider;
  delete daq->mbx;

  return 0;
}

//### the main program follows ################################################

static void printusage(char *filename)
{
  printf("Usage: %s <-itemlist=filename> <-shm=filename> <-mbx=filename> <-sleep=milliseconds> <-max_name_length=char> <-shmsize=bytes> <-debug>\n\n", filename);

  printf("Defaults:\n");
  printf("-itemlist=dataacquisition.itemlist                                                              # may be created by Browse\n");
  printf("-shm=/srv/automation/shm/dataacquisition.shm OR c:\\automation\\shm\\dataacquisition.shm on windows # location of the shared memory\n");
  printf("-mbx=/srv/automation/mbx/dataacquisition.mbx OR c:\\automation\\mbx\\dataacquisition.mbx on windows # location of the mailbox\n");
  printf("-sleep=1000                                                                                    # time between read calls\n");
  printf("-max_name_length=31                                                                            # max length of result name\n"); 
  printf("-shmsize=65536                                                                                 # total size of the shared memory\n\n");
}

#define MAX_PATH_LENGTH 1024

int main(int argc, char *argv[])
{
  char *arg;
  int   i;
  PV_DAQ_PARAM daq;

  // set the default parameters
  daq.mbx      = NULL;
  daq.provider = NULL;
  daq.running  = 0;
  strcpy(daq.itemlist,"dataacquisition.itemlist");
#ifdef _WIN32
  strcpy(daq.shmname,"c:\\automation\\shm\\dataacquisition.shm");
  strcpy(daq.mbxname,"c:\\automation\\mbx\\dataacquisition.mbx");
#else
  strcpy(daq.shmname,"/srv/automation/shm/dataacquisition.shm");
  strcpy(daq.mbxname,"/srv/automation/mbx/dataacquisition.mbx");
#endif
  daq.sleep           = 1000;
  daq.max_name_length = 31;
  daq.shmsize         = 65536;
  daq.debug           = 0;

  // check the command line parameters
  // TODO: you may add your own parameters
  for(i=1; i<argc; i++)
  {
    arg = argv[i];
    if(strlen(arg) <= MAX_PATH_LENGTH)
    {
      if(strncmp(arg,"-itemlist=",10) == 0)
      {
        sscanf(arg,"-itemlist=%s",daq.itemlist);
      }
      if(strncmp(arg,"-shm=",5) == 0)
      {
        sscanf(arg,"-shm=%s",daq.shmname);
      }
      if(strncmp(arg,"-mbx=",5) == 0)
      {
        sscanf(arg,"-mbx=%s",daq.mbxname);
      }
      if(strncmp(arg,"-sleep=",7) == 0)
      {
        sscanf(arg,"-sleep=%d",&daq.sleep);
        if(daq.sleep < 10) daq.sleep = 10;
      }
      if(strncmp(arg,"-max_name_length=",17) == 0)
      {
        sscanf(arg,"-max_name_length=%d",&daq.max_name_length);
        if(daq.max_name_length < 31) daq.max_name_length = 31;
      }
      if(strncmp(arg,"-shmsize=",8) == 0)
      {
        sscanf(arg,"-shmsize=%ld",&daq.shmsize);
        if(daq.shmsize < 128) daq.shmsize = 128;
      }
      if(strcmp(arg,"-debug") == 0)
      {
        daq.debug = 1;
      }
      if(strncmp(arg,"-h",2) == 0)
      {
        printusage(argv[0]);
        return 0;
      }
    }
    else
    {
      printf("arg is too long arg=%s\n", arg);
    }
  }

  return run(&daq); // here the work is done
}

