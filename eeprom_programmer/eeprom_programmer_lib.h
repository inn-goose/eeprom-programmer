// TODO: https://docs.arduino.cc/learn/contributions/arduino-creating-library-guide/

#ifndef __eeprom_programmer_lib_h__
#define __eeprom_programmer_lib_h__

// #define _EEPROM_DEBUG_LOGGING
#ifdef _EEPROM_DEBUG_LOGGING
#define _debugPrint(log) Serial.print(log)
#define _debugPrintln(log) Serial.println(log)
#else
#define _debugPrint(log)
#define _debugPrintln(log)
#endif  // _EEPROM_DEBUG_LOGGING

namespace EepromProgrammerLibrary {

// Error Codes

enum ErrorCode : int {
  SUCCESS = 0,
  CHIP_NOT_SUPPORTED = 1,
  ALREADY_INITIALIZED = 2,
  NOT_INITIALIZED = 3,
  INVALID_PAGE_SIZE = 4,
  INVALID_PAGE_NO = 5,
  INVALID_ADDRESS = 6,
  READ_MODE_DISABLED = 7,
  READ_FAILED = 8,
  WRITE_MODE_DISABLED = 9,
  WRITE_FAILED = 10,
  UNKNOWN_ERROR = 1000
};


// EEPROM Programmer

class EepromProgrammer {
public:
  EepromProgrammer(
    // address
    const uint8_t* addressPins,
    // data
    const uint8_t* dataPins,
    // control
    const uint8_t chipEnablePin,
    const uint8_t outputEnablePin,
    const uint8_t writeEnablePin,
    // status
    const uint8_t readyBusyOutputPin,
    // non-connected
    const uint8_t* nonConnectedPins);

  // init
  void init_api();
  ErrorCode init_chip(const String& chip_type);
  inline int get_page_size_bytes() {
    return _page_size_bytes;
  }

  // read
  ErrorCode set_read_mode(const int page_size_bytes);
  ErrorCode read_page(const int page_no, uint8_t* bytes);
  ErrorCode read_byte(const uint16_t address, uint8_t &byte);

  // write
  ErrorCode set_write_mode(const int page_size_bytes);
  ErrorCode write_page(const int page_no, const uint8_t* bytes);
  ErrorCode write_byte(const uint16_t address, const uint8_t data);

  // performance
  int busyStateUsec() {
    return _busyStateUsec;
  }

  // helpers
  static String address_to_binary_string(const uint16_t address) {
    bool bAddress[_EEPROM_28C64_ADDRRESS_BUS_SIZE];
    _addressToBitsArray(address, bAddress);
    String result = "";
    for (int i = 0; i < _EEPROM_28C64_ADDRRESS_BUS_SIZE; i++) {
      // print in reverse order, since the printed A0 should be the last bit
      result += bAddress[_EEPROM_28C64_ADDRRESS_BUS_SIZE - 1 - i] ? 1 : 0;
    }
    return result;
  }

  static String address_to_hex_string(const uint16_t address) {
    char buf[8];
    sprintf(buf, "%08x", address);
    return String(buf);
  }

  static String data_to_hex_string(const uint8_t data) {
    char buf[2];
    sprintf(buf, "%02x", data);
    return String(buf);
  }

private:
  static const int _EEPROM_28C64_ADDRRESS_BUS_SIZE = 13;
  static const int _EEPROM_28C64_DATA_BUS_SIZE = 8;
  static const int _MAX_PAGE_SIZE = 64;

  enum _DataPinsMode {
    DATA_PINS_READ,
    DATA_PINS_WRITE,
  };
  void _changeDataPinsMode(const _DataPinsMode mode);

  // PINS
  // address
  uint8_t _addressPins[_EEPROM_28C64_ADDRRESS_BUS_SIZE];
  // data
  uint8_t _dataPins[_EEPROM_28C64_DATA_BUS_SIZE];
  // control
  uint8_t _chipEnablePin;    // !CE
  uint8_t _outputEnablePin;  // !OE
  uint8_t _writeEnablePin;   // !WE
  // status
  uint8_t _readyBusyOutputPin;  // READY / !BUSY
  // non-connected / up to 4 NC for AT28C16
  uint8_t _nonConnectedPins[4];
  size_t _nonConnectedPinsSize;

  // inner state
  bool _chip_is_ready;
  int _memory_size_bytes;
  int _page_size_bytes;
  bool _read_mode;
  bool _write_mode;

  // performance
  int _busyStateUsec;

  // bit operations
  // Most Significant Bit First ordering
  // { 0,0,0,0,0,0,0,1 } == 1
  // { 1,0,0,0,0,0,0,0 } == 128

  static void _addressToBitsArray(uint16_t address, bool* bitsArray) {
    // Ensure address is within 13-bit range (0 to 8191)
    address &= 0x1FFF;
    for (int i = 0; i < _EEPROM_28C64_ADDRRESS_BUS_SIZE; ++i) {
      // MSB order
      // bitsArray[_EEPROM_28C64_ADDRRESS_BUS_SIZE - 1 - i] = (address >> i) & 1;
      // LSB order
      bitsArray[i] = (address >> i) & 1;
    }
  }

  static void _dataToBitsArray(uint8_t data, bool* bitsArray) {
    // MSB order
    for (int i = 0; i < _EEPROM_28C64_DATA_BUS_SIZE; i++) {
      // MSP order
      // bitsArray[_EEPROM_28C64_DATA_BUS_SIZE - 1 - i] = bitRead(data, i);
      // LSB order
      bitsArray[i] = (data >> i) & 1;
    }
  }

  static uint8_t _bitsArrayToData(bool* bitsArray) {
    // MSB order
    uint8_t data = 0;
    for (int i = 0; i < _EEPROM_28C64_DATA_BUS_SIZE; i++) {
      data = (data << 1) | bitsArray[_EEPROM_28C64_DATA_BUS_SIZE - 1 - i];
    }
    return data;
  }
};

EepromProgrammer::EepromProgrammer(
  // address
  const uint8_t* addressPins,
  // data
  const uint8_t* dataPins,
  // control
  const uint8_t chipEnablePin,
  const uint8_t outputEnablePin,
  const uint8_t writeEnablePin,
  // status
  const uint8_t readyBusyOutputPin,
  // non-connected
  const uint8_t* nonConnectedPins) {
  // address
  memcpy(_addressPins, addressPins, _EEPROM_28C64_ADDRRESS_BUS_SIZE);
  // data
  memcpy(_dataPins, dataPins, _EEPROM_28C64_DATA_BUS_SIZE);
  // control
  _chipEnablePin = chipEnablePin;
  _outputEnablePin = outputEnablePin;
  _writeEnablePin = writeEnablePin;
  // status
  _readyBusyOutputPin = readyBusyOutputPin;
  // non-connected
  _nonConnectedPinsSize = sizeof(nonConnectedPins) / sizeof(nonConnectedPins[0]);
  if (_nonConnectedPinsSize > 0) {
    memcpy(_nonConnectedPins, nonConnectedPins, _nonConnectedPinsSize);
  }

  // inner state
  _chip_is_ready = false;
  _memory_size_bytes = 0;
  _page_size_bytes = 0;
  _read_mode = false;
  _write_mode = false;

  // performance
  _busyStateUsec = 0;
}

void EepromProgrammer::_changeDataPinsMode(const EepromProgrammer::_DataPinsMode mode) {
  if (mode == EepromProgrammer::_DataPinsMode::DATA_PINS_READ) {
    for (int i = 0; i < _EEPROM_28C64_DATA_BUS_SIZE; i++) {
      pinMode(_dataPins[i], INPUT_PULLUP);
    }

  } else if (mode == EepromProgrammer::_DataPinsMode::DATA_PINS_WRITE) {
    for (int i = 0; i < _EEPROM_28C64_DATA_BUS_SIZE; i++) {
      pinMode(_dataPins[i], OUTPUT);
    }
  }
}

void EepromProgrammer::init_api() {
}

ErrorCode EepromProgrammer::init_chip(const String& chip_type) {
  if (_chip_is_ready) {
    return ErrorCode::ALREADY_INITIALIZED;
  }
  String _chip_type = chip_type;
  _chip_type.toUpperCase();
  if (_chip_type != "AT28C64") {
    return ErrorCode::CHIP_NOT_SUPPORTED;
  }
  _chip_is_ready = true;
  // AT28C64 only, hardcoded
  _memory_size_bytes = 8192;

  // set control pins
  pinMode(_chipEnablePin, OUTPUT);
  pinMode(_outputEnablePin, OUTPUT);
  pinMode(_writeEnablePin, OUTPUT);
  // disable all control pins
  digitalWrite(_chipEnablePin, HIGH);
  digitalWrite(_outputEnablePin, HIGH);
  digitalWrite(_writeEnablePin, HIGH);

  // address
  for (int i = 0; i < _EEPROM_28C64_ADDRRESS_BUS_SIZE; i++) {
    pinMode(_addressPins[i], OUTPUT);
  }
  // reset address
  for (int i = 0; i < _EEPROM_28C64_ADDRRESS_BUS_SIZE; i++) {
    digitalWrite(_addressPins[i], 0);
  }

  _changeDataPinsMode(_DataPinsMode::DATA_PINS_READ);

  // status pin / open drain / NC
  pinMode(_readyBusyOutputPin, INPUT_PULLUP);

  // non-connected
  for (int i = 0; i < _nonConnectedPinsSize; i++) {
    // best practice: use INPUT_PULLUP non non-connected pins
    pinMode(_nonConnectedPins[i], INPUT_PULLUP);
    // pinMode(_nonConnectedPins[i], OUTPUT);
    // digitalWrite(_nonConnectedPins[i], LOW);
  }

  return ErrorCode::SUCCESS;
}

ErrorCode EepromProgrammer::set_read_mode(const int page_size_bytes) {
  if (!_chip_is_ready) {
    return ErrorCode::NOT_INITIALIZED;
  }
  if (page_size_bytes < 1 || page_size_bytes > _MAX_PAGE_SIZE) {
    return ErrorCode::INVALID_PAGE_SIZE;
  }
  _read_mode = true;
  _page_size_bytes = page_size_bytes;

  // initial READ waveforms state
  digitalWrite(_chipEnablePin, HIGH);    // off
  digitalWrite(_outputEnablePin, HIGH);  // off
  digitalWrite(_writeEnablePin, HIGH);   // not in use
  // switch data pins to READ mode
  _changeDataPinsMode(_DataPinsMode::DATA_PINS_READ);

  // status pin / open drain / NC
  pinMode(_readyBusyOutputPin, INPUT_PULLUP);

  return ErrorCode::SUCCESS;
}

ErrorCode EepromProgrammer::read_page(const int page_no, uint8_t* bytes) {
  if (!_chip_is_ready) {
    return ErrorCode::NOT_INITIALIZED;
  }
  if (!_read_mode) {
    return ErrorCode::READ_MODE_DISABLED;
  }
  const int max_page_no = _memory_size_bytes / _page_size_bytes;
  if (page_no < 0 || page_no >= max_page_no) {
    return ErrorCode::INVALID_PAGE_NO;
  }

  const int start_address = page_no * _page_size_bytes;
  for (int i = 0; i < _page_size_bytes; i++) {
    uint8_t byte = -1;
    ErrorCode code = read_byte(start_address + i, byte);
    if (code != ErrorCode::SUCCESS) {
      return ErrorCode::READ_FAILED;
    }
    bytes[i] = byte;
  }

  return ErrorCode::SUCCESS;
}

ErrorCode EepromProgrammer::read_byte(const uint16_t address, uint8_t &byte) {
  if (!_chip_is_ready) {
    return ErrorCode::NOT_INITIALIZED;
  }
  if (!_read_mode) {
    return ErrorCode::READ_MODE_DISABLED;
  }
  if (address < 0 || address >= _memory_size_bytes) {
    return ErrorCode::INVALID_ADDRESS;
  }

  bool bData[_EEPROM_28C64_DATA_BUS_SIZE];

  // (0) prepare inputs
  // convert address to bits
  bool bAddress[_EEPROM_28C64_ADDRRESS_BUS_SIZE];
  _addressToBitsArray(address, bAddress);

  // (1) set address
  _debugPrint("(API) R [" + String(address) + "] | addr[LSB]: b");
  for (int i = 0; i < _EEPROM_28C64_ADDRRESS_BUS_SIZE; i++) {
    digitalWrite(_addressPins[i], bAddress[i]);
    _debugPrint(bAddress[i]);
  }

  // (2) chip enable
  digitalWrite(_chipEnablePin, LOW);

  // (3) output enable
  digitalWrite(_outputEnablePin, LOW);

  // (4) !OE to Output Delay (delta between OE and data ready) == 100 ns MAX
  delayMicroseconds(1);  // arduino cannot delay in ns, only us

  // (5) read data
  _debugPrint(" | data[LSB]: b");
  for (int i = 0; i < _EEPROM_28C64_DATA_BUS_SIZE; i++) {
    bData[i] = digitalRead(_dataPins[i]) == HIGH ? 1 : 0;
    _debugPrint(bData[i]);
  }
  _debugPrintln();

  // (6) output disable
  digitalWrite(_outputEnablePin, HIGH);

  // (7) chip disable
  digitalWrite(_chipEnablePin, HIGH);

  // (8) reset address
  for (int i = 0; i < _EEPROM_28C64_ADDRRESS_BUS_SIZE; i++) {
    digitalWrite(_addressPins[i], 0);
  }

  byte = _bitsArrayToData(bData);

  return ErrorCode::SUCCESS;
}

ErrorCode EepromProgrammer::set_write_mode(const int page_size_bytes) {
  if (!_chip_is_ready) {
    return ErrorCode::NOT_INITIALIZED;
  }
  if (page_size_bytes < 1 || page_size_bytes > _MAX_PAGE_SIZE) {
    return ErrorCode::INVALID_PAGE_SIZE;
  }
  _write_mode = true;
  _page_size_bytes = page_size_bytes;

  // initial WRITE waveforms state (!WE controlled)
  digitalWrite(_chipEnablePin, HIGH);    // off
  digitalWrite(_outputEnablePin, HIGH);  // not in use
  digitalWrite(_writeEnablePin, HIGH);   // off

  // switch data pins to WRITE mode
  _changeDataPinsMode(_DataPinsMode::DATA_PINS_WRITE);

  // status pin / open drain / read status
  pinMode(_readyBusyOutputPin, INPUT_PULLUP);

  return ErrorCode::SUCCESS;
}

ErrorCode EepromProgrammer::write_page(const int page_no, const uint8_t* bytes) {
  if (!_chip_is_ready) {
    return ErrorCode::NOT_INITIALIZED;
  }
  if (!_write_mode) {
    return ErrorCode::WRITE_MODE_DISABLED;
  }
  const int max_page_no = _memory_size_bytes / _page_size_bytes;
  if (page_no < 0 || page_no >= max_page_no) {
    return ErrorCode::INVALID_PAGE_NO;
  }

  const int start_address = page_no * _page_size_bytes;
  for (int i = 0; i < _page_size_bytes; i++) {
    ErrorCode code = write_byte(start_address + i, bytes[i]);
    if (code != ErrorCode::SUCCESS) {
      return ErrorCode::WRITE_FAILED;
    }
  }

  return ErrorCode::SUCCESS;
}

ErrorCode EepromProgrammer::write_byte(const uint16_t address, const uint8_t data) {
  if (!_chip_is_ready) {
    return ErrorCode::NOT_INITIALIZED;
  }
  if (!_write_mode) {
    return ErrorCode::WRITE_MODE_DISABLED;
  }
  if (address < 0 || address >= _memory_size_bytes) {
    return ErrorCode::INVALID_ADDRESS;
  }

  // (0) prepare inputs
  // convert address to bits
  bool bAddress[_EEPROM_28C64_ADDRRESS_BUS_SIZE];
  _addressToBitsArray(address, bAddress);
  // convert data to bits
  bool bData[_EEPROM_28C64_DATA_BUS_SIZE];
  _dataToBitsArray(data, bData);

  // (1) set address
  _debugPrint("(API) W [" + String(address) + "] | addr: b");
  for (int i = 0; i < _EEPROM_28C64_ADDRRESS_BUS_SIZE; i++) {
    digitalWrite(_addressPins[i], bAddress[i]);
    _debugPrint(bAddress[i]);
  }

  // (2) chip enable
  digitalWrite(_chipEnablePin, LOW);

  // (3) wrtie enable
  digitalWrite(_writeEnablePin, LOW);

  // (4) write data
  _debugPrint(" | data: b");
  for (int i = 0; i < _EEPROM_28C64_DATA_BUS_SIZE; i++) {
    digitalWrite(_dataPins[i], bData[i]);
    _debugPrint(bData[i]);
  }
  _debugPrintln();

  // (5) wrtie disable (initiates the data flush)
  digitalWrite(_writeEnablePin, HIGH);

  // (6) chip disable
  digitalWrite(_chipEnablePin, HIGH);

  // (7) wait until device switches to !BUSY state
  // Time to Device Busy (delta between WE and !BUSY) == 50 ms MAX (spec)
  delayMicroseconds(1);  // arduino cannot delay in ns, only us
  int currBusyState = digitalRead(_readyBusyOutputPin);

  int busyStateStart = micros();

  // wait until !BUSY state switches to READY state (1 ms MAX)
  // or just wait for the Write Cycle Time MAX
  static const int totalDelayMsec = 1.4 * 1000;  // 1 ms is not enough
  if (currBusyState == LOW) {
    // device is in !BUSY state
    // use the READY/!BUSY pin status to wait for the Write Cycle End
    int prevBusyState = currBusyState;
    static const int attemptDelayMsec = 200;
    for (int i = 0; i < totalDelayMsec / attemptDelayMsec; i++) {
      delayMicroseconds(attemptDelayMsec);
      prevBusyState = currBusyState;
      currBusyState = digitalRead(_readyBusyOutputPin);
      if (prevBusyState == LOW && currBusyState == HIGH) {  // rising edge
        break;
      }
    }
  } else {
    // device not in !BUSY state
    // use generic delay
    delayMicroseconds(totalDelayMsec);
  }

  // (8) reset address
  for (int i = 0; i < _EEPROM_28C64_ADDRRESS_BUS_SIZE; i++) {
    digitalWrite(_addressPins[i], 0);
  }

  _busyStateUsec = micros() - busyStateStart;

  return ErrorCode::SUCCESS;
}

}  // EepromProgrammerLibrary

#endif  // !__eeprom_programmer_lib_h__