# Riru-CarrotJuicer

Hooks the decryption function in `libnative.so` of ウマ娘プリティーダービー (Umamusume Pretty Derby), to allow inspecting the packets.

## Usage

Currently this only supports Android >= 8.0.

1. Install [Magisk](https://github.com/topjohnwu/Magisk) and [Riru](https://github.com/RikkaApps/Riru).
2. Install Riru-CarrotJuicer and reboot.
3. Start the game as usual. The captured packets will be saved into its data directory, `/data/data/jp.co.cygames.umamusume/files/CarrotJuicer/`.
4. You can investigate the responses with msgpack tools like `msgpack2json -di 123456789R.msgpack`.

Hopefully useful commands:

* `adb shell 'su -c rm /data/data/jp.co.cygames.umamusume/files/CarrotJuicer/* && rm -rf /sdcard/CarrotJuicer'`: clears CarrotJuicer folders in data directory and sdcard.
* `adb shell 'su -c cp -R /data/data/jp.co.cygames.umamusume/files/CarrotJuicer /sdcard/' && adb pull /sdcard/CarrotJuicer`: pulls CarrotJuicer folder, via a temporary directory in sdcard. 

## Build

1. `git clone`
2. `git submodule update --init` 
3. Run gradle task `:module:assembleRelease` task from Android Studio or command line, zip will be saved in `out`.

## Credits

This module is largely ~~copied from~~ inspired by [Riru-Il2CppDumper](https://github.com/Perfare/Riru-Il2CppDumper/).
