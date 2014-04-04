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

#define CM_PER_ADDR          0x44E00000
#define CM_WKUP_OFFSET       0x00000400

#define CM_WKUP_ADC_TSC_CLKCTRL 0xBC

#define CM_PER_EPWMSS0_CLKCTRL  0xD4
#define CM_PER_EPWMSS1_CLKCTRL  0xCC
#define CM_PER_EPWMSS2_CLKCTRL  0xD8

static int mux;
static int adcsw;
static int epwmsw;

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

static ssize_t adcsw_show (struct kobject *kobj,
                           struct kobj_attribute *attr,
                           char *buf) {
  return sprintf(buf, "%d\n", adcsw);
}

static ssize_t adcsw_store (struct kobject *kobj,
                            struct kobj_attribute *attr,
                            const char *buf, size_t count) {
  void *control_module;
  int value, n;

  n = sscanf(buf, "%d", &value);
  adcsw = n == 1 ? 0 : 1;

  if (mux) {
    printk(KERN_INFO "NXPMX: invalid input\n");
    return count;
  }

  if (value != 0 && value != 1) {
    printk(KERN_INFO "NXPMX: invalid value\n");
    return count;
  }

  if (!(control_module = ioremap(CM_PER_ADDR,
                                 0x4000))) {
    printk(KERN_INFO "NXPMX: unable to map clock module\n");
    return count;
  }

  n = ioread32(control_module + CM_WKUP_OFFSET + CM_WKUP_ADC_TSC_CLKCTRL);
  if (value == 1)
    n |= 0x2;
  else
    n &= ~0x2;

  iowrite32(n, control_module + CM_WKUP_OFFSET + CM_WKUP_ADC_TSC_CLKCTRL);
  iounmap(control_module);
  
  return count;
}

static ssize_t epwmsw_show (struct kobject *kobj,
                            struct kobj_attribute *attr,
                            char *buf) {
  return sprintf(buf, "%d\n", epwmsw);
}

static ssize_t epwmsw_store (struct kobject *kobj,
                             struct kobj_attribute *attr,
                             const char *buf, size_t count) {
  void *control_module;
  int value, n;

  n = sscanf(buf, "%d", &value);
  adcsw = n == 1 ? 0 : 1;

  if (mux) {
    printk(KERN_INFO "NXPMX: invalid input\n");
    return count;
  }

  if (value != 0 && value != 1) {
    printk(KERN_INFO "NXPMX: invalid value\n");
    return count;
  }

  if (!(control_module = ioremap(CM_PER_ADDR,
                                 0x4000))) {
    printk(KERN_INFO "NXPMX: unable to map clock module\n");
    return count;
  }

  n = ioread32(control_module + CM_PER_EPWMSS0_CLKCTRL);
  if (value == 1)
    n |= 0x2;
  else
    n &= ~0x2;

  iowrite32(n, control_module + CM_PER_EPWMSS0_CLKCTRL);
  
  n = ioread32(control_module + CM_PER_EPWMSS1_CLKCTRL);
  if (value == 1)
    n |= 0x2;
  else
    n &= ~0x2;

  iowrite32(n, control_module + CM_PER_EPWMSS1_CLKCTRL);
  
  n = ioread32(control_module + CM_PER_EPWMSS2_CLKCTRL);
  if (value == 1)
    n |= 0x2;
  else
    n &= ~0x2;

  iowrite32(n, control_module + CM_PER_EPWMSS2_CLKCTRL);
  
  iounmap(control_module);
  
  return count;
}

static struct kobj_attribute mux_attribute =
  __ATTR(mux, 0600, mux_show, mux_store);
static struct kobj_attribute adcsw_attribute =
  __ATTR(adcsw, 0600, adcsw_show, adcsw_store);
static struct kobj_attribute epwmsw_attribute =
  __ATTR(epwmsw, 0600, epwmsw_show, epwmsw_store);

static struct attribute *attrs[] = {
  &mux_attribute.attr,
  &adcsw_attribute.attr,
  &epwmsw_attribute.attr,
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
MODULE_AUTHOR("Sungjin");
MODULE_DESCRIPTION("BBB PinMux Helper Module");
