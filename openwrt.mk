#cross compile для mips
#путь где лежит buildroot openwrt
WRTP = /home/prog/openwrt/trunk-rb941-2nd
#эта переменная нужна компиляроту. указывает на диру с toolchain.
export STAGING_DIR = $(WRTP)/staging_dir/toolchain-mips_34kc_gcc-5.3.0_musl-1.1.14
#где лежат бинарники компилятора(gcc, ldd)
PATH := $(PATH):$(STAGING_DIR)/bin
#указываем архитекруту
export ARCH = mips
#указываем какой префикс юзать перед сажем gcc или ldd(то есть какое имя у файла бинарника компилятора или линкера)
export CROSS_COMPILE = mips-openwrt-linux-
#дальше все как обычно для модуля
#путь к исходнику ядра
KERNEL_DIR = $(WRTP)/build_dir/target-mips_34kc_musl-1.1.14/linux-ar71xx_mikrotik/linux-4.1.20
