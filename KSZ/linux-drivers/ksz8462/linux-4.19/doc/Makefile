obj-m := ksz8462-ins.o

KERNELDIR ?=

all default: modules
install: modules_install

modules modules_install help clean:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) $@
