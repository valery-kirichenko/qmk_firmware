/* Copyright 2023 Valery Kirichenko <github@valery.dev>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

void led_init_ports(void) {
    setPinOutput(C14); // Top - Caps Lock LED
    setPinOutput(C15); // Middle - Colemak Layer LED
    setPinOutput(C0); // Bottom - Colemak Layer Active status
};

bool led_update_user(led_t led_state) {
    writePin(C14, led_state.caps_lock);
    // writePin(C15, led_state.caps_lock);
    // writePin(C0, led_state.caps_lock);
    return true;
}

void keyboard_post_init_user(void) {
  debug_enable = true;
}

enum custom_layers {
    MAIN,
    COLEMAK,
    FN,
};

enum custom_keycodes {
    DASH = SAFE_RANGE,
    CLMK_TGL,
    CLMK_SWTCH,
};

static bool colemak_active = false;

layer_state_t layer_state_set_user(layer_state_t state) {
    writePin(C15, layer_state_cmp(state, COLEMAK));
    return state;
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint8_t mods_state = get_mods();
    switch (keycode) {
    case DASH:
        if (!record->event.pressed) break;
        if (mods_state & MOD_BIT(KC_LSFT)) {
            unregister_code(KC_LSFT);
            SEND_STRING(SS_LALT(SS_TAP(X_KP_0) SS_TAP(X_KP_1) SS_TAP(X_KP_5) SS_TAP(X_KP_1)));
            break;
        }
        if (mods_state & MOD_BIT(KC_RSFT)) {
            unregister_code(KC_RSFT);
            SEND_STRING(SS_LALT(SS_TAP(X_KP_0) SS_TAP(X_KP_1) SS_TAP(X_KP_5) SS_TAP(X_KP_1)));
            break;
        }
        SEND_STRING(SS_LALT(SS_TAP(X_KP_0) SS_TAP(X_KP_1) SS_TAP(X_KP_5) SS_TAP(X_KP_0)));
        break;
    case CLMK_TGL:
        if (!colemak_active || !(mods_state & MOD_MASK_ALT)) {
            if (record->event.pressed) {
                register_mods(MOD_BIT(KC_LSFT));
            } else {
                unregister_mods(MOD_BIT(KC_LSFT));
            }
            break;
        } 
        if (record->event.pressed) {
            register_mods(MOD_BIT(KC_LSFT));
            layer_invert(COLEMAK);
        } else {
            unregister_mods(MOD_BIT(KC_LSFT));
        }
        break;
    case CLMK_SWTCH:
        if (record->event.pressed) {
            #ifdef CONSOLE_ENABLE
                uprintf("Layer state: %d, var state: %d\n", layer_state_is(COLEMAK), colemak_active);
            #endif
            if (!colemak_active || layer_state_is(COLEMAK)) {
                layer_invert(COLEMAK);
            }
            colemak_active = !colemak_active;
            writePin(C0, colemak_active);
        }
    }
    return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[MAIN] = LAYOUT(
    KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_PSCR,  KC_DEL,
    KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_HOME,
    KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_END,
    KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,   KC_PGUP,
    CLMK_TGL, KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,            KC_UP,    KC_PGDN,
    KC_LCTL,  KC_LGUI,  KC_LALT,                      KC_SPC,                                 KC_LALT,  MO(FN),   KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT
),

[COLEMAK] = LAYOUT(
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
    _______,  KC_Q,     KC_W,     KC_F,     KC_P,     KC_G,     KC_J,     KC_L,     KC_U,     KC_Y,     KC_SCLN,  _______,  _______,  _______,  _______,
    _______,  KC_A,     KC_R,     KC_S,     KC_T,     KC_D,     KC_H,     KC_N,     KC_E,     KC_I,     KC_O,     _______,            _______,  _______,
    CLMK_TGL, KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_K,     KC_M,     _______,  _______,  _______,  _______,            _______,  _______,
    _______,  _______,  _______,                      _______,                                _______,  _______,  _______,  _______,  _______,  _______
),

[FN] = LAYOUT(
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,  KC_INS,
    _______,  RGB_TOG,  RGB_MOD,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  DASH,     _______,  _______,  _______,
    _______,  RGB_HUI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  QK_BOOT,  _______,
    _______,  RGB_VAI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,
    _______,  RGB_SPI,  _______, CLMK_SWTCH,_______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,  _______,            G(KC_UP), _______,
    _______,  _______,  _______,                      KC_MPLY,                                KC_RALT,  _______,  _______,G(KC_LEFT), G(KC_DOWN), G(KC_RGHT)
),

};
