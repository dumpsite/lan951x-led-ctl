/*
	lan951x-led-ctl - control LEDs of LAN951X ethernet/usb controllers

	Copyright (C) 2015-2020 Dominic Radermacher <dominic@familie-radermacher.ch>

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License version 3 as
	published by the Free Software Foundation

	This program is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software Foundation,
	Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#define USB_CTRL_TIMEOUT	(5000)
#define LAN951X_VENDOR_ID	(0x0424)
#define	LAN951X_PRODUCT_ID	(0xec00)

/* values taken from kernel driver smsc95xx.h */
#define USB_VENDOR_REQUEST_WR_REG	(0xa0)
#define USB_VENDOR_REQUEST_RD_REG	(0xa1)
#define LED_GPIO_CFG			(0x24)
/* meaning of the bits in the LED_GPIO_CFG register */
#define GPDAT0		(1<<0)
#define GPDAT1		(1<<1)
#define GPDAT2		(1<<2)
#define GPDIR0		(1<<4)
#define GPDIR1		(1<<5)
#define GPDIR2		(1<<6)
#define GPCTL0		(1<<16)
#define GPCTL1		(1<<20)
#define GPCTL2		(1<<24)

#define GP_ALLDIR	(GPDIR0 | GPDIR1 | GPDIR2)
#define GP_ALLDAT	(GPDAT0 | GPDAT1 | GPDAT2)
#define GP_ALLCTL	(GPCTL0 | GPCTL1 | GPCTL2)

#define DUP_MASK	(GPCTL0 | GPDIR0 | GPDAT0)
#define LNK_MASK	(GPCTL1 | GPDIR1 | GPDAT1)
#define SPD_MASK	(GPCTL2 | GPDIR2 | GPDAT2)

#define DUPIDX		(0)
#define LNKIDX		(1)
#define SPDIDX		(2)

#define MODE_OFF	0
#define MODE_ON		1
#define MODE_STATUS	2
#define MODE_KEEP	4
#define MODE_ERR	-1

void usage(void);
int ledmode(const char* str);
int parse_args(int argc, char** argv);
libusb_device_handle* lan951x_open(int vid, int pid);
int lan951x_rd_reg(libusb_device_handle* h, uint16_t reg, uint32_t* val);
int lan951x_wr_reg(libusb_device_handle* h, uint16_t reg, uint32_t val);
