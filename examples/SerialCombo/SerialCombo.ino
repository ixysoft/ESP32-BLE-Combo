#include <BLECombo.h>

BLECombo device;
bool isConnected = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  device.begin();

  Serial.println("BLE device started");
}

void loop() {
    if(device.isConnected()) {
        isConnected = true;
        if(Serial.available() > 0) {
            String line = Serial.readStringUntil('\n');
            if(line.startsWith("move ")) {
                int x, y;
                sscanf(line.c_str(), "move %d %d", &x, &y);
                device.mouse()->move(x, y);
            } else if(line.startsWith("click ")) {
                int button;
                sscanf(line.c_str(), "click %d", &button);
                device.mouse()->click(button);
            } else if(line.startsWith("scroll ")) {
                int x, y;
                sscanf(line.c_str(), "scroll %d %d", &x, &y);
                device.mouse()->move(0, 0, x, y);
            } else if(line.startsWith("press ")) {
                String text = line.substring(6);
                device.keyboard()->write((const uint8_t *)text.c_str(), text.length());
            } else if(line == "mute") {
                device.keyboard()->write(KEY_MEDIA_MUTE);
            } else if(line == "vol+" || line == "volume up") {
                device.keyboard()->write(KEY_MEDIA_VOLUME_UP);
            } else if(line == "vol-" || line == "volume down") {
                device.keyboard()->write(KEY_MEDIA_VOLUME_DOWN);
            } else if(line == "swipe up") {
                device.mouse()->move(0,0, -60, 0);
            } else if(line == "swipe down") {
                device.mouse()->move(0,0, 60, 0);
            }
        }
    } else if(isConnected) {
        isConnected = false;
        Serial.println("Disconnected");
        delay(2000);
    }

    delay(50);
}