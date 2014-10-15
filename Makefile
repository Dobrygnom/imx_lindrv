obj-m:= zda_n_zpv_module.o
PWD = $(shell pwd)
KDIR := /home/lucid/ltib/rpm/BUILD/linux-2.6.35.3

default: zda_n_zpv_module.ko
zda_n_zpv_module.ko: module_entry.c
	make M=$(PWD) -C $(KDIR) ARCH=arm CROSS_COMPILE=arm-fsl-linux-gnueabi-
clean:
	rm *.o *.ko
