#include <ESP8266WiFi.h>
#include <GyverStepper.h>

GStepper<STEPPER4WIRE> stepper(2038,D8,D6,D7,D5 );
bool task = false;

// const char* ssid = "HUAWEI-6dNj";
// const char* password = "48575443A76F7F9D";
const char* ssid = "VALTEST";
const char* password = "asnastas";
int LED_TCP = D1; 
WiFiServer wifiServer(5045);

void setup() {
  stepper.setSpeedDeg(1);        // в градусах/сек

  Serial.begin(115200);
  delay(1000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting..");
  }
  Serial.print("Connected to WiFi. IP:");
  Serial.println(WiFi.localIP());
  wifiServer.begin();
}

void loop() {
  WiFiClient client = wifiServer.available();
  if (client) {
    while (client.connected()) {
      String data;
      while (client.available() > 0) {
        // Получить
        char c = client.read();
        Serial.println(c);

        if(c == '$') task = true;
        else data+=c;

        delay(10);

      }
      if(task && data!="") 
      {
        switch(data[0]){
          case 'd':
            data.remove(0,1);
            Serial.println(data);
            stepper.setTargetDeg(data.toFloat());  // в градусах 
          break;
          case 'c':
            data.remove(0,1);
            Serial.println(data);
            stepper.setTargetDeg(stepper.getCurrentDeg()+data.toFloat());  // в градусах 
          break;
          case 'r':
            stepper.setCurrentDeg(0);
          break;
        }
        data = "";
      }
      else if(task && !stepper.tick()) task = false;
      // Отправить
      {
        String dataSend(stepper.getCurrentDeg());
        size_t len = dataSend.length();
        //Serial.println(dataSend);

        uint8_t sbuf[len];
        dataSend.getBytes(sbuf,len);
        client.write(sbuf, len);
        delay(1);
      }
    }
  }
  client.stop();
}