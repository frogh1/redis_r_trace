/*
*port_vhash.h
*create by qinghuahan 2016/11/18
*/

#ifndef __PORT_VHASH_H__
#define __PORT_VHASH_H__

#define PORT_VHASH_NUM_HASHSIZE 0x800//hashtable size
#define PORT_VHASH(num) ((num) & (PORT_VHASH_NUM_HASHSIZE - 1))

typedef struct _tagPORT_ITEM
{
	u_short sport;
	u_short dport;
	u_int sip;
	u_int dip;
}PORT_ITEM;

typedef struct _tagPORT_VHASH_ITEM
{
	u_int sec;
	u_int u_sec;
	char cmd[32];
	PORT_ITEM stPortItem;
}PORT_VHASH_ITEM;

void PORT_VHash_INIT();
u_int PORT_VHash_Add(PORT_ITEM *pstItem,PORT_VHASH_ITEM **ppstVhashItem);
u_int PORT_VHash_Update(PORT_ITEM *pstItem,PORT_VHASH_ITEM **ppstVhashItem);
u_int PORT_VHash_Del(PORT_ITEM *pstItem);
u_int PORT_VHash_FindStruct(PORT_ITEM *pstItem,PORT_VHASH_ITEM **ppstVhashItem);
u_int PORT_VHash_Get_NextNode(PORT_ITEM *pstItem,PORT_VHASH_ITEM *ppstVhashItem_prev, PORT_VHASH_ITEM **ppstVhashItem);
void PORT_VHash_EXIT();
void PORT_VHash_Lock();
void PORT_VHash_unLock();
u_int PORT_VHash_GetCount(unsigned int *puicount);

void PORT_VHash_Clean();



#endif




