obj-m += i2c_client.o
all:
	make -C ~/linux/kernel M=$(PWD) modules  

clean:
	rm *.o *.cmd .Makefile.swp .ktrace* *mod.c *.ko Module.symvers modules.order .tmp_versions -rf 
