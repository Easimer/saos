#include <dev/pci.h>
#include <stdint.h>
#include <stddef.h>
#include <port.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct _pci_device {
  uint16_t vendor;
  uint16_t device;
  char name[64];
} _pci_devices[] = {
//  Vendor   Device  Name
    {0x8086, 0x1237, "Intel PCI & Memory"},
    {0x8086, 0x7000, "Intel PIIX3 PCI2ISA Bridge (Triton II)"},
    {0x8086, 0x1038, "Intel PRO/100"},
    {0x8086, 0x100e, "Intel Pro 1000/MT"},
    {0x1234, 0x1111, "QEMU/Bochs Virtual VGA"},
    {0x10ec, 0x8029, "Ne2000 PCI"},
    {}
};

#define PCI_PORT_CONF_ADDR 0xCF8
#define PCI_PORT_CONF_DATA 0xCFC

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
  uint32_t addr;
  uint32_t lbus = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)func;
  uint16_t t = 0;

  addr = (uint32_t)((lbus<<16) | (lslot << 11) | (lfunc << 8) |
          (offset & 0xfc) | ((uint32_t)0x80000000));
  outl(PCI_PORT_CONF_ADDR, addr);
  uint32_t in = inl(0xCFC);
  t = (uint16_t)((in >> ((offset & 2) * 8)) & 0xffff);
  return t;
}

void pciinit()
{
  puts("Probing PCI bus...\n");
  for(int bus = 0; bus < 4; bus++)
  {
    for(int slot = 0; slot < 0xf; slot++)
    {
      char b[16];
      memset(b, 0, 16);
      puts("  "); itoa(bus, b, 16); puts(b);
      putc(':'); itoa(slot, b, 16); puts(b);
      puts(": ");
      uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
      if(vendor == 0xffff)
      {
        puts("No device\n");
        continue;
      }
      uint16_t device = pci_config_read_word(bus, slot, 0, 2);
      puts("Vendor: 0x"); itoa(vendor, b, 16); puts(b);
      puts(" Device: 0x"); itoa(device, b, 16); puts(b);
      puts(" - ");
      struct _pci_device* pdptr = &_pci_devices[0];
      while(pdptr->vendor != 0)
      {
        if(pdptr->vendor == vendor && pdptr->device == device)
        {
          puts(pdptr->name);
          break;
        }
        pdptr++;
      }
      if(pdptr->vendor == 0) puts("Unknown");
      putc('\n');
    }
  }
}
