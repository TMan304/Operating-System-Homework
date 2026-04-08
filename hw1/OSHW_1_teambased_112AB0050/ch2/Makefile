obj-m += jiffies.o seconds.o

KNERAL_VERSION := $(shell uname -r)
KNERAL_DIR := /lib/modules/$(KNERAL_VERSION)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KNERAL_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KNERAL_DIR) M=$(PWD) clean