#include <linux/module.h>
#include <linux/pci.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/pci-dma.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/printk.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

#include "dummy_pcie_drv.h"
#include "nfy2020.h"

#define virt_to_bus virt_to_phys
#define BUF_LEN 64*1024

static int total_num = 0;   /* The quantity that written in */

static ssize_t show_nfy_info(struct device *dev, struct device_attribute *attr, char *buf){
	sprintf(buf,"%s()\n", __func__);
	return strlen(buf);
}

static DEVICE_ATTR(nfy_info, S_IRUGO, show_nfy_info, NULL);

static int nfy_open(struct inode *inode, struct file *file)
{
	printk("%s()\n", __func__);
	total_num = 0;
	return 0;
}

static int nfy_close(struct inode *inode, struct file *file)
{
    printk("%s()\n", __func__);
	return 0;
}

static ssize_t nfy_read(struct file *file, char __user *buf, size_t count,loff_t *pos)
{
    printk("%s()\n", __func__);
	return total_num;
}

static ssize_t nfy_write(struct file *file, const char __user *buf, size_t count, loff_t *pos)
{
    printk("%s() count=%lu\n", __func__, count);
    if (BUF_LEN >= count){
        total_num += count;
        return count;
    }else {
        printk ("Support maximum length = %d\n", BUF_LEN);
        return -EINVAL;
    }
}

const static struct file_operations nfy_fops = {
	.owner = THIS_MODULE,
	.open = nfy_open,
	.release = nfy_close,
	.read = nfy_read,
	.write = nfy_write,
};

int nfy2020_remove(void *nfy_dev){
	int i;
	NFY_DEV *pdev=(NFY_DEV *)nfy_dev;
	struct fpga_dev * fpga=(struct fpga_dev *)pdev->private;

	pci_iounmap(pdev->pDev,fpga->dma_bar);
	pci_iounmap(pdev->pDev,fpga->control_bar);

	device_remove_file(&pdev->pDev->dev, &dev_attr_nfy_info);
	for(i=0;i<NFY_CHANNEL_NUMBER;i++){
		cdev_del(&fpga->nfy[i].chdev);
		device_destroy(pdev->pClass,fpga->nfy[i].device->devt);
	}
	unregister_chrdev_region(fpga->cdevno_base, NFY_CHANNEL_NUMBER);
	kfree(fpga);

	return 0;
}

int nfy2020_init(void *nfy_dev){
	int rc = 0;
	NFY_DEV *pdev=(NFY_DEV *)nfy_dev;
	struct fpga_dev * fpga;
    int i;

	fpga = kzalloc(sizeof(struct fpga_dev), GFP_KERNEL); if(!fpga) return -ENOMEM;
    pdev->private=fpga;
    rc = pci_enable_msi(pdev->pDev);

    if(rc==0) rc = device_create_file(&pdev->pDev->dev, &dev_attr_nfy_info);
    if(rc!=0){
        kfree(fpga);
		printk(KERN_ERR "nfy2020 init error, rc:%d\n",rc);
        return -ENOMEM;
    }

	fpga->dev=pdev;
	fpga->state=0;
    fpga->control_bar=pci_iomap(pdev->pDev, 0, pci_resource_len(pdev->pDev, 0));
	printk(KERN_NOTICE "control_bar:\t%px:%llx\n",fpga->control_bar,virt_to_bus(fpga->control_bar));
    fpga->dma_bar=pci_iomap(pdev->pDev, 2, pci_resource_len(pdev->pDev, 2));
	printk(KERN_NOTICE "dma_bar:\t%px:%llx\n",fpga->dma_bar,virt_to_bus(fpga->dma_bar));
	mutex_init(&fpga->mlock);
	spin_lock_init(&fpga->slock);
	rc = alloc_chrdev_region(&fpga->cdevno_base, 0, NFY_CHANNEL_NUMBER, "NFY2020_drv");

	if(rc==0){
		for(i=0;i<NFY_CHANNEL_NUMBER;i++){
			cdev_init(&fpga->nfy[i].chdev, &nfy_fops);
			fpga->nfy[i].chdev.owner=THIS_MODULE;
			rc=  cdev_add(&fpga->nfy[i].chdev, MKDEV(MAJOR(fpga->cdevno_base),i), 1); if(rc!=0) break;
			fpga->nfy[i].device=device_create(pdev->pClass,&pdev->pDev->dev,MKDEV(MAJOR(fpga->cdevno_base),i),0,"nfy2020-%d",i+NFY_CHANNEL_NUMBER*pdev->instance);
			if(fpga->nfy[i].device==0){rc=-ENOMEM; break;}
			fpga->nfy[i].fpga=fpga;
		}
	}
	return rc;
}
