import sys

from serial_json_rpc import client


class EepromApiClientError(Exception):
    pass


class EepromApiClient:
    _EEPROM_CHIP_SETTINGS = {
        "AT28C64": {
            "memory_size": 8192,
        },
    }

    _READ_PAGE_SIZE = 64
    _WRITE_PAGE_SIZE = 8

    @classmethod
    def _get_eeprom_chip_settings(cls, eeprom_type: str):
        if not eeprom_type:
            raise EepromApiClientError(
                "failed to read data, eeprom_type is unknown")

        eeprom_type = eeprom_type.upper()
        if eeprom_type not in cls._EEPROM_CHIP_SETTINGS:
            raise EepromApiClientError(
                f"unsupported EEPROM type: {eeprom_type}")

        return cls._EEPROM_CHIP_SETTINGS[eeprom_type]

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
    def read_data(cls, json_rpc_client: client.SerialJsonRpcClient, eeprom_type: str) -> bytes:
        chip_settings = cls._get_eeprom_chip_settings(eeprom_type)

        # init READ mode
        try:
            res = json_rpc_client.send_request("init_read", [eeprom_type])
            print(f"init_read: {res}")
        except Exception as ex:
            raise EepromApiClientError(
                f"failed to init READ mode for the {eeprom_type} board with: {ex}")

        memory_size = chip_settings["memory_size"]
        page_size = cls._READ_PAGE_SIZE
        pages_total = int(memory_size / page_size)

        data = []
        for page_no in range(pages_total):
            resp = json_rpc_client.send_request(
                "read_page", [page_size, page_no])
            data += resp

        return data

    @classmethod
    def read_data_to_file(cls, json_rpc_client: client.SerialJsonRpcClient, eeprom_type: str, file_path: str):
        if not file_path:
            raise EepromApiClientError(
                "failed to read data, file_path is empty")

        data = cls.read_data(json_rpc_client, eeprom_type)

        with open(file_path, "wb") as f:
            f.write(bytes(data))

    @classmethod
    def erase_data(cls, json_rpc_client: client.SerialJsonRpcClient, eeprom_type: str, erase_pattern: int):
        chip_settings = cls._get_eeprom_chip_settings(eeprom_type)

        # init WRITE mode
        try:
            res = json_rpc_client.send_request("init_write", [eeprom_type])
            print(f"init_write: {res}")
        except Exception as ex:
            raise EepromApiClientError(
                f"failed to init WRITE mode for the {eeprom_type} board with: {ex}")
        
        memory_size = chip_settings["memory_size"]
        page_size = cls._WRITE_PAGE_SIZE
        pages_total = int(memory_size / page_size)

        write_data = [erase_pattern] * page_size

        for page_no in range(pages_total):
            json_rpc_client.send_request("write_page", [page_size, page_no, write_data])

    @classmethod
    def write_data_to_file(cls, json_rpc_client: client.SerialJsonRpcClient, eeprom_type: str, file_path: str):
        raise EepromApiClientError("write mode is unsupported")
