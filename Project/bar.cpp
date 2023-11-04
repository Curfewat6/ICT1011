
#include "bar.h"

void PercentBarHorizontal::tick(float percent_){

  // To keep track
  percent = percent_;

  // Only update bar if percent has changed
  if(percent != last_percent){
    // Draw the outline of the percent bar (always not filled)
    display.drawRect(x, y, width, height, TSRectangleNoFill, outline_color);
    
    // Calculate how much internal bar should be displayed
    int bar_x = (width-2) * percent;
    
    // Handle <= 0 separately since can't give draw rect zero for x
    // Also handle the bar going out of bounds when percent is more than 100%
    if(bar_x > 0 && percent < 1.0){
    
      // Display the internal bar (offset due to outline thickness)
      display.drawRect(x+1, y+1, bar_x, height-2, TSRectangleFilled, bar_color);
      
      // Calculate width of bar that's empty/needs to be cleared
      int clear_width = (width-bar_x);
      
      // Clear/overwrite the empty side of the bar (only do so if there is empty space)
      if (bar_x != width && clear_width-2 > 0){
        display.drawRect(x+bar_x+1, y+1, clear_width-2, height-2, TSRectangleFilled, TS_8b_Black);
      }
    
    }else if(bar_x <= 0){
      // The bar is empty, overwrite the whole thing with black
      display.drawRect(x+1, y+1, width-2, height-2, TSRectangleFilled, TS_8b_Black);
    }else if(percent >= 1.0){
      // The bar is full, overwrite the whole thing with bar color
      display.drawRect(x+1, y+1, width-2, height-2, TSRectangleFilled, bar_color);
    }
  }

  // If true, show the percentage text under the bar (always update text)
  if(show_percentage && percent >= 0){
    display.setFont(thinPixel7_10ptFontInfo);

    // Hard offset of 15 is fine, percent character size doesn't change
    display.setCursor(x+(width/2)-15, y+height+13);
    display.fontColor(font_color, TS_8b_Black);
    display.print(String(percent * 100) + "% ");
  }
}

