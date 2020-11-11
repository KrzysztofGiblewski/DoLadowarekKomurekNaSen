#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DS3231.h> // zegar

DS3231 Clock;
RTClib RTC;

int godziny = 12;
int   minuty = 15;
int    sekundy = 20;

int odliczanie = 0;
int opoznienie = 0;
int minutyPoprzednie = 0;

int interwal = 90;   // to ilosc minut dodawana przez klikniecie przycisku

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Ustawienie adresu ukladu na 0x27         A4 SDA        A5 SCL

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  lcd.print("0");
  pinMode(A0, INPUT_PULLUP); //przycisk dodawania sztuki A0
  pinMode(A1, INPUT_PULLUP); // przycisk odejmowania A1
  pinMode(A2, INPUT_PULLUP); //przycisk wyboru A2
  pinMode(A3, OUTPUT); //Konfiguracja A3 jako wyjście dla buzzera
  pinMode(8, OUTPUT);       //przekaznik1 jako wyjście
  digitalWrite(8, false); // na start wylaczony przekaznik



  ////////////*********************************************************************************///////////
  /////******* to po kolei wybraną linie odkomentować żeby ustawić zegar a potem zakomentować,  prymitywne ale dziala ;)  ////
  //         koniecznie podłączyć 3,3V bo przy 5V wariuje ****************************************************************////
  //   Clock.setHour(19);    //GODZINA                    ****************************************************************////
  //   Clock.setMinute(3);   //MINUTY                     ****************************************************************////
  //   Clock.setSecond(15);  //SEKUNDY                    ****************************************************************////
  ////********************************************************************************************************************////

}

void loop() {
  DateTime now = RTC.now();
  godziny = now.hour();
  minuty = now.minute();
  sekundy = now.second();


sprawdz();

  if (digitalRead(A1) == LOW && digitalRead(A0) == LOW)  // jednoczesnie przytrzymane przyciski A0 i A1 dodaje opuznienie a po przekroczeniu 300 minut = sie 0 i tak w kolko
  {
    if (opoznienie < 300)
    {
      opoznienie += 60;
      delay(100);
    }
    else if (opoznienie >= 300)
      opoznienie = 0;
  }
  if (digitalRead(A0) == LOW && digitalRead(A1) == HIGH)    //przycisk wyboru A0 bedzie dodawal dlugosc ladowania o interwal
  {
    
    odliczanie += interwal;
    delay(200);
    sprawdz();
  }
  if (digitalRead(A1) == LOW)    //przycisk wyboru A1 bedzie odejmowal dlugosc ladowania pod warunkiem ze nie pozostalo mniej niz chce odjac
  {
    if (odliczanie > (interwal / 2 ))
      odliczanie -= interwal / 2;
    delay(200);
    sprawdz();
  }
  if (digitalRead(A2) == LOW)    //przycisk wyboru A2 bedzie konczyc ladowanie
  {
    odliczanie = 0;
    opoznienie = 0;
    delay(200);
    sprawdz();
  }

  lcd.setCursor(0, 0);
  lcd.print("Teraz ");
  if (godziny < 10) //jak godziny od 0 do 9 to trzeba zero dopisac zeby ładnie było
    lcd.print(0);
  lcd.print(godziny);
  lcd.print(":");
  if (minuty < 10) //jak minuty od 0 do 9 to trzeba zero dopisac
    lcd.print(0);
  lcd.print(minuty);
  lcd.print(":");
  if (sekundy < 10) //jak sekundy od 0 do 9 to trzeba zero dopisac
    lcd.print(0);
  lcd.print(sekundy);
  lcd.print("   ");

  if (odliczanie > 0)
  {
    lcd.setCursor(0, 1);
    lcd.print(odliczanie);
    lcd.print(" min   ZA ");
    lcd.print(opoznienie);
    lcd.print("  ");

    if (minutyPoprzednie != minuty) // jesli minuty rozne od poprzednich nie wazne w ktora strone
   {   minutyPoprzednie = minuty;
    if (opoznienie>0)
    opoznienie--;
    if (opoznienie<=0)
    odliczanie--;
    sprawdz();
  }}
  if (odliczanie <= 0 )
  {
    lcd.setCursor(0, 1);
    lcd.print("Koniec            ");
    digitalWrite(8, false);
  }

}

void sprawdz() {
  if (opoznienie > 0  && odliczanie > 0) // a opoznienie nadal odlicza
    digitalWrite(8, false);
  if (opoznienie <= 0 && odliczanie > 0)
    digitalWrite(8, true);
}
