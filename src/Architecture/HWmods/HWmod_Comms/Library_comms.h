/*Include all the libraries*/

/*src/Includes*/
#include <sdb_group.h>
#include <abs.h>
#include <abs_test.h>
#include <mcs.h>
#include <sdb.h>

/*bin/Includes*/

/*bin/libraries*/


/*COMMAND OF HWMOD COMMS*/
#define COMMS			    102

#define COMMAND_RUN		    200
#define COMMAND_INIT		201
#define COMMAND_CHECK		202
#define COMMAND_HALT		203	

#define COMMAND_FLAG_RECEIVE 204
#define COMMAND_FLAG_TO_SEND_FILE 205	
/*define the error command*/
#define ERROR			    0xBB	
#define NOERROR			    0xAA
#define ERROR_INT		    -1    /* error sdb*/
#define NOERROR_INT		    0	
	
//size
#define MAX_SIZE_INFO_FRAME	256		/*Maximum size of the information(256bytes of MCS - 18 bytes of the rest of trama)...*/
#define DATA_SIZE_U_S		17		/*Size of the unnumbered frame(except UI) and supervision frame*/
#define DATA_SIZE_I_HAND	3		/*Size of the handshake packet*/

 
/*PARAMETERS OF FRAMES*/
#define FLAG 			    0x7E		/*Flag of the frame*/
#define direction1		    0xD1		/*Direction of the cubesat(for notify)*/
#define direction2		    0xD2
#define direction3		    0xD3
#define direction4		    0xD4
#define direction5		    0xD5
#define direction6		    0xD6 
#define direction7		    0xD7 
#define directionSSID		0xD8
#define PID			        0xD0

/*PARAMETERS OF THE TX/RX*/
#define DATA_RATE		     250		/*Data rate*/
/*Frequencies, check if the frequencies are 433 MHz*/
#define FREQ                 433

//define of the timeouts...
#define TIMEOUT_T_FRAME			10000000     /*Timeout for wait one frame. 10 seconds for this moment*/
#define TIMEOUT_N_DISC			3       /*Maximum number of DISC*/
#define TIMEOUT_N_REJ			3   /*Maximum number of REJ*/
#define TIMEOUT_N_I_HAND		10	/*Maximum number of I_HAND, it have more number of frames*/
#define TIMEOUT_N_I			    3	/*Maximum number of I*/
#define TIMEOUT_N_UA			3	/*Maximum number of UA*/
#define TIMEOUT_N_RNR			3	/*Maximum number of RNR*/
#define TIMEOUT_N_RR			3	/*Maximum number of RR*/

#define TIMEOUT				0xCC	/*The return in the receive frame*/

/*To timeout, the return defines of the wait function of SDB(...CONSULT TO MARC MARI)*/
#define CHECK_ARRIVE		1	/*When the check function return the OK(arrive the packet)*/
#define CHECK_NO_ARRIVE		0	/*When the check function return that not arrive the packet*/

/*STATES OF THE HWMOD COMMUNICATION*/
typedef enum { 
		STATE_CREATED,		/*The first state*/	
		STATE_INIT, 		/*State when the system is in init function*/
		STATE_CHECK, 		/*State when the system is in the check function*/
		STATE_RUN_LISTEN, 	/*State when the system is in the run function and listen frames(SABM) of connect*/
		STATE_RUN_CONNECT,	/*State when arrive one frame of the earth and makes the link(UA)*/
		STATE_RECEIVE_FILE,  /*State when the system is in the function receive file*/
		STATE_SEND_FILE ,    /*State when the system is in the function send file*/
		STATE_HALT		/*State when the system is in the halt function*/
} states_hwmod;

/*TIMEOUTS: Maybe needs more, but for now not.*/
typedef struct timeouts {
	unsigned int t_frame;			/*Timeout of the response one frame, or the next frame of information*/
	unsigned int n_disc;			/*Maximum number of DISC for do the disconnect*/
	unsigned int n_rej;			/*Maximum number of REJ for reject the frames of I*/
	unsigned int n_i_hand;			/*Maximum number of handskae packet for reject one frame of I*/
	unsigned int n_i;			/*Maximum number of I for reject to send */
	unsigned int n_ua;			/*Maximum number of UA for response SABM or DISC*/
	unsigned int n_rnr;			/*Maximum number of RNR*/
	unsigned int n_rr;			/*Maximum number of RR*/
} timeouts;

/*Define commands to ARDUINO...*/
#define 	CONFIGURE	0x09	/*Configure default parameters of ARDUINO*/	
#define 	CHANGE_X	0x0C	/*Configure specific parameters of ARDUINO*/
#define 	TRANSMIT	0x0A	/*Say to Arduino transmit a frame */
#define 	RECEIVE		0x0B	/*Say to Arduino receive a frame*/
#define     GIVE_ME_THE_PACKET  0x0E /*If the frame arrives, say to Arduino send the frame to the HWmod*/
#define		FCS_OK		0x00	/*FCS ok!*/
#define 	FCS_ERROR	0x02	/*FCS error!*/

#define		NO_DIRECT	0xA7	/*The direction is not of the Cubesat*/
#define 	DIRECT_OK	0xA8	/*The direction is the Cubesat*/

/*Define commands to Cubesat*/
#define		NO_SEND_FILE	0x56	/*Command of the Cubesat to say no send file*/
#define 	SEND_FILE	    0x57	/*Command of the Cubesat to say send file*/

/*destinies*/
#define 	ARDUINO		0xA0
#define 	BROADCAST	0xB0		

/*Define commands to say in the high level...*/

/*TYPES OF PACKETS/FRAMES*/
/*FRAME NO NUMBERED*/
/*Types of Unnumbered frames*/
#define		SABM	0x3F		/*For ask if can do the connection. */
#define		UA	    0x73		/*Unnumbered ACK. for response the SABM or DISC.IN ASCII 0x73*/
#define		DM	    0x1F		/*Disconnect Mode response. when the connection is not possible */
#define		FRMR	0x97		/*Frame Reject. Whem is not possible make the link and is not possible adjust*/
#define		UI 	    0x03		/*Unnumbered Information. Allow send information without numbered frame.*/
#define		DISC	0x53		/*Disconnect frame*/


/*Types of Supervision frames*/ 
#define		RR	    0x01		/*Reciever Ready. The frame have sent correct*/
#define		RNR	    0x05		/*Reciever is Not Ready. The frame have sent correct but can't recieve more frames.*/
#define		REJ	    0x09		/*Reject the frames since the number of the frame */
#define		SREJ	0x1D		/*Selective Reject. Reject the number of the frame*/

#define		I	    0xE0		/*Frame of information*/
#define		I_HAND	0xE1		/*Frame of handshake with the struct of information frame*/

#define		UNKNOWN	0xE2		/*Unknown packet*/

#define		FIRST_H	0xD2		/*The first handshake to make the connection*/
#define 	NO_F_H	0xD3





/*Structure of the frames*/
typedef struct frame {
	unsigned char directionSat[8];			/*Direction of sat */
	unsigned char directionGS[8];			/*Direction of Ground Station*/
	unsigned char control;				/*Define the type of frame(I, S or U) and the subtype*/
	unsigned char pid;				/*The protocol of the system(only to frame I)*/
	unsigned char information[MAX_SIZE_INFO_FRAME];	/*Field of information(only to frame I and UI)*/
	
} frame;

/*Structure of the file(obtain of the field information)*/
typedef struct file {
	unsigned char name;			/*The command of file */
	int  size_of_file;			/*The size of the file*/
	char data[MAX_SIZE_INFO_FRAME * 256];	/*Information of the file, PUT THE NUMBER OF FRAMES...*/
} file;

/*GLOBAL VARIABLES*/
states_hwmod state;         /*State of the system*/
timeouts timeout;           /*Timeouts of the system*/
frame frames;               /*Structure of the frame I*/
extern unsigned int freq;          /*To implement...*/
extern unsigned int protocol_delay;            /*Delay introduced to simulate propagation delays.To discuse...*/
extern unsigned int frame_delay;               /*Delay between iterative packet transmissions.To discuse...*/
extern unsigned int disc_repeat;       /*Amount of repetitions of frame DISC*/
extern unsigned int rej_repeat;        /*Amount of repetitions of frame REJ to receive*/
extern unsigned int i_hand_repeat;     /*Amount of repetitions of frame I_HAND to send*/
extern unsigned int i_repeat;          /*Amount of repetitions of I frame to send*/
extern unsigned int ua_repeat;         /*Amount of repetitions of frame UA*/
extern unsigned int rnr_repeat;        /*Amount of repetitions of frame RNR*/
extern unsigned int rr_repeat;         /*Amount of repetitions of frame RR*/
extern unsigned int datarate;          /*data rate*/
extern file file_received;         /*file which save the information of the frames*/
extern file file_send;             /*File which send the information with frames*/
extern unsigned char directionSat[8];      /*Direction of the Sat*/
extern unsigned char directionGS[8];       //Save the direction of the ground station 
extern unsigned char pid;          /*PID of the Information frame*/
extern unsigned char command_file;     /*Command of the data received*/
extern unsigned char first_handshake;      /*Check if the handshake packet is the first received*/
extern unsigned char type_of_frame;        /*The type of frame which arrive*/
extern unsigned int last_size_info;        /*The size of the last frame of information*/
extern unsigned int num_of_frames;     /*Number of packets to receive or send*/
extern unsigned char name_of_file;     /*The name file*/
extern unsigned int run_in_out;        /*If the variable is 1, do the run. If is 0 out of run*/
extern FILE *f;                        /*To save and take files in the base of data*/
extern unsigned int flag_receive_frame;         /*Flag to comunicate the HWmod that the frame has arrived or not. arrive=1, not arrive=0;*/
extern unsigned int flag_send_file;       /*Flag of the Cubesat to send one file*/
extern unsigned char wait_name_file;   /*If the Cubesat call the function "flag_to_send_file", The name of the file is saved in this variable for, in the case that there is in one process, not affect*/

/*FUNCTIONS*/
/*One shot functions*/
extern int main();
extern void init(MCSPacket *pkt_in, MCSPacket **pkt_out);
extern void check(MCSPacket *pkt_in, MCSPacket **pkt_out);
extern void run(MCSPacket *pkt_in, MCSPacket **pkt_out);
extern void halt(MCSPacket *pkt_in, MCSPacket **pkt_out);

extern void flag_receive(MCSPacket *pkt_in, MCSPacket **pkt_out);
extern void flag_to_send_file(MCSPacket *pkt_in, MCSPacket **pkt_out);

/*Functions to do the HWmod*/
extern unsigned char receive_file(unsigned char name_file, int number_frames, int last_info_frame);
extern unsigned char send_file(unsigned char name_of_file);
extern unsigned char analize_frame();
extern unsigned char send_frame(unsigned char type, unsigned char *info, int info_size);
extern unsigned char receive_frame(int size_data_info);





