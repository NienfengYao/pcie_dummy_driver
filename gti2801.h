#ifndef __GTI2800_H__
#define __GTI2800_H__

#define GTI_CHANNEL_NUMBER (1)

struct gti_dev{
	struct fpga_dev * fpga;
	struct cdev chdev;
	struct device * device;
	int channel;
	int mode;
	int count;
	char * buffer;
};

struct fpga_dev {
	GTI_DEV *dev;
	int instance;		/* instance number */
	dev_t cdevno_base;
	struct gti_dev gti[GTI_CHANNEL_NUMBER];	
	/* PCIe BAR management */
	void *__iomem control_bar;
	void *__iomem dma_bar;

	struct mutex 	mlock;
	struct spinlock slock;

	int state;
};

#endif /* __GTI2800_H__ */
