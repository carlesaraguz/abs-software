#include "Library_comms.h"


/*GLOBAL VARIABLES*/
states_hwmod state;         /*State of the system*/
timeouts timeout;           /*Timeouts of the system*/
frame frames;               /*Structure of the frame I*/
unsigned int freq;          /*To implement...*/
unsigned int protocol_delay;            /*Delay introduced to simulate propagation delays.To discuse...*/
unsigned int frame_delay;               /*Delay between iterative packet transmissions.To discuse...*/
unsigned int disc_repeat;       /*Amount of repetitions of frame DISC*/
unsigned int rej_repeat;        /*Amount of repetitions of frame REJ to receive*/
unsigned int i_hand_repeat;     /*Amount of repetitions of frame I_HAND to send*/
unsigned int i_repeat;          /*Amount of repetitions of I frame to send*/
unsigned int ua_repeat;         /*Amount of repetitions of frame UA*/
unsigned int rnr_repeat;        /*Amount of repetitions of frame RNR*/
unsigned int rr_repeat;         /*Amount of repetitions of frame RR*/
unsigned int datarate;          /*data rate*/
file file_received;         /*file which save the information of the frames*/
file file_send;             /*File which send the information with frames*/
unsigned char directionSat[8] = {direction1, direction2, 
                                 direction3, direction4, direction5, direction6, direction7, directionSSID};     /*Direction of the Sat*/
unsigned char directionGS[8];       //Save the direction of the ground station 
unsigned char pid;          /*PID of the Information frame*/
unsigned char command_file;     /*Command of the data received*/
unsigned char first_handshake;      /*Check if the handshake packet is the first received*/
unsigned char type_of_frame;        /*The type of frame which arrive*/
unsigned int last_size_info;        /*The size of the last frame of information*/
unsigned int num_of_frames;     /*Number of packets to receive or send*/
unsigned char name_of_file;     /*The name file*/
unsigned int run_in_out;        /*If the variable is 1, do the run. If is 0 out of run*/
FILE *f;                        /*To save and take files in the base of data*/

unsigned int flag_send_file;       /*Flag of the Cubesat to send one file*/
unsigned char wait_name_file;   /*If the Cubesat call the function "flag_to_send_file", The name of the file is saved in this
variable for, in the case that there is in one process, not affect*/



/*MAIN*/
int main()
{
    int noerror;
    char *name;
    int i;

    noerror = NOERROR;
    i = 0;
    /*Do the connect of the SDB*/
    name = malloc(5 * sizeof(char));
    name = "COMMS";
    noerror = sdb_connect(name, SDB_GROUP_HWMOD);
    if(noerror != NOERROR_INT) {
        printf("Error in the connection with the SDB");
        return ERROR_INT;
    }
    if(noerror == NOERROR_INT) {
        return NOERROR_INT;
    }

    /*Put the register callbacks of the SDB*/
    noerror = sdb_register_callback(COMMAND_CHECK, check);
    noerror = sdb_register_callback(COMMAND_INIT, init);
    noerror = sdb_register_callback(COMMAND_RUN, run);
    noerror = sdb_register_callback(COMMAND_HALT, halt);

    noerror = sdb_register_callback(COMMAND_FLAG_TO_SEND_FILE, flag_to_send_file);
    
    if(noerror == NOERROR_INT) {
        return NOERROR_INT;
    }
}

/*Function init*/
void init(MCSPacket *pkt_in, MCSPacket **pkt_out)
{
    state = STATE_INIT;
    MCSPacket *pkt_in_ard, *pkt_out_ard;
    int noerror;
    unsigned char dest;
    unsigned char command;
    dest = ARDUINO;
    command = CONFIGURE;
	
    /*Said to ARDUINO configure default parameters*/
    pkt_in_ard = mcs_create_packet_with_dest(COMMS, &dest, 1, &command, 0, NULL);
    noerror = sdb_send_sync(pkt_in_ard, &pkt_out_ard);
    if(noerror != ERROR_INT) {
        printf("Error in the send packet through SDB");
        *pkt_out = mcs_err_packet(pkt_in, ERROR_INT); /*CHECK THE ERROR_INT(PROVISIONAL)...*/
    }else {
        /*Configure timeouts*/
        timeout.t_frame = TIMEOUT_T_FRAME; /*To discuse...*/
        timeout.n_disc = TIMEOUT_N_DISC;
        timeout.n_rej = TIMEOUT_N_REJ;
        timeout.n_i_hand = TIMEOUT_N_I_HAND;
        timeout.n_i = TIMEOUT_N_I;  
        timeout.n_ua = TIMEOUT_N_UA;
        timeout.n_rnr = TIMEOUT_N_RNR;
        timeout.n_rr = TIMEOUT_N_RR;
        /*Implement the frequency...*/
        freq = 433;
        datarate = 250;
        /*Initialize the global variables*/
        protocol_delay = 0; 
        frame_delay = 0;
        disc_repeat = 0;
        rej_repeat = 0;
        i_hand_repeat = 0;
        i_repeat = 0;
        ua_repeat = 0;
        rnr_repeat = 0;
        rr_repeat = 0;
        /*Intilialize the default parameters of the frame*/
        pid = PID; /*For default*/
        frames.pid = pid; /*For default*/
        first_handshake = NO_F_H;
        last_size_info = 0;
        num_of_frames = 0;
	    run_in_out = 1;
	    flag_time = 0;
        *pkt_out = mcs_ok_packet(pkt_in); /*Return the ok*/
    } 
	
}

/*Function check*/
void check(MCSPacket *pkt_in, MCSPacket **pkt_out)
{
    int error;
    int i;
    char *name;
    *pkt_out = mcs_ok_packet(pkt_in);
}



/*Function run*/
void run(MCSPacket *pkt_in, MCSPacket **pkt_out)
{
    MCSPacket *pkt_in_ard, *pkt_out_ard;
    MCSPacket *pkt_in_sat, *pkt_out_sat;
    unsigned char dest, command;
    unsigned char res_send, res_receive; /*Result of the send and receive frame or file */
    unsigned char fcs;
    int i, noerror, error_run;
    unsigned char *data;
    
    i = 0;
    error_run = NOERROR_INT;
    /*After init put the system in listening*/
    if(state == STATE_INIT) {
        state = STATE_RUN_LISTEN;
    }else{   
        run_in_out = 0;
        error_run = ERROR_INT;
    }

run_start:
    while(run_in_out == 1) {
        state = STATE_RUN_LISTEN; /*After close one communication, put in state listening*/
        first_handshake = FIRST_H;

        /*RECEIVE(LISTEN) THE SABM TO CONNECT AND CONTROL ERRORS*/
        printf("Listen SABM");
        res_receive = receive_frame(0);
        /*Check the type of packets and possible errors. If there are, send DM*/
        if(res_receive == FCS_ERROR) {
            res_send = send_frame(DM, NULL, 0);
            goto run_start;
        }else if(res_receive == NO_DIRECT) {
            goto run_start;
        }else if(res_receive == UNKNOWN) {
            res_send = send_frame(DM, NULL, 0);
            goto run_start;
        }else if(res_receive == TIMEOUT) {
            res_send = send_frame(DM, NULL, 0);
            goto run_start;
        }else if(type_of_frame != SABM) {
            res_send = send_frame(DM, NULL, 0);
            goto run_start;
        }else { res_send = send_frame(UA, NULL, 0); }


        /*while don't receive the handshake packet the connection for part of GS is not done!!*/
        while(type_of_frame != I_HAND && res_receive != NOERROR) { 
            res_receive = receive_frame(3); /*Receive the frame*/
            /*Check errors*/
            if(res_receive == FCS_ERROR) {
                res_send = send_frame(REJ, NULL, 0);
                i_hand_repeat++;
            }else if(res_receive == UNKNOWN) {
                res_send = send_frame(REJ, NULL, 0);
                i_hand_repeat++;
            }else if(res_receive == TIMEOUT) {
                res_send = send_frame(REJ, NULL, 0);
                i_hand_repeat++;
            }else if(type_of_frame != I_HAND) {
                res_send = send_frame(REJ, NULL, 0);
                i_hand_repeat++;
            }
            /*If the repeat of send handshake packet arrived, close the link and go to run start*/
            if(i_hand_repeat == timeout.n_i_hand) {
                printf("The handshake repeat arrive to the maximum number, close the link");
                i_hand_repeat = 0;
                goto run_start;
            }if(type_of_frame == I_HAND && res_receive == NOERROR) {
                i_hand_repeat = 0;
            }
        }
        printf("The connection was successfully");
        

            
        /*STATE CONNECT, READY TO RECEIVE INFORMATION*/
        
        state = STATE_RUN_CONNECT;
        /*While the state is connect, the Cubesat and the ground station can exchange information*/
        while(state == STATE_RUN_CONNECT) {          

            /*After receive the first file is possible receive one I_HAND or DISC*/
            if(first_handshake != FIRST_H) {
                res_receive = receive_frame(3);
                /*Check the errors*/
                if(res_receive == FCS_ERROR) {
                    res_send = send_frame(REJ, NULL, 0);
                    rej_repeat++;
                }else if(res_receive == UNKNOWN) {
                    res_send = send_frame(REJ, NULL, 0);
                    rej_repeat++;
                }else if(res_receive == TIMEOUT) {
                    res_send = send_frame(REJ, NULL, 0);
                    rej_repeat++;
                }else if(type_of_frame != I_HAND && type_of_frame != DISC && type_of_frame != RR) {
                    res_send = send_frame(REJ, NULL, 0);
                    rej_repeat++;
                }
                /*If the repeat packets arrive to maximum number, close the link*/
                if(rej_repeat == timeout.n_rej) {
                    printf("The handshake or DISC repeat arrive to the maximum number, close the link");
                    rej_repeat = 0;
                    goto run_start;
                }
                
            }
            rej_repeat = 0;
            /*If the received frame is I_HAND, receive the file*/
            if(type_of_frame == I_HAND) {
                /*Receive the file*/
                name_of_file = frames.information[0]; /*Put the name of file*/
                num_of_frames = (unsigned int)frames.information[1]; /* Put the number of frames*/
                last_size_info = (unsigned int)frames.information[2]; /* Put the size of the last frame of info.*/
                first_handshake = NO_F_H;

                /*The response of the I handshake is in the receive_file*/
                /*RECEIVE THE FILE*/
                res_receive = receive_file(name_of_file, num_of_frames, last_size_info);
                
                state = STATE_RUN_CONNECT; /*Put the state in connect after out of the function*/
                if(res_receive == ERROR) {
                    printf("The receive file has an error, close the link");
                    goto run_start;
                }
                /*Notify to Cubesat the name of the receive file(use the field data to put in the field of args of MCSPacket)*/
                data = malloc(1 * sizeof(unsigned char));
                data[0] = name_of_file;
                dest = BROADCAST;
                pkt_in_sat = mcs_create_packet_with_dest(COMMS, &dest, 1, data, 0, NULL);//...
                noerror = sdb_send_sync(pkt_in_sat, &pkt_out_sat);//...
                /*The args of MCSPacket to notify if it need send a file is: 1.SEND/NOSEND FILE, 2. name of file*/
                if(pkt_out_sat->args[0] == SEND_FILE) {
                    name_of_file = pkt_out_sat->args[1]; /*The name of the file that the Cubesat pass to the HWmod*/

                    res_send = send_file(name_of_file);
                    state = STATE_RUN_CONNECT; /*Put the state in connect after out of the function*/
                    if(res_send == ERROR) {
                        printf("The send file has an error, close the link");
                        goto run_start;
                    }
                }
            /*If the frame is DISC, disconnect and go to run start(state listen)*/
            }else if(type_of_frame == DISC) {
                res_send = send_frame(UA, NULL, 0);
                goto run_start;
            /*If the received frame is RR, the system respond with another RR to 
            keep the connection while not send or receive one file, or if the system do one flag to
            send one file (flag_send_file = 1) send the file*/
            }else if(type_of_frame == RR) {
                /*If the system not does one flag to send the file, send one RR to respond*/
                if(flag_send_file == 0) {
                    res_send = send_frame(RR, NULL, 0);
                /*If the system does the flag to send one file, send the file*/
                }else if(flag_send_file == 1) {
                    /*Send the file with the name that the Cubesat send to HWmod*/
                    name_if_file = wait_name_file; 
                    res_send = send_file(name_of_file);
                    state = STATE_RUN_CONNECT; /*Put the state in connect after out of the function*/
                    if(res_send == ERROR) {
                        printf("The send file has an error, close the link");
                        goto run_start;
                    }
                }
            }       
            
        }
        
    }
    /*Check if there are an error in the call of function run*/
    if(error_run == NOERROR_INT) {
        *pkt_out = mcs_ok_packet(pkt_in); /*Return the ok*/
        printf("The function run has worked correctly");
    }else{ 
        *pkt_out = mcs_err_packet(pkt_in, ERROR_INT); /*Check what is the COMMAND OF ERROR_INT(PROVISIONAL)...*/
        printf("The function run has not been called in the correct moment(it must call after function init)");
    }
    
    printf("Out of the function run");
}

/*Function halt*/
void halt(MCSPacket *pkt_in, MCSPacket **pkt_out)
{
    /*To go out of the function run*/
    state = STATE_HALT;
    run_in_out = 0;
    printf("Halt: close the function run");
    *pkt_out = mcs_ok_packet(pkt_in);
    
    
}

unsigned char receive_file(unsigned char name_file, int number_frames, int last_info_frame)
{
/*After do the connection, receive the information and response*/
    int i, j; 
    unsigned char noerror; /*For return*/
    int n_error; /*For amount of number of errors*/
    unsigned char res_send_frame; /*The result of the function send_frame*/
    unsigned char res_receive_frame; /*The result of the function receive_frame*/
    unsigned char *data; /*For save the file in the base of data*/

    i = 0;
    j = 0;
    noerror = NOERROR;  
    n_error = 0;
    res_receive_frame = ERROR;
    
    state = STATE_RECEIVE_FILE; /*Put the state in receive file*/
    
    res_send_frame = send_frame(RR, NULL, 0); /*Send the response of I_HAND*/
    /*While the type of frame is not information or has an error send one REJ*/
    while(type_of_frame != I || res_receive_frame != NOERROR) { 
        if(number_frames == 1) {
            res_receive_frame = receive_frame(last_info_frame); /*Receive the frame*/
        }else { res_receive_frame = receive_frame(MAX_SIZE_INFO_FRAME); }
        /*Check the errors*/
        if(res_receive_frame == FCS_ERROR) {
            res_send_frame = send_frame(REJ, NULL, 0);
            rej_repeat++;
        }else if(res_receive_frame == UNKNOWN) {
            res_send_frame = send_frame(REJ, NULL, 0);
            rej_repeat++;
        }else if(res_receive_frame == TIMEOUT) {
            res_send_frame = send_frame(REJ, NULL, 0);
            rej_repeat++;
        }else if(type_of_frame == I_HAND) {
            res_send_frame = send_frame(RR, NULL, 0);
            rr_repeat++;
        }
        /*If arrive to maximum numbers, close the link*/
        if(rej_repeat == timeout.n_rej || rr_repeat == timeout.n_rr) {
            printf("The response of handshake or reject frames arrive to the maximum number, close the link");
            rr_repeat = 0;
            rej_repeat = 0;
            noerror = ERROR;
            type_of_frame = I; /*This is for out of the loop*/
            res_receive_frame = NOERROR; /*This is for out of the loop*/
            /*How the noerror = ERROR, it doesn't access to the loop and goes directly to return the error*/
        }
    }
    rr_repeat = 0;
    rej_repeat = 0;
    /*While don't finish the receive frames and there isn't an error run.*/
    while(i < number_frames && noerror == NOERROR) {
        /*Check if the arrived frame is the last frame*/
        if(i == (num_of_frames - 1)) {

            /*This is for the case of there are only one frame of information
            and the first frame of information has arrived already*/
            if(number_frames > 1) {
                res_receive_frame = receive_frame(last_size_info); /*Receive the frame*/
            }

            /*Control of errors*/
            /*If the reject repeat arrive to the maximum number return error*/
            if(rej_repeat == timeout.n_rej || rnr_repeat == timeout.n_rnr) {
                printf("The REJ arrive to his maximum number");
                rej_repeat = 0;
                rnr_repeat = 0;
                noerror = ERROR;
            /*If the receive frame has an fcs error send REJ*/
            }else if(res_receive_frame == FCS_ERROR) {
                res_send_frame = send_frame(REJ, NULL, 0);
                rej_repeat++;
            /*If the receive frame has an no correct direction send REJ*/
            }else if(res_receive_frame == NO_DIRECT) {
                res_send_frame = send_frame(REJ, NULL, 0); /*??? why it can happen??*/
                rej_repeat++;
            /*If the receive frame has an unknown frame send REJ*/
            }else if(res_receive_frame == UNKNOWN) {
                res_send_frame = send_frame(REJ, NULL, 0); /*??? why it can happen??*/
                rej_repeat++;
            /*If the receive frame has a timeout, send REJ*/
            }else if(res_receive_frame == TIMEOUT) {
                res_send_frame = send_frame(REJ, NULL, 0); /*??? why it can happen??*/
                rej_repeat++;
            /*If the receive frame is not information frame send REJ*/
            }else if(type_of_frame != I && type_of_frame != REJ) {
                res_send_frame = send_frame(REJ, NULL, 0); /*??? why it can happen??*/
                rej_repeat++;
            /*If the receive frame is ok, send RR and put the information in the field of data of file*/
            }else if(res_receive_frame == NOERROR && type_of_frame == I) {
                rej_repeat = 0;
                rnr_repeat = 0;
                for(j = 0; j < last_size_info; j++) {
                    file_received.data[j + i * MAX_SIZE_INFO_FRAME] = frames.information[j];
                }
                file_received.name = name_of_file; /*Put the name in the struct of file*/
                file_received.size_of_file = (number_frames - 1) * MAX_SIZE_INFO_FRAME + last_size_info; /*Put the size of file*/
                res_send_frame = send_frame(RNR, NULL, 0); /*Send the RNR to conclude the receive file*/
                i++;
                printf("The received frames was successfully"); 
               
            }else if(res_receive_frame == NOERROR && type_of_frame == REJ) {
                rr_repeat++;
                res_send_frame = send_frame(RNR, NULL, 0);
                printf("The received frame is REJ, send one RNR again");
            }
        /*Check the first information frame(it don't need the receive frame because this arrive yet)*/
        }else{
            /*This is for the first information frame, because it has arrived already*/
            if(i != 0) {
                res_receive_frame = receive_frame(MAX_SIZE_INFO_FRAME); /*Receive the frame*/
            }
            /*Control of errors*/
            /*If the reject repeat arrive to the maximum number return error*/
            if(rej_repeat == timeout.n_rej || rr_repeat == timeout.n_rr) {
                printf("The REJ or RR arrive to his maximum number");
                rr_repeat = 0;
                rej_repeat = 0;
                noerror = ERROR;
            /*If the receive frame has an fcs error send REJ*/
            }else if(res_receive_frame == FCS_ERROR) {
                res_send_frame = send_frame(REJ, NULL, 0);
                rej_repeat++;
            /*If the receive frame has an no correct direction send REJ*/
            }else if(res_receive_frame == NO_DIRECT) {
                res_send_frame = send_frame(REJ, NULL, 0); /*??? why it can happen??*/
                rej_repeat++;
            /*If the receive frame has an unknown frame send REJ*/
            }else if(res_receive_frame == UNKNOWN) {
                res_send_frame = send_frame(REJ, NULL, 0); /*??? why it can happen??*/
                rej_repeat++;
            /*If the receive frame has a timeout, send REJ*/
            }else if(res_receive_frame == TIMEOUT) {
                res_send_frame = send_frame(REJ, NULL, 0); /*??? why it can happen??*/
                rej_repeat++;
            /*If the receive frame is not information frame send REJ*/
            }else if(type_of_frame != I && type_of_frame != REJ) {
                res_send_frame = send_frame(REJ, NULL, 0); /*??? why it can happen??*/
                rej_repeat++;
            /*If the receive frame is ok, send RR and put the info in the field of the data of file*/
            }else if(res_receive_frame == NOERROR && type_of_frame == I) {
                rej_repeat = 0;
                rr_repeat = 0;
                for(j = 0; j < MAX_SIZE_INFO_FRAME; j++) {
                    file_received.data[j + i * MAX_SIZE_INFO_FRAME] = frames.information[j];
                }
                res_send_frame = send_frame(RR, NULL, 0);
                i++;
                printf("The received frames was successfully");
            }else if(res_receive_frame == NOERROR && type_of_frame == REJ) {
                rr_repeat++;
                res_send_frame = send_frame(RR, NULL, 0);
                printf("The received frame is REJ, send one RR again");
            }
            
        }

    }
    /*Create and put the file in a data base if there isn't any error*/
    if(noerror == NOERROR) {
        printf("Put the file in the base of data"); 
        /*Put the file recieved in the base of data*/
        data = malloc(file_received.size_of_file * sizeof(unsigned char));
        /*Put the file's information in *data*/
        for(j = 0; j < file_received.size_of_file; j++) {
            data[i] = file_received.data[j];
        }
                
        f = fopen((const char)&file_received.name, "w");/*Open the base of data*/
        fwrite(data, 1, file_received.size_of_file, f);/*Save the file*/
        fclose(f);/*Close the base of data*/
        free(data);
    }
	/*After make the process(with or without errors) is necessary clean the buffers...*/
    return noerror;
}

unsigned char send_file(unsigned char name_of_file)
{
/*Send the file with the name name_of_file, the last size of information frame to the ground station*/
/*Return ERROR or NOERROR*/
    unsigned char noerror;
    unsigned char res_send_frame, res_receive_frame;
    int i, j;
    unsigned char *data;
    int long size_f; /*Size of the file of the base of data*/

    i = 0;
    j = 0;
    noerror = NOERROR;
    
    state = STATE_SEND_FILE; /*Put the state in the send file*/

    /*GO TO TAKE THE FILE...*/
    f = fopen(&name_of_file, "r"); /*Open the file in the base of data*/
    
    fseek(f, 0, SEEK_END);
    size_f = ftell(f); /*The size of the file*/
    data = malloc(size_f * sizeof(unsigned char));/*Create the space on data*/
    fread(data, 1, size_f, f);/*Put the bytes in the field data*/
    
    fclose(f);/*Close the file in the base of data*/
    
    file_send.name = name_of_file;/*Put the name of the file*/
    file_send.size_of_file = size_f;/*Put the size of the file*/
    /*Put the data in the struct of the file_send*/
    for(i = 0; i < size_f; i++){
        file_send.data[i] = data[i];
    }
    
    free(data); /*Free the memory space*/
    
    /*Number of frames and the size of the last frame*/
    num_of_frames = file_send.size_of_file / MAX_SIZE_INFO_FRAME; 
    last_size_info = file_send.size_of_file % MAX_SIZE_INFO_FRAME; 
        if(last_size_info > 0) num_of_frames = num_of_frames + 1; 
        else last_size_info = MAX_SIZE_INFO_FRAME;


    /*Send the handshake packet to notify the GS the file to send*/
    while(res_receive_frame != NOERROR || type_of_frame != RR) {
        data = malloc(DATA_SIZE_I_HAND * sizeof(unsigned char));
        data[0] = name_of_file; /*Put the name of file in the data*/
        data[1] = (unsigned char)num_of_frames; /* Put the number of frames in data(in format bytes)*/
        data[2] = (unsigned char)last_size_info;/*Put the number of frames in data(in format bytes)*/

        res_send_frame = send_frame(I_HAND, data, DATA_SIZE_I_HAND); /*Send the handshake packet*/
        res_receive_frame = receive_frame(0); /*Receive the response */
		/*Check the errors*/
        if(res_receive_frame == FCS_ERROR) {
            i_hand_repeat++;
        }else if(type_of_frame != RR) {
            i_hand_repeat++;
        }else if(res_receive_frame == UNKNOWN) {
            i_hand_repeat++;
        }else if(res_receive_frame == NO_DIRECT) {
            i_hand_repeat++;
        }else if(res_receive_frame == TIMEOUT) {
            i_hand_repeat++;
        }else if(res_receive_frame == NOERROR) {
            i_hand_repeat = 0;
            printf("The handshake frame has been send successfully");
        }
        /*Check if arrive to the maximum number*/
        if(i_hand_repeat == timeout.n_i_hand) {
            i_hand_repeat = 0;
            noerror = ERROR;
            res_receive_frame = NOERROR; /*For out of the loop*/
            type_of_frame = RR; /*For out of the loop*/
            /*How the noerror = ERROR, doesn't access to the send of file and return ERROR*/
        }
        free(data);/*Free the memory space*/
    }
    i_hand_repeat = 0;
    i = 0;
    /*Send the frames of the file*/
    while(i < num_of_frames && noerror == NOERROR) {
        /*The last frame*/
        if(i == (num_of_frames - 1)) {
            data = malloc(last_size_info * sizeof(unsigned char));
            /*Put the information on the MCSPacket field*/
            for(j = 0; j < (last_size_info); j++) {
                data[j] = file_send.data[j + i * MAX_SIZE_INFO_FRAME]; 
            }
            res_send_frame = send_frame(I, data, last_size_info); /*Send the last info frame*/
            res_receive_frame = receive_frame(0); /*Receive the response */
            /*Check if is the RR*/
            if(type_of_frame != RR) {
                i_repeat++;
                printf("The response is not RR");
            }else if(res_receive_frame != NOERROR) {
                rej_repeat++;
                res_send_frame = send_frame(REJ, NULL, 0);
                printf("The received frame has an error");
            }else {
                rej_repeat = 0;
                i_repeat = 0;
                i++;
            }
            free(data); /*Free the memory space*/
        }else {
            data = malloc(MAX_SIZE_INFO_FRAME * sizeof(unsigned char));
            /*Put the information on the MCSPacket field*/
            for(j = 0; j < (MAX_SIZE_INFO_FRAME); j++) {
                data[j] = file_send.data[j + i * MAX_SIZE_INFO_FRAME]; 
            }
            res_send_frame = send_frame(I, data, MAX_SIZE_INFO_FRAME); /*Send the frame "i"*/
            res_receive_frame = receive_frame(0); /*Receive the response */
            /*Check if is RR and the RR dosen't have any error*/
            if(type_of_frame != RR) {
                i_repeat++;
                printf("The response is not RR");
            }else if(res_receive_frame != NOERROR) {
                rej_repeat++;
                res_send_frame = send_frame(REJ, NULL, 0);


                printf("The received frame has an error");
            }else {
                rej_repeat = 0;
                i_repeat = 0;
                i++;
            }
            free(data);
        }
        /*Check if the repeatitions of I or REJ arrive to maximum number*/
        if(i_repeat == timeout.n_i || rej_repeat == timeout.n_rej) {
            i_repeat = 0;
            rej_repeat = 0;
            noerror = ERROR;
            printf("The rejects frames has arrived to maximum number");
        }

    }
	/*After make the process(with or without errors) is necessary clean the buffers...*/
    return noerror;
    
}

unsigned char analize_frame()
{
/*Ananilze the frame and make the structure of the frame
Return the type of frame or ERROR
data: arrray that we want analize */
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
/*Send the type of frame and control of errors
If the send frame is correct return noerror, if not, error*/
    unsigned char *data;
    int i, noerror;
    unsigned char dest;
    unsigned char command;
    unsigned char res; /*The return of the function*/
    
    dest = ARDUINO;
    command = TRANSMIT;
    MCSPacket *pkt_in, *pkt_out;
	
	noerror = 0;
    i = 0;
    /*frame = create_frame(type, directionGS[8], info); //Create the type of frame*/
    frames.control = type; /*Put the frame*/
    /*Put the directions in the frame struct*/
    for(i = 0; i < 8; i++) {
        frames.directionSat[i] = directionSat[i];
        frames.directionGS[i] = directionGS[i];
    }
    /*If the size of the information is not 0, put the information in the field of information of the */
    if(info_size > 0) {
        for(i = 0; i < info_size; i++) {
            frames.information[i] = info[i];
        }
    }

    


    /*PUT THE FRAME IN FIELD DATA OF MCSPacket*/
    data = malloc(DATA_SIZE_U_S * sizeof(unsigned char));
    /*Put the direction of Ground Station*/
    for(i = 0; i < 8; i++) {
        data[i] = frames.directionGS[i];
        }
    /*Put the direction of Cubesat*/
    for(i = 8; i < 16; i++) {
        data[i] = frames.directionSat[i];
    }
    data[16] = frames.control; /*Put the control*/
    /*If the type of frame is information */
    if(type == I) {
        /*Put the information in the frame */
        for(i = 17; i < info_size + 17; i++) {
            data[i] = frames.information[i-17];
        }
    }else if(type == I_HAND) {
        for(i = 17; i < info_size + 17; i++) {
            data[i] = frames.information[i-17];
        }
    }
        
    pkt_in = mcs_create_packet_with_dest(COMMS, &dest, 1, &command, DATA_SIZE_U_S, data);
    noerror = sdb_send_sync(pkt_in, &pkt_out); /*Study what is happened if there are ERROR*/
    if(noerror != NOERROR_INT) {
        res = ERROR;
        printf("Error in the send packet through SDB"); 
    }else { res = NOERROR;}
    free(data);
    return res;
}




/*CHANGE THINGS OF RUN!!!*/
unsigned char receive_frame(int size_data_info) 
{
/*Size_data_info: in the case that the receive frame was Information or handshake
Receive one frame. Return type of frame or error*/
    struct timeval time;
    MCSPacket *pkt_in, *pkt_out;
    SDBPendingPacket *pkt_out_async;
    unsigned char fcs; /*The fcs of the frame*/
    unsigned char res; /*the return*/
    unsigned char dest;
    unsigned char command;
    int i, noerror, check, res_time;
    int long long time_start, time_now, time_def; 
    
    i = 0;
    noerror = 0; /*To the return of functions of SDB*/
    check = 0; /*return of the function of wait SDB*/
    dest = ARDUINO;
    command = RECEIVE;
    time_start = 0; /*The time start*/
    time_now = 0; /*The time in the specific moment*/
    time_def = 0; /*The diference between the times for check the time out*/
    res_time = 0; /*Return of gettimeofday function*/
    
 
    pkt_in = mcs_create_packet_with_dest(COMMS, &dest, 1, &command, 0, NULL);/*Say to ARDUINO receive */

    /*CONTROL OF THE TIME*/
    noerror = sdb_send_async(pkt_in, &pkt_out_async);
    /*Check if the send packet through the SDB not has an error*/
    if(noerror != NOERROR_INT) {
        res = ERROR;
        printf("Error in the send packet through SDB");
    }else {
        /*While the packet not arrive or the time not arrive to the time out, checking */
		res_time = gettimeofday(&time, NULL);
		time_start = (int long long)time.tv_sec * 1000000 + time.tv_usec;
        while(time_def < timeout.t_frame && check != CHECK_ARRIVE) {
            res_time= gettimeofday(&time, NULL);
		    time_now = (int long long)time.tv_sec * 1000000 + time.tv_usec;
            time_def = time_now - time_start;
            check = sdb_check_async(pkt_out_async);
        }

        /*If the packet arrive, check the control errors. If not, return TIMEOUT*/
        if(check == CHECK_ARRIVE) {
            /*GET THE FIELDS OF FRAME*/
            /*Get the direction of the Sat*/
            for(i = 0; i < 8; i++) {
                frames.directionSat[i] = pkt_out_async->pp_pkt->data[i];
            }
            /*Get the direction of Ground Station*/
            for(i = 8; i < 16; i++) {
                frames.directionGS[i - 8] = pkt_out_async->pp_pkt->data[i];
            }
            frames.control = pkt_out_async->pp_pkt->data[i];; /*Get the field control*/
    
            fcs = pkt_out->args[1]; 
            /*Check if the fcs is ok*/
            if(fcs == FCS_ERROR) {
                res = FCS_ERROR;
                printf("The frame has an FCS error");
            }
        
            /*Check if the direction is ok*/
            for(i = 0; i < 8; i++) {
                if(frames.directionSat[i] != directionSat[i]) {
                    res = NO_DIRECT;
                }
            }
			/*If the direction is equal to the Cubesat, put in the global variable of the directionGS*/
            if(res != NO_DIRECT) {
                for(i = 0; i < 8; i++) {
                    directionGS[i] = frames.directionGS[i];
                }
            }else { printf("The direction is not of the Cubesat"); }
        
            type_of_frame = analize_frame();  /*Analize the frame*/
			/*If the frame is unknown, return unknown*/
            if(res != NO_DIRECT && type_of_frame == UNKNOWN && res != FCS_ERROR) {
                res = UNKNOWN;
                printf("The frame is unknown");
            }else if(fcs != FCS_ERROR && type_of_frame != UNKNOWN && res != NO_DIRECT){
                res = NOERROR;
                /*If the type of frame is I_HAND, put the information in the struct of frame*/
                if(type_of_frame == I_HAND) {
                    frames.information[0] = pkt_out_async->pp_pkt->data[0];
                    frames.information[1] = pkt_out_async->pp_pkt->data[1];
                    frames.information[2] = pkt_out_async->pp_pkt->data[2];
				/*If the type of frame is I, put the information in the struct of frame*/
                }else if(type_of_frame == I) {
                    for(i = 0; i < size_data_info; i++) {
                        frames.information[i] = pkt_out_async->pp_pkt->data[i+17];
                    }
                }
                printf("The frame has not errors");
            }
        }else { 
            res = TIMEOUT; 
            printf("The frame has not arrived(timeout)");
        }
    }
    return res; 
}



void flag_to_send_file(MCSPacket *pkt_in, MCSPacket **pkt_out)
{
/*This function is called by the Cubesat to notify send the file with the name which are in the field of
args of the MCS packet*/
    flag_send_file = 1;
    wait_name_file = pkt_in->args[0];  
    *pkt_out = mcs_ok_packet(pkt_in); 
}


    




