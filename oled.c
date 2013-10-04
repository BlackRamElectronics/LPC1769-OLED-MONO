#include "includes.h"

/* Solomon Systech SSD1303 OLED controller commands */

#define SET_LOWER_COLUMN_ADD        0x00            /* Set Lower Column Address */
#define SET_HIGHER_COLUMN_ADD       0x10            /* Set Higher Column Address */
#define SET_HORIZ_SCROLL_SETUP      0x26            /* Set Horizontal Scroll Setup */
#define SET_ACTIVE_HORIZ_SCROLL     0x2F            /* Activate Horizontal Scroll */
#define SET_DEACTIVE_HORIZ_SCROLL   0x2E            /* Deactivate Horizontal Scroll */
#define SET_CONTRAST                0x81            /* Set contrast level */
#define SET_BRIGHTNESS_CB           0x82            /* Set brightness level */
#define SET_SEGMENT_REMAP           0xA1            /* Set segment remap */
#define SET_DISPLAY_START_LINE      0x40            /* Set display start line */
#define SET_DISPLAY_OFFSET          0xD3            /* Set display offset */
#define SET_MULTIPLEX_RATIO         0xA8            /* Set multiplex ratio */
#define SET_ENTIRE_DISPLAY_ON_OFF   0xA4            /* Set entire display ON / OFF */
#define SET_NORMAL_INVERSE_DISPLAY  0xA6            /* Set normal / inverse display */
#define SET_DISPLAY_ON_OFF          0xAE            /* Set display ON / OFF */
#define SET_PAGE_ADDRESS            0xB0            /* Set page address */
#define SET_COM_OUTPUT_SCAN_DIR     0xC8            /* Set COM output scan direction */
#define SET_CLOCK_DIVIDE            0xD5            /* Set Display Clock Divide Ratio / Oscillator Frequency */
#define SET_AREA_COLOUR_MODE_ON_OFF 0xD8            /* Set area colour mode on/off & low power display mode */
#define SET_PRECHARGE_PERIOD        0xD9            /* Set Precharge period */
#define SET_COM_PINS_HW_CONFIG      0xDA            /* Set COM pins hardware configuration */
#define SET_VCOM_DESELECT_LEVEL     0xDB            /* Set VCOM deselect level */
#define SET_DC_DC_ON_OFF            0xAD            /* DC DC ON / OFF */
#define NOP_COMMAND                 0xAA            /* No opweration */

/* OLED display port command or data */

#define OLED_CMD                    'C'             /* Command */
#define OLED_DATA                   'D'             /* Data */

#define TEXT_CHARACTERS_PER_ROW     16              /* Number of text characters per row */
#define TEXT_ROWS                   2               /* Number of rows of text */

#define X_PIXELS                    96              /* X resultion */
#define Y_PIXELS                    16              /* Y resolution */

char oled_text_screen [2 * TEXT_CHARACTERS_PER_ROW]; /* Copy of text on OLED display */
int oled_disp_state = 0;                            /* State variable controlling display state machine */
int current_row = 0;                                /* Current text cursor row position */
int current_column = 0;                             /* Current text cursor column position */
int oled_contrast = 128;                            /* OLED display contrast setting */
int oled_brightness = 128;                          /* OLED display brightness setting */

/* Graphics buffer shadowing OLED display */

unsigned char oled_graphics_buffer [(X_PIXELS * Y_PIXELS) / 8]; /* Graphics buffer shadowing OLED display */

void init_oled_display (void);
void reprogram_oled_disp (void);
void set_oled_contrast (unsigned int contrast_level);
void set_oled_brightness (unsigned int brightness_level);
void home_oled_display (void);
void clear_oled_display (void);
void output_byte_oled (int val);
void putchar_oled (unsigned int oled_char);
void oled_disp_state_0 (unsigned int oled_char);
void oled_disp_state_1 (unsigned int oled_char);
void oled_disp_state_2 (unsigned int oled_char);
void oled_disp_state_3 (unsigned int oled_char);
void oled_disp_state_4 (unsigned int oled_char);
void display_oled (unsigned int disp_char);
void clear_eol_oled (void);
void proc_oled_cr (void);
void proc_oled_lf (void);
void refresh_oled_text_display (void);
void assert_oled_CS (void);
void deassert_oled_CS (void);
void set_oled_command_data_state (int state);
void lower_oled_cd_line (void);
void raise_oled_cd_line (void);
void put_pixel (unsigned int xpos, unsigned int ypos, unsigned int on_off);
void display_text_at_cursor (unsigned char val, unsigned int row, unsigned int column);
void set_lower_column_address (int address);
void set_higher_column_address (int address);
void set_segment_remap (void);

/* Initialise oled display */

void init_oled_display (void)

{
    reprogram_oled_disp ();
    clear_oled_display ();
}

/* Reprogram oled display from scratch */

void reprogram_oled_disp (void)

{
    set_lower_column_address (4);
    set_higher_column_address (12);
    set_oled_contrast (oled_contrast);
    set_oled_brightness (oled_brightness);
    set_segment_remap ();
}

/* Set lower column address */

void set_lower_column_address (int address)

{
    assert_oled_CS ();
    set_oled_command_data_state (OLED_CMD);
    output_byte_oled (SET_LOWER_COLUMN_ADD | address);
    output_byte_oled (NOP_COMMAND);
    deassert_oled_CS ();
}

void set_higher_column_address (int address)

/* Set higher column address */

{
    assert_oled_CS ();
    set_oled_command_data_state (OLED_CMD);
    output_byte_oled (SET_HIGHER_COLUMN_ADD | address);
    output_byte_oled (NOP_COMMAND);
    deassert_oled_CS ();
}

/* Set OLED display contrast */

void set_oled_contrast (unsigned int contrast_level)

{
    assert_oled_CS ();
    set_oled_command_data_state (OLED_CMD);
    output_byte_oled (SET_CONTRAST);
    output_byte_oled (contrast_level & 0xff);
    output_byte_oled (NOP_COMMAND);
    deassert_oled_CS ();
}

/* Set OLED display brightness */

void set_oled_brightness (unsigned int brightness_level)

{
    assert_oled_CS ();
    set_oled_command_data_state (OLED_CMD);
    output_byte_oled (SET_BRIGHTNESS_CB);
    output_byte_oled (brightness_level & 0xff);
    output_byte_oled (NOP_COMMAND);
    deassert_oled_CS ();
}

/* Set OLED Segment Re-map */

void set_segment_remap (void)

{
    assert_oled_CS ();
    set_oled_command_data_state (OLED_CMD);
    output_byte_oled (SET_SEGMENT_REMAP);
    output_byte_oled (NOP_COMMAND);
    deassert_oled_CS ();
}


/* Set OLED display start line */

void set_display_start_line (unsigned int line)

{
    assert_oled_CS ();
    set_oled_command_data_state (OLED_CMD);
    output_byte_oled (SET_DISPLAY_START_LINE);
    output_byte_oled (NOP_COMMAND);
    deassert_oled_CS ();
}

/* Set OLED display ofset */

void set_display_start_offset (unsigned int offset)

{
    assert_oled_CS ();
    set_oled_command_data_state (OLED_CMD);
    output_byte_oled (SET_DISPLAY_START_LINE);
    output_byte_oled (offset);
    output_byte_oled (NOP_COMMAND);
    deassert_oled_CS ();
}
/* Home cursor on oled display */

void home_oled_display (void)

{
    current_row = 0;
    current_column = 0;
}

/* Clear oled display, and home cursor */

void clear_oled_display (void)

{
    int n;

    for (n = 0; n < (TEXT_CHARACTERS_PER_ROW * TEXT_ROWS); n++) {
        oled_text_screen [n] = ' ';
    }

    refresh_oled_text_display ();
    home_oled_display ();
}

/* Send a byte to oled display */

void output_byte_oled (int val)

{
}

/* Display a character on oled display */

void putchar_oled (unsigned int oled_char)

{
    switch (oled_disp_state) {
        case 0:
            oled_disp_state_0 (oled_char);
            break;
        case 1:
            oled_disp_state_1 (oled_char);
            break;
        case 2:
            oled_disp_state_2 (oled_char);
            break;
        case 3:
            oled_disp_state_3 (oled_char);
            break;
        case 4:
            oled_disp_state_4 (oled_char);
            break;
    }
}

/* OLED display state 0 - waiting for first control or display character */

void oled_disp_state_0 (unsigned int oled_char)

{
    switch (oled_char) {
        case HOME:
            home_oled_display ();
            break;
        case CLS:
            init_oled_display ();
            break;
        case ESC:
            oled_disp_state = 1;
            break;
        case CR:
            proc_oled_cr ();
            break;
        case LF:
            proc_oled_lf ();
            break;
        default:
            display_oled (oled_char);
    }
}

/* OLED display state 1 - waiting for next character after 'ESC' */

void oled_disp_state_1 (unsigned int oled_char)

{
    if (oled_char == '=') {
        oled_disp_state = 2;
    }

    else if (oled_char == '!') {
        oled_disp_state = 4;
    }

    else if (oled_char == 'T') {
        clear_eol_oled ();
        oled_disp_state = 0;
    }

    else {
        oled_disp_state = 0;
    }
}

/* OLED display state 2 - waiting for 'ROW' parameter of ESC, '=' command */


void oled_disp_state_2 (unsigned int oled_char)

{
    current_row = (oled_char & 1);
    oled_disp_state = 3;
}

/* OLED display state 3 - waiting for 'COLUMN' parameter of ESC, '=' command */

void oled_disp_state_3 (unsigned int oled_char)

{
    current_column = oled_char - ' ';
    oled_disp_state = 0;
}

/* OLED display state 4 - waiting for backlight/contrast control byte of ESC, '!' command */

void oled_disp_state_4 (unsigned int oled_char)

{
    switch (oled_char) {
        case 'C':
            oled_contrast++;
            break;

        case 'c':
            oled_contrast--;
            break;

        case 'B':
            oled_brightness++;
            break;

        case 'b':
            oled_brightness--;
            break;
    }

    set_oled_contrast (oled_contrast);
    set_oled_brightness (oled_brightness);
    oled_disp_state = 0;
}


/* Output a character on OLED display at the text cursor postion, and bump cursor */

void display_oled (unsigned int disp_char)

{
    unsigned char *ptr;

    ptr = oled_text_screen + (TEXT_CHARACTERS_PER_ROW * current_row) + current_column;
    *ptr = disp_char;

    display_text_at_cursor (disp_char, current_row, current_column);

    if (current_column == (TEXT_CHARACTERS_PER_ROW - 1)) {
        if (current_row != 0) {
            home_oled_display ();
        }
        else {
            current_column = 0;
            current_row = 1;
        }
    }

    else {
        current_column++;
    }
}

/* Clear to end of line */

void clear_eol_oled (void)

{
    unsigned int row_save, column_save;
    unsigned int count;

    row_save = current_row;
    column_save = current_column;

    count = TEXT_CHARACTERS_PER_ROW - current_column;

    while (count--) {
        display_oled (' ');
    }

    current_row = row_save;
    current_column = column_save;
}

/* Perform a 'carriage return' on oled display */

void proc_oled_cr (void)

{
    current_column = 0;
}

/* Perform a 'line feed' on oled display */

void proc_oled_lf (void)

{
    current_row++;
    current_row &= 1;
}

void refresh_oled_text_display (void)

{
    int row_save, col_save, row, col, n;

    row_save = current_row;
    col_save = current_column;
    home_oled_display ();
    n = 0;

    for (row = 0; row < 2; row++) {
        for (col = 0; col < TEXT_CHARACTERS_PER_ROW; col++) {
            display_text_at_cursor (oled_text_screen [n], row, col);
            n++;
        }
    }
    current_column = col_save;
    current_row = row_save;
}




void assert_oled_CS (void)

{
}

void deassert_oled_CS (void)

{
}

/* Enablwe COMMAND or DATA mode */

void set_oled_command_data_state (int state)

{
    if (state == OLED_CMD) {
        lower_oled_cd_line ();    /* 0 = Command mode */
    }
    else {
        raise_oled_cd_line ();    /* 1 = Data mode */
    }
}

/* Lower OLED D/C line */

void lower_oled_cd_line (void)

{
}

/* Raise OLED D/C line */

void raise_oled_cd_line (void)

{
}

/* 5 by 7 font in a 6 by 8 cell */

const unsigned char font_5_7_in_6_8_cell [] = {

    0, 0, 0, 0, 0, 0, 0, 0,               /* 'space' ASCII 32 */
    32, 32, 32, 32, 32, 0, 32, 0,
    80, 80, 80, 0, 0, 0, 0, 0,
    48, 72, 64, 224, 64, 64, 248, 0,
    112, 168, 160, 112, 40, 168, 112, 0,
    192, 200, 16, 32, 64, 152, 24, 0,
    64, 160, 160, 64, 168, 144, 104, 0,
    32, 32, 32, 0, 0, 0, 0, 0,
    16, 32, 64, 64, 64, 32, 16, 0,
    64, 32, 16, 16, 16, 32, 64, 0,
    32, 168, 112, 32, 112, 168, 32, 0,
    0, 32, 32, 248, 32, 32, 0, 0,
    0, 0, 0, 0, 0, 32, 32, 64,
    0, 0, 0, 112, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 32, 0,
    0, 8, 16, 32, 64, 128, 0, 0,
    32, 80, 136, 136, 136, 80, 32, 0,
    32, 96, 32, 32, 32, 32, 112, 0,
    112, 136, 8, 48, 64, 128, 248, 0,
    248, 8, 16, 48, 8, 136, 112, 0,
    16, 48, 80, 144, 248, 16, 16, 0,
    248, 128, 240, 8, 8, 136, 112, 0,
    48, 64, 128, 240, 136, 136, 112, 0,
    248, 8, 16, 32, 64, 64, 64, 0,
    112, 136, 136, 112, 136, 136, 112, 0,
    112, 136, 136, 120, 8, 16, 96, 0,
    0, 0, 32, 0, 0, 0, 32, 0,
    0, 0, 32, 0, 0, 32, 32, 64,
    16, 32, 64, 128, 64, 32, 16, 0,
    0, 0, 248, 0, 248, 0, 0, 0,
    64, 32, 16, 8, 16, 32, 64, 0,
    112, 136, 16, 32, 32, 0, 32, 0,
    112, 136, 184, 168, 184, 128, 112, 0,
    32, 80, 136, 136, 248, 136, 136, 0,
    240, 136, 136, 240, 136, 136, 240, 0,
    112, 136, 128, 128, 128, 136, 112, 0,
    240, 136, 136, 136, 136, 136, 240, 0,
    248, 128, 128, 240, 128, 128, 248, 0,
    248, 128, 128, 240, 128, 128, 128, 0,
    112, 136, 128, 128, 152, 136, 120, 0,
    136, 136, 136, 248, 136, 136, 136, 0,
    112, 32, 32, 32, 32, 32, 112, 0,
    8, 8, 8, 8, 8, 136, 112, 0,
    136, 144, 160, 192, 160, 144, 136, 0,
    128, 128, 128, 128, 128, 128, 248, 0,
    136, 216, 168, 168, 136, 136, 136, 0,
    136, 136, 200, 168, 152, 136, 136, 0,
    112, 136, 136, 136, 136, 136, 112, 0,
    240, 136, 136, 240, 128, 128, 128, 0,
    112, 136, 136, 136, 168, 144, 104, 0,
    240, 136, 136, 240, 160, 144, 136, 0,
    112, 136, 128, 112, 8, 136, 112, 0,
    248, 32, 32, 32, 32, 32, 32, 0,
    136, 136, 136, 136, 136, 136, 112, 0,
    136, 136, 136, 80, 80, 32, 32, 0,
    136, 136, 136, 168, 168, 168, 80, 0,
    136, 136, 80, 32, 80, 136, 136, 0,
    136, 136, 80, 32, 32, 32, 32, 0,
    248, 8, 16, 32, 64, 128, 248, 0,
    192, 128, 128, 128, 128, 128, 128, 192,
    128, 64, 32, 16, 8, 4, 2, 0,
    24, 8, 8, 8, 8, 8, 8, 24,
    0, 32, 80, 136, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 248,
    0, 0, 0, 0, 0, 0, 248, 0,
    0, 0, 112, 8, 120, 136, 120, 0,
    128, 128, 240, 136, 136, 136, 240, 0,
    0, 0, 120, 128, 128, 128, 120, 0,
    8, 8, 120, 136, 136, 136, 120, 0,
    0, 0, 112, 136, 248, 128, 112, 0,
    16, 32, 32, 112, 32, 32, 32, 0,
    0, 0, 120, 136, 136, 120, 8, 112,
    128, 128, 240, 136, 136, 136, 136, 0,
    32, 0, 96, 32, 32, 32, 112, 0,
    32, 0, 32, 32, 32, 32, 32, 64,
    64, 64, 72, 80, 96, 80, 72, 0,
    96, 32, 32, 32, 32, 32, 112, 0,
    0, 0, 208, 168, 168, 168, 168, 0,
    0, 0, 240, 136, 136, 136, 136, 0,
    0, 0, 112, 136, 136, 136, 112, 0,
    0, 0, 240, 136, 136, 240, 128, 128,
    0, 0, 120, 136, 136, 120, 8, 8,
    0, 0, 88, 96, 64, 64, 64, 0,
    0, 0, 120, 128, 112, 8, 240, 0,
    32, 32, 112, 32, 32, 32, 16, 0,
    0, 0, 136, 136, 136, 136, 120, 0,
    0, 0, 136, 136, 80, 80, 32, 0,
    0, 0, 136, 136, 168, 168, 80, 0,
    0, 0, 136, 80, 32, 80, 136, 0,
    0, 0, 136, 136, 136, 120, 8, 112,
    0, 0, 248, 16, 32, 64, 248, 0,
    0, 48, 64, 64, 192, 64, 64, 48,
    32, 32, 32, 0, 32, 32, 32, 0,
    0, 96, 16, 16, 24, 16, 16, 96,
    0, 32, 0, 248, 0, 32, 0, 0,
    248, 248, 248, 248, 248, 248, 248, 0,

};

const char reverse_bits_table [8] = {
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

/* Display a 6 by 8 font character */

void display_text_at_cursor (unsigned char val, unsigned int row, unsigned int column)

{
    unsigned char *chargen_ptr ;
    unsigned char chargen_row_data;
    unsigned long chargen_row, pixel_no;


    if ((row > (TEXT_ROWS - 1)) || (column > (TEXT_CHARACTERS_PER_ROW -1))) {
        return;
    }

    chargen_ptr = (unsigned char *)&font_5_7_in_6_8_cell;
    chargen_ptr += ((val - ' ') * 8);

    for (chargen_row = 0; chargen_row < 8; chargen_row++) {
        chargen_row_data = *chargen_ptr;
        for (pixel_no = 0; pixel_no < 6; pixel_no++) {
            if (chargen_row_data & 0x80) {
                put_pixel ((column * 6) + pixel_no, (row * 8) + chargen_row, 1);
            }
            else {
                put_pixel ((column * 6) + pixel_no, (row * 8) + chargen_row, 0);
            }
            chargen_row_data <<= 1;
        }
        chargen_ptr++;
    }
}

void put_pixel (unsigned int xpos, unsigned int ypos, unsigned int on_off)

{
    unsigned char *ptr;
    unsigned int bit_number;

    if ((xpos >= X_PIXELS) || (ypos >= Y_PIXELS)) {
        return;
    }

    ptr = oled_graphics_buffer + ((ypos * X_PIXELS) / 8);

    ptr += (xpos / 8);
    bit_number = xpos % 8;

    if (on_off) {
        *ptr |= reverse_bits_table [bit_number];
    }
    else {
        *ptr &= ~reverse_bits_table [bit_number];
    }
}

