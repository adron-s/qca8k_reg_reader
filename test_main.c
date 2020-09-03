#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/udp.h>
#include <net/route.h>
#include <linux/pkt_sched.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_tcpudp.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/etherdevice.h> /* eth_type_trans */
#include <linux/proc_fs.h>
#include <linux/time.h>
#include <linux/if_arp.h>
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/err.h>
#include <linux/iopoll.h>
#include <net/protocol.h>
#include <net/dsa.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergey Sergeev <sergey.sergeev@yapic.net>");
MODULE_DESCRIPTION("kernel test");

static int owl = 0;
module_param(owl, int, 0);
MODULE_PARM_DESC(owl, "An OwL");

static void *regs[] = {
	0x04, "PAD0_MODE",
	0x08, "PAD5_MODE",
	0x0c, "PAD6_MODE",
	0x10, "POWER_ON_STRIP",
	0xe0, "SGMII_CTRL",
	0xe4, "MAC_POWER_SEL",
	0x7c, "PORT0_STATUS",
	0x94, "PORT6_STATUS",
	0x970, "QM_PORT0_CTRL0",
	0x974, "QM_PORT0_CTRL1",
	0x978, "QM_PORT1_CTRL0",
	0x97c, "QM_PORT1_CTRL1",
	0x980, "QM_PORT2_CTRL0",
	0x984, "QM_PORT2_CTRL1",
	0x988, "QM_PORT3_CTRL0",
	0x98c, "QM_PORT3_CTRL1",
	0x990, "QM_PORT4_CTRL0",
	0x994, "QM_PORT4_CTRL1",
	0x998, "QM_PORT5_CTRL0",
	0x99c, "QM_PORT5_CTRL1",
	0x9a0, "QM_PORT6_CTRL0",
	0x9a4, "QM_PORT6_CTRL1"
};

static u32 (*qca8k_read)(struct qca8k_priv *priv, u32 reg) = NULL;

//*********************************************************
//выполняется при загрузке модуля
static int __init test_m_module_init(void){
	struct net_device *dev;
	struct dsa_switch	*ds;
	void *priv;
	u32 vvv; int a;
	char dev_name[10];
	snprintf(dev_name, sizeof(dev_name), "eth%d", owl);
	dev = dev_get_by_name(&init_net, dev_name);
	if(!dev){
		printk(KERN_ERR "Can't get dev %s!\n", dev_name);
		return -ENODEV;
	}
	printk(KERN_INFO "Found dev with name = %s and index = %d\n", 
		dev->name, dev->ifindex);
	printk(KERN_INFO "netdev_uses_dsa(%s) = %d\n", 
		dev->name, netdev_uses_dsa(dev));
	if(!netdev_uses_dsa(dev)){
		printk(KERN_ERR "DSA for device '%s' is not enabled!\n", dev->name);
		goto end;
	}
	ds = dev->dsa_ptr->ds;
	if(!ds){
		printk(KERN_ERR "dev->ds_ptr->ds == NULL!\n");
		goto end;
	}
	priv = ds->priv;
	if(!priv){
		printk(KERN_ERR "dev->ds_ptr->ds->priv == NULL!\n");
		goto end;
	}
	qca8k_read = (void*)kallsyms_lookup_name("qca8k_read");
	if(!qca8k_read){
		printk(KERN_ERR "qca8k_read == NULL!\n");
		goto end;
	}
	for(a = 0; a < ARRAY_SIZE(regs); a+= 2){
		u32 reg = regs[a];
		char *name = regs[a + 1];
		vvv = qca8k_read(priv, reg);
		printk(KERN_INFO "0x%04x: 0x%08x /* %s */\n", reg, vvv, name);
	}
end:
	dev_put(dev);
	return -ENOMEM;
	return 0;
}//--------------------------------------------------------

//*********************************************************
//выполняется при выгрузке модуля
static void __exit test_m_module_exit (void){
}//--------------------------------------------------------

module_init(test_m_module_init);
module_exit(test_m_module_exit);
