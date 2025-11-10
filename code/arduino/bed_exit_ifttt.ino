// インクルード設定
#include <WiFi.h>
#include <esp_http_client.h>

// ネットワーク設定
const char* ssid = "WiFiのSSID名";　// 自身の設定を入力する
const char* password = "SSIDのパスワード";　// 自身の設定を入力する

// IFTTT設定
const char* IFTTT_EVENT = "3_7にて作成したイベント名";　// 自身の設定を入力する
const char* IFTTT_KEY = "3_12にて取得したWebhookキー";　// 自身の設定を入力する

// ピン設定
const int PIN_TRIG = 5;
const int PIN_ECHO = 18;

// パラメータ設定
const float DISTANCE_CM = 100.0; // 離床の距離の設定
const unsigned long STABLE_MS = 2000; // 継続時間の設定
const unsigned long SUPPRESS_MS = 60000; // 通知後のサプレッション設定

// 変数設定
unsigned long belowStrat = 0; // 離床検出開始時間
unsigned long lastNotifyTime = 0; // 最終通知時間
bool isOnBed = true; // ベッド上にいるかどうかの状態（true = ベッド上、false = ベッド外）

// 距離を取得する
float measureDistanceCm() {

  // 測定開始信号を出す
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // pulseIn関数で距離を算出する
  unsigned long duration = pulseIn(PIN_ECHO, HIGH,38000UL);

  // 距離が離れている場合はタイムアウトする
  if (duration == 0) {
    return 999.0;
  }

  // 測定した時間を距離(cm)に変換する
  float distance = (duration * 0.0343) / 2.0;

  // 距離を返す
  return distance;
}

// IFTTT通知を行う
bool sendIFTTT(const char* event, const char* key, String value1 = "") {

  // HTTP通信できない場合処理を中断する
  if (WiFi.status() != WL_CONNECTED) return false;

  // HTTPClientクラスのインスタンスを生成する
  HTTPClient http;

  // エンドポイントURLの組み立て、接続を初期化する
  String url = String("https://maker.ifttt.com/trigger/") + event + "/with/key/" + key;
  http.begin(url);

  // HTTPヘッダを追加する
  http.addHeader("Content-Type", "application/json");

  // JSON本文を作成する
  String payload = "{\"value1\":\"" + value1 + "\"}";

  // HTTP POSTを実行し、リソースを解放する
  int code = http.POST(payload);
  http.end();
  
  // 成功判定を返す
  return (code >= 200 && code < 300);
}

// 通信の初期化を行う
void setup() {

  // シリアル通信を初期化する
  Serial.begin(115200);
  delay(100);

  // ピンの設定を行う
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  // WiFi接続を行う
  Serial.printf("Connection to WiFi '%s' ...\n", ssid);
  WiFi.begin(ssid, password);

  // WiFi接続の待機処理を行う
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  // 接続成功した場合、接続状態とIPアドレスを表示する
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi conneted.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

  // 接続失敗した場合、エラーメッセージを表示する
  } else {
    Serial.println("WiFi connect failed (will retry later).");
  }
}

// 距離を5回測定し、中央値を扱う
void loop() {

  // 距離を5回測定する
  const int SAMPLES = 5;
  float vals[SAMPLES];
  for (int i = 0; i < SAMPLES; i++) {
    vals[i] = measureDistanceCm();
    delay(60);
  }

  // 中央値を算出し、表示する
  for (int i = 0; i < SAMPLES - 1; i++) {
    for (int j = i + 1; j < SAMPLES; j++) {
      if (vals[j] < vals[i]) {
        float t = vals[i];
        vals[i] = vals[j];
        vals[j] = t;
      }
    }
  }
  float distance = vals[SAMPLES / 2];
  Serial.printf("Distance: %.1f cm\n", distance);

  // サプレッション中の通知連打を防止する
  unsigned long now = mills();
  if (now - lastNotifyTime < SUPPRESS_MS) {

    // DISTANCE_CMより近い場合、通知はしないが状態の更新は行う
    if (distance < DISTANCE_CM) {
      isOnBed = true;
    }
    delay(200);
    return;
  }

  // DISTANCE_CMより離れているか判定を行う
  if (distance >= DISTANCE_CM) {

    // はじめて離床を検出した場合は現在時刻でbelowStartを初期化する
    if (belowStart == 0) {
      belowStart = now;
    }

    // STABLE_MS以上離れている場合、離床を確定する
    if (now - belowStart >= STABLE_MS) {

      // ベッド上にいた状態から離れた場合、離床を通知する
      if(isOnBed) {
        Serial.println("離床検出：sending notification...");

        // スマホに距離を送信する
        bool ok = sendIFTTT(IFTTT_EVENT, IFTTT_KEY, String("Distance=") + String(distane, 1));

        // 送信成功した場合、成功メッセージを表示する
        if (ok) {
          Serial.println("IFTTT sent.");

        // 送信失敗した場合、失敗メッセージを表示し0.5秒待ってリトライする
        } else {
          Serial.println("IFTTT sendfailed.");
          delay(500);
          ok = sendIFTTT(IFTTT_EVENT, IFTTT_KEY, String("Retry Distance=") + String(distance, 1));
          if (ok) {
            Serial.println("IFTTT retry ok.");
          }
        }

        // 状態を更新する
        lastNotifyTime = now;
        isOnBed = false;
      }
      belowStart = 0;
    }

  // STABLE_MSより近い場合、belowStartをリセットしisOnBedをtrueに戻す
  } else {
    belowStart = 0;
    isOnBed = true;
  }
  delay(250);
}
