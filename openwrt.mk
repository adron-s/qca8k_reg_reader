#cross compile для mips
#путь где лежит buildroot openwrt
WRTP = /home/prog/openwrt/2021-openwrt/openwrt-2021
#эта переменная нужна компиляроту. указывает на диру с toolchain.
export STAGING_DIR = $(WRTP)/staging_dir/toolchain-mips_24kc_gcc-8.4.0_musl

#где лежат бинарники компилятора(gcc, ldd)
PATH := $(PATH):$(STAGING_DIR)/bin
#указываем архитекруту
export ARCH = mips
#указываем какой префикс юзать перед сажем gcc или ldd(то есть какое имя у файла бинарника компилятора или линкера)
export CROSS_COMPILE = mips-openwrt-linux-
#дальше все как обычно для модуля
#путь к исходнику ядра
KERNEL_DIR = $(WRTP)/build_dir/target-mips_24kc_musl/linux-ath79_mikrotik/linux-5.4.96
