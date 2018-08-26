#include <Arduino.h>
#include <SPI.h>

#define RED 0xf800
#define GREEN 0x07e0
#define BLUE 0x001f
#define BLACK 0x0000
#define YELLOW 0xffe0
#define WHITE 0xffff

#define TFT_MIN_X 0
#define TFT_MIN_Y 0
#define TFT_MAX_X 239
#define TFT_MAX_Y 319

inline void tft_dc_low(void) {
  digitalWrite(6, LOW);
}

inline void tft_dc_high(void) {
  digitalWrite(6, HIGH);
}

inline void tft_cs_low(void) {
  digitalWrite(5, LOW);
}

inline void tft_cs_high(void) {
  digitalWrite(5, HIGH);
}

inline void _send_cmd(unsigned char index) {
  tft_dc_low();
  tft_cs_low();
  SPI.transfer(index);
  tft_cs_high();
}

inline void _write_data(unsigned char data) {
  tft_dc_high();
  tft_cs_low();
  SPI.transfer(data);
  tft_cs_high();
}

inline void _send_data(unsigned short data) {
  unsigned char head = data >> 8;
  unsigned char tail = data & 0xff;

  tft_dc_high();
  tft_cs_low();

  SPI.transfer(head);
  SPI.transfer(tail);
  
  tft_cs_high();
}

void tft_set_col(unsigned short start, unsigned short e) {
  _send_cmd(0x2A);
  _send_data(start);
  _send_data(e);
}

void tft_set_page(unsigned short start, unsigned short e) {
  _send_cmd(0x2B);
  _send_data(start);
  _send_data(e);
}

void tft_set_x_y(unsigned short pos_x, unsigned short pos_y) {
  tft_set_col(pos_x, pos_x);
  tft_set_page(pos_y, pos_y);
  
  _send_cmd(0x2c);
}

void tft_set_pixel(unsigned short pos_x, unsigned short pos_y, unsigned short colour) {
  tft_set_x_y(pos_x, pos_y);
  _send_data(colour);
}

void tft_fill_screen(void) {
  tft_set_col(0, 239);
  tft_set_page(0, 319);

  _send_cmd(0x2c);

  tft_dc_high();
  tft_cs_low();

  for (unsigned short i = 0; i < 38400; i++) {
      SPI.transfer(0);
      SPI.transfer(0);
      SPI.transfer(0);
      SPI.transfer(0);
  }

  tft_cs_high();
}

void tft_fill_screen(unsigned short xl, unsigned short xr, unsigned short yu, unsigned short yd, unsigned short colour) {
  unsigned long xy = 0;

  if (xl > xr) {
    xl = xl ^ xr;
    xr = xl ^ xr;
    xl = xl ^ xr;
  }

  if (yu > yd) {
    yu = yu ^ yd;
    yd = yu ^ yd;
    yu = yu ^ yd;
  }

  xl = constrain(xl, TFT_MIN_X, TFT_MAX_X);
  xr = constrain(xr, TFT_MIN_X, TFT_MAX_X);
  
  yu = constrain(yu, TFT_MIN_Y, TFT_MAX_Y);
  yd = constrain(yd, TFT_MIN_Y, TFT_MAX_Y);

  xy = (xr - xl + 1);
  xy = xy * (yd - yu + 1);

  tft_set_col(xl, xr);
  tft_set_page(yu, yd);

  _send_cmd(0x2c);

  tft_dc_high();
  tft_cs_low();

  unsigned char h_colour = (colour >> 8) & 0xff;
  unsigned char l_colour = colour & 0xff;

  for (unsigned long i = 0; i < xy; i++) {
    SPI.transfer(h_colour);
    SPI.transfer(l_colour);
  }

  tft_cs_high();
}

void tft_setup(void) {
  SPI.begin();

  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(5, LOW);
  digitalWrite(7, HIGH);
  
  tft_cs_high();
  tft_dc_high();

  delay(500);
  _send_cmd(0x01);
  delay(200);

  // ===============
  _send_cmd(0xCF);
  _write_data(0x00);
  _write_data(0x8B);
  _write_data(0X30);
  // ===============

  // ===============
  _send_cmd(0xED);
  _write_data(0x67);
  _write_data(0x03);
  _write_data(0X12);
  _write_data(0X81);
  // ===============

  // ===============
  _send_cmd(0xE8);
  _write_data(0x85);
  _write_data(0x10);
  _write_data(0x7A);
  // ===============

  // ===============
  _send_cmd(0xCB);
  _write_data(0x39);
  _write_data(0x2C);
  _write_data(0x00);
  _write_data(0x34);
  _write_data(0x02);
  // ===============

  // ===============
  _send_cmd(0xF7);
  _write_data(0x20);
  // ===============

  // ===============
  _send_cmd(0xEA);
  _write_data(0x00);
  _write_data(0x00);
  // ===============

  // ===============
  _send_cmd(0xC0); // _power_control_1();
  _write_data(0x1B);
  // ===============

  // ===============
  _send_cmd(0xC1); // _power_control_2();
  _write_data(0x10);
  // ===============

  // ===============
  _send_cmd(0xC5); // _vcm_control();
  _write_data(0x3F);
  _write_data(0x3C);
  // ===============

  // ===============
  _send_cmd(0xC7); // _vcm_control_2();
  _write_data(0XB7);
  // ===============

  // ===============
  _send_cmd(0x36); // _memory_access_control();
  _write_data(0x08);
  // ===============

  // ===============
  _send_cmd(0x3A);
  _write_data(0x55);
  // ===============

  // ===============
  _send_cmd(0xB1);
  _write_data(0x00);
  _write_data(0x1B);
  // ===============

  // ===============
  _send_cmd(0xB6); // _display_function_control();
  _write_data(0x0A);
  _write_data(0xA2);
  // ===============

  // ===============
  _send_cmd(0xF2); // _gamma_function_disable();
  _write_data(0x00);
  // ===============

  // ===============
  _send_cmd(0x26); // _gamma_curve_selected();
  _write_data(0x01);
  // ===============

  // ===============
  _send_cmd(0xE0); // _set_gamma_2();
  _write_data(0x0F);
  _write_data(0x2A);
  _write_data(0x28);
  _write_data(0x08);
  _write_data(0x0E);
  _write_data(0x08);
  _write_data(0x54);
  _write_data(0XA9);
  _write_data(0x43);
  _write_data(0x0A);
  _write_data(0x0F);
  _write_data(0x00);
  _write_data(0x00);
  _write_data(0x00);
  _write_data(0x00);
  // ===============

  // ===============
  _send_cmd(0XE1); // _set_gamma_1();
  _write_data(0x00);
  _write_data(0x15);
  _write_data(0x17);
  _write_data(0x07);
  _write_data(0x11);
  _write_data(0x06);
  _write_data(0x2B);
  _write_data(0x56);
  _write_data(0x3C);
  _write_data(0x05);
  _write_data(0x10);
  _write_data(0x0F);
  _write_data(0x3F);
  _write_data(0x3F);
  _write_data(0x0F);
  // ===============

  // ===============
  _send_cmd(0x11); // _exit_sleep();
  // ===============
  
  delay(120);

  // ===============
  _send_cmd(0x29); // _display_on();
  // ===============

  tft_fill_screen();
}

void setup() {
  tft_setup();
  tft_fill_screen(80, 160, 50, 150, RED);
}

void loop() {

}
