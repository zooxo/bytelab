/* Welcome to BYTELAB

  BYTELAB

  Arduino genuino micro
  Keypad with 8 keys 2 rows and 4 colums (pins 4...9)
  OLED 128x32 SSD1306
  SDA - PIN2
  SCK - PIN3

  BUTTONS: 0 2 3 4
           1 5 6 7

  KEYBOARD:
                 reg     term
    unshifted    1 2 3   q w e   ESC  UP   OK
                 4 5 6   a s d   LEFT DOWN RIGHT
    f-shifted    A B C   4 5 6
                 D E F   r t z
    g-shifted    G H I   f g h
                 J K L   v b n
    f-held       M N O   7 8 9
                 P Q R   u i o
    g-held       S T U   j k l
                 V W X   m , .

    -    f g F G    -    f g F G    -     f g F G
    ESC  A G M S    UP   B H N T    OK    C I O U
    LEFT D J P V    DOWN E K Q W    RIGHT F L R X

  Command mode:
    f/F
    g/G
  Edit mode:
    ESC UP EDIT
    DEL DN INS
  Calculator mode:
    UNSHIFTED        F       G         f       g
    CLX MENU ENTER   7 8 9   1 2 3     _ _ /   RCL  STO -
    OFF SHOW SUM     4 5 6   0 . CHS   _ _ *   SWAP EE  +

    UNSHIFTED         HOLD     SHIFT
    CLX MENU ENTER  F 7 8 9  f SQRT  INV /
    OFF SHOW SUM      4 5 6    EXP   POW *
                    G 1 2 3  g RCL   STO -
                      0 . #    SWAP  EE  +

  Keyboard mode:
    UNSHIFTED          HOLD                SHIFT
    ESC  UP   ENTER  F tab  pgup bkspce  f F3    F4  F5
    LEFT DOWN RIGHT    home pgdn end       F6    F7  F8
                     G esc  up   enter   g shift ctl alt
                       left down right     shift ctl alt

  ABBREVIATIONS of commands:
    Bytestack
    Constant
    ...Calc
    Disk
    Keyboard
    Floatstack
    Settings
    Watch

    ASCII TABLE:

      DEC     |  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
          HEX |  0 1 2 3 4 5 6 7 8 9 a b c d e f
      ------------------------------------------
      032 20  |    ! " # $ % & ' ( ) * + , - . /
      048 30  |  0 1 2 3 4 5 6 7 8 9 : ; < = > ?
      064 40  |  @ A B C D E F G H I J K L M N O
      080 50  |  P Q R S T U V W X Y Z [ \ ] ^ _
      096 60  |  ` a b c d e f g h i j k l m n o
      112 70  |  p q r s t u v w x y z { | } ~

  Leonardo's definitions for modifier keys (Key Hex Dec):
  KEY_LEFT_CTRL  0x80  128
  KEY_LEFT_SHIFT  0x81  129
  KEY_LEFT_ALT  0x82  130
  KEY_LEFT_GUI  0x83  131
  KEY_RIGHT_CTRL  0x84  132
  KEY_RIGHT_SHIFT  0x85  133
  KEY_RIGHT_ALT  0x86  134
  KEY_RIGHT_GUI  0x87  135
  KEY_UP_ARROW  0xDA  218
  KEY_DOWN_ARROW  0xD9  217
  KEY_LEFT_ARROW  0xD8  216
  KEY_RIGHT_ARROW  0xD7  215
  KEY_BACKSPACE  0xB2  178
  KEY_TAB  0xB3  179
  KEY_RETURN  0xB0  176
  KEY_ESC  0xB1  177
  KEY_INSERT  0xD1  209
  KEY_DELETE  0xD4  212
  KEY_PAGE_UP  0xD3  211
  KEY_PAGE_DOWN  0xD6  214
  KEY_HOME  0xD2  210
  KEY_END  0xD5  213
  KEY_CAPS_LOCK  0xC1  193
  KEY_F1  0xC2  194
  KEY_F2  0xC3  195
  KEY_F3  0xC4  196
  KEY_F4  0xC5  197
  KEY_F5  0xC6  198
  KEY_F6  0xC7  199
  KEY_F7  0xC8  200
  KEY_F8  0xC9  201
  KEY_F9  0xCA  202
  KEY_F10  0xCB  203
  KEY_F11  0xCC  204
  KEY_F12  0xCD  205


*/


// INCLUDES AND DEFINES
#include <Wire.h>     // Control OLED-display
#include <EEPROM.h>   // "Harddisk"
//#include <avr/sleep.h> // Sleep mode
#include <Keyboard.h> // For sending keyboard presses via USB

// Initialize custom keyboard (2x4)
#include <Keypad.h>
const byte ROWS = 2; // Two rows
const byte COLS = 4; // Four columns
char hexaKeys[ROWS][COLS] = {{'0', '2', '3', '4'}, {'1', '5', '6', '7'}}; // Button chars
byte rowPins[ROWS] = {8, 9}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 5, 4, 6}; // Connect to the column pinouts of the keypad
Keypad but = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

#define DEFAULTCONTRAST 0xFF // Default contrast
#define MAXBUT 8 // Number of buttons
#define MAXBYTE 255 // Maximal value of byte
#define CMDLINESTEP 8 // Page up/down step for commands
#define LINESTEP   16 // Page up/down step for byte input
#define BYTESTACKSIZE 16 // Size of bytestack
#define FLOATSTACKSIZE 4 // Size of floatstack
#define ESC   '1' // Main control keys
#define UP    '2'
#define OK    '3'
#define LEFT  '4'
#define DOWN  '5'
#define RIGHT '6'
#define EEADDRFKEYS 0 // EEPROM address for 12 function keys
#define EEADDRFSSTART 12 // Begin of EEPROM memory to store files
#define EEADDRFSEND 1024 // End of EEPROM memory to store files
#define TIMECORRECTION 0 // Additional time in ms per PRINTINTERVAL
#define DAILYTIMECORRECTION 0 // Additional time in ms per day
#define PRINTINTERVAL 10 // Prints time every 10 s
#define PRGPTRSTART 4 // Start of commands in prg[] (after header)
#define CMDOFFSET 32 // Offset to align commands to ascii characters
#define PAUSE 1000 // Pause time in ms
#define FIX               3 // Display 3 decimals
#define FIXMANT           3 // FIX in 10th-exponent
#define FIXSHOW           6 // FIX for SHOW
#define FIXMANTSHOW       6 // FIX in 10th-exponent
#define TINYNUMBER     1e-7 // Number for rounding to 0
#define MAXEE            38 // Maximal EE for Y10X
#define MAXITERATE      100 // Maximal number of Taylor series loops to iterate
#define RAD        57.29578 // 180/PI
#define GOTO "Pgoto"
#define INPUTBYTE "iB"

// Modes
#define CMDMODE    1 // Entering command
#define INMODE     2 // Input byte
#define VIEWBS     3 // View bytestack (X)
#define CLKMODE    4 // Show clock
#define EDITMODE   5 // Edit prg[]
#define EDITINMODE 6 // Edit prg-step byte
#define FSMODE     7 // File system (df, rm, load, save)
#define MSGMODE    8 // View/print message
#define RUNMODE    9 // Run/execute prg
#define CALCMODE  10 // Calculator mode
#define KBDMODE   11 // Keyboard mode
#define STOPMODE  12 // Stops program execution

// OLED display (128x32) with ssd1306 controller
#define OLED_I2C_ADDRESS 0x3C // SLA (0x3C) + WRITE_MODE (0x00)
#define OLED_CONTROL_BYTE_CMD_SINGLE  0x80 // Control byte for single command (has to be sent before a command)
#define OLED_CONTROL_BYTE_CMD_STREAM  0x00 // Control byte for command stream (has to be sent before a command)
#define OLED_CONTROL_BYTE_DATA_STREAM 0x40 // Control byte for data stream (has to be sent before a data stream)

#define OLED_CMD_SET_CONTRAST     0x81 // Set contrast (0...ff)
#define OLED_CMD_DISPLAY_RAM      0xA4 // Enable rendering from GDDRAM (Graphic Display Data RAM)
#define OLED_CMD_DISPLAY_ALLON    0xA5 // Entire display available
#define OLED_CMD_DISPLAY_NORMAL   0xA6 // Display not inverted
#define OLED_CMD_DISPLAY_INVERTED 0xA7 // Inverted display
#define OLED_CMD_DISPLAY_OFF      0xAE // Set display off
#define OLED_CMD_DISPLAY_ON       0xAF // Set display on

#define OLED_CMD_SET_MEMORY_ADDR_MODE 0x20  // Horizontal (0x00) or vertical (0x01) mode
#define OLED_CMD_SET_COLUMN_RANGE     0x21  // Set column range
#define OLED_CMD_SET_PAGE_RANGE       0x22  // Set row/page range

#define OLED_CMD_SET_DISPLAY_START_LINE 0x40 // Set display start line register (0...63)
#define OLED_CMD_SET_SEGMENT_REMAP      0xA1 // Screen orientation 0xA0 or flip (0xA1)
#define OLED_CMD_SET_MUX_RATIO          0xA8 // Change multiplex mode (16..63; default=63)
#define OLED_CMD_SET_COM_SCAN_MODE      0xC8 // Scan direction
#define OLED_CMD_SET_DISPLAY_OFFSET     0xD3 // Set display offset
#define OLED_CMD_SET_COM_PIN_MAP        0xDA // Pin mapping (128x64 or 128x32)
#define OLED_CMD_SET_DISPLAY_CLK_DIV    0xD5 // Clock divied ratio and oszillator frequency
#define OLED_CMD_SET_PRECHARGE          0xD9 // Pre charge period
#define OLED_CMD_SET_VCOMH_DESELCT      0xDB // Vcomh deselect regulator level
#define OLED_CMD_SET_CHARGE_PUMP        0x8D // Charge pump level
#define OLED_CMD_NOP                    0xE3 // No operation

#define OLED_COL 0x7f // 128 colums
#define OLED_ROW 0x03 // 4 rows (pages)

// FONTS
//const byte font4x16 [] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x60, 0xf0, 0xf0, 0x60}; // .
const byte fontdot [] PROGMEM = {0x70, 0xf8, 0xf8, 0xf8, 0x70}; // .

const byte font7x16 [] PROGMEM = {
  0xfe, 0xff, 0x03, 0x83, 0x63, 0xff, 0xfe, 0x7f, 0xff, 0xc6, 0xc1, 0xc0, 0xff, 0x7f, // 0
  0x00, 0x04, 0x06, 0xff, 0xff, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0xff, 0xff, 0xc0, 0xc0, // 1
  0x0e, 0x0f, 0x03, 0x83, 0xc3, 0xff, 0x7e, 0xfc, 0xfe, 0xc7, 0xc3, 0xc1, 0xc0, 0xc0, // 2
  0x03, 0x43, 0x63, 0x73, 0xfb, 0xdf, 0x8f, 0x70, 0xf0, 0xc0, 0xc0, 0xc0, 0xff, 0x7f, // 3
  0xe0, 0xf0, 0xb8, 0x9c, 0xfe, 0xff, 0x80, 0x01, 0x01, 0x01, 0x01, 0xff, 0xff, 0x01, // 4
  0xff, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0x83, 0x70, 0xf0, 0xc0, 0xc0, 0xc0, 0xff, 0x7f, // 5
  0xfe, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0x80, 0x7f, 0xff, 0xc0, 0xc0, 0xc0, 0xff, 0x7f, // 6
  0x03, 0x03, 0x03, 0xc3, 0xf3, 0x3f, 0x0f, 0xf0, 0xfc, 0x0f, 0x03, 0x00, 0x00, 0x00, // 7
  0xfe, 0xff, 0x83, 0x83, 0x83, 0xff, 0xfe, 0x7f, 0xff, 0xc1, 0xc1, 0xc1, 0xff, 0x7f, // 8
  0x7e, 0xff, 0xc3, 0xc3, 0xc3, 0xff, 0xfe, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xff, 0x7f, // 9
  0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, // -
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // :_
  0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00, // ;>
  //0x08, 0x0c, 0xfe, 0xff, 0xfe, 0x0c, 0x08, 0x10, 0x30, 0x7f, 0xff, 0x7f, 0x30, 0x10, // ;^
};

const byte font5x8 [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, // sp
  0x00, 0x00, 0x2f, 0x00, 0x00, // !
  0x00, 0x07, 0x00, 0x07, 0x00, // "
  0x14, 0x7f, 0x14, 0x7f, 0x14, // #
  0x24, 0x2a, 0x7f, 0x2a, 0x12, // $
  0x62, 0x64, 0x08, 0x13, 0x23, // %
  0x36, 0x49, 0x55, 0x22, 0x50, // &
  0x00, 0x05, 0x03, 0x00, 0x00, // '
  0x00, 0x1c, 0x22, 0x41, 0x00, // (
  0x00, 0x41, 0x22, 0x1c, 0x00, // )
  0x14, 0x08, 0x3E, 0x08, 0x14, // *
  0x08, 0x08, 0x3E, 0x08, 0x08, // +
  0x00, 0x00, 0xA0, 0x60, 0x00, // ,
  0x08, 0x08, 0x08, 0x08, 0x08, // -
  0x00, 0x60, 0x60, 0x00, 0x00, // .
  0x20, 0x10, 0x08, 0x04, 0x02, // /
  0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
  0x00, 0x42, 0x7F, 0x40, 0x00, // 1
  0x42, 0x61, 0x51, 0x49, 0x46, // 2
  0x21, 0x41, 0x45, 0x4B, 0x31, // 3
  0x18, 0x14, 0x12, 0x7F, 0x10, // 4
  0x27, 0x45, 0x45, 0x45, 0x39, // 5
  0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
  0x01, 0x71, 0x09, 0x05, 0x03, // 7
  0x36, 0x49, 0x49, 0x49, 0x36, // 8
  0x06, 0x49, 0x49, 0x29, 0x1E, // 9
  0x00, 0x36, 0x36, 0x00, 0x00, // :
  0x00, 0x56, 0x36, 0x00, 0x00, // ;
  0x08, 0x14, 0x22, 0x41, 0x00, // <
  0x14, 0x14, 0x14, 0x14, 0x14, // =
  0x00, 0x41, 0x22, 0x14, 0x08, // >
  0x02, 0x01, 0x51, 0x09, 0x06, // ?
  0x32, 0x49, 0x59, 0x51, 0x3E, // @
  0x7C, 0x12, 0x11, 0x12, 0x7C, // A
  0x7F, 0x49, 0x49, 0x49, 0x36, // B
  0x3E, 0x41, 0x41, 0x41, 0x22, // C
  0x7F, 0x41, 0x41, 0x22, 0x1C, // D
  0x7F, 0x49, 0x49, 0x49, 0x41, // E
  0x7F, 0x09, 0x09, 0x09, 0x01, // F
  0x3E, 0x41, 0x49, 0x49, 0x7A, // G
  0x7F, 0x08, 0x08, 0x08, 0x7F, // H
  0x00, 0x41, 0x7F, 0x41, 0x00, // I
  0x20, 0x40, 0x41, 0x3F, 0x01, // J
  0x7F, 0x08, 0x14, 0x22, 0x41, // K
  0x7F, 0x40, 0x40, 0x40, 0x40, // L
  0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
  0x7F, 0x04, 0x08, 0x10, 0x7F, // N
  0x3E, 0x41, 0x41, 0x41, 0x3E, // O
  0x7F, 0x09, 0x09, 0x09, 0x06, // P
  0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
  0x7F, 0x09, 0x19, 0x29, 0x46, // R
  0x46, 0x49, 0x49, 0x49, 0x31, // S
  0x01, 0x01, 0x7F, 0x01, 0x01, // T
  0x3F, 0x40, 0x40, 0x40, 0x3F, // U
  0x1F, 0x20, 0x40, 0x20, 0x1F, // V
  0x3F, 0x40, 0x38, 0x40, 0x3F, // W
  0x63, 0x14, 0x08, 0x14, 0x63, // X
  0x07, 0x08, 0x70, 0x08, 0x07, // Y
  0x61, 0x51, 0x49, 0x45, 0x43, // Z
  0x00, 0x7F, 0x41, 0x41, 0x00, // [
  0x02, 0x04, 0x08, 0x10, 0x20, // b
  0x00, 0x41, 0x41, 0x7F, 0x00, // ]
  0x04, 0x02, 0x01, 0x02, 0x04, // ^
  0x40, 0x40, 0x40, 0x40, 0x40, // _
  0x00, 0x01, 0x02, 0x04, 0x00, // '
  0x20, 0x54, 0x54, 0x54, 0x78, // a
  0x7F, 0x48, 0x44, 0x44, 0x38, // b
  0x38, 0x44, 0x44, 0x44, 0x20, // c
  0x38, 0x44, 0x44, 0x48, 0x7F, // d
  0x38, 0x54, 0x54, 0x54, 0x18, // e
  0x08, 0x7E, 0x09, 0x01, 0x02, // f
  0x18, 0xA4, 0xA4, 0xA4, 0x7C, // g
  0x7F, 0x08, 0x04, 0x04, 0x78, // h
  0x00, 0x44, 0x7D, 0x40, 0x00, // i
  0x40, 0x80, 0x84, 0x7D, 0x00, // j
  0x7F, 0x10, 0x28, 0x44, 0x00, // k
  0x00, 0x41, 0x7F, 0x40, 0x00, // l
  0x7C, 0x04, 0x18, 0x04, 0x78, // m
  0x7C, 0x08, 0x04, 0x04, 0x78, // n
  0x38, 0x44, 0x44, 0x44, 0x38, // o
  0xFC, 0x24, 0x24, 0x24, 0x18, // p
  0x18, 0x24, 0x24, 0x18, 0xFC, // q
  0x7C, 0x08, 0x04, 0x04, 0x08, // r
  0x48, 0x54, 0x54, 0x54, 0x20, // s
  0x04, 0x3F, 0x44, 0x40, 0x20, // t
  0x3C, 0x40, 0x40, 0x20, 0x7C, // u
  0x1C, 0x20, 0x40, 0x20, 0x1C, // v
  0x3C, 0x40, 0x30, 0x40, 0x3C, // w
  0x44, 0x28, 0x10, 0x28, 0x44, // x
  0x1C, 0xA0, 0xA0, 0xA0, 0x7C, // y
  0x44, 0x64, 0x54, 0x4C, 0x44, // z
  0x00, 0x08, 0x36, 0x41, 0x00, // {
  0x00, 0x00, 0xff, 0x00, 0x00, // |
  0x41, 0x36, 0x08, 0x00, 0x00, // },
  0x10, 0x08, 0x18, 0x10, 0x08  // ~
};


// GLOBAL VARIABLES
static byte state[MAXBUT]; // State of keys 0/1/2/3 (idle/pressed/hold/released)
static byte key = NULL; // Assigned key
static byte mode; // Mode (see mode table)
static byte oldmode; // Previous mode (needed for printmsg and inputBYTE)
static byte msg; // Number of message to print
char strbuf[10]; // Holds string to print
static byte bytestack[BYTESTACKSIZE]; // Main data stack
static double floatstack[FLOATSTACKSIZE]; // Float stack
static byte byteselect; // Selected byte
static byte cmdselect;  // Selected command
const char hex[] = "0123456789ABCDEF"; // For converting nibble to hex
static byte inputbytes;
static byte f[12]; // Function keys holding their commands (6xf and 6xg)
static unsigned long clockmillis = 0; // Time
static unsigned long starttime = 0; // Marker for time
static byte prg [MAXBYTE]; // Program
static byte prgptr; // Pointer to actual step in prg[]
static int fsptr; // Pointer to file system of EEPROM
static byte decimals = 0;     // Number of decimals entered - used for input after decimal dot
static double sx = 0.0, sxx = 0.0, sy = 0.0, sxy = 0.0; // Statistical/LR variables
static byte sn = 0;

static boolean isf, isg;     // True if function key was pressed
static boolean isF, isG;     // True if funktion key is holding
static boolean isdisplayoff; // True if display is off (screensaver)
static boolean isvBS;        // True if printing of bytestack demanded
static boolean isbyteselect; // True if byte selecting is on
//static boolean isclock;      // True if clock is on
static boolean isinsertbyte; // True if byte should be insertet in prg[]
static boolean isnewnumber;  // True if stack has to be lifted before entering a new number
static boolean ispushed;     // True if stack was already pushed by ENTER
static boolean isdot;        // True if dot was pressed and decimals will be entered
//static boolean isshift;      // True if shift was pressed in calculator mode
static boolean ismenu;       // True if MENU was demanded in calculator mode
static boolean isshow;       // True, if SHOW was demanded
static boolean isterm;       // True, if terminal controls

// Messages
const char m0[] PROGMEM = "Error"; // Error
const char m1[] PROGMEM = "OK";    // OK
const char m2[] PROGMEM = "NOMEM"; // No EEPROM
const char m3[] PROGMEM = "STOP";  // Program execution stopped
const char* const msg_table[] PROGMEM = {m0, m1, m2, m3};


// COMMANDS

const char c0[] PROGMEM = "B!=";    // True if X <> Y on bytestack
const char c1[] PROGMEM = "B%";     // Y modulo X
const char c2[] PROGMEM = "B&";     // Logical AND X and Y on bytestack
const char c3[] PROGMEM = "B*";     // Multiply X and Y on bytestack
const char c4[] PROGMEM = "B+";     // Add X and Y on bytestack
const char c5[] PROGMEM = "B-";     // Subtract X from Y on bytestack
const char c6[] PROGMEM = "B/";     // Divide Y by X on bytestack
const char c7[] PROGMEM = "B2F";    // Push bytestack to floatstack
const char c8[] PROGMEM = "B<";     // True if X < Y on bytestack
const char c9[] PROGMEM = "B<=";    // True if X <= Y on bytestack
const char c10[] PROGMEM = "B=";    // True if X = Y on bytestack
const char c11[] PROGMEM = "B>";    // True if X > Y on bytestack
const char c12[] PROGMEM = "B>=";   // True if X >= Y on bytestack
const char c13[] PROGMEM = "Bswap"; // Swap bytestack
const char c14[] PROGMEM = "B|";    // Logical OR X from Y on bytestack
const char c15[] PROGMEM = "B~";    // Negate byte X on bytestack
const char c16[] PROGMEM = "CALC";  // Set calculator mode
const char c17[] PROGMEM = "cB";    // Clear bytestack
const char c18[] PROGMEM = "D";     // Disk system
const char c19[] PROGMEM = "Dfmt";  // Format disk system
const char c20[] PROGMEM = "F!";    // Calculate GAMMA ... !
const char c21[] PROGMEM = "F*";    // Multiply X and Y on floatstack
const char c22[] PROGMEM = "F+";    // Add X and Y on floatstack
const char c23[] PROGMEM = "F-";    // Subtract X from Y on floatstack
const char c24[] PROGMEM = "F/";    // Divide Y by X on floatstack
const char c25[] PROGMEM = "F2h";   // Convert H.MS to H
const char c26[] PROGMEM = "F2hms"; // Convert H to H.MS
const char c27[] PROGMEM = "F2p";   // Convert rectangular to polar coordinates
const char c28[] PROGMEM = "F2r";   // Convert polar to rectangular coordinates
const char c29[] PROGMEM = "Facos"; // Calculate ACOS(X)
const char c30[] PROGMEM = "Facsh"; // Calculate ACOSH(X)
const char c31[] PROGMEM = "Fannu"; // Calculate annuity/preset value (Y: rate, X:years)
const char c32[] PROGMEM = "Fasin"; // Calculate ASIN(X)
const char c33[] PROGMEM = "Fasnh"; // Calculate ASINH(X)
const char c34[] PROGMEM = "Fatan"; // Calculate ATAN(X)
const char c35[] PROGMEM = "Fatnh"; // Calculate ATANH(X)
const char c36[] PROGMEM = "Fce";   // Clear last entry of floatstack
const char c37[] PROGMEM = "Fchs";  // Change sign of X on floatstack
const char c38[] PROGMEM = "Fclx";  // Clear X of floatstack
const char c39[] PROGMEM = "Fcos";  // Calculate COS(X)
const char c40[] PROGMEM = "Fcosh"; // Calculate COSH(X)
const char c41[] PROGMEM = "FEE";   // Calculate EE (Y*10^X) on floatstack
const char c42[] PROGMEM = "Fexp";  // Calculate EXP(X)
const char c43[] PROGMEM = "Fgaus"; // Calculate CDF (Cumulative Distribution Function) and PDF (Probability Density Function)
const char c44[] PROGMEM = "Finv";  // Calculate 1/X
const char c45[] PROGMEM = "Fln";   // Calculate LN( X)
const char c46[] PROGMEM = "FLR";   // Calculate L.R. (Xprev, Yprev)
const char c47[] PROGMEM = "Fpow";  // Calculate POW(Y,X) ... Y^X
const char c48[] PROGMEM = "Frcl";  // Recall float memory to floatstack (X)
const char c49[] PROGMEM = "Frot";  // Rotate floatstack
const char c50[] PROGMEM = "Fsin";  // Calculate SIN(X)
const char c51[] PROGMEM = "Fsinh"; // Calculate SINH(X)
const char c52[] PROGMEM = "Fsqrt"; // Calculate SQRT(X)
const char c53[] PROGMEM = "Fstat"; // Calculate statistics STAT ... X: mean Y: standard deviation
const char c54[] PROGMEM = "Fsto";  // Store floatstack (X) to float memory
const char c55[] PROGMEM = "Fsum";  // Input of statistical data (SUM)
const char c56[] PROGMEM = "Fswap"; // Swap X and Y on floatstack
const char c57[] PROGMEM = "Ftan";  // Calculate TAN(X)
const char c58[] PROGMEM = "Ftan";  // Calculate TANH(X)
const char c59[] PROGMEM = INPUTBYTE; // Input byte and push to bytestack
const char c60[] PROGMEM = "iB2";   // Input 2 bytes and push to bytestack
const char c61[] PROGMEM = "iB4";   // Input 4 bytes and push to bytestack
const char c62[] PROGMEM = "KBD";   // Keyboard mode
const char c63[] PROGMEM = "pause"; // Pause for PAUSE milliseconds
const char c64[] PROGMEM = "pB";    // Push bytestack
const char c65[] PROGMEM = "Pedit"; // Edit prg[]
const char c66[] PROGMEM = "Pexp";  // Export prg[]
const char c67[] PROGMEM = "pF";    // Push floatstack
const char c68[] PROGMEM = GOTO;    // Goto address
const char c69[] PROGMEM = "Pif";   // Skip next 2 program steps if bytestack is true (X > 0)
const char c70[] PROGMEM = "Pimp";  // Import prg[]
const char c71[] PROGMEM = "Prun";  // Run prg[]
const char c72[] PROGMEM = "reset"; // Reset device
const char c73[] PROGMEM = "sFi";   // Set function key and save to EEPROM
const char c74[] PROGMEM = "sLIT";  // Set lit (contrast) 0...255
const char c75[] PROGMEM = "sPn";   // Set program name (ZYX)
const char c76[] PROGMEM = "stop";  // Stops program execution till keypress
const char c77[] PROGMEM = "TERM";  // Toggle terminal control
const char c78[] PROGMEM = "vB";    // View bytestack (till next keypress)
const char c79[] PROGMEM = "vB+";   // View bytestack (large)
const char c80[] PROGMEM = "WATCH"; // Show watch
const char c81[] PROGMEM = "Wset";  // Set watch from bytestack (YY:XX)
const char* const cmd[] PROGMEM = {
  c0, c1, c2, c3, c4, c5,  c6,  c7,  c8,  c9,  c10, c11, c12, c13, c14, c15, c16, c17, c18, c19, c20,
  c21, c22, c23, c24, c25, c26, c27, c28, c29, c30, c31, c32, c33, c34, c35, c36, c37, c38, c39, c40,
  c41, c42, c43, c44, c45, c46, c47, c48, c49, c50, c51, c52, c53, c54, c55, c56, c57, c58, c59, c60,
  c61, c62, c63, c64, c65, c66, c67, c68, c69, c70, c71, c72, c73, c74, c75, c76, c77, c78, c79, c80,
  c81
};
#define numberofcommands (sizeof(cmd)/sizeof(const char *))

// Function pointer array
static void Bne(void) {
  bytestack[1] = (bytestack[0] != bytestack[1]);
  pullbytestack();
}
static void Bmod(void) {
  bytestack[0] = bytestack[1] % bytestack[0];
  pullupperbytestack();
}
static void Band(void) {
  bytestack[0] &= bytestack[1];
  pullupperbytestack();
}
static void Bmult(void) {
  bytestack[0] *= bytestack[1];
  pullupperbytestack();
}
static void Badd(void) {
  bytestack[0] += bytestack[1];
  pullupperbytestack();
}
static void Bsub(void) {
  bytestack[0] = bytestack[1] - bytestack[0];
  pullupperbytestack();
}
static void Bdiv(void) {
  bytestack[0] = bytestack[1] / bytestack[0];
  pullupperbytestack();
}
static void B2F(void) {
  floatstack[0] = floatstack[0] * 10 + bytestack[0] - '0'; // Append digit to number
  //dd();
  pullbytestack();
}
static void Blt(void) {
  bytestack[1] = (bytestack[0] <= bytestack[1]);
  pullbytestack();
}
static void Ble(void) {
  bytestack[1] = (bytestack[0] <= bytestack[1]);
  pullbytestack();
}
static void Beq(void) {
  bytestack[1] = (bytestack[0] == bytestack[1]);
  pullbytestack();
}
static void Bgt(void) {
  bytestack[1] = (bytestack[0] > bytestack[1]);
  pullbytestack();
}
static void Bge(void) {
  bytestack[1] = (bytestack[0] >= bytestack[1]);
  pullbytestack();
}
static void Bswap(void) {
  byte tmp = bytestack[0];
  bytestack[0] = bytestack[1];
  bytestack[1] = tmp;
}
static void Bor(void) {
  bytestack[0] |= bytestack[1];
  pullupperbytestack();
}
static void Bneg(void) {
  bytestack[0] = ~bytestack[0];
}
static void CALC(void) {
  mode = CALCMODE;
}
static void cB(void) {
  for (byte i = 0; i < BYTESTACKSIZE; i++) bytestack[i] = 0;
}
static void D(void) {
  mode = FSMODE;
}
static void Dfmt(void) {
  EEPROM.write(EEADDRFSSTART, NULL);
  fsptr = EEADDRFSSTART;
}
static void Fgamma(void) {
  // 1 + ENTER ENTER INV EXP ENTER INV CHS + 2 / * SWAP 6 PWR 810 * INV + LN * 2 /
  floatstack[0] += 1;
  pF();
  pF();
  Finv();
  Fexp();
  pF();
  Finv();
  Fchs();
  Fadd();
  floatstack[0] /= 2;
  Fmult();
  Fswap();
  pF();
  floatstack[0] = 6;
  Fpow();
  floatstack[0] *= 810; //37791360
  Finv();
  Fadd();
  Fln();
  Fmult();
  floatstack[0] /= 2;
  // SWAP CHS + ROT ROT LN SWAP .5 - * + .9189385 + EXP
  Fswap();
  Fchs();
  Fadd();
  Frot();
  Frot();
  Fln();
  Fswap();
  floatstack[0] -= 0.5;
  Fmult();
  Fadd();
  floatstack[0] += 0.9189385;
  Fexp();
}
static void Fmult(void) {
  floatstack[0] *= floatstack[1];
  pullupperfloatstack();
}
static void Fadd(void) {
  floatstack[0] += floatstack[1];
  pullupperfloatstack();
}
static void Fsub(void) {
  floatstack[0] = floatstack[1] - floatstack[0];
  pullupperfloatstack();
}
static void Fdiv(void) {
  floatstack[0] = floatstack[1] / floatstack[0];
  pullupperfloatstack();
}
static void F2h(void) {
  floatstack[0] = (int)floatstack[0] + ((int)(100 * (floatstack[0] - (int)floatstack[0]))) / 60.0 + (100 * (100 * floatstack[0] - (int)(100 * floatstack[0]))) / 3600.0;
}
static void F2hms(void) {
  floatstack[0] = (int)floatstack[0] + ((int)(60 * (floatstack[0] - (int)floatstack[0]))) / 100.0 + (60 * ((60 * (floatstack[0] - (int)floatstack[0])) - (int)(60 * (floatstack[0] - (int)floatstack[0])))) / 10000.0;
}
static void F2p(void) { // R2P: ENTER ROT ROT SWAP ROT ENTER ROT SWAP / ATAN ROT ROT * ROT ROT * + SQRT
  pF();
  Frot();
  Frot();
  Fswap();
  Frot();
  pF();
  Frot();
  Fswap();
  Fdiv();
  Fatan();
  Frot();
  Frot();
  Fmult();
  Frot();
  Frot();
  Fmult();
  Fadd();
  Fsqrt();
}
static void F2r(void) { // P2R: SWAP SIN ENTER ENTER * CHS 1 + SQRT ROT * ROT * SWAP ROT
  Fswap();
  Fsin();
  pF();
  pF();
  Fmult(); Fchs();
  floatstack[0] += 1.0;
  Fsqrt();
  Frot();
  Fmult();
  Frot();
  Fmult();
  Fswap();
  Frot();
}
static void Facos(void) { // ACOS: ASIN CHS 90 +
  Fasin();
  Fchs();
  floatstack[0] += 90;
}
static void Facosh(void) { // ACOSH: ENTER ENTER * 1 - SQRT + LN
  pF();
  pF();
  Fmult();
  floatstack[0] -= 1;
  Fsqrt();
  Fadd();
  Fln();
}
static void Fannu(void) { // ANNU: SWAP ENTER ENTER ROT ROT 1 + SWAP PWR INV CHS 1 + SWAP /
  Fswap();
  pF();
  pF();
  Frot();
  Frot();
  floatstack[0] += 1.0;
  Fswap();
  Fpow();
  Finv();
  Fchs();
  floatstack[0] += 1.0;
  Fswap();
  Fdiv();
}
static void Fasin(void) {
  floatstack[0] = atan(floatstack[0] / _sqrt(1 - floatstack[0] * floatstack[0])) * RAD;
}
static void Fasinh(void) { // ASINH: ENTER ENTER * 1 + SQRT + LN
  pF();
  pF();
  Fmult();
  floatstack[0] += 1;
  Fsqrt();
  Fadd();
  Fln();
}
static void Fatan(void) {
  floatstack[0] = atan(floatstack[0]) * RAD;
}
static void Fatanh(void) { // ATANH: ENTER ENTER 1 + SWAP CHS 1 + / SQRT LN
  pF();
  pF();
  floatstack[0] += 1;
  Fswap();
  Fchs();
  floatstack[0] += 1;
  Fdiv();
  Fsqrt();
  Fln();
}
static void Fcos(void) { // COS: SIN ENTER * CHS 1 + SQRT
  Fsin();
  pF();
  Fmult();
  Fchs();
  floatstack[0] += 1;
  Fsqrt();
}
static void Fcosh(void) { // COSH: EXP ENTER INV + 2 /
  Fexp();
  pF();
  Finv();
  Fadd();
  floatstack[0] /= 2;
}
static void Fce(void) {
  if (isdot) {
    if (floatstack[0] > TINYNUMBER && decimals > 0)
      floatstack[0] -= ((long)(floatstack[0] * _pow10(decimals)) % 10) / _pow10(decimals--);
    else isdot = false;
  }
  else floatstack[0] = (long)(floatstack[0] / 10.0);
}
static void Fchs(void) {
  floatstack[0] = -floatstack[0];
}
static void Fclx(void) {
  floatstack[0] = 0.0;
}
static void Fee(void) {
  if (floatstack[0] <= MAXEE) {
    floatstack[0] = floatstack[1] * _pow10(floatstack[0]);
    pullupperfloatstack();
  }
}
static void Fexp(void) {
  floatstack[0] = _exp_sin(floatstack[0], true);
  isnewnumber = true;
}
static void Fgaus(void) {
  // GAUSS-CDF: ENTER ENTER ENTER * * .07 * CHS SWAP 1.6 * CHS + EXP 1 + INV
  //            SWAP
  //       PDF: ENTER * CHS 2 / EXP 0.3989423 *
  pF();
  pF();
  pF();
  Fmult();
  Fmult();
  floatstack[0] *= 0.07;
  Fchs();
  Fswap();
  floatstack[0] *= 1.6;
  Fchs();
  Fadd();
  Fexp();
  floatstack[0] += 1.0;
  Finv();
  Fswap();
  pF();
  Fmult();
  Fchs();
  floatstack[0] /= 2.0;
  Fexp();
  floatstack[0] *= 0.3989423;
}
static void Finv(void) {
  floatstack[0] = 1.0 / floatstack[0];
}
static void Fln(void) {
  floatstack[0] = log(floatstack[0]);
}
static void FLR(void) {
  pushfloatstack();
  double tmp = (sxy / sn - sx * sy / sn / sn) / (sxx / sn - sx * sx / sn / sn);
  floatstack[1] = sy / sn - tmp * sx / sn + tmp * floatstack[0]; // ->y
  floatstack[0] = (floatstack[0] - sy / sn + tmp * sx / sn) / tmp; // ->x
}
static void Fpow(void) {
  floatstack[0] = _exp_sin(floatstack[0] * log(floatstack[1]), true);
  pullupperfloatstack();
}
static void Frcl(void) {
  pushfloatstack();
  floatstack[0] = floatstack[FLOATSTACKSIZE];
}
static void Frot(void) {
  double tmp = floatstack[0];
  pullfloatstack();
  floatstack[FLOATSTACKSIZE - 1] = tmp;
}
static void Fsin(void) {
  floatstack[0] = _exp_sin(floatstack[0] / RAD, false);
}
static void Fsinh(void) { // SINH: EXP ENTER INV CHS + 2 /
  Fexp();
  pF();
  Finv();
  Fchs();
  Fadd();
  floatstack[0] /= 2;
}
static void Fsqrt(void) {
  if (floatstack[0] > 0.0) floatstack[0] = _sqrt(floatstack[0]);
}
static void Fstat(void) {
  pF();
  floatstack[1] = _sqrt((sxx - sx * sx / sn) / (sn - 1)); // stdev
  floatstack[0] = sx / sn; // mean
}
static void Fsto(void) {
  floatstack[FLOATSTACKSIZE] = floatstack[0];
}
static void Fsum(void) {
  sx += floatstack[0];
  sy += floatstack[1];
  sxx += floatstack[0] * floatstack[0];
  sxy += floatstack[0] * floatstack[1];
  floatstack[0] = ++sn;
}
static void Fswap(void) {
  double tmp = floatstack[0];
  floatstack[0] = floatstack[1];
  floatstack[1] = tmp;
}
static void Ftan(void) { // TAN: SIN ENTER ENTER * CHS 1 + SQRT INV *
  Fsin();
  pF();
  pF();
  Fmult();
  Fchs();
  floatstack[0] += 1;
  Fsqrt();
  Finv();
  Fmult();
}
static void Ftanh(void) { // TANH: EXP ENTER ENTER INV CHS + SWAP ENTER INV + /
  Fexp();
  pF();
  pF();
  Finv();
  Fchs();
  Fadd();
  Fswap();
  pF();
  Finv();
  Fadd();
  Fdiv();
}
static void i2B(void) {
  oldmode = CMDMODE;
  mode = INMODE;
  inputbytes = 1;
}
static void i2B2(void) {
  mode = INMODE;
  inputbytes = 2;
}
static void i2B4(void) {
  mode = INMODE;
  inputbytes = 4;
}
static void KBD(void) {
  mode = (mode == KBDMODE) ? CMDMODE : KBDMODE;
}
static void pause(void) {
  delay(PAUSE);
}
static void pB(void) {
  pushbytestack();
}
static void Pedit(void) {
  mode = EDITMODE;
}
static void Pexp(void) {
  Keyboard.press(KEY_LEFT_ALT); Keyboard.press(KEY_F1); delay(200); Keyboard.releaseAll();
  Keyboard.print("minicom -b 9600 -D /dev/ttyACM0 -C serial.dmp"); Keyboard.write(KEY_RETURN);
  delay(200);
  Serial.begin(9600);
  for (byte i = 0; i < prg[0]; i++) {
    Serial.write(prg[i]);
  }
  delay(200);
  Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('a'); delay(200); Keyboard.releaseAll();
  Keyboard.write('q'); Keyboard.write(KEY_RETURN);
  Keyboard.print("exit"); Keyboard.write(KEY_RETURN);
  oldmode = CMDMODE;
  mode = MSGMODE;
  msg = 1;
}
static void pF(void) {
  pushfloatstack();
}
static void Pgoto(void) {
}
static void Pif(void) {
  if (bytestack[0] != 0) prgptr += 2;
  pullbytestack();
}
static void Pimp(void) {
  Keyboard.press(KEY_LEFT_ALT); Keyboard.press(KEY_F1); delay(200); Keyboard.releaseAll();
  Keyboard.print("cat serial.dmp > /dev/ttyACM0"); Keyboard.write(KEY_RETURN);
  byte i = 0;
  while (Serial.available() <= 0) ;
  prg[0] = Serial.read();
  for (byte i = 1; i < prg[0]; i++) prg[i] = Serial.read();
  Keyboard.print("exit"); Keyboard.write(KEY_RETURN);
  oldmode = CMDMODE;
  mode = MSGMODE;
  msg = 1;
}
static void Prun(void) {
  prgptr = PRGPTRSTART;
  mode = RUNMODE;
}
static void rst(void) {
  reset();
}
static void sFi(void) {
  EEPROM.write(EEADDRFKEYS + bytestack[0] - 1, bytestack[1]);
  f[bytestack[0] - 1] = bytestack[1];
  pullbytestack();
  pullbytestack();
}
static void sLIT(void) {
  setcontrast(bytestack[0]);
  pullbytestack();
}
static void sPn(void) {
  for (byte i = 1; i < PRGPTRSTART; i++) prg[i] = bytestack[PRGPTRSTART - 1 - i];
}
static void stp(void) {
  oldmode = CMDMODE;
  mode = MSGMODE;
  msg = 3;
  printscreen();
}
static void term(void) {
  isterm = isterm ? false : true;
}
static void vB(void) {
  oldmode = mode;
  mode = VIEWBS;
  printscreen();
}
static void vBplus(void) {
  printdata();
}
static void WATCH(void) {
  printclock();
  mode = CLKMODE;
}
static void Wset(void) {
  setclock();
  mode = CLKMODE;
}
static void (*dispatch[])(void) = { // Function pointer array
  &Bne,
  &Bmod,
  &Band,
  &Bmult,
  &Badd,
  &Bsub,
  &Bdiv,
  &B2F,
  &Blt,
  &Ble,
  &Beq,
  &Bgt,
  &Bge,
  &Bswap,
  &Bor,
  &Bneg,
  &CALC,
  &cB,
  &D,
  &Dfmt,
  &Fgamma,
  &Fmult,
  &Fadd,
  &Fsub,
  &Fdiv,
  &F2h,
  &F2hms,
  &F2p,
  &F2r,
  &Facos,
  &Facosh,
  &Fannu,
  &Fasin,
  &Fasinh,
  &Fatan,
  &Fatanh,
  &Fce,
  &Fchs,
  &Fclx,
  &Fcos,
  &Fcosh,
  &Fee,
  &Fexp,
  &Fgaus,
  &Finv,
  &Fln,
  &FLR,
  &Fpow,
  &Frcl,
  &Frot,
  &Fsin,
  &Fsinh,
  &Fsqrt,
  &Fstat,
  &Fsto,
  &Fsum,
  &Fswap,
  &Ftan,
  &Ftanh,
  &i2B,
  &i2B2,
  &i2B4,
  &KBD,
  &pause,
  &pB,
  &Pedit,
  &Pexp,
  &pF,
  &Pgoto,
  &Pif,
  &Pimp,
  &Prun,
  &rst,
  &sFi,
  &sLIT,
  &sPn,
  &stp,
  &term,
  &vB,
  &vBplus,
  &WATCH,
  &Wset,
};


// SUBROUTINES

// Macros
#define _abs(x) ((x<0)?(-x):(x)) // abs()-substitute macro
#define _ones(x) ((x)%10)        // Calculates ones unit
#define _tens(x) (((x)/10)%10)   // Calculates tens unit
#define _huns(x) (((x)/100)%10)  // Calculates hundreds unit
#define _tsds(x) (((x)/1000)%10) // Calculates thousands unit

void WireStartCmdStream(void) { // Begin command stream
  Wire.beginTransmission(OLED_I2C_ADDRESS); // Begin I2C communications
  Wire.write(OLED_CONTROL_BYTE_CMD_STREAM); // Tell that a command stream is incoming
}

void WireStartDataStream(void) { // Begin data stream
  Wire.beginTransmission(OLED_I2C_ADDRESS);
  Wire.write(OLED_CONTROL_BYTE_DATA_STREAM);
}

void oledinit(void) { // Init OLED display
  Wire.begin(); // Init I2C (SDA/SCK) A4/A5 on Uno board; 2/3 on Genuino/Leonardo
  WireStartCmdStream();
  Wire.write(OLED_CMD_DISPLAY_OFF); // Turn the Display OFF
  Wire.write(OLED_CMD_SET_MUX_RATIO); // Set mux ratio t0 select max number of rows - 64
  Wire.write(0x3F);
  Wire.write(OLED_CMD_SET_DISPLAY_OFFSET); // Set the display offset to 0
  Wire.write(0x00);
  Wire.write(OLED_CMD_SET_DISPLAY_START_LINE); // Display start line to 0
  Wire.write(OLED_CMD_SET_SEGMENT_REMAP); // Screen orientation 0xA0 or flip (0xA1)
  Wire.write(OLED_CMD_SET_COM_SCAN_MODE); // Screen 0xC0 or mirroring (0xC1)
  Wire.write(OLED_CMD_SET_COM_PIN_MAP); // Pin mapping
  Wire.write(0x22); // 0x12 for 128x64, 0x22 for 128x32
  Wire.write(OLED_CMD_SET_CONTRAST); // Set contrast
  Wire.write(DEFAULTCONTRAST);
  Wire.write(OLED_CMD_DISPLAY_RAM); // Enable rendering from GDDRAM (Graphic Display Data RAM)
  Wire.write(OLED_CMD_DISPLAY_NORMAL); // Normal mode (not inverted)
  Wire.write(OLED_CMD_SET_DISPLAY_CLK_DIV); // Default oscillator clock
  Wire.write(0x80);
  Wire.write(OLED_CMD_SET_CHARGE_PUMP);  // Enable the charge pump
  Wire.write(0x14);
  Wire.write(OLED_CMD_SET_PRECHARGE); // Set precharge cycles to high cap type
  Wire.write(0x22);
  Wire.write(OLED_CMD_SET_VCOMH_DESELCT); // Set the V_COMH deselect voltage to max
  Wire.write(0x30);
  Wire.write(OLED_CMD_SET_MEMORY_ADDR_MODE); // Horizontal addressing mode
  Wire.write(0x00);
  Wire.write(OLED_CMD_DISPLAY_ON); // Turn the Display ON
  Wire.endTransmission(); // End the I2C communications
}

void oledoff(void) { // Deactivate display
  WireStartCmdStream();
  Wire.write(OLED_CMD_DISPLAY_OFF);
  Wire.endTransmission();
}
void oledon(void) { // Activate display
  WireStartCmdStream();
  Wire.write(OLED_CMD_DISPLAY_ON);
  Wire.endTransmission();
}
void setcontrast(byte c) { // Set contrast
  WireStartCmdStream();
  Wire.write(OLED_CMD_SET_CONTRAST);
  Wire.write(c);
  Wire.endTransmission();
}

static void oledsetarea(byte x, byte width, byte y, byte height) { // Set area for printing
  WireStartCmdStream();
  Wire.write(OLED_CMD_SET_COLUMN_RANGE);
  Wire.write(x);
  Wire.write(x + width - 1);
  Wire.write(OLED_CMD_SET_PAGE_RANGE);
  Wire.write(y);
  Wire.write(y + height - 1);
  Wire.endTransmission();
}

static void oledclr(void) { // Clear OLED screen
  oledsetarea(0, OLED_COL + 1, 0, OLED_ROW + 1);
  for (int i = 0; i < 32; i++) {
    WireStartDataStream();
    for (byte j = 0; j < 16; j++) Wire.write(0x00);
    Wire.endTransmission();
  }
}

static byte expand4bit(byte b) { // 0000abcd  Expand 4 bits (lower nibble)
  b = (b | (b << 2)) & 0x33;     // 00ab00cd
  b = (b | (b << 1)) & 0x55;     // 0a0b0c0d
  return (b | (b << 1));         // aabbccdd
}

static byte expand2bit(byte b) { // Expand 2 bits 000000ab
  b = (b | (b << 3)) & 0x11;                   // 000a000b
  for (byte i = 0; i < 3; i++) b |= (b << 1);  // aaaabbbb
  return (b);
}

static void printnumber(byte x, byte y, byte ch) { // Print number
  oledsetarea(x, 7, y, 2);
  WireStartDataStream();
  for (byte k = 0; k < 2; k++)
    for (byte j = 0; j < 7; j++) {
      Wire.write(pgm_read_byte(&font7x16[14 * ch + j + k * 7]));
    }
  Wire.endTransmission();
}

static void printnumber2(byte x, byte ch) { // Print number
  oledsetarea(x, 14, 0, 4);
  for (byte k = 0; k < 2; k++) {
    WireStartDataStream();
    for (byte j = 0; j < 7; j++) {
      byte tmp = expand4bit(pgm_read_byte(&font7x16[14 * ch + j + k * 7]) & 0x0f);
      for (byte i = 0; i < 2; i++) Wire.write(tmp);
    }
    for (byte j = 0; j < 7; j++) {
      byte tmp = expand4bit(pgm_read_byte(&font7x16[14 * ch + j + k * 7]) >> 4);
      for (byte i = 0; i < 2; i++) Wire.write(tmp);
    }
    Wire.endTransmission();
  }
  if (isterm) {
    if (ch + '0' == ':') Serial.write('-');
    else if (ch + '0' == ';') Serial.write(' ');
    else if (ch + '0' == '<') Serial.write('>');
    else Serial.write(ch + '0');
  }
}

static void printdot(byte x) { // Print big dot
  oledsetarea(x, 5, 3, 0);
  WireStartDataStream();
  for (byte j = 0; j < 5; j++) Wire.write(pgm_read_byte(&fontdot[j]));
  Wire.endTransmission();
}

static void print(byte x, byte y, byte ch) { // Print character (small font)
  oledsetarea(x, 5, y, 1);
  WireStartDataStream();
  for (byte j = 0; j < 5; j++) {
    Wire.write(pgm_read_byte(&font5x8[5 * ch + j]));
  }
  Wire.endTransmission();
  if (isterm) Serial.write(ch + ' ');
}

static void print2(byte x, byte y, byte ch) { // Print double-sized character (small font)
  oledsetarea(x, 10, y, 2);
  WireStartDataStream();
  for (byte k = 0; k < 2; k++) { // Two nibbles
    for (byte j = 0; j < 5; j++) {
      byte tmp = expand4bit((pgm_read_byte(&font5x8[5 * ch + j]) >> (k * 4)) & 0x0f); // Expand 0000abcd
      for (byte i = 0; i < 2; i++) Wire.write(tmp);
    }
  }
  Wire.endTransmission();
  if (isterm) Serial.write(ch + ' ');
}

static void print4(byte x, byte y, byte ch) { // Print quad-sized character (small font)
  oledsetarea(x, 20, y, 4);
  for (byte k = 0; k < 4; k++) { // 4 pairs of bits (ab cd ef gh)
    WireStartDataStream();
    for (byte j = 0; j < 5; j++) {
      byte tmp = expand2bit((pgm_read_byte(&font5x8[5 * ch + j]) >> (2 * k)) & 0x03); // Expand 000000ab
      for (byte i = 0; i < 4; i++) Wire.write(tmp);
    }
    Wire.endTransmission();
  }
  if (isterm) Serial.write(ch + ' ');
}

static void printL(void) { // Print buffer[] to OLED (large)
  for (byte i = 0; i < strlen(strbuf); i++) print4(i * 25 + 2, 0 , strbuf[i] - ' ');
  if (isterm) Serial.println();
}

static void printM(byte pos) { // Print buffer[] to OLED (medium) at position 1, 2, 3 or 4
  for (byte i = 0; i < strlen(strbuf); i++)
    print2(((pos & 0x01) ? 0 : 64) + i * 12 + 1, (pos > 2) ? 2 : 0 , strbuf[i] - ' ');
  if (isterm) Serial.println();
}

//printS

/*static void printbuffer(void) { // Print buffer[] to OLED
  while (strlen(strbuf) < 6) strcat(strbuf, " "); // Expand buffer with " "
  byte i = 0;
  while (strbuf[i]) print4(i++ * 24, 0, strbuf[i] - ' ');
  }*/

static void printmsg(byte nr) { // Print message
  strcpy_P(strbuf, (char*)pgm_read_word(&(msg_table[nr]))); // Copy string to buffer
  while (strlen(strbuf) < 5) strcat(strbuf, " "); // Expand buffer with " "
  printL();
}

static void printstep(byte back, byte location) { // Print program step (actual: back=0, before: back=1)
  strbuf[0] = NULL;
  strcat(strbuf, "...-");
  byte tmp = prgptr - back - PRGPTRSTART + 1;
  strbuf[0] = _huns(tmp) + '0';
  strbuf[1] = _tens(tmp) + '0';
  strbuf[2] = _ones(tmp) + '0';

  byte tmpstr[] = ".....";
  if (prg[prgptr - back - 1] >= CMDOFFSET && prg[prgptr - back - 1] <= CMDOFFSET + numberofcommands)
    strcpy_P(tmpstr, (char*)pgm_read_word(&(cmd[prg[prgptr - back - 1] - CMDOFFSET])));

  if (strcmp(tmpstr, GOTO) == 0 ||
      prg[prgptr - back] < CMDOFFSET ||
      prg[prgptr - back] > CMDOFFSET + numberofcommands) { // Print number (if prev = goto or cmd is out of range)
    strcat(strbuf, "...");
    tmp = prg[prgptr - back];
    strbuf[4] = _huns(tmp) + '0';
    strbuf[5] = _tens(tmp) + '0';
    strbuf[6] = _ones(tmp) + '0';
  }
  else { // Print cmd
    strcpy_P(tmpstr, (char*)pgm_read_word(&(cmd[prg[prgptr - back] - CMDOFFSET ])));
    strcat(strbuf, tmpstr);
  }
  printM(location);
}

static void printee(int addr, byte location) { // Print EEPROM-entry
  strbuf[0] = NULL;
  strcat(strbuf, "...-");
  for (byte i = 0; i < PRGPTRSTART - 1; i++) strbuf[i] = EEPROM.read(addr + i + 1);
  strcat(strbuf, "...");
  byte t = EEPROM.read(addr);
  strbuf[4] = _huns(t) + '0';
  strbuf[5] = _tens(t) + '0';
  strbuf[6] = _ones(t) + '0';
  printM(location);
}

static void printscreen(void) { // Prints screen due to modes and flags
  oledclr();
  if (mode == CMDMODE) {
    strcpy_P(strbuf, (char*)pgm_read_word(&(cmd[cmdselect])));
    printL();
  }

  else if (mode == INMODE || mode == VIEWBS || mode == EDITINMODE || mode == KBDMODE) {
    byte b = byteselect;
    if (mode == VIEWBS) {
      b = bytestack[0];
      mode = oldmode;
    }
    strbuf[0] = NULL; // dec
    strcat(strbuf, "___");
    if (b > 99) strbuf[0] = _huns(b) + '0';
    if (b > 9) strbuf[1] = _tens(b) + '0';
    strbuf[2] = _ones(b) + '0';
    printM(1);
    strbuf[0] = NULL; // hex
    strcat(strbuf, "__h");
    if (b > 15) strbuf[0] = hex[(b >> 4) & 0x0f];
    strbuf[1] = hex[b & 0x0f];
    printM(3);
    if (b >= ' ' && b <= '~') { // char
      strbuf[0] = NULL; // char
      strcat(strbuf, "_");
      strbuf[0] = b;
      printM(2);
    }
    //    if (b < numberofcommands) { // cmd
    if (b >= CMDOFFSET && b < CMDOFFSET + numberofcommands) { // cmd
      strbuf[0] = NULL;
      strcat(strbuf, ".....");
      strcpy_P(strbuf, (char*)pgm_read_word(&(cmd[b - CMDOFFSET])));
      printM(4);
    }
  }

  else if (mode == EDITMODE) {
    if (prgptr == PRGPTRSTART) { // Begin of prg (print name)
      strbuf[0] = NULL;
      strcat(strbuf, "...: ...");
      memmove(&strbuf[0], &prg[1], (PRGPTRSTART - 1)*sizeof(byte));
      strbuf[5] = _huns(prg[0]) + '0';
      strbuf[6] = _tens(prg[0]) + '0';
      strbuf[7] = _ones(prg[0]) + '0';
      printM(1);
      if (prg[0] <= PRGPTRSTART) { // Empty program
        strbuf[0] = NULL;
        strcat(strbuf, "END");
        printM(3);
      }
      else { // First command
        printstep(0, 3);
      }
    }
    else { // Not begin of prg
      if (prgptr < prg[0]) { // Middle
        printstep(1, 1);
        printstep(0, 3);
      }
      else { // End
        printstep(1, 1);
        strbuf[0] = NULL;
        strcat(strbuf, "END");
        printM(3);
      }
    }
  }

  else if (mode == FSMODE) {
    /*Serial.print("fsptr="); Serial.print(fsptr);
      for (int i = EEADDRFSSTART; i < EEADDRFSSTART + 20; i++) {
      Serial.print(" "); Serial.print(EEPROM.read(i));
      }
      Serial.println();*/
    if (fsptr == EEADDRFSSTART) { // Begin of file system (print disk free)
      strbuf[0] = NULL;
      strcat(strbuf, "FREE: ....");
      while (EEPROM.read(fsptr) != NULL) fsptr += EEPROM.read(fsptr);
      int tmp = EEADDRFSEND - fsptr;
      strbuf[6] = _tsds(tmp) + '0';
      strbuf[7] = _huns(tmp) + '0';
      strbuf[8] = _tens(tmp) + '0';
      strbuf[9] = _ones(tmp) + '0';
      printM(1);
      fsptr = EEADDRFSSTART;
      if (EEPROM.read(fsptr) == 0) { // Empty file system
        strbuf[0] = NULL;
        strcat(strbuf, "END");
        printM(3);
      }
      else { // First entry
        printee(fsptr, 3);
      }
    }
    else { // Not begin of file system
      if (EEPROM.read(fsptr) == NULL) { // End of FS
        int tmp = EEADDRFSSTART;
        while (tmp + EEPROM.read(tmp) < fsptr && EEPROM.read(tmp) != NULL)
          tmp += EEPROM.read(tmp);
        printee(tmp, 1);
        strbuf[0] = NULL;
        strcat(strbuf, "END");
        printM(3);
      }
      else { // Middle of FS
        int tmp = EEADDRFSSTART;
        while (tmp + EEPROM.read(tmp) < fsptr && EEPROM.read(tmp) != NULL)
          tmp += EEPROM.read(tmp);
        printee(tmp, 1);
        printee(fsptr, 3);
      }
    }
  }

  else if (mode == CALCMODE) {
    oledclr();
    strbuf[0] = NULL;
    strcat(strbuf, ";;;;;;;;");

    double f = floatstack[0];
    long m; // Mantissa
    int8_t e; // Exponent
    byte dot = 2; // Set local variables for "normal print"
    byte mantdigits = 4;
    byte fix = FIX;
    byte fixmant = FIXMANT;
    if (isshow) { // Set local variables for "SHOW print" if SHOW was demanded
      dot = 0;
      mantdigits = 7;
      fix = FIXSHOW;
      fixmant = FIXMANTSHOW;
      strbuf[0] = '<';
    }
    if (f < 0.0) {
      f = - f;
      if (!isshow) strbuf[0] = ':';
    }
    e = log10(f); // Exponent
    m = (f / _pow10(e - fix)) + .5; // Mantissa
    if (m > 0 && m < _pow10(fixmant)) { // Change (n-1)-digit-mantissa to n digits
      e -= 1;
      m = (f / _pow10(e - fix)) + .5; // "New" mantissa
    }
    for (byte i = mantdigits; i > 0; i--) { // Print mantissa
      strbuf[i] = _ones(m) + '0';
      m /= 10;
    }
    if (!isshow) { // Rest of "normal print"
      if (e < 0) { // Negative exponent
        strbuf[5] = ':';
        e = -e;
      }
      if (e < 10) { // Print exponent
        strbuf[6] = '0';
        strbuf[7] = e + '0';
      }
      else {
        strbuf[6] = _tens(e) + '0';
        strbuf[7] = _ones(e) + '0';
      }
    }
    byte dotshift = isshow ? 0 : 8;
    printnumber2(0, strbuf[0] - '0');
    printnumber2(15 + (isshow ? 8 : 0), strbuf[1] - '0');
    if (!isshow) printdot(31);
    for (byte i = 2; i < 8; i++) printnumber2(15 * i + 8, strbuf[i] - '0');
    if (isterm) Serial.println();
  }

  else if (mode == CLKMODE) printclock();

  else if (mode == MSGMODE) printmsg(msg);
}

void printdata() { // Prints data in runmode
  strbuf[0] = NULL;
  strcat(strbuf, "___");
  if (bytestack[0] > 99) strbuf[0] = _huns(bytestack[0]) + '0';
  if (bytestack[0] > 9) strbuf[1] = _tens(bytestack[0]) + '0';
  strbuf[2] = _ones(bytestack[0]) + '0';
  oledclr();
  printL();
}

void printclock() { // Prints digital time
  strbuf[0] = NULL;
  strcat(strbuf, " 0:00");
  unsigned long tmp = clockmillis / 36000000;
  if (tmp) strbuf[0] = tmp + '0';
  strbuf[1] = clockmillis / 3600000 % 10 + '0';
  tmp = clockmillis / 60000;
  strbuf[3] = (tmp - tmp / 60 * 60) / 10 + '0';
  strbuf[4] = (tmp - tmp / 60 * 60) % 10 + '0';
  printL(); // Print time
}

void setclock() { // Set time - hours (Y) and minutes (X)
  if (bytestack[1] < 24) clockmillis = bytestack[1] * 3600000;
  if (bytestack[0] < 59) clockmillis += bytestack[0] * 60000;
  pullbytestack();
  pullbytestack();
  starttime = millis();
}

static void pushbytestack(void) { // Push bytestack
  memmove(&bytestack[1], &bytestack[0], (BYTESTACKSIZE - 1)*sizeof(byte));
}

static void pullbytestack(void) { // Pull bytestack
  memmove(&bytestack[0], &bytestack[1], (BYTESTACKSIZE - 1)*sizeof(byte));
}

static void pullupperbytestack(void) { // Pull upper part of bytestack
  memmove(&bytestack[1], &bytestack[2], (BYTESTACKSIZE - 2)*sizeof(byte));
}

static void pushfloatstack(void) { // Push floatstack
  memmove(&floatstack[1], &floatstack[0], 3 * sizeof(double));
}

static void pullfloatstack(void) { // Pull floatstack
  memmove(&floatstack[0], &floatstack[1], (FLOATSTACKSIZE - 1) * sizeof(double));
}

static void pullupperfloatstack(void) { // Pull upper part of floatstack
  memmove(&floatstack[1], &floatstack[2], 2 * sizeof(double));
}

static int eofs(void) { // Returns EEPROM-address of end of file system
  int addr = EEADDRFSSTART;
  while (EEPROM.read(addr) != NULL) addr += EEPROM.read(addr);
  return (addr);
}

static double _pow10(int8_t e) { // Returns 10^e
  boolean ne = (e < 0);
  double f = 1.0;
  if (ne) while (e++)  f *= 10;
  else while (e--)  f *= 10;
  if (ne) f = 1 / f;
  return (f);
}

static double _exp_sin(double f, boolean isexp) { // Calculate exp or sin with Taylor series
  double result = f;
  double frac = f;
  if (isexp) result = frac = 1.0;
  for (byte i = 1; _abs(frac) > TINYNUMBER && i < MAXITERATE; i++) {
    if (isexp) frac *= f / i; // Fraction for exp
    else frac *=  f  / (-2 * i * (2 * i + 1)) * f; // Fraction for sin
    result += frac;
  }
  return (result);
}

static double _sqrt(double f) { // Calculating square root without using sqrt()
  return (_exp_sin(0.5 * log(f), true));
}

static void reset() {
  but.setHoldTime(200); // Time to switch from PRESS to HOLD

  oledinit(); // Init the OLED controller
  oledclr();

  for (byte i = 0; i < MAXBUT; i++) state[i] = 0;
  for (byte i = 0; i < BYTESTACKSIZE; i++) bytestack[i] = 0;
  for (byte i = 0; i < FLOATSTACKSIZE; i++) floatstack[i] = 0.0;
  mode = CMDMODE;
  byteselect = cmdselect = 0;
  isf = isg = isF = isG = isdisplayoff = isvBS = isbyteselect = false;
  for (byte i = 0; i < 12; i++) f[i] = EEPROM.read(EEADDRFKEYS + i);
  prg[0] = NULL;
  strcat(prg, ".ABC");
  prg[0] = prgptr = PRGPTRSTART;
  fsptr = EEADDRFSSTART;
  isnewnumber = true;
  ispushed = false;
  decimals = 0;
  isshow = false;
  isdot = false;
  isterm = false;
}


// SETUP & LOOP

void setup()   {
  reset();

  oledclr();
  byte si = 6;
  print(35, 0, si);
  print2(45, 0, si);
  print4(60, 0, si);
  printnumber(90, 1, 12);
  printnumber2(100, 8);
}


void loop() {
  boolean isbut; // Button state changes
  byte key = NULL; // Pressed (virtual) key

  if (millis() - starttime > PRINTINTERVAL * 1000) { // Show clock and correct time
    clockmillis = clockmillis + millis() - starttime + TIMECORRECTION;
    starttime = millis();
    if (mode == CLKMODE) printclock(); // Show clock
  }
  if (clockmillis >= 86400000) clockmillis = DAILYTIMECORRECTION; // Reset time after 24 hours

  if (isbut = but.getKeys()) { // *** Get state of buttons
    for (int i = 0; i < LIST_MAX; i++) { // Scan the whole key list
      if (but.key[i].stateChanged) { // Only find keys that have changed state
        state[but.key[i].kchar - '0'] = but.key[i].kstate; // Save state in state[]
      }
    }
  }

  if (state[0] && state[1] && state[4]) { // Reset
    reset();
    printscreen();
    key = NULL;
  }
  else if (state[0] && state[1]) { // Program break
    mode = CMDMODE;
    printscreen();
    key = NULL;
  }
  else if (isbut) { // *** Evaluate state of buttons
    if (state[0] == 1) { // Check f-key pressed
      if (isf) {
        if (!isdisplayoff) {
          oledoff();
          isdisplayoff = true;
          isf = false;
        }
      }
      else {
        isf = true;
        isg = false;
        if (isdisplayoff) {
          oledon();
          isf = false; // Do like f was not pressed before
          isdisplayoff = false;
        }
      }
    }
    if (state[1] == 1) { // Check g-key pressed
      if (isg) isg = false;
      else {
        isg = true;
        isf = false;
      }
    }
    if (state[0] == 2) { // Check f-key holding
      /*if (mode == CALCMODE) {
        oledclr();
        strbuf[0] = NULL;
        strcat(strbuf, " 7 8 9");
        printM(1);
        strbuf[0] = NULL;
        strcat(strbuf, " 4 5 6");
        printM(3);
        }*/
      isF = true;
      isf = false;
    }
    /*else if (state[0]==3){ oledclr(); printscreen();}*/
    else isF = false;
    if (state[1] == 2) { // Check g-key holding
      isG = true;
      isg = false;
    }
    else isG = false;
    key = NULL;
    for (byte i = 0; i < 6; i++) { // Check other keys and assign key
      if (state[i + 2] == 1) {
        if (isf) key = 'A' + i;
        else if (isg) key = 'G' + i;
        else if (isF) key = 'M' + i;
        else if (isG) key = 'S' + i;
        else key = '1' + i;
        isf = isg = false;
      }
    }
  }

  if (isterm && Serial.available()) { // Read and convert key from terminal
    //char from[] = "qweasd456rtyfghvbn789uiojklm,."; // US-keyboard
    char from[] = "qweasd456rtzfghvbn789uiojklm,."; // German keyboard
    char to[] =   "123456ABCDEFGHIJKLMNOPQRSTUVWX";
    byte k = Serial.read();
    for (byte i = 0; i < strlen(from); i++) if (k == from[i]) key = to[i];
  }

  if (mode == RUNMODE) { // *** Run program
    char tmpstr[] = ".....";
    (*dispatch[prg[prgptr] - CMDOFFSET])(); // Run selected command from function table
    /*Serial.print("prgptr="); Serial.print(prgptr);
      Serial.print(" prg[0]="); Serial.print(prg[0]);
      Serial.print(" prg="); Serial.print(prg[prgptr]);
      Serial.print(" cmd="); Serial.print(prg[prgptr] - CMDOFFSET);
      Serial.print(" mode="); Serial.println(mode);*/
    if (prg[prgptr] >= CMDOFFSET && prg[prgptr] <= CMDOFFSET + numberofcommands)
      strcpy_P(tmpstr, (char*)pgm_read_word(&(cmd[prg[prgptr] - CMDOFFSET])));

    if (strcmp(tmpstr, INPUTBYTE) == 0) {
      oldmode = RUNMODE;
      mode = INMODE;
      printscreen();
    }
    else if (strcmp(tmpstr, GOTO) == 0) {
      prgptr = prg[prgptr + 1] + PRGPTRSTART - 2;
    }
    if (prgptr < prg[0] - 1) prgptr++;
    else {
      oldmode = CMDMODE;
      mode = MSGMODE;
      msg = 1;
      printscreen();
    }
  }

  else if (key != NULL) { // *** Process key

    if (mode == CMDMODE) { // Select command
      if (key >= 'A' && key <= 'L') (*dispatch[f[key - 'A']])(); // Function keys for cmd
      if (key == ESC) {
        if (ismenu) { // Go back to calculator
          ismenu = false;
          mode = CALCMODE;
        }
      }
      else if (key == UP)
        if (cmdselect > CMDLINESTEP) cmdselect -= CMDLINESTEP;
        else cmdselect = 0;
      else if (key == OK) {
        (*dispatch[cmdselect])(); // Run selected command from function table
        if (ismenu) { // Go back to calculator
          isnewnumber=true;
          ismenu = false;
          mode = CALCMODE;
        }
      }
      else if (key == LEFT)
        if (cmdselect > 0) cmdselect--;
        else cmdselect = numberofcommands - 1;
      else if (key == DOWN)
        if (cmdselect < numberofcommands - CMDLINESTEP) cmdselect += CMDLINESTEP;
        else cmdselect = numberofcommands - 1;
      else if (key == RIGHT)
        if (cmdselect < numberofcommands - 1) cmdselect++;
        else cmdselect = 0;
    }

    else if (mode == INMODE) { // Select byte
      if (key == ESC) mode = oldmode; // ESC
      else if (key == UP)
        if (byteselect > LINESTEP) byteselect -= LINESTEP;
        else byteselect = 0;
      else if (key == OK) {
        pushbytestack();
        bytestack[0] = byteselect;
        inputbytes--;
        if (!inputbytes) mode = oldmode;
      }
      else if (key == LEFT)
        if (byteselect > 0) byteselect--;
        else byteselect = MAXBYTE;
      else if (key == DOWN)
        if (byteselect < MAXBYTE - LINESTEP) byteselect += LINESTEP;
        else byteselect = MAXBYTE;
      else if (key == RIGHT)
        if (byteselect < MAXBYTE) byteselect++;
        else byteselect = 0;
    }

    else if (mode == EDITMODE) { // Edit prg[]
      if (key == ESC) mode = CMDMODE; // ESC
      else if (key == UP) {
        if (prgptr > PRGPTRSTART) prgptr--;
      }
      else if (key == OK && prgptr < prg[0]) { // Edit step
        mode = EDITINMODE;
        byteselect = prg[prgptr];
      }
      else if (key == LEFT) { // Delete step
        for (byte i = PRGPTRSTART; i < prgptr; i++) { // Repair gotos
          char tmpstr[] = ".....";
          strcpy_P(tmpstr, (char*)pgm_read_word(&(cmd[prg[prgptr] - CMDOFFSET])));
          if (strcmp(tmpstr, GOTO) == 0 && prg[prgptr + 1] > prgptr) prg[prgptr + 1]--;
        }
        if (prgptr < prg[0]) {
          for (byte i = prgptr; i < prg[0] - 1; i++) prg[i] = prg[i + 1];
          prg[0]--;
        }
      }
      else if (key == DOWN) {
        if (prgptr < prg[0]) prgptr++;
      }
      else if (key == RIGHT) { // Insert step
        for (byte i = PRGPTRSTART; i < prgptr; i++) { // Repair gotos
          char tmpstr[] = ".....";
          strcpy_P(tmpstr, (char*)pgm_read_word(&(cmd[prg[prgptr] - CMDOFFSET])));
          if (strcmp(tmpstr, GOTO) == 0 && prg[prgptr + 1] > prgptr) prg[prgptr + 1]++;
        }
        if (prgptr < prg[0]) { // Insert
          for (byte i = prg[0]; i > prgptr; i--) prg[i] = prg[i - 1];
          mode = EDITINMODE;
          isinsertbyte = true;
        }
        else { // Append
          mode = EDITINMODE;
          isinsertbyte = true;
        }
      }
    }

    else if (mode == EDITINMODE) {
      if (key == ESC) {
        mode = EDITMODE; // ESC
        //for (byte i = prgptr; i < prg[0] - 1; i++) prg[i] = prg[i + 1];
        memmove(&prg[prgptr], &prg[prgptr + 1], (prg[0] - 2)*sizeof(byte));
        prg[0]--;
      }
      else if (key == UP)
        if (byteselect > LINESTEP) byteselect -= LINESTEP;
        else byteselect = 0;
      else if (key == OK) {
        prg[prgptr] = byteselect;
        if (isinsertbyte) {
          isinsertbyte = false;
          prgptr++;
          prg[0]++;
        }
        mode = EDITMODE;
      }
      else if (key == LEFT)
        if (byteselect > 0) byteselect--;
        else byteselect = MAXBYTE;
      else if (key == DOWN)
        if (byteselect < MAXBYTE - LINESTEP) byteselect += LINESTEP;
        else byteselect = MAXBYTE;
      else if (key == RIGHT)
        if (byteselect < MAXBYTE) byteselect++;
        else byteselect = 0;
    }

    else if (mode == FSMODE) { // Manipulate file system
      if (key == ESC) mode = CMDMODE; // ESC
      else if (key == UP) {
        int tmp = EEADDRFSSTART;
        while (tmp + EEPROM.read(tmp) < fsptr && EEPROM.read(tmp) != NULL)
          tmp += EEPROM.read(tmp);
        fsptr = tmp;
      }
      else if (key == OK) { // Load file to prg[]
        for (byte i = 0; i < EEPROM.read(fsptr); i++) prg[i] = EEPROM.read(fsptr + i);
      }
      else if (key == LEFT) { // Delete file
        int tmp = EEPROM.read(fsptr);
        for (int i = fsptr; i <= eofs() - EEPROM.read(fsptr); i++)
          EEPROM.write(i, EEPROM.read(i + tmp));
      }
      else if (key == DOWN) {
        if (EEPROM.read(fsptr) != 0) fsptr += EEPROM.read(fsptr);
      }
      else if (key == RIGHT) { // Save prg[] to EEPROM
        int tmp = eofs();
        if (EEADDRFSEND - tmp > prg[0]) { // Enough memory
          for (int i = tmp + prg[0]; i >= fsptr + prg[0]; i--)
            EEPROM.write(i, EEPROM.read(i - prg[0]));
          for (byte i = 0; i < prg[0]; i++) EEPROM.write(fsptr + i, prg[i]);
        }
        else { // No memory
          oldmode = FSMODE;
          mode = MSGMODE;
          msg = 2;
        }
      }
    }

    else if (mode == MSGMODE) mode = oldmode;

    else if (mode == STOPMODE) ;//mode = oldmode;

    else if (mode == CALCMODE) { // Calculator mode
      if (key >= 'M' && key <= 'V') { // * Fast keys
        const byte k2c[] = {'7', '8', '9', '4', '5', '6', '1', '2', '3', '0', '.', ':'};
        key = k2c[key - 'M']; // Transform M...V to 7...CHS
        if (isnewnumber) { // New number
          if (ispushed) ispushed = false;
          else pushfloatstack();
          floatstack[0] = 0.0;
          decimals = 0;
          isdot = false;
          isnewnumber = false;
        }
        if (isdot) floatstack[0] += (key - '0') / _pow10(++decimals); // Append decimal to number
        else floatstack[0] = floatstack[0] * 10 + key - '0'; // Append digit to number
      }
      else if (key == 'W') { // * Dot
        if (isnewnumber) { // Enable starting new number with .
          if (ispushed) ispushed = false;
          else pushfloatstack();
          floatstack[0] = 0.0;
          decimals = 0;
          isnewnumber = false;
        }
        isdot = true;
      }
      else if (key == '1') { // * CLX or CE
        if (isnewnumber) floatstack[0] = 0.0; // CLX
        else { // CE
          if (isdot) {
            if (floatstack[0] > TINYNUMBER && decimals > 0)
              floatstack[0] -= ((long)(floatstack[0] * _pow10(decimals)) % 10) / _pow10(decimals--);
            else isdot = false;
          }
          else floatstack[0] = (long)(floatstack[0] / 10.0);
        }
      }
      else if (key == '5') isshow = isshow ? false : true; // SHOW

      else { // * Other keys - finally setting isnewnumber true
        if (key == 'X') Fchs(); // CHS
        else if (key == '3') { // ENTER
          pushfloatstack();
          ispushed = true;
        }
        else if (key == '2') { // MENU
          ismenu = ismenu ? false : true;
          mode = CMDMODE;
        }
        else if (key == '4') mode = CMDMODE; // OFF
        else if (key == '6') Fsum(); // SUM+
        else if (key == 'C') Fdiv();  // /
        else if (key == 'F') Fmult(); // *
        else if (key == 'I') Fsub();  // -
        else if (key == 'J') Fswap(); // SWAP
        else if (key == 'K') Fee();   // EE resp. Y*10^X
        else if (key == 'L') Fadd();  // +

        isnewnumber = true;
      }
    }

    else if (mode == CLKMODE) mode = CMDMODE; // Show clock

    else if (mode == KBDMODE) { // Keyboard mode
      if (key == 'A') Keyboard.write(KEY_F3);
      else if (key == 'B') Keyboard.write(KEY_F4);
      else if (key == 'C') Keyboard.write(KEY_F5);
      else if (key == 'D') Keyboard.write(KEY_F6);
      else if (key == 'E') Keyboard.write(KEY_F7);
      else if (key == 'F') Keyboard.write(KEY_F8);
      else if (key == 'G') Keyboard.press(KEY_RIGHT_SHIFT);
      else if (key == 'H') Keyboard.press(KEY_RIGHT_CTRL);
      else if (key == 'I') Keyboard.press(KEY_RIGHT_ALT);
      else if (key == 'J') Keyboard.press(KEY_LEFT_SHIFT);
      else if (key == 'K') Keyboard.press(KEY_LEFT_CTRL);
      else if (key == 'L') Keyboard.press(KEY_LEFT_ALT);
      else if (key == 'M') Keyboard.write(KEY_TAB);
      else if (key == 'N') Keyboard.write(KEY_PAGE_UP);
      else if (key == 'O') Keyboard.write(KEY_BACKSPACE);
      else if (key == 'P') Keyboard.write(KEY_HOME);
      else if (key == 'Q') Keyboard.write(KEY_PAGE_DOWN);
      else if (key == 'R') Keyboard.write(KEY_END);
      else if (key == 'S') Keyboard.write(KEY_ESC);
      else if (key == 'T') Keyboard.write(KEY_UP_ARROW);
      else if (key == 'U') Keyboard.write(KEY_RETURN);
      else if (key == 'V') Keyboard.write(KEY_LEFT_ARROW);
      else if (key == 'W') Keyboard.write(KEY_DOWN_ARROW);
      else if (key == 'X') Keyboard.write(KEY_RIGHT_ARROW);
      else if (key == ESC) mode = CMDMODE; // ESC
      else if (key == UP)
        if (byteselect > LINESTEP) byteselect -= LINESTEP;
        else byteselect = 0;
      else if (key == OK) {
        //Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('a'); delay(200); Keyboard.releaseAll();
        //Keyboard.write('q'); Keyboard.write(KEY_RETURN);
        //Keyboard.print("exit"); Keyboard.write(KEY_RETURN);
        //Keyboard.write(bytestack[0]);
        //pullbytestack();
        Keyboard.write(byteselect);
        delay(200);
        Keyboard.releaseAll();
      }
      else if (key == LEFT)
        if (byteselect > 0) byteselect--;
        else byteselect = MAXBYTE;
      else if (key == DOWN)
        if (byteselect < MAXBYTE - LINESTEP) byteselect += LINESTEP;
        else byteselect = MAXBYTE;
      else if (key == RIGHT)
        if (byteselect < MAXBYTE) byteselect++;
        else byteselect = 0;
    }

    /*Serial.print("prgptr=");
    Serial.print(prgptr);
    Serial.print(" prg=");
    for (byte i=0; i < min(10,prg[0]); i++) {
      Serial.print(prg[i]); Serial.print(" ");
    }
    Serial.println();
    /*Serial.print("BS=");
    for (byte i=0; i < BYTESTACKSIZE; i++) {
      Serial.print(bytestack[i]); Serial.print(" ");
    }
    Serial.println();
    /*Serial.print("ptr="); Serial.println(prgptr);
      Serial.print("len="); Serial.println(strlen(prg));
      Serial.print("PRG=");
      for (byte i; i < 10; i++) {
      Serial.print(prg[i]); Serial.print(" ");
      }
      Serial.println();*/
    //Serial.print(floatstack[0]); Serial.print(" "); Serial.print(floatstack[1]); Serial.print(" "); Serial.println(floatstack[2]);

    printscreen();
  }

}




