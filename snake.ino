#include <Adafruit_NeoPixel.h>
#include <IRremote.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define MATRIX_PIN 6
#define MATRIX_LED 100
#define MATRIX_SIDE 10
#define RECV_PIN 12


#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_LEFT 3
#define KEY_RIGHT 4

IRrecv irrecv(RECV_PIN);
decode_results results;

Adafruit_NeoPixel Matrix = Adafruit_NeoPixel(MATRIX_LED, MATRIX_PIN, NEO_GRB + NEO_KHZ800);
unsigned long MainTime;

int direction = 1;
int Snake[MATRIX_LED];
int Apple;
int LongSnake = 0;
int sx = 0;
int sy = 0;
int point = 0;
int corectx = 3;
int corecty = 2;
int ff = 0;
int num2 = 0;
int typegame = 0;

// 0 = nowall
// 1 = wall
// 2 = lab
//EDIT CONSOLE MOD
//GAME0 GAME1 GAME2

bool KeyUP = false;
bool KeyDOWN = false;
bool KeyLEFT = false;
bool KeyRIGHT = false;
bool updated = true;
bool colidewall = false;
bool conw, cona, cons, cond = false;
bool pause = true;
bool magicmod = false;
bool chetmod = false;
bool continuemod = false;
bool on = true;
bool debug = false;

float time = 0.1;
float maxTime = 0.2;

void setup() {
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  irrecv.enableIRIn();
  Matrix.begin();
  Matrix.clear();
  Matrix.show();
  Serial.begin(115200);
  GameStart();
  irrecv.blink13(true);
}
void ON(){
  if(on){
    on = false;
    Serial.println("Game OFF");
    Matrix.clear();
    Matrix.show();
  }else{
    on = true;
    Serial.println("Game ON");
    GameStart();
  }
}
void Debug(){
  if(debug){
    debug = false;
    Serial.println("Debug:OFF");
  }else{
    debug = true;
    Serial.println("Debug:ON");
  }
}
void MagicMod(){
  if(magicmod){
    magicmod = false;
    Serial.println("MagicMod:OFF");
  }else{
    magicmod = true;
    Serial.println("MagicMod:ON");
  }
}

void ContinueMod(){
  if(continuemod){
    continuemod = false;
    Serial.println("ContinueMod:OFF");
  }else{
    continuemod = true;
    Serial.println("ContinueMod: ON");
  }
}

void ChetMod(){
  if(chetmod){
    chetmod = false;
    Serial.println("ChetMod:OFF");
  }else{
    chetmod = true;
    Serial.println("ChetMod: ON");
  }
}

void loop() {
  
  while (!irrecv.isIdle())delay(90);
  if (irrecv.decode(&results)) {
    if(debug)
      Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  } else {
    int editmod = false;
    conw = false;
    cona = false;
    cons = false;
    cond = false;
    if (results.value == 0xE0E020DF){
      typegame = 0;
      editmod = true;
    }else if (results.value == 0xE0E0A05F){
      typegame = 1;
      editmod = true;
    }else if(results.value == 0xE0E006F9){//or digitalRead(9) == 0){
      conw = true;
    }else if(results.value == 0xE0E0A659){//or digitalRead(11) == 0){
      cona = true;
    }else if(results.value == 0xE0E08679){//or digitalRead(8) == 0){
      cons = true;
    }else if(results.value == 0xE0E046B9){//or digitalRead(10) == 0){
      cond = true;
    }else if (results.value == 0xE0E052AD && chetmod){
        pause = true;
    }else if (results.value == 0xE0E06897){// D
      MagicMod();
    }else if (results.value == 0xE0E07C83){// P.SIZE
      ChetMod();
    }else if (results.value == 0xE0E040BF or results.value == 0xF4BA2988){
      ON();
    }else if(results.value == 0xE0E0F807){
      Debug();
    }
    results.value = 0x0;
    if (Serial.available() >0) {
      int e = Serial.read();
      if(e==49){//1
          typegame = 0;
          editmod = true;
      }else if(e == 50){//2
          typegame = 1;
          editmod = true;
      }else if(e == 51){//3
          typegame = 2;
          editmod = true;
      }else if(e == 119){//w
        conw = true;
      }else if(e == 97){//a
        cona = true;
      }else if(e == 115){//s
        cons = true;
      }else if(e == 100){//d
        cond = true;
      }else if(e == 112 && chetmod){//p
        pause = true;
      }else if(e == 27 && Serial.read() == 91){//strzałko
          int s = Serial.read();
          if(s == 65)
            conw = true;
          else if(s == 68)
            cona = true;
          else if(s == 66)
            cons = true;
          else if(s == 67)
            cond = true;
      }else if(e == 61 && chetmod){
        LongSnake++;
        Serial.print("Long Snake:");
        Serial.println(LongSnake);
      }else if (e == 45 && (magicmod or chetmod)){
        int c = Snake[LongSnake];
        LongSnake--;
        Snake[LongSnake] = c;
        Serial.print("Long Snake:");
        Serial.println(LongSnake);
      }else if (e == 109){// m
        MagicMod();
      }else if (e == 92){//
        ChetMod();
      }else if (e == 101 && (magicmod or chetmod)){
        int tab[MATRIX_LED];
        BackDirect();
        for(int i=0;i<=LongSnake;i++){
          tab[i] = Snake[i];
        }
        for(int i=0;i<=LongSnake;i++){
          Snake[i] = tab[LongSnake - i];
        }
        sy = GetPositionY(Snake[LongSnake]);
        sx = GetPositionX(Snake[LongSnake]);
      }else if(e == 99){// c
        ContinueMod();  
      }else if(e == 27){
        ON();
      }else if(e == 47){
        Debug();
      }
      if(debug){
        Serial.print("I received: ");
        Serial.println(e, DEC);
      }
    }
    if (editmod) {
      Serial.print("Type Game: ");
      Serial.println(typegame, DEC);
      point = 0;
      GameStart();
    }
    Check(conw, cona, cons, cond);
    unsigned long delta = millis() - MainTime;
    if (delta > 10 && on) {
      MainTime = millis();
      if(!pause)
        Update((float) delta / 1000.0);
      if (updated) {
        Matrix.clear();
        for (int i = 0; i < LongSnake; i++) {
          Matrix.setPixelColor(Snake[i], 0, 255, 0);
        }
        Matrix.setPixelColor(Snake[LongSnake], 255, 255, 0);
        Matrix.setPixelColor(Apple, 255, 0, 0);
        Matrix.show();
      }
    }
  }
}

int Colide_Anim(int point) {
  for (int i = 0; i < 3; i++) {
    Matrix.setPixelColor(point, 255, 0, 0);
    Matrix.setPixelColor(Snake[LongSnake], 255, 0, 0);
    Matrix.show();
    delay(300);
    Matrix.setPixelColor(point, 0, 255, 0);
    Matrix.setPixelColor(Snake[LongSnake], 255, 255, 0);
    Matrix.show();
    delay(300);
  }
}

int Colide(int a) {
  if (a == Apple) return 1;
  for (int i = 0; i <= LongSnake; i++) {
    if (a == Snake[i]) {
      point = LongSnake - i - 1;
      Colide_Anim(Snake[i]);
      return 2;
    }
  }
  if (colidewall){
    Colide_Anim(-1);
    return 3;
  }
  return 0;
}

int Position(int x, int y) {
  if (x % 2 == 0)
    return (MATRIX_SIDE * x) + y;
  return (MATRIX_SIDE * x) + (MATRIX_SIDE - 1) - y;
}
int GetPositionX(int pos){
  return (pos - (pos % MATRIX_SIDE)) / MATRIX_SIDE; 
}
int GetPositionY(int pos){
  if(GetPositionX(pos) % 2 == 0)
    return pos % 10;
  else
    return (MATRIX_SIDE-1) - (pos % MATRIX_SIDE);
}
int BackDirect(){
  if(LongSnake > 0){
    if(GetPositionY(Snake[0]) - GetPositionY(Snake[1]) == -1 )
      direction = KEY_UP;
    else if(GetPositionY(Snake[0]) - GetPositionY(Snake[1]) == 1)
      direction = KEY_DOWN;
    else if(GetPositionX(Snake[0]) - GetPositionX(Snake[1]) == -1)
      direction = KEY_RIGHT;
    else if(GetPositionX(Snake[0]) - GetPositionX(Snake[1]) == 1)
      direction = KEY_LEFT; 
  }
}

void AppleGen() {
  Apple = Position(random(MATRIX_SIDE - 1), random(MATRIX_SIDE - 1));
  for (int i = 0; i <= LongSnake ; i++) {
    if (Apple == Snake[i]) {
      Apple = Position(random(MATRIX_SIDE - 1), random(MATRIX_SIDE - 1));
      i = 0;
    }
  }
}

void GameOver() {
  DrawNumber(LongSnake);
  delay(3 * 1000);
  GameStart();
}

void GameStart() {
  pause = true;
  for (int i = 0; i < MATRIX_LED; i++) Snake[i] = -1;
  if(continuemod)
    LongSnake = point;
  else
    LongSnake = 0;
  sx = random(MATRIX_SIDE - 1);
  delay(4);
  sy = random(MATRIX_SIDE - 1);
  Snake[0] = Position(sx, sy);
  AppleGen();
  updated = true;
}

void Check(bool up, bool left, bool down,  bool right) {
  if(!on)
    return;
  if (up) {
    if (!KeyUP) {
      direction = KEY_UP;
      KeyUP = true;
      pause = false;
    }
  } else {
    KeyUP = false;
  }
  /////////////////////////////////////////
  if (down) {
    if (!KeyDOWN) {
      direction = KEY_DOWN;
      KeyDOWN = true;
      pause = false;
    }
  } else {
    KeyDOWN = false;
  }
  /////////////////////////////////////////
  if (left) {
    if (!KeyLEFT) {
      direction = KEY_LEFT;
      KeyLEFT = true;
      pause = false;
    }
  } else {
    KeyLEFT = false;
  }
  /////////////////////////////////////////
  if (right) {
    if (!KeyRIGHT) {
      direction = KEY_RIGHT;
      KeyRIGHT = true;
      pause = false;
    }
  } else {
    KeyRIGHT =  false;
  }
}

void Update(float tt) {
  time -= tt;
  if (time > 0) return;
  time = maxTime;
  updated = true;
  switch ( direction) {
    case KEY_DOWN:
      sy++;
      if (sy >= MATRIX_SIDE) {
        if (typegame == 0)
          sy = 0;
        else if (typegame == 1)
          colidewall = true;
      }
      break;
    case KEY_UP:
      sy--;
      if (sy < 0) {
        if (typegame == 0)
          sy = MATRIX_SIDE - 1;
        else if (typegame == 1)
          colidewall = true;
      }
      break;
    case KEY_RIGHT:
      sx--;
      if (sx < 0 ) {
        if (typegame == 0)
          sx = MATRIX_SIDE - 1;
        else if (typegame == 1)
          colidewall = true;
      }
      break;
    case KEY_LEFT:
      sx++;
      if (sx >= MATRIX_SIDE) {
        if (typegame == 0)
          sx = 0;
        else if (typegame == 1)
          colidewall = true;
      }
      break;
  }

  if (Colide(Position(sx, sy)) == 2) {
    Serial.print("Game Over | Wynik:");
    Serial.print(LongSnake);
    Serial.print(" Coolide:");
    Serial.println(point);
    GameOver();
    return;
  } else if (Colide(Position(sx, sy)) == 1) {
    LongSnake++;
    Serial.print("Apple | Wynik:");
    Serial.println(LongSnake);
    if (LongSnake >= MATRIX_LED - 1) {
      Serial.print("Game Full | Wynik:");
      Serial.print(LongSnake);
      GameOver();
      return;
    }
    Snake[LongSnake] = Position(sx, sy);
    AppleGen();
    return;
  } else if (Colide(Position(sx, sy)) == 3) {
    Serial.print("Game Over | Wynik:");
    Serial.print(LongSnake);
    Serial.println(" Coolide Wall");
    colidewall = false;
    GameOver();
    return;
  }

  for (int i = 0; i <= LongSnake; i++) {
    Snake[i] = Snake[i + 1];
  }
  Snake[LongSnake] = Position(sx, sy);
}

int cipari[11][7][4] = {
  { //0
    {1, 1, 1, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 1, 1, 1}
  }, { //1
    {0, 0, 0, 1},
    {0, 0, 1, 1},
    {0, 1, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1}
  }, { //2
    {1, 1, 1, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {1, 1, 1, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 1, 1, 1}
  }, { //3
    {1, 1, 1, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {1, 1, 1, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {1, 1, 1, 1}
  }, { //4
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 1, 1, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1}
  }, { //5
    {1, 1, 1, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {1, 1, 1, 1}
  }, { //6
    {1, 1, 1, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 1, 1, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 1, 1, 1}
  }, { //7
    {1, 1, 1, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1}
  }, { //8
    {1, 1, 1, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 1, 1, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 1, 1, 1},
  }, { //9
    {1, 1, 1, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 1, 1, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {1, 1, 1, 1}
  }
};


void DrawNumber(int num) {
  if (num < 10) {
    corectx = 3;
    corecty = 1;
    ff = 0;
  } else {
    num2 = (num % 10);
    num = ((num - num2) / 10);
    corectx = 0;
    corecty = 1;
    ff = 1;
  }
  Matrix.clear();
  for (int ii = 0; ii <= ff; ii++) {
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 4; j++) {
        if (cipari[num][i][j] > 0) Matrix.setPixelColor(Position((MATRIX_SIDE - 1) - j - corectx, i + corecty), 255, 0, 0);
      }
    }
    corectx = 5;
    num = num2;
  }
  Matrix.show();
}
