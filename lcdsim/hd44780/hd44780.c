#include "../lcdsim-defs/lcdsim-defs.h"
#include "hd44780.h"

#define CGROM_BIN_FILE "cgrom.bin"

/*
 * Private function protoypes:
 * ==============================================
 */

static void GraphicUnit_Init(GraphicUnit *graph_unit);
static void hd44780_reset_pixels(Pixel pixel[][LCD_FONT_WIDTH][LCD_FONT_HEIGHT]);
static void hd44780_update_pixels(HD44780 mcu, Pixel pixel[][LCD_FONT_WIDTH][LCD_FONT_HEIGHT]);
static void hd44780_draw_pixels(GraphicUnit *graph_unit);

void HD44780_Init(HD44780 *mcu, GraphicUnit *graph_unit)
{

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

    /* Init CGROM */
    Uint16 i;
    Uint8 j, cgrom_array[CHARACTER_PATTERN_SIZE];
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

    GraphicUnit_Init(graph_unit);
    hd44780_reset_pixels(graph_unit->pixel);
}

void HD44780_Draw(HD44780 mcu, GraphicUnit *graph_unit)
{
    hd44780_update_pixels(mcu, graph_unit->pixel);
    hd44780_draw_pixels(graph_unit);
}

static void GraphicUnit_Init(GraphicUnit *graph_unit)
{
    Uint8 i;
    SDL_Surface *colors_surface[COLORS];

    graph_unit->position.x = 0;
    graph_unit->position.y = 0;

    /* Create the tiny pixels of the LCD: */
    for (i = 0; i < COLORS; i++)
    {
        colors_surface[i] = SDL_CreateRGBSurface(0, PIXEL_SIZE, PIXEL_SIZE, 32, 0, 0, 0, 0);
    }

    SDL_FillRect(colors_surface[BLACK], NULL, SDL_MapRGB(colors_surface[BLACK]->format, 0, 0, 0));
    SDL_FillRect(colors_surface[GREEN], NULL, SDL_MapRGB(colors_surface[GREEN]->format, 125, 159, 50));

    graph_unit->color[BLACK] = SDL_CreateTextureFromSurface(graph_unit->screen, colors_surface[BLACK]);
    graph_unit->color[GREEN] = SDL_CreateTextureFromSurface(graph_unit->screen, colors_surface[GREEN]);

    /* Free the surfaces since are not going to be used anymore */
    free(colors_surface[BLACK]);
    free(colors_surface[GREEN]);

    /* Load the image for the LCD: */
    graph_unit->image = IMG_LoadTexture(graph_unit->screen, "../res/lcd_layout.bmp");
}

static void hd44780_reset_pixels(Pixel pixel[][LCD_FONT_WIDTH][LCD_FONT_HEIGHT])
{
    Uint8 char_idx, x , y;

    for (char_idx = 0; char_idx < NUM_CHARS_LCD; char_idx++)
    {
        for(x = 0 ; x < LCD_FONT_WIDTH; x++)
        {
            for(y = 0 ; y < LCD_FONT_HEIGHT; y++)
            {
                pixel[char_idx][x][y].position.x = MARGIN_LCD_X + ((char_idx % CHARS_PER_LINE) * CHARS_PER_LINE) +
                                                    (x * PIXEL_SIZE);
                
                pixel[char_idx][x][y].position.y = MARGIN_LCD_Y + ((char_idx >= CHARS_PER_LINE) * 25) +
                                                    (y * PIXEL_SIZE);
                
                pixel[char_idx][x][y].position.w = PIXEL_SIZE;
                pixel[char_idx][x][y].position.h = PIXEL_SIZE;
                pixel[char_idx][x][y].color = GREEN;
            }
        }
    }
}

static void hd44780_update_pixels(HD44780 mcu, Pixel pixel[][LCD_FONT_WIDTH][LCD_FONT_HEIGHT])
{
    Uint8 char_idx, x ,y;
    Uint8 ddram_address = 0;
    Uint8 ddram_char_code = 0;
    
    for (char_idx = 0; char_idx < NUM_CHARS_LCD; char_idx++)
    {
        for (x = 0; x < LCD_FONT_WIDTH; x++)
        {
            for (y = 0; y < LCD_FONT_HEIGHT; y++)
            {
                if (!mcu.LCD_DisplayEnable)
                {
                    pixel[char_idx][x][y].color = GREEN;
                }
                else
                {
                    /* Get the DDRAM address (LCD position) where the given character is going to be displayed */
                    ddram_address = mcu.DDRAM_display + (char_idx % CHARS_PER_LINE) +
                                    (char_idx >= CHARS_PER_LINE) * SECOND_LINE_ADDRESS;
                    
                    /* Get the character code fromt the DDRAM address */
                    ddram_char_code = mcu.DDRAM[ddram_address];

                    /* Turn ON or OFF the corresponding pixels of the character pattern given by the character code */
                    if ((mcu.CGROM[ddram_char_code][y] >> (LCD_FONT_WIDTH - 1 - x)) & 0x01)
                    {
                        pixel[char_idx][x][y].color = BLACK;
                    }
                    else
                    {
                        pixel[char_idx][x][y].color = GREEN;
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
                    pixel[char_idx][x][y].color = BLACK;
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
                    pixel[char_idx][x][y].color = BLACK;
                }
            }
        }
    }
}

static void hd44780_draw_pixels(GraphicUnit *graph_unit)
{
    Uint8 char_idx, x, y;
    Uint8 pixel_color;
    SDL_Rect *pixel_area;
    SDL_Texture *pixel_to_draw;

    for (char_idx = 0; char_idx < NUM_CHARS_LCD; char_idx++)
    {
        for (x = 0; x < LCD_FONT_WIDTH; x++)
        {
            for (y = 0; y < LCD_FONT_HEIGHT; y++)
            {
                pixel_color = graph_unit->pixel[char_idx][x][y].color;
                pixel_to_draw = graph_unit->color[pixel_color];
                pixel_area = &graph_unit->pixel[char_idx][x][y].position;

                SDL_RenderCopy(graph_unit->screen, pixel_to_draw, NULL, pixel_area);
            }
        }
    }
}