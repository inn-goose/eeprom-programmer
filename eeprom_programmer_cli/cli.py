#!/usr/bin/env python3

import argparse
import sys
import time

from core import eeprom_programmer_client
from serial_json_rpc import client


def read(json_rpc_client, device: str, file_path: str):
    print(f"read data to {file_path}")

    ts = time.time()
    try:
        eeprom_programmer_client.EepromProgrammerClient.read_data_to_file(json_rpc_client, device, file_path)
    except Exception as ex:
        print(f"reading data: failed, {str(ex)}")
        return 1

    elapsed = time.time() - ts
    print(f"reading data: success, {elapsed:.02f} sec")
    return 0


def write(json_rpc_client, device: str, file_path: str):
    print(f"write data to {file_path}")

    ts = time.time()

    try:
        eeprom_programmer_client.EepromProgrammerClient.write_data_to_file(json_rpc_client, device, file_path)
    except Exception as ex:
        print(f"writing data: failed, {str(ex)}")
        return 1

    elapsed = time.time() - ts
    print(f"writing data: success, {elapsed:.02f} sec")
    return 0


def erase(json_rpc_client, device: str, erase_pattern_str: str):
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

    print(f"erase data with {hex(erase_pattern)}")

    ts = time.time()

    try:
        eeprom_programmer_client.EepromProgrammerClient.erase_data(json_rpc_client, device, erase_pattern)
    except Exception as ex:
        print(f"erasing data: failed, {str(ex)}")
        return 1

    elapsed = time.time() - ts
    print(f"erasing data: success, {elapsed:.02f} sec")
    return 0


def cli() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("port", type=str)
    parser.add_argument("--baudrate", type=int, default=115200)
    parser.add_argument("--init-timeout", type=int, default=3)
    parser.add_argument("-p", "--device", type=str, required=True)
    parser.add_argument("-r", "--read", type=str, required=False)
    parser.add_argument("-w", "--write", type=str, required=False)
    parser.add_argument("--skip-erase", action="store_true", required=False)
    parser.add_argument("--erase", action="store_true", required=False)
    parser.add_argument("--erase-pattern", type=str, required=False)
    args = parser.parse_args()

    # init
    json_rpc_client = client.SerialJsonRpcClient(
        port=args.port, baudrate=args.baudrate, init_timeout=float(args.init_timeout))
    init_result = json_rpc_client.init()
    if init_result is not None:
        print(f"init: {init_result}")

    if args.read is not None:
        return read(json_rpc_client, args.device, args.read)

    elif args.erase is not None:
        return erase(json_rpc_client, args.device, args.erase_pattern)

    elif args.write is not None:
        if not args.skip_erase:
            erase_ret = erase(json_rpc_client, args.device, args.erase_pattern)
            if erase_ret != 0:
                return erase_ret
        return write(json_rpc_client, args.device, args.write)

    else:
        print("unknown mode")


if __name__ == '__main__':
    sys.exit(cli())
