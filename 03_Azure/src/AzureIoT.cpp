#include <SimpleDHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

// DHT Sensor setting
// for DHT11,
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2

// WiFi settings
const char *ssid = "Redmi-PS";
const char *password = "password";

int pinDHT11 = D4;
SimpleDHT11 dht11;
// Azure Setting
const String DeviceId = "hackDevice";
const String AzureEventHubURI = "https://pawansiothub.azure-devices.net/devices/" + DeviceId + "/messages/events?api-version=2016-02-03";
const String AzureEventHubFingerPrint = "9C:AC:A9:4C:32:F9:53:0A:85:CA:AA:4D:CE:A2:D9:A2:83:9A:B8:AB";
const String AzureEventHubAuth = "SharedAccessSignature sr=pawansiothub.azure-devices.net&sig=qR82T6RdLRMRCRdTZqXxw4Qcn4%2bljUdAM%2b%2bfL4vlo%2bo%3d&se=1529765807&skn=iothubowner";

float humidity, temp;             // Values read from sensor
unsigned long previousMillis = 0; // will store last temp was read
const long interval = 11000;      // interval at which to read sensor

// Main program settings
const int sleepTimeS = 20;

int RestPostData(String uri, String fingerPrint, String authorization, String postData)
{
  HTTPClient http;
  Serial.println(AzureEventHubURI);
  http.begin(uri, fingerPrint);
  http.addHeader("Authorization", authorization);
  http.addHeader("Content-Type", "application/json;type=entry;charset=utf-8");
  int returnCode = http.POST(postData);
  Serial.println(returnCode);
  if (returnCode < 0)
  {
    Serial.println("RestPostData: Error sending data: " + String(http.errorToString(returnCode).c_str()));
  }
  http.end();
  return returnCode;
}

void setup()
{
  // Init serial line
  Serial.begin(115200);
  Serial.println("ESP8266 starting in normal mode");
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop()
{
  // Read sensor
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you read the sensor
    previousMillis = currentMillis;
    // read without samples.
    byte temperature = 0;
    byte humidity = 0;
    if (dht11.read(pinDHT11, &temperature, &humidity, NULL))
    {
      Serial.print("Read DHT11 failed.");
    }
    else
    {
      String postData = "{ \"deviceId\":\"" + DeviceId + "\",\"Temperature\":" + String((int)temperature) + ",\"Humidity\":" + String((int)humidity) + "}";
      Serial.println(postData);
      // Send data to cloud
      int returnCode = RestPostData(AzureEventHubURI, AzureEventHubFingerPrint, AzureEventHubAuth, postData);
    }
  }
  Serial.print("Waiting...");
  delay(5000);
  Serial.println("Done.");
}
