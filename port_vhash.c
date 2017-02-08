
#include "product.h"
#include "vhash.h"
#include "port_vhash.h"

VHASH_UINT4 PORT_VHash_Calc(void *pkey);
VHASH_UINT4 PORT_VHash_Compare(void *pkey,void *pData);
VHASH_UINT4 PORT_VHash_CleanData(void *pData);


VHASH_UINT4 PORT_VHash_Calc(void *pkey)
{
	unsigned int uinum = 0;
	unsigned char *puiNum = (unsigned char *)pkey;
	unsigned int i; 	
	for(i = 0; i < 12; i++)
	{
		uinum ^= puiNum[i];	
	}
	
	return PORT_VHASH(uinum);
}



VHASH_UINT4 PORT_VHash_Compare(void *pkey,void *pData)
{
	PORT_ITEM *key = (PORT_ITEM *)pkey;
	PORT_VHASH_ITEM *pstVhashItem = (PORT_VHASH_ITEM *)pData;
	
	if(key->sip == pstVhashItem->stPortItem.sip && 
	   key->dip == pstVhashItem->stPortItem.dip &&
	   key->dport == pstVhashItem->stPortItem.dport &&
	   key->sport == pstVhashItem->stPortItem.sport)
	{
		return __VHASH_OK;
	}
	
	return __VHASH_ERR;
}

VHASH_UINT4 PORT_VHash_IsUpdate(void *pData,void *pNewData)
{
	return __VHASH_OK;
}

VHASH_UINT4 PORT_VHash_clean(void *pData)
{	
	return __VHASH_OK;
}

VHASH_UINT4 MAC_VHash_copy(void *pDestData,void *pSrcData,unsigned int uiLen)
{
	VHASH_UINT4 uiRet = __VHASH_OK;
	PORT_VHASH_ITEM *pstVhashItem = (PORT_VHASH_ITEM *)pDestData;
	PORT_ITEM *pstItem = (PORT_ITEM *)pSrcData;
	
	if (sizeof(PORT_VHASH_ITEM) > uiLen)
	{
		return __VHASH_ERR;
	}
	
	memcpy(&pstVhashItem->stPortItem, pstItem,sizeof(PORT_ITEM));

	return uiRet;
}



void PORT_VHash_INIT()
{
	VHASH_MOUDLE stvhm;
	memset(&stvhm,0,sizeof(VHASH_MOUDLE));
	stvhm.uiBucketCount = PORT_VHASH_NUM_HASHSIZE;
	stvhm.uiDataLen = sizeof(PORT_VHASH_ITEM);
	stvhm.pfnVHash_Calc = PORT_VHash_Calc;
	stvhm.pfnVHash_Compare = PORT_VHash_Compare;
	stvhm.pfnVHash_IsUpdate = PORT_VHash_IsUpdate;
	stvhm.pfnVHash_clean = PORT_VHash_clean;
	stvhm.pfnVHash_cpy = MAC_VHash_copy;
	VHash_Create_Moudle(en_PORT,&stvhm);
}

u_int PORT_VHash_Add(PORT_ITEM *pstItem,PORT_VHASH_ITEM **ppstVhashItem)
{
	PORT_VHASH_ITEM *pstVhashItem = NULL;
	VHASH_UINT4 vhash_ret;
	vhash_ret = VHash_Add_Node2(en_PORT,(void *)pstItem,(void *)pstItem,(void **)&pstVhashItem);
	if((__VHASH_OK == vhash_ret  || __VHASH_UPDATE_OK == vhash_ret ) && NULL != pstVhashItem && NULL != ppstVhashItem)
	{
		*ppstVhashItem = pstVhashItem;
	}
	return vhash_ret;
}

u_int PORT_VHash_Update(PORT_ITEM *pstItem,PORT_VHASH_ITEM **ppstVhashItem)
{
	PORT_VHASH_ITEM *pstVhashItem = NULL;
	VHASH_UINT4 vhash_ret;
	vhash_ret = VHash_Add_Node2(en_PORT,(void *)pstItem,(void *)pstItem,(void **)&pstVhashItem);
	if(__VHASH_UPDATE_OK == vhash_ret && NULL != pstVhashItem && NULL != ppstVhashItem)
	{
		*ppstVhashItem = pstVhashItem;
	}
	return vhash_ret;
}

u_int PORT_VHash_Del(PORT_ITEM *pstItem)
{
	return VHash_Del_Node(en_PORT,(void *)pstItem);
}



u_int PORT_VHash_FindStruct(PORT_ITEM *pstItem,PORT_VHASH_ITEM **ppstVhashItem)
{
	VHASH_UINT4 ulRet;
	ulRet = VHash_Get_Node(en_PORT,(void *)pstItem,(void **)ppstVhashItem);
	return ulRet;
}

u_int PORT_VHash_Get_NextNode(PORT_ITEM *pstItem,PORT_VHASH_ITEM *ppstVhashItem_prev, PORT_VHASH_ITEM **ppstVhashItem)
{
	VHASH_UINT4 ulRet;
	ulRet = VHash_Get_NextNode(en_PORT,(void *)pstItem,(void *)ppstVhashItem_prev,(void **)ppstVhashItem);
	return ulRet;
}

void PORT_VHash_Clean()
{
	VHash_Clean_Moudle(en_PORT);
}

void PORT_VHash_EXIT()
{
	VHash_Destory_Moudle(en_PORT);
}


void PORT_VHash_Lock()
{
	VHash_Lock(en_PORT);
}


void PORT_VHash_unLock()
{
	VHash_unLock(en_PORT);
}

u_int PORT_VHash_GetCount(unsigned int *puicount)
{
	return VHash_Node_Total(en_PORT,puicount);
}




