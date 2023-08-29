#include <Arduino.h>
#include <HX711_ADC.h> // Load Cell AD
#include <U8g2lib.h> // Display
#include <Wire.h> // IIC for Display

#if defined(ESP32)
    #include <Preferences.h> // Preferences is the prefered method to read and write the Flash memory. The library EEPROM is outdated. https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/
#endif

//pins:
#if defined(ESP32)
    const int HX711_dout_1 = 16; //mcu > HX711 no 1 dout pin
    const int HX711_sck_1 = 17; //mcu > HX711 no 1 sck pin
    const int HX711_dout_2 = 18; //mcu > HX711 no 2 dout pin
    const int HX711_sck_2 = 23; //mcu > HX711 no 2 sck pin
    const int HX711_dout_3 = 19; //mcu > HX711 no 3 dout pin
    const int HX711_sck_3 = 22; //mcu > HX711 no 3 sck pin
    #define Button1 4
#endif
#if defined(ESP8266)
    #define HX711_dout_1 D0 //mcu > HX711 no 1 dout pin
    #define HX711_sck_1 D8 //mcu > HX711 no 1 sck pin
    #define HX711_dout_2 D5 //mcu > HX711 no 2 dout pin
    #define HX711_sck_2 D2 //mcu > HX711 no 2 sck pin
    #define HX711_dout_3 D6 //mcu > HX711 no 3 dout pin
    #define HX711_sck_3 D2 //mcu > HX711 no 3 sck pin
    #define DisplayData D4
    #define DisplayClk D3
    #define Button_1 D7
    #define Jumper_1 
    #define Jumper_2 
    #define Jumper_3
    // #define Jumper_3 
// D0 HX711_dout_1
// D1
// D2
// D3 DisplayClk
// D4 DisplayData
// D5
// D6
// D7 Button_1
// D8 HX711_sck_1
#endif

// Display
U8G2_SH1107_64X128_1_SW_I2C u8g2(U8G2_R1, DisplayClk, DisplayData, U8X8_PIN_NONE);

// HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell_1(HX711_dout_1, HX711_sck_1); //HX711 1
HX711_ADC LoadCell_2(HX711_dout_2, HX711_sck_2); //HX711 2
HX711_ADC LoadCell_3(HX711_dout_3, HX711_sck_3); //HX711 3

// Variables
unsigned long lastUpdateMillis_1 = 0; // Software Debounce for buttons
unsigned long lastUpdateMillis_2 = 0; // Delay for the load cells
boolean isButtonPressed = false;
boolean availableLoadCell_1 = true; // check if the loadcells are plugged in
boolean availableLoadCell_2 = false;
boolean availableLoadCell_3 = false;
float resultLoadCell_1;
float resultLoadCell_2;
float resultLoadCell_3;
float sumLoadCell;


// Define Interrupts 
IRAM_ATTR void interrupt_1() {
    isButtonPressed=true;
}

// Create Preferences instance
#if defined(ESP32)
    Preferences preferences;
#endif

void calibrate() {
    
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_ncenB12_tr);
        u8g2.setCursor(0, 14);
        u8g2.print("Kalibrierung..."); // "Calibration..."
    } 
    while (u8g2.nextPage());
    delay(2000);

    Serial.println("Now, place your known mass on the loadcell.");
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_ncenB10_tr);
        u8g2.setCursor(0, 14);
        u8g2.print("Gewicht auf WZ1"); // "Put the weight on the load cell"
        u8g2.setCursor(0, 32);
        u8g2.print("legen und Knopf "); 
        u8g2.setCursor(0, 50);
        u8g2.print("druecken"); 
    }
    while (u8g2.nextPage());
    delay(2000);

    float known_mass = 1000;
    boolean _resume = false;
    float newCalibrationValue_1;
    float newCalibrationValue_2;
    float newCalibrationValue_3;

    #if defined(ESP32)
        preferences.begin("Storage", false); // Open the Preferences Storage
    #endif

    if (availableLoadCell_1){
        _resume = false;
        while (_resume == false) {
            if (isButtonPressed && millis() - lastUpdateMillis_1 > 500) {
                isButtonPressed = false;
                lastUpdateMillis_1 = millis();
                Serial.println("interrupt_1");
                LoadCell_1.refreshDataSet(); //refresh the dataset to be sure that the known mass is measured correct
                newCalibrationValue_1 = LoadCell_1.getNewCalibration(known_mass); //get the new calibration value
                Serial.print("calibration value 1: ");
                Serial.print(newCalibrationValue_1);
                #if defined(ESP32) 
                preferences.putFloat("calVal_1", newCalibrationValue_1)
                #endif
                _resume = true;
            }
            yield(); // Required to avoid blocking by the while-Function.
        }
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_ncenB10_tr);
        u8g2.setCursor(0, 14);
        u8g2.print("Waegezelle 1"); // "Load cell 1 is calibrated"
        u8g2.setCursor(0, 32);
        u8g2.print("ist kalibriert");
        u8g2.setCursor(0, 50);
        u8g2.print("Naechste?"); 
    } 
    while (u8g2.nextPage());
    delay(2000);
    }
    
    if (availableLoadCell_2){
        _resume = false;
        while (_resume == false) {
            if (isButtonPressed && millis() - lastUpdateMillis_1 > 500) {
                isButtonPressed = false;
                lastUpdateMillis_1 = millis();
                Serial.println("interrupt_1");
                LoadCell_2.refreshDataSet(); //refresh the dataset to be sure that the known mass is measured correct
                newCalibrationValue_2 = LoadCell_2.getNewCalibration(known_mass); //get the new calibration value
                Serial.print("calibration value 2: ");
                Serial.print(newCalibrationValue_2);
                #if defined(ESP32) 
                preferences.putFloat("calVal_2", newCalibrationValue_2)
                #endif
                _resume = true;
            }            
            yield();
        }
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_ncenB10_tr);
        u8g2.setCursor(0, 14);
        u8g2.print("Waegezelle 2"); // "Load cell 2 is calibrated"
        u8g2.setCursor(0, 32);
        u8g2.print("ist kalibriert");
        u8g2.setCursor(0, 50);
        u8g2.print("Naechste?");  
    } 
    while (u8g2.nextPage());
    delay(2000);
    }

    if (availableLoadCell_3){
        _resume = false;
        while (_resume == false) {
            if (isButtonPressed && millis() - lastUpdateMillis_1 > 500) {
                isButtonPressed = false;
                lastUpdateMillis_1 = millis();
                Serial.println("interrupt_1");
                LoadCell_3.refreshDataSet(); //refresh the dataset to be sure that the known mass is measured correct
                newCalibrationValue_3 = LoadCell_3.getNewCalibration(known_mass); //get the new calibration value
                Serial.print("calibration value: ");
                Serial.print(newCalibrationValue_3);
                #if defined(ESP32) 
                preferences.putFloat("calVal_3", newCalibrationValue_3)
                #endif
                _resume = true;
            }
            yield();
        }
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_ncenB10_tr);
        u8g2.setCursor(0, 14);
        u8g2.print("Waegezelle 3"); // "Load cell 3 is calibrated"
        u8g2.setCursor(0, 32);
        u8g2.print("ist kalibriert"); 
    } 
    while (u8g2.nextPage());
    delay(2000);    
    }    

    #if defined(ESP32)
        preferences.end(); // Close the storage
    #endif

    Serial.println("End calibration");
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_ncenB10_tr);
        u8g2.setCursor(0, 14);
        u8g2.print("Kalibrierung"); // ""
        u8g2.setCursor(0, 32);
        u8g2.print("abgeschlossen"); 
    } 
    while (u8g2.nextPage());
    delay(2000);
}

void setup() {
    Serial.begin(115200); delay(10);
    Serial.println();
    Serial.println("Starting...");

    // Start the Display
    u8g2.begin();

    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_ncenB12_tr);
        u8g2.setCursor(0, 14);
        u8g2.print("Starten..."); // "Starting..."
    } while ( u8g2.nextPage() );
    delay(500);
 
    // Register the Interrups
    pinMode(Button_1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(Button_1), interrupt_1, FALLING);

    // Check the attached Load Cells
    // pinMode(Jumper_1, INPUT_PULLUP);
    // if (digitalRead(Jumper_1) == LOW){
    //     availableLoadCell_1 = true;
    // }
    // pinMode(Jumper_2, INPUT_PULLUP);
    // if (digitalRead(Jumper_2) == LOW){
    //     availableLoadCell_2 = true;
    // }
    // pinMode(Jumper_3, INPUT_PULLUP);
    // if (digitalRead(Jumper_3) == LOW){
    //     availableLoadCell_3 = true;
    // }

    Serial.print("LoadCell1: ");
    Serial.println(availableLoadCell_1);
    Serial.print("LoadCell2: ");
    Serial.println(availableLoadCell_2);
    Serial.print("LoadCell3: ");
    Serial.println(availableLoadCell_3);

    // Display the connected Loadcells
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_ncenB12_tr);
        u8g2.setCursor(0, 14);
        u8g2.print("Waegezelle 1:");
        u8g2.setCursor(0, 32);
        u8g2.print("Waegezelle 2:"); 
        u8g2.setCursor(0, 50);
        u8g2.print("Waegezelle 3:");
        u8g2.setCursor(120, 14);
        u8g2.print(availableLoadCell_1);
        u8g2.setCursor(120, 32);
        u8g2.print(availableLoadCell_2);
        u8g2.setCursor(120, 50);
        u8g2.print(availableLoadCell_3);
    } while ( u8g2.nextPage() );
    
    // Load calibration values
    #if defined(ESP32)
        preferences.begin("Storage", false); // Open the storage
        // preferences.putFloat("calVal_1", 1000);
        // preferences.putFloat("calVal_2", 1000);
        // preferences.putFloat("calVal_3", 1000);
        float calibrationValue_1 = preferences.getFloat("calVal_1", 100); // calibration value load cell 1
        float calibrationValue_2 = preferences.getFloat("calVal_2", 100); // calibration value load cell 2
        float calibrationValue_3 = preferences.getFloat("calVal_3", 100); // calibration value load cell 3
        preferences.end();
    #endif
    #if defined(ESP8266)
        float calibrationValue_1 = 500;
        float calibrationValue_2 = 500;
        float calibrationValue_3 = 500;
    #endif
    Serial.println(calibrationValue_1);
    Serial.println(calibrationValue_2);
    Serial.println(calibrationValue_3);

    unsigned long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
    boolean _tare = true; //set this to false if you don't want tare to be performed in the next step

    if (availableLoadCell_1){
        LoadCell_1.begin();
        LoadCell_1.start(stabilizingtime, _tare);
        if (LoadCell_1.getTareTimeoutFlag() || LoadCell_1.getSignalTimeoutFlag()) {
             Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
            // while (1);
        }
        LoadCell_1.setCalFactor(calibrationValue_1); // user set calibration value (float)
        //LoadCell_1.setReverseOutput();
    }
    if (availableLoadCell_2){
        LoadCell_2.begin();
        LoadCell_2.start(stabilizingtime, _tare);
        if (LoadCell_2.getTareTimeoutFlag() || LoadCell_2.getSignalTimeoutFlag()) {
             Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
            // while (1);
        }
        LoadCell_2.setCalFactor(calibrationValue_2); // user set calibration value (float)
        //LoadCell_2.setReverseOutput();
    }
    if (availableLoadCell_3){
        LoadCell_3.begin();
        LoadCell_3.start(stabilizingtime, _tare);
        if (LoadCell_3.getTareTimeoutFlag() || LoadCell_3.getSignalTimeoutFlag()) {
             Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
            // while (1);
        }
        LoadCell_3.setCalFactor(calibrationValue_3); // user set calibration value (float)
        //LoadCell_3.setReverseOutput();
    }

    delay(1000); // Enough time for the last Display, which shows the connected load cells
    Serial.println("Startup is complete");
    Serial.println("Press interrupt_1 for calibration");
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_ncenB10_tr);
        u8g2.setCursor(0, 14);
        u8g2.print("Zum Kalibrieren");
        u8g2.setCursor(0, 32);
        u8g2.print("Knopf  innerhalb ");
        u8g2.setCursor(0, 50);
        u8g2.print("von 3s druecken");
    } while ( u8g2.nextPage() );
    delay(3000);
    
    // interrupt_1 initiates the calibration
    if (isButtonPressed && millis() - lastUpdateMillis_1 > 500) {
        isButtonPressed = false;
        lastUpdateMillis_1 = millis();
        Serial.println("calibration");
        calibrate();
    }
    
}

void loop() {

    static boolean newDataReady = 0;
    const int serialPrintInterval = 1000; //increase value to slow down serial print activity

    if (availableLoadCell_1){
        if (LoadCell_1.update()){
            newDataReady = true;
        }
    }
    if (availableLoadCell_2){
        if (LoadCell_2.update()){
            newDataReady = true;
        }
    }    
    if (availableLoadCell_3){
        if (LoadCell_3.update()){
            newDataReady = true;
        }
    }

    if (newDataReady) {
        if (millis() > lastUpdateMillis_2 + serialPrintInterval) {
            if (availableLoadCell_1){ 
                resultLoadCell_1 = LoadCell_1.getData();
                Serial.print("Load_cell 1: ");
                Serial.println(resultLoadCell_1);
            }
            if (availableLoadCell_2){ 
                resultLoadCell_2 = LoadCell_2.getData();
                Serial.print("Load_cell 2: ");
                Serial.println(resultLoadCell_2);
            }
            if (availableLoadCell_3){ 
                resultLoadCell_3 = LoadCell_3.getData();
                Serial.print("Load_cell 3: ");
                Serial.println(resultLoadCell_3);
            }
            sumLoadCell = resultLoadCell_1 + resultLoadCell_2 + resultLoadCell_3;
            Serial.print("Sum: ");
            Serial.println(sumLoadCell);
            newDataReady = 0;
            lastUpdateMillis_2 = millis();

            u8g2.firstPage();
            do {
                u8g2.setFont(u8g2_font_ncenB12_tr);
                u8g2.setCursor(0, 14);
                u8g2.print("Summe:");
                u8g2.setCursor(50, 50);
                u8g2.print(sumLoadCell);
            } 
            while (u8g2.nextPage());
        }
    }

    // Tare
    if (isButtonPressed && millis() - lastUpdateMillis_1 > 500) {
        isButtonPressed = false;
        lastUpdateMillis_1 = millis();

        u8g2.firstPage();
        do {
            u8g2.setFont(u8g2_font_ncenB12_tr);
            u8g2.setCursor(0, 14);
            u8g2.print("Tarieren"); // "Tare"
        } 
        while (u8g2.nextPage());

        LoadCell_1.tareNoDelay();
        LoadCell_2.tareNoDelay();
        LoadCell_3.tareNoDelay();
    }
    if (LoadCell_1.getTareStatus() == true) {
        Serial.println("Tare load cell 1 complete");
    }
    if (LoadCell_2.getTareStatus() == true) {
        Serial.println("Tare load cell 2 complete");
    }
    if (LoadCell_3.getTareStatus() == true) {
        Serial.println("Tare load cell 3 complete");
    }

}