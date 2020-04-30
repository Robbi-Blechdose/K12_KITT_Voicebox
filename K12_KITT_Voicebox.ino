#include <TMRpcm.h>
#include <SPI.h>
#include <SD.h>

#include "LedControl.h"

/**
 * K12 KITT Voicebox
 * Copyright (C) 2020 Julius Clasen
 * Licensed under GNU AGPLv3
 * (See LICENSE for full license)
 */

/**
 * Overview of files on SD card:
 * IAMKITT.WAV - "I am the voice of Knight Industry 200s microprocessor. K.I.T.T. for easy reference. KITT if you prefer."
 * 
 * KPHANDS.WAV - "I'd prefer it if you'd kindly keep your hands to yourself."
 * OWNERS.WAV  - "You are not the proper owners of this vehicle, so either you get out or I will take the appropriate measures."
 * 
 * HOCKEY.WAV  - "The similarity of hockey and a war between various caveman tribes on a frozen lake bears a very close parallel."
 * LSPORT.WAV  - "I must confess, I have a difficult time computing the logic of this sport... as you insist on calling it."
 * 
 * DASH.WAV    - "I don't have a strange-looking dash. I'm proud of my dash."
 * RRSCAN.WAV  - "Can a Rolls-Royce do computer scans?"
 * ARGUE.WAV   - "I don't argue, I merely correct improper calculations."
 * 
 * KEY0.WAV    - Key 0 beep
 * KEY1.WAV    - Key 1 beep
 * KEY2.WAV    - Key 2 beep
 * KEY3.WAV    - Key 3 beep
 */

//Pin definitions
#define P_MAX_DIN   6
#define P_MAX_CLK   8
#define P_MAX_CS    7

#define P_SPEAKER   9
#define P_SD_CS    10
#define P_AUDIO_IN A0

#define P_REED_A   A2

#define P_IN_INTRO  2
#define P_IN_PROUD  3
#define P_IN_JOKE   4
#define P_IN_SPORTS 5

LedControl lc = LedControl(P_MAX_DIN, P_MAX_CLK, P_MAX_CS, 1);
TMRpcm tmrpcm;

boolean hoodClosed;

void setup()
{
  //Initialize some pins
  pinMode(P_IN_INTRO, INPUT_PULLUP);
  pinMode(P_IN_PROUD, INPUT_PULLUP);
  pinMode(P_IN_JOKE, INPUT_PULLUP);
  pinMode(P_IN_SPORTS, INPUT_PULLUP);
  pinMode(P_AUDIO_IN, INPUT);
  
  //Wake up the chip
  lc.shutdown(0, false);
  //Medium brightness
  lc.setIntensity(0, 12); //Used to be 8
  lc.clearDisplay(0);

  //Initialize audio
  tmrpcm.speakerPin = P_SPEAKER;
  //tmrpcm.setVolume(7);
  SD.begin(P_SD_CS);

  startSequence();
}

void loop()
{
  //TODO
  //displayVUMeter(analogRead(P_AUDIO_IN));
  //delay(100);
  
  //Check reed contact
  if(digitalRead(P_REED_A) == LOW) //Magnet is close -> Hood is closed
  {
    lc.setLed(0, 7, 0, true);
    hoodClosed = true;
  }
  else //Magnet is away -> Hood has been opened
  {
    lc.setLed(0, 7, 0, false);

    //Hood was closed, is now open -> play sound file
    if(hoodClosed)
    {
      tmrpcm.play("KPHANDS.WAV");
    }
    
    hoodClosed = false;
  }

  //Check all buttons
  if(digitalRead(P_IN_INTRO) == LOW)
  {
    tmrpcm.play("IAMKITT.WAV");
  }
  else if(digitalRead(P_IN_PROUD) == LOW)
  {
    switch(random(0, 3))
    {
      case 0:
      {
        tmrpcm.play("DASH.WAV");
        break;
      }
      case 1:
      {
        tmrpcm.play("RRSCAN.WAV");
        break;
      }
      case 2:
      {
        tmrpcm.play("ARGUE.WAV");
        break;
      }
    }
  }
  else if(digitalRead(P_IN_JOKE) == LOW)
  {
    
  }
  else if(digitalRead(P_IN_SPORTS) == LOW)
  {
    switch(random(0, 2))
    {
      case 0:
      {
        tmrpcm.play("HOCKEY.WAV");
        break;
      }
      case 1:
      {
        tmrpcm.play("LSPORT.WAV");
        break;
      }
    }
  }
}

void displayVUMeter(int value)
{
  //Precalculations (we need the number of LEDs to light up)
  int num = (value * 8) / 1000; //Should really be /1024, but we'd never reach full amplitude because of the analog input 
  //Center
  for(int i = 0; i < 8; i++)
  {
    if(i >= (4 - (num / 2)) && i < (4 + (num / 2)))
    {
      lc.setLed(0, i, 4, true);
      lc.setLed(0, i, 5, true);
    }
    else
    {
      lc.setLed(0, i, 4, false);
      lc.setLed(0, i, 5, false);
    }
  }

  //Scale value
  num = (num * 6) / 8;
  //Left and Right
  for(int i = 0; i < 8; i++)
  {
    if(i >= (4 - (num / 2)) && i < (4 + (num / 2)))
    {
      lc.setLed(0, i, 2, true);
      lc.setLed(0, i, 3, true);
      lc.setLed(0, i, 6, true);
      lc.setLed(0, i, 7, true);
    }
    else
    {
      lc.setLed(0, i, 2, false);
      lc.setLed(0, i, 3, false);
      lc.setLed(0, i, 6, false);
      lc.setLed(0, i, 7, false);
    }
  }
}

void startSequence()
{
  tmrpcm.play("KEY0.WAV");
  lc.setLed(0, 7, 0, true);
  delay(300);
  
  tmrpcm.play("KEY1.WAV");
  lc.setLed(0, 5, 0, true);
  delay(300);
  
  tmrpcm.play("KEY2.WAV");
  lc.setLed(0, 3, 0, true);
  delay(300);
  
  tmrpcm.play("KEY3.WAV");
  lc.setLed(0, 1, 0, true);
  delay(300);
  
  lc.setLed(0, 7, 0, false);
  lc.setLed(0, 5, 0, false);
  lc.setLed(0, 3, 0, false);
  lc.setLed(0, 1, 0, false);
}
