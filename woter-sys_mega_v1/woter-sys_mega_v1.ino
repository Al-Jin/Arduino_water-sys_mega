#include <Ultrasonic.h>                 // Подключаем библиотеку для работы с дальномером переделана в миллиметрах
#include <iarduino_DHT.h>               // Подключаем библиотеку для работы с датчиком DHT
//#include <U8glib.h>                     // Подключаем библиотеку для работы с граф дисплеем
#include <UTFT.h>                       //Подключаем библиотеку для работы с tft дисплеем
//Цифровой D0 - RX зарезервирован для WiFi
//Цифровой D2 - TX зарезервирован для WiFi
Ultrasonic ultrasonic(2, 3);            // Trig - 2, Echo - 3 дальномер
#define SEN_IN 4                        // Подключение датчика перелива к цифровому выходу D4
#define LED_p_in1 5                     // Подключение светодиода утечки воды к цифровому выходу D5
#define buttonPin1 6                    // Кнопка отключения светодиода перелива воды к цифровому выходу D6
iarduino_DHT AM2320(7);                 // Объявляем объект датчик температуры и влажности, указывая номер вывода, к которому подключён модуль
//#define RELEY_UL 8                    // Зарезервирован для реле закрытия подачи воды на улицу к цифровому выходу D8
//#define RELEY_IN 9                    // Зарезервирован для реле закрытия подачи воды в бак к цифровому выходу D9
#define buttonPin2 10                   // Кнопка отключения светодиода отсутствия давления воды на входе подключена к цифровому выводу D10
#define LED_p_in2 11                    // Светодиод когда давление воды на входе 0 подключен к цифровому выводу D11
//U8GLIB_ST7920_128X64 u8g(11, 12, 13, U8G_PIN_NONE);   // Инициализация дисплея 128х64 (13 последний цифровой)

// назначить аналоговый выход А0 18
#define DP 19          // Аналоговый вход A1 подключен датчик давления на входе
#define buttonPin3 20  // Кнопка светодиода закрытия воды на улицу подключена к аналоговому выводу A2
#define LED_p_in3 21   // Светодиод перекрытия воды на улицу подключен к аналоговому выводу A3



UTFT myGLCD(MAR3951,38,39,40,41);   //Инициализируем подключение tft дисплея

  extern uint8_t SmallFont[];         // Добавляем маленький шрифт
  extern uint8_t BigFont[];           // Добавляем большой шрифт
  extern uint8_t SevenSegNumFont[];   // Добавляем шрифт имитирующий семисегментный индикатор
  extern uint8_t Arial_round_16x24[]; // Новый шрифт

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
    K1 = 1;
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

p_in = (Median1 - 102) * 1.2 / (882 - 102) * 10;     // Вычисляем давление воды на входе в bar
if(Median1 < 102){    // Условие когда давление равно 0
    p_in = 0;
  }
  
if(p_in < 0.05){
      p_in = 0;
    }

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

int X=1+0.5*Vvp;                           //Вычисляем координаты прямоугольника с водой
int KU=64-X;

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
//    if(millis() < 5001){     // Вывод загрузучной картинки первые 5 секунд работы
//    draw();
//    }
       
//    else { u8g.setFont(u8g_font_6x12); //задаем шрифт

   myGLCD.setColor(255, 255, 255);   // Задаем цвет выводимого ниже не экран
   
//if(p_in > 0 && K1 > 0){             // Условие вывода времена с момента подачи воды на входе
//      myGLCD.(32, 7, ":");      //Выводим текст в кавычках
//      
//    if(h < 10){  // Обнуление второго разряда часов
//      u8g.setPrintPos(18, 7);     //Указываем координаты вывода значения времени
//      u8g.print(0);
//      u8g.setPrintPos(25, 7);     //Указываем координаты вывода значения времени
//      u8g.print(h);   
//  }   else {
//      u8g.setPrintPos(18, 7);     //Указываем координаты вывода значения времени
//      u8g.print(h);
//  }
//  if(m < 10){  // Обнуление второго разряда часов
//     u8g.setPrintPos(38, 7);     //Указываем координаты вывода значения времени
//     u8g.print(0);
//      u8g.setPrintPos(45, 7);     //Указываем координаты вывода значения времени
//      u8g.print(m);   
//  }   else {
//      u8g.setPrintPos(38, 7);     //Указываем координаты вывода значения времени
//      u8g.print(m);
//  }
// }
float test = 24.78;
myGLCD.setFont(Arial_round_16x24);    
    myGLCD.print("p:     bar", 10, 50);       //Выводим текст в кавычках
    myGLCD.printNumF(p_in, 2, 50, 50);        //Выводим значение давления на входе
    myGLCD.print("T:     `C", 10, 120);      //Выводим текст в кавычках
    myGLCD.printNumF(AM2320.tem, 1, 50, 120); //Выводим значение температуры в подвале
    myGLCD.print("H:      %", 10, 150);       //Выводим текст в кавычках
    myGLCD.printNumF(Hk, 1, 50, 150);         //Выводим значение влажности в подвале

//    u8g.drawStr( 7, 64, "H:      %");       //Выводим текст в кавычках
//    u8g.setPrintPos(23, 64);                //Указываем координаты вывода значения влажности
//    u8g.print(Hk,1);                        //Выводим значение влажности
//    u8g.setPrintPos(89, 7);                 //Указываем координаты вывода объема воды
//    u8g.print(Vp,0);                        //Выводим значение юбъем в % округляем до целего
//    u8g.drawBox(0, 26, 70, 16);             // Рисуем прямоугольник - фон для вывода объема
//    u8g.setColorIndex(0);                   // Выводим текст черным
//    u8g.setFont(u8g_font_9x15B);            //задаем шрифт
//    u8g.setPrintPos(20, 39);                //Указываем координаты вывода значения объема
//    u8g.print(V,0);                         //Выводим объем в литрах
//    u8g.drawStr( 5, 39, "V     L");         //Выводим текст в кавычках
//    u8g.setColorIndex(1);                   // Возвращаем вывод текста белым
//    u8g.setFont(u8g_font_6x12);             //задаем шрифт
//    u8g.drawStr( 110, 7, "%");              //Выводим текст в кавычках
//    u8g.drawLine(80, 9, 80, 64);            // Рисуем три линии обозначающие емкость
//    u8g.drawLine(80, 64, 120, 64);
//    u8g.drawLine(120, 9, 120, 64);
//    u8g.drawBox(80, KU, 40, X);             // Рисуем прямоугольник х=80, показывающий уровень воды шириной 40 пикселей
//    u8g.drawLine(70, 0, 70, 64);            // Рисуем вертикальную линию, разделяющую показания
      } 
//  }while( u8g.nextPage() );
//      }
//}
