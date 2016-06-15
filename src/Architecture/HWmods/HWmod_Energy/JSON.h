
"command_list" : [
	
	{
	"id" : "mcs_state_temperature",
	"description" : "Get temperature from the temperature sensor in the Arduino board",
	"nargs" : 1,
	"raw_data" : false,
	"type" : "state",
	"config" : {
		"update_function" : "temperature_function",
		"dimensions" : 1,
		"return_type" : "float",
		"unit" : "K",
		"dimension_name" : null,
		"expire_group" : [{"app" : 15}]
		}
	},
        {
	"id" : "mcs_state_irradiance",
	"description" : "Get irradiance from the irradiance sensor in the Arduino board",
	"nargs" : 1,
	"raw_data" : false,
	"type" : "state",
	"config" : {
		"update_function" : "irradiance_function",
		"dimensions" : 1,
		"return_type" : "float",
		"unit" : "W*(m^2)",
		"dimension_name" : null,
		"expire_group" : [{"app" : 15}]
		}
	},
        {
	"id" : "mcs_state_voltage",
	"description" : "Get voltage from the POLs and sensors in the Arduino board",
	"nargs" : 1,
	"raw_data" : false,
	"type" : "state",
	"config" : {
		"update_function" : "voltage_function",
		"dimensions" : 1,
		"return_type" : "float",
		"unit" : "V",
		"dimension_name" : null,
		"expire_group" : [{"app" : 15}]
		}
	},
        {
	"id" : "mcs_state_current,
	"description" : "Get current from the POLs and sensors in the Arduino board",
	"nargs" : 1,
	"raw_data" : false,
	"type" : "state",
	"config" : {
		"update_function" : "irradiance_function",
		"dimensions" : 1,
		"return_type" : "float",
		"unit" : "W*(m^2)",
		"dimension_name" : null,
		"expire_group" : [{"app" : 15}]
		}
	},
        {
	"id" : "mcs_state_soc",
	"description" : "Get the state of the battery of the Nexus5",
	"nargs" : 1,
	"raw_data" : false,
	"type" : "state",
	"config" : {
		"update_function" : "soc_function",
		"dimensions" : 1,
		"return_type" : "float",
		"unit" : "%",
		"dimension_name" : null,
		"expire_group" : [{"app" : 15}]
		}
	},
        {
        "id" : "mcs_message_check",
        "description" : "check the correct performance of the management of energy"
        "nargs" : 1,
        "raw_data" : false,  
        "type" : "message",
        "config" : {
            "destination": @arg ,
            "origin_groups" : any ,
            "destination_groups" : ["hwmod","appmod"]  ,
            "response_size" : -1  
	},
        {
        "id" : "mcs_message_init",
        "description" : "initialize the energy HwMode"
        "nargs" : 1,
        "raw_data" : false,  
        "type" : "message",
        "config" : {
            "destination": @arg ,
            "origin_groups" : any,
            "destination_groups" : ["hwmod","appmod"] ,
            "response_size" : -1  
	},
        {
        "id" : "mcs_message_run",
        "description" : " Principal function of the energy HwMode , runs continuously"
        "nargs" : 1,
        "raw_data" : false,  
        "type" : "message",
        "config" : {
            "destination": @arg ,
            "origin_groups" : any ,
            "destination_groups" : ["hwmod","appmod"]  ,
            "response_size" : -1  
	},
        {
        "id" : "mcs_message_halt",
        "description" : "Interrupts the energy HwMode"
        "nargs" : 1,
        "raw_data" : false,  
        "type" : "message",
        "config" : {
            "destination": @arg ,
            "origin_groups" : any ,
            "destination_groups" : ["hwmod","appmod"]  ,
            "response_size" : -1  
	},
        
/*** PAYLOAD FUNCTIONS ********************************************************************************/ 
                
            
    {
        "id" : "mcs_message_enablepol",
        "description" : "enable a certain pol"
        "nargs" : 1,
        "raw_data" : false,  
        "type" : "message",
        "config" : {
            "destination": @arg ,
            "origin_groups" : any ,
            "destination_groups" : ["hwmod","appmod"] ,
            "response_size" : 1
	},
        {
        "id" : "mcs_message_disablepol",
        "description" : "disable a certain pol"
        "nargs" : 1,
        "raw_data" : false,  
        "type" : "message",
        "config" : {
            "destination": @arg ,
            "origin_groups" : any ,
            "destination_groups" : ["hwmod","appmod"] ,
            "response_size" : 1
	},
        {
        "id" : "mcs_message_ispolenabled",
        "description" : "check if a certain pol is enabled"
        "nargs" : 1,
        "raw_data" : false, 
        "type" : "message",
        "config" : {
            "destination": @arg ,
            "origin_groups" : any ,
            "destination_groups" : ["hwmod","appmod"] ,
            "response_size" : 1
	} 
      
]
}

/*** Arduino Functions ********************************************************************************/


       {
"command_list" : [
	
	
	{
	"name" : "arduino_read_i2c",
	"description" : "Read a specific direction of the i2c protocol",
	"nargs" : 1,
	"raw_data" : false,
	"type" : "payload",
	"config" : {
		"command" : "010",
		"parameters" : "0001",
		"arguments" : "@arg0",
		"data" : null,
		"response_size" : -1
		}
	}
	{
	"name" : "arduino_write_i2c",
    "description" : "Write in a specific direction of the i2c protocol",
    "nargs" : 1,
    "raw_data" : true,
    "type" : "payload",
    "config" : {
         "command" : "010",
         "parameters" : "0010",
         "arguments" : "@arg0",
         "data" : "data", 
         "response_size" : -1
            }

	}
    {
    "name" : "arduino_analog_read",
    "description" : "Read a specific arduino pin",
    "nargs" : 1,
    "raw_data" : false,
    "type" : "payload",
    "config" : {
         "command" : "001",
         "parameters" : "0010",
         "arguments" : "@arg0",
         "data" : null,
         "response_size" : -1
            }
                      
    }
    {
     "name" : "arduino_init_i2c",
     "description" : "Initialize de protocol i2c",
     "nargs" : 2,
     "raw_data" : false,
     "type" : "payload",
     "config" : {
          "command" : "010",
          "parameters" : "0000",
          "arguments" : ["@arg0","@arg1"]
          "data" : null,
          "response_size" : -1
             }
                      
    }
                      
    }
]

