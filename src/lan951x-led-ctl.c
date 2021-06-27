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

#include <stdio.h>
#include <stdlib.h>		/* exit() */
#include <string.h>		/* strlen() */
#include <stdint.h>
#include <libusb-1.0/libusb.h>
#include "lan951x-led-ctl.h"

/* global variables */
int led_arr[3] = { MODE_KEEP, MODE_KEEP, MODE_KEEP };

libusb_device_handle* lan951x_open(int vid, int pid)
{
	libusb_device** devs;
	libusb_device* dev;
	libusb_device_handle* handle = NULL;
	struct libusb_device_descriptor desc;
	int r,i=0;
	
	if ((libusb_init(NULL)) < 0) {
		fprintf(stderr, "libusb_init() failed\n");
		return NULL;
	}
//	libusb_set_debug(NULL, 3);
	if (libusb_get_device_list(NULL, &devs) < 0) {
		return NULL;
	}
	while ((dev=devs[i++]) != NULL) {
		if ((r=libusb_get_device_descriptor(dev, &desc)) < 0) {
			fprintf(stderr, "failed to get device descriptor");
			libusb_free_device_list(devs, 1);
			return NULL;
		}
		if ((desc.idVendor == vid) && (desc.idProduct == pid)) {
//			fprintf(stderr, "LAN951x found on USB bus %d, device %d\n",
//				libusb_get_bus_number(dev),
//				libusb_get_device_address(dev));
			if ((r=libusb_open(dev, &handle)) != 0) {
				fprintf(stderr, "libusb_open error :%s\n", libusb_error_name(r));
				return NULL;
			}
			libusb_free_device_list(devs, 1);
			return handle;
		}
	}
	fprintf(stderr, "No LAN951x found on USB\n");
	libusb_free_device_list(devs, 1);
	return NULL;
}

int lan951x_rd_reg(libusb_device_handle* h, uint16_t reg, uint32_t* val)
{
	return libusb_control_transfer(h, LIBUSB_REQUEST_TYPE_VENDOR|0x80,
		USB_VENDOR_REQUEST_RD_REG, 0, reg, (uint8_t*)val, 4,
		USB_CTRL_TIMEOUT);
}

int lan951x_wr_reg(libusb_device_handle* h, uint16_t reg, uint32_t val)
{
	return libusb_control_transfer(h, LIBUSB_REQUEST_TYPE_VENDOR,
		USB_VENDOR_REQUEST_WR_REG, 0, reg, (uint8_t*)&val, 4,
		USB_CTRL_TIMEOUT);
}

int ledmode(const char* s)
{
	if (*s == '0') {
		return MODE_OFF;
	} else if (*s == '1') {
		return MODE_ON;
	} else if ((*s == 's') || (*s == 'S')) {
		return MODE_STATUS;
	}
	return MODE_ERR;
}

void usage(void)
{
	printf("usage: lan951x-led-ctl [--fdx=x][--lnk=x][--spd=x]\n");
	printf("\twhere x is one of:\n");
	printf("\t0 - turn LED off\n\t1 - turn LED on\n\ts - LED shows status\n");
	exit(1);
}

int parse_args(int argc, char **argv)
{
	int i;

	for (i=1; i<argc; i++) {
		char *p=argv[i];
		if (strncmp(p, "--fdx=", 6) == 0) {
			led_arr[DUPIDX] = ledmode(p+6);
			printf("setting FDX LED to status %i\n", led_arr[DUPIDX]);
		} else if (strncmp(p, "--lnk=", 6) == 0) {
			led_arr[LNKIDX] = ledmode(p+6);
			printf("setting LNK LED to status %i\n", led_arr[LNKIDX]);
		} else if (strncmp(p, "--spd=", 6) == 0) {
			led_arr[SPDIDX] = ledmode(p+6);
			printf("setting SPD LED to status %i\n", led_arr[SPDIDX]);
		} else {
			usage();
		}
	}
	return i;
}

int main(int argc, char* argv[])
{
	libusb_device_handle* handle = NULL;
	uint32_t val;

	if (argc < 2) {
		printf("at least one argument is required\n");
		usage();
	}
	parse_args(argc, argv);

	uint32_t gp_mask[3];
	gp_mask[DUPIDX] = DUP_MASK;
	gp_mask[LNKIDX] = LNK_MASK;
	gp_mask[SPDIDX] = SPD_MASK;

	if ((handle=lan951x_open(LAN951X_VENDOR_ID, LAN951X_PRODUCT_ID)) == NULL) {
		printf("can not open usb device. are you root?\n");
		exit(1);
	}
	lan951x_rd_reg(handle, LED_GPIO_CFG, &val);
	for (int i=0; i < 3; i++) {
		if (led_arr[i] == MODE_ON) {
			val &= ~(gp_mask[i] & (GP_ALLCTL|GP_ALLDAT));
			val |= (gp_mask[i] & GP_ALLDIR);
		}
		if (led_arr[i] == MODE_OFF) {
			val &= ~(gp_mask[i] & GP_ALLCTL);
			val |= (gp_mask[i] & (GP_ALLDIR|GP_ALLDAT));
		}
		if (led_arr[i] == MODE_STATUS) {
			val &= ~(gp_mask[i] & (GP_ALLDIR|GP_ALLDAT));
			val |= (gp_mask[i] & GP_ALLCTL);
		}
	}
	lan951x_wr_reg(handle, LED_GPIO_CFG, val);
	libusb_close(handle);
}
