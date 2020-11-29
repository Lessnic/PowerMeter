#include <Adafruit_INA219.h>
#include <U8x8lib.h>

float avg_mA = 0.0F;
uint64_t measures = 0U;

U8X8_SH1106_128X64_NONAME_HW_I2C display(U8X8_PIN_NONE);
Adafruit_INA219 ina219;

void initDevice(const bool success, const bool addToDisplay, const char *errorMsg)
{
  if (!success)
  {
    Serial.println(errorMsg);

    display.drawString(0, 0, errorMsg);

    while (1)
    {
      delay(10);
    }
  }
}

void setup()
{

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(9600);
  Serial.setTimeout(2000);

  while (!Serial) { }
  Serial.println(F("init"));

  display.setFont(u8x8_font_victoriamedium8_r);
  initDevice(display.begin(), false, "SSD1106 error");
  initDevice(ina219.begin(), true, "INA219 error");

  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{

  Serial.print(F("loop "));

  char time[21];
  uint64_t allSeconds = millis() / 1000U;
  float runHoursWithReminder = (float) allSeconds / 3600.0F;
  uint32_t runHours = allSeconds / 3600U;
  uint32_t secsRemaining = allSeconds % 3600U;
  uint32_t runMinutes = secsRemaining / 60U;
  uint32_t runSeconds = secsRemaining % 60U;
  float shuntvoltage = 0U;
  float busvoltage = 0U;
  float current_mA = 0U;
  float loadvoltage = 0U;
  float power_mW = 0U;
 
  measures++;
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  avg_mA = ((avg_mA * (float) measures + current_mA) / ((float) measures + 1.0F));
  sprintf(time, "%02d:%02d:%02d", (int)runHours, (int)runMinutes, (int)runSeconds);

  Serial.println(String(current_mA) + "mA " + avg_mA + "mA " + power_mW + "mW " + shuntvoltage + "v " + busvoltage + "v " + loadvoltage + "v");

  display.drawString(0U, 0U, (String(avg_mA) + "mA  ").c_str());
  display.drawString(0U, 1U, (String(avg_mA * runHoursWithReminder) + "mAh  ").c_str());
  display.drawString(0U, 2U, (String(current_mA) + "mA  ").c_str());
  display.drawString(0U, 3U, (String(loadvoltage) + "v  ").c_str());
  display.drawString(0U, 4U, (String(time) + "  ").c_str());
  delay(300);
}
