# PCIe Dummy Driver
  * We have a existed PCIe device. I use it to exercise a basic PCIe dummy driver. The probe() will be called if the PID/VID matched.

# Test the system calls to copy data between kernel and user space
* Behavior:
  * AP read a binary file and write to device driver, 64KB/time.
  * AP read the total size of written data from driver.

* Please install kernel headers first:
```shell
sudo apt-get install linux-headers-generic
```

* Usage:
```shell
$ make check
```

* Generate binary file for test
```shell
$ dd if=/dev/zero of=test.bin bs=65K count=1

```

