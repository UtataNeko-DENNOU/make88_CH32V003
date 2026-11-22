 /**
 * ============================================================================
 * @file    make88_CH32V003_matrix_photo_display.ino
 * @brief   8x8 LED Matrix Multi-Pattern Still Display for Cover Photo & Exhibition
 * @target  CH32V003F4P6 (QingKe 32-bit RISC-V2A / TSSOP20)
 * @author  電脳好奇心@うたたねこ
 * @details
 *   撮影および展示用の静止パターン順送り表示スケッチ。
 *   PSW1(PC0)を押すごとに登録された8x8ビットマップ（ハート、ひらがな「で」など）を順次切り替えます。
 *   露光オーバーやフリッカーを防ぐデューティ比制御パラメータを備えています。
 *
 * 【内部管理情報】
 *   - PCB Rev / Code ID: photo_multipattern_display
 * ============================================================================
 */

#include <Arduino.h>

// 行ピン (上端 PA1 → 下端 PD0 / カソード LOWアクティブ)
const uint8_t rawPins[8] = { PA1, PD7, PD6, PD5, PD4, PD3, PD2, PD0 };

// 列ピン (左端 PC1 → 右端 PA2 / アノード HIGHアクティブ)
const uint8_t colPins[8] = { PC1, PC2, PC3, PC4, PC5, PC6, PC7, PA2 };

// PA1/PA2 GPIOリマップレジスタ
#define REMAP_REG (*(uint32_t *)0x40010004)

// スイッチ入力ピン (PSW1: PC0)
const uint8_t buttonPin = PC0;
int lastButtonState = HIGH;

// =============================================================
// ★ 撮影用 輝度調整パラメータ (数値を小さくすると暗くなります)
// =============================================================
const uint16_t LED_ON_TIME_US  = 400;  // 点灯時間 (目安: 50〜200μs)
const uint16_t LED_OFF_TIME_US = 300;  // 消灯時間 (目安: 400〜600μs)

// =============================================================
// 表示したいパターンリスト（ここへ自由に追加・削除してください）
// =============================================================
const uint8_t patterns[][8] = {
  // 0: ハート
  { 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00 },
  { 0x0C, 0xF8, 0x15, 0x25, 0x20, 0x20, 0x10, 0x0E }, // で
  { 0x3C, 0x42, 0xB9, 0xA9, 0xAB, 0xBC, 0x40, 0x3C} //@
};

// パターンの総数を自動算出（手動で数を指定する必要がありません）
const int PATTERN_COUNT = sizeof(patterns) / sizeof(patterns[0]);

int currentPatternIndex = 0; // 現在表示中のパターンの番号

void setup() {
  delay(200);

  // PA1/PA2 を GPIO に割り当てるリマップ
  REMAP_REG &= ~(1 << 15);

  // LEDの行・列ピン初期化
  for (int i = 0; i < 8; i++) {
    pinMode(rawPins[i], OUTPUT);
    digitalWrite(rawPins[i], HIGH); // 全消灯 (LOWアクティブ)
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], LOW);  // 全消灯 (HIGHアクティブ)
  }

  // スイッチピン初期化（内蔵プルアップ）
  pinMode(buttonPin, INPUT_PULLUP);
  lastButtonState = digitalRead(buttonPin);
}

void loop() {
  // --- 1. ボタン入力検知（立ち下がり検出 ＋ チャタリング防止） ---
  int state = digitalRead(buttonPin);
  if (lastButtonState == HIGH && state == LOW) {
    delay(25); // チャタリング除去待ち
    if (digitalRead(buttonPin) == LOW) {
      // 次のパターンへ進める（末尾まで行ったら0番に戻る）
      currentPatternIndex = (currentPatternIndex + 1) % PATTERN_COUNT;
    }
  }
  lastButtonState = state;

  // --- 2. ダイナミック点灯（静止表示 1/8スキャン駆動） ---
  for (int row = 0; row < 8; row++) {
    uint8_t rowData = patterns[currentPatternIndex][row];

    // 列データ出力
    for (int col = 0; col < 8; col++) {
      bool on = (rowData >> (7 - col)) & 0x01;
      digitalWrite(colPins[col], on ? HIGH : LOW);
    }

    // 1. 行を点灯
    digitalWrite(rawPins[row], LOW);
    delayMicroseconds(LED_ON_TIME_US); // ★ここで発光時間を絞る

    // 2. 行を消灯（ゴースト防止＋減光用消灯ウェイト）
    digitalWrite(rawPins[row], HIGH);
    for (int col = 0; col < 8; col++) {
      digitalWrite(colPins[col], LOW);
    }
    delayMicroseconds(LED_OFF_TIME_US); // ★消灯時間を設けて全体のDuty比を下げる
  }
}