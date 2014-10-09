obj-m:= example_module.o
PWD = $(shell pwd)
KDIR := /home/lucid/ltib/rpm/BUILD/linux-2.6.35.3
default: example_module.ko
example_module.ko: example_module.c
	make M=$(PWD) -C $(KDIR) ARCH=arm CROSS_COMPILE=arm-fsl-linux-gnueabi-
clean:
	rm *.o *.ko
