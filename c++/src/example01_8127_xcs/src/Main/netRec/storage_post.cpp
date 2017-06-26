#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <stdlib.h>
#include "storage_post.h"
#include "ioStream.h"
#include "syslog.h"
#include "storage_manager.h"

#if MODULE_NETREC

#define MAX_POSTER_NUM  (1)
#define MAX_POST_PATH   (250)
#define MAXDATASIZE     (100)

#define POSTER_SINGLETON   \
do{ \
    if (!posterFlag) { \
        memset((void *)postTo, 0, sizeof(postTo)); \
        posterFlag = 1; \
    } \
}while(0)

unsigned long long   hl64ton_2(unsigned long long    host)
{
    unsigned long long      ret = 0;
    unsigned long        high,low;
    low         =   host & 0xFFFFFFFF;
    high        =  (host >> 32) & 0xFFFFFFFF;
    low         =   htonl(low);
    high        =   htonl(high);
    ret         =   low;
    ret         <<= 32;
    ret         |=   high;
    return   ret;
}

#define UNKNOWN_CONTENT_LENGTH  ((uint64_t) ~0)

typedef struct _destin destin;

struct _destin
{
    uint32_t    dwAddr;
    uint16_t    wPort;
    //char        cPath[MAX_POST_PATH];
};

typedef struct http_header_t {
    char    *name;      /* HTTP header name */
    char    *value;     /* HTTP header value    */
} HTTP_HEADER_S,*LPHTTP_HEADER_S;

struct rtcp_header{     
	unsigned char cc:4;  
	unsigned char x:1;  
	unsigned char p:1;  
	unsigned char v:2;  

	unsigned char pt:7;  
	unsigned char m:1;  

	unsigned short sequence_number;  
	unsigned int timestamp;  
	unsigned int ssrc;  
	unsigned short extend_len;				
	unsigned long long time;
	unsigned long long session;
};

static int posterFlag = 0;
static destin postTo[MAX_POSTER_NUM];


unsigned int uNetAddrStr2IntHost(char szAddr[])
{
    unsigned int nAddr = 0;
    unsigned char * pAddr = (unsigned char*)&nAddr;
    int arrTemp[4] = {0};
    int i;
    if(4 != sscanf(szAddr,"%d.%d.%d.%d",&arrTemp[3],&arrTemp[2],&arrTemp[1],&arrTemp[0]))
    {
        return 0;
    }
    for(i=0;i<4;i++)
    {
        pAddr[i] = arrTemp[i];
    }

    return nAddr;
}


/*
 * Skip the characters until one of the delimiters characters found.
 * 0-terminate resulting word. Skip the rest of the delimiters if any.
 * Advance pointer to buffer to the next word. Return found 0-terminated word.
 */
static char *
skip(char **buf, const char *delimiters)
{
    char    *p, *begin_word, *end_word, *end_delimiters;

    begin_word = *buf;
    end_word = begin_word + strcspn(begin_word, delimiters);
    end_delimiters = end_word + strspn(end_word, delimiters);

    for (p = end_word; p < end_delimiters; p++)
        *p = '\0';

    *buf = end_delimiters;

    return (begin_word);
}

/*
 * Parse HTTP headers from the given buffer, advance buffer to the point
 * where parsing stopped.
 */
static void
parse_http_headers(char **buf, HTTP_HEADER_S *httpHeader, int *pHeaderSize)
{
    nasPostDebug("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    int i;
    int HeaderSize = 0;
    for (i = 0; i < *pHeaderSize; i++) {
        httpHeader[i].name = skip(buf, ": ");
        httpHeader[i].value = skip(buf, "\r\n");
        if (httpHeader[i].name[0] == '\0')
            break;
       HeaderSize = i + 1;
	   //nasPostDebug("%d name %s value %s\n", i, httpHeader[i].name, httpHeader[i].value);
    }

    *pHeaderSize = HeaderSize;
}


/*
 * Return HTTP header value, or NULL if not found.
 */
static const char *
get_header(const HTTP_HEADER_S *header, const int headerSize, const char *name)
{
    int i;

    for (i = 0; i < headerSize; i++)
        if (!strcasecmp(name, header[i].name))
            return (header[i].value);

    return (NULL);
}

static uint64_t
get_content_length(const HTTP_HEADER_S *header, const int headerSize)
{
    const char *cl = get_header(header,headerSize, "Content-Length");
    return (cl == NULL ? UNKNOWN_CONTENT_LENGTH : strtoull(cl, NULL, 10));
}


//HTTP/1.1 status_code status_desc\r\n
int parseHttp(char *pszHttpText, int nLen,int *pBodyLen, int bResponse)
{
    int ret = -1;
    char    *http_version;
	char	*http_method;
	if(bResponse)
	{
		//nasPostDebug("pszHttpText = %s\n", pszHttpText);
		
	    http_version = skip(&pszHttpText, " ");
		//nasPostDebug("http_version = %s\n", http_version);
	    int status_code = atoi(skip(&pszHttpText, " "));
	    if (status_code != 200)
	    {
			nasPostDebug("error:%s %s %d status_code:%d\n", __FILE__, __FUNCTION__, __LINE__, status_code);
	        return -1;
	    }
	}
	else
	{
	    http_method = skip(&pszHttpText, " ");
	    skip(&pszHttpText, " ");
	}
	
    HTTP_HEADER_S  httpHeader[8];
    int httpHeaderSize = 8;

    parse_http_headers(&pszHttpText, httpHeader, &httpHeaderSize);
    
	//nasPostDebug("%s %s %d, httpHeaderSize:%d.\n", __FILE__, __FUNCTION__, __LINE__, httpHeaderSize);

    if(httpHeaderSize > 0)
    {
        uint64_t    content_len = get_content_length(httpHeader, httpHeaderSize);
        
		//nasPostDebug("%s %s %d, content_len:%d.\n", __FILE__, __FUNCTION__, __LINE__, content_len);
		
        if (content_len == UNKNOWN_CONTENT_LENGTH)
        {
            //error (conn, 411, "Length Required", "");
            return -2;
        }
        else
        {
            *pBodyLen = content_len;
            ret = content_len;
        }
    }
    return ret;
}



static int inline postDestin(destin *dst,const char *pszReqURI,
                        const char *content, uint32_t len,
                        char *pszResult, uint32_t *pResultLen)
{
    int ret = -1;
    int bFlag;
    int sock = -1;

    struct sockaddr_in  addr = {0};
    ioStream header;
    char buffer[4096];

    int nReUseAddr = 1;

    do
    {
        if (!dst || !dst->dwAddr)
        {
            nasPostDebug("error:%s:%d inParma is null\n",__FUNCTION__,__LINE__);
            break;
        }

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == sock)
        {
            nasPostDebug("%s:%d socket() failed.\n",__FUNCTION__,__LINE__);
            break;
        }
        else
        {
        	nasPostDebug("%s:%d socket() is %d.\n",__FUNCTION__,__LINE__, sock);
        }
        

        // 设置地址重使用属性
        ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&nReUseAddr, sizeof(int));
        if (ret != 0)
        {
            nasPostDebug("%s:%d setsockopt() failed.\n",__FUNCTION__,__LINE__);

            break;
        }

        bFlag = 1;
        ret = setsockopt(sock,     // socket
            IPPROTO_TCP,        // socket level
            TCP_NODELAY,        // socket option
            (char *)&bFlag, // option value
            sizeof (bFlag));// size of value
        if (ret != 0)
        {
            nasPostDebug("%s:%d setsockopt() failed.\n",__FUNCTION__,__LINE__);
            break;
        }
#if 0        

        //Nonblock the socket
        int flags = fcntl(sock,F_GETFL,0);
        fcntl(sock,F_SETFL,flags|O_NONBLOCK);

#endif

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(dst->dwAddr);
        addr.sin_port = htons(dst->wPort);



        ret = connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));

		/*
        if(errno!=EINPROGRESS)
        {
            nasPostDebug("%s,%s:%d Tcp connect to (0x%x:%d) failed,errno=%d\n",__FILE__,__FUNCTION__,__LINE__,dst->dwAddr,dst->wPort,errno);
            break;
        }
        */
        if(ret < 0)
        {
        	nasPostDebug("%s,%s:%d Tcp connect to (0x%x:%d) failed,errno=%d errnoStr:%s.\n",__FILE__,__FUNCTION__,__LINE__,dst->dwAddr,dst->wPort,errno,strerror(errno));
            break;
        }
        else
        {
        	nasPostDebug("%s,%s:%d Tcp connect to (0x%x:%d) ok\n",__FILE__,__FUNCTION__,__LINE__,dst->dwAddr,dst->wPort);
        }

        //connect to  nas http server
        fd_set wset;
        FD_ZERO(&wset);
        FD_SET(sock,&wset);
        int maxfd = sock+1;
        struct timeval timeout;
        timeout.tv_sec = 9;
        timeout.tv_usec = 0;
        ret = select(maxfd, NULL,&wset, NULL,&timeout);
        if(ret < 0)
        {
        	syslog(LOG_ERR|LOG_USER, "Storage post select error,killall xcs!\n");
        	system("killall -9 xcs");
            nasPostDebug("%s,%s:%d select failed,errno=%d ret:%d,fd:%d\n",__FILE__,__FUNCTION__,__LINE__,errno,ret,sock);
            break;
        }
		else if(ret == 0)
		{
			//syslog(LOG_ERR, "%s,%s:%d select timeout,errno=%d (%s) ret:%d\n",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),ret);
            nasPostDebug("%s,%s:%d select timeout,errno=%d (%s) ret:%d,fd:%d\n",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),ret,sock);
			ret =  -1;
			break;
		}

        if(FD_ISSET(sock, &wset))
        {
            int error;
            int errorLen = sizeof(error);

            ret = getsockopt(sock,SOL_SOCKET,SO_ERROR,&error,(socklen_t*)&errorLen);
            if(ret<0||error!=0)
            {
                nasPostDebug("%s,%s:%d getsockopt failed,ret:%d error:%d  errno=%d(%s)\n",__FILE__,__FUNCTION__,__LINE__,ret,error,errno,strerror(errno));
                break;
            }


	       	//nasPostDebug("%s,%s:%d Tcp connect to (0x%x:%d) ok\n",__FILE__,__FUNCTION__,__LINE__,dst->dwAddr,dst->wPort);        

	        //send data to nas http server
	        header.buffer = buffer;
	        header.size = sizeof(buffer);
	        ioStreamRewind(&header);

	        ioStreamPrint(&header, "POST %s HTTP/1.1\r\n", pszReqURI);
	        //ioStreamPrint(&header, "Accept: */*\r\n");
	        ioStreamPrint(&header, "Content-Type: application/xml; encoding='utf-8'\r\n");
	        ioStreamPrint(&header, "Host: %u.", ((dst->dwAddr >> 24) & 0xFF));
	        ioStreamPrint(&header, "%u.", ((dst->dwAddr >> 16) & 0xFF));
	        ioStreamPrint(&header, "%u.", ((dst->dwAddr >> 8) & 0xFF));
	        ioStreamPrint(&header, "%u:%d\r\n", (dst->dwAddr & 0xFF), dst->wPort);
	        ioStreamPrint(&header, "Content-Length: %d\r\n", len);
	        ioStreamPrint(&header, "Connection: Close\r\n");
	        ioStreamPrint(&header, "\r\n");

			//nasPostDebug("send postDestin len:%d content:%s\n", len,content);
			memcpy(buffer + ioStreamTell(&header), content, len);

			int iSendSum =0;
			int iRealSend =0;
			char *pTmp = NULL;

			iSendSum = ioStreamTell(&header)+len;
			pTmp = buffer;
			do
			{
				ret = send(sock, pTmp, iSendSum - iRealSend, 0);
				if(ret > 0)
				{
					iRealSend += ret;
					if(iRealSend < iSendSum)
					{
						pTmp = buffer + iRealSend;
					}
					else
					{
						break;
					}
				}
			}while(ret > 0);
	        //if(ret != ioStreamTell(&header))
	        //{
	        //    nasPostDebug("(%s %d)send failed ret:%d \n",__FILE__, __LINE__, ret);
	        //}
	        //send(sock, content, len, 0);
	        //if(ret != len)
	        //{
	        //    nasPostDebug("(%s %d)send failed ret:%d \n",__FILE__, __LINE__, ret);
	        //}
			nasPostDebug("[ %s:%d ] send ok,buffer:%s realSend bytes = %d,need send:%d.\n", __FILE__, __LINE__, buffer, iRealSend, iSendSum);
        }
        else
        {
        	nasPostDebug("[ %s:%d ] select wset is can't write.\n", __FUNCTION__, __LINE__);
        	ret =-1;
        	break;
        }
        
        //receive response data from nas http server
        int recvLen = 0;
        int httpHeaderLen = -1;
        int nContentLen = -1;
        char *body = NULL;

select_flg:
        fd_set rset;
        FD_ZERO(&rset);
        FD_SET(sock,&rset);
        maxfd = sock+1;
        timeout.tv_sec = 9;
        timeout.tv_usec = 0;
        ret = select(maxfd, &rset, NULL, NULL, &timeout);
        if(ret < 0)
        {
            nasPostDebug("%s,%s:%d select failed,errno=%d (%s) ret:%d fd:%d\n",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),ret,sock);
            break;
        }
		else if(ret == 0)
		{
            nasPostDebug("%s,%s:%d select timeout,errno=%d (%s) ret:%d fd:%d\n",__FILE__,__FUNCTION__,__LINE__,errno,strerror(errno),ret,sock);
			ret =  -1;
			break;
		}

        if(FD_ISSET(sock,&rset))
        {
recv_flg:
           int nread = recv(sock, pszResult+recvLen, (size_t) *pResultLen-recvLen, 0);
           if(nread < 0 )
           {
               if(errno != EAGAIN)
               {
               	    nasPostDebug("Recv  %d  break!\n", __LINE__);
                    nasPostDebug("[ %s %d ] %d %s  \n",__FILE__, __LINE__,errno, strerror(errno));
                    break;
               }
               goto select_flg;
           }
           else if(0 == nread)
           {
                if(httpHeaderLen != -1 && nContentLen == -1)//Content-Length 未指定的情况
                {
                	/*
                	nasPostDebug("[ %s %d ] recv over: body:%s,recvLen:%d,httpHeaderLen:%d.\n", __FILE__, __LINE__,\
                	body+4, recvLen, httpHeaderLen);
                	
                    nasPostDebug("[ %s %d ] recv over:%s\n", __FILE__, __LINE__, pszResult);
					*/
					
                    memcpy(pszResult, body+4, recvLen - httpHeaderLen);
                    *pResultLen = recvLen - httpHeaderLen;
                    pszResult[*pResultLen] = 0;

                    goto ok_flg;
                }
                
                nasPostDebug("[ %s %d ] socket[%d] recv 0, closed\n", __FILE__, __LINE__, sock);
                break;
           }

           recvLen += nread ;

            if(httpHeaderLen != -1 && nContentLen != -1)
            {
                if(recvLen >= httpHeaderLen+nContentLen)
                {
                	nasPostDebug("[ %s %d ] recv over: body:%s,nContentLen:%d\n", __FILE__, __LINE__,\
                	body+4, nContentLen);
                	
                    memcpy(pszResult, body+4, nContentLen);
                    *pResultLen = nContentLen;
                    pszResult[*pResultLen] = 0;

                    nasPostDebug("[ %s %d ] recv over:%s\n", __FILE__, __LINE__, pszResult);
                    goto ok_flg;
                }
            }
            else
            {
                if((body = strstr(pszResult, "\r\n\r\n")) == NULL)
                {
                    goto recv_flg;
                }
                else
                {
                    char httpHeaderBuf[1024];

                    httpHeaderLen = body-pszResult+4;
                    memcpy(httpHeaderBuf, pszResult, httpHeaderLen);


                    if((ret = parseHttp(httpHeaderBuf, httpHeaderLen, &nContentLen, 1)) < 0)
                    {
                        if(-2 == ret)//Content-Length 未指定 (收齐 socket 关闭前的所有数据)
                        {
                            goto recv_flg;
                        }
                        nasPostDebug("[ %s %d ] parseHttp failed  \n",__FILE__, __LINE__);
						
						nasPostDebug("Recv	%d	break!\n", __LINE__);
                        break;
                    }

                    if(recvLen >= httpHeaderLen+nContentLen)
                    {
                    	/*nasPostDebug("[ %s %d ] recv over: body:%s,nContentLen:%d\n", __FILE__, __LINE__,\
                			body+4, nContentLen); */

                        memcpy(pszResult, body+4, nContentLen);
                        *pResultLen = nContentLen;
                        pszResult[*pResultLen] = 0;
                        goto ok_flg;
                    }
                    else
                    {
                        goto recv_flg;
                    }
                }
            }
        }
        else
        {
        
            nasPostDebug("[ %s %d ] select error  \n",__FILE__, __LINE__);
			
			nasPostDebug("Recv	%d	break!\n", __LINE__);
            break;
        }
ok_flg:
	
    	//nasPostDebug("[ %s:%d ] recv ok,contentLen:%d, contem is %s.\n", __FILE__, __LINE__, *pResultLen, pszResult);
        ret = 0;
    }while(0);

    if(-1 != sock)
    {
        ret = shutdown(sock,2);
        nasPostDebug("[ %s %d ] shutdown sock:%d ret:%d\n",__FILE__, __LINE__,sock,ret);
	}
    if(ret != 0)
         nasPostDebug("[ %s %d ] ret:%d \n",__FILE__, __LINE__,ret);

    return ret==0?0:-1;
}

int  nasHttpSet(char *pszIp, int nPort)
{
    int ret = 0;
    POSTER_SINGLETON;
	nasPostDebug("+++++++++++++++++++++%s %d+++++++++++++++++++\n", pszIp, nPort);
    postTo[0].dwAddr    = uNetAddrStr2IntHost(pszIp);
    postTo[0].wPort     = nPort;


    return ret;
}

int  nasHttpInfoSet(int Ip, int nPort)
{
    int ret = 0;
    char szTemp[256];
    struct in_addr in;

    POSTER_SINGLETON;
    
    in.s_addr = htonl(Ip);
    sprintf(szTemp,"%s", inet_ntoa(in));
    
	nasPostDebug("nasHttpInfoSet,ip:%s,port:%d\n", szTemp, nPort);
    postTo[0].dwAddr    = Ip;
    postTo[0].wPort     = nPort;
    return ret;
}

int nasHttp_Post(const char *pszURI, const char *content, uint32_t len,
                            char *pszResult, uint32_t *pResultLen)
{
    int ret = 0;
    POSTER_SINGLETON;
	struct in_addr addr;
	addr.s_addr = ntohl(postTo[0].dwAddr);
	nasPostDebug("Http_postTo[0].dwAddr:%s postTo[0].wPort:%d,pszURI %s,len:%d\n", (inet_ntoa(addr)),postTo[0].wPort, pszURI,len);

    ret = postDestin(&postTo[0], pszURI, content,len, pszResult, pResultLen);

    return ret;
}

int nasHttp_Listen(int port)
{	
	int fd =-1;
	int acceptfd = -1;
    struct sockaddr_in  addrServer;
	socklen_t addrlen = 0;
	int ret =  -1;
	int ret2 =  -1;

	int recvLen = 0;
	int httpHeaderLen = -1;
	int nContentLen = -1;
	char *body = NULL;
	fd_set rset;
	char szRecvBuf[1024] = {0};
	int nRecvLen = 1024;
	int maxfd = 0;
	struct timeval timeout;
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0)
	{
		syslog(LOG_ERR, "%s %s %d errno:%d %s\n", __FILE__, __FUNCTION__, __LINE__, errno, strerror(errno));
		return -1;
	}
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = htonl(INADDR_ANY);
	addrServer.sin_port = htons(port);
	addrlen = sizeof(addrServer);

#if 0	
	int bFlag;
	// set socket option SO_REUSEADDR to reuse listen port
	bFlag = 1;
	ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&bFlag, sizeof (bFlag));	
	//	set socket option SO_KEEPALIVE to enable the use of keep-alive packets on TCP connections 
	bFlag = 1;
	ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&bFlag, sizeof (bFlag));
	if (ret != 0)
	{
		syslog(LOG_ERR,  " nasHttp_listen set SO_KEEPALIVE option failed.\n");
	}


	bFlag = fcntl(fd,F_GETFL,0);
	ret = fcntl(fd,F_SETFL,O_NONBLOCK|bFlag);
	if (ret != 0)
	{
		syslog(LOG_ERR,  " nasHttp_listen set O_NONBLOCK option failed.\n");
	}
#endif

	ret = bind(fd, (struct sockaddr *)&addrServer, sizeof(addrServer));
	if (ret < 0)
	{
		syslog(LOG_ERR, " nasHttp_listen bind failed , errno %d\n", errno);
		nasPostDebug("[ %s %d ] nasHttp_Listen bind failed,errno:%d,strerror:%s.\n",__FILE__, __LINE__, errno, strerror(errno));		
		goto ret_flag;
	}

	ret = listen(fd, 5) ;
	if(ret < 0)
	{
		nasPostDebug("[ %s %d ] nasHttp_Listen listen failed,errno:%d,strerror:%s.\n",__FILE__, __LINE__, errno, strerror(errno));
		goto ret_flag;
	}
	
	while(1)
	{
		FD_ZERO(&rset);
		FD_SET(fd,&rset);
		maxfd = fd+1;

		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		
		recvLen = 0;
		nRecvLen = 1024;
		httpHeaderLen = -1;
		nContentLen = -1;
		memset(szRecvBuf, 0 , 1024);

		ret = select(maxfd, &rset, NULL, NULL,&timeout);
		if(ret < 0)
		{
        	syslog(LOG_ERR|LOG_USER, "nashttp select error,killall xcs!\n");
        	system("killall -9 xcs");
			syslog(LOG_ERR, "%s %s %d select failed.error %d:%s\n", __FILE__, __FUNCTION__, __LINE__, errno, strerror(errno));
			break;
		}
		else if(ret == 0)
		{
			nasPostDebug("[ %s %d ] nasHttp_Listen select timeout fd:%d\n",__FILE__, __LINE__, fd);
		}
		
        if(FD_ISSET(fd,&rset))
		{
            if(acceptfd == -1)
            {
                acceptfd = accept(fd, (struct sockaddr *)&addrServer, &addrlen);
                if(acceptfd < 0)
                {
                    nasPostDebug("%s %s %d, accept failed,error:%d %s\n", __FILE__, __FUNCTION__, __LINE__, errno, strerror(errno));
                    continue;
                }
                else
                {
                	nasPostDebug("%s %s %d, accept ok fd:%d\n", __FILE__, __FUNCTION__, __LINE__, acceptfd);
                }
            }
            
    		recv_flg:
    		int nread = recv(acceptfd, szRecvBuf+recvLen, (size_t) nRecvLen -recvLen, 0);
    		if(nread < 0 )
    		{
   				nasPostDebug("[ %s %d ] recv failed,shutdown acceptfd.error:%d %s  \n",__FILE__, __LINE__,errno, strerror(errno));
				close(acceptfd);
				acceptfd = -1;
    			continue;
    		}
    		else if(0 == nread)
    		{
    			if(httpHeaderLen != -1 && nContentLen == -1)//Content-Length 未指定的情况
    			{
    				nasPostDebug("[ %s %d ] %s \n", __FILE__, __LINE__, szRecvBuf);

    				memcpy(szRecvBuf, body+4, recvLen - httpHeaderLen - 4);
    				nRecvLen = recvLen - httpHeaderLen - 4;
    				szRecvBuf[nRecvLen] = 0;

    				goto ok_flg;
    			}
    			nasPostDebug("[ %s %d ] socket[%d] recv 0, closed\n", __FILE__, __LINE__, acceptfd);
                close(acceptfd);
    			acceptfd = -1;
    			continue;
    		}

    		recvLen += nread ;

    		if(httpHeaderLen != -1 && nContentLen != -1)
    		{
    			if(recvLen >= httpHeaderLen+nContentLen)
    			{
    				memcpy(szRecvBuf, body+4, nContentLen);
    				nRecvLen = nContentLen;
    				szRecvBuf[nRecvLen] = 0;
    				goto ok_flg;
    			}
    		}
    		else
    		{
    			if((body = strstr(szRecvBuf, "\r\n\r\n")) == NULL)
    			{
    				goto recv_flg;
    			}
    			else
    			{
    				char httpHeaderBuf[1024];

    				httpHeaderLen =  body-szRecvBuf+4;
    				memcpy(httpHeaderBuf, szRecvBuf,httpHeaderLen);

    				if((ret = parseHttp(httpHeaderBuf, httpHeaderLen,&nContentLen,0)) < 0)
    				{
    					if(-2 == ret)//Content-Length 未指定 (收齐 socket 关闭前的所有数据)
    					{
    						goto recv_flg;
    					}
    					nasPostDebug("[ %s %d ] parseHttp failed  \n",__FILE__, __LINE__);
                    	syslog(LOG_ERR, "%s %s %d error %d:%s\n", __FILE__, __FUNCTION__, __LINE__, errno, strerror(errno));

    					close(acceptfd);
		    			acceptfd = -1;
		    			continue;
    				}

    				if(recvLen >= httpHeaderLen+nContentLen)
    				{
    					memcpy(szRecvBuf, body+4, nContentLen);
    					nRecvLen = nContentLen;
    					szRecvBuf[nRecvLen] = 0;
    					goto ok_flg;
    				}
    				else
    				{
    					goto recv_flg;
    				}
    			}
    		}
    		ok_flg:
    		{
    			//nasPostDebug("start nasTrackRedirect szRecvBuf = %s\n", szRecvBuf);
    			//收到重定向，直接响应，代表收到重定向指令,可以进行码流切换了。
    			ret2 = nasTrackRedirect(szRecvBuf, nRecvLen);
                if(-1 != ret2)
    			{
        			nasRecordRedirectResp(acceptfd, 0);
    			}
    			
    			if(acceptfd != -1)
    			{
                    close(acceptfd);
        			acceptfd = -1;
    			}
    		}
		}
	}
	syslog(LOG_ERR, "Listen Nru redirect thread exit!\n");
ret_flag:	
	if(fd != -1)
	{
		close(fd);
	}
	return 0;
}

int nasUdpHeartBeatSend(sockaddr_in server, unsigned short sequence_number, unsigned long long session)
{ 
	int sockfd, num;
	char buf[MAXDATASIZE];

	int sta = -1;
	struct timeval timeout = {3, 1000}; //UDP time out 500ms
	struct rtcp_header RtcpHeartBeat = {0};
	sockaddr_in peer;
	socklen_t  addrlen;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0))==-1)
	{
		nasPostDebug("%s %d socket() failed\n",__FUNCTION__,__LINE__);
		return -1;
	}
	
    sta = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
    if(sta != 0) 
    {
    	close(sockfd);
    	nasPostDebug("setsockopt() SO_SNDTIMEO error\n");
		return -1;
    }
    
	sta = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)); 
    if(sta != 0) 
    {
    	close(sockfd);
    	nasPostDebug("setsockopt() SO_RCVTIMEO error\n");
		return -1;
    }

	RtcpHeartBeat.cc = 1;
	RtcpHeartBeat.x = 1;
	RtcpHeartBeat.p = 0;
	RtcpHeartBeat.v = 2;
	RtcpHeartBeat.m = 0;
	RtcpHeartBeat.pt = 200;

	RtcpHeartBeat.ssrc = htonl(0x12345678);
	RtcpHeartBeat.extend_len = htons(16);
	//nasPostDebug("sequence_number:%d\n", sequence_number);

	RtcpHeartBeat.sequence_number = htons(sequence_number);
	RtcpHeartBeat.timestamp = htonl(time(NULL));
	RtcpHeartBeat.time = htonl(time(NULL));
	RtcpHeartBeat.session = hl64ton_2(session);

	nasPostDebug("[ %s %d ] session:%llu\n",__FILE__, __LINE__, session);

	sendto(sockfd, &RtcpHeartBeat, sizeof(RtcpHeartBeat), 0, (struct sockaddr *)&server, sizeof(server));
	
	if ((num = recvfrom(sockfd, buf, MAXDATASIZE, 0, (struct sockaddr *)&peer, &addrlen))== -1)
	{
		nasPostDebug("[ %s %d ] %s:%d NRU Dead\n",__FILE__, __LINE__, inet_ntoa(server.sin_addr), ntohs(server.sin_port));
		close(sockfd);	   
		return -1;
	}
#if 0
	nasPostDebug("num:%d\n", num);
	for (int i = 0; i < num; i++)
	{
		nasPostDebug("%02X ", buf[i]);
	}
	nasPostDebug("\n\n");
#endif

	if ((num == sizeof(RtcpHeartBeat)) && (memcmp(buf, &RtcpHeartBeat, num) == 0))
	{
		close(sockfd);	   
		nasPostDebug("[ %s %d ] %s:%d NRU Alive\n",__FILE__, __LINE__, inet_ntoa(server.sin_addr), ntohs(server.sin_port));
		return 0;
	}
	
	close(sockfd);	
	return -1;
}

#endif


