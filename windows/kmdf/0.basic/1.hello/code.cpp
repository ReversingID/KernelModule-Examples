/*
    Hello
    Archive of Reversing.ID
    Universal Driver

    Tested on Windows 10.0.19041

	Mencetak informasi di kernel mode.

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


DRIVER_INITIALIZE			entry;
EVT_WDF_DRIVER_DEVICE_ADD	EventAddCallback;

VOID UnloadDriver (PDRIVER_OBJECT driver);


/* 
	DriverEntry adalah entrypoint, fungsi yang akan pertama kali dipanggil.

	Umumnya entrypoint akan melakukan inisialisasi dan meregister fungsi-fungsi
	lain untuk menangani event.
*/
NTSTATUS 
DriverEntry (PDRIVER_OBJECT driver, PUNICODE_STRING regpath)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDF_DRIVER_CONFIG config;

    KdPrintEx ((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "RevID: Driver Entry!\n"));

    // inisialisasi driver configuration untuk register callback EvtDeviceAdd
	WDF_DRIVER_CONFIG_INIT (&config, EventAddCallback);

	// mendaftarkan UnloadDriver sebagai handler
	driver->DriverUnload = UnloadDriver;

    // ciptakan Driver Object
	status = WdfDriverCreate (driver, regpath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);

	return status;
}

/*
	Dipanggil oleh framework ketika perangkat terdeteksi dan AddDevice()
	dipanggil oleh PnP manager.
	Digunakan untuk inisialisasi struktur data dan resource untuk perangkat.
*/
NTSTATUS 
EventAddCallback (WDFDRIVER driver, PWDFDEVICE_INIT devinit)
{
    /* parameter driver tidak diperlukan sehingga dapat ditandai unreferenced */
	UNREFERENCED_PARAMETER (driver);
	NTSTATUS	status;
	WDFDEVICE	device;

    // ciptakan Device Object
	status = WdfDeviceCreate (&devinit, WDF_NO_OBJECT_ATTRIBUTES, &device);

	KdPrintEx ((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "RevID: device added!\n"));

	return status;
}

/*
	Dipanggil oleh framework ketika driver dikeluarkan dari kernel.
	Harus didaftarkan terlebih dahulu.

	Digunakan untuk membebaskan resource yang telah dialokasikan selama inisialisasi.
*/
VOID
UnloadDriver (PDRIVER_OBJECT driver)
{
}