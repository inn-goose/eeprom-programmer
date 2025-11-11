from typing import List

from serial_json_rpc import client


class EepromProgrammerClientError(Exception):
    pass


class EepromProgrammerClient:
    _READ_PAGE_SIZE = 64
    _WRITE_PAGE_SIZE = 64

    def __init__(self, json_rpc_client: client.SerialJsonRpcClient):
        self.json_rpc_client = json_rpc_client

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

    def init_chip(self, chip_type: str):
        try:
            chip_settings = self.json_rpc_client.send_request("init_chip", [chip_type])
        except Exception as ex:
            raise EepromProgrammerClientError(
                f"failed to init {chip_type} chip with: {ex}")
        if not chip_settings:
            raise EepromProgrammerClientError(
                f"empty chip settings for {chip_type}")
        self.chip_settings = {
            "memory_size": chip_settings[0],
        }
        print(f"chip settings: {self.chip_settings}")

    def _set_read_mode(self, page_size: int):
        try:
            res = self.json_rpc_client.send_request("set_read_mode", [page_size])
            print(f"set_read_mode: {res}")
        except Exception as ex:
            raise EepromProgrammerClientError(
                f"failed to set READ mode with: {ex}")

    def _read_data(self) -> bytes:
        page_size = self._READ_PAGE_SIZE
        memory_size = self.chip_settings["memory_size"]
        pages_total = int(memory_size / page_size)

        # set READ mode
        self._set_read_mode(page_size)

        read_data = []
        for page_no in range(pages_total):
            resp = self.json_rpc_client.send_request("read_page", [page_no])
            read_data += resp

        return read_data

    def read_data_to_file(self, file_path: str):
        if not file_path:
            raise EepromProgrammerClientError(
                "failed to read data, file_path is empty")

        read_data = self._read_data()

        with open(file_path, "wb") as f:
            f.write(bytes(read_data))

    def _set_write_mode(self, page_size: int):
        try:
            res = self.json_rpc_client.send_request("set_write_mode", [page_size])
            print(f"set_write_mode: {res}")
        except Exception as ex:
            raise EepromProgrammerClientError(
                f"failed to set WRITE mode with: {ex}")
        
    def _write_data(self, write_data: bytes):
        page_size = self._WRITE_PAGE_SIZE
        pages_total = int(len(write_data) / page_size)

        # set WRITE mode
        self._set_write_mode(page_size)

        # convert bytes to array
        write_data = [b for b in write_data]

        for page_no in range(pages_total):
            address = page_no * page_size
            page_data = write_data[address:(address+page_size)]
            self.json_rpc_client.send_request("write_page", [page_no, page_data])


    def erase_data(self, erase_pattern: int):
        if erase_pattern < 0 or erase_pattern > 255:
            raise EepromProgrammerClientError(
                f"failed to erase data, invalid pattern: {erase_pattern}")

        memory_size = self.chip_settings["memory_size"]
        write_data = bytes([erase_pattern] * memory_size)

        self._write_data(write_data)

    def write_data_to_file(self, file_path: str):
        if not file_path:
            raise EepromProgrammerClientError(
                "failed to write data, file_path is empty")
        
        write_data = None
        with open(file_path, "rb") as f:
            write_data = bytes(f.read())
        if not write_data:
            raise EepromProgrammerClientError(
                "failed to write data, source is empty")

        memory_size = self.chip_settings["memory_size"]
        if len(write_data) > memory_size:
            raise EepromProgrammerClientError(
                "failed to write data, source is bigger than the chip memory size")
        
        self._write_data(write_data)
