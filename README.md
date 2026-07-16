# ir-remote

An ESP32 project which repeats the recieved IR message

## To Try It

1. Connect IR LED according to this diagram

    ```text
                     3.3V
                       |
                     20Ω
                       |
                    IR LED
                       |
                       |
                     Collector
                       |
    GPIO2 ── Base─NPN Transistor
                       |
                     Emitter
                       |
                       |
                      GND
    ```

2. Connect reciever according to this diagram

    ```text
                     3.3V
                       |
    GPIO23 ── Output─Reciever
                       |
                      GND
    ```

3. Upload the code

4. Point a remote to the reciever and press any button

The IR LED will repeat the recieved message
