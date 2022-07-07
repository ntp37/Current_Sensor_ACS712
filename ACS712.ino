//int sensitivity = 66; // for 30A
//int sensitivity = 100; // for 20A
int sensitivity = 185; // for 5A
int adcValue = 0;
int offsetVoltage = 2505;   //ค่า Default คือ 2500
double adcVoltage = 0;
double currentValue = 0;

int buttonState = 0;
int current_status = 0;                                                     //กำหนดสถานะของชิ้นงาน (ถ้าเป็น 0 คือ FAIL,ยังไม่ได้เทสงาน และ 1 คือ งานที่ PASS)

int my_current = 400;                                                       //กำหนดกระแสที่ต้องการเปรียบเทียบในหน่วยของ mA
int delay_turn_on = 500;                                                   //หน่วงเวลาในการจ่ายไฟใน Loop 10นาที delay(600000)
int delay_turn_off = 500;                                                  //หน่วงเวลาในการหยุดจ่ายไฟใน Loop 10นาที delay(600000)

const int currentPin = A0;                                                  //กำหนดรับค่า Analog จากเซนเซอร์ ACS712 ที่ขา A0
int relay = 6;                                                              //กำหนด Relay ที่ขา 6
int buttonPin = 7;                                                          //กำหนด Switch ที่ขา 7
int green_led = 11;                                                         //กำหนด LED สีเขียว ที่ขา 11
int yellow_led = 12;                                                        //กำหนด LED สีเหลือง ที่ขา 12
int red_led = 13;                                                           //กำหนด LED สีแดง ที่ขา 13
 
void setup() {
    Serial.begin(9600);
    pinMode(buttonPin, INPUT);
    pinMode(relay, OUTPUT);
    pinMode(red_led, OUTPUT);
    pinMode(yellow_led, OUTPUT);
    pinMode(green_led, OUTPUT);
}
 
void loop() {
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH) {
        check_current1();                                                                 //ไปที่ฟังก์ชั่นวัดกระแสครั้งที่1 เพื่อคัดแยกชิ้นงาน   
    }
}

void check_current1() {                                                                 //ฟังก์ชั่นวัดกระแสครั้งที่1 เพื่อคัดแยกชิ้นงาน
    digitalWrite(green_led, LOW);
    digitalWrite(red_led, LOW);
    digitalWrite(relay, HIGH);
    delay(1500);
    Serial.println("------ Turn On ------ Check Current 1");
    digitalWrite(relay, HIGH);                                                          //Relay ทำงาน เริ่มทำการวัดกระแส
    delay(1500);
    adcValue = analogRead(currentPin);                                                  //ตัวแปร adcValue รับค่าจากเซนเซอร์ผ่านขา A0 ของ Arduino
    adcVoltage = ((adcValue / 1024.0) * 5000);                                          //กำหนดตัวแปร adcVoltage ให้เป็นสูตรหาค่าแรงดันไฟฟ้าที่ขา VIOUT โดยมีหน่วยเป็น mV
    currentValue = ((adcVoltage - offsetVoltage) / sensitivity) *1000;                  //กำหนดตัวแปร currentValue ให้เป็นสูตรหาค่ากระแสไฟฟ้าโดยมีหน่วยเป็น mA

    Serial.print("Raw Sensor Value = " );
    Serial.print(adcValue);                                                             //แสดงค่า Analog ของตัวแปร adcValue ที่ได้จากเซนเซอร์
    Serial.print("\t | Voltage(V) = ");
    Serial.print(adcVoltage/1000,3);                                                    //แสดงค่าแรงดันไฟฟ้าจากตัวแปร adcVoltage ด้วยทศนิยม 3ตำแหน่ง พร้อมกับแปลงหน่วยจาก mV เป็น V
    Serial.print("\t | Current(mA) = ");
    Serial.println(currentValue,3);                                                     //แสดงค่ากระแสไฟฟ้าจากตัวแปร currentValue ด้วยทศนิยม 3ตำแหน่ง
    delay(1500);

    if (currentValue >= my_current) {                                                   //เช็คเงื่อนไขของกระแส (ตัวอย่าง ถ้ากระแสที่วัดได้มากกว่าหรือเท่ากับ ตัวแปร my_current ชิ้นงานจะเป็น PASS)
      Serial.println("\n");
      Serial.println("... Check Current ---- PASS ...");
      Serial.println("\n");
      current_loop();                                                                   //ถ้าผ่านเงื่อนไขจะไปที่ฟังก์ชั่นวัดกระแส 12 Loop
      
    } else {                                                                            //ถ้าไม่ผ่านเงื่อนไขจะเป็น FAIL
        Serial.println("\n");
        Serial.println("... Check Current ---- FAIL (NG) ...");
        Serial.println("\n");
        digitalWrite(red_led, HIGH);                                                    //LED สีแดงติด เพื่อแสดงว่าชิ้นงาน FAIL
        Serial.println("Red LED --- ON");
        delay(1500);
        digitalWrite(relay, LOW);                                                                     //สั่งหยุดการจ่ายไฟ
        Serial.println("--------------------------------- FAIL ---------------------------------");
        Serial.println("\n");
        Serial.println("END");                                                                        //สิ้นสุดการทำงาน
        Serial.println("\n");
    }
}

void check_current2() {                                                               //ฟังก์ชั่นวัดกระแสครั้งสุดท้ายหลังจากทำงานครบ 12Loop ก่อนสิ้นสุดการทำงาน
    Serial.println("------ Turn On ------ Check Current Final");
    digitalWrite(yellow_led, LOW);                                                    //LED สีเหลืองดับ
    digitalWrite(relay, HIGH);                                                        //relay เปิด เริ่มทำการจ่ายไฟ เพื่อวัดกระแส
    delay(1500);
    
    adcValue = analogRead(currentPin);                                                //ตัวแปร adcValue รับค่าจากเซนเซอร์ผ่านขา A0 ของ Arduino                         
    adcVoltage = ((adcValue / 1024.0) * 5000);                                        //กำหนดตัวแปร adcVoltage ให้เป็นสูตรหาค่าแรงดันไฟฟ้าที่ขา VIOUT โดยมีหน่วยเป็น mV
    currentValue = ((adcVoltage - offsetVoltage) / sensitivity) *1000;                //กำหนดตัวแปร currentValue ให้เป็นสูตรหาค่ากระแสไฟฟ้าโดยมีหน่วยเป็น mA

    Serial.print("Raw Sensor Value = " );
    Serial.print(adcValue);                                                           //แสดงค่า Analog ของตัวแปร adcValue ที่ได้จากเซนเซอร์
    Serial.print("\t | Voltage(V) = ");
    Serial.print(adcVoltage/1000,3);                                                  //แสดงค่าแรงดันไฟฟ้าจากตัวแปร adcVoltage ด้วยทศนิยม 3ตำแหน่ง พร้อมกับแปลงหน่วยจาก mV เป็น V
    Serial.print("\t | Current(mA) = ");
    Serial.println(currentValue,3);                                                   //แสดงค่ากระแสไฟฟ้าจากตัวแปร currentValue ด้วยทศนิยม 3ตำแหน่ง
    delay(1500);

    if (currentValue >= my_current) {                                                 //เช็คเงื่อนไขของกระแส (ตัวอย่าง ถ้ากระแสที่วัดได้มากกว่าหรือเท่ากับ ตัวแปร my_current ชิ้นงานจะเป็น PASS)
      Serial.println("\n");
      Serial.println("... Check Current ---- PASS ...");
      Serial.println("\n");
      
    } else {                                                                          //ถ้าไม่ผ่านเงื่อนไขจะเป็น FAIL
        Serial.println("\n");
        Serial.println("... Check Current ---- FAIL (NG) ...");
        Serial.println("\n");
        current_status = 0;
    }
}

void current_loop() {                                                               //ฟังก์ชั่นวัดกระแส 12 Loop
  for (int i = 1; i <= 12; i++) {                                                   //กำหนด Loop การทำงาน 12Loop ด้วยตัวแปร i
    digitalWrite(yellow_led, LOW);
    Serial.print("******************************* START Loop ");
    Serial.print(i);
    Serial.println(" *******************************");
    Serial.println("Turn On ... Please wait ... 10 Minutes ...");
    digitalWrite(relay, HIGH);                                                      //เปิด Relay เพื่อทำการจ่ายไฟ
    delay(delay_turn_on);                                                           //หน่วงเวลาในการจ่ายไฟ 10นาที delay(600000)
    Serial.println("Time Out....");
    
    delay(1500);
    Serial.println("\n");
    Serial.println("Turn Off ... Please wait ... 10 Minutes ...");
    digitalWrite(relay, LOW);                                                       //ปิด Relay เพื่อหยุดจ่ายไฟ
    digitalWrite(yellow_led, HIGH);                                                 //LED สีเหลืองติด เพื่อแสดงว่ามีการหยุดจ่ายเข้าชิ้นงาน
    Serial.println("Yellow LED --- ON");
    delay(delay_turn_off);                                                          //หน่วงเวลาในการหยุดจ่ายไฟ 10นาที delay(600000)
    Serial.println("Time Out....");
    Serial.println("\n");
    delay(1500);

    Serial.print("------ Turn On ------ Check Current Loop ");
    Serial.println(i);
    digitalWrite(yellow_led, LOW);                                                    //LED สีเหลืองดับ
    digitalWrite(relay, HIGH);                                                        //relay เปิด เริ่มทำการจ่ายไฟอีกครั้ง เพื่อวัดกระแส
    delay(1500);
    
    adcValue = analogRead(currentPin);                                                //ตัวแปร adcValue รับค่าจากเซนเซอร์ผ่านขา A0 ของ Arduino                              
    adcVoltage = ((adcValue / 1024.0) * 5000);                                        //กำหนดตัวแปร adcVoltage ให้เป็นสูตรหาค่าแรงดันไฟฟ้าที่ขา VIOUT โดยมีหน่วยเป็น mV
    currentValue = ((adcVoltage - offsetVoltage) / sensitivity) *1000;                //กำหนดตัวแปร currentValue ให้เป็นสูตรหาค่ากระแสไฟฟ้าโดยมีหน่วยเป็น mA

    Serial.print("Raw Sensor Value = " );
    Serial.print(adcValue);                                                           //แสดงค่า Analog ของกระแสไฟฟ้าที่อ่านค่าได้
    Serial.print("\t | Voltage(V) = ");
    Serial.print(adcVoltage/1000,3);                                                  //แสดงค่าแรงดันไฟฟ้าจากตัวแปร adcVoltage ด้วยทศนิยม 3ตำแหน่ง พร้อมกับแปลงหน่วยจาก mV เป็น V
    Serial.print("\t | Current(mA) = ");
    Serial.println(currentValue,3);                                                   //แสดงค่ากระแสไฟฟ้าจากตัวแปร currentValue ด้วยทศนิยม 3ตำแหน่ง
    delay(1500);

    if (currentValue >= my_current) {                                                 //เช็คเงื่อนไขของกระแส (ตัวอย่าง ถ้ากระแสที่วัดได้มากกว่าหรือเท่ากับ ตัวแปร my_current ชิ้นงานจะเป็น PASS)
      Serial.println("\n");
      Serial.println("... Check Current ---- PASS ...");
      Serial.println("\n");
      current_status = 1;
      
    } else {
      Serial.println("\n");
      Serial.println("... Check Current ---- FAIL (NG) ...");
      Serial.println("\n");
      current_status = 0;
      break;                                                                        //หยุดการทำงานทันทีเมื่อเจองานเสียระหว่างใน Loop
    }  
  }

  if (current_status == 1) {                                                       //ถ้า LED สีแดงไม่ติด
    digitalWrite(yellow_led, LOW);                                                    
    Serial.print("******************************* END 12 LOOP ");
    Serial.println(" *******************************");
    Serial.println("\n");
    check_current2();                                                                 //ฟังก์ชั่นวัดกระแสครั้งสุดท้าย
    if (current_status == 0) {
      goto label;
    }
    Serial.println("\n");
    digitalWrite(relay,LOW);                                                          //Relay สั่งหยุดจ่ายไฟ
    digitalWrite(green_led, HIGH);                                                    //LED สีเขียวติด เพื่อยืนยันว่าชิ้นงาน PASS
    Serial.println("Green LED --- ON");
    Serial.println("--------------------------------- PASS ---------------------------------");
    Serial.println("\n");
    Serial.println("END");                                                            //สิ้นสุดการทำงาน
    Serial.println("\n");  
  }
  
  else {
    label:
    digitalWrite(red_led, HIGH);
    Serial.println("Red LED --- ON");                                             //LED สีแดงติด เพื่อยืนยันว่าชิ้นงานนี้ FAIL จริงๆ
    delay(1500);
    digitalWrite(relay, LOW);                                                     //Relay หยุดการทำงาน
    Serial.println("--------------------------------- FAIL ---------------------------------");
    Serial.println("\n");
    Serial.println("END");
    Serial.println("\n");
  }
}
