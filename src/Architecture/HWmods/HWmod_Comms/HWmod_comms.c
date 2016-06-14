#include "HWmod_comms_library.h"


//GLOBAL VARIABLES
states_hwmod state;			//State of the system
timeouts timeout;			//Timeouts of the system
frame frames;				//Structure of the frame I
unsigned int freq;			//To implement...
unsigned int protocol_delay;         	//Delay introduced to simulate propagation delays.To discuse...
unsigned int frame_delay;           	//Delay between iterative packet transmissions.To discuse...
unsigned int disc_repeat;		//Amount of repetitions of frame DISC
unsigned int rej_repeat;		//Amount of repetitions of frame REJ to receive
unsigned int i_hand_repeat;		//Amount of repetitions of frame I_HAND to send
unsigned int i_repeat;			//Amount of repetitions of I frame to send
unsigned int ua_repeat;			//Amount of repetitions of frame UA
unsigned int rnr_repeat;		//Amount of repetitions of frame RNR
unsigned int rr_repeat;			//Amount of repetitions of frame RR
unsigned int datarate;			//data rate
file files;				//file which save the information of the frame
unsigned char directionSat[8];		//Direction of the Sat
unsigned char directionGS[8];		//Save the direction of the ground station 
unsigned char pid;			//PID of the Information frame
unsigned char command_file;		//Command of the data received
unsigned char first_handshake;		//Check if the handshake packet is the first received
unsigned char type_of_frame;		//The type of frame which arrive
unsigned int last_size_info;		//The size of the last frame of information
unsigned int num_of_frames;		//Number of packets to receive or send
unsigned char name_of_file;		//The name file



//MAIN
int main()
{
	int noerror;
	char *name;
	int i;

	noerror = NOERROR;
	i = 0;
	//Do the connect of the SDB
	name = malloc(5 * sizeof(char));
	name = "COMMS";
	noerror = sdb_connect(name, SDB_GROUP_HWMOD);
	if(noerror == ERROR) {
		printf("Error in the connection with the SDB");
		return ERROR;
	}
	if(noerror == NOERROR) {
		return NOERROR;
	}

	//Put the register callbacks of the SDB
	noerror = sdb_register_callback(COMMAND_CHECK, check);
	noerror = sdb_register_callback(COMMAND_INIT, init);
	noerror = sdb_register_callback(COMMAND_RUN, run);
	noerror = sdb_register_callback(COMMAND_HALT, halt);
}

//Function init
void init(MCSPacket *pkt_in, MCSPacket **pkt_out)
{
	state = STATE_INIT;
	MCSPacket *pkt_in_ard, *pkt_out_ard;
	int noerror;
	unsigned char dest;
	unsigned char command;
	dest = ARDUINO;
	command = CONFIGURE;

init_start:
	//Said to ARDUINO configure default parameters
	pkt_in_ard = mcs_create__packet__with_dest(COMMS, &dest, 1, &command, 0, NULL);//...
	noerror = sdb_send_sync(pkt_in_ard, &pkt_out_ard);
	if(noerror != NOERROR) goto init_start;
	//Configure timeouts
	timeout.t_frame = TIMEOUT_T_FRAME; //To discuse...
	timeout.n_disc = TIMEOUT_N_DISC;
	timeout.n_rej = TIMEOUT_N_REJ;
	timeout.n_i_hand = TIMEOUT_N_I_HAND;
	timeout.n_i = TIMEOUT_N_I;	
	timeout.n_ua = TIMEOUT_N_UA;
	timeout.n_rnr = TIMEOUT_N_RNR;
	timeout.n_rr = TIMEOUT_N_RR;
	//Implement the frequency...
	freq = 433;
	datarate = 250;
	//Initialize the global variables
	protocol_delay = 0; 
	frame_delay = 0;
	disc_repeat = 0;
	rej_repeat = 0;
	i_hand_repeat = 0;
	i_repeat = 0;
	ua_repeat = 0;
	rnr_repeat = 0;
	rr_repeat = 0;
	//Intilialize the default parameters of the frame
	directionSat[8] = {direction1, direction2, direction3, direction4, direction5, direction6, direction7, directionSSID};
	pid = PID; //For default
	frames.pid = pid; //For default
	first_handshake = NO_F_H;
	last_size_info = 0;
	num_of_frames = 0;
	name_of_file = NULL;		
}

//Function check
void check(MCSPacket *pkt_in, **pkt_out)
{
	int error;
	int i;
	char *name;

	printf("Check the system");
}



//CHANGE THINGS!!It need add the timeouts!!
//Function run
void run(MCSPacket *pkt_in, MCSPacket **pkt_out)
{
	MCSPacket *pkt_in_ard, *pkt_out_ard;
	MCSPacket *pkt_in_sat, *pkt_out_sat;
	unsigned char dest, command;
	unsigned char res_send, res_receive; //Result of the send and receive frame or file 
	unsigned char fcs;
	int i, noerror;
	unsigned char *data;
	
	i = 0;
	//After init put the system in listening
	if(state == STATE_INIT) {
		state = STATE_RUN_LISTEN;
	}//else{	pkt_out->args = ERROR; } //Look the commands...
	
	
run_start:
	while(1) {
		state = STATE_RUN_LISTEN; //After close one communication, put in state listening
		first_handshake = FIRST_H;

		//RECEIVE(LISTEN) THE SABM TO CONNECT AND CONTROL ERRORS
		printf("Listen SABM");
		res_receive = receive_frame();
		//Check the type of packets and possible errors. If there are, send DM
		if(res_receive == FCS_ERROR) {
			res_send = send_frame(DM, NULL, 0);
			goto run_start;
		}else if(res_receive == NO_DIRECT) {
			goto run_start;
		}else if(res_receive == UNKNOWN) {
			res_send = send_frame(DM, NULL, 0);
			goto run_start;
		}else if(type_of_frame != SABM) {
			res_send = send_frame(DM, NULL, 0);
			goto run_start;
		}else { res_send = send_frame(UA, NULL, 0); }


		//while don't receive the handshake packet the connection for part of GS is not done!!
		while(type_of_frame != I_HAND && res_receive != NOERROR) { 
			res_receive = receive_frame(); //Receive the frame
			//Check errors
			if(res_receive == FCS_ERROR) {
				res_send = send_frame(REJ, NULL, 0);
				i_hand_repeat++;
			}else if(res_receive == UNKNOWN) {
				res_send = send_frame(REJ, NULL, 0);
				i_hand_repeat++;
			}else if(type_of_frame != I_HAND) {
				res_send = send_frame(REJ, NULL, 0);
				i_hand_repeat++;
			}
			//If the repeat of send handshake packet arrived, close the link and go to run start
			if(i_hand_repeat == timeout.n_i_hand) {
				printf("The handshake repeat arrive to the maximum number, close the link");
				i_hand_repeat = 0;
				goto run_start;
			}if(type_of_frame == I_HAND && res_receive == NOERROR) {
				i_hand_repeat = 0;
			}
		}
		printf("The connection was successfully");
		

			
		//STATE CONNECT, READY TO RECEIVE INFORMATION
		
		state = STATE_RUN_CONNECT;
		//While the state is connect, the Cubesat and the ground station can exchange information
		while(state == STATE_RUN_CONNECT || state == STATE_RUN_SEND_FILE || state == STATE_RUN_RECEIVE_FILE) {			

			//After receive the first file is possible receive one I_HAND or DISC
			if(first_handshake != FIRST_H) {
				res_receive = receive_frame();
				if(res_receive == FCS_ERROR) {
					res_send = send_frame(REJ, NULL, 0);
					i_hand_repeat++;
					disc_repeat++;
				}else if(res_receive == UNKNOWN) {
					res_send = send_frame(REJ, NULL, 0);
					i_hand_repeat++;
					disc_repeat++;
				}else if(type_of_frame != I_HAND && type_of_frame != DISC) {
					res_send = send_frame(REJ, NULL, 0);
					i_hand_repeat++;
					disc_repeat++;
				}
				if(i_hand_repeat == timeout.n_i_hand || disc_repeat == timeout.n_disc) {
					printf("The handshake or DISC repeat arrive to the maximum number, close the link");
					i_hand_repeat = 0;
					disc_repeat = 0;
					goto run_start;
				}
				
			}
			i_hand_repeat = 0;
			disc_repeat = 0;
			//If the received frame is I_HAND, receive the file
			if(type_of_frame == I_HAND) {
				//Receive the file
				first_handshake = NO_F_H;
				res_receive = receive_file(name_of_file, num_of_frames, last_size_info);
				if(res_receive == ERROR) {
					printf("The receive file has an error, close the link");
					goto run_start;
				}
				//Notify to Cubesat the name of the receive file(use the field data to put in the field of args of MCSPacket)
				data = malloc(1 * (unsigned char));
				data[0] = name_of_file;
				dest = BROADCAST;
				pkt_in_sat = mcs_create_packet_with_dest(COMMS, &dest, 1, data, 0, NULL);//...
				noerror = sdb_send_sync(pkt_in_sat, &pkt_out_sat);//...
				//The args of MCSPacket to notify if it need send a file is: 1.SEND/NOSEND FILE, 2. name of file
				if(pkt_out_sat->args[0] == SEND_FILE) {
					name_of_file = pkt_out_sat->args[1];
					res_send = send_file(name_of_file);
					if(res_send == ERROR) {
						printf("The send file has an error, close the link");
						goto run_start;
					}
				}
			//If the frame is DISC, disconnect and go to run start(state listen)
			}else if(type_of_frame == DISC) {
				res_send = send_frame(UA, NULL, 0);
				goto run_start;
			}		
			
		}
		
	}
	//When the function finish, need return something
	/*if(true) {
		pkt_out->args = NOERROR; //Look the commands...
	}*/	
			
			
			
			

}

//Function halt
void halt(MCSPacket *pkt_in, MCSPacket **pkt_out)
{
	int error;
	error = NOERROR;

	error = sdb_disconnect();
	/*if(error == ERROR) {
		pkt_in->args = ERROR;
	}

	sdb_uninit();
	if(error = NOERROR) {
		pkt_in->args = NOERROR;
	}*/
}

unsigned char recieve_file(unsigned char name_file, int number_frames, int last_info_frame)
{
//After do the connection, receive the information and response
//Maybe, the receive packet is the type of DISC
	int i, j; 
	unsigned char noerror; //For return
	int n_error; //For amount of number of errors
	unsigned char res_send_frame; //The result of the function send_frame
	unsigned char res_receive_frame; //The result of the function receive_frame

	i = 0;
	j = 0;
	noerror = NOERROR;	
	n_error = 0;
	res_receive_frame = ERROR;
	
	res_send_frame = send_frame(RR, NULL, 0); //Send the response of I_HAND
	//While the type of frame is not information or has an error send one REJ
	while(type_of_frame != I || res_receive_frame != NOERROR) { 
		res_receive_frame = receive_frame(); //Receive the frame
		if(res_receive_frame == FCS_ERROR) {
			res_send_frame = send_frame(REJ, NULL, 0);
			rej_repeat++;
		}else if(res_receive_frame == UNKNOWN) {
			res_send_frame = send_frame(REJ, NULL, 0);
			rej_repeat++;
		}else if(type_of_frame == I_HAND) {
			res_send_frame = send_frame(RR, NULL, 0);
			rr_repeat++;
		}
		if(rej_repeat == timeout.n_rej || rr_repeat == timeout.n_rr) {
			printf("The response of handshake or reject frames arrive to the maximum number, close the link");
			rr_repeat = 0;
			rej_repeat = 0;
			noerror = ERROR;
			type_of_frame = I; //This is for out of the loop
			res_receive_frame = NOERROR; //This is for out of the loop
			//How the noerror = ERROR, it doesn't access to the loop and goes directly to return the error
		}
	}
	rr_repeat = 0;
	rej_repeat = 0;
	//While don't finish the receive frames and there isn't an error run.
	while(i < number_frames && noerror == NOERROR) {
		//Check if the arrived frame is the last frame
		if(i == (num_of_frames - 1)) {

			/*This is for the case of there are only one frame of information
			and the first frame of information has arrived already*/
			if(number_frames > 1) {
				res_receive_frame = receive_frame(); //Receive the frame
			}

			//Control of errors
			//If the reject repeat arrive to the maximum number return error
			if(rej_repeat == timeout.n_rej) {
				printf("The REJ arrive to his maximum number");
				rej_repeat = 0;
				noerror = ERROR;
			//If the receive frame has an fcs error send REJ
			}else if(res_receive_frame == FCS_ERROR) {
				res_send_frame = send_frame(REJ, NULL, 0);
				rej_repeat++;
			//If the receive frame has an no correct direction send REJ
			}else if(res_receive_frame == NO_DIRECT) {
				res_send_frame = send_frame(REJ, NULL, 0); //??? why it can happen??
				rej_repeat++;
			//If the receive frame has an unknown frame send REJ
			}else if(res_receive_frame == UNKNOWN) {
				res_send_frame = send_frame(REJ, NULL, 0); //??? why it can happen??
				rej_repeat++;
			//If the receive frame is not information frame send REJ
			}else if(type_of_frame != I) {
				res_send_frame = send_frame(REJ, NULL, 0); //??? why it can happen??
				rej_repeat++;
			//If the receive frame is ok, send RR and put the information in the field of data of file
			}else if(res_receive_frame == NOERROR && type_of_frame == I) {
				rej_repeat = 0;
				for(j = 0; j < last_size_info; j++) {
					files->data[j + i * MAX_SIZE_INFO_FRAME] = frame.information[j];
				}
				files.name = name_of_file; //Put the name in the struct of file
				files.size_of_file = (number_frames - 1) * MAX_SIZE_INFO_FRAME + last_size_info; //Put the size of file
				res_send_frame = send_frame(RNR, NULL, 0); //Send the RNR to conclude the receive file
				i++;
			}
		//Check the first information frame(it don't need the 
		}else{
			//This is for the first information frame, because it has arrived already
			if(i != 0) {
				res_receive_frame = receive_frame(); //Receive the frame
			}
			//Control of errors
			//If the reject repeat arrive to the maximum number return error
			if(rej_repeat == timeout.n_rej || rr_repeat == timeout.n_rr) {
				printf("The REJ or RR arrive to his maximum number");
				rr_repeat = 0;
				rej_repeat = 0;
				noerror = ERROR;
			//If the receive frame has an fcs error send REJ
			}else if(res_receive_frame == FCS_ERROR) {
				res_send_frame = send_frame(REJ, NULL, 0);
				rej_repeat++;
			//If the receive frame has an no correct direction send REJ
			}else if(res_receive_frame == NO_DIRECT) {
				res_send_frame = send_frame(REJ, NULL, 0); //??? why it can happen??
				rej_repeat++;
			//If the receive frame has an unknown frame send REJ
			}else if(res_receive_frame == UNKNOWN) {
				res_send_frame = send_frame(REJ, NULL, 0); //??? why it can happen??
				rej_repeat++;
			//If the receive frame is not information frame send REJ
			}else if(type_of_frame != I) {
				res_send_frame = send_frame(REJ, NULL, 0); //??? why it can happen??
				rej_repeat++;
			//If the receive frame is ok, send RR and put the info in the field of the data of file
			}else if(res_receive_frame == NOERROR && type_of_frame == I) {
				rej_repeat = 0;
				for(j = 0; j < MAX_SIZE_INFO_FRAME; j++) {
					files->data[j + i * MAX_SIZE_INFO_FRAME] = frame.information[j];
				}
				res_send_frame = send_frame(RR, NULL, 0);
				i++;
			}else if(res_receive_frame == NOERROR && type_of_frame == REJ) {
				rr_repeat++;
				res_send_frame = send_frame(RR, NULL, 0);
			}
			
		}
	}
	return noerror;
}

unsigned char send_file(unsigned char name_of_file)
{
//Send the file with the name name_of_file, the last size of information frame to the ground station
//Return ERROR or NOERROR
	unsigned char noerror;
	unsigned char res_send_frame, res_receive_frame;
	int i, j;
	unsigned char *data;

	i = 0;
	j = 0;
	noerror = NOERROR;

	//GO TO TAKE THE FILE...

	//Number of frames and the size of the last frame
	num_of_frames = files.size_of_file / MAX_SIZE_INFO_FRAME; 
   	last_size_info = files.size_of_file % MAX_SIZE_INFO_FRAME; 
    	if(last_size_info > 0) num_of_frames = num_of_frames + 1; 
    	else last_size_info = MAX_SIZE_INFO_FRAME;

	//Take the space of the file
	files->data = malloc(files.size_of_file * (unsigned char));

	//Send the handshake packet to notify the GS the file to send
	while(res_receive_frame != NOERROR || type_of_frame != RR) {
		data = malloc(DATA_SIZE_I_HAND * sizeof(unsigned char));
		data[0] = name_of_file;
		data[1] = num_of_frames;
		data[2] = last_size_info;

		res_send_frame = send_frame(I_HAND, data, DATA_SIZE_I_HAND); //Send the handshake packet
		res_receive_frame = receive_frame(); //Receive the response 
		if(res_receive_frame == FCS_ERROR) {
			i_hand_repeat++;
		}else if(type_of_frame != RR) {
			i_hand_repeat++;
		}else if(res_receive_frame == UNKNOWN) {
			i_hand_repeat++;
		}else if(res_receive_frame == NO_DIRECT) {
			i_hand_repeat++;
		}else if(res_receive_frame == NOERROR) {
			i_hand_repeat = 0;
			printf("The handshake frame has been send successfully");
		}
		//Check if arrive to the maximum number
		if(i_hand_repeat == timeout.n_i_hand) {
			i_hand_repeat = 0;
			noerror = ERROR;
			res_receive_frame = NOERROR; //For out of the loop
			type_of_frame = RR; //For out of the loop
			//How the noerror = ERROR, doesn't access to the send of file and return ERROR
		}
	}
	i_hand_repeat = 0;

	//Send the frames of the file
	while(i < num_of_frames && noerror == NOERROR) {
		//The last frame
		if(i == (num_of_frames - 1)) {
			data = malloc(last_size_info * sizeof(unsigned char));
			//Put the information on the MCSPacket field
			for(j = 0; j < (last_size_info); j++) {
				data[j] = files->data[j + i * MAX_SIZE_INFO_FRAME]; 
			}
			res_send_frame = send_frame(I, data, last_size_info); //Send the last info frame
			res_receive_frame = receive_frame(); //Receive the response 
			//Check if is the RR
			if(type_of_frame != RR) {
				i_repeat++;
			}else if(res_receive_frame != NOERROR) {
				rej_repeat++;
				res_send_frame = send_frame(REJ, NULL, 0);
			}else {
				rej_repeat = 0;
				i_repeat = 0;
				i++;
			}
		}else {
			data = malloc(MAX_SIZE_INFO_FRAME * sizeof(unsigned char));
			//Put the information on the MCSPacket field
			for(j = 0; j < (MAX_SIZE_INFO_FRAME); j++) {
				data[j] = files->data[j + i * MAX_SIZE_INFO_FRAME]; 
			}
			res_send_frame = send_frame(I, data, MAX_SIZE_INFO_FRAME); //Send the frame "i"
			res_receive_frame = receive_frame(); //Receive the response 
			//Check if is RR and the RR dosen't have any error
			if(type_of_frame != RR) {
				i_repeat++;
			}else if(res_receive_frame != NOERROR) {
				rej_repeat++;
				res_send_frame = send_frame(REJ, NULL, 0);
			}else {
				rej_repeat = 0;
				i_repeat = 0;
				i++;
			}
		}
		//Check if the repeatitions of I or REJ arrive to maximum number
		if(i_repeat == timeout.n_i || rej_repeat == timeout.n_rej) {
			i_repeat = 0;
			rej_repeat = 0;
			noerror = ERROR;
		}

	}
	return noerror;
	
}

unsigned char analize_frame(frame frame)
{
//Ananilze the frame and make the structure of the frame
//Return the type of frame or ERROR
//data: arrray that we want analize 
	unsigned char type_frame;
	
	if(frames.control == SABM) {
		type_frame = SABM;
	}else if(frames.control == DISC) {
		type_frame = DISC;
	}else if(frames.control == RR){
		type_frame = RR;
	}else if(frames.control == RNR){
		type_frame = RNR;
	}else if(frames.control == REJ) {
		type_frame = REJ;
	}else if(frames.control == SREJ) {
		type_frame = SREJ;
	}else if(frames.control == I_HAND) {
		type_frame = I_HAND;
		name_of_file = frames.information[0];
		num_of_frames = frames.information[1];
		last_size_info = frames.information[2];
	}else if(frames.control == UA) {
		type_frame = UA;
	}else if(frames.control == FRMR) {
		type_frame = FRMR;
	}else if(frames.control == I) {
		type_frame = I;
	}else { type_frame = UNKNOWN; }

	return type_frame;
	
}

unsigned char send_frame(unsigned char type, unsigned char *info, int info_size)
{
//Send the type of frame and control of errors
//If the send frame is correct return noerror, if not, error
	unsigned char *data;
	int i;
	unsigned char dest;
	unsigned char command;
	int noerror;
	dest = ARDUINO;
	command = TRANSMIT;
	MCSPacket *pkt_in, *pkt_out;

	i = 0;
	//frame = create_frame(type, directionGS[8], info); //Create the type of frame
	frames.control = type; //Put the frame
	//Put the directions in the frame struct
	for(i = 0; i < 8; i++) {
		frames.directionSat[i] = directionSat[i];
		frames.directionGS[i] = directionGS[i];
	}
	//If the size of the information is not 0, put the information in the field of information of the frame
	if(info_size > 0) {
		for(i = 0; i < info_size; i++) {
			frames.information[i] = info[i];
		}
	}

	


	//PUT THE FRAME IN FIELD DATA OF MCSPacket...
	data = malloc(DATA_SIZE_U_S * sizeof(unsigned char));
	//Put the direction of Ground Station
	for(i = 0; i < 8; i++) {
		data[i] = frames.directionGS[i];
    	}
	//Put the direction of Cubesat
	for(i = 8; i < 16; i++) {
		data[i] = frames.directionSat[i];
	}
	data[16] = frames.control; //Put the control
	//If the type of frame is information 
	if(type == I) {
		//Put the information in the frame 
		for(i = 17; i < info_size + 17; i++) {
			data[i] = frames.information[i-17];
		}
	}else if(type == I_HAND) {
		for(i = 17; i < info_size + 17; i++) {
			data[i] = frames.information[i-17];
		}
	}
		
	pkt_in = mcs_create_packet_with_dest(COMMS, &dest, 1, &command, DATA_SIZE_U_S, data);//...
	noerror = sdb_send_sync(pkt_in, &pkt_out); //Study what is happened if there are ERROR
	return NOERROR;
}




//CHANGE THINGS OF RUN!!!
unsigned char receive_frame() 
{
//Receive one frame. Return type of frame or error
	MCSPacket *pkt_in, *pkt_out;
	unsigned char fcs; //The fcs of the frame
	unsigned char res; //the return
	unsigned char dest;
	unsigned char command;
	int i, noerror;
	

	dest = ARDUINO;
	command = RECEIVE;

	//PUT THE TIMEOUTS HERE!!!! 
	pkt_in = mcs_create_packet_with_dest(COMMS, &dest, 1, &command, 0, NULL);//Say to ARDUINO receive 
	noerror = sdb_send_sync(pkt_in, &pkt_out);	//Study what is happened if there are ERROR

	//GET THE FIELDS OF FRAME
	//Get the direction of the Sat
	for(i = 0; i < 8; i++) {
		frames.directionSat[i] = pkt_out->data[i];
	}
	//Get the direction of Ground Station
	for(i = 8; i < 16; i++) {
		frames.directionGS[i - 8] = pkt_out->data[i];
	}
	frames.control = pkt_out->data[16]; //Get the field control

	fcs = pkt_out->args[1]; 
	//Check if the fcs is ok
	if(fcs == FCS_ERROR) {
		res = FCS_ERROR;
	}
	
	//Check if the direction is ok
	for(i = 0; i < 8; i++) {
		if(frames.directionSat[i] != directionSat[i]) {
			res = NO_DIRECT;
		}
	}
	if(res != NO_DIRECT) {
		for(i = 0; i < 8; i++) {
			directionGS[i] = frames.directionGS[i];
		}
	}

	type_of_frame = analize_frame(frames);	//Analize the frame
	if(res != NO_DIRECT && type_of_frame == UNKNOWN && res != FCS_ERROR) {
		res = UNKNOWN;
	}else if(fcs != FCS_ERROR && type_of_frame != UNKNOWN && res != NO_DIRECT){
		res = NOERROR; 
	}
	
	return res;	
}



	

















