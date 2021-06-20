// ALICI PROGRAMI (MEGA) :

#include <SPI.h>
#include <RF24.h>
#include <Servo.h>
#include <nRF24L01.h>
#include <AFMotor.h>

RF24 radio(48, 53); // CSN-53, CE-48, SCK-52, MOSI-51, MISO-50 NRF24 pin bağlantıları.
const byte address[6] = "00001";// NRF24 byte adresi.
AF_DCMotor motor1(1, MOTOR12_64KHZ);//Ana motor çıkışı. (M1)
AF_DCMotor motor2(2, MOTOR12_64KHZ);//Ana motor çıkışı. (M2)
AF_DCMotor puruva(3, MOTOR12_64KHZ);//Puruva pervane çıkışı. (M3)
AF_DCMotor kornaM(4, MOTOR12_64KHZ);//Korna çıkışı :) (M4)
Servo servoy1;// Dümen servosu(Pin10)
Servo servoy2;// Dümen servosu(Pin9)

int x_pos;//Motor devir bilgisi.
int y_pos;//Dümen bilgisi.
bool buton;// Motor devir kilit.
bool puruva_sag;// Puruva motor sağa dönüş.
bool puruva_sol;// Puruva motor sola dönüş.
bool korna;// Korna.
bool isik;// Işık.
int servo_piny1 = 10;// Dümen servo açısı(Pin 9/10).
int servo_piny2 = 9;// Dümen servo açısı(Pin 9/10).
int gaz=0;
int kornaPin = 22;// Işık çıkış pini.
int isikPin = 24;// Korna çıkış pini.
int joystick[7];// 7 kanallı joystik bilgisi değişkeni.
int SagtrigPin = 14; /* Sensorun Sagtrig pini Arduinonun 14 numaralı ayağına bağlandı */
int SagechoPin = 15;  /* Sensorun Sagecho pini Arduinonun 15 numaralı ayağına bağlandı */
int SoltrigPin = 16; /* Sensorun Soltrig pini Arduinonun 16 numaralı ayağına bağlandı */
int SolechoPin = 17;  /* Sensorun Solecho pini Arduinonun 17 numaralı ayağına bağlandı */
long Sagsure;
long Saguzaklik;
long Solsure;
long Soluzaklik;

void setup() {
  Serial.begin(9600);
  radio.begin();
  servoy1.attach (servo_piny1 ) ;
  servoy2.attach (servo_piny2 ) ;
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();// Alıcı dinlemeye geçiyor.
  pinMode(SagtrigPin, OUTPUT); /* Sagtrig pini çıkış olarak ayarlandı */
  pinMode(SagechoPin,INPUT); /* Sagecho pini giriş olarak ayarlandı */
  pinMode(SoltrigPin, OUTPUT); /* Soltrig pini çıkış olarak ayarlandı */
  pinMode(SolechoPin,INPUT); /* Solecho pini giriş olarak ayarlandı */
}

void loop() {
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
    
    y_pos = map(y_pos, 0, 1023, 110, 70);// Haritalandırma (y_pos) Dümen bilgisi.
    x_pos = map(x_pos, 0, 1023, 0, 255);// Haritalandırma (x_pos) Ana motor devir bilgisi.
    motor1.setSpeed(gaz);// Ana Motor devri (gaz) degerine göre değişiyor.
    motor1.run(FORWARD);// Ana motorlar ileri yönde çalışıyor.
    motor2.setSpeed(gaz);// Ana Motor devri (gaz) degerine göre değişiyor.
    motor2.run(FORWARD);// Ana motorlar ileri yönde çalışıyor.
    if (isik==0){// Ana motorlar geri yönde çalışır.
      motor1.run(BACKWARD);// Ana motorlar ileri yönde çalışıyor.
      motor2.run(BACKWARD);// Ana motorlar ileri yönde çalışıyor.
    }
    

   // ANA MOTOR DEVİR:
    if (buton == 0) { // Ana motor devir ayarı.
    gaz = x_pos;
    }
          
    // PURUVA PERVANESİ KUMANDA:
    if (puruva_sol == LOW) { // Puruva pervanesi sağa dönüş için doğru yönde çalışıyor.
      puruva.run (FORWARD);
      puruva.setSpeed (255);
      delay(1);
    }
    else if (puruva_sag == LOW) { // Puruva pervanesi sola dönüş için ters yönde çalışıyor.
      puruva.run (BACKWARD);
      puruva.setSpeed (255);
      delay(1);
    }
    else if (y_pos <= 30) { //  Dümen açısı 30 dereceden küçük ise puruva pervanesi doğru yönde dönüyor.
      puruva.run (FORWARD);
      puruva.setSpeed (255);
      delay(1);
    }
    else if (y_pos >= 150) { // Dümen açısı 150 dereceden büyük ise puruva prvanesi ters yönde dönüyor.
      puruva.run (BACKWARD);
      puruva.setSpeed (255);
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
    //Serial.print("GAZ: ");
    //Serial.println(gaz);
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
  }
  //Sağ sensör mesafe ölçümü
  digitalWrite(SagtrigPin, LOW); /* sensör pasif hale getirildi */
  delayMicroseconds(5);
  digitalWrite(SagtrigPin, HIGH); /* Sensore ses dalgasının üretmesi için emir verildi */
  delayMicroseconds(10);
  digitalWrite(SagtrigPin, LOW);  /* Yeni dalgaların üretilmemesi için trig pini LOW konumuna getirildi */
  Sagsure = pulseIn(SagechoPin, HIGH); /* ses dalgasının geri dönmesi için geçen sure ölçülüyor */
  Saguzaklik= Sagsure /29.1/2; /* ölçülen sure uzaklığa çevriliyor */
  //Sol sensör mesafe ölçümü
  digitalWrite(SoltrigPin, LOW); /* sensör pasif hale getirildi */
  delayMicroseconds(5);
  digitalWrite(SoltrigPin, HIGH); /* Sensore ses dalgasının üretmesi için emir verildi */
  delayMicroseconds(10);
  digitalWrite(SoltrigPin, LOW);  /* Yeni dalgaların üretilmemesi için trig pini LOW konumuna getirildi */
  Solsure = pulseIn(SolechoPin, HIGH); /* ses dalgasının geri dönmesi için geçen sure ölçülüyor */
  Soluzaklik= Solsure /29.1/2; /* ölçülen sure uzaklığa çevriliyor */
  
        if (Saguzaklik <=150) { // Saguzaklık 150cm eşit veya küçük ise.
      servoy1.write (110);
      servoy2.write (110);
      delay(1);
    }
    else { // Saguzaklık 150cm den büyük ise.
      servoy1.write (y_pos);// 1 Numaralı servo (Pin 10)üzerinden dümen açısı bilgisi(y_pos) gidiyor.
      servoy2.write (y_pos);// 2 Numaralı servo (Pin 9)üzerinden dümen açısı bilgisi(y_pos) gidiyor.
    }
     if (Soluzaklik <=150) { // Soluzaklık 150cm eşit veya küçük ise.
      servoy1.write (70);
      servoy2.write (70);
      delay(1);
    }
    else // Soluzaklık 150cm den büyük ise.
      servoy1.write (y_pos);// 1 Numaralı servo (Pin 10)üzerinden dümen açısı bilgisi(y_pos) gidiyor.
      servoy2.write (y_pos);// 2 Numaralı servo (Pin 9)üzerinden dümen açısı bilgisi(y_pos) gidiyor.
}
