## 🛏️ Bed Exit Sensor (ESP32 + HC-SR04 + IFTTT)

入院患者・高齢者の **離床検知** を目的とした IoT センサーです。  
ESP32で超音波センサー（HC-SR04）から距離を測定し、  
閾値を超えた場合に **IFTTT Webhook** を使ってスマホに通知します。

---

## 🚀 主な機能

| 機能 | 説明 |
|------|------|
| 超音波による距離測定 | HC-SR04で対象との距離を算出 |
| 離床判定 | 閾値を超えた状態が一定時間続いた場合に通知 |
| IFTTT Webhook連携 | スマホ/メール/LINEなどに自動通知 |
| シリアルログ出力 | 距離や状態を確認可能 |
| ESP32 WiFi接続 | 無線でクラウドへ連携 |

---

## 🧰 使用パーツ

| 部品 | 型番 | 数量 |
|------|------|------|
| マイコン | ESP32 DevKitC / NodeMCU-32S | 1 |
| 超音波センサー | HC-SR04 | 1 |
| 抵抗 | 10kΩ + 20kΩ（ECHO電圧降圧用）| 各1 |
| ジャンパーワイヤー | オス-オス/メス-オス | 適量 |
| ブレッドボード | 任意 | 1 |

👉 詳細は `hardware/wiring_diagram` を参照

---

## 🔌 配線図

ESP32 GPIO5 (TRIG) → HC-SR04 TRIG
ESP32 GPIO18 (ECHO) → HC-SR04 ECHO (10k + 20k分圧)
ESP32 5V → HC-SR04 VCC
ESP32 GND → HC-SR04 GND

電圧分圧：
ECHO ─ 10k ─+── GPIO18
│
20k
│
GND

より詳しい回路図は `/hardware/wiring_diagram.png` にあります。

---

## 🧪 動作条件

| 項目 | 必要環境 |
|------|---------|
| Wi-Fi | 2.4GHz帯 |
| IFTTT | Webhook Key と EventName |
| Arduino IDE | ESP32ボードマネージャ必須 |
| 通知端末 | iPhone / Android / Email など |

---

## 💾 実装方法
実装コードは `/code/arduino/bed_exit_ifttt.ino` にあります。

以下のサイトに手順を記載したため参考にする。

Qiita：https://qiita.com/Gen_00_/items/9c065e083f0b96886417

---

## 📊 動作テスト
##### 操作手順1
センサーの検知範囲内に人がいる状態で動作を開始する。
##### 期待値1
Arduinoで人との距離を測定できること。
![image.png](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/3795189/9338d075-8dd1-4c90-b1dc-8fc48653a0c4.png)
##### 操作手順2
その後、センサーが人を検知できない位置まで離れる。
##### 期待値2
- エディタ上で「IFTTT sent.」と表示されること。
![スクリーンショット 2025-11-15 145128.png](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/3795189/a82f9083-4113-49d5-9ec2-4ac7b8d4fb99.png)
- 同時にスマートフォンに IFTTT 通知が届くこと。
![前.jpeg](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/3795189/d6b5c024-6a6a-451b-9419-91f6489768bb.jpeg)
![後.png](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/3795189/fb58ad61-0818-4cac-91fd-616885cc6387.png)
---

## 🧱 今後の展望
今後は「NTCR」要件をさらに満たし、より実用的かつ信頼性の高いシステムへ発展させていくことを目指します。

##### N（Navigation / 精度）：
センサーの検知範囲と精度を向上させる。
たとえば、超音波センサーに加えて赤外線センサーや複数点測定を組み合わせ、誤検知を低減する。

##### T（Timing / リアルタイム性）：
離床検知の反応時間を短縮し、ハードリアルタイム処理を意識した設計に改善する。
割り込み処理（interrupt）やFreeRTOSタスク制御を活用して遅延を最小化する。

##### C（Cost / 消費資源）：
消費電力とメモリ使用量を最適化する。
Deep Sleepモードや軽量データ処理アルゴリズムを導入し、バッテリー駆動でも安定動作を目指す。

##### R（Reliability / 信頼性）：
高信頼性が求められる医療・介護用途（特定管理医療機器）を意識し、
冗長化・フェイルセーフ設計や異常検知ロジックの導入を検討する。

---

## 📄 ライセンス
本リポジトリは MIT License に基づき公開されています。


