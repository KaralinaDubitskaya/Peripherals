#include <stdio.h>
#include <sys/io.h>
#include "pci.h"


int GetVendorID(unsigned short int data[2]) {
    // pci.h
    for (int i = 0; i < PCI_VENTABLE_LEN; i++)
    {
        if (PciVenTable[i].VendorId == data[0])
        {
            printf("Vendor ID: 0x%X", data[0]);
            printf("  %s\n", PciVenTable[i].VendorName);
            return 0;
        }
    }
    return 0;
}


int GetDeviceID(unsigned short int data[2]) {
    // pci.h
    for (int i = 0; i < PCI_DEVTABLE_LEN; i++)
    {
        if (PciDevTable[i].VendorId == data[0])
        {
            if (PciDevTable[i].DeviceId == data[1])
            {
                printf("Device ID: 0x%X", data[1]);
                printf("  %s\n", PciDevTable[i].DeviceName);
                return 0;
            }
        }
    }
    return 0;
}


void GetClassCodeInfo(unsigned int *dataReg) {
    char *data;
    data = (char *) dataReg;
    printf("\nBase class: %.2X\n", data[3]);
    printf("Subclass:   %.2X\n", data[2]);
    printf("Interface:  %.2X\n", data[1]);

    if ((data[3] == 06) && (data[2] == 04) && (data[1] == 00))
    {
        printf("\nClass: Bus PCI-PCI\n");
    }
}

/* Calculate the address of the configuration register using the bus number,
   the device number, function number and register number */
unsigned int CalculateAddress(int bus, int device, int function, int reg) {
    unsigned int address = 1;
    address = address << 15;
    address += bus;          // Bus number, 8 bits
    address = address << 5;
    address += device;       // Device number, 5 bits
    address = address << 3;
    address += function;     // Function number, 3 bits
    address = address << 8;
    address += reg;          // Port number
    return address;
}

void ShowDeviceInfo(int bus, int dev, int func) {

    // Configuration register address
    unsigned int configAddress = CalculateAddress(bus, dev, func, 0x00);

    // Read fields of the configuration space
    outl(configAddress, 0xCF8);
    unsigned int regData = inl(0xCFC);

    if (regData != 0xFFFFFFFF) // If there is no device, then the register value is = 0xFFFFFFFF
    {
        printf("Address: %d:%d.%d\n", bus, dev, func);

        GetVendorID((unsigned short int *)&regData);
        GetDeviceID((unsigned short int *)&regData);

        // Get Header Type field address
        configAddress += 0x0C;
        outl(configAddress, 0xCF8);
        regData = inl(0xCFC);

        configAddress -= 0x0C;

        if (!(regData & 0x10000))
        {
            // *Not bus*
            // Get Base I/O register field address
            configAddress += 0x10;

            for (int i = 0; i < 6; i++)
            {
                configAddress += i * 0x04;
                outl(configAddress, 0xCF8);
                regData = inl(0xCFC);

                if (regData & 1)
                {
                    printf("\nBAR I/O registers: 0x%.8X\n", regData);
                }
            }

            // Get ROM registers field address
            configAddress -= 0x24;
            configAddress += 0x30;
            outl(configAddress, 0xCF8);
            regData = inl(0xCFC);

            printf("\nROM registers: 0x%X\n", regData);
        }
        else
        {
            // *Bus*
            // Get Class Code field address
            configAddress += 0x08;
            outl(configAddress, 0xCF8);
            regData = inl(0xCFC);

            GetClassCodeInfo(&regData);
        }

        printf("_________________________________________________________________________________________________________\n\n");
    }

}

int main(void) {
    //Granted privileges 3 for port access
    if (iopl(3))
    {
        printf("I/O Privilege level change error.\nTry running under ROOT user\n");
        return 1;
    }

    //Main cycle
    for (int bus = 0; bus < 256; bus++)              // Bus number, 8 bits, 2^8 = 256
        for (int dev = 0; dev < 32; dev++)           // Device number, 5 bits, 2^5 = 32
            for (int func = 0; func < 8; func++)     // Function number, 3 bits, 2^3 = 8
                ShowDeviceInfo(bus, dev, func);

    return 0;
}
