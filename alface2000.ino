#include "DHT.h"
#include <LiquidCrystal.h>

#define sensor_co2 A0 // 
#define co2Zero     60 //calibrated CO2 0 level
#define DHTPIN A1     // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11

 
// Conecte pino 1 do sensor (esquerda) ao +5V
// Conecte pino 2 do sensor ao pino de dados definido em seu Arduino
// Conecte pino 4 do sensor ao GND
// Conecte o resistor de 10K entre pin 2 (dados) 
// e ao pino 1 (VCC) do sensor
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int gasLevel = 0;                         //int variable for gas level
int co2ppm = 0; 


byte ico_temp[8] = //icon for termometer
{
    B00100,
    B01010,
    B01010,
    B01110,
    B01110,
    B11111,
    B11111,
    B01110
};

byte ico_umid[8] = //icon for water droplet
{
    B00100,
    B00100,
    B01010,
    B01010,
    B10001,
    B10001,
    B10001,
    B01110,
};

void setup() 
{
  pinMode(sensor_co2,INPUT); 
  
  Serial.begin(9600);
  Serial.println("Alface 2000");
  
  lcd.begin(16, 2);
  lcd.clear();
  lcd.createChar(1,ico_temp);
  lcd.createChar(2,ico_umid);
  
  dht.begin();
}
 
void loop() 
{
  // le umidade do solo
  float anal_solo = analogRead(A3);
  
  // le umidade & temperatura
  int h = dht.readHumidity();
  int t = dht.readTemperature();

  // le co2
  int co2now[10];        //int array for co2 readings
  int co2raw = 0;        //int for raw value of co2
  int co2comp = 0;       //int for compensated co2 
  int co2ppm = 0;        //int for calculated ppm
  int zzz = 0;           //int for averaging

  //Sample co2 10x over 2 seconds
  for (int x = 0;x<10;x++)
  {                   
    co2now[x]=analogRead(sensor_co2);
    delay(200);
  }
  //Add samples together
  for (int x = 0;x<10;x++)
  {                     
    zzz=zzz + co2now[x];
  }
  co2raw = zzz/10;                            //divide samples by 10
  co2comp = co2raw - co2Zero;                 //get compensated value
  co2ppm = map(co2comp,0,1023,400,5000);      //map value for atmospheric levels

  //gasLevel = analogRead(sensor_co2);
  //co2ppm = map(gasLevel,0,1023,400,5000); 
  
  // imprime valores na serial
  Serial.print(" - Umidade >> ");
  Serial.print(h);
  Serial.print(" % ");
  Serial.print(" - Temperatura >> ");
  Serial.print(t);
  Serial.print(" C ");
  Serial.print(" - CO2 >> ");
  Serial.print(co2ppm);
  Serial.print(" ppm");
  Serial.print(" - Umi. Solo >> ");
  Serial.print(anal_solo);
  Serial.println(" ");

  // Imprime no display
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.print(":");
  lcd.print(t);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(7, 0);
  lcd.write(2);
  lcd.print(":");
  lcd.print(h);
  lcd.print("%");
  
  lcd.setCursor(0, 1);
  lcd.print("C:");
  lcd.print(co2ppm);
  lcd.print("ppm");
  lcd.setCursor(9, 1);
  lcd.print("S:");
  if (anal_solo > 0 && anal_solo < 400)
  {
    lcd.print("Exces");
  }
  //Solo com umidade moderada, acende led amarelo
  if (anal_solo > 400 && anal_solo < 800)
  {
    lcd.print("OK   ");
  }
  //Solo seco, acende led vermelho
  if (anal_solo > 800 && anal_solo < 1024)
  {
    lcd.print("Seco");
  }
  delay(2000);  
}
