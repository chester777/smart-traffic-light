KDIR := ~/linux-rpi/
ARM := ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
PWD := $(shell pwd)
obj-m := ultra.o

defulat :
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules -lrt
