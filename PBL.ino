#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include<string.h>


//Book item details for test
#define book_tag "sda21211213"
#define location "A2"
#define book_status "availaible"


#define FIREBASE_HOST "library-automation-f6467-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "5TW44VPU76jBxLJIqehBp8qR2DAgeXzPZJm63wjg"
#define WIFI_SSID "SID"
#define WIFI_PASSWORD "96ginger01"

//Define Firebase Data object
FirebaseData fbdo;
FirebaseJson updateData;
FirebaseJson json1;
String book_name = "Witcher";
String path = "/books/" + book_name;
bool mode_toggle;
unsigned int lastTrigger = 0;
int a=1;

void printResult(FirebaseData &data);
void set_data(FirebaseData &fbdo, FirebaseJson json1, String &path);
void get_data(FirebaseData &fbdo, String &path);
void update_data(FirebaseData &fbdo, FirebaseJson &updateData, int data);
void read_data(FirebaseJson &json1, FirebaseData &data);

ICACHE_RAM_ATTR void toggle(){
  if(millis() - lastTrigger <= 100)
  {
  Serial.println("Mode Toggled");
  if(mode_toggle == false){
  mode_toggle = true;
  }else{
    mode_toggle = false;
  }
  lastTrigger = millis(); 
  }
}

void setup()
{

    Serial.begin(115200);
    Serial.println();
    Serial.println();

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

    //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
    fbdo.setBSSLBufferSize(1024, 1024);

    //Set the size of HTTP response buffers in the case where we want to work with large data.
    fbdo.setResponseSize(1024);

    read_data(json1, fbdo);
    set_data(fbdo,json1,path);
    get_data(fbdo, path);

    attachInterrupt(digitalPinToInterrupt(4), toggle, CHANGE);
}


void set_data(FirebaseData &fbdo, FirebaseJson json1, String &path)
{
   Serial.println("------------------------------------");
    Serial.println("Set JSON test...");

    if (Firebase.set(fbdo, path, json1))
    {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
        Serial.print("VALUE: ");
        printResult(fbdo);
        Serial.println("------------------------------------");
        Serial.println();
    }
    else
    {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
    } 
}


void get_data(FirebaseData &fbdo, String &path)
{
    Serial.println("------------------------------------");
    Serial.println("Get JSON test...");

    if (Firebase.get(fbdo, path))
    {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
        Serial.print("VALUE: ");
        if (fbdo.dataType() == "json")
        {
            printResult(fbdo);
        }

        Serial.println("------------------------------------");
        Serial.println();
    }
    else
    {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
    }
}


void read_data(FirebaseJson &json1, FirebaseData &data)
{
    
    String jsonStr = "";

    json1.set("Book tag", book_tag);
    json1.set("Location", location);
    json1.set("Status", book_status);

    Serial.println("------------------------------------");
    Serial.println("JSON Data");
    json1.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println("------------------------------------");
}


void update_data(FirebaseData &fbdo,FirebaseJson &updateData, int data)
{
    updateData.set("Status",data);
    Firebase.updateNode(fbdo, "/books/Witcher/Json", updateData);

}


void printResult(FirebaseData &data)
{

    if (data.dataType() == "int")
        Serial.println(data.intData());
    else if (data.dataType() == "float")
        Serial.println(data.floatData(), 5);
    else if (data.dataType() == "double")
        printf("%.9lf\n", data.doubleData());
    else if (data.dataType() == "boolean")
        Serial.println(data.boolData() == 1 ? "true" : "false");
    else if (data.dataType() == "string")
        Serial.println(data.stringData());
    else if (data.dataType() == "json")
    {
        Serial.println();
        FirebaseJson &json = data.jsonObject();
        //Print all object data
        Serial.println("Pretty printed JSON data:");
        String jsonStr;
        json.toString(jsonStr, true);
        Serial.println(jsonStr);
        Serial.println();
        Serial.println("Iterate JSON data:");
        Serial.println();
        size_t len = json.iteratorBegin();
        String key, value = "";
        int type = 0;
        for (size_t i = 0; i < len; i++)
        {
            json.iteratorGet(i, type, key, value);
            Serial.print(i);
            Serial.print(", ");
            Serial.print("Type: ");
            Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
            if (type == FirebaseJson::JSON_OBJECT)
            {
                Serial.print(", Key: ");
                Serial.print(key);
            }
            Serial.print(", Value: ");
            Serial.println(value);
        }
        json.iteratorEnd();
    }
    else if (data.dataType() == "array")
    {
        Serial.println();
        //get array data from FirebaseData using FirebaseJsonArray object
        FirebaseJsonArray &arr = data.jsonArray();
        //Print all array values
        Serial.println("Pretty printed Array:");
        String arrStr;
        arr.toString(arrStr, true);
        Serial.println(arrStr);
        Serial.println();
        Serial.println("Iterate array values:");
        Serial.println();
        for (size_t i = 0; i < arr.size(); i++)
        {
            Serial.print(i);
            Serial.print(", Value: ");

            FirebaseJsonData &jsonData = data.jsonData();
            //Get the result data from FirebaseJsonArray object
            arr.get(jsonData, i);
            if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
                Serial.println(jsonData.boolValue ? "true" : "false");
            else if (jsonData.typeNum == FirebaseJson::JSON_INT)
                Serial.println(jsonData.intValue);
            else if (jsonData.typeNum == FirebaseJson::JSON_FLOAT)
                Serial.println(jsonData.floatValue);
            else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
                printf("%.9lf\n", jsonData.doubleValue);
            else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
                     jsonData.typeNum == FirebaseJson::JSON_NULL ||
                     jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
                     jsonData.typeNum == FirebaseJson::JSON_ARRAY)
                Serial.println(jsonData.stringValue);
        }
    }
    else if (data.dataType() == "blob")
    {

        Serial.println();

        for (int i = 0; i < data.blobData().size(); i++)
        {
            if (i > 0 && i % 16 == 0)
                Serial.println();

            if (i < 16)
                Serial.print("0");

            Serial.print(data.blobData()[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
    else if (data.dataType() == "file")
    {

        Serial.println();

        File file = data.fileStream();
        int i = 0;

        while (file.available())
        {
            if (i > 0 && i % 16 == 0)
                Serial.println();

            int v = file.read();

            if (v < 16)
                Serial.print("0");

            Serial.print(v, HEX);
            Serial.print(" ");
            i++;
        }
        Serial.println();
        file.close();
    }
    else
    {
        Serial.println(data.payload());
    }
}


void loop()
{
  Serial.println(mode_toggle);
  //returning a book
if(mode_toggle == true){
  update_data(fbdo,updateData, 1);
}else if(mode_toggle ==false){
  //lending a book
  a += 1;
  update_data(fbdo,updateData, a);
}
}
