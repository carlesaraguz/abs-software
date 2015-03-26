#include "sdb_usb.h"

static byte[] sdbToUsb(MCSPacket *packet)
{
    if(packet->type == MCS_TYPE_PAYLOAD) {

        MCSCommandOptionsPayload command = optionPayload[packet->cmd];

        byte packet[] = {(byte)command->cmd,(byte)10}

        return packet;

    } else {
        return NULL;
    }
}

void *usb_thread(void *arg)
{
    int index, resp_type;

    byte[] packet_usb;
    byte[] response_usb;

    MCSPacket packet, *response;

    int data_size = 0, i=0;

    int fd = open(SDB_USB_DEVICE, O_RDWR);

    while(packet = usb_queue_pop(&index)) {

        buffer = sdbToUsb(packet);
        
        write(fd,&buffer,sizeof(buffer));
        
        read(fd,response_usb, MAX_SIZE_USB_RESPONSE); 
        
        resp_type = (response_usb[0] << 3) & 0x03;

        switch(resp_type){
            case OK:
                response = mcs_ok_packet();
                break;
            case OK_DATA:
                data_size = (int) (result[0] << 1) & 0xFF);
                for(i=0; i<data_size; i++) {
                    response_usb[i] = response_usb[2+i];
                }
                response_usb = realloc(response_usb,data_size*response_usb);
                response = mcs_ok_packet_data(&response_usb,(result[0] << 1) & 0xFF);
                break;
            case ERROR:
                response = mcs_err_packet(EHWFAULT);
                break;
        }
        write_mcs_packet_module(&response, index);
    }
}