DRIVER_NAME = pcie_dummy_drv
obj-m := $(DRIVER_NAME).o

$(DRIVER_NAME)-objs := dummy_pcie_drv.o gti2801.o

all: kernel_module writer

kernel_module:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

writer: writer.c
	$(CC) -Wall -o writer writer.c -lrt

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm -f writer

check: all
	lsmod | grep $(DRIVER_NAME) || $(MAKE) install_driver
	sudo ./writer

install_driver:
	sudo insmod $(DRIVER_NAME).ko

uninstall_driver:
	sudo rmmod $(DRIVER_NAME)
