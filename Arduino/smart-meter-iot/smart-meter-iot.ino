int analogValue = 0;


enum ListType {
    List1 = 0x1,                // List1: Active Power (All meters)
    List2SinglePhase = 0x9,     // List2: (Single phase meters)
    List2 = 0x13,               // List2: (3 phase meters)
    List3 = 0x12,               // List3: List2 + cumulative data (meter reading)
    List3SinglePhase = 0xE      // List3: List2 + cumulative data (meter reading) (Single phase meters)
};

void setup()
{
  pinMode(13, OUTPUT);

  Serial.begin(115200, SERIAL_8N1);
  Serial2.begin(2400, SERIAL_8E1);
}


void GetPacket() {
  if (Serial2.available() <= 0) return;
  
    while (true) {
        byte b = Serial2.read();
        if (b != 0x7E) continue;
        if (Serial2.read() == 0x7E) continue;

        byte packetLength = (byte) Serial2.read();
        Serial.println("Packet length: " + String(packetLength));

        byte packet[packetLength + 2] = {0x7E, 0xA0, packetLength};


        int readBytes = 3;
        while (readBytes < packetLength + 2) {
            packet[readBytes] = (byte) Serial2.read();
            readBytes++;
        }

        Serial.println("Read bytes: " + String(readBytes));

        ListType type = (ListType) packet[32];

        switch (type) {
            case List1:
                //return DecodeList1Packet(packet);
                Serial.println("Found List1");

                {
                  unsigned long power = packet[37];
                  power = power | packet[36] << 8;
                  power = power | packet[35] << 16;
                  power = power | packet[34] << 24;

                  Serial.println("Power: " + String(power));

                  unsigned int year = packet[20];
                  year = year | packet[19] << 8;

                  byte month = packet[21];
                  byte day = packet[22];
                  byte hours = packet[24];
                  byte minutes = packet[25];
                  byte seconds = packet[26];

                  Serial.println("Date: " + String(year) + "/" + String(month) + "/" + String(day));
                  Serial.println("Time: " + String(hours) + ":" + String(minutes) + ":" + String(seconds));
                }
                break;

            case List2SinglePhase:
                //return DecodeList2SinglePhasePacket(packet);
                Serial.println("Found List2SinglePhase");

                {
                  unsigned long current = packet[94];
                  current = current | packet[93] << 8;
                  current = current | packet[92] << 16;
                  current = current | packet[91] << 24;

                  float fCurrent = current / 1000.0f;
  
                  Serial.println("Current: " + String(fCurrent));
  
                  //CurrentL1 = BitConverter.ToInt32(data.Skip(91).Take(4).Reverse().ToArray(), 0),
                  //VoltageL1 = BitConverter.ToUInt32(data.Skip(96).Take(4).Reverse().ToArray(), 0),
                }
                break;

            case List3SinglePhase:
                //return DecodeList3SinglePhasePacket(packet);
                Serial.println("Found List3SinglePhase");
                break;

            default:
                Serial.println("Found unknown list:");
                Serial.write(packet, readBytes);
                //OutputPacketAsHex(packet);
                Serial.println("");
                break;
        }

        return;
    }
}



void loop()
{
  // Converts the signal from the voltage divider into a TTL signal and feed it into the Serial2 RX pin
  analogValue = analogRead(A0);
  digitalWrite(13, analogValue > 1800);

  //if (Serial2.available() > 0) {
  //  Serial.write(Serial2.read());
  //}

  GetPacket();
}
