#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/module.h>

/* data line is address bus (here, connect to A2) */
#define CMD_HIGH		0x04
#define BCM_GPIO_BASE	0x3F200000

/* see cat /proc/iomem */
/*
root@foo:~# grep -E '/soc/gpio|bcm2708_gpio' /proc/iomem
3f200000-3f2000b3 : /soc/gpio@7e200000
*/
#define KSZ8462HLI_PA_DATA	(BCM_GPIO_BASE + 0xb4)
#define KSZ8462HLI_PA_CMD	(KSZ8462HLI_PA_DATA + CMD_HIGH)
#define KSZ8462HLI_SZ		4

#define IRQ_KSZ8462HLI		0

static struct resource ksz8462hli_resource[] = {
	[0] = {
		.start = KSZ8462HLI_PA_DATA,
		.end   = KSZ8462HLI_PA_DATA + KSZ8462HLI_SZ - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = KSZ8462HLI_PA_CMD,
		.end   = KSZ8462HLI_PA_CMD + KSZ8462HLI_SZ - 1,
		.flags = IORESOURCE_MEM,
	},
	[2] = {
		.start = IRQ_KSZ8462HLI,
		.end   = IRQ_KSZ8462HLI,
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device *pdev;

static int __init ksz8462_add(void)
{	
    int inst_id = -1;
    int err;
    pdev = platform_device_alloc("ksz8462_hli", inst_id);
	if (!pdev) {
		err = -ENOMEM;
		pr_err("Device allocation failed\n");
		goto exit;
	}
    err = platform_device_add_resources(pdev, ksz8462hli_resource,
		ARRAY_SIZE(ksz8462hli_resource));
	if (err) {
		pr_err("Device resources addition failed (%d)\n", err);
		goto exit_device_put;
	}
    err = platform_device_add(pdev);
    if (err) {
		pr_err("Device addition failed (%d)\n", err);
		goto exit_device_put;
	}
    return 0;

exit_device_put:
	platform_device_put(pdev);
exit:
	return err;
}

static void __exit ksz8462_remove(void)
{
	platform_device_put(pdev);
}

module_init(ksz8462_add);
module_exit(ksz8462_remove);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pierre-Jean Texier <pjtexier@koncepto.io>");
