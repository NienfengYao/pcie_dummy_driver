DRIVER_NAME = pcie_dummy_drv
obj-m := $(DRIVER_NAME).o

$(DRIVER_NAME)-objs := dummy_pcie_drv.o gti2801.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

