

#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

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

float mashTemp[] = {50.0,52.0,62.0,68.0,78.0};
int mashTime[] = {15,15,30,15};
int mashState = 0;

int boilTime[] = {50,10};
const float boilTemp = 100.0;
int boilState = 0;

char* p[] = {"ALL OFF","COOLER ON","HEATER ON"}; 
char* pd[] = {"LC BEER","MASH","BOIL","FERM","INFO"}; 
char* ss[] = {"STOP","START"}; 
int d = 250;
int i = 0;

int countButton1 = 0;
int countButton2 = 0;
int valButton1 = 0;
int valButton2 = 0;

int timerStatus = 0;
unsigned long timerStartMillis = 0;
unsigned long timerCurrentMillis = 0;



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

  mashTemp[0] = mashTemp[0] - tempTollerance;
  mashTemp[1] = mashTemp[1] - tempTollerance;
  mashTemp[2] = mashTemp[2] - tempTollerance;
  mashTemp[3] = mashTemp[3] - tempTollerance;
  mashTemp[4] = mashTemp[4] - tempTollerance;
  
}

void loop()
{
  
  if(countButton2 == 0){
    
    digitalWrite(COOLER,LOW);
    digitalWrite(HEATER,LOW);
    timerStatus = 0;
    d = 250;
    i = 0;
    
    valButton1 = digitalRead(Button1);
    if(valButton1 == HIGH){
      countButton1++; 
    } 
    if(countButton1 == 5){
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
      case 4:
        d = 500;
        infoPrg();
        break;
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
    }
  }
    
  delay( d );
}

// END LOOP



/*
INFO
*/

void infoPrg()
{
  lcd.clear();              
  int c;
  for(c=0;c<4;c++){
    lcd.setCursor( 0, 0 );    
    lcd.print( pd[1]);
    lcd.print(" S:");
    lcd.print(c);
    lcd.setCursor( 0, 1 );
    lcd.print(mashTemp[c+1] + tempTollerance);
    lcd.print("/");
    lcd.print(mashTime[c]);
    lcd.print("min");
    delay(2000);
  }
  
  lcd.clear(); 
  for(c=0;c<2;c++){
    lcd.setCursor( 0, 0 );    
    lcd.print( pd[2]);
    lcd.print(" S:");
    lcd.print(c);
    lcd.setCursor( 0, 1 );    
    lcd.print(boilTemp);
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
  float temp = readTemp();
  sendStatusTempToLCD(mashState,temp);
  mashGo(temp,mashState);
}


/*

BOIL

*/


void boilPrg()
{
  float temp = readTemp();
  sendStatusTempToLCD(boilState,temp);
  
  
  if(temp > boilTemp){
    digitalWrite(HEATER,LOW);
    if(timerStatus == 0){
      timerStartMillis = millis();  
      timerStatus = 1;
    }
  }else{
    digitalWrite(HEATER,HIGH);
  }
  
  if(timerStatus == 1){
     int minutes = displayTimer();
    
     if(boilState == 0 && minutes == boilTime[0]){
       boilState = 1;
       timerStartMillis = millis();
       minutes = 0;
     }
     if(boilState == 1 && minutes >= boilTime[1]) {
       digitalWrite(HEATER,LOW);
       sendEndToLCD();
     }
  }
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




float readTemp()
{
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0); 
}


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

void mashGo(float temp,int s){
  
  switch (s){
    case 0:
      if(temp >= mashTemp[s]){
        digitalWrite(HEATER,LOW);
        mashState = s + 1;
        delay(5000);
      }else{
       digitalWrite(HEATER,HIGH);   
      }
      break;  
    default:
      digitalWrite(COOLER,HIGH);
      
      if(temp >= mashTemp[s]){
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
      
        if(minutes == mashTime[s-1]){
          if(s == 4){
             sendEndToLCD();
          }else{
            mashState = s + 1;
            timerStatus = 0;
          }
        }
      }
      break;
  }
}
