#include <LiquidCrystal.h>
#define DHT11Pin 2
//for LCD1602
//Vss -> GND
//Vdd -> 5V
//V0 -> 6 (use pin6 to control light of LCD since I don't have suitable resister)
//Rs -> 3
//Rw -> 4
//E -> 5
//D4 -> 10
//D5 -> 11
//D6 -> 12
//D7 -> 13
#define V0 6
#define Rs 3
#define Rw 4
#define En 5
#define D4 10
#define D5 11
#define D6 12
#define D7 13
//circle for unit of temp
byte circle[8]={
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
};

float OldResult[3]={1,1,1};
float Result[3]={0};//store new result that is after convert
int chr[40]={0};//store result that is before convert
LiquidCrystal lcd(Rs,Rw,En,D4,D5,D6,D7);//define lcd

void setup(){
  analogWrite(V0,2.5/5*255);
  lcd.begin(16,2);
  lcd.createChar(0,circle);
  lcd.setCursor(0,0);
  lcd.print("Waiting for");
  lcd.setCursor(0,1);
  lcd.print("collecting data");
}

void loop(){
  Measure(DHT11Pin);
  //if temp and humi reading is wrong, print wrong
  if ((Result[0]==0 && Result[1]==0 && Result[2]==0) || (Result[0]==1 && Result[1]==1 && Result[2]==1) || (Result[0]==1 && Result[1]==0 && Result[2]==1)){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Wrong");
  }else{
    //if reading is correct and the number is changed, print new number.
    //Otherwise, no change
    if(OldResult!=Result){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Humi : ");
      lcd.print(Result[0]);
      lcd.print(" %");
      lcd.setCursor(0,1);
      lcd.print("Temp : ");
      lcd.print(Result[1]);
      lcd.write(byte(0));
      lcd.print("C");
      for(int i=0;i<3;i++){
        OldResult[i]=Result[i];
      }
    }
  }
}


//Measure Temp and humi. And get tol.
//input is DHT11 pin number (digital pin)
void Measure(int Pin){
    delay(2000);
    //set the mode of pin to output
    pinMode(Pin,OUTPUT);
    //Start signal
    //low -> 20ms (>18ms)
    //high -> 40us (20us~40us)
    digitalWrite(Pin,LOW);
    delay(20);
    digitalWrite(Pin,HIGH);
    delayMicroseconds(40);
    //Set pin low to wait responds (set mode to input)
    digitalWrite(Pin,LOW);
    pinMode(Pin,INPUT);
    //responds: low -> 80us, high -> 80us
    //To avoid ignore data that is after the responds, use while loop to test responds
    int loopCount=1000;
    while(digitalRead(Pin)!=HIGH){
      if(loopCount--==0){
        Result[0]=0;
        Result[1]=0;
        Result[3]=0;
        return;
      }
    }
    loopCount=3000;
    while(digitalRead(Pin)!=LOW){
      if(loopCount--==0){
        Result[0]=1;
        Result[1]=1;
        Result[3]=1;
        return;
      }
    }
    //After get responds, begin to get data
    //low -> waiting
    //high -> If 26us~28us, it is "0". If 70us, it is "1". I use 50us as the threshold
    for(int i=0;i<40;i++){
      while(digitalRead(Pin)==LOW){}
      long time = micros();
      while(digitalRead(Pin)==HIGH){}
      if(micros()-time>50){
        chr[i]=1;
      }else{
        chr[i]=0;
      }
    }
    //Convert data to result
    //Result[0] -> humi
    //Result[1] -> Temp
    //Result[2] -> tol (if tol!=humi+Temp, result is wrong and Result=101)
    Result[0]=chr[0]*128+chr[1]*64+chr[2]*32+chr[3]*16+chr[4]*8+chr[5]*4+chr[6]*2+chr[7];
    Result[1]=chr[16]*128+chr[17]*64+chr[18]*32+chr[19]*16+chr[20]*8+chr[21]*4+chr[22]*2+chr[23];
    Result[2]=chr[32]*128+chr[33]*64+chr[34]*32+chr[35]*16+chr[36]*8+chr[37]*4+chr[38]*2+chr[39];
    if(Result[2]==Result[0]+Result[1]){
      Result[0]=round((chr[0]*128+chr[1]*64+chr[2]*32+chr[3]*16+chr[4]*8+chr[5]*4+chr[6]*2+chr[7]+chr[8]/2.00+chr[9]/4.00+chr[10]/8.00+chr[11]/16.00+chr[12]/32.00+chr[13]/64.00+chr[14]/128.00+chr[15]/256.00)*100)/100.00;
      Result[1]=round((chr[16]*128+chr[17]*64+chr[18]*32+chr[19]*16+chr[20]*8+chr[21]*4+chr[22]*2+chr[23]+chr[24]/2.00+chr[25]/4.00+chr[26]/8.00+chr[27]/16.00+chr[28]/32.00+chr[29]/64.00+chr[30]/128.00+chr[31]/256.00)*100)/100.00;
    }else{
      Result[0]=1;
      Result[1]=0;
      Result[2]=1;
    }
}
