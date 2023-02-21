#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
//#include <conio.h> /*for clrscr()*/
#include <ncurses.h>
#include "commands.h"
#include "serial.h"
#include "error_codes.h"
#include <time.h>



void parse_stareg(uint8_t val, int converter);
void delay(int number_of_seconds);
/*
Desc: This command checks the
*/
int ping()
{
    uint8_t txVal = PING;
    uint8_t rxVal;

    if(serial_write(&txVal, 1) != SERIAL_WRITE_SUCCESS)
    {
        printf("ERROR : Serial write failed\n");
        return SERIAL_WRITE_FAIL;
    }

    if(serial_read(&rxVal, 1) != SERIAL_READ_SUCCESS) {
        printf("ERROR : Serial read failed\n");
        return SERIAL_READ_FAIL;
    }

    printf("Returned:  0x%02X\n", rxVal);

    if(rxVal == PING_RESPONSE) {
        return PING_SUCCESS;
    } else {
        return PING_FAIL;
    }
}

int Con_Toggle(int select, int converter)
{
    uint8_t txBuf[2];
    uint8_t rxVal;
    if(converter == V3_3)
    {
      txBuf[0] = V33_TOGGLE;


      if(select == OFF)
      {
          txBuf[1] = 0x00;
      }
      else if(select == ON)
      {
        txBuf[1] = 0x01;
      }
      else
      {
        printf("ERROR : invalid input\n");
        return -1;
      }
    }
    else if(converter == V2_5)
    {
      txBuf[0] = V25_TOGGLE;
      //uint8_t rxVal;

      if(select == OFF)
      {
          txBuf[1] = 0x00;
      }
      else if(select == ON)
      {
        txBuf[1] = 0x01;
      }
      else
      {
        printf("ERROR : invalid input\n");
        return -1;
      }
    }

    if(serial_write(txBuf, 2) != SERIAL_WRITE_SUCCESS)
    {
        printf("ERROR : Serial write failed\n");
        return SERIAL_WRITE_FAIL;
    }
    else
    {
        printf("\nSENT: txBuf[0]: 0x%02X, txBuf[1]: 0x%02X\n",txBuf[0],txBuf[1]);
    }

    //usleep(1000);
/*
    if(read(file, &rxVal, 1) != 1) {
        printf("ERROR : i2c read failed\n");
        return -3;
    }

    printf("Returned: %i\n", rxVal);
*/
    return 0;
}

void con_health_check(void)
{
  uint8_t txVal = HEALTH;
  uint8_t rxVal[10];
  uint8_t error = NO_ERRORS;


  if(serial_write(&txVal, 1) != SERIAL_WRITE_SUCCESS)
  {
      printf("ERROR : Serial write failed\n");
      error = SERIAL_WRITE_FAIL;
  }

  if(serial_read(rxVal, 10) != SERIAL_READ_SUCCESS)
  {
      printf("ERROR : Serial read failed\n");
      error = SERIAL_READ_FAIL;
  }
  //printf("rxVal[0] = %02X\trxVal[1] = %02X\n",rxVal[0],rxVal[1]);
  if(error == NO_ERRORS)
  {
    parse_stareg(rxVal[0],1);
    parse_stareg(rxVal[1],2);
    printf("\n\n***Online Parameters***\n");
    printf("Converter 1 MOSFET Temperature = %d\n", (int)(rxVal[2]<<8)|(rxVal[3]));
    printf("Converter 2 MOSFET Temperature = %d\n", (int)(rxVal[4]<<8)|(rxVal[5]));
    printf("Converter 1 Output Voltage = %f V\n", (float)(((rxVal[6]<<8)|(rxVal[7]))*(3.3/1024)));
    printf("Converter 2 Output Voltage = %f V\n", (float)(((rxVal[8]<<8)|(rxVal[9]))*(3.3/1024)));
  }

}

uint8_t Duty_change(uint8_t converter, uint16_t value)
{
  uint8_t txBuf[3];
  uint8_t rxVal;
  if(converter == V3_3)
  {
    txBuf[0] = DUTY33;
    txBuf[1] = (value>>8); //store hi byte in the next buffer location
    txBuf[2] = (uint8_t)value;//store lo byte
  }
  else if(converter == V2_5)
  {
    txBuf[0] = DUTY25;
    txBuf[1] = (value>>8); //store hi byte in the next buffer location
    txBuf[2] = (uint8_t)value;//store lo byte
  }

  if(serial_write(txBuf, 3) != SERIAL_WRITE_SUCCESS)
  {
      printf("ERROR : Serial write failed\n");
      return SERIAL_WRITE_FAIL;
  }
  else
  {
      printf("\nSENT: txBuf[0]: 0x%02X, txBuf[1]: 0x%02X,, txBuf[2]: 0x%02X\n",txBuf[0],txBuf[1],txBuf[2]);
  }
}

void exit_ser(void)
{
  close_serial_port();

}


void parse_stareg(uint8_t val,int converter)
{
  printf("\n****----Parsed Data of Converter %i----****\n",converter);
    if(( val & 0x01) == 1)
    {
        printf("\nCon %d Status: Active",converter);
    }
    else
    {
        printf("\nCon %d Status: Inactive", converter);
    }
    if( ((val>>1) & 0x01) == 1)
    {
        printf("\nGate Driver %d MCU Set", converter);
    }
    else
    {
        printf("\nGate Driver %d MCU Not Set", converter);
    }
    if( ((val>>2) & 0x01) == 1)
    {
        printf("\nOvercurrent Event %d: True", converter);
    }
    else
    {
        printf("\nOvercurrent Event %d: False", converter);
    }
    if( ((val>>3) & 0x01) == 0)
    {
        printf("\nMOSFET Temperature %d: Nominal", converter);
    }
    else
    {
        printf("\nMOSFET Temperature %d: Abnormal", converter);
    }
    if( ((val>>4) & 0x01) == 0)
    {
        printf("\nController Mode %d: Open Loop", converter);
    }
    else
    {
        printf("\nController Mode %d: Closed Loop", converter);
    }
    if( (val>>5) & 0x01 == 1)
    {
        printf("\nGate Driver %d: Active", converter);
    }
    else
    {
        printf("\nGate Driver %d: Inactive", converter);
    }
    if((val>>6) & 0x01 == 1)
    {
        printf("\nReserved");
    }
    else
    {
        printf("\nReserved");
    }
    if((val>>7) & 0x01 == 1)
    {
        printf("\nReserved");
    }
    else
    {
        printf("\nReserved");
    }
}


/*
Streams data until it receives 'q'
*/
void start_stream(void)
{
    char c;

    /*
    while ((c != 'q'))
    {
      scanf("%c", &c);
      //clrscr(); //clear output screen
      printf("\e[1;1H\e[2J");
      delay(100);
      con_health_check();
      delay(100);
    }
    */

    //ncurses approach
/*
    initscr();
    printw("Hello World!\n");
    refresh();



    endwin();*/
    while(1)
    {
      con_health_check();
      delay(100);
    }
}

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds);
}


int Controller_Toggle(int select, int converter)
{
    uint8_t txBuf[2];
    uint8_t rxVal;
    if(converter == V3_3)
    {
      txBuf[0] = CONTROLLER1_TOGGLE;


      if(select == OFF)
      {
          txBuf[1] = 0x00;
      }
      else if(select == ON)
      {
        txBuf[1] = 0x01;
      }
      else
      {
        printf("ERROR : invalid input\n");
        return -1;
      }
    }
    else if(converter == V2_5)
    {
      txBuf[0] = CONTROLLER2_TOGGLE;
      //uint8_t rxVal;

      if(select == OFF)
      {
          txBuf[1] = 0x00;
      }
      else if(select == ON)
      {
        txBuf[1] = 0x01;
      }
      else
      {
        printf("ERROR : invalid input\n");
        return -1;
      }
    }

    if(serial_write(txBuf, 2) != SERIAL_WRITE_SUCCESS)
    {
        printf("ERROR : Serial write failed\n");
        return SERIAL_WRITE_FAIL;
    }
    else
    {
        printf("\nSENT: txBuf[0]: 0x%02X, txBuf[1]: 0x%02X\n",txBuf[0],txBuf[1]);
    }

    //usleep(1000);
/*
    if(read(file, &rxVal, 1) != 1) {
        printf("ERROR : i2c read failed\n");
        return -3;
    }

    printf("Returned: %i\n", rxVal);
*/
    return 0;
}
