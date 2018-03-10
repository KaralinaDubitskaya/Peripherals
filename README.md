# Peripherals

## Description
The program generates a cycle for checking and identifying PCI devices, 
reading the first two fields of the configuration space: Vendor ID (manufacturer) 
and Device ID (device) codes. 
Using the structures defined in the header file pci.h, the names of manufacturers 
and devices are decoded.

The result of the program is the information displayed on the screen:
1. device address (bus number, device number and function number);
2. 16-bit manufacturer's code (in hex-format);
3. 16-bit device code (in hex-format);
4. manufacturer and name of the device.

If the device is not a bridge (0-bit of the Header Type field = 0), the value of the fields 
of the basic I / O registers and the base memory (ROM) fields of the memory are output and decrypt.
If the device is a bridge (0-bit field Header Type = 1) is output and the value 
of the field Class Code is deciphered.

## Usage
To execute the program just open terminal and run:
```
$ sudo ./Peripherals.out
```

## Contact
Karalina Dubitskaya                                                                                              
karalinadubitskaya@gmail.com
