/**
 * @file    make88_CH32V003_matrix_led_all_patterns.ino
 * @brief   8x8 LEDマトリクス 動作確認テスト
 * @target  CH32V003F4P6 (WCH RISC-V マイコン)
 * @details 
 *   PSW1(PC0)のプッシュスイッチを押すごとに、以下の各テストを順送りで切り替えます。
 *     [Mode 0]  斜め対角線スキャン (1〜15) ＆ 外周スパイラルスキャン
 *     [Mode 1]  同心正方形の波紋（アップ／ダウンスキャン）
 *     [Mode 2]  市松模様（チェッカーボード）反転点滅（ハンダショート検査）
 *     [Mode 3]  縦横バー走査（横5回 ⇄ 縦5回　繰り返し）
 *     [Mode 4]  64ドット全点滅（電源負荷・BOD耐性ストレステスト）
 *
 * 【ハードウェア接続】
 *  - 行ピン (Row / カソード側 / LOWアクティブ):
 *      Row 0 (上端) -> PA1 (要GPIOリマップ)
 *      Row 1        -> PD7 (Option ByteでNRSTからGPIO変更済み)
 *      Row 2        -> PD6
 *      Row 3        -> PD5
 *      Row 4        -> PD4
 *      Row 5        -> PD3
 *      Row 6        -> PD2
 *      Row 7 (下端) -> PD0
 *
 *  - 列ピン (Col / アノード側 / HIGHアクティブ):
 *      Col 0 (左端) -> PC1
 *      Col 1        -> PC2
 *      Col 2        -> PC3
 *      Col 3        -> PC4
 *      Col 4        -> PC5
 *      Col 5        -> PC6
 *      Col 6        -> PC7
 *      Col 7 (右端) -> PA2 (要GPIOリマップ)
 *
 *  - スイッチ入力ピン:
 *      PSW1         -> PC0 (LOWアクティブ / 外部または内部プルアップ)
 */

#include <Arduino.h>

// 行ピン (上端 PA1 → 下端 PD0 / カソード LOWアクティブ)
const uint8_t rawPins[8] = { PA1, PD7, PD6, PD5, PD4, PD3, PD2, PD0 };

// 列ピン (左端 PC1 → 右端 PA2 / アノード HIGHアクティブ)
const uint8_t colPins[8] = { PC1, PC2, PC3, PC4, PC5, PC6, PC7, PA2 };

// スイッチ入力ピン (PSW1)
const uint8_t buttonPin = PC0;
int lastButtonState = HIGH;

// PA1/PA2 GPIOリマップレジスタ
#define REMAP_REG (*(uint32_t *)0x40010004)

// モード管理 (0〜4)
const int MODE_COUNT = 5;
volatile int currentMode = 0;

// 全消灯
void clearAll() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(rawPins[i], HIGH);
    digitalWrite(colPins[i], LOW);
  }
}

// 1ドットのみ点灯
void lightPixel(int row, int col) {
  clearAll();
  digitalWrite(rawPins[row], LOW);
  digitalWrite(colPins[col], HIGH);
}

// =============================================================
// スイッチチェック共通関数
// 戻り値: ボタンが押されてモードが進んだ場合 true（アニメーション中断用）
// =============================================================
bool checkButtonTrigger() {
  int state = digitalRead(buttonPin);
  if (lastButtonState == HIGH && state == LOW) {
    delay(20); // チャタリング除去
    if (digitalRead(buttonPin) == LOW) {
      currentMode = (currentMode + 1) % MODE_COUNT;
      clearAll();
      lastButtonState = LOW;
      // ボタンが離されるのを待つ
      while (digitalRead(buttonPin) == LOW);
      delay(20);
      return true; // 切り替え発生
    }
  }
  lastButtonState = state;
  return false;
}

// ボタン監視付きのミリ秒待機関数
bool interruptibleDelay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    if (checkButtonTrigger()) return true;
    delayMicroseconds(500);
  }
  return false;
}

// =============================================================
// Mode 0: 斜めバー ＆ スパイラルスキャン
// =============================================================
bool runDiagonalStep(int step, unsigned long durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    if (checkButtonTrigger()) return true;
    for (int row = 0; row < 8; row++) {
      int col = step - 1 - row;
      if (col >= 0 && col < 8) {
        digitalWrite(rawPins[row], LOW);
        digitalWrite(colPins[col], HIGH);
        delayMicroseconds(500);
        digitalWrite(rawPins[row], HIGH);
        digitalWrite(colPins[col], LOW);
      }
    }
  }
  return false;
}

bool runDiagonalStep2(int step, unsigned long durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    if (checkButtonTrigger()) return true;
    for (int row = 0; row < 8; row++) {
      int col = row + 8 - step;
      if (col >= 0 && col < 8) {
        digitalWrite(rawPins[row], LOW);
        digitalWrite(colPins[col], HIGH);
        delayMicroseconds(500);
        digitalWrite(rawPins[row], HIGH);
        digitalWrite(colPins[col], LOW);
      }
    }
  }
  return false;
}

bool runSpiralScan(unsigned long pixelMs) {
  int top = 0, bottom = 7, left = 0, right = 7;
  while (top <= bottom && left <= right) {
    for (int col = left; col <= right; col++) {
      lightPixel(top, col);
      if (interruptibleDelay(pixelMs)) return true;
    }
    top++;
    for (int row = top; row <= bottom; row++) {
      lightPixel(row, right);
      if (interruptibleDelay(pixelMs)) return true;
    }
    right--;
    if (top <= bottom) {
      for (int col = right; col >= left; col--) {
        lightPixel(bottom, col);
        if (interruptibleDelay(pixelMs)) return true;
      }
      bottom--;
    }
    if (left <= right) {
      for (int row = bottom; row >= top; row--) {
        lightPixel(row, left);
        if (interruptibleDelay(pixelMs)) return true;
      }
      left++;
    }
  }
  return false;
}

void mode0_DiagonalAndSpiral() {
  for (int step = 1; step <= 15; step++) {
    if (runDiagonalStep(step, 80)) return;
  }
  clearAll();
  if (interruptibleDelay(150)) return;

  for (int step = 1; step <= 15; step++) {
    if (runDiagonalStep2(step, 80)) return;
  }
  clearAll();
  if (interruptibleDelay(150)) return;

  if (runSpiralScan(25)) return;
  clearAll();
  interruptibleDelay(300);
}

// =============================================================
// Mode 1: 波紋（四角枠同心アップ／ダウン）
// =============================================================
const uint8_t layerBitmask[5][8] = {
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF }, // Layer 1 (8x8)
  { 0x00, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x7E, 0x00 }, // Layer 2 (6x6)
  { 0x00, 0x00, 0x3C, 0x24, 0x24, 0x3C, 0x00, 0x00 }, // Layer 3 (4x4)
  { 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00 }  // Layer 4 (2x2)
};

bool runLayerDisplay(int layer, unsigned long durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    if (checkButtonTrigger()) return true;
    for (int row = 0; row < 8; row++) {
      uint8_t rowPattern = layerBitmask[layer][row];
      if (rowPattern == 0) continue;
      for (int col = 0; col < 8; col++) {
        digitalWrite(colPins[col], (rowPattern >> (7 - col)) & 0x01 ? HIGH : LOW);
      }
      digitalWrite(rawPins[row], LOW);
      delayMicroseconds(600);
      digitalWrite(rawPins[row], HIGH);
      for (int col = 0; col < 8; col++) digitalWrite(colPins[col], LOW);
    }
  }
  return false;
}

void mode1_Ripple() {
  const int stepDelay = 120;
  for (int layer = 1; layer <= 4; layer++) {
    if (runLayerDisplay(layer, stepDelay)) return;
  }
  for (int layer = 3; layer >= 2; layer--) {
    if (runLayerDisplay(layer, stepDelay)) return;
  }
}

// =============================================================
// Mode 2: 市松模様反転
// =============================================================
const uint8_t checkerPatterns[2][8] = {
  { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 },
  { 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA }
};

void mode2_Checkerboard() {
  for (int p = 0; p < 2; p++) {
    unsigned long start = millis();
    while (millis() - start < 400) {
      if (checkButtonTrigger()) return;
      for (int row = 0; row < 8; row++) {
        uint8_t pattern = checkerPatterns[p][row];
        for (int col = 0; col < 8; col++) {
          digitalWrite(colPins[col], (pattern >> (7 - col)) & 1 ? HIGH : LOW);
        }
        digitalWrite(rawPins[row], LOW);
        delayMicroseconds(600);
        digitalWrite(rawPins[row], HIGH);
        for (int col = 0; col < 8; col++) digitalWrite(colPins[col], LOW);
      }
    }
  }
}

// =============================================================
// Mode 3:行点灯・列点灯（横バー5回 ⇄ 縦バー5回）
// =============================================================
bool runHorizontalBar(unsigned long stepMs) {
  for (int targetRow = 0; targetRow < 8; targetRow++) {
    unsigned long start = millis();
    while (millis() - start < stepMs) {
      if (checkButtonTrigger()) return true;
      for (int c = 0; c < 8; c++) digitalWrite(colPins[c], HIGH);
      digitalWrite(rawPins[targetRow], LOW);
      delayMicroseconds(600);
      digitalWrite(rawPins[targetRow], HIGH);
      for (int c = 0; c < 8; c++) digitalWrite(colPins[c], LOW);
    }
  }
  return false;
}

bool runVerticalBar(unsigned long stepMs) {
  for (int targetCol = 0; targetCol < 8; targetCol++) {
    unsigned long start = millis();
    while (millis() - start < stepMs) {
      if (checkButtonTrigger()) return true;
      for (int r = 0; r < 8; r++) {
        digitalWrite(colPins[targetCol], HIGH);
        digitalWrite(rawPins[r], LOW);
        delayMicroseconds(200);
        digitalWrite(rawPins[r], HIGH);
        digitalWrite(colPins[targetCol], LOW);
      }
    }
  }
  return false;
}

void mode3_BarScan() {
  for (int i = 0; i < 5; i++) {
    if (runHorizontalBar(70)) return;
  }
  clearAll();
  if (interruptibleDelay(150)) return;

  for (int i = 0; i < 5; i++) {
    if (runVerticalBar(70)) return;
  }
  clearAll();
  interruptibleDelay(300);
}

// =============================================================
// Mode 4:全点滅（電源負荷・ストレステスト）
// =============================================================
void mode4_AllBlinkStress() {
  // 全点灯（約1秒間走査）
  unsigned long start = millis();
  while (millis() - start < 1000) {
    if (checkButtonTrigger()) return;
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) digitalWrite(colPins[col], HIGH);
      digitalWrite(rawPins[row], LOW);
      delayMicroseconds(600);
      digitalWrite(rawPins[row], HIGH);
      for (int col = 0; col < 8; col++) digitalWrite(colPins[col], LOW);
    }
  }

  // 全消灯（0.4秒休止）
  clearAll();
  interruptibleDelay(400);
}

// =============================================================
// Arduino 基本関数 (setup / loop)
// =============================================================
void setup() {
  delay(300); // 起動安定待ち

  // PA1/PA2 を GPIO に変更するリマップ
  REMAP_REG &= ~(1 << 15);

  for (int i = 0; i < 8; i++) {
    pinMode(rawPins[i], OUTPUT);
    digitalWrite(rawPins[i], HIGH);
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], LOW);
  }

  pinMode(buttonPin, INPUT_PULLUP);
  lastButtonState = digitalRead(buttonPin);

  clearAll();
}

void loop() {
  switch (currentMode) {
    case 0:
      mode0_DiagonalAndSpiral(); // 002
      break;
    case 1:
      mode1_Ripple();            // 003
      break;
    case 2:
      mode2_Checkerboard();      // 004
      break;
    case 3:
      mode3_BarScan();           // 005
      break;
    case 4:
      mode4_AllBlinkStress();    // 006
      break;
    default:
      currentMode = 0;
      break;
  }
}