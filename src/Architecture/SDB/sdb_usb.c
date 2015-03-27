#include "sdb_usb.h"

static byte[] sdbToUsb(MCSPacket *packet)
{
    if(packet->type == MCS_TYPE_PAYLOAD) {
        MCSCommandOptionsPayload pack = optionPayload[packet->cmd];
        byte packet[] ={'\x00','\x00'};
        
        //TODO
        
        return packet;
    } else {
        return NULL;
    }
}

void *usb_thread(void *arg)
{
    int index, resp_type;
 
    byte[] packet_usb;   
    byte[] response_usb, response_data_usb;

    MCSPacket packet, *response;
 
    int data_size = 0, i=0;
 
    int fd = open(SDB_USB_DEVICE, O_RDWR);
 
    while(packet = usb_queue_pop(&index)) {
        buffer = sdbToUsb(packet);

        write(fd,&buffer,sizeof(buffer));
        
        read(fd,response_usb, MAX_SIZE_USB_RESPONSE);     
        
        resp_type = (response_usb[0] << 3) & 0x03;
        
        switch(resp_type) {
            case OK:
                response = mcs_ok_packet();
                break;
            case OK_DATA:
                data_size = (int)(result[0] << 1);
                memmove(response_data_usb, response_usb + 2, data_size);                
                response = mcs_ok_packet_data(&response_usb, data_size);
                break;
            case ERROR:
                response = mcs_err_packet(EHWFAULT);
                break;
        }
        write_mcs_packet_module(&response, index);
    }
}