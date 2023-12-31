//******************************************************************************
//
//
// This source code is licensed under Microsoft Shared Source License
// Version 1.0 for Windows CE.
// For a copy of the license visit http://go.microsoft.com/fwlink/?LinkId=3223.
//******************************************************************************

/*++

Module Name:  
    usbclass.h

Abstract:  
    USB driver access class.
    
Notes: 

--*/
#ifndef __USBCLASS_H_
#define __USBCLASS_H_

#include <usbdi.h>
#include "syncobj.h"
#include "usbtest.h"

#define DEFAULT_USB_DRIVER TEXT("USBD.DLL")

class UsbClientDrv;
#define MAX_SERVICE_ENTRY 5
#define MAX_USB_CLIENT_DRIVER 128
class UsbDriverArray : public CCriticalSection {
public:
	UsbDriverArray(BOOL bAutoDelete,DWORD arraySize=MAX_USB_CLIENT_DRIVER);
	~UsbDriverArray();
	UsbClientDrv * operator[](int nIndex) const;
	UsbDriverArray& operator=(UsbClientDrv * oneDriver);
	BOOL AddClientDrv(UsbClientDrv * );
	BOOL RemoveClientDrv(UsbClientDrv *,BOOL bDelete );
	UsbClientDrv * GetAt(int nIndex) const;
	DWORD GetArraySize() { return dwArraySize; };
       DWORD GetCurAvailDevs() {return dwCurDevs; };
	BOOL IsEmpty();
	BOOL IsContainClientDrv(UsbClientDrv * pClientDriver);
private:
	UsbClientDrv ** arrayUsbClientDrv;
	const BOOL autoDelete;
	DWORD dwArraySize;
       DWORD dwCurDevs;

};

class USBDriverClass : public UsbDriverArray{
public:
	USBDriverClass(BOOL bAutoDelete=TRUE) ;
	USBDriverClass(LPCTSTR lpDrvName,BOOL bAutoDelete=TRUE);
	~USBDriverClass() { FreeLibrary(hInst);};
 
//access function
	VOID GetUSBDVersion(LPDWORD lpdwMajorVersion, LPDWORD lpdwMinorVersion);
	BOOL RegisterClientDriverID(LPCWSTR szUniqueDriverId);
	BOOL UnRegisterClientDriverID(LPCWSTR szUniqueDriverId);
	BOOL RegisterClientSettings(LPCWSTR lpszDriverLibFile,
                            LPCWSTR lpszUniqueDriverId, LPCWSTR szReserved,
                            LPCUSB_DRIVER_SETTINGS lpDriverSettings);
	BOOL UnRegisterClientSettings(LPCWSTR lpszUniqueDriverId, LPCWSTR szReserved,
                              LPCUSB_DRIVER_SETTINGS lpDriverSettings);
	HKEY OpenClientRegistryKey(LPCWSTR szUniqueDriverId);

	BOOL IsOk() { return(!UsbDriverClassError);};
private:
	BOOL CreateUsbAccessHandle(HINSTANCE hInst);
    LPOPEN_CLIENT_REGISTRY_KEY          lpOpenClientRegistyKey;
    LPREGISTER_CLIENT_DRIVER_ID         lpRegisterClientDriverID;
    LPUN_REGISTER_CLIENT_DRIVER_ID      lpUnRegisterClientDriverID;
    LPREGISTER_CLIENT_SETTINGS          lpRegisterClientSettings;
    LPUN_REGISTER_CLIENT_SETTINGS       lpUnRegisterClientSettings;
    LPGET_USBD_VERSION                  lpGetUSBDVersion;
	BOOL UsbDriverClassError;
	HINSTANCE hInst;
};

extern USBDriverClass *pUsbDriver;
extern const   TCHAR   gcszTestDriverId[];


#endif
