#cross compile для arm
#root
OPENWRT = /home/prog/openwrt/2020-openwrt/openwrt-ipq806x
#где лежат бинарники компилятора(gcc, ldd)
#PATH := $(PATH):$(STAGING_DIR)/bin
#указываем архитекруту
export ARCH = arm
export STAGING_DIR = ${OPENWRT}/staging_dir/toolchain-arm_cortex-a15+neon-vfpv4_gcc-8.4.0_musl_eabi
export CROSS_COMPILE = $(STAGING_DIR)/bin/arm-openwrt-linux-

#дальше все как обычно для модуля
#путь к исходнику ядра
KERNEL_DIR = ${OPENWRT}/build_dir/target-arm_cortex-a15+neon-vfpv4_musl_eabi/linux-ipq806x_generic/linux-5.4.61
