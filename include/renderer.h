// renderer.h

// BGRa
typedef struct color {
  u8 B;
  u8 G;
  u8 R;
  u8 A;
} color;

#define COLOR(R, G, B) (color) {B, G, R, .A = 255}
#define COLORA(R, G, B, ColorA) (color) {B, G, R, .A = ColorA}
