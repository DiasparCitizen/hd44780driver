/*
 
HD44780 microaddict library 1.0
Copyright (C) 2017 Ismael García-Marlowe

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

*/

#include <string.h>
#define F_CPU 16000000
#include <util/delay.h>
#include <asf.h>

#ifndef LCD4BITS_H_
#define LCD4BITS_H_

/************************************************************/
/************** CUSTOMIZE HERE ******************************/

/// Data
#define LCD_DATA_PORT            PORTC    // Select port where LCD data lines are connected
#define LCD_DATA_PORT_CONFIG     DDRC
#define LCD_DATA_PORT_LSB        0        // Select port bit where the first LCD data line is connected
/// Enable
#define LCD_E_PORT               PORTC    // Select port where Enable line is connected
#define LCD_E_PORT_CONFIG        DDRC
#define LCD_E_BIT                4        // Select port bit where Enable line is connected
/// RS
#define LCD_RS_PORT              PORTC    // Select port where RS line is connected
#define LCD_RS_PORT_CONFIG       DDRC
#define LCD_RS_BIT               6        // Select port bit where RS line is connected



#define TEXT_UNITS_AMT           2        // Select how many text units can be memorized by LCD module
#define TEXT_UNIT_MAX_LEN        30       // Select a maximum length for the text units to be memorized

/************************************************************/
/************************************************************/

/// DEFINITIONS

#define LCD_DATA_MASK            (0x0F << LCD_DATA_PORT_LSB)
#define LCD_E_MASK               (0x01 << LCD_E_BIT)
#define LCD_RS_MASK              (0x01 << LCD_RS_BIT)

#define SET_ADDRESS              0x80
#define CMD_DISP_RIGHT           0x1C
#define CMD_DISP_LEFT            0x18
#define CMD_CURSOR_RIGHT         0x14
#define CMD_CURSOR_LEFT          0x10
#define CMD_DISP_CLEAR           0x01
#define CMD_RET_HOME             0x02
#define CMD_DISP_ON              0x0C
#define CMD_DISP_OFF             0x08
#define CURSOR_BIT               0x02
#define CURSORBLINK_BIT          0x01

#define L1_START                 0x00
#define L2_START                 0x40
#define RIGHT                    0
#define LEFT                     1
#define JUMP                     1

#define TRUE                     0x01
#define FALSE                    0x00

#define ROW_LENGTH               40
#define DISPLAY_WIDTH            16


/// TYPE DEFINITIONS

typedef unsigned char byte;
typedef struct {
    byte start_address;
    byte size;
    byte window_size;
    byte jump;
    byte offset;
} unit_tag;
typedef unit_tag unit_tags[TEXT_UNITS_AMT];
typedef char text_units[TEXT_UNITS_AMT][TEXT_UNIT_MAX_LEN];


/// MACROS

#define _set_data(data)       LCD_DATA_PORT = ( LCD_DATA_PORT & ~LCD_DATA_MASK ) | ( (data << LCD_DATA_PORT_LSB) & LCD_DATA_MASK )
#define _set_EN_to_1()        LCD_E_PORT = LCD_E_PORT | LCD_E_MASK
#define _set_EN_to_0()        LCD_E_PORT = LCD_E_PORT & ~LCD_E_MASK
#define _set_RS_to_1()        LCD_RS_PORT = LCD_RS_PORT | LCD_RS_MASK
#define _set_RS_to_0()        LCD_RS_PORT = LCD_RS_PORT & ~LCD_RS_MASK

#define _ini_wait1()          _delay_ms(20)   // Wait fore more than 15 ms
#define _ini_wait2()          _delay_ms(5)    // Wait for more than 4.1 ms
#define _ini_wait3()          _delay_us(500)  // Wait for more than 100 us
#define _ini_wait4()          _delay_ms(50)
#define _exec_wait()          _delay_ms(2)
#define _ena_wait1()          _delay_us(30)
#define _ena_wait2()          _delay_us(15)


/// PROTOTYPES

/******************************************************************************
 * Summary:         Initializes LCD in 4 bit mode.
 *
 * Input:           byte cursor_on   :    Set to bit_CURSOR to activate cursor, or
 *                                        to 0 to deactivate it.
 *                  byte blink_on    :    Set to bit_CURSORBLINK to activate cursor
 *                                        blink, or to 0 to keep it deactivated.
 *****************************************************************************/
void initialize_lcd(byte cursor_on, byte blink_on);

/******************************************************************************
 * Summary:            Resets LCD
 *****************************************************************************/
void reset_lcd(byte cursor_on, byte blink_on);

/******************************************************************************
 * Summary:            Clears all data on screen.
 *****************************************************************************/
void clear_screen(void);

/******************************************************************************
 * Summary:         Sets parameter "address" into LCD's address counter.
 *
 * Input:           byte address     :    Parameter address is a permitted DDRAM address.
 *                                        To place the cursor on the first position of either the
 *                                        first or the second line, set parameter to defined values
 *                                        L1_START or L2_START respectively. Choose concrete positions
 *                                        in either line by adding an offset, as in L1_START+offset.
 *****************************************************************************/
void gotoaddress(byte address);

/******************************************************************************
 * Summary:         Returns both display and cursor to the original position (address 0).
 *****************************************************************************/
void gohome(void);

/******************************************************************************
 * Summary:         Turns display and cursor on or off.
 *
 * Input:           byte on         :    Set to zero in order to turn off display, or to
 *                                       any other value to turn it on.
 *****************************************************************************/
void switch_display(byte on);

/******************************************************************************
 * Summary:         Cursor is moved right or left.
 *
 * Input:           byte times    :   The cursor will be moved right or left a
 *                                    maximum of 255 times.
 *****************************************************************************/
void move_cursor_right(byte times);
void move_cursor_left(byte times);

/******************************************************************************
 * Summary:         Display will be shifted left or right a total of "times" times.
 *                  Text will remain in place.
 *
 * Input:           byte times    :    The cursor will be moved right or left a
 *                                     maximum of 255 times.
 *****************************************************************************/
void move_screen_left(byte times);
void move_screen_right(byte times);

/******************************************************************************
 * Summary:         SCREEN_WIDTH space chars will be written starting on start_address.
 *
 * Input:           byte start_address    :   Address starting on which a whole line will
 *                                            be erased.
 *****************************************************************************/
void erase_line(byte start_address);

/******************************************************************************
 * Summary:         This function will write str[] on display. Unrecognized
 *                  chars will be shown as "#".
 *
 * Input:           char str[]            :    Array of chars to be written on display.
 *                  byte start_address    :    Address of str[0].
 *                  byte jump             :    If jump == 1 and length of str[] is greater than
 *                                             display width, str[] will continue on row 2 if
 *                                             "start_address" was on row 1, and vice versa. Also,
 *                                             if jump == 1, only SCREENWIDTH*2 chars will be
 *                                             written.
 *****************************************************************************/
void write_text(char str[], byte start_address, byte jump);

/******************************************************************************
 * Summary:         Writes a char at the position indicated by the current address.
 *
 * Input:           ch                   :    The char to be written.
 *****************************************************************************/
void write_char(char ch);

/******************************************************************************
 * Summary:          This function will register str[] as a logical text unit in the "units" array.
 *                   A numerical value will be returned containing the newly created logical text unit's tag.
 *                   This tag can be used in other functions to apply changes to the logical text
 *                   unit identified by it.
 *
 * Input:            char str[]          :    Array of chars to be written on display.
 *                   byte window_size    :    Size of the character window inside of which str[] will
 *                                            be shown. Set to 0 in order to equate the size of the
 *                                            window to the length of str[]. If window_size > 0, then
 *                                            only window_size chars of str[] will be displayed.
 *                   byte jump           :    If jump == 1 and length of str[] is greater than
 *                                            DISPLAY_WIDTH, str[] will continue on row 2 if
 *                                            "start_address" was on row 1, and vice versa. Also,
 *                                            if jump == 1, only SCREENWIDTH*2 chars will be
 *                                            written.
 * Output:           byte tag            :    This tag is the index of the newly created logical text unit's
 *                                            position inside the array of tags "units".
 *****************************************************************************/
byte register_text_unit(char str[], byte window_size, byte jump);

/******************************************************************************
 * Summary:         This function will show the text identified by "tag" starting on "start_address".
 *
 * Input:           byte tag                  :    Identifies a logical text unit.
 *                  byte start_address        :    Address starting on which the array of chars will be written.
 *****************************************************************************/
void write_text_unit(byte tag, byte start_address);

/******************************************************************************
 * Summary:         This function will move the text identified by "tag" to "start_address".
 *
 * Input:           byte tag                  :    Identifies a logical text unit.
 *                  byte start_address        :    Address starting on which the array of chars will be written.
 *****************************************************************************/
void move_text_unit(byte tag, byte start_address);

/******************************************************************************
 * Summary:         This function will circularly shift the logical text unit identified
 *                  by "tag".
 *
 * Input:           byte tag              :    Identifies a text unit.
 *                  byte direction        :    Input "RIGHT" for a right shift, "LEFT" for a left shift.
 *                  byte stride           :    The text unit identified by "tag" will be shifted "stride" positions.
 *****************************************************************************/
void rotate_text_unit(byte tag, byte direction, byte stride);

/******************************************************************************
 * Summary:         Erases or rewrites the logical text unit identified by "tag".
 *                  This function is useful for generating a blinking effect.
 *
 * Input:           byte tag             :    Identifies a text unit.
 *                  byte on              :    Set to "1" to rewrite text. Set to "0" to erase.
 *****************************************************************************/
void toggle_text_unit(byte tag, byte on);

/******************************************************************************
 * Summary:         Returns the logical text unit identified by "tag" to its initial
 *                  position.
 *
 * Input:           byte tag             :    Identifies a text unit.
 *****************************************************************************/
void return_text_unit_to_initial_position(byte tag);


/******************************************************************************
 * Summary:           Overwrites the logical text unit identified by "tag" with the chars
 *                    contained in the "chars" array at the offsets specified in the "offsets"
 *                    array.
 *
 * Input:             byte tag            :    Identifies a text unit.
 *****************************************************************************/
void replace_chars_in_text_unit(byte tag, byte *offsets, char *chars, byte num_offsets);

#endif /* LCD4BITS_H_ */