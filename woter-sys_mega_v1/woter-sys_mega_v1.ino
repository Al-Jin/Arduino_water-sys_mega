#include <Ultrasonic.h>                 // Подключаем библиотеку для работы с дальномером переделана в миллиметрах
#include <iarduino_DHT.h>               // Подключаем библиотеку для работы с датчиком DHT
#include <UTFT.h>                       //Подключаем библиотеку для работы с tft дисплеем
//Цифровой D0 - пустой
//Цифровой D1 - пустой
Ultrasonic ultrasonic(2, 3);      // Trig - 2, Echo - 3 дальномер
#define SEN_IN 4                  // Подключение датчика перелива к цифровому выходу D4
iarduino_DHT AM2320(5);           // Объявляем объект датчик температуры и влажности, указывая номер вывода, к которому подключён модуль
#define LED_p_in1 6               // Подключение светодиода утечки воды к цифровому выходу D6
#define buttonPin1 7              // Кнопка отключения светодиода перелива воды к цифровому выходу D7
#define LED_p_in2 8               // Светодиод когда давление воды на входе 0 подключен к цифровому выводу D8
#define buttonPin2 9              // Кнопка отключения светодиода отсутствия давления воды на входе подключена к цифровому выводу D9
#define LED_p_in3 10              // Светодиод перекрытия воды на улицу подключен к цифровому выводу D10
#define buttonPin3 11             // Кнопка светодиода закрытия воды на улицу подключена к аналоговому выводу D11
//#define RELEY_UL 12             // Зарезервирован для реле закрытия подачи воды на улицу к цифровому выходу D12
//#define RELEY_IN 13             // Зарезервирован для реле закрытия подачи воды в бак к цифровому выходу D13

UTFT myGLCD(MAR3951,38,39,40,41);   //Инициализируем подключение tft дисплея

#define DP 54                      // Датчик давления на входе подключен к аналоговому выводу A0

  extern uint8_t SmallFont[];              // Добавляем маленький шрифт
  extern uint8_t BigFont[];                // Добавляем большой шрифт
  extern uint8_t SevenSegNumFont[];        // Добавляем шрифт имитирующий семисегментный индикатор
  extern uint8_t Arial_round_16x24[];      // Новый шрифт
  extern uint8_t SixteenSegment16x24[];    // Семисигиентный шрифт для вывода времени 
  extern uint8_t SixteenSegment24x36[];    // Семисигиентный шрифт для вывода времени  

int K1 = 0;              // Коэффициент горения светодиода отсутствия давления воды на входе  
int K2 = 0;              // Коэффициент включения времени мигания светодиода
int K3 = 0;              // Коэффициент горения светодиода перелива
int K4 = 0;              // Коэффициент отсчета времени включения светодиода давления воды

int buttonState1 = 0;    // Переменная для хранения состояния кнопки отключения светодиода перелива
int buttonState2 = 0;    // Переменная для хранения состояния кнопки отключения светодиода отсутствия давления воды на входе

unsigned long time0 = 0;   // Переменная для хранения точки отсчета в мигании светодиодом отсутствия воды
unsigned long time1 = 0;   // Переменная для хранения точки отсчета в для гашения светодиода при кратковременном отсутствии воды
//unsigned long time2 = 0;   // Переменная для хранения точки отсчета времени обновления экрана
unsigned long time3 = 0;   // Переменная для хранения точки отсчета считывания температуры и влажности
unsigned long time4 = 0;   // Переменная для хранения точки отсчета в мигании светодиода протечки
unsigned long time5 = 0;   // Переменная для хранения точки отсчета времени подачи воды
unsigned long time6 = 0;   // Переменная для хранения точки отсчета времени в вычислении медиан
float Hk = 0;              //Переменная влажности с учетом понижающего коэффициента
int m = 0;                 // Переменная для хранения минут времени с начала подачи воды
int h = 0;                 // Переменная для хранения часов времени с начала подачи воды
float p_in = 1;            // Переменная для хранения давления воды на входе
float distance[20]; // Массив для вычисления медианы данных с дальномера
float pr[20];       // Массив для вычисления медианы данных с датчика давления

void setup()
{
  Serial.begin(9600);                      // Инициируем передачу данных в монитор последовательного порта, на скорости 9600 бод
  myGLCD.InitLCD();                     // Объявляем дисплей
  myGLCD.clrScr();                      // Очищает экран и заливает черным цветом
   for (byte i = 0; i < 20; i++) {              // Для вычисления медианы данных с дальномера
     distance[i] = ultrasonic.Ranging(MM);
   }
   
   for (byte j = 0; j < 20; j++) {              // Для вычисления медианы данных с датчика давления
     pr[j] = analogRead(DP);
   } 
   
//  pinMode(RELEY_UL, OUTPUT);    // Реле закрытия воды на улицу
  pinMode(SEN_IN, INPUT);         // Датчик перелива
  pinMode(LED_p_in1, OUTPUT);     // Светодиод перелива
//  pinMode(RELEY_IN, OUTPUT);    // Реле закрытия подачи воды в бак
  pinMode(LED_p_in2, OUTPUT);    // Светодиод когда давление воды на входе 0
  pinMode(buttonPin1, INPUT);    // Инициализируем пин, подключенный к кнопке отключения светодиода перелива, как вход
  pinMode(buttonPin2, INPUT);    // Инициализируем пин, подключенный к кнопке отключения светодиода отсутствия давления воды на входе, как вход
}

void loop()
{
buttonState1 = digitalRead(buttonPin1);                // Считваем значения с кнопки отключения светодиода перелива  
buttonState2 = digitalRead(buttonPin2);                // Считваем значения с кнопки отключения светодиода отсутствия давления воды на входе

if(buttonState1 == HIGH){                          // Условие отключения светодиода перелива при нажатии кнопки
    K3 = 0;                                          // Обнуление коэффициента, чтобы светодиод потух
    digitalWrite(LED_p_in1, LOW);
  }

if(buttonState2 == HIGH){                          // Условие отключения светодиода давления воды ноль на входе при нажатии кнопки
    K1 = 0;                                          // Обнуление коэффициента, чтобы светодиод потух
    K2 = 0;
    K4 = 0;
    digitalWrite(LED_p_in2, LOW);
  }

if(millis() - time3 > 3000){       // Условие опроса датчика температуры и влажности раз в 3 секунды
    switch(AM2320.read());           //Считываем температуру и влажность
    time3 = millis();
  }
  
//Serial.println(AM2320.hum);

Hk = AM2320.hum * 0.73;    // Поправочный коэффициент влажности

if(digitalRead(SEN_IN) == LOW){    // Условие включения светодиода и реле закрытия подачи воды в бак при протечке
    digitalWrite(LED_p_in1, HIGH);
    K3 = 1;
//    digitalWrite(RELEY_IN, HIGH);
  } else {
    digitalWrite(LED_p_in1, LOW);
//    digitalWrite(RELEY_IN, LOW);
  }
  
if(SEN_IN != LOW && K3 > 0 && millis() - time4 > 500){       // Условие мигания светодиода протечки при понижении уровня воды до нормального
    digitalWrite(LED_p_in1, !digitalRead(LED_p_in1));        // включаем / выключаем LED
    time4 = millis();
  }  

if(p_in == 0 && K4 == 0){       // Изменение значения переменной для отсчета включения светодиода когда давление воды на входе равно 0
    time1 = millis();
    K4 = 1;
  } 

if(p_in > 0){       // Изменение значения переменной для отсчета включения светодиода когда давление воды на входе равно 0
    K4 = 0;
  } 

if(p_in == 0 && millis() - time1 > 300000 && K1 == 0){       // Условие включения светодиода когда давление воды на входе равно 0 в течении 5 минут
    digitalWrite(LED_p_in2, HIGH);
    K1 = 1;                                 // Коэффициент, для мигания светодиода после подачи воды
  }

if(p_in > 0 && K1 > 0 && millis() - time0 > 500){          // Условие мигания светодиода после подачи воды давление воды на входе равно 0
    digitalWrite(LED_p_in2, !digitalRead(LED_p_in2));      // включаем / выключаем LED
    time0 = millis();
  }  

if(p_in > 0 && K1 > 0 && K2 == 0){              // Условие начала отсчета с момента подачи воды
  K2 = 1;
  m = 0;
  h = 0;
  time5 = millis();  
  } 

if(millis() - time5 > 60000){
      time5 = millis();
        ++m; // отсчет 1 мин
      }
      if(m > 59){
        m = 0;
        ++h; // отсчет 1 час
      }
      if(h > 23){
        h = 0;
      }

if(millis() - time6 > 150){             // Задержка опроса датчиков для вычисления медиан
distance[0] = ultrasonic.Ranging(MM);   //Новый замер дальномера
  //delay(150);
   byte previous = 18;
   byte current = 19;
   for (byte i = 1; i < 20; i++) {             //Функция для обновления данных в массиве
    distance[current] = distance[previous];
     previous--;
     current--;
   }
   bool leave = false;          //Функция для сортировки массива в порядке возрастания
  float temp;
   while (!leave) {
     leave = true;
     byte i = 0;
     for (byte counter = 0; counter < 19; counter++) {
       if (distance[i] > distance[i + 1]) {
         temp = distance[i];
         distance[i] = distance[i + 1];
         distance[i + 1] = temp;
         leave = false;
       }
       i++;
     }
   }
   
pr[0] = analogRead(DP);      // Новый замер датчика давления
  //delay(150);
   byte previous1 = 18;
   byte current1 = 19;
   for (byte j = 1; j < 20; j++) {             //Функция для обновления данных в массиве
    pr[current1] = pr[previous1];
     previous1--;
     current1--;
   }
   bool leave1 = false;          //Функция для сортировки массива в порядке возрастания
  float temp1;
   while (!leave1) {
     leave1 = true;
     byte j = 0;
     for (byte counter1 = 0; counter1 < 19; counter1++) {
       if (pr[j] > pr[j + 1]) {
         temp1 = pr[j];
         pr[j] = pr[j + 1];
         pr[j + 1] = temp1;
         leave1 = false;
       }
       j++;
     }
   }
   time6 = millis();
}

float Median = (distance[9] + distance[10]) / 2; //расчет медианы дальномера
float Median1 = (pr[9] + pr[10]) / 2;            //расчет медианы датчика давления

Serial.println(analogRead(DP));

p_in = (Median1 - 93) * 1.2 / (882 - 93) * 10;     // Вычисляем давление воды на входе в bar
if(Median1 < 93){    // Условие когда давление равно 0
    p_in = 0;
  }
  
//if(p_in < 0.05){
//      p_in = 0;
//    }

float H = 1531-(Median - 135);           //Высота воды в баке
if (H < 0)
{
  H = 0;
}

float Vv = H*0.65316786;                  //Вычисляем объем в литрах, принимая что в 1 см 6,53 литра, в 1 мм 0,65 литра
float Vvp = Vv*0.1;                       //Вычисляем объем в %
float V = 0;
float Vp = 0;
if (Vv > 1000)
{
  V = 1000;
}
else 
{
  V = Vv;
}

if (Vvp > 100)
{
  Vp = 100;
}
else
{
  Vp = Vvp;
}

int Y = 300-200*Vp*0.01;                //Вычисляем координату Y прямоугольника с водой

//if(V < 400){                           // Условие перекрытия подачи воды на улицу
    //digitalWrite(RELEY_UL, HIGH);
//  } else {
    //digitalWrite(RELEY_UL, LOW);
//  }

//  if(millis() - time2 > 1000){       // Условие вывода информации на экран раз в секунду
//    time2 = millis();
//    u8g.firstPage();  
//      do
//  {

    myGLCD.setColor(255, 255, 255);      // Задаем цвет выводимого ниже не экран
    myGLCD.setFont(Arial_round_16x24);   //Задаем шрифт выводимого ниже на экран
    
if(p_in > 0 && K1 > 0){             // Условие вывода времена с момента подачи воды на входе
    myGLCD.print(":", 50, 10);      //Выводим текст в кавычках      
    if(h < 10){                     // Обнуление второго разряда часов
    myGLCD.printNumI(0, 10, 10);
    myGLCD.printNumI(h, 30, 10);
  }   else {
    myGLCD.printNumI(h, 10, 10);
  }
  if(m < 10){                      // Обнуление второго разряда минут
    myGLCD.printNumI(0, 60, 10);
    myGLCD.printNumI(m, 70, 10);
  }   else {
    myGLCD.printNumI(m, 60, 10);
  }
 }
 
    myGLCD.print("p:", 10, 80);                    //Выводим текст в кавычках
    myGLCD.print("bar", 130, 80);                  //Выводим текст в кавычках
    myGLCD.printNumF(p_in, 2, 50, 80);             //Выводим значение давления на входе
    myGLCD.print("T:", 10, 260);                   //Выводим текст в кавычках
    myGLCD.print("`C", 130, 260);                  //Выводим текст в кавычках
    myGLCD.printNumF(AM2320.tem, 1, 50, 260);      //Выводим значение температуры в подвале
    myGLCD.print("H:", 10, 290);                   //Выводим текст в кавычках
    myGLCD.print("%", 130, 290);                   //Выводим текст в кавычках
    myGLCD.printNumF(Hk, 1, 50, 290);              //Выводим значение влажности в подвале
    myGLCD.print("V:", 310, 45);                   //Выводим текст в кавычках
    myGLCD.print("L", 430, 45);                    //Выводим текст в кавычках
    myGLCD.printNumI(V, 350, 45);                  //Выводим значение количества воды в баках
    myGLCD.print("%", 405, 10);                    //Выводим текст в кавычках
    myGLCD.printNumI(Vp, 345, 10);                 //Выводим значение количества воды в баках в %
    
    myGLCD.drawLine(310,80,310,300);          // Рисуем три линии обозначающие емкость
    myGLCD.drawLine(310,300,450,300);
    myGLCD.drawLine(450,80,450,300);
    myGLCD.drawLine(280,0,280,320);           // Рисуем вертикальную линию, разделяющую показания    
    if (V < 200) {
      myGLCD.setColor(250, 2, 2);            // Задаем красный цвет прямоугольника с водой   
    } else {
      if (V < 500) {
      myGLCD.setColor(247, 236, 22);            // Задаем желтый цвет прямоугольника с водой    
    } else {
      myGLCD.setColor(35, 144, 252);            // Задаем синий цвет прямоугольника с водой
    }
    }
    myGLCD.fillRect(311,299,449,Y);           // Рисуем прямоугольник, показывающий уровень воды
 
}
