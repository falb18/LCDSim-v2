#include "../lcdsim-defs/lcdsim-defs.h"
#include "hd44780.h"

#define CGROM_BIN_FILE "cgrom.bin"

void HD44780_Init(HD44780 *self)
{

    /* Init internal registers of the HD44780 hardware emulator */
    self->CGRAM_counter = 0;
    self->DDRAM_counter = 0;
    self->DDRAM_display = 0;
    self->LCD_EntryMode = 0x02;
    self->LCD_CursorEnable = 0;
    self->LCD_CursorBlink = FIXED;
    self->LCD_CursorState = 0;
    self->LCD_DisplayEnable = 1;
    self->RAM_current = DDR;

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
                self->CGROM[cgrom_array[i]][j] = cgrom_array[i+j+1];
            }
        }
        else
        { 
            break;
        }
    }

    fclose(cgrom);

    /* Initialize DDRAM with blank character (character code = 0x20) */
    memset(self->DDRAM, 0x20, 104);
}

void GraphicUnit_Init(GraphicUnit *self)
{
    Uint8 i;
    SDL_Surface *colors_surface[COLORS];

    self->position.x = 0;
    self->position.y = 0;

    /* Create the tiny pixels of the LCD: */
    for (i = 0; i < COLORS; i++)
    {
        colors_surface[i] = SDL_CreateRGBSurface(0, PIXEL_SIZE, PIXEL_SIZE, 32, 0, 0, 0, 0);
    }

    SDL_FillRect(colors_surface[BLACK], NULL, SDL_MapRGB(colors_surface[BLACK]->format, 0, 0, 0));
    SDL_FillRect(colors_surface[GREEN], NULL, SDL_MapRGB(colors_surface[GREEN]->format, 125, 159, 50));

    self->color[BLACK] = SDL_CreateTextureFromSurface(self->screen, colors_surface[BLACK]);
    self->color[GREEN] = SDL_CreateTextureFromSurface(self->screen, colors_surface[GREEN]);

    /* Free the surfaces since are not going to be used anymore */
    free(colors_surface[BLACK]);
    free(colors_surface[GREEN]);

    /* Load the image for the LCD: */
    self->image = IMG_LoadTexture(self->screen, "../res/lcd_layout.bmp");
    
    Pixel_Init(self->pixel);
}

void Pixel_Init(Pixel pixel[][LCD_FONT_WIDTH][LCD_FONT_HEIGHT])
{
    Uint8 z, x , y;

    for (z = 0; z < NUM_CHARS_LCD; z++)
    {
        for(x = 0 ; x < LCD_FONT_WIDTH; x++)
        {
            for(y = 0 ; y < LCD_FONT_HEIGHT; y++)
            {
                pixel[z][x][y].position.x = MARGIN_LCD_X + ((z % CHARS_PER_LINE) * CHARS_PER_LINE) + (x * PIXEL_SIZE);
                pixel[z][x][y].position.y = MARGIN_LCD_Y + ((z >= CHARS_PER_LINE) * 25) + (y * PIXEL_SIZE);
                pixel[z][x][y].position.w = PIXEL_SIZE;
                pixel[z][x][y].position.h = PIXEL_SIZE;
                pixel[z][x][y].color = GREEN;
            }
        }
    }
}

void Pixel_Refresh(HD44780 mcu, Pixel pixel[][LCD_FONT_WIDTH][LCD_FONT_HEIGHT])
{
    Uint8 z, x ,y, n;
    
    for (z = 0; z < NUM_CHARS_LCD; z++)
    {
        for (x = 0; x < LCD_FONT_WIDTH; x++)
        {
            for (y = 0; y < LCD_FONT_HEIGHT; y++)
            {
                if (!mcu.LCD_DisplayEnable)
                {
                    pixel[z][x][y].color = GREEN;
                }
                else
                {
                    n = mcu.DDRAM_display + (z % CHARS_PER_LINE) + (z >= CHARS_PER_LINE) * SECOND_LINE_ADDRESS;
                    if ((mcu.CGROM[mcu.DDRAM[n]][y] >> (LCD_FONT_WIDTH - 1 - x)) & 0x01)
                    {
                        pixel[z][x][y].color = BLACK;
                    }
                    else
                    {
                        pixel[z][x][y].color = GREEN;
                    }
                }
            }
        }
    }

    if ((mcu.LCD_CursorState || mcu.LCD_CursorBlink == FIXED) && mcu.LCD_DisplayEnable && mcu.LCD_CursorEnable)
    {
        if ((mcu.DDRAM_display <= mcu.DDRAM_counter) && ((mcu.DDRAM_display + 0x0F) >= mcu.DDRAM_counter))
        {
            for (x = 0; x < LCD_FONT_WIDTH; x++)
            {
                for (y = 0; y < LCD_FONT_HEIGHT; y++)
                {
                    pixel[mcu.DDRAM_counter - mcu.DDRAM_display][x][y].color = BLACK;
                }
            }
        }

        if ((SECOND_LINE_ADDRESS + mcu.DDRAM_display <= mcu.DDRAM_counter) && ((mcu.DDRAM_display + 0x4F) >= mcu.DDRAM_counter))
        {
            for (x = 0; x < LCD_FONT_WIDTH; x++)
            {
                for (y = 0; y < LCD_FONT_HEIGHT; y++)
                {
                    pixel[CHARS_PER_LINE + mcu.DDRAM_counter - mcu.DDRAM_display - SECOND_LINE_ADDRESS][x][y].color = BLACK;
                }
            }
        }
    }
}

void Pixel_Draw(GraphicUnit *self)
{
    Uint8 z, x, y;
    Uint8 pixel_color;
    SDL_Rect *pixel_area;
    SDL_Texture *pixel_to_draw;

    for (z = 0; z < NUM_CHARS_LCD; z++)
    {
        for (x = 0; x < LCD_FONT_WIDTH; x++)
        {
            for (y = 0; y < LCD_FONT_HEIGHT; y++)
            {
                pixel_color = self->pixel[z][x][y].color;
                pixel_to_draw = self->color[pixel_color];
                pixel_area = &self->pixel[z][x][y].position;

                SDL_RenderCopy(self->screen, pixel_to_draw, NULL, pixel_area);
            }
        }
    }
}