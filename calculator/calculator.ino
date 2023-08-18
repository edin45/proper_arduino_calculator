#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

/*--------------------------------------------------------------------------
 * I, Edin Spiegel based this calculator code on the keyboard code of David Wieland,
 * his original notice will be below:
 * This code was written by David Wieland aka. Datulab Tech 
 * https://www.youtube.com/datulabtech to work with the 
 * custom PCBs showcased in this video: https://youtu.be/eH8FnLNZwlk
 * A more detailed explanation can be found here: https://youtu.be/Iq3oY91x9Vk
 * Feel free to use it for your projects, just know that you will have to 
 * change some things for it to work with your hardware.
 * ------------------------------------------------------------------------- */

#include <TimerOne.h>

int longPressDelay = 350;           //customizable encoderValues
int spamSpeed = 15;

byte inputs[] = {9,8,7,6};          //declaring inputs and outputs
const int inCount = sizeof(inputs)/sizeof(inputs[0]);
byte outputs[] = {13,12,11,10};
const int outCount = sizeof(outputs)/sizeof(outputs[0]);

char layout[4][4] = {               //layout grid for characters
  {'7','/','*','-'},
  {'4','8','9','+'},
  {'1','5','6','e'},
  {'0','2','3','x'},
};

int keyDown[4][4];
bool keyLong[4][4];

int cursor_row = 0;
int cursor_col = 0;

String calculation = "";

void setup(){
  
  for(int i=0; i<outCount; i++){    //declaring all the outputs and setting them high
    pinMode(outputs[i],OUTPUT);
    digitalWrite(outputs[i],HIGH);
  }
  for(int i=0; i<inCount; i++){     //declaring all the inputs and activating the internal pullup resistor
    pinMode(inputs[i],INPUT_PULLUP);
  }
  pinMode(5, INPUT_PULLUP);
  
  // Keyboard.begin();
  lcd.begin();
	lcd.backlight();

  // lcd.print("Hello World");

  Serial.begin(9600);
  
  // //////Serial.print("Hello World\n");

  //encoder = new ClickEncoder(16, 14, 15); //initializing the encoder
  //Timer1.initialize(1000);
  //Timer1.attachInterrupt(timerIsr); 
  //55555encoderLast = -1;
}

//Main loop going through all the keys, then waiting 0.5ms
void loop() {
  for (int i=0; i < 4; i++)
  {
    digitalWrite(outputs[i],LOW);   //setting one row low
    delayMicroseconds(5);           //giving electronics time to settle down
    
    for(int j=0; j < 4; j++)
    {
      // //////Serial.print(digitalRead(inputs[j]));
      if(digitalRead(inputs[j]) == LOW)
      {
        
        keyPressed(i,j);            //calling keyPressed function if one of the inputs reads low
      }
      else if(keyDown[i][j] != 0)   //resetting the key if it is not pressed any more
      {  
        resetKey(i,j);
      }
    }
    
    digitalWrite(outputs[i],HIGH);
    delayMicroseconds(500);         //setting the row high and waiting 0.5ms until next cycle
  }
  //checkEncoder();
}

//if a key is pressed, this Funtion is called and outputs to serial the key location, it also sends the keystroke if not already done so
void keyPressed(int row, int col){
  
  if(keyDown[row][col]==0){         //if the function is called for the first time for this key
  keyDown[row][col] = 1;
  // //////Serial.print(keyDown[3][3]);
    // lcd.setCursor(cursor_row,cursor_col);
    if(layout[row][col] == 'e') {
      
      String seperate_calculations[calculation.length()];
      String seperate_calculations_temp[calculation.length()];
      int seperate_calculations_length = 0;

      int StringSep1Count = 0;

      String calculation_dup = calculation;

      // while (calculation_dup.length() > 0)
      // {
      //   int index = calculation_dup.indexOf('(');
      //   if (index == -1) // No space found
      //   {
      //     seperate_calculations_temp[StringSep1Count++] = calculation_dup;
      //     break;
      //   }
      //   else
      //   {
      //     seperate_calculations_temp[StringSep1Count++] = calculation_dup.substring(0, index);
      //     calculation_dup = calculation_dup.substring(index+1);
      //   }
      // }

      while (calculation_dup.length() > 0)
      {
        int index = calculation_dup.indexOf('(');
        if (index == -1) // No space found
        {
          seperate_calculations_temp[StringSep1Count++] = calculation_dup;
          break;
        }
        else
        {
          seperate_calculations_temp[StringSep1Count++] = calculation_dup.substring(0, index);
          calculation_dup = calculation_dup.substring(index+1);
        }
      }

      for (int x = 0; x < StringSep1Count; x++)
      {

        String c[seperate_calculations_temp[x].length()];

        int StringSep2Count = 0;

        String calculation_dup_2 = seperate_calculations_temp[x];

        // while (calculation_dup_2.length() > 0)
        // {
        //   int index = calculation_dup_2.indexOf(')');
        //   if (index == -1) // No space found
        //   {
        //     c[StringSep2Count++] = calculation_dup_2;
        //     break;
        //   }
        //   else
        //   {
        //     c[StringSep2Count++] = calculation_dup_2.substring(0, index);
        //     calculation_dup_2 = calculation_dup_2.substring(index+1);
        //   }
        // }

        while (calculation_dup_2.length() > 0)
        {
          int index = calculation_dup_2.indexOf(')');
          if (index == -1) // No space found
          {
            c[StringSep2Count++] = calculation_dup_2;
            break;
          }
          else
          {
            c[StringSep2Count++] = calculation_dup_2.substring(0, index);
            calculation_dup_2 = calculation_dup_2.substring(index+1);
          }
        }


        for (int x = 0; x < StringSep2Count; x++)
        {
          //Serial.print(" c[" + String(x) + "]: " + String(c[x]) + " ");
          seperate_calculations[seperate_calculations_length] = c[x];
          seperate_calculations_length++;
          
        }    

      }

      for (int x = 0; x < seperate_calculations_length; x++)
      {
        seperate_calculations[x] = calcPass(seperate_calculations[x]); 
        //Serial.print(" x: " + String(x) + " seperate_calculations[x] " + seperate_calculations[x] + " ");
      }

      String new_calc = "";
      for (int x = 0; x < seperate_calculations_length; x++)
      {
        new_calc += seperate_calculations[x];
      }

      //Serial.print(" new_calc: " + new_calc + " "); 

      String done_calc = String(calcPass(new_calc));

      lcd.clear();
      lcd.print(done_calc);

      if(digitalRead(5) == HIGH) {
        calculation = done_calc;
        cursor_row = done_calc.length();
      }else{
        calculation = "";
        cursor_row = 0;
      }
      cursor_col = 0;

    }else if(layout[row][col] == '-' && keyDown[3][3] >= 1) {
      cursor_row--;
      if(cursor_row < 0 && cursor_col == 1) {
        cursor_row = 16;
        cursor_col = 0;
      }
      lcd.setCursor(cursor_row, cursor_col);
      lcd.print(" ");
      if(calculation.length() == 1) {
        calculation = "";
      }else{
        calculation.remove(calculation.length()-1);
      }
    }else if(layout[row][col] == '8' && keyDown[3][3] >= 1) {
      if(calculation == "") {
        lcd.clear();
      }
      lcd.setCursor(cursor_row, cursor_col);
      lcd.print("(");
      calculation += "(";
      // temp_num += layout[row][col];
      // ////////Serial.print(calculation + " - ");
      cursor_row++;
    }else if(layout[row][col] == '9' && keyDown[3][3] >= 1) {
      if(calculation == "") {
        lcd.clear();
      }
      lcd.setCursor(cursor_row, cursor_col);
      lcd.print(")");
      calculation += ")";
      // temp_num += layout[row][col];
      // ////////Serial.print(calculation + " - ");
      cursor_row++;
    }else if(layout[row][col] == '0' && keyDown[3][3] >= 1) {
      if(calculation == "") {
        lcd.clear();
      }
      lcd.setCursor(cursor_row, cursor_col);
      lcd.print(".");
      calculation += ".";
      // temp_num += layout[row][col];
      // ////////Serial.print(calculation + " - ");
      cursor_row++;
    }else if(layout[row][col] != 'x'){
      if(calculation == "") {
        lcd.clear();
      }
      lcd.setCursor(cursor_row, cursor_col);
      lcd.print(layout[row][col]);
      calculation += layout[row][col];
      // temp_num += layout[row][col];
      // ////////Serial.print(calculation + " - ");
      cursor_row++;
    
    }
    
    // Keyboard.press(layout[row][col]);
  }
  // else if(keyLong[row][col] && keyDown[row][col] > spamSpeed){ //if the key has been held long enough to warrant another keystroke set
  //   // Keyboard.press(layout[row][col]);
  //   // // lcd.setCursor(cursor_row,cursor_col);
  //   // lcd.print(layout[row][col]);
  //   // calculation += layout[row][col];
  //   // ////////Serial.print(calculation + " - ");
  //   keyDown[row][col] = 1;
  // }
  // else if(keyDown[row][col] > longPressDelay){ //if the key has been held for longer that longPressDelay, it switches into spam mode
  //   keyLong[row][col] = true;
  // }

  if(cursor_row < 0) {
    cursor_row = -1;
  }

  if(cursor_col < 0) {
    cursor_col = 0;
  }

  if(cursor_row >= 16 && cursor_col != 1) {
    cursor_row = 0;
    cursor_col = 1;
  }

  keyDown[row][col]++;
}

void resetKey(int row, int col){ //resetting the variables after key is released
  // Keyboard.release(layout[row][col]);
  keyDown[row][col] = 0;
  keyLong[row][col] = false;
}

String calcPass(String item_to_calc) {
  //Serial.print(" _____item_to_calc______: " + item_to_calc + " ");
  ////Serial.print(" item_to_calc.indexOf(*): " + String(item_to_calc.indexOf("*")) + " ");
  String add[item_to_calc.length()];
  int StringAddCount = 0;

  String item_to_calc_dup = item_to_calc;

  //Serial.print(" item_to_calc_dup: " + String(item_to_calc_dup) + " ");

  // while (item_to_calc_dup.length() > 0)
  // {
  //   int index = item_to_calc_dup.indexOf('+');
  //   if (index == -1) // No space found
  //   {
  //     add[StringAddCount++] = item_to_calc;
  //     break;
  //   }
  //   else
  //   {
  //     add[StringAddCount++] = item_to_calc_dup.substring(0, index);
  //     item_to_calc_dup = item_to_calc_dup.substring(index+1);
  //   }
  // }

  while (item_to_calc_dup.length() > 0)
  {
    int index = item_to_calc_dup.indexOf('+');
    if (index == -1) // No space found
    {
      add[StringAddCount++] = item_to_calc_dup;
      break;
    }
    else
    {
      add[StringAddCount++] = item_to_calc_dup.substring(0, index);
      item_to_calc_dup = item_to_calc_dup.substring(index+1);
    }
  }

  for (int i = 0; i < StringAddCount; i++)
  {
    //Serial.print(i);
    //Serial.print(": \"");
    //Serial.print(add[i]);
    //Serial.println("\"");
  }

  for (int i = 0; i < StringAddCount; i++)
  {

    //Serial.print(" add[i]: " + add[i] + " ");

    // //Serial.print(" add[i].indexOf(*): " + String(add[i].indexOf("*")) + " ");

    String sub[add[i].length()];
    int StringSubCount = 0;

    String add_dup = add[i];

    while (add_dup.length() > 0)
    {
      int index = add_dup.indexOf('-');
      if (index == -1) // No space found
      {
        sub[StringSubCount++] = add_dup;
        break;
      }
      else
      {
        sub[StringSubCount++] = add_dup.substring(0, index);
        add_dup = add_dup.substring(index+1);
      }
    }

    for (int x = 0; x < StringSubCount; x++)
    {

      String div[sub[x].length()];
      int StringDivCount = 0;

      String sub_dup = sub[x];

      // while (sub_dup.length() > 0)
      // {
      //   int index = sub_dup.indexOf('/');
      //   if (index == -1) // No space found
      //   {
      //     div[StringDivCount++] = sub_dup;
      //     break;
      //   }
      //   else
      //   {
      //     div[StringDivCount++] = sub_dup.substring(0, index);
      //     sub_dup = sub_dup.substring(index+1);
      //   }
      // }

      while (sub_dup.length() > 0)
      {
        int index = sub_dup.indexOf('/');
        if (index == -1) // No space found
        {
          div[StringDivCount++] = sub_dup;
          break;
        }
        else
        {
          div[StringDivCount++] = sub_dup.substring(0, index);
          sub_dup = sub_dup.substring(index+1);
        }
      }


      for (int y = 0; y < StringDivCount; y++)
      {
        ////Serial.print(div[y]);

        
        ////Serial.print(" div[y].indexOf(*): " + String(div[y].indexOf("*")) + " ");

        int contains = div[y].indexOf("*");

        String mul[div[y].length()];
        int StringMulCount = 0;

        String div_dup = div[y];

        while (div_dup.length() > 0)
        {
          int index = div_dup.indexOf("*");
          if (index == -1) // No space found
          {
            mul[StringMulCount++] = div_dup;
            break;
          }
          else
          {
            mul[StringMulCount++] = div_dup.substring(0, index);
            div_dup = div_dup.substring(index+1);
          }
        }

        ////Serial.print(" contains: " + String(contains) + " ");

        if(contains > 0 && StringMulCount > 1 && mul[1] != "" && mul[0] != "") {
          double loc_result = 0;

          ////Serial.print(" StringMulCount: " + String(StringMulCount) + " ");

          for (int j = 1; j < StringMulCount; j++)
          {
            ////Serial.print(" j: " + String(j) + " ");
            if(j == 1) {
              ////Serial.print(" 1: " + String(mul[j-1].toDouble()) + " 2: " + String(mul[j].toDouble()) + " ");
              loc_result = mul[j-1].toDouble() * mul[j].toDouble();
            }else{
              ////Serial.print(" 1: " + String(mul[j].toDouble()) + " ");
              loc_result *= mul[j].toDouble();
            }
          }

          ////Serial.print(" items_to_calc_thing: " + item_to_calc + " ");

          // //Serial.print(" div[y] " + String(div[y]) + " ");

          item_to_calc.replace(div[y], String(loc_result));

          ////Serial.print(" loc_result: " + String(loc_result) + " ");

          
        }


      }

    }

  }

  String add2[item_to_calc.length()];
  int StringAddCount2 = 0;

  String item_to_calc_dup2 = item_to_calc;

  //Serial.print(" item_to_calc_dup2: " + String(item_to_calc_dup2) + " ");

  while (item_to_calc_dup2.length() > 0)
  {
    int index = item_to_calc_dup2.indexOf('+');
    if (index == -1) // No space found
    {
      add2[StringAddCount2++] = item_to_calc_dup2;
      break;
    }
    else
    {
      add2[StringAddCount2++] = item_to_calc_dup2.substring(0, index);
      item_to_calc_dup2 = item_to_calc_dup2.substring(index+1);
    }
  }


  for (int i = 0; i < StringAddCount2; i++)
  {

    String sub[add2[i].length()];
    int StringSubCount = 0;

    String add_dup2 = add2[i];

    while (add_dup2.length() > 0)
    {
      int index = add_dup2.indexOf('-');
      if (index == -1) // No space found
      {
        sub[StringSubCount++] = add_dup2;
        break;
      }
      else
      {
        sub[StringSubCount++] = add_dup2.substring(0, index);
        add_dup2 = add_dup2.substring(index+1);
      }
    }

    for (int x = 0; x < StringSubCount; x++)
    {

      String mul[sub[x].length()];
      int StringMulCount = 0;

      String sub_dup = sub[x];

      while (sub_dup.length() > 0)
      {
        int index = sub_dup.indexOf('*');
        if (index == -1) // No space found
        {
          mul[StringMulCount++] = sub_dup;
          break;
        }
        else
        {
          mul[StringMulCount++] = sub_dup.substring(0, index);
          sub_dup = sub_dup.substring(index+1);
        }
      }

      //Serial.print(" StringMulCount: " + String(StringMulCount) + " ");

      for (int y = 0; y < StringMulCount; y++)
      {
        ////Serial.print(mul[y]);
        

        String div[mul[y].length()];
        int StringDivCount = 0;

        String mul_dup = mul[y];
        //Serial.print(" mul[y] " + mul[y] + " ");

        // while (mul_dup.length() > 0)
        // {
        //   int index = mul_dup.indexOf('/');
        //   if (index == -1) // No space found
        //   {
        //     div[StringDivCount++] = mul_dup;
        //     break;
        //   }
        //   else
        //   {
        //     div[StringDivCount++] = mul_dup.substring(0, index);
        //     mul_dup = mul_dup.substring(index+1);
        //   }
        // }

        // while (mul_dup.length() > 0)
        // {
        //   int index = mul_dup.indexOf('/');
        //   if (index == -1) // No space found
        //   {
        //     div[StringDivCount++] = mul_dup;
        //     break;
        //   }
        //   else
        //   {
        //     div[StringDivCount++] = mul_dup.substring(0, index);
        //     mul_dup = mul_dup.substring(index+1);
        //   }
        // }

        while (mul_dup.length() > 0)
        {
          int index = mul_dup.indexOf('/');
          if (index == -1) // No space found
          {
            div[StringDivCount++] = mul_dup;
            break;
          }
          else
          {
            div[StringDivCount++] = mul_dup.substring(0, index);
            mul_dup = mul_dup.substring(index+1);
          }
        }

        int contains = mul[y].indexOf('/');

        if(contains > 0 && StringDivCount > 1 && div[1] != "" && div[0] != "") {
          double loc_result = 0;

          for (int j = 0; j < StringDivCount; j++)
          {
            //Serial.print(" div[j] " + div[j] + " ");
          }

          for (int j = 1; j < StringDivCount; j++)
          {
            if(j == 1) {
              loc_result = div[j-1].toDouble() / div[j].toDouble();
            }else{
              loc_result /= div[j].toDouble();
            }
          }

          ////Serial.print(" loc_result: " + String(loc_result) + " ");

          item_to_calc.replace(mul[y], String(loc_result));

        }


      }

    }

  }


  String add3[item_to_calc.length()];
  int StringAddCount3 = 0;
  String item_to_calc_dup3 = item_to_calc;

  ////Serial.print(" item_to_calc_dup3: " + String(item_to_calc_dup3) + " ");

  while (item_to_calc_dup3.length() > 0)
  {
    int index = item_to_calc_dup3.indexOf('+');
    if (index == -1) // No space found
    {
      add3[StringAddCount3++] = item_to_calc_dup3;
      break;
    }
    else
    {
      add3[StringAddCount3++] = item_to_calc_dup3.substring(0, index);
      item_to_calc_dup3 = item_to_calc_dup3.substring(index+1);
    }
  }


  for (int i = 0; i < StringAddCount3; i++)
  {

    String sub[add3[i].length()];
    int StringSubCount = 0;
    String add_dup_3 = add3[i];


    while (add_dup_3.length() > 0)
    {
      int index = add_dup_3.indexOf('-');
      if (index == -1) // No space found
      {
        sub[StringSubCount++] = add_dup_3;
        break;
      }
      else
      {
        sub[StringSubCount++] = add_dup_3.substring(0, index);
        add_dup_3 = add_dup_3.substring(index+1);
      }
    }

    

        int contains = add3[i].indexOf('-');

        if(contains > 0 && StringSubCount > 1 && sub[1] != "" && sub[0] != "") {
          double loc_result = 0;

          for (int j = 1; j < StringSubCount; j++)
          {
            if(j == 1) {
              loc_result = sub[j-1].toDouble() - sub[j].toDouble();
            }else{
              loc_result -= sub[j].toDouble();
            }
          }

          item_to_calc.replace(add3[i], String(loc_result));
          
        }

    

  }

  String add4[item_to_calc.length()];
  int StringAddCount4 = 0;

  String item_to_calc_dup_4 = item_to_calc;

  ////Serial.print(" item_to_calc_dup_4: " + String(item_to_calc_dup_4) + " ");

  while (item_to_calc_dup_4.length() > 0)
  {
    int index = item_to_calc_dup_4.indexOf('+');
    if (index == -1) // No space found
    {
      add4[StringAddCount4++] = item_to_calc_dup_4;
      break;
    }
    else
    {
      add4[StringAddCount4++] = item_to_calc_dup_4.substring(0, index);
      item_to_calc_dup_4 = item_to_calc_dup_4.substring(index+1);
    }
  }


        int contains = item_to_calc.indexOf('+');

        if(contains > 0 && StringAddCount4 > 1 && add4[1] != "" && add4[0] != "") {
          double loc_result = 0;

          for (int j = 1; j < StringAddCount4; j++)
          {
            if(j == 1) {
              loc_result = add4[j-1].toDouble() + add4[j].toDouble();
            }else{
              loc_result += add4[j].toDouble();
            }
          }

          item_to_calc.replace(item_to_calc, String(loc_result));
          
        }

        //////Serial.print(" item_to_calc: " + item_to_calc + " ");

    return item_to_calc;

}