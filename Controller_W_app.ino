/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
int STBY = 7;//5
int AWave = 30;//6
int BWave = 27;//9
int A1IN = 16;//11
int A2IN = 15;//10
int B1IN = 31;//12
int B2IN = 11;//13

int S = 175;
int deg = 50;


#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

// BLE Service
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // battery

void forwardD(int S)
{
  digitalWrite(STBY, HIGH);
  digitalWrite(A1IN, HIGH);
  digitalWrite(A2IN, LOW);
  digitalWrite(B1IN, HIGH);
  digitalWrite(B2IN, LOW);

}
void backwardD(int S)
{
  digitalWrite(STBY, HIGH);
  digitalWrite(A1IN, LOW);
  digitalWrite(A2IN, HIGH);
  digitalWrite(B1IN, LOW);
  digitalWrite(B2IN, HIGH);

}
void leftD(int S)
{
  digitalWrite(STBY, HIGH);
  digitalWrite(A1IN, HIGH);
  digitalWrite(A2IN, LOW);
  digitalWrite(B1IN, LOW);
  digitalWrite(B2IN, HIGH);

}
void rightD(int S)
{
  digitalWrite(STBY, HIGH);
  digitalWrite(A1IN, LOW);
  digitalWrite(A2IN, HIGH);
 digitalWrite(B1IN, HIGH);
  digitalWrite(B2IN, LOW);

}

void halt()
{
  digitalWrite(STBY, LOW);
}


void setup()
{

  //setup pinouts
 pinMode(STBY,OUTPUT); 
 pinMode(AWave,OUTPUT); 
 pinMode(BWave,OUTPUT); 
 pinMode(A1IN,OUTPUT); 
 pinMode(A2IN,OUTPUT); 
 pinMode(B1IN,OUTPUT); 
 pinMode(B2IN,OUTPUT); 

  
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb
  
  Serial.println("Bluefruit52 BLEUART Example");
  Serial.println("---------------------------\n");

  // Setup the BLE LED to be enabled on CONNECT
  // Note: This is actually the default behaviour, but provided
  // here in case you want to control this LED manually via PIN 19
  Bluefruit.autoConnLed(true);

  // Config the peripheral connection with maximum bandwidth 
  // more SRAM required by SoftDevice
  // Note: All config***() function must be called before begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("Bluefruit52");
  //Bluefruit.setName(getMcuUniqueID()); // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();

  // Start BLE Battery Service
  blebas.begin();
  blebas.write(100);

  // Set up and start advertising
  startAdv();

  Serial.println("Please use Adafruit's Bluefruit LE app to connect in UART mode");
  Serial.println("Once connected, enter character(s) that you wish to send");
}

void startAdv(void)
{
  Serial.println("started advertising"); 
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
  Serial.println("stopped advertising");  
}

void loop()
{
  // Forward data from HW Serial to BLEUART
 /* while (Serial.available())
  {
     Serial.println("Service available");
    // Delay to wait for enough input, since we have a limited transmission buffer
    delay(2);

    uint8_t buf[64];
    int count = Serial.readBytes(buf, sizeof(buf));
    bleuart.write( buf, count );
    Serial.println("Service Found");
  }*/

  // Forward from BLEUART to HW Serial
 if ( bleuart.available() )
  {
    Serial.println("bleuart Available");
   // uint8_t ch;
    int in;
    //ch = (uint8_t) bleuart.read();
    in =  (int) bleuart.read();
    Serial.println(in);
  }
 /*   switch (in) {
  case 5:
  
      Serial.println("forward pressed");
      forwardD(S);
    
    break;
  case 50:
      halt();
    break;
    
  case 6:
  
      Serial.println("backward pressed");
      backwardD(S);

    break;

  case 60:
      halt();
    break;
    
  case 7:
 
      Serial.println("left pressed");
      leftD(S);

    break;

  case 70:
      halt();
    break;
    
  case 8:
  
    Serial.println("right pressed");
      rightD(S);

    break;

  case 80:
      halt();
    break;
    
   }*/
  //}
}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}
