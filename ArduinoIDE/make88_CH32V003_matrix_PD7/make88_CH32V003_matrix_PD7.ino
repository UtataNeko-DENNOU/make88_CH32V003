/**
 * 【設定専用】CH32V003 PD7(NRST) GPIO化スケッチ
 * ※実行するとNRSTピンが通常のGPIO入力/出力になります。
 *  再書き込み時はWCH-LinkEの3.3V/GNDピン抜き差しリセット等が必要になる場合があります。
 */
#include <ch32v00x.h>

void setup() {
  // すでにPD7化されているか確認 (0x18がセットされていればスキップ)
  if ((OB->USER & 0x18) != 0x18) {
    // FLASH/OB アンロック
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;
    FLASH->OBKEYR = 0x45670123;
    FLASH->OBKEYR = 0xCDEF89AB;

    while (FLASH->STATR & FLASH_STATR_BSY);

    FLASH->CTLR |= FLASH_CTLR_OPTPG;
    FLASH->CTLR |= FLASH_CTLR_STRT;

    // NRSTピンを通常のGPIO(PD7)へ割り当て
    OB->USER |= 0x18;

    while (FLASH->STATR & FLASH_STATR_BSY);

    FLASH->CTLR &= ~FLASH_CTLR_OPTPG;

    // 変更を反映するためソフトリセット
    NVIC_SystemReset();
  }
}

void loop() {
  // 書き込み完了後は何もしない
}