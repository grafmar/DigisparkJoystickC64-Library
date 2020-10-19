/*
 * Based on Obdev's AVRUSB code and under the same license.
 *
 * TODO: Make a proper file header. :-)
 * Modified for Digispark by Digistump
 * And now modified by Sean Murphy (duckythescientist) from a keyboard device to a joystick device
 * And now modified by Bluebie to have better code style, not ruin system timers, and have delay() function
 * Most of the credit for the joystick code should go to Raphaël Assénat
 */
#ifndef __DigiJoystick_h__
#define __DigiJoystick_h__
 
#define GCN64_REPORT_SIZE 8

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <string.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "usbdrv.h"
//#include "devdesc.h"
#include "oddebug.h"
#include "usbconfig.h"

#ifdef USB_CFG_SERIAL_NUMBER_LENGTH
#undef USB_CFG_SERIAL_NUMBER_LENGTH
#endif // USB_CFG_SERIAL_NUMBER_LENGTH

const static uchar *rt_usbHidReportDescriptor=NULL;
static uchar rt_usbHidReportDescriptorSize=0;
const static uchar *rt_usbDeviceDescriptor=NULL;
static uchar rt_usbDeviceDescriptorSize=0;

// TODO: Work around Arduino 12 issues better.
//#include <WConstants.h>
//#undef int()

//typedef uint8_t byte;

/* What was most recently read from the controller */
unsigned char last_built_report[GCN64_REPORT_SIZE];

/* What was most recently sent to the host */
unsigned char last_sent_report[GCN64_REPORT_SIZE];

uchar        reportBuffer[8];

// report frequency set to default of 50hz
#define DIGIJOYSTICK_DEFAULT_REPORT_INTERVAL 20
static unsigned char must_report = 0;
static unsigned char idle_rate = DIGIJOYSTICK_DEFAULT_REPORT_INTERVAL / 4; // in units of 4ms
// new minimum report frequency system:
static unsigned long last_report_time = 0;


const unsigned char gcn64_usbHidReportDescriptor[] PROGMEM = {
    0x05, 0x01,             //  Usage Page (Generic Desktop)
    0x09, 0x04,             //  Usage (Joystick)
    0xA1, 0x01,             //  Collection (Application)
    0xA1, 0x02,             //      Collection (Logical)
    0x75, 0x08,             //          Report Size (8)
    0x95, 0x05,             //          Report Count (5)
    0x15, 0x00,             //          Logical Minimum (0)
    0x26, 0xFF, 0x00,       //          Logical Maximum (255)
    0x35, 0x00,             //          Physical Minimum (0)
    0x46, 0xFF, 0x00,       //          Physical Maximum (255)
    0x09, 0x30,             //          Usage (X)
    0x09, 0x31,             //          Usage (Y)
    0x09, 0x31,             //          Usage (Y)
    0x09, 0x31,             //          Usage (Y)
    0x09, 0x31,             //          Usage (Y)
    0x81, 0x02,             //          Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)
    0x75, 0x04,             //          Report Size (4)
    0x95, 0x01,             //          Report Count (1)
    0x81, 0x01,             //          Input (Cnst,Ary,Abs)
    0x75, 0x01,             //          Report Size (1)
    0x95, 0x0C,             //          Report Count (12)
    0x25, 0x01,             //          Logical Maximum (1)
    0x45, 0x01,             //          Physical Maximum (1)
    0x05, 0x09,             //          Usage Page (Button)
    0x19, 0x01,             //          Usage Minimum (Button 1)
    0x29, 0x08,             //          Usage Maximum (Button 8)
    0x81, 0x02,             //          Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)
    0x06, 0x00, 0xFF,       //          Usage Page (Vendor-Defined 1)
    0x75, 0x01,             //          Report Size (1)
    0x95, 0x08,             //          Report Count (8)
    0x25, 0x01,             //          Logical Maximum (1)
    0x45, 0x01,             //          Physical Maximum (1)
    0x09, 0x01,             //          Usage (Vendor-Defined 1)
    0x81, 0x02,             //          Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)
    0xC0,                   //      End Collection
    0xA1, 0x02,             //      Collection (Logical)
    0x75, 0x08,             //          Report Size (8)
    0x95, 0x07,             //          Report Count (7)
    0x46, 0xFF, 0x00,       //          Physical Maximum (255)
    0x26, 0xFF, 0x00,       //          Logical Maximum (255)
    0x09, 0x02,             //          Usage (Vendor-Defined 2)
    0x91, 0x02,             //          Output (Data,Var,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)
    0xC0,                   //      End Collection
    0xC0                    //  End Collection
};


#define USBDESCR_DEVICE                 1

const unsigned char usbDescrDevice[] PROGMEM = {        /* USB device descriptor */
        18,                 /* sizeof(usbDescrDevice): length of descriptor in bytes */
        USBDESCR_DEVICE,        /* descriptor type */
        0x01, 0x01, /* USB version supported */
        USB_CFG_DEVICE_CLASS,
        USB_CFG_DEVICE_SUBCLASS,
        0,                  /* protocol */
        8,                  /* max packet size */
        USB_CFG_VENDOR_ID,  /* 2 bytes */
        USB_CFG_DEVICE_ID,  /* 2 bytes */
        USB_CFG_DEVICE_VERSION, /* 2 bytes */
#if USB_CFG_VENDOR_NAME_LEN
        1,                  /* manufacturer string index */
#else
        0,                  /* manufacturer string index */
#endif
#if USB_CFG_DEVICE_NAME_LEN
        2,                  /* product string index */
#else
        0,                  /* product string index */
#endif

 #if USB_CFG_SERIAL_NUMBER_LENGTH
        3,                  /* serial number string index */
#else
        0,                  /* serial number string index */
#endif
        1,                  /* number of configurations */
};


void gamecubeBuildReport(unsigned char *reportBuf) {
    if (reportBuf != NULL) {
        memcpy(reportBuf, last_built_report, GCN64_REPORT_SIZE);
    }
    
    memcpy(last_sent_report, last_built_report, GCN64_REPORT_SIZE); 
}

int getGamepadReport(unsigned char *dstbuf) {
    gamecubeBuildReport(dstbuf);
    return GCN64_REPORT_SIZE;
}

 
class DigiJoystickDevice {
 public:
    DigiJoystickDevice () {
        cli();
        usbDeviceDisconnect();
        _delay_ms(250);
        usbDeviceConnect();
    
        rt_usbHidReportDescriptor = gcn64_usbHidReportDescriptor;
        rt_usbHidReportDescriptorSize = sizeof(gcn64_usbHidReportDescriptor);
        rt_usbDeviceDescriptor = usbDescrDevice;
        rt_usbDeviceDescriptorSize = sizeof(usbDescrDevice);
        
        usbInit();
        
        sei();
        
        last_report_time = millis();
    }
    
    void update() {
        usbPoll();
        
        // instead of above code, use millis arduino system to enforce minimum reporting frequency
        unsigned long time_since_last_report = millis() - last_report_time;
        if (time_since_last_report >= (idle_rate * 4 /* in units of 4ms - usb spec stuff */)) {
            last_report_time += idle_rate * 4;
            must_report = 1;
        }
        
        // if the report has changed, try force an update anyway
        if (memcmp(last_built_report, last_sent_report, GCN64_REPORT_SIZE)) {
            must_report = 1;
        }
    
        // if we want to send a report, signal the host computer to ask us for it with a usb 'interrupt'
        if (must_report) {
            if (usbInterruptIsReady()) {
                must_report = 0;
                
                gamecubeBuildReport(reportBuffer);
                usbSetInterrupt(reportBuffer, GCN64_REPORT_SIZE);
            }
        }
    }
    
    // delay while updating until we are finished delaying
    void delay(long milli) {
        unsigned long last = millis();
      while (milli > 0) {
        unsigned long now = millis();
        milli -= now - last;
        last = now;
        update();
      }
    }
    
    void setX(byte value) {
        last_built_report[0] = value;
    }
    
    void setY(byte value) {
        last_built_report[1] = value;
        last_built_report[2] = value;
        last_built_report[3] = value;
        last_built_report[4] = value;
    }
    
    void setX(char value) {
        setX(*(reinterpret_cast<byte *>(&value)));
    }
    
    void setY(char value) {
        setY(*(reinterpret_cast<byte *>(&value)));
    }
    
    
    void setButtons(unsigned char low) {
        //  7F 7F 7F 7F 7F 0F 00 40
        last_built_report[5] = 0x0F | (low << 4);
        last_built_report[6] = low >> 4;
        last_built_report[7] = 0x40;
    }
    
    void setButtons(char low) {
        setButtons(*reinterpret_cast<unsigned char *>(&low));
    }
    
    void setValues(unsigned char values[]) {
        memcpy(last_built_report, values, GCN64_REPORT_SIZE);
    }
    
    void setValues(char values[]) {
        unsigned char *foo = reinterpret_cast<unsigned char *>(values);//preserves bit values in cast
        memcpy(last_built_report, foo, GCN64_REPORT_SIZE);
    }
};

// Create global singleton object for users to make use of
DigiJoystickDevice DigiJoystick = DigiJoystickDevice();





#ifdef __cplusplus
extern "C"{
#endif 
    // USB_PUBLIC uchar usbFunctionSetup
    
    uchar usbFunctionSetup(uchar data[8]) {
        usbRequest_t *rq = (usbRequest_t *)data;

        usbMsgPtr = reportBuffer;
        if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) { // class request type
            if (rq->bRequest == USBRQ_HID_GET_REPORT){ // wValue: ReportType (highbyte), ReportID (lowbyte)
                // we only have one report type, so don't look at wValue
                //curGamepad->buildReport(reportBuffer);
                //return curGamepad->report_size;
                return GCN64_REPORT_SIZE;
            } else if (rq->bRequest == USBRQ_HID_GET_IDLE) {
                usbMsgPtr = &idle_rate;
                return 1;
            } else if (rq->bRequest == USBRQ_HID_SET_IDLE) {
                idle_rate = rq->wValue.bytes[1];
            }
        } else {
            /* no vendor specific requests implemented */
        }
        return 0;
    }

    uchar usbFunctionDescriptor(struct usbRequest *rq) {
        if ((rq->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_STANDARD) {
            return 0;
        }

        if (rq->bRequest == USBRQ_GET_DESCRIPTOR) {
            // USB spec 9.4.3, high byte is descriptor type
            switch (rq->wValue.bytes[1]) {
                case USBDESCR_DEVICE:
                    usbMsgPtr = rt_usbDeviceDescriptor;
                    return rt_usbDeviceDescriptorSize;
                    break;
                    
                case USBDESCR_HID_REPORT:
                    usbMsgPtr = rt_usbHidReportDescriptor;
                    return rt_usbHidReportDescriptorSize;
                    break;
                    
            }
        }
        
        return 0;
    }
    
#ifdef __cplusplus
} // extern "C"
#endif


#endif // __DigiKeyboard_h__
