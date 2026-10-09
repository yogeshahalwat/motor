#include "firebase.h"
#include "gsm.h"
#include <string.h>
#include <stdio.h>

bool Firebase_Init(const char *apn)
{
  if (GSM_WaitReady(30000) != GSM_OK)
    return false;

  GSM_SendATCommand("ATE0", "OK", 2000);

  uint32_t start = HAL_GetTick();
  while ((HAL_GetTick() - start) < 60000)
  {
    if (GSM_CheckNetwork() == GSM_OK)
      break;
    HAL_Delay(3000);
  }
  if (GSM_CheckNetwork() != GSM_OK)
    return false;

  if (GSM_InitGPRS(apn) != GSM_OK)
    return false;

  return true;
}

bool Firebase_ReadCommand(char *command_out, uint8_t max_len)
{
  char response[256];
  char url[256];

  snprintf(url, sizeof(url), "%s/%s/command.json", FIREBASE_DB_URL, FIREBASE_DEVICE_PATH);

  if (GSM_HTTPGet(url, response, sizeof(response)) != GSM_OK)
    return false;

  /* Firebase returns the value in JSON: "ON" or "OFF" (with quotes) */
  char *start = strchr(response, '"');
  if (start != NULL)
  {
    start++;
    char *end = strchr(start, '"');
    if (end != NULL)
    {
      uint8_t len = end - start;
      if (len >= max_len) len = max_len - 1;
      strncpy(command_out, start, len);
      command_out[len] = '\0';
      return true;
    }
  }

  return false;
}

bool Firebase_UpdateStatus(bool motor_running, bool fault, bool all_ok)
{
  char url[256];
  char json[256];

  snprintf(url, sizeof(url), "%s/%s.json", FIREBASE_DB_URL, FIREBASE_DEVICE_PATH);

  /* Use Firebase server timestamp so the app can calculate "device online" correctly
     — the STM32 has no real-time clock, so we let Firebase fill in the actual time */
  snprintf(json, sizeof(json),
    "{\"motor_running\":%s,\"fault\":%s,\"all_ok\":%s,\"last_seen\":{\".sv\":\"timestamp\"}}",
    motor_running ? "true" : "false",
    fault ? "true" : "false",
    all_ok ? "true" : "false");

  return GSM_HTTPPut(url, json) == GSM_OK;
}
