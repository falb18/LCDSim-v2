# How to configure a third party library to use it with the LCD simulator

You have a custom or third party library, like [avr-hd44780](https://github.com/aostanin/avr-hd44780), that controls and
communicates with the LCD. So to use that library with the simulator you have to do the following configuration:

First, on the third party library comment out everything related to the hardware pins since are not valid for the simulator.
For example, in file **lcd.h**, from the library, the pins are defined, which we have to comment out:

```c
/*
#pragma once

#include <avr/io.h>

#if ( !defined LCD_DDR || !defined LCD_PORT )
	#warning "Please define LCD_DDR and LCD_PORT"
	#define LCD_DDR  DDRB
	#define LCD_PORT PORTB
#endif

#ifndef LCD_RS
	#warning "LCD Using default pin"
	#define LCD_RS 0
	#define LCD_RW 1
	#define LCD_EN 2
	#define LCD_D0 4
	#define LCD_D1 5
	#define LCD_D2 6
	#define LCD_D3 7
#endif
*/
```

Also, comment the headers which are related to the MCU or hardware functions. For example, in file **lcd.c** we are using
is defined a header that is only use in the avr-gcc compiler, which has to be commented out:

```c
/* #include <util/delay.h> */
```

Look for the functions that send data to the LCD through the gpios. Edit those functions so instead of writing to gpios,
they call the LCDSim write functions. In our example, we edited the functions ***lcd_send()***, ***lcd_init()***

```c
void lcd_send(uint8_t value, uint8_t mode) {
  /* if (mode) {
    LCD_PORT = LCD_PORT | (1 << LCD_RS);
  } else {
    LCD_PORT = LCD_PORT & ~(1 << LCD_RS);
  }

  LCD_PORT = LCD_PORT & ~(1 << LCD_RW);

  lcd_write_nibble(value >> 4);
  lcd_write_nibble(value); */

  if (mode == 1) {
    LCDSim_Instruction(p_lcd, (Uint16)(0x100 | value));
  } else {
    LCDSim_Instruction(p_lcd, (Uint16)value);
  }
}

void lcd_init(void) {
/*   // Configure pins as output
  LCD_DDR = LCD_DDR
    | (1 << LCD_RS)
    | (1 << LCD_RW)
    | (1 << LCD_EN)
    | (1 << LCD_D0)
    | (1 << LCD_D1)
    | (1 << LCD_D2)
    | (1 << LCD_D3);

  // Wait for LCD to become ready (docs say 15ms+)
  _delay_ms(15);

  LCD_PORT = LCD_PORT
    & ~(1 << LCD_EN)
    & ~(1 << LCD_RS)
    & ~(1 << LCD_RW);

  _delay_ms(4.1);

  lcd_write_nibble(0x03); // Switch to 4 bit mode
  _delay_ms(4.1);

  lcd_write_nibble(0x03); // 2nd time
  _delay_ms(4.1);

  lcd_write_nibble(0x03); // 3rd time
  _delay_ms(4.1);

  lcd_write_nibble(0x02); // Set 8-bit mode (?) */

  lcd_command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);

  lcd_displayparams = LCD_CURSORON | LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}
```

The third party library may use functions that are defined in other header files and are hardware specific, comment also
those functions to prevent any errors during compilation, like not finding the declaration of them.

```c
void lcd_clear(void) {
  lcd_command(LCD_CLEARDISPLAY);
//   _delay_ms(2);
}

void lcd_return_home(void) {
  lcd_command(LCD_RETURNHOME);
//   _delay_ms(2);
}
```

Finally, it is important to add the init function of the LCD simulator on the library. Edit the following files to add
the function:

```c
/* In lcd.h add the header file of the LCD simulator and declare the initialize function */
#include "lcdsim.h"

void lcd_init_lib(LCDSim *lcd);

/* IN lcd.c define the function that initializes the simulator and the lcd library */

void lcd_init_lib(LCDSim *lcd)
{
    p_lcd = lcd;
    lcd_init();
}
```

Once we are sure that the project compiles without errors of missing headers or functions not declared, now we can
control the lcd through the third party library and see the characters on the LCD simulator.