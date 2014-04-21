/*
 * NXCTRL BeagleBone Black Control Library - Kernel Module for Mux
 *
 * Copyright (C) 2014 Sungjin Chun <chunsj@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <asm/io.h>

#define CONTROL_MODULE_ADDR  0x44E10000
#define CONTROL_MODULE_LEN   0x2000

static int mux;

static ssize_t mux_show (struct kobject *kobj,
                           struct kobj_attribute *attr,
                           char *buf) {
  return sprintf(buf, "%d\n", mux);
}

static ssize_t mux_store (struct kobject *kobj,
                            struct kobj_attribute *attr,
                            const char *buf, size_t count) {
  void *control_module;
  int offset, value, n;

  n = sscanf(buf, "%x:%x", &offset, &value);
  mux = n == 2 ? 0 : 1;

  if (mux) {
    printk(KERN_INFO "NXPMX: invalid input\n");
    return count;
  }

  if (offset > CONTROL_MODULE_LEN) {
    printk(KERN_INFO "NXPMX: invalid offset\n");
    return count;
  }

  if (value > 128) {
    printk(KERN_INFO "NXPMX: invalid value\n");
    return count;
  }

  if (!(control_module = ioremap(CONTROL_MODULE_ADDR,
                                 CONTROL_MODULE_LEN))) {
    printk(KERN_INFO "NXPMX: unable to map control module\n");
    return count;
  }

  iowrite32(value, control_module + offset);
  iounmap(control_module);
  
  return count;
}

static struct kobj_attribute mux_attribute =
  __ATTR(mux, 0600, mux_show, mux_store);

static struct attribute *attrs[] = {
  &mux_attribute.attr,
  NULL,
};

static struct attribute_group attr_group = {
  .attrs = attrs,
};

static struct kobject *nxpmx_kobj;

static int __init
nxpmx_init (void) {
  int n;

  nxpmx_kobj = kobject_create_and_add("nxpmx", kernel_kobj);
  if (!nxpmx_kobj)
    return -ENOMEM;

  n = sysfs_create_group(nxpmx_kobj, &attr_group);
  if (n)
    kobject_put(nxpmx_kobj);

  printk(KERN_INFO "NXPMX Pin Mux Module Loaded\n");

  return n;
}

static void __exit
nxpmx_exit (void) {
  kobject_put(nxpmx_kobj);
  printk(KERN_INFO "NXPMX Pin Mux Module Unloaded\n");
}

module_init(nxpmx_init);
module_exit(nxpmx_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sungjin Chun    <chunsj@gmail.com>");
MODULE_DESCRIPTION("BBB PinMux Helper Module");
