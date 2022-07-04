#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>      

LiquidCrystal_I2C lcd(0x27,16,2); // 16x2 lik LCD icin I2C tanimlaniyor.
float mapf(const float& x,const float& in_min,const float& in_max,const float& out_min,const float& out_max);
int RX = 2;        // pin 2 3 (Bluetooth haberlesmesi icin UART pinleri)
int TX = 3;        // TX pini      
bool LOC = false; //Arduino once calistirildiysa veri gelmeden LCD'ye konum yazdirmaya calismasin diye flag.
String dataX,dataY; //Gelen verinin x ile y verisi olarak ayri tutacak stringler
String serialBuff, bt_serialBuff; //debug icin kullanilacak serial ile bluetooth serial haberlesmeleri icin bufferler

float mappedX; //Tabla degerleri 0 90 arasi ama -25 25 arasi map'lemek istiyoruz.
float mappedY; //Y verisini tutacak degisken
SoftwareSerial bluetooth(RX,TX);//kutuphane ile yeni UART pinleri tanimlama
void setup() {
  // put your setup code here, to run once:
bluetooth.begin(9600); //Bluetooth baslat
Serial.begin(9600); //Debug serial'i baslat
while (!Serial);
 lcd.begin();//lcd baslat
}

void loop() {
lcd.setCursor(0,0); //lcd baslangic konumuna getir.
if (dataX.toFloat()>101||dataY.toFloat()>101||(!LOC))//Top olmamasi 500-500 verisi geldiginde oluyor. O durum ile baslangic durumu kontrol ediliyor
{
  lcd.print("Top Bulunamadi");//Top bulunamamasi durumunu yazdirma.
  lcd.setCursor(0,1); //LCD alt satira indirme.
  lcd.print("          "); //Alt satir sildiriyor.
}
else
{  
  lcd.print("X = ");
  if(LOC)
  { 
    mappedX = map(dataX.toFloat(),-2,92,-25,25);
    //if (mappedX>=0)
      //lcd.print(" ");
    //mappedX = mapf(dataX.toFloat(),-2.0,92.0,-25.0,25.0); //-2 92 tabla verilerini -25 25 arasina map ediyor, float olarak.
    lcd.print(mappedX); // maplanmis veri LCD'ye yazdiriliyor
    lcd.print("        ");// Top Bulunamadi yazisindan kalanlari sildiriyor.
  }
  lcd.setCursor(0,1);//Alt satir
  lcd.print("Y = ");
  if(LOC)
  {
    mappedY = map(dataY.toFloat(),-2,92,-25,25);
    //if (mappedY>=0)
      //lcd.print(" ");
    //mappedY = mapf(dataY.toFloat(),-2.0,92.0,-25.0,25.0); //x ekseni ile ustteki durumun aynisi sadece y icin yapiliyor.
    lcd.print(mappedY);
    lcd.print("   ");
    //Serial.println("r = "+String(sqrt(pow(mappedX,2)+pow(mappedY,2))));//debug icin merkeze uzaklik kontrolu
  }
}

  while (bluetooth.available()) { //veri geliyorsa oku.
    bt_serialBuff += String((char)bluetooth.read()); //UART ile gelen uint-8 verilerini char'a cevir,
    //char verilerini de bufferle ayni tur olabilmesi icin string turune donustur. 
  }

  if (bt_serialBuff.length() > 0 ) {

    //Serial.println("Server response: " + bt_serialBuff);
    LOC = true; //Veri gelmeye basladi flagi.
    int index = bt_serialBuff.indexOf('-'); //gelen verideki '-' nin indexini bul
    dataX = bt_serialBuff.substring(0,index); //0'dan indexe kadar olan veri X verisi
    dataY = bt_serialBuff.substring(index+1,bt_serialBuff.length()); // -'den sonrasi y verisi
    //Serial.println("data x ="+dataX); //debug icin ..
    

    bt_serialBuff = ""; //veri okunduktan sonra bufferi bosalt
  }

}
float mapf(const float& x,const float& in_min,const float& in_max,const float& out_min,const float& out_max) {
     float result;
     result = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
     //in_min-in_max araligini out_min out_max araligina map ediyor.
     //arduino built in fonksiyonundan farkli olarak int yerine float degerlerine de map edebiliyor.
     return result;
}