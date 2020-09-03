#cross compile для mips
#путь где лежит buildroot openwrt
#WRTP = /home/prog/openwrt/lede-all/new-lede-rb941/source
#эта переменная нужна компиляроту. указывает на диру с toolchain.
export STAGING_DIR = /home/prog/openwrt/switch/ros-gpl/fake-kernel/OpenWrt-SDK-ar71xx-for-linux-x86_64-gcc-4.8-linaro_uClibc-0.9.33.2/staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-0.9.33.2
#где лежат бинарники компилятора(gcc, ldd)
PATH := $(PATH):$(STAGING_DIR)/bin
#указываем архитекруту
export ARCH = mips
#указываем какой префикс юзать перед сажем gcc или ldd(то есть какое имя у файла бинарника компилятора или линкера)
export CROSS_COMPILE = mips-openwrt-linux-
#дальше все как обычно для модуля
#путь к исходнику ядра
#KERNEL_DIR = $(WRTP)/build_dir/target-mips_24kc_musl-1.1.16/linux-ar71xx_mikrotik/linux-4.4.46
KERNEL_DIR = /home/prog/openwrt/switch/ros-gpl/fake-kernel/linux-3.3.5
#KERNEL_DIR = /home/prog/openwrt/switch/ros-gpl/fake-kernel/work/linux-4.4.46

#тут должна быть только одна строка команды. иначе будет ошибка. если нужно
#более одной строки то используй объединение команд посредством ; и ( )
define ManualBuild/Install
	mkdir -p $(WRTP)/files/tests
	cat $(TARGETS) > $(WRTP)/files/tests/$(TARGETS)
endef
