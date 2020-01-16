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
static int gInstance=0;
static DEFINE_MUTEX(gti_mutex);

static const struct pci_device_id pci_ids[] = {
        { PCI_DEVICE(0x10ee, 0x7014), },  //default xilinx ID
        { PCI_DEVICE(0x10ee, 0x7022), },  //default xilinx ID
        {0,}
};
MODULE_DEVICE_TABLE(pci, pci_ids);


static void remove(struct pci_dev *pdev){
	NFY_DEV * pDev=pci_get_drvdata(pdev);

	printk("%s()\n", __func__);
	if(pDev->exitFn) pDev->exitFn(pDev);
	pci_release_regions(pdev);
	pci_disable_msi(pdev);
	pci_disable_device(pdev);
}

static int probe(struct pci_dev *pdev, const struct pci_device_id *id){
	int rc = 0;
	NFY_DEV * nfy_dev;

	printk("%s()\n", __func__);
    nfy_dev = kzalloc(sizeof(NFY_DEV), GFP_KERNEL);
    if(!nfy_dev) return -ENOMEM;
    pci_set_drvdata(pdev, nfy_dev);
    nfy_dev->pDev=pdev;
    nfy_dev->id=*id;
    nfy_dev->pClass=dev_class;
    mutex_lock(&gti_mutex);
    nfy_dev->instance=gInstance++;
    mutex_unlock(&gti_mutex);

    printk("%s()\n", __func__);
    printk("id: (%x, %x, %x, %x)\n", id->vendor, id->device, id->subvendor, id->subdevice);
    printk("id: (%x, %x, %x, %x)\n", pdev->vendor, pdev->device, pdev->subsystem_vendor, pdev->subsystem_device);

    rc = pci_enable_device(pdev);
    pci_set_master(pdev);
    //if(rc==0) rc = pci_enable_msi(pdev);
    if(rc==0) rc = pcie_capability_set_word(pdev, PCI_EXP_DEVCTL, PCI_EXP_DEVCTL_RELAX_EN);
    if(rc==0) rc = pci_request_regions(pdev, DRV_NAME);

    nfy_dev->initFn=nfy2020_init;
	nfy_dev->exitFn=nfy2020_remove;
	rc=nfy_dev->initFn(nfy_dev);
    if(rc!=0){
        remove(pdev);
        printk(KERN_ERR "Could not initialize PCI device, rc:%d\n",rc);
        return -ENOMEM;
    }
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
  
