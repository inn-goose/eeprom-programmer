#include "eeprom_wiring.h"
#include "eeprom_api.h"
#include "serial_json_rpc.h"

using namespace EepromApiLibrary;
using namespace SerialJsonRpcLibrary;


// EEPROM API

static EepromApi eeprom_api(
  // address
  EEPROM_ADDRESS_PINS,
  // data
  EEPROM_DATA_PINS,
  // management
  EEPROM_CHIP_ENABLE_PIN,
  EEPROM_OUTPUT_ENABLE_PIN,
  EEPROM_WRITE_ENABLE_PIN,
  // status
  EEPROM_READY_BUSY_OUTPUT_PIN,
  // non-connected
  NON_CONNECTED_PINS);


// Serial JSON RPC Processor

static SerialJsonRpcBoard rpc_board(rpc_processor);

void rpc_processor(int request_id, const String &method, const String params[], int params_size) {
  if (method == "init_chip") {
    if (params_size != 1) {
      rpc_board.send_error(request_id, -32602, "Invalid params", "expected: (chip_type)");
      return;
    }
    String chip_type = params[0];

    eeprom_api.init_chip(chip_type);
    rpc_board.send_result_string(request_id, "chip is ready");

  } else if (method == "set_read_mode") {
    if (params_size != 1) {
      rpc_board.send_error(request_id, -32602, "Invalid params", "expected: (read_page_size_bytes)");
      return;
    }
    const int read_page_size_bytes = atoi(params[0].c_str());

    eeprom_api.set_read_mode(read_page_size_bytes);
    rpc_board.send_result_string(request_id, "READ mode is ON");

  } else if (method == "read_page") {
    if (params_size != 1) {
      rpc_board.send_error(request_id, -32602, "Invalid params", "expected: (page_no)");
      return;
    }
    const int page_no = atoi(params[0].c_str());

    const int buffer_size = eeprom_api.get_read_page_size_bytes();
    uint8_t buffer[buffer_size];

    eeprom_api.read_page(page_no, buffer);
    rpc_board.send_result_bytes(request_id, buffer, buffer_size);

  } else if (method == "set_write_mode") {
    if (params_size != 1) {
      rpc_board.send_error(request_id, -32602, "Invalid params", "expected: (write_page_size_bytes)");
      return;
    }
    const int write_page_size_bytes = atoi(params[0].c_str());

    eeprom_api.set_write_mode(write_page_size_bytes);
    rpc_board.send_result_string(request_id, "WRITE mode is ON");

  } else if (method == "write_page") {
    if (params_size != 2) {
      rpc_board.send_error(request_id, -32602, "Invalid params", "expected: (page_no, bytes_to_write)");
      return;
    }
    const int page_no = atoi(params[0].c_str());

    const int buffer_size = eeprom_api.get_write_page_size_bytes();
    uint8_t buffer[buffer_size];
    SerialJsonRpcBoard::json_array_to_byte_array(params[1], buffer, buffer_size);

    eeprom_api.write_page(page_no, buffer);
    rpc_board.send_result_string(request_id, "done");

  } else {
    rpc_board.send_error(request_id, -32601, "Method not found", method.c_str());
  }
}


// Arduino

void setup() {
  // rpc board
  rpc_board.init();
  // eeprom api
  eeprom_api.init_api();
}

void loop() {
  rpc_board.loop();
}
