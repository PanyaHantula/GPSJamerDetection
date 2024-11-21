#include <HardwareSerial.h>

#define RXD 22
#define TXD 21

HardwareSerial GPS(2);  // Using HardwareSerial1 for GPS communication (GPIO16 RX, GPIO17 TX)

// Buffer for the incoming GPS NMEA sentence
String gpsData = "";
int Recive_GPS_Name[20];
int Recive_GPS_SNR[20];
int SatellitesIndex = 0;


void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(115200);

  // Initialize HardwareSerial for GPS communication (9600 baud rate)
  GPS.begin(9600, SERIAL_8N1, RXD, TXD);  // Pin 16 RX, Pin 17 TX for example

  Serial.println("GPS Start");
}

void loop() {
  // Read incoming data from the GPS module
  while (GPS.available()) {
    char c = GPS.read();
    gpsData += c;  // Append incoming byte to gpsData string

    // If we reach a newline character, process the sentence
    if (c == '\n') {
      gpsData.trim();  // Remove any leading/trailing spaces or newlines
      if (gpsData.startsWith("$GPGSV")) {
        Serial.println(gpsData);

        // split data with ","
        int numFields = 0;
        String GPS_fields[60];  // Array to store split fields (max of 20 fields per sentence)
        char delimiter = ',';

        int startIdx = 0;
        for (int i = 0; i < gpsData.length(); i++) {
          if (gpsData[i] == delimiter) {
            GPS_fields[numFields++] = gpsData.substring(startIdx, i);
            startIdx = i + 1;
          }
        }
        GPS_fields[numFields++] = gpsData.substring(startIdx);

        // Extracting values from the GPGSV sentence
        int messageCount = GPS_fields[1].toInt();     // Number of messages in this sentence
        int messageNum = GPS_fields[2].toInt();       // Message number in this sentence
        int totalSatellites = GPS_fields[3].toInt();  // Total number of satellites in view

        // Print basic info about the GPGSV sentence
        //Serial.print("Total Satellites in View: ");
        //Serial.println(totalSatellites);
        //Serial.print("Message Count: ");
        //Serial.println(messageCount);
        //Serial.print("Message Number: ");
        //Serial.println(messageNum);
        //Serial.println("--------------------------");

        // -----------------------------------------------------------------------------------------------------
        // Extract and display satellite data (4 fields per satellite)
        int numberOfSatellites = (numFields - 4) / 4;  // Each satellite data takes 4 fields
        for (int i = 0; i < numberOfSatellites; i++) {
          int prn = GPS_fields[4 + i * 4].toInt();  // PRN (Satellite ID)
          int snr = GPS_fields[7 + i * 4].toInt();  // Signal-to-noise ratio (dB)

          // Print satellite data
          //Serial.print("Satellite PRN: ");
          //Serial.print(prn);
          //Serial.print("\tSNR: ");
          //Serial.println(snr);

          Recive_GPS_Name[SatellitesIndex] = prn;
          Recive_GPS_SNR[SatellitesIndex] = snr;
          SatellitesIndex = SatellitesIndex + 1;
        }


        if (messageCount == messageNum) {
          Serial.println();
          Serial.print("Total Satellites in View: ");
          Serial.println(totalSatellites);

          Serial.print("PRN:\t");
          for (int i = 0; i < SatellitesIndex; i++) {
            Serial.print(Recive_GPS_Name[i]);
            Serial.print("\t");
          }
          Serial.println();

          Serial.print("SNR:\t");
          for (int i = 0; i < SatellitesIndex; i++) {
            Serial.print(Recive_GPS_SNR[i]);
            Serial.print("\t");
          }
          Serial.println();
          Serial.println("----------------------------------------------------------------------------------------------");
          Serial.println();

          SatellitesIndex = 0;
        }
      }
      gpsData = "";  // Reset gpsData for the next sentence
    }
  }
}
