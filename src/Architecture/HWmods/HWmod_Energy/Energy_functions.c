/* ABS ******************************************************************************************
 *  File:   Energy_functions.c                                                                      *
 *  Descr.: EPS Hardware Module - Energy Managment.                                                 *
 *  Author: David Riudor.                                                                           *
 *  Date:   2016-may-22                                                                             *
 *  Vers.:  1.0                                                                                     *
 *                                                                                                  *
 *  This file is part of the ABS v1.0 project. "ABS" is an educational project developed at         *
 *  the Technical University of Catalonia - BarcelonaTech (Universitat Politècnica de Catalunya).   *
 ****************************************************************************************************
 *  Changelog:                                                                                      *
 *  - v1.0  David Riudor.                                                                           *
 ****************************************************************************************************/

/*** INCLUDE SECTION ********************************************************************************/

#include "mcs_riudor.h"


/*++ GLOBAL VARIABLES David Riudor*******************************************************************/

int haltFlag ;
int Pol_enabled[NUM_PAYLOADS] ;

// Global variables to dump sensor measurements David Riudor:

/*
Temperature_t     temperature_os[TEMP_SENSORS];     // Temperature data sensor.
Voltage_t         voltage_os[VOLT_SENSORS];         // Voltage data sensor.
Current_t         current_os[CUR_SENSORS];          // Current data sensor.
Irradiance_t      irradiance_os[IRR_SENSORS];       // Irradiance data sensor.
SoC_t             soc_os[SOC_SENSORS];              // State of charge battery.
*/
pthread_mutex_t mylock;

/*** FUNCTIONS **************************************************************************************/




/* INIT_FUNCTION FUNCTION **************************************************************************
 *  Name    :   init_function                                                                       *
 *  Descr.  :   Inicializate  the Sensors and Eps. The main functions doing are:                    *
 *               - Creates periodic task for checking the Point of Load.                            *
 *               - Creates periodic task for the data sensors adquisition.                          *
 *               - Creates mutex for the POL access control.                                        *
 *                                                                                                  *
 *  Returns :   void                                                                                *
 *                                                                                                  *
 *  Args.   :   pkt_in           Null                                                               *
 *              pkt_out          The functions saves inside this MCSPACKET if there was an error    *
 *                               or not during the function.                                        *
 *                                                                                                  *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   in the pkt_out we can see ERROR if something wrong happens while doing the init     *
 *              task or NOERROR if all have worked correctly.                                       *
 ****************************************************************************************************/
void init_function(MCSPacket *pkt_in , MCSPacket **pkt_out) /* setup_function() in the Cubsat*/
{
       
       pthread_mutex_init(&mylock, NULL);

       int currentState;
       currentState = MCS_MESSAGE_INIT ;

       MCSCommand actual = MCS_PAYLOAD_ARDUINO_INIT_I2C;
       int nargs= 2 ;
       int i = 0 ;
       unsigned short lon_data = 0;
       unsigned char *data = NULL;
       int anyerror = 0 ;

       unsigned char *argumentos ;
       argumentos = malloc(2*sizeof(unsigned char));
       argumentos[0] = (char)i; /*serial i2c id*/
       argumentos[1] = (char)RATE_I2C;     /*bitrate*/
       MCSPacket *pktToSend, *pktToReceive;
       pktToSend = mcs_create_packet(actual, nargs, argumentos, lon_data, data);
       anyerror = sdb_send_sync(pktToSend, &pktToReceive);

       if(anyerror == ERROR_INT){

          printf("Error in the initialization of the i2c Protocol\n");
          
          *pkt_out = mcs_err_packet(pkt_in , ERROR );
          /*(*pkt_out)->data= (unsigned char)ERROR ;*/

       }
       if( pktToReceive->type != MCS_TYPE_OK ){

          printf("Error in the received sdb packet\n");
          mcs_err_packet(pkt_in, ERROR);
          /*(*pkt_out)->data= (unsigned char)ERROR ;*/

       }
       for( i=0 ; i< NUM_POL ; i++ ){

           Pol_enabled[i] = 0 ;

       }

       if( pktToReceive->type == MCS_TYPE_OK || anyerror != ERROR_INT ){

          haltFlag = 1;  /* Allow the correct functionalities of run function*/
          *pkt_out = mcs_ok_packet(pkt_in);
          /*(*pkt_out)->data= (unsigned char)NOERROR ;*/
       }
}



/* HALT_FUNCTION FUNCTION ***************************************************************************
 *  Name    :   halt_function                                                                       *
 *  Descr.  :   Halt action for the Sensors and EPS. The main functions doing are:                  *
 *                - Disable all the POLs                                                            *
 *                - Uninitializate the i2c protocol                                                 *
 *                - Finish the run function                                                         *
 *                                                                                                  *
 *  Returns :   void                                                                                *
 *                                                                                                  *
 *  Args.   :   pkt_in           Null                                                               *
 *              pkt_out          The functions saves inside this MCSPACKET if there was an error    *
 *                               or not during the function.                                        *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
void halt_function(MCSPacket *pkt_in , MCSPacket **pkt_out)
{

    int currentState;
    currentState = MCS_MESSAGE_HALT ;

    int i;
    int a = 2 ; /*random number different to 0 and 1 because 0 is disabled and 1 enabled*/
    int counter ;
    unsigned char anyerror = 0x00;
    counter = 0;
    int direction;
    int nargs = 1 ;
    unsigned short lon_data = 0;
    unsigned char *data;
    data = NULL;
    unsigned char *argumentos ;
    int anyerror2 = 0;
    
    for(i = 0; i < NUM_PAYLOADS; i++ ){

        direction = FIRST_POL_DIRECTION + i ;
        a = ispolenabled_function2((unsigned char)direction);

        if(a == 1 ){

	    anyerror = disablepol_function2((unsigned char)direction);

            if( anyerror == (unsigned char)ERROR ){

            printf("The payload %d could'nt been disabled ", direction );

            }
            else{

            counter++;

            }
        }
    }

    if(counter == NUM_PAYLOADS){

    printf("All the payloads could disable correctly");

    }

    MCSCommand actual = MCS_PAYLOAD_ARDUINO_UNINIT_I2C ;
    argumentos = NULL ;
    MCSPacket *pktToSend, *pktToReceive;
    pktToSend = mcs_create_packet(actual, nargs, argumentos, lon_data, data);
  
    anyerror2 = sdb_send_sync(pktToSend, &pktToReceive);

    if(anyerror2 == ERROR_INT){

       printf("Error unitializating the i2c protocol");
       mcs_err_packet(pkt_in , ERROR);
       *pkt_out = mcs_err_packet(pkt_in, ERROR);
       /*(*pkt_out)->data= (unsigned char)ERROR ;*/

    }

    if(pktToReceive->type != MCS_TYPE_OK){

       printf("Error in the received sdb packet\n");
       
       *pkt_out = mcs_err_packet(pkt_in, ERROR);
       /*(*pkt_out)->data= (unsigned char)ERROR ;*/

    }


    if(pktToReceive->type == MCS_TYPE_OK || anyerror2 != ERROR_INT){

       haltFlag = 0;  /*Finishes the run function*/
       *pkt_out = mcs_ok_packet(pkt_in);
       /*(*pkt_out)->data= (unsigned char)NOERROR ;*/

       }

    pthread_mutex_destroy(&mylock);

}



/* CHECK_FUNCTION FUNCTION  **************************************************************************
 *  Name    :   check_function                                                                      *
 *  Descr.  :   Performs an initial test of the hardware, to check if is ready to work.             *
 *                                                                                                  *
 *  Returns :   void                                                                                *
 *                                                                                                  *
 *  Args.   :   pkt_in           Null                                                               *
 *              pkt_out          The functions saves inside this MCSPACKET if there was an error    *
 *                               or not following this standards:                                   *
 *                                                                                                  *
 *                                                                                                  *
 *              Returns the errors occured in the differents stages of the init task. The           *
 *              codification of the errors is the following:                                        *
 *                                                                                                  *
 *                ERROR: 0 0 0 0 0 0 0 0 0  0 1 0                                                   *
 *                       | | | | | | | | |  | | |                                                   *
 *                       | | | | | | | | |  | | |                                                   *
 *                       | | | | | | | | |  +-+-+------------> (3 bits) Primary error.              *
 *                       +-+-+-+-+-+-+-+-+------------------> ( 9 bits) Secondary error number.     *
 *                                                                                                  *
 *              The primary error number will be used to indicate the type of errors occured in init*
 *              test.                                                                               *
 *              The secondary error number will be used to indicate the number of Point of Load that*
 *              has fails.                                                                          *
 *                                                                                                  *
 *              error 000 error with initialization of the i2c protocol                             *
 *              error 001 error with the SdB                                                        *
 *              error 010 error related with the enable of the POL                                  *
 *              error 011 error related with the disable of the POL                                 *
 *              error 100 battery too much charged                                                  *
 *              error 101 battery not enough charged                                                *
 *              error 110 error with the uninitialization of the i2c protocol                       *
 *              error 111 more than one error                                                       *
 *                                                                                                  *
 *              If not error found, NOERROR is returned.                                            *
 *                                                                                                  *
 *  Author  :   David Riudor                                                                        *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
void check_function(MCSPacket *pkt_in, MCSPacket **pkt_out)
{
    static pthread_mutex_t mylock;
    pthread_mutex_init(&mylock, NULL);

    int currentState;
    currentState = MCS_MESSAGE_CHECK ;

    MCSCommand actual =  MCS_PAYLOAD_ARDUINO_INIT_I2C ;
    int nargs = 2 ;
    int anyerror;
    unsigned short lon_data = 0;
    unsigned char *data;
    data = NULL;
    unsigned char *result;
    result = malloc(2*sizeof(unsigned char));
    int i;
    int a = 2; /*random number different to 0 and 1 because 0 is disabled and 1 enabled*/
    int counter = 0;
    int direction;

    unsigned char *argumentos;
    argumentos = malloc(2*sizeof(unsigned char));
    argumentos[0] = pkt_in->data[0] ;  /*serial i2c id*/
    argumentos[1] = (unsigned char)RATE_I2C ;     /*bitrate*/
    MCSPacket *pktToSend , *pktToReceive ;
    pktToSend = mcs_create_packet(actual, nargs, argumentos, lon_data, data);
    anyerror = sdb_send_sync(pktToSend, &pktToReceive);

    int provisionalReturn = NOERROR;

    if(pktToReceive->type != MCS_TYPE_OK_DATA){ /*error in the initi2c task*/

        provisionalReturn = 0x000;

    }

    if(anyerror == ERROR_INT){ /*Error with the SdB*/

        provisionalReturn = provisionalReturn | 0x001;

    }

    for(i=0; i < NUM_PAYLOADS; i++){

        direction = FIRST_POL_DIRECTION +i ;
        enablepol_function2((unsigned char)direction);
        a = ispolenabled_function2((unsigned char)direction);

        if(a == (unsigned char)0 && counter == 0){
            if (provisionalReturn != 0x000){

                provisionalReturn = (provisionalReturn & 0xFF8) | 0x007   ;

            }
            provisionalReturn = provisionalReturn | 0x002   ;
        }
        if(a == (unsigned char)0 ){

            provisionalReturn = provisionalReturn | (0x800<<(NUM_PAYLOADS - i)) ; 

        }
        else{

            counter ++ ;
        }
     }
    if(counter == NUM_PAYLOADS){

        printf("All the payloads could enable correctly");

    }

    counter = 0;
    direction = 0;
    for(i=0 ;i < NUM_PAYLOADS; i++){

        direction = FIRST_POL_DIRECTION + i ;
        disablepol_function2((unsigned char)direction);
        a = ispolenabled_function2((unsigned char)direction);

        if(a == (unsigned char)1 && counter == 0){
            if (provisionalReturn != 0x000){

                provisionalReturn = (provisionalReturn&0xFF8) | 0x007;

            }
            provisionalReturn = provisionalReturn | 0x003;
        }
        if(a == (unsigned char)0){

            provisionalReturn = provisionalReturn|(0x800<<(NUM_PAYLOADS - i));

        }
        else{

            counter ++ ;
        }
    }
    if(counter == NUM_PAYLOADS){

        printf("All the payloads could enable correctly");

    }


    /*Check the sensors <--- Not done*/

    unsigned char SoC_t ;
    SoC_t = soc_function2(); 

    if(SoC_t > (unsigned char)MAX_VOLTAGE_BATTERY){
        if(provisionalReturn != 0x000){

            provisionalReturn = (provisionalReturn&0xFF8) | 0x007;

        }
        provisionalReturn = provisionalReturn | 0x004;
        printf("The battery is full charged");

    }

    if(SoC_t < (unsigned char)MIN_VOLTAGE_BATTERY){
        if(provisionalReturn != 0x000){

            provisionalReturn = (provisionalReturn & 0xFF8) | 0x007 ;

        }
        provisionalReturn = provisionalReturn | 0x005;
        printf("The battery is low charged");

    }

    /*Free the memory <--- Not done ( unmalloc )*/

    actual = MCS_PAYLOAD_ARDUINO_UNINIT_I2C ; 
    nargs = 1;
    lon_data = 0;
    data = NULL;
    argumentos = NULL;
    
    pktToSend = mcs_create_packet(actual, nargs, argumentos, lon_data, data);
    anyerror = sdb_send_sync(pktToSend, &pktToReceive);

    if( anyerror == ERROR_INT ){ /*error sdb*/
        if ( provisionalReturn != 0x000 ){

            provisionalReturn = (provisionalReturn & 0xFF8) | 0x007;

        }
        provisionalReturn = provisionalReturn | 0x001 ;
    }
    if(pktToReceive->type != MCS_TYPE_OK_DATA){ /*error uninit*/
        if ( provisionalReturn != 0x000 ){

            provisionalReturn = (provisionalReturn & 0xFF8) | 0x007;

        }
        provisionalReturn = provisionalReturn | 0x006;
    }
    result[0] = ((unsigned char)provisionalReturn & 0xFF00) >> 8 ;
    result[1] = (unsigned char)provisionalReturn & 0x00FF ;
    mcs_ok_packet_data(pkt_in, result, 2);
    /*(*pkt_out)->data = (unsigned char)provisionalReturn;*/

    pthread_mutex_destroy(&mylock);

}

/* RUN_FUNCTION FUNCTION ****************************************************************************
 *  Name    :   run_function                                                                        *
 *  Descr.  :   Run mode for the Sensors and Eps. The main functions doing are:                     *
 *               - Register all the one shot callbacks.                                             *
 *               - Continuously collect data form POL and sensors.                                  *
 *               - Continiously check te battery State and do the necessaries adjusts.              *
 *               - Unregister all the one shot callbacks.                                           *
 *                                                                                                  *
 *                                                                                                  *
 *  Returns :   void                                                                                *
 *                                                                                                  *
 *  Args.   :   pkt_in           Null                                                               *
 *              pkt_out          The functions saves inside this MCSPACKET if there was an error    *
 *                               or not.                                                            *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
void run_function(MCSPacket *pkt_in , MCSPacket **pkt_out)
{
    int sdb_res;
    unsigned char temperature;
    unsigned char irradiance;
    int i;
    int direction;
    int contadorRun = 1;
    
    sdb_res = sdb_register_callback(MCS_STATE_VOLTAGE,voltage_function);
    sdb_res = sdb_register_callback(MCS_STATE_CURRENT,current_function);
    sdb_res = sdb_register_callback(MCS_STATE_SOC,soc_function);
    sdb_res = sdb_register_callback(MCS_STATE_TEMPERATURE,temperature_function);
    sdb_res = sdb_register_callback(MCS_STATE_IRRADIANCE,irradiance_function);
    sdb_res = sdb_register_callback(MCS_MESSAGE_ENABLEPOL,enablepol_function);
    sdb_res = sdb_register_callback(MCS_MESSAGE_DISABLEPOL,disablepol_function);
    sdb_res = sdb_register_callback(MCS_MESSAGE_ISPOL,ispolenabled_function);

    while(haltFlag){


        unsigned char battery;
        battery = soc_function2();

        if(battery < (unsigned char)MIN_VOLTAGE_BATTERY){

            if(ispolenabled_function2((unsigned char)FIRST_LESS_IMPORTANT) == (unsigned char)1 ){

                disablepol_function2((unsigned char)FIRST_LESS_IMPORTANT);
            }
        }
        else if((battery < (unsigned char)MEDIUM_VOLTAGE_BATTERY ) && (battery > (unsigned char)MIN_VOLTAGE_BATTERY)){


                if(ispolenabled_function2((unsigned char)SECOND_LESS_IMPORTANT) == (unsigned char)1){

                     disablepol_function2((unsigned char)SECOND_LESS_IMPORTANT);

                }

        }

        if(contadorRun == 1){

            printf("[HWmod:eps:run] All RT tasks started\n");
            contadorRun == 2;
        }

        for ( i=0 ; i< NUM_PAYLOADS ; i++ ){

            direction = FIRST_POL_DIRECTION + i ;
            voltage_function2((unsigned char)direction);      /*the data is saved in the sdb database*/
            current_function2((unsigned char)direction);

        }
        for ( i=0 ; i< NUM_SENSORS ; i++ ){

            direction = FIRST_SENSOR_DIRECTION + i ;
            temperature = temperature_function2((unsigned char)direction);
            irradiance = irradiance_function2((unsigned char)direction);

        }

        wait(1000);
    }


    printf("Run has finished");   /*Should never finish*/

    sdb_res = sdb_unregister_callback(MCS_STATE_VOLTAGE);
    sdb_res = sdb_unregister_callback(MCS_STATE_CURRENT);
    sdb_res = sdb_unregister_callback(MCS_STATE_SOC);
    sdb_res = sdb_unregister_callback(MCS_STATE_TEMPERATURE);
    sdb_res = sdb_unregister_callback(MCS_STATE_IRRADIANCE);
    sdb_res = sdb_unregister_callback(MCS_MESSAGE_ENABLEPOL);
    sdb_res = sdb_unregister_callback(MCS_MESSAGE_DISABLEPOL);
    sdb_res = sdb_unregister_callback(MCS_MESSAGE_ISPOL);

    *pkt_out = mcs_ok_packet( pkt_in );
    /*(*pkt_out)->data= (unsigned char)NOERROR ;*/      
}

/* MAIN_FUNCTION FUNCTION ****************************************************************************
 *  Name    :   main_function                                                                        *
 *  Descr.  :   Main function that executes all the other functions developed in the EPS             *
 *                                                                                                   *
 *                                                                                                   *
 *  Returns :   void                                                                                 *
 *                                                                                                   *
 *  Args.   :   --                                                                                   *
 *                                                                                                   *
 *  Author  :   David Riudor.                                                                        *
 *  Remarks :   --                                                                                   *
 ****************************************************************************************************/
void main()
{
    int anyerror;
    int sdb_res;
    char *name;
    name = "EPS" ;
    
    anyerror = sdb_connect(name, SDB_GROUP_HWMOD);

    if(anyerror == ERROR){

        printf("Error in the connection of the sdb");
    }

    sdb_res = sdb_register_callback(MCS_MESSAGE_RUN,run_function);
    sdb_res = sdb_register_callback(MCS_MESSAGE_CHECK,check_function);
    sdb_res = sdb_register_callback(MCS_MESSAGE_INIT,init_function);
    sdb_res = sdb_register_callback(MCS_MESSAGE_HALT,halt_function);


     sdb_uninit();



    /*
    int i= 0;

    for(i=0 ; i< MAX_THREADS ; i++ ){

        pthread_join(tabla_thr[i], (void *)&res);

    }
     */


}

