/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "quantum.h"

#ifdef SWAP_HANDS_ENABLE
__attribute__((weak)) const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
    // Left
    {{0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}},
    {{0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}},
    {{0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}},
    {{0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}},
    // Right
    {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}},
    {{0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}},
    {{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}},
    {{0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}}
};
#endif

#ifdef OLED_ENABLE

oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        return OLED_ROTATION_180; // flips the display 180 degrees if offhand
    }
    return rotation;
}

static void oled_render_layer_state(void) {
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state)) {
        case 0:
            oled_write_ln_P(PSTR("Default"), false);
            break;
        case 1:
            oled_write_ln_P(PSTR("Lower"), false);
            break;
        case 2:
            oled_write_ln_P(PSTR("Raise"), false);
            break;
        case 3:
            oled_write_ln_P(PSTR("Adjust"), false);
            break;
        default:
            oled_write_ln_P(PSTR("Undef"), false);
            break;
    }
}

char     key_name = ' ';
uint16_t last_keycode;
uint8_t  last_row;
uint8_t  last_col;

static const char PROGMEM code_to_name[60] = {' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'R', 'E', 'B', 'T', '_', '-', '=', '[', ']', '\\', '#', ';', '\'', '`', ',', '.', '/', ' ', ' ', ' '};

static void set_keylog(uint16_t keycode, keyrecord_t *record) {
    key_name     = ' ';
    last_keycode = keycode;
    if (IS_QK_MOD_TAP(keycode)) {
        if (record->tap.count) {
            keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
        } else {
            keycode = 0xE0 + biton(QK_MOD_TAP_GET_MODS(keycode) & 0xF) + biton(QK_MOD_TAP_GET_MODS(keycode) & 0x10);
        }
    } else if (IS_QK_LAYER_TAP(keycode) && record->tap.count) {
        keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
    } else if (IS_QK_MODS(keycode)) {
        keycode = QK_MODS_GET_BASIC_KEYCODE(keycode);
    } else if (IS_QK_ONE_SHOT_MOD(keycode)) {
        keycode = 0xE0 + biton(QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0xF) + biton(QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0x10);
    }
    if (keycode > ARRAY_SIZE(code_to_name)) {
        return;
    }

    // update keylog
    key_name = pgm_read_byte(&code_to_name[keycode]);
    last_row = record->event.key.row;
    last_col = record->event.key.col;
}

static const char *depad_str(const char *depad_str, char depad_char) {
    while (*depad_str == depad_char)
        ++depad_str;
    return depad_str;
}

static void oled_render_keylog(void) {
    const char *last_row_str = get_u8_str(last_row, ' ');
    oled_write(depad_str(last_row_str, ' '), false);
    oled_write_P(PSTR("x"), false);
    const char *last_col_str = get_u8_str(last_col, ' ');
    oled_write(depad_str(last_col_str, ' '), false);
    oled_write_P(PSTR(", k"), false);
    const char *last_keycode_str = get_u16_str(last_keycode, ' ');
    oled_write(depad_str(last_keycode_str, ' '), false);
    oled_write_P(PSTR(":"), false);
    oled_write_char(key_name, false);
}

// static void render_bootmagic_status(bool status) {
//     /* Show Ctrl-Gui Swap options */
//     static const char PROGMEM logo[][2][3] = {
//         {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
//         {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
//     };
//     if (status) {
//         oled_write_ln_P(logo[0][0], false);
//         oled_write_ln_P(logo[0][1], false);
//     } else {
//         oled_write_ln_P(logo[1][0], false);
//         oled_write_ln_P(logo[1][1], false);
//     }
// }

__attribute__((weak)) void oled_render_logo(void) {
    // clang-format off

    // static const char PROGMEM crkbd_logo[] = {
    //     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
    //     0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
    //     0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
    //     0};

    static const char PROGMEM arasaka_logo[] = {
        // 'arasaka', 128x32px
        0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xc0, 0x60, 
        0x30, 0x30, 0x18, 0x18, 0x08, 0x08, 0x0c, 0xec, 0xec, 0x0c, 0x0c, 0x08, 0x18, 0x18, 0x10, 0x30, 
        0x60, 0xc0, 0xc0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x01, 0x00, 0x00, 0x38, 0xf8, 0xf0, 0xf0, 0x30, 0x30, 0xf0, 
        0xf0, 0x78, 0x18, 0x00, 0x00, 0x08, 0x18, 0x38, 0x70, 0xe0, 0xc0, 0x80, 0xf8, 0xf8, 0xf0, 0x00, 
        0x18, 0x78, 0xf0, 0xf0, 0x30, 0x30, 0xf0, 0xf0, 0xf8, 0x38, 0x00, 0x00, 0xf0, 0xf8, 0xd8, 0xd8, 
        0xd8, 0x98, 0x98, 0xb8, 0x30, 0x00, 0x18, 0x78, 0xf0, 0xf0, 0x30, 0x30, 0xf0, 0xf0, 0xf8, 0x38, 
        0x00, 0x00, 0xf8, 0xf8, 0xe0, 0x40, 0x40, 0x40, 0x40, 0xe0, 0xf8, 0xf8, 0x00, 0x00, 0x18, 0x78, 
        0xf0, 0xf0, 0x30, 0x30, 0xf0, 0xf0, 0x78, 0x18, 0x00, 0x00, 0xf0, 0x7c, 0x07, 0x81, 0xe0, 0xe0, 
        0xe0, 0xe0, 0xe0, 0xe0, 0xf0, 0x3c, 0x1e, 0xff, 0xff, 0x1e, 0x1c, 0xf8, 0xf0, 0xe0, 0xe0, 0xe0, 
        0xe0, 0xe0, 0xc1, 0x03, 0x1f, 0xf8, 0x00, 0x00, 0x01, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xe0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc3, 0xc7, 0xcf, 0xcf, 0xc7, 
        0xc1, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xcc, 0xce, 0xc7, 0xc3, 0xc1, 0xc1, 0xc7, 0xcf, 0xc7, 0xc0, 
        0xc0, 0xc0, 0xc1, 0xc7, 0xcf, 0xce, 0xc7, 0xc3, 0xc0, 0xc0, 0xc0, 0xc0, 0xc6, 0xc6, 0xcc, 0xcc, 
        0xcc, 0xcc, 0xc7, 0xc7, 0xc7, 0xc0, 0xc0, 0xc0, 0xc1, 0xc7, 0xcf, 0xcf, 0xc7, 0xc3, 0xc0, 0xc0, 
        0xc0, 0xc0, 0xc7, 0xc7, 0xce, 0xcc, 0xcc, 0xcc, 0xcc, 0xce, 0xcf, 0xc7, 0xc0, 0xc0, 0xc0, 0xc0, 
        0xc1, 0xc7, 0xcf, 0xcf, 0xc7, 0xc1, 0xc0, 0xc0, 0xc0, 0xc0, 0xcf, 0xfe, 0xe0, 0xc7, 0x0f, 0x1f, 
        0x1f, 0x1f, 0x1f, 0x0f, 0xe7, 0xf8, 0xf8, 0xff, 0xff, 0xf8, 0xf8, 0xf7, 0x0f, 0x1f, 0x1f, 0x1f, 
        0x1f, 0x1f, 0x8f, 0xe0, 0xf8, 0xdf, 0xc0, 0xc0, 0xe0, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x06, 
        0x06, 0x0c, 0x08, 0x18, 0x19, 0x13, 0x17, 0x17, 0x17, 0x17, 0x13, 0x11, 0x18, 0x08, 0x0c, 0x04, 
        0x06, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // clang-format on
    //oled_write_P(arasaka_logo, false);
    oled_write_raw_P(arasaka_logo, sizeof(arasaka_logo));
}

bool oled_task_kb(void) {
    if (!oled_task_user()) {
        return false;
    }
    if (is_keyboard_master()) {
        oled_render_layer_state();
        oled_render_keylog();
    } else {
        oled_render_logo();
    }
    return false;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        set_keylog(keycode, record);
    }
    return process_record_user(keycode, record);
}
#endif // OLED_ENABLE
