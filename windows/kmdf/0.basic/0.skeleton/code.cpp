/*
	Windows KMDF Skeleton
	Archive of Reversing.ID
	Universal Driver

	Tested on Windows 10.0.19041

	Kerangka kode Windows KMDF (Kernel-Mode Drivers Framework)

Load:
	$ sc create DriverService type=kernel binpath=code.sys
	$ sc start DriverService

Unload:
	$ sc stop DriverService
	$ sc delete DriverService
*/
#pragma warning (disable : 4100)

#include <ntddk.h>
#include <wdf.h>



/* ****************** Type Definition & Data Declaration ****************** */

DRIVER_INITIALIZE			entry;
EVT_WDF_DRIVER_DEVICE_ADD	EventAddCallback;


/* ********************* Internal Functions Prototype ********************* */

VOID UnloadDriver (PDRIVER_OBJECT driver);


/* *************************** Helper Functions *************************** */


/* ****************** Loadable Kernel Module Initialize ******************* */

NTSTATUS DriverEntry (PDRIVER_OBJECT driver, PUNICODE_STRING regpath)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDF_DRIVER_CONFIG config;

	WDF_DRIVER_CONFIG_INIT (&config, EventAddCallback);

	driver->DriverUnload = UnloadDriver;

	status = WdfDriverCreate (driver, regpath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);

	return status;
}


NTSTATUS EventAddCallback (WDFDRIVER driver, PWDFDEVICE_INIT devinit)
{
	UNREFERENCED_PARAMETER (driver);
	NTSTATUS	status;
	WDFDEVICE	device;

	status = WdfDeviceCreate (&devinit, WDF_NO_OBJECT_ATTRIBUTES, &device);

	return status;
}

VOID UnloadDriver (PDRIVER_OBJECT driver)
{
	
}