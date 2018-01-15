/*
Copyright (c) 2017-2018 NicoHood
See the readme for credit to other people.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "usb.h"

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

    // Attention! Order is important because of ORDERED_EP_CONFIG!
#ifdef DMBS_MODULE_USB_CDC_SERIAL
    usb_cdc_serial_init();
	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
#endif
#ifdef DMBS_MODULE_USB_KEYBOARD
	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Keyboard_HID_Interface);
#endif

	USB_Device_EnableSOFEvents();
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
#ifdef DMBS_MODULE_USB_KEYBOARD
    HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
#endif
#ifdef DMBS_MODULE_USB_CDC_SERIAL
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
#endif
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
    uint8_t PrevSelectedEndpoint = Endpoint_GetCurrentEndpoint();

#ifdef DMBS_MODULE_USB_KEYBOARD
    // Update HID reports if required
    HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
    HID_Device_USBTask(&Keyboard_HID_Interface);
#endif

#ifdef DMBS_MODULE_USB_CDC_SERIAL
    // TODO a general problem:
    // when you send data via cdc serial, but nobody aknowledges this, it will block for a long time
    // after that the sketch will continue and the keyboard code etc can continue.
    // https://github.com/abcminiuser/lufa/issues/106
    // Flush CDC serial
    CDC_Device_MillisecondElapsed();
    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
#endif

    Endpoint_SelectEndpoint(PrevSelectedEndpoint);
}

#ifdef DMBS_MODULE_USB_KEYBOARD
/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean \c true to force the sending of the report, \c false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
    // Determine which interface must have its report generated
    if (ReportType == HID_REPORT_ITEM_In){
        if (HIDInterfaceInfo == &Keyboard_HID_Interface)
        {
            *ReportSize = sizeof(USB_KeyboardReport_Data_t);
            return CALLBACK_HID_Keyboard_CreateHIDReport((USB_KeyboardReport_Data_t*) ReportData);
        }
    }
	return false;
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
    // Determine which interface received an hid report
    if (ReportType == HID_REPORT_ITEM_Out) {
        if (HIDInterfaceInfo == &Keyboard_HID_Interface)
        {
            CALLBACK_HID_Keyboard_ProcessHIDReport((uint8_t*)ReportData);
        }
    }
}
#endif // #ifdef DMBS_MODULE_USB_KEYBOARD
