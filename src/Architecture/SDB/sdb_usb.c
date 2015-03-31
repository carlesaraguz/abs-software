#include "sdb_usb.h"

static char *sdb_to_usb(MCSPacket *packet, int *size)
{
    if(packet->type == MCS_TYPE_PAYLOAD) {
        MCSCommandOptionsPayload pack = option_payload[packet->cmd];
        char *usb_packet ={'\x00', '\x00'};
        //TODO
        *size = 10;
        return usb_packet;
    } else {
        return NULL;
    }
}

void *usb_thread(void *arg)
{
    char *buffer;
    QueueElement element;
    MCSPacket *response;
    int data_size, packet_size, response_type;
    char response_usb[MAX_SIZE_USB_RESPONSE];
    int fd = open(SDB_USB_DEVICE, O_RDWR);

    while(element = usb_queue_pop()) {
        buffer = sdb_to_usb(element.data, &packet_size);
        write(fd, buffer, packet_size);     
        read(fd, response_usb, MAX_SIZE_USB_RESPONSE);           
        response_type = (response_usb[0] << 3) & 0x03;
        switch(response_type) {
            case OK:
                response = mcs_ok_packet();
                break;
            case OK_DATA:
                data_size = (int)(result[0] << 1);
                response = mcs_ok_packet_data(&response_usb[2], data_size);
                break;
            case ERROR:
                response = mcs_err_packet(EHWFAULT);
                break;
        }
        write_mcs_packet_module(response, element.id_process);
    }
}