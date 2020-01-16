#if !defined(PCIE_DUMMY_DRV_H_)
#define PCIE_DUMMY_DRV_H_

#include <linux/types.h>
#include <linux/pci.h>

typedef int (*nfy_drv_init)(void *nfy_dev);
typedef int (*nfy_drv_remove)(void *nfy_dev);

extern int nfy2020_init(void *nfy_dev);
extern int nfy2020_remove(void *nfy_dev);

typedef struct {
	struct pci_dev *pDev;	/* pci device struct from probe() */
	struct pci_device_id id;
	struct class * pClass;
	int instance;
	void * private;
	nfy_drv_init initFn;
	nfy_drv_remove exitFn;
}NFY_DEV;

#endif

