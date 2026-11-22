/**
 * ============================================================================
 * @file    make88_CH32V003_matrix_scroll_complete.ino
 * @brief   8x8 LED Matrix Dynamic Drive & Smooth Japanese/English Message Scroller
 * @target  CH32V003F4P6 (QingKe 32-bit RISC-V2A / TSSOP20)
 * @author  電脳好奇心@うたたねこ
 * @details
 *   タイマー割り込み駆動＋ダブルバッファリングによる文字スクロール完全版。
 *   - ひらがな、英大文字・小文字、数字、基本記号、長音符のUTF-8完全デコード
 *   - 英字（A〜Z, a〜z）は横6列固定幅＋字間1列（計7列）でスリムに展開
 *   - 濁音・半濁音の分離合成（清音8列＋記号3列＋字間1列＝計12列）による高視認性表示
 *   - 自作ひらがなフォント(font_data.h) ＆ 6列幅最適化英数(font_abc_data.h)対応
 *   - PSW1(PC0)によるメッセージ切り替えとデバウンス処理
 *   - PA1/PA2 GPIOリマップ対応
 *   - スイッチ切替・チャタリング防止対応
 *
 * 【内部管理情報】
 *   - PCB Rev / Code ID: make81_CH32V003_matrix_ALL_COMPLETE
 * ============================================================================
 */

#include <Arduino.h>
#include "font_data.h"      // 記号・数字・ひらがな・長音符
#include "font_abc_data.h"  // 大文字 A〜Z・小文字 a〜z

// 行ピン (上端 PA1 → 下端 PD0 / カソード LOWアクティブ)
const uint8_t rawPins[8] = { PA1, PD7, PD6, PD5, PD4, PD3, PD2, PD0 };

// 列ピン (左端 PC1 → 右端 PA2 / アノード HIGHアクティブ)
const uint8_t colPins[8] = { PC1, PC2, PC3, PC4, PC5, PC6, PC7, PA2 };

// PA1/PA2 GPIOリマップ用レジスタ
#define REMAP_REG (*(uint32_t *)0x40010004)

// スイッチ入力ピン
const uint8_t buttonPin = PC0;
int lastButtonState = HIGH;

// --- 表示メッセージリスト ---
const char* const messages[] = {
  " 11がつ23にち ぎじゅつしょてん おふらいん かいさい! ",
  " ぎじゅつしょてん Offline さんしゃいんしてぃ ",
  " ぼたんでんちで もじがながれる! ",
  " ごくしょうLEDがじえっとせいさくき ",
  " でんのうこうきしん@うたたねこ ",
  " すうじ 1234567890 1+2*3/4=2.5 ",
  " English: Hello World! ABC abc 123 ",
  " ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwuxyz",
  " !\"#$%&'()*+,-./:;<=>?@ ",
  " がぎぐげござじずぜぞだぢづでどばびぶべぼぱぴぷぺぽ ",
  " らーめん すぽーつ ぁぃぅぇぉっゃゅょゎ "
};

const int MSG_COUNT = sizeof(messages) / sizeof(messages[0]);
int currentMsg = 0;

// -------------------------------------------------------------
// 濁点・半濁点の3列ドットパターン定義 (縦8行 × 横3列)
// -------------------------------------------------------------
enum DiacriticType {
  DIACRITIC_NONE = 0,
  DIACRITIC_DAKUTEN,    // 濁点「゛」
  DIACRITIC_HANDAKUTEN  // 半濁点「゜」
};

// 濁点「゛」パターン（右上の1〜3行目を点灯）
const uint8_t DAKUTEN_PAT[3] = {
  0x06, // 1列目: 0b00000110
  0x00, // 2列目: 0b00000000
  0x06  // 3列目: 0b00000110
};

// 半濁点「゜」パターン（右上に2×2の丸を配置）
const uint8_t HANDAKUTEN_PAT[3] = {
  0x07, // 1列目: 0b00000111
  0x05, // 2列目: 0b00000101
  0x07  // 3列目: 0b00000111
};

// 濁音・半濁音を「清音」と「記号種別」に分解する関数
uint16_t decomposeHiragana(uint16_t unicode, DiacriticType &diacritic) {
  diacritic = DIACRITIC_NONE;

  switch (unicode) {
    // か行 濁音
    case 0x304C: diacritic = DIACRITIC_DAKUTEN; return 0x304B; // が -> か
    case 0x304E: diacritic = DIACRITIC_DAKUTEN; return 0x304D; // ぎ -> き
    case 0x3050: diacritic = DIACRITIC_DAKUTEN; return 0x304F; // ぐ -> く
    case 0x3052: diacritic = DIACRITIC_DAKUTEN; return 0x3051; // げ -> け
    case 0x3054: diacritic = DIACRITIC_DAKUTEN; return 0x3053; // ご -> こ

    // さ行 濁音
    case 0x3056: diacritic = DIACRITIC_DAKUTEN; return 0x3055; // ざ -> さ
    case 0x3058: diacritic = DIACRITIC_DAKUTEN; return 0x3057; // じ -> し
    case 0x305A: diacritic = DIACRITIC_DAKUTEN; return 0x3059; // ず -> す
    case 0x305C: diacritic = DIACRITIC_DAKUTEN; return 0x305B; // ぜ -> せ
    case 0x305E: diacritic = DIACRITIC_DAKUTEN; return 0x305D; // ぞ -> そ

    // た行 濁音
    case 0x3060: diacritic = DIACRITIC_DAKUTEN; return 0x305F; // だ -> た
    case 0x3062: diacritic = DIACRITIC_DAKUTEN; return 0x3061; // ぢ -> ち
    case 0x3065: diacritic = DIACRITIC_DAKUTEN; return 0x3064; // づ -> つ
    case 0x3067: diacritic = DIACRITIC_DAKUTEN; return 0x3066; // で -> て
    case 0x3069: diacritic = DIACRITIC_DAKUTEN; return 0x3068; // ど -> と

    // は行 濁音
    case 0x3070: diacritic = DIACRITIC_DAKUTEN; return 0x306F; // ば -> は
    case 0x3073: diacritic = DIACRITIC_DAKUTEN; return 0x3072; // び -> ひ
    case 0x3076: diacritic = DIACRITIC_DAKUTEN; return 0x3075; // ぶ -> ふ
    case 0x3079: diacritic = DIACRITIC_DAKUTEN; return 0x3078; // べ -> へ
    case 0x307C: diacritic = DIACRITIC_DAKUTEN; return 0x307B; // ぼ -> ほ

    // は行 半濁音
    case 0x3071: diacritic = DIACRITIC_HANDAKUTEN; return 0x306F; // ぱ -> は
    case 0x3074: diacritic = DIACRITIC_HANDAKUTEN; return 0x3072; // ぴ -> ひ
    case 0x3077: diacritic = DIACRITIC_HANDAKUTEN; return 0x3075; // ぷ -> ふ
    case 0x307A: diacritic = DIACRITIC_HANDAKUTEN; return 0x3078; // ぺ -> へ
    case 0x307D: diacritic = DIACRITIC_HANDAKUTEN; return 0x307B; // ぽ -> ほ

    default:
      return unicode;
  }
}

// Unicodeからfont_data.hのインデックスを取得
int getFontIndex(uint16_t unicode) {
  // 長音符「ー」(0x30FC)
  if (unicode == 0x30FC) return 115;

  // 1. 記号 !〜/ (Unicode 33-47) -> Index 0〜14
  if (unicode >= 0x21 && unicode <= 0x2F) return unicode - 0x21;

  // 2. 数字 0〜9 (Unicode 48-57) -> Index 15〜24
  if (unicode >= 0x30 && unicode <= 0x39) return (unicode - 0x30) + 15;

  // 3. 記号 :〜@ (Unicode 58-64) -> Index 25〜31
  if (unicode >= 0x3A && unicode <= 0x40) return (unicode - 0x3A) + 25;

  // 4. ひらがな ぁ〜ん (Unicode 12353-12435) -> Index 32〜114
  if (unicode >= 0x3041 && unicode <= 0x3093) return (unicode - 0x3041) + 32;

  return -1; // 未登録文字（空白扱い）
}

// 該当するフォントの8バイトデータへのポインタを取得
const uint8_t* getFontPattern(uint16_t unicode) {
  // --- 全角文字の正規化 ---
  if (unicode >= 0xFF10 && unicode <= 0xFF19) unicode = 0x30 + (unicode - 0xFF10); // 全角０〜９
  if (unicode == 0xFF01) unicode = 0x21; // 全角！ -> 半角!
  if (unicode == 0xFF1F) unicode = 0x3F; // 全角？ -> 半角?

  // --- 英大文字 A〜Z (0x41〜0x5A) -> font_abc[0〜25] ---
  if (unicode >= 0x41 && unicode <= 0x5A) {
    return font_abc[unicode - 0x41];
  }

  // --- 英小文字 a〜z (0x61〜0x7A) -> font_abc[26〜51] ---
  if (unicode >= 0x61 && unicode <= 0x7A) {
    return font_abc[(unicode - 0x61) + 26];
  }

  // --- 記号・数字・ひらがな (font_data.h) ---
  int idx = getFontIndex(unicode);
  if (idx >= 0) {
    return font[idx];
  }

  return NULL; // 未登録文字
}

// 90°左回転
void rotateLeft90(const uint8_t* src, uint8_t* dst) {
  if (src == NULL) {
    for (int i = 0; i < 8; i++) dst[i] = 0;
    return;
  }
  for (int y = 0; y < 8; y++) {
    dst[y] = 0;
    for (int x = 0; x < 8; x++) {
      if (src[x] & (1 << (7 - y))) {
        dst[y] |= (1 << x);
      }
    }
  }
}

uint8_t scrollBuffer[256];
int scrollLen = 0;

// UTF-8文字列を解析しながらバッファを作成
void buildScrollBuffer(const char* msg) {
  int pos = 0;
  int i = 0;

  // 開始余白（画面幅8ドット消灯）
  for (int s = 0; s < 8; s++) {
    if (pos < sizeof(scrollBuffer)) scrollBuffer[pos++] = 0x00;
  }

  while (msg[i] != '\0') {
    uint16_t unicode = 0;
    uint8_t b = msg[i];

    if (b < 0x80) { // 1バイト (ASCII)
      unicode = b;
      i += 1;
    } else if ((b & 0xE0) == 0xC0) { // 2バイト
      unicode = ((b & 0x1F) << 6) | (msg[i+1] & 0x3F);
      i += 2;
    } else if ((b & 0xF0) == 0xE0) { // 3バイト (日本語)
      unicode = ((b & 0x0F) << 12) | ((msg[i+1] & 0x3F) << 6) | (msg[i+2] & 0x3F);
      i += 3;
    } else { 
      i++; 
      continue; 
    }

    // スペース処理（半角/全角）：3列の空白
    if (unicode == 0x20 || unicode == 0x3000) {
      for (int s = 0; s < 3; s++) {
        if (pos < sizeof(scrollBuffer)) scrollBuffer[pos++] = 0x00;
      }
      continue;
    }

    // 濁音・半濁音の分解チェック
    DiacriticType diacritic = DIACRITIC_NONE;
    uint16_t baseUnicode = decomposeHiragana(unicode, diacritic);

    // ① 文字本体の展開
    uint8_t tmp[8];
    rotateLeft90(getFontPattern(baseUnicode), tmp);

    // ★ 英字（A〜Z, a〜z）は横6列、それ以外（ひらがな・数字・記号）は8列展開
    int colWidth = ((baseUnicode >= 'A' && baseUnicode <= 'Z') || 
                    (baseUnicode >= 'a' && baseUnicode <= 'z')) ? 6 : 8;

    for (int c = 0; c < colWidth; c++) {
      if (pos < sizeof(scrollBuffer)) scrollBuffer[pos++] = tmp[c];
    }

    // ② 濁音または半濁音なら、直後に3列を付加 (合計 8 + 3 = 11列)
    if (diacritic == DIACRITIC_DAKUTEN) {
      for (int c = 0; c < 3; c++) {
        if (pos < sizeof(scrollBuffer)) scrollBuffer[pos++] = DAKUTEN_PAT[c];
      }
    } else if (diacritic == DIACRITIC_HANDAKUTEN) {
      for (int c = 0; c < 3; c++) {
        if (pos < sizeof(scrollBuffer)) scrollBuffer[pos++] = HANDAKUTEN_PAT[c];
      }
    }

    // ③ 共通の文字間スペース（どの文字の後にも必ず1列挿入）
    if (pos < sizeof(scrollBuffer)) scrollBuffer[pos++] = 0x00;
  }

  // 終了余白（画面外へ流し切るための8列消灯）
  for (int s = 0; s < 8; s++) {
    if (pos < sizeof(scrollBuffer)) scrollBuffer[pos++] = 0x00;
  }

  scrollLen = pos;
}

void scrollDisplay(uint16_t speed) {
  if (scrollLen < 8) return;

  for (int offset = 0; offset <= scrollLen - 8; offset++) {
    unsigned long start = millis();
    while (millis() - start < speed) {
      // ボタン検出（立ち下がり ＋ チャタリング防止）
      int state = digitalRead(buttonPin);
      if (lastButtonState == HIGH && state == LOW) {
        delay(20);
        if (digitalRead(buttonPin) == LOW) {
          currentMsg = (currentMsg + 1) % MSG_COUNT;
          buildScrollBuffer(messages[currentMsg]);
          lastButtonState = LOW;
          return; // 新メッセージでスクロール再開
        }
      }
      lastButtonState = state;

      // ダイナミック点灯スキャン（1/8駆動）
      for (int row = 0; row < 8; row++) {
        digitalWrite(rawPins[row], LOW); // 行選択 ON
        for (int col = 0; col < 8; col++) {
          bool on = (scrollBuffer[offset + col] >> row) & 0x01;
          digitalWrite(colPins[col], on ? HIGH : LOW);
        }
        delayMicroseconds(600); // 輝度調整
        digitalWrite(rawPins[row], HIGH); // ゴースト防止消灯
        for (int col = 0; col < 8; col++) {
          digitalWrite(colPins[col], LOW);
        }
      }
    }
  }
}

void setup() {
  delay(200);

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

  buildScrollBuffer(messages[currentMsg]);
}

void loop() {
  scrollDisplay(100); // 1列進む速度(ms)
}