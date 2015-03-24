#include "sdb_usb.h"

static byte[] sdbToUsb(MCSPacket packet)
{
    if(packet.type == MCS_TYPE_PAYLOAD) {

        MCSCommandOptionsPayload = packet.cmd

        byte packet[] = {(byte)packet.command,(byte)10}
        return packet;

    } else {
        return NULL;
    }
}


void* usb_thread(void *arg)
{
    char *packet;

    int fd = open(SDB_USB_DEVICE, O_RDWR);

    MCSPacket packet;

    while(packet = usb_queue_pop()){
        char[] buf = sdbToUsb(packet);
        write(fd,&buffer,sizeof(buffer));	
        read(fd,&response);
    }
}


