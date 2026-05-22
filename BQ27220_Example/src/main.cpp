#include <Arduino.h>
#include <SparkFunBQ27220.h>
#include <Wire.h>

/*
  Compared to BQ27441, Full Charge Capacity & Design Capacity Editing is Quite Different.
  BQ27220 Checksum Calculation Seems to Be Different, Requires Data Length Parameter as Well (Which is Not Properly Documented).
  The UNSEAL Key is also different while the datasheet mentioned the one on BQ27441.

  Got More Info from Ti's forums, and the most useful is attached below
  https://e2e.ti.com/support/power-management-group/power-management/f/power-management-forum/722828/bq27220-bq27220-can-not-modify-the-design-capacity/2672165
*/

// Set Battery Full Charge Capacity to the design capacity of your battery.
#define BATTERY_CAPCITY_FULL 1050

void printBatteryStats();

void setup()
{
  Serial.begin(115200);

  pinMode(GPIO_NUM_25, INPUT);
  delay(100);

  if (!lipo.begin())
  {
    Serial.println("Error: Unable to communicate with BQ27220.");
    while (1)
      ;
  }
  else
  {
    Serial.println("Connected to BQ27441!");
  }

  unsigned int _capacity_full = lipo.capacity(FULL);

  if (_capacity_full != BATTERY_CAPCITY_FULL)
  {
    Serial.print("Wrong Capacity -> ");
    Serial.print(_capacity_full);
    Serial.print("mAh");
    Serial.println();

    Serial.print("Setting Capacity -> ");
    Serial.print(BATTERY_CAPCITY_FULL);
    Serial.print("mAh");
    Serial.println();

    lipo.enterConfig(true);
    lipo.setCapacity(BATTERY_CAPCITY_FULL);
    lipo.exitConfig(true);
  }
  else
  {
    Serial.print("Battery Capacity -> ");
    Serial.print(_capacity_full);
    Serial.print("mAh");
    Serial.println();
  }
}

void loop()
{
  delay(1000);
  printBatteryStats();
}

void printBatteryStats()
{
  // Read battery stats from the BQ27441-G1A
  unsigned int soc = lipo.soc();                   // Read state-of-charge (%)
  unsigned int volts = lipo.voltage();             // Read battery voltage (mV)
  int current = lipo.current(AVG);                 // Read average current (mA)
  unsigned int fullCapacity = lipo.capacity(FULL); // Read full capacity (mAh)
  unsigned int capacity = lipo.capacity(REMAIN);   // Read remaining capacity (mAh)
  int power = lipo.power();                        // Read average power draw (mW)
  int health = lipo.soh();                         // Read state-of-health (%)

  // Now print out those values:
  String toPrint = String(soc) + "% | ";
  toPrint += String(volts) + " mV | ";
  toPrint += String(current) + " mA | ";
  toPrint += String(capacity) + " / ";
  toPrint += String(fullCapacity) + " mAh | ";
  toPrint += String(power) + " mW | ";
  toPrint += String(health) + "%";

  Serial.println(toPrint);
}
