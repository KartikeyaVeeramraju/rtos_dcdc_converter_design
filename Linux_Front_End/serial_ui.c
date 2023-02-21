#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>

#include "serial.h"
#include "commands.h"
#include "error_codes.h"

#define SIZE_OF(array)      (sizeof(array)/sizeof(*array))



#define MENU_EXIT            0
#define MENU_PING            1
#define MENU_3V3_ON              2
#define MENU_3V3_OFF             3
#define MENU_2V5_ON              4
#define MENU_2V5_OFF             5
#define MENU_EMERGENCY_SHUTDOWN 6
#define MENU_HEALTH_CHECK 7
#define MENU_DUTY_33 8
#define MENU_DUTY_25 9
#define MENU_DATA_STREAM 10
#define MENU_CONTROLLER1_TOGGLE_ON 11
#define MENU_CONTROLLER1_TOGGLE_OFF 12
#define MENU_CONTROLLER2_TOGGLE_ON 13
#define MENU_CONTROLLER2_TOGGLE_OFF 14

#define MAX_COUNT_1 180
#define MAX_COUNT_2 257



void clearStream(void);

uint8_t error;
static const char *MENU[] = {"Exit",
															"Comms Check",
															"3.3 V Supply On",
															"3.3 V Supply Off",
															"2.5 V Supply On",
															"2.5 V Supply Off",
															"Converter Emergency Shutdown",
															"Read Health Data",
															"Duty Change 1",
															"Duty Change 2",
															"Start Data Stream",
															"Controller 1 ON",
															"Controller 1 OFF",
															"Controller 2 ON",
															"Controller 2 OFF"
															};




int menu(void);

int main()
{
	serial_init();

	printf("\n\n---Synchronous Buck Interface Terminal--\n\n");

	int run =1;
	while(run)
	{
		if(menu() < 1)
		{
			run = 0;
		}
	}
	return 0;
}


int menu()
{
		int input = -1;
		printf("\nActions: \n");
		for (int i = 0; i < (signed) SIZE_OF(MENU); i++)
		{
			printf("\t%d) %s\n",i,MENU[i]);
		}
		printf("Selection > ");

		char term;
		if(scanf("%d%c", &input, &term) != 2 || term != '\n')
		{
				printf("\nPlease enter an integer value\n");
				clearStream();
				return 1;
		}

		if(input < 0 || input >= (signed) SIZE_OF(MENU))
		{
        printf("\nPlease input a valid menu option\n");
        return 1;
    }
		printf("\n----------------\n");

		switch(input)
		{
        case MENU_EXIT:
										{
											exit_ser();
											break;
										}

				case MENU_PING:
										{
                    	error = ping();
                    	if(error == PING_SUCCESS)
												{
                        	printf("MCU : ping success\n");
                      	}
												else
												{
                      		printf("MCU : ping failed\terror code: %i\n",(int)error);
                    		}
                    		break;
										}
				case MENU_3V3_ON:
										{
						            Con_Toggle(ON,V3_3);
						            break;
						        }
				case MENU_3V3_OFF:
										{
						            Con_Toggle(OFF,V3_3);
						            break;
										}
				case MENU_2V5_ON:
										{
												Con_Toggle(ON,V2_5);
												break;
										}
				case MENU_2V5_OFF:
										{
												Con_Toggle(OFF,V2_5);
												break;
										}
				case MENU_EMERGENCY_SHUTDOWN:
										{
												Con_Toggle(OFF,V2_5);
												Con_Toggle(OFF,V3_3);
											break;
										}
				case MENU_HEALTH_CHECK:
										{
											con_health_check();
											break;
										}
				case MENU_DUTY_33:
										{
											int duty;
											uint16_t count;
											uint16_t error_code;
											printf("Enter Duty: ");
											scanf("%d",&duty);
											printf("Duty: %d\n",duty);
											count = ((MAX_COUNT_1*duty)/100);
											printf("Calculated Count: %d",(int)count);
											Duty_change(V3_3, count);
											if(error_code != NO_ERRORS)
											{
												printf("ERROR CODE: %d\n", error_code);
											}
											break;
										}
				case MENU_DUTY_25:
										{
											int duty;
											uint16_t count;
											uint16_t error_code;
											printf("Enter Duty: ");
											scanf("%d",&duty);
											count = ((MAX_COUNT_2*duty)/100);
											printf("Calculated Count: %d",(int)count);
											error_code = Duty_change(V2_5, count);
											if(error_code != NO_ERRORS)
											{
												printf("ERROR CODE: %d\n", error_code);
											}
											break;
										}
					case MENU_DATA_STREAM:
										{
											start_stream();
											break;
										}
					case MENU_CONTROLLER1_TOGGLE_ON:
										{
												Controller_Toggle(ON,V3_3);
												break;
										}
					case MENU_CONTROLLER1_TOGGLE_OFF:
										{
												Controller_Toggle(OFF,V3_3);
												break;
										}
					case MENU_CONTROLLER2_TOGGLE_ON:
										{
												Controller_Toggle(ON,V2_5);
												break;
										}
					case MENU_CONTROLLER2_TOGGLE_OFF:
										{
												Controller_Toggle(OFF,V2_5);
												break;
										}
				default:printf("Invalid selection\n");
    }

    return input;

	}

void clearStream()
{
    char c;
    while((c = getchar()) != '\n') {}
}
