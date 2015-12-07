#ifndef H_PCI
#define H_PCI

#include <stdint.h>

void pciinit();
uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

#endif
