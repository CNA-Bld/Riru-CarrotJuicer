# Riru-CarrotJuicer

Hooks the decryption function in `libnative.so` of ウマ娘プリティーダービー (Umamusume Pretty Derby), to allow inspecting the packets.

## Usage

Currently this only supports Android >= 8.0 and Android 6.

1. Install [Magisk](https://github.com/topjohnwu/Magisk) and [Riru](https://github.com/RikkaApps/Riru).
2. Install Riru-CarrotJuicer and reboot.
3. Start the game as usual. The captured packets will be saved into its data directory, `/data/data/jp.co.cygames.umamusume/files/CarrotJuicer/`.
4. You can investigate the responses with msgpack tools like `msgpack2json -di 123456789R.msgpack`.

[Hakuraku](https://github.com/SSHZ-ORG/hakuraku) has a UI for investigating the captured packets [here](https://hakuraku.sshz.org/#/carrotjuicer).

Hopefully useful commands:

* `adb shell 'su -c rm /data/data/jp.co.cygames.umamusume/files/CarrotJuicer/* && rm -rf /sdcard/CarrotJuicer'`: clears CarrotJuicer folders in data directory and sdcard.
* `adb shell 'su -c cp -R /data/data/jp.co.cygames.umamusume/files/CarrotJuicer /sdcard/' && adb pull /sdcard/CarrotJuicer`: pulls CarrotJuicer folder, via a temporary directory in sdcard. 

### `race_scenario`

In packets containing races, there is a base64-encoded field, often named `race_scenario`. This includes per-frame and per-chara information in the race.

More details can be found in README of [Hakuraku](https://github.com/SSHZ-ORG/hakuraku). Its web UI is able to parse this field for you.

### Requests

Requests (files ending with `Q.msgpack`) are not actually msgpack. The current observation is:

* The first 4 bytes likely represent a little-endian int. We name it `offset`, currently always observed to be 0xA6 (166).
* The following 52 bytes `[+0x04, +0x38)` never change for a single client, even across sessions. We did not test whether this is per-account or per-client.
* The following 114 bytes `[+0x38, +0xB0)` are different for each request.
* All remaining is a standard msgpack message. This starts at `+0xB0` which is exactly `offset + 4`.

To investigate the content, remove the first 170 bytes and use msgpack tools, like `tail -c+171 123456789Q.msgpack | msgpack2json -d`.

## Build

1. `git clone`
2. `git submodule update --init` 
3. Run gradle task `:module:assembleRelease` task from Android Studio or command line, zip will be saved in `out`.

## Credits

This module is largely ~~copied from~~ inspired by [Riru-Il2CppDumper](https://github.com/Perfare/Riru-Il2CppDumper/).
