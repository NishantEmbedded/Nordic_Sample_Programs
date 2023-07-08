#ifndef CS_SERVICE_H
#define CS_SERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "ble_srv_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define  BLE_UUID_CS_TX_CHAR    0x1A03   //The UUID of the Tx(Notify) Characteristic
#define  BLE_UUID_CS_INDI_CHAR  0x1A04   //The UUID of the  Indication Characteristic
#define  BLE_UUID_CS_MIC_CHAR   0X1A05   //The UUID of the Notify Characteristic 
#define  BLE_UUID_CS_WRITE_CHAR 0x1A06   //The UUID of the Write Characteristic 
#define  BLE_UUID_CS_READ_CHAR  0x1A07   //The UUID of the Read Characteristic  
#define  BLE_UUID_CS_SERVICE    0xCB1D   //The UUID of Custom Service

#define SH_CS_WRITE_CHAR_BUFFER_SIZE (255) /**< Buffer size of write characteristic of CS service. */
#define SH_CS_TX_CHAR_BUFFER_SIZE (255)    /**< Buffer size of tx characteristic of CS service. */
#define SH_CS_READ_CHAR_BUFFER_SIZE (255)  /**< Buffer size of read characteristic of CS service. */

#define CS_BASE_UUID                                                                                   \
    {                                                                                                  \
        0x2f, 0x99, 0xc4, 0xc1, 0x2c, 0x25, 0xca, 0x98, 0x35, 0x40, 0xe6, 0x7c, 0x07, 0x0c, 0xe4, 0x71 \
    }

/*CS SERVER EVENT TYPE*/
    typedef enum
    {
        CS_EVT_DATA_RX = 1,      // Event indicating that the RMS service received data from client.
        CS_EVT_NOTIFY_COMPLETED, // Event indicating that notified data to client successfully.
        CS_EVT_INDICATION_SENT,
    } CS_EVT;

/*@brief Structure containing the CS event data received from the peer.*/
    typedef struct
    {
        CS_EVT evtType;
        uint16_t connHandle;
        uint8_t *pData;
        uint8_t dataLength;
        ble_uuid_t uuid;
    } CS_EVT_TYPE;

    //Forward declaration of the CS_Service TYPE
    typedef struct cs_services CS_SERVICES;

    // CS Service event handler type
    typedef void (*CS_EVENT_HANDLER)(CS_SERVICES *pService, const CS_EVT_TYPE *PEvent);

    //CS SERVICE initialization structure.
    //This structure contains the initialization information for the service. The application must fill this structure and pass it to the service using the @ref RMS_InitService function.
    typedef struct 
    {
        CS_EVENT_HANDLER        eventHanlder; 
        ble_srv_error_handler_t errorHandler;
        ble_srv_report_ref_t    *pReportRef;
        ble_srv_utf8_str_t       writeData; 
        ble_srv_utf8_str_t       txData;
        ble_srv_utf8_str_t       readData;
        ble_srv_utf8_str_t       IndiData;
        ble_srv_utf8_str_t       micData;     
    }CS_SERVICES_INIT;

    // CS SERVICE STRUCTURE
    // This structure contains status information related to the service.
    struct cs_services
    {
        uint8_t uuidType;                           // UUID type for CS Service Base UUID.
        uint16_t serviceHandle;                     // Handle of CS Service (as provided by the SoftDevice).
        ble_gatts_char_handles_t writeHandles;      // Handles related to the write characteristic (as provided by the SoftDevice).
        ble_gatts_char_handles_t txHandles;         // Handles related to the TX characteristic (as provided by the SoftDevice).
        ble_gatts_char_handles_t readHandles;       // Handles related to the read characteristic (as provided by the SoftDevice).
        ble_gatts_char_handles_t IndiHandles;       // Handles related to the Indication characteristic (as provided by the SoftDevice).
        ble_gatts_char_handles_t microphoneHandles; // Handles related to the Microphone notification characteristic (as provided by the SoftDevice).
        uint16_t connHandle;                        // Handle of the current connection (as provided by the SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection.
        bool isNotificationEnabled;                 // Variable to indicate if the peer has enabled notification of the RX characteristic
        bool isIndicationEnabled;                   // Variable to indicate if the peer has enabled indication of the RX characteristic
        bool isMicNotificaionEnabled;
        CS_EVENT_HANDLER eventHandler; // Event handler to be called for handling received data.
    };
uint32_t CS_InitService(CS_SERVICES *pService,const CS_SERVICES_INIT *pServiceInit);
void CS_CallOnBleEvent(CS_SERVICES *pService, ble_evt_t *pBleEvent);
void ble_cs_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context);
uint32_t CS_SendDataToPeer(CS_SERVICES *pService, uint8_t *pData, uint16_t length);
uint32_t CS_NotifyOn_SendData(uint8_t *pData, uint16_t length);
void Custom_Service_Init(void);


#ifdef __cplusplus
}
#endif

#endif