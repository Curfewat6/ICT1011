
void nameTag(uint8_t buttons){
  currentDisplayState = displayTimer;



  if(!buttons){
    displayBuffer.clearWindow(0, 8, 96, 64);
    displayBuffer.fontColor(TS_16b_White, TS_16b_Black);
    displayBuffer.setCursor(8, 15 - 6);
    displayBuffer.print("Back");
    displayBuffer.drawLine(1, 14,    1, 12, 0xFFFF);
    displayBuffer.drawLine(1, 12,    6, 12, 0xFFFF);
    backArrow();
    
  }else{
    buttonStuff(backButton);
  }
}
void testing(){
  displayBuffer.fontColor(TS_16b_White, TS_16b_Black);
  displayBuffer.setCursor(8, 30 - 6);
  displayBuffer.print("working");
}