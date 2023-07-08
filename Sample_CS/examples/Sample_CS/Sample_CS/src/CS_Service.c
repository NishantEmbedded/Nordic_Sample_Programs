#include <string.h>
#include <boards.h>
#include "CS_Service.h"
#include "nrf_sdh_ble.h"
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_log.h"

static uint8_t  _maxLengthTxChar        =0;   // Maximum data length of notify Characteristic.
static uint8_t  _maxLengthIndiChar      =0;   // Maximum data length of Indication Characteristic.
static uint8_t  _maxLengthMicNotifyChar =0;   // Maximum data length of notify Characteristic.


// Data buffer for write characteristic, size 1+ for null character.
static uint8_t _writeCharBuffer[SH_CS_WRITE_CHAR_BUFFER_SIZE + 1];

// Data buffer for tx characteristic, size 1+ for null character.
static uint8_t _TxCharBuffer[SH_CS_TX_CHAR_BUFFER_SIZE + 1];

// Data buffer for Long Read characteristic, size 1+ for null character.
static uint8_t _readCharBuffer[SH_CS_READ_CHAR_BUFFER_SIZE + 1];

// RM service instance.
static CS_SERVICES _CSService;

// Information needed to initialize the service.
static CS_SERVICES_INIT _CSServiceInit;


/**@brief   	This function will handle the data received from write and/or Tx characteristic of
 *				the CS BLE Service.
 *
 *
 *
 * @param[in]	pCfsService	CS Service structure.
 * @param[in] 	pData 		Data to be process.
 * @param[in] 	length   	Length of the data.
 *
 * @retval 		none
 */
static void CS_HandleServiceEvent(CS_SERVICES *pService, const CS_EVT_TYPE *pEvent)
{
 switch(pEvent->evtType)
 {
  default:
  {
   NRF_LOG_INFO("Error in CS_HandleServiceEvent");
   break; 
  }
  case CS_EVT_DATA_RX:
  {
   if(pEvent->dataLength > SH_CS_WRITE_CHAR_BUFFER_SIZE)
   {
     return;
   }
   //Here Write a data rx function
   break;
  }
  case CS_EVT_NOTIFY_COMPLETED:
  {
    break;
  } 
 }
}

/**@brief   	Function for handling the @ref BLE_GAP_EVT_CONNECTED event from the S132
 *				SoftDevice.
 *
 * @details 	Function for handling the @ref BLE_GAP_EVT_CONNECTED event from the S132
 *				SoftDevice.
 *
 * @param[in]	pService	CS Service structure.
 * @param[in] 	pBleEvent 	Pointer to the event received from BLE stack.
 *
 * @retval 		None
 */
static void CS_CallOnConnect(CS_SERVICES *pService, ble_evt_t *pBleEvent)
{
  pService->connHandle  = pBleEvent->evt.gap_evt.conn_handle;
}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   pService       Custom Service structure.
 * @param[in]   pBleEvent   Event received from the BLE stack.
 */

 static void CS_CallOnDisconnect(CS_SERVICES  *pService, ble_evt_t *pBleEvent)
 {
    UNUSED_PARAMETER(pBleEvent);
    pService->connHandle  = BLE_CONN_HANDLE_INVALID;
 }

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_cus       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */

static void CS_CallOnWrite(CS_SERVICES *pService, ble_evt_t *p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    // Check if the handle passed with the event matches the Custom Value Characteristic handle.
    if((p_evt_write->handle  ==  pService->txHandles.cccd_handle) && (p_evt_write->len ==  2))
    {
      if(ble_srv_is_notification_enabled(p_evt_write->data))
      {
        NRF_LOG_INFO("Enabled");
        pService->isMicNotificaionEnabled = true;
      }
      else
      {
        NRF_LOG_INFO("Disabled");
        pService->isMicNotificaionEnabled = false;      
      }
    }

    else if ((p_evt_write->handle == pService->IndiHandles.cccd_handle) && (p_evt_write->len == 2))
    {
        if (ble_srv_is_indication_enabled(p_evt_write->data))
        {
        }
        else
        {
            NRF_LOG_INFO("Indication: Disabled");
        }
    }

    else if((p_evt_write->handle  ==  pService->writeHandles.value_handle) && (pService->eventHandler != NULL))
    {
        CS_EVT_TYPE csEvent;
        csEvent.evtType=CS_EVT_DATA_RX;
        csEvent.pData=p_evt_write->data;
        csEvent.dataLength=p_evt_write->len;
        csEvent.uuid=p_evt_write->uuid;
        
        pService->eventHandler(pService,&csEvent);      
    }
}

/**@brief   	Function for handling the @ref BLE_BLE_EVT_TX_COMPLETE event from the S132
 *				SoftDevice.
 *
 * @details 	Function for handling the @ref BLE_BLE_EVT_TX_COMPLETE event from the S132
 *				SoftDevice.
 *
 * @param[in]	pService	CS Service structure.
 * @param[in] 	pBleEvent 	Pointer to the event received from BLE stack.
 *
 * @retval 		None
 */
static void CS_CallOnNotificationComplete(CS_SERVICES * pService, ble_evt_t *pBleEvent)
{
  if(pService->eventHandler !=NULL)
  {
    CS_EVT_TYPE csEvent;
    csEvent.evtType = CS_EVT_NOTIFY_COMPLETED;
    pService->eventHandler(pService,&csEvent);
  }
}


/**@brief   	Function for handling the @ref BLE_GATTS_EVT_HVC event from the S132
 *				SoftDevice.
 *
 * @details 	Function for handling the @ref BLE_GATTS_EVT_HVC event from the S132
 *				SoftDevice.
 *
 * @param[in]	pService	CS Service structure.
 * @param[in] 	pBleEvent 	Pointer to the event received from BLE stack.
 *
 * @retval 		None
 */
 static void CS_CallOnIndicationComplete(CS_SERVICES *pService, ble_evt_t *pBleEvent)
 {
  if(pService->eventHandler !=NULL)
  {
    CS_EVT_TYPE csEvent;
    csEvent.evtType = CS_EVT_INDICATION_SENT;
    pService->eventHandler(pService,&csEvent);
  }
 }

/**@brief   	Function for adding write characteristic.
*
* @param[in]	pService	CS Service structure.
* @param[in] 	pServiceInit 	Information needed to initialize the service
*
* @retval 		NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t  CS_AddWriteChar(CS_SERVICES *pService,const CS_SERVICES_INIT *pServiceInit)
{
 ble_gatts_char_md_t charMd;
 ble_gatts_attr_md_t cccdMd;
 ble_gatts_attr_t    attrCharValue;
 ble_uuid_t          bleuuid;
 ble_gatts_attr_md_t attrMd;
 
 memset(&cccdMd,0,sizeof(cccdMd));

 BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccdMd.read_perm); 
 BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccdMd.write_perm);
 
 cccdMd.vloc = BLE_GATTS_VLOC_STACK;
 
 memset(&charMd,0,sizeof(charMd));
 
  charMd.char_props.write=1;
  charMd.p_char_user_desc=NULL;
  charMd.p_char_pf  =NULL;
  charMd.p_user_desc_md=NULL;
  charMd.p_cccd_md=&cccdMd;
  charMd.p_sccd_md=NULL;

  bleuuid.type  = pService->uuidType;
  bleuuid.uuid  = BLE_UUID_CS_WRITE_CHAR;

  memset(&attrMd, 0, sizeof(attrMd));

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attrMd.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attrMd.write_perm);

  attrMd.vloc = BLE_GATTS_VLOC_USER;
  attrMd.rd_auth = 0;
  attrMd.wr_auth = 0;
  attrMd.vlen = 1;

  memset(&attrCharValue, 0, sizeof(attrCharValue));

  attrCharValue.p_uuid = &bleuuid;
  attrCharValue.p_attr_md = &attrMd;
  attrCharValue.init_len = pServiceInit->writeData.length;
  attrCharValue.init_offs = 0;
  attrCharValue.max_len = pServiceInit->writeData.length;
  attrCharValue.p_value = pServiceInit->writeData.p_str;
  
  _maxLengthTxChar = pServiceInit->writeData.length;
  return sd_ble_gatts_characteristic_add(pService->serviceHandle,&charMd,&attrCharValue,&pService->writeHandles); 
}


/**@brief   	Function for adding Tx characteristic.
 *
 *
 * @param[in]	pService        CS Service structure.
 * @param[in] 	pServiceInit 	Information needed to initialize the service
 *
 * @retval 		NRF_SUCCESS on success, otherwise an error code.
*/
static uint32_t CS_AddTxChar(CS_SERVICES * pService, const CS_SERVICES_INIT *pServiceInit)
{
    ble_gatts_char_md_t charMd;
    ble_gatts_attr_md_t cccdMd;
    ble_gatts_attr_t    attrCharValue;
    ble_uuid_t          bleuuid;
    ble_gatts_attr_md_t attrMd;

    memset(&cccdMd,0,sizeof(cccdMd));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccdMd.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccdMd.write_perm);

    cccdMd.vloc =  BLE_GATTS_VLOC_STACK;
    
    memset(&charMd,0,sizeof(charMd));

    charMd.char_props.notify=1;
    charMd.p_char_user_desc=NULL;
    charMd.p_char_pf  =NULL;
    charMd.p_user_desc_md=NULL;
    charMd.p_cccd_md=&cccdMd;
    charMd.p_sccd_md=NULL;

    bleuuid.type  = pService->uuidType;
    bleuuid.uuid  = BLE_UUID_CS_TX_CHAR;

    memset(&attrMd,0,sizeof(attrMd));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attrMd.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attrMd.write_perm);
    attrMd.vloc = BLE_GATTS_VLOC_USER;
    attrMd.rd_auth = 0;
    attrMd.wr_auth = 0;
    attrMd.vlen = 1;

    memset(&attrCharValue, 0, sizeof(attrCharValue));

    attrCharValue.p_uuid = &bleuuid;
    attrCharValue.p_attr_md = &attrMd;
    attrCharValue.init_len = pServiceInit->txData.length;
    attrCharValue.init_offs = 0;
    attrCharValue.max_len = pServiceInit->txData.length;
    attrCharValue.p_value = pServiceInit->txData.p_str;

    _maxLengthTxChar = pServiceInit->txData.length;
    return sd_ble_gatts_characteristic_add(pService->serviceHandle,&charMd,&attrCharValue,&pService->txHandles);
}


/**@brief   	Function for initializing the CS Service.
 *
**/
uint32_t CS_InitService(CS_SERVICES *pService,const CS_SERVICES_INIT *pServiceInit)
{
  uint32_t      err_code;
  ble_uuid_t    bleuuid;
  ble_uuid128_t base_uuid = CS_BASE_UUID;

  VERIFY_PARAM_NOT_NULL(pService);
  VERIFY_PARAM_NOT_NULL(pServiceInit);

  //// Initialize the service structure.
  pService->connHandle=BLE_CONN_HANDLE_INVALID;
  pService->eventHandler=pServiceInit->eventHanlder;
  pService->isMicNotificaionEnabled=false;
  pService->isIndicationEnabled=false;
  pService->isMicNotificaionEnabled=false;

  //Add a custom base uuid 
  err_code=sd_ble_uuid_vs_add(&base_uuid,&pService->uuidType);

  // Add Custom Service UUID
  bleuuid.type=pService->uuidType;
  bleuuid.uuid=BLE_UUID_CS_SERVICE;

  // Add the Custom Service
  err_code=sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,&bleuuid,&pService->serviceHandle);
  
  VERIFY_SUCCESS(err_code);

  //Add the write Characteristic
  err_code=CS_AddWriteChar(pService,pServiceInit);
  VERIFY_SUCCESS(err_code);

  ////Add the read Characteristic
  //err_code=CS_AddReadChar(pService,pServiceInit);
  //VERIFY_SUCCESS(err_code);

  //Add the Tx Characteristic
  err_code=CS_AddTxChar(pService,pServiceInit);
  VERIFY_SUCCESS(err_code);

  return NRF_SUCCESS;
}

void Custom_Service_Init(void)
{
  uint32_t err_code;

  memset(&_CSServiceInit, 0, sizeof(_CSServiceInit));
  
  memset(_writeCharBuffer, 0x01, sizeof(_writeCharBuffer));
  memset(_TxCharBuffer, 0x01, sizeof(_TxCharBuffer));
  memset(_readCharBuffer, 0x01, sizeof(_readCharBuffer));

/* put null on last byte of buffer */
  _writeCharBuffer[SH_CS_WRITE_CHAR_BUFFER_SIZE] = 0;
  _TxCharBuffer[SH_CS_TX_CHAR_BUFFER_SIZE] = 0;
  _readCharBuffer[SH_CS_READ_CHAR_BUFFER_SIZE] = 0;

   ble_srv_ascii_to_utf8(&_CSServiceInit.writeData, (char *)_writeCharBuffer);
   ble_srv_ascii_to_utf8(&_CSServiceInit.txData, (char *)_TxCharBuffer);
   ble_srv_ascii_to_utf8(&_CSServiceInit.readData, (char *)_readCharBuffer);

   _CSServiceInit.eventHanlder  = CS_HandleServiceEvent;//call funcation which is read the data...
   _CSServiceInit.pReportRef=NULL;
   _CSServiceInit.errorHandler=NULL;
   
   err_code=CS_InitService(&_CSService,&_CSServiceInit);
   APP_ERROR_CHECK(err_code);   
}


void CS_CallOnBleEvent(CS_SERVICES *pService, ble_evt_t *pBleEvent)
 {
    if((pService == NULL) || (pBleEvent == NULL))
    {
        return;
    }

    switch (pBleEvent->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        CS_CallOnConnect(pService,pBleEvent);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
         CS_CallOnDisconnect(pService,pBleEvent);
        break;

    case BLE_GATTS_EVT_WRITE:
         CS_CallOnWrite(pService,pBleEvent);
        break;

    case BLE_GATTS_EVT_HVC:
         CS_CallOnIndicationComplete(pService,pBleEvent);
        break;

    case BLE_GATTS_EVT_TIMEOUT:
        // Not sure what is use for now.
        break;

    case BLE_GATTS_EVT_HVN_TX_COMPLETE:
         CS_CallOnNotificationComplete(pService,pBleEvent); 
        break;

    default:
        // No implementation needed.
        break;
    }
}

void ble_cs_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context)
{ 
    CS_CallOnBleEvent(&_CSService,  (ble_evt_t *)p_ble_evt);
}

NRF_SDH_BLE_OBSERVER(m_ble_evt_observer,PM_BLE_OBSERVER_PRIO,ble_cs_on_ble_evt,NULL);

uint32_t CS_SendDataToPeer(CS_SERVICES *pService, uint8_t *pData, uint16_t length)
{
  ble_gatts_hvx_params_t hvxParams;
  
  if((pService->connHandle  ==  BLE_CONN_HANDLE_INVALID) || (!pService->isMicNotificaionEnabled))
  {
    return NRF_ERROR_INVALID_STATE;
  }
  
  if(length > _maxLengthTxChar)
  {
    return NRF_ERROR_INVALID_PARAM;
  }
  
  memset(&hvxParams,0,sizeof(hvxParams));
  
  hvxParams.handle  = pService->txHandles.value_handle;
  hvxParams.p_data  = pData;
  hvxParams.p_len = &length;
  hvxParams.type  = BLE_GATT_HVX_NOTIFICATION;
  return sd_ble_gatts_hvx(pService->connHandle, &hvxParams);   
} 

uint32_t CS_NotifyOn_SendData(uint8_t *pData, uint16_t length)
{
  uint32_t errCode=0;

  errCode=CS_SendDataToPeer(&_CSService,pData,length);
  if(errCode==NRF_SUCCESS)
  {
    NRF_LOG_RAW_HEXDUMP_INFO(pData,length);
  }
  return errCode;
}