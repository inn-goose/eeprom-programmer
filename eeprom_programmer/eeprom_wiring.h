#ifndef __eeprom_wiring_h__
#define __eeprom_wiring_h__

// ========================================
// DIP28 to ARDUINO
// ========================================

// 29  --  1 --|    |-- 28 -- VCC
// 31  --  2 --|    |-- 27 -- 22
// 33  --  3 --|    |-- 26 -- 24
// 35  --  4 --|    |-- 25 -- 26
// 37  --  5 --|    |-- 24 -- 28
// 39  --  6 --|    |-- 23 -- 30
// 41  --  7 --|    |-- 22 -- 32
// 43  --  8 --|    |-- 21 -- 34
// 45  --  9 --|    |-- 20 -- 36
// 47  -- 10 --|    |-- 19 -- 38
// 49  -- 11 --|    |-- 18 -- 40
// 51  -- 12 --|    |-- 17 -- 42
// 53  -- 13 --|    |-- 16 -- 44
// GND -- 14 --|    |-- 15 -- 46

const uint8_t DIP28_TO_ARDUINO_PINS[] = {
  // left side, 1-14, top-down
  29,  // 1
  31,  // 2
  33,  // 3
  35,  // 4
  37,  // 5
  39,  // 6
  41,  // 7
  43,  // 8
  45,  // 9
  47,  // 10
  49,  // 11
  51,  // 12
  53,  // 13
  -1,  // 14 / GND
  // right side, 15-28, bottom-up
  46,  // 15
  44,  // 16
  42,  // 17
  40,  // 18
  38,  // 19
  36,  // 20
  34,  // 21
  32,  // 22
  30,  // 23
  28,  // 24
  26,  // 25
  24,  // 26
  22,  // 27
  -1,  // 28 / VCC
};

// ========================================
// EEPROM Wiring Info
// ========================================

class EepromWiringInfo {};

// AT28C64 / DIP28

// 1  -- | !BSY  VCC |-- VCC
// 2  -- | A12   !WE |-- 27
// 3  -- | A7    xNC |-- 26
// 4  -- | A6     A8 |-- 25
// 5  -- | A5     A9 |-- 24
// 6  -- | A4    A11 |-- 23
// 7  -- | A3    !OE |-- 22
// 8  -- | A2    A10 |-- 21
// 9  -- | A1    !CE |-- 20
// 10 -- | A0    IO7 |-- 19
// 11 -- | IO0   IO6 |-- 18
// 12 -- | IO1   IO5 |-- 17
// 13 -- | IO2   IO4 |-- 16
// 14 -- | GND   IO3 |-- 15

class AT28C64 : public EepromWiringInfo {
  // static const uint8_t ADDRESS_PINS[] = { 10, 9, 8, 7, 6, 5, 4, 3, 25, 24, 21, 23, 2 };
  // static const uint8_t DATA_PINS[] = { 11, 12, 13, 15, 16, 17, 18, 19 };
  // static const uint8_t MANAGEMENT_PINS[] = { 20, 22, 27, 1 };  // !CE, !OE, !WE, [!BSY]
  // static const uint8_t NC_PINS[] = { 26 };
};

// AT28C256 / DIP28

// 1  -- | A14  VCC |-- VCC
// 2  -- | A12   !WE |-- 27
// 3  -- | A7    A13 |-- 26
// 4  -- | A6     A8 |-- 25
// 5  -- | A5     A9 |-- 24
// 6  -- | A4    A11 |-- 23
// 7  -- | A3    !OE |-- 22
// 8  -- | A2    A10 |-- 21
// 9  -- | A1    !CE |-- 20
// 10 -- | A0    IO7 |-- 19
// 11 -- | IO0   IO6 |-- 18
// 12 -- | IO1   IO5 |-- 17
// 13 -- | IO2   IO4 |-- 16
// 14 -- | GND   IO3 |-- 15

class AT28C256 : public EepromWiringInfo {
  // static const uint8_t ADDRESS_PINS[] = { 10, 9, 8, 7, 6, 5, 4, 3, 25, 24, 21, 23, 2, 26, 1 };
  // static const uint8_t DATA_PINS[] = { 11, 12, 13, 15, 16, 17, 18, 19 };
  // static const uint8_t MANAGEMENT_PINS[] = { 20, 22, 27 };  // !CE, !OE, !WE, [!BSY]
  // static const uint8_t NC_PINS[] = {};
};


// ========================================
// !!! REDEFINE FOR YOUR WIRING SCHEME
// ========================================

// PIN -- !BSY -- |    | -- VCC  -- VCC
// PIN --  A12 -- |    | -- !WE  -- PIN
// PIN --   A7 -- |    | -- NC   -- –––
// PIN --   A6 -- |    | -- A8   -- PIN
// PIN --   A5 -- |    | -- A9   -- PIN
// PIN --   A4 -- |    | -- A11  -- PIN
// PIN --   A3 -- |    | -- !OE  -- PIN
// PIN --   A2 -- |    | -- A10  -- PIN
// PIN --   A1 -- |    | -- !CE  -- PIN
// PIN --   A0 -- |    | -- IO7  -- PIN
// PIN --  IO0 -- |    | -- IO6  -- PIN
// PIN --  IO1 -- |    | -- IO5  -- PIN
// PIN --  IO2 -- |    | -- IO4  -- PIN
// GND --  GND -- |    | -- IO3  -- PIN

// wire pins from 0 to N
// const uint8_t EEPROM_ADDRESS_PINS[] = { A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12 };
// const uint8_t EEPROM_DATA_PINS[] = { IO0, IO1, IO2, IO3, IO4, IO5, IO6, IO7 };
// const uint8_t EEPROM_CHIP_ENABLE_PIN = CE;
// const uint8_t EEPROM_OUTPUT_ENABLE_PIN = OE;
// const uint8_t EEPROM_WRITE_ENABLE_PIN = WE;
// const uint8_t EEPROM_READY_BUSY_OUTPUT_PIN = BSY;

// ========================================
// MEGA
// ========================================

// 29 -- !BSY -- |    | -- VCC  -- VCC
// 31 --  A12 -- |    | -- !WE  -- 22
// 33 --   A7 -- |    | -- NC   -- 24
// 35 --   A6 -- |    | -- A8   -- 26
// 37 --   A5 -- |    | -- A9   -- 28
// 39 --   A4 -- |    | -- A11  -- 30
// 41 --   A3 -- |    | -- !OE  -- 32
// 43 --   A2 -- |    | -- A10  -- 34
// 45 --   A1 -- |    | -- !CE  -- 36
// 47 --   A0 -- |    | -- IO7  -- 38
// 49 --  IO0 -- |    | -- IO6  -- 40
// 51 --  IO1 -- |    | -- IO5  -- 42
// 53 --  IO2 -- |    | -- IO4  -- 44
// GND -- GND -- |    | -- IO3  -- 46

const uint8_t EEPROM_ADDRESS_PINS[] = { 47, 45, 43, 41, 39, 37, 35, 33, 26, 28, 34, 30, 31 };
const uint8_t EEPROM_DATA_PINS[] = { 49, 51, 53, 46, 44, 42, 40, 38 };
const uint8_t EEPROM_CHIP_ENABLE_PIN = 36;
const uint8_t EEPROM_OUTPUT_ENABLE_PIN = 32;
const uint8_t EEPROM_WRITE_ENABLE_PIN = 22;
const uint8_t EEPROM_READY_BUSY_OUTPUT_PIN = 29;
const uint8_t NON_CONNECTED_PINS[] = { 24 };

#endif  // !__eeprom_wiring_h__