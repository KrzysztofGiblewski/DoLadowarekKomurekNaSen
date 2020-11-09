#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DS3231.h> // zegar

DS3231 Clock;
RTClib RTC;

int godziny = 12;
int   minuty = 15;
int    sekundy = 20;

int odliczanie = 2;
int minutyPoprzednie;

int ekrany = 0;
int wyjdzZMenu = 0;
int ekranyUstawien = 0; //czy rozszerzone menu czy krutkie
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Ustawienie adresu ukladu na 0x27         A4 SDA        A5 SCL

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  lcd.print("0");
  pinMode(A0, INPUT_PULLUP); //przycisk dodawania sztuki A0
  pinMode(A1, INPUT_PULLUP); // przycisk odejmowania A1
  pinMode(A2, INPUT_PULLUP); //przycisk wyboru A2
  pinMode(A3, OUTPUT); //Konfiguracja A3 jako wyjście dla buzzera
  //modul na pinie A4 SDA  dla I2C
  //    i A5 SCL dla I2C

  ////////////*********************************************************************************///////////
  // ******* to po kolei wybraną linie odkomentować żeby ustawić zegar a potem zakomentować,  prymitywne ale dziala ;)
  //*********koniecznie podłączyć 3,3V bo przy 5V wariuje
  //ROK
  //  Clock.setYear(18);
  //MIESIAC
  //   Clock.setMonth(6);
  //DZIEN
  //   Clock.setDate(11);
  //DZIEN TYGODNIA NP 1 TO PONIEDZIAŁEK
  //   Clock.setDoW(2);
  //GODZINA
  //Clock.setHour(19);
  //MINUTY
  //Clock.setMinute(3);
  //SEKUNDY
  //   Clock.setSecond(15);
  //////////////*********************************************************************************////////////

}

void loop() {
  DateTime now = RTC.now();
  godziny = now.hour();
  minuty = now.minute();
  sekundy = now.second();

  if (digitalRead(A0) == LOW)    //przycisk wyboru A0 bedzie dodawal dlugosc ladowania
  {
    odliczanie++;
    delay(150);
  }
  if (digitalRead(A1) == LOW)    //przycisk wyboru A1 bedzie odejmowal dlugosc ladowania
  {
    odliczanie--;
    delay(150);
  }
  if (digitalRead(A2) == LOW)    //przycisk wyboru A2 bedzie konczyc ladowanie
  {
    odliczanie = 0;
    delay(150);
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
    lcd.print("  <---:  ");
    if (sekundy < 10) //jak sekundy od 0 do 9 to trzeba zero dopisac
      lcd.print(0);
    lcd.print(sekundy);
    if (minutyPoprzednie + 1 == minuty ) //jak minuty od 0 do 9 to trzeba zero dopisac
      odliczanie--;
  }
  if (odliczanie <= 0)
  {
    lcd.setCursor(0, 1);
    lcd.print("Koniec           ");
  }
  minutyPoprzednie = minuty;
}
