#ifndef __NFY2020_H__
#define __NFY2020_H__

#define NFY_CHANNEL_NUMBER (1)

struct nfy_dev{
	struct fpga_dev * fpga;
	struct cdev chdev;
	struct device * device;
	int channel;
	int mode;
	int count;
	char * buffer;
};

struct fpga_dev {
	NFY_DEV *dev;
	int instance;		/* instance number */
	dev_t cdevno_base;
	struct nfy_dev nfy[NFY_CHANNEL_NUMBER];	
	/* PCIe BAR management */
	void *__iomem control_bar;
	void *__iomem dma_bar;

	struct mutex 	mlock;
	struct spinlock slock;

	int state;
};

#endif /* __NFY2020_H__ */
