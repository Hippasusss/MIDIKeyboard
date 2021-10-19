#include <MIDIUSB.h>

const byte NUMK       = 88;
const byte NUMI       = 11;
const byte NUMO       = 8;
const byte CHANNEL    = 1;
const byte OFFSET     = 9 + 12;
const byte SUSTAINCC  = 64;

const byte sustainPins[2]     = {20, 21};
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
    digitalWrite(sustainPins[1], LOW);
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

            const byte inRead = digitalRead(inputPins[j]);
            const byte inReadVel = digitalRead(inputPinsVel[j]);
            //KEY START - key has began being pressed
            if (inRead == LOW)
            {
                if (noteTimer[currentNote] == 0)
                {
                    noteTimer[currentNote] = micros();
                }
            }
            else //read HIGH
            {
                noteTimer[currentNote] = 0;
                noteVel[currentNote] = 0;
                noteOn[currentNote] = false;
            }

            //KEY END - key has been fully actuated
            if (inReadVel == LOW)
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
            else //read HIGH
            {
                
            }
        }


        digitalWrite(outputPins[i], HIGH);
    }
}

void updateSustain()
{
    sustainOn = digitalRead(sustainPins[0]);
}

byte calcVel(unsigned long value)
{

    const int VelocityFastMs = 5000;
    const int VelocitySlowMs = 50000;
    const float vPower = 0.5f;


    value = constrain(value, VelocityFastMs, VelocitySlowMs );
    value = (pow((value - VelocityFastMs), vPower) / pow( VelocitySlowMs - VelocityFastMs, vPower)) * VelocitySlowMs + VelocityFastMs;
    value = constrain(value, VelocityFastMs, VelocitySlowMs ); 
    // Double contrain is there on purpose to get the exponent to behave itself at extreme values

    value = map(value, VelocityFastMs, VelocitySlowMs, 127, 1);

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
            if (noteOn[i] == true) sendNoteOn (CHANNEL, note, noteVel[i]);
            else                   sendNoteOff(CHANNEL, note, 0);
            noteOnPrev[i] = noteOn[i];
            // BREAK NEEDED: stops bug where notes hang. possibly bug in midiusb library. 
            // ensures that notes are sent in seperate loops
            break;
        }
    }

    //Send Sustain Data
    if (sustainOn != sustainOnPrev)
    {
        byte value = sustainOn ? 0 : 127;
        sendControlChange(CHANNEL, SUSTAINCC, value);
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
