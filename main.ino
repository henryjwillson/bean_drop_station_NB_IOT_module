
#include "ATOM_DTU_NB.h"
#include "M5Atom.h"

ATOM_DTU_NB DTU;

String readstr;
String ServerIP = "191.165.1.110";
int command_success = 0; // 0 = ready for new command, 1 last command completed successfully, 0 last command failure

void setup()
{
    Serial.begin(9600);
    Serial.print("Initialising Serial");
    M5.begin();
    //SIM7020
    DTU.Init(&Serial2, 19, 22);
    // DTU.Init();
    //Reset Module
    DTU.sendMsg("AT+CRESET\r\n");
    delay(5000);

    DTU.sendMsg("AT+CSMINS=?\r\n");
    readstr = DTU.waitMsg(1000);
    Serial.print(readstr);

    while(1){
        DTU.sendMsg("AT+CSQ\r\n\r\n");
        readstr = DTU.waitMsg(1000);
        Serial.print(readstr);
        if(readstr.indexOf("0,0") ==-1){
            break;
        }
    }

    bool network_connection = false;
    while (network_connection == false){
      DTU.sendMsg("AT+CREG?\r\n");
      readstr = DTU.waitMsg(2000);
      Serial.print(readstr);
      if (readstr.indexOf("0,5") > 0){
        network_connection = true;
        Serial.println("Connection found");
        Serial.println(network_connection);
        DTU.sendMsg("AT+CENG?\r\n");
        readstr = DTU.waitMsg(8000);
        while (readstr.indexOf("ERROR") > 0){
          Serial.println("Not yet found");
          Serial.println(readstr);
          readstr = DTU.waitMsg(2000);
          }
        Serial.print(readstr);
        }
      Serial.println(network_connection);
    }
    

    DTU.sendMsg("AT+COPS=?\r\n");
    int loop_timer = 1;
    while (loop_timer < 5){
      readstr = DTU.waitMsg(1000);
      Serial.print(readstr);
      loop_timer += 1;
      }

    DTU.sendMsg("AT+COPS?\r\n");
    readstr = DTU.waitMsg(2000);
    Serial.print(readstr);

    // Connect to APN
    DTU.sendMsg("AT+CSTT=\"iot.1nce.net\",\"\",\"\"\r");
    readstr = DTU.waitMsg(2000);
    Serial.print(readstr);

    //Bring up wireless connection
    DTU.sendMsg("AT+CIICR\r\n");
    readstr = DTU.waitMsg(2000);
    Serial.print(readstr);

    //Get my networks IP Address
    String IP_address;
    DTU.sendMsg("AT+CIFSR\r\n");
    IP_address = DTU.waitMsg(2000);
    Serial.print(IP_address);
    //Pinging IP Address on new OpenVPN Server
    DTU.sendMsg("AT+CIPPING=\"10.55.0.111\"\r\n");
    readstr = DTU.waitMsg(1000);
    Serial.print(readstr);

    //Create MQTT connection
    //If succeed, MQTT id will return.
    String MQTT_Connect_MSG; 
    DTU.sendMsg("AT+CMQNEW=\"10.55.0.111\",\"1883\",12000,1024\r\n");
    readstr = DTU.waitMsg(2000);
    Serial.print(readstr);

    DTU.sendMsg("AT+CMQCON=0,3,\"myclient\",600,0,0\r\n");
    readstr = DTU.waitMsg(2000);
    Serial.print(readstr);

    DTU.sendMsg("AT+CMQPUB=0,\"BDS/outbuilding\",0,0,0,24,\"48656C6C6F20576F726C6411\"\r\n");
    readstr = DTU.waitMsg(2000);
    Serial.print(readstr);

    DTU.sendMsg("AT+CMQDISCON=0\r\n");
    readstr = DTU.waitMsg(1000);
    Serial.print(readstr);

}

void loop()
{
    if (Serial.available() > 0) {
    String data1 = Serial.readStringUntil('\n');
    Serial.print("You sent me: ");
    Serial.println(data1);
    returned_cup_msg(data1);
    }
    delay(1000);
}

void returned_cup_msg (String input_data){
  //Create MQTT connection
    //If succeed, MQTT id will return.
    Serial.print("starting mqtt packed in loop");
    String MQTT_Connect_MSG; 
    DTU.sendMsg("AT+CMQNEW=\"10.55.0.111\",\"1883\",12000,1024\r\n");
    readstr = DTU.waitMsg(2000);
    Serial.print(readstr);

    DTU.sendMsg("AT+CMQCON=0,3,\"myclient\",600,0,0\r\n");
    readstr = DTU.waitMsg(1000);
    Serial.print(readstr);

    String Combined_msg;
    String topic = "BDS/outbuilding";
    String rfid_msg = input_data;
    unsigned int lastStringLength = rfid_msg.length();
    String Str_Int_Convert = String(lastStringLength);
    Combined_msg =  ("AT+CMQPUB=0,\"" + topic + "\",2,0,0," + Str_Int_Convert + ",\"" + rfid_msg + "\"\r\n");
    Serial.println(Combined_msg);
    DTU.sendMsg(Combined_msg);

    //Reading Response
    readstr = DTU.waitMsg(2000);
    Serial.print(readstr);

    DTU.sendMsg("AT+CMQDISCON=0\r\n");
    readstr = DTU.waitMsg(1000);
    Serial.print(readstr);
  }
