#ifndef __SDB_USB_H
#define __SDB_USB_H

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "usb_queue.h"

#define SDB_USB_DEVICE "/dev/usb_accessory"

void* sdb_usb(void *arg);

#endif /* __USB_QUEUE_H */