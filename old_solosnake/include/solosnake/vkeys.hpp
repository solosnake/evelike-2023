#ifndef solosnake_vkeys_hpp
#define solosnake_vkeys_hpp

/**
* If windows has been included, it will have defined VK_LBUTTON, so
* defer to that.
*/
#ifndef VK_LBUTTON

/*
 * Virtual Keys, Standard Set
 */
#define VK_LBUTTON  (0x01u) 
#define VK_RBUTTON  (0x02u) 
#define VK_CANCEL   (0x03u) 
#define VK_MBUTTON  (0x04u) /* NOT contiguous with L & RBUTTON */
#define VK_XBUTTON1 (0x05u) /* NOT contiguous with L & RBUTTON */
#define VK_XBUTTON2 (0x06u) /* NOT contiguous with L & RBUTTON */

/*
 * 0x07 : unassigned
 */

#define VK_BACK     (0x08u)
#define VK_TAB      (0x09u)

/*
 * 0x0A - 0x0B : reserved
 */

#define VK_CLEAR    (0x0Cu)
#define VK_RETURN   (0x0Du)
#define VK_SHIFT    (0x10u)
#define VK_CONTROL  (0x11u)
#define VK_MENU     (0x12u)
#define VK_PAUSE    (0x13u)
#define VK_CAPITAL  (0x14u)
#define VK_KANA     (0x15u)
#define VK_HANGEUL  (0x15u) /* old name - should be here for compatibility */
#define VK_HANGUL   (0x15u)
#define VK_JUNJA    (0x17u)
#define VK_FINAL    (0x18u)
#define VK_HANJA    (0x19u)
#define VK_KANJI    (0x19u)
#define VK_ESCAPE   (0x1Bu)

#define VK_CONVERT      (0x1Cu)
#define VK_NONCONVERT   (0x1Du)
#define VK_ACCEPT       (0x1Eu)
#define VK_MODECHANGE   (0x1Fu)

#define VK_SPACE    (0x20u)
#define VK_PRIOR    (0x21u)
#define VK_NEXT     (0x22u)
#define VK_END      (0x23u)
#define VK_HOME     (0x24u)
#define VK_LEFT     (0x25u)
#define VK_UP       (0x26u)
#define VK_RIGHT    (0x27u)
#define VK_DOWN     (0x28u)
#define VK_SELECT   (0x29u)
#define VK_PRINT    (0x2Au)
#define VK_EXECUTE  (0x2Bu)
#define VK_SNAPSHOT (0x2Cu)
#define VK_INSERT   (0x2Du)
#define VK_DELETE   (0x2Eu)
#define VK_HELP     (0x2Fu)

/*
 * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
 * 0x40 : unassigned
 * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
 */

#define VK_LWIN         (0x5Bu)
#define VK_RWIN         (0x5Cu)
#define VK_APPS         (0x5Du)

/*
 * 0x5E : reserved
 */

#define VK_SLEEP        (0x5Fu)
#define VK_NUMPAD0      (0x60u)
#define VK_NUMPAD1      (0x61u)
#define VK_NUMPAD2      (0x62u)
#define VK_NUMPAD3      (0x63u)
#define VK_NUMPAD4      (0x64u)
#define VK_NUMPAD5      (0x65u)
#define VK_NUMPAD6      (0x66u)
#define VK_NUMPAD7      (0x67u)
#define VK_NUMPAD8      (0x68u)
#define VK_NUMPAD9      (0x69u)
#define VK_MULTIPLY     (0x6Au)
#define VK_ADD          (0x6Bu)
#define VK_SEPARATOR    (0x6Cu)
#define VK_SUBTRACT     (0x6Du)
#define VK_DECIMAL      (0x6Eu)
#define VK_DIVIDE       (0x6Fu)
#define VK_F1           (0x70u)
#define VK_F2           (0x71u)
#define VK_F3           (0x72u)
#define VK_F4           (0x73u)
#define VK_F5           (0x74u)
#define VK_F6           (0x75u)
#define VK_F7           (0x76u)
#define VK_F8           (0x77u)
#define VK_F9           (0x78u)
#define VK_F10          (0x79u)
#define VK_F11          (0x7Au)
#define VK_F12          (0x7Bu)
#define VK_F13          (0x7Cu)
#define VK_F14          (0x7Du)
#define VK_F15          (0x7Eu)
#define VK_F16          (0x7Fu)
#define VK_F17          (0x80u)
#define VK_F18          (0x81u)
#define VK_F19          (0x82u)
#define VK_F20          (0x83u)
#define VK_F21          (0x84u)
#define VK_F22          (0x85u)
#define VK_F23          (0x86u)
#define VK_F24          (0x87u)

/*
 * 0x88 - 0x8F : unassigned
 */

#define VK_NUMLOCK      (0x90u)
#define VK_SCROLL       (0x91u)

/*
 * NEC PC-9800 kbd definitions
 */
#define VK_OEM_NEC_EQUAL 0x92 // '=' key on numpad

/*
 * Fujitsu/OASYS kbd definitions
 */
#define VK_OEM_FJ_JISHO 0x92   // 'Dictionary' key
#define VK_OEM_FJ_MASSHOU 0x93 // 'Unregister word' key
#define VK_OEM_FJ_TOUROKU 0x94 // 'Register word' key
#define VK_OEM_FJ_LOYA 0x95    // 'Left OYAYUBI' key
#define VK_OEM_FJ_ROYA 0x96    // 'Right OYAYUBI' key

/*
 * 0x97 - 0x9F : unassigned
 */

/*
 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
 * No other API or message will distinguish left and right keys in this way.
 */
#define VK_LSHIFT 0xA0
#define VK_RSHIFT 0xA1
#define VK_LCONTROL 0xA2
#define VK_RCONTROL 0xA3
#define VK_LMENU 0xA4
#define VK_RMENU 0xA5

#define VK_BROWSER_BACK 0xA6
#define VK_BROWSER_FORWARD 0xA7
#define VK_BROWSER_REFRESH 0xA8
#define VK_BROWSER_STOP 0xA9
#define VK_BROWSER_SEARCH 0xAA
#define VK_BROWSER_FAVORITES 0xAB
#define VK_BROWSER_HOME 0xAC

#define VK_VOLUME_MUTE 0xAD
#define VK_VOLUME_DOWN 0xAE
#define VK_VOLUME_UP 0xAF
#define VK_MEDIA_NEXT_TRACK 0xB0
#define VK_MEDIA_PREV_TRACK 0xB1
#define VK_MEDIA_STOP 0xB2
#define VK_MEDIA_PLAY_PAUSE 0xB3
#define VK_LAUNCH_MAIL 0xB4
#define VK_LAUNCH_MEDIA_SELECT 0xB5
#define VK_LAUNCH_APP1 0xB6
#define VK_LAUNCH_APP2 0xB7

/*
 * 0xB8 - 0xB9 : reserved
 */

#define VK_OEM_1 0xBA      // ';:' for US
#define VK_OEM_PLUS 0xBB   // '+' any country
#define VK_OEM_COMMA 0xBC  // ',' any country
#define VK_OEM_MINUS 0xBD  // '-' any country
#define VK_OEM_PERIOD 0xBE // '.' any country
#define VK_OEM_2 0xBF      // '/?' for US
#define VK_OEM_3 0xC0      // '`~' for US

/*
 * 0xC1 - 0xD7 : reserved
 */

/*
 * 0xD8 - 0xDA : unassigned
 */

#define VK_OEM_4 0xDB //  '[{' for US
#define VK_OEM_5 0xDC //  '\|' for US
#define VK_OEM_6 0xDD //  ']}' for US
#define VK_OEM_7 0xDE //  ''"' for US
#define VK_OEM_8 0xDF

/*
 * 0xE0 : reserved
 */

/*
 * Various extended or enhanced keyboards
 */
#define VK_OEM_AX 0xE1   //  'AX' key on Japanese AX kbd
#define VK_OEM_102 0xE2  //  "<>" or "\|" on RT 102-key kbd.
#define VK_ICO_HELP 0xE3 //  Help key on ICO
#define VK_ICO_00 0xE4   //  00 key on ICO

#define VK_PROCESSKEY 0xE5

#define VK_ICO_CLEAR 0xE6

#define VK_PACKET 0xE7

/*
 * 0xE8 : unassigned
 */

/*
 * Nokia/Ericsson definitions
 */
#define VK_OEM_RESET 0xE9
#define VK_OEM_JUMP 0xEA
#define VK_OEM_PA1 0xEB
#define VK_OEM_PA2 0xEC
#define VK_OEM_PA3 0xED
#define VK_OEM_WSCTRL 0xEE
#define VK_OEM_CUSEL 0xEF
#define VK_OEM_ATTN 0xF0
#define VK_OEM_FINISH 0xF1
#define VK_OEM_COPY 0xF2
#define VK_OEM_AUTO 0xF3
#define VK_OEM_ENLW 0xF4
#define VK_OEM_BACKTAB 0xF5

#define VK_ATTN 0xF6
#define VK_CRSEL 0xF7
#define VK_EXSEL 0xF8
#define VK_EREOF 0xF9
#define VK_PLAY 0xFA
#define VK_ZOOM 0xFB
#define VK_NONAME 0xFC
#define VK_PA1 0xFD
#define VK_OEM_CLEAR 0xFE

#endif

#endif
