#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <errno.h>
#include <syslog.h>
#include <sys/epoll.h>
#include <sys/un.h>
#include <stdint.h>
//#include <uuid/uuid.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <sys/time.h>
#include <syslog.h>
#include "mxml.h"
#include "storage_post.h"
#include "storage_manager.h"
#include "Zone_rand.h"
#include "cfgSock.h"
#include "SysConf.h"
#include "uuid.h"
#include "dsp.h"
#include "MmsSend.h"

#if MODULE_NETREC

#define SEPARATE                        "."
#define MAX_NODE_LEVEL                  (10)

#define	XML_HEAD	"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
#define	XML_BODY	"<Message Version=\"1.0.0\"></Message>"

/*
#define NRM_STORAGE                      "NruStorage"
#define NRM_STORAGE_IP                   "Ip"
#define NRM_STORAGE_PORT                 "Port"
#define NRM_STORAGE_IP_BAK               "IpBak"
#define NRM_STORAGE_PORT_BAK             "PortBak"
#define NRM_STORAGE_PUID				 "Puid"
#define NRM_STORAGE_PLATID				 "Platid"
#define NRM_STORAGE_GUID                 "GUID"

#define OTHER_PLAT                       "GE_TERM_CONF_INFO_02"
#define OTHER_PLAT_TERM_CODE             "TermCode"         
#define OTHER_PLAT_TERM_CODE_EX          "TermCodeEx"         
#define OTHER_PLAT_GROUP_ID              "GroupCode"         

#define LAST_NRU_STORAGE                 "LastNruStorage"
#define LAST_NRU_STORAGE_IP              "IP"
#define LAST_NRU_STORAGE_HTTP_PORT       "Http_Port"
*/

typedef struct _GUID
{
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID, UUID;

typedef struct ChannelStatus
{
    sockaddr_in s_ChannelHeartBeatAddr;
    unsigned short RtcpSquence;
    int	NruDeadTimes;
    int	UsedNruIpFlag;     //0:no use, use NRU get from NRM     1:NRM die, use last use NRU
    unsigned long long session;
};

static mxml_node_t* createXmlRootNode()
{
    mxml_node_t *xmlhandle = NULL;
    char xml_buf[32*1024] = {0} ;
    int xml_buf_len = 0 ;

    memcpy(xml_buf , XML_HEAD , strlen(XML_HEAD)) ;
    xml_buf_len += strlen(XML_HEAD) ;
    memcpy(xml_buf + xml_buf_len , XML_BODY , strlen(XML_BODY)) ;
    xml_buf_len += strlen(XML_BODY) ;

    xmlhandle = mxmlLoadString(NULL, xml_buf, MXML_NO_CALLBACK);
    //if(xmlhandle)
    //    xmlhandle = xmlhandle->child;
    return xmlhandle;
}

static void destoryXmlRoot(mxml_node_t *xmlhandle)
{
    return mxmlDelete(xmlhandle);
}



static int modifyXmlNode(mxml_node_t* XmlHandle,const char *pNodeName, const char *pNodeValue)
{
    int ret = 0;
    int i = 0;
    char nodeNameArry[MAX_NODE_LEVEL][128] = {0};
    char NodeName_tmp[128] = {0};
    int count = 0;
    char *token = NULL;
    mxml_node_t *pNode;
    mxml_node_t *pParentNode;

    if(NULL == pNodeName || NULL == pNodeValue)
    {
        nasPostDebug("%s %d  paramter is invalid\n",__FILE__, __LINE__);

        ret = -1;
        goto done;
    }

    strcpy(NodeName_tmp, pNodeName);
    strcpy(nodeNameArry[0], NodeName_tmp);


    token = strtok((char *) NodeName_tmp, SEPARATE );


    while( (token != NULL)  && (count++ < MAX_NODE_LEVEL) )
    {
        strcpy(nodeNameArry[count - 1], token);
        // Get next token:
        token = strtok( NULL, SEPARATE );
    }

    if(MAX_NODE_LEVEL == count )
    {
        nasPostDebug("%s %d   node level is greater than %d \n",__FILE__, __LINE__,count -1);

        ret = -1;
        goto done;
    }

    pParentNode = XmlHandle;
    for( i=0; i<count; i++)
    {
        pNode = mxmlFindElement(pParentNode,pParentNode, nodeNameArry[i], NULL, NULL, MXML_DESCEND);
        if(NULL == pNode)
        {
            if(NULL == (pNode = mxmlNewElement(pParentNode, nodeNameArry[i])))
            {
                nasPostDebug("%s %d   mxmlNewElement failed \n",__FILE__, __LINE__);

                ret = -1;
                goto done;
            }

        }
        else
        {
            //node is already exist
        }
        pParentNode = pNode;
    }

    if(NULL ==pParentNode->child)
    {
        if(mxmlNewText(pParentNode, 0, pNodeValue) == NULL)
        {
            nasPostDebug("%s %d   mxmlNewText failed \n",__FILE__, __LINE__);
            ret = -1;
        }
    }
    else//text node is already exist
    {
        if(mxmlSetText(pParentNode->child, 0 , pNodeValue) == -1)
        {
            nasPostDebug("%s %d   mxmlSetText failed \n",__FILE__, __LINE__);
            ret = -1;
        }
    }

done:
    return ret;
}


static int getXmlNode(mxml_node_t *XmlHandle,const char *pNodeName,  char *pNodeValue)
{
    int ret = 0;
    int i = 0;
    char nodeNameArry[MAX_NODE_LEVEL][64] = {0};
    char NodeName_tmp[128] = {0};
    int count = 0;
    char *token = NULL;
    mxml_node_t *pNode;
    mxml_node_t *pParentNode;

    if(NULL == pNodeName || NULL == pNodeValue)
    {
        nasPostDebug("%s %d  paramter is invalid\n",__FILE__, __LINE__);

        ret = -1;
        goto done;
    }

    strcpy(NodeName_tmp, pNodeName);
    strcpy(nodeNameArry[0], NodeName_tmp);

    token = strtok((char *) NodeName_tmp, SEPARATE );
    while( (token != NULL)  && (count++ < MAX_NODE_LEVEL) )
    {
        strcpy(nodeNameArry[count - 1], token);
        // Get next token:
        token = strtok( NULL, SEPARATE );
    }

    if(MAX_NODE_LEVEL == count )
    {
        nasPostDebug("%s %d   node level is greater than %d \n",__FILE__, __LINE__,count -1);

        ret = -1;
        goto done;
    }


    pParentNode = XmlHandle;
    for( i=0; i<count; i++)
    {
        pNode = mxmlFindElement(pParentNode,pParentNode, nodeNameArry[i], NULL, NULL, MXML_DESCEND);
        if(NULL == pNode)
        {
            //nasPostDebug("%s %d   mxmlFindElement failed \n",__FILE__, __LINE__);
            ret = -1;
            goto done;
        }
        else
        {
            //node is already exist
        }
        pParentNode = pNode;
    }

    if(NULL ==pParentNode->child)
    {
        ret = -1;
        goto done;
    }
    else//text node is already exist
    {
        strcpy(pNodeValue,pParentNode->child->value.text.string );
    }

done:
    return ret;
}


static int dumpXml2string(mxml_node_t* handle, char** xml, int* len)
{
	int 	bytes;			
	char	buffer[8192];	
	char	*s; 		

	*xml = NULL;
	*len = 0;
	
	mxmlSetCodeType(MXML_GBK);
	
	bytes = mxmlSaveString(handle, buffer, sizeof(buffer), MXML_NO_CALLBACK);

	if (bytes <= 0)
	{
		return (-1);
	}


	if (bytes < (int)(sizeof(buffer) - 1))
	{
		*xml = strdup(buffer);
		*len = bytes;
		return -1;
	}

	if ((s = (char *)malloc(bytes + 1)) == NULL)
		return (-1);

	mxmlSaveString(handle, s, bytes + 1, MXML_NO_CALLBACK);
	
	*xml = s;
	*len = bytes;
	
	return 0;

}

static int nasTrackOpenParase(const char *xml_buf, int nLen,char *szIp, char *szPort, char *szHttpPort, char *szSession)
{
    int ret = 0;
    mxml_node_t *xmlhandle = NULL;
    char szCtx[128];
    uint32_t nErrorCode = 0;
    nasPostDebug("[ %s  %s  %d ] %s \n",__FILE__,__FUNCTION__, __LINE__,xml_buf);


    xmlhandle = mxmlLoadString(NULL, xml_buf, MXML_NO_CALLBACK);

    if(NULL == xmlhandle)
    {
        nasPostDebug("[ %s %d ] nLen:%d  %s\n",__FILE__, __LINE__,nLen,xml_buf);
        return -1;
    }

#if 1
    char szErrorCode[128];
    memset(szErrorCode, 0, sizeof(szErrorCode));
    if(getXmlNode(xmlhandle,"IE_RESULT.ErrorCode",szErrorCode) == 0)
    {
        sscanf(szErrorCode, "%x", &nErrorCode);
        nasPostDebug("[ %s  %s  %d ] nErrorCode:0x%x\n",__FILE__,__FUNCTION__, __LINE__,nErrorCode);
    }
#endif

	memset(szCtx, 0, sizeof(szCtx));
    if(getXmlNode(xmlhandle,"IE_NRU.IP",szCtx) == 0)
    {
        if(szIp)
            strcpy(szIp, szCtx);
        nasPostDebug("[ %s  %s  %d ] szIp:%s\n",__FILE__,__FUNCTION__, __LINE__,szIp);

    }
    
	memset(szCtx, 0, sizeof(szCtx));
    if(getXmlNode(xmlhandle,"IE_NRU.PORT",szCtx) == 0)
    {
        if(szPort)
            strcpy(szPort, szCtx);
        nasPostDebug("[ %s  %s  %d ] szPort:%s\n",__FILE__,__FUNCTION__, __LINE__,szPort);
    }
    
    memset(szCtx, 0, sizeof(szCtx));
    if(getXmlNode(xmlhandle,"IE_NRU.Session",szCtx) == 0)
    {
        if(szSession)
            strcpy(szSession, szCtx);
        nasPostDebug("[ %s  %s  %d ] Session:%s\n",__FILE__,__FUNCTION__, __LINE__,szSession);
    }

	memset(szCtx, 0, sizeof(szCtx));
    if(getXmlNode(xmlhandle,"IE_NRU.HTTP_PORT",szCtx) == 0)
    {
        nasPostDebug("[ %s  %s  %d ] rec szHttpPort:%s\n",__FILE__,__FUNCTION__, __LINE__,szCtx);    	

        if(szHttpPort)
            strcpy(szHttpPort, szCtx);
        nasPostDebug("[ %s  %s  %d ] szHttpPort:%s\n",__FILE__,__FUNCTION__, __LINE__,szHttpPort);
    }
	else
	{
		memset(szCtx, 0, sizeof(szCtx));
		if(getXmlNode(xmlhandle,"Message.HTTP_PORT",szCtx) == 0)
	    {
	        nasPostDebug("[ %s  %s  %d ] rec szHttpPort:%s\n",__FILE__,__FUNCTION__, __LINE__,szCtx);    	

	        if(szHttpPort)
	            strcpy(szHttpPort, szCtx);
	        nasPostDebug("[ %s  %s  %d ] szHttpPort:%s\n",__FILE__,__FUNCTION__, __LINE__,szHttpPort);
	    }
	}

    destoryXmlRoot(xmlhandle);

    if(ret == -1)
         nasPostDebug("[ %s %d ] ret:%d \n",__FILE__, __LINE__,ret);

    return ret;
}

inline const char *VFMT_RESOLUTION_STR(unsigned int nFmtType )
{
	
    switch(nFmtType)
    {
        case VFMT_4CIF:
            return "704*576";
        case VFMT_720P:
            return "1280*720";
        case VFMT_1080P:
            return "1920*1080"; 
        default :
            break;
    }
    return "UNKNOWN";
}
int nasTrackOpen(int chanid, 
					const char *puid, 
					const char *platid,
					int payload,
					int transType,
					int resolution,
					int frameRate,
					int qutoa,
					int saveday,
					char *szIp,
					char *szPort,
					char *szHttpPort,
					char *szSession)
{
    int ret = 0;
	char tmp[128];
    char * xml_dmp_buf = NULL ;	
    int xml_dmp_buf_len = 0 ;
    mxml_node_t *XmlHandle;
    int bitrate = 0;

    VIDEO_CONF_INFO strVideoConfInfo;
    ret = SysConfGet(e_VIDEO_CONF, &strVideoConfInfo, 0);
    if (ret == 0 )
    {
    	bitrate = 1024*strVideoConfInfo.uiBitRate;
    }

    XmlHandle = createXmlRootNode();
    if(NULL == XmlHandle)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        return -1;
    }

    ret = modifyXmlNode(XmlHandle,"Message.IE_HEADER.MessageType", "MSG_START_PU_RECORD_REQ");
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

	
    ret = modifyXmlNode(XmlHandle,"Message.IE_HEADER.Reserved", "");
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    ret = modifyXmlNode(XmlHandle,"Message.IE_CHANNEL.Puid", puid);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    ret = modifyXmlNode(XmlHandle,"Message.IE_CHANNEL.ChannelType", "");
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

	sprintf(tmp, "%d", chanid+1);
    ret = modifyXmlNode(XmlHandle,"Message.IE_CHANNEL.ChannelID", tmp);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    ret = modifyXmlNode(XmlHandle,"Message.IE_RECORD_ATTR.OtherPlatformId", platid);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }
	sprintf(tmp, "%d", payload);
    ret = modifyXmlNode(XmlHandle,"Message.IE_RECORD_ATTR.PayLoad", tmp);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }
	
	sprintf(tmp, "%d", transType);
    ret = modifyXmlNode(XmlHandle,"Message.IE_RECORD_ATTR.Transfer", tmp);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    ret = modifyXmlNode(XmlHandle,"Message.IE_RECORD_ATTR.Resolution", VFMT_RESOLUTION_STR(resolution));
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

	sprintf(tmp, "%d", frameRate);
    ret = modifyXmlNode(XmlHandle,"Message.IE_RECORD_ATTR.FrameRate", tmp);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

	sprintf(tmp, "%dG", qutoa);
    ret = modifyXmlNode(XmlHandle,"Message.IE_RECORD_ATTR.SizeLimit", tmp);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }
	
	sprintf(tmp, "%d", saveday);
    ret = modifyXmlNode(XmlHandle,"Message.IE_RECORD_ATTR.TimeLimit", tmp);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

	sprintf(tmp, "%d", bitrate);
    ret = modifyXmlNode(XmlHandle,"Message.IE_RECORD_ATTR.BitRate", tmp);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    dumpXml2string(XmlHandle,&xml_dmp_buf, &xml_dmp_buf_len);
    
    //nasPostDebug("[ %s %d ] len:%d %s \n",__FILE__, __LINE__, xml_dmp_buf_len, xml_dmp_buf);
    
    if (xml_dmp_buf_len > 0)
    {
        char buf[1024];
        memset(buf, 0x00, sizeof(buf));
        uint32_t  nLen = 1024;

        ret = nasHttp_Post(NAS_TRACK_OPEN,xml_dmp_buf, xml_dmp_buf_len, buf, &nLen);
		//syslog(LOG_INFO, "nasTrackOpen   nasHttp_Post ret %d\n", ret);

        if(ret < 0 || nLen<= 0)
        {
            //nasPostDebug("[ %s %d ] ret:%d  nLen:%d\n",__FILE__, __LINE__, ret, nLen);
            ret = -1;
            goto done;
        }
        ret = nasTrackOpenParase(buf, nLen, szIp, szPort, szHttpPort, szSession);
		//syslog(LOG_INFO, "szIp %s szPort =%s\n", szIp,szPort);
    }
    else
    {
        syslog(LOG_ERR, "[ %s %s %d ] \n",__FILE__, __FUNCTION__, __LINE__);
        ret = -1;
        goto done;
    }

done:
	if(xml_dmp_buf)
    	free(xml_dmp_buf);

    destoryXmlRoot(XmlHandle);

    if(ret == -1)
         nasPostDebug("[ %s %d ] ret:%d \n",__FILE__, __LINE__,ret);

    return ret;
}

static int nasTrackCloseParase(const char *xml_buf, int nLen)
{
    int ret = 0;
    mxml_node_t *xmlhandle = NULL;
    uint32_t nErrorCode;

    xmlhandle = mxmlLoadString(NULL, xml_buf, MXML_NO_CALLBACK);

    if(NULL == xmlhandle)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
         return -1;
    }

    char szErrorCode[128];
    if(getXmlNode(xmlhandle,"IE_RESULT.ErrorCode",szErrorCode) <0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }
    sscanf(szErrorCode, "%x", &nErrorCode);
    if(nErrorCode != 0)
    {
        nasPostDebug("[ %s  %s  %d ] nErrorCode:%d\n",__FILE__,__FUNCTION__, __LINE__,nErrorCode);

        ret = -1;
        goto done;
    }
    nasPostDebug("[ %s  %s  %d ] nErrorCode:%d\n",__FILE__,__FUNCTION__, __LINE__,nErrorCode);
done:

    destoryXmlRoot(xmlhandle);

    if(ret == -1)
         nasPostDebug("[ %s %d ] ret:%d \n",__FILE__, __LINE__,ret);

    return ret;
}
int nasTrackClose(const char *Mac, const char * Guid)
{
    int ret = 0;
    char * xml_dmp_buf = NULL ;	
    int xml_dmp_buf_len = 0 ;
    mxml_node_t *XmlHandle;

    if(NULL == Mac || NULL == Guid)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        return -1;
    }

    XmlHandle = createXmlRootNode();
    if(NULL == XmlHandle)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        return -1;
    }

    ret = modifyXmlNode(XmlHandle,"IE_HEADER.MessageType", "MSG_STOP_TRACK");
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }


    ret = modifyXmlNode(XmlHandle,"IE_MAC_AUTH.MacAddress", Mac);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    ret = modifyXmlNode(XmlHandle,"IE_MAC_AUTH.ChannelGUID", Guid);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }


    dumpXml2string(XmlHandle,&xml_dmp_buf, &xml_dmp_buf_len);
    if (xml_dmp_buf_len > 0)
    {
        char buf[1024];
        memset(buf, 0x00, sizeof(buf));
        uint32_t  nLen = 1024;
        ret = nasHttp_Post(NAS_TRACK_CLOSE,xml_dmp_buf, xml_dmp_buf_len, buf, &nLen);
        if(ret < 0 || nLen<= 0)
        {
            nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
            ret = -1;
            goto done;
        }
        ret = nasTrackCloseParase(buf, nLen);
    }
    else
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);

        ret = -1;
        goto done;
    }
	if(xml_dmp_buf)
    	free(xml_dmp_buf) ;

done:

    destoryXmlRoot(XmlHandle);

    if(ret == -1)
         nasPostDebug("[ %s %d ] ret:%d \n",__FILE__, __LINE__,ret);

    return ret;
}


static int nasQuotaGetParase(const char *xml_buf, int nLen)
{
    int ret = 0;
    mxml_node_t *xmlhandle = NULL;

    char szSizeLimit[128];
    char szTimeLimit[128];
    uint32_t nErrorCode = 0;

    xmlhandle = mxmlLoadString(NULL, xml_buf, MXML_NO_CALLBACK);

    if(NULL == xmlhandle)
    {
        nasPostDebug("[ %s %d ] %s\n",__FILE__, __LINE__,xml_buf);
         return -1;
    }

    char szErrorCode[128];
    if(getXmlNode(xmlhandle,"IE_RESULT.ErrorCode",szErrorCode) == 0)
    {
        sscanf(szErrorCode, "%x", &nErrorCode);
        nasPostDebug("[ %s  %s  %d ] nErrorCode:%d\n",__FILE__,__FUNCTION__, __LINE__,nErrorCode);
    }


    if(getXmlNode(xmlhandle,"IE_TRK_QUOTA.SizeLimit",szSizeLimit) == 0)
    {
        nasPostDebug("[ %s  %s  %d ] SizeLimit:%s\n",__FILE__,__FUNCTION__, __LINE__,szSizeLimit);
    }


    if(getXmlNode(xmlhandle,"IE_TRK_QUOTA.TimeLimit",szTimeLimit) == 0)
    {
        nasPostDebug("[ %s  %s  %d ] TimeLimit:%s\n",__FILE__,__FUNCTION__, __LINE__,szTimeLimit);
    }

    destoryXmlRoot(xmlhandle);

    if(ret == -1)
        nasPostDebug("[ %s %d ] ret:%d \n",__FILE__, __LINE__,ret);

   return ret;
}

int nasQuotaGet(const char *Mac, const char * Guid,  uint32_t *pSizeLimit, uint32_t *pTimeLimit)
{
    int ret = 0;
    mxml_node_t *XmlHandle;	
    char * xml_dmp_buf = NULL ;
    int xml_dmp_buf_len = 0 ;

    if(NULL == Mac || NULL == Guid)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        return -1;
    }

    if(NULL == pSizeLimit || NULL == pTimeLimit)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        return -1;
    }

    XmlHandle = createXmlRootNode();
    if(NULL == XmlHandle)
    {
        nasPostDebug("[ %s %d ] createXmlRootNode failed\n",__FILE__, __LINE__);
        return -1;
    }

    ret = modifyXmlNode(XmlHandle,"IE_HEADER.MessageType", "MSG_GET_QUOTA");
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] modifyXmlNode failed\n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }


    ret = modifyXmlNode(XmlHandle,"IE_MAC_AUTH.MacAddress", Mac);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] modifyXmlNode failed\n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    ret = modifyXmlNode(XmlHandle,"IE_MAC_AUTH.ChannelGUID", Guid);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] modifyXmlNode failed\n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }


    dumpXml2string(XmlHandle,&xml_dmp_buf, &xml_dmp_buf_len);
	
    if (xml_dmp_buf_len > 0)
    {
        char buf[1024];
        memset(buf, 0x00, sizeof(buf));
        uint32_t  nLen = 1024;
        ret = nasHttp_Post(NAS_QUOTA_GET,xml_dmp_buf, xml_dmp_buf_len, buf, &nLen);
        if(ret < 0 || nLen<= 0)
        {
            nasPostDebug("[ %s %d ]nasHttp_Post failed \n",__FILE__, __LINE__);

            ret = -1;
            goto done;
        }
        ret = nasQuotaGetParase(buf, nLen);
    }
    else
    {
        nasPostDebug("[ %s %d ] dumpXml2string failed \n",__FILE__, __LINE__);

        ret = -1;
        goto done;
    }
	if(xml_dmp_buf)
    	free(xml_dmp_buf) ;

done:

    destoryXmlRoot(XmlHandle);

    if(ret == -1)
         nasPostDebug("[ %s %d ] ret:%d \n",__FILE__, __LINE__,ret);

    return ret;
}

static int nasQuotaSetParase(const char *xml_buf, int nLen)
{
    int ret = 0;
    mxml_node_t *xmlhandle = NULL;
    uint32_t nErrorCode;

    xmlhandle = mxmlLoadString(NULL, xml_buf, MXML_NO_CALLBACK);

    if(NULL == xmlhandle)
    {
         nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
         return -1;
    }

    char szErrorCode[128];
    if(getXmlNode(xmlhandle,"IE_RESULT.ErrorCode",szErrorCode) <0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }
    sscanf(szErrorCode, "%x", &nErrorCode);
    if(nErrorCode != 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }
    nasPostDebug("[ %s  %s  %d ] nErrorCode:%d\n",__FILE__,__FUNCTION__, __LINE__,nErrorCode);
done:

    destoryXmlRoot(xmlhandle);

    if(ret == -1)
         nasPostDebug("[ %s %d ] ret:%d \n",__FILE__, __LINE__,ret);

    return ret;
}
int nasQuotaSet(const char *Mac, const char * Guid, uint32_t SizeLimit,
                uint32_t TimeLimit)
{
    int ret = 0;
    char * xml_dmp_buf = NULL ;	
    int xml_dmp_buf_len = 0 ;
    mxml_node_t *XmlHandle;

    if(NULL == Mac || NULL == Guid)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        return -1;
    }

    XmlHandle = createXmlRootNode();
    if(NULL == XmlHandle)
    {
        nasPostDebug("[ %s %d ] createXmlRootNode failed\n",__FILE__, __LINE__);
        return -1;
    }

    ret = modifyXmlNode(XmlHandle,"IE_HEADER.MessageType", "MSG_SET_QUOTA");
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] modifyXmlNode failed\n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    ret = modifyXmlNode(XmlHandle,"IE_MAC_AUTH.MacAddress", Mac);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] modifyXmlNode failed\n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    ret = modifyXmlNode(XmlHandle,"IE_MAC_AUTH.ChannelGUID", Guid);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] modifyXmlNode failed\n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    char tmp[128];
    sprintf(tmp, "%d", SizeLimit);
    ret = modifyXmlNode(XmlHandle,"IE_TRK_QUOTA.SizeLimit", tmp);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] modifyXmlNode failed\n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    sprintf(tmp, "%d", TimeLimit);
    ret = modifyXmlNode(XmlHandle,"IE_TRK_QUOTA.TimeLimit", tmp);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] modifyXmlNode failed\n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    dumpXml2string(XmlHandle,&xml_dmp_buf, &xml_dmp_buf_len);
    if (xml_dmp_buf_len > 0)
    {
        char buf[1024];
        memset(buf, 0x00, sizeof(buf));
        uint32_t  nLen = 1024;
        ret = nasHttp_Post(NAS_QUOTA_SET,xml_dmp_buf, xml_dmp_buf_len, buf, &nLen);
        if(ret < 0 || nLen<= 0)
        {
            nasPostDebug("[ %s %d ] nasHttp_Post failed\n",__FILE__, __LINE__);
            ret = -1;
            goto done;
        }
        ret = nasQuotaSetParase(buf, nLen);
    }
    else
    {
        nasPostDebug("[ %s %d ] umpXml2string failed\n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }
	if(xml_dmp_buf)
    	free(xml_dmp_buf) ;

done:

    destoryXmlRoot(XmlHandle);

    if(ret == -1)
         nasPostDebug("[ %s %d ] ret:%d \n",__FILE__, __LINE__,ret);

    return ret;
}
static int nasMapTrackParase(const char *xml_buf, int nLen)
{
    int ret = 0;
    mxml_node_t *xmlhandle = NULL;

    uint32_t nErrorCode = 0;

    nasPostDebug("[ %s  %s  %d ] \n %s \n\n",__FILE__,__FUNCTION__, __LINE__,xml_buf);
    xmlhandle = mxmlLoadString(NULL, xml_buf, MXML_NO_CALLBACK);

    if(NULL == xmlhandle)
    {
        nasPostDebug("[ %s %d ] nLen:%d  %s\n",__FILE__, __LINE__,nLen,xml_buf);
        return -1;
    }

    char szErrorCode[128];
    if(getXmlNode(xmlhandle,"IE_RESULT.ErrorCode",szErrorCode) == 0)
    {
        sscanf(szErrorCode, "%x", &nErrorCode);
        nasPostDebug("[ %s  %s  %d ] nErrorCode:0x%x\n",__FILE__,__FUNCTION__, __LINE__,nErrorCode);
    }


    destoryXmlRoot(xmlhandle);

    if(ret == -1)
         nasPostDebug("[ %s %d ] ret:%d \n",__FILE__, __LINE__,ret);

    return ret;
}
static int nasMapTrack(const char *Guid, const char *puid,const int type, const int nChannel)
{
    int ret = 0;
    char * xml_dmp_buf = NULL ;	
    int xml_dmp_buf_len = 0 ;
    char tmp[16];
    mxml_node_t *XmlHandle;

    XmlHandle = createXmlRootNode();
    if(NULL == XmlHandle)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        return -1;
    }

    ret = modifyXmlNode(XmlHandle,"IE_HEADER.MessageType", "MSG_MAP_TRACK");
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }


    ret = modifyXmlNode(XmlHandle,"IE_ID_MAP.ChannelGUID", Guid);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    sprintf(tmp, "%d", type);
    ret = modifyXmlNode(XmlHandle,"IE_ID_MAP.PlatformType", tmp);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    ret = modifyXmlNode(XmlHandle,"IE_ID_MAP.PUID", puid);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }

    sprintf(tmp, "%d", nChannel);
    ret = modifyXmlNode(XmlHandle,"IE_ID_MAP.ChannelNo", tmp);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }


    dumpXml2string(XmlHandle,&xml_dmp_buf, &xml_dmp_buf_len);
    //nasPostDebug("[ %s %d ] %s \n",__FILE__, __LINE__, xml_dmp_buf);
    if (xml_dmp_buf_len > 0)
    {
        char buf[1024];
        memset(buf, 0x00, sizeof(buf));
        uint32_t  nLen = 1024;

        ret = nasHttp_Post(NAS_MAP_TRACK,xml_dmp_buf, xml_dmp_buf_len, buf, &nLen);

        if(ret < 0 || nLen<= 0)
        {
            nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
            ret = -1;
            goto done;
        }
        ret = nasMapTrackParase(buf, nLen);
    }
    else
    {
        nasPostDebug("[ %s %d ] \n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }
	if(xml_dmp_buf)
    	free(xml_dmp_buf) ;

done:

    destoryXmlRoot(XmlHandle);

    if(ret == -1)
         nasPostDebug("[ %s %d ] ret:%d \n",__FILE__, __LINE__,ret);

    return ret;

}


//static  char s_MacAddress[64];
//static  char s_NrmStorageIp[64];
static  int  s_NrmStorageIp;
static  int  s_NrmStoragePort;
//static  char s_NrmStorageIpBak[64];
static  char s_NrmStorageIpBak;
static  int  s_NrmStoragePortBak;
static 	char s_NrmPuid[64];
//static  char s_NrmPlatid[64];
static  char s_OtherPlatTermid[64] = {0, };

/* 记录视频会话id */
static  char s_ChannelGUID[OUTPUT_CHANNEL_NUM][64];

volatile char s_ChannelStorageStatus[OUTPUT_CHANNEL_NUM]={0};
static  int s_ChannelStartRepeatTimes[OUTPUT_CHANNEL_NUM] = {0};
//static  char s_ChannelLastNruIp[OUTPUT_CHANNEL_NUM][24];
//static  char s_ChannelNruHttpPort[OUTPUT_CHANNEL_NUM][16] = {0};
static  int s_ChannelLastNruIp[OUTPUT_CHANNEL_NUM];
static  int s_ChannelNruHttpPort[OUTPUT_CHANNEL_NUM];

static  ChannelStatus s_ChannelStatus[OUTPUT_CHANNEL_NUM] = {0};
static  sem_t s_sem_nas;
static sem_t s_sem_Record;
static MSG_Q_ID s_msgQ_redirect = NULL;
#if 0
int StorageCfgInit(void)
{
    char szDefault[256];
    const char *pszVal = 0;
/*
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%02x:%02x:%02x:%02x:%02x:%02x",0x00,0x11,\
        zone_rand(0x00,0xff),\
        zone_rand(0x00,0xff),\
        zone_rand(0x00,0xff),\
        zone_rand(0x00,0xff));
    pszVal =cfgSockGetValue("ethaddr",NULL,szDefault);
    strcpy(s_MacAddress,pszVal );
*/
    pszVal =cfgSockGetValue(NRM_STORAGE,NRM_STORAGE_IP,"0.0.0.0");
    strcpy(s_NrmStorageIp,pszVal );

    pszVal =cfgSockGetValue(NRM_STORAGE,NRM_STORAGE_PORT,"0");
    s_NrmStoragePort = atoi(pszVal);


    pszVal =cfgSockGetValue(NRM_STORAGE,NRM_STORAGE_IP_BAK,"0.0.0.0");
    strcpy(s_NrmStorageIpBak,pszVal );

    pszVal =cfgSockGetValue(NRM_STORAGE,NRM_STORAGE_PORT_BAK,"0");
    s_NrmStoragePortBak = atoi(pszVal);

	pszVal =cfgSockGetValue(NRM_STORAGE,NRM_STORAGE_PUID,"0");
    strcpy(s_NrmPuid,pszVal );
    
	/*
	pszVal =cfgSockGetValue(NRM_STORAGE,NRM_STORAGE_PLATID,"0");
	strcpy(s_NrmPlatid,pszVal );
	*/

	pszVal =cfgSockGetValue(OTHER_PLAT, OTHER_PLAT_GROUP_ID, "0");
	strcpy(s_OtherPlatTermid,pszVal);

	pszVal =cfgSockGetValue(OTHER_PLAT, OTHER_PLAT_TERM_CODE_EX, "0");
	strcpy(s_OtherPlatTermid + strlen(s_OtherPlatTermid), pszVal);

	pszVal =cfgSockGetValue(OTHER_PLAT, OTHER_PLAT_TERM_CODE, "0");
	strcpy(s_OtherPlatTermid + strlen(s_OtherPlatTermid), pszVal);
	
	
    printf("NruStorage Ip:%s  Port:%d Puid:%s Nruid:%s\n",s_NrmStorageIp,s_NrmStoragePort, s_NrmPuid, s_NrmPlatid);
	shell_print("NruStorage Ip:%s  Port:%d Puid:%s Nruid:%s\n",s_NrmStorageIp,s_NrmStoragePort, s_NrmPuid, s_NrmPlatid);
    for(int i=0; i<OUTPUT_CHANNEL_NUM; i++)
    {
        char tmp[16];
        sprintf(tmp , "%s_%d",NRM_STORAGE_GUID, i);
        pszVal =cfgSockGetValue(NRM_STORAGE,tmp,"00000000-0000-0000-0000-000000000000");
        strcpy(s_ChannelGUID[i],pszVal );
    }

    for(int i = 0; i < OUTPUT_CHANNEL_NUM; i++)
    {
        char tmp[16];
        sprintf(tmp , "%s_%d", LAST_NRU_STORAGE_IP, i);
        pszVal = cfgSockGetValue(LAST_NRU_STORAGE, tmp, "0.0.0.0");
        strcpy(s_ChannelLastNruIp[i], pszVal);
        printf("%s=%s:", LAST_NRU_STORAGE, pszVal);

        sprintf(tmp , "%s_%d", LAST_NRU_STORAGE_HTTP_PORT, i);
        pszVal = cfgSockGetValue(LAST_NRU_STORAGE, tmp, "0");
        strcpy(s_ChannelNruHttpPort[i], pszVal);
        printf("%s\n", pszVal);
    }
    
    memset((void*)s_ChannelStorageStatus, 0x00, sizeof(s_ChannelStorageStatus));

    if(sem_init(&s_sem_nas, 0 ,1) < 0)
    {
        nasPostDebug("%s %d  sem_init failed\n", __FILE__, __LINE__);
        return -1;
    }
	
    if(sem_init(&s_sem_Record, 0 ,1) < 0)
    {
        nasPostDebug("%s %d  sem_init failed\n", __FILE__, __LINE__);
        return -1;
    }

    s_msgQ_redirect = msgQCreate(16,sizeof(int),0);
    if(NULL == s_msgQ_redirect)
    {
        nasPostDebug("%s %d  msgQCreate failed\n", __FILE__, __LINE__);
        return -1;
    }
    
#if 1
    return nasHttpSet(s_NrmStorageIp, s_NrmStoragePort);
#else
    return nasHttpSet("172.16.65.120", 9090);
#endif
}
#else
int StorageCfgInit(void)
{
	int ret =0;
    NET_REC_MANAGE_INFO strNRMInfo;
    memset(&strNRMInfo, 0, sizeof(strNRMInfo));
	ret = SysConfGet(e_NET_REC_MANAGE_CONF, &strNRMInfo, 0);
	if( strNRMInfo.cEnable <= 0)
	{
		nasPostDebug("%s %d  nrm not enable,return!\n", __FILE__, __LINE__);
		return -1;
	}

    s_NrmStorageIp = strNRMInfo.uiMainNRMIp;

    s_NrmStoragePort = strNRMInfo.usMainNRMPort;

    s_NrmStorageIpBak = strNRMInfo.uiBakNRMIp;

    s_NrmStoragePortBak = strNRMInfo.usBakNRMPort;

    strcpy(s_NrmPuid, strNRMInfo.szPuID);
    
	strcpy(s_OtherPlatTermid, s_NrmPuid);
	
	nasPostDebug("NruStorage Ip:0x%x  Port:%d Puid:%s.\n",s_NrmStorageIp,s_NrmStoragePort, s_NrmPuid);

    for(int i = 0; i < OUTPUT_CHANNEL_NUM; i++)
    {
        s_ChannelLastNruIp[i] = strNRMInfo.uiLastNRUIp[i];
        s_ChannelNruHttpPort[i] = strNRMInfo.usLastNRUHttpPort[i];
    }
    
    memset((void*)s_ChannelStorageStatus, 0x00, sizeof(s_ChannelStorageStatus));

    if(sem_init(&s_sem_nas, 0 ,1) < 0)
    {
        nasPostDebug("%s %d  sem_init failed\n", __FILE__, __LINE__);
        return -1;
    }
	
    if(sem_init(&s_sem_Record, 0 ,1) < 0)
    {
        nasPostDebug("%s %d  sem_init failed\n", __FILE__, __LINE__);
        return -1;
    }

    s_msgQ_redirect = msgQCreate(16, sizeof(int),0);
    if(NULL == s_msgQ_redirect)
    {
        nasPostDebug("%s %d  msgQCreate failed\n", __FILE__, __LINE__);
        return -1;
    }
    
    return nasHttpInfoSet(s_NrmStorageIp, s_NrmStoragePort);

}
#endif


#if 0
char guid_test[16][64]=
{
    "7BDADC4F-D75F-11DF-B726-005056C00008",
    "7BF2D121-D75F-11DF-B9E5-005056C00008",
    "7C05E3F0-D75F-11DF-82BE-005056C00008",
    "7C18F6C0-D75F-11DF-B696-005056C00008",
    "7C29988F-D75F-11DF-8A1E-005056C00008",
    "7C3CAB61-D75F-11DF-BA1F-005056C00008",
    "7C4FBE30-D75F-11DF-A987-005056C00008",
    "7C606000-D75F-11DF-BC86-005056C00008"
};
#endif

int nasReportPuid(const char *puid1, const char *puid2)
{
    int ret = 0;
    int i;

    for(i=0; i< OUTPUT_CHANNEL_NUM; i++)
    {
        if(puid1)
        {
            nasMapTrack(s_ChannelGUID[i],puid1, 1, i+1);
//其他平台
            nasPostDebug("%s %s\n",s_ChannelGUID[i],puid1);
        }
        if(puid2)
        {
            nasMapTrack(s_ChannelGUID[i],puid2, 0, i+1);//自己的平台
            nasPostDebug("%s %s\n",s_ChannelGUID[i],puid2);
        }


    }


    return ret;
}

static void UpdateChannelStorageStatus(unsigned char nChannel, unsigned char usStatus)
{
    sem_wait(&s_sem_nas);
	//syslog(LOG_INFO, "Channed %d UpdateChannelStorageStatus status %d\n", nChannel, usStatus);
    s_ChannelStorageStatus[nChannel] = usStatus;
    sem_post(&s_sem_nas);
}
/*
static int MmsSendCallback(unsigned char *pszSessionId, void *pParam)
{
    int nChannel = (int)pParam;

    if( 0 == memcmp(s_ChannelGUID[nChannel],(void *)pszSessionId, sizeof(s_ChannelGUID[nChannel])))
    {
       nasPostDebug("[ %s %d ] channel :%d pszSessionId:%s  stop \n",
            __FILE__, __LINE__,nChannel+1, pszSessionId);

         syslog(LOG_INFO," channel :%d ChannelGUID:%s  stop\n",
                        nChannel+1, pszSessionId);

       UpdateChannelStorageStatus(nChannel, 0);
    }
    else
    {
        nasPostDebug("[ %s %d ] channel :%d pszSessionId:%s  expect sessionid : %s\n",
            __FILE__, __LINE__,nChannel+1, pszSessionId, s_ChannelGUID[nChannel]);
    }
    return 0;
}*/

int nasChannelStorageStart(unsigned int nChannel)
{
    int ret = 0;
    char szChannelGUID[128] = {0};
    char szPort[16] = {0};
    char szHttpPort[16] = {0};
    char szIp[32] = {0};
    char szSession[32] = {0};
    unsigned char  ucNetType = NAS_UDP;
    struct timeval tv_now;
    TActivePack activePack;
	unsigned long long ullSession;
    TActivePack SessionId;
	
    sem_wait(&s_sem_Record);
    if( nChannel >= OUTPUT_CHANNEL_NUM)
    {
        nasPostDebug("[ %s %d ] channel:%d  is invalid\n",__FILE__, __LINE__,nChannel);
        ret = -1;
        goto done;
    }

    nasPostDebug("[ %s %d ] Status:%d\n",__FILE__, __LINE__, s_ChannelStatus[nChannel].UsedNruIpFlag);

	nasPostDebug("[ %s:%d ] Chan:%d Stat:%d Repeat:%d TMOUT:%d\n",__FILE__, __LINE__, 
	                                                           nChannel,
	                                                           s_ChannelStatus[nChannel].UsedNruIpFlag,
	                                                           s_ChannelStartRepeatTimes[nChannel],
	                                                           s_ChannelStatus[nChannel].NruDeadTimes);

    if(s_ChannelStorageStatus[nChannel]&&(s_ChannelStatus[nChannel].NruDeadTimes < 3)
       &&(RECORDING_TO_NRU_GET_FROM_NRM == s_ChannelStatus[nChannel].UsedNruIpFlag))
    {
        nasPostDebug("[ %s %d ] channel:%d already recording NruDeadTimes:%d\n",__FILE__, __LINE__,nChannel, 
                                                                            s_ChannelStatus[nChannel].NruDeadTimes);
        ret = -1;
        goto done;
    }

    nasChannelStorageNrmManageIpPortSet(nChannel);

    //nasTrackClose(s_MacAddress,s_ChannelGUID[nChannel]);
    bzero(szChannelGUID, sizeof(szChannelGUID));
    ret = nasTrackOpen(nChannel,
						s_NrmPuid,
						s_OtherPlatTermid,
						96,
						NAS_UDP,
						VFMT_720P,
						DEFAULT_ENC_FRAMERATE,
						60,
						30,
						szIp,
						szPort,
						szHttpPort,
						szSession);
						

	if(ret < 0)
	{
		nasPostDebug("[ %s %d ] get nru ip failed.\n",__FILE__, __LINE__);
		nasChannelStorageDealWithGettingNruError(nChannel);
        goto done;
	}
	else//从服务端获取NRU IP成功
	{
		nasPostDebug("[ %s %d ] get nru ip success.\n",__FILE__, __LINE__);
		nasChannelStorageDealWithGettingNruSuccess(nChannel, szIp, szHttpPort);
	}
	
	syslog(LOG_INFO, "[ %s:%d ] Chan:%d Ip:%s Port:%s HttpPort:%s Session:%s ret = %d\n",\
	__FILE__, __LINE__, nChannel, szIp, szPort, szHttpPort, szSession, ret);

    nasPostDebug("[ %s %d ]UsedNruIpFlag:%d\n",__FILE__, __LINE__, s_ChannelStatus[nChannel].UsedNruIpFlag);

    UUID uuid;
    if(!uuid_create((uuid_t *)&uuid))
    {
        nasPostDebug("[ %s %d ] uuid_parse failed\n",__FILE__, __LINE__);
        ret = -1;
        goto done;
    }
	memcpy(szChannelGUID, &uuid, sizeof(UUID));
	memcpy(s_ChannelGUID[nChannel], &uuid, sizeof(UUID));

    s_ChannelStartRepeatTimes[nChannel] = 0;
	s_ChannelStatus[nChannel].NruDeadTimes = 0;
	s_ChannelStatus[nChannel].s_ChannelHeartBeatAddr.sin_family = AF_INET;
	s_ChannelStatus[nChannel].s_ChannelHeartBeatAddr.sin_port = htons(atoi(szPort) + 1);
	s_ChannelStatus[nChannel].s_ChannelHeartBeatAddr.sin_addr.s_addr = inet_addr(szIp);
	
    gettimeofday(&tv_now,  NULL);

    activePack.nPackLen = 8;
    ullSession = atoll(szSession);
    memcpy(activePack.ucPackBuf,&(ullSession),8);

    SessionId.nPackLen = sizeof(UUID);
    memcpy(SessionId.ucPackBuf,szChannelGUID,SessionId.nPackLen);

	s_ChannelStatus[nChannel].session = ullSession;
	
    ret = MmsStartEx(nChannel,
                        1,  // need active
                        (uNetAddrStr2IntHost(szIp)), atoi(szPort),
                        0xd, // need nas split mode (ssf)
                        ucNetType,
                        &activePack,
						(unsigned char *)SessionId.ucPackBuf, 
						SessionId.nPackLen,
						1   //send buffer before
                        /*,MmsSendCallback, (void*)nChannel*/);
	
	
    if(0!=ret)
    {
        ret = -1;
		nasPostDebug("[ %s:%d ] MmsStartEx failed, szIp:%s szPort:%s\n", __FILE__, __LINE__, szIp, szPort);
        goto done;
    }
    else
    {
		nasPostDebug("[ %s:%d ] MmsStartEx success, szIp:%s szPort:%s ret = %d\n", __FILE__, __LINE__, szIp, szPort, ret);    	
    }
	syslog(LOG_INFO, "[ %s:%d ] MmsStartEx success, szIp:%s szPort:%s ret = %d\n", __FILE__, __LINE__, szIp, szPort, ret);

	s_ChannelStatus[nChannel].NruDeadTimes = 0;  //开启录像，重置RTCP未到次数
    UpdateChannelStorageStatus(nChannel, 1);
done:
    sem_post(&s_sem_Record);
    return ret;
}

int nasChannelStorageStop(unsigned int nChannel)
{
    int ret = -1;
	
    sem_wait(&s_sem_Record);
    if( nChannel >= OUTPUT_CHANNEL_NUM)
    {
        nasPostDebug("[ %s %d ] channel:%d is invalid\n",__FILE__, __LINE__,nChannel);
        goto done;
    }

	s_ChannelStartRepeatTimes[nChannel] = 0;

    if(!s_ChannelStorageStatus[nChannel])
    {
        nasPostDebug("[ %s %d ] channel:%d no record \n",__FILE__, __LINE__,nChannel);
        goto done;
    }
    if(strcmp(s_ChannelGUID[nChannel], "00000000-0000-0000-0000-000000000000") == 0)
    {
        nasPostDebug("[ %s %d ] channel:%d  guid is invalid\n",__FILE__, __LINE__,s_ChannelGUID[nChannel]);
        goto done;
    }

    //ret = nasTrackClose(s_MacAddress,s_ChannelGUID[nChannel]);

    if(/*!ret && */ MmsStopEx((unsigned char *)(s_ChannelGUID[nChannel]),strlen((s_ChannelGUID[nChannel]))) < 0)
    {
        nasPostDebug("[ %s %d ] MmsStopEx failed (channel:%d)\n",__FILE__, __LINE__,nChannel);
        goto done;
    }
    else
    {
    	nasPostDebug("[ %s %d ] MmsStopEx ok (channel:%d)\n",__FILE__, __LINE__,nChannel);
    }

    s_ChannelStatus[nChannel].NruDeadTimes = 0;
    s_ChannelStatus[nChannel].UsedNruIpFlag = GETTING_NRU_IP_FROM_NRM;
    
    UpdateChannelStorageStatus(nChannel, 0);
done:
    sem_post(&s_sem_Record);
    return ret;
}


int nasChannelStorageNrmManageIpPortSet(unsigned int nChannel)
{
    switch(s_ChannelStatus[nChannel].UsedNruIpFlag)
    {
        case GETTING_NRU_IP_FROM_NRM:
        case RECORD_DIRECT_TO_NAS:
        //case RECORDING_TO_NRU_FROM_NRU_HTTP:
        case RECORDING_TO_NRU_GET_FROM_NRM:
    		nasPostDebug("[ %s %d ] channel:%d RECORDING_TO_NRU_GET_FROM_NRM\n",__FILE__, __LINE__,nChannel);
            nasHttpInfoSet(s_NrmStorageIp, s_NrmStoragePort);
	        //syslog(LOG_INFO, "[ %s %d ] chan:%d NRM=%d:%d\n",__FILE__, __LINE__,nChannel,s_NrmStorageIp,s_NrmStoragePort);
            break;
        case GETTING_NRU_IP_FROM_NRU_HTTP:
    		nasPostDebug("[ %s %d ] channel:%d GETTING_NRU_IP_FROM_NRU_HTTP\n",__FILE__, __LINE__,nChannel);
			nasHttpInfoSet(s_ChannelLastNruIp[nChannel], s_ChannelNruHttpPort[nChannel]);
	        //syslog(LOG_INFO, "[ %s %d ] chan:%d last NRU=%d:%d\n",__FILE__, __LINE__,nChannel,s_ChannelLastNruIp[nChannel],s_ChannelNruHttpPort[nChannel]);
            break;
        default:
            break;
    }
    return 0;
}


int nasChannelStorageDealWithGettingNruSuccess(unsigned int nChannel, char *szIp,char *szHttpPort)
{
	int ret =0;
	int iTmpIp =0;
   	NET_REC_MANAGE_INFO strNRMInfo;
   	
	nasPostDebug("[ %s %d ] channel:%d  szIp:%s httpPort:%s\n", __FILE__, __LINE__, nChannel, szIp, szHttpPort);
	/* 需要实际保存到配置文件中 */
    if(s_ChannelStatus[nChannel].UsedNruIpFlag != GETTING_NRU_IP_FROM_NRU_HTTP)
    {
    	iTmpIp = inet_addr(szIp);
    	iTmpIp = ntohl(iTmpIp);
    	
    	if(s_ChannelLastNruIp[nChannel] != iTmpIp)
    	{
	   	    s_ChannelLastNruIp[nChannel] = iTmpIp;
    	}

    	if( s_ChannelNruHttpPort[nChannel] != atoi(szHttpPort) )
    	{
    	    s_ChannelNruHttpPort[nChannel] = atoi(szHttpPort);//使用上次使用的NRU IP。及从NRM上获取到的NRU HTTP 端口
    	}
    	
	    memset(&strNRMInfo, 0, sizeof(strNRMInfo));
		ret = SysConfGet(e_NET_REC_MANAGE_CONF, &strNRMInfo, 0);
		if(ret == 0 )
		{
			strNRMInfo.uiLastNRUIp[nChannel] = s_ChannelLastNruIp[nChannel];
			strNRMInfo.usLastNRUHttpPort[nChannel] = s_ChannelNruHttpPort[nChannel];
			ret = SysConfSet(e_NET_REC_MANAGE_CONF, &strNRMInfo, 0);
			if(ret != 0)
			{
				nasPostDebug("[ %s %d ] save nrmInfo error!\n", __FILE__, __LINE__);
			}
			else
			{
				nasPostDebug("[ %s %d ] save nrmInfo ok!\n", __FILE__, __LINE__);
			}
		}
	}
	
    if(GETTING_NRU_IP_FROM_NRU_HTTP == s_ChannelStatus[nChannel].UsedNruIpFlag)
    {
		nasPostDebug("[ %s %d ] channel:%d  nasHttpSet back to NRM\n", __FILE__, __LINE__, nChannel);
        //s_ChannelStatus[nChannel].UsedNruIpFlag = RECORDING_TO_NRU_FROM_NRU_HTTP;
        //nasHttpInfoSet(s_NrmStorageIp, s_NrmStoragePort);//使用从上次使用NRU中获取的NRU IP录像，同时监控NRM是否恢复。

		s_ChannelStatus[nChannel].UsedNruIpFlag = RECORDING_TO_NRU_FROM_NRU_HTTP;
    }
    else if(RECORD_DIRECT_TO_NAS == s_ChannelStatus[nChannel].UsedNruIpFlag)//写NAS时NRM连接成功，数据写到NRM
    {
        s_ChannelStatus[nChannel].UsedNruIpFlag = RECORDING_TO_NRU_GET_FROM_NRM;
    }
    else if(GETTING_NRU_IP_FROM_NRM == s_ChannelStatus[nChannel].UsedNruIpFlag)
    {
		nasPostDebug("[ %s %d ] channel:%d  http port:%d\n", __FILE__, __LINE__, nChannel, s_ChannelNruHttpPort[nChannel]);

        s_ChannelStatus[nChannel].UsedNruIpFlag = RECORDING_TO_NRU_GET_FROM_NRM;
    }
    else if(RECORDING_TO_NRU_FROM_NRU_HTTP == s_ChannelStatus[nChannel].UsedNruIpFlag)
    {
        s_ChannelStatus[nChannel].UsedNruIpFlag = RECORDING_TO_NRU_GET_FROM_NRM;
    }

	nasPostDebug("[ %s %d ] MmsStopEx!\n", __FILE__, __LINE__);
	sleep(2);
    if(MmsStopEx((unsigned char *)(s_ChannelGUID[nChannel]), strlen((s_ChannelGUID[nChannel]))) < 0)
    {
        nasPostDebug("[ %s %d ] MmsStopEx failed (channel:%d),guid:%s.\n",__FILE__, __LINE__,nChannel,s_ChannelGUID[nChannel]);
    }
    else
    {
        nasPostDebug("[ %s %d ] MmsStopEx Success (channel:%d),guid:%s.\n",__FILE__, __LINE__,nChannel,s_ChannelGUID[nChannel]);
    }
    return 0;
}

int nasChannelStorageDealWithGettingNruError(unsigned int nChannel)
{
	s_ChannelStartRepeatTimes[nChannel]++;
	nasPostDebug("[ %s %d ] RepeatTimes[%d]=%d UsedNruIpFlag:%d\n",__FILE__, __LINE__,nChannel, s_ChannelStartRepeatTimes[nChannel],s_ChannelStatus[nChannel].UsedNruIpFlag);
	if(10 == s_ChannelStartRepeatTimes[nChannel])
	{			
		if(GETTING_NRU_IP_FROM_NRM == s_ChannelStatus[nChannel].UsedNruIpFlag)
		{
			s_ChannelStartRepeatTimes[nChannel] = 0;
    		nasPostDebug("[ %s %d ] channel:%d  last NRU=0x%x:%d\n",__FILE__, __LINE__,nChannel,s_ChannelLastNruIp[nChannel],s_ChannelNruHttpPort[nChannel]);
		    if( (0 != s_ChannelLastNruIp[nChannel])
			   &&(0 != s_ChannelNruHttpPort[nChannel]) )
			{
        		nasPostDebug("[ %s %d ] channel:%d  last NRU=0x%x:%d\n",__FILE__, __LINE__,nChannel,s_ChannelLastNruIp[nChannel],s_ChannelNruHttpPort[nChannel]);
				s_ChannelStatus[nChannel].UsedNruIpFlag = GETTING_NRU_IP_FROM_NRU_HTTP;
				nasHttpInfoSet(s_ChannelLastNruIp[nChannel], s_ChannelNruHttpPort[nChannel]);
			}
			else
			{
				s_ChannelStatus[nChannel].UsedNruIpFlag = RECORD_DIRECT_TO_NAS;
	            syslog(LOG_INFO, "[ %s:%d ] chan:%d RECORD_DIRECT_TO_NAS\n",__FILE__, __LINE__,nChannel);
        		nasPostDebug("[ %s %d ] channel:%d  RECORD_DIRECT_TO_NAS\n\n\n",__FILE__, __LINE__,nChannel);
			}
		}
		else if(GETTING_NRU_IP_FROM_NRU_HTTP == s_ChannelStatus[nChannel].UsedNruIpFlag)
		{
            nasHttpInfoSet(s_NrmStorageIp, s_NrmStoragePort);
            s_ChannelStartRepeatTimes[nChannel] = 0;
			s_ChannelStatus[nChannel].UsedNruIpFlag = RECORD_DIRECT_TO_NAS;
            syslog(LOG_INFO, "[ %s:%d ] chan:%d RECORD_DIRECT_TO_NAS\n",__FILE__, __LINE__,nChannel);
    		nasPostDebug("[ %s %d ] channel:%d  RECORD_DIRECT_TO_NAS\n",__FILE__, __LINE__,nChannel);
		}
	}
	
    if((s_ChannelStatus[nChannel].NruDeadTimes >= 3))
    {
		nasPostDebug("[ %s %d ] channel:%d  NruDeadTimes:%d\n", __FILE__, __LINE__,nChannel, s_ChannelStatus[nChannel].NruDeadTimes);
		if(RECORDING_TO_NRU_GET_FROM_NRM == s_ChannelStatus[nChannel].UsedNruIpFlag)
		{
    		nasPostDebug("[ %s %d ] channel:%d  RECORDING_TO_NRU_GET_FROM_NRM\n", __FILE__, __LINE__,nChannel);
		    nasPostDebug("[ %s %d ] channel:%d  change to GETTING_NRU_IP_FROM_NRM\n", __FILE__, __LINE__,nChannel);
	        s_ChannelStatus[nChannel].UsedNruIpFlag = GETTING_NRU_IP_FROM_NRM;
            UpdateChannelStorageStatus(nChannel, 0);
            if(MmsStopEx((unsigned char *)(s_ChannelGUID[nChannel]),strlen((s_ChannelGUID[nChannel]))) < 0)
            {
                nasPostDebug("[ %s %d ] MmsStopEx failed (channel:%d)\n",__FILE__, __LINE__,nChannel);
            }
            else
            {
                nasPostDebug("[ %s %d ] MmsStopEx successed (channel:%d)\n",__FILE__, __LINE__,nChannel);
            }
		}
		else if(RECORDING_TO_NRU_FROM_NRU_HTTP == s_ChannelStatus[nChannel].UsedNruIpFlag)//使用备用NRU,通过RTCP发现也不在线
		{
    		nasPostDebug("[ %s %d ] channel:%d  RECORDING_TO_NRU_FROM_NRU_HTTP\n", __FILE__, __LINE__,nChannel);
		    nasPostDebug("[ %s %d ] channel:%d  change to RECORD_DIRECT_TO_NAS\n", __FILE__, __LINE__,nChannel);
            syslog(LOG_INFO, "[ %s %d ] chan:%d RECORD_DIRECT_TO_NAS\n",__FILE__, __LINE__,nChannel);
            s_ChannelStatus[nChannel].UsedNruIpFlag = RECORD_DIRECT_TO_NAS;
            UpdateChannelStorageStatus(nChannel, 0);
            if(MmsStopEx((unsigned char *)(s_ChannelGUID[nChannel]),strlen((s_ChannelGUID[nChannel]))) < 0)
            {
                nasPostDebug("[ %s %d ] MmsStopEx failed (channel:%d)\n",__FILE__, __LINE__,nChannel);
            }
            else
            {
                nasPostDebug("[ %s %d ] MmsStopEx successed (channel:%d)\n",__FILE__, __LINE__,nChannel);
            }
		}
    }
    return 0;
}

int nasChannelStorageNruScan(unsigned int nChannel)
{
	int ret = 0;
	int ret2 = 0;
	
    if( nChannel >= OUTPUT_CHANNEL_NUM)
    {
        nasPostDebug("[ %s %d ] channel:%d  is invalid\n",__FILE__, __LINE__, nChannel);
        ret = -1;
        goto done;
    }
    
    if(!s_ChannelStorageStatus[nChannel])//未开启录像则RTCP不检测
    {
        nasPostDebug("[ %s %d ] channel:%d no record \n",__FILE__, __LINE__,nChannel);
        ret = 0;
        goto done;
    }

    ret2 = nasUdpHeartBeatSend(s_ChannelStatus[nChannel].s_ChannelHeartBeatAddr, 
                               s_ChannelStatus[nChannel].RtcpSquence++,
                               s_ChannelStatus[nChannel].session);
    if(0 != ret2)
    {
    	s_ChannelStatus[nChannel].NruDeadTimes++;
    }
    else
    {
    	s_ChannelStatus[nChannel].NruDeadTimes = 0;
    }
    
    nasPostDebug("[ %s %d ] channel:%d NruDeadTimes:%d\n",__FILE__, __LINE__, nChannel,
                                             s_ChannelStatus[nChannel].NruDeadTimes);

	if(s_ChannelStatus[nChannel].NruDeadTimes >= 3)
	{
		ret = -1;
	}
	else
	{
		ret = 0;
	}
    
done:
	return ret;
}

int nasChannelStorageGetStartTimes(unsigned int nChannel)
{
	return s_ChannelStartRepeatTimes[nChannel];
}

int nasChannelStorageGetRecordPositon(unsigned int nChannel)
{
	return s_ChannelStatus[nChannel].UsedNruIpFlag;
}

int nasChannelStorageGetNruStatus(unsigned int nChannel)
{
	return (s_ChannelStatus[nChannel].NruDeadTimes >=3)?-1:0;
}

static int nasRecordRedirectParse(const char *xml_buf, int nLen)
{
	int ret = 0;
	mxml_node_t *xmlhandle = NULL;
	char szBuf[128] = {0};
	int channelId = -1;
	unsigned int puid_len = 0;
	xmlhandle = mxmlLoadString(NULL, xml_buf, MXML_NO_CALLBACK);

	if(NULL == xmlhandle)
	{
		nasPostDebug("error:[ %s:%d ] xmlhandle is null\n",__FILE__, __LINE__);
	    return -1;
	}


	if(getXmlNode(xmlhandle,"Message.IE_HEADER.MessageType",szBuf) <0)
	{
		nasPostDebug("error:[ %s:%d ] get msgType failed.\n",__FILE__, __LINE__);
		ret = -1;
		goto done;
	}
	if(memcmp(szBuf ,"MSG_REDIRECT_REQ", strlen("MSG_REDIRECT_REQ")))
	{
		nasPostDebug("Redirect Msg Parse error, %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		ret = -1;
		goto done;
	}
	
	if(getXmlNode(xmlhandle,"Message.ChannelID",szBuf) <0)
	{
		nasPostDebug("error:[ %s:%s:%d ] get channel id error.\n",__FILE__,__FUNCTION__, __LINE__);

		ret = -1;
		goto done;
	}
	channelId = atoi(szBuf)-1;
	nasPostDebug("[ %s:%d ] channelId:%d\n",__FILE__, __LINE__,channelId);

    memset(szBuf, 0x00, sizeof(szBuf));
	if(getXmlNode(xmlhandle,"Message.PUID",szBuf) <0)
	{
		nasPostDebug("error:[ %s:%s:%d ] get puid error.\n",__FILE__,__FUNCTION__, __LINE__);

		ret = -1;
		goto done;
	}

    puid_len = strlen(s_NrmPuid);
    if(puid_len > sizeof(s_NrmPuid))
    {
    	nasPostDebug("error:[ %s %d ] puid:%s string len big arraw.\n",__FILE__, __LINE__,s_NrmPuid);
		ret = -1;
		goto done;
    }

    szBuf[puid_len] = '\0';
	nasPostDebug("[ %s %d ] org puid:%s recv:%s\n",__FILE__, __LINE__,s_NrmPuid, szBuf);
	if(0 != strcmp(s_NrmPuid, szBuf))
	{
		nasPostDebug("error:[ %s %d ] puid:%s recv:%s not match\n",__FILE__, __LINE__,s_NrmPuid, szBuf);
	    ret = -1;
	}
done:

	destoryXmlRoot(xmlhandle);
	nasPostDebug("[ %s:%d ]Redirect Req Msg  channelId:%d ret:%d\n",__FILE__, __LINE__, channelId, ret);
	if(ret == -1)
	{
		 nasPostDebug("error:[ %s %d ] ret:%d \n",__FILE__, __LINE__,ret);
		 return -1;
    }

	return (channelId>=0)?channelId:0;
}

int nasRecordRedirectResp(int sockfd ,int nErrorCode)
{
    int ret = 0;
	char tmp[128];
    char * xml_dmp_buf = NULL ;	
    int xml_dmp_buf_len = 0 ;
    mxml_node_t *XmlHandle;

    XmlHandle = createXmlRootNode();
    if(NULL == XmlHandle)
    {
        nasPostDebug("[ %s %d ] error,XmlHandle is null\n",__FUNCTION__, __LINE__);
        return -1;
    }
	sprintf(tmp , "%d", nErrorCode);
    ret = modifyXmlNode(XmlHandle,"Message.IE_RESULT.ErrorCode", tmp);
    if(ret < 0)
    {
        nasPostDebug("[ %s %d ] creat node failed\n",__FUNCTION__, __LINE__);
        ret = -2;
        goto done;
    }
	
	dumpXml2string(XmlHandle,&xml_dmp_buf, &xml_dmp_buf_len);

	if (xml_dmp_buf_len > 0)
	{
		char buf[1024];
		char szHeader[1204] = {0};
		memset(buf, 0x00, sizeof(buf));
		int  nLen = 1024;

		nLen = sprintf(szHeader, "HTTP/1.1 200 OK\r\nContent-Type:application/xml\r\nContent-Length:%zu\r\n", xml_dmp_buf_len);
		memcpy(buf, szHeader, nLen);
		memcpy(buf+nLen, xml_dmp_buf, xml_dmp_buf_len);
		//ret = nasHttp_Post(NAS_TRACK_OPEN,xml_dmp_buf, xml_dmp_buf_len, buf, &nLen);
		ret = send(sockfd, buf, nLen + xml_dmp_buf_len, 0);

		if(ret < 0 || nLen<= 0)
		{
			nasPostDebug("send redirect response error error: %d %s\n", errno, strerror(errno));
			syslog(LOG_ERR, "send redirect response error error: %d %s\n", errno, strerror(errno));
			ret = -3;
			goto done;
		}
		
		if( ret == (nLen+ xml_dmp_buf_len) )
		{
			nasPostDebug("[ %s:%d ] send Redirect Response msg sucess!\n",__FILE__, __LINE__);
		}
		else
		{
			nasPostDebug("[ %s:%d ] send Redirect Response msg failed!\n",__FILE__, __LINE__);
		}
	}
	else
	{
		nasPostDebug("[ %s:%d ] Redirect Response msg failed!\n",__FILE__, __LINE__);
		ret = -1;
		goto done;
	}
	if(xml_dmp_buf)
		free(xml_dmp_buf) ;

done:

	destoryXmlRoot(XmlHandle);

	if(ret < 0)
		 nasPostDebug("[ %s:%d ] send Redirect Response failed ret:%d \n",__FUNCTION__, __LINE__,ret);

	return ret;

}

int nasTrackRedirect(const char *xml_buf, int nLen)
{
	int nRet, channel;
	channel = nRet = nasRecordRedirectParse(xml_buf, nLen);
	if(nRet == -1)
	{
		nasPostDebug("[ %s:%d ] nasRecordRedirectParse failed\n",__FUNCTION__, __LINE__);
		return -1;
	}
	
	nRet = msgQSend(s_msgQ_redirect, (char*)(&channel), sizeof(channel), 0);

	nasPostDebug("[ %s:%d ] msgQSend s_msgQ_redirect\n",__FUNCTION__, __LINE__);

	return nRet;
}

MSG_Q_ID nasRedirectMsgQGet(void)
{
	return s_msgQ_redirect;
}

#endif
