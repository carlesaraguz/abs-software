/* ABS ******************************************************************************************
 *  File:   mcs_riudor.h                                                                            *
 *  Descr.: EPS Hardware Module library - Energy Managment.                                         *
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

#ifndef MCS_RIUDOR_H
#define MCS_RIUDOR_H

#ifdef __cplusplus
extern "C" {
#endif
/*** INCLUDE SECTION ********************************************************************************/

/*Standard C libraries*/
#include <stdlib.h>
#include <stdio.h>
#include "sdb_group.h"
#include "abs.h"
#include "abs_test.h"
#include "mcs.h"
#include "sdb.h"
#include <pthread.h>

/*** GLOBAL CONSTANTS *******************************************************************************/

/*define the error command*/
#define ERROR			255
#define NOERROR			0xAA
#define ERROR_INT		1    /*error sdb*/
#define NOERROR_INT		2

/*define length of the differents types of data*/                                     /*Not used yet*/
#define     VOLT_CHARS          4   /* Units + Tenths + Hundredths + Thousandths*/
#define     CUR_CHARS           4
#define     IRR_CHARS           4
#define     TEMP_CHARS          4
#define     SOC_CHARS           4

/*Rate of the i2c*/
#define RATE_I2C		         800

/*provisional arduino*/

#define MCS_PAYLOAD_ARDUINO_INIT_I2C 100
#define MCS_PAYLOAD_ARDUINO_UNINIT_I2C 101
#define MCS_PAYLOAD_ARDUINO_READ_I2C 102
#define MCS_PAYLOAD_ARDUINO_WRITE_I2C 103

/*Point of Load commands*/

#define  MCS_MESSAGE_ENABLEPOL   0x60    /*Enable a PoL*/
#define  MCS_MESSAGE_DISABLEPOL  0x70    /*Disable a PoL*/
#define  MCS_MESSAGE_ISPOL       0x50    /*Check the status of a PoL*/

/*Sensors commands*/

#define  MCS_STATE_TEMPERATURE   0x42    /*Get temperature data sensors*/
#define  MCS_STATE_IRRADIANCE    0x43    /*Get irradiance data sensors*/
#define  MCS_STATE_VOLTAGE       0x44    /*Get voltage data sensors*/
#define  MCS_STATE_CURRENT       0x45    /*Get current data sensors*/
#define  MCS_STATE_SOC           0x46    /*Get state of charge*/

/*Principal functions commands*/

#define  MCS_MESSAGE_CHECK       0x31    /*Check Function command*/
#define  MCS_MESSAGE_INIT        0x32    /*Init Function command*/
#define  MCS_MESSAGE_RUN         0x33    /*Run Function command*/
#define  MCS_MESSAGE_HALT        0x34    /*Halt Function command*/
/*Other commands*/

#define     FIRST_POL_DIRECTION    0x00    /*invented*/
#define     LAST_POL_DIRECTION     0x0A    /*invented*/
#define     FIRST_SENSOR_DIRECTION 0x0B    /*invented*/
#define     LAST_SENSOR_DIRECTION  0x0C    /*invented*/
#define     POL                    0x0D    /*invented  In order to differentiate between pol and sensor in the OSF*/
#define     SENSOR                 0x0E    /*invented  In order to differentiate between pol and sensor in the OSF*/
#define     NUM_PAYLOADS           8
#define     NUM_SENSORS            4
#define     NUM_POL                4
#define     MAX_VOLTAGE_BATTERY    0x90 
#define     MIN_VOLTAGE_BATTERY    0x20
#define     MEDIUM_VOLTAGE_BATTERY 0x40
#define     FIRST_LESS_IMPORTANT   0x03
#define     SECOND_LESS_IMPORTANT  0x04


/*TASK NAMES*/
/*Init and check tasks*/
#define     TASK_NAME_INIT              "hwmod_eps_init"              /* Init task name.*/
#define     TASK_NAME_CHECK             "hwmod_eps_check"             /*Check task name*/
#define     TASK_NAME_RUN               "hwmod_eps_run"               /*Check task name*/
#define     TASK_NAME_HALT              "hwmod_eps_halt"              /*Check task name*/


/*Data sensors tasks*/

#define     TASK_NAME_GET_VOLTAGE       "hwmod_eps_getv"    /*Get voltage data sensor task name*/
#define     TASK_NAME_GET_CURRENT       "hwmod_eps_getc"    /*Get current data sensor task name*/
#define     TASK_NAME_GET_TEMPERATURE   "hwmod_eps_gett"    /*Get temperature data sensor task name*/
#define     TASK_NAME_GET_IRRADIANCE    "hwmod_eps_geti"    /*Get irradiance data sensor task name*/
#define     TASK_NAME_GET_SOC           "hwmod_eps_gets"    /*Get soc (state of charge) task name*/

// PoL tasks:
#define     TASK_NAME_ENABLE_POL        "hwmod_eps_enpol"       /*Enable PoL task name*/
#define     TASK_NAME_DISABLE_POL       "hwmod_eps_dispol"      /*Disable PoL task name*/
#define     TASK_NAME_ISPOL_ENABLE      "hwmod_eps_ispol"       /*Status PoL task name*/


/*** TYPEDEFS ***************************************************************************************/

/*
 typedef enum MCSCommand{

        MCS_STATE_TEMPERATURE = 0x42,
        MCS_STATE_IRRADIANCE = 0x43,
        MCS_STATE_VOLTAGE = 0x44,
        MCS_STATE_CURRENT = 0x45,
        MCS_STATE_SOC = 0x46,

        MCS_MESSAGE_ENABLEPOL = 0x60,
        MCS_MESSAGE_DISABLEPOL = 0x70,
        MCS_MESSAGE_ISPOL = 0x50,

        MCS_MESSAGE_CHECK = 0x31,
        MCS_MESSAGE_INIT = 0x32,
        MCS_MESSAGE_RUN = 0x33,
        MCS_MESSAGE_HALT = 0x34,


    }MCSCommand;

*/

 /*
    typedef enum MCSType {

    MCS_TYPE_MESSAGE    = 0,
    MCS_TYPE_STATE      = 1,
    MCS_TYPE_PAYLOAD    = 2,
    MCS_TYPE_OK         = 253,
    MCS_TYPE_OK_DATA    = 254,
    MCS_TYPE_ERR        = 255,

    }MCSType;
*/
/*    typedef enum MCSCommand{

    arduino_i2c_read    = 88,
    arduino_i2c_write   = 89,
    arduino_init_i2c    = 100,
    arduino_uninit_i2c  = 101,

    }MCSCommand;
*/
/*
    typedef struct MCSPacket {

    enum MCSType type;
    unsigned short cmd;
    unsigned short nargs;
    unsigned char *args;
    char *dest;
    unsigned short data_size;
    unsigned char *data;

    } MCSPacket;

    typedef struct thr_param_t {

    int id;
    char *cadena;

    } thr_param_t;
 */

/*** GLOBAL VARIABLES *******************************************************************************/

extern int haltFlag ;
extern int Pol_enabled[NUM_PAYLOADS] ;
extern pthread_mutex_t mylock;

/*** FUNCTIONS **************************************************************************************/

// HWmod interface:
extern  void    init_function(MCSPacket *pkt_in , MCSPacket **pkt_out);           /*Setup function*/
extern  void    run_function(MCSPacket *pkt_in , MCSPacket **pkt_out);            /*Run function*/
extern  void    halt_function(MCSPacket *pkt_in , MCSPacket **pkt_out);           /*Halt function*/
extern  void    check_function(MCSPacket *pkt_in , MCSPacket **pkt_out);          /*Initial check function*/

extern 	void	voltage_function(MCSPacket *pkt_in , MCSPacket **pkt_out);		  /*Get voltage function*/
extern 	void    current_function(MCSPacket *pkt_in , MCSPacket **pkt_out);		  /*Get current function*/
extern 	void	temperature_function(MCSPacket *pkt_in , MCSPacket **pkt_out);	  /*Get temperature function*/
extern 	void 	irradiance_function(MCSPacket *pkt_in , MCSPacket **pkt_out);	  /*Get irradiance function*/
extern 	void	soc_function(MCSPacket *pkt_in , MCSPacket **pkt_out);	          /*Get SoC (state of charge) function*/
extern 	void 	enablepol_function (MCSPacket *pkt_in , MCSPacket **pkt_out);	  /*Enable specific PoL*/
extern 	void 	disablepol_function (MCSPacket *pkt_in , MCSPacket **pkt_out);	  /*Disable specific PoL*/
extern 	void 	ispolenabled_function (MCSPacket *pkt_in , MCSPacket **pkt_out);  /*Status of specific PoL*/

extern 	unsigned char	voltage_function2(unsigned char direction);		  /*Get voltage function*/
extern 	unsigned char   current_function2(unsigned char direction);		  /*Get current function*/
extern 	unsigned char	temperature_function2(unsigned char direction);	  /*Get temperature function*/
extern 	unsigned char 	irradiance_function2(unsigned char direction);	  /*Get irradiance function*/
extern 	unsigned char 	enablepol_function2(unsigned char pol);		      /*Enable specific PoL*/
extern 	unsigned char 	disablepol_function2(unsigned char pol);	      /*Disable specific PoL*/
extern 	unsigned char 	ispolenabled_function2(unsigned char pol);	      /*Status of specific PoL*/
extern  unsigned char   soc_function2();

#ifdef __cplusplus
}
#endif

#endif /* MCS_RIUDOR_H */

