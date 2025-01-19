/*
Arduino IDE
MCU: Arduino Nano Atmega 328
LED Strip: WS2812B, 164 LEDs (0 to 163)

Функции и особенности:
1. Управление светодиодной лентой с помощью кнопок:
   - BUTTON_MODE (пин 9): переключение режимов работы
   - BUTTON_BRIGHT_UP (пин 10): увеличение яркости
   - BUTTON_BRIGHT_DOWN (пин 11): уменьшение яркости
   - LED_ON_OFF (пин 12): включение и выключение ленты
2. 9 режимов работы, включая бегущие огоньки с затуханием (режимы 5 и 9).
3. Используется библиотека FastLED для управления лентой.

Состояние пинов кнопок:
- LOW: Кнопка не нажата
- HIGH: Кнопка нажата
*/

#include <FastLED.h>

#define NUM_LEDS 164
#define DATA_PIN 13
#define BUTTON_MODE 9
#define BUTTON_BRIGHT_UP 10
#define BUTTON_BRIGHT_DOWN 11
#define LED_ON_OFF 12

CRGB leds[NUM_LEDS];
int brightness = 80; // Начальная яркость (0-255)
bool ledOn = true;    // Состояние включения светодиодов
int mode = 0;         // Текущий режим

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);

  pinMode(BUTTON_MODE, INPUT_PULLUP);
  pinMode(BUTTON_BRIGHT_UP, INPUT_PULLUP);
  pinMode(BUTTON_BRIGHT_DOWN, INPUT_PULLUP);
  pinMode(LED_ON_OFF, INPUT_PULLUP);

  Serial.begin(9600); // Инициализация Serial Monitor
  Serial.println("Система запущена. Режим: 0 (Выключено)");
}

void loop() {
  handleButtons(); // Обработка кнопок

  if (ledOn) {
    switch (mode) {
      case 1: blur(); break;
      case 2: fadeColor(CRGB::Green); break;
      case 3: fadeColor(CRGB::Red); break;
      case 4: fadeColor(CRGB::Blue); break;
      case 5: runningSnake(); break;
      case 6: trailingRunner(); break; // Изменён режим 5
      case 7: constantColor(CRGB::Red); break;
      case 8: constantColor(CRGB::Blue); break;
      case 9: constantColor(CRGB::Green); break;
      case 10: runningLights(); break;
      case 11: blur(); break;
      default: FastLED.clear(); FastLED.show(); break;
    }
  } else {
    FastLED.clear(); // Выключить все светодиоды
    FastLED.show();
  }
}

void handleButtons() {
  if (digitalRead(BUTTON_MODE) == HIGH) {
    mode = (mode + 1); // Переключение между 10 режимами (0 - выключено)
    if (mode >= 12 ) {
      mode = 0;  
    }
    Serial.print("Режим изменён: ");
    Serial.println(mode);
    delay(200); // Защита от дребезга
  }

  if (digitalRead(BUTTON_BRIGHT_UP) == HIGH) {
    brightness = min(brightness + 16, 255);
    FastLED.setBrightness(brightness);
    Serial.print("Яркость увеличена: ");
    Serial.println(brightness);
    delay(200);
  }

  if (digitalRead(BUTTON_BRIGHT_DOWN) == HIGH) {
    brightness = max(brightness - 16, 0);
    FastLED.setBrightness(brightness);
    Serial.print("Яркость уменьшена: ");
    Serial.println(brightness);
    delay(200);
  }

  if (digitalRead(LED_ON_OFF) == HIGH) {
    ledOn = !ledOn;
    Serial.print("Состояние ленты: ");
    Serial.println(ledOn ? "Включено" : "Выключено");
    delay(200);
  }
}

// Режимы работы
// режим 1 - поочередная смена всех режимов с задержкой
void auto_mode(){
 //

}

void fadeColor(CRGB color) {
  static int fadeAmount = 5;
  static int currentBrightness = 0;
  static bool increasing = true;

  if (increasing) {
    currentBrightness += fadeAmount;
    if (currentBrightness >= 255) {
      increasing = false;
      currentBrightness = 255;
    }
  } else {
    currentBrightness -= fadeAmount;
    if (currentBrightness <= 0) {
      increasing = true;
      currentBrightness = 0;
    }
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
    leds[i].nscale8(currentBrightness); // Применение яркости
  }
  FastLED.show();
  delay(30);
}

void runningSnake() {
  static int position = 0;
  FastLED.clear();

  for (int i = 0; i < 15; i++) {
    int ledIndex = (position + i) % NUM_LEDS;
    leds[ledIndex] = CHSV((ledIndex * 10) % 255, 255, 255); // Плавно меняющиеся цвета
  }

  position++;
  FastLED.show();
  delay(50);
}

// Новый режим 5: бегущий огонек с затухающим следом
void trailingRunner() {
  static int position = 0;
  FastLED.clear();

  // Основной огонёк
  leds[position] = CRGB::White;

  // Затухающий след
  for (int i = 1; i <= 10; i++) {
    int fadeIndex = (position - i + NUM_LEDS) % NUM_LEDS; // Учёт переполнения индекса
    uint8_t fadeBrightness = 255 - (i * 25); // Затухание яркости
    leds[fadeIndex] = CRGB::White;
    leds[fadeIndex].nscale8(fadeBrightness); // Применение яркости для затухания
  }

  position = (position + 1) % NUM_LEDS; // Сдвиг позиции
  FastLED.show();
  delay(50);
}

void constantColor(CRGB color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();
}


void runningLights() {
  static int position = 0;                 // Текущая позиция огонька
  static int currentColorIndex = 0;        // Индекс текущего цвета
  static CRGB colors[] = {CRGB::White, CRGB::Red, CRGB::Blue, CRGB::Yellow, CRGB::Green, CRGB::Purple};

  FastLED.clear();

  // Устанавливаем огоньки с затуханием
  for (int i = 0; i < NUM_LEDS; i += 10) {
    int ledIndex = (position + i) % NUM_LEDS;
    for (int fadeStep = 0; fadeStep < 10; fadeStep++) {
      int fadeIndex = (ledIndex + fadeStep) % NUM_LEDS;
      uint8_t brightness = 255 - (fadeStep * 25); // Плавное затухание
      leds[fadeIndex] = colors[currentColorIndex];
      leds[fadeIndex].nscale8(brightness);
    }
  }

  FastLED.show();
  delay(50);

  // Перемещаем позицию огонька
  position = (position + 1) % NUM_LEDS;

  // Меняем цвет после завершения полного прохода
  if (position == 0) {
    currentColorIndex = (currentColorIndex + 1) % 6;
  }

}

// новый режим Blur
void blur() {
  static uint8_t pos = 0;
  static bool toggle = false;
  // Add a bright pixel that moves
  leds[pos] = CHSV(pos * 2, 255, 255);
  // Blur the entire strip
  blur1d(leds, NUM_LEDS, 172);
  fadeToBlackBy(leds, NUM_LEDS, 16);
  FastLED.show();
  // Move the position of the dot
  if (toggle) {
    pos = (pos + 1) % NUM_LEDS;
  }
  toggle = !toggle;
  delay(20);

}

 
