#include <inttypes.h>
#include <avr/pgmspace.h>

#include <avrpins.h>
#include <max3421e.h>
#include <usbhost.h>
#include <usb_ch9.h>
#include <Usb.h>
#include <usbhub.h>
#include <address.h>

#include <printhex.h>
#include <message.h>
#include <hexdump.h>
#include <parsetools.h>

#include <ptp.h>
#include <ptpcallback.h>
#include <ptpdebug.h>
#include "ptpobjhandleparser.h"

class CamStateHandlers : public PTPStateHandlers
{
      enum CamStates { stInitial, stDisconnected, stConnected };
      CamStates stateConnected;
    
public:
      CamStateHandlers() : stateConnected(stInitial) {};
      
      virtual void OnDeviceDisconnectedState(PTP *ptp);
      virtual void OnDeviceInitializedState(PTP *ptp);
} CamStates;

USB      Usb;
USBHub   Hub1(&Usb);
PTP      Ptp(&Usb, &CamStates);

void CamStateHandlers::OnDeviceDisconnectedState(PTP *ptp)
{
    if (stateConnected == stConnected || stateConnected == stInitial)
    {
        stateConnected = stDisconnected;
        Notify(PSTR("Camera disconnected\r\n"), 0x80);
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp)
{
    if (stateConnected == stDisconnected || stateConnected == stInitial)
    {
        stateConnected = stConnected;
        Notify(PSTR("Camera connected\r\n"), 0x80);
        
      	HexDump          dmp;
      	ptp->GetObjectHandles(0xFFFFFFFF, 0, 0, &dmp);
        Notify(PSTR("\n"), 0x80);
              
        PTPObjHandleParser  prs;
      	ptp->GetObjectHandles(0xFFFFFFFF, 0, 0, &prs);
        Notify(PSTR("\n"), 0x80);
    }
}

void setup() 
{
    Serial.begin( 115200 );
    Serial.println("Start");

    if (Usb.Init() == -1)
        Serial.println("OSC did not start.");
    
    delay( 200 );
}

void loop() 
{
    Usb.Task();
}
