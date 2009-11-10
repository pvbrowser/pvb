#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

#undef unix
struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = __stringify(KBUILD_MODNAME),
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0xd2c34a4b, "struct_module" },
	{ 0x7da8156e, "__kmalloc" },
	{ 0x89b301d4, "param_get_int" },
	{ 0x51305729, "pcmcia_register_driver" },
	{ 0x3de88ff0, "malloc_sizes" },
	{ 0x806d5133, "param_array_get" },
	{ 0x6483655c, "param_get_short" },
	{ 0xb1f02ce, "pcmcia_parse_tuple" },
	{ 0x98bd6f46, "param_set_int" },
	{ 0x26ad39f1, "pcmcia_release_configuration" },
	{ 0x89cef6fb, "param_array_set" },
	{ 0x37e74642, "get_jiffies_64" },
	{ 0x299d5c5c, "pcmcia_request_configuration" },
	{ 0xda02d67, "jiffies" },
	{ 0xaa136450, "param_get_charp" },
	{ 0x1b7d4074, "printk" },
	{ 0xa9dc2c3d, "pci_find_device" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0x1c90171d, "pcmcia_request_window" },
	{ 0xdc718692, "pci_bus_write_config_dword" },
	{ 0x9eac042a, "__ioremap" },
	{ 0x8aba3360, "pcmcia_register_client" },
	{ 0x6a6f0027, "kmem_cache_alloc" },
	{ 0xa4aced56, "pci_bus_read_config_word" },
	{ 0x879f82f, "pci_bus_read_config_dword" },
	{ 0x26e96637, "request_irq" },
	{ 0x4292364c, "schedule" },
	{ 0x6022172a, "register_chrdev" },
	{ 0xf96c6617, "pcmcia_release_window" },
	{ 0x2cd7da6c, "param_set_charp" },
	{ 0xbce89f77, "__wake_up" },
	{ 0x37a0cba, "kfree" },
	{ 0x2e60bace, "memcpy" },
	{ 0xaed752d8, "pcmcia_get_tuple_data" },
	{ 0x407a28d8, "interruptible_sleep_on_timeout" },
	{ 0xd259d0ee, "pcmcia_map_mem_page" },
	{ 0xedc03953, "iounmap" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0xb180e175, "pcmcia_unregister_driver" },
	{ 0x40046949, "param_set_short" },
	{ 0x25c36def, "pcmcia_get_first_tuple" },
	{ 0x551305f7, "pcmcia_deregister_client" },
	{ 0xd6c963c, "copy_from_user" },
	{ 0x89b4793f, "cs_error" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=ds,pcmcia_core";

