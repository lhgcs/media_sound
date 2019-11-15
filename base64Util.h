


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>		/* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <termios.h>  
#define  TEMP_STR_LEN  512    //1024
#define   NONE         0
#define   HARDWARE     1
#define   SOFTWARE     2

#include <QtDebug>

typedef struct
{
	unsigned int  groupnumber;
	unsigned int  bitnumber;
	unsigned int  value;
} gpio_groupbit_info;

typedef struct
{
	unsigned int  groupnumber;
	unsigned int  bitnumber;
	unsigned int  configuration_value;
} gpio_groupbit_config;

#define GPIO_FILE   "/dev/gpio"

#define GPIO_SET_DIR   0x1
#define GPIO_GET_DIR   0x2
#define GPIO_READ_BIT  0x3
#define GPIO_WRITE_BIT 0x4
#define IO_CONFIG      0x5


int set_gpio_configuration(int group, int bitnum, unsigned int configuration)
{
	int fd;
	gpio_groupbit_config GpioConfig;
	fd = open(GPIO_FILE, O_RDWR);
	if (fd < 0)
	{
		printf("open %s fail\n", GPIO_FILE);
		return 0;
	}
	GpioConfig.groupnumber = group;
	GpioConfig.bitnumber = bitnum;
	GpioConfig.configuration_value = configuration;
	//printf("setGpioConfig groupnumber=%d bitnumber=%d,configuration_value=%d\n",GpioConfig.groupnumber,GpioConfig.bitnumber,GpioConfig.configuration_value);
	ioctl(fd, IO_CONFIG, &GpioConfig);
	close(fd);
	return 1;
}

int set_gpio_dir_output(int group, int bitnum)
{
	int fd;
	gpio_groupbit_info Gpio;
	fd = open(GPIO_FILE, O_RDWR);
	if (fd < 0)
	{
		printf("open %s fail\n", GPIO_FILE);
		return 0;
	}
	Gpio.groupnumber = group;
	Gpio.bitnumber = bitnum;
	Gpio.value = 1;
	ioctl(fd, GPIO_SET_DIR, &Gpio);
	close(fd);
	return 1;
}
int set_gpio_dir_input(int group, int bitnum)
{
	int fd;
	gpio_groupbit_info Gpio;
	fd = open(GPIO_FILE, O_RDWR);
	if (fd < 0)
	{
		printf("open %s fail\n", GPIO_FILE);
		return 0;
	}
	Gpio.groupnumber = group;
	Gpio.bitnumber = bitnum;
	Gpio.value = 0;
	ioctl(fd, GPIO_SET_DIR, &Gpio);
	close(fd);
	return 1;
}
int set_gpio_value(int group, int bitnum, int value)
{
	int fd;
	gpio_groupbit_info Gpio;
	fd = open(GPIO_FILE, O_RDWR);
	if (fd < 0)
	{
		printf("open %s fail\n", GPIO_FILE);
		return 0;
	}
	Gpio.groupnumber = group;
	Gpio.bitnumber = bitnum;
	Gpio.value = value;
	ioctl(fd, GPIO_WRITE_BIT, &Gpio);
	close(fd);
	return 1;
}
int get_gpio_value(int group, int bitnum, int *value)
{
	int fd;
	gpio_groupbit_info Gpio;
	fd = open(GPIO_FILE, O_RDWR);
	if (fd < 0)
	{
		printf("open %s fail\n", GPIO_FILE);
		return 0;
	}
	Gpio.groupnumber = group;
	Gpio.bitnumber = bitnum;
	ioctl(fd, GPIO_READ_BIT, &Gpio);
	*value = Gpio.value;
	close(fd);
	return 1;
}


//static int
//FindStrFromBuf (const char *buf, int buflen, const char *findingstr,int findinglen, int direction)
//{
//  int i, j;
//  if (direction == 0)
//    {
//        for (i = 0; i < buflen - findinglen + 1; i++)
//        {
//            for (j = 0; j < findinglen; j++)
//            {
//                if (buf[i + j] != findingstr[j])
//                break;
//                if (j == findinglen - 1)
//                return i;
//            }
//        }
//    }
//  else
//    {
//        for (i = buflen - findinglen; i >= 0; i--)
//        {
//            for (j = 0; j < findinglen; j++)
//            {
//                if (buf[i + j] != findingstr[j])
//                break;
//                if (j == findinglen - 1)
//                return i;
//            }
//        }
//    }
//  return -1;
//}

int set_uart1_read()
{
    int group = 9;
    int bitnum = 0;
    int value = 0;
    if (!set_gpio_dir_output(group, bitnum))
    {
    printf("Can't set gpio to input\n");
    return -1;
    }

     if (!set_gpio_value(group, bitnum, value))
     {
	 printf("Can't set value\n");
	 return -1;
     }
     return 0;
}


/* Table of CRC constants - implements x^16+x^12+x^5+1 */
static  int crc16_tab[] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
};

//把CRC码的高字节(8bit)全部移出，与一个byte的数据做XOR运算，
//根据运算结果来选择一个值(称为余式)，与原来的CRC码再做一次XOR运算，就可以得到新的CRC码
short cyg_crc16(unsigned char *buf, int len)//数据地址，数据长度,返回校验值
{
    int  i     = 0;
    short cksum = 0;

    cksum = 0;

	*buf++;

    for (i = 0;  i < len-1;  i++)
    {
	    cksum = crc16_tab[((cksum>>8) ^ (*buf++ & 0xFF)) & 0xFF] ^ (cksum << 8);
    }

    return cksum;
}


int send_cmd(int fd, unsigned char c1, unsigned char c2)
{
	int i = 0;

	unsigned char cmd[8]={0xA5, 0x05, 0x01, 0x82, 0x1F, 0x00, 0x00, 0x00};
    char temp[100]="";

	cmd[2] = c1;
	cmd[5] = c2;
	cmd[6] = cyg_crc16(cmd, 6)>>8;
	cmd[7] = cyg_crc16(cmd, 6)&0xff;

    printf("Start send data: \n\t");

	for(i=0; i<8; i++)
	{
		//printf("%d,", cmd[i]);
        sprintf(temp,"%s,0x%02x", temp, cmd[i]);
	}
        printf("%s", temp);

    //只保证数据成功发送出去，不保证对方成功接收，所以发送3次
    write(fd, cmd, sizeof(cmd));
    usleep(100);
    write(fd, cmd, sizeof(cmd));
    usleep(100);
    write(fd, cmd, sizeof(cmd));
    usleep(100);

    char str[30]={0};
    sprintf(str,"open box:%d,%d\n",c1,c2);
    printf("%s",str);
    qDebug(str);

	return 0;
}


#define B64_DEF_LINE_SIZE   72
#define B64_MIN_LINE_SIZE    4

static const char
  cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char
  cd64[] =
  "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";


static void
encodeblock (unsigned char in[3], unsigned char out[4], int len)
{
  out[0] = cb64[in[0] >> 2];
  out[1] = cb64[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];
  out[2] =
    (unsigned char) (len >
		     1 ? cb64[((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6)] :
		     '=');
  out[3] = (unsigned char) (len > 2 ? cb64[in[2] & 0x3f] : '=');
}

static int
base64_encode_str (unsigned char *instr, int inlen, unsigned char *outstr)
{
  unsigned char in[3], out[4];
  int inc = 0, outc = 0, i, len;

  /* only size of buffer required */
    if (!outstr)
    {
        while (inc < inlen)
        {
            len = 0;
            for (i = 0; i < 3; i++)
            {
                if (inc++ < inlen)
                {
                  len++;
                }
            }
            if (len)
            {
              outc += 4;
            }
        }
        return outc;
    }

  //assert(outstr);   
    while (inc < inlen)
    {
        len = 0;
        for (i = 0; i < 3; i++)
        {
            in[i] = instr[inc];
            if (inc++ < inlen)
            {
                len++;
            }
            else
            {
                in[i] = 0;
            }
        }
        if (len)
        {
            encodeblock (in, out, len);
            for (i = 0; i < 4; i++)
            {
                outstr[outc++] = out[i];
            }
        }
    }
    return outc;
}

static void
decodeblock (unsigned char in[4], unsigned char out[3])
{
  out[0] = (unsigned char) (in[0] << 2 | in[1] >> 4);
  out[1] = (unsigned char) (in[1] << 4 | in[2] >> 2);
  out[2] = (unsigned char) (((in[2] << 6) & 0xc0) | in[3]);
}

static int
base64_decode_str (const unsigned char *instr, int inlen,
		   unsigned char *outstr)
{
  unsigned char in[4], out[3], v;
  int inc = 0, outc = 0, i, len;

  /* only size of buffer required */
  if (!outstr)
    {
      while (inc < inlen)
	{
	  for (len = 0, i = 0; i < 4 && inc < inlen; i++)
	    {
	      v = 0;
	      while ((inc < inlen) && (v == 0))
		{
		  v = instr[inc++];
		  v =
		    (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[v - 43]);
		  if (v)
		    {
		      v = (unsigned char) ((v == '$') ? 0 : v - 61);
		    }
	      } if (inc < inlen)
		{
		  len++;
		}
	    }
	  if (len > 1)
	    {
	      outc += (len - 1);
	    }
	}
      return outc;
    }

  //assert(outstr);   
  while (inc < inlen)
    {
      for (len = 0, i = 0; i < 4 && inc < inlen; i++)
	{
	  v = 0;
	  while ((inc < inlen) && (v == 0))
	    {
	      v = instr[inc++];
	      v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[v - 43]);
	      if (v)
		{
		  v = (unsigned char) ((v == '$') ? 0 : v - 61);
		}
	  } if (inc < inlen)
	    {
	      len++;
	      if (v)
		{
		  in[i] = (unsigned char) (v - 1);
		}
	    }

	  else
	    {
	      in[i] = 0;
	    }
	}
      if (len)
	{
	  decodeblock (in, out);
	  for (i = 0; i < len - 1; i++)
	    {
	      outstr[outc++] = out[i];
	    }
	}
    }
  return outc;
}


 