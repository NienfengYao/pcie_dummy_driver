#include <linux/module.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/printk.h>

#include "dummy_pcie_drv.h"

#define DRV_NAME "PCIE_DUMMY_DRIVER"
#define DRV_MODULE_VERSION "0.1"

MODULE_LICENSE("GPL v2");
MODULE_VERSION(DRV_MODULE_VERSION);

static struct class *dev_class=0;

static const struct pci_device_id pci_ids[] = {
        { PCI_DEVICE(0x10ee, 0x7014), },  //default xilinx ID
        { PCI_DEVICE(0x10ee, 0x7022), },  //default xilinx ID
        { PCI_DEVICE(0x10ee, 0x7042), },  //default xilinx ID
        { PCI_DEVICE(0x10ee, 0x7122), },
        { PCI_DEVICE(0x1e00, 0x2803), },
        {0,}
};
MODULE_DEVICE_TABLE(pci, pci_ids);



static void remove(struct pci_dev *pdev){
	printk("%s()\n", __func__);
}

static int probe(struct pci_dev *pdev, const struct pci_device_id *id){
	int rc = 0;

	printk("%s()\n", __func__);
	return rc;
}

static struct pci_driver pci_driver = {
	.name = DRV_NAME,
	.id_table = pci_ids,
	.probe = probe,
	.remove = remove,
};

static int __init dummy_pcie_init(void){
	printk("%s()\n", __func__);
	printk(KERN_NOTICE DRV_NAME " v" DRV_MODULE_VERSION "\n");
	dev_class = class_create(THIS_MODULE, DRV_NAME); if(dev_class==0) return -1;
	return pci_register_driver(&pci_driver);
}

static void __exit dummy_pcie_exit(void){
	printk("%s()\n", __func__);
	pci_unregister_driver(&pci_driver);
	class_destroy(dev_class);
}

module_init(dummy_pcie_init);
module_exit(dummy_pcie_exit);
  
