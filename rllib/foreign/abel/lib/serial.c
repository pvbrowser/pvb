#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

typedef unsigned char byte;

#define NUL 0x00
#define DLE 0x10
#define STX 0x02
#define SOH 0x01
#define ETX 0X03
#define ENQ 0x05
#define ACK 0x06
#define NAK 0x0f

#define FALSE 0
#define TRUE 1


speed_t bauds[] = {B38400, B19200, B9600, B2400, B1200, B300};
int baud_count=6;

byte *baudstring[] ={"38400","19200","9600","2400","1200","300"};


int calc_crc (int crc, int buffer)
{
int temp1,y;

printf ("buffer[x] = %02X\n",buffer);
temp1 = crc ^ buffer;
crc = (crc & 0xff00) | (temp1 & 0xff);
for (y=0;y<8;y++)
  {
  if (crc & 1)
    {
    crc = crc >> 1;
    crc ^= 0xa001;
    }
  else
    {
    crc = crc >> 1;
    }
  }
return crc;
}

unsigned short calculatecrc ( byte *buffer)
{
unsigned long x,y,crc,temp1;
int end;
end = FALSE;
x=0;
crc=0;
while (end == FALSE)
  {
  if (buffer[x] == DLE)
    {
    printf ("DLE found in stream\n");
    switch (buffer[x+1])
      {
      case ENQ:
        printf ("DLE ENQ Found.\n");
        x++;
        end = TRUE;
        break;
      case DLE:
        printf ("DLE DLE Found.\n");
        x++;
        break;
      case ETX:
        printf ("DLE ETX Found.\n");
        x++;
        end = TRUE;
        break;
      case STX:
        if (x==0)
          x=2;
        printf ("DLE STX Found.\n");
        break;
      }
    }

  if ((end == FALSE) || (buffer[x] != ENQ))
    crc = calc_crc(crc,buffer[x]);
  x++;
  }

return (crc);
}





int opencom (char *comdev, struct termios com)
{
int file1=open(comdev,O_RDWR|O_NOCTTY|O_NONBLOCK);
if (file1 < 0)
  { 
  return (-1);
  }
bzero (&com,sizeof(com));
com.c_cflag = CS8 | CREAD | CLOCAL;
com.c_iflag= IGNPAR;
com.c_cc[VTIME]=100;
com.c_cc[VMIN]=255;
cfsetispeed (&com,B19200);
cfsetospeed (&com,B19200);
tcflush (file1,TCIOFLUSH);
tcsetattr(file1,TCSANOW,&com);
return (file1);
}

void setbaud (int speed, int file, struct termios com)
{
printf ("Setting baud rate to %s\n",baudstring[speed]);
cfsetispeed (&com, bauds[speed]);
cfsetospeed (&com, bauds[speed]);
tcflush (file, TCIOFLUSH);
usleep(100000);
tcsetattr (file, TCSANOW, &com);
usleep(100000);
return;
}

#ifdef DAS_MACHT_MAN_NICHT
int main (void)
{
struct termios com;

byte outbuf[512];
byte inbuf[512];
int return_len,x,y;
unsigned short crc;
struct timeval timeout;
fd_set set1;

int file=opencom("/dev/ttyS0",com);

crc=0;

if (file == -1)
  {
  printf ("OpenCom Failed\n");
  return (-1);
  }
usleep(150000);
//for (y=0;y<baud_count;y++)
//  {

//  setbaud (y,file,com);
  usleep (10000);
//  printf ("Baud rate set to %s\n",baudstring[y]);
  outbuf[0]=DLE;
  outbuf[1]=STX;
  outbuf[2]=0x01;
  outbuf[3]=0x00;
  outbuf[4]=0x06;
  outbuf[5]=0x00;
  outbuf[6]=0x02;
  outbuf[7]=0x08;
  outbuf[8]=0x03;
  outbuf[9]=DLE;
  outbuf[10]=ETX;
/*  outbuf[11]=0x00;
  outbuf[12]=0x00;
  outbuf[13]=0x00;
  outbuf[14]=0x00;
  outbuf[15]=0x00;
  outbuf[16]=0x00;
  outbuf[17]=0x00;
  outbuf[18]=0x00;
  outbuf[19]=0x00;
  outbuf[20]=DLE;
  outbuf[21]=ETX; */
  crc=calculatecrc(outbuf);
printf ("CRC return value = %04X\n",crc);

  outbuf[11]=(crc & 255);
  outbuf[12]=(crc >> 8);

  for (x=0;x<13;x++)
    printf ("[%02X]",outbuf[x]);
  printf ("\n");

  FD_ZERO(&set1);
  FD_SET(file, &set1);
  timeout.tv_sec=0;
  timeout.tv_usec=250000;
  write (file, &outbuf,13);
printf ("Data sent...\n");
  return_len=0;
//  if (select(file,&set1,NULL,NULL,&timeout) != 0)
//    {
//    printf ("Got something back");
    return_len = read (file, &inbuf,512);
//    }

  if (return_len == 1)
    return_len = read (file, &inbuf, 512);

   switch (return_len)
    {
    case 0:
      printf ("No Data returned from PLC\n");
      break;
    default:
      printf ("Data Received...\n");
      for (x=0;x<return_len;x++);
        printf ("%02X ",inbuf[x]);
      printf ("\n\n");
    }
//  }

close (file);
return 0;
}
#endif


















































