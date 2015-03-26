#ifndef __SDB_USB_H
#define __SDB_USB_H

#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <mcs.h>
#include <abs.h>
#include "usb_queue.h"

#define SDB_USB_DEVICE "/dev/usb_accessory"

#define MAX_SIZE_USB_RESPONSE 256

typedef enum {
    OK,
    OK_DATA,
    ERROR
} UsbResponse;

void* sdb_usb(void *arg);

#endif /* __USB_QUEUE_H */