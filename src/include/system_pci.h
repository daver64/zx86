#ifndef _SYSTEM_PCI_H_
#define _SYSTEM_PCI_H_


#ifdef __cplusplus
extern "C"
{
#endif

/* PCI.C */
void pci_init();

// the device vendor
char *pci_codes_lookup_vendor(unsigned short vendor_id);
// the exact device name
char *pci_codes_lookup_device(unsigned short device_id, unsigned short vendor_id, unsigned short subvendor_id, unsigned short subsystem_id);
// PCI device type (network adapter, disk controller, GPU, ur mum, etc)
char *pci_codes_lookup_type(unsigned char pciclass, unsigned char subclass, unsigned char prog_if);
#ifdef __cplusplus
}
#endif
#endif