#ifndef __GSM_H
#define __GSM_H

#include "stm32g0xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

#define GSM_RX_BUF_SIZE 1024
#define GSM_TX_BUF_SIZE 512

typedef enum {
  GSM_OK = 0,
  GSM_ERROR,
  GSM_TIMEOUT
} GSM_Status;

void GSM_Init(UART_HandleTypeDef *huart);
GSM_Status GSM_SendATCommand(const char *cmd, const char *expected, uint32_t timeout_ms);
GSM_Status GSM_WaitReady(uint32_t timeout_ms);
GSM_Status GSM_CheckNetwork(void);
GSM_Status GSM_InitGPRS(const char *apn);
GSM_Status GSM_HTTPGet(const char *url, char *response, uint16_t max_len);
GSM_Status GSM_HTTPPut(const char *url, const char *json_body);
const char *GSM_GetLastResponse(void);

#endif /* __GSM_H */
