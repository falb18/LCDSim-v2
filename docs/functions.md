# Functions

Allocate memory for the object LCDSim. Require a SDL_surface* which the LCD will be draw on it, at an (x,y) position.
```
LCDSim* LCDSim_Create(SDL_Surface *screen, int x, int y);
```

---
Blit the LCD surface on the SDL_Surface* that you give in LCDSim_Create. Require the address of the LCDSim object.
```
void LCDSim_Draw(LCDSim *lcdsim);
```

---
Execute an instruction. Check the instruction set of the HD44780 to use this. Require the address of the LCDSim object.
```
void LCDSim_Instruction(LCDSim *lcdsim, Uint16 instruction);
```

Signification of the bits of 'Uint16 instruction' :

(MSB) RS DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0 (LSB)

---

Free the memory used by the LCDSim object. Require the address of the LCDSim object.
```
LCDSim* LCDSim_Destroy(LCDSim *lcdsim);
```

---
Put a character into the LCD. Require the address of the LCDSim object.
```
void LCD_PutChar(LCDSim *lcdsim, char car);
```

---
Put a string into the LCD. Require the address of the LCDSim object.
```
void LCD_PutS(LCDSim *lcdsim, char *s);
```

---
Clear the display. Require the address of the LCDSim object.
```
void LCD_Clear(LCDSim *lcdsim);
```

---
Move the cursor to the beginning of the display.
```
void LCD_Home(LCDSim *lcdsim);
```

---
Enable the display, enable the cursor, make it blink (1) or not (0). Just give 0 or 1 to theses parameters. Require the
address of the LCDSim object.
```
void LCD_State(LCDSim *lcdsim, Uint8 display_enable, Uint8 cursor_enable, Uint8 blink);
```

---
Shift the cursor of the display to the Left or the Right. Require the address of the LCDSim object.
```
void LCD_Sh_Cursor_R(LCDSim *lcdsim);
void LCD_Sh_Cursor_L(LCDSim *lcdsim);
void LCD_Sh_Display_R(LCDSim *lcdsim);
void LCD_Sh_Display_L(LCDSim *lcdsim);
```

---
Clear the line 0 or the line 1. Require the address of the LCDSim object.
```
void LCD_ClearLine(LCDSim *lcdsim, Uint8 line);
```

---
Set the cursor at the position that you want. Require the address of the LCDSim object.
```
void LCD_SetCursor(LCDSim *lcdsim, Uint8 line, Uint8 column);
```

---
Set the cursor at the position that you want. Require the address of the LCDSim object.
```
void LCD_SetCursor(LCDSim *lcdsim, Uint8 line, Uint8 column);
```

---
Write a custom character into the CGRAM by telling the char number (0-7) and an array of 8 bytes that describes the
character to create. Require the address of the LCDSim object.
```
void LCD_CustomChar(LCDSim *lcdsim, Uint8 char_number, Uint8* custom);
```