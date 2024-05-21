#include <SPI.h> // Include the SPI library for communication
#include <Adafruit_GFX.h> // Include the Adafruit GFX library for display functions
#include <Adafruit_SSD1306.h> // Include the Adafruit SSD1306 library for the OLED display

#define BAUD_RATE 115200 // Define the baud rate for serial communication
#define TIMEOUT_LIMIT 100 // Define the timeout limit for SPI communication

#define NOP 0x00 // Define the no operation command for SPI
#define RD_POS 0x10 // Define the read position command for SPI
#define SET_ZERO_POINT 0x70 // Define the set zero point command for SPI

const int CS = 3; // Define the chip select pin for SPI communication

Adafruit_SSD1306 display(128, 32, &Wire, -1); // Initialize the OLED display with width 128, height 32, using I2C communication

void setup() {
  Serial.begin(BAUD_RATE); // Start serial communication at the defined baud rate
  SPI.begin(); // Initialize the SPI communication
  pinMode(CS, OUTPUT); // Set the chip select pin as an output

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Initialize the OLED display with the I2C address 0x3C
  display.display(); // Display an initial screen buffer on the OLED
  delay(2000); // Wait for 2 seconds
  display.clearDisplay(); // Clear the OLED display
}

void loop() {
  uint8_t data; // Variable to store SPI data
  uint8_t timeoutCounter; // Variable to count timeout occurrences
  uint16_t currentPosition; // Variable to store the current position

  while (true) { // Infinite loop
    timeoutCounter = 0; // Reset the timeout counter

    data = SPIWrite(RD_POS); // Send the read position command via SPI

    while (data != RD_POS && timeoutCounter++ < TIMEOUT_LIMIT) { // Loop until the correct response is received or timeout limit is reached
      data = SPIWrite(NOP); // Send no operation command to keep SPI active
    }

    if (timeoutCounter < TIMEOUT_LIMIT) { // Check if timeout limit was not reached
      currentPosition = (SPIWrite(NOP) & 0x0F) << 8; // Read the high byte of the position and mask the lower 4 bits
      currentPosition |= SPIWrite(NOP); // Read the low byte of the position and combine with the high byte
    } else {
      Serial.println("Error obtaining position. Reset Arduino to restart program."); // Print error message if timeout occurred
      while (true); // Enter an infinite loop to halt the program
    }

    Serial.print("Current position: "); // Print the current position to the serial monitor
    Serial.println(currentPosition); // Print the value of the current position

    display.clearDisplay(); // Clear the OLED display
    display.setTextSize(2); // Set the text size to 2
    display.setTextColor(SSD1306_WHITE); // Set the text color to white
    display.setCursor(0, 0); // Set the cursor position to the top-left corner
    display.print("Position: "); // Print "Position: " on the OLED display
    display.println(currentPosition); // Print the value of the current position on the OLED display
    display.display(); // Update the OLED display with the new data

    delay(1000); // Wait for 1 second before repeating the loop
  }
}

uint8_t SPIWrite(uint8_t sendByte) { // Function to send a byte via SPI and receive a byte
  uint8_t data; // Variable to store the received byte

  digitalWrite(CS, LOW); // Pull the chip select pin low to enable the SPI device
  data = SPI.transfer(sendByte); // Transfer the byte via SPI and receive the response
  digitalWrite(CS, HIGH); // Pull the chip select pin high to disable the SPI device

  delayMicroseconds(10); // Short delay to ensure proper timing
  return data; // Return the received byte
}
