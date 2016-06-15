/* ABS ******************************************************************************************
 *  File:   OneShot_functions.c                                                                     *
 *  Descr.: EPS Hardware Module - Energy Managment.                                                 *
 *  Author: David Riudor.                                                                           *
 *  Date:   2016-march-17                                                                           *
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

/*** GLOBAL VARIABLES *******************************************************************************/



/* VOLTAGE_FUNCTION FUNCTION    *********************************************************************
 *  Name    :   voltage_function                                                                    *
 *  Descr.  :   One-shot function of voltage. Returns the voltage of the selected POL/SENSOR        *
 *                                                                                                  *
 *  Returns :   void                                                                                *
 *                                                                                                  *
 *  Args.   :   pkt_in           MCSPACKET with the direction of the POL/SENSOR that we want        *
 *                               to read inside the data field                                      *
 *              pkt_out          The functions saves the required voltage inside the data of        *
 *                               this MCSPACKET                                                     *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/

void voltage_function(MCSPacket *pkt_in , MCSPacket **pkt_out_def) 
{

    unsigned char command;      /* Action sent to MicroController.*/
    command = MCS_STATE_VOLTAGE;  /* Initialize the specific command.*/
    
    unsigned char direction;
    unsigned char voltage;
    
    direction = pkt_in->data[0] ;
    
    voltage = voltage_function2(direction);
    
    
    if((direction > FIRST_POL_DIRECTION) && (direction < LAST_POL_DIRECTION)){
        
       (*pkt_out_def)->args[0] = POL;
    }
    else{
           
       (*pkt_out_def)->args[0] = SENSOR;
           
    }

    (*pkt_out_def)->data_size = 1;


    (*pkt_out_def)->data[0] = voltage;


}
    
/* VOLTAGE_FUNCTION FUNCTION ********************************************************************
 *  Name    :   voltage_function                                                                    *
 *  Descr.  :   One-shot function of voltage. Returns one measure (unsigned char) of the            *
 *              selected POL/SENSOR                                                                 *
 *                                                                                                  *
 *  Args.   :   direction          unsigned char with the direction of the sensor that we want to   *
 *                                 read.                                                            *
 *                                                                                                  *
 *  Returns :   unsigned char   -> Voltage of the selected POL/SENSOR                               *
 *              ERROR           -> Error in the function.                                           *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/

unsigned char voltage_function2( unsigned char direction ){
    
    MCSCommand actual = MCS_PAYLOAD_ARDUINO_READ_I2C ;
    int nargs= 1;
    int anyerror = 0 ;
    unsigned short lon_data = 0;
    int lonpkt ;
    int *datapktpol ;
    unsigned char *data;
    data = NULL ;
    
    unsigned char *argumentos;
    argumentos = malloc(2*sizeof(unsigned char));
    argumentos[0] =  (char)direction ; /*the direction of the i2c protocol that we want to read*/
    argumentos[1] = 'V' ;
    MCSPacket *pktToSend, *pktToReceive;
    pktToSend = mcs_create_packet(actual, nargs, argumentos, lon_data, data);
    anyerror = sdb_send_sync(pktToSend, &pktToReceive);
    
    if(anyerror == ERROR_INT){
        
        printf("Error with the sdb\n");
        return (unsigned char)ERROR ;
        
    }
    if(pktToReceive->type != MCS_TYPE_OK_DATA){
        
        printf("Error in the received sdb packet\n");
        return (unsigned char)ERROR ;
    }
        
    if(pktToReceive->type == MCS_TYPE_OK_DATA || anyerror != ERROR_INT){
            
        lonpkt = pktToReceive->data_size;    
        datapktpol = malloc(lonpkt*1*sizeof(int));
        
        return pktToReceive->data[0];
    }
  
}

/* CURRENT_FUNCTION FUNCTION    *********************************************************************
 *  Name    :   current_function                                                                    *
 *  Descr.  :   One-shot function of current. Returns the current of the selected POL/SENSOR        *
 *                                                                                                  *
 *  Returns :   void                                                                                *
 *                                                                                                  *
 *  Args.   :   pkt_in           MCSPACKET with the direction of the POL/SENSOR that we want        *
 *                               to read inside the data field                                      *
 *              pkt_out          The functions saves the required current inside the data of        *
 *                               this MCSPACKET                                                     *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
    
void current_function(MCSPacket *pkt_in , MCSPacket **pkt_out_def)
{
        
    unsigned char command;      /*Action sent to MicroController.*/
    command = MCS_STATE_CURRENT;  /* Initialize the specific command.*/
        
    unsigned char direction;
    unsigned char current;
        
    direction = pkt_in->data[0] ;
        
    current = current_function2(direction);
        
        
    if((direction > FIRST_POL_DIRECTION) && (direction< LAST_POL_DIRECTION)){
            
        (*pkt_out_def)->args[0] = POL;
    }
    else{
            
        (*pkt_out_def)->args[0] = SENSOR;
            
    }
        
    (*pkt_out_def)->data_size = 1 ;
        
        
    (*pkt_out_def)->data[0] = current ;
        
        
}
 
/* CURRENT_FUNCTION FUNCTION ********************************************************************
 *  Name    :   current_function                                                                    *
 *  Descr.  :   One-shot function of current. Returns one measure (unsigned char) of the            *
 *              selected POL/SENSOR                                                                 *
 *                                                                                                  *
 *  Args.   :   direction          unsigned char with the direction of the sensor that we want to   *
 *                                 read                                                             *
 *                                                                                                  *
 *  Returns :   unsigned char   -> current of the selected POL/SENSOR                               *
 *              ERROR           -> Error in the function.                                           *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/

unsigned char current_function2(unsigned char direction){
    
    MCSCommand actual = MCS_PAYLOAD_ARDUINO_READ_I2C;
    int nargs = 1;
    int anyerror = 0 ;
    unsigned short lon_data = 0;
    unsigned char *data;
    data = NULL;
    MCSPacket pkt_out_def;
    
    unsigned char *argumentos;
    argumentos = malloc(2*sizeof(unsigned char));
    argumentos[0] =  (char)direction ; /*the direction of the i2c protocol that we want to read*/
    argumentos[1] = 'C' ; /*Means current*/
    MCSPacket *pktToSend, *pktToReceive;
    pktToSend = mcs_create_packet(actual, nargs, argumentos, lon_data, data);
    anyerror = sdb_send_sync(pktToSend, &pktToReceive);
    
    if( anyerror == ERROR_INT ){
        
        printf("Error with the sdb\n");
        return (unsigned char) ERROR ;
        
    }
    if(pktToReceive->type != MCS_TYPE_OK_DATA){
        
        printf("Error in the received sdb packet\n");
        return (unsigned char)ERROR ;
        
    }
    
    if(pktToReceive->type == MCS_TYPE_OK_DATA || anyerror != ERROR_INT){
            
        int lonpkt ;
        lonpkt = pktToReceive->data_size;
        int *datapktpol ;
            
        datapktpol = malloc(lonpkt*1*sizeof(int));
        return pktToReceive->data[0];
    }
}


/* soc_function FUNCTION ****************************************************************************
 *  Name    :   soc_function                                                                        *
 *  Descr.  :   One-shot function of state of charge. Returns the measurements of the state of      *
 *              charge from the battery.                                                            *
 *                                                                                                  *
 *  Returns :   void                                                                                *
 *                                                                                                  *
 *  Args.   :   pkt_in           null                                                               *
 *              pkt_out          The functions saves the state of the battery inside the data of    *
 *                               this MCSPACKET                                                     *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
void soc_function(MCSPacket *pkt_in , MCSPacket **pkt_out_def)
{
    /*
    RT_TASK get_soc;            // get_soc Real-time TASK.
    unsigned char command;      // Action sent to MicroController.

#ifdef DISABLE_EPS_DEBUG
    SoC_t *s;
    s = (SoC_t *)malloc(sizeof(SoC_t)*SOC_SENSORS);
    s->battery_id = 1;
    s->soc = 70.0;
    printf("[HWmod:eps:get:soc(dbg)] Getting state-of-charge\n");
    return s;
#endif

    command = EPS_CMD_GET_SOC;  // Initialize the specific command.
    error_get_sensors = EGP0;   // Initialize error variable (will be written in the RT task)

    //Create get_soc task:
    if (rt_task_create(&get_soc, TASK_NAME_GET_SOC, EPS_STACKSIZE, EPS_GETSOC_PRIO, T_JOINABLE) != 0)
    {
        syslog_hwmod(LOG_TYPE_ERROR, ESOFT, "Error creating GET SOC task in the get soc one shot function.", "eps");
        return NULL;
    }

    //Start get_soc task:
    if (rt_task_start(&get_soc, &get_sensors, &command) != 0)
    {
        syslog_hwmod(LOG_TYPE_ERROR, ESOFT, "Error initialising GET SOC task in the get soc one shot function.", "eps");
        return NULL;
    }

    // Wait for get_soc task completion:
    rt_task_join(&get_soc);
    rt_task_delete(&get_soc);

    // Check final errors:
    if(error_get_sensors == NOERROR) return soc_os;
    else return NULL;
     */
}
unsigned char soc_function2()
{

/*To do*/

}

/* ENABLEPOL_FUNCTION FUNCTION **********************************************************************
 *   Name   :   enablepol_function                                                                  *
 *  Descr.  :   One-shot function to enable POL.                                                    *
 *                                                                                                  *
 *21  Args.   :   pkt_in           MCSPACKET with the direction of the POL that we want to enable     *
 *              pkt_out          The functions saves in this MCSPACKEt wether or not the enabling   *
 *                               of the POL was succesful with the message ERROR or NOERROR         *
 *  Returns :   void                                                                                *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
void enablepol_function( MCSPacket *pkt_in , MCSPacket **pkt_out) 
{
    
    unsigned char command;      /* Action sent to MicroController.*/
    command = MCS_MESSAGE_ENABLEPOL;  /* Initialize the specific command.*/
    
    unsigned char direction;
    int result;
    
    direction= pkt_in->data[0];
    
    result = enablepol_function2(direction);
    
    (*pkt_out)->data[0] = (unsigned char)result ;

}

/* ENABLEPOL_FUNCTION FUNCTION **********************************************************************
 *  Name    :   enablepol_function                                                                  *
 *  Descr.  :   One-shot function to enable POL.                                                    *
 *                                                                                                  *
 *  Args.   :   direction          unsigned char with the direction of the POL that we want to      *
 *                                 enable.                                                          *
 *                                                                                                  *
 *  Returns :   NOERROR       -> Point Of Load enabled successful.                                  *
 *              ERROR         -> Error enabling the POL                                             *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
unsigned char enablepol_function2(unsigned char direction)
{
    pthread_mutex_lock(&mylock);
    
    unsigned char booleanpol;
    int nargs = 1;
    int anyerror;
    unsigned short lon_data = 1;
    unsigned char data[1];
    data[0] = 1 ;                 /*means enable*/
    MCSCommand actual =  MCS_PAYLOAD_ARDUINO_WRITE_I2C ;
    unsigned char *argumentos ;
    argumentos[0] = direction;
    /*argumentos[0]= pkt_in->data ;*/
    MCSPacket *pktToSend, *pktToReceive;
    
    booleanpol = ispolenabled_function2( direction );
    
    if(booleanpol == (unsigned char)0){  /*if the pol is disabled I can enable it .*/

        pktToSend = mcs_create_packet(actual, nargs, argumentos, lon_data, data);
        anyerror = sdb_send_sync(pktToSend, &pktToReceive);
        
    
       if(anyerror == ERROR_INT){
        
          printf("Error with the sdb\n");
          return (unsigned char)ERROR ;
        
       }
       if(pktToReceive->type != MCS_TYPE_OK_DATA){
        
           printf(" Error in the received sdb packet\n");
           return (unsigned char)ERROR ;
        
       }
        if(pktToReceive->type == MCS_TYPE_OK_DATA && anyerror != ERROR_INT){
        
            /*if ( pkt_out->data= pktToReceive->data ){*/
            
                return (unsigned char)NOERROR ;
         
            /*}*/
    
        }
    }
    else{
        
        printf("This POL has arleardy been enabled");
        return (unsigned char)NOERROR ;
        
    }
    
    pthread_mutex_unlock(&mylock);

}
/* DISABLEPOL_FUNCTION FUNCTION *********************************************************************
 *  Name    :   disablepol_function                                                                 *
 *  Descr.  :   One-shot function to enable POL.                                                    *
 *                                                                                                  *
 *  Args.   :   pkt_in           MCSPACKET with the direction of the POL that we want to disable    *
 *              pkt_out          The functions saves in this MCSPACKEt wether or not the disabling  *
 *                               of the POL was succesful with the message ERROR or NOERROR         *
 *  Returns :   void                                                                                *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
void disablepol_function( MCSPacket *pkt_in , MCSPacket **pkt_out)
{
    unsigned char command;      /* Action sent to MicroController.*/
    command = MCS_MESSAGE_DISABLEPOL;  /*Initialize the specific command.*/
    
    unsigned char direction;
    int result;
    
    direction = pkt_in->data[0] ;
    
    result = disablepol_function2(direction);
    
    (*pkt_out)->data[0] = (unsigned char)result ;
    
}
/* DISABLEPOL_FUNCTION FUNCTION *********************************************************************
 *  Name    :   disablepol_function                                                                 *
 *  Descr.  :   One-shot function to disable POL.                                                   *
 *                                                                                                  *
 *  Args.   :   direction          unsigned char with the direction of the POL that we want to      *
 *                                 disable                                                          *
 *                                                                                                  *
 *  Returns :   NOERROR       -> Point Of Load disabled successfully.                               *
 *              ERROR         -> Error disabling the POL                                            *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/

unsigned char disablepol_function2( unsigned char direction)
{
    pthread_mutex_lock(&mylock);
    
    unsigned char booleanpol;
    
    booleanpol = ispolenabled_function2(direction);
    
    if(booleanpol == (unsigned char)1){  /*if the pol is enabled I can disable it*/
        
        int nargs= 1 ;
        int anyerror=0 ;
        unsigned short lon_data= 1;
        unsigned char data[1];
        data[0] = 0 ;                 /*means disable*/
        MCSCommand actual =  MCS_PAYLOAD_ARDUINO_WRITE_I2C ;
        unsigned char *argumentos;
        argumentos[0] = direction;
        /*argumentos[0]= direction ;     before it was pkt_in->data*/
        MCSPacket *pktToSend, *pktToReceive;
        pktToSend= mcs_create_packet(actual, nargs, argumentos, lon_data, data);
        anyerror = sdb_send_sync(pktToSend, &pktToReceive);
        
        
        if(anyerror == ERROR_INT){
            
            printf("Error with the sdb\n");
            return (unsigned char)ERROR ;
            
        }
        if(pktToReceive->type != MCS_TYPE_OK_DATA){
            
            printf("Error in the received sdb packet\n");
            return (unsigned char)ERROR ;
            
        }
        if((pktToReceive->type == MCS_TYPE_OK_DATA) && (anyerror != ERROR_INT)){
            
           /* if ( pkt_out->data= pktToReceive->data ){*/
                
                return (unsigned char)NOERROR ;
                
            /*}*/
            
        }
    }
    else{
        
        printf("This POL has arleardy been disabled");
        return (unsigned char)NOERROR ;
        
    }
    
    pthread_mutex_unlock(&mylock);
    
}

/* ISPOLENABLED_FUNCTION FUNCTION *******************************************************************
 *  Name    :   ispolenabled                                                                        *
 *  Descr.  :   One-shot function to check if a Point Of Load is enabled.                           *
 *                                                                                                  *
 *  Args.   :   pkt_in           MCSPACKET with the direction of the POL that we want to check if   *
 *                               it is enabled or not                                               *
 *              pkt_out          The function saves in this MCSPACKET if the POL is enabled or not  *
 *                                                                                                  *
 *  Returns :   void                                                                                *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
void ispolenabled_function(MCSPacket *pkt_in , MCSPacket **pkt_out) // 0 disable 1 enable  
{
    
    unsigned char command;      /* Action sent to MicroController.*/
    command = MCS_MESSAGE_ISPOL ;  /* Initialize the specific command.*/
    unsigned char direction;
    unsigned char booleanpol;
    
    direction = pkt_in->data[0] ;
    
    booleanpol = ispolenabled_function2(direction);
    
    (*pkt_out)->data[0] = (unsigned char)booleanpol ;

    
}
/* ISPOLENABLED_FUNCTION FUNCTION *******************************************************************
 *  Name    :   ispolenable                                                                         *
 *  Descr.  :   One-shot function to check if a Point Of Load is enabled.                           *
 *                                                                                                  *
 *  Args.   :   direction          unsigned char with the direction of the pol that we want to      *
 *                                 check if it is enabled or not.                                   *
 *                                                                                                  *
 *  Returns :   (unsigned char)1        -> If Point Of Load is enabled.                             *
 *              (unsigned char)0        -> If Point Of Load is not enabled.                         *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
unsigned char ispolenabled_function2(unsigned char direction){
    
    int nargs = 1;
    int anyerror = 0;
    unsigned short lon_data = 1;
    unsigned char data[1];
    data[0] = 'B' ;                           
    MCSCommand actual =  MCS_PAYLOAD_ARDUINO_WRITE_I2C;
    unsigned char *argumentos ;
    argumentos[0] = (unsigned char) direction; /*POL that we want to check if it is enabled*/
    /*argumentos[0]= pkt_in->data ;*/
    MCSPacket *pktToSend , *pktToReceive;
    pktToSend = mcs_create_packet(actual, nargs, argumentos, lon_data, data);
    anyerror = sdb_send_sync(pktToSend, &pktToReceive);
    
    
    if(anyerror == ERROR_INT){
        
        printf("Error with the sdb\n");
        return ERROR ;
        
    }
    if(pktToReceive->type != MCS_TYPE_OK_DATA){
        
        printf("Error in the received sdb packet\n");
        return ERROR ;
    }
    
    if((pktToReceive->type == MCS_TYPE_OK_DATA) || (anyerror != ERROR_INT)){
        
        return (unsigned char)pktToReceive->data[0];
        
    }
    
}


/* TEMPERATURE_FUNCTION FUNCTION ********************************************************************
 *  Name    :   temperature_function                                                                *
 *  Descr.  :   One-shot function of temperature. Returns the temperature of the selected POL/SENSOR*
 *                                                                                                  *
 *  Args.   :   pkt_in           MCSPACKET with the direction of the POL/SENSOR that we want        *
 *                               to read inside the data field                                      *
 *              pkt_out          The functions saves the required temperature inside the data of    *
 *                               this MCSPACKET                                                     *
 *                                                                                                  *
 *  Returns :   void                                                                                *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
void temperature_function(MCSPacket *pkt_in , MCSPacket **pkt_out_def)
{
            
    unsigned char command;      /*Action sent to MicroController*/
    command = MCS_STATE_TEMPERATURE;  /*Initialize the specific command*/
            
    unsigned char direction;
    unsigned char temperature;
            
    direction = pkt_in->data[0] ;
            
    temperature = temperature_function2(direction);
            
            
    (*pkt_out_def)->args[0] = SENSOR;
            
            
    (*pkt_out_def)->data_size = 1;
            
            
    (*pkt_out_def)->data[0] = temperature;
            
            
}
/* TEMPERATURE_FUNCTION FUNCTION ********************************************************************
 *  Name    :   temperature_function                                                                *
 *  Descr.  :   One-shot function of temperature. Returns one measure (unsigned char) of the        *
 *              selected POL/SENSOR                                                                 *
 *                                                                                                  *
 *  Args.   :   direction          unsigned char with the direction of the sensor that we want to   *
 *                                 read                                                             *
 *                                                                                                  *
 *  Returns :   unsigned char   -> Temperature of the selected POL/SENSOR                           *
 *              ERROR           -> Error in the function.                                           *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
        
unsigned char temperature_function2( unsigned char direction )
{
            
    MCSCommand actual = MCS_PAYLOAD_ARDUINO_READ_I2C;
    unsigned char *argumentos;
    unsigned short lon_data = 0;
    unsigned char *data;
    int lonpkt ; 
    int *datapktsensor ;
    data = NULL ;
    int nargs = 2 ;
    int anyerror;
    argumentos = malloc(2*sizeof(unsigned char));
    argumentos[0] =  (char)direction;  /*the PIN that we want to read*/
    argumentos[1] = 'T';     /*Means Temperature*/
    MCSPacket *pktToSend, *pktToReceive;
    pktToSend = mcs_create_packet(actual, nargs, argumentos, lon_data, data);
    anyerror = sdb_send_sync(pktToSend, &pktToReceive);
    
    if(anyerror == ERROR_INT){
        
        printf("Error with the sdb\n");
        return (unsigned char)ERROR;
                
    }
    if(pktToReceive->type != MCS_TYPE_OK_DATA){
        
        printf("Error in the received sdb packet\n");
        return (unsigned char)ERROR ;
    }
    
    if((pktToReceive->type == MCS_TYPE_OK_DATA) || (anyerror != ERROR_INT)){
        
        lonpkt = pktToReceive->data_size;
        datapktsensor = malloc(lonpkt*1*sizeof(int));
        
        return (unsigned char)pktToReceive->data[0];
        
    }
            
}
        



/* IRRADIANCE_FUNCTION FUNCTION *********************************************************************
 *  Name    :   irradiance_function                                                                 *
 *  Descr.  :   One-shot function of temperature. Returns the temperature of the selected POL/SENSOR*
 *                                                                                                  *
 *                                                                                                  *
 *  Args.   :   pkt_in           MCSPACKET with the direction of the POL/SENSOR that we want        *
 *                               to read inside the data field                                      *
 *              pkt_out          The functions saves the required irradiance inside the data of     *
 *                               this MCSPACKET                                                     *
 *                                                                                                  *
 *  Returns :   void                                                                                *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
void irradiance_function(MCSPacket *pkt_in , MCSPacket **pkt_out_def)
{
    
    unsigned char command;      /*Action sent to MicroController*/
    command = MCS_STATE_IRRADIANCE;  /*Initialize the specific command*/

    unsigned char direction;
    unsigned char irradiance;
    
    direction = pkt_in->data[0] ;
    
    irradiance = irradiance_function2(direction);
    
            
    (*pkt_out_def)->args[0] = SENSOR;
            
            
    (*pkt_out_def)->data_size = 1;
            
            
    (*pkt_out_def)->data[0] = irradiance; 
    
            
}
    
/* TEMPERATURE_FUNCTION FUNCTION ********************************************************************
 *  Name    :   temperature_function                                                                *
 *  Descr.  :   One-shot function of irradiance. Returns one measure (unsigned char) of the         *
 *              selected POL/SENSOR                                                                 *
 *                                                                                                  *
 *  Args.   :   direction          unsigned char with the direction of the sensor that we want to   *
 *                                 read                                                             *
 *                                                                                                  *
 *  Returns :   unsigned char   -> Temperature of the selected POL/SENSOR                           *
 *              ERROR           -> Error in the function.                                           *
 *                                                                                                  *
 *  Author  :   David Riudor.                                                                       *
 *  Remarks :   --                                                                                  *
 ****************************************************************************************************/
unsigned char irradiance_function2( unsigned char direction )
{

    MCSCommand actual = MCS_PAYLOAD_ARDUINO_READ_I2C;
    unsigned char *argumentos;
    unsigned short lon_data = 0;
    unsigned char *data;
    int *datapktsensor ;
    int lonpkt ; 
    data = NULL;
    int nargs = 2 ;
    int anyerror;
    argumentos= malloc(2*sizeof(unsigned char));
    argumentos[0] =  (char)direction ; /*the PIN that we want to read*/
    argumentos[1] = 'I' ;     /*Means Irradiance*/
    MCSPacket *pktToSend , *pktToReceive ;
    pktToSend = mcs_create_packet(actual, nargs, argumentos, lon_data, data);
    anyerror = sdb_send_sync(pktToSend, &pktToReceive);
        
    if(anyerror == ERROR_INT){
            
        printf("Error with the sdb\n");
        return (unsigned char)ERROR ;
            
    }
    if(pktToReceive->type != MCS_TYPE_OK_DATA){
            
        printf("Error in the received sdb packet\n");
        return (unsigned char)ERROR ;
    }
        
    if( pktToReceive->type == MCS_TYPE_OK_DATA || anyerror != ERROR_INT ){
            
        lonpkt = pktToReceive->data_size; 
            
        datapktsensor = malloc(lonpkt*1*sizeof(int));
            
        return (unsigned char)pktToReceive->data[0];
            
    }
    
}



