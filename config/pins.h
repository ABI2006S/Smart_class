#ifndef PINS_H
#define PINS_H
#define PIN_LDR_AO 1      
#define PIN_MQ2_AO 5      
#define PIN_MQ2_DO 6      
#define PIN_DHT11_DATA 4  
#define PIN_WINDOW_SERVO_1 10  
#define PIN_WINDOW_SERVO_2 11  
#define PIN_DOOR_SERVO 18      
#define PIN_OLED_SDA 39  
#define PIN_OLED_SCL 40  
#ifndef OLED_I2C_ADDRESS
#define OLED_I2C_ADDRESS 0x3C
#endif
#ifndef OLED_SCREEN_WIDTH
#define OLED_SCREEN_WIDTH 128
#endif
#ifndef OLED_SCREEN_HEIGHT
#define OLED_SCREEN_HEIGHT 64
#endif
#define PIN_PN532_SS 12    
#define PIN_PN532_MOSI 13  
#define PIN_PN532_SCK 14   
#define PIN_PN532_MISO 20  
#define PIN_LIGHT_RELAY 15  
#define PIN_FAN_RELAY 16    
#define PIN_SMOKE_RELAY 38  
#define PIN_BUZZER_RELAY 38
#define PIN_BUZZER                                                             \
  7  
#endif  
