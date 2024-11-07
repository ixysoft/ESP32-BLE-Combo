#include "BLECombo.h"

#if defined(USE_NIMBLE)
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include <NimBLEHIDDevice.h>
#else
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#endif // USE_NIMBLE
#include "HIDTypes.h"
#include <driver/adc.h>
#include "sdkconfig.h"


#if defined(CONFIG_ARDUHAL_ESP_LOG)
  #include "esp32-hal-log.h"
  #define LOG_TAG ""
#else
  #include "esp_log.h"
  static const char* LOG_TAG = "BLEDevice";
#endif

// 设备报告ID:Report ID
#define KEYBOARD_REPORT_ID			0x01   // 键盘
#define MEDIA_KEYBOARD_REPORT_ID    0x02   // 媒体键
#define MOUSE_REPORT_ID             0x03   // 鼠标
#define PEN_REPORT_ID      			0x04   // 触控笔


static const uint8_t _hidReportDescriptor[] = {
  USAGE_PAGE(1),      0x01,          // USAGE_PAGE (Generic Desktop Ctrls)
  USAGE(1),           0x06,          // USAGE (Keyboard)
  COLLECTION(1),      0x01,          // COLLECTION (Application)
  // ------------------------------------------------- Keyboard
  REPORT_ID(1),       KEYBOARD_REPORT_ID,   //   REPORT_ID (1)
  USAGE_PAGE(1),      0x07,          //   USAGE_PAGE (Kbrd/Keypad)
  USAGE_MINIMUM(1),   0xE0,          //   USAGE_MINIMUM (0xE0)
  USAGE_MAXIMUM(1),   0xE7,          //   USAGE_MAXIMUM (0xE7)
  LOGICAL_MINIMUM(1), 0x00,          //   LOGICAL_MINIMUM (0)
  LOGICAL_MAXIMUM(1), 0x01,          //   Logical Maximum (1)
  REPORT_SIZE(1),     0x01,          //   REPORT_SIZE (1)
  REPORT_COUNT(1),    0x08,          //   REPORT_COUNT (8)
  HIDINPUT(1),        0x02,          //   INPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  REPORT_COUNT(1),    0x01,          //   REPORT_COUNT (1) ; 1 byte (Reserved)
  REPORT_SIZE(1),     0x08,          //   REPORT_SIZE (8)
  HIDINPUT(1),        0x01,          //   INPUT (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  REPORT_COUNT(1),    0x05,          //   REPORT_COUNT (5) ; 5 bits (Num lock, Caps lock, Scroll lock, Compose, Kana)
  REPORT_SIZE(1),     0x01,          //   REPORT_SIZE (1)
  USAGE_PAGE(1),      0x08,          //   USAGE_PAGE (LEDs)
  USAGE_MINIMUM(1),   0x01,          //   USAGE_MINIMUM (0x01) ; Num Lock
  USAGE_MAXIMUM(1),   0x05,          //   USAGE_MAXIMUM (0x05) ; Kana
  HIDOUTPUT(1),       0x02,          //   OUTPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  REPORT_COUNT(1),    0x01,          //   REPORT_COUNT (1) ; 3 bits (Padding)
  REPORT_SIZE(1),     0x03,          //   REPORT_SIZE (3)
  HIDOUTPUT(1),       0x01,          //   OUTPUT (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  REPORT_COUNT(1),    0x06,          //   REPORT_COUNT (6) ; 6 bytes (Keys)
  REPORT_SIZE(1),     0x08,          //   REPORT_SIZE(8)
  LOGICAL_MINIMUM(1), 0x00,          //   LOGICAL_MINIMUM(0)
  LOGICAL_MAXIMUM(1), 0x65,          //   LOGICAL_MAXIMUM(0x65) ; 101 keys
  USAGE_PAGE(1),      0x07,          //   USAGE_PAGE (Kbrd/Keypad)
  USAGE_MINIMUM(1),   0x00,          //   USAGE_MINIMUM (0)
  USAGE_MAXIMUM(1),   0x65,          //   USAGE_MAXIMUM (0x65)
  HIDINPUT(1),        0x00,          //   INPUT (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  END_COLLECTION(0),                 // END_COLLECTION
  // ------------------------------------------------- Media Keys
  USAGE_PAGE(1),      0x0C,          // USAGE_PAGE (Consumer)
  USAGE(1),           0x01,          // USAGE (Consumer Control)
  COLLECTION(1),      0x01,          // COLLECTION (Application)
  REPORT_ID(1),       MEDIA_KEYBOARD_REPORT_ID, //   REPORT_ID (3)
  USAGE_PAGE(1),      0x0C,          //   USAGE_PAGE (Consumer)
  LOGICAL_MINIMUM(1), 0x00,          //   LOGICAL_MINIMUM (0)
  LOGICAL_MAXIMUM(1), 0x01,          //   LOGICAL_MAXIMUM (1)
  REPORT_SIZE(1),     0x01,          //   REPORT_SIZE (1)
  REPORT_COUNT(1),    0x10,          //   REPORT_COUNT (16)
  USAGE(1),           0xB5,          //   USAGE (Scan Next Track)     ; bit 0: 1
  USAGE(1),           0xB6,          //   USAGE (Scan Previous Track) ; bit 1: 2
  USAGE(1),           0xB7,          //   USAGE (Stop)                ; bit 2: 4
  USAGE(1),           0xCD,          //   USAGE (Play/Pause)          ; bit 3: 8
  USAGE(1),           0xE2,          //   USAGE (Mute)                ; bit 4: 16
  USAGE(1),           0xE9,          //   USAGE (Volume Increment)    ; bit 5: 32
  USAGE(1),           0xEA,          //   USAGE (Volume Decrement)    ; bit 6: 64
  USAGE(2),           0x23, 0x02,    //   Usage (WWW Home)            ; bit 7: 128
  USAGE(2),           0x94, 0x01,    //   Usage (My Computer) ; bit 0: 1
  USAGE(2),           0x92, 0x01,    //   Usage (Calculator)  ; bit 1: 2
  USAGE(2),           0x2A, 0x02,    //   Usage (WWW fav)     ; bit 2: 4
  USAGE(2),           0x21, 0x02,    //   Usage (WWW search)  ; bit 3: 8
  USAGE(2),           0x26, 0x02,    //   Usage (WWW stop)    ; bit 4: 16
  USAGE(2),           0x24, 0x02,    //   Usage (WWW back)    ; bit 5: 32
  USAGE(2),           0x83, 0x01,    //   Usage (Media sel)   ; bit 6: 64
  USAGE(2),           0x8A, 0x01,    //   Usage (Mail)        ; bit 7: 128
  HIDINPUT(1),        0x02,          //   INPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  END_COLLECTION(0),                  // END_COLLECTION

  // ------------------------------------------------- Mouse
  USAGE_PAGE(1),       0x01, // USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x02, // USAGE (Mouse)
  COLLECTION(1),       0x01, // COLLECTION (Application)
  USAGE(1),            0x01, //   USAGE (Pointer)
  COLLECTION(1),       0x00, //   COLLECTION (Physical)
  REPORT_ID(1),        MOUSE_REPORT_ID, //     REPORT_ID (1)
  // ------------------------------------------------- Buttons (Left, Right, Middle, Back, Forward)
  USAGE_PAGE(1),       0x09, //     USAGE_PAGE (Button)
  USAGE_MINIMUM(1),    0x01, //     USAGE_MINIMUM (Button 1)
  USAGE_MAXIMUM(1),    0x05, //     USAGE_MAXIMUM (Button 5)
  LOGICAL_MINIMUM(1),  0x00, //     LOGICAL_MINIMUM (0)
  LOGICAL_MAXIMUM(1),  0x01, //     LOGICAL_MAXIMUM (1)
  REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
  REPORT_COUNT(1),     0x05, //     REPORT_COUNT (5)
  HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;5 button bits
  // ------------------------------------------------- Padding
  REPORT_SIZE(1),      0x03, //     REPORT_SIZE (3)
  REPORT_COUNT(1),     0x01, //     REPORT_COUNT (1)
  HIDINPUT(1),         0x03, //     INPUT (Constant, Variable, Absolute) ;3 bit padding
  // ------------------------------------------------- X/Y position, Wheel
  USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
  USAGE(1),            0x30, //     USAGE (X)
  USAGE(1),            0x31, //     USAGE (Y)
  USAGE(1),            0x38, //     USAGE (Wheel)
  LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
  LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
  REPORT_COUNT(1),     0x03, //     REPORT_COUNT (3)
  HIDINPUT(1),         0x06, //     INPUT (Data, Variable, Relative) ;3 bytes (X,Y,Wheel)
  // ------------------------------------------------- Horizontal wheel
  USAGE_PAGE(1),       0x0c, //     USAGE PAGE (Consumer Devices)
  USAGE(2),      0x38, 0x02, //     USAGE (AC Pan)
  LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
  LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
  REPORT_COUNT(1),     0x01, //     REPORT_COUNT (1)
  HIDINPUT(1),         0x06, //     INPUT (Data, Var, Rel)
  END_COLLECTION(0),         //   END_COLLECTION
  END_COLLECTION(0),          // END_COLLECTION

	// 触控笔
	0x05, 0x0D,        // Usage Page (Digitizer)
	0x09, 0x02,        // Usage (Pen)
	0xA1, 0x01,        // Collection (Application)
	0x85, PEN_REPORT_ID,        //   Report ID (10)
	0x09, 0x20,        //   Usage (Stylus)
	0xA1, 0x00,        //   Collection (Physical)
	0x09, 0x42,        //     Usage (Tip Switch)
	0x15, 0x00,        //     Logical Minimum (0)
	0x25, 0x01,        //     Logical Maximum (1)
	0x35, 0x00,        //     Physical Minimum (0)
	0x45, 0x01,        //     Physical Maximum (1)
	0x65, 0x00,        //     Unit (None)
	0x55, 0x00,        //     Unit Exponent (0)
	0x75, 0x01,        //     Report Size (1)
	0x95, 0x01,        //     Report Count (1)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0x44,        //     Usage (Barrel Switch)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0x45,        //     Usage (Eraser)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x95, 0x03,        //     Report Count (3)
	0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0x32,        //     Usage (In Range)
	0x95, 0x01,        //     Report Count (1)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
	0x09, 0x30,        //     Usage (X)
	0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
	0x46, 0x00, 0x08,  //     Physical Maximum (2048)
	0x65, 0x33,        //     Unit (System: English Linear, Length: Inch)
	0x55, 0x0D,        //     Unit Exponent (-3)
	0x75, 0x10,        //     Report Size (16)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0x31,        //     Usage (Y)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x0D,        //     Usage Page (Digitizer)
	0x09, 0x30,        //     Usage (Tip Pressure)
	0x26, 0xFF, 0x1F,  //     Logical Maximum (8191)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0x3D,        //     Usage (X Tilt)
	0x25, 0x7F,        //     Logical Maximum (127)
	0x75, 0x08,        //     Report Size (8)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0x3E,        //     Usage (Y Tilt)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC1, 0x00,        //   End Collection
	0xC1, 0x00,        // End Collection
};

BLEKeyboard::BLEKeyboard(BLECombo* device) : device(device) {
	this->inputKeyboard = device->inputKeyboard;
	this->inputMediaKeys = device->inputMediaKeys;
	this->outputKeyboard = device->outputKeyboard;
}

void BLEKeyboard::begin(void)
{
}

void BLEKeyboard::end(void)
{
}

/**
 * @brief Sets the waiting time (in milliseconds) between multiple keystrokes in NimBLE mode.
 * 
 * @param ms Time in milliseconds
 */
void BLEKeyboard::setDelay(uint32_t ms) {
  this->_delay_ms = ms;
}

void BLEKeyboard::sendReport(KeyReport* keys)
{
  if (this->device->isConnected())
  {
    this->inputKeyboard->setValue((uint8_t*)keys, sizeof(KeyReport));
    this->inputKeyboard->notify();
#if defined(USE_NIMBLE)        
    // vTaskDelay(delayTicks);
    this->delay_ms(_delay_ms);
#endif // USE_NIMBLE
  }	
}

void BLEKeyboard::sendReport(MediaKeyReport* keys)
{
  if (this->device->isConnected())
  {
    this->inputMediaKeys->setValue((uint8_t*)keys, sizeof(MediaKeyReport));
    this->inputMediaKeys->notify();
#if defined(USE_NIMBLE)        
    //vTaskDelay(delayTicks);
    this->delay_ms(_delay_ms);
#endif // USE_NIMBLE
  }	
}

extern
const uint8_t _asciimap[128] PROGMEM;

#define SHIFT 0x80
const uint8_t _asciimap[128] =
{
	0x00,             // NUL
	0x00,             // SOH
	0x00,             // STX
	0x00,             // ETX
	0x00,             // EOT
	0x00,             // ENQ
	0x00,             // ACK
	0x00,             // BEL
	0x2a,			// BS	Backspace
	0x2b,			// TAB	Tab
	0x28,			// LF	Enter
	0x00,             // VT
	0x00,             // FF
	0x00,             // CR
	0x00,             // SO
	0x00,             // SI
	0x00,             // DEL
	0x00,             // DC1
	0x00,             // DC2
	0x00,             // DC3
	0x00,             // DC4
	0x00,             // NAK
	0x00,             // SYN
	0x00,             // ETB
	0x00,             // CAN
	0x00,             // EM
	0x00,             // SUB
	0x00,             // ESC
	0x00,             // FS
	0x00,             // GS
	0x00,             // RS
	0x00,             // US

	0x2c,		   //  ' '
	0x1e|SHIFT,	   // !
	0x34|SHIFT,	   // "
	0x20|SHIFT,    // #
	0x21|SHIFT,    // $
	0x22|SHIFT,    // %
	0x24|SHIFT,    // &
	0x34,          // '
	0x26|SHIFT,    // (
	0x27|SHIFT,    // )
	0x25|SHIFT,    // *
	0x2e|SHIFT,    // +
	0x36,          // ,
	0x2d,          // -
	0x37,          // .
	0x38,          // /
	0x27,          // 0
	0x1e,          // 1
	0x1f,          // 2
	0x20,          // 3
	0x21,          // 4
	0x22,          // 5
	0x23,          // 6
	0x24,          // 7
	0x25,          // 8
	0x26,          // 9
	0x33|SHIFT,      // :
	0x33,          // ;
	0x36|SHIFT,      // <
	0x2e,          // =
	0x37|SHIFT,      // >
	0x38|SHIFT,      // ?
	0x1f|SHIFT,      // @
	0x04|SHIFT,      // A
	0x05|SHIFT,      // B
	0x06|SHIFT,      // C
	0x07|SHIFT,      // D
	0x08|SHIFT,      // E
	0x09|SHIFT,      // F
	0x0a|SHIFT,      // G
	0x0b|SHIFT,      // H
	0x0c|SHIFT,      // I
	0x0d|SHIFT,      // J
	0x0e|SHIFT,      // K
	0x0f|SHIFT,      // L
	0x10|SHIFT,      // M
	0x11|SHIFT,      // N
	0x12|SHIFT,      // O
	0x13|SHIFT,      // P
	0x14|SHIFT,      // Q
	0x15|SHIFT,      // R
	0x16|SHIFT,      // S
	0x17|SHIFT,      // T
	0x18|SHIFT,      // U
	0x19|SHIFT,      // V
	0x1a|SHIFT,      // W
	0x1b|SHIFT,      // X
	0x1c|SHIFT,      // Y
	0x1d|SHIFT,      // Z
	0x2f,          // [
	0x31,          // bslash
	0x30,          // ]
	0x23|SHIFT,    // ^
	0x2d|SHIFT,    // _
	0x35,          // `
	0x04,          // a
	0x05,          // b
	0x06,          // c
	0x07,          // d
	0x08,          // e
	0x09,          // f
	0x0a,          // g
	0x0b,          // h
	0x0c,          // i
	0x0d,          // j
	0x0e,          // k
	0x0f,          // l
	0x10,          // m
	0x11,          // n
	0x12,          // o
	0x13,          // p
	0x14,          // q
	0x15,          // r
	0x16,          // s
	0x17,          // t
	0x18,          // u
	0x19,          // v
	0x1a,          // w
	0x1b,          // x
	0x1c,          // y
	0x1d,          // z
	0x2f|SHIFT,    // {
	0x31|SHIFT,    // |
	0x30|SHIFT,    // }
	0x35|SHIFT,    // ~
	0				// DEL
};


uint8_t USBPutChar(uint8_t c);

// press() adds the specified key (printing, non-printing, or modifier)
// to the persistent key report and sends the report.  Because of the way
// USB HID works, the host acts like the key remains pressed until we
// call release(), releaseAll(), or otherwise clear the report and resend.
size_t BLEKeyboard::press(uint8_t k)
{
	uint8_t i;
	if (k >= 136) {			// it's a non-printing key (not a modifier)
		k = k - 136;
	} else if (k >= 128) {	// it's a modifier key
		_keyReport.modifiers |= (1<<(k-128));
		k = 0;
	} else {				// it's a printing key
		k = pgm_read_byte(_asciimap + k);
		if (!k) {
			setWriteError();
			return 0;
		}
		if (k & 0x80) {						// it's a capital letter or other character reached with shift
			_keyReport.modifiers |= 0x02;	// the left shift modifier
			k &= 0x7F;
		}
	}

	// Add k to the key report only if it's not already present
	// and if there is an empty slot.
	if (_keyReport.keys[0] != k && _keyReport.keys[1] != k &&
		_keyReport.keys[2] != k && _keyReport.keys[3] != k &&
		_keyReport.keys[4] != k && _keyReport.keys[5] != k) {

		for (i=0; i<6; i++) {
			if (_keyReport.keys[i] == 0x00) {
				_keyReport.keys[i] = k;
				break;
			}
		}
		if (i == 6) {
			setWriteError();
			return 0;
		}
	}
	sendReport(&_keyReport);
	return 1;
}

size_t BLEKeyboard::press(const MediaKeyReport k)
{
    uint16_t k_16 = k[1] | (k[0] << 8);
    uint16_t mediaKeyReport_16 = _mediaKeyReport[1] | (_mediaKeyReport[0] << 8);

    mediaKeyReport_16 |= k_16;
    _mediaKeyReport[0] = (uint8_t)((mediaKeyReport_16 & 0xFF00) >> 8);
    _mediaKeyReport[1] = (uint8_t)(mediaKeyReport_16 & 0x00FF);

	sendReport(&_mediaKeyReport);
	return 1;
}

// release() takes the specified key out of the persistent key report and
// sends the report.  This tells the OS the key is no longer pressed and that
// it shouldn't be repeated any more.
size_t BLEKeyboard::release(uint8_t k)
{
	uint8_t i;
	if (k >= 136) {			// it's a non-printing key (not a modifier)
		k = k - 136;
	} else if (k >= 128) {	// it's a modifier key
		_keyReport.modifiers &= ~(1<<(k-128));
		k = 0;
	} else {				// it's a printing key
		k = pgm_read_byte(_asciimap + k);
		if (!k) {
			return 0;
		}
		if (k & 0x80) {							// it's a capital letter or other character reached with shift
			_keyReport.modifiers &= ~(0x02);	// the left shift modifier
			k &= 0x7F;
		}
	}

	// Test the key report to see if k is present.  Clear it if it exists.
	// Check all positions in case the key is present more than once (which it shouldn't be)
	for (i=0; i<6; i++) {
		if (0 != k && _keyReport.keys[i] == k) {
			_keyReport.keys[i] = 0x00;
		}
	}

	sendReport(&_keyReport);
	return 1;
}

size_t BLEKeyboard::release(const MediaKeyReport k)
{
    uint16_t k_16 = k[1] | (k[0] << 8);
    uint16_t mediaKeyReport_16 = _mediaKeyReport[1] | (_mediaKeyReport[0] << 8);
    mediaKeyReport_16 &= ~k_16;
    _mediaKeyReport[0] = (uint8_t)((mediaKeyReport_16 & 0xFF00) >> 8);
    _mediaKeyReport[1] = (uint8_t)(mediaKeyReport_16 & 0x00FF);

	sendReport(&_mediaKeyReport);
	return 1;
}

void BLEKeyboard::releaseAll(void)
{
	_keyReport.keys[0] = 0;
	_keyReport.keys[1] = 0;
	_keyReport.keys[2] = 0;
	_keyReport.keys[3] = 0;
	_keyReport.keys[4] = 0;
	_keyReport.keys[5] = 0;
	_keyReport.modifiers = 0;
    _mediaKeyReport[0] = 0;
    _mediaKeyReport[1] = 0;
	sendReport(&_keyReport);
	sendReport(&_mediaKeyReport);
}

size_t BLEKeyboard::write(uint8_t c)
{
	uint8_t p = press(c);  // Keydown
	release(c);            // Keyup
	return p;              // just return the result of press() since release() almost always returns 1
}

size_t BLEKeyboard::write(const MediaKeyReport c)
{
	uint16_t p = press(c);  // Keydown
	release(c);            // Keyup
	return p;              // just return the result of press() since release() almost always returns 1
}

size_t BLEKeyboard::write(const uint8_t *buffer, size_t size) {
	size_t n = 0;
	while (size--) {
		if (*buffer != '\r') {
			if (write(*buffer)) {
			  n++;
			} else {
			  break;
			}
		}
		buffer++;
	}
	return n;
}

void BLEKeyboard::delay_ms(uint64_t ms) {
  uint64_t m = esp_timer_get_time();
  if(ms){
    uint64_t e = (m + (ms * 1000));
    if(m > e){ //overflow
        while(esp_timer_get_time() > e) { }
    }
    while(esp_timer_get_time() < e) {}
  }
}


// 鼠标逻辑
BLEMouse::BLEMouse(BLECombo* device) : device(device) {
	this->inputMouse = device->inputMouse;
}

void BLEMouse::execute(uint8_t buttons, int8_t rel_x, int8_t rel_y, int8_t wheel, int8_t hWheel) {
	if(device->isConnected()) {
		uint8_t params[5];
		params[0] = buttons;
		params[1] = rel_x;
		params[2] = rel_y;
		params[3] = wheel;
		params[4] = hWheel;

		_buttons = buttons;
		inputMouse->setValue(params, 5);
		inputMouse->notify();
	}
}

void BLEMouse::move(char x, char y, char wheel, char hWheel) {
	execute(_buttons, x, y, wheel, hWheel);
}

void BLEMouse::click(uint8_t btn) {
	execute(btn);
	execute(0);
}

void BLEMouse::buttons(uint8_t btns) {
	if(btns != _buttons) {	// 按下的按钮变更
		execute(btns);
	}
}

void BLEMouse::press(uint8_t btn) {
	execute(btn | _buttons);
}

void BLEMouse::release(uint8_t btn) {
	execute(_buttons & ~btn);
}

void BLEMouse::releaseAll() {
	execute();
}

bool BLEMouse::isPressed(uint8_t btn) {
	return (_buttons & btn) != 0;
}

// 触摸屏逻辑
BLEPen::BLEPen(BLECombo* device) : device(device) {
	this->inputPen = device->inputPen;
}

void BLEPen::execute(uint8_t tip_switch, uint16_t x, uint16_t y, uint16_t pressure = 0) {
	struct {
		uint8_t tip_switch:1;
        uint8_t barrel_switch:1;
        uint8_t eraser:1;
        uint8_t _reserved1:3;
        uint8_t in_range:1;
        uint8_t _reserved2:1;
	} state = {0};
	state.tip_switch = !!tip_switch;
	state.in_range = !!tip_switch;
	state.barrel_switch = !!tip_switch;

	x = x * 32768 / _w;
	y = y * 32768 / _h;

	uint8_t params[9];
	params[0] = *(uint8_t*)&state;	// Tip Switch, InRange, Confidence
	// params[1] = 1;			// ID
	params[1] = (x & 0xFF);	// X lSB
	params[2] = ((x >> 8) & 0xFF);	// X MSB
	params[3] = (y & 0xFF);	// Y LSB
	params[4] = ((y >> 8) & 0xFF);	// Y MSB
	params[5] = (pressure & 0xFF);
	params[6] = ((pressure >> 8) & 0xFF);
	params[7] = 0;
	params[8] = 0;
	inputPen->setValue(params, sizeof(params));
	inputPen->notify();
}

void BLEPen::press(uint16_t x, uint16_t y, uint16_t pressure) {
	execute(1, x, y, pressure);
	_pressed = true;
}

void BLEPen::release() {
	execute(0, 0, 0, 0);
	_pressed = false;
}

void BLEPen::touch(uint16_t x, uint16_t y, uint16_t pressure) {
	press(x, y, pressure);
	release();
}

void BLEPen::swipe(uint16_t x, uint16_t y, int16_t rx, int16_t ry, uint16_t pressure) {
	press(x, y, pressure);
	press(x+rx/2, y+ry/2, pressure);
	press(x+rx, y+ry, pressure);
	release();
}

void BLEPen::set_resolution(uint16_t w, uint16_t h) {
	_w = (w > 32767 || w == 0) ? 32767 : w;
	_h = (h > 32767 || h == 0) ? 32767 : h;
}

bool BLEPen::isPressed() {
	return _pressed;
}

// 键鼠逻辑
BLECombo::BLECombo(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel)
 	: hid(0)
	, deviceName(std::string(deviceName).substr(0, 15))
	, deviceManufacturer(std::string(deviceManufacturer).substr(0,15))
	, batteryLevel(batteryLevel) {}

BLECombo::~BLECombo() {
	delete this->hid;
	delete this->_keyboard;
	delete this->_mouse;
	delete this->_pen;
}


void BLECombo::begin(uint16_t w, uint16_t h)
{
  BLEDevice::init(deviceName.c_str());		//初始化BLE设备名
  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(this);


  hid = new BLEHIDDevice(pServer);
  // 键盘HID
  inputKeyboard = hid->inputReport(KEYBOARD_REPORT_ID);  // <-- input REPORTID from report map
  outputKeyboard = hid->outputReport(KEYBOARD_REPORT_ID);
  inputMediaKeys = hid->inputReport(MEDIA_KEYBOARD_REPORT_ID);
  
  // 鼠标HID
  inputMouse = hid->inputReport(MOUSE_REPORT_ID);

  // 触摸屏HID
  inputPen = hid->inputReport(PEN_REPORT_ID);

	_mouse = new BLEMouse(this);		//初始化鼠标
	_keyboard = new BLEKeyboard(this);	//初始化键盘
	_pen = new BLEPen(this);	//初始化触摸屏

	_pen->set_resolution(w, h);

  outputKeyboard->setCallbacks(this);

  hid->manufacturer()->setValue(deviceManufacturer.c_str());

  hid->pnp(0x02, vid, pid, version);
  hid->hidInfo(0x00, 0x01);


#if defined(USE_NIMBLE)

  BLEDevice::setSecurityAuth(true, true, true);

#else

  BLESecurity* pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

#endif // USE_NIMBLE

  hid->reportMap((uint8_t*)_hidReportDescriptor, sizeof(_hidReportDescriptor));
  hid->startServices();

  onStarted(pServer);

  advertising = pServer->getAdvertising();
  advertising->setAppearance(HID_KEYBOARD);
  advertising->addServiceUUID(hid->hidService()->getUUID());
  advertising->setScanResponse(false);
  advertising->start();
  hid->setBatteryLevel(batteryLevel);

  ESP_LOGD(LOG_TAG, "Advertising started!");
}


void BLECombo::onConnect(BLEServer* pServer) {
  this->connected = true;

#if !defined(USE_NIMBLE)

  BLE2902* desc = (BLE2902*)this->inputKeyboard->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(true);
  desc = (BLE2902*)this->inputMediaKeys->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(true);
  // 鼠标标记
  desc = (BLE2902*)this->inputMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(true);

  // 触摸屏标记
  desc = (BLE2902*)this->inputPen->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(true);

#endif // !USE_NIMBLE

}

void BLECombo::onDisconnect(BLEServer* pServer) {
  this->connected = false;

#if !defined(USE_NIMBLE)

  BLE2902* desc = (BLE2902*)this->inputKeyboard->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(false);
  desc = (BLE2902*)this->inputMediaKeys->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(false);
  // 鼠标标记释放
  desc = (BLE2902*)this->inputMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(false);  

  // 触摸屏标记释放
  desc = (BLE2902*)this->inputPen->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(false);

  advertising->start();

#endif // !USE_NIMBLE
}

void BLECombo::onWrite(BLECharacteristic* me) {
  uint8_t* value = (uint8_t*)(me->getValue().c_str());
  (void)value;
  ESP_LOGI(LOG_TAG, "special keys: %d", *value);
}


bool BLECombo::isConnected(void) {
  return this->connected;
}

void BLECombo::setBatteryLevel(uint8_t level) {
  this->batteryLevel = level;
  if (hid != 0)
    this->hid->setBatteryLevel(this->batteryLevel);
}

//must be called before begin in order to set the name
void BLECombo::setName(std::string deviceName) {
  this->deviceName = deviceName;
}

void BLECombo::set_vendor_id(uint16_t vid) { 
	this->vid = vid; 
}

void BLECombo::set_product_id(uint16_t pid) { 
	this->pid = pid; 
}

void BLECombo::set_version(uint16_t version) { 
	this->version = version; 
}