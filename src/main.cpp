#include <Arduino.h>
#include <HX711_ADC.h>
#include <Preferences.h> // Preferences ist die bevorzugte Methode den Flashspeicher zu nutzen. Die Library EEPROM wird nicht mehr entwickelt. https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/

//pins:
const int HX711_dout_1 = 16; //mcu > HX711 no 1 dout pin
const int HX711_sck_1 = 17; //mcu > HX711 no 1 sck pin
const int HX711_dout_2 = 18; //mcu > HX711 no 2 dout pin
const int HX711_sck_2 = 23; //mcu > HX711 no 2 sck pin
const int HX711_dout_3 = 19; //mcu > HX711 no 3 dout pin
const int HX711_sck_3 = 22; //mcu > HX711 no 3 sck pin
#define Button1 4

// HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell_1(HX711_dout_1, HX711_sck_1); //HX711 1
HX711_ADC LoadCell_2(HX711_dout_2, HX711_sck_2); //HX711 2
HX711_ADC LoadCell_3(HX711_dout_3, HX711_sck_3); //HX711 3

long lastUpdateMillis = 0;
boolean isButtonPressed = false;

// Interrupts definieren
IRAM_ATTR void Taster() {
    isButtonPressed=true;
    // Serial.println("Hallo");
}

// Preferences-Instanz erstellen
Preferences preferences;

void setup() {
    Serial.begin(115200); delay(10);
    Serial.println();
    Serial.println("Starting...");

    // Interrups registrieren
    attachInterrupt(digitalPinToInterrupt(Button1), Taster, HIGH); // hier muss man ggf. mit den Varainten HIGH, LOW CHANGE, FALLING und RISING probieren

    // Speicher öffnen
    // preferences.begin("Speicher", false);
    // preferences.putFloat("calVal_1", 1000);
    // preferences.putFloat("calVal_2", 1000);
    // preferences.putFloat("calVal_3", 1000);
    // float calibrationValue_1 = preferences.getFloat("calVal_1", 1000); // calibration value load cell 1
    // float calibrationValue_2 = preferences.getFloat("calVal_2", 1000); // calibration value load cell 2
    // float calibrationValue_3 = preferences.getFloat("calVal_3", 1000); // calibration value load cell 3
    // preferences.end();

    // zu Testzwecken feste Kalibierwerte nutzen
    float calibrationValue_1 = 696.0; // uncomment this if you want to set this value in the sketch
    float calibrationValue_2 = 733.0; // uncomment this if you want to set this value in the sketch
    float calibrationValue_3 = 733.0; // uncomment this if you want to set this value in the sketch

    Serial.println("A");

    

    LoadCell_1.begin();
    LoadCell_2.begin();
    LoadCell_3.begin();

    Serial.println("B");

    //LoadCell_1.setReverseOutput();
    //LoadCell_2.setReverseOutput();
    //LoadCell_3.setReverseOutput();
    unsigned long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
    boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
    byte loadcell_1_rdy = 0;
    byte loadcell_2_rdy = 0;
    byte loadcell_3_rdy = 0;
    while ((loadcell_1_rdy + loadcell_2_rdy + loadcell_3_rdy) < 3) { //run startup, stabilization and tare, both modules simultaniously
        if (!loadcell_1_rdy) loadcell_1_rdy = LoadCell_1.startMultiple(stabilizingtime, _tare);
        if (!loadcell_2_rdy) loadcell_2_rdy = LoadCell_2.startMultiple(stabilizingtime, _tare);
        if (!loadcell_3_rdy) loadcell_3_rdy = LoadCell_3.startMultiple(stabilizingtime, _tare);
    }
    if (LoadCell_1.getTareTimeoutFlag()) {
        Serial.println("Timeout, check MCU>HX711 no.1 wiring and pin designations");
    }
    if (LoadCell_2.getTareTimeoutFlag()) {
        Serial.println("Timeout, check MCU>HX711 no.2 wiring and pin designations");
    }
    if (LoadCell_3.getTareTimeoutFlag()) {
        Serial.println("Timeout, check MCU>HX711 no.3 wiring and pin designations");
    }
    LoadCell_1.setCalFactor(calibrationValue_1); // user set calibration value (float)
    LoadCell_2.setCalFactor(calibrationValue_2); // user set calibration value (float)
    LoadCell_3.setCalFactor(calibrationValue_3); // user set calibration value (float)
    Serial.println("Startup is complete");

    // Interupt soll die Kalibrierung auslösen
    if (isButtonPressed)  {
        Serial.println("Kalibrierung");
    }
}

//ab hier weiter bearbeiten. Es muss jetzt die Kombination aus der Vorlage mit Interupt und der einkopierten Vorlage erstellt werden.

void loop() {
     static boolean newDataReady = 0;

    if (isButtonPressed && millis() - lastUpdateMillis > 500) {
        isButtonPressed = false;
        lastUpdateMillis = millis();
        Serial.println("Taster");
        newDataReady = true;
    }

    //get smoothed value from data set
    if ((newDataReady)) {
        float a = LoadCell_1.getData();
        float b = LoadCell_2.getData();
        float c = LoadCell_3.getData();
        Serial.print("Load_cell 1 output val: ");
        Serial.print(a);
        Serial.print("    Load_cell 2 output val: ");
        Serial.println(b);
        Serial.print("    Load_cell 3 output val: ");
        Serial.println(c);
        newDataReady = 0;  
    }
    /* Wahrscheinlich ist dieser Bereich nicht nötig, da nur im Setup tariert werden soll
  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') {
      LoadCell_1.tareNoDelay();
      LoadCell_2.tareNoDelay();
    }
  }

  //check if last tare operation is complete
  if (LoadCell_1.getTareStatus() == true) {
    Serial.println("Tare load cell 1 complete");
  }
  if (LoadCell_2.getTareStatus() == true) {
    Serial.println("Tare load cell 2 complete");
  }
    */

}

void calibrate() {

     Serial.println("Now, place your known mass on the loadcell.");

    float known_mass = 1000;
    boolean _resume = false;
    float newCalibrationValue_1;
    float newCalibrationValue_2;
    float newCalibrationValue_3;
    preferences.begin("Speicher", false); // Speicher öffnen

    u_int8_t count = 3; // Hier sollte noch eine Funktion eingebaut werden, die prüft, wie viele Wägezellen tatsächlich angeschlossen sind.
    for (size_t i = 1; i <= count; i++) {
        _resume = false;
        while (_resume == false) {
            if (isButtonPressed && millis() - lastUpdateMillis > 500) {
                isButtonPressed = false;
                lastUpdateMillis = millis();
                Serial.println("Taster gedrückt");
                    switch (i)
                    {
                        case 1:
                            LoadCell_1.refreshDataSet(); //refresh the dataset to be sure that the known mass is measured correct
                            newCalibrationValue_1 = LoadCell_1.getNewCalibration(known_mass); //get the new calibration value
                            Serial.print("New calibration value has been set to: ");
                            Serial.print(newCalibrationValue_1);
                            // preferences.putFloat("calVal_1", newCalibrationValue_1)
                            break;
                        case 2:
                            LoadCell_2.refreshDataSet(); //refresh the dataset to be sure that the known mass is measured correct
                            newCalibrationValue_2 = LoadCell_2.getNewCalibration(known_mass); //get the new calibration value
                            Serial.print("New calibration value has been set to: ");
                            Serial.print(newCalibrationValue_2);
                            // preferences.putFloat("calVal_2", newCalibrationValue_2)
                            break;                       
                        case 3:
                            LoadCell_3.refreshDataSet(); //refresh the dataset to be sure that the known mass is measured correct
                            newCalibrationValue_3 = LoadCell_3.getNewCalibration(known_mass); //get the new calibration value
                            Serial.print("New calibration value has been set to: ");
                            Serial.print(newCalibrationValue_3);
                            // preferences.putFloat("calVal_3", newCalibrationValue_3)
                            break; 
                        default:
                            break;
                    }
                _resume = true;
            }
        }
        
    }
    preferences.end(); // Speicher schließen
    Serial.println("End calibration");
}