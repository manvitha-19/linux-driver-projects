#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xb7ca47dd, "cdev_init" },
	{ 0x362de9f7, "cdev_add" },
	{ 0x72ee3816, "class_create" },
	{ 0x60954194, "device_create" },
	{ 0x4c03a563, "random_kmalloc_seed" },
	{ 0xbe742e99, "kmalloc_caches" },
	{ 0x87b82067, "__kmalloc_cache_noprof" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x30a96774, "class_destroy" },
	{ 0x037a0cba, "kfree" },
	{ 0x6d47ec52, "device_destroy" },
	{ 0x392cc665, "cdev_del" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xd6d9f15b, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "106674774284C4D2290BBC3");
