#include "I2Cdev.h"
#include "MPU6050.h"
#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01

#define PIN_POT A0  // Номер пина Arduino, к которому подключен потенциометр
#define PIN_CE  10  // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 9 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля
#define TO_DEG 57.2957f
#define TIME_OUT 20

MPU6050 accgyro;
RF24 radio(PIN_CE, PIN_CSN); // Создаём объект radio с указанием выводов CE и CSN
int accuracy = 30; // Точность
float anglex;
float angley;
long int t1;

void setup() {
  Serial.begin(9600);
  accgyro.initialize(); // Инициализация гироскопа
  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(5); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openWritingPipe(0x7878787878LL); // Открываем трубу с уникальным ID
}

void loop() {

  long int t = millis();

  if(t1 < t) {
    int16_t ax, ay, az, gx, gy, gz;
    float accx, accy,gyrox;
    t1 = t + TIME_OUT;
    accgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    // преобразование в единицы гравитации при настройках 1G = 4096
    accx = ax/4096.0;
    accy = ay/4096.0;
    // границы от -1G до +1G
    accx = constrain(accx, -1.0, 1.0);
    accy = constrain(accy, -1.0, 1.0);
    // получить значение в градусах
    if(accy >= 0){
      anglex = 90 - TO_DEG*acos(accy);
    } else {
      anglex = TO_DEG*acos(-accy) - 90;
    }

    if(accx >= 0){
      angley = 90 - TO_DEG*acos(accx);
    } else {
      angley = TO_DEG*acos(-accx) - 90;
    }

    int x = angley;
    int y = anglex;
    int direct[2];
    
    if(x < accuracy && x > -accuracy) {
      if(y < accuracy && y > -accuracy) {
        direct[0] = 0;//STOP    
      } else if(y > accuracy) {
        direct[0] = 2;//Влево
      } else if(y < -accuracy) {
        direct[0] = 1; //Вправо
      }   
    } else if( x > accuracy) {
      //Вперед
      if(y < accuracy && y > -accuracy) {
        direct[0] = 6;//Назад    
      } else if(y > accuracy) {
        direct[0] = 8;//Вправо и Назад
      } else if(y < -accuracy) {
        direct[0] = 7; //Влево и Назад
      }   
    } else if(x < -accuracy) {
      //Назад
      if(y < accuracy && y > -accuracy) {
        direct[0] = 3;//Вперед    
      } else if(y > accuracy) {
        direct[0] = 5;//Вправо и Вперед
      } else if(y < -accuracy) {
        direct[0] = 4; //Влево и Вперед
      }   
    }

    int speed = max(abs(x), abs(y));
    if(speed > accuracy) {
      direct[1] = map(speed, accuracy, 90, 30, 100);
      Serial.println();
      Serial.print("Type:");
      Serial.print(direct[0]);
      Serial.print("X:");
      Serial.print(anglex,0);
      Serial.print(" ");
      Serial.print("Y:");
      Serial.print(angley,0);
      Serial.println(" ");
    } else {
      direct[1] = 0;
    }
    
    radio.write(direct, sizeof(direct)); // Отправляем комманду по радиоканалу
  }
}
