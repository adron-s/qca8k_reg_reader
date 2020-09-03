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

#define QCA8K_PORT_LOOKUP_CTRL(_i)			(0x660 + (_i) * 0xc)
#define   QCA8K_PORT_LOOKUP_MEMBER			GENMASK(6, 0)
#define   QCA8K_PORT_LOOKUP_STATE_MASK			GENMASK(18, 16)
#define   QCA8K_PORT_LOOKUP_STATE_DISABLED		(0 << 16)
#define   QCA8K_PORT_LOOKUP_STATE_BLOCKING		(1 << 16)
#define   QCA8K_PORT_LOOKUP_STATE_LISTENING		(2 << 16)
#define   QCA8K_PORT_LOOKUP_STATE_LEARNING		(3 << 16)
#define   QCA8K_PORT_LOOKUP_STATE_FORWARD		(4 << 16)
#define   QCA8K_PORT_LOOKUP_STATE			GENMASK(18, 16)
#define   QCA8K_PORT_LOOKUP_LEARN			BIT(20)


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
	0xe4, "MAC_POWER_SEL"
};

static u32 (*qca8k_read)(struct qca8k_priv *priv, u32 reg) = NULL;

//*********************************************************
static char *decode_port_state_bits(u32 val){
	static char *vals[ ] = {
		" DISABLED", //000
		" BLOCKING", //001
		"LISTENING", //010
		" LEARNING", //011
		"  FORWARD", //100
		" UNKNOWN!", //> 100
	};
	val >>= 16;
	val &= 0x7;
	if(val > 0x4){
		return vals[0x5];
	}
	return vals[val];
}//--------------------------------------------------------

//*********************************************************
static char *decode_pbvm(unsigned int val){
	static char ret[8];
	char *p = ret + sizeof(ret) - 1;
	val &= 0x7F;
	for(*(p--) = '\0'; p >= ret; val >>= 1)
		*(p--) = (val & 0x1) ? '1' : '0';
	return ret;
}//--------------------------------------------------------

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
	for(a = 0; a < 6; a++){
		u32 reg = QCA8K_PORT_LOOKUP_CTRL(a);
		vvv = qca8k_read(priv, reg);
		printk(KERN_INFO "0x%04x: 0x%08x /* CTRL(%d), STATE = %s, LEARN = %s, PBVM = %s */\n",
			reg, vvv, a, decode_port_state_bits(vvv),
			vvv & QCA8K_PORT_LOOKUP_LEARN ? "On" : "OFF",
			decode_pbvm(vvv)
		);
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
