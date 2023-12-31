//******************************************************************************
//
//
// This source code is licensed under Microsoft Shared Source License
// Version 1.0 for Windows CE.
// For a copy of the license visit http://go.microsoft.com/fwlink/?LinkId=3223.
//******************************************************************************
/*++
Module Name: 
	usbclass.cpp

Abstract:  
    USB driver access class.


    davli

Modified:
    weichen

Functions:

Notes: 

--*/
#define __THIS_FILE__   TEXT("UsbClass.cpp")

#include <windows.h>
#include "syncobj.h"
#include "usbclass.h"
#include "usbserv.h"


UsbDriverArray::UsbDriverArray(BOOL bAutoDelete,DWORD arraySize) : autoDelete(bAutoDelete)
{
	DEBUGMSG(DBG_INI, (TEXT("UsbDriverArray Initial")));
	if (arraySize<5)
		arraySize=5;
	arrayUsbClientDrv= (UsbClientDrv **)malloc(sizeof(UsbClientDrv *)*arraySize);
	DEBUGMSG(DBG_INI, (TEXT("UsbDriverArray Initial addr=%lx,length=%ld"),arrayUsbClientDrv,arraySize));
	ASSERT(arrayUsbClientDrv);
	memset(arrayUsbClientDrv,0,sizeof(UsbClientDrv *)*arraySize);
	dwArraySize=arraySize;
	dwCurDevs = 0;
};
UsbDriverArray::~UsbDriverArray()
{
	if (autoDelete) {
		for (int i=0;i<(int)dwArraySize;i++) {
			if (arrayUsbClientDrv[i]!=NULL)
				delete arrayUsbClientDrv[i];
			arrayUsbClientDrv[i]=NULL;
		};
	}
	free(arrayUsbClientDrv);
}
UsbClientDrv * UsbDriverArray::operator[](int nIndex) const
{
	return GetAt(nIndex);
}
UsbDriverArray& UsbDriverArray::operator=(UsbClientDrv * oneDriver)
{
	AddClientDrv(oneDriver);
	return *this;
}

BOOL UsbDriverArray::AddClientDrv(UsbClientDrv * oneDriver)
{
	DEBUGMSG(DBG_INI, (TEXT("AddClientDrv addr=%lx,length=%ld"),arrayUsbClientDrv,dwArraySize));
	ASSERT(oneDriver);
	Lock();
	for (int i=0;i<(int)dwArraySize;i++){
		if (*(arrayUsbClientDrv+i)==NULL)
			break;
	};
	DEBUGMSG(DBG_INI,(TEXT("AddClientDrv position=%d"),i));
	BOOL bReturn;
	if ((DWORD)i<dwArraySize) {
		*(arrayUsbClientDrv+i)=oneDriver;
		DEBUGMSG(DBG_INI,(TEXT("AddClientDrv return success")));
		bReturn=TRUE;
		dwCurDevs ++;
	}
	else {
		DEBUGMSG(DBG_INI,(TEXT("AddClientDrv return failure")));
		bReturn=FALSE;
	};
	Unlock();
	return bReturn;
}
BOOL UsbDriverArray::RemoveClientDrv(UsbClientDrv * pClientDriver,BOOL bDelete)
{
	if (pClientDriver==NULL)
		return FALSE;
	Lock();
	for (int i=0;i<(int)dwArraySize;i++)
		if (arrayUsbClientDrv[i]==pClientDriver)
			break;
	BOOL bReturn;
	if ((DWORD)i<dwArraySize) {
		arrayUsbClientDrv[i]=NULL;
		if (bDelete) 
			delete pClientDriver;
		bReturn=TRUE;
		dwCurDevs --;
	}
	else
		bReturn=FALSE;
	Unlock();
	return bReturn;
}
BOOL UsbDriverArray::IsContainClientDrv(UsbClientDrv * pClientDriver)
{
	for (int i=0;i<(int)dwArraySize;i++)
		if (arrayUsbClientDrv[i]==pClientDriver)
			break;
	BOOL bReturn=(i<(int)dwArraySize);
	return bReturn;
}
BOOL UsbDriverArray::IsEmpty()
{
	Lock();
	for (int i=0;i<(int)dwArraySize;i++)
		if (arrayUsbClientDrv[i]!=NULL)
			break;
	Unlock();
	if ((DWORD)i<dwArraySize)
		return TRUE;
	else
		return FALSE;
}

UsbClientDrv * UsbDriverArray::GetAt(int nIndex) const
{
	if ((DWORD)nIndex<dwArraySize) 
		return arrayUsbClientDrv[nIndex];
	else
		return NULL;
}

BOOL USBDriverClass::CreateUsbAccessHandle(HINSTANCE hInst)
{
	lpOpenClientRegistyKey=(LPOPEN_CLIENT_REGISTRY_KEY)GetProcAddress(hInst,TEXT("OpenClientRegistryKey"));
    lpRegisterClientDriverID=(LPREGISTER_CLIENT_DRIVER_ID)GetProcAddress(hInst,TEXT("RegisterClientDriverID"));
    lpUnRegisterClientDriverID=(LPUN_REGISTER_CLIENT_DRIVER_ID)GetProcAddress(hInst,TEXT("UnRegisterClientDriverID"));
    lpRegisterClientSettings=(LPREGISTER_CLIENT_SETTINGS)GetProcAddress(hInst,TEXT("RegisterClientSettings"));
    lpUnRegisterClientSettings=(LPUN_REGISTER_CLIENT_SETTINGS)GetProcAddress(hInst,TEXT("UnRegisterClientSettings"));
    lpGetUSBDVersion=(LPGET_USBD_VERSION)GetProcAddress(hInst,TEXT("GetUSBDVersion"));
	if (lpOpenClientRegistyKey &&
			lpRegisterClientDriverID &&
			lpUnRegisterClientDriverID &&
			lpRegisterClientSettings &&
			lpUnRegisterClientSettings &&
			lpGetUSBDVersion ) {
		UsbDriverClassError=FALSE;
		return TRUE;
	}
	else {
		UsbDriverClassError=TRUE;
		DEBUGMSG(DBG_ERR,(TEXT(" Usb Liberary Load Error")));
		return FALSE;
	};
};
USBDriverClass::USBDriverClass(BOOL bAutoDelete) :UsbDriverArray(bAutoDelete)
{
	DEBUGMSG(DBG_INI,(TEXT("USBDriverClass Initial")));
    CreateUsbAccessHandle(hInst=LoadLibrary(DEFAULT_USB_DRIVER));
}
USBDriverClass::USBDriverClass(LPCTSTR lpDrvName,BOOL bAutoDelete):UsbDriverArray(bAutoDelete)
{
	DEBUGMSG(DBG_INI,(TEXT("USBDriverClass Initial")));
    CreateUsbAccessHandle(hInst=LoadLibrary(lpDrvName));
}
VOID USBDriverClass::GetUSBDVersion(LPDWORD lpdwMajorVersion, LPDWORD lpdwMinorVersion)
{
	if(lpGetUSBDVersion == NULL)
	    return;
	
	(*lpGetUSBDVersion)(lpdwMajorVersion,lpdwMinorVersion);
};
BOOL USBDriverClass::RegisterClientDriverID(LPCWSTR szUniqueDriverId)
{
	if(lpRegisterClientDriverID == NULL)
	    return FALSE;
	return (*lpRegisterClientDriverID)(szUniqueDriverId);
};
BOOL USBDriverClass::UnRegisterClientDriverID(LPCWSTR szUniqueDriverId)
{
	if(lpUnRegisterClientDriverID == NULL)
	    return FALSE;
	return (*lpUnRegisterClientDriverID)(szUniqueDriverId);
};
BOOL USBDriverClass::RegisterClientSettings(LPCWSTR lpszDriverLibFile,
                            LPCWSTR lpszUniqueDriverId, LPCWSTR szReserved,
                            LPCUSB_DRIVER_SETTINGS lpDriverSettings)
{
	if(lpRegisterClientSettings == NULL)
	    return FALSE;
	return (*lpRegisterClientSettings)(lpszDriverLibFile,lpszUniqueDriverId,szReserved,lpDriverSettings);
}
BOOL USBDriverClass::UnRegisterClientSettings(LPCWSTR lpszUniqueDriverId, LPCWSTR szReserved,
                              LPCUSB_DRIVER_SETTINGS lpDriverSettings)
{
	if(lpUnRegisterClientSettings == NULL)
	    return FALSE;
	return (*lpUnRegisterClientSettings)(lpszUniqueDriverId,szReserved,lpDriverSettings);
}
HKEY USBDriverClass::OpenClientRegistryKey(LPCWSTR szUniqueDriverId)
{
	if(lpOpenClientRegistyKey == NULL)
	    return NULL;
	return (*lpOpenClientRegistyKey)(szUniqueDriverId);
};




