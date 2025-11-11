from typing import List

from serial_json_rpc import client


class EepromProgrammerClientError(Exception):
    pass


class EepromProgrammerClient:
    _EEPROM_CHIP_SETTINGS = {
        "AT28C64": {
            "memory_size": 8192,
        },
    }

    _READ_PAGE_SIZE = 64
    _WRITE_PAGE_SIZE = 64

    @classmethod
    def _get_eeprom_chip_settings(cls, chip_type: str):
        if not chip_type:
            raise EepromProgrammerClientError(
                "failed to read data, chip_type is unknown")

        chip_type = chip_type.upper()
        if chip_type not in cls._EEPROM_CHIP_SETTINGS:
            raise EepromProgrammerClientError(
                f"unsupported EEPROM type: {chip_type}")

        return cls._EEPROM_CHIP_SETTINGS[chip_type]

    @staticmethod
    def _to_decoded(val):
        if (val >= 33 and val <= 126) or val == 20:
            return chr(val)
        return '.'

    @classmethod
    def _dump_page(cls, page_no, page_size, data):
        address = page_size * page_no
        hex = "".join([f"{r:02x}" + (" " if (n % 2 == 1) else "")
                       for (n, r) in enumerate(data)])
        decoded = "".join([cls._to_decoded(r) for r in data])
        print(f"{address:08x}: {hex}{decoded}")

    @classmethod
    def init_chip(cls, json_rpc_client: client.SerialJsonRpcClient, chip_type: str):
        try:
            res = json_rpc_client.send_request("init_chip", [chip_type])
            print(f"init_chip: {res}")
        except Exception as ex:
            raise EepromProgrammerClientError(
                f"failed to init {chip_type} chip with: {ex}")
        
    @classmethod
    def _set_read_mode(cls, json_rpc_client: client.SerialJsonRpcClient, page_size: int):
        try:
            res = json_rpc_client.send_request("set_read_mode", [page_size])
            print(f"set_read_mode: {res}")
        except Exception as ex:
            raise EepromProgrammerClientError(
                f"failed to set READ mode with: {ex}")

    @classmethod
    def _read_data(cls, json_rpc_client: client.SerialJsonRpcClient, chip_type: str) -> bytes:
        chip_settings = cls._get_eeprom_chip_settings(chip_type)
        page_size = cls._READ_PAGE_SIZE
        memory_size = chip_settings["memory_size"]
        pages_total = int(memory_size / page_size)

        # set READ mode
        cls._set_read_mode(json_rpc_client, page_size)

        read_data = []
        for page_no in range(pages_total):
            resp = json_rpc_client.send_request("read_page", [page_no])
            read_data += resp

        return read_data

    @classmethod
    def read_data_to_file(cls, json_rpc_client: client.SerialJsonRpcClient, chip_type: str, file_path: str):
        if not file_path:
            raise EepromProgrammerClientError(
                "failed to read data, file_path is empty")

        read_data = cls._read_data(json_rpc_client, chip_type)

        with open(file_path, "wb") as f:
            f.write(bytes(read_data))

    @classmethod
    def _set_write_mode(cls, json_rpc_client: client.SerialJsonRpcClient, page_size: int):
        try:
            res = json_rpc_client.send_request("set_write_mode", [page_size])
            print(f"set_write_mode: {res}")
        except Exception as ex:
            raise EepromProgrammerClientError(
                f"failed to set WRITE mode with: {ex}")
        
    @classmethod
    def _write_data(cls, json_rpc_client: client.SerialJsonRpcClient, write_data: bytes):
        page_size = cls._WRITE_PAGE_SIZE
        pages_total = int(len(write_data) / page_size)

        # set WRITE mode
        cls._set_write_mode(json_rpc_client, page_size)

        # convert bytes to array
        write_data = [b for b in write_data]

        for page_no in range(pages_total):
            address = page_no * page_size
            page_data = write_data[address:(address+page_size)]
            json_rpc_client.send_request("write_page", [page_no, page_data])


    @classmethod
    def erase_data(cls, json_rpc_client: client.SerialJsonRpcClient, chip_type: str, erase_pattern: int):
        if erase_pattern < 0 or erase_pattern > 255:
            raise EepromProgrammerClientError(
                f"failed to erase data, invalid pattern: {erase_pattern}")

        chip_settings = cls._get_eeprom_chip_settings(chip_type)
        memory_size = chip_settings["memory_size"]
        write_data = bytes([erase_pattern] * memory_size)

        cls._write_data(json_rpc_client, write_data)

    @classmethod
    def write_data_to_file(cls, json_rpc_client: client.SerialJsonRpcClient, chip_type: str, file_path: str):
        if not file_path:
            raise EepromProgrammerClientError(
                "failed to write data, file_path is empty")
        
        write_data = None
        with open(file_path, "rb") as f:
            write_data = bytes(f.read())
        if not write_data:
            raise EepromProgrammerClientError(
                "failed to write data, source is empty")

        chip_settings = cls._get_eeprom_chip_settings(chip_type)
        memory_size = chip_settings["memory_size"]
        if len(write_data) > memory_size:
            raise EepromProgrammerClientError(
                "failed to write data, source is bigger than the chip memory size")
        
        cls._write_data(json_rpc_client, write_data)
