obj-m += i2c_drv.o
obj-m += i2c_dev.o

all:
	make -C ~/linux/kernel M=$(PWD) modules  

clean:
	rm *.o *.cmd .Makefile.swp .ktrace* *mod.c *.ko Module.symvers modules.order .tmp_versions .i2c* -rf 
