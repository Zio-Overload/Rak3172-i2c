#include <Wire.h>                       //enable I2C.

 #define ABP_PERIOD   (1800000)
// #define ABP_PERIOD   (120000)

/*************************************

   LoRaWAN band setting:
     RAK_REGION_EU433
     RAK_REGION_CN470
     RAK_REGION_RU864
     RAK_REGION_IN865
     RAK_REGION_EU868
     RAK_REGION_US915
     RAK_REGION_AU915
     RAK_REGION_KR920
     RAK_REGION_AS923

 *************************************/
#define ABP_BAND     (RAK_REGION_AU915)
#define ABP_DEVADDR  {0x26, 0x0D, 0x0A, 0x7E}
#define ABP_APPSKEY  {0x90, 0x30, 0xF3, 0xEA, 0xFB, 0xD6, 0x09, 0x01, 0xEB, 0xB7, 0xCE, 0xD7, 0xA7, 0x17, 0x21, 0x4B}
#define ABP_NWKSKEY  {0x08, 0x5C, 0xA6, 0x22, 0x4B, 0x9F, 0x5D, 0xD4, 0x87, 0x07, 0x51, 0x4A, 0x56, 0x9E, 0x19, 0x4D}


#define one_byte_read 0x01              //used in a function to read data from the device  
#define two_byte_read 0x02              //used in a function to read data from the device
#define four_byte_read 0x04             //used in a function to read data from the device

#define sen1_i2c_address 0x64
#define sen2_i2c_address 0x65
#define sen3_i2c_address 0x66
#define sen4_i2c_address 0x67
#define sen5_i2c_address 0x68

#define ON_SEN1_PIN PA7
#define ON_SEN2_PIN PB7
#define ON_SEN3_PIN PA5
#define ON_SEN4_PIN PB2
#define ON_SEN5_PIN PA4

#define ON_RGB_PIN PA15
#define DIN_RGB_PIN PB4

#define INT_SEN1_PIN PA1
#define INT_SEN2_PIN PB6
#define INT_SEN3_PIN PA6
#define INT_SEN4_PIN PB5
#define INT_SEN5_PIN PA8

#define ADC_BAT_PIN PA10
#define VM_BAT_PIN PA9
#define WKUP_PIN PA0

union sensor_mem_handler                //declare the use of a union data type
{
  byte i2c_data[4];                     //define a 4 byte array in the union
  unsigned long answ;                   //define an unsigned long in the union
};

volatile uint16_t boot_count = 0;

union sensor_mem_handler move_data;     //declare that we will refer to the union as �move_data�
volatile byte new_reading_s1 = 0;          //a flag to signal when the interrupt pin sees a new reading 
volatile byte new_reading_s2 = 0;          //a flag to signal when the interrupt pin sees a new reading 
volatile byte new_reading_s3 = 0;          //a flag to signal when the interrupt pin sees a new reading 
volatile byte new_reading_s4 = 0;          //a flag to signal when the interrupt pin sees a new reading 
volatile byte new_reading_s5 = 0;          //a flag to signal when the interrupt pin sees a new reading 

volatile byte continuous_mode = 0;      //use to enable / disable continuous readings 

void Scanner (void);

void new_reading_sen1() {                                                    //define the function to go to when a new reading interrupt happens  
  if (continuous_mode) { new_reading_s1 = 1; Serial.println("nra sen1"); }                                       //set a flag to show that a new reading is available
}

void new_reading_sen2() {                                                    //define the function to go to when a new reading interrupt happens  
  if (continuous_mode) { new_reading_s2 = 1; Serial.println("nra sen2"); }                                       //set a flag to show that a new reading is available
}

void new_reading_sen3() {                                                    //define the function to go to when a new reading interrupt happens  
  if (continuous_mode) { new_reading_s3 = 1; Serial.println("nra sen3"); }                                       //set a flag to show that a new reading is available
}

void new_reading_sen4() {                                                    //define the function to go to when a new reading interrupt happens  
  if (continuous_mode) { new_reading_s4 = 1; Serial.println("nra sen4"); }                                       //set a flag to show that a new reading is available
}

void new_reading_sen5() {                                                    //define the function to go to when a new reading interrupt happens  
  if (continuous_mode) { new_reading_s5 = 1; Serial.println("nra sen5"); }                                       //set a flag to show that a new reading is available
}



void recvCallback(SERVICE_LORA_RECEIVE_T * data)
{
  if (data->BufferSize > 0) {
    Serial.println("Something received!");
    for (int i = 0; i < data->BufferSize; i++) {
      Serial.printf("%x", data->Buffer[i]);
    }
    Serial.print("\r\n");
  }
}

void joinCallback(int32_t status)
{
  Serial.printf("Join status: %d\r\n", status);
}

void sendCallback(int32_t status)
{
  if (status == 0) {
    Serial.println("Successfully sent");
  } else {
    Serial.println("Sending failed");
  }
}

void setup()
{
  Serial.begin(115200);
  // Wire.begin();
  
  Serial.println("RAKwireless LoRaWan ABP");
  Serial.println("------------------------------------------------------");

  pinMode(ON_SEN1_PIN, OUTPUT);
  pinMode(ON_SEN2_PIN, OUTPUT);
  pinMode(ON_SEN3_PIN, OUTPUT);
  pinMode(ON_SEN4_PIN, OUTPUT);
  pinMode(ON_SEN5_PIN, OUTPUT);

  
  pinMode(ON_RGB_PIN, OUTPUT);
  pinMode(DIN_RGB_PIN, OUTPUT);

  digitalWrite(ON_SEN1_PIN, LOW);
  digitalWrite(ON_SEN2_PIN, LOW);
  digitalWrite(ON_SEN3_PIN, LOW);
  digitalWrite(ON_SEN4_PIN, LOW);
  digitalWrite(ON_SEN5_PIN, LOW);

  digitalWrite(ON_RGB_PIN, HIGH);
  digitalWrite(DIN_RGB_PIN, LOW);
  
  pinMode(INT_SEN1_PIN, INPUT);
  pinMode(INT_SEN2_PIN, INPUT);
  pinMode(INT_SEN3_PIN, INPUT);
  pinMode(INT_SEN4_PIN, INPUT);
  pinMode(INT_SEN5_PIN, INPUT);


  pinMode(VM_BAT_PIN, INPUT);
  // pinMode(ADC_BAT_PIN, INPUT);
  pinMode(WKUP_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(INT_SEN1_PIN), new_reading_sen1, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(INT_SEN2_PIN), new_reading_sen2, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(INT_SEN3_PIN), new_reading_sen3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INT_SEN4_PIN), new_reading_sen4, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INT_SEN5_PIN), new_reading_sen5, CHANGE);

  
  
/*
  attachInterrupt(digitalPinToInterrupt(INT_SEN1_PIN), new_reading_sen1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INT_SEN2_PIN), new_reading_sen2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INT_SEN3_PIN), new_reading_sen3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INT_SEN4_PIN), new_reading_sen4, CHANGE);
  attachInterrupt(digitalPinToInterrupt(INT_SEN5_PIN), new_reading_sen5, CHANGE);
*/

/*
  digitalWrite(ON_SEN1_PIN, HIGH);
  digitalWrite(ON_SEN2_PIN, HIGH);
  digitalWrite(ON_SEN3_PIN, HIGH);
  digitalWrite(ON_SEN4_PIN, HIGH);
  digitalWrite(ON_SEN5_PIN, HIGH);

  digitalWrite(ON_RGB_PIN, LOW);
*/  
  continuous_mode = 1;

/*
  active_con(sen1_i2c_address,"on");
  active_con(sen2_i2c_address,"on");
  active_con(sen3_i2c_address,"on");
  active_con(sen4_i2c_address,"on");
  active_con(sen5_i2c_address,"on");

  led_con(sen1_i2c_address,"off");
  led_con(sen2_i2c_address,"off");
  led_con(sen3_i2c_address,"off");
  led_con(sen4_i2c_address,"off");
  led_con(sen5_i2c_address,"off");

*/  
  // ABP Device Address MSB first
  uint8_t node_dev_addr[4] = ABP_DEVADDR;
  // ABP Application Session Key
  uint8_t node_app_skey[16] = ABP_APPSKEY;
  // ABP Network Session Key
  uint8_t node_nwk_skey[16] = ABP_NWKSKEY;

  if (!api.lorawan.daddr.set(node_dev_addr, 4)) {
    Serial.printf("LoRaWan ABP - set device addr is incorrect! \r\n");
    api.system.sleep.all();
    return;
  }
  if (!api.lorawan.appskey.set(node_app_skey, 16)) {
    Serial.printf("LoRaWan ABP - set application session key is incorrect! \r\n");
    api.system.sleep.all();
    return;
  }
  if (!api.lorawan.nwkskey.set(node_nwk_skey, 16)) {
    Serial.printf("LoRaWan ABP - set network session key is incorrect! \r\n");
    api.system.sleep.all();
    return;
  }

  if (!api.lorawan.band.set(ABP_BAND)) {
    Serial.printf("LoRaWan ABP - set band is incorrect! \r\n");
    api.system.sleep.all();
    return;
  }
  if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_A)) {
    Serial.printf("LoRaWan ABP - set device class is incorrect! \r\n");
    api.system.sleep.all();
    return;
  }
  if (!api.lorawan.njm.set(RAK_LORA_ABP))  // Set the network join mode to ABP
  {
    Serial.printf("LoRaWan ABP - set network join mode is incorrect! \r\n");
    api.system.sleep.all();
    return;
  }

/*
  uint8_t join_count = 0;
  
  while(api.lorawan.njs.get() == 0)
    { 
     if(join_count>=4) break;
      Serial.print("Waiting for Lorawan join...");
      api.lorawan.join();
      join_count++;
      api.system.sleep.cpu(12000);
    }
*/  
 
 
  if (!api.lorawan.adr.set(false)) {
    Serial.printf("LoRaWan ABP - set adaptive data rate is incorrect! \r\n");
    api.system.sleep.all();
    return;
  }
  if (!api.lorawan.rety.set(0)) {
    Serial.printf("LoRaWan ABP - set retry times is incorrect! \r\n");
    api.system.sleep.all();
    return;
  }
  if (!api.lorawan.cfm.set(0)) {
    Serial.printf("LoRaWan ABP - set confirm mode is incorrect! \r\n");
    api.system.sleep.all();
    return;
  }

  if (!api.lorawan.dr.set(4)) {
    Serial.printf("LoRaWan ABP - set data rate is incorrect! \r\n");
    api.system.sleep.all();
    return;
  }


  /** Check LoRaWan Status*/
  Serial.printf("Duty cycle is %s\r\n", api.lorawan.dcs.get()? "ON" : "OFF");  // Check Duty Cycle status
  Serial.printf("Packet is %s\r\n", api.lorawan.cfm.get()? "CONFIRMED" : "UNCONFIRMED");  // Check Confirm status
  uint8_t assigned_dev_addr[4] = { 0 };
  api.lorawan.daddr.get(assigned_dev_addr, 4);
  Serial.printf("Device Address is %02X%02X%02X%02X\r\n", assigned_dev_addr[0], assigned_dev_addr[1], assigned_dev_addr[2], assigned_dev_addr[3]);  // Check Device Address
  Serial.printf("Uplink period is %ums\r\n", ABP_PERIOD);
  Serial.println("");
  api.lorawan.registerRecvCallback(recvCallback);
  // api.lorawan.registerJoinCallback(joinCallback);
  api.lorawan.registerSendCallback(sendCallback);
}
/*
void uplink_routine(uint8_t* collected_data, uint8_t data_len, uint8_t port)
{   
  Serial.println("Data Packet:");
  for (int i = 0; i < data_len; i++) {
    Serial.printf("%02X ", collected_data[i]);
  }
  Serial.println("");

  if (api.lorawan.send(data_len, collected_data, port, false, 0)) {
    Serial.println("Sending is requested");
  } else {
    Serial.println("Sending failed");
  }
}

*/
void loop()
{

  static uint64_t last = 0;
  static uint64_t elapsed;

  float temp_1=0, temp_2=0, temp_3=0;
  float do_1=0, do_2=0;
  float bat_vol=0;
  int adc_bat;
  
  boot_count++;
  
  if ((millis() - last > ABP_PERIOD) || boot_count == 1) {

  
  uint8_t join_count = 0;
  uint8_t read_count = 0;


if(boot_count==1) {

  uint8_t in_collected_data[32] = { 0 };
  uint8_t in_data_len = 0;
  in_collected_data[in_data_len++] = (uint8_t) 0x00;
  in_collected_data[in_data_len++] = (uint8_t) 0x00;
  in_collected_data[in_data_len++] = (uint8_t) 0x00;
  in_collected_data[in_data_len++] = (uint8_t) 0x00;
  in_collected_data[in_data_len++] = (uint8_t) 0x00;
  in_collected_data[in_data_len++] = (uint8_t) 0x00;
  in_collected_data[in_data_len++] = (uint8_t) 0x00;
  in_collected_data[in_data_len++] = (uint8_t) 0x00;
  in_collected_data[in_data_len++] = (uint8_t) 0x00;
  in_collected_data[in_data_len++] = (uint8_t) 0x00;
  in_collected_data[in_data_len++] = (uint8_t) 0x00;
  in_collected_data[in_data_len++] = (uint8_t) 0x00;
  
  Serial.print("Sending initial messages...");
  while(join_count<3){
    
  if (api.lorawan.send(in_data_len, in_collected_data, 2, false, 0)) {
    Serial.println("Sending is requested");
  } else {
    Serial.println("Sending failed");
    api.system.sleep.cpu(6000);
  }
      api.system.sleep.cpu(12000);
  join_count++;
  }
}

/*
if(boot_count%40==0) {
  Serial.print("Waiting for Lorawan join...");
      api.lorawan.join();
      join_count++;
      api.system.sleep.cpu(12000);
}

if(boot_count%12==0){
  while(api.lorawan.njs.get() == 0)
    { 
     if(join_count>=3) break;
      Serial.print("Waiting for Lorawan join...");
      api.lorawan.join();
      join_count++;
      api.system.sleep.cpu(12000);
    }
}
*/
//  if(api.lorawan.njs.get() == 1) {
  
 
  Wire.begin();
 
  digitalWrite(ON_RGB_PIN, LOW);
  
  digitalWrite(ON_SEN1_PIN, HIGH);
  digitalWrite(ON_SEN2_PIN, HIGH);
  digitalWrite(ON_SEN3_PIN, HIGH);
  digitalWrite(ON_SEN4_PIN, HIGH);
  digitalWrite(ON_SEN5_PIN, HIGH);

  delay(100);

  unsigned long start_reading_time = millis();
  
  active_con(sen1_i2c_address,"on");
  active_con(sen2_i2c_address,"on");
  active_con(sen3_i2c_address,"on");
  active_con(sen4_i2c_address,"on");
  active_con(sen5_i2c_address,"on");
/*
  if(api.lorawan.njs.get() == 0)
    {
      Serial.print("Waiting for Lorawan join...");
      api.lorawan.join();
    }*/

  led_con(sen1_i2c_address,"off");
  led_con(sen2_i2c_address,"off");
  led_con(sen3_i2c_address,"off");
  led_con(sen4_i2c_address,"off");
  led_con(sen5_i2c_address,"off");

// if(boot_count==0) delay(3000);  

  new_reading_s1 = 0;
  while(!new_reading_s1){
    if(millis() - start_reading_time > 2000) break;
    delay(50);
  }
  if(new_reading_s1){
    new_reading_s1=0;
    Serial.println("Read by interrupt sen1");
  }
/*
start_reading_time = millis();
  new_reading_s4 = 0;
  while(!new_reading_s4){
    if(millis() - start_reading_time > 1500) break;
    delay(50);
  }
  if(new_reading_s4){
    new_reading_s4=0;
    Serial.println("Read by interrupt sen4");
  }
*/

/** Packet buffer for sending */
uint8_t collected_data[64];
// if(boot_count==1) calibration_RTD_2(sen1_i2c_address,"t","15.55");
      Serial.print(" -- ");
      reading_RTD_2(sen1_i2c_address, &temp_1);                                            //take a reading
read_count = 0;
while(temp_1<12.5 || temp_1 >25) {
//while(1){
  if(read_count==3) break;
  delay(200);
  start_reading_time = millis();
  new_reading_s1 = 0;
  while(!new_reading_s1){
    if(millis() - start_reading_time > 1500) break;
    delay(50);
  }
  if(new_reading_s1){
    new_reading_s1=0;
    Serial.println("Read by interrupt sen1");
  }
  Serial.print(" -- ");
      reading_RTD_2(sen1_i2c_address, &temp_1); 
  read_count++;
}
      Serial.print(temp_1);
      Serial.println(" °C");

char temp1[10];
    strcpy(temp1, String(temp_1, 2).c_str());
if(temp_1>=13 && temp_1<=25)      compensation(sen2_i2c_address, "t", temp1);

    Serial.print(" -- ");
    reading_RTD_2(sen3_i2c_address, &temp_2);                                            //take a reading
if(temp_2<13 || temp_2 >25) {
  delay(500);
  start_reading_time = millis();
  new_reading_s1 = 0;
  while(!new_reading_s1){
    if(millis() - start_reading_time > 1500) break;
    delay(50);
  }
  if(new_reading_s1){
    new_reading_s1=0;
    Serial.println("Read by interrupt sen1");
  }
  Serial.print(" -- ");
      reading_RTD_2(sen3_i2c_address, &temp_2);  
}

    Serial.print(temp_2);
    Serial.println(" °C");

start_reading_time = millis();
  new_reading_s5=0;
  while(!new_reading_s5){
    if(millis() - start_reading_time > 2000) break;
    delay(50);
  }
  if(new_reading_s5){
    Serial.println("Read by interrupt sen5");
    new_reading_s5=0;
  }
// if(boot_count==1) calibration_RTD_2(sen5_i2c_address,"t","14.55");
    Serial.print(" -- ");
    reading_RTD_2(sen5_i2c_address, &temp_3);                                            //take a reading
read_count=0;
while(temp_3<13 || temp_3 >25) {
//while(1){
  if(read_count==2) break;
  delay(400);
  start_reading_time = millis();
  new_reading_s5 = 0;
  while(!new_reading_s5){
    if(millis() - start_reading_time > 1500) break;
    delay(50);
  }
  if(new_reading_s5){
    new_reading_s5=0;
    Serial.println("Read by interrupt sen5");
  }
  Serial.print(" -- ");
      reading_RTD_2(sen5_i2c_address, &temp_3);
read_count++;  
}
    Serial.print(temp_3);
    Serial.println(" °C");
    
    char temp3[10];
    strcpy(temp3, String(temp_3, 2).c_str()); 
   if(temp_3>=13 && temp_3<=25)  compensation(sen4_i2c_address, "t", temp3);
 
      start_reading_time = millis();
      new_reading_s4=0;
      
  while(!new_reading_s4){
    if(millis() - start_reading_time > 1500) break;
    delay(50);
  }
  if(new_reading_s4) new_reading_s4=0;

/*  
  start_reading_time = millis();
 new_reading_s5 = 0;
 while(!new_reading_s5){
    if(millis() - start_reading_time > 1500) break;
    delay(50);
  }
  if(new_reading_s5) new_reading_s5=0;
*/
    Serial.print(" -- ");
    reading_DO(sen2_i2c_address, &do_1);                                            //take a reading
read_count=0;
while(do_1<=0.01 || do_1 >7) {
  if(read_count==2) break;
  delay(500);
  start_reading_time = millis();
  new_reading_s4 = 0;
  while(!new_reading_s4){
    if(millis() - start_reading_time > 2000) break;
    delay(50);
  }
  if(new_reading_s4){
    new_reading_s4=0;
    Serial.println("Read by interrupt sen4");
  }
  Serial.print(" -- ");
      reading_DO(sen2_i2c_address, &do_1);
      read_count++;  
}    

Serial.print(do_1);
    Serial.println(" mg/ml");


      start_reading_time = millis();
      new_reading_s4=0;
      
  while(!new_reading_s4){
    if(millis() - start_reading_time > 1500) break;
    delay(50);
  }
  if(new_reading_s4) new_reading_s4=0;

    Serial.print(" -- ");
    reading_DO(sen4_i2c_address, &do_2);                                            //take a reading

read_count=0;
while(do_2<=0.01 || do_2 >7) {
  if(read_count==2) break;
  delay(1000);
  start_reading_time = millis();
  new_reading_s4 = 0;
  while(!new_reading_s4){
    if(millis() - start_reading_time > 2000) break;
    delay(50);
  }
  if(new_reading_s4){
    new_reading_s4=0;
    Serial.println("Read by interrupt sen4");
  }
  Serial.print(" -- ");
      reading_DO(sen4_i2c_address, &do_2);
      read_count++;   
}      
Serial.print(do_2);
    Serial.println(" mg/ml");

  
  // pinMode(ADC_BAT_PIN, INPUT);
  
  digitalWrite(ON_RGB_PIN, HIGH);
  digitalWrite(ON_SEN1_PIN, LOW);
  digitalWrite(ON_SEN2_PIN, LOW);
  digitalWrite(ON_SEN3_PIN, LOW);
  digitalWrite(ON_SEN4_PIN, LOW);
  digitalWrite(ON_SEN5_PIN, LOW);

  Wire.end();

  // bat_vol = 4.12;
  
  analogReadResolution(10);
    Serial.print(" -- ");
    adc_bat = analogRead(ADC_BAT_PIN);
    bat_vol = adc_bat*2.0*3.3*1.017/1023.0;
    Serial.print(adc_bat);
    Serial.print("   -   ");
    Serial.print(bat_vol,2);
    Serial.println(" V");
//  pinMode(ADC_BAT_PIN, INPUT_PULLUP);
 
  
  /** Payload of Uplink */
  uint8_t data_len = 0;

  Serial.println(int(temp_1*100));
  Serial.println(int(do_1*100));
  Serial.println(int(temp_2*100));
  Serial.println(int(do_2*100));
  Serial.println(int(temp_3*100));
  Serial.println(int(bat_vol*100));
  
  collected_data[data_len++] = (int)floor(temp_1*100)/256;
  collected_data[data_len++] = (int)floor(temp_1*100)%256;
  collected_data[data_len++] = (int)floor(do_1*100)/256;
  collected_data[data_len++] = (int)floor(do_1*100)%256;
  collected_data[data_len++] = (int)floor(temp_2*100)/256;
  collected_data[data_len++] = (int)floor(temp_2*100)%256;
  collected_data[data_len++] = (int)floor(do_2*100)/256;
  collected_data[data_len++] = (int)floor(do_2*100)%256;
  collected_data[data_len++] = (int)floor(temp_3*100)/256;
  collected_data[data_len++] = (int)floor(temp_3*100)%256;
  collected_data[data_len++] = max(0,(int)floor(bat_vol*100)-256);
    
    // uplink_routine(collected_data, data_len, 1);
  Serial.println("Data Packet:");
  for (int i = 0; i < data_len; i++) {
    Serial.printf("0x%02X ", collected_data[i]);
  }
  Serial.println("");
/*
  if(api.lorawan.njs.get() == 0)
    {
      Serial.print("Waiting for Lorawan join...");
      api.lorawan.join();
      delay(500);
    }*/

 if (api.lorawan.send(data_len, collected_data, 2, false, 0)) {
    Serial.println("Sending is requested");
  } else {
    Serial.println("Sending failed");
  }
// }

    last = millis();
  }
  // pinMode(DIN_RGB_PIN, OUTPUT);
  // pinMode(ADC_BAT_PIN, INPUT_PULLUP);
  
  digitalWrite(ON_SEN1_PIN, LOW);
  digitalWrite(ON_SEN2_PIN, LOW);
  digitalWrite(ON_SEN3_PIN, LOW);
  digitalWrite(ON_SEN4_PIN, LOW);
  digitalWrite(ON_SEN5_PIN, LOW);

  digitalWrite(ON_RGB_PIN, HIGH);
  // digitalWrite(DIN_RGB_PIN, LOW);

  // Wire.end();
   
  // api.system.sleep.cpu(1000);
  Serial.printf("Try sleep %ums..", ABP_PERIOD);
  api.system.sleep.all(ABP_PERIOD);
  Serial.println("Wakeup..");
}


void info(byte bus_address) {                                 //if the command is: i
  const byte device_type_register = 0x00;                     //register to read from 


  i2c_read(bus_address, device_type_register, two_byte_read);              //I2C_read(OEM register, number of bytes to read)   

  Serial.print("device:");
  Serial.println(move_data.i2c_data[1]);                      //print info from register 0x01
  Serial.print("version:");
  Serial.println(move_data.i2c_data[0]);                      //print info from register 0x00
}

void adress_con(byte bus_address, char *data_byte_1, char *data_byte_2) {   //if the command is: adr
                                //adr,? = read the I2C bus address
                                //adr,unlock = unlock bus address registers
                                //adr,lock = lock the bus address registers
                                //adr,new,XXX = change the devices bus address (after device has been unlocked)

  byte new_i2c_address;                                   //used to hold a new bus address
  const byte address_lock = 0x00;                               //used to lock the new bus address register 
  const byte address_unlock_A = 0x55;                           //first part of the unlock code
  const byte address_unlock_B = 0xAA;                           //second part of the unlock code
  const byte address_lock_register = 0x02;                      //register to read / write 
  const byte new_address_register = 0x03;                       //register to read / write 


  if (strcmp(data_byte_1, "?") == 0) {                        //if the command sent was:  
    i2c_read(bus_address, new_address_register, one_byte_read);            //I2C_read(OEM register, number of bytes to read) 
    Serial.print("SMBus/I2C bus_address:");
    Serial.println(move_data.i2c_data[0]);                    //print info from register 0x03
  }

  if (strcmp(data_byte_1, "unlock") == 0) {                       //if the command sent was: "adr,unlock"
    i2c_write_byte(bus_address, address_lock_register, address_unlock_A);      //write the fist part of the unlock command(0x55) to register 0x02 
    i2c_write_byte(bus_address, address_lock_register, address_unlock_B);      //write the fist part of the unlock command(0xAA) to register 0x02 
    i2c_read(bus_address, address_lock_register, one_byte_read);               //read from the lock / unlock register to confirm it is unlocked 
    if (move_data.i2c_data[0] == 0)Serial.println("unlocked");    //if the lock/unlock register is unlocked print "unlocked"
  }

  if (strcmp(data_byte_1, "lock") == 0) {                         //if the command sent was: "adr,lock"
    i2c_write_byte(bus_address, address_lock_register, address_lock);          //write the address lock command to register 0x02  
    i2c_read(bus_address, address_lock_register, one_byte_read);               //read from the lock / unlock register to confirm it is locked 
    if (move_data.i2c_data[0])Serial.println("locked");            //if the lock/unlock register is locked print "locked"
  }

  if (strcmp(data_byte_1, "new") == 0) {                                                  //if the command sent was: "adr,new,xxx" (where xxx is the new I2C address number
    new_i2c_address = atoi(data_byte_2);                                                  //convert the new address number from a string to an int
    if (new_i2c_address < 1 || new_i2c_address > 127)Serial.println("out of range");      //if the number entered is 0 or >127 "out of range"   
    else                                                                                  //however, if the number entered for a new I2c buss address is within range, lets do it
    {
      i2c_read(bus_address, address_lock_register, one_byte_read);                                     //call function on line: xxx to make sure the I2C address register is unlocked 
      if (move_data.i2c_data[0])Serial.println("unlock register first");                  //if the I2C address address register is locked "unlock register first" 
      if (move_data.i2c_data[0] == 0) {                                                   //if the I2C address address register is unlocked
        i2c_write_byte(bus_address, new_address_register, new_i2c_address);                            //write the new I2C address to register 0x03
        bus_address = new_i2c_address;                                                    //update this code with the new I2C address                
        Serial.print("adress changed to:");                                               //print the new I2C address                 
        Serial.println(bus_address);
      }
    }
  }
}

void int_con(byte bus_address, char *data_byte_1) {                                                                 //if the command is: int                                 
                                         //int,? = read the state of the interrupt control register
                                         //int,[high],[low],[inv],[off] = set the interrupt control register 

  const byte interrupt_control_register = 0x04;                                    //register to read / write
  const byte interrupt_control_off = 0x00;                                         //turn interrupt pin off                                   
  const byte interrupt_control_high = 0x02;                                        //interrupt pin high on new reading(not self resetting)
  const byte interrupt_control_low = 0x04;                                         //interrupt pin low on new reading(not self resetting) 
  const byte interrupt_control_invert = 0x08;                                      //interrupt pin high/low on new reading(this is self resetting) 


  if (strcmp(data_byte_1, "?") == 0) {                                           //if the command sent was: "int,?"
    i2c_read(bus_address, interrupt_control_register, one_byte_read);                         //I2C_read(OEM register, number of bytes to read)
    Serial.print("Interrupt control register:");
    Serial.println(move_data.i2c_data[0]);                                       //print info from register 0x04                         
  }

  if (strcmp(data_byte_1, "high") == 0) {                                        //if the command sent was:"int,high"
    i2c_write_byte(bus_address, interrupt_control_register, interrupt_control_high);          //write the interrupt high on new reading command to register 0x04 
    i2c_read(bus_address, interrupt_control_register, one_byte_read);                         //read from the interrupt control register to confirm it is set correctly   
    if (move_data.i2c_data[0] == 2)Serial.println("*ok, pin high on new reading");
  }

  if (strcmp(data_byte_1, "low") == 0) {                                         //if the command sent was: "int,low"
    i2c_write_byte(bus_address, interrupt_control_register, interrupt_control_low);           //write the interrupt low on new reading command to register 0x04
    i2c_read(bus_address, interrupt_control_register, one_byte_read);                         //read from the interrupt control register to confirm it is set correctly 
    if (move_data.i2c_data[0] == 4)Serial.println("*ok, pin low on new reading");
  }


  if (strcmp(data_byte_1, "inv") == 0) {                                         //if the command sent was: "int,inv" 
    i2c_write_byte(bus_address, interrupt_control_register, interrupt_control_invert);        //write the interrupt inverts on new reading command to register 0x04  
    i2c_read(bus_address, interrupt_control_register, one_byte_read);                         //read from the interrupt control register to confirm it is set correctly  
    if (move_data.i2c_data[0] == 8)Serial.println("*ok, pin invert on new reading");
  }

  if (strcmp(data_byte_1, "off") == 0) {                                         //if the command sent was: "int,off"
    i2c_write_byte(bus_address, interrupt_control_register, interrupt_control_off);           //write the interrupt off command to register 0x04   
    i2c_read(bus_address, interrupt_control_register, one_byte_read);                         //read from the interrupt control register to confirm it is set correctly   
    if (move_data.i2c_data[0] == 0)Serial.println("*ok, interrupt off");
  }
}

void led_con(byte bus_address, char *data_byte_1) {                                                //if the command is: led                                    
                                //led,? = read the state of the LED control register
                                //led,[on],[off] = Turn the LED on / off

  const byte led_control_register = 0x05;                         //register to read / write
  const byte led_control_on = 0x01;                               //led on
  const byte led_control_off = 0x00;                              //led off


  if (strcmp(data_byte_1, "?") == 0) {                          //if the command sent was: "led,?"
    Serial.print("LED= ");
    i2c_read(bus_address, led_control_register, one_byte_read);              //I2C_read(OEM register, number of bytes to read)                   
    if (move_data.i2c_data[0])Serial.println("on");             //print info from register 0x05  
    if (move_data.i2c_data[0] == 0)Serial.println("off");       //print info from register 0x05
  }

  if (strcmp(data_byte_1, "on") == 0) {                         //if the command sent was: "led,on"
    i2c_write_byte(bus_address, led_control_register, led_control_on);       //write the led on command
    i2c_read(bus_address, led_control_register, one_byte_read);              //read from the led control register to confirm it is set correctly 
    if (move_data.i2c_data[0] == 1)Serial.println("*LED ON");
  }

  if (strcmp(data_byte_1, "off") == 0) {                        //if the command sent was: "led,off"
    i2c_write_byte(bus_address, led_control_register, led_control_off);      //write the led off command
    i2c_read(bus_address, led_control_register, one_byte_read);              //read from the led control register to confirm it is set correctly 
    if (move_data.i2c_data[0] == 0)Serial.println("*LED off");
  }

}

void active_con(byte bus_address, char *data_byte_1) {                          //if the command is: act
                                  //act,? = read the state of the active / hibernate control register
                                  //act,[on],[off] = take readings  


  const byte active_hibernate_register = 0x06;                    //register to read / write
  const byte active_mode = 0x01;                                  //take readings
  const byte hibernate_mode = 0x00;                               //stop taking readings

  if (strcmp(data_byte_1, "on") == 0) {                 //if the command sent was: "on"
    i2c_write_byte(bus_address, active_hibernate_register, active_mode);     //write the active mode enable command
    i2c_read(bus_address, active_hibernate_register, one_byte_read);         //read from the active / hibernate control register to confirm it is set correctly 
    if (move_data.i2c_data[0] == 1)Serial.println("active");
  }

  if (strcmp(data_byte_1, "off") == 0) {                  //if the command sent was: "off"
    i2c_write_byte(bus_address, active_hibernate_register, hibernate_mode);  //write the active mode disable command
    i2c_read(bus_address, active_hibernate_register, one_byte_read);     //read from the active / hibernate control register to confirm it is set correctly 
    if (move_data.i2c_data[0] == 0)Serial.println("hibernate");
  }
}

bool nra(byte bus_address, char *data_byte_1) {                                                       //if the command is: nra
                                   //nra,? = read the state of the new reading available register
                                   //nra,clr = clear the new reading available register                            

  const byte new_reading_register = 0x07;                              //register to read / write
  const byte reg_clr = 0x00;                                         //clear reading available register 
  bool nra_flag = false;

  if (strcmp(data_byte_1, "?") == 0) {                             //if the command sent was: "nra,?"
    Serial.print("new reading available: ");
    i2c_read(bus_address, new_reading_register, one_byte_read);                 //I2C_read(OEM register, number of bytes to read) 
    if (move_data.i2c_data[0]){
      Serial.println("yes");               //print info from register 0x07
      nra_flag = true;
    }
    if (move_data.i2c_data[0] == 0){
      Serial.println("no");           //print info from register 0x07
      nra_flag = false;    
    }
  }

  if (strcmp(data_byte_1, "clr") == 0) i2c_write_byte(bus_address, new_reading_register, reg_clr); //if the command sent was: "nra,clr" clear the new reading available register     
  return nra_flag;
}

void reading_DO(byte bus_address, float *do_mg_ml) {                                                  //if the command is: r
                                  //r = take a single
                                  //r,1 = take a reading one after the other **interrupt control must be set to "inv" first
                                  //r,0 = stop taking readings one after the other    

  const byte mg_ml_register = 0x22;                                 //register to read
  float oxygen = 0;                                                 //used to hold the new D.O. value

  i2c_read(bus_address, mg_ml_register, four_byte_read);                       //I2C_read(OEM register, number of bytes to read)                  
  oxygen = move_data.answ;                                        //move the 4 bytes read into a float
  oxygen /= 100;                                                  //divide by 100 to get the decimal point 
  *do_mg_ml = oxygen;
  Serial.print("D.O.= ");
  Serial.println(oxygen);                                         //print info from register block
}

void reading_RTD_2(byte bus_address, float *temp_c) {                                                  //if the command is: r
                                  //r = take a single
                                  //r,1 = take a reading one after the other **interrupt control must be set to "inv" first
                                  //r,0 = stop taking readings one after the other    

  const byte temp_c_register = 0x0E;                                 //register to read
  float temperature = 0;                                                 //used to hold the new D.O. value

  i2c_read(bus_address, temp_c_register, four_byte_read);                       //I2C_read(OEM register, number of bytes to read)                  
  temperature = move_data.answ;                                        //move the 4 bytes read into a float
  temperature /= 1000;                                                  //divide by 100 to get the decimal point 
  *temp_c = temperature;
  Serial.print("Temperature(°C) = ");
  Serial.println(temperature);                                         //print info from register block
}

void calibration_DO(byte bus_address, char *data_byte_1) {                                                //if the command is: cal
                                  //cal,? = read the state of the calibration control register
                                  //cal,atm = calibrate to atmosphere 
                                  //cal,0 = calibrate to 0 D.O.
                                  //cal,clr = clear the calibration

  const byte calibration_control_register = 0x08;                     //register to read / write
  const byte calibration_confirmation_register = 0x09;                //register to read
  const byte cal_clear = 0x01;                                        //clear calibration
  const byte atmospheric = 0x02;                                      //calibrate to atmosphere
  const byte zero_DO = 0x03;                                          //calibrate to 0 D.O.


  if (strcmp(data_byte_1, "?") == 0) {                              //if the command sent was: "cal,?"
    Serial.print("calibration status: ");
    i2c_read(bus_address, calibration_confirmation_register, one_byte_read);     //I2C_read(OEM register, number of bytes to read) and print calibration state
    if (move_data.i2c_data[0] == 0)Serial.println("no calibration");
    if (move_data.i2c_data[0] == 1)Serial.println("calibrated to atmosphere");
    if (move_data.i2c_data[0] == 2)Serial.println("calibrated to 0 Dissolved Oxygen");
    if (move_data.i2c_data[0] == 3)Serial.println("calibrated to both atmospheric and 0 Dissolved Oxygen");
  }

  if (strcmp(data_byte_1, "clr") == 0) {                          //if the command sent was: "cal,clr"
    i2c_write_byte(bus_address, calibration_control_register, cal_clear);      //write the calibration clear command to the calibration control register  
    delay(40);                                                    //wait for the calibration event to finish
    i2c_read(bus_address, calibration_confirmation_register, one_byte_read);   //read from the calibration control register to confirm it is set correctly 
    if (move_data.i2c_data[0] == 0)Serial.println("calibration cleard 0");
  }

  if (strcmp(data_byte_1, "atm") == 0) {                          //if the command sent was: "cal,atm" 
    i2c_write_byte(bus_address, calibration_control_register, atmospheric);    //write the calibration command to the calibration control register 
    delay(40);                                                    //wait for the calibration event to finish
    i2c_read(bus_address, calibration_confirmation_register, one_byte_read);   //read from the calibration control register to confirm it is set correctly
    if (move_data.i2c_data[0] == 1 || move_data.i2c_data[0] == 3) Serial.println("calibrated to atmosphere");
  }

  if (strcmp(data_byte_1, "0") == 0) {                            //if the command sent was: "cal,0"
    i2c_write_byte(bus_address, calibration_control_register, zero_DO);        //write the calibration command to the calibration control register 
    delay(40);                                                    //wait for the calibration event to finish  
    i2c_read(bus_address, calibration_confirmation_register, one_byte_read);   //read from the calibration control register to confirm it is set correctly
    if (move_data.i2c_data[0] == 2 || move_data.i2c_data[0] == 3)Serial.println("calibrated 0 Dissolved Oxygen");
  }

}

void calibration_RTD_2(byte bus_address, char *data_byte_1, char *data_byte_2){
  const byte calibration_control_register = 0x08;
  const byte calibration_request_register = 0x0C;
  const byte calibration_confirmation_register = 0x0D;
  const byte cal_clear = 0x01;                                        //clear calibration
  const byte single_point = 0x02;                                      //calibrate 

  float calibration = 0;
  
  if (strcmp(data_byte_1, "?") == 0) {                              //if the command sent was: "comp,?"
    Serial.println("calibration status: ");

    i2c_read(bus_address, calibration_confirmation_register, one_byte_read);      //I2C_read(OEM register, number of bytes to read)  
    if (move_data.i2c_data[0] == 0) Serial.println("no calibration");
    if (move_data.i2c_data[0] == 1)Serial.println("calibrated");
  }

  if (strcmp(data_byte_1, "clr") == 0) {                              //if the command sent was: "comp,?"
    i2c_write_byte(bus_address, calibration_request_register, cal_clear);      //write the calibration clear command to the calibration control register  
    delay(40);                                                    //wait for the calibration event to finish
    i2c_read(bus_address, calibration_confirmation_register, one_byte_read);   //read from the calibration control register to confirm it is set correctly 
    if (move_data.i2c_data[0] == 0)Serial.println("calibration cleard 0");
  }

  if (strcmp(data_byte_1, "t") == 0) {                             //if the command sent was: "comp,t,xxxx" where xxx is the value to be sent
    calibration = atof(data_byte_2);                              //convert the compensation value entered from a string to a float
    calibration *= 1000;                                           //multiply by 100 to remove the decimal point
    move_data.answ = calibration;                                 //move the float to an unsigned long 
    i2c_write_long(bus_address, calibration_control_register, move_data.answ);//write the 4 bytes of the unsigned long to the compensation register
    i2c_write_byte(bus_address, calibration_request_register, single_point);      //write the calibration clear command to the calibration control register  
    delay(40);
    i2c_read(bus_address, calibration_confirmation_register, one_byte_read);   //read from the calibration control register to confirm it is set correctly 
    if (move_data.i2c_data[0] == 1)Serial.println("single point calibration done");  
  }
}

void compensation(byte bus_address, char *data_byte_1, char *data_byte_2) {                                               //if the command is: comp 
                                  //comp,s = set the salinity compensation in microsemines 
                                  //cal,p = set the pressure compensation in kPa  
                                  //cal,t = set the temperature compensation in C 

  const byte active_hibernate_register = 0x06;                        //register to read 
  const byte salinity_confirmation_register = 0x16;                   //register to read 
  const byte pressure_confirmation_register = 0x1A;                   //register to read   
  const byte temperature_confirmation_register = 0x1E;                //register to read 
  const byte salinity_compensation_register = 0x0A;                   //register to write
  const byte pressure_compensation_register = 0x0E;                   //register to write
  const byte temperature_compensation_register = 0x12;                //register to write
  float compensation = 0;                                             //used to hold the new compensation value 


  if (strcmp(data_byte_1, "?") == 0) {                              //if the command sent was: "comp,?"
    Serial.println("compensation:");
    i2c_read(bus_address, active_hibernate_register, one_byte_read);             //I2C_read(OEM register, number of bytes to read)              
    if (move_data.i2c_data[0] == 0)Serial.println("device must be taking readings for compensation registers to update");

    i2c_read(bus_address, salinity_confirmation_register, four_byte_read);      //I2C_read(OEM register, number of bytes to read)  
    compensation = move_data.answ;                                 //move the 4 bytes read into a float
    compensation /= 100;                                           //divide by 100 to get the decimal point  
    Serial.print("salinity= ");
    Serial.println(compensation);                                  //print info from register block  

    i2c_read(bus_address, pressure_confirmation_register, four_byte_read);      //call function on line: xxx
    compensation = move_data.answ;                                 //move the 4 bytes read into a float
    compensation /= 100;                                           //divide by 100 to get the decimal point
    Serial.print("pressure= ");
    Serial.println(compensation);                                  //print info from register block

    i2c_read(bus_address, temperature_confirmation_register, four_byte_read);   //I2C_read(OEM register, number of bytes to read)
    compensation = move_data.answ;                                 //move the 4 bytes read into a float
    compensation /= 100;                                           //divide by 100 to get the decimal point
    Serial.print("temperature= ");
    Serial.println(compensation);                                  //print info from register block
  }

  if (strcmp(data_byte_1, "s") == 0) {                             //if the command sent was: "comp,s,xxxx" where xxx is the value to be sent                                                                       
    compensation = atof(data_byte_2);                              //convert the compensation value entered from a string to a float  
    compensation *= 100;                                           //multiply by 100 to remove the decimal point
    move_data.answ = compensation;                                 //move the float to an unsigned long  
    i2c_write_long(bus_address, salinity_compensation_register, move_data.answ);//write the 4 bytes of the unsigned long to the compensation register  
    Serial.println("done");
  }

  if (strcmp(data_byte_1, "p") == 0) {                             //if the command sent was: "comp,p,xxxx" where xxx is the value to be sent
    compensation = atof(data_byte_2);                              //convert the compensation value entered from a string to a float
    compensation *= 100;                                           //multiply by 100 to remove the decimal point
    move_data.answ = compensation;                                 //move the float to an unsigned long  
    i2c_write_long(bus_address, pressure_compensation_register, move_data.answ);//write the 4 bytes of the unsigned long to the compensation register  
    Serial.println("done");
  }

  if (strcmp(data_byte_1, "t") == 0) {                             //if the command sent was: "comp,t,xxxx" where xxx is the value to be sent
    compensation = atof(data_byte_2);                              //convert the compensation value entered from a string to a float
    compensation *= 100;                                           //multiply by 100 to remove the decimal point
    move_data.answ = compensation;                                 //move the float to an unsigned long 
    i2c_write_long(bus_address, temperature_compensation_register, move_data.answ);//write the 4 bytes of the unsigned long to the compensation register
    Serial.println("done");
  }
}

void i2c_read(byte bus_address, byte reg, byte number_of_bytes_to_read) {                         //used to read 1,2,and 4 bytes: i2c_read(starting register,number of bytes to read)    

  byte i;                                                                                         //counter

  Wire.beginTransmission(bus_address);                                                          //call the device by its ID number
  Wire.write(reg);                                                                              //transmit the register that we will start from
  Wire.endTransmission();                                                                       //end the I2C data transmission
  Wire.requestFrom(bus_address, (byte)number_of_bytes_to_read);                                 //call the device and request to read X bytes
  for (i = number_of_bytes_to_read; i>0; i--) { move_data.i2c_data[i - 1] = Wire.read(); }      //with this code we read multiple bytes in reverse
  Wire.endTransmission();                                                                       //end the I2C data transmission  
  /*
  Serial.println("#*#");
  for (i = 0; i<number_of_bytes_to_read; i++){
    Serial.println(move_data.i2c_data[i]);
  }
  Serial.println(move_data.answ);
  Serial.println("#*#");
  */
}

void i2c_write_byte(byte bus_address, byte reg, byte data) {                                    //used to write a single byte to a register: i2c_write_byte(register to write to, byte data) 

  Wire.beginTransmission(bus_address);                                                          //call the device by its ID number
  Wire.write(reg);                                                                              //transmit the register that we will start from
  Wire.write(data);                                                                             //write the byte to be written to the register 
  Wire.endTransmission();                                                                       //end the I2C data transmission
}

void i2c_write_long(byte bus_address, byte reg, unsigned long data) {                           //used to write a 4 bytes to a register: i2c_write_longe(register to start at,unsigned long data )                     

  int i;                                                                                          //counter

  Wire.beginTransmission(bus_address);                                                          //call the device by its ID number
  Wire.write(reg);                                                                              //transmit the register that we will start from
  for (i = 3; i >= 0; i--) {                                                                    //with this code we write multiple bytes in reverse
    Wire.write(move_data.i2c_data[i]);
  }
  Wire.endTransmission();                                                                       //end the I2C data transmission
}


void Scanner ()
{
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;

  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (Wire.endTransmission () == 0)  // Receive 0 = success (ACK response) 
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);     // PCF8574 7 bit address
      Serial.println (")");
      count++;
    }
  }
  Serial.print ("Found ");      
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (" device(s).");
}
