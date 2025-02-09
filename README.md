<p align="center">
    <img src="https://gist.githubusercontent.com/tijme/490b0e350729f670b636a98a1c4efdbe/raw/5bc396c0484d7ad974f9c81989217034f35e8fe0/logo.svg" width="500" alt="C-implant (PIC) for NimPlant C2" />
</p>
<p align="center">
    <a href="https://github.com/tijme/nimplant-beacon-position-independent-c-code/blob/master/LICENSE.md">
        <img src="https://img.shields.io/badge/License-MPL%20V2.0-e30b42?style=for-the-badge&labelColor=555555" />
    </a> &nbsp;
    <a href="https://github.com/tijme/nimplant-beacon-position-independent-c-code/releases">
        <img src="https://img.shields.io/github/v/release/tijme/nimplant-beacon-position-independent-c-code?style=for-the-badge&labelColor=e30b42&color=555555" />
    </a> &nbsp;
    <a href="https://github.com/tijme/nimplant-beacon-position-independent-c-code/actions">
        <img src="https://img.shields.io/github/actions/workflow/status/tijme/nimplant-beacon-position-independent-c-code/compile.yml?style=for-the-badge&labelColor=555555&color=e30b42" />
    </a>
</p>
<p align="center">
    <b>A truly Position Independent Code (PIC) NimPlant C2 beacon written in C, <i>without</i> reflective loading.</b>
    <br/>
    <sup>Built with ♥ by <a href="https://x.com/tijme">Tijme Gommers</a> – Buy me a coffee via <a href="https://www.paypal.me/tijmegommers">PayPal</a>.</sup>
    <br/>
</p>
<p align="center">
    <a href="#abstract">Abstract</a>
    •
    <a href="#progress">Progress</a>
    •
    <a href="#getting-started">Getting started</a>
    •
    <a href="#issues--requests">Issues & requests</a>
    •
    <a href="#license--copyright">License & copyright</a>
</p>
<hr>

## Abstract

A minimal Proof-of-Concept (PoC) [NimPlant C2](https://github.com/chvancooten/NimPlant) beacon in plain C, designed to be truly Position Independent Code (PIC), *without* relying on reflective loader such as Donut and sRDI. By manually resolving imports, storing required data on the stack, and executing without fixed addresses, the beacon compiles directly to PIC. As advantage, the beacon is much smaller in size compared to using a NimPlant beacon *with* a reflective loader (30kb vs 800kb). It is built on [Relocatable](https://github.com/tijme/relocatable), a boilerplate to develop raw and truly Position Independent Code (PIC).

> ⚠️ This minimal Proof-of-Concept (PoC) was developed to showcase [Kong Loader](https://github.com/tijme/kong-loader) (to be released at BlackHat Asia on the 3rd of April 2025). **It is a Minimum Viable Product (MVP) and is definitely not production and opsec safe. Memory is not being used safely either, possibly causing memory leaks and vulnerabilities in the code.** ⚠️

## Getting started

Clone this repository first. Install the dependencies, then [review the code](https://github.com/tijme/nimplant-beacon-position-independent-c-code/blob/master/.github/review-the-code.gif).

**Dependencies**

* [MinGW](https://formulae.brew.sh/formula/mingw-w64)

**Modify the code**

Adjust the implant settings in the `__main` function from `./src/main.c` to your needs. 

**Compiling**

    make

**Usage**

Load `./dst/shellcode.bin` using your shellcode loader (technique) of choice. An example loader that you can use is [Kong Loader](https://github.com/tijme/kong-loader), which will be released at BlackHat Asia on the 3rd of April 2025. 

Another option is to convert it to a c-code array using `xxd`.

    xxd -i dst/shellcode.bin

This outputs about ~30kb of truly Position Independent Code (PIC) to be used in your own loader.

## Progress

As stated, this is just a minimal Proof-of-Concept (PoC) showcasing a beacon in truly Position Independent Code (PIC). Not all functionalities and commands that you are used to in Nimplant beacons are available yet.

- [✅] Implant initialization functionality.
- [✅] Implant registration functionality.
- [✅] AES encryption of data passed via JSON.
- [✅] Retrieve metadata and send back to Nimplant server.
- [✅] Support implant command `cat`.
- [✅] Support implant command `cd`.
- [✅] Support implant command `cp`.
- [✅] Support implant command `whoami`.
- [✅] Support implant command `ls`.
- [✅] Support implant command `pwd`.
- [⚠️] TODO: Support all implant configuration options.
- [⚠️] TODO: Support jitter if sleep is used.
- [⚠️] TODO: Support incremental delay if server not reachable.
- [⚠️] TODO: Basic opsec such as string obfuscation.
- [⚠️] TODO: Low coupling, high cohesion.
- [⚠️] TODO: Support implant command `curl`.
- [⚠️] TODO: Support implant command `download`.
- [⚠️] TODO: Support implant command `env`.
- [⚠️] TODO: Support implant command `execute-assembly`.
- [⚠️] TODO: Support implant command `get-av`.
- [⚠️] TODO: Support implant command `get-domain`.
- [⚠️] TODO: Support implant command `get-local-admin`.
- [⚠️] TODO: Support implant command `inline-execute`.
- [⚠️] TODO: Support implant command `mkdir`.
- [⚠️] TODO: Support implant command `mv`.
- [⚠️] TODO: Support implant command `powershell`.
- [⚠️] TODO: Support implant command `ps`.
- [⚠️] TODO: Support implant command `reg`.
- [⚠️] TODO: Support implant command `rm`.
- [⚠️] TODO: Support implant command `run`.
- [⚠️] TODO: Support implant command `screenshot`.
- [⚠️] TODO: Support implant command `shell`.
- [⚠️] TODO: Support implant command `shinject`.
- [⚠️] TODO: Support implant command `sleep`.
- [⚠️] TODO: Support implant command `upload`.
- [⚠️] TODO: Support implant command `wget`.

## Issues & requests

Issues or new feature requests can be reported via the [issue tracker](https://github.com/tijme/nimplant-beacon-position-independent-c-code/issues). Please make sure your issue or feature has not yet been reported by anyone else before submitting a new one.

## License & copyright

Copyright (c) 2025 Tijme Gommers. This project is released under the GNU General Public License, version 2.0. View [LICENSE.md](https://github.com/tijme/nimplant-beacon-position-independent-c-code/blob/master/LICENSE.md) for the full license.