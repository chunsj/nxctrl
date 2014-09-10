/*
 * lirc_nxctrl.c
 *
 * lirc_nxctrl - Device driver that records pulse- and pause-lengths
 *	      (space-lengths) (just like the lirc_serial driver does)
 *	      between GPIO interrupt events on the Raspberry Pi.
 *	      Lots of code has been taken from the lirc_serial module,
 *	      so I would like say thanks to the authors.
 *            Changed code for my own NXCTRL on BeagleBone Black.
 *
 * Copyright (C) 2014 Aron Robert Szabo <aron@reon.hu>,
 *		      Michael Bishop <cleverca22@gmail.com>
 *		      Bengt Martensson <barf@bengt-martensson.de>
 *                    Sungjin Chun <chunsj@gmail.com>
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/spinlock.h>
#include <media/lirc.h>
#include <media/lirc_dev.h>
#include <linux/gpio.h>

#define LIRC_DRIVER_NAME "lirc_nxctrl"
/*#define RBUF_LEN 256*/
#define RBUF_LEN 1024
#define LIRC_TRANSMITTER_LATENCY 256

#ifndef MAX_UDELAY_MS
#define MAX_UDELAY_US 5000
#else
#define MAX_UDELAY_US (MAX_UDELAY_MS*1000)
#endif

#define LIRC_NXCTRL_MAX_TRANSMITTERS 8
#define INVALID -1

#define dprintk(fmt, args...)                   \
  do {                                          \
    if (debug)					\
      printk(KERN_DEBUG LIRC_DRIVER_NAME ": "	\
             fmt, ## args);			\
  } while (0)

/* module parameters */

/* set the default GPIO input pin */
static int gpio_in_pin = 60; // P9:12
/* enable debugging messages */
static int debug;
/* INVALID = auto, 0 = active high, 1 = active low */
static int sense = INVALID;
/* use softcarrier by default */
static int softcarrier = 1;
/* 0 = do not invert output, 1 = invert output */
static int invert = 0;
/* Transmit mask */
unsigned int tx_mask = 1; /* First transmitter (#1) selected */

/* forward declarations */
static void lirc_nxctrl_exit(void);

int valid_gpio_pins[] = { 48, 60 };

static struct platform_device *lirc_nxctrl_dev;
static struct timeval lasttv = { 0, 0 };
static struct lirc_buffer rbuf;
static spinlock_t lock;

/* initialized/set in init_timing_params() */
static unsigned int freq = 38000;
static unsigned int duty_cycle = 50;
static unsigned long period;
static unsigned long pulse_width;
static unsigned long space_width;

static int init_timing_params(unsigned int new_duty_cycle,
                              unsigned int new_freq)
{
  /*
   * period, pulse/space width are kept with 8 binary places -
   * IE multiplied by 256.
   */
  if (new_freq > 0) {
    if (256 * 1000000L / new_freq * new_duty_cycle / 100 <=
        LIRC_TRANSMITTER_LATENCY)
      return -EINVAL;
    if (256 * 1000000L / new_freq * (100 - new_duty_cycle) / 100 <=
        LIRC_TRANSMITTER_LATENCY)
      return -EINVAL;
    duty_cycle = new_duty_cycle;
    freq = new_freq;
    period = 256 * 1000000L / freq;
    pulse_width = period * duty_cycle / 100;
    space_width = period - pulse_width;
  } else {
    duty_cycle = INVALID;
    freq = 0;
    period = INVALID;
    pulse_width = INVALID;
    space_width = INVALID;
  }
  dprintk("in init_timing_params, freq=%d pulse=%ld, "
          "space=%ld\n", freq, pulse_width, space_width);
  return 0;
}

static void rbwrite(int l)
{
  if (lirc_buffer_full(&rbuf)) {
    /* no new signals will be accepted */
    dprintk("Buffer overrun\n");
    return;
  }
  lirc_buffer_write(&rbuf, (void *)&l);
}

static void frbwrite(int l)
{
  /* simple noise filter */
  static int pulse, space;
  static unsigned int ptr;

  if (ptr > 0 && (l & PULSE_BIT)) {
    pulse += l & PULSE_MASK;
    if (pulse > 250) {
      rbwrite(space);
      rbwrite(pulse | PULSE_BIT);
      ptr = 0;
      pulse = 0;
    }
    return;
  }
  if (!(l & PULSE_BIT)) {
    if (ptr == 0) {
      if (l > 20000) {
        space = l;
        ptr++;
        return;
      }
    } else {
      if (l > 20000) {
        space += pulse;
        if (space > PULSE_MASK)
          space = PULSE_MASK;
        space += l;
        if (space > PULSE_MASK)
          space = PULSE_MASK;
        pulse = 0;
        return;
      }
      rbwrite(space);
      rbwrite(pulse | PULSE_BIT);
      ptr = 0;
      pulse = 0;
    }
  }
  rbwrite(l);
}

static irqreturn_t irq_handler(int i, void *blah, struct pt_regs *regs)
{
  struct timeval tv;
  long deltv;
  int data;
  int signal;

  /* use the GPIO signal level */
  signal = gpio_get_value(gpio_in_pin);

  if (sense != INVALID) {
    /* get current time */
    do_gettimeofday(&tv);

    /* calc time since last interrupt in microseconds */
    deltv = tv.tv_sec-lasttv.tv_sec;
    if (tv.tv_sec < lasttv.tv_sec ||
        (tv.tv_sec == lasttv.tv_sec &&
         tv.tv_usec < lasttv.tv_usec)) {
      printk(KERN_WARNING LIRC_DRIVER_NAME
             ": AIEEEE: your clock just jumped backwards\n");
      printk(KERN_WARNING LIRC_DRIVER_NAME
             ": %d %d %lx %lx %lx %lx\n", signal, sense,
             tv.tv_sec, lasttv.tv_sec,
             tv.tv_usec, lasttv.tv_usec);
      data = PULSE_MASK;
    } else if (deltv > 15) {
      data = PULSE_MASK; /* really long time */
      if (!(signal^sense)) {
        /* sanity check */
        printk(KERN_WARNING LIRC_DRIVER_NAME
               ": AIEEEE: %d %d %lx %lx %lx %lx\n",
               signal, sense, tv.tv_sec, lasttv.tv_sec,
               tv.tv_usec, lasttv.tv_usec);
        /*
         * detecting pulse while this
         * MUST be a space!
         */
        sense = sense ? 0 : 1;
      }
    } else {
      data = (int) (deltv*1000000 +
                    (tv.tv_usec - lasttv.tv_usec));
    }
    frbwrite(signal^sense ? data : (data|PULSE_BIT));
    lasttv = tv;
    wake_up_interruptible(&rbuf.wait_poll);
  }

  return IRQ_HANDLED;
}

static int init_port(void)
{
  int i, nlow, nhigh, ret;

  if (gpio_request(gpio_in_pin, LIRC_DRIVER_NAME " ir/in")) {
    printk(KERN_ALERT LIRC_DRIVER_NAME
           ": cant claim gpio pin %d\n", gpio_in_pin);
    ret = -ENODEV;
    goto exit_gpio_free_in_pin;
  }

  /* dprintk("claimed gpios"); */

  gpio_direction_input(gpio_in_pin);

  /* if pin is high, then this must be an active low receiver. */
  if (sense == -1) {
    /* wait 1/2 sec for the power supply */
    msleep(500);

    /*
     * probe 9 times every 0.04s, collect "votes" for
     * active high/low
     */
    nlow = 0;
    nhigh = 0;
    for (i = 0; i < 9; i++) {
      if (gpio_get_value(gpio_in_pin))
        nlow++;
      else
        nhigh++;
      msleep(40);
    }
    sense = (nlow >= nhigh ? 1 : 0);
    printk(KERN_INFO LIRC_DRIVER_NAME
           ": auto-detected active %s receiver on GPIO pin %d\n",
           sense ? "low" : "high", gpio_in_pin);
  } else {
    printk(KERN_INFO LIRC_DRIVER_NAME
           ": manually using active %s receiver on GPIO pin %d\n",
           sense ? "low" : "high", gpio_in_pin);
  }

  return 0;

 exit_gpio_free_in_pin:
  gpio_free(gpio_in_pin);

  return ret;
}

/* called when the character device is opened */
static int set_use_inc(void *data)
{
  int result;
  unsigned long flags;

  /* initialize timestamp */
  do_gettimeofday(&lasttv);

  result = request_irq(gpio_to_irq(gpio_in_pin),
                       (irq_handler_t) irq_handler,
                       IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
                       LIRC_DRIVER_NAME, (void*) 0);

  switch (result) {
  case -EBUSY:
    printk(KERN_ERR LIRC_DRIVER_NAME
           ": IRQ %d is busy\n",
           gpio_to_irq(gpio_in_pin));
    return -EBUSY;
  case -EINVAL:
    printk(KERN_ERR LIRC_DRIVER_NAME
           ": Bad irq number or handler\n");
    return -EINVAL;
  default:
    dprintk("Interrupt %d obtained\n",
            gpio_to_irq(gpio_in_pin));
    break;
  };

  /* initialize pulse/space widths */
  init_timing_params(duty_cycle, freq);

  spin_lock_irqsave(&lock, flags);

  spin_unlock_irqrestore(&lock, flags);

  return 0;
}

static void set_use_dec(void *data)
{
  unsigned long flags;

  spin_lock_irqsave(&lock, flags);

  spin_unlock_irqrestore(&lock, flags);

  free_irq(gpio_to_irq(gpio_in_pin), (void *) 0);

  dprintk(KERN_INFO LIRC_DRIVER_NAME
          ": freed IRQ %d\n", gpio_to_irq(gpio_in_pin));
}

static ssize_t lirc_write(struct file *file, const char *buf,
                          size_t n, loff_t *ppos)
{
  return n;
}

static long lirc_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
  int result;
  __u32 value;

  switch (cmd) {
  case LIRC_GET_SEND_MODE:
    return -ENOIOCTLCMD;
    break;

  case LIRC_SET_SEND_MODE:
    result = get_user(value, (__u32 *) arg);
    if (result)
      return result;
    /* only LIRC_MODE_PULSE supported */
    if (value != LIRC_MODE_PULSE)
      return -ENOSYS;
    break;

  case LIRC_GET_LENGTH:
    return -ENOSYS;
    break;

  case LIRC_SET_SEND_DUTY_CYCLE:
    dprintk("SET_SEND_DUTY_CYCLE\n");
    result = get_user(value, (__u32 *) arg);
    if (result)
      return result;
    if (value <= 0 || value > 100)
      return -EINVAL;
    return init_timing_params(value, freq);
    break;

  case LIRC_SET_SEND_CARRIER:
    dprintk("SET_SEND_CARRIER\n");
    result = get_user(value, (__u32 *) arg);
    if (result)
      return result;
    /* if (value > 500000 || value < 20000) */
    if (value > 500000 || value < 0)
      return -EINVAL;
    return init_timing_params(duty_cycle, value);
    break;

  default:
    return lirc_dev_fop_ioctl(filep, cmd, arg);
  }
  return 0;
}

static const struct file_operations lirc_fops = {
  .owner		= THIS_MODULE,
  .write		= lirc_write,
  .unlocked_ioctl	= lirc_ioctl,
  .read		= lirc_dev_fop_read,
  .poll		= lirc_dev_fop_poll,
  .open		= lirc_dev_fop_open,
  .release	= lirc_dev_fop_close,
  .llseek		= no_llseek,
};

static struct lirc_driver driver = {
  .name		= LIRC_DRIVER_NAME,
  .minor		= INVALID,
  .code_length	= 1,
  .sample_rate	= 0,
  .data		= NULL,
  .add_to_buf	= NULL,
  .rbuf		= &rbuf,
  .set_use_inc	= set_use_inc,
  .set_use_dec	= set_use_dec,
  .fops		= &lirc_fops,
  .dev		= NULL,
  .owner		= THIS_MODULE,
};

static struct platform_driver lirc_nxctrl_driver = {
  .driver = {
    .name   = LIRC_DRIVER_NAME,
    .owner  = THIS_MODULE,
  },
};

static int __init lirc_nxctrl_init(void)
{
  int result;

  /* Init read buffer. */
  result = lirc_buffer_init(&rbuf, sizeof(int), RBUF_LEN);
  if (result < 0)
    return -ENOMEM;

  result = platform_driver_register(&lirc_nxctrl_driver);
  if (result) {
    printk(KERN_ERR LIRC_DRIVER_NAME
           ": lirc register returned %d\n", result);
    goto exit_buffer_free;
  }

  lirc_nxctrl_dev = platform_device_alloc(LIRC_DRIVER_NAME, 0);
  if (!lirc_nxctrl_dev) {
    result = -ENOMEM;
    goto exit_driver_unregister;
  }

  result = platform_device_add(lirc_nxctrl_dev);
  if (result)
    goto exit_device_put;

  return 0;

 exit_device_put:
  platform_device_put(lirc_nxctrl_dev);

 exit_driver_unregister:
  platform_driver_unregister(&lirc_nxctrl_driver);

 exit_buffer_free:
  lirc_buffer_free(&rbuf);

  return result;
}

static void lirc_nxctrl_exit(void)
{
  if (gpio_in_pin != INVALID)
    gpio_free(gpio_in_pin);
  platform_device_unregister(lirc_nxctrl_dev);
  platform_driver_unregister(&lirc_nxctrl_driver);
  lirc_buffer_free(&rbuf);
}

static int __init lirc_nxctrl_init_module(void)
{
  int result;
  int in_pin_ok = 0;

  result = lirc_nxctrl_init();
  if (result)
    return result;

  if (gpio_in_pin != INVALID)
    in_pin_ok = 1;
  
  result = 0;
  if (gpio_in_pin == INVALID)
    printk(KERN_WARNING LIRC_DRIVER_NAME
           ": no GPIO inpin specified, probably desired.\n");
  else if (in_pin_ok != 1) {
    result = -EINVAL;
    printk(KERN_ERR LIRC_DRIVER_NAME
           ": invalid GPIO inpin specified!\n");
    goto exit_rpi;
  }

  driver.features = 0;
  if (softcarrier)
    driver.features |= LIRC_CAN_SET_SEND_DUTY_CYCLE | 
      LIRC_CAN_SET_SEND_CARRIER;
  if (gpio_in_pin != INVALID)
    driver.features |= LIRC_CAN_REC_MODE2;

  driver.dev = &lirc_nxctrl_dev->dev;
  driver.minor = lirc_register_driver(&driver);

  if (driver.minor < 0) {
    printk(KERN_ERR LIRC_DRIVER_NAME
           ": device registration failed with %d\n", result);
    result = -EIO;
    goto exit_rpi;
  }

  printk(KERN_INFO LIRC_DRIVER_NAME ": driver registered!\n");

  result = init_port();
  if (result < 0)
    goto exit_rpi;

  return 0;

 exit_rpi:
  lirc_nxctrl_exit();

  return result;
}

static void __exit lirc_nxctrl_exit_module(void)
{
  lirc_nxctrl_exit();

  lirc_unregister_driver(driver.minor);
  printk(KERN_INFO LIRC_DRIVER_NAME ": cleaned up module\n");
}

module_init(lirc_nxctrl_init_module);
module_exit(lirc_nxctrl_exit_module);

MODULE_DESCRIPTION("Infra-red receiver and blaster driver for NXCTRL in BBB.");
MODULE_AUTHOR("Aron Robert Szabo <aron@reon.hu>");
MODULE_AUTHOR("Michael Bishop <cleverca22@gmail.com>");
MODULE_AUTHOR("Bengt Martensson <barf@bengt-martensson.de>");
MODULE_AUTHOR("Sungjin Chun <chunsj@gmail.com>");
MODULE_LICENSE("GPL");

module_param(gpio_in_pin, int, S_IRUGO);
MODULE_PARM_DESC(gpio_in_pin, "GPIO input pin number. Use an interrupt capable pin."
		 " default is 60(P9:12)");

module_param(sense, int, S_IRUGO);
MODULE_PARM_DESC(sense, "Override autodetection of IR receiver circuit"
		 " (0 = active high, 1 = active low )");

module_param(softcarrier, int, S_IRUGO);
MODULE_PARM_DESC(softcarrier, "Software carrier (0 = off, 1 = on, default on)");

module_param(invert, int, S_IRUGO);
MODULE_PARM_DESC(invert, "Invert output (0 = off, 1 = on, default off)");

module_param(tx_mask, int, S_IRUGO);
MODULE_PARM_DESC(tx_mask, "Transmitter mask (default: 0x01)");

module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Enable debugging messages");
