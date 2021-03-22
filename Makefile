CONFIG_MODULE_SIG=n 
obj-m := remapping-driver.o
KDIR := /usr/src/linux-headers-$(shell uname -r)
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
unload:
	sudo rmmod ./remapping-driver.ko
load:
	sudo insmod  ./remapping-driver.ko

reload: unload load

clean:
	make -C $(KDIR) M=$(PWD) clean