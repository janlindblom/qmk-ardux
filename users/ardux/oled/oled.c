// Copyright (c) 2021 Mike "KemoNine" Crosson
// SPDX-License-Identifier: Apache-2.0

#include QMK_KEYBOARD_H

// //////////
// ARDUX layer ids (needed everywhere)
#include "../layout/layer_ids.h"

#include "oled.h"
#include "logo.h"

uint32_t boot_logo_timer        = 0;
bool     boot_logo_cleared      = false;
char     keylog_str[KEYLOG_LEN] = {0};
uint16_t log_timer              = 0;

// clang-format off
const char code_to_name[60] = {
    ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
     20,  19,  17,  26,  22, '+',  39, 'a',  29, '<',
   '\'', 'o', 'a',  21, ',', '.', '-', 127,  63,  63};
// clang-format on

static void render_icon(void) {
    oled_set_cursor(0, 0);
    oled_write_raw_P(ARDUX_OLED_ICON, sizeof(ARDUX_OLED_ICON));
}

#ifdef ARDUX_BOOT_LOGO
static void render_logo(void) {
    oled_set_cursor(0, 0);
    oled_write_raw_P(boot_abullet, sizeof(boot_abullet));
}
#endif

void add_keylog(uint16_t keycode) {
    if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) || (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX)) {
        keycode = keycode & 0xFF;
    }

    for (uint8_t i = KEYLOG_LEN - 1; i > 0; i--) {
        keylog_str[i] = keylog_str[i - 1];
    }
    if (keycode < 60) {
        keylog_str[0] = code_to_name[keycode];
    }
    keylog_str[KEYLOG_LEN - 1] = 0;

    log_timer = timer_read();
}

void update_log(void) {
    if (timer_elapsed(log_timer) > KEYLOG_TIMEOUT) {
        add_keylog(0);
    }
}

bool process_record_user_oled(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        add_keylog(keycode);
    }
    return true;
}

void render_keylogger_status(uint8_t col, uint8_t line) {
    oled_set_cursor(col, line);
    oled_write(keylog_str, false);
}

void render_keylock_status(void) {
    uint8_t led_usb_state = host_keyboard_leds();
    oled_write_P(PSTR("Lock:"), false);
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("N"), led_usb_state & (1 << USB_LED_NUM_LOCK));
    oled_write_P(PSTR("C"), led_usb_state & (1 << USB_LED_CAPS_LOCK));
    oled_write_ln_P(PSTR("S"), led_usb_state & (1 << USB_LED_SCROLL_LOCK));
}

void render_mod_status(void) {
    uint8_t modifiers = get_oneshot_mods();
    oled_write_P(PSTR("Mods:"), false);
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("S"), (modifiers & MOD_MASK_SHIFT));
    oled_write_P(PSTR("C"), (modifiers & MOD_MASK_CTRL));
    oled_write_P(PSTR("A"), (modifiers & MOD_MASK_ALT));
    oled_write_P(PSTR("G"), (modifiers & MOD_MASK_GUI));
}

static void render_status(void) {
#ifdef ARDUX_OLED_ICON
    oled_set_cursor(0, 5);
#else
    oled_set_cursor(0, 1);
#endif
    switch (get_highest_layer(layer_state)) {
        case LAYER_ID_BASE:
#ifdef ARDUX_SIZE_STD
            oled_write_P(PSTR(" ART "), false);
            oled_write_P(PSTR(" SEY "), false);
#endif
#ifdef ARDUX_SIZE_BIG
            oled_write_P(PSTR("*ART "), false);
            oled_write_P(PSTR(" SEY*"), false);
#endif
#ifdef ARDUX_SIZE_40P
            oled_write_P(PSTR("*ART*"), false);
            oled_write_P(PSTR("*SEY*"), false);
#endif
            break;
        case LAYER_ID_NUMBERS:
            oled_write_P(PSTR(" Num "), false);
            oled_write_P(PSTR(" ber "), false);
            break;
        case LAYER_ID_SYMBOLS:
            oled_write_P(PSTR(" Sym "), false);
            oled_write_P(PSTR(" bol "), false);
            break;
        case LAYER_ID_PARENTHETICALS:
            oled_write_P(PSTR(" Par "), false);
            oled_write_P(PSTR("  en "), false);
            break;
        case LAYER_ID_NAVIGATION:
            oled_write_P(PSTR(" Nav "), false);
            oled_write_P(PSTR("     "), false);
            break;
        case LAYER_ID_MOUSE:
            oled_write_P(PSTR(" Mou "), false);
            oled_write_P(PSTR("  se "), false);
            break;
        case LAYER_ID_CUSTOM:
            oled_write_P(PSTR(" Cus "), false);
            oled_write_P(PSTR(" tom "), false);
            break;
#ifdef ARDUX_SIZE_BIG
        case LAYER_ID_BIG_SYM:
            oled_write_P(PSTR("*Sym "), false);
            oled_write_P(PSTR(" bol*"), false);
            break;
        case LAYER_ID_BIG_FUN:
            oled_write_P(PSTR("*Fun "), false);
            oled_write_P(PSTR("  ct*"), false);
            break;
#endif
#ifdef ARDUX_SIZE_40P
        case LAYER_ID_BIG_SYM:
            oled_write_P(PSTR("*Sym*"), false);
            oled_write_P(PSTR("*bol*"), false);
            break;
        case LAYER_ID_BIG_FUN:
            oled_write_P(PSTR("*Fun*"), false);
            oled_write_P(PSTR("* ct*"), false);
            break;
        case LAYER_ID_40P_BASE:
            oled_write_P(PSTR(" 40% "), false);
            oled_write_P(PSTR(" ANSI"), false);
            break;
        case LAYER_ID_40P_FUNCTION:
            oled_write_P(PSTR(" 40% "), false);
            oled_write_P(PSTR(" Fun "), false);
            break;
        case LAYER_ID_40P_NAVIGATION:
            oled_write_P(PSTR(" 40% "), false);
            oled_write_P(PSTR(" Nav "), false);
            break;
#endif
        default:
            oled_write_P(PSTR(" ??? "), false);
            oled_write_P(PSTR(" ??? "), false);
            break;
    }

#ifdef ARDUX_OLED_ICON
    oled_set_cursor(0, 8);
#else
    oled_set_cursor(0, 4);
#endif
    render_keylock_status();
#ifdef ARDUX_OLED_ICON
    oled_set_cursor(0, 11);
#else
    oled_set_cursor(0, 7);
#endif
    render_mod_status();

    // oled_set_cursor(0, 9);
    // oled_write_raw_P(icon_caps_lock_abullet, sizeof(icon_caps_lock_abullet));
    // oled_set_cursor(5, 2);
    // oled_write_P(icon_scroll_lock_abullet, sizeof(icon_scroll_lock_abullet));
    // oled_set_cursor(0, 3);
    // oled_write_P(icon_navigation_abullet, sizeof(icon_navigation_abullet));
    // oled_set_cursor(5, 3);
    // oled_write_P(icon_mouse_abullet, sizeof(icon_mouse_abullet));
    // oled_set_cursor(0, 4);
    // oled_write_P(icon_settings_abullet, sizeof(icon_settings_abullet));
    render_keylogger_status(0, 14);
}

bool oled_task_user(void) {
    // Don't do anything if the display is off
    if (!is_oled_on()) {
        return false;
    }

#ifdef ARDUX_BOOT_LOGO
    if (boot_logo_timer == 0) {
        boot_logo_timer = timer_read32();
    }
    if (timer_elapsed32(boot_logo_timer) < ARDUX_BOOT_LOGO_TIMEOUT) {
        render_logo();
    } else {
        if (!boot_logo_cleared) {
            oled_clear();
            boot_logo_cleared = true;
        }
#endif
        // TODO: See if active layer/mods can be displayed on non master half oled
        if (!is_keyboard_master()) {
            return false;
        }
        update_log();
        render_icon();
        render_status();
        oled_render();
#ifdef ARDUX_BOOT_LOGO
    }
#endif
    return false;
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270;
}
