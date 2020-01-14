#if !defined(PCIE_DUMMY_DRV_H_)
#define PCIE_DUMMY_DRV_H_

#include <linux/types.h>
#include <linux/pci.h>

typedef int (*gti_drv_init)(void *gti_dev);
typedef int (*gti_drv_remove)(void *gti_dev);

extern int gti2801_init(void *gti_dev);
extern int gti2801_remove(void *gti_dev);

typedef struct {
	struct pci_dev *pDev;	/* pci device struct from probe() */
	struct pci_device_id id;
	struct class * pClass;
	int instance;
	void * private;
	gti_drv_init initFn;
	gti_drv_remove exitFn;
}GTI_DEV;

#endif

