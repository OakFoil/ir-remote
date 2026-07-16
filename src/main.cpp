#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRac.h>

const int LED_PIN = 2,
          RECIEVER_PIN = 23,
          bufferSize = 1024,
          timeout = 50,
          rawrecieved_messageTransmitFreq = 38000;

IRrecv reciever(RECIEVER_PIN, bufferSize, timeout);
IRsend transmitter(LED_PIN);
decode_results recieved_message;

void printMessageInfo()
{
    Serial.print(resultToHumanReadableBasic(&recieved_message));
    String description = IRAcUtils::resultAcToString(&recieved_message);
    if (description.length())
        Serial.println(D_STR_MESGDESC ": " + description);
    Serial.println(resultToTimingInfo(&recieved_message));
    Serial.println(resultToSourceCode(&recieved_message));
}

bool transmitMessage()
{
    bool success = true;
    if (recieved_message.decode_type == decode_type_t::UNKNOWN)
    {
        uint16_t *raw_array = resultToRawArray(&recieved_message);
        uint16_t correctedLen = getCorrectedRawLength(&recieved_message);
        transmitter.sendRaw(raw_array, correctedLen, rawrecieved_messageTransmitFreq);
        delete[] raw_array;
    }
    else if (hasACState(recieved_message.decode_type))
        success = transmitter.send(recieved_message.decode_type, recieved_message.state, recieved_message.bits / 8);
    else
        success = transmitter.send(recieved_message.decode_type, recieved_message.value, recieved_message.bits);

    return success;
}

void setup()
{
    Serial.begin(115200);

    reciever.enableIRIn();
    transmitter.begin();

    while (!reciever.decode(&recieved_message))
        ; // Wait until recieved_message is recieved

    printMessageInfo();
}

void loop()
{
    bool success = transmitMessage();
    Serial.printf("A %d-bit %s recieved_message was %ssuccessfully retransmitted.\n",
                  recieved_message.bits, typeToString(recieved_message.decode_type).c_str(), success ? "" : "un");

    delay(1000);
}
