#include <WiFi.h>
#include <FirebaseESP32.h>
#include<string.h>
#include <SPI.h>
#include <MFRC522.h>


#define location "A2"
#define book_status 1


#define FIREBASE_HOST "library-automation-f6467-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "5TW44VPU76jBxLJIqehBp8qR2DAgeXzPZJm63wjg"
#define WIFI_SSID "SID"
#define WIFI_PASSWORD "96ginger01"


const int RST_PIN = 22; // Reset pin
const int SS_PIN = 21; // Slave select pin
int user_name;
 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

//Define Firebase Data object
FirebaseData fbdo;
FirebaseJson updateData;
FirebaseJson json1;


void printResult(FirebaseData &data);
void set_data(FirebaseData &fbdo, FirebaseJson json1, String path);
String get_data(FirebaseData &fbdo, String path);
void update_data(FirebaseData &fbdo, FirebaseJson &updateData, int b);
void read_data(FirebaseJson &json1, String b, int c);
void delete_data(FirebaseData &fbdo,String path);



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

    //Set the size of HTTP response buffers in the case where we want to work with large data.
    fbdo.setResponseSize(1024);
    while (!Serial); // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin(); // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522
    mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));


}




//sending data to remote database
void set_data(FirebaseData &fbdo, FirebaseJson json1, String path)
{
   Serial.println("------------------------------------");
    Serial.println("Set JSON test.. .");
    String new_path = "/books/" + path;
    if (Firebase.set(fbdo, new_path, json1))
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

//getting data from remote database
String get_data(FirebaseData &fbdo, String path)
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
        return fbdo.dataType();
    }
    else
    {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
        return "error";
    }
}

//Deleting data from node
void delete_data(FirebaseData &fbdo,String path){
  Firebase.deleteNode(fbdo, "/books/"+path);
  Serial.println("Deleted Node");
}

//seting data into json1 object
void read_data(FirebaseJson &json1, String book_tag, int user_name)
{
    
    String jsonStr = "";

    json1.set("Book tag", book_tag);
    json1.set("Status", 1);
    json1.set("Location", "A1");
    json1.set("Name",user_name);

    Serial.println("------------------------------------");
    Serial.println("JSON Data");
    json1.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println("------------------------------------");
}


void update_data(FirebaseData &fbdo,FirebaseJson &updateData, int data)
{
    updateData.set("Status",data);
    Firebase.updateNode(fbdo, "/books/Witcher", updateData);

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


String read_card(){
  Serial.println("processing");
  Serial.print(" UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  return content;
  
  }


  
void loop()
{


  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }

  String card_no = read_card();

 if(get_data(fbdo,"/books/"+card_no)== "null"){
  Serial.println();
  Serial.println("Enter roll number:");
    while(Serial.available()==0);
    user_name = Serial.parseInt();
  read_data(json1, card_no,user_name);
  Serial.println("Sending data to firebase");
  set_data(fbdo,json1,card_no);
 }else{
  Serial.println("Thank you for returning the book");
  delete_data(fbdo,card_no);
 }
  
  
  


}
