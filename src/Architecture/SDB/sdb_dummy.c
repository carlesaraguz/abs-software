#include "sdb_dummy.h"


void* push(void *arg)
{
    while(1){
        usb_queue_push("A1", 1); 
        sleep(1);
    }
}



int main(int argc , char *argv[])
{
    pthread_t tid[2];

    usb_queue_init();

    pthread_create(&(tid[0]), NULL, &push, NULL);

    pthread_create(&(tid[1]), NULL, &sdb_usb, NULL);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);	

}


