uint8_t msg0[36] = {0xdd, 0x03, 0x00, 0x1d, 0x12, 0x6a, 0x00, 0x00, 0x08, 0x97, 
  0x13, 0x88, 0x00, 0x00, 0x2d, 0x53, 0x00, 0x00, 0x00 ,0x00, 0x00, 0x00, 0x22, 0x2c, 0x01,
  0x0D, 0x03, 0x0b, 0xb4, 0x0b, 0xa8, 0x0b, 0xa0, 0xfb, 0x31, 0x77};
uint8_t msg1[37] = {0xDD, 0x04, 0x00, 0x1E, 0x0F, 0x66, 0x0F, 0x63, 0x0F, 0x63, 
  0x0F, 0x64, 0x0F, 0x3E, 0x0F, 0x63, 0x0F, 0x37, 0x0F, 0x5B, 0x0F, 0x65, 0x0F, 0x3B, 0x0F,
  0x63, 0x0F, 0x63, 0x0F, 0x3C, 0x0F, 0x66, 0x0F, 0x3D, 0xF9, 0xF9, 0x77};
uint8_t msg2[36] = {0xdd, 0x03, 0x00, 0x1d, 0x12, 0x3e, 0x00, 0x00, 0x08, 0x68, 
  0x13, 0x88, 0x00, 0x00, 0x2d, 0x53, 0x00, 0x00, 0x00 ,0x00, 0x00, 0x00, 0x22, 0x2c, 0x01,
  0x0D, 0x03, 0x0b, 0xb2, 0x0b, 0xab, 0x0b, 0xa0, 0xfb, 0x8b, 0x77};
uint8_t msg3[37] = {0xDD, 0x04, 0x00, 0x1E, 0x0F, 0x66, 0x0F, 0x58, 0x0F, 0x3F, 
  0x0F, 0x62, 0x0F, 0x5B, 0x0F, 0x63, 0x0F, 0x37, 0x0F, 0x5B, 0x0F, 0x65, 0x0F, 0x3B, 0x0F,
  0x63, 0x0F, 0x63, 0x0F, 0x3C, 0x0F, 0x66, 0x0F, 0x3D, 0xfa, 0x0d, 0x77};
uint8_t msg_flag = 0;
uint8_t* messages[4] = {reinterpret_cast<uint8_t*>(&msg0), 
  reinterpret_cast<uint8_t*>(&msg1), 
  reinterpret_cast<uint8_t*>(&msg2), 
  reinterpret_cast<uint8_t*>(&msg3)};

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long cnt = millis();
  while (millis() - cnt < 500);
  for (int i = 0; i < ((msg_flag % 2) ? 37 : 36); ++i)
  {
    Serial.write(messages[msg_flag][i]);
  }
  msg_flag == 3 ? msg_flag = 0 : ++msg_flag;
}
