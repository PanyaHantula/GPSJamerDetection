#include <HardwareSerial.h>

#define RXD 22
#define TXD 21
#define Buzzer 4

HardwareSerial GPS(2);

String gpsData = "";
int Receive_GPS_Name[20];
int Receive_GPS_SNR[20];
int SatellitesIndex = 0;
int CounterGPSRecevie = 0;

int SumOfSNR = 0;
float NowAverageSNR = 0.0, LastAverageSNR = 0.0;

//===========================================
void setup() {
  Serial.begin(115200);
  GPS.begin(9600, SERIAL_8N1, RXD, TXD);  // Pin 16 RX, Pin 17 TX for example
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, LOW);
  Serial.println("GPS Start");

  digitalWrite(Buzzer, HIGH);
  delay(300);
  digitalWrite(Buzzer, LOW);
  delay(300);
}

/*
void loop() {
  // Read incoming data from the GPS module
  while (GPS.available()) {
    Serial.print(char(GPS.read()));
  }
}
*/

void loop() {
  while (GPS.available()) {
    char c = GPS.read();
    gpsData += c;  // Append incoming byte to gpsData string
    if (c == '\n') {
      gpsData.trim();  // Remove any leading/trailing spaces or newlines
      if (gpsData.startsWith("$GPGSV")) {
        //Serial.println(gpsData);

        // Split GPS data with ','
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

        // -----------------------------------------------------------------------------------------------------
        // Extract and display satellite data (4 fields per satellite)
        int numberOfSatellites = (numFields - 4) / 4;  // Each satellite data takes 4 fields
        for (int i = 0; i < numberOfSatellites; i++) {
          int prn = GPS_fields[4 + i * 4].toInt();  // PRN (Satellite ID)
          int snr = GPS_fields[7 + i * 4].toInt();  // Signal-to-noise ratio (dB)

          Receive_GPS_Name[SatellitesIndex] = prn;
          Receive_GPS_SNR[SatellitesIndex] = snr;
          SatellitesIndex = SatellitesIndex + 1;
        }

        // show GPS SNR when a message count is final round
        if (messageCount == messageNum) {
          Serial.print(CounterGPSRecevie);
          Serial.print("\tIndex\t");
          for (int i = 0; i < SatellitesIndex; i++) {
            Serial.print(i);
            Serial.print("\t");
          }
          Serial.println();

          Serial.print(CounterGPSRecevie);
          Serial.print("\tPRN\t");
          for (int i = 0; i < SatellitesIndex; i++) {
            Serial.print(Receive_GPS_Name[i]);
            Serial.print("\t");
          }
          Serial.println();

          Serial.print(CounterGPSRecevie);
          Serial.print("\tSNR\t");
          for (int i = 0; i < SatellitesIndex; i++) {
            Serial.print(Receive_GPS_SNR[i]);
            Serial.print("\t");

            SumOfSNR = SumOfSNR + Receive_GPS_SNR[i];  // Sum ALL SNR
          }

          NowAverageSNR = SumOfSNR / float(totalSatellites);  // Calculate Average SNR
          float deltaAverageSNR = NowAverageSNR - LastAverageSNR;

          Serial.println();
          Serial.print("Sum Of SNR : ");
          Serial.println(SumOfSNR);
          Serial.print("satellites in view :");
          Serial.println(totalSatellites);
          Serial.print("Now Average SNR : ");
          Serial.println(NowAverageSNR);
          Serial.print("Last Average SNR : ");
          Serial.println(LastAverageSNR);
          Serial.print("Delta Average SNR : ");
          Serial.println(deltaAverageSNR);

          if ((deltaAverageSNR <= -2.0) && (deltaAverageSNR > -60.0)) {
            Serial.println("****** GPS Jam ******");
            digitalWrite(Buzzer, HIGH);
          } else {
            LastAverageSNR = NowAverageSNR;
            digitalWrite(Buzzer, LOW);
          }

          Serial.println("---------------------------------------------");
          Serial.println();

          // Clear parameter
          SatellitesIndex = 0;
          SumOfSNR = 0;
          CounterGPSRecevie++;
        }
      }
      gpsData = "";  // Reset gpsData for the next sentence
    }
  }
}
