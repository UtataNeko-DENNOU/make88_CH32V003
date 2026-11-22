/**
 * @file    make88_CH32V003_matrix_longpress_gauge.ino
 * @brief   8x8 LED Matrix Single-Button Long-Press Progress Gauge
 * @target  CH32V003F4P6 (QingKe 32-bit RISC-V2A / TSSOP20)
 * @author  電脳好奇心@うたたねこ
 * @details
 *   1ボタンUIの操作性を高めるプログレスゲージ（長押し蓄積）の実装例。
 *   PC0のボタンを押している間、8x8マトリクスの下段から上段へバーが伸び、
 *   約2秒押し切ると満タン達成アニメーション（3回点滅）を発動します。
 *   途中でボタンを離した場合は即座にキャンセルされます。
 *
 * 【ハードウェア接続】
 *   - 行ピン: PA1, PD7, PD6, PD5, PD4, PD3, PD2, PD0 (カソード LOW)
 *   - 列ピン: PC1, PC2, PC3, PC4, PC5, PC6, PC7, PA2 (アノード HIGH)
 *   - ボタン: PC0 (PSW1 / LOWアクティブ)
 *
 * 【内部管理情報】
 *   - PCB Rev / Code ID: make88_102 
 * ============================================================================
 */

#include <Arduino.h>

// 行ピン (上端 PA1 → 下端 PD0)
const uint8_t rawPins[8] = { PA1, PD7, PD6, PD5, PD4, PD3, PD2, PD0 };

// 列ピン (左端 PC1 → 右端 PA2)
const uint8_t colPins[8] = { PC1, PC2, PC3, PC4, PC5, PC6, PC7, PA2 };

const uint8_t btnPin = PC0;

#define REMAP_REG (*(uint32_t *)0x40010004)

// 全消灯
void clearAll() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(rawPins[i], HIGH);
    digitalWrite(colPins[i], LOW);
  }
}

// 下から currentLevel 行分をダイナミック点灯 (level: 0〜8)
void displayGauge(int level, unsigned long durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    if (level == 0) {
      clearAll();
      delayMicroseconds(500);
      continue;
    }

    // 行7 (下端) から上に向かって点灯
    for (int row = 7; row >= (8 - level); row--) {
      for (int c = 0; c < 8; c++) digitalWrite(colPins[c], HIGH);
      digitalWrite(rawPins[row], LOW);
      delayMicroseconds(400);

      digitalWrite(rawPins[row], HIGH);
      for (int c = 0; c < 8; c++) digitalWrite(colPins[c], LOW);
    }
  }
}

void setup() {
  delay(500);
  REMAP_REG &= ~(1 << 15);

  for (int i = 0; i < 8; i++) {
    pinMode(rawPins[i], OUTPUT);
    pinMode(colPins[i], OUTPUT);
  }
  pinMode(btnPin, INPUT);
  clearAll();
}

void loop() {
  // ボタンが押されているか確認 (LOW = 押下)
  if (digitalRead(btnPin) == LOW) {
    int gaugeLevel = 0;

    // ボタンを押している間、徐々にゲージを増やす（約2秒で満タン）
    while (digitalRead(btnPin) == LOW && gaugeLevel <= 8) {
      displayGauge(gaugeLevel, 250); // 1ステップあたり250ms
      gaugeLevel++;
    }

    // 8行すべて溜まった（長押し成功）場合のアクション
    if (gaugeLevel > 8) {
      // 達成のピカピカ点滅（3回）
      for (int k = 0; k < 3; k++) {
        displayGauge(8, 150);
        clearAll();
        delay(150);
      }
      // ボタンが離されるまで待機
      while (digitalRead(btnPin) == LOW);
      delay(50);
    }
  } else {
    // 待機中は消灯
    clearAll();
    delay(10);
  }
}