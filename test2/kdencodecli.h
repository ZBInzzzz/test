#ifndef __KDENCODECLI_H__
#define __KDENCODECLI_H__

/*************************************
Version : 1.0.0.3
*************************************/

// 加密方式定义
#define NOT_ENCODE        0			// 不加密
#define DES_ENCODE        1			// DES
#define MD5_ENCODE        2			// MD5(不可逆)
#define RIJNDAEL_ENCODE   3			// AES
#define BASE64_ENCODE     4			// BASE64
#define KDSIMPLE_ENCODE   5			// 金证简单加密
#define KDCOMPLEX_ENCODE  6			// 金证复合加密
#define GMSM4_ENCODE	  7			// 国密算法(SM4)

// KD2HsReEncode加密方式定义
#define KD2HS_NORMAL      0			// 标准的Blowfish加密方式
#define KD2HS_HS		  1			// 特殊的Blowfish加密方式


#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
KDEncode                   加密
参数:
int nEncode_Level          加密级别 = KDCOMPLEX_ENCODE
unsigned char *pSrcData    需要加密的源文
int nSrcDataLen            需要加密源文的长度
unsigned char *pDestData   密文缓冲区
int nDestDataBufLen        密文缓冲区的大小
void *pKey                 密钥
int nKeyLen                密钥的长度

返回值:                    －1为错误，成功为生成密文的长度

注: 加密级别 = KDCOMPLEX_ENCODE 时
密文缓冲区的大小           nDestDataBufLen = 2*nSrcDataLen(加密源文的长度)并且nDestDataBufLen>= 32，生成密文为0x00结尾ASCII字符串
***********************************************************/
int KDEncode(int nEncode_Level, 
			 unsigned char* pSrcData,  int nSrcDataLen, 
			 unsigned char* pDestData, int nDestDataBufLen, 
			 void*          pKey,      int nKeyLen);



/***********************************************************
KDReEncode                 重新加密
参数:
unsigned char *pSrcData    需要加密的源文
int nSrcDataLen            需要加密源文的长度
unsigned char *pDestData   密文缓冲区
int nDestDataBufLen        密文缓冲区的大小
int nOldEncode_Level       旧加密级别 = KDCOMPLEX_ENCODE
void *pOldKey              旧密钥
int nOldKeyLen             旧密钥的长度
int nOldEncode_Level       新加密级别 = KDCOMPLEX_ENCODE
void *pNewKey              新密钥
int nNewKeyLen             新密钥的长度

返回值:                    －1为错误，成功为生成新密文的长度

注: 加密级别 = KDCOMPLEX_ENCODE 时
密文缓冲区的大小           nDestDataBufLen = 2*nSrcDataLen(加密源文的长度)并且nDestDataBufLen>= 32，生成密文为0x00结尾ASCII字符串
***********************************************************/
int KDReEncode(unsigned char* pSrcData,  int   nSrcDataLen, 
			   unsigned char* pDestData, int   nDestDataBufLen, 
			   int nOldEncode_Level,     void* pOldKey, int nOldKeyLen, 
			   int nNewEncode_Level,     void* pNewKey, int nNewKeyLen);



/***********************************************************
KDEncrypt                  加密
参数:
int nEncode_Level          加密级别 = DES_ENCODE, RIJNDAEL_ENCODE, BASE64_ENCODE, KDSIMPLE_ENCODE, GMSM4_ENCODE
unsigned char *pSrcData    需要加密的源文
int nSrcDataLen            需要加密源文的长度
unsigned char *pDestData   密文缓冲区
int nDestDataBufLen        密文缓冲区的大小
void *pKey                 密钥
int nKeyLen                密钥的长度

返回值:                    －1为错误，成功为生成密文的长度
***********************************************************/
int KDEncrypt(int nEncode_Level, 
			  unsigned char* pSrcData,  int nSrcDataLen, 
			  unsigned char* pDestData, int nDestDataBufLen, 
			  void*          pKey,      int nKeyLen);


/***********************************************************
KDDecrypt                  解密(和KDEncrypt为一对)
参数:
int nEncode_Level          加密级别 = DES_ENCODE, RIJNDAEL_ENCODE, KDSIMPLE_ENCODE, GMSM4_ENCODE
unsigned char *pSrcData    需要解密的密文
int nSrcDataLen            需要解密密文的长度
unsigned char *pDestData   源文缓冲区
int nDestDataBufLen        源文缓冲区的大小
void *pKey                 密钥
int nKeyLen                密钥的长度

返回值:                    －1为错误，成功为源文的长度
***********************************************************/
int KDDecrypt(int nEncode_Level, 
			  unsigned char* pSrcData,  int nSrcDataLen, 
			  unsigned char* pDestData, int nDestDataBufLen, 
			  void*          pKey,      int nKeyLen);


/***********************************************************
KDEncryptText              加密(密文为可见字符)
参数:
int nEncode_Level          加密级别 = DES_ENCODE, RIJNDAEL_ENCODE, BASE64_ENCODE, KDSIMPLE_ENCODE, GMSM4_ENCODE
unsigned char *pSrcData    需要加密的源文
int nSrcDataLen            需要加密源文的长度
unsigned char *pDestData   密文缓冲区
int nDestDataBufLen        密文缓冲区的大小
void *pKey                 密钥
int nKeyLen                密钥的长度

返回值:                    －1为错误，成功为生成密文的长度
***********************************************************/
int KDEncryptText(int nEncode_Level, 
			      unsigned char* pSrcData,  int nSrcDataLen, 
			      unsigned char* pDestData, int nDestDataBufLen, 
			      void*          pKey,      int nKeyLen);


/***********************************************************
KDDecryptText              解密(和KDEncryptText为一对)
参数:
int nEncode_Level          加密级别 = DES_ENCODE, RIJNDAEL_ENCODE, KDSIMPLE_ENCODE, GMSM4_ENCODE
unsigned char *pSrcData    需要解密的密文
int nSrcDataLen            需要解密密文的长度
unsigned char *pDestData   源文缓冲区
int nDestDataBufLen        源文缓冲区的大小
void *pKey                 密钥
int nKeyLen                密钥的长度

返回值:                    －1为错误，成功为源文的长度
***********************************************************/
int KDDecryptText(int nEncode_Level, 
			      unsigned char* pSrcData,  int nSrcDataLen, 
			      unsigned char* pDestData, int nDestDataBufLen, 
			      void*          pKey,      int nKeyLen);

/***********************************************************
KD2HsReEncode              重新加密(使用Blowfish方式)
参数:
int nEncodeType            加密方式，KD2HS_NORMAL为标准Blowfish，KD2HS_HS为特殊Blowfish
unsigned char *pSrcData    需要加密的源文
int nSrcDataLen            需要加密源文的长度
unsigned char *pDestData   密文缓冲区
int nDestDataBufLen        密文缓冲区的大小
int nOldEncode_Level       旧加密级别 = KDCOMPLEX_ENCODE
void *pOldKey              旧密钥
int nOldKeyLen             旧密钥的长度

返回值:                    －1为错误，成功为生成新密文的长度
***********************************************************/
int KD2HsReEncode(int nEncodeType,
				  unsigned char *pSrcData, int nSrcDataLen, 
				  unsigned char *pDestData, int nDestDataBufLen, 
				  int nOldEncode_Level, void *pOldKey, int nOldKeyLen);

int KD2HsReEncodeExt(int nEncodeType,
				  unsigned char *pSrcData, int nSrcDataLen, 
				  unsigned char *pDestData, int nDestDataBufLen, 
				  int nOldEncode_Level, void *pOldKey, int nOldKeyLen, void *pHsKey = NULL);


/***********************************************************
KD2XnReEncode              重新加密(使用携宁自定义加密方式)
参数:
unsigned char *pSrcData    需要加密的源文
int nSrcDataLen            需要加密源文的长度
unsigned char *pDestData   密文缓冲区
int nDestDataBufLen        密文缓冲区的大小
int nOldEncode_Level       旧加密级别 = KDCOMPLEX_ENCODE
void *pOldKey              旧密钥
int nOldKeyLen             旧密钥的长度

返回值:                    －1为错误，成功为生成新密文的长度
***********************************************************/
int KD2XnReEncode(unsigned char *pSrcData, int nSrcDataLen, 
				  unsigned char *pDestData, int nDestDataBufLen, 
				  int nOldEncode_Level, void *pOldKey, int nOldKeyLen);

#ifdef __cplusplus
}
#endif

#endif
