#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <String.h>

union num1 {
  struct {
    unsigned long mantissa : 23;
    unsigned int exponent : 8;
    unsigned int sign : 1;
  } raw;
  float f;
} num1;

union num2 {
  struct {
    unsigned long mantissa : 23;
    unsigned int exponent : 8;
    unsigned int sign : 1;
  } raw;
  float f;
} num2;

union num3 {
  struct {
    unsigned long mantissa : 23;
    unsigned int exponent : 8;
    unsigned int sign : 1;
  } raw;
  float f;
} num3;

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; //connect to the column pinouts of the keypad



Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
LiquidCrystal_I2C lcd(0x27, 16, 2);

void wynikDodatni() {
  //bity do przesuniecia
  int bitsToShift = num1.raw.exponent - num2.raw.exponent;
  //tylko dla cyfr które mają różne eksponenty
  if (bitsToShift > 0) {

    //wyrownywanie wykladnikow
    num2.raw.mantissa >>= 1;
    num2.raw.mantissa += 4194304;
    num2.raw.mantissa >>= (bitsToShift - 1);
    num2.raw.exponent += bitsToShift;

    //przesuwam, żeby upewnić się, że mantysa.num1>mantysa.num2, bo shift bitów mógł to zmienić( a właściwie to zmienił to napewno)
    num1.raw.mantissa >>= 1;
    num1.raw.mantissa += 4194304;
    num1.raw.exponent += 1;
    num2.raw.mantissa >>= 1;
    num2.raw.exponent += 1;

  }
  //odejmuje mantysy, bo eksponenty już są wyrównane ich względem
  num3.raw.mantissa = num1.raw.mantissa - num2.raw.mantissa;
  //wyrównanie eksponenty wyniku
  //zmienna pomocnicza
  int expoDoOdjecia = 0;
  while (num3.raw.mantissa < 4194304) {
    num3.raw.mantissa *= 2;
    expoDoOdjecia -= 1;

  }
  num3.raw.mantissa *= 2;
  expoDoOdjecia -= 1;

  //teraz jak mantysy graja to trzeba znormalizowac wynik poprostu
  num3.raw.exponent = num2.raw.exponent;
  num3.raw.exponent += expoDoOdjecia;
}

void wynikUjemny() {
  int bitsToShift = num2.raw.exponent - num1.raw.exponent;
  if (bitsToShift > 0) {
    //wyrownywanie wykladnikow

    num1.raw.mantissa >>= 1;
    num1.raw.mantissa += 4194304;
    num1.raw.mantissa >>= (bitsToShift - 1);
    num1.raw.exponent += bitsToShift;

    num2.raw.mantissa >>= 1;
    num2.raw.mantissa += 4194304;
    num2.raw.exponent += 1;
    num1.raw.mantissa >>= 1;
    num1.raw.exponent += 1;
  }
  num3.raw.mantissa = num2.raw.mantissa - num1.raw.mantissa;
  int expoDoOdjecia = 0;
  while (num3.raw.mantissa < 4194304) {
    num3.raw.mantissa *= 2;
    expoDoOdjecia -= 1;
  }
  num3.raw.mantissa *= 2;
  expoDoOdjecia -= 1;

  //teraz jak mantysy graja to trzeba znormalizowac wynik poprostu
  num3.raw.exponent = num1.raw.exponent;
  num3.raw.exponent += expoDoOdjecia;
  num3.raw.sign = !num3.raw.sign;

}

void rowWykDod() {


  num1.raw.mantissa >>= 1;
  num1.raw.exponent += 1;
  num2.raw.mantissa >>= 1;
  num2.raw.exponent += 1;

  num3.raw.exponent = num1.raw.exponent;
  num3.raw.mantissa = num1.raw.mantissa + num2.raw.mantissa;

  //shift bits
  //        num3.raw.mantissa >>= 1;
  //        num3.raw.exponent+=1;


}
void num1WiekszeDod() {

  int sizeToShift = num1.raw.exponent - num2.raw.exponent;
  num2.raw.mantissa >>= 1;
  num2.raw.mantissa += 4194304;
  num2.raw.mantissa >>= (sizeToShift - 1);
  num2.raw.exponent = num1.raw.exponent;
  num3.raw.mantissa = num1.raw.mantissa + num2.raw.mantissa;
  num3.raw.exponent = num1.raw.exponent;
  if (num3.raw.mantissa < num2.raw.mantissa && num3.raw.mantissa < num1.raw.mantissa) {
    num3.raw.mantissa >>= 1;
    num3.raw.exponent += 1;
  }
}

void num2WiekszeDod() {
  float temp;
  temp = num1.f;
  num1.f = num2.f;
  num2.f = temp;
  int sizeToShift = num1.raw.exponent - num2.raw.exponent;
  num2.raw.mantissa >>= 1;
  num2.raw.mantissa += 4194304;
  num2.raw.mantissa >>= (sizeToShift - 1);
  num2.raw.exponent = num1.raw.exponent;
  num3.raw.mantissa = num1.raw.mantissa + num2.raw.mantissa;
  num3.raw.exponent = num1.raw.exponent;
  if (num3.raw.mantissa < num2.raw.mantissa && num3.raw.mantissa < num1.raw.mantissa) {
    num3.raw.mantissa >>= 1;
    num3.raw.exponent += 1;
  }
}

//funkcja resetująca/początkowa
void start() {
  num1.f = 0.0;
  num2.f = 0.0;
  lcd.clear();
  lcd.print("Wybor operacji:");
  lcd.setCursor(0, 1);
  lcd.print("add,sub,mul,div");
  delay(100);
}

//funkcja zczytujaca 1 cyfre
void setInput(float *num) {
  int pot10 = 10;
  lcd.clear();
  while (true) {
    char key = keypad.getKey();
    if (key == '*') {
      lcd.print('.');
      while (true) {
        char key = keypad.getKey();
        if (key && key!='A') {
          *num += (((1.0 * key) - 48) / pot10);
          pot10 *= 10;
          Serial.println("");
          Serial.println(*num, 6);
          lcd.print(key);

        }
        if (key == 'A') {
          break;
        }
      }
      break;
    }
    if (key && key!='A') {
      *num = 10 * (*num);
      *num += (key - 48);
      lcd.print(key);
      Serial.println(*num);
    }
    if (key == 'A') {
      break;
    }

  }
}

void add() {
  lcd.clear();

  //TE SAME WYKLADNIKI
  if (num2.raw.exponent == num1.raw.exponent) {
    rowWykDod();
  }

  //NUM1 WIEKSZY
  if (num1.raw.exponent > num2.raw.exponent) {
    num1WiekszeDod();
  }

  //NUM2 WIEKSZE
  if (num2.raw.exponent > num1.raw.exponent) {
    num2WiekszeDod();

  }

  lcd.print(num3.f,6);

}

void sub() {
  lcd.clear();
  if (num2.raw.exponent < num1.raw.exponent || (num1.raw.exponent == num2.raw.exponent && num2.raw.mantissa < num1.raw.mantissa)) {
    wynikDodatni();
  }

  //to samo co u góry, korzystam z zależności x-y=-(y-x), więc zwykły swap num2 i num1 działa, na końcu jedynie zmieniam znak wyniku
  else
  {
    //WYNIK 0
    if (num1.raw.mantissa == num2.raw.mantissa && num2.raw.exponent == num1.raw.exponent) {

    }
    //WYNIK UJEMNY
    else {
      wynikUjemny();
    }
  }
  lcd.print(num3.f,6);


}

void mul() {
  lcd.clear();
  num3.raw.exponent = num1.raw.exponent + num2.raw.exponent - 127;

  num1.raw.mantissa = num1.raw.mantissa;
  //przesuwam ukryte jedynki do mantysy

  Serial.println(num1.raw.mantissa);
  num1.raw.mantissa >>= 1;
  num1.raw.mantissa += 4194304;
  num2.raw.mantissa >>= 1;
  num2.raw.mantissa += 4194304;
  Serial.println(num1.raw.mantissa);


  int a[23];
  long n = num1.raw.mantissa;
  for (int i = 0; n > 0; i++)
  {
    a[i] = n % 2;
    n = n / 2;
  }
  int b[23];
  n = num2.raw.mantissa;
  for (int i = 0; n > 0; i++)
  {
    b[i] = n % 2;
    n = n / 2;
  }
  Serial.print("mantissa1:");
  for (int i = 0; i < 23; i++) {
    Serial.print(a[i]);
  }
  Serial.println("");

  Serial.print("mantissa2:");
  for (int i = 0; i < 23; i++) {
    Serial.print(b[i]);
  }
  Serial.println("");

  int result[46] = {0};

  for (int i = 0; i < 23; i++) {
    int bitShift = i;
    if (b[i] == 1) {
      for (int k = 0; k < 23; k++) {
        int partial = result[k + bitShift] + a[k];
        if (partial == 3) {
          result[k + bitShift + 1] += 1;
          result[k + bitShift] = 1;
        } else if (partial == 2) {
          result[k + bitShift + 1] += 1;
          result[k + bitShift] = 0;
        } else {
          result[k + bitShift] = partial;
        }
      }
    }
  }
  for (int i = 46; i > 0; i--) {
    Serial.print(result[i]);
  }


  //zmienaim wynik binarny na inta
  long multiplier = 1;
  unsigned long mantysa2temp = 0;
  num2.raw.mantissa = 0;
  int output = 0;
  unsigned long power = 1;
  for (int i = 22; i < 45; i++)
  {
    num2.raw.mantissa += result[i] * power;
    power *= 2;
  }
  Serial.println(num2.raw.mantissa);
  //dziala
  //jezeli overflow

  int zmiennaPomocnicza = result[46] + result[45];


  if (result[45] == 1) {
    Serial.println("zmiennaPomocnicza1");
    num3.raw.exponent += 1;
    num3.raw.mantissa = num2.raw.mantissa;
  } else {
    Serial.println("zmiennaPomocnicza0");
    num2.raw.mantissa <<= 1;
    num3.raw.mantissa = num2.raw.mantissa;
  }
  lcd.print(num3.f,6);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(10, INPUT_PULLUP);

  lcd.begin();
  lcd.backlight();
  lcd.clear();
  start();
}

void loop() {
  //przycisk do resetu
  if (digitalRead(10) == LOW) {
    start();
  }
  //zaladuj klik z keypada
  char key = keypad.getKey();

  //dodawanie
  if (key == 'D') {
    Serial.println("rozpoczeto setowanie inputu 1");
    setInput(&num1.f);
    Serial.println("zakonczenie setowania inputu 1");
    Serial.println("rozpoczeto setowanie inputu 2");
    setInput(&num2.f);
    Serial.println("zakonczono setowania inputu 2");
    Serial.println(num1.f);
    Serial.println(num2.f);
    add();
    num1.f = 0.0;
    num2.f = 0.0;
    Serial.println(num3.f);
  }

  //odejmowanie
  if (key == 'C') {
    Serial.println("rozpoczeto setowanie inputu 1");
    setInput(&num1.f);
    Serial.println("zakonczenie setowania inputu 1");
    Serial.println("rozpoczeto setowanie inputu 2");
    setInput(&num2.f);
    Serial.println("zakonczono setowania inputu 2");
    Serial.println(num1.f);
    Serial.println(num2.f);
    sub();
    num1.f = 0.0;
    num2.f = 0.0;
    Serial.println(num3.f);
  }

  if (key == 'B') {
    Serial.println("rozpoczeto setowanie inputu 1");
    setInput(&num1.f);
    Serial.println("zakonczenie setowania inputu 1");
    Serial.println("rozpoczeto setowanie inputu 2");
    setInput(&num2.f);
    Serial.println("zakonczono setowania inputu 2");
    Serial.println(num1.f);
    Serial.println(num2.f);
    mul();
    num1.f = 0.0;
    num2.f = 0.0;
    Serial.println(num3.f);

  }


  if (key == '#') {
    Serial.println("rozpoczeto setowanie inputu 1");
    setInput(&num1.f);
    Serial.println("zakonczenie setowania inputu 1");
    Serial.println("rozpoczeto setowanie inputu 2");
    setInput(&num2.f);
    Serial.println("zakonczono setowania inputu 2");
    Serial.println(num1.f);
    Serial.println(num2.f);
    lcd.clear();
    float x=((1.0)*num1.f)/num2.f;
    lcd.print(x,6);
    num1.f = 0.0;
    num2.f = 0.0;
    Serial.println(num3.f);
  }


  if (key && key != 'D' && key != 'C'  && key != 'B' && key != '#') {

  }

}
