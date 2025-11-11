#ifndef __eeprom_programmer_wiring_h__
#define __eeprom_programmer_wiring_h__

namespace EepromProgrammerWiring {

enum WiringType : int {
  DIP28 = 1,
  DIP24 = 2,
  DIP28_SHIFT = 3,
  DIP24_SHIFT = 4
};

enum ChipType : int {
  AT28C64 = 1,
  UNKNOWN = 1000
};

ChipType str_to_chip_type(const String& chip_type) {
  String _chip_type = chip_type;
  _chip_type.toUpperCase();
  if (_chip_type == "AT28C64") {
    return ChipType::AT28C64;
  }
  return ChipType::UNKNOWN;
}

typedef uint8_t PIN_NO;


// ========================================
// DIP28 WIRING
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

const PIN_NO DIP28_WIRING[28] = {
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
  0,   // 14 / GND
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
  0,   // 28 / VCC
};


// ========================================
// Chip WIRING
// ========================================

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

namespace AT28C64_Wiring {
static const size_t ADDRESS_BUS_SIZE = 13;
static const PIN_NO ADDRESS_BUS_PINS[ADDRESS_BUS_SIZE] = { 10, 9, 8, 7, 6, 5, 4, 3, 25, 24, 21, 23, 2 };
static const size_t DATA_BUS_SIZE = 8;
static const PIN_NO DATA_BUS_PINS[DATA_BUS_SIZE] = { 11, 12, 13, 15, 16, 17, 18, 19 };
static const size_t MANAGEMENT_SIZE = 4;
static const PIN_NO MANAGEMENT_PINS[MANAGEMENT_SIZE] = { 20, 22, 27, 1 };  // !CE, !OE, !WE, !BSY
}


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

class AT28C256 {
  static const size_t ADDRESS_BUS_SIZE = 15;
  static const PIN_NO ADDRESS_BUS_PINS[ADDRESS_BUS_SIZE] = { 10, 9, 8, 7, 6, 5, 4, 3, 25, 24, 21, 23, 2, 26, 1 };
  static const size_t DATA_BUS_SIZE = 8;
  static const PIN_NO DATA_BUS_PINS[DATA_BUS_SIZE] = { 11, 12, 13, 15, 16, 17, 18, 19 };
  static const size_t MANAGEMENT_SIZE = 4;
  static const PIN_NO MANAGEMENT_PINS[MANAGEMENT_SIZE] = { 20, 22, 27, 0 };  // !CE, !OE, !WE, [!BSY]
};


class WiringController {
public:
  static const size_t MAX_BOARD_BUS_SIZE = 28;    // DIP28
  static const size_t MAX_ADDRESS_BUS_SIZE = 15;  // AT28C256: A0 to A14
  static const size_t MAX_DATA_BUS_SIZE = 8;      // AT28C256: I/O0 to I/O7
  static const size_t MAX_MANAGEMENT_SIZE = 4;    // CE, OE, WE, BSY

  WiringController(const WiringType wiring_type)
    : _wiring_type(wiring_type) {}

  void set_chip_type(const ChipType chip_type) {
    _chip_type = chip_type;
  }
  ChipType get_chip_type() {
    return _chip_type;
  }

  size_t get_board_bus_pins(PIN_NO* pins_array, const size_t array_size) {
    size_t board_bus_size = 0;
    PIN_NO* board_bus_pins = 0;

    switch (_wiring_type) {
      case WiringType::DIP28:
        board_bus_size = 28;
        board_bus_pins = DIP28_WIRING;
        break;
      default:
        break;
    }

    if (board_bus_size == 0 || board_bus_pins == 0) {
      return -1;
    }
    if (array_size < board_bus_size) {
      return -1;
    }

    // memcpu
    for (size_t i = 0; i < board_bus_size; i++) {
      pins_array[i] = board_bus_pins[i];
    }
    return board_bus_size;
  }

  size_t get_address_bus_pins(PIN_NO* pins_array, const size_t array_size) {
    size_t address_bus_size = 0;
    PIN_NO* address_bus_pins = 0;
    PIN_NO* dip_wiring_mapping = 0;

    switch (_wiring_type) {
      case WiringType::DIP28:
        dip_wiring_mapping = DIP28_WIRING;
        switch (_chip_type) {
          case ChipType::AT28C64:
            address_bus_size = AT28C64_Wiring::ADDRESS_BUS_SIZE;
            address_bus_pins = AT28C64_Wiring::ADDRESS_BUS_PINS;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }

    if (dip_wiring_mapping == 0) {
      return -1;
    }
    if (address_bus_size == 0 || address_bus_pins == 0) {
      return -1;
    }
    if (array_size < address_bus_size) {
      return -1;
    }

    // mapping
    for (size_t i = 0; i < address_bus_size; i++) {
      // mapping starts from 0, but PIN numbers start from 1 for convenience
      pins_array[i] = dip_wiring_mapping[address_bus_pins[i] - 1];
    }
    return address_bus_size;
  }

  size_t get_data_bus_pins(PIN_NO* pins_array, const size_t array_size) {
    size_t data_bus_size = 0;
    PIN_NO* data_bus_pins = 0;
    PIN_NO* dip_wiring_mapping = 0;

    switch (_wiring_type) {
      case WiringType::DIP28:
        dip_wiring_mapping = DIP28_WIRING;
        switch (_chip_type) {
          case ChipType::AT28C64:
            data_bus_size = AT28C64_Wiring::DATA_BUS_SIZE;
            data_bus_pins = AT28C64_Wiring::DATA_BUS_PINS;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }

    if (dip_wiring_mapping == 0) {
      return -1;
    }
    if (data_bus_size == 0 || data_bus_pins == 0) {
      return -1;
    }
    if (array_size < data_bus_size) {
      return -1;
    }

    // mapping
    for (size_t i = 0; i < data_bus_size; i++) {
      // mapping starts from 0, but PIN numbers start from 1 for convenience
      pins_array[i] = dip_wiring_mapping[data_bus_pins[i] - 1];
    }
    return data_bus_size;
  }

  size_t get_management_pins(PIN_NO* pins_array, const size_t array_size) {
    size_t management_size = 0;
    PIN_NO* management_pins = 0;
    PIN_NO* dip_wiring_mapping = 0;

    switch (_wiring_type) {
      case WiringType::DIP28:
        dip_wiring_mapping = DIP28_WIRING;
        switch (_chip_type) {
          case ChipType::AT28C64:
            management_size = AT28C64_Wiring::MANAGEMENT_SIZE;
            management_pins = AT28C64_Wiring::MANAGEMENT_PINS;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }

    if (dip_wiring_mapping == 0) {
      return -1;
    }
    if (management_size == 0 || management_pins == 0) {
      return -1;
    }
    if (array_size < management_size) {
      return -1;
    }

    // mapping
    for (size_t i = 0; i < management_size; i++) {
      // mapping starts from 0, but PIN numbers start from 1 for convenience
      pins_array[i] = dip_wiring_mapping[management_pins[i] - 1];
    }
    return management_size;
  }

private:
  WiringType _wiring_type;
  ChipType _chip_type;
};

}  // EepromProgrammerWiring

#endif  // !__eeprom_programmer_wiring_h__