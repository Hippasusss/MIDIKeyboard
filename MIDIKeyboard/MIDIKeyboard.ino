#include <MIDIUSB.h>

const byte NUMK    = 88;
const byte NUMI    = 11;
const byte NUMO    = 8;
const byte CHANNEL = 1;
const byte OFFSET = 9+24;
const byte SUSTAINCC = 64;

const byte sustainPins[2] = {20, 21};
const byte inputPins[NUMI]    = {22, 24, 26, 28, 30, 41, 43, 45, 47, 49, 51};
const byte inputPinsVel[NUMI] = {23, 25, 27, 29, 31, 40, 42, 44, 46, 48, 50};
const byte outputPins[NUMO]   = {32, 33, 34, 35, 36, 37, 38, 39};

bool sustainOn                = false;
bool sustainOnPrev            = false;
bool noteOn[NUMK]             = {};
bool noteOnPrev[NUMK]         = {};
unsigned long noteVel[NUMK]   = {};
unsigned long noteTimer[NUMK] = {};

void setup()
{
    // Key pins
    for (int i = 0; i < NUMI; ++i)
    {
        pinMode(inputPins[i], INPUT_PULLUP);
        pinMode(inputPinsVel[i], INPUT_PULLUP);
    }
    for (int i = 0; i < NUMO; ++i)
    {
        pinMode(outputPins[i], OUTPUT);
    }

    //Sustain Pins
    pinMode(sustainPins[0], INPUT_PULLUP);
    pinMode(sustainPins[1], OUTPUT);
    
    Serial.begin(9600);
}

void loop()
{
    updateNoteArray();
    updateSustain();
    sendMIDI();
}


void updateNoteArray()
{
    for (byte i = 0; i < NUMO; ++i)
    {
        digitalWrite(outputPins[i], LOW ); 

        for (byte j = 0; j < NUMI; ++j) 
        {
            byte currentNote = ((j + 1) * NUMO) - ((NUMO - 1) - i) - 1; 

            //KEY START - key has began being pressed
            if (digitalRead(inputPins[j]) == LOW) 
            {
                if (noteTimer[currentNote] == 0) 
                {
                    noteTimer[currentNote] = micros();  
                }
            }
            else 
            {
                noteTimer[currentNote] = 0;
                noteVel[currentNote] = 0;
            }

            //KEY END - key has been full actuated
            if (digitalRead(inputPinsVel[j]) == LOW)
            {
                if (noteVel[currentNote] == 0)   
                {
                    noteVel[currentNote] = calcVel(micros() - noteTimer[currentNote]);
                }  
                if (noteOn[currentNote]  == false)
                {
                    noteOn[currentNote] = true;
                } 
            }
            else
            {
                noteOn[currentNote] = false;
            }
        }

        digitalWrite(outputPins[i], HIGH);
    }
}

void updateSustain()
{
  digitalWrite(sustainPins[0], LOW);
  const bool sustainOn = digitalRead(sustainPins[1]);
  if (sustainOn != sustainOnPrev)
  {
    
    
  }
  digitalWrite(sustainPins[0], HIGH);
  
}

byte calcVel(unsigned long value)
{
    value = constrain(value, 5000, 100000);
    value = map(value, 5000, 100000, 127, 1);
    return (byte)value;
}

void sendMIDI()
{
    // Send Note Data
    for (int i = 0; i < NUMK; i++)
    {
        if (noteOn[i] != noteOnPrev[i])
        {
            byte note = i + OFFSET;
            if (noteOn[i] == true) sendNoteOn (1, note, noteVel[i]);
            else                   sendNoteOff(1, note, 0);
            noteOnPrev[i] = noteOn[i];
        }
    }

    //Send Sustain Data
    if (sustainOn != sustainOnPrev)
    {
      sendControlChange(1, SUSTAINCC, sustainOn * 120);
      sustainOnPrev = sustainOn;
    }
    
    MidiUSB.flush();
}

void sendControlChange(byte channel, byte control, byte value) {

  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};

  MidiUSB.sendMIDI(event);
}

void sendNoteOn(byte channel, byte pitch, byte velocity) 
{
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void sendNoteOff(byte channel, byte pitch, byte velocity) 
{
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}
