
void startPong(){
  currentDisplayState = displayPong;
  displayBuffer.clearWindow(0, 0, 96, 64);
  //display.clearScreen();
  getButtons();
 
 // Pausing function of the left button(Is left button equal to the previous value?
  if(display.getButtons(TSButtonUpperLeft))
  {
    newMenu(3);
  }

   
  //Asks if the right button has been pressed before and if it's something else 
  if( rightButton && !preRightButton)
  {
    //If the ball hits the goal, it stops and resets back to the center of the field and gets tossed to start the game
    if(bMovementX == 0)
   {
     drawBall(ballX, ballY, kBallWidth, kBallHeight, BLACK); 
     ballX = 48;
     ballY = 32;
     bMovementX = -1;
     bMovementY = 1;
     
     //When the mode is handball, make sure the right score is reset every time it's selected
     if( mode == kHandballMode)
     {
       rightPlayerScore = 0;
     }
   }
   
   else
   {
    //Full reset, screen clear, score reset, ball reset, and movement reset
    clearScreen();
    leftPlayerScore = 0;
    rightPlayerScore = 0;
    ballX = 48;
    ballY = 32;
    bMovementX = 1;
    bMovementY = 1;
   }
  }
  preRightButton = rightButton;
  
 
   
  // Update the Left Paddle with the new position.
  //
  if( mode != kHandballMode)
  {
    paddleUpdate(LY, kLPaddleX, lPaddleY);
  }
  // Update the Right Paddle with the new position.
  //
  paddleUpdate(LY, kLPaddleX, lPaddleY);
 
 
  ballUpdate( ballX, ballY);
  
  //Score Counter display on the screen
  //
  display.setCursor(60, 3);
  display.print(rightPlayerScore);
  
  if( mode != kHandballMode)
  {
    display.setCursor(36, 3);
    display.print(leftPlayerScore);
  }
  
  displayField();
  // Limit the animation speed.
  //
  delay(0);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Function that clears the screen when called without and weird lines that show up without a delay
void clearScreen()
{
  display.clearWindow(kScreenLeft, kScreenTop, kScreenWidth, kScreenHeight );
  delayMicroseconds(50);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//This function draws the playing field based on which mode is selected
void displayField()
{
   if( mode == kHockeyMode)
  {
    //Yes so draw the goal lines, the center divide, and the top and bottom lines
    display.drawLine(kScreenLeft,kScreenTop,kScreenRight,kScreenTop,TS_8b_White);
    display.drawLine(kScreenLeft,kScreenBottom,kScreenRight,kScreenBottom,TS_8b_White);
    display.drawLine(kScreenWidth/2, kScreenTop, kScreenWidth/2, kScreenBottom, TS_8b_White);
    display.drawLine(kScreenLeft,kScreenTop,kScreenLeft,kScreenBottom/2 - 10,TS_8b_White);
    display.drawLine(kScreenLeft,kScreenBottom,kScreenLeft,kScreenBottom/2 + 10,TS_8b_White);
    display.drawLine(kScreenRight,kScreenTop,kScreenRight,kScreenBottom/2 - 10,TS_8b_White);
    display.drawLine(kScreenRight,kScreenBottom,kScreenRight,kScreenBottom/2 + 10,TS_8b_White);
  }
  
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//This function determines the properties of the paddle and creates its movement
void paddleUpdate(int stickY, int positionX, int& positionY)
{
  // Did the user move the stick enough to move the paddle?
  //
  if( stickY > kStickSensitivity )
  {
    // Yes, so update the position
    //
    positionY = positionY - kPaddleStep;

    // Did the paddle fall off the top of the screen?
    //
    if( positionY < kScreenTop )
    {
      // Yes, so put it at the top of the screen.
      //
      positionY = kScreenTop;
    }

    // Nope, didn't fall off the top of the screen.
    //
    else
    {
      // Erase from the bottom of the paddle by how much we moved up
      //
      drawPaddle(positionX, positionY + kPaddleHeight, kPaddleWidth, kPaddleStep, BLACK);
    }
  } //if( stickY > kStickSensitivity )
  
  
  // Did the user move the stick enough to move the paddle?
  //
  if( stickY < -kStickSensitivity )
  {
    //Is the paddle at the bottom of the screen?
    //
    if( positionY >= kScreenBottom - kPaddleHeight )
    {
      //Yes it is
      positionY = kScreenBottom - kPaddleHeight;
    }
    
    //Draw over the old position of the paddle
    else
    {
    drawPaddle(positionX, positionY, kPaddleWidth, kPaddleStep, BLACK);
    
    positionY = positionY + kPaddleStep;
    }
  }
  
  
  drawPaddle( positionX, positionY, kPaddleWidth, kPaddleHeight, WHITE);
  
} //paddleUpdate(void)


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Code for ball movement
//
void ballUpdate(int& positionX, int& positionY)
{
  int upperGoal;
  int lowerGoal;
  //Draw the ball black
    drawBall(positionX, positionY, kBallWidth, kBallHeight, BLACK);
  
  //The movement on the screen
    positionX = positionX + bMovementX;
    positionY = positionY + bMovementY;
    
    //The ball being drawn in white
    drawBall(positionX, positionY, kBallWidth, kBallHeight, WHITE);
    
   //Will switch between what mode it is and determines the properties of the right wall
    switch(mode)
    {
      //Tennis goal boundaries
      case kTennisMode: 
        upperGoal = kTennisGoalUpper;
        lowerGoal = kTennisGoalLower;
      break;
      //Handball goal boundaries
      case kHandballMode:
        upperGoal = kHandballGoalUpper;
        lowerGoal = kHandballGoalLower;
      break;
      //Hockey goal boundaries
      case kHockeyMode:
        upperGoal = kHockeyGoalUpper;
        lowerGoal = kHockeyGoalLower;
      break;
    }
    
    //Is the ball at the right side of the screen?
    if( positionX >= kScreenRight - kBallWidth )
    {
      //Is the ball in the goal area?
      if( positionY > upperGoal && positionY + kBallHeight < lowerGoal )
      {
        //Is the mode handball and the ball is in the same position as the paddle?
        if( mode != kHandballMode && bMovementX != 0)
        {
          //Yes, so increase the score
          leftPlayerScore++;
        }
        //Stops the ball from moving after missing the paddle
        bMovementX = 0;
        bMovementY = 0;
      }      
     //Reverses the movement
      bMovementX = -bMovementX;
      
    }
   //Goal boundaries changes between games
   switch(mode)
    {
      //Tennis goal boundaries
      case kTennisMode: 
        upperGoal = kTennisGoalUpper;
        lowerGoal = kTennisGoalLower;
      break;
      //Handball Goal boundaries 
      case kHandballMode:
        upperGoal = kHandballGoalUpper;
        lowerGoal = kHandballGoalLower;
      break;
      //Hockey goal boundaries
      case kHockeyMode:
        upperGoal = kHockeyGoalUpper;
        lowerGoal = kHockeyGoalLower;
      break;
    }
     
    //Is the ball at the left side of the screen?
    if( positionX <= kScreenLeft)
    {
      //Is the ball within the goal boundaries?
      if( positionY > upperGoal && positionY + kBallHeight < lowerGoal )
      {
        //Is the mode not handball?
        if( mode != kHandballMode && bMovementX != 0)
        {
          //Then increase right player score
          rightPlayerScore++;
        }
        //Is the mode not handball?      
        if( mode != kHandballMode)
        {
          //Then stop the ball from moving
          bMovementX = 0;
          bMovementY = 0;
        }
      }
      //Yes, reverse direction and increase right's score.
      bMovementX = -bMovementX;
      
    }
    
    // Collisions for the bottom
    
    if( positionY >= kScreenBottom - kBallWidth)
    {
      bMovementY = -bMovementY;
    }
    
    //Collision detection for the top
    
    if( positionY <= kScreenTop)
    {
      bMovementY = -bMovementY;
    }
    
    //Is the ball at the same row as the paddle?
   if( mode != kHandballMode){
    if( positionX == paddleX)
    {
      //Is it at the same position as the paddle?
      if( positionY >= lPaddleY - kBallHeight/2 && positionY <= lPaddleY + kPaddleHeight - kBallHeight/2)
      {
        //Yes so change direction
        bMovementX = -bMovementX;
      }
    }
   }
    //Is the ball at the same row as the paddle?
     if( positionX == kScreenRight - paddleX)
    {
      
      //Is it at the same position as the paddle?
      if( positionY >= rPaddleY - kBallHeight/2 && positionY <= rPaddleY + kPaddleHeight - kBallHeight/2)
      {
        
         //Yes so change direction
        bMovementX = -bMovementX;
        if(mode == kHandballMode)
        {
          rightPlayerScore++;
        }
      }
    }
    
    
    
   //Ball Drawing code for use on the screen
    
    drawBall( ballX, ballY, kBallWidth, kBallHeight, WHITE);
} //void ballUpdate


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Code for drawing the basic shape of the paddle

void drawPaddle(int x, int y, int width, int height, byte color)
{
  for(int i=0;i<=height;i++)
  {
    display.drawLine(x,y+i,x+width,y+i,color);
    
  }
  //delayMicroseconds(50);
} //void drawPaddle


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Code for drawing the basic shape of the ball
void drawBall(int x, int y, int width, int height, byte color)
{
  for(int i=0;i<width;i++)
  {
    display.drawLine(x,y+i,x+width,y+i,color);
    
  }
  //delayMicroseconds(50);
}//void drawBall


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Joystick code for use by the paddles
void getButtons() 
{
    leftButton = display.getButtons(TSButtonUpperLeft); // You can choose another button if you like.
    rightButton = display.getButtons(TSButtonUpperRight);  // You can choose another button if you like.
    
    // The following can replace joystick behavior.
    // Note: This is a basic example. You might want more advanced behavior.
    if (display.getButtons(TSButtonLowerLeft))
        LY = kStickSensitivity + 1;
    else if (display.getButtons(TSButtonLowerRight))
        LY = -kStickSensitivity - 1;
    else 
        LY = 0;
  
    if (display.getButtons(TSButtonLowerLeft))
        LX = kStickSensitivity + 1;
    else if (display.getButtons(TSButtonLowerRight))
        LX = -kStickSensitivity - 1;
    else 
        LX = 0;
}