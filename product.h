
/*
*product.h
*create by qinghuahan 2016/11/17
*/


#ifndef __PRODUCT_H_
#define __PRODUCT_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <syslog.h>
#include <getopt.h>
#include <time.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netinet/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <netdb.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <endian.h>
#include <ifaddrs.h>


#include "list.h"
#include "spinlock.h"
#include "thread.h"

typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   u_int;
typedef unsigned long  u_long;


#define pkt_swapu16(n)  ((((n) & 0x00ff) << 8) | (((n) & 0xff00) >> 8)) //ntohs()
#define pkt_swapu32(n)   ((((n) & 0xff000000) >> 24) | (((n) & 0x000000ff) << 24) | (((n) & 0x00ff0000) >> 8) | (((n) & 0x0000ff00) << 8))

typedef enum _en_DIR
{
	en_UNKNOWN = 0,
	en_IN,
	en_OUT,
	en_DIR_MAX
}en_DIR;

typedef enum _EN_VHASH_MOUDLE_ID
{
	en_PORT = 0,
	en_VHASH_MOUDLE_MAX
}EN_VHASH_MOUDLE_ID;

#define INVALID_OFFSET ((unsigned int)(-1))


#endif //__PRODUCT_H_


