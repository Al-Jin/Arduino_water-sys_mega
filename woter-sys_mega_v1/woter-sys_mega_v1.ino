#include <Ultrasonic.h>                 // Подключаем библиотеку для работы с дальномером переделана в миллиметрах
#include <iarduino_DHT.h>               // Подключаем библиотеку для работы с датчиком DHT
#include <UTFT.h>                       //Подключаем библиотеку для работы с tft дисплеем
//Цифровой D0 - пустой


//Цифровой D2 - датчик расхода воды резерв
//Цифровой D3 - пустой

Ultrasonic ultrasonic(4, 5);      // Trig - 2, Echo - 3 дальномер
#define SEN_IN 6                  // Подключение датчика перелива к цифровому выходу D4
iarduino_DHT AM2320(7);           // Объявляем объект датчик температуры и влажности, указывая номер вывода, к которому подключён модуль
#define LED_p_in1 8               // Подключение светодиода утечки воды к цифровому выходу D6
#define buttonPin1 9              // Кнопка отключения светодиода перелива воды к цифровому выходу D7
#define LED_p_in2 10               // Светодиод когда давление воды на входе 0 подключен к цифровому выводу D8
#define buttonPin2 11              // Кнопка отключения светодиода отсутствия давления воды на входе подключена к цифровому выводу D9
#define LED_p_in3 12              // Светодиод перекрытия воды на улицу подключен к цифровому выводу D10
#define buttonPin3 13             // Кнопка светодиода закрытия воды на улицу подключена к цифровому выводу D11


#define RELEY_Ul_On 17            // Реле закрытия подачи воды на улицу к цифровому выходу D17
#define RELEY_Ul_Off 18           // Реле открытия подачи воды на улицу к цифровому выходу D18
#define RELEY_In_On 19            // Реле закрытия подачи воды в баки к цифровому выходу D19
#define RELEY_In_Off 20           // Реле открытия подачи воды в баки к цифровому выходу D20



UTFT myGLCD(MAR3951,38,39,40,41);   //Инициализируем подключение tft дисплея

#define DP 54                      // Датчик давления на входе подключен к аналоговому выводу A0
#define buttonPin4 55                      // Кнопка увеличения полива подключена к аналоговому выводу A1
#define buttonPin5 56                      // Кнопка уменьшения полива подключена к аналоговому выводу A2
#define buttonPin6 57                      // Кнопка ввода полива подключена к аналоговому выводу A3

  extern uint8_t SmallFont[];              // Добавляем маленький шрифт
  extern uint8_t Arial_round_16x24[];      // Новый шрифт
  extern uint8_t Inconsola[];
  extern uint8_t Arial_bold_16x16[];       // Новый шрифт
  extern uint8_t ArialNumFontPlus[];
//  extern uint8_t SixteenSegment16x24[];    // Семисигментный шрифт для вывода времени 
//  extern uint8_t SixteenSegment24x36[];    // Семисигментный шрифт для вывода времени

int K1 = 0;              // Коэффициент горения светодиода отсутствия давления воды на входе  
int K2 = 0;              // Коэффициент включения времени мигания светодиода
int K3 = 0;              // Коэффициент горения светодиода перелива
int K4 = 0;              // Коэффициент отсчета времени включения светодиода давления воды
int K5 = 0;              // Коэффициент горения светодиода перекрытия воды на улицу
int KinOn = 1;           // Коэффициент открытия подачи воды в бак
int KinOff = 0;          // Коэффициент закрытия подачи воды в бак
int KUlOn = 1;           // Коэффициент открытия подачи воды на улицу
int KUlOff = 0;          // Коэффициент закрытия подачи воды на улицу
int Kpoliv = 0;          // Коэффициент полива
int Kpoliv1 = 0;         // Коэффициент включения индикации полива

int buttonState1 = 0;    // Переменная для хранения состояния кнопки отключения светодиода перелива
int buttonState2 = 0;    // Переменная для хранения состояния кнопки отключения светодиода отсутствия давления воды на входе
int buttonState3 = 0;    // Переменная для хранения состояния кнопки отключения светодиода перекрытия воды на улицу
int buttonState4 = 0;    // Переменная для хранения состояния кнопки увеличения полива
int buttonState5 = 0;    // Переменная для хранения состояния кнопки уменьшения полива
int buttonState6 = 0;    // Переменная для хранения состояния кнопки ввода полива

unsigned long time0 = 0;   // Переменная для хранения точки отсчета в мигании светодиодом отсутствия воды
unsigned long time1 = 0;   // Переменная для хранения точки отсчета в для гашения светодиода при кратковременном отсутствии воды
unsigned long time2 = 0;   // Переменная для хранения точки отсчета в мигании светодиода перекрытия выды на улицу
unsigned long time3 = 0;   // Переменная для хранения точки отсчета считывания температуры и влажности
unsigned long time4 = 0;   // Переменная для хранения точки отсчета в мигании светодиода протечки
unsigned long time5 = 0;   // Переменная для хранения точки отсчета времени подачи воды
unsigned long time6 = 0;   // Переменная для хранения точки отсчета времени в вычислении медиан
unsigned long time7 = 0;   // Переменная для хранения точки отсчета времени для открытия подачи воды в бак
unsigned long time8 = 0;   // Переменная для хранения точки отсчета времени для закрытия подачи воды в бак
unsigned long time9 = 0;   // Переменная для хранения точки отсчета времени для открытия подачи воды на улицу
unsigned long time10 = 0;   // Переменная для хранения точки отсчета времени для закрытия подачи воды на улицу
unsigned long time11 = 0;   // Переменная для хранения точки отсчета времени для мигания значения полива до ввода
float Hk = 0;              //Переменная влажности с учетом понижающего коэффициента
int s = 0;                 // Переменная для хранения секунд времени с начала подачи воды
int m = 0;                 // Переменная для хранения минут времени с начала подачи воды
int h = 0;                 // Переменная для хранения часов времени с начала подачи воды
float p_in = 1;            // Переменная для хранения давления воды на входе
float distance[20]; // Массив для вычисления медианы данных с дальномера
float pr[20];       // Массив для вычисления медианы данных с датчика давления
int V = 1000;
int Vp = 100;
int poliv = 0;     // Переменная для хранения количества литров полива

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

//При начале работы электрокраны должны быть открыты!!!!!
   
  pinMode(SEN_IN, INPUT);             // Датчик перелива
  pinMode(LED_p_in1, OUTPUT);         // Светодиод перелива
  pinMode(RELEY_In_On, OUTPUT);       // Реле открытия подачи воды в бак
  digitalWrite(RELEY_In_On, HIGH);    // Выключаем реле подачи воды в бак - посылаем высокий сигнал
  pinMode(RELEY_In_Off, OUTPUT);      // Реле закрытия подачи воды в бак
  digitalWrite(RELEY_In_Off, HIGH);   // Выключаем реле закрытия подачи воды в бак - посылаем высокий сигнал
  pinMode(RELEY_Ul_On, OUTPUT);       // Реле открытия подачи воды на улицу
  digitalWrite(RELEY_Ul_On, HIGH);    // Выключаем реле подачи воды на улицу - посылаем высокий сигнал
  pinMode(RELEY_Ul_Off, OUTPUT);      // Реле закрытия подачи воды на улицу
  digitalWrite(RELEY_Ul_Off, HIGH);   // Выключаем реле закрытия подачи воды на улицу - посылаем высокий сигнал
  pinMode(LED_p_in2, OUTPUT);         // Светодиод когда давление воды на входе 0
  pinMode(LED_p_in3, OUTPUT);         // Светодиод перекрытия воды на улицу
  pinMode(buttonPin1, INPUT);         // Инициализируем пин, подключенный к кнопке отключения светодиода перелива, как вход
  pinMode(buttonPin2, INPUT);         // Инициализируем пин, подключенный к кнопке отключения светодиода отсутствия давления воды на входе, как вход
  pinMode(buttonPin3, INPUT);         // Инициализируем пин, подключенный к кнопке отключения светодиода закрытия крана на улицу, как вход
  pinMode(buttonPin4, INPUT);         // Инициализируем пин, подключенный к кнопке увеличения полива, как вход
  pinMode(buttonPin5, INPUT);         // Инициализируем пин, подключенный к кнопке уменьшения полива, как вход
  pinMode(buttonPin6, INPUT);         // Инициализируем пин, подключенный к кнопке ввода полива, как вход
}

void loop()
{
buttonState1 = digitalRead(buttonPin1);                // Считваем значения с кнопки отключения светодиода перелива  
buttonState2 = digitalRead(buttonPin2);                // Считваем значения с кнопки отключения светодиода отсутствия давления воды на входе
buttonState3 = digitalRead(buttonPin3);                // Считваем значения с кнопки отключения светодиода перекрытия воды на улицу
buttonState4 = digitalRead(buttonPin4);                // Считваем значения с кнопки увеличения полива
buttonState5 = digitalRead(buttonPin5);                // Считваем значения с кнопки уменьшения полива
buttonState6 = digitalRead(buttonPin6);                // Считваем значения с кнопки ввода полива

if(buttonState1 == HIGH){                          // Условие отключения светодиода перелива при нажатии кнопки
    K3 = 0;                                          // Обнуление коэффициента, чтобы светодиод потух
    digitalWrite(LED_p_in1, LOW);
  }

if(buttonState2 == HIGH){                          // Условие отключения светодиода давления воды ноль на входе при нажатии кнопки
    K1 = 0;                                          // Обнуление коэффициента, чтобы светодиод потух
    K2 = 0;
    K4 = 0;
    digitalWrite(LED_p_in2, LOW);
    myGLCD.setFont(Arial_round_16x24);      //Задаем шрифт выводимого ниже на экран
    myGLCD.print("               ", 10, 70);           //Выводим пробелы, чтобы убрать время с момента подачи воды
  }

if(buttonState4 == HIGH && Kpoliv == 0){                   // Условие активации ввода полива
    poliv = poliv + 100;                                   // Увеличение литров полива
    Kpoliv1 = 1;
  }

if(buttonState5 == HIGH && Kpoliv == 0){                   // Условие активации ввода полива
    poliv = poliv - 100;                                  // Уменьшение литров полива
    Kpoliv1 = 1;
  }

if(poliv < 0){                   // Проверка чтобы полив не уходил в отрицательное число
    poliv = 0;
  }

if(buttonState6 == HIGH && Kpoliv == 0){                   // Условие ввода и начала полива
    Kpoliv = 1;
  }

if(buttonState3 == HIGH){                          // Условие отключения светодиода перекрытия воды на улицу при нажатии кнопки
    K5 = 0;                                          // Обнуление коэффициента, чтобы светодиод потух
    digitalWrite(LED_p_in3, LOW);
  }

if(millis() - time3 > 3000){       // Условие опроса датчика температуры и влажности раз в 3 секунды
    switch(AM2320.read());           //Считываем температуру и влажность
    time3 = millis();
  }
  
Serial.println(Kpoliv);
Serial.println(Kpoliv1);
Serial.println(poliv);
Serial.println("*");
Serial.println(buttonState4);
Serial.println(buttonState5);
Serial.println(buttonState6);
Serial.println("*****************************");

Hk = AM2320.hum * 0.73;    // Поправочный коэффициент влажности

if(digitalRead(SEN_IN) == LOW){    // Условие включения светодиода при протечке
    digitalWrite(LED_p_in1, HIGH);
    K3 = 1;
  }

if(V < 400){                            // Условие включения светодиода перекрытия воды на улицу
    digitalWrite(LED_p_in3, HIGH);
    K5 = 1;
  }
  
  if (millis() > 60000) {        // Условие игнорирующее открытие или закрытие кранов в первые 60 секунд работы программы
    if ((KinOff == 0 && K1 == 1) || (KinOff == 0 && K3 == 1)) {  //Условия закрытия крана подачи воды
      time8 = millis();
      KinOff = 1;
      KinOn = 0;
    }
    if ((KinOn == 0 && K1 == 0 && K3 == 0)) {  //Условия открытия крана подачи воды
      time7 = millis();
      KinOff = 0;
      KinOn = 1;
    } 
    if ((K1 == 1 && (millis() - time8) < 20000) || (K3 == 1 && (millis() - time8) < 20000)) {  // Подача напряжения на электро кран для закрытия подачи воды в бак 20 секунд
     digitalWrite(RELEY_In_Off, LOW);   //Включение реле закрытия крана подачи воды
    } else {
      digitalWrite(RELEY_In_Off, HIGH);
   }
   if ((K1 == 0 && millis() - time7 < 20000) || (K3 == 0 && millis() - time7 < 20000)) {  // Подача напряжения на электро кран для открытия подачи воды в бак 20 секунд
   digitalWrite(RELEY_In_On, LOW);   //Включение реле открытия крана подачи воды
   } else {
   digitalWrite(RELEY_In_On, HIGH);
    }
   if (KUlOn == 0 && K5 == 0) {  //Условия открытия крана подачи воды на улицу
      time9 = millis();
      KUlOff = 0;
      KUlOn = 1;
   }
   if (KUlOff == 0 && K5 == 1) {  //Условия закрытия крана подачи воды на улицу
      time10 = millis();
      KUlOff = 1;
      KUlOn = 0;
   }
   if (K5 == 1 && (millis() - time10) < 20000) {                       // Подача напряжения на электро кран для закрытия подачи воды на улицу 20 секунд
     digitalWrite(RELEY_Ul_Off, LOW);                   //Включение реле закрытия крана подачи воды на улицу
    } else {
      digitalWrite(RELEY_Ul_Off, HIGH);
   }
   if (K5 == 0 && millis() - time9 < 20000) {  // Подача напряжения на электро кран для открытия подачи воды на улицу 20 секунд
   digitalWrite(RELEY_Ul_On, LOW);                              //Включение реле открытия крана подачи воды на улицу
   } else {
   digitalWrite(RELEY_Ul_On, HIGH);
    } 
  }
  
  if(digitalRead(SEN_IN) != LOW && K3 > 0 && millis() - time4 > 500){       // Условие мигания светодиода протечки при понижении уровня воды до нормального
    digitalWrite(LED_p_in1, !digitalRead(LED_p_in1));        // включаем / выключаем LED
    time4 = millis();
  }  

  if(V > 450 && K5 > 0 && millis() - time2 > 500){       // Условие мигания светодиода перекрытия воды на улицу при повышении уровня воды до нормального
    digitalWrite(LED_p_in3, !digitalRead(LED_p_in3));        // включаем / выключаем LED
    time2 = millis();
  }  

if(p_in == 0 && K4 == 0){       // Изменение значения переменной для отсчета включения светодиода когда давление воды на входе равно 0
    time1 = millis();
    K4 = 1;
  } 

if(p_in > 0){       // Изменение значения переменной для отсчета включения светодиода когда давление воды на входе равно 0
    K4 = 0;
  } 

if(p_in == 0 && millis() - time1 > 600000 && K1 == 0){       // Условие включения светодиода когда давление воды на входе равно 0 в течении 10 минут 600000
    digitalWrite(LED_p_in2, HIGH);
    K1 = 1;                                 // Коэффициент, для мигания светодиода после подачи воды
  } else {
//    K1 = 0;
  }

if(p_in > 0 && K1 > 0 && millis() - time0 > 500){          // Условие мигания светодиода после подачи воды
    digitalWrite(LED_p_in2, !digitalRead(LED_p_in2));      // включаем / выключаем LED
    time0 = millis();
  }  

if (p_in == 0 && K1 > 0){
  K2 = 0;
  digitalWrite(LED_p_in2, HIGH);
}

if(p_in > 0 && K1 > 0 && K2 == 0){              // Условие начала отсчета с момента подачи воды
  K2 = 1;
  s = 0;
  m = 0;
  h = 0;
  time5 = millis();  
  } 

if(millis() - time5 > 1000){
      time5 = millis();
        ++s;   //Отсчет 1 секунды
      }
      if(s > 59){
        s = 0;
        ++m;      // отсчет 1 мин
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

//Serial.println(analogRead(DP));

p_in = (Median1 - 92) * 1.2 / (882 - 92) * 10;     // Вычисляем давление воды на входе в bar
if(Median1 < 92){    // Условие когда давление равно 0
    p_in = 0;
  }
  
//if(p_in < 0.05){
//      p_in = 0;
//    }

//p_in = 0;

float H = 1531-(Median - 135);           //Высота воды в баке
if (H < 0) {
  H = 0;
}

int Vv = H*0.65316786;                  //Вычисляем объем в литрах, принимая что в 1 см 6,53 литра, в 1 мм 0,65 литра
int Vvp = Vv*0.1;                       //Вычисляем объем в %

if (Vv > 1000) {
  V = 1000;
} else {
  V = Vv;
}
if (Vvp > 100)
{  Vp = 100;
} else{
  Vp = Vvp;
}

int Y = 300-200*Vp*0.01;     //Вычисляем координату Y прямоугольника с водой
int Y1 = Y-1;                //Вычисляем координату Y прямоугольника с водой


    myGLCD.setColor(255, 255, 255);        // Задаем цвет выводимого ниже на экран - белый
    myGLCD.setFont(Arial_round_16x24);   //Задаем шрифт выводимого ниже на экран

if(p_in > 0 && K1 > 0){                   // Условие вывода времена с момента подачи воды на входе
    myGLCD.print("p>0:", 10, 70);             //Выводим текст в кавычках
    myGLCD.print(":", 120, 70);             //Выводим текст в кавычках        
    if(h < 10){                            // Обнуление второго разряда часов
    myGLCD.printNumI(0, 85, 70);
    myGLCD.printNumI(h, 102, 70);
  }   else {
    myGLCD.printNumI(h, 85, 70);
  } if(m < 10){                         // Обнуление второго разряда минут
    myGLCD.printNumI(0, 136, 70);
    myGLCD.printNumI(m, 153, 70);
  }   else {
    myGLCD.printNumI(m, 136, 70);
  } if(s < 10){                         // Обнуление второго разряда минут
    myGLCD.setFont(Arial_bold_16x16);   //Задаем шрифт выводимого ниже на экран
    myGLCD.printNumI(0, 175, 75);
    myGLCD.printNumI(s, 192, 75);
  }   else {
    myGLCD.setFont(Arial_bold_16x16);   //Задаем шрифт выводимого ниже на экран
    myGLCD.printNumI(s, 175, 75);
  }} else {
    myGLCD.setFont(Arial_round_16x24);   //Задаем шрифт выводимого ниже на экран
    myGLCD.print("               ", 10, 70);           //Выводим пробелы, чтобы убрать время с момента подачи воды
  }

    myGLCD.setFont(Arial_round_16x24);   //Задаем шрифт выводимого ниже на экран
    
if(millis() - time11 > 1000 && Kpoliv1 == 1){       // Условие вывода информации полив на экран раз в секунду
    time11 = millis();
    myGLCD.print("Vpoliv:", 10, 165);             //Выводим текст в кавычках
    myGLCD.print("L", 250, 165);                 //Выводим текст в кавычках
    myGLCD.setFont(ArialNumFontPlus);                   //Задаем шрифт выводимого ниже на экран
    myGLCD.printNumI(poliv, 140, 126, 3);             //Выводим текст в кавычках
  } else {
      myGLCD.setColor(0, 0, 0);                  // Задаем черный цвет прямоугольника
      myGLCD.fillRect(140,126,270,210);           // Рисуем прямоугольник, для закрашивания цифры полива
//      myGLCD.setFont(ArialNumFontPlus);                   //Задаем шрифт выводимого ниже на экран
//      myGLCD.print("    ", 140, 126);             //Выводим текст в кавычках
      }
      
    myGLCD.setColor(255, 255, 255);        // Задаем цвет выводимого ниже на экран - белый  
    myGLCD.setFont(Arial_round_16x24);              //Задаем шрифт выводимого ниже на экран
    myGLCD.drawLine(0,106,280,106);           // Рисуем линию отделяющую показания давления
    myGLCD.drawLine(0,214,280,214);           // Рисуем линию отделяющую показания температуры и влажности
    
    myGLCD.print("p:", 10, 20);                    //Выводим текст в кавычках
    myGLCD.print("bar", 130, 20);                  //Выводим текст в кавычках
    myGLCD.printNumF(p_in, 2, 50, 20);             //Выводим значение давления на входе
    myGLCD.print("T:", 10, 235);                   //Выводим текст в кавычках
    myGLCD.print("`C", 130, 235);                  //Выводим текст в кавычках
    myGLCD.printNumF(AM2320.tem, 1, 50, 235);      //Выводим значение температуры в подвале
    myGLCD.print("H:", 10, 280);                   //Выводим текст в кавычках
    myGLCD.print("%", 130, 280);                   //Выводим текст в кавычках
    myGLCD.printNumF(Hk, 1, 50, 280);              //Выводим значение влажности в подвале
    myGLCD.print("V:", 340, 45);                   //Выводим текст в кавычках
    myGLCD.print("L", 447, 45);                    //Выводим текст в кавычках
    myGLCD.printNumI(V, 375, 45, 4);                  //Выводим значение количества воды в баках
    myGLCD.print("%", 435, 10);                    //Выводим текст в кавычках
    myGLCD.printNumI(Vp, 377, 10, 3);                 //Выводим значение количества воды в баках в %
    
    myGLCD.drawLine(330,80,330,300);           // Рисуем три линии обозначающие емкость
    myGLCD.drawLine(330,300,470,300);
    myGLCD.drawLine(470,80,470,300);
    myGLCD.drawLine(285,99,329,99);            // Рисуем линию на уровне полных баков
    myGLCD.drawLine(319,150,329,150);          // Рисуем линию на уровне 3/4 баков
    myGLCD.drawLine(285,200,329,200);          // Рисуем линию на уровне половины баков
    myGLCD.drawLine(319,250,329,250);          // Рисуем линию на уровне четверти баков
    myGLCD.setFont(SmallFont);                 //Задаем шрифт выводимого ниже на экран
    myGLCD.print("100%", 290, 87);             //Выводим текст в кавычках
    myGLCD.print("50%", 295, 187);             //Выводим текст в кавычках
    myGLCD.drawLine(280,0,280,320);           // Рисуем вертикальную линию, разделяющую показания    
    if (V < 200) {
      myGLCD.setColor(250, 0, 0);             // Задаем красный цвет прямоугольника с водой   
    } else {
      if (V < 500) {
      myGLCD.setColor(247, 236, 22);            // Задаем желтый цвет прямоугольника с водой    
    } else {
      myGLCD.setColor(35, 144, 252);            // Задаем синий цвет прямоугольника с водой
      }
    }
    myGLCD.fillRect(331,299,469,Y);            // Рисуем прямоугольник, показывающий уровень воды
    if (Vp < 100) {
    myGLCD.setColor(0, 0, 0);                  // Задаем черный цвет прямоугольника
    myGLCD.fillRect(331,100,469,Y1);           // Рисуем прямоугольник, для закрашивания остатка прямоугольника уровня воды
    }
 
}
