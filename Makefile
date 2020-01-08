DRIVER_NAME = pcie_dummy_drv
obj-m := $(DRIVER_NAME).o

all: kernel_module

kernel_module: $(DRIVER_NAME).c
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

