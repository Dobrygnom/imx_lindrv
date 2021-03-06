#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x3972220f, "module_layout" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x2dfa9905, "__mxc_ioremap" },
	{ 0xadf42bd5, "__request_region" },
	{ 0x859c6dc7, "request_threaded_irq" },
	{ 0x65d6d0f0, "gpio_direction_input" },
	{ 0x47229b5c, "gpio_request" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x788fe103, "iomem_resource" },
	{ 0xea147363, "printk" },
	{ 0x37a0cba, "kfree" },
	{ 0x9bce482f, "__release_region" },
	{ 0x45a55ec8, "__iounmap" },
	{ 0xfe990052, "gpio_free" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

