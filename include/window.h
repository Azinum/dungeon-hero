// window.h

#include <X11/keysym.h>

enum key_codes {
  KEY_SPACE         = XK_space,
  KEY_APOSTROPHE    = XK_apostrophe,
  KEY_COMMA         = XK_comma,
  KEY_MINUS         = XK_minus,
  KEY_PERIOD        = XK_period,
  KEY_SLASH         = XK_slash,

  KEY_0             = XK_0,
  KEY_1             = XK_1,
  KEY_2             = XK_2,
  KEY_3             = XK_3,
  KEY_4             = XK_4,
  KEY_5             = XK_5,
  KEY_6             = XK_6,
  KEY_7             = XK_7,
  KEY_8             = XK_8,
  KEY_9             = XK_9,

  KEY_SEMICOLON     = XK_semicolon,
  KEY_EQUAL         = XK_equal,

  KEY_A             = XK_a,
  KEY_B             = XK_b,
  KEY_C             = XK_c,
  KEY_D             = XK_d,
  KEY_E             = XK_e,
  KEY_F             = XK_f,
  KEY_G             = XK_g,
  KEY_H             = XK_h,
  KEY_I             = XK_i,
  KEY_J             = XK_j,
  KEY_K             = XK_k,
  KEY_L             = XK_l,
  KEY_M             = XK_m,
  KEY_N             = XK_n,
  KEY_O             = XK_o,
  KEY_P             = XK_p,
  KEY_Q             = XK_q,
  KEY_R             = XK_r,
  KEY_S             = XK_s,
  KEY_T             = XK_t,
  KEY_U             = XK_u,
  KEY_V             = XK_v,
  KEY_W             = XK_w,
  KEY_X             = XK_x,
  KEY_Y             = XK_y,
  KEY_Z             = XK_z,

// TODO(lucas): Moarrr key codes!
  MAX_KEY = 255,
};
