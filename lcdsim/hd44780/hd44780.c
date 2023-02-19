#include <string.h>

#include "../lcdsim-defs/lcdsim-defs.h"
#include "hd44780.h"

#define CGROM_BIN_FILE "cgrom.bin"

/*
 * Private function protoypes:
 * ==============================================
 */

static void hd44780_update_pixels(HD44780 mcu, Uint8 pixels[][LCD_FONT_WIDTH][LCD_FONT_HEIGHT]);

void HD44780_Init(HD44780 *mcu, GraphicUnit *graph_unit)
{
    Uint16 i;
    Uint8 j, cgrom_array[CHARACTER_PATTERN_SIZE];

    /* Init internal registers of the HD44780 hardware emulator */
    mcu->CGRAM_counter = 0;
    mcu->DDRAM_counter = 0;
    mcu->DDRAM_display = 0;
    mcu->LCD_EntryMode = 0x02;
    mcu->LCD_CursorEnable = 0;
    mcu->LCD_CursorBlink = FIXED;
    mcu->LCD_CursorState = 0;
    mcu->LCD_DisplayEnable = 1;
    mcu->RAM_current = DDR;
    
    memset(mcu->CGROM, 0x00, (NUM_CHARACTER_CODES * BYTES_PER_PATTERN));

    /* Init CGROM */
    FILE *cgrom = fopen("../res/cgrom.bin", "rb");
    
    if (cgrom == NULL)
    {
        printf("File %s not found\n", CGROM_BIN_FILE);
    }
    else
    {
        fread(cgrom_array, sizeof(cgrom_array), 1, cgrom);
    }

    for (i = 0; i < CHARACTER_PATTERN_SIZE; i += BYTES_PER_CHARACTER)
    {
        if (cgrom_array[i] != 0)
        {
            for (j = 0; j < BYTES_PER_PATTERN; j++)
            {
                mcu->CGROM[cgrom_array[i]][j] = cgrom_array[i+j+1];
            }
        }
        else
        { 
            break;
        }
    }

    fclose(cgrom);

    /* Initialize DDRAM with blank character (character code = 0x20) */
    memset(mcu->DDRAM, 0x20, 104);

    memset(graph_unit->lcd_pixels, 0x00, (NUM_CHARS_LCD * LCD_FONT_WIDTH * LCD_FONT_HEIGHT));
}

void HD44780_Update(HD44780 mcu, GraphicUnit *graph_unit)
{
    /* Clear the LCD pixel's buffer each time we write on it to avoid random data */
    memset(graph_unit->lcd_pixels, 0x00, (NUM_CHARS_LCD * LCD_FONT_WIDTH * LCD_FONT_HEIGHT));
    
    hd44780_update_pixels(mcu, graph_unit->lcd_pixels);
}

static void hd44780_update_pixels(HD44780 mcu, Uint8 pixels[][LCD_FONT_WIDTH][LCD_FONT_HEIGHT])
{
    Uint8 char_idx, x ,y;
    Uint8 ddram_address = 0;
    Uint8 ddram_char_code = 0;
    
    if (mcu.LCD_DisplayEnable == DISPLAY_OFF)
    {
        memset(pixels, 0x00, (NUM_CHARS_LCD * LCD_FONT_WIDTH * LCD_FONT_HEIGHT));
    }
    else
    {
        for (char_idx = 0; char_idx < NUM_CHARS_LCD; char_idx++)
        {
            for (x = 0; x < LCD_FONT_WIDTH; x++)
            {
                for (y = 0; y < LCD_FONT_HEIGHT; y++)
                {
                    /* Get the DDRAM address (LCD position) where the given character is going to be displayed */
                    ddram_address = mcu.DDRAM_display + (char_idx % CHARS_PER_LINE) +
                                    (char_idx >= CHARS_PER_LINE) * SECOND_LINE_ADDRESS;
                    
                    /* Get the character code fromt the DDRAM address */
                    ddram_char_code = mcu.DDRAM[ddram_address];

                    /* Turn ON or OFF the corresponding pixels of the character pattern given by the character code */
                    if ((mcu.CGROM[ddram_char_code][y] >> (LCD_FONT_WIDTH - 1 - x)) & 0x01)
                    {
                        pixels[char_idx][x][y] = BLACK;
                    }
                    else
                    {
                        pixels[char_idx][x][y] = GREEN;
                    }
                }
            }
        }
    }

    /* Turn ON or OFF the cursor depending of its configuraton */
    if ((mcu.LCD_CursorState || mcu.LCD_CursorBlink == FIXED) && mcu.LCD_DisplayEnable && mcu.LCD_CursorEnable)
    {
        if ((mcu.DDRAM_display <= mcu.DDRAM_counter) && ((mcu.DDRAM_display + 0x0F) >= mcu.DDRAM_counter))
        {
            for (x = 0; x < LCD_FONT_WIDTH; x++)
            {
                for (y = 0; y < LCD_FONT_HEIGHT; y++)
                {
                    char_idx = mcu.DDRAM_counter - mcu.DDRAM_display;
                    pixels[char_idx][x][y] = BLACK;
                }
            }
        }

        if ((SECOND_LINE_ADDRESS + mcu.DDRAM_display <= mcu.DDRAM_counter) &&
                ((mcu.DDRAM_display + 0x4F) >= mcu.DDRAM_counter))
        {
            for (x = 0; x < LCD_FONT_WIDTH; x++)
            {
                for (y = 0; y < LCD_FONT_HEIGHT; y++)
                {
                    char_idx = CHARS_PER_LINE + mcu.DDRAM_counter - mcu.DDRAM_display - SECOND_LINE_ADDRESS;
                    pixels[char_idx][x][y] = BLACK;
                }
            }
        }
    }
}