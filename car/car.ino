#include <SPI.h>  // Подключаем библиотеку для работы с SPI-интерфейсом
#include <SparkFun_TB6612.h>
#include <nRF24L01.h> // Подключаем файл конфигурации из библиотеки RF24
#include <RF24.h> // Подключаем библиотеку для работа для работы с модулем NRF24L01

#define PIN_LED 3  // Номер пина Arduino, к которому подключен светодиод
#define PIN_CE  10  // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 9 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля
#define PWMA 5
#define AIN1 3
#define AIN2 4
#define BIN1 8
#define BIN2 7
#define PWMB 6
#define STBY 14 //(A0)

RF24 radio(PIN_CE, PIN_CSN); // Создаём объект radio с указанием выводов CE и CSN
int potValue[2]; // Создаём массив для приёма значений

const int offsetA = 1;
const int offsetB = 1;

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);


void setup() {
  pinMode(PIN_LED, OUTPUT); // Настраиваем на выход пин светодиода
  radio.begin();  // Инициализация модуля NRF24L01
  radio.setChannel(5); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openReadingPipe (1, 0x7878787878LL); // Открываем трубу ID передатчика
  radio.startListening(); // Начинаем прослушивать открываемую трубу
}

 

void loop() {
  if(radio.available()){ // Если в буфер приёмника поступили данные
    radio.read(&potValue, sizeof(potValue)); // Читаем показания потенциометра
    int direct = potValue[0];
    int speedd = map(potValue[1], 0, 100, 0, 255);

    switch (direct){
        case 0://СТОП
          motor1.drive(speedd);
          motor2.drive(speedd);
          break;
        case 1://Вправо
          motor1.drive(-speedd);
          motor2.drive(speedd);
          break;
        case 2://Влево
          motor1.drive(speedd);
          motor2.drive(-speedd);
          break;
        case 3://Вперед
          motor1.drive(speedd);
          motor2.drive(speedd);
          break;
        case 4://Вправо и Вперед
          motor1.drive(speedd * 0.5);
          motor2.drive(speedd);
          break;
        case 5://Влево и Вперед
          motor1.drive(speedd);
          motor2.drive(speedd * 0.5);
          break;
        case 6://Назад
          motor1.drive(-speedd);
          motor2.drive(-speedd);
          break;
        case 7://Вправо и Назад
          motor1.drive(-speedd * 0.5);
          motor2.drive(-speedd);
          break;
        case 8://Влево и Назад
          motor1.drive(-speedd);
          motor2.drive(-speedd * 0.5);
          break;
      }

  }

}
