#include <BLECombo.h>

BLECombo combo;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  combo.begin(1440, 3200);  //设置屏幕分辨率,我的手机是1440x3200,不设置默认为32767x32767
  Serial.println("键鼠库启动");
}

void loop() {
  // put your main code here, to run repeatedly:
  char tmp[32] = "";
  if(combo.isConnected()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if(line.length() > 0) {
      Serial.println("received: " + line);
      if(line.startsWith("press ")) {
        uint16_t x,y,pressure;
        sscanf(line.c_str(), "press %hu,%hu,%hu", &x, &y, &pressure);
        combo.pen()->press(x, y, pressure);
      } else if(line.startsWith("touch ")) {
        uint16_t x,y,pressure;
        sscanf(line.c_str(), "touch %hu,%hu,%hu", &x, &y, &pressure);
        combo.pen()->touch(x, y, pressure);
      } else if(line == "release") {
        combo.pen()->release();
      } else if(line.startsWith("swipe ")) {
        uint16_t x,y,pressure;
        int16_t rx,ry;
        sscanf(line.c_str(), "swipe %hu,%hu,%hd,%hd,%hu", &x, &y, &rx, &ry, &pressure);
        combo.pen()->swipe(x, y, rx, ry, pressure);
      }
    }
  }

  delay(50);
}
