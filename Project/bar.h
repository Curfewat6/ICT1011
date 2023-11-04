
// For Color definitions
#include "TinyScreen.h"


class PercentBarHorizontal{
public:
    PercentBarHorizontal(TinyScreen display_, int x_, int y_, int width_, int height_, uint8_t outline_color_, uint8_t bar_color_, uint8_t font_color_, float percent_, bool show_percentage_) 
    : display(display_)
    , x(x_)
    , y(y_)
    , width(width_)
    , height(height_)
    , percent(percent_)
    , show_percentage(show_percentage_)
    , outline_color(outline_color_)
    , bar_color(bar_color_)
    , font_color(font_color_)
    {}
    
        // Show the next frame if it is time, otherwise keep track of the tick/speed
    void tick(float percent_);
    
private:
    TinyScreen display;
    
    // Information about the graphic and how it behaves/looks
    int x;
    int y;
    int width;
    int height;
    float percent = 0.0;
    float last_percent = percent;
    bool show_percentage;
    uint8_t outline_color;
    uint8_t bar_color;
    uint8_t font_color;
};