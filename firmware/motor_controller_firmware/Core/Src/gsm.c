#include "gsm.h"
#include <string.h>
#include <stdio.h>

static UART_HandleTypeDef *gsm_uart;
static char rx_buf[GSM_RX_BUF_SIZE];
static uint16_t rx_index;

void GSM_Init(UART_HandleTypeDef *huart)
{
  gsm_uart = huart;
  rx_index = 0;
  memset(rx_buf, 0, sizeof(rx_buf));
}

static void GSM_ClearRxBuf(void)
{
  rx_index = 0;
  memset(rx_buf, 0, sizeof(rx_buf));
}

static GSM_Status GSM_ReadUntil(const char *expected, uint32_t timeout_ms)
{
  uint32_t start = HAL_GetTick();
  uint8_t byte;

  while ((HAL_GetTick() - start) < timeout_ms)
  {
    if (HAL_UART_Receive(gsm_uart, &byte, 1, 10) == HAL_OK)
    {
      if (rx_index < GSM_RX_BUF_SIZE - 1)
      {
        rx_buf[rx_index++] = (char)byte;
        rx_buf[rx_index] = '\0';
      }
      if (strstr(rx_buf, expected) != NULL)
        return GSM_OK;
      if (strstr(rx_buf, "ERROR") != NULL)
        return GSM_ERROR;
    }
  }
  return GSM_TIMEOUT;
}

GSM_Status GSM_SendATCommand(const char *cmd, const char *expected, uint32_t timeout_ms)
{
  GSM_ClearRxBuf();
  HAL_UART_Transmit(gsm_uart, (uint8_t *)cmd, strlen(cmd), 1000);
  HAL_UART_Transmit(gsm_uart, (uint8_t *)"\r\n", 2, 100);
  return GSM_ReadUntil(expected, timeout_ms);
}

GSM_Status GSM_WaitReady(uint32_t timeout_ms)
{
  uint32_t start = HAL_GetTick();

  while ((HAL_GetTick() - start) < timeout_ms)
  {
    if (GSM_SendATCommand("AT", "OK", 1000) == GSM_OK)
      return GSM_OK;
    HAL_Delay(500);
  }
  return GSM_TIMEOUT;
}

GSM_Status GSM_CheckNetwork(void)
{
  /* +CREG: 0,1 = registered home network, +CREG: 0,5 = registered roaming */
  if (GSM_SendATCommand("AT+CREG?", "+CREG: 0,1", 3000) == GSM_OK)
    return GSM_OK;
  if (GSM_SendATCommand("AT+CREG?", "+CREG: 0,5", 3000) == GSM_OK)
    return GSM_OK;
  return GSM_ERROR;
}

GSM_Status GSM_InitGPRS(const char *apn)
{
  char cmd[128];

  /* Close any existing PDP context */
  GSM_SendATCommand("AT+CGACT=0,1", "OK", 5000);
  HAL_Delay(500);

  /* Set APN */
  snprintf(cmd, sizeof(cmd), "AT+CGDCONT=1,\"IP\",\"%s\"", apn);
  if (GSM_SendATCommand(cmd, "OK", 5000) != GSM_OK)
    return GSM_ERROR;

  /* Activate PDP context */
  if (GSM_SendATCommand("AT+CGACT=1,1", "OK", 15000) != GSM_OK)
    return GSM_ERROR;

  return GSM_OK;
}

GSM_Status GSM_HTTPGet(const char *url, char *response, uint16_t max_len)
{
  char cmd[384];

  /* Terminate any previous HTTP session */
  GSM_SendATCommand("AT+HTTPTERM", "OK", 2000);
  HAL_Delay(300);

  if (GSM_SendATCommand("AT+HTTPINIT", "OK", 5000) != GSM_OK)
    return GSM_ERROR;

  /* Enable SSL for HTTPS */
  if (GSM_SendATCommand("AT+HTTPPARA=\"SSLCFG\",1", "OK", 3000) != GSM_OK)
  {
    /* Some firmware versions use a different command */
    GSM_SendATCommand("AT+HTTPSSL=1", "OK", 3000);
  }

  /* Content type for Firebase REST API */
  GSM_SendATCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"", "OK", 3000);

  /* Set URL */
  snprintf(cmd, sizeof(cmd), "AT+HTTPPARA=\"URL\",\"%s\"", url);
  if (GSM_SendATCommand(cmd, "OK", 5000) != GSM_OK)
  {
    GSM_SendATCommand("AT+HTTPTERM", "OK", 2000);
    return GSM_ERROR;
  }

  /* Execute GET */
  if (GSM_SendATCommand("AT+HTTPACTION=0", "+HTTPACTION: 0,200", 30000) != GSM_OK)
  {
    GSM_SendATCommand("AT+HTTPTERM", "OK", 2000);
    return GSM_ERROR;
  }

  /* Read response data */
  GSM_ClearRxBuf();
  GSM_SendATCommand("AT+HTTPREAD=0,512", "+HTTPREAD:", 10000);

  /* Copy response, skipping the +HTTPREAD: header line */
  if (response != NULL)
  {
    const char *data_start = strstr(rx_buf, "\n");
    if (data_start != NULL)
    {
      data_start++;
      uint16_t copy_len = strlen(data_start);
      if (copy_len > max_len - 1)
        copy_len = max_len - 1;
      strncpy(response, data_start, copy_len);
      response[copy_len] = '\0';
    }
    else
    {
      response[0] = '\0';
    }
  }

  GSM_SendATCommand("AT+HTTPTERM", "OK", 2000);
  return GSM_OK;
}

GSM_Status GSM_HTTPPut(const char *url, const char *json_body)
{
  char cmd[384];
  uint16_t body_len = strlen(json_body);

  GSM_SendATCommand("AT+HTTPTERM", "OK", 2000);
  HAL_Delay(300);

  if (GSM_SendATCommand("AT+HTTPINIT", "OK", 5000) != GSM_OK)
    return GSM_ERROR;

  GSM_SendATCommand("AT+HTTPPARA=\"SSLCFG\",1", "OK", 3000);
  GSM_SendATCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"", "OK", 3000);

  /* Firebase REST API uses PUT via PATCH method */
  GSM_SendATCommand("AT+HTTPPARA=\"CUSTOMREQUEST\",\"PATCH\"", "OK", 3000);

  snprintf(cmd, sizeof(cmd), "AT+HTTPPARA=\"URL\",\"%s\"", url);
  if (GSM_SendATCommand(cmd, "OK", 5000) != GSM_OK)
  {
    GSM_SendATCommand("AT+HTTPTERM", "OK", 2000);
    return GSM_ERROR;
  }

  /* Tell module how many bytes of body data we'll send */
  snprintf(cmd, sizeof(cmd), "AT+HTTPDATA=%u,10000", body_len);
  if (GSM_SendATCommand(cmd, "DOWNLOAD", 5000) != GSM_OK)
  {
    GSM_SendATCommand("AT+HTTPTERM", "OK", 2000);
    return GSM_ERROR;
  }

  /* Send the JSON body */
  HAL_UART_Transmit(gsm_uart, (uint8_t *)json_body, body_len, 5000);
  if (GSM_ReadUntil("OK", 5000) != GSM_OK)
  {
    GSM_SendATCommand("AT+HTTPTERM", "OK", 2000);
    return GSM_ERROR;
  }

  /* Execute POST (action=1), which with CUSTOMREQUEST becomes PATCH */
  if (GSM_SendATCommand("AT+HTTPACTION=1", "+HTTPACTION: 1,200", 30000) != GSM_OK)
  {
    GSM_SendATCommand("AT+HTTPTERM", "OK", 2000);
    return GSM_ERROR;
  }

  GSM_SendATCommand("AT+HTTPTERM", "OK", 2000);
  return GSM_OK;
}

const char *GSM_GetLastResponse(void)
{
  return rx_buf;
}
