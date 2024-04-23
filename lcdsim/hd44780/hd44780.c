#include <string.h>

#include "hd44780.h"

/*
 * Private function protoypes:
 * ==============================================
 */

static void hd44780_update_pixels(HD44780 *mcu, Uint8 *pixels);
static uint8_t get_ddram_address(uint8_t ddram_display, uint8_t char_idx);
static uint8_t get_char_idx(uint8_t ddram_display, uint8_t ddram_counter);

/*
 * External functions:
 * ==============================================
 */

void HD44780_Init(HD44780 *mcu, GraphicUnit *graph_unit)
{
    uint16_t i;
    uint8_t j;
    Uint8 cgrom_array[CHARACTER_PATTERN_SIZE];

    /* Init internal registers of the HD44780 hardware emulator */
    mcu->CGRAM_counter = 0;
    mcu->DDRAM_counter = 0;
    mcu->DDRAM_display = 0;
    mcu->LCD_EntryMode = INCREMENT_DDRAM_ADDRESS;
    mcu->LCD_CursorEnable = 0;
    mcu->LCD_CursorBlink = FIXED;
    mcu->LCD_CursorState = 0;
    mcu->LCD_DisplayEnable = 1;
    mcu->RAM_current = DDR;
    
    memset(mcu->CGROM, 0x00, (NUM_CHARACTER_CODES * BYTES_PER_PATTERN));

    /* Init CGROM */
    FILE *cgrom = fopen(LCDSIM_CGROM_BIN, "rb");
    
    if (cgrom == NULL)
    {
        printf("File %s not found\n", LCDSIM_CGROM_BIN);
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

    graph_unit->lcd_pixels = (Uint8 *)calloc(NUM_CHARS_LCD, LCD_FONT_WIDTH * LCD_FONT_HEIGHT);
}

void HD44780_Update(HD44780 *mcu, GraphicUnit *graph_unit)
{
    /* Clear the LCD pixel's buffer each time we write on it to avoid random data */
    memset(graph_unit->lcd_pixels, 0x00, (NUM_CHARS_LCD * LCD_FONT_WIDTH * LCD_FONT_HEIGHT));
    
    hd44780_update_pixels(mcu, graph_unit->lcd_pixels);
}

void HD44780_ParseCMD(HD44780 *mcu, Uint16 instruction)
{
    uint8_t i, n, m;

    if (instruction & 0x0100)
    {
        if (mcu->RAM_current == CGR)
        {
            n = mcu->CGRAM_counter / 8;
            m = mcu->CGRAM_counter % 8;
            mcu->CGROM[n][m] = instruction & 0xFF;
            if (mcu->CGRAM_counter < 64)
                mcu->CGRAM_counter++;
        }
        else
        {
            mcu->DDRAM[mcu->DDRAM_counter] = instruction & 0xFF;
            if (mcu->LCD_EntryMode & INCREMENT_DDRAM_ADDRESS)
            {
                if (mcu->DDRAM_counter < (SECOND_LINE_LAST_POS_DDRAM_ADDR + 1))
                {
                    if (mcu->DDRAM_counter == FIRST_LINE_LAST_POS_DDRAM_ADDR)
                        mcu->DDRAM_counter = SECOND_LINE_ADDRESS;
                    else
                        mcu->DDRAM_counter++;
                }
                if (mcu->LCD_EntryMode & SHIFT_DISPLAY)
                {
                    if (mcu->DDRAM_display < 24)
                        mcu->DDRAM_display++;
                }
            }
            else
            {
                if (mcu->DDRAM_counter > 0)
                {
                    if (mcu->DDRAM_counter == SECOND_LINE_ADDRESS)
                        mcu->DDRAM_counter = FIRST_LINE_LAST_POS_DDRAM_ADDR;
                    else
                        mcu->DDRAM_counter--;
                }
                if (mcu->LCD_EntryMode & SHIFT_DISPLAY)
                {
                    if (mcu->DDRAM_display > 0)
                        mcu->DDRAM_display--;
                }
            }
        }
    }
    else
    {
        for (i = 0; i < 8; i++)
            if (instruction & (SET_DDRAM_ADDRESS >> i))
                break;
        switch (i)
        {
            /* SET DDRAM ADDRESS */
            case 0:
                mcu->DDRAM_counter = instruction & 0x7F;
                mcu->RAM_current = DDR;
                break;
            /* SET CGRAM ADDRESS */
            case 1:
                mcu->CGRAM_counter = instruction & 0x3F;
                mcu->RAM_current = CGR;
                break;
            /* CURSOR/DISPLAY SHIFT */
            case 3:
                if (instruction & DISPLAY_SHIFT)
                {
                    if (instruction & MOVE_RIGHT)
                    {
                        if (mcu->DDRAM_display < 24)
                            mcu->DDRAM_display++;
                    }
                    else
                    {
                        if (mcu->DDRAM_display > 0)
                            mcu->DDRAM_display--;
                    }
                }
                else
                {
                    if (instruction & MOVE_RIGHT)
                    {
                        if (mcu->DDRAM_counter < (SECOND_LINE_LAST_POS_DDRAM_ADDR + 1))
                        {
                            if (mcu->DDRAM_counter == FIRST_LINE_LAST_POS_DDRAM_ADDR)
                                mcu->DDRAM_counter = SECOND_LINE_ADDRESS;
                            else
                                mcu->DDRAM_counter++;
                        }
                    }
                    else
                    {
                        if (mcu->DDRAM_counter > 0)
                        {
                            if (mcu->DDRAM_counter == SECOND_LINE_ADDRESS)
                                mcu->DDRAM_counter = FIRST_LINE_LAST_POS_DDRAM_ADDR;
                            else
                                mcu->DDRAM_counter--;
                        }
                    }
                }
                break;
            /* DISPLAY ON/OFF CONTROL */
            case 4:
                mcu->LCD_CursorBlink = instruction & BLINK_ON;
                mcu->LCD_CursorEnable = (instruction & CURSOR_ON) >> 1;
                mcu->LCD_DisplayEnable = (instruction & DISPLAY_ON) >> 2;
                mcu->LCD_CursorState = 0;
                break;
            /* ENTRY MODE SET */
            case 5:
                mcu->LCD_EntryMode = instruction & (INCREMENT_DDRAM_ADDRESS | SHIFT_DISPLAY);
                break;
            /* HOME */
            case 6:
                mcu->DDRAM_counter = 0;
                mcu->DDRAM_display = 0;
                break;
            /* CLEAR */
            case 7:
                for (i = 0; i < 80; i++)
                    mcu->DDRAM[i] = 0x20;
                mcu->DDRAM_counter = 0;
                mcu->DDRAM_display = 0;
                break;
        }
    }
}

/*
 * Private functions:
 * ==============================================
 */

static void hd44780_update_pixels(HD44780 *mcu, Uint8 *pixels)
{
    uint8_t char_idx = 0 , x = 0 ,y = 0;
    uint16_t idx = 0;
    uint8_t ddram_address = 0;
    uint8_t ddram_char_code = 0;
    
    if (mcu->LCD_DisplayEnable == DISPLAY_OFF)
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
                    ddram_address = get_ddram_address(mcu->DDRAM_display, char_idx);

                    /* Get the character code from the DDRAM address */
                    ddram_char_code = mcu->DDRAM[ddram_address];

                    /* Turn ON or OFF the corresponding pixels of the character pattern given by the character code */
                    idx = y + (x * LCD_FONT_HEIGHT) + (char_idx * LCD_FONT_WIDTH * LCD_FONT_HEIGHT);
                    if ((mcu->CGROM[ddram_char_code][y] >> (LCD_FONT_WIDTH - 1 - x)) & 0x01)
                    {
                        pixels[idx] = BLACK;
                    }
                    else
                    {
                        pixels[idx] = GREEN;
                    }
                }
            }
        }
    }

    /* Turn ON or OFF the cursor depending of its configuraton */
    if ((mcu->LCD_CursorState || mcu->LCD_CursorBlink == FIXED) && mcu->LCD_DisplayEnable && mcu->LCD_CursorEnable)
    {
        char_idx = get_char_idx(mcu->DDRAM_display, mcu->DDRAM_counter);

        for (x = 0; x < LCD_FONT_WIDTH; x++)
        {
            for (y = 0; y < LCD_FONT_HEIGHT; y++)
            {
                idx = y + (x * LCD_FONT_HEIGHT) + (char_idx * LCD_FONT_WIDTH * LCD_FONT_HEIGHT);
                pixels[idx] = BLACK;
            }
        }
    }
}

static uint8_t get_ddram_address(uint8_t ddram_display, uint8_t char_idx)
{
    uint8_t row = 0;
    uint8_t start_address = 0;

    row = (uint8_t)(char_idx / CHARS_PER_LINE);

    switch (row)
    {
        case 0:
            start_address = FIRST_LINE_ADDRESS;
            break;
        
        case 1:
            start_address = SECOND_LINE_ADDRESS;
            break;

        #ifdef LCDSIM_20x4
        
        case 2:
            start_address = THIRD_LINE_ADDRESS;
            break;
        
        case 3:
            start_address = FOURTH_LINE_ADDRESS;
            break;
        
        #endif

        default:
            break;
    }

    return ddram_display + (char_idx % CHARS_PER_LINE) +  start_address;
}

static uint8_t get_char_idx(uint8_t ddram_display, uint8_t ddram_counter)
{
    #ifdef LCDSIM_20x4

    if (ddram_counter < THIRD_LINE_ADDRESS)
    {
        /* Index is in the first row */
        return ddram_counter - ddram_display;
    }
    else if (ddram_counter < SECOND_LINE_ADDRESS)
    {
        /* Index is in the third row */
        return (CHARS_PER_LINE*2) + (ddram_counter - ddram_display - THIRD_LINE_ADDRESS);
    }
    else if (ddram_counter < FOURTH_LINE_ADDRESS)
    {
        /* Index is in the second row */
        return CHARS_PER_LINE + (ddram_counter - ddram_display - SECOND_LINE_ADDRESS);
    }
    else
    {
        /* Index is in the fourth row */
        return (CHARS_PER_LINE*3) + (ddram_counter - ddram_display - FOURTH_LINE_ADDRESS);
    }

    #else
    
    if ((ddram_display <= ddram_counter) && ((ddram_display + 0x0F) >= ddram_counter))
    {
        return ddram_counter - ddram_display;
    }

    if ((SECOND_LINE_ADDRESS + ddram_display <= ddram_counter) &&
            ((ddram_display + 0x4F) >= ddram_counter))
    {
        return CHARS_PER_LINE + ddram_counter - ddram_display - SECOND_LINE_ADDRESS;
    }
    
    #endif
}