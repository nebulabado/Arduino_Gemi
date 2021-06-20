// ALICI PROGRAMI (MEGA) :

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <AFMotor.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
RF24 radio(48, 53); // CSN-48, CE-53, SCK-52, MOSI-51, MISO-50 NRF24 pin bağlantıları.
AF_DCMotor motor(1, MOTOR12_64KHZ);//Ana motor çıkışı. (M1)
AF_DCMotor puruva(2, MOTOR12_64KHZ);//Puruva pervane çıkışı. (M2)
AF_DCMotor kornaM(3, MOTOR12_64KHZ);//Korna çıkışı :) (M3)
Servo servoy;// Dümen servosu
TinyGPSPlus gps;

const byte address[6] = "00001";// NRF24 byte adresi.
static const int RXPin = A8, TXPin = A9;
static const uint32_t GPSBaud = 9600;
SoftwareSerial ss(RXPin, TXPin);
int x_pos;//Motor devir bilgisi.
int y_pos;//Dümen bilgisi.
bool buton;// Motor devir kilit.
bool puruva_sag;// Puruva motor sağa dönüş.
bool puruva_sol;// Puruva motor sola dönüş.
bool korna;// Korna.
bool isik;// Işık.
int servo_piny = 10;// Dümen servo açısı(Pin 9/10).
int gaz = 0;
int kornaPin = 22;// Işık çıkış pini.
int isikPin = 24;// Korna çıkış pini.
int joystick[7];// 7 kanallı joystik bilgisi değişkeni.
int SAT;
int HDOP;
float LAT;
float LAN;
int AGE;
float ALT;
float YON;
float HIZ;

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
  radio.begin();
  servoy.attach (servo_piny ) ;
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();// Alıcı dinlemeye geçiyor.
  }

void loop() {
  static const double Hedef_LAT = 51.508131, Hedef_LON = -0.128002;

  SAT = (gps.satellites.value(), gps.satellites.isValid(), 5);
  HDOP = (gps.hdop.value(), gps.hdop.isValid(), 5);
  LAT = gps.location.lat();
  LAN = gps.location.lng();
  AGE = gps.location.age();
  printDateTime(gps.date, gps.time);
  ALT = gps.altitude.meters();
  YON = gps.course.deg();
  HIZ = gps.speed.kmph();
  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.value()) : "*** ", 6);


  unsigned long distanceKmToHedef =
    (unsigned long)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      Hedef_LAT,
      Hedef_LON) / 1000;
  printInt(distanceKmToHedef, gps.location.isValid(), 9);

  double courseToHedef =
    TinyGPSPlus::courseTo(
      gps.location.lat(),
      gps.location.lng(),
      Hedef_LAT,
      Hedef_LON);

  printFloat(courseToHedef, gps.location.isValid(), 7, 2);

  const char *cardinalToHedef = TinyGPSPlus::cardinal(courseToHedef);

  printStr(gps.location.isValid() ? cardinalToHedef : "*** ", 6);

  printInt(gps.charsProcessed(), true, 6);
  printInt(gps.sentencesWithFix(), true, 10);
  printInt(gps.failedChecksum(), true, 9);
  Serial.println();

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i = flen; i < len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }

  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i = 0; i < len; ++i)
    Serial.print(i < slen ? str[i] : ' ');
  smartDelay(0);
  //Serial.println();
  //Serial.print("LAT: ");
  //Serial.print(LAT, 7);
  //Serial.println();
  //Serial.print("LAN: ");
  //Serial.print(LAN, 7);
  //Serial.println();
  //Serial.print("ALT: ");
  //Serial.print(ALT);
  //Serial.println();
  //Serial.print("YÖN: ");
  //Serial.print(YON);
  //Serial.println();
  //Serial.print("HIZ: ");
  //Serial.print(HIZ);

  //----KUMANDA PROGRAMI----
  Serial.println();
  Serial.println("KUMANDA PROGRAMI");

  if (radio.available()) {// Vericiden sinyal alıyosa.
    radio.read(joystick, sizeof(joystick));// Vericiden gelen bilgileri kayıt edıyor.
    x_pos = joystick[0];
    y_pos = joystick[1];
    buton = joystick[2];
    puruva_sag = joystick[3];
    puruva_sol = joystick[4];
    korna = joystick[5];
    isik = joystick[6];

    y_pos = map(y_pos, 0, 1023, 180, 0);// Haritalandırma (y_pos) Dümen bilgisi.
    x_pos = map(x_pos, 0, 1023, 0, 255);// Haritalandırma (x_pos) Ana motor devir bilgisi.
    motor.setSpeed(gaz);// Ana Motor devri (x_pos) degerine göre değişiyor.
    motor.run(FORWARD);// Ana motorlar ileri yönde çalışıyor.
    servoy.write (y_pos);// 1 Numaralı servo (Pin 10)üzerinden dümen açısı bilgisi(y_pos) gidiyor.

    // ANA MOTOR DEVİR:
    if (buton == 0) { // Ana motor devir ayarı.
      gaz = x_pos;
    }
    // PURUVA PERVANESİ KUMANDA:
    if (puruva_sol == LOW) { // Puruva pervanesi sağa dönüş için doğru yönde çalışıyor.
      puruva.run (FORWARD);
      puruva.setSpeed (250);
      delay(1);
    }
    else if (puruva_sag == LOW) { // Puruva pervanesi sola dönüş için ters yönde çalışıyor.
      puruva.run (BACKWARD);
      puruva.setSpeed (250);
      delay(1);
    }
    else if (y_pos <= 30) { //  Dümen açısı 30 dereceden küçük ise puruva pervanesi doğru yönde dönüyor.
      puruva.run (FORWARD);
      puruva.setSpeed (250);
      delay(1);
    }
    else if (y_pos >= 150) { // Dümen açısı 150 dereceden büyük ise puruva prvanesi ters yönde dönüyor.
      puruva.run (BACKWARD);
      puruva.setSpeed (250);
      delay(1);
    }
    else {
      puruva.setSpeed (0);
    }

    // KORNA ve IŞIK
    if (korna == LOW) {
      kornaM.run (FORWARD);// 3 Numaralı motor sürücüsü üzerinden KORNA bağlı.
      kornaM.setSpeed (255);
    }
    else {
      kornaM.setSpeed (0);
    }
    digitalWrite (isikPin, not isik); // 24 Numaralı pin üzerinden IŞIK kumandası gidiyor.
    // KONTROL
    //Serial.print("x_pos: ");
    //Serial.println(x_pos);
    //Serial.print("y_pos: ");
    //Serial.println(y_pos);
    //Serial.print("buton: ");
    //Serial.println(buton);
    //Serial.print("puruva_sag: ");
    //Serial.println(puruva_sag);
    //Serial.print("puruva_sol: ");
    //Serial.println(puruva_sol);
    //Serial.print("korna: ");
    //Serial.println(korna);
    //Serial.print("ısık: ");
    //Serial.println(isik);
    Serial.print("TARİH/SAAT: ");
    printDateTime(gps.date, gps.time);
   
     }
}
