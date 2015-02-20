#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/* Definizioni globali */
/* Corrispondenza pin LCD <-> pin digitali di Arduino */

#define RS  7
#define EN  6
#define D7  2  
#define D6  3
#define D5  4
#define D4  5

#define ONE_WIRE_BUS 10
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define COOLER 8
#define HEATER 9
#define Button1 13
#define Button2 12

#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN) 
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)

LiquidCrystal lcd( RS, EN, D4, D5, D6, D7 ); 

const float tempTollerance = 1.0;

float fermenterhTemp = 24.0;
float fermenterlTemp = 18.0;

float mashTemp[6];
int mashTime[6];
int mashNum = 0;

float boilTemp[6];
int boilTime[6];
int boilNum = 0;

int state = 0;

char* p[] = {"ALL OFF","COOLER ON","HEATER ON"}; 
char* pd[] = {"LC BEER","MASH","BOIL","FERM","INFO","SEL BEER"}; 
char* ss[] = {"STOP","START"}; 
int d = 250;
int i = 0;

int countButton1 = 0;
int countButton2 = 0;
int countButton3 = 1;
int valButton1 = 0;
int valButton2 = 0;

int timerStatus = 0;
unsigned long timerStartMillis = 0;
unsigned long timerCurrentMillis = 0;

/* Impostazione dell'hardware */

void setup() 
{
  //Serial.begin(9600);
  sensors.begin();
  lcd.begin( 2, 16 );  
  pinMode(COOLER,OUTPUT);
  pinMode(HEATER,OUTPUT);
  digitalWrite(COOLER, LOW);
  digitalWrite(HEATER, LOW);
  pinMode(Button1,INPUT);
  pinMode(Button2,INPUT);
  
  fermenterhTemp = fermenterhTemp - tempTollerance;
  fermenterlTemp = fermenterlTemp + tempTollerance;
  
  mashTemp[0] = 0;

  
}

void loop()
{
  
  //controllo che il tasto B2 sia a STOP, solo così posso scegliere il programma con il tasto B1
  //scelto il programma posso premere B2 per farlo partire e non posso più cambiarlo
  //se premo B2 di nuovo lo STOPPO e posso scegliere ancora un altro programma
  
  if(countButton2 == 0){
    
    digitalWrite(COOLER,LOW);
    digitalWrite(HEATER,LOW);
    timerStatus = 0;
    d = 250;
    i = 0;
    state = 0;
    
    valButton1 = digitalRead(Button1);
    if(valButton1 == HIGH){
      countButton1++; 
    } 
    if(countButton1 == 6){
      countButton1 = 0; 
    }
  
    sendProgToLCD(countButton1);
  }
  
  if(countButton1 > 0){
    valButton2 = digitalRead(Button2);
    if(valButton2 == HIGH){
      countButton2++; 
      if(countButton2 == 2){
        countButton2 = 0;
      }
      sendStartToLCD(countButton2,countButton1);
    }
  }
  
  if(countButton2 == 1){
    switch(countButton1){
      case 1:
        d = 500;
        mashPrg();
        break;
      case 2:
        d = 500;
        boilPrg();
        break;
      case 3:
        d = 3000;
        fermenterPrg();
        break;
      case 4:
        d = 1000;
        infoPrg();
        break;
      case 5:
        d = 250;
        selPrg();
        break;
    }
  }
    
  delay( d );
}

// END LOOP


/*

SEL

*/

void selPrg()
{

  valButton1 = digitalRead(Button1);
  if(valButton1 == HIGH){
      countButton3++; 
  }
  
  if(countButton3 == 4){
    countButton3 = 1;
  }
  
  switch(countButton3){
    case 1:
    //STOUT
        lcd.clear();
        lcd.setCursor( 0, 0 );
        lcd.print("STOUT");
        lcd.setCursor( 0, 1 );
        lcd.print("M: 65 68 78");
        mashTemp[0] = 65.0;
        mashTemp[1] = 68.0;
        mashTemp[2] = 78.0;
        mashTime[0] = 0;
        mashTime[1] = 59;
        mashTime[2] = 15;
        mashNum = 3;
        boilTemp[0] = 100.0;
        boilTemp[1] = 100.0;
        boilTemp[2] = 100.0;
        boilTemp[3] = 100.0;
        boilTime[0] = 0;
        boilTime[1] = 30;
        boilTime[2] = 20;
        boilTime[3] = 10;
        boilNum = 4;
        break; 
    case 2:
    //LAGER - WAISS
        lcd.clear();
        lcd.setCursor( 0, 0 );
        lcd.print("LAGER - WEISS");
        lcd.setCursor( 0, 1 );
        lcd.print("M: 52 62 68 78");
        mashTemp[0] = 50.0;
        mashTemp[1] = 52.0;
        mashTemp[2] = 62.0;
        mashTemp[3] = 68.0;
        mashTemp[4] = 78.0;
        mashTime[0] = 0;
        mashTime[1] = 15;
        mashTime[2] = 15;
        mashTime[3] = 30;
        mashTime[4] = 15;
        mashNum = 5;
        boilTemp[0] = 100.0;
        boilTemp[1] = 100.0;
        boilTemp[2] = 100.0;
        boilTime[0] = 0;
        boilTime[1] = 50;
        boilTime[2] = 10;
        boilNum = 3;
        break;
    case 3:
    //PILSNER
        lcd.clear();
        lcd.setCursor( 0, 0 );
        lcd.print("PILSNER");
        lcd.setCursor( 0, 1 );
        lcd.print("M: 52 65 70 78");
        mashTemp[0] = 45.0;
        mashTemp[1] = 52.0;
        mashTemp[2] = 65.0;
        mashTemp[3] = 70.0;
        mashTemp[4] = 78.0;
        mashTime[0] = 0;
        mashTime[1] = 15;
        mashTime[2] = 30;
        mashTime[3] = 15;
        mashTime[4] = 15;
        mashNum = 5;
        boilTemp[0] = 100.0;
        boilTemp[1] = 100.0;
        boilTemp[2] = 100.0;
        boilTemp[2] = 100.0;
        boilTime[0] = 0;
        boilTime[1] = 30;
        boilTime[2] = 20;
        boilTime[3] = 10;
        boilNum = 4;
        break;
   }
}

/*
INFO
*/

void infoPrg()
{
  lcd.clear();              
  int c;
  for(c=0;c<mashNum;c++){
    lcd.setCursor( 0, 0 );    
    lcd.print( pd[1]);
    lcd.print(" S:");
    lcd.print(c);
    lcd.setCursor( 0, 1 );
    lcd.print(mashTemp[c]);
    lcd.print("/");
    lcd.print(mashTime[c]);
    lcd.print("min");
    delay(2000);
  }
  
  lcd.clear(); 
  for(c=0;c<boilNum;c++){
    lcd.setCursor( 0, 0 );    
    lcd.print( pd[2]);
    lcd.print(" S:");
    lcd.print(c);
    lcd.setCursor( 0, 1 );    
    lcd.print(boilTemp[c]);
    lcd.print("/");
    lcd.print(boilTime[c]);
    lcd.print("min");
    delay(2000);
  }
  
  lcd.clear(); 
  lcd.setCursor( 0, 0 );    
  lcd.print( pd[3]);
  lcd.setCursor( 0, 1 );    
  lcd.print(fermenterlTemp - tempTollerance);
  lcd.print("/");
  lcd.print(fermenterhTemp + tempTollerance);
  delay(2000);
  
  countButton1 = 0;
  countButton2 = 0;
}




/*

MASH

*/

void mashPrg()
{
  
  if(mashTemp[0] == 0){
    countButton1 = 0;
    countButton2 = 0;
    return;
  }
  float temp = readTemp();
  sendStatusTempToLCD(state,temp);
  mash_boil_Go(temp,state,mashTemp[state],mashTime[state],mashNum);
}


/*

BOIL

*/


void boilPrg()
{
  if(mashTemp[0] == 0){
    countButton1 = 0;
    countButton2 = 0;
    return;
  }
  float temp = readTemp();
  sendStatusTempToLCD(state,temp);
  mash_boil_Go(temp,state,boilTemp[state],boilTime[state],boilNum);
}

/*
fermenterPrg
*/

void fermenterPrg()
{
  float temp = readTemp(); 

  if( temp >= fermenterhTemp){
        digitalWrite(COOLER, HIGH);
        digitalWrite(HEATER, LOW);
        i=1;
  }
  
  else if( temp <= fermenterlTemp){
        digitalWrite(COOLER, LOW);
        digitalWrite(HEATER, HIGH);
        i=2;
  }
  
  else {
        digitalWrite(COOLER,LOW);
        digitalWrite(HEATER,LOW);
        i=0;
  }
  
  sendStatusTempToLCD(0,temp);
  sendStatusToLCD(i);
  
}



/* Legge la temperatura */
float readTemp()
{
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0); 
}

/* Invia la temperatura su un LCD (modo 4bit) */
void sendStatusTempToLCD(int s, float temp)
{
  lcd.clear();               
  lcd.setCursor( 0, 0 );
  lcd.print(pd[countButton1]);
  lcd.print( " S:");
  lcd.print( s );
  lcd.print( " T:");
  lcd.print( temp );
}

void sendStatusToLCD(int i)
{
  lcd.setCursor( 0, 1 );    
  lcd.print(p[i]);
}

void sendProgToLCD(int p)
{
  lcd.clear();               
  lcd.setCursor( 0, 0 );    
  lcd.print( "PROG: ");
  lcd.print(pd[p]);
}

void sendStartToLCD(int k, int j)
{
  lcd.clear();              
  lcd.setCursor( 0, 0 );    
  lcd.print( ss[k]);
  lcd.setCursor( 0, 1 );    
  lcd.print(pd[j]);
}

void sendTimeToLCD(int h,int m,int s)
{
  lcd.setCursor( 0, 1 );
  lcd.print(h);
  lcd.print(":");
  lcd.print(m);
  lcd.print(":");
  lcd.print(s);
}


void sendEndToLCD()
{
  lcd.setCursor( 0, 1 );
  lcd.print("END - END - END");
}

int displayTimer()
{
 timerCurrentMillis = millis();
 unsigned long time = (timerCurrentMillis - timerStartMillis)/1000;
 int hours = numberOfHours(time);
 int minutes = numberOfMinutes(time);
 int seconds = numberOfSeconds(time);
 sendTimeToLCD(hours,minutes,seconds); 
 return minutes;
}

void mash_boil_Go(float temp,int s, float t, int m, int num){
  
  switch (s){
    case 0:
      if(temp >= t){
        digitalWrite(HEATER,LOW);
        state = s + 1;
        delay(5000);
      }else{
       digitalWrite(HEATER,HIGH);   
      }
      break;  
    default:    
      if(temp >= (t - tempTollerance)){
        digitalWrite(HEATER,LOW); 
        if(timerStatus == 0){
          timerStartMillis = millis();  
          timerStatus = 1;
        } 
      }else{
        digitalWrite(HEATER,HIGH); 
      }
    
      if(timerStatus == 1) {
        int minutes = displayTimer();
      
        if(minutes == m){
          state = s + 1;
          if(state == num){
            sendEndToLCD();
            delay(5000);
            countButton1 = 0;
            countButton2 = 0;
          }else{
            timerStatus = 0;
          }
        }
      }
      break;
  }
}
