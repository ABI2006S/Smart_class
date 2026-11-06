#ifndef DHT_FAN_INO
#define DHT_FAN_INO
#include "dht_fan.h"
inline void dhtFanModuleInit() { dhtFanInit(); }
inline void dhtFanModuleUpdate() { dhtFanUpdate(); }
#endif 