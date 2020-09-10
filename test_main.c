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
#include <linux/of_mdio.h>
#include <linux/mfd/syscon.h>
#include <net/protocol.h>
#include <net/dsa.h>
#include <../drivers/net/dsa/qca8k.h>
#include <linux/synclink.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergey Sergeev <sergey.sergeev@yapic.net>");
MODULE_DESCRIPTION("kernel test");

static int owl = 0;
module_param(owl, int, 0);
MODULE_PARM_DESC(owl, "An OwL");

static int yyy = 0;
module_param(yyy, int, 0);
MODULE_PARM_DESC(yyy, "An YYY");

static void *regs[] = {
	0x04, "PAD0_MODE",
	0x08, "PAD5_MODE",
	0x0c, "PAD6_MODE",
	0x10, "POWER_ON_STRIP",
	0xe0, "SGMII_CTRL",
	0x3c, "MDIO_MASTER_CTRL",
	0xe4, "MAC_POWER_SEL",
	0x7c, "PORT0_STATUS",
	0x94, "PORT6_STATUS"
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
static void do_port_xxx(struct device_node *port){
	int reg = 0, err = 0, len = 0;
	struct device_node *phy_node;
	err = of_property_read_u32(port, "reg", &reg);
	printk(KERN_INFO "OWL: %s, %s, reg = 0x%x, err = 0x%x\n", port->name, port->full_name, reg, err);
	//для phy блоков нужен спец парсер!
	phy_node = of_parse_phandle(port, "phy-handle", 0);
	if(!phy_node)
		return;
	//вот мы и добрались до блока phy@0
	printk(KERN_INFO "OWL: phy-handle = %p, %s\n", phy_node, phy_node->full_name);
	struct phy_device *phy = of_phy_find_device(phy_node);
	if(!phy){
		printk(KERN_ERR "can't find  phy device for phy node %s\n", phy_node->full_name);
	}else{
		printk(KERN_INFO "phy device FOUND OK !!!\n");
	}
	of_node_put(phy_node);
}//--------------------------------------------------------

//*********************************************************
static void do_phy_yyy(struct device_node *sw_node){
	int reg = 0, err = 0, len = 0;
	struct device_node *phy_node;
	printk(KERN_INFO "OWL: %s, %s\n", sw_node->name, sw_node->full_name);
	//для phy блоков нужен спец парсер!
	phy_node = of_parse_phandle(sw_node, "phy-owl-handle", 0);
	if(!phy_node)
		return;
	//пот мы и добрались до блока phy@0
	printk(KERN_INFO "OWL: phy-handle = %p, %s\n", phy_node, phy_node->full_name);
	struct phy_device *phy = of_phy_find_device(phy_node);
	if(!phy){
		printk(KERN_ERR "can't find  phy device for phy node %s\n", phy_node->full_name);
	}else{
		printk(KERN_INFO "phy device FOUND OK !!!\n");
	}
	of_node_put(phy_node);
}//--------------------------------------------------------

static int (*ipq8064_mdio_read)(struct mii_bus *bus, int phy_addr, int reg_offset) = NULL;

//*********************************************************
static void do_mdio_xxx(struct qca8k_priv *priv){
	struct mii_bus *bus = priv->bus;
	int res1, res2;
	int phy_addr = 0x00;
	int a;
	/* 
	//вместо этого можно просто вызвать mdiobus_read !?!?!?!
	ipq8064_mdio_read = (void*)kallsyms_lookup_name("ipq8064_mdio_read");
	if(!ipq8064_mdio_read){
		printk(KERN_ERR "Can' lookup ipq8064_mdio_read!\n");
		return;
	}*/
	for(a = 0; a < 5; a++){
		int bmcr;
		phy_addr = a;
		//res1 = ipq8064_mdio_read(bus, phy_addr, 0x02);
		//res2 = ipq8064_mdio_read(bus, phy_addr, 0x03);
		res1 = bus->read(bus, phy_addr, 0x02);
		res2 = bus->read(bus, phy_addr, 0x03);
		printk(KERN_INFO "mdio_read(0x%x) = 0x%x, 0x%x\n", phy_addr, res1, res2);
		bmcr = bus->read(bus, phy_addr, MII_BMCR);
		bmcr |= BMCR_PDOWN;
		bus->write(bus, phy_addr, MII_BMCR, bmcr);
	}
	/*struct phy_device *phydev = get_phy_device(bus, 0x0, 0);
	if(IS_ERR(phydev)){
		printk("Can't create PHY device. err = 0x%x\n", phydev);
	}else{
		printk(KERN_INFO "phy device create OK! phy_id = 0x%x\n", phydev->phy_id);
		phy_device_free(phydev);
	}*/
}//--------------------------------------------------------

//*********************************************************
void do_reset_gpio(struct qca8k_priv *priv){
	struct gpio_desc *reset_gpio = priv->reset_gpio;
	//printk(KERN_INFO "reset_gpio = 0x%p: %x, %x, %x, %x\n", reset_gpio,
		//reset_gpio->state, reset_gpio->smask, reset_gpio->dir, reset_gpio->dmask);
	printk(KERN_INFO "is_active_low = %d, direction = %s\n",
		gpiod_is_active_low(reset_gpio), gpiod_get_direction(reset_gpio) == 0 ? "out" : "in");
	if(yyy == 10)
		gpiod_toggle_active_low(reset_gpio);
	if(yyy >= 100 && yyy <= 101){
		int val = yyy == 101 ? 1 : 0;
		printk(KERN_INFO "setting gpio valut to %d\n", val);
		gpiod_set_value_cansleep(priv->reset_gpio, val);
	}
	if(yyy == 222){
		printk(KERN_INFO "setting gpio valut to %d\n", 1);
		gpiod_set_value_cansleep(priv->reset_gpio, 1);
		/* The active low duration must be greater than 10 ms
		 * and checkpatch.pl wants 20 ms.
		 */
		msleep(20);
		gpiod_set_value_cansleep(priv->reset_gpio, 0);
		printk(KERN_INFO "setting gpio valut to %d\n", 0);
	}
}//--------------------------------------------------------

//*********************************************************
//выполняется при загрузке модуля
static int __init test_m_module_init(void){
	struct net_device *dev;
	struct dsa_switch	*ds;
	struct qca8k_priv *priv;
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
	//do_mdio_xxx(priv);
	//do_reset_gpio(priv);

	if(1){
		qca8k_read = (void*)kallsyms_lookup_name("qca8k_read");
		if(!qca8k_read){
			printk(KERN_ERR "qca8k_read == NULL!\n");
			goto end;
		}
		//mutex_lock(&priv->reg_mutex);
		for(a = 0; a < ARRAY_SIZE(regs); a+= 2){
			u32 reg = regs[a];
			char *name = regs[a + 1];
			vvv = qca8k_read(priv, reg);
			printk(KERN_INFO "0x%04x: 0x%08x /* %s */\n", reg, vvv, name);
		}
		//this causes glitches in qca8k! links fall for no reason!
		for(a = 0; a < 6; a++){ // a < 6
			u32 reg = QCA8K_PORT_LOOKUP_CTRL(a);
			//u32 reg = QCA8K_REG_PORT_STATUS(a);
			vvv = qca8k_read(priv, reg);
			//printk(KERN_INFO "QCA8K_REG_PORT_STATUS(%d) = 0x%04x\n", a, vvv);
			if(1){
				printk(KERN_INFO "0x%04x: 0x%08x /* CTRL(%d), STATE = %s, LEARN = %s, PBVM = %s */\n",
					reg, vvv, a, decode_port_state_bits(vvv),
					vvv & QCA8K_PORT_LOOKUP_LEARN ? "On" : "OFF",
					decode_pbvm(vvv)
				);
			}
		}
		//mutex_unlock(&priv->reg_mutex);
	}
	if(0){ //for phy of node digging
		if(ds->dev && ds->dev->of_node){
			struct device_node *node;
			int len = 0;
			//node = of_get_property(ds->dev->of_node, "qca,strict-cpu-mode", &len);
			//node = of_get_property(ds->dev->of_node, "mdio", &len);
			//printk(KERN_INFO "Of node = %p, len = 0x%x\n", node, len);
			/* if(of_property_read_bool(ds->dev->of_node, "qca,qca8337,strict-cpu-mode"))
				printk(KERN_INFO "qca,qca8337,strict-cpu-mode is OK\n"); */
			struct device_node *ports, *port;
			ports = of_get_child_by_name(priv->dev->of_node, "ports");
			if(!ports){
				printk(KERN_ERR "no DTS ports node found!\n");
				goto end;
			}
			printk("%s\n", ports->full_name);
			//do_phy_yyy(ds->dev->of_node);
			if(1){
				for_each_available_child_of_node(ports, port){
					//int err, reg;
					//reg = 0;
					//err = of_property_read_u32(port, "reg", &reg);
					//printk(KERN_INFO "OWL: %s, %s, reg = 0x%x, err = 0x%x\n", port->name, port->full_name, reg, err);
					if(port->full_name && strcmp(port->full_name, "port@1") == 0){
						do_port_xxx(port);
						//printk(KERN_INFO "owl-X211 done\n");
					}
					of_node_put(port);
				}
				of_node_put(ports);
			}
		}
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
