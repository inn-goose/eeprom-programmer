# EEPROM API for Arduino

## TLDR

🚧 WIP 🚧

[EEPROM Read and Write Operations with Arduino](https://goose.sh/blog/eeprom-read-and-write-operations/)

[EEPROM API Performance with Arduino](https://goose.sh/blog/eeprom-api-performance/)

[Debugging the EEPROM API](https://goose.sh/blog/debugging-eeprom-api/)


## EEPROM API Board

set pins layout in `eeprom_wiring.h`

API interface:
```cpp
// initializes the pinout 
void init_read(String &eeprom_type);

// reads one page of data, returns raw bytes
byte[] read_page(int page_size_bytes, int page_no);
```

### Manual API Testing

Serial Monitor / `115200` baud

#### Init Chip

```json
# init_chip(chip_type)
{"jsonrpc":"2.0","id":0,"method": "init_chip", "params": ["AT28C64"]}
```

#### Read

```json
# set_read_mode(read_page_size_bytes)
{"jsonrpc":"2.0","id":0,"method": "set_read_mode", "params": [4]}

# read_page(page_no)
{"jsonrpc":"2.0","id":0,"method": "read_page", "params": [0]}
```

#### Write

```json
# set_write_mode(write_page_size_bytes)
{"jsonrpc":"2.0","id":0,"method": "set_write_mode", "params": [4]}

# write_page(page_no, data)
{"jsonrpc":"2.0","id":0,"method": "write_page", "params": [0, [255, 255, 255, 255]]}
{"jsonrpc":"2.0","id":0,"method": "write_page", "params": [0, [127, 127, 127, 127]]}
```


## EEPROM API python CLI

Uses the [Serial JSON RPC](https://github.com/inn-goose/serial-json-rpc-arduino) interface.

### init CLI

```commandline
pip3 install virtualenv

PATH=${PATH}:~/Library/Python/3.9/bin/ ./env/init.sh

source venv/bin/activate

deactivate
```

### Usage

```commandline
python -m serial.tools.list_ports
...
/dev/cu.usbmodem2101
```

#### read

```commandline
mkdir ./tmp

# read the data
PYTHONPATH=./eeprom_api_py_cli/:$PYTHONPATH python3 ./eeprom_api_py_cli/cli.py /dev/cu.usbmodem2101 -p AT28C64 -r tmp/dump_eeprom_api.bin

# convert to HEX
xxd tmp/dump_eeprom_api.bin > tmp/dump_eeprom_api.hex
```

#### erase

```commandline
# erase with FF pattern
PYTHONPATH=./eeprom_api_py_cli/:$PYTHONPATH python3 ./eeprom_api_py_cli/cli.py /dev/cu.usbmodem2101 -p AT28C64 --erase ff
```

#### write

```commandline
🚧 WIP 🚧
```


## XGecu Programmer as a Reference

```commandline
brew install minipro

mkdir ./tmp

# get a "real" EEPROM dump
curl -LJ --output tmp/zenith_zt1_eeprom.bin "https://github.com/misterblack1/zenith_zt1/raw/refs/heads/main/444-187%20U114%20ROM%202732.bin"

# write the "real" dump to the chip
minipro -p AT28C64 -s -u -w tmp/zenith_zt1_eeprom.bin

# read the data
minipro -p AT28C64 -u -r tmp/dump_programmer.bin

# convert to HEX
xxd tmp/dump_programmer.bin > tmp/dump_programmer.hex

# compare
vimdiff tmp/dump_eeprom_api.hex tmp/dump_programmer.hex
```



## Tests

### Read Noise

```commandline
# read w/o reset
PYTHONPATH=./eeprom_api_py_cli/:$PYTHONPATH python3 ./eeprom_api_py_cli/cli.py /dev/cu.usbmodem2101 -p AT28C64 -r tmp/dump_eeprom_api.bin --attempts 3

# read w/ reset
for i in `seq 1 3`; do PYTHONPATH=./eeprom_api_py_cli/:$PYTHONPATH python3 ./eeprom_api_py_cli/cli.py /dev/cu.usbmodem2101 -p AT28C64 -r tmp/dump_eeprom_api_$i.bin; done

# convert to HEX
for i in `seq 1 3`; do xxd tmp/dump_eeprom_api_$i.bin > tmp/dump_eeprom_api_$i.hex; done

# compare

vimdiff tmp/dump_eeprom_api_[1,3].hex

vimdiff tmp/dump_eeprom_api_*.hex

vimdiff tmp/dump_eeprom_api_1.hex tmp/dump_programmer.hex
```
