#include <BLECombo.h>

BLECombo combo;
bool isConnected = false; //最后一次连接状态

void setup() {
  Serial.begin(115200);
  combo.begin();
}

void loop() {
  if(combo.isConnected()) { // 判断键鼠是否已经连接到设备
    isConnected = true;
    Serial.println("按下静音按钮");
    combo.keyboard()->write(KEY_MEDIA_MUTE);
    delay(100);

    Serial.println("音量加");
    combo.keyboard()->write(KEY_MEDIA_VOLUME_UP);
    delay(100);

    Serial.println("音量减");
    combo.keyboard()->write(KEY_MEDIA_VOLUME_DOWN);
    delay(100);

    Serial.println("鼠标移动");
    combo.mouse()->move(random(-127,128), random(-127,128));
    delay(100);

    Serial.println("鼠标中键滚动");
    combo.mouse()->move(0, 0, random(-127,128), 0);
    delay(100);
  } else if(isConnected) {
    isConnected = false;
    Serial.println("Device disconnected");
  }
  delay(50);
}