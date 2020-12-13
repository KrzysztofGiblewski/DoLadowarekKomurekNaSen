#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DS3231.h> // zegar

DS3231 Clock;
RTClib RTC;

int godziny = 12;  // ta zmienna bedzie przechowywac godzine
int  minuty = 15;  // ta zmienna bedzie przechowywac minuty
int sekundy = 20;  // ta zmienna bedzie przechowywac sekundy

int odliczanie = 0;                                  // odlicza czas do konca ladowania, zaczyna odliczac po osiagnieciu przez opoznienie wartosci 0
int opoznienie = 0;                                  // ilosc minut pozostala do rozpoczecia ladowania
int minutyOKtorychWylaczySieWentylator = 0;          // to znaczy ze jak bedzie ustawione 10:07 na 3min to ta zmienna bedzie miala wartosc 10
int limitCzasuWlaczeniaWent = 25;                    // maksymalny czas wlaczenia wentylatora
int sumaCzasuWlaczeniaWentylatora = 0;               // sumujemy interwaly wlaczenia wentylatora (max patrz limitCzasuWlaczeniaWentylatora)
int interwaCzasulWlaczeniaWentylatora = 3;           // interwal dodawania czasu dzialania wentylatora w minutach
int minutyPoprzednie = 0;                            // taka wartosc tymczasowa zeby mozna bylo zobaczyc czy bierzaca minuta nie jest rowna poprzedniej minucie
int godzinaWentylator;                               // tylko po to zeby muc wyswietlic godzinke po przekroczeniu kolejnej godziny
int interwal = 30;                                   // to ilosc minut dodawana przez klikniecie przycisku, przy odejmowaniu odejmuje polowe tej wartosci
int ekranyKolejne=0;
boolean kontrolkaWlaczonegoWentylatora = false;      // kontrolka wlaczonego (true) lub wylaczonego (false) stanu wentylatora

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Ustawienie adresu ukladu na 0x27         A4 SDA        A5 SCL

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  lcd.print("0");
  pinMode(A0, INPUT_PULLUP); // Przycisk dodawania sztuki A0
  pinMode(A1, INPUT_PULLUP); // Przycisk odejmowania A1
  pinMode(A2, INPUT_PULLUP); // Przycisk dodawania opuznienia
  pinMode(A3, OUTPUT);       // Konfiguracja A3 jako wyjście dla buzzera
  pinMode(8, OUTPUT);        // Przekaznik jako wyjście dla ladowarek
  pinMode(7, OUTPUT);        // Przekaznik jako wyjście dla wentylatora

  digitalWrite(8, false);    // Na start wylaczony przekaznik ladowarek
  digitalWrite(7, true);     // Na start wylaczony przekaznik wentylatora

  ////////////*****************************************************************************************************///////////
  /////******* to po kolei wybraną linie odkomentować żeby ustawić zegar a potem zakomentować,  prymitywne ale dziala ;)  ////
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
  wyswietl();

  if (digitalRead(A0) == LOW)    //przycisk wyboru A0 bedzie dodawal dlugosc ladowania o interwal
  {
    odliczanie += interwal;
    if (odliczanie > 181)
      odliczanie = 0;
    delay(350);
  }
  if (digitalRead(A1) == LOW)  //przycisk uruchamia wentylator na interwaCzasulWlaczeniaWentylatora minut
  {
    sumaCzasuWlaczeniaWentylatora += interwaCzasulWlaczeniaWentylatora; //to dodaj do minut wylaczenia czas
    kontrolkaWlaczonegoWentylatora = true;
    godzinaWentylator = godziny;
    if (limitCzasuWlaczeniaWent < sumaCzasuWlaczeniaWentylatora  )// a jak przekroczy limit czasu dlugosci dzialania
    {
      sumaCzasuWlaczeniaWentylatora = 0;  // to wyzerowanie przez nastawienie minut wylaczenia na biezace minuty
    }
    minutyOKtorychWylaczySieWentylator = minuty + sumaCzasuWlaczeniaWentylatora; // ustawienie koncowej minuty
    if (minutyOKtorychWylaczySieWentylator > 59)
    {
      minutyOKtorychWylaczySieWentylator = minutyOKtorychWylaczySieWentylator - 60;
      godzinaWentylator++;
    }
    delay(350);                            // pauza zeby klikniecia nie byly zbyt szybkie bo wtedy ciezko cos ustawic
  }

  if (digitalRead(A2) == LOW  )            // przycisk A2 dodaje opuznienie a po przekroczeniu 300 minut = sie 0 i tak w kolko
  {
    opoznienie += 60;
    round(opoznienie);
    if (opoznienie > 300)
      opoznienie = 0;
    delay(350);
  }
}

void wyswietl() {
  lcd.setCursor(0, 0);
  if (kontrolkaWlaczonegoWentylatora == true)
  {
    
    lcd.print("W ");
    lcd.print(godzinaWentylator);
    lcd.print(":");
    if (minutyOKtorychWylaczySieWentylator < 10)          //jak minutyOKtorychWylaczySieWentylator od 0 do 9 to trzeba zero dopisac
      lcd.print(0);
    lcd.print(minutyOKtorychWylaczySieWentylator);
    lcd.print(" ");
  }
if (godziny < 10)                                       // jak godziny od 0 do 9 to trzeba zero dopisac zeby ładnie było
    lcd.print(0);
  lcd.print(godziny);
  lcd.print(":");
  if (minuty < 10)                                        //jak minuty od 0 do 9 to trzeba zero dopisac
    lcd.print(0);
  lcd.print(minuty);
  lcd.print(":");
  if (sekundy < 10)                                       //jak sekundy od 0 do 9 to trzeba zero dopisac
    lcd.print(0);
  lcd.print(sekundy);
  lcd.print("          ");
  
  if (odliczanie != 0 || opoznienie != 0)                 // jak juz dojdzie do konca odliczania
  {
    lcd.setCursor(0, 1);
    lcd.print(odliczanie);
    
    if (opoznienie > 0)
    {
      lcd.print(" minut ZA ");
      lcd.print(opoznienie);
      lcd.print("    ");
    }
    else
      lcd.print(" minut ladowania   ");

    if (minutyPoprzednie != minuty)                       // jesli minuty rozne od poprzednich nie wazne w ktora strone
    {
      minutyPoprzednie = minuty;                          // nadaje zmiennej wartosc bierzacej minuty
      if (opoznienie > 0)
        opoznienie--;
      if (opoznienie <= 0)
        odliczanie--;
      sprawdz();
    }
  }
  if (odliczanie <= 0 && opoznienie <= 0)                   // jak zarowno odliczanie opuznienia i czas ladowania jest zerowy
  {
    lcd.setCursor(0, 1);
    lcd.print("Nie ma zasilania ladowarek         ");
    digitalWrite(8, false);                                 // wylacza napiecie
  }
}

void sprawdz() {
  if (opoznienie > 0  && odliczanie > 0)                                           // a opoznienie nadal odlicza
    digitalWrite(8, false);                                                        // przekaznik nie podaje napiecia
  if (opoznienie <= 0 && odliczanie > 0)                                           // jak opuznienie doszlo do zera i czas ladowania jest nadal wiekszy od zera
    digitalWrite(8, true);                                                         // to przekaznik podaje napiecie

  if (sumaCzasuWlaczeniaWentylatora > 0 && kontrolkaWlaczonegoWentylatora == true)  // jesli kontrolka wylaczenia wentylatora wylaczona
  {
    digitalWrite(7, false);                                                         // przekaznik wentylatora wlaczony

  }
  if (minutyOKtorychWylaczySieWentylator == minuty && kontrolkaWlaczonegoWentylatora == true)   // jak minutyOKtorychWylaczySieWentylator czyli minuty rowne  nastawione minuty to koniec
  {
    digitalWrite(7, true);                                                                      // przekaznik wylaczony wentylator nie dziala
    kontrolkaWlaczonegoWentylatora = false;                                                     // kontrolka zmiana stanu na wylaczony
  }

  if (godziny == 10 && minuty == 10)            // tak na sztywno zapisany czas wlaczenia wentylatora zeby sie przewietrzylo
  {
    sumaCzasuWlaczeniaWentylatora = 5;          // tylko po to zeby warunek wekszej od zera byl spelniony
    digitalWrite(7, false);                     // przekaznik wlaczony wentylator dziala
    minutyOKtorychWylaczySieWentylator = 15;    // tak na sztywno minuty wylaczenia wentylatora
    kontrolkaWlaczonegoWentylatora = true;      // kontrolka wlaczonego wentylatora
  }
  if (godziny == 13 && minuty == 10)            // no i znowu przewietrzanie
  {
    sumaCzasuWlaczeniaWentylatora = 5;          // tylko po to zeby warunek wekszej od zera byl spelniony
    digitalWrite(7, false);                     // przekaznik wlaczony wentylator dziala
    minutyOKtorychWylaczySieWentylator = 15;    // tak na sztywno minuty wylaczenia wentylatora
    kontrolkaWlaczonegoWentylatora = true;      // kontrolka wlaczonego wentylatora
  }

}
