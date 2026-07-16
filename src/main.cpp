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
          rawMessageTransmitFreq = 38000;

IRrecv reciever(RECIEVER_PIN, bufferSize, timeout);
IRsend transmitter(LED_PIN);
decode_results recieved_message;

void printMessageInfo(const decode_results &message)
{
    Serial.print(resultToHumanReadableBasic(&message));
    String description = IRAcUtils::resultAcToString(&message);
    if (description.length())
        Serial.println(D_STR_MESGDESC ": " + description);
    Serial.println(resultToTimingInfo(&message));
    Serial.println(resultToSourceCode(&message));
}

bool transmitMessage(IRsend &transmitter, const decode_results &message)
{
    bool success = true;
    if (message.decode_type == decode_type_t::UNKNOWN)
    {
        uint16_t *raw_array = resultToRawArray(&message);
        uint16_t correctedLen = getCorrectedRawLength(&message);
        transmitter.sendRaw(raw_array, correctedLen, rawMessageTransmitFreq);
        delete[] raw_array;
    }
    else if (hasACState(message.decode_type))
        success = transmitter.send(message.decode_type, message.state, message.bits / 8);
    else
        success = transmitter.send(message.decode_type, message.value, message.bits);

    return success;
}

void setup()
{
    Serial.begin(115200);

    reciever.enableIRIn();
    transmitter.begin();

    while (!reciever.decode(&recieved_message))
        ; // Wait until message is recieved

    printMessageInfo(recieved_message);
}

void loop()
{
    bool success = transmitMessage(transmitter, recieved_message);
    Serial.printf("A %d-bit %s message was %ssuccessfully retransmitted.\n",
                  recieved_message.bits, typeToString(recieved_message.decode_type).c_str(), success ? "" : "un");

    delay(1000);
}
