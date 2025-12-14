NSC – Nintendo Switch Controller for ESP32-S3 (USB HID)
===
NSC は **ESP32-S3 を Nintendo Switch の有線コントローラとして認識させる**
低レイヤー向け Arduino ライブラリです。

本ライブラリは「実行器」に徹し、
人間にとって扱いやすい表現（DSL / 角度指定 / 強度指定など）は
**外部ツールで変換してから使用する**ことを前提としています。

## 注意事項
- `USB.begin()` はユーザーコードで呼ばないでください（NSC 内部で呼ばれます）
- 自己責任で使用してください

## 動作確認
- Nintendo Switch Lite
- M5AtomS3 Lite（**ESP32-S3**）
- USB 有線接続

上記の組み合わせでテストしています。

## 設計ポリシー
- NSC は **低レイヤー実行器**
- HID に近い最終入力値のみを扱う
- 状態保持型（press / release / send）
- ヒープ不使用・軽量・決定論的

角度表現・±表記・DSL などは
**別プロジェクトで C++ 配列へ変換**してください。

## 基本的な使い方
```cpp
#include "NSC.h"

NSC::Controller nsc;

void setup() {
  nsc.begin(); // 内部で USB.begin() が呼ばれる
}
```

## 利用可能なコマンド（nsc.）
### 初期化
```cpp
bool nsc.begin();
```

### ボタン操作
```cpp
void nsc.press(NSC::Button button);
void nsc.release(NSC::Button button);
void nsc.set(NSC::Button button, bool on);
void nsc.releaseAll();
void nsc.send();
```

### ヘルパ
```cpp
void nsc.tap(NSC::Button button, uint16_t hold_ms, uint16_t gap_ms);
void nsc.chordHoldTap(NSC::Button hold, NSC::Button tap, uint16_t tap_ms);
```

### スティック操作
```cpp
void nsc.setLeftStick(uint8_t x, uint8_t y);   // 0..255 (128=center)
void nsc.setRightStick(uint8_t x, uint8_t y);
void nsc.centerSticks();
```

### HAT 操作
```cpp
void nsc.setHat(NSC::Hat dir);
```

## ボタン一覧（NSC::Button）
- A / B / X / Y
- L / R / ZL / ZR
- L3 / R3
- PLUS / MINUS
- HOME / CAPTURE

## レシピ（C++ 配列）仕様
```cpp
struct Step {
  NSC::Button button;
  uint16_t    hold_ms;
  int16_t     gap_ms; // -1: default, 0: no gap
};
```

例：

```cpp
static constexpr NSC::Step recipe[] = {
  { NSC::Button::ZL, 200, 0  },
  { NSC::Button::B,  100, -1 },
  { NSC::Button::ZL, 200, 0  },
  { NSC::Button::X,  100, -1 },
};
```
