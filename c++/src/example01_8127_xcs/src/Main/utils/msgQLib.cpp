/*msgQLib Linux version create 2007.6.12 by caoyf*/


#include "msgQLib.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>


MSG_Q_ID msgQCreate
    (
    int maxMsgs,              /* max messages that can be queued */
    int maxMsgLength,         /* max bytes in a message */
    int options               /* message queue options */
    )
{
	MSG_Q_ID msgQId=NULL;
	int ret = 0;

	if((maxMsgs<=0)||(maxMsgs>0xffff)||(maxMsgLength<0)||(maxMsgLength>0xffff)\
					||(maxMsgs*maxMsgLength>0x1000000))
	{
		printf("ERROR:msgQCreate() parameter error.\n");
		goto error_ret;
	}
	
	msgQId = new MSGQ;
	if(NULL==msgQId)
	{
		goto error_ret;
	}

	memset(msgQId,0x00,sizeof(MSGQ));

	msgQId->maxMsgs = maxMsgs;
	msgQId->maxMsgLength = maxMsgLength;
	msgQId->options = options;
	
	ret = sem_init(&(msgQId->semCount),0,0);
	if(0!=ret)
	{
		goto error_ret;
	}
	
	ret = pthread_mutex_init(&(msgQId->muxOP),NULL);
	if(0!=ret)
	{
		goto error_ret;
	}

	msgQId->bufUnitCount = maxMsgs+1;
	msgQId->bufUnitLen = maxMsgLength+sizeof(int);
	msgQId->totalBufLen = (msgQId->bufUnitCount)*(msgQId->bufUnitLen);
	msgQId->pBuf = (char *)(new int[msgQId->totalBufLen/4]);
	if(NULL==msgQId->pBuf)
	{
		goto error_ret;
	}
	msgQId->nRBufUnit = 0;
	msgQId->nWBufUnit = 0;
	
	return msgQId;

error_ret:
	if(msgQId)
	{
		if(msgQId->pBuf)
		{
			delete(msgQId->pBuf);
		}
	
		pthread_mutex_destroy(&(msgQId->muxOP));
		sem_destroy(&(msgQId->semCount));

		delete(msgQId);
	}

	return NULL;
}

int msgQDelete
    (
    MSG_Q_ID msgQId           /* message queue to delete */
    )
{
	if(NULL==msgQId)
	{
		return (-1);
	}

	pthread_mutex_lock(&(msgQId->muxOP));
	
	sem_destroy(&(msgQId->semCount));

	if(msgQId->pBuf)
	{
		delete(msgQId->pBuf);
	}
	msgQId->pBuf = NULL;

	msgQId->nRBufUnit = 0;
	msgQId->nWBufUnit = 0;

	pthread_mutex_unlock(&(msgQId->muxOP));

	pthread_mutex_destroy(&(msgQId->muxOP));

	delete(msgQId);

	return (1);	
}

int msgQSend
    (
    MSG_Q_ID msgQId,          /* message queue on which to send */
    char *   buffer,          /* message to send */
    int      nBytes,          /* length of message */
    int      timeout         /* time(ms) to wait */
    )
{
	int ret;
	char *pBuf=NULL;
	
	if((NULL ==msgQId )||(nBytes < 0)||(nBytes > msgQId->maxMsgLength) )
	{
		return (-1);
	}
	
	pthread_mutex_lock(&(msgQId->muxOP));

	//full?
	if((msgQId->nWBufUnit+msgQId->bufUnitCount-msgQId->nRBufUnit)%(msgQId->bufUnitCount)\
						>=msgQId->maxMsgs)
	{
		goto error_ret;
	}

	pBuf=msgQId->pBuf+(msgQId->nWBufUnit)*(msgQId->bufUnitLen);
	*((int *)pBuf) = nBytes;
	pBuf = pBuf+sizeof(int);
    if(NULL != buffer)
		memcpy(pBuf,buffer,nBytes);
    else
        printf("msgQlib buffer is null3\n");
	ret = sem_post(&(msgQId->semCount));
	if(0!=ret)
	{
		goto error_ret;
	}

	msgQId->nWBufUnit=(++msgQId->nWBufUnit)%(msgQId->bufUnitCount);

	pthread_mutex_unlock(&(msgQId->muxOP));

	return nBytes;
	
error_ret:

	pthread_mutex_unlock(&(msgQId->muxOP));
	return (-1);
	
}


int msgQReceive
    (
    MSG_Q_ID msgQId,          /* message queue from which to receive */
    char *   buffer,          /* buffer to receive message */
    int      maxNBytes,       /* length of buffer */
    int      timeout          /* time(ms) to wait */
    )
{
	int ret;
	int ret1 = 0;
	char * pBuf = NULL;
	if((NULL==msgQId)||(NULL==buffer)||(maxNBytes<0)||(timeout<-1))
	{
		return (-1);
	}
	
	if(timeout<0)/*wait forever*/
	{
		ret = sem_wait(&(msgQId->semCount));
		if( 0==ret )
		{
			pthread_mutex_lock(&(msgQId->muxOP));
			//empty??
			if((msgQId->nWBufUnit+msgQId->bufUnitCount-msgQId->nRBufUnit)%(msgQId->bufUnitCount)>0)
			{
				pBuf = msgQId->pBuf+(msgQId->nRBufUnit)*(msgQId->bufUnitLen);
				ret1 = *((int *)pBuf)>maxNBytes ? maxNBytes:*((int *)pBuf);
				pBuf = pBuf+sizeof(int);
				if(NULL != buffer)
    				memcpy(buffer,pBuf,ret1);
    		    else
    		        printf("msgQlib buffer is null\n");

				msgQId->nRBufUnit=(++msgQId->nRBufUnit)%(msgQId->bufUnitCount);
			}
			else
			{
				//impossible
			}
			
			pthread_mutex_unlock(&(msgQId->muxOP));
			
			return (ret1);
		}
	}
	else if(0==timeout)/*return immediately*/
	{
		ret = sem_trywait(&(msgQId->semCount));
		if( 0==ret )
		{
			
			pthread_mutex_lock(&(msgQId->muxOP));
			//empty??
			if((msgQId->nWBufUnit+msgQId->bufUnitCount-msgQId->nRBufUnit)%(msgQId->bufUnitCount)>0)
			{
				pBuf = msgQId->pBuf+(msgQId->nRBufUnit)*(msgQId->bufUnitLen);
				ret1 = *((int *)pBuf)>maxNBytes ? maxNBytes:*((int *)pBuf);
				pBuf = pBuf+sizeof(int);
				if(NULL != buffer)
                    memcpy(buffer,pBuf,ret1);
    		    else
    		        printf("msgQlib buffer is null2\n");

				msgQId->nRBufUnit=(++msgQId->nRBufUnit)%(msgQId->bufUnitCount);
			}
			else
			{
				//impossible
			}
		
			pthread_mutex_unlock(&(msgQId->muxOP));
				
			return (ret1);
		}
	}
	else
	{
		struct timespec abs_ts;
		struct timeval tv;
		struct timezone tz;
		
		gettimeofday(&tv,&tz);
		
		abs_ts.tv_sec = tv.tv_sec+(timeout+(tv.tv_usec/1000))/1000;
		abs_ts.tv_nsec = ((timeout+(tv.tv_usec/1000))%1000)*1000*1000;
		
		ret = sem_timedwait(&(msgQId->semCount),&abs_ts);
						
		if( 0==ret )
		{
				pthread_mutex_lock(&(msgQId->muxOP));
				//empty??
				if((msgQId->nWBufUnit+msgQId->bufUnitCount-msgQId->nRBufUnit)%(msgQId->bufUnitCount)>0)
				{
					pBuf = msgQId->pBuf+(msgQId->nRBufUnit)*(msgQId->bufUnitLen);
					ret1 = *((int *)pBuf)>maxNBytes ? maxNBytes:*((int *)pBuf);
					pBuf = pBuf+sizeof(int);
                    if(NULL != buffer)
                        memcpy(buffer,pBuf,ret1);
                    else
                        printf("msgQlib buffer is null4\n");

					msgQId->nRBufUnit=(++msgQId->nRBufUnit)%(msgQId->bufUnitCount);
				}
				else
				{
					//impossible
				}
		
				pthread_mutex_unlock(&(msgQId->muxOP));
				
				return (ret1);
	
		}
		else //error or timeout
		{
		}
	}
	
	return (-1);
}

int msgQNumMsgs
    (
    MSG_Q_ID msgQId           /* message queue to examine */
    )
{
	int ret = -1;
	if(NULL==msgQId)
	{
		return ret;
	}

	pthread_mutex_lock(&(msgQId->muxOP));
	ret = (msgQId->nWBufUnit+msgQId->bufUnitCount-msgQId->nRBufUnit)%(msgQId->bufUnitCount);
	pthread_mutex_unlock(&(msgQId->muxOP));

	return ret;
}
