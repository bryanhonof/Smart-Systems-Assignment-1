//*****************************************************************************
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//*****************************************************************************
//
// Application Name     - Free-RTOS Demo
// Application Overview - The objective of this application is to showcasing the 
//                        FreeRTOS feature like Multiple task creation, Inter 
//                        task communication using queues. Two tasks and one 
//                        queue is created. one of the task sends a constant 
//                        message into the queue and the other task receives the 
//                        same from the queue. after receiving every message, it 
//                        displays that message over UART.
// Application Details  -
// http://processors.wiki.ti.com/index.php/CC32xx_FreeRTOS_Application
// or
// docs\examples\CC32xx_FreeRTOS_Application.pdf
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup freertos_demo
//! @{
//
//*****************************************************************************

//*****************************************************************************
// Same application can be used to demonstrate TI-RTOS demo with a small change
// in CCS project proerties: 
//      a. add ti_rtos_config in workspace and add as dependencies in 
//         project setting Build->Dependecies->add
//      b. Define USE_TIRTOS in Properties->Build->Advanced Options->
//         Predefined Symbols instead of USE_FREERTOS
//      c. add ti_rtos.a in linker files search option
//*****************************************************************************

// Standard includes.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "osi.h"

// SimpleLink includes
#include "simplelink.h"

// Driverlib includes
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_types.h"
#include "hw_ints.h"
#include "interrupt.h"
#include "rom.h"
#include "rom_map.h"
#include "uart.h"
#include "prcm.h"
#include "utils.h"

// Common interface includes
#include "uart_if.h"
#include "common.h"
#include "pinmux.h"
#include "i2c_if.h"
#include "tmp006drv.h"

// HTTP Client lib
#include <http/client/httpcli.h>
#include <http/client/common.h>

// JSON Parser
#include "jsmn.h"

//*****************************************************************************
//                      MACRO DEFINITIONS
//*****************************************************************************
#define APPLICATION_VERSION     "1.1.1"
#define UART_PRINT              Report
#define SPAWN_TASK_PRIORITY     9
#define OSI_STACK_SIZE          4096
#define APP_NAME                "FreeRTOS Demo"
#define MAX_MSG_LENGTH			16

#define POST_REQUEST_URI        "/api/temp"
#define LOCATION                "Jeffrey Kot"
#define POST_DATA               "{\"value\":\"%.2f\",\"place\":\"%s\"}"

#define DELETE_REQUEST_URI      "/api/temp"


#define PUT_REQUEST_URI         "/api/temp"
#define PUT_DATA                "PUT request."

//#define GET_REQUEST_URI       "/get" http://iot.pxl-ea-ict.be/SmartSystems/add.php?temp1=66&hum1=45
#define GET_REQUEST_URI         "/api/compare"
#define HOST_NAME               "ss-pxl-11603121.herokuapp.com" //"<host name>"
#define HOST_PORT               80

#define PROXY_IP                <proxy_ip>
#define PROXY_PORT              <proxy_port>

#define READ_SIZE               1450
#define MAX_BUFF_SIZE           1460

// Application specific status/error codes
typedef enum{
 /* Choosing this number to avoid overlap with host-driver's error codes */
    DEVICE_NOT_IN_STATION_MODE = -0x7D0,
    DEVICE_START_FAILED = DEVICE_NOT_IN_STATION_MODE - 1,
    INVALID_HEX_STRING = DEVICE_START_FAILED - 1,
    TCP_RECV_ERROR = INVALID_HEX_STRING - 1,
    TCP_SEND_ERROR = TCP_RECV_ERROR - 1,
    FILE_NOT_FOUND_ERROR = TCP_SEND_ERROR - 1,
    INVALID_SERVER_RESPONSE = FILE_NOT_FOUND_ERROR - 1,
    FORMAT_NOT_SUPPORTED = INVALID_SERVER_RESPONSE - 1,
    FILE_OPEN_FAILED = FORMAT_NOT_SUPPORTED - 1,
    FILE_WRITE_ERROR = FILE_OPEN_FAILED - 1,
    INVALID_FILE = FILE_WRITE_ERROR - 1,
    SERVER_CONNECTION_FAILED = INVALID_FILE - 1,
    GET_HOST_IP_FAILED = SERVER_CONNECTION_FAILED  - 1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
volatile unsigned long  g_ulStatus = 0;//SimpleLink Status
unsigned long  g_ulDestinationIP; // IP address of destination server
unsigned long  g_ulGatewayIP = 0; //Network Gateway IP address
unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID
unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID
unsigned char g_buff[MAX_BUFF_SIZE+1];
long bytesReceived = 0; // variable to store the file size

// The queue used to send strings to the task1.
OsiMsgQ_t MsgQ;

#ifndef USE_TIRTOS
/* in case of TI-RTOS don't include startup_*.c in app project */
#if defined(gcc) || defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************


//*****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS
//*****************************************************************************
static void vTestTask1( void *pvParameters );
static void vTestTask2( void *pvParameters );
static void vTempTask(void *pvParameters);
static void vUploadDataTask(void *pvParameters);
static void BoardInit();

//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- Start
//*****************************************************************************


//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);

            //
            // Information about the connected AP (like name, MAC etc) will be
            // available in 'slWlanConnectAsyncResponse_t'-Applications
            // can use it if required
            //
            //  slWlanConnectAsyncResponse_t *pEventData = NULL;
            // pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            //

            // Copy new connection SSID and BSSID to global parameters
            memcpy(g_ucConnectionSSID,pWlanEvent->EventData.
                   STAandP2PModeWlanConnected.ssid_name,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
            memcpy(g_ucConnectionBSSID,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
                   SL_BSSID_LENGTH);

            UART_PRINT("[WLAN EVENT] STA Connected to the AP: %s ,"
                            " BSSID: %x:%x:%x:%x:%x:%x\n\r",
                      g_ucConnectionSSID,g_ucConnectionBSSID[0],
                      g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                      g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                      g_ucConnectionBSSID[5]);
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            // If the user has initiated 'Disconnect' request,
            //'reason_code' is SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION
            if(SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                UART_PRINT("[WLAN EVENT]Device disconnected from the AP: %s,"
                "BSSID: %x:%x:%x:%x:%x:%x on application's request \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            else
            {
                UART_PRINT("[WLAN ERROR]Device disconnected from the AP AP: %s,"
                "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
            memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
        }
        break;

        default:
        {
            UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                       pWlanEvent->Event);
        }
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        {
            SlIpV4AcquiredAsync_t *pEventData = NULL;

            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            //Ip Acquired Event Data
            pEventData = &pNetAppEvent->EventData.ipAcquiredV4;

            //Gateway IP address
            g_ulGatewayIP = pEventData->gateway;

            UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
            "Gateway=%d.%d.%d.%d\n\r",
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,0),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,0));
        }
        break;

        default:
        {
            UART_PRINT("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
                       pNetAppEvent->Event);
        }
        break;
    }
}


//*****************************************************************************
//
//! \brief This function handles HTTP server events
//!
//! \param[in]  pServerEvent - Contains the relevant event information
//! \param[in]    pServerResponse - Should be filled by the user with the
//!                                      relevant response information
//!
//! \return None
//!
//****************************************************************************
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
    // Unused in this application
}

//*****************************************************************************
//
//! \brief This function handles General Events
//!
//! \param[in]     pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    //
    // Most of the general errors are not FATAL are are to be handled
    // appropriately by the application
    //
    UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
               pDevEvent->EventData.deviceEvent.status,
               pDevEvent->EventData.deviceEvent.sender);
}


//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]      pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    //
    // This application doesn't work w/ socket - Events are not expected
    //
       switch( pSock->Event )
    {
        case SL_SOCKET_TX_FAILED_EVENT:
            switch( pSock->socketAsyncEvent.SockTxFailData.status )
            {
                case SL_ECLOSE:
                    UART_PRINT("[SOCK ERROR] - close socket (%d) operation "
                    "failed to transmit all queued packets\n\n",
                           pSock->socketAsyncEvent.SockAsyncData.sd);
                    break;
                default:
                    UART_PRINT("[SOCK ERROR] - TX FAILED : socket %d , reason"
                        "(%d) \n\n",
                        pSock->socketAsyncEvent.SockAsyncData.sd,
                        pSock->socketAsyncEvent.SockTxFailData.status);
            }
            break;

        default:
            UART_PRINT("[SOCK EVENT] - Unexpected Event [%x0x]\n\n",pSock->Event);
    }
}


//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- End
//*****************************************************************************

//*****************************************************************************
//
//! \brief This function initializes the application variables
//!
//! \param    None
//!
//! \return None
//!
//*****************************************************************************
static void InitializeAppVariables()
{
    g_ulStatus = 0;
    g_ulGatewayIP = 0;
    memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
    memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
}


//*****************************************************************************
//! \brief This function puts the device in its default state. It:
//!           - Set the mode to STATION
//!           - Configures connection policy to Auto and AutoSmartConfig
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************
static long ConfigureSimpleLinkToDefaultState()
{
    SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode = -1;

    lMode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lMode);

    // If the device is not in station-mode, try configuring it in station-mode
    if (ROLE_STA != lMode)
    {
        if (ROLE_AP == lMode)
        {
            // If the device is in AP mode, we need to wait for this event
            // before doing anything
            while(!IS_IP_ACQUIRED(g_ulStatus))
            {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask();
#endif
            }
        }

        // Switch to STA role and restart
        lRetVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is in station again
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in STA-mode
            return DEVICE_NOT_IN_STATION_MODE;
        }
    }

    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt,
                                &ucConfigLen, (unsigned char *)(&ver));
    ASSERT_ON_ERROR(lRetVal);

    UART_PRINT("Host Driver Version: %s\n\r",SL_DRIVER_VERSION);
    UART_PRINT("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n\r",
    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
    ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
    ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
    ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
    ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);

    // Set connection policy to Auto + SmartConfig
    //      (Device's default connection policy)
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION,
                                SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove all profiles
    lRetVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(lRetVal);

    //
    // Device in station-mode. Disconnect previous connection if any
    // The function returns 0 if 'Disconnected done', negative number if already
    // disconnected Wait for 'disconnection' event if 0 is returned, Ignore
    // other return-codes
    //
    lRetVal = sl_WlanDisconnect();
    if(0 == lRetVal)
    {
        // Wait
        while(IS_CONNECTED(g_ulStatus))
        {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask();
#endif
        }
    }

    // Enable DHCP client
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
    ASSERT_ON_ERROR(lRetVal);

    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID,
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
    ASSERT_ON_ERROR(lRetVal);

    // Set PM policy to normal
    lRetVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Unregister mDNS services
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(lRetVal);


    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(lRetVal);

    InitializeAppVariables();

    return SUCCESS;
}



//****************************************************************************
//
//! \brief Connecting to a WLAN Accesspoint
//!
//!  This function connects to the required AP (SSID_NAME) with Security
//!  parameters specified in te form of macros at the top of this file
//!
//! \param  Status value
//!
//! \return  None
//!
//! \warning    If the WLAN connection fails or we don't aquire an IP
//!            address, It will be stuck in this function forever.
//
//****************************************************************************
static long WlanConnect()
{
    SlSecParams_t secParams = {0};
    long lRetVal = 0;

    secParams.Key = (signed char *)SECURITY_KEY;
    secParams.KeyLen = strlen(SECURITY_KEY);
    secParams.Type = SECURITY_TYPE;

    lRetVal = sl_WlanConnect((signed char *)SSID_NAME,
                           strlen((const char *)SSID_NAME), 0, &secParams, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Wait for WLAN Event
    while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus)))
    {
        // wait till connects to an AP
        _SlNonOsMainLoopTask();
    }

    return SUCCESS;

}


//*****************************************************************************
//
//! \brief Flush response body.
//!
//! \param[in]  httpClient - Pointer to HTTP Client instance
//!
//! \return 0 on success else error code on failure
//!
//*****************************************************************************
static int FlushHTTPResponse(HTTPCli_Handle httpClient)
{
    const char *ids[2] = {
                            HTTPCli_FIELD_NAME_CONNECTION, /* App will get connection header value. all others will skip by lib */
                            NULL
                         };
    char buf[128];
    int id;
    int len = 1;
    bool moreFlag = 0;
    char ** prevRespFilelds = NULL;


    /* Store previosly store array if any */
    prevRespFilelds = HTTPCli_setResponseFields(httpClient, ids);

    /* Read response headers */
    while ((id = HTTPCli_getResponseField(httpClient, buf, sizeof(buf), &moreFlag))
            != HTTPCli_FIELD_ID_END)
    {

        if(id == 0)
        {
            if(!strncmp(buf, "close", sizeof("close")))
            {
                UART_PRINT("Connection terminated by server\n\r");
            }
        }

    }

    /* Restore previosuly store array if any */
    HTTPCli_setResponseFields(httpClient, (const char **)prevRespFilelds);

    while(1)
    {
        /* Read response data/body */
        /* Note:
                moreFlag will be set to 1 by HTTPCli_readResponseBody() call, if more
                data is available Or in other words content length > length of buffer.
                The remaining data will be read in subsequent call to HTTPCli_readResponseBody().
                Please refer HTTP Client Libary API documenation @ref HTTPCli_readResponseBody
                for more information.
        */
        HTTPCli_readResponseBody(httpClient, buf, sizeof(buf) - 1, &moreFlag);
        ASSERT_ON_ERROR(len);

        if ((len - 2) >= 0 && buf[len - 2] == '\r' && buf [len - 1] == '\n'){
            break;
        }

        if(!moreFlag)
        {
            /* There no more data. break the loop. */
            break;
        }
    }
    return 0;
}


//*****************************************************************************
//
//! \brief Handler for parsing JSON data
//!
//! \param[in]  ptr - Pointer to http response body data
//!
//! \return 0 on success else error code on failure
//!
//*****************************************************************************
int ParseJSONData(char *ptr)
{
    long lRetVal = 0;
    int noOfToken;
    jsmn_parser parser;
    jsmntok_t   *tokenList;


    /* Initialize JSON PArser */
    jsmn_init(&parser);

    /* Get number of JSON token in stream as we we dont know how many tokens need to pass */
    noOfToken = jsmn_parse(&parser, (const char *)ptr, strlen((const char *)ptr), NULL, 10);
    if(noOfToken <= 0)
    {
        UART_PRINT("Failed to initialize JSON parser\n\r");
        return -1;

    }

    /* Allocate memory to store token */
    tokenList = (jsmntok_t *) malloc(noOfToken*sizeof(jsmntok_t));
    if(tokenList == NULL)
    {
        UART_PRINT("Failed to allocate memory\n\r");
        return -1;
    }

    /* Initialize JSON Parser again */
    jsmn_init(&parser);
    noOfToken = jsmn_parse(&parser, (const char *)ptr, strlen((const char *)ptr), tokenList, noOfToken);
    if(noOfToken < 0)
    {
        UART_PRINT("Failed to parse JSON tokens\n\r");
        lRetVal = noOfToken;
    }
    else
    {
        UART_PRINT("Successfully parsed %ld JSON tokens\n\r", noOfToken);
    }

    free(tokenList);

    return lRetVal;
}

/*!
    \brief This function read respose from server and dump on console

    \param[in]      httpClient - HTTP Client object

    \return         0 on success else -ve

    \note

    \warning
*/
static int readResponse(HTTPCli_Handle httpClient)
{
    long lRetVal = 0;
    int bytesRead = 0;
    int id = 0;
    unsigned long len = 0;
    int json = 0;
    char *dataBuffer=NULL;
    bool moreFlags = 1;
    const char *ids[4] = {
                            HTTPCli_FIELD_NAME_CONTENT_LENGTH,
                            HTTPCli_FIELD_NAME_CONNECTION,
                            HTTPCli_FIELD_NAME_CONTENT_TYPE,
                            NULL
                         };

    /* Read HTTP POST request status code */
    lRetVal = HTTPCli_getResponseStatus(httpClient);
    if(lRetVal > 0)
    {
        switch(lRetVal)
        {
        case 200:
        {
            UART_PRINT("HTTP Status 200\n\r");
            /*
                 Set response header fields to filter response headers. All
                  other than set by this call we be skipped by library.
             */
            HTTPCli_setResponseFields(httpClient, (const char **)ids);

            /* Read filter response header and take appropriate action. */
            /* Note:
                    1. id will be same as index of fileds in filter array setted
                    in previous HTTPCli_setResponseFields() call.

                    2. moreFlags will be set to 1 by HTTPCli_getResponseField(), if  field
                    value could not be completely read. A subsequent call to
                    HTTPCli_getResponseField() will read remaining field value and will
                    return HTTPCli_FIELD_ID_DUMMY. Please refer HTTP Client Libary API
                    documenation @ref HTTPCli_getResponseField for more information.
             */
            while((id = HTTPCli_getResponseField(httpClient, (char *)g_buff, sizeof(g_buff), &moreFlags))
                    != HTTPCli_FIELD_ID_END)
            {

                switch(id)
                {
                case 0: /* HTTPCli_FIELD_NAME_CONTENT_LENGTH */
                {
                    len = strtoul((char *)g_buff, NULL, 0);
                }
                break;
                case 1: /* HTTPCli_FIELD_NAME_CONNECTION */
                {
                }
                break;
                case 2: /* HTTPCli_FIELD_NAME_CONTENT_TYPE */
                {
                    if(!strncmp((const char *)g_buff, "application/json",
                            sizeof("application/json")))
                    {
                        json = 1;
                    }
                    else
                    {
                        /* Note:
                                Developers are advised to use appropriate
                                content handler. In this example all content
                                type other than json are treated as plain text.
                         */
                        json = 0;
                    }
                    UART_PRINT(HTTPCli_FIELD_NAME_CONTENT_TYPE);
                    UART_PRINT(" : ");
                    UART_PRINT("application/json\n\r");
                }
                break;
                default:
                {
                    UART_PRINT("Wrong filter id\n\r");
                    lRetVal = -1;
                    goto end;
                }
                }
            }
            bytesRead = 0;
            if(len > sizeof(g_buff))
            {
                dataBuffer = (char *) malloc(len);
                if(dataBuffer)
                {
                    UART_PRINT("Failed to allocate memory\n\r");
                    lRetVal = -1;
                    goto end;
                }
            }
            else
            {
                dataBuffer = (char *)g_buff;
            }

            /* Read response data/body */
            /* Note:
                    moreFlag will be set to 1 by HTTPCli_readResponseBody() call, if more
                    data is available Or in other words content length > length of buffer.
                    The remaining data will be read in subsequent call to HTTPCli_readResponseBody().
                    Please refer HTTP Client Libary API documenation @ref HTTPCli_readResponseBody
                    for more information

             */
            bytesRead = HTTPCli_readResponseBody(httpClient, (char *)dataBuffer, len, &moreFlags);
            if(bytesRead < 0)
            {
                UART_PRINT("Failed to received response body\n\r");
                lRetVal = bytesRead;
                goto end;
            }
            else if( bytesRead < len || moreFlags)
            {
                UART_PRINT("Mismatch in content length and received data length\n\r");
                goto end;
            }
            dataBuffer[bytesRead] = '\0';

            if(json)
            {
                /* Parse JSON data */
                lRetVal = ParseJSONData(dataBuffer);
                if(lRetVal < 0)
                {
                    goto end;
                }
            }
            else
            {
                /* treating data as a plain text */
            }

        }
        break;

        case 404:
            UART_PRINT("File not found. \r\n");
            /* Handle response body as per requirement.
                  Note:
                    Developers are advised to take appopriate action for HTTP
                    return status code else flush the response body.
                    In this example we are flushing response body in default
                    case for all other than 200 HTTP Status code.
             */
        default:
            /* Note:
              Need to flush received buffer explicitly as library will not do
              for next request.Apllication is responsible for reading all the
              data.
             */
            FlushHTTPResponse(httpClient);
            break;
        }
    }
    else
    {
        UART_PRINT("Failed to receive data from server.\r\n");
        goto end;
    }

    lRetVal = 0;

end:
    if(len > sizeof(g_buff) && (dataBuffer != NULL))
    {
        free(dataBuffer);
    }
    return lRetVal;
}

//*****************************************************************************
//
//! \brief HTTP POST Demonstration
//!
//! \param[in]  httpClient - Pointer to http client
//!
//! \return 0 on success else error code on failure
//!
//*****************************************************************************
static int HTTPPostMethod(HTTPCli_Handle httpClient, char *message, size_t messageSize)
{
    bool moreFlags = 1;
    bool lastFlag = 1;
    char tmpBuf[4];
    long lRetVal = 0;
    HTTPCli_Field fields[4] = {
                                {HTTPCli_FIELD_NAME_HOST, HOST_NAME},
                                {HTTPCli_FIELD_NAME_ACCEPT, "*/*"},
                                {HTTPCli_FIELD_NAME_CONTENT_TYPE, "application/json"},
                                {NULL, NULL}
                            };


    /* Set request header fields to be send for HTTP request. */
    HTTPCli_setRequestFields(httpClient, fields);

    /* Send POST method request. */
    /* Here we are setting moreFlags = 1 as there are some more header fields need to send
       other than setted in previous call HTTPCli_setRequestFields() at later stage.
       Please refer HTTP Library API documentaion @ref HTTPCli_sendRequest for more information.
    */
    moreFlags = 1;
    lRetVal = HTTPCli_sendRequest(httpClient, HTTPCli_METHOD_POST, POST_REQUEST_URI, moreFlags);
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to send HTTP POST request header.\n\r");
        return lRetVal;
    }

    sprintf((char *)tmpBuf, "%d", messageSize - 1);
    UART_PRINT("%s\r\n", tmpBuf);

    /* Here we are setting lastFlag = 1 as it is last header field.
       Please refer HTTP Library API documentaion @ref HTTPCli_sendField for more information.
    */
    lastFlag = 1;
    lRetVal = HTTPCli_sendField(httpClient, HTTPCli_FIELD_NAME_CONTENT_LENGTH, (const char *)tmpBuf, lastFlag);
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to send HTTP POST request header.\n\r");
        return lRetVal;
    }


    /* Send POST data/body */
    UART_PRINT("%s\r\n", message);
    lRetVal = HTTPCli_sendRequestBody(httpClient, message, messageSize - 1);
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to send HTTP POST request body.\n\r");
        return lRetVal;
    }


    lRetVal = readResponse(httpClient);

    return lRetVal;
}


//*****************************************************************************
//
//! \brief HTTP DELETE Demonstration
//!
//! \param[in]  httpClient - Pointer to http client
//!
//! \return 0 on success else error code on failure
//!
//*****************************************************************************
static int HTTPDeleteMethod(HTTPCli_Handle httpClient)
{

    long lRetVal = 0;
    HTTPCli_Field fields[3] = {
                                {HTTPCli_FIELD_NAME_HOST, HOST_NAME},
                                {HTTPCli_FIELD_NAME_ACCEPT, "*/*"},
                                {NULL, NULL}
                            };
    bool moreFlags;


    /* Set request header fields to be send for HTTP request. */
    HTTPCli_setRequestFields(httpClient, fields);

    /* Send DELETE method request. */
    /* Here we are setting moreFlags = 0 as there are no more header fields need to send
       at later stage. Please refer HTTP Library API documentaion @ref HTTPCli_sendRequest
       for more information.
    */
    moreFlags = 0;
    lRetVal = HTTPCli_sendRequest(httpClient, HTTPCli_METHOD_DELETE, DELETE_REQUEST_URI, moreFlags);
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to send HTTP DELETE request header.\n\r");
        return lRetVal;
    }

    lRetVal = readResponse(httpClient);

    return lRetVal;
}

//*****************************************************************************
//
//! \brief HTTP PUT Demonstration
//!
//! \param[in]  httpClient - Pointer to http client
//!
//! \return 0 on success else error code on failure
//!
//*****************************************************************************
static int HTTPPutMethod(HTTPCli_Handle httpClient)
{

    long lRetVal = 0;
    HTTPCli_Field fields[4] = {
                                {HTTPCli_FIELD_NAME_HOST, HOST_NAME},
                                {HTTPCli_FIELD_NAME_ACCEPT, "*/*"},
                                {HTTPCli_FIELD_NAME_CONTENT_TYPE, "text/html"},
                                {NULL, NULL}
                            };
    bool        moreFlags = 1;
    bool        lastFlag = 1;
    char        tmpBuf[4];


    /* Set request header fields to be send for HTTP request. */
    HTTPCli_setRequestFields(httpClient, fields);

    /* Send PUT method request. */
    /* Here we are setting moreFlags = 1 as there are some more header fields need to send
       other than setted in previous call HTTPCli_setRequestFields() at later stage.
       Please refer HTTP Library API documentaion @ref HTTPCli_sendRequest for more information.
    */
    moreFlags = 1;
    lRetVal = HTTPCli_sendRequest(httpClient, HTTPCli_METHOD_PUT, PUT_REQUEST_URI, moreFlags);
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to send HTTP PUT request header.\n\r");
        return lRetVal;
    }

    sprintf((char *)tmpBuf, "%d", (sizeof(PUT_DATA)-1));

    /* Here we are setting lastFlag = 1 as it is last header field.
       Please refer HTTP Library API documentaion @ref HTTPCli_sendField for more information.
    */
    lastFlag = 1;
    lRetVal = HTTPCli_sendField(httpClient, HTTPCli_FIELD_NAME_CONTENT_LENGTH, (char *)tmpBuf, lastFlag);
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to send HTTP PUT request header.\n\r");
        return lRetVal;
    }

    /* Send PUT data/body */
    lRetVal = HTTPCli_sendRequestBody(httpClient, PUT_DATA, (sizeof(PUT_DATA)-1));
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to send HTTP PUT request body.\n\r");
        return lRetVal;
    }

    lRetVal = readResponse(httpClient);

    return lRetVal;
}

//*****************************************************************************
//
//! \brief HTTP GET Demonstration
//!
//! \param[in]  httpClient - Pointer to http client
//!
//! \return 0 on success else error code on failure
//!
//*****************************************************************************
static int HTTPGetMethod(HTTPCli_Handle httpClient)
{

    long lRetVal = 0;
    HTTPCli_Field fields[4] = {
                                {HTTPCli_FIELD_NAME_HOST, HOST_NAME},
                                {HTTPCli_FIELD_NAME_ACCEPT, "*/*"},
                                {HTTPCli_FIELD_NAME_CONTENT_LENGTH, "0"},
                                {NULL, NULL}
                            };
    bool        moreFlags;


    /* Set request header fields to be send for HTTP request. */
    HTTPCli_setRequestFields(httpClient, fields);

    /* Send GET method request. */
    /* Here we are setting moreFlags = 0 as there are no more header fields need to send
       at later stage. Please refer HTTP Library API documentaion @ HTTPCli_sendRequest
       for more information.
    */
    moreFlags = 0;
    lRetVal = HTTPCli_sendRequest(httpClient, HTTPCli_METHOD_GET, GET_REQUEST_URI, moreFlags);
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to send HTTP GET request.\n\r");
        return lRetVal;
    }


    lRetVal = readResponse(httpClient);

    return lRetVal;
}


//*****************************************************************************
//
//! Function to connect to AP
//!
//! \param  none
//!
//! \return Error-code or SUCCESS
//!
//*****************************************************************************
static long ConnectToAP()
{
    long lRetVal = -1;

    //
    // Following function configure the device to default state by cleaning
    // the persistent settings stored in NVMEM (viz. connection profiles &
    // policies, power policy etc)
    //
    // Applications may choose to skip this step if the developer is sure
    // that the device is in its desired state at start of applicaton
    //
    // Note that all profiles and persistent settings that were done on the
    // device will be lost
    //
    lRetVal = ConfigureSimpleLinkToDefaultState();
    if(lRetVal < 0)
    {
        if (DEVICE_NOT_IN_STATION_MODE == lRetVal)
        {
            UART_PRINT("Failed to configure the device in its default state, "
                            "Error-code: %d\n\r", DEVICE_NOT_IN_STATION_MODE);
        }

        return -1;
    }

    UART_PRINT("Device is configured in default state \n\r");

    //
    // Assumption is that the device is configured in station mode already
    // and it is in its default state
    //
    lRetVal = sl_Start(0, 0, 0);
    if (lRetVal < 0 || ROLE_STA != lRetVal)
    {
        ASSERT_ON_ERROR(DEVICE_START_FAILED);
    }

    UART_PRINT("Device started as STATION \n\r");

    // Connecting to WLAN AP - Set with static parameters defined at the top
    // After this call we will be connected and have IP address
    lRetVal = WlanConnect();

    UART_PRINT("Connected to the AP: %s\r\n", SSID_NAME);
    return 0;
}

//*****************************************************************************
//
//! Function to connect to HTTP server
//!
//! \param  httpClient - Pointer to HTTP Client instance
//!
//! \return Error-code or SUCCESS
//!
//*****************************************************************************
static int ConnectToHTTPServer(HTTPCli_Handle httpClient)
{
    long lRetVal = -1;
    struct sockaddr_in addr;


#ifdef USE_PROXY
    struct sockaddr_in paddr;
    paddr.sin_family = AF_INET;
    paddr.sin_port = htons(PROXY_PORT);
    paddr.sin_addr.s_addr = sl_Htonl(PROXY_IP);
    HTTPCli_setProxy((struct sockaddr *)&paddr);
#endif

    /* Resolve HOST NAME/IP */
    lRetVal = sl_NetAppDnsGetHostByName((signed char *)HOST_NAME,
                                          strlen((const char *)HOST_NAME),
                                          &g_ulDestinationIP,SL_AF_INET);
    if(lRetVal < 0)
    {
        ASSERT_ON_ERROR(GET_HOST_IP_FAILED);
    }

    /* Set up the input parameters for HTTP Connection */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(HOST_PORT);
    addr.sin_addr.s_addr = sl_Htonl(g_ulDestinationIP);

    /* Testing HTTPCli open call: handle, address params only */
    HTTPCli_construct(httpClient);
    lRetVal = HTTPCli_connect(httpClient, (struct sockaddr *)&addr, 0, NULL);
    if (lRetVal < 0)
    {
        UART_PRINT("Connection to server failed. error(%d)\n\r", lRetVal);
        ASSERT_ON_ERROR(SERVER_CONNECTION_FAILED);
    }
    else
    {
        UART_PRINT("Connection to server created successfully\r\n");
    }

    return 0;
}

#ifdef USE_FREERTOS
//*****************************************************************************
// FreeRTOS User Hook Functions enabled in FreeRTOSConfig.h
//*****************************************************************************

//*****************************************************************************
//
//! \brief Application defined hook (or callback) function - assert
//!
//! \param[in]  pcFile - Pointer to the File Name
//! \param[in]  ulLine - Line Number
//!
//! \return none
//!
//*****************************************************************************
void
vAssertCalled( const char *pcFile, unsigned long ulLine )
{
    //Handle Assert here
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined idle task hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void
vApplicationIdleHook( void)
{
    //Handle Idle Hook for Profiling, Power Management etc
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    //Handle Memory Allocation Errors
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook( OsiTaskHandle *pxTask,
                                   signed char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}
#endif //USE_FREERTOS

//******************************************************************************
//
//! First test task
//!
//! \param pvParameters is the parameter passed to the task while creating it.
//!
//!    This Function
//!        1. Receive message from the Queue and display it on the terminal.
//!
//! \return none
//
//******************************************************************************
void vTestTask1( void *pvParameters )
{
	char pcMessage[MAX_MSG_LENGTH];
    for( ;; )
    {
    	/* Wait for a message to arrive. */
    	osi_MsgQRead(&MsgQ, pcMessage, OSI_WAIT_FOREVER);

		UART_PRINT("message = ");
		UART_PRINT(pcMessage);
		UART_PRINT("\n\r");
		osi_Sleep(200);
    }
}

//******************************************************************************
//
//! Second test task
//!
//! \param pvParameters is the parameter passed to the task while creating it.
//!
//!    This Function
//!        1. Creates a message and send it to the queue.
//!
//! \return none
//
//******************************************************************************
void vTestTask2( void *pvParameters )
{
   unsigned long ul_2;
   const char *pcInterruptMessage[4] = {"Welcome","to","CC32xx"
           ,"development !\n"};

   ul_2 =0;

   for( ;; )
     {
       /* Queue a message for the print task to display on the UART CONSOLE. */
	   osi_MsgQWrite(&MsgQ, (void*) pcInterruptMessage[ul_2 % 4], OSI_NO_WAIT);
	   ul_2++;
	   osi_Sleep(200);
     }
}

void
vTempTask(void *pvParameters)
{
    HTTPCli_Struct httpClient;
    float fCurrentTemp = 0.0;
    long  lRetVal = -1;

    while (1) {
        lRetVal = ConnectToAP();
        if (lRetVal < 0) {
            LOOP_FOREVER();
        }

        lRetVal = ConnectToHTTPServer(&httpClient);
        if (lRetVal < 0) {
            LOOP_FOREVER();
        }

        TMP006DrvGetTemp(&fCurrentTemp);

        size_t needed = snprintf(NULL, 0, POST_DATA, fCurrentTemp, LOCATION) + 1;
        char *buffer = malloc(needed);
        sprintf(buffer, POST_DATA, fCurrentTemp, LOCATION);

        UART_PRINT("HTTP Post Begin:\n\r");
        lRetVal = HTTPPostMethod(&httpClient, buffer, needed);
        if (lRetVal < 0) {
            UART_PRINT("HTTP Post failed.\n\r");
        }
        UART_PRINT("HTTP Post End:\n\r");

        free(buffer);
        sl_WlanDisconnect();

        osi_Sleep(900000); // sleep for 15mins (15min * 60.000ms = 900.000
    }
}

void
vUploadDataTask(void *pvParameters)
{

    while (1) {

    }
}

//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
static void
DisplayBanner(char * AppName)
{

    Report("\n\n\n\r");
    Report("\t\t *************************************************\n\r");
    Report("\t\t    CC3200 %s Application       \n\r", AppName);
    Report("\t\t *************************************************\n\r");
    Report("\n\n\n\r");
}

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs) || defined(gcc)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
  //
  // Enable Processor
  //
  MAP_IntMasterEnable();
  MAP_IntEnable(FAULT_SYSTICK);

  PRCMCC3200MCUInit();
}

//*****************************************************************************
//
//!  main function handling the freertos_demo.
//!
//! \param  None
//!
//! \return none
//
//*****************************************************************************
int main( void )
{
    long   lRetVal = -1;
    OsiReturnVal_e osi_retVal;

    //
    // Initialize the board
    //
    BoardInit();

    PinMuxConfig();

    //
    // Initializing the terminal
    //
    InitTerm();

    //
    // Clearing the terminal
    //
    ClearTerm();

    //
    // Diasplay Banner
    //
    DisplayBanner(APP_NAME);

    InitializeAppVariables();

    //
    // I2C Init
    //
    lRetVal = I2C_IF_Open(I2C_MASTER_MODE_FST);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    //Init Temprature Sensor
    lRetVal = TMP006DrvOpen();
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    //
    // Creating a queue for 10 elements.
    //

    //osi_retVal = osi_MsgQCreate(&MsgQ, "MSGQ", MAX_MSG_LENGTH, 1);
    //if(osi_retVal != OSI_OK)
    //{
    //	// Queue was not created and must not be used.
    //	while(1);
    //}

    //
    // Create the Queue Receive task
    //
    //osi_TaskCreate( vTestTask1, "TASK1",\
    //							OSI_STACK_SIZE, NULL, 1, NULL );

    //
    // Create the Queue Send task
    //
    //osi_TaskCreate( vTestTask2, "TASK2",\
    //							OSI_STACK_SIZE,NULL, 1, NULL );

    //
    // Create the get Temperature task
    //
    osi_retVal = osi_TaskCreate( vTempTask, "TEMPERATURETASK",\
                                OSI_STACK_SIZE, NULL, 1, NULL );
    if(osi_retVal != OSI_OK)
    {
      // Queue was not created and must not be used.
      while(1);
    }

    //
    // Create the upload Temperature task
    //
    //osi_TaskCreate( vUploadDataTask, "UPLOADTASK",\
    //                            OSI_STACK_SIZE, NULL, 1, NULL );

    //
    // Start the task scheduler
    //
    osi_start();

    return 0;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
