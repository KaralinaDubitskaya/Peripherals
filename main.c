#include <stdio.h>
#include <sys/io.h>
#include "pci.h"


int getV(unsigned short int data[2])
{
    int i = 0;
    for (i; i < PCI_VENTABLE_LEN; i++)
    {
        if (PciVenTable[i].VendorId == data[0])
        {
            printf("Vendor ID: %x", data[0]);
            printf("  %s\n", PciVenTable[i].VendorName);
            return 0;
        }
    }
    return 0;
}


int getD(unsigned short int data[2])
{
    int i = 0;
    for (i; i < PCI_DEVTABLE_LEN; i++)
    {
        if (PciDevTable[i].VendorId == data[0])
        {
            if (PciDevTable[i].DeviceId == data[1])
            {
                printf("Device ID: %x", data[1]);
                printf("  %s\n", PciDevTable[i].DeviceName);
                return 0;
            }
        }
    }
    return 0;
}

void getDevVen(unsigned int *datar)
{
    unsigned short int *data;
    data = (unsigned short int*)datar;
    getV(data);
    getD(data);
}

void getInterruptInfo(unsigned int data1)
{
    char *data = (char*)&data1;
    printf("Interrupt line: ");
    switch (data[0])
    {
        case 255: printf("255(unknown input or not used)\n");
            break;
        default:
            printf("%d\n", data[0]);
            break;
    }
    printf("Interrupt pin: ");
    switch (data[1])
    {
        case 0: printf("not used(0)\n");
            break;
        case 1: printf("INTA#(1)\n");
            break;
        case 2: printf("INTB#(2)\n");
            break;
        case 3: printf("INTC#(3)\n");
            break;
        case 4: printf("INTD#(4)\n");
            break;
        case 5: printf("FFh reserve(5)\n");
            break;
        default:
            printf("%d\n", data[1]);
            break;
    }
}

void getPortInfo(unsigned int *datar)
{
    char *data;
    data = (char*)datar;
    printf("Primary bus number: %d\n", data[0]);
    printf("Secondary bus number: %d\n", data[1]);
    printf("Subordinate bus number: %d\n", data[2]);
}

void getInfo()
{
    unsigned int addr = 0x80000000;
    unsigned int bus = addr;
    char i, j, k;
    i = 0;
    for (bus; bus <= 0x80FF0000; bus += 0x10000)
    {
        j = 0;
        unsigned int dev = bus;
        for (dev; dev <= bus + 0xF800; dev += 0x800)
        {
            k = 0;
            unsigned int func = dev;
            for (func; func <= dev + 0x700; func += 0x100)
            {
                outl(func, 0xCF8);
                unsigned int data = inl(0xCFC);
                if (data != 0xFFFFFFFF)
                {
                    printf("%d:%d:%d\n", i, j, k);
                    printf("%x\n", data);
                    getDevVen(&data);
                    func += 0x0C;
                    outl(func, 0xCF8);
                    data = inl(0xCFC);
                    if (!(data & 0x10000))
                    {
                        printf("not port\n");
                        func -= 0x0C;
                        func += 0x3C;
                        outl(func, 0xCF8);
                        data = inl(0xCFC);
                        getInterruptInfo(data);
                        func -= 0x3C;
                    }
                    else
                    {
                        printf("port\n");
                        func -= 0x0C;
                        func += 0x18;
                        outl(func, 0xCF8);
                        data = inl(0xCFC);
                        getPortInfo(&data);
                        func -= 0x18;
                    }
                    printf("---------------------\n");
                }
                else
                    break;
                k++;
            }
            j++;
        }
        i++;
    }
}

// Составить адрес конфигурационного регистра по номеру шины,
// номеру устройства, номеру функции и номеру регистра
unsigned long calculateAddress(int bus, int device, int function, int reg)
{
    unsigned long address = 1;
    address = address << 15;
    address += bus;          // Номер шины, 8 бит
    address = address << 5;
    address += device;       // Номер устройства, 5 бит
    address = address << 3;
    address += function;     // Номер функции, 3 бита
    address = address << 8;
    address += reg;
    return address;
}

int main(void)
{
    printf("\n\nbegin\n");
    if(iopl(3))
    {
        printf("I/O Privilege level change error.\nTry running under ROOT user\n");
        return 1;
    } //granted privileges 3 for port access
    getInfo();
    printf("end\n");
    return 0;
}
