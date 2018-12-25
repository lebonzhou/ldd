
#include <linux/module.h>
#include <linux/init.h>

#include <linux/err.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>

#include <linux/i2c.h>

static const unsigned short normal_i2c[] = { 0x48, 0x49, 0x4a, 0x4c,
					0x4d, 0x4e, I2C_CLIENT_END };


static int foo_probe(struct i2c_client *client,
			 const struct i2c_device_id *id);
static int foo_remove(struct i2c_client *client);


static struct i2c_device_id foo_idtable[] = {
	{ "foo", 0 },
	{ "bar", 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, foo_idtable);


static ssize_t show_temp(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	//struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	//struct lm73_data *data = dev_get_drvdata(dev);
	int temp;


	temp = (100 * 250) / 32;
	return scnprintf(buf, PAGE_SIZE, "%d\n", temp);
}

/* sysfs attributes for hwmon */

static SENSOR_DEVICE_ATTR(temp1_max, S_IWUSR | S_IRUGO,
			show_temp, NULL, 2);
static SENSOR_DEVICE_ATTR(temp1_min, S_IWUSR | S_IRUGO,
			show_temp, NULL, 3);


static struct attribute *foo_attrs[] = {

	&sensor_dev_attr_temp1_max.dev_attr.attr,
	&sensor_dev_attr_temp1_min.dev_attr.attr,

	NULL
};
ATTRIBUTE_GROUPS(foo);




/* Return 0 if detection is successful, -ENODEV otherwise */
static int foo_detect(struct i2c_client *new_client,
			struct i2c_board_info *info)
{
	//struct i2c_adapter *adapter = new_client->adapter;
	
	pr_info("foo_detect: client name:%s\n", new_client->name);
#if 0
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA |
					I2C_FUNC_SMBUS_WORD_DATA))
		return -ENODEV;

	/*
	 * Do as much detection as possible with byte reads first, as word
	 * reads can confuse other devices.
	 */
	ctrl = i2c_smbus_read_byte_data(new_client, LM73_REG_CTRL);
	if (ctrl < 0 || (ctrl & 0x10))
		return -ENODEV;

	conf = i2c_smbus_read_byte_data(new_client, LM73_REG_CONF);
	if (conf < 0 || (conf & 0x0c))
		return -ENODEV;

	id = i2c_smbus_read_byte_data(new_client, LM73_REG_ID);
	if (id < 0 || id != (LM73_ID & 0xff))
		return -ENODEV;

	/* Check device ID */
	id = i2c_smbus_read_word_data(new_client, LM73_REG_ID);
	if (id < 0 || id != LM73_ID)
		return -ENODEV;

	strlcpy(info->type, "lm73", I2C_NAME_SIZE);
#endif
	return 0;
}

static int
foo_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct device *hwmon_dev;

	pr_info("probe client name'%s', addr:0x%x\n", client->name, client->addr);

	hwmon_dev = devm_hwmon_device_register_with_groups(dev, client->name,
							   NULL, foo_groups);
    if(hwmon_dev != NULL)
		pr_info("foo register hwmon sucessed!!!");
	return 0;
}

static int
foo_remove(struct i2c_client *client)
{
	struct device *dev = &client->dev;

	hwmon_device_unregister(dev);

	pr_info("remove client name'%s'\n", client->name);

	return 0;
}




static struct i2c_driver foo_driver = {
	.driver = {
		.name	= "foo",
		//.pm	= &foo_pm_ops,	/* optional */
	},

	.id_table	= foo_idtable,
	.probe		= foo_probe,
	.remove		= foo_remove,
	/* if device autodetection is needed: */
	//.class		= I2C_CLASS_SOMETHING,
	.detect		= foo_detect,
	.address_list	= normal_i2c,

	//.shutdown	= foo_shutdown,	/* optional */
	//.command	= foo_command,	/* optional, deprecated */
};

int foo_read_value(struct i2c_client *client, u8 reg)
{
	if (reg < 0x10)	/* byte-sized register */
		return i2c_smbus_read_byte_data(client, reg);
	else		/* word-sized register */
		return i2c_smbus_read_word_data(client, reg);
}

int foo_write_value(struct i2c_client *client, u8 reg, u16 value)
{
	if (reg == 0x10)	/* Impossible to write - driver error! */
		return -EINVAL;
	else if (reg < 0x10)	/* byte-sized register */
		return i2c_smbus_write_byte_data(client, reg, value);
	else			/* word-sized register */
		return i2c_smbus_write_word_data(client, reg, value);
}

static struct i2c_board_info const h4_i2c_board_info[]  = {

	{	/* EEPROM on mainboard */
		I2C_BOARD_INFO("foo", 0x52),
		.platform_data	= NULL,
	},
	{	/* EEPROM on cpu card */
		I2C_BOARD_INFO("bar", 0x57),
		.platform_data	= NULL,
	},
};


static int foo_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
	
	return 0;
}

static u32 foo_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static const struct i2c_algorithm foo_algorithm = {
	.master_xfer = foo_xfer,
	.functionality = foo_func,
};


static struct i2c_adapter foo_adapter = {
	.owner = THIS_MODULE,
	.name = "foo-i2c",
	.class = I2C_CLASS_DEPRECATED,
	.algo = &foo_algorithm,
};

static void  register_i2c_devices(void)
{

	//i2c_register_board_info(1, h4_i2c_board_info,
		//	ARRAY_SIZE(h4_i2c_board_info));
		i2c_add_adapter(&foo_adapter);
		i2c_new_device(&foo_adapter,&h4_i2c_board_info[0]);

}


static int __init foo_init(void)
{
	pr_info("foo_init...");
	register_i2c_devices();
	return i2c_add_driver(&foo_driver);
}
module_init(foo_init);

static void __exit foo_cleanup(void)
{
	i2c_del_driver(&foo_driver);
	pr_info("foo_exit...");
}
module_exit(foo_cleanup);

MODULE_LICENSE("GPL");


