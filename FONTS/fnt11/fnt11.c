#include "includes.h"
#include "lcd_ui_fonts.h"


const uint8_t fnt11_data[] = {
			// "SP-32" width 5
			// .....
			// .....
			// .....
			// .....
			// .....
			// .....
			// .....
			// .....
			// .....
			// .....
			// .....
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,

			// "!-33" width 2
			// ..
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// ..
			// 0.
			// ..
			// ..
		0x54,0x15,0x01,

			// ""-34" width 5
			// 0.0..
			// 0.0..
			// .0.0.
			// .....
			// .....
			// .....
			// .....
			// .....
			// .....
			// .....
			// .....
		0xA5,0x28,0x00,0x00,0x00,0x00,0x00,

			// "#-35" width 8
			// ........
			// ...0.0..
			// ...0.0..
			// .000000.
			// ..0.0...
			// ..0.0...
			// 000000..
			// .0.0....
			// .0.0....
			// ........
			// ........
		0x00,0x28,0x28,0x7E,0x14,0x14,0x3F,0x0A,0x0A,0x00,0x00,

			// "$-36" width 6
			// ..0...
			// ..0...
			// .0000.
			// 0.0...
			// 0.0...
			// .000..
			// ..0.0.
			// ..0.0.
			// 0000..
			// ..0...
			// ..0...
		0x04,0xE1,0x15,0x85,0x43,0x51,0x0F,0x41,0x00,

			// "%-37" width 11
			// ...........
			// .00...0....
			// 0..0..0....
			// 0..0.0.....
			// .00..0.....
			// ....0..00..
			// ....0.0..0.
			// ...0..0..0.
			// ...0...00..
			// ...........
			// ...........
		0x00,0x30,0x42,0x12,0x52,0x60,0x02,0xC8,0x40,0x09,0x49,0x88,
		0x01,0x00,0x00,0x00,

			// "&-38" width 8
			// ........
			// .00.....
			// 0..0....
			// 0..0....
			// .00..0..
			// 0..0.0..
			// 0...0...
			// 0...00..
			// .000..0.
			// ........
			// ........
		0x00,0x06,0x09,0x09,0x26,0x29,0x11,0x31,0x4E,0x00,0x00,

			// "'-39" width 3
			// .0.
			// .0.
			// 0..
			// ...
			// ...
			// ...
			// ...
			// ...
			// ...
			// ...
			// ...
		0x52,0x00,0x00,0x00,0x00,

			// "(-40" width 4
			// ..0.
			// .0..
			// .0..
			// 0...
			// 0...
			// 0...
			// 0...
			// 0...
			// .0..
			// .0..
			// ..0.
		0x24,0x12,0x11,0x11,0x22,0x04,

			// ")-41" width 4
			// 0...
			// .0..
			// .0..
			// ..0.
			// ..0.
			// ..0.
			// ..0.
			// ..0.
			// .0..
			// .0..
			// 0...
		0x21,0x42,0x44,0x44,0x22,0x01,

			// "*-42" width 6
			// ..0...
			// 0.0.0.
			// .000..
			// 0.0.0.
			// ..0...
			// ......
			// ......
			// ......
			// ......
			// ......
			// ......
		0x44,0xE5,0x54,0x04,0x00,0x00,0x00,0x00,0x00,

			// "+-43" width 8
			// ........
			// ........
			// ...0....
			// ...0....
			// ...0....
			// 0000000.
			// ...0....
			// ...0....
			// ...0....
			// ........
			// ........
		0x00,0x00,0x08,0x08,0x08,0x7F,0x08,0x08,0x08,0x00,0x00,

			// ",-44" width 3
			// ...
			// ...
			// ...
			// ...
			// ...
			// ...
			// ...
			// .0.
			// .0.
			// .0.
			// 0..
		0x00,0x00,0x40,0x52,0x00,

			// "--45" width 4
			// ....
			// ....
			// ....
			// ....
			// ....
			// 000.
			// ....
			// ....
			// ....
			// ....
			// ....
		0x00,0x00,0x70,0x00,0x00,0x00,

			// ".-46" width 2
			// ..
			// ..
			// ..
			// ..
			// ..
			// ..
			// ..
			// 0.
			// 0.
			// ..
			// ..
		0x00,0x40,0x01,

			// "/-47" width 4
			// ..0.
			// ..0.
			// ..0.
			// .0..
			// .0..
			// .0..
			// .0..
			// .0..
			// 0...
			// 0...
			// 0...
		0x44,0x24,0x22,0x22,0x11,0x01,

			// "0-48" width 6
			// ......
			// .000..
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// .000..
			// ......
			// ......
		0x80,0x13,0x45,0x51,0x14,0x45,0x0E,0x00,0x00,

			// "1-49" width 4
			// ....
			// .0..
			// 00..
			// .0..
			// .0..
			// .0..
			// .0..
			// .0..
			// 000.
			// ....
			// ....
		0x20,0x23,0x22,0x22,0x07,0x00,

			// "2-50" width 6
			// ......
			// .000..
			// 0...0.
			// ....0.
			// ...0..
			// ..0...
			// .0....
			// 0.....
			// 00000.
			// ......
			// ......
		0x80,0x13,0x41,0x08,0x21,0x04,0x1F,0x00,0x00,

			// "3-51" width 6
			// ......
			// .000..
			// 0...0.
			// ....0.
			// ..00..
			// ....0.
			// ....0.
			// 0...0.
			// .000..
			// ......
			// ......
		0x80,0x13,0x41,0x0C,0x04,0x45,0x0E,0x00,0x00,

			// "4-52" width 6
			// ......
			// ...0..
			// ..00..
			// .0.0..
			// 0..0..
			// 00000.
			// ...0..
			// ...0..
			// ...0..
			// ......
			// ......
		0x00,0xC2,0x28,0xC9,0x87,0x20,0x08,0x00,0x00,

			// "5-53" width 6
			// ......
			// 00000.
			// 0.....
			// 0.....
			// 0000..
			// ....0.
			// ....0.
			// 0...0.
			// .000..
			// ......
			// ......
		0xC0,0x17,0x04,0x0F,0x04,0x45,0x0E,0x00,0x00,

			// "6-54" width 6
			// ......
			// ..00..
			// .0....
			// 0.....
			// 0000..
			// 0...0.
			// 0...0.
			// 0...0.
			// .000..
			// ......
			// ......
		0x00,0x23,0x04,0x4F,0x14,0x45,0x0E,0x00,0x00,

			// "7-55" width 6
			// ......
			// 00000.
			// ....0.
			// ...0..
			// ...0..
			// ..0...
			// ..0...
			// .0....
			// .0....
			// ......
			// ......
		0xC0,0x07,0x21,0x08,0x41,0x08,0x02,0x00,0x00,

			// "8-56" width 6
			// ......
			// .000..
			// 0...0.
			// 0...0.
			// .000..
			// 0...0.
			// 0...0.
			// 0...0.
			// .000..
			// ......
			// ......
		0x80,0x13,0x45,0x4E,0x14,0x45,0x0E,0x00,0x00,

			// "9-57" width 6
			// ......
			// .000..
			// 0...0.
			// 0...0.
			// 0...0.
			// .0000.
			// ....0.
			// ...0..
			// .00...
			// ......
			// ......
		0x80,0x13,0x45,0x91,0x07,0x21,0x06,0x00,0x00,

			// ":-58" width 2
			// ..
			// ..
			// ..
			// 0.
			// 0.
			// ..
			// ..
			// 0.
			// 0.
			// ..
			// ..
		0x40,0x41,0x01,

			// ";-59" width 3
			// ...
			// ...
			// ...
			// .0.
			// .0.
			// ...
			// ...
			// .0.
			// .0.
			// .0.
			// 0..
		0x00,0x24,0x40,0x52,0x00,

			// "<-60" width 7
			// .......
			// .......
			// .....0.
			// ...00..
			// .00....
			// 0......
			// .00....
			// ...00..
			// .....0.
			// .......
			// .......
		0x00,0x00,0x08,0x63,0x08,0x18,0x30,0x20,0x00,0x00,

			// "=-61" width 8
			// ........
			// ........
			// ........
			// ........
			// 0000000.
			// ........
			// 0000000.
			// ........
			// ........
			// ........
			// ........
		0x00,0x00,0x00,0x00,0x7F,0x00,0x7F,0x00,0x00,0x00,0x00,

			// ">-62" width 7
			// .......
			// .......
			// 0......
			// .00....
			// ...00..
			// .....0.
			// ...00..
			// .00....
			// 0......
			// .......
			// .......
		0x00,0x40,0xC0,0x80,0x01,0x61,0x0C,0x01,0x00,0x00,

			// "?-63" width 5
			// .....
			// 000..
			// ...0.
			// ...0.
			// ..0..
			// .0...
			// .0...
			// .....
			// .0...
			// .....
			// .....
		0xE0,0x20,0x44,0x84,0x00,0x02,0x00,

			// "@-64" width 10
			// ..........
			// ..00000...
			// .0.....0..
			// 0..000..0.
			// 0.0..0..0.
			// 0.0..0..0.
			// 0.0..0..0.
			// 0..00000..
			// .0........
			// ..0000....
			// ..........
		0x00,0xF0,0x21,0x48,0x4E,0x25,0x95,0x54,0x52,0x3E,0x02,0xF0,
		0x00,0x00,

			// "A-65" width 7
			// .......
			// ..00...
			// ..00...
			// .0..0..
			// .0..0..
			// .0..0..
			// 000000.
			// 0....0.
			// 0....0.
			// .......
			// .......
		0x00,0x06,0x43,0x22,0x91,0xFC,0x42,0x21,0x00,0x00,

			// "B-66" width 6
			// ......
			// 0000..
			// 0...0.
			// 0...0.
			// 0000..
			// 0...0.
			// 0...0.
			// 0...0.
			// 0000..
			// ......
			// ......
		0xC0,0x13,0x45,0x4F,0x14,0x45,0x0F,0x00,0x00,

			// "C-67" width 7
			// .......
			// ..0000.
			// .0.....
			// 0......
			// 0......
			// 0......
			// 0......
			// .0.....
			// ..0000.
			// .......
			// .......
		0x00,0x9E,0x20,0x10,0x08,0x04,0x04,0x3C,0x00,0x00,

			// "D-68" width 7
			// .......
			// 0000...
			// 0...0..
			// 0....0.
			// 0....0.
			// 0....0.
			// 0....0.
			// 0...0..
			// 0000...
			// .......
			// .......
		0x80,0x47,0x24,0x14,0x0A,0x85,0x22,0x0F,0x00,0x00,

			// "E-69" width 6
			// ......
			// 00000.
			// 0.....
			// 0.....
			// 0000..
			// 0.....
			// 0.....
			// 0.....
			// 00000.
			// ......
			// ......
		0xC0,0x17,0x04,0x4F,0x10,0x04,0x1F,0x00,0x00,

			// "F-70" width 6
			// ......
			// 00000.
			// 0.....
			// 0.....
			// 00000.
			// 0.....
			// 0.....
			// 0.....
			// 0.....
			// ......
			// ......
		0xC0,0x17,0x04,0x5F,0x10,0x04,0x01,0x00,0x00,

			// "G-71" width 7
			// .......
			// ..0000.
			// .0.....
			// 0......
			// 0......
			// 0..000.
			// 0....0.
			// .0...0.
			// ..0000.
			// .......
			// .......
		0x00,0x9E,0x20,0x10,0xC8,0x85,0x44,0x3C,0x00,0x00,

			// "H-72" width 7
			// .......
			// 0....0.
			// 0....0.
			// 0....0.
			// 000000.
			// 0....0.
			// 0....0.
			// 0....0.
			// 0....0.
			// .......
			// .......
		0x80,0x50,0x28,0xF4,0x0B,0x85,0x42,0x21,0x00,0x00,

			// "I-73" width 4
			// ....
			// 000.
			// .0..
			// .0..
			// .0..
			// .0..
			// .0..
			// .0..
			// 000.
			// ....
			// ....
		0x70,0x22,0x22,0x22,0x07,0x00,

			// "J-74" width 5
			// .....
			// .000.
			// ...0.
			// ...0.
			// ...0.
			// ...0.
			// ...0.
			// ...0.
			// 000..
			// .....
			// .....
		0xC0,0x21,0x84,0x10,0x42,0x07,0x00,

			// "K-75" width 6
			// ......
			// 0...0.
			// 0..0..
			// 0.0...
			// 00....
			// 00....
			// 0.0...
			// 0..0..
			// 0...0.
			// ......
			// ......
		0x40,0x94,0x14,0xC3,0x50,0x24,0x11,0x00,0x00,

			// "L-76" width 5
			// .....
			// 0....
			// 0....
			// 0....
			// 0....
			// 0....
			// 0....
			// 0....
			// 0000.
			// .....
			// .....
		0x20,0x84,0x10,0x42,0x08,0x0F,0x00,

			// "M-77" width 8
			// ........
			// 00...00.
			// 00...00.
			// 0.0.0.0.
			// 0.0.0.0.
			// 0..0..0.
			// 0..0..0.
			// 0.....0.
			// 0.....0.
			// ........
			// ........
		0x00,0x63,0x63,0x55,0x55,0x49,0x49,0x41,0x41,0x00,0x00,

			// "N-78" width 7
			// .......
			// 00...0.
			// 00...0.
			// 0.0..0.
			// 0.0..0.
			// 0..0.0.
			// 0..0.0.
			// 0...00.
			// 0...00.
			// .......
			// .......
		0x80,0xD1,0xA8,0x54,0x4A,0xA5,0x62,0x31,0x00,0x00,

			// "O-79" width 8
			// ........
			// ..000...
			// .0...0..
			// 0.....0.
			// 0.....0.
			// 0.....0.
			// 0.....0.
			// .0...0..
			// ..000...
			// ........
			// ........
		0x00,0x1C,0x22,0x41,0x41,0x41,0x41,0x22,0x1C,0x00,0x00,

			// "P-80" width 6
			// ......
			// 0000..
			// 0...0.
			// 0...0.
			// 0...0.
			// 0000..
			// 0.....
			// 0.....
			// 0.....
			// ......
			// ......
		0xC0,0x13,0x45,0xD1,0x13,0x04,0x01,0x00,0x00,

			// "Q-81" width 8
			// ........
			// ..000...
			// .0...0..
			// 0.....0.
			// 0.....0.
			// 0.....0.
			// 0.....0.
			// .0...0..
			// ..000...
			// ....0...
			// .....00.
		0x00,0x1C,0x22,0x41,0x41,0x41,0x41,0x22,0x1C,0x10,0x60,

			// "R-82" width 7
			// .......
			// 0000...
			// 0...0..
			// 0...0..
			// 0...0..
			// 0000...
			// 0..0...
			// 0...0..
			// 0....0.
			// .......
			// .......
		0x80,0x47,0x24,0x12,0x79,0x24,0x22,0x21,0x00,0x00,

			// "S-83" width 6
			// ......
			// .0000.
			// 0.....
			// 0.....
			// .000..
			// ....0.
			// ....0.
			// ....0.
			// 0000..
			// ......
			// ......
		0x80,0x17,0x04,0x0E,0x04,0x41,0x0F,0x00,0x00,

			// "T-84" width 6
			// ......
			// 00000.
			// ..0...
			// ..0...
			// ..0...
			// ..0...
			// ..0...
			// ..0...
			// ..0...
			// ......
			// ......
		0xC0,0x47,0x10,0x04,0x41,0x10,0x04,0x00,0x00,

			// "U-85" width 7
			// .......
			// 0....0.
			// 0....0.
			// 0....0.
			// 0....0.
			// 0....0.
			// 0....0.
			// 0....0.
			// .0000..
			// .......
			// .......
		0x80,0x50,0x28,0x14,0x0A,0x85,0x42,0x1E,0x00,0x00,

			// "V-86" width 6
			// ......
			// 0...0.
			// 0...0.
			// 0...0.
			// .0.0..
			// .0.0..
			// .0.0..
			// ..0...
			// ..0...
			// ......
			// ......
		0x40,0x14,0x45,0x8A,0xA2,0x10,0x04,0x00,0x00,

			// "W-87" width 10
			// ..........
			// 0...0...0.
			// 0...0...0.
			// 0...0...0.
			// .0.0.0.0..
			// .0.0.0.0..
			// .0.0.0.0..
			// ..0...0...
			// ..0...0...
			// ..........
			// ..........
		0x00,0x44,0x14,0x51,0x44,0xAA,0xA8,0xA2,0x0A,0x11,0x44,0x00,
		0x00,0x00,

			// "X-88" width 6
			// ......
			// 0...0.
			// 0...0.
			// .0.0..
			// ..0...
			// ..0...
			// .0.0..
			// 0...0.
			// 0...0.
			// ......
			// ......
		0x40,0x14,0x29,0x04,0xA1,0x44,0x11,0x00,0x00,

			// "Y-89" width 6
			// ......
			// 0...0.
			// 0...0.
			// .0.0..
			// .0.0..
			// ..0...
			// ..0...
			// ..0...
			// ..0...
			// ......
			// ......
		0x40,0x14,0x29,0x0A,0x41,0x10,0x04,0x00,0x00,

			// "Z-90" width 6
			// ......
			// 00000.
			// ....0.
			// ...0..
			// ..0...
			// ..0...
			// .0....
			// 0.....
			// 00000.
			// ......
			// ......
		0xC0,0x07,0x21,0x04,0x21,0x04,0x1F,0x00,0x00,

			// "[-91" width 4
			// 000.
			// 0...
			// 0...
			// 0...
			// 0...
			// 0...
			// 0...
			// 0...
			// 0...
			// 0...
			// 000.
		0x17,0x11,0x11,0x11,0x11,0x07,

			// "\-92" width 4
			// 0...
			// 0...
			// 0...
			// .0..
			// .0..
			// .0..
			// .0..
			// .0..
			// ..0.
			// ..0.
			// ..0.
		0x11,0x21,0x22,0x22,0x44,0x04,

			// "]-93" width 4
			// 000.
			// ..0.
			// ..0.
			// ..0.
			// ..0.
			// ..0.
			// ..0.
			// ..0.
			// ..0.
			// ..0.
			// 000.
		0x47,0x44,0x44,0x44,0x44,0x07,

			// "^-94" width 8
			// ........
			// ...0....
			// ..0.0...
			// .0...0..
			// 0.....0.
			// ........
			// ........
			// ........
			// ........
			// ........
			// ........
		0x00,0x08,0x14,0x22,0x41,0x00,0x00,0x00,0x00,0x00,0x00,

			// "_-95" width 7
			// .......
			// .......
			// .......
			// .......
			// .......
			// .......
			// .......
			// .......
			// .......
			// .......
			// 000000.
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x0F,

			// "`-96" width 3
			// 0..
			// .0.
			// ...
			// ...
			// ...
			// ...
			// ...
			// ...
			// ...
			// ...
			// ...
		0x11,0x00,0x00,0x00,0x00,

			// "a-97" width 6
			// ......
			// ......
			// ......
			// .000..
			// ....0.
			// .0000.
			// 0...0.
			// 0...0.
			// .0000.
			// ......
			// ......
		0x00,0x00,0x38,0x90,0x17,0x45,0x1E,0x00,0x00,

			// "b-98" width 6
			// 0.....
			// 0.....
			// 0.....
			// 0000..
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// 0000..
			// ......
			// ......
		0x41,0x10,0x3C,0x51,0x14,0x45,0x0F,0x00,0x00,

			// "c-99" width 5
			// .....
			// .....
			// .....
			// .000.
			// 0....
			// 0....
			// 0....
			// 0....
			// .000.
			// .....
			// .....
		0x00,0x00,0x17,0x42,0x08,0x0E,0x00,

			// "d-100" width 6
			// ....0.
			// ....0.
			// ....0.
			// .0000.
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// .0000.
			// ......
			// ......
		0x10,0x04,0x79,0x51,0x14,0x45,0x1E,0x00,0x00,

			// "e-101" width 6
			// ......
			// ......
			// ......
			// .000..
			// 0...0.
			// 00000.
			// 0.....
			// 0...0.
			// .000..
			// ......
			// ......
		0x00,0x00,0x38,0xD1,0x17,0x44,0x0E,0x00,0x00,

			// "f-102" width 4
			// .00.
			// 0...
			// 0...
			// 000.
			// 0...
			// 0...
			// 0...
			// 0...
			// 0...
			// ....
			// ....
		0x16,0x71,0x11,0x11,0x01,0x00,

			// "g-103" width 6
			// ......
			// ......
			// ......
			// .0000.
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// .0000.
			// ....0.
			// .000..
		0x00,0x00,0x78,0x51,0x14,0x45,0x1E,0xE4,0x00,

			// "h-104" width 6
			// 0.....
			// 0.....
			// 0.....
			// 0000..
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// ......
			// ......
		0x41,0x10,0x3C,0x51,0x14,0x45,0x11,0x00,0x00,

			// "i-105" width 2
			// ..
			// 0.
			// ..
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// ..
			// ..
		0x44,0x55,0x01,

			// "j-106" width 3
			// ...
			// .0.
			// ...
			// 00.
			// .0.
			// .0.
			// .0.
			// .0.
			// .0.
			// .0.
			// 0..
		0x10,0x26,0x49,0x52,0x00,

			// "k-107" width 6
			// 0.....
			// 0.....
			// 0.....
			// 0..0..
			// 0.0...
			// 00....
			// 0.0...
			// 0..0..
			// 0...0.
			// ......
			// ......
		0x41,0x10,0x24,0xC5,0x50,0x24,0x11,0x00,0x00,

			// "l-108" width 2
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// ..
			// ..
		0x55,0x55,0x01,

			// "m-109" width 8
			// ........
			// ........
			// ........
			// 000.00..
			// 0..0..0.
			// 0..0..0.
			// 0..0..0.
			// 0..0..0.
			// 0..0..0.
			// ........
			// ........
		0x00,0x00,0x00,0x37,0x49,0x49,0x49,0x49,0x49,0x00,0x00,

			// "n-110" width 6
			// ......
			// ......
			// ......
			// 0000..
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// ......
			// ......
		0x00,0x00,0x3C,0x51,0x14,0x45,0x11,0x00,0x00,

			// "o-111" width 6
			// ......
			// ......
			// ......
			// .000..
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// .000..
			// ......
			// ......
		0x00,0x00,0x38,0x51,0x14,0x45,0x0E,0x00,0x00,

			// "p-112" width 6
			// ......
			// ......
			// ......
			// 0000..
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// 0000..
			// 0.....
			// 0.....
		0x00,0x00,0x3C,0x51,0x14,0x45,0x4F,0x10,0x00,

			// "q-113" width 6
			// ......
			// ......
			// ......
			// .0000.
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// .0000.
			// ....0.
			// ....0.
		0x00,0x00,0x78,0x51,0x14,0x45,0x1E,0x04,0x01,

			// "r-114" width 4
			// ....
			// ....
			// ....
			// 0.0.
			// 00..
			// 0...
			// 0...
			// 0...
			// 0...
			// ....
			// ....
		0x00,0x50,0x13,0x11,0x01,0x00,

			// "s-115" width 5
			// .....
			// .....
			// .....
			// .000.
			// 0....
			// 00...
			// ..00.
			// ...0.
			// 000..
			// .....
			// .....
		0x00,0x00,0x17,0x06,0x43,0x07,0x00,

			// "t-116" width 4
			// ....
			// 0...
			// 0...
			// 000.
			// 0...
			// 0...
			// 0...
			// 0...
			// .00.
			// ....
			// ....
		0x10,0x71,0x11,0x11,0x06,0x00,

			// "u-117" width 6
			// ......
			// ......
			// ......
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// .0000.
			// ......
			// ......
		0x00,0x00,0x44,0x51,0x14,0x45,0x1E,0x00,0x00,

			// "v-118" width 6
			// ......
			// ......
			// ......
			// 0...0.
			// 0...0.
			// .0.0..
			// .0.0..
			// ..0...
			// ..0...
			// ......
			// ......
		0x00,0x00,0x44,0x91,0xA2,0x10,0x04,0x00,0x00,

			// "w-119" width 8
			// ........
			// ........
			// ........
			// 0..0..0.
			// 0..0..0.
			// 0.0.0.0.
			// 0.0.0.0.
			// .0...0..
			// .0...0..
			// ........
			// ........
		0x00,0x00,0x00,0x49,0x49,0x55,0x55,0x22,0x22,0x00,0x00,

			// "x-120" width 6
			// ......
			// ......
			// ......
			// 0...0.
			// .0.0..
			// ..0...
			// ..0...
			// .0.0..
			// 0...0.
			// ......
			// ......
		0x00,0x00,0x44,0x0A,0x41,0x28,0x11,0x00,0x00,

			// "y-121" width 6
			// ......
			// ......
			// ......
			// 0...0.
			// 0...0.
			// .0.0..
			// .0.0..
			// ..0...
			// ..0...
			// .0....
			// .0....
		0x00,0x00,0x44,0x91,0xA2,0x10,0x84,0x20,0x00,

			// "z-122" width 5
			// .....
			// .....
			// .....
			// 0000.
			// ...0.
			// ..0..
			// .0...
			// 0....
			// 0000.
			// .....
			// .....
		0x00,0x80,0x87,0x88,0x08,0x0F,0x00,

			// "{-123" width 5
			// ...0.
			// ..0..
			// ..0..
			// ..0..
			// ..0..
			// 00...
			// ..0..
			// ..0..
			// ..0..
			// ..0..
			// ...0.
		0x88,0x10,0x42,0x06,0x21,0x84,0x20,

			// "|-124" width 2
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
			// 0.
		0x55,0x55,0x15,

			// "}-125" width 5
			// 0....
			// .0...
			// .0...
			// .0...
			// .0...
			// ..00.
			// .0...
			// .0...
			// .0...
			// .0...
			// 0....
		0x41,0x08,0x21,0x98,0x10,0x42,0x04,

			// "~-126" width 8
			// ........
			// ........
			// ........
			// ........
			// .00...0.
			// 0..0..0.
			// 0...00..
			// ........
			// ........
			// ........
			// ........
		0x00,0x00,0x00,0x00,0x46,0x49,0x31,0x00,0x00,0x00,0x00,

			// "�-192" width 7
			// .......
			// ..00...
			// ..00...
			// .0..0..
			// .0..0..
			// .0..0..
			// 000000.
			// 0....0.
			// 0....0.
			// .......
			// .......
		0x00,0x06,0x43,0x22,0x91,0xFC,0x42,0x21,0x00,0x00,

			// "�-193" width 6
			// ......
			// 00000.
			// 0.....
			// 0.....
			// 0000..
			// 0...0.
			// 0...0.
			// 0...0.
			// 0000..
			// ......
			// ......
		0xC0,0x17,0x04,0x4F,0x14,0x45,0x0F,0x00,0x00,

			// "�-194" width 6
			// ......
			// 0000..
			// 0...0.
			// 0...0.
			// 0000..
			// 0...0.
			// 0...0.
			// 0...0.
			// 0000..
			// ......
			// ......
		0xC0,0x13,0x45,0x4F,0x14,0x45,0x0F,0x00,0x00,

			// "�-195" width 6
			// ......
			// 00000.
			// 0.....
			// 0.....
			// 0.....
			// 0.....
			// 0.....
			// 0.....
			// 0.....
			// ......
			// ......
		0xC0,0x17,0x04,0x41,0x10,0x04,0x01,0x00,0x00,

			// "�-196" width 8
			// ........
			// ...000..
			// ...0.0..
			// ..0..0..
			// ..0..0..
			// ..0..0..
			// .0...0..
			// .0...0..
			// 0000000.
			// 0.....0.
			// 0.....0.
		0x00,0x38,0x28,0x24,0x24,0x24,0x22,0x22,0x7F,0x41,0x41,

			// "�-197" width 6
			// ......
			// 00000.
			// 0.....
			// 0.....
			// 0000..
			// 0.....
			// 0.....
			// 0.....
			// 00000.
			// ......
			// ......
		0xC0,0x17,0x04,0x4F,0x10,0x04,0x1F,0x00,0x00,

			// "�-198" width 10
			// ..........
			// 00..0..00.
			// .0..0..0..
			// ..0.0.0...
			// ..0.0.0...
			// ...000....
			// ..0.0.0...
			// .0..0..0..
			// 0...0...0.
			// ..........
			// ..........
		0x00,0x4C,0x26,0x09,0x15,0x54,0xE0,0x40,0x85,0x24,0x11,0x01,
		0x00,0x00,

			// "�-199" width 6
			// ......
			// 0000..
			// ....0.
			// ....0.
			// .000..
			// ....0.
			// ....0.
			// ....0.
			// 0000..
			// ......
			// ......
		0xC0,0x03,0x41,0x0E,0x04,0x41,0x0F,0x00,0x00,

			// "�-200" width 7
			// .......
			// 0....0.
			// 0...00.
			// 0..0.0.
			// 0..0.0.
			// 0.0..0.
			// 0.0..0.
			// 00...0.
			// 0....0.
			// .......
			// .......
		0x80,0x50,0x2C,0x95,0x2A,0x95,0x46,0x21,0x00,0x00,

			// "�-201" width 7
			// ..00...
			// 0....0.
			// 0...00.
			// 0..0.0.
			// 0..0.0.
			// 0.0..0.
			// 0.0..0.
			// 00...0.
			// 0....0.
			// .......
			// .......
		0x8C,0x50,0x2C,0x95,0x2A,0x95,0x46,0x21,0x00,0x00,

			// "�-202" width 7
			// .......
			// 0...00.
			// 0..0...
			// 0..0...
			// 000....
			// 0.0....
			// 0..0...
			// 0...0..
			// 0....0.
			// .......
			// .......
		0x80,0x58,0x22,0x71,0x28,0x24,0x22,0x21,0x00,0x00,

			// "�-203" width 7
			// .......
			// .00000.
			// .0...0.
			// .0...0.
			// .0...0.
			// .0...0.
			// .0...0.
			// .0...0.
			// 0....0.
			// .......
			// .......
		0x00,0x9F,0x48,0x24,0x12,0x89,0x44,0x21,0x00,0x00,

			// "�-204" width 8
			// ........
			// 00...00.
			// 00...00.
			// 0.0.0.0.
			// 0.0.0.0.
			// 0..0..0.
			// 0..0..0.
			// 0.....0.
			// 0.....0.
			// ........
			// ........
		0x00,0x63,0x63,0x55,0x55,0x49,0x49,0x41,0x41,0x00,0x00,

			// "�-205" width 7
			// .......
			// 0....0.
			// 0....0.
			// 0....0.
			// 000000.
			// 0....0.
			// 0....0.
			// 0....0.
			// 0....0.
			// .......
			// .......
		0x80,0x50,0x28,0xF4,0x0B,0x85,0x42,0x21,0x00,0x00,

			// "�-206" width 8
			// ........
			// ..000...
			// .0...0..
			// 0.....0.
			// 0.....0.
			// 0.....0.
			// 0.....0.
			// .0...0..
			// ..000...
			// ........
			// ........
		0x00,0x1C,0x22,0x41,0x41,0x41,0x41,0x22,0x1C,0x00,0x00,

			// "�-207" width 7
			// .......
			// 000000.
			// 0....0.
			// 0....0.
			// 0....0.
			// 0....0.
			// 0....0.
			// 0....0.
			// 0....0.
			// .......
			// .......
		0x80,0x5F,0x28,0x14,0x0A,0x85,0x42,0x21,0x00,0x00,

			// "�-208" width 6
			// ......
			// 0000..
			// 0...0.
			// 0...0.
			// 0...0.
			// 0000..
			// 0.....
			// 0.....
			// 0.....
			// ......
			// ......
		0xC0,0x13,0x45,0xD1,0x13,0x04,0x01,0x00,0x00,

			// "�-209" width 7
			// .......
			// ..0000.
			// .0.....
			// 0......
			// 0......
			// 0......
			// 0......
			// .0.....
			// ..0000.
			// .......
			// .......
		0x00,0x9E,0x20,0x10,0x08,0x04,0x04,0x3C,0x00,0x00,

			// "�-210" width 6
			// ......
			// 00000.
			// ..0...
			// ..0...
			// ..0...
			// ..0...
			// ..0...
			// ..0...
			// ..0...
			// ......
			// ......
		0xC0,0x47,0x10,0x04,0x41,0x10,0x04,0x00,0x00,

			// "�-211" width 7
			// .......
			// 0....0.
			// 0....0.
			// .0..0..
			// .0..0..
			// ..00...
			// ..00...
			// ..0....
			// 00.....
			// .......
			// .......
		0x80,0x50,0x48,0x22,0x61,0x30,0x08,0x03,0x00,0x00,

			// "�-212" width 8
			// ........
			// ...0....
			// .00000..
			// 0..0..0.
			// 0..0..0.
			// 0..0..0.
			// 0..0..0.
			// .00000..
			// ...0....
			// ........
			// ........
		0x00,0x08,0x3E,0x49,0x49,0x49,0x49,0x3E,0x08,0x00,0x00,

			// "�-213" width 6
			// ......
			// 0...0.
			// 0...0.
			// .0.0..
			// ..0...
			// ..0...
			// .0.0..
			// 0...0.
			// 0...0.
			// ......
			// ......
		0x40,0x14,0x29,0x04,0xA1,0x44,0x11,0x00,0x00,

			// "�-214" width 8
			// ........
			// 0....0..
			// 0....0..
			// 0....0..
			// 0....0..
			// 0....0..
			// 0....0..
			// 0....0..
			// 0000000.
			// ......0.
			// ......0.
		0x00,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x7F,0x40,0x40,

			// "�-215" width 7
			// .......
			// 0....0.
			// 0....0.
			// 0....0.
			// 0....0.
			// .00000.
			// .....0.
			// .....0.
			// .....0.
			// .......
			// .......
		0x80,0x50,0x28,0x14,0xF2,0x81,0x40,0x20,0x00,0x00,

			// "�-216" width 10
			// ..........
			// 0...0...0.
			// 0...0...0.
			// 0...0...0.
			// 0...0...0.
			// 0...0...0.
			// 0...0...0.
			// 0...0...0.
			// 000000000.
			// ..........
			// ..........
		0x00,0x44,0x14,0x51,0x44,0x11,0x45,0x14,0x51,0x44,0xFF,0x01,
		0x00,0x00,

			// "�-217" width 11
			// ...........
			// 0...0...0..
			// 0...0...0..
			// 0...0...0..
			// 0...0...0..
			// 0...0...0..
			// 0...0...0..
			// 0...0...0..
			// 0000000000.
			// .........0.
			// .........0.
		0x00,0x88,0x48,0x44,0x22,0x12,0x91,0x88,0x44,0x24,0x22,0xFF,
		0x03,0x10,0x80,0x00,

			// "�-218" width 8
			// ........
			// 000.....
			// ..0.....
			// ..0.....
			// ..0000..
			// ..0...0.
			// ..0...0.
			// ..0...0.
			// ..0000..
			// ........
			// ........
		0x00,0x07,0x04,0x04,0x3C,0x44,0x44,0x44,0x3C,0x00,0x00,

			// "�-219" width 8
			// ........
			// 0.....0.
			// 0.....0.
			// 0.....0.
			// 0000..0.
			// 0...0.0.
			// 0...0.0.
			// 0...0.0.
			// 0000..0.
			// ........
			// ........
		0x00,0x41,0x41,0x41,0x4F,0x51,0x51,0x51,0x4F,0x00,0x00,

			// "�-220" width 7
			// .......
			// .0.....
			// .0.....
			// .0.....
			// .0000..
			// .0...0.
			// .0...0.
			// .0...0.
			// .0000..
			// .......
			// .......
		0x00,0x81,0x40,0xE0,0x11,0x89,0x44,0x1E,0x00,0x00,

			// "�-221" width 7
			// .......
			// 0000...
			// ....0..
			// .....0.
			// .00000.
			// .....0.
			// .....0.
			// ....0..
			// 0000...
			// .......
			// .......
		0x80,0x07,0x04,0xE4,0x03,0x81,0x20,0x0F,0x00,0x00,

			// "�-222" width 10
			// ..........
			// 0...000...
			// 0..0...0..
			// 0.0.....0.
			// 000.....0.
			// 0.0.....0.
			// 0.0.....0.
			// 0..0...0..
			// 0...000...
			// ..........
			// ..........
		0x00,0xC4,0x91,0x48,0x41,0x07,0x15,0x54,0x50,0x22,0x71,0x00,
		0x00,0x00,

			// "�-223" width 7
			// .......
			// ..0000.
			// .0...0.
			// .0...0.
			// .0...0.
			// ..0000.
			// ..0..0.
			// .0...0.
			// 0....0.
			// .......
			// .......
		0x00,0x9E,0x48,0x24,0xE2,0x91,0x44,0x21,0x00,0x00,

			// "�-224" width 6
			// ......
			// ......
			// ......
			// .000..
			// ....0.
			// .0000.
			// 0...0.
			// 0...0.
			// .0000.
			// ......
			// ......
		0x00,0x00,0x38,0x90,0x17,0x45,0x1E,0x00,0x00,

			// "�-225" width 6
			// ..000.
			// .0....
			// 0.....
			// 0000..
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// .000..
			// ......
			// ......
		0x9C,0x10,0x3C,0x51,0x14,0x45,0x0E,0x00,0x00,

			// "�-226" width 6
			// ......
			// ......
			// ......
			// 0000..
			// 0...0.
			// 0000..
			// 0...0.
			// 0...0.
			// 0000..
			// ......
			// ......
		0x00,0x00,0x3C,0xD1,0x13,0x45,0x0F,0x00,0x00,

			// "�-227" width 5
			// .....
			// .....
			// .....
			// 0000.
			// 0....
			// 0....
			// 0....
			// 0....
			// 0....
			// .....
			// .....
		0x00,0x80,0x17,0x42,0x08,0x01,0x00,

			// "�-228" width 7
			// .......
			// .......
			// .......
			// ..000..
			// ..0.0..
			// ..0.0..
			// .0..0..
			// .0..0..
			// 000000.
			// 0....0.
			// .......
		0x00,0x00,0x80,0x43,0xA1,0x48,0x24,0xBF,0x10,0x00,

			// "�-229" width 6
			// ......
			// ......
			// ......
			// .000..
			// 0...0.
			// 00000.
			// 0.....
			// 0...0.
			// .000..
			// ......
			// ......
		0x00,0x00,0x38,0xD1,0x17,0x44,0x0E,0x00,0x00,

			// "�-230" width 8
			// ........
			// ........
			// ........
			// 0..0..0.
			// .0.0.0..
			// .0.0.0..
			// ..000...
			// .0.0.0..
			// 0..0..0.
			// ........
			// ........
		0x00,0x00,0x00,0x49,0x2A,0x2A,0x1C,0x2A,0x49,0x00,0x00,

			// "�-231" width 5
			// .....
			// .....
			// .....
			// 000..
			// ...0.
			// .00..
			// ...0.
			// ...0.
			// 000..
			// .....
			// .....
		0x00,0x80,0x83,0x0C,0x42,0x07,0x00,

			// "�-232" width 6
			// ......
			// ......
			// ......
			// 0...0.
			// 0..00.
			// 0.0.0.
			// 00..0.
			// 0...0.
			// 0...0.
			// ......
			// ......
		0x00,0x00,0x44,0x59,0x35,0x45,0x11,0x00,0x00,

			// "�-233" width 6
			// .0..0.
			// ..00..
			// ......
			// 0...0.
			// 0..00.
			// 0.0.0.
			// 00..0.
			// 0...0.
			// 0...0.
			// ......
			// ......
		0x12,0x03,0x44,0x59,0x35,0x45,0x11,0x00,0x00,

			// "�-234" width 6
			// ......
			// ......
			// ......
			// 0..00.
			// 0.0...
			// 00....
			// 0.0...
			// 0..0..
			// 0...0.
			// ......
			// ......
		0x00,0x00,0x64,0xC5,0x50,0x24,0x11,0x00,0x00,

			// "�-235" width 6
			// ......
			// ......
			// ......
			// .0000.
			// .0..0.
			// .0..0.
			// .0..0.
			// .0..0.
			// 0...0.
			// ......
			// ......
		0x00,0x00,0x78,0x92,0x24,0x49,0x11,0x00,0x00,

			// "�-236" width 6
			// ......
			// ......
			// ......
			// 0...0.
			// 00.00.
			// 0.0.0.
			// 0.0.0.
			// 0...0.
			// 0...0.
			// ......
			// ......
		0x00,0x00,0x44,0x5B,0x55,0x45,0x11,0x00,0x00,

			// "�-237" width 6
			// ......
			// ......
			// ......
			// 0...0.
			// 0...0.
			// 00000.
			// 0...0.
			// 0...0.
			// 0...0.
			// ......
			// ......
		0x00,0x00,0x44,0xD1,0x17,0x45,0x11,0x00,0x00,

			// "�-238" width 6
			// ......
			// ......
			// ......
			// .000..
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// .000..
			// ......
			// ......
		0x00,0x00,0x38,0x51,0x14,0x45,0x0E,0x00,0x00,

			// "�-239" width 6
			// ......
			// ......
			// ......
			// 00000.
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// ......
			// ......
		0x00,0x00,0x7C,0x51,0x14,0x45,0x11,0x00,0x00,

			// "�-240" width 6
			// ......
			// ......
			// ......
			// 0000..
			// 0...0.
			// 0...0.
			// 0...0.
			// 0...0.
			// 0000..
			// 0.....
			// 0.....
		0x00,0x00,0x3C,0x51,0x14,0x45,0x4F,0x10,0x00,

			// "�-241" width 5
			// .....
			// .....
			// .....
			// .000.
			// 0....
			// 0....
			// 0....
			// 0....
			// .000.
			// .....
			// .....
		0x00,0x00,0x17,0x42,0x08,0x0E,0x00,

			// "�-242" width 6
			// ......
			// ......
			// ......
			// 00000.
			// ..0...
			// ..0...
			// ..0...
			// ..0...
			// ..0...
			// ......
			// ......
		0x00,0x00,0x7C,0x04,0x41,0x10,0x04,0x00,0x00,

			// "�-243" width 6
			// ......
			// ......
			// ......
			// 0...0.
			// 0...0.
			// .0.0..
			// .0.0..
			// ..0...
			// ..0...
			// .0....
			// .0....
		0x00,0x00,0x44,0x91,0xA2,0x10,0x84,0x20,0x00,

			// "�-244" width 8
			// ...0....
			// ...0....
			// ...0....
			// .00000..
			// 0..0..0.
			// 0..0..0.
			// 0..0..0.
			// 0..0..0.
			// .00000..
			// ...0....
			// ...0....
		0x08,0x08,0x08,0x3E,0x49,0x49,0x49,0x49,0x3E,0x08,0x08,

			// "�-245" width 6
			// ......
			// ......
			// ......
			// 0...0.
			// .0.0..
			// ..0...
			// ..0...
			// .0.0..
			// 0...0.
			// ......
			// ......
		0x00,0x00,0x44,0x0A,0x41,0x28,0x11,0x00,0x00,

			// "�-246" width 6
			// ......
			// ......
			// ......
			// 0..0..
			// 0..0..
			// 0..0..
			// 0..0..
			// 0..0..
			// 00000.
			// ....0.
			// ......
		0x00,0x00,0x24,0x49,0x92,0x24,0x1F,0x04,0x00,

			// "�-247" width 6
			// ......
			// ......
			// ......
			// 0...0.
			// 0...0.
			// 0...0.
			// .0000.
			// ....0.
			// ....0.
			// ......
			// ......
		0x00,0x00,0x44,0x51,0xE4,0x41,0x10,0x00,0x00,

			// "�-248" width 8
			// ........
			// ........
			// ........
			// 0..0..0.
			// 0..0..0.
			// 0..0..0.
			// 0..0..0.
			// 0..0..0.
			// 0000000.
			// ........
			// ........
		0x00,0x00,0x00,0x49,0x49,0x49,0x49,0x49,0x7F,0x00,0x00,

			// "�-249" width 9
			// .........
			// .........
			// .........
			// 0..0..0..
			// 0..0..0..
			// 0..0..0..
			// 0..0..0..
			// 0..0..0..
			// 00000000.
			// .......0.
			// .........
		0x00,0x00,0x00,0x48,0x92,0x24,0x49,0x92,0x24,0xFF,0x00,0x01,
		0x00,

			// "�-250" width 7
			// .......
			// .......
			// .......
			// 000....
			// ..0....
			// ..000..
			// ..0..0.
			// ..0..0.
			// ..000..
			// .......
			// .......
		0x00,0x00,0xE0,0x40,0xE0,0x90,0x48,0x1C,0x00,0x00,

			// "�-251" width 8
			// ........
			// ........
			// ........
			// 0.....0.
			// 0.....0.
			// 0000..0.
			// 0...0.0.
			// 0...0.0.
			// 0000..0.
			// ........
			// ........
		0x00,0x00,0x00,0x41,0x41,0x4F,0x51,0x51,0x4F,0x00,0x00,

			// "�-252" width 7
			// .......
			// .......
			// .......
			// .0.....
			// .0.....
			// .0000..
			// .0...0.
			// .0...0.
			// .0000..
			// .......
			// .......
		0x00,0x00,0x40,0x20,0xF0,0x88,0x44,0x1E,0x00,0x00,

			// "�-253" width 5
			// .....
			// .....
			// .....
			// 000..
			// ...0.
			// .000.
			// ...0.
			// ...0.
			// 000..
			// .....
			// .....
		0x00,0x80,0x83,0x1C,0x42,0x07,0x00,

			// "�-254" width 9
			// .........
			// .........
			// .........
			// 0...000..
			// 0..0...0.
			// 0000...0.
			// 0..0...0.
			// 0..0...0.
			// 0...000..
			// .........
			// .........
		0x00,0x00,0x00,0x88,0x93,0xE8,0x51,0xA2,0x44,0x71,0x00,0x00,
		0x00,

			// "�-255" width 6
			// ......
			// ......
			// ......
			// .0000.
			// 0...0.
			// 0...0.
			// .0000.
			// .0..0.
			// 0...0.
			// ......
			// .....0
		0x00,0x00,0x78,0x51,0xE4,0x49,0x11,0x00,0x02,

};
const uint16_t fnt11_width[] = {
		5, // "SP-32" 
		2, // "!-33" 
		5, // ""-34" 
		8, // "#-35" 
		6, // "$-36" 
		11, // "%-37" 
		8, // "&-38" 
		3, // "'-39" 
		4, // "(-40" 
		4, // ")-41" 
		6, // "*-42" 
		8, // "+-43" 
		3, // ",-44" 
		4, // "--45" 
		2, // ".-46" 
		4, // "/-47" 
		6, // "0-48" 
		4, // "1-49" 
		6, // "2-50" 
		6, // "3-51" 
		6, // "4-52" 
		6, // "5-53" 
		6, // "6-54" 
		6, // "7-55" 
		6, // "8-56" 
		6, // "9-57" 
		2, // ":-58" 
		3, // ";-59" 
		7, // "<-60" 
		8, // "=-61" 
		7, // ">-62" 
		5, // "?-63" 
		10, // "@-64" 
		7, // "A-65" 
		6, // "B-66" 
		7, // "C-67" 
		7, // "D-68" 
		6, // "E-69" 
		6, // "F-70" 
		7, // "G-71" 
		7, // "H-72" 
		4, // "I-73" 
		5, // "J-74" 
		6, // "K-75" 
		5, // "L-76" 
		8, // "M-77" 
		7, // "N-78" 
		8, // "O-79" 
		6, // "P-80" 
		8, // "Q-81" 
		7, // "R-82" 
		6, // "S-83" 
		6, // "T-84" 
		7, // "U-85" 
		6, // "V-86" 
		10, // "W-87" 
		6, // "X-88" 
		6, // "Y-89" 
		6, // "Z-90" 
		4, // "[-91" 
		4, // "\-92" 
		4, // "]-93" 
		8, // "^-94" 
		7, // "_-95" 
		3, // "`-96" 
		6, // "a-97" 
		6, // "b-98" 
		5, // "c-99" 
		6, // "d-100" 
		6, // "e-101" 
		4, // "f-102" 
		6, // "g-103" 
		6, // "h-104" 
		2, // "i-105" 
		3, // "j-106" 
		6, // "k-107" 
		2, // "l-108" 
		8, // "m-109" 
		6, // "n-110" 
		6, // "o-111" 
		6, // "p-112" 
		6, // "q-113" 
		4, // "r-114" 
		5, // "s-115" 
		4, // "t-116" 
		6, // "u-117" 
		6, // "v-118" 
		8, // "w-119" 
		6, // "x-120" 
		6, // "y-121" 
		5, // "z-122" 
		5, // "{-123" 
		2, // "|-124" 
		5, // "}-125" 
		8, // "~-126" 
		7, // "�-192" 
		6, // "�-193" 
		6, // "�-194" 
		6, // "�-195" 
		8, // "�-196" 
		6, // "�-197" 
		10, // "�-198" 
		6, // "�-199" 
		7, // "�-200" 
		7, // "�-201" 
		7, // "�-202" 
		7, // "�-203" 
		8, // "�-204" 
		7, // "�-205" 
		8, // "�-206" 
		7, // "�-207" 
		6, // "�-208" 
		7, // "�-209" 
		6, // "�-210" 
		7, // "�-211" 
		8, // "�-212" 
		6, // "�-213" 
		8, // "�-214" 
		7, // "�-215" 
		10, // "�-216" 
		11, // "�-217" 
		8, // "�-218" 
		8, // "�-219" 
		7, // "�-220" 
		7, // "�-221" 
		10, // "�-222" 
		7, // "�-223" 
		6, // "�-224" 
		6, // "�-225" 
		6, // "�-226" 
		5, // "�-227" 
		7, // "�-228" 
		6, // "�-229" 
		8, // "�-230" 
		5, // "�-231" 
		6, // "�-232" 
		6, // "�-233" 
		6, // "�-234" 
		6, // "�-235" 
		6, // "�-236" 
		6, // "�-237" 
		6, // "�-238" 
		6, // "�-239" 
		6, // "�-240" 
		5, // "�-241" 
		6, // "�-242" 
		6, // "�-243" 
		8, // "�-244" 
		6, // "�-245" 
		6, // "�-246" 
		6, // "�-247" 
		8, // "�-248" 
		9, // "�-249" 
		7, // "�-250" 
		8, // "�-251" 
		7, // "�-252" 
		5, // "�-253" 
		9, // "�-254" 
		6, // "�-255" 
};



LCDUI_FONT font_fnt11 = {
		(uint16_t*)fnt11_width,
		(uint8_t*)fnt11_data,
		11,
		sizeof(fnt11_width)/sizeof(uint16_t)
		};
