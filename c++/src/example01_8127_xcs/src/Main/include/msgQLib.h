/*msgQLib Linux version create sys */

#ifndef	_msgQLib_h__
#define	_msgQLib_h__


#include <semaphore.h>
#include <pthread.h>

typedef struct tagMsgQueue
{
	int maxMsgs;
	int maxMsgLength;
	int options;

	sem_t semCount;
	pthread_mutex_t muxOP;

	int bufUnitCount;
	int bufUnitLen;
	int totalBufLen;
	char *pBuf;
	int nRBufUnit;
	int nWBufUnit;

}MSGQ,*MSG_Q_ID;

#ifdef __cplusplus
extern "C" {
#endif

/* msgQLib Related */
/* create and initialize a message queue
 * Param:
 *  maxMsgs -- max messages that can be queued ,MAX = 0xffff
 *	maxMsgLength -- max bytes in a message ,MAX = 0xffff,(maxMsgs*maxMsgLength)<=0x1000000
 *	options -- message queue options ,default is 0,now not used
 * Return Value:  
 *   MSG_Q_ID, or NULL if error.
 */
MSG_Q_ID msgQCreate( int maxMsgs,int maxMsgLength,int options );

/* delete a message queue
 * Param:
 *  msgQId -- message queue to delete  
 * Return Value:  
 *   (1) on success or (-1) otherwise.
 */
int msgQDelete( MSG_Q_ID msgQId );

/* send a message to a message queue
 * Param:
 *  msgQId -- message queue on which to send   
 *  buffer -- message to send   
 *  nBytes -- length of message 
 *  timeout -- time(ms) to wait,now not used  
 * Return Value:  
 *   (-1) failed or sended Bytes otherwise.
 */
int msgQSend( MSG_Q_ID msgQId,char *buffer,int nBytes,int timeout );

/* receive a message from a message queue
 * Param:
 *  msgQId -- message queue from which to receive 
 *  buffer -- buffer to receive message 
 *  maxNBytes -- length of buffer  
 *  timeout -- time(ms) to wait  
 * Return Value:  
 *   The number of bytes copied to buffer or (-1) failed.
 */
int msgQReceive( MSG_Q_ID msgQId,char *buffer,int maxNBytes,int timeout );

/* get the number of messages queued to a message queue
 * Param:
 *  msgQId -- message queue to examine  
 * Return Value:  
 *   The number of messages queued or (-1) failed.
 */
int msgQNumMsgs( MSG_Q_ID msgQId );

#ifdef __cplusplus
}
#endif

#endif /* define _msgQLib_h__*/
