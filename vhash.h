
/*  vhash.h */

#ifndef _VHASH_H
#define _VHASH_H


#define __VHASH_OK  0
#define __VHASH_ERR  1
#define __VHASH_NOT_INNIF  2
#define __VHASH_NOT_FOUND  3
#define __VHASH_EXIST      4
#define __VHASH_INTERFACE_ERR  5
#define __VHASH_UPDATE_OK  6
#define __VHASH_ENUM_END  7



typedef unsigned int VHASH_UINT4;


typedef struct _tagHASH_MOUDLE
{
	unsigned int uiModeId;
	VHASH_UINT4 uiBucketCount;
	VHASH_UINT4 uiDataLen;
	VHASH_UINT4 (* pfnVHash_Calc)(void *pkey);
	VHASH_UINT4 (* pfnVHash_Compare)(void *pkey,void *pData);
	VHASH_UINT4 (* pfnVHash_IsUpdate)(void *pData,void *pNewData);
	VHASH_UINT4 (* pfnVHash_clean)(void *pData);
	VHASH_UINT4 (* pfnVHash_cpy)(void *pDestData,void *pSrcData,unsigned int uiLen);
	int (*pfnPrintf)(const char * format, ...);
	void *(*fncalloc)(unsigned int n,unsigned int uisize);
}VHASH_MOUDLE;


int VHash_Init(unsigned int uiMaxVhashModules);
VHASH_UINT4 VHash_Create_Moudle(unsigned int  en_moudle_id,VHASH_MOUDLE *pHashMoudle);
VHASH_UINT4 VHash_Destory_Moudle(unsigned int  en_moudle_id);
VHASH_UINT4 VHash_Clean_Moudle(unsigned int  en_moudle_id);
VHASH_UINT4 VHash_Get_Node(unsigned int  en_moudle_id,void *pKey,void **ppData);
VHASH_UINT4 VHash_Add_Node(unsigned int  en_moudle_id,void *pKey,void *pData);
VHASH_UINT4 VHash_Add_Node2(unsigned int  en_moudle_id,void *pKey,void *pData,void **ppNodeData);
VHASH_UINT4 VHash_Get_NextNode(unsigned int  en_moudle_id,void *pKey,void *pData,void **ppNodeData);
VHASH_UINT4 VHash_Del_Node(unsigned int  en_moudle_id,void *pKey);
VHASH_UINT4 VHash_Node_Stat(unsigned int  en_moudle_id,unsigned int *puiNodeStat);
VHASH_UINT4 VHash_Node_Total(unsigned int  en_moudle_id,unsigned int *puiTotal);
void VHash_Lock(unsigned int  en_moudle_id);
void VHash_unLock(unsigned int  en_moudle_id);





#endif

