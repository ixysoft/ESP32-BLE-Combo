[TOC]
# ESP32-BLE-Combo库  
## 1. 为什么创建这个库?  
网上搜到的ESP32键鼠库太老了,在我的手机上只能连接一次,蓝牙一掉就没法重新连接了,所以参考ESP32-BLE-Keyboard库和网上老的ESP32-BLE-Combo重写了一个键鼠库,并扩展了一些功能  

## 2. 这个库实现了什么?  
**键盘功能:**  
ESP32-BLE-Keyboard的所有功能  

**鼠标功能:**  
鼠标点击,移动,滚轮功能,按下,释放功能  

**触控笔功能:**  
触控笔按压,点击,释放功能

## 3. 函数列表  
### 3.1. BLECombo类  
构造函数: `BLECombo(std::string deviceName = "Francis Combo", std::string deviceManufacturer = "IXYsoft", uint8_t batteryLevel = 100)`  
```
参数:
deviceName: 设备名称,搜索蓝牙时显示的名字
deviceManufacturer: 设备制造商
batteryLevel: 电池电量
```

函数: `BLEMouse* mouse()`  
说明: 获取鼠标对象指针

函数: `BLEKeyboard* keyboard()`  
说明: 获取键盘对象指针

函数: `bool isConnected(void)`  
说明: 判断键鼠库是否连接  

函数: `void setBatteryLevel(uint8_t level)`  
说明: 设置电池电量(0~100)  

函数: `void setName(std::string name)`  
说明: 设置设备名称  

函数: `void begin(void)`  
说明: 初始化键鼠库  

函数: `void end(void)`  
说明: 关闭键鼠库  


### 3.2. 鼠标类: BLEMouse  
```
常量:
MOUSE_LEFT      左键
MOUSE_RIGHT     右键
MOUSE_MIDDLE    中键
MOUSE_BACK      后退键
MOUSE_FORWARD   前进键
MOUSE_ALL       全部按键
```

函数: `void click(uint8_t btn = MOUSE_LEFT)`  
说明: 按下并释放鼠标键,默认为左键  

函数: `void move(char x, char y, char wheel = 0, char hWheel = 0)`  
说明: 移动鼠标,参数为x,y轴移动距离,滚轮和水平滚轮移动距离

函数: `void press(uint8_t btn = MOUSE_LEFT)`  
说明: 按下鼠标键,默认为左键  

函数: `void release(uint8_t btn = MOUSE_LEFT)`  
说明: 释放鼠标键,默认为左键  

函数: `void releaseAll()`  
说明: 释放所有鼠标按键

函数: `bool isPressed(uint8_t btn = MOUSE_LEFT)`  
说明: 判断鼠标键是否被按下,默认为左键  

### 3.3 键盘类: BLEKeyboard  
```
按键分类:
按键以KEY_开头
1. 修饰按键:
KEY_LEFT_CTRL
KEY_LEFT_SHIFT
KEY_LEFT_ALT
KEY_LEFT_GUI
KEY_RIGHT_CTRL
KEY_RIGHT_SHIFT
KEY_RIGHT_ALT
KEY_RIGHT_GUI

2. 功能按键
KEY_UP_ARROW
KEY_DOWN_ARROW
KEY_LEFT_ARROW
KEY_RIGHT_ARROW
KEY_BACKSPACE
KEY_TAB
KEY_RETURN
KEY_ESC
KEY_INSERT
KEY_PRTSC
KEY_DELETE
KEY_PAGE_UP
KEY_PAGE_DOWN
KEY_HOME
KEY_END
KEY_CAPS_LOCK
KEY_F1
KEY_F2
KEY_F3
KEY_F4
KEY_F5
KEY_F6
KEY_F7
KEY_F8
KEY_F9
KEY_F10
KEY_F11
KEY_F12
KEY_F13
KEY_F14
KEY_F15
KEY_F16
KEY_F17
KEY_F18
KEY_F19
KEY_F20
KEY_F21
KEY_F22
KEY_F23
KEY_F24

3. 数字功能键
KEY_NUM_0
KEY_NUM_1
KEY_NUM_2
KEY_NUM_3
KEY_NUM_4
KEY_NUM_5
KEY_NUM_6
KEY_NUM_7
KEY_NUM_8
KEY_NUM_9
KEY_NUM_SLASH
KEY_NUM_ASTERISK
KEY_NUM_MINUS
KEY_NUM_PLUS
KEY_NUM_ENTER
KEY_NUM_PERIOD

4. 媒体按键
KEY_MEDIA_NEXT_TRACK                下一曲
KEY_MEDIA_PREVIOUS_TRACK            上一曲
KEY_MEDIA_STOP                      停止播放
KEY_MEDIA_PLAY_PAUSE                播放/暂停
KEY_MEDIA_MUTE                      静音
KEY_MEDIA_VOLUME_UP                 音量加
KEY_MEDIA_VOLUME_DOWN               音量减
KEY_MEDIA_WWW_HOME                  网页主页
KEY_MEDIA_LOCAL_MACHINE_BROWSER     本地浏览器
KEY_MEDIA_CALCULATOR                计算器
KEY_MEDIA_WWW_BOOKMARKS             书签
KEY_MEDIA_WWW_SEARCH                搜索
KEY_MEDIA_WWW_STOP                  网页停止
KEY_MEDIA_WWW_BACK                  网页回退
KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION    配置
KEY_MEDIA_EMAIL_READER              邮箱客户端
```

函数: `size_t press(uint8_t k)`  
说明: 按下指定按键  

函数: `size_t press(const MediaKeyReport k)`  
说明: 按下指定媒体按键  

函数: `size_t release(uint8_t k)`  
说明: 释放指定按键  

函数: `size_t release(const MediaKeyReport k)`  
说明: 释放指定媒体按键  

函数: `size_t write(uint8_t c)`  
说明: 键盘发送指定键值,包含按下/松开  

函数: `size_t write(const MediaKeyReport c)`  
说明: 键盘发送指定媒体键值,包含按下/松开  

函数: `size_t write(const uint8_t *buffer, size_t size)`  
说明: 键盘批量发送指定键值,可用于输出字符串,包含按下/松开  

函数: `void releaseAll(void)`  
说明: 释放所有按下的按键  

### 3.4 触控笔类: BLEPen  
触控笔类用于模拟触控笔设备,电脑,手机端均可用  
函数: `void touch(uint16_t x, uint16_t y, uint16_t pressure)`  
说明: 模拟触控笔点击, (x,y)为点击位置, pressure为压力  

函数: `void press(uint16_t x, uint16_t y, uint16_t pressure)`  
说明: 模拟触控笔按压, (x,y)为点击位置, pressure为压力  

函数: `void swipe(uint16_t x, uint16_t y, int16_t rx, int16_t ry, uint16_t pressure)`  
说明: 模拟触控笔滑动, (x,y)为起始位置, (rx,ry)为滑动距离, pressure为压力  

函数: `void set_resolution(uint16_t w, uint16_t h)`  
说明: 设置触控笔控制设备的分辨率,比如手机的分辨率或者电脑屏幕分辨率  

函数: `void release()`  
说明: 释放触控笔按压  

函数: `bool isPressed()`  
说明: 判断触控笔是否释放  

## 4. 实例  
```
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
```