import argparse
import sys

from core import eeprom_api_client
from serial_json_rpc import client


def number_file_path(file_path: str, index: int) -> str:
    if '.' in file_path:
        file_name_parts = file_path.split('.')
        file_name = ".".join(file_name_parts[:-1])
        file_ext = file_name_parts[-1]
        return f"{file_name}_{index}.{file_ext}"
    else:
        return f"{file_path}_{index}"


def cli() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("port", type=str)
    parser.add_argument("--baudrate", type=int, default=115200)
    parser.add_argument("--init-timeout", type=int, default=3)
    parser.add_argument("-p", "--device", type=str, required=True)
    parser.add_argument("-r", "--read", type=str, required=False)
    parser.add_argument("-w", "--write", type=str, required=False)
    parser.add_argument("--erase", type=str, required=False)
    parser.add_argument("--attempts", type=int, default=1)
    args = parser.parse_args()

    # init
    json_rpc_client = client.SerialJsonRpcClient(
        port=args.port, baudrate=args.baudrate, init_timeout=float(args.init_timeout))
    init_result = json_rpc_client.init()
    if init_result is not None:
        print(f"init: {init_result}")

    if args.read is not None:
        for i in range(1, args.attempts + 1):
            if args.attempts > 1:
                file_path = number_file_path(args.read, i)
            else:
                file_path = args.read
            print(f"read data to {file_path}")
            try:
                eeprom_api_client.EepromApiClient.read_data_to_file(json_rpc_client, args.device, file_path)
            except Exception as ex:
                print(f"failed to read data with: {str(ex)}")
                return 1

    elif args.erase is not None:
        try:
            erase_pattern = int(args.erase, 16)
            if erase_pattern < 0 or erase_pattern > 255:
                print(f"invalid erase pattern {args.erase}, should be within [0, 255] range")
                return 1
        except Exception:
            print(f"invalid erase pattern {args.erase}, should be a HEX value")
            return 1
        try:
            eeprom_api_client.EepromApiClient.erase_data(json_rpc_client, args.device, erase_pattern)
        except Exception as ex:
            print(f"failed to erase chip with: {str(ex)}")
            return 1

    elif args.write is not None:
        for i in range(1, args.attempts + 1):
            if args.attempts > 1:
                file_path = number_file_path(args.write, i)
            else:
                file_path = args.write
            print(f"write data to {file_path}")
            try:
                eeprom_api_client.EepromApiClient.write_data_to_file(json_rpc_client, args.device, file_path)
            except Exception as ex:
                print(f"failed to write data with: {str(ex)}")
                return 1
    else:
        print("unknown mode")


if __name__ == '__main__':
    sys.exit(cli())
