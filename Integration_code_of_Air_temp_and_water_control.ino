#include <SoftwareSerial.h>                    // use the software serial library
SoftwareSerial mySerial(3, 2);              // receive data at pin 3; transmit data at pin 2

int val;
//variable for fan,led, and pump //
int fan = 8;
int led = 7;
int pump = 6;

// This is the code that establishes the water sensor
int water_sensor;
int water_LCL = 150;// the og was 430
int water_UCL = 480;// the og was 580
//

int currentTemp;
int currentTemp_celsius;
int setpoint;
int setpoint_celsius;
int standard_dev = 3.3024027;
int UCL;
int UCL_celsius;
int LCL;
int LCL_celsius;
int Celsius;
int avg_Celsius;
int Totalval = 0;
int avg;


void setup() {
  // put your setup code here, to run once:
  pinMode(fan, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(pump, OUTPUT);
  Serial.begin(9600);
  // This portion is the team name and the status of the fan, led, and pump.
  mySerial.begin(9600); delay(500);           // set data rate to 9600 baud; wait for bootup
  mySerial.write(254);  mySerial.write(1);    // clear screen & move to top left position
  mySerial.write(254);  mySerial.write(128);  // move cursor to row 0, position 3
  mySerial.write("Team 4/HBU Eng");           // write a text string starting at (0,3) and Team Name + HBU
  mySerial.write(254);  mySerial.write(192);  // move cursor to row 1, position 5
  mySerial.write("OFF/OFF/"); // This refers to the status of the fan/led/pump
  mySerial.write(254); mySerial.write(202);
  mySerial.write("OFF");
  ///// End of team, fan, led, and pump portion

  val = analogRead(5);
  Totalval = 0;
  // This gets the 20 measurements and calculate the average room temperature //
  for (int i = 0; i < 20; i++) {
    val = analogRead(5);
    Totalval = Totalval + val;
    Serial.println(val);
    delay(1500);
  }
  ////////////////////////////////////////////////////////////////
  Serial.print("Total Value: "); Serial.print(Totalval);
  delay(1000);


  // This gets the avg temp val in analog and celsius values //

  avg = Totalval / 20;
  Serial.print("Average: "); Serial.println(avg);
  delay(100);

  avg_Celsius = avg / (5.2008 + 18.3);
  Serial.print("Average Analog to Celsius Value: "); Serial.println(avg_Celsius);
  //

  // This gets and shows the setpoint val in both analog and celsius. (setpoint is the avg temp after 20 measures) //
  setpoint = avg + 25;// 25 is the analog equivalent of 5 degrees celsius// don't do the just get the first temperature value in the Serial Monitor
  Serial.print("The setpoint in analog is: "); Serial.println(setpoint);
  setpoint_celsius = avg_Celsius + 5;
  Serial.print("The setpoint in Celsius is: "); Serial.println(setpoint_celsius);
  //

  // This calculates and displays the UCL in both analog and celsius values //
  UCL = setpoint + 10;// probably just add 10 to avoid complication // remove the 3 * stand_dev
  Serial.print("The UCL in analog is: "); Serial.println(UCL);
  UCL_celsius = setpoint_celsius + 10;// probably just add 10 to avoid nonsense and complication// remove the 3*standard_dev
  Serial.print("The UCL in Celsius is: "); Serial.println(UCL);

  //This calculate and displays the LCL in both analog and celsius values //
  LCL = setpoint - 10;// just subtract by 10 // remove 3*standard_dev
  Serial.print("The LCL in analog is: "); Serial.println(LCL);
  LCL_celsius = setpoint_celsius - 10;

  /// This portion refers to the LCL, UCL, and current temperature of the thermistor
  mySerial.write(254); mySerial.write(148);// PLacement of the LCL temp
  mySerial.print(LCL);//Temperature LCL
  mySerial.write(254); mySerial.write(151);
  mySerial.write("/");

  mySerial.write(254); mySerial.write(152);
  mySerial.print(UCL);
  mySerial.write(254); mySerial.write(155);
  mySerial.write("/");
  ////


}

void loop() {
  delay(5000);

  // This gets the current temp every measurement in both analog and celsius values //
  currentTemp = analogRead(5);// change analog pin to anything other than 5
  Serial.print("The current temp in analog is: "); Serial.println(currentTemp);
  // this portion goes in void loop
  mySerial.write(254); mySerial.write(156);
  mySerial.print(currentTemp);

  // this gets the water sensor levels
  water_sensor = analogRead(4);
  Serial.print("H2O level = "); Serial.println(water_sensor);
  mySerial.write(254); mySerial.write(212);
  mySerial.print(water_sensor);




  // This prints the LCL and UCL in the Serial Monitor
  Serial.print("The UCL:"); Serial.println(UCL);
  Serial.print("The LCL:"); Serial.println(LCL);

  // This code turns the fan on if the temp gets too hot or above the UCL // I can probably just put if current temp > UCL, don't put need selsius condition
  if (currentTemp > UCL) {
    Serial.print("Too Hot: FAN ON");
    mySerial.write(254);  mySerial.write(192);  // move cursor to row 1, position 5
    mySerial.write("ON/OFF/"); // This refers to the status of the fan/led/pump
    mySerial.write(254); mySerial.write(202);
    mySerial.write("OFF");
    fan_on();
  }
  else if (currentTemp < LCL ) {
    Serial.print("Too Cold: LED ON");
    mySerial.write(254);  mySerial.write(192);  // move cursor to row 1, position 5
    mySerial.write("OFF/ON/"); // This refers to the status of the fan/led/pump
    mySerial.write(254); mySerial.write(202);
    mySerial.write("OFF");
    led_on();
  }
  else if (water_sensor < water_LCL) {
    Serial.print("Water level low: PUMP ON");
    mySerial.write(254);  mySerial.write(192);  // move cursor to row 1, position 5
    mySerial.write("OFF/OFF/ "); // This refers to the status of the fan/led/pump
    mySerial.write(254); mySerial.write(202);
    mySerial.write(" ON ");
    pump_on();
  }
  // if statement where if the water sensor is above than the UCL 800
  else if (water_sensor > water_UCL) {
    Serial.print("Water Level high: PUMP OFF");
    mySerial.write(254);  mySerial.write(192);  // move cursor to row 1, position 5
    mySerial.write("OFF/OFF/"); // This refers to the status of the fan/led/pump
    mySerial.write(254); mySerial.write(202);
    mySerial.write("OFF");
    pump_off();
  }
  else {
    Serial.println(water_sensor);
    mySerial.write(254); mySerial.write(212);
    mySerial.print(water_sensor);

  }

  delay(500);

}


// Functions for turning the fan and led off and on // ( might have to put delay and LOW for fan and led)
void fan_on() {
  //int fan = 8;
  digitalWrite(led, LOW);
  digitalWrite(pump, LOW);
  digitalWrite(fan, HIGH);


}
void led_on() {
  //int led = 7;
  digitalWrite(fan, LOW);
  digitalWrite(pump, LOW);
  digitalWrite(led, HIGH);


}

void pump_on() {
  // int pump is connected to digital pin 6
  digitalWrite(fan, LOW);
  digitalWrite(led, LOW);
  Serial.println("PUMP ON");
  digitalWrite(pump, HIGH);


}

void pump_off() {
  // int pump is connected to digital pin 6
  Serial.println("Pump OFF");
  digitalWrite(pump, LOW);
}
