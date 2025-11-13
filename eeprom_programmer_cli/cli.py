#!/usr/bin/env python3

import argparse
import sys
import time

from core import eeprom_programmer_client
from serial_json_rpc import client


def connect(port: str, baudrate: int, init_timeout: int):
    print(f"connect: {port}")

    ts = time.time()
    json_rpc_client = client.SerialJsonRpcClient(
        port=port, baudrate=baudrate, init_timeout=float(init_timeout))

    init_result = json_rpc_client.init()
    if init_result is not None:
        print(f"connect: response {init_result}")

    elapsed = time.time() - ts
    print(f"connect: success, {elapsed:.02f} sec")

    return json_rpc_client


def init(programmer_client: eeprom_programmer_client.EepromProgrammerClient, device: str):
    print(f"chip init: {device}")

    try:
        programmer_client.init_chip(device)
    except Exception as ex:
        print(f"chip init: failed, {str(ex)}")
        return 1

    print(f"chip init: success")

    return 0


def read(programmer_client: eeprom_programmer_client.EepromProgrammerClient, file_path: str):
    print(f"read data: {file_path}")

    ts = time.time()
    try:
        programmer_client.read_data_to_file(file_path)
    except Exception as ex:
        print(f"read data: failed, {str(ex)}")
        return 1

    elapsed = time.time() - ts
    print(f"read data: success, {elapsed:.02f} sec")

    return 0


def fetch_write(programmer_client: eeprom_programmer_client.EepromProgrammerClient, file_path: str) -> bytes:
    print(f"write data: {file_path}")

    return programmer_client.fetch_write_data_from_file(file_path)


def write(programmer_client: eeprom_programmer_client.EepromProgrammerClient, write_data: bytes, collect_write_performance: bool):
    ts = time.time()

    try:
        programmer_client.write_data_to_file(write_data, collect_write_performance)
    except Exception as ex:
        print(f"write data: failed, {str(ex)}")
        return 1

    elapsed = time.time() - ts
    print(f"write data: success, {elapsed:.02f} sec")

    return 0


def erase(programmer_client: eeprom_programmer_client.EepromProgrammerClient, erase_pattern_str: str, collect_write_performance: bool):
    erase_pattern = 255  # FF
    if erase_pattern_str:
        try:
            erase_pattern = int(erase_pattern_str, 16)
            if erase_pattern < 0 or erase_pattern > 255:
                print(f"invalid erase pattern {erase_pattern_str}, should be within [0, 255] range")
                return 1
        except Exception:
            print(f"invalid erase pattern {erase_pattern_str}, should be a HEX value")
            return 1

    print(f"erase data: 0x{erase_pattern:02X}")

    ts = time.time()

    try:
        programmer_client.erase_data(erase_pattern, collect_write_performance)
    except Exception as ex:
        print(f"erase data: failed, {str(ex)}")
        return 1

    elapsed = time.time() - ts
    print(f"erase data: success, {elapsed:.02f} sec")

    return 0


def cli() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("port", type=str, metavar="<port>", help="Specify the USP port address for the serial connection")
    parser.add_argument("--baudrate", type=int, default=115200, metavar="<baud>", help="Set the serial connection speed")
    parser.add_argument("--init-timeout", type=int, default=3, metavar="<sec>", help="Set the MAX Arduino's reset-on-connect timeout in seconds")
    # parser.add_argument("-l", "--list", type=str, required=False, help="List all supported devices")
    parser.add_argument("-p", "--device", type=str, required=True, metavar="<device>", help="Specify the device type, like AT28C64")
    parser.add_argument("-r", "--read", type=str, required=False, metavar="<filename>", help="Read from the device and write the contents to this file")
    # parser.add_argument("-m", "--verify", type=str, required=False, metavar="<filename>", help="Verify memory in the device against this file")
    parser.add_argument("-w", "--write", type=str, required=False, metavar="<filename>", help="Write to the device using this file")
    parser.add_argument("-e", "--skip-erase", action="store_true", help="Do NOT erase the device")
    # parser.add_argument("-v", "--skip_verify", action="store_true", help="Do NOT verify after write")
    parser.add_argument("-E", "--erase", action="store_true", help="Just erase the device")
    parser.add_argument("--erase-pattern", type=str, required=False, metavar="<hex>", help="Specify the erase pattern, like CC or AA, default: FF")
    parser.add_argument("--collect-write-performance", action="store_true", help="Collect the write operation performance")
    args = parser.parse_args()

    # connect
    json_rpc_client = connect(args.port, args.baudrate, args.init_timeout)

    # init eeprom programmer client
    programmer_client = eeprom_programmer_client.EepromProgrammerClient(json_rpc_client)

    # init chip
    init(programmer_client, args.device)

    if args.read is not None:
        return read(programmer_client, args.read)

    elif args.erase:
        return erase(programmer_client, args.erase_pattern, args.collect_write_performance)

    elif args.write is not None:
        bin_data = fetch_write(programmer_client, args.write)
        if not args.skip_erase:
            erase_ret = erase(programmer_client, args.erase_pattern, args.collect_write_performance)
            if erase_ret != 0:
                return erase_ret
        return write(programmer_client, bin_data, args.collect_write_performance)

    else:
        print("unknown mode")


if __name__ == '__main__':
    sys.exit(cli())
