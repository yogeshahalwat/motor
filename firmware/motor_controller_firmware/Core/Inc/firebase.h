#ifndef __FIREBASE_H
#define __FIREBASE_H

#include <stdbool.h>
#include <stdint.h>

#define FIREBASE_DB_URL "https://motor-controller-be320-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_DEVICE_PATH "devices/test_device_001"

typedef struct {
  char command[8];
  bool motor_running;
  bool fault;
  bool all_ok;
} DeviceState;

bool Firebase_Init(const char *apn);
bool Firebase_ReadCommand(char *command_out, uint8_t max_len);
bool Firebase_UpdateStatus(bool motor_running, bool fault, bool all_ok);

#endif /* __FIREBASE_H */
