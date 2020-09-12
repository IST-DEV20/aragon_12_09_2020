/*
 * rosserial Service Server
 */

//////////////////////////////////////////////////////
////////////////      PIN Install   //////////////////
//////////////////////////////////////////////////////

#define DHT_PIN   2             

#define BUTTON_1_PIN  22
#define BUTTON_2_PIN  23
#define BUTTON_3_PIN  24
#define BUTTON_4_PIN  25

#define EMERGENCY_SWITCH_PIN  20    // Interrupt pin (mega2560 2, 3, 18, 19, 20, 21)
int emergency_state = 0;             // default_state 

#define CLIFF_MIDDLE_PIN  A0        // IR SHARP
#define CLIFF_LEFT_PIN    A1
#define CLIFF_RIGHT_PIN   A2

#define IR_TRIGGER_PIN      3       // MB1242 MAXSONAR
#define IR_FRONT_MIDDLE_PIN A3
#define IR_FRONT_LEFT_PIN   A4
#define IR_FRONT_RIGHT_PIN  A5
#define IR_LEFT_PIN         A6
#define IR_RIGHT_PIN        A7
#define IR_BACK_PIN         A8

#define MOTION_FRONT_MIDDLE_PIN 26
#define MOTION_FRONT_LEFT_PIN   27
#define MOTION_FRONT_RIGHT_PIN  28
#define MOTION_LEFT_PIN         39
#define MOTION_RIGHT_PIN        30
#define MOTION_BACK_PIN         31

#define LED_STRIP_PIN           50



///////////////////////////////////////////////////// aragorn
////////////////////////////////////////////////////



#include <ros.h>
#include <aragorn_msg/ServiceCmd.h>
#include <aragorn_msg/SensorState.h>
#include <aragorn_msg/Emergency.h>



////////////////      DHT22      ///////////////////
#include "DHT.h"
//#define DHT_PIN 2
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);


///////////////       SharpIR    ////////////////////
#include <SharpIR.h>
// #define CLIFF_MIDDLE_PIN A0
// #define CLIFF_LEFT_PIN A1
// #define CLIFF_RIGHT_PIN A2
SharpIR cliff_middle( SharpIR::GP2Y0A41SK0F, CLIFF_MIDDLE_PIN );
SharpIR cliff_left( SharpIR::GP2Y0A41SK0F, CLIFF_LEFT_PIN );
SharpIR cliff_right( SharpIR::GP2Y0A41SK0F, CLIFF_RIGHT_PIN );




ros::NodeHandle  nh;



void LedStatus_callback(const aragorn_msg::ServiceCmd::Request & req, aragorn_msg::ServiceCmd::Response & res)
{
  String cmd_req = String(req.command);

  if(cmd_req == "on")
  {
    digitalWrite(13, HIGH);
    res.status = "led on";
  }

  if(cmd_req == "off")
  {
    digitalWrite(13, LOW);
    res.status = "led off";
  }
}



void UvcLamp_callback(const aragorn_msg::ServiceCmd::Request & req, aragorn_msg::ServiceCmd::Response & res)
{
  String cmd_req = String(req.command);

  if(cmd_req == "on")
  {
    digitalWrite(13, HIGH);
    res.status = "UVC Lamp on";
  }

  if(cmd_req == "off")
  {
    digitalWrite(13, LOW);
    res.status = "UVC Lamp off";
  }
}



/////////////////////////      Srv server        ////////////////////
ros::ServiceServer<aragorn_msg::ServiceCmd::Request, aragorn_msg::ServiceCmd::Response> server_led("/aragorn_io/led_status", &LedStatus_callback);
ros::ServiceServer<aragorn_msg::ServiceCmd::Request, aragorn_msg::ServiceCmd::Response> server_uvc("/aragorn_io/uvc_lamp",     &UvcLamp_callback);


////////////////////////       Srv client        ////////////////////
//ros::ServiceClient<ServiceCmd::Request, ServiceCmd::Response> client_button("/aragorn/button_status");




////////////////////////       Publisher         ///////////////////
aragorn_msg::SensorState sensorstate_msg;
aragorn_msg::Emergency   emergency_msg;
ros::Publisher pub_aragorn_iostate("/aragorn_io/sensor_states", &sensorstate_msg);
ros::Publisher pub_aragorn_emergency("/aragorn_io/emergency", &emergency_msg);



///////////////////////        Subscribe         //////////////////





void setup()
{
  
  Serial.begin(9600);

  dht.begin();

  pinMode(13, OUTPUT);

  pinMode(MOTION_FRONT_MIDDLE_PIN, INPUT);
  pinMode(MOTION_FRONT_LEFT_PIN, INPUT);
  pinMode(MOTION_FRONT_RIGHT_PIN, INPUT);
  pinMode(MOTION_LEFT_PIN, INPUT);
  pinMode(MOTION_RIGHT_PIN, INPUT);
  pinMode(MOTION_BACK_PIN, INPUT);

  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(BUTTON_2_PIN, INPUT);
  pinMode(BUTTON_3_PIN, INPUT);
  pinMode(BUTTON_4_PIN, INPUT);

  pinMode(CLIFF_MIDDLE_PIN, INPUT);
  pinMode(CLIFF_LEFT_PIN, INPUT);
  pinMode(CLIFF_RIGHT_PIN, INPUT);

  pinMode(IR_TRIGGER_PIN, OUTPUT);
  pinMode(IR_FRONT_MIDDLE_PIN, INPUT);
  pinMode(IR_FRONT_LEFT_PIN, INPUT);
  pinMode(IR_FRONT_RIGHT_PIN, INPUT);
  pinMode(IR_LEFT_PIN, INPUT);
  pinMode(IR_RIGHT_PIN, INPUT);
  pinMode(IR_BACK_PIN, INPUT);

  pinMode(EMERGENCY_SWITCH_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(EMERGENCY_SWITCH_PIN), pub_emergency, CHANGE);


  nh.initNode();
  nh.getHardware()->setBaud(9600);
  nh.advertiseService(server_led);
  nh.advertiseService(server_uvc);
  //nh.serviceClient(client_button);
  nh.advertise(pub_aragorn_iostate);
  nh.advertise(pub_aragorn_emergency);


  while(!nh.connected())
  {
    nh.spinOnce();
  }
  nh.loginfo("InterfacesystechRobot I/O connected");
  delay(1);

}








void loop()
{

  sensorstate_msg.led_status    = "GOO";
  sensorstate_msg.uvc_status    = "on";
  
  
  pub_aragorn_iostate.publish( &sensorstate_msg );
  
  nh.spinOnce();
  delay(10);
}




void pub_emergency()
{
  emergency_state = !emergency_state;
  emergency_msg.emergency_state = emergency_state;
  pub_aragorn_emergency.publish( &emergency_msg );
}



void temp_dht22()
{
  float t = dht.readTemperature();
  sensorstate_msg.temperature = t;
}




void cliff_sharpIR()
{
  int cliff_middle_range  = cliff_middle.getDistance();
  int cliff_left_range    = cliff_left.getDistance();
  int cliff_right_range   = cliff_right.getDistance();

  sensorstate_msg.cliff_middle  = cliff_middle_range;
  sensorstate_msg.cliff_left    = cliff_left_range;
  sensorstate_msg.cliff_right   = cliff_right_range;
}




void motion_PIR()
{
  int motion_front_middle = digitalRead(MOTION_FRONT_MIDDLE_PIN);
  int motion_front_left   = digitalRead(MOTION_FRONT_LEFT_PIN);
  int motion_front_right  = digitalRead(MOTION_FRONT_RIGHT_PIN);
  int motion_left         = digitalRead(MOTION_LEFT_PIN);
  int motion_right        = digitalRead(MOTION_RIGHT_PIN);
  int motion_back         = digitalRead(MOTION_BACK_PIN);

  sensorstate_msg.motion_front_middle = motion_front_middle;
  sensorstate_msg.motion_front_left   = motion_front_left;
  sensorstate_msg.motion_front_right  = motion_front_right;
  sensorstate_msg.motion_left         = motion_left;
  sensorstate_msg.motion_right        = motion_right;
  sensorstate_msg.motion_back         = motion_back;
}



void ir_SonarMax()
{

  digitalWrite(IR_TRIGGER_PIN, HIGH);                        // Start sensor
  delay(1);
  digitalWrite(IR_TRIGGER_PIN, LOW);

  int ir_front_middle = analogRead(IR_FRONT_MIDDLE_PIN);     // Read sensor
  int ir_front_left   = analogRead(IR_FRONT_LEFT_PIN);
  int ir_front_right  = analogRead(IR_FRONT_RIGHT_PIN);
  int ir_left         = analogRead(IR_LEFT_PIN);
  int ir_right        = analogRead(IR_RIGHT_PIN);
  int ir_back         = analogRead(IR_BACK_PIN);

  sensorstate_msg.ir_front_middle = ir_front_middle;
  sensorstate_msg.ir_front_left   = ir_front_left;
  sensorstate_msg.ir_front_right  = ir_front_right;
  sensorstate_msg.ir_left         = ir_left;
  sensorstate_msg.ir_right        = ir_right;
  sensorstate_msg.ir_back         = ir_back;
}



void button_state()
{
  int button_1 = digitalRead(BUTTON_1_PIN);
  int button_2 = digitalRead(BUTTON_2_PIN);
  int button_3 = digitalRead(BUTTON_3_PIN);
  int button_4 = digitalRead(BUTTON_4_PIN);

  sensorstate_msg.button_1 = button_1;
  sensorstate_msg.button_2 = button_2;
  sensorstate_msg.button_3 = button_3;
  sensorstate_msg.button_4 = button_4;
}
