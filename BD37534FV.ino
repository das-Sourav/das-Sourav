#define ENC_BUTTON       10
#define IN_BUTTON        2
#define SET_BUTTON       3
#define POWER_BUTTON     4
#define STANDBY_OUT     13
#define MUTE_BUTTON      5

#define IR_POWER    0x807F00FF
#define IR_MUTE     0x807F609F
#define IR_UP       0x807FA05F
#define IR_DW       0x807F20DF
#define IR_MENU     0x807F807F
#define IR_INPUT    0x807F22DD
#define IR_SET      0x807F906F

#include <Wire.h> 
#include <OLED_I2C.h>           
#include <Encoder.h>               
#include <EEPROM.h>
#include <MsTimer2.h>                  
#include <boarddefs.h>          
#include <IRremote.h>           
#include <BD37534FV.h>
 BD37534FV tda; 
 OLED  myOLED(SDA, SCL, 8);
 extern uint8_t SmallFont[],BigNumbers[];
 IRrecv irrecv(12); // 
 Encoder myEnc(9, 8);// DT, CLK
 decode_results ir; 
 long times,oldPosition  = -999,newPosition;
 int menu,vol_reg,in_reg,treb_reg,bass_reg,mid_reg,gain0,gain1,gain2,gain3;
 bool w2,w,www,gr1,gr2,power,mute;
 int f_treb,q_treb,f_mid,q_mid,f_bass,q_bass,set=0,f_sub,f_lon,gain_lon;
 int gain_lf,gain_rf,gain_lr,gain_rr,gain_sub,faza_sub;

void setup(){
  Wire.begin();
  //Wire.setClock(400000L);
  irrecv.enableIRIn();
  myOLED.begin();
  myOLED.setBrightness(100);
  //Serial.begin(9600);
  pinMode(ENC_BUTTON,INPUT);        
  pinMode(IN_BUTTON,INPUT_PULLUP); // КНОПКА INPUT
  pinMode(SET_BUTTON,INPUT_PULLUP); // КНОПКА SET MENU2
  pinMode(MUTE_BUTTON,INPUT_PULLUP);  // КНОПКА MUTE 
  pinMode(STANDBY_OUT,OUTPUT);       // ВЫХОД УПРАВЛЕНИЯ STANDBY
  pinMode(POWER_BUTTON,INPUT_PULLUP); // КНОПКА POWER
 
  MsTimer2::set(3, to_Timer);MsTimer2::start();
  if(EEPROM.read(100)!=0){for(int i=0;i<101;i++){EEPROM.update(i,0);}}// очистка памяти при первом включении  
  myOLED.clrScr();
  myOLED.setFont(SmallFont);
  myOLED.print(F("SMART TECHNOLOGY"), CENTER, 10);
  myOLED.print(F("BD37534FV"), CENTER, 30);
  myOLED.update();
  delay(2000);
  vol_reg = EEPROM.read(0)-79;in_reg = EEPROM.read(1);gain1 = EEPROM.read(2);gain2 = EEPROM.read(3);gain3 = EEPROM.read(4);
  treb_reg = EEPROM.read(5)-20;f_treb = EEPROM.read(6);q_treb = EEPROM.read(7);
  mid_reg = EEPROM.read(8)-20;f_mid = EEPROM.read(9);q_mid = EEPROM.read(10);
  bass_reg = EEPROM.read(11)-20;f_bass = EEPROM.read(12);q_bass = EEPROM.read(13);f_sub = EEPROM.read(14);
  f_lon = EEPROM.read(15);gain_lon = EEPROM.read(16);
  gain_lf = EEPROM.read(17)-79;gain_rf = EEPROM.read(18)-79;gain_lr = EEPROM.read(19)-79;gain_rr = EEPROM.read(20)-79;gain_sub = EEPROM.read(21)-79;
  faza_sub = EEPROM.read(22);

  switch(in_reg){
   case 0: gain0 = gain1;break;
   case 1: gain0 = gain2;break;
   case 2: gain0 = gain3;break;
   }
  } 

void to_Timer(){newPosition = myEnc.read()/4;}   

void loop(){
/// IR ////////////////////////////////////////
   if ( irrecv.decode( &ir )) {//Serial.print("0x");Serial.println( ir.value,HEX);
   irrecv.resume();times=millis();w=1;w2=1;}
   if(ir.value==0){gr1=0;gr2=0;}// запрет нажатий не активных кнопок пульта 
   
//////// BUTTON //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if((digitalRead(POWER_BUTTON)==LOW || ir.value==IR_POWER)&& power==0){power=1;mute=1;gr1=0;gr2=0;cl();
    myOLED.setFont(SmallFont);myOLED.print(F("POWER OFF"), CENTER, 10);myOLED.update();delay(1500);cl();menu=100;myOLED.setBrightness(0);}
  if((digitalRead(POWER_BUTTON)==LOW || ir.value==IR_POWER)&& power==1){power=0;mute=0;myOLED.setBrightness(100);gr1=0;gr2=0;times=millis();w=1;w2=1;
    myOLED.setFont(SmallFont);myOLED.print(F("POWER ON"), CENTER, 10);myOLED.update();delay(1500);cl();menu=0;}  

if(power==0 && mute==0){  
  if(digitalRead(SET_BUTTON)==LOW || ir.value==IR_SET){set=1;gr1=0;gr2=0;cl();delay(200);times=millis();w=1;w2=1;
    myOLED.setFont(SmallFont);myOLED.print(F("SETTING MENU"), CENTER, 10);myOLED.update();delay(1500);cl();menu=10;
  }
  if((digitalRead(ENC_BUTTON)==LOW || ir.value==IR_MENU) && set==0){menu++;gr1=0;gr2=0;cl();delay(200);times=millis();w=1;w2=1;if(menu>3){menu=0;}}
  if((digitalRead(ENC_BUTTON)==LOW || ir.value==IR_MENU) && set==1){menu++;gr1=0;gr2=0;cl();delay(200);times=millis();w=1;w2=1;if(menu>24){menu=10;}}
  
  if(digitalRead(IN_BUTTON)==LOW || ir.value==IR_INPUT){in_reg++;menu=5;cl();times=millis();w=1;w2=1;www=1;if(in_reg>2){in_reg=0;}} 
}
if(power==0){
  if((digitalRead(MUTE_BUTTON)==LOW || ir.value==IR_MUTE)&& mute==0){mute=1;gr1=0;gr2=0;cl();
    myOLED.setFont(SmallFont);myOLED.print(F("MUTE"), CENTER, 10);myOLED.update();cl();menu=100;}
  if((digitalRead(MUTE_BUTTON)==LOW  || ir.value==IR_MUTE)&& mute==1){mute=0;gr1=0;gr2=0;times=millis();w=1;w2=1;cl();menu=0;}  
}
if(power==1){digitalWrite(STANDBY_OUT, HIGH);}else{digitalWrite(STANDBY_OUT, LOW);} 
//////// VOLUME //////////////////////////////////////////////////////////////////////////////////////////////////////////////  
 if(menu==0){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();vol_reg=vol_reg+newPosition;low_low();vol_func();audio();}

   if(ir.value==IR_UP){vol_reg++;ir_low();vol_func();audio();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){vol_reg++;ir_low2();vol_func();audio();}// кнопка >>>>>>
   if(ir.value==IR_DW){vol_reg--;ir_low1();vol_func();audio();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){vol_reg--;ir_low3();vol_func();audio();}// кнопка <<<<<<
 
  if(w==1){w=0;
  myOLED.clrScr();
  myOLED.setFont(SmallFont);myOLED.print(F("VOLUME"), LEFT, 0);myOLED.setFont(BigNumbers);myOLED.printNumI(vol_reg+79, 75, 0);
  myOLED.setFont(SmallFont);myOLED.print(F("IN"), LEFT, 10);
  if(in_reg==0){myOLED.print(F("BT"), 30, 10);}
  if(in_reg==1){myOLED.print(F("XLR"), 30, 10);}
  if(in_reg==2){myOLED.print(F("RCA"), 30, 10);}
  //myOLED.printNumI(in_reg+1, 30, 10);
  myOLED.print(F("GAIN"), LEFT, 20);myOLED.printNumI(gain0, 30, 20);
  myOLED.drawLine(0, 30, 128, 30);
  myOLED.setFont(SmallFont);myOLED.print(F("TREBLE"), LEFT, 35);myOLED.printNumI(treb_reg, 75, 35);
  myOLED.setFont(SmallFont);myOLED.print(F("MIDDLE"), LEFT, 45);myOLED.printNumI(mid_reg, 75, 45);
  myOLED.setFont(SmallFont);myOLED.print(F("BASS"), LEFT, 55);myOLED.printNumI(bass_reg, 75, 55);
  myOLED.update();
  }}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//////// TREBLE //////////////////////////////////////////////////////////////////////////////////////////////////////////////  
 if(menu==1){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();treb_reg=treb_reg+newPosition;low_low();treb_func();audio();}

   if(ir.value==IR_UP){treb_reg++;ir_low();treb_func();audio();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){treb_reg++;ir_low2();treb_func();audio();}// кнопка >>>>>>
   if(ir.value==IR_DW){treb_reg--;ir_low1();treb_func();audio();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){treb_reg--;ir_low3();treb_func();audio();}// кнопка <<<<<<
 
  if(w==1){w=0;
  myOLED.clrScr();
  myOLED.setFont(SmallFont);myOLED.print(F("TREBLE"), LEFT, 0);myOLED.setFont(BigNumbers);myOLED.printNumI(treb_reg, 75, 0);
  myOLED.setFont(SmallFont);
  switch(f_treb){
  case 0:myOLED.print(F("F 7.5 kHz"), LEFT, 10);break;
  case 1:myOLED.print(F("F 10  kHz"), LEFT, 10);break;
  case 2:myOLED.print(F("F 12.5kHz"), LEFT, 10);break;
  case 3:myOLED.print(F("F 15  kHz"), LEFT, 10);break;
  }
  myOLED.setFont(SmallFont);
  switch(q_treb){
  case 0: myOLED.print(F("Q 0.75"), LEFT, 20);break;
  case 1: myOLED.print(F("Q 1.25"), LEFT, 20);break;
  }
  myOLED.drawLine(0, 30, 128, 30);
  myOLED.setFont(SmallFont);
  myOLED.print(F("MIDDLE"), LEFT, 35);myOLED.printNumI(mid_reg, 75, 35);
  myOLED.print(F("BASS"), LEFT, 45);myOLED.printNumI(bass_reg, 75, 45);
  myOLED.print(F("VOLUME"), LEFT, 55);myOLED.printNumI(vol_reg+79, 75, 55);
  myOLED.update();
  }}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//////// MIDDLE //////////////////////////////////////////////////////////////////////////////////////////////////////////////  
 if(menu==2){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();mid_reg=mid_reg+newPosition;low_low();mid_func();audio();}

   if(ir.value==IR_UP){mid_reg++;ir_low();mid_func();audio();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){mid_reg++;ir_low2();mid_func();audio();}// кнопка >>>>>>
   if(ir.value==IR_DW){mid_reg--;ir_low1();mid_func();audio();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){mid_reg--;ir_low3();mid_func();audio();}// кнопка <<<<<<
 
  if(w==1){w=0;
  myOLED.clrScr();
  myOLED.setFont(SmallFont);myOLED.print(F("MIDDLE"), LEFT, 0);myOLED.setFont(BigNumbers);myOLED.printNumI(mid_reg, 75, 0);
  myOLED.setFont(SmallFont);
  switch(f_mid){
  case 0:myOLED.print(F("F 500 Hz"), LEFT, 10);break;
  case 1:myOLED.print(F("F 1  kHz"), LEFT, 10);break;
  case 2:myOLED.print(F("F 1.5kHz"), LEFT, 10);break;
  case 3:myOLED.print(F("F 2.5kHz"), LEFT, 10);break;
  }
  myOLED.setFont(SmallFont);
  switch(q_mid){
  case 0: myOLED.print(F("Q 0.75"), LEFT, 20);break;
  case 1: myOLED.print(F("Q 1.00"), LEFT, 20);break;
  case 2: myOLED.print(F("Q 1.25"), LEFT, 20);break;
  case 3: myOLED.print(F("Q 1.50"), LEFT, 20);break;
  }
  myOLED.drawLine(0, 30, 128, 30);
  myOLED.setFont(SmallFont);
  myOLED.print(F("BASS"), LEFT, 35);myOLED.printNumI(bass_reg, 75, 35);
  myOLED.print(F("VOLUME"), LEFT, 45);myOLED.printNumI(vol_reg+79, 75, 45);
  myOLED.print(F("TERBLE"), LEFT, 55);myOLED.printNumI(treb_reg, 75, 55);
  myOLED.update();
  }}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

//////// BASS //////////////////////////////////////////////////////////////////////////////////////////////////////////////  
 if(menu==3){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();bass_reg=bass_reg+newPosition;low_low();bass_func();audio();}

   if(ir.value==IR_UP){bass_reg++;ir_low();bass_func();audio();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){bass_reg++;ir_low2();bass_func();audio();}// кнопка >>>>>>
   if(ir.value==IR_DW){bass_reg--;ir_low1();bass_func();audio();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){bass_reg--;ir_low3();bass_func();audio();}// кнопка <<<<<<
 
  if(w==1){w=0;
  myOLED.clrScr();
  myOLED.setFont(SmallFont);myOLED.print(F("BASS"), LEFT, 0);myOLED.setFont(BigNumbers);myOLED.printNumI(bass_reg, 75, 0);
  myOLED.setFont(SmallFont);
  switch(f_bass){
  case 0:myOLED.print(F("F 60 Hz"), LEFT, 10);break;
  case 1:myOLED.print(F("F 80 Hz"), LEFT, 10);break;
  case 2:myOLED.print(F("F 100kHz"), LEFT, 10);break;
  case 3:myOLED.print(F("F 120kHz"), LEFT, 10);break;
  }
  switch(q_bass){
  case 0: myOLED.print(F("Q 0.50"), LEFT, 20);break;
  case 1: myOLED.print(F("Q 1.00"), LEFT, 20);break;
  case 2: myOLED.print(F("Q 1.50"), LEFT, 20);break;
  case 3: myOLED.print(F("Q 2.00"), LEFT, 20);break;
  }
  myOLED.drawLine(0, 30, 128, 30);
  myOLED.setFont(SmallFont);
  myOLED.print(F("VOLUME"), LEFT, 35);myOLED.printNumI(vol_reg+79, 75, 35);
  myOLED.print(F("TREBLE"), LEFT, 45);myOLED.printNumI(treb_reg, 75, 45);
  myOLED.print(F("MIDDLE"), LEFT, 55);myOLED.printNumI(mid_reg, 75, 55);
  myOLED.update();
  }}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

////////////// INPUT GAIN ///////////////////////////////////////////////////////////////////
 if(menu==5){
  switch(in_reg){
     case 0: gain0 = gain1;break;
     case 1: gain0 = gain2;break;
     case 2: gain0 = gain3;break;
     }
   if(ir.value==IR_UP){gain0++;ir_low();gain_func();audio();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){gain0++;ir_low2();gain_func();audio();}// кнопка >>>>>>
   if(ir.value==IR_DW){gain0--;ir_low1();gain_func();audio();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){gain0--;ir_low3();gain_func();audio();}// кнопка <<<<<<
 
  if (newPosition != oldPosition){oldPosition = newPosition;enc_err();gain0=gain0+newPosition;myEnc.write(0);newPosition=0;times=millis();www=1;w=1;w2=1;gain_func();}
  switch(in_reg){
     case 0: gain1 = gain0;break;
     case 1: gain2 = gain0;break;
     case 2: gain3 = gain0;break;
     }  
  if(www==1){audio();www=0;}
 
  if(w==1){w=0;
  myOLED.clrScr();
  myOLED.setFont(SmallFont);myOLED.print(F("INPUT"), LEFT, 0);//myOLED.setFont(BigNumbers);myOLED.printNumI(in_reg+1, 65, 0);
  myOLED.setFont(SmallFont);//myOLED.print(F("IN"), LEFT, 10);
  if(in_reg==0){myOLED.print(F("BT"), 65, 0);}
  if(in_reg==1){myOLED.print(F("XLR"), 65, 0);}
  if(in_reg==2){myOLED.print(F("RCA"), 65, 0);}
  myOLED.drawLine(0, 30, 128, 30);
  myOLED.setFont(SmallFont);myOLED.print(F("GAIN"), LEFT, 35);myOLED.setFont(BigNumbers);myOLED.printNumI(gain0, 65, 35);
  myOLED.update();
  }} 

////////////// SET F_TREB ///////////////////////////////////////////////////////////////////  
if(menu==10){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();f_treb=f_treb+newPosition;en_low();f_treb_func();audio();}

   if(ir.value==IR_UP){f_treb++;ir_low();f_treb_func();audio();}// кнопка > 
   if(ir.value==IR_DW){f_treb--;ir_low1();f_treb_func();audio();}// кнопка <
  
  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("Frequency TREBLE"), CENTER, 0);
  myOLED.setFont(BigNumbers);
  switch(f_treb){
  case 0:myOLED.print(F("7.50"), CENTER, 20);break;
  case 1:myOLED.print(F("10.0"), CENTER, 20);break;
  case 2:myOLED.print(F("12.5"), CENTER, 20);break;
  case 3:myOLED.print(F("15.0"), CENTER, 20);break;
  }
  myOLED.setFont(SmallFont);myOLED.print(F("kHz"), CENTER, 50);
  myOLED.update();}
  }
////////////// SET F_MID ///////////////////////////////////////////////////////////////////  
if(menu==11){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();f_mid=f_mid+newPosition;low_low();f_mid_func();audio();}

   if(ir.value==IR_UP){f_mid++;ir_low();f_mid_func();audio();}// кнопка > 
   if(ir.value==IR_DW){f_mid--;ir_low1();f_mid_func();audio();}// кнопка <
  
  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("Frequency MIDDLE"), CENTER, 0);
  myOLED.setFont(BigNumbers);
  switch(f_mid){
  case 0:myOLED.print(F("0.5"), CENTER, 20);break;
  case 1:myOLED.print(F("1.0"), CENTER, 20);break;
  case 2:myOLED.print(F("1.5"), CENTER, 20);break;
  case 3:myOLED.print(F("2.5"), CENTER, 20);break;
  }
  myOLED.setFont(SmallFont);myOLED.print(F("kHz"), CENTER, 50);
  myOLED.update();}
  }  
////////////// SET F_BASS ///////////////////////////////////////////////////////////////////  
if(menu==12){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();f_bass=f_bass+newPosition;en_low();f_bass_func();audio();}

   if(ir.value==IR_UP){f_bass++;ir_low();f_bass_func();audio();}// кнопка > 
   if(ir.value==IR_DW){f_bass--;ir_low1();f_bass_func();audio();}// кнопка <
  
  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("Frequency BASS"), CENTER, 0);
  myOLED.setFont(BigNumbers);
  switch(f_bass){
  case 0:myOLED.print(F("60"), CENTER, 20);break;
  case 1:myOLED.print(F("80"), CENTER, 20);break;
  case 2:myOLED.print(F("100"), CENTER, 20);break;
  case 3:myOLED.print(F("120"), CENTER, 20);break;
  }
  myOLED.setFont(SmallFont);myOLED.print(F("Hz"), CENTER, 50);
  myOLED.update();}
  }   
////////////// SET F_SUB ///////////////////////////////////////////////////////////////////  
if(menu==13){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();f_sub=f_sub+newPosition;en_low();f_sub_func();audio();}

   if(ir.value==IR_UP){f_sub++;ir_low();f_sub_func();audio();}// кнопка > 
   if(ir.value==IR_DW){f_sub--;ir_low1();f_sub_func();audio();}// кнопка <
  
  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("Frequency SUBWOOFER"), CENTER, 0);
  switch(f_sub){
  case 0:myOLED.setFont(SmallFont);myOLED.print(F("OFF"), CENTER, 20);break;
  case 1:myOLED.setFont(BigNumbers);myOLED.print(F("55"), CENTER, 20);break;
  case 2:myOLED.setFont(BigNumbers);myOLED.print(F("85"), CENTER, 20);break;
  case 3:myOLED.setFont(BigNumbers);myOLED.print(F("120"), CENTER, 20);break;
  case 4:myOLED.setFont(BigNumbers);myOLED.print(F("160"), CENTER, 20);break;
  }
  if(f_sub>0){myOLED.setFont(SmallFont);myOLED.print(F("Hz"), CENTER, 50);}
  myOLED.update();}
  }   
////////////// SET F_LOUDNESS ///////////////////////////////////////////////////////////////////  
if(menu==14){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();f_lon=f_lon+newPosition;en_low();f_lon_func();audio();}

  if(ir.value==IR_UP){f_lon++;ir_low();f_lon_func();audio();}// кнопка > 
  if(ir.value==IR_DW){f_lon--;ir_low1();f_lon_func();audio();}// кнопка <

  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("Frequency LOUDNESS"), CENTER, 0);
  switch(f_lon){
  case 0:myOLED.setFont(BigNumbers);myOLED.print(F("250"), CENTER, 20);break;
  case 1:myOLED.setFont(BigNumbers);myOLED.print(F("400"), CENTER, 20);break;
  case 2:myOLED.setFont(BigNumbers);myOLED.print(F("800"), CENTER, 20);break;
  case 3:myOLED.setFont(SmallFont);myOLED.print(F("Prohibition"), CENTER, 20);break;
  }
  if(f_lon<3){myOLED.setFont(SmallFont);myOLED.print(F("Hz"), CENTER, 50);}
  myOLED.update();}
  }     
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
////////////// SET Q_TREB ///////////////////////////////////////////////////////////////////  
if(menu==15){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();q_treb=q_treb+newPosition;low_low();q_treb_func();audio();}

  if(ir.value==IR_UP){q_treb++;ir_low();q_treb_func();audio();}// кнопка > 
  if(ir.value==IR_DW){q_treb--;ir_low1();q_treb_func();audio();}// кнопка <
  
  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("Q TREBLE"), CENTER, 0);
  myOLED.setFont(BigNumbers);
  switch(q_treb){
  case 0: myOLED.print(F("0.75"), CENTER, 20);break;
  case 1: myOLED.print(F("1.25"), CENTER, 20);break;
  }
  myOLED.update();}
  }  
////////////// SET Q_MIDDLE ///////////////////////////////////////////////////////////////////  
if(menu==16){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();q_mid=q_mid+newPosition;low_low();q_mid_func();audio();}

  if(ir.value==IR_UP){q_mid++;ir_low();q_mid_func();audio();}// кнопка > 
  if(ir.value==IR_DW){q_mid--;ir_low1();q_mid_func();audio();}// кнопка <
  
  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("Q MIDDLE"), CENTER, 0);
  myOLED.setFont(BigNumbers);
  switch(q_mid){
  case 0:myOLED.print(F("0.75"), CENTER, 20);break;
  case 1:myOLED.print(F("1.00"), CENTER, 20);break;
  case 2:myOLED.print(F("1.25"), CENTER, 20);break;
  case 3:myOLED.print(F("1.50"), CENTER, 20);break;
  }
  myOLED.update();}
  }   
////////////// SET Q_BASS ///////////////////////////////////////////////////////////////////  
if(menu==17){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();q_bass=q_bass+newPosition;low_low();q_bass_func();audio();}

  if(ir.value==IR_UP){q_bass++;ir_low();q_bass_func();audio();}// кнопка > 
  if(ir.value==IR_DW){q_bass--;ir_low1();q_bass_func();audio();}// кнопка <
  
  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("Q BASS"), CENTER, 0);
  myOLED.setFont(BigNumbers);
  switch(q_bass){
  case 0: myOLED.print(F("0.50"), CENTER, 20);break;
  case 1: myOLED.print(F("1.00"), CENTER, 20);break;
  case 2: myOLED.print(F("1.50"), CENTER, 20);break;
  case 3: myOLED.print(F("2.00"), CENTER, 20);break;
  }
  myOLED.update();}
  } 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////// SET GAIN LOUDNESS ///////////////////////////////////////////////////////////////////  
if(menu==18){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();gain_lon=gain_lon+newPosition;en_low();gain_lon_func();audio();}

   if(ir.value==IR_UP){gain_lon++;ir_low();gain_lon_func();audio();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){gain_lon++;ir_low2();gain_lon_func();audio();}// кнопка >>>>>>
   if(ir.value==IR_DW){gain_lon--;ir_low1();gain_lon_func();audio();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){gain_lon--;ir_low3();gain_lon_func();audio();}// кнопка <<<<<<
  
  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("GAIN LOUDNESS"), CENTER, 0);
  myOLED.setFont(BigNumbers);myOLED.printNumI(gain_lon, CENTER, 20);
  myOLED.setFont(SmallFont);myOLED.print(F("dB"), CENTER, 50);
  myOLED.update();}
  }       
////////////// SET GAIN LEFT FRONT ///////////////////////////////////////////////////////////////////  
if(menu==19){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();gain_lf=gain_lf+newPosition;en_low();gain_lf_func();audio();}

   if(ir.value==IR_UP){gain_lf++;ir_low();gain_lf_func();audio();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){gain_lf++;ir_low2();gain_lf_func();audio();}// кнопка >>>>>>
   if(ir.value==IR_DW){gain_lf--;ir_low1();gain_lf_func();audio();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){gain_lf--;ir_low3();gain_lf_func();audio();}// кнопка <<<<<<
  
  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("GAIN LEFT FRONT"), CENTER, 0);
  myOLED.setFont(BigNumbers);myOLED.printNumI(gain_lf, CENTER, 20);
  myOLED.setFont(SmallFont);myOLED.print(F("dB"), CENTER, 50);
  myOLED.update();}
  }  
////////////// SET GAIN RIGHT FRONT ///////////////////////////////////////////////////////////////////  
if(menu==20){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();gain_rf=gain_rf+newPosition;en_low();gain_rf_func();audio();}

   if(ir.value==IR_UP){gain_rf++;ir_low();gain_rf_func();audio();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){gain_rf++;ir_low2();gain_rf_func();audio();}// кнопка >>>>>>
   if(ir.value==IR_DW){gain_rf--;ir_low1();gain_rf_func();audio();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){gain_rf--;ir_low3();gain_rf_func();audio();}// кнопка <<<<<<
  
  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("GAIN RIGHT FRONT"), CENTER, 0);
  myOLED.setFont(BigNumbers);myOLED.printNumI(gain_rf, CENTER, 20);
  myOLED.setFont(SmallFont);myOLED.print(F("dB"), CENTER, 50);
  myOLED.update();}
  }  
////////////// SET GAIN LEFT REAR ///////////////////////////////////////////////////////////////////  
if(menu==21){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();gain_lr=gain_lr+newPosition;en_low();gain_lr_func();audio();}

   if(ir.value==IR_UP){gain_lr++;ir_low();gain_lr_func();audio();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){gain_lr++;ir_low2();gain_lr_func();audio();}// кнопка >>>>>>
   if(ir.value==IR_DW){gain_lr--;ir_low1();gain_lr_func();audio();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){gain_lr--;ir_low3();gain_lr_func();audio();}// кнопка <<<<<<
  
  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("GAIN LEFT REAR"), CENTER, 0);
  myOLED.setFont(BigNumbers);myOLED.printNumI(gain_lr, CENTER, 20);
  myOLED.setFont(SmallFont);myOLED.print(F("dB"), CENTER, 50);
  myOLED.update();}
  }  
////////////// SET GAIN RIGHT REAR ///////////////////////////////////////////////////////////////////  
if(menu==22){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();gain_rr=gain_rr+newPosition;en_low();gain_rr_func();audio();}

   if(ir.value==IR_UP){gain_rr++;ir_low();gain_rr_func();audio();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){gain_rr++;ir_low2();gain_rr_func();audio();}// кнопка >>>>>>
   if(ir.value==IR_DW){gain_rr--;ir_low1();gain_rr_func();audio();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){gain_rr--;ir_low3();gain_rr_func();audio();}// кнопка <<<<<<
  
  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("GAIN RIGHT REAR"), CENTER, 0);
  myOLED.setFont(BigNumbers);myOLED.printNumI(gain_rr, CENTER, 20);
  myOLED.setFont(SmallFont);myOLED.print(F("dB"), CENTER, 50);
  myOLED.update();}
  }   
////////////// SET GAIN SUBWOOFER ///////////////////////////////////////////////////////////////////  
if(menu==23){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();gain_sub=gain_sub+newPosition;en_low();gain_sub_func();audio();}

   if(ir.value==IR_UP){gain_sub++;ir_low();gain_sub_func();audio();}// кнопка > 
   if(ir.value==0xFFFFFFFF and gr1==1){gain_sub++;ir_low2();gain_sub_func();audio();}// кнопка >>>>>>
   if(ir.value==IR_DW){gain_sub--;ir_low1();gain_sub_func();audio();}// кнопка <
   if(ir.value==0xFFFFFFFF and gr2==1){gain_sub--;ir_low3();gain_sub_func();audio();}// кнопка <<<<<<
  
  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("GAIN SUBWOOFER"), CENTER, 0);
  myOLED.setFont(BigNumbers);myOLED.printNumI(gain_sub, CENTER, 20);
  myOLED.setFont(SmallFont);myOLED.print(F("dB"), CENTER, 50);
  myOLED.update();}
  }    

////////////// SET FAZA SUBWOOFER ///////////////////////////////////////////////////////////////////  
if(menu==24){
  if(newPosition != oldPosition){oldPosition = newPosition;enc_err();faza_sub=faza_sub+newPosition;en_low();faza_sub_func();audio();}

   if(ir.value==IR_UP){faza_sub++;ir_low();faza_sub_func();audio();}// кнопка > 
   if(ir.value==IR_DW){faza_sub--;ir_low1();faza_sub_func();audio();}// кнопка <
  
  if(w==1){w=0;
  myOLED.setFont(SmallFont);
  myOLED.print(F("PHASE SUBWOOFER"), CENTER, 0);
  myOLED.setFont(BigNumbers);myOLED.printNumI(faza_sub*180, CENTER, 20);
  myOLED.update();}
  }  
   
         

////////////////// EEPROM //////////////////////////////////////////////////////////////
 if(millis()-times>5000 && w2==1 && power==0 && mute==0){
     EEPROM.update(0,vol_reg+79);EEPROM.update(1,in_reg);EEPROM.update(2,gain1);EEPROM.update(3,gain2);EEPROM.update(4,gain3);
     EEPROM.update(5,treb_reg+20);EEPROM.update(6,f_treb);EEPROM.update(7,q_treb);
     EEPROM.update(8,mid_reg+20);EEPROM.update(9,f_mid);EEPROM.update(10,q_mid);
     EEPROM.update(11,bass_reg+20);EEPROM.update(12,f_bass);EEPROM.update(13,q_bass);EEPROM.update(14,f_sub);
     EEPROM.update(15,f_lon);EEPROM.update(16,gain_lon);
     EEPROM.update(17,gain_lf+79);EEPROM.update(18,gain_rf+79);EEPROM.update(19,gain_lr+79);EEPROM.update(20,gain_rr+79);EEPROM.update(21,gain_sub+79);
     EEPROM.update(22,faza_sub);
     cl();set=0;menu=0;w2=0;w=1;}        
} // loop end 


void low_low(){myEnc.write(0);newPosition=0;times=millis();w=1;w2=1;}
void en_low(){myEnc.write(0);newPosition=0;times=millis();myOLED.clrScr();w=1;w2=1;}
void ir_low3(){gr1=0;cl();times=millis();w=1;w2=1;}
void ir_low2(){gr2=0;cl();times=millis();w=1;w2=1;}
void ir_low1(){gr1=0;gr2=1;cl();times=millis();w=1;w2=1;}
void ir_low(){gr1=1;ir_low2();}
void faza_sub_func(){if(faza_sub<0){faza_sub=1;}if(faza_sub>1){faza_sub=0;}}
void gain_lf_func(){if(gain_lf<-79){gain_lf=-79;}if(gain_lf>15){gain_lf=15;}}
void gain_rf_func(){if(gain_rf<-79){gain_rf=-79;}if(gain_rf>15){gain_rf=15;}}
void gain_lr_func(){if(gain_lr<-79){gain_lr=-79;}if(gain_lr>15){gain_lr=15;}}
void gain_rr_func(){if(gain_rr<-79){gain_rr=-79;}if(gain_rr>15){gain_rr=15;}}
void gain_sub_func(){if(gain_sub<-79){gain_sub=-79;}if(gain_sub>15){gain_sub=15;}}
void gain_lon_func(){if(gain_lon<0){gain_lon=0;}if(gain_lon>20){gain_lon=20;}}
void f_lon_func(){if(f_lon<0){f_lon=0;}if(f_lon>3){f_lon=3;}}
void f_sub_func(){if(f_sub<0){f_sub=0;}if(f_sub>4){f_sub=4;}}
void f_treb_func(){if(f_treb<0){f_treb=0;}if(f_treb>3){f_treb=3;}}
void f_mid_func(){if(f_mid<0){f_mid=0;}if(f_mid>3){f_mid=3;}}
void f_bass_func(){if(f_bass<0){f_bass=0;}if(f_bass>3){f_bass=3;}}
void q_mid_func(){if(q_mid<0){q_mid=0;}if(q_mid>3){q_mid=3;}}
void q_bass_func(){if(q_bass<0){q_bass=0;}if(q_bass>3){q_bass=3;}}
void q_treb_func(){if(q_treb<0){q_treb=0;}if(q_treb>1){q_treb=1;}}
void bass_func(){if(bass_reg<-20){bass_reg=-20;}if(bass_reg>20){bass_reg=20;}}
void mid_func(){if(mid_reg<-20){mid_reg=-20;}if(mid_reg>20){mid_reg=20;}}
void treb_func(){if(treb_reg<-20){treb_reg=-20;}if(treb_reg>20){treb_reg=20;}}
void gain_func(){if(gain0<0){gain0=0;}if(gain0>20){gain0=20;}}  
void enc_err(){if(newPosition>1){newPosition=1;}if(newPosition<-1){newPosition=-1;}}
void vol_func(){if(vol_reg<-79){vol_reg=-79;}if(vol_reg>15){vol_reg=15;}}
void cl(){myOLED.clrScr();ir.value=0;delay(300);}
void audio(){
   tda.setSetup_1(1,2,0); // default (1,2,0)
   tda.setSetup_2(f_sub,0,0,faza_sub); 
                   // int sub_f ------ OFF 55Hz 85Hz 120Hz 160Hz = int 0...4
                   // int sub_out ---- LPF Front Rear Prohibition = int 0...3
                   // int level_metr - HOLD REST = int 0..1
                   // int faza ------- 0 180 = int 0...1
   tda.setLoudness_f(f_lon); // 250Hz 400Hz 800Hz Prohibition = int 0...3
   tda.setIn(in_reg); // 0...2
   tda.setIn_gain(gain0,mute); // in_gain -- 0...20 = 0...20 dB, mute -- 0 default 
   tda.setVol(vol_reg); // -79...+15 dB = int -79...15
   tda.setFront_1(gain_lf); // -79...+15 dB = int -79...15
   tda.setFront_2(gain_rf); // -79...+15 dB = int -79...15
   tda.setRear_1(gain_lr); // -79...+15 dB = int -79...15
   tda.setRear_2(gain_rr); // -79...+15 dB = int -79...15
   tda.setSub(gain_sub); // -79...+15 dB = int -79...15
   tda.mix();
   tda.setBass_setup(q_bass,f_bass); // 0.5 1.0 1.5 2.0 --- int 0...3, 60Hz 80Hz 100Hz 120Hz --- int 0...3
   tda.setMiddle_setup(q_mid,f_mid); // 0.75 1.0 1.25 1.5 --- int 0...3, 500Hz 1kHz 1.5kHz 2.5kHz --- int 0...3
   tda.setTreble_setup(q_treb,f_treb); // 0.75 1.25  --- int 0...1, 7.5kHz 10kHz 12.5kHz 15kHz --- int 0...3
   tda.setBass_gain(bass_reg); // --20 ... +20 dB = int -20 ... 20  
   tda.setMiddle_gain(mid_reg); // --20 ... +20 dB = int -20 ... 20
   tda.setTreble_gain(treb_reg); // --20 ... +20 dB = int -20 ... 20   
   tda.setLoudness_gain(gain_lon); // 0 ... 20 dB = int 0 ... 20
  }