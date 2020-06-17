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

#include "lcd4bits.h"

byte        last_unit_available;
text_units  units;
unit_tags   tags;

void enable(void);
void exec_instruction(byte inst);


void initialize_lcd(byte cursor_on, byte blink_on){

    LCD_DATA_PORT_CONFIG    |= LCD_DATA_MASK;
    LCD_E_PORT_CONFIG       |= LCD_E_MASK;
    LCD_RS_PORT_CONFIG      |= LCD_RS_MASK;

    reset_lcd(cursor_on, blink_on);

}

void reset_lcd(byte cursor_on, byte blink_on){

    byte aux;

    // Wait longer than 15ms
    _ini_wait1();

    // FIRST GO
    _set_data(0b00000011);
    enable();

    // Wait at least 4.1 ms
    _ini_wait2();

    // SECOND GO
    _set_data(0b00000011);
    enable();

    // Wait 100 us
    _ini_wait3();

    // THIRD GO
    _set_data(0b00000011);
    enable();

    // FUNCTION SET
    _set_data(0b00000010);
    enable();

    // FUNCION SET 2
    _set_data(0b00000010);
    enable();
    _set_data(0b00001000);
    enable();

    // Display ON/OFF control
    _set_data(0b00000000);
    enable();

    aux = 0b00001100;
    if (cursor_on) aux |= CURSOR_BIT;
    if (blink_on) aux |= CURSORBLINK_BIT;
    _set_data(aux);
    enable();

    // Entry mode set
    _set_data(0b00000000);
    enable();
    _set_data(0b00000110);
    enable();

    // Wait
    _ini_wait4();

}

void clear_screen(void){
    exec_instruction(CMD_DISP_CLEAR);
}

void write_char(char ch){

    // Prepare data transfer
    _set_RS_to_1();

    // 2nd half
    _set_data(ch >> 4);
    enable();

    // 1st half
    _set_data(ch);
    enable();

    // Disable data transfer
    _set_RS_to_0();

}

void exec_instruction(byte inst){

    // 2nd half
    _set_data(inst >> 4);
    enable();

    // 1st half
    _set_data(inst);
    enable();

    // Wait 10 ms
    _exec_wait();

}

void enable(void){
    _set_EN_to_1();
    _ena_wait1();
    _set_EN_to_0();
    _ena_wait2();
}

void gotoaddress(byte address){
    exec_instruction(address | SET_ADDRESS);
}

void gohome(void){
    exec_instruction(CMD_RET_HOME);
}

void switch_display(byte on){
    if (on) exec_instruction(CMD_DISP_ON);
    else exec_instruction(CMD_DISP_OFF);
}

void move_cursor_right(byte times){
    byte i;
    for (i = 0; i < times; i++)
        exec_instruction(CMD_CURSOR_RIGHT);
}

void move_cursor_left(byte times){
    byte i;
    for (i = 0; i < times; i++)
        exec_instruction(CMD_CURSOR_LEFT);
}

void move_screen_left(byte times){
    byte i;
    for (i = 0; i < times; i++)
        exec_instruction(CMD_DISP_RIGHT);
}

void move_screen_right(byte times){
    byte i;
    for (i = 0; i < times; i++)
        exec_instruction(CMD_DISP_LEFT);
}

void erase_line(byte start_address){
    byte i;
    gotoaddress(start_address);
    for (i = 0; i < DISPLAY_WIDTH; i++)
        write_char(' ');
    gotoaddress(start_address);
}

void write_text(char str[], byte start_address, byte jump){

    byte len, i, next_address;
    len = strlen(str);

    gotoaddress(start_address);

    if (!jump){

        for (i = 0; i < len; i++)
			write_char(str[i]);

    }else{

        if (len > DISPLAY_WIDTH*2)
			len = DISPLAY_WIDTH*2;

        next_address = start_address >= L2_START ? start_address - L1_START : start_address + L2_START;

        for (i = 0; i < len; i++){
            if (i == DISPLAY_WIDTH) gotoaddress(next_address);
            write_char(str[i]);
        }

    }

}

byte register_text_unit(char str[], byte window_size, byte jump){

    byte i, len;

    len = strlen(str);
    if (window_size <= 0 || window_size > len) window_size = len;

    // Register text unit
    tags[last_unit_available].start_address   = L1_START;
    tags[last_unit_available].size            = len;
    tags[last_unit_available].window_size     = window_size;
    tags[last_unit_available].offset          = 0;
    tags[last_unit_available].jump            = jump;

    // Write text unit into text_units matrix
    for (i = 0; i < len && i < TEXT_UNIT_MAX_LEN; i++)
        units[last_unit_available][i] = str[i];

    last_unit_available = (last_unit_available + 1)%TEXT_UNITS_AMT;
    return last_unit_available-1;

}

void write_text_unit(byte tag, byte start_address){

    byte i, index;
    char aux_array[ROW_LENGTH];

    index = tags[tag].offset;

    for (i = 0; i < tags[tag].window_size; i++){
        aux_array[i] = units[tag][index];
        index = (index+1)%(tags[tag].size);
    }
    aux_array[tags[tag].window_size] = '\0';

    write_text(aux_array, start_address, tags[tag].jump);

}

void move_text_unit(byte tag, byte start_address){

    toggle_text_unit(tag, 0);
    tags[tag].start_address = start_address;
    write_text_unit(tag, start_address);

}

void toggle_text_unit(byte tag, byte on){

    byte i, index;
    char aux_array[ROW_LENGTH];

    index = tags[tag].offset;

    if (on){
        for (i = 0; i < tags[tag].window_size; i++){
            aux_array[i] = units[tag][index];
            index = (index+1)%(tags[tag].size);
        }
    }else{
        for (i = 0; i < tags[tag].window_size; i++)
            aux_array[i] = ' ';
    }

    aux_array[tags[tag].window_size] = '\0';
    write_text(aux_array, tags[tag].start_address, tags[tag].jump);

}

void rotate_text_unit(byte tag, byte direction, byte stride){

    byte size, i, index;
    char aux_array[ROW_LENGTH];

    size = tags[tag].size;
    tags[tag].offset = direction == LEFT ? (tags[tag].offset+stride)%size : (tags[tag].offset+size-1)%size;
    index = tags[tag].offset;

    for (i = 0; i < tags[tag].window_size; i++){
        aux_array[i] = units[tag][index];
        index = (index+1) % size;
    }

    aux_array[tags[tag].window_size] = '\0';
    write_text(aux_array, tags[tag].start_address, tags[tag].jump);

}

void return_text_unit_to_initial_position(byte tag){

    byte size, i, index;
    char aux_array[ROW_LENGTH];

    size = tags[tag].size;
    tags[tag].offset = 0;
    index = tags[tag].offset;

    for (i = 0; i < tags[tag].window_size; i++){
        aux_array[i] = units[tag][index];
        index = (index+1)%size;
    }

    aux_array[tags[tag].window_size] = '\0';
    write_text(aux_array, tags[tag].start_address, tags[tag].jump);

}

void replace_chars_in_text_unit(byte tag, byte *offsets, char *chars, byte num_offsets){

    byte size, i, index, chars_replaced;
    char aux_array[ROW_LENGTH];

    // Rewrite text unit

    size = tags[tag].size;
    index = tags[tag].offset;
    chars_replaced = 0;

    for (i = 0; i < tags[tag].window_size; i++){

        if (chars_replaced < num_offsets && index == offsets[chars_replaced]){
            aux_array[i] = chars[chars_replaced];
            chars_replaced++;
        }else{
            aux_array[i] = units[tag][index];
        }

        index = (index+1) % size;

    }

    aux_array[tags[tag].window_size] = '\0';
    write_text(aux_array, tags[tag].start_address, tags[tag].jump);

}