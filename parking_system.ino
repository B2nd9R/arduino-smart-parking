/**
 * نظام مواقف السيارات الذكي
 * المبرمج: Bandar Aljameely
 * تاريخ آخر تعديل: 12-05-2025
 * 
 * الوصف:
 * نظام إدارة مواقف سيارات ذكي باستخدام الأردوينو
 * يتحكم في بوابات الدخول والخروج ويعرض عدد الأماكن المتاحة
 */

#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

// تعريف الثوابت للنظام
#define TOTAL_SPOTS 3        // إجمالي عدد المواقف
#define GATE_OPEN_ANGLE 90   // زاوية فتح البوابة
#define GATE_CLOSED_ANGLE 0  // زاوية غلق البوابة
#define GATE_OPEN_TIME 3000  // مدة فتح البوابة بالميلي ثانية
#define REFRESH_RATE 250     // معدل تحديث النظام

// تكوين شاشة LCD (العنوان 0x27، 16 عمود، سطرين)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// تكوين السيرفو لبوابتي الدخول والخروج
Servo entryGate;  // بوابة الدخول
Servo exitGate;   // بوابة الخروج

// تعريف منافذ الأجهزة وفقاً للرسم التخطيطي والقطع المتوفرة
const int IR_SPOT1 = 2;     // حساس المكان الأول (IR1)
const int IR_SPOT2 = 3;     // حساس المكان الثاني (IR2)
const int IR_SPOT3 = 4;     // حساس المكان الثالث (IR3)
const int IR_ENTRY = 5;     // حساس بوابة الدخول (IR4)
const int IR_EXIT = 6;      // حساس بوابة الخروج (IR5)
const int ENTRY_SERVO = 8;  // سيرفو بوابة الدخول (ser1)
const int EXIT_SERVO = 9;   // سيرفو بوابة الخروج (ser2)
const int BUZZER_PIN = 7;   // منفذ جرس الإنذار (اختياري)

// مصفوفة حساسات المواقف للتسهيل
const int parkingSensors[] = {IR_SPOT1, IR_SPOT2, IR_SPOT3};

// متغيرات النظام
int availableSpots = TOTAL_SPOTS;
unsigned long entryGateCloseTime = 0;  // وقت إغلاق بوابة الدخول
unsigned long exitGateCloseTime = 0;   // وقت إغلاق بوابة الخروج
bool isEntryGateOpen = false;
bool isExitGateOpen = false;
bool wasParkedFull = false;             // لتتبع تغيير الحالة

/**
 * تهيئة النظام
 */
void setup() {
  Serial.begin(9600);  // بدء الاتصال التسلسلي للتصحيح
  Serial.println(F("نظام مواقف السيارات الذكي - تم البدء"));
  
  // تهيئة شاشة LCD
  lcd.begin(16, 2);
  lcd.backlight();
  showWelcomeMessage();
  
  // ربط وتهيئة السيرفو
  entryGate.attach(ENTRY_SERVO);
  exitGate.attach(EXIT_SERVO);
  closeAllGates();
  
  // إعداد المنافذ كمداخل أو مخارج
  for (int i = 0; i < TOTAL_SPOTS; i++) {
    pinMode(parkingSensors[i], INPUT_PULLUP);  // تفعيل المقاومة الداخلية للسحب لأعلى
  }
  
  pinMode(IR_ENTRY, INPUT_PULLUP);
  pinMode(IR_EXIT, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);  // التأكد من إطفاء الجرس
  
  // فحص أولي للمواقف
  updateParkingStatus();
}

/**
 * عرض رسالة ترحيبية على الشاشة
 */
void showWelcomeMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Parking");
  lcd.setCursor(0, 1);
  lcd.print("System v1.2");
  delay(2000);
  
  // عرض معلومات المبرمج
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("By: Bandar");
  lcd.setCursor(0, 1);
  lcd.print("Aljameely");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing");
  lcd.setCursor(0, 1);
  lcd.print("System...");
  delay(1000);
  lcd.clear();
}

/**
 * الحلقة الرئيسية المستمرة
 */
void loop() {
  // تحديث حالة المواقف
  updateParkingStatus();
  
  // التحقق من حساسات البوابات
  checkEntrySensor();
  checkExitSensor();
  
  // التحقق من وقت إغلاق البوابات إذا كانت مفتوحة
  checkGatesAutoClose();
  
  // تأخير قصير لتقليل استهلاك المعالج
  delay(REFRESH_RATE);
}

/**
 * تحديث حالة المواقف وعرضها
 */
void updateParkingStatus() {
  int occupied = 0;
  
  // فحص جميع حساسات المواقف
  for (int i = 0; i < TOTAL_SPOTS; i++) {
    // LOW يعني وجود سيارة (الحساس IR يرسل LOW عند اكتشاف عائق)
    bool isOccupied = (digitalRead(parkingSensors[i]) == LOW);
    
    if (isOccupied) {
      occupied++;
      Serial.print(F("Spot "));
      Serial.print(i + 1);
      Serial.println(F(": Occupied"));
    }
  }
  
  // حساب الأماكن المتاحة
  availableSpots = TOTAL_SPOTS - occupied;
  
  // تحديث الشاشة بالمعلومات الجديدة
  updateDisplay(occupied);
  
  // تفعيل الجرس عند امتلاء المواقف (فقط عند تغير الحالة)
  bool isParkingFull = (availableSpots == 0);
  if (isParkingFull && !wasParkedFull) {
    soundBuzzer(3);  // 3 صفارات للإشعار بالامتلاء
    wasParkedFull = true;
  } 
  else if (!isParkingFull && wasParkedFull) {
    wasParkedFull = false;  // إعادة تعيين المتغير عند توفر مكان
    soundBuzzer(1);  // صفارة واحدة عند توفر مكان جديد
  }
}

/**
 * تحديث معلومات الشاشة
 */
void updateDisplay(int occupied) {
  // إظهار عدد الأماكن المتاحة في السطر الأول
  lcd.setCursor(0, 0);
  lcd.print("Available: ");
  lcd.print(availableSpots);
  lcd.print("/");
  lcd.print(TOTAL_SPOTS);
  lcd.print("  ");  // مسافات إضافية لمسح أي بقايا
  
  // إظهار حالة المواقف في السطر الثاني
  lcd.setCursor(0, 1);
  
  if (availableSpots == 0) {
    lcd.print("PARKING FULL     ");
  } 
  else if (availableSpots == TOTAL_SPOTS) {
    lcd.print("ALL SPOTS EMPTY  ");
  } 
  else {
    lcd.print("Occupied: ");
    lcd.print(occupied);
    lcd.print("      ");
  }
}

/**
 * فحص حساس بوابة الدخول
 */
void checkEntrySensor() {
  // فتح البوابة فقط إذا وصلت سيارة وهناك أماكن متاحة والبوابة مغلقة حالياً
  if (digitalRead(IR_ENTRY) == LOW && availableSpots > 0 && !isEntryGateOpen) {
    openEntryGate();
  }
}

/**
 * فحص حساس بوابة الخروج
 */
void checkExitSensor() {
  // فتح بوابة الخروج فقط إذا وصلت سيارة للخروج والبوابة مغلقة حالياً
  if (digitalRead(IR_EXIT) == LOW && !isExitGateOpen) {
    openExitGate();
  }
}

/**
 * فتح بوابة الدخول
 */
void openEntryGate() {
  entryGate.write(GATE_OPEN_ANGLE);
  isEntryGateOpen = true;
  entryGateCloseTime = millis() + GATE_OPEN_TIME;
  
  // تحديث الشاشة مؤقتاً لإظهار الحالة
  lcd.setCursor(0, 1);
  lcd.print("Entry Gate: OPEN ");
  
  Serial.println(F("بوابة الدخول: فتح"));
  soundBuzzer(1);  // صفارة واحدة للإشعار بالفتح
}

/**
 * فتح بوابة الخروج
 */
void openExitGate() {
  exitGate.write(GATE_OPEN_ANGLE);
  isExitGateOpen = true;
  exitGateCloseTime = millis() + GATE_OPEN_TIME;
  
  // تحديث الشاشة مؤقتاً لإظهار الحالة
  lcd.setCursor(0, 1);
  lcd.print("Exit Gate: OPEN  ");
  
  Serial.println(F("بوابة الخروج: فتح"));
  soundBuzzer(1);  // صفارة واحدة للإشعار بالفتح
}

/**
 * التحقق من الإغلاق التلقائي للبوابات
 */
void checkGatesAutoClose() {
  unsigned long currentTime = millis();
  
  // إغلاق بوابة الدخول بعد انقضاء المدة
  if (isEntryGateOpen && currentTime >= entryGateCloseTime) {
    entryGate.write(GATE_CLOSED_ANGLE);
    isEntryGateOpen = false;
    Serial.println(F("بوابة الدخول: إغلاق تلقائي"));
    updateParkingStatus();  // تحديث الشاشة بعد إغلاق البوابة
  }
  
  // إغلاق بوابة الخروج بعد انقضاء المدة
  if (isExitGateOpen && currentTime >= exitGateCloseTime) {
    exitGate.write(GATE_CLOSED_ANGLE);
    isExitGateOpen = false;
    Serial.println(F("بوابة الخروج: إغلاق تلقائي"));
    updateParkingStatus();  // تحديث الشاشة بعد إغلاق البوابة
  }
}

/**
 * إغلاق جميع البوابات
 */
void closeAllGates() {
  entryGate.write(GATE_CLOSED_ANGLE);
  exitGate.write(GATE_CLOSED_ANGLE);
  isEntryGateOpen = false;
  isExitGateOpen = false;
  Serial.println(F("تم إغلاق جميع البوابات"));
}

/**
 * تشغيل الجرس لعدد محدد من المرات
 */
void soundBuzzer(int beeps) {
  for (int i = 0; i < beeps; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    if (i < beeps - 1) {
      delay(100);  // فاصل بين الصفارات
    }
  }
}