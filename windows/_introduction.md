# Kernel Module Examples

Windows Kernel Module Driver

## Driver Type

#### KMDF Drivers

- Device Function Driver
- Device Filter Driver
- Software Driver
- Filesystem Filter Driver
- Filesystem Driver

#### WDM Drivers

- `Bus Driver`: menangani I/O bus device dan menawarkan fungsionalitas yang independen terhadap perangkat.
- `Function Driver`: menangani perangkat secara individual.
- `Filter Driver`: menyarin I/O request untuk device, class of device, dan bus.

## Driver Packages

Pada umumnya, setiap kompilasi driver yang berhasil akan menghasilkan beberapa file:

- `driver.sys`: binary dari driver.
- `driver.inf`: informasi terkait driver
- `driver.cer`: sertifikat untuk signed driver.

## Windows Driver Architecture

#### Device Objects and Stacks

## I/O Model

Data direpresentasikan sebagai Request Packets atau I/O Request Packets. Mekanisme komunikasi ini diadopsi oleh Windows untuk berkomunikasi antar komponen berbeda dalam Windows Driver Architecture.

