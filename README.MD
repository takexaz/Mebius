Mebius
============================
(多分)世界初のwinmugen用ModLoader, FunctionHooker

## Requirements
* [Microsoft Visual C++ 再頒布可能パッケージ(x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)
* [MebiusLoader](https://github.com/takexaz/MebiusLoader/releases/latest)

## Installation
1. winmugen.exeがあるディレクトリに[DINPUT.dll](https://github.com/takexaz/MebiusLoader/releases/latest/download/DINPUT.dll)を配置
2. modsディレクトリを作成し、[Mebius.toml](https://github.com/takexaz/Mebius/releases/download/0.0.2/Mebius.toml)ファイルを配置
3. [Mebius.dll](https://github.com/takexaz/Mebius/releases/latest/download/Mebius.dll)ファイルを配置
```
root
└mods
| └Mebius.toml
└DINPUT.dll
└Mebius.dll
└winmugen.exe
```
## License
* [MIT](LICENSE.txt)

## Credits
- [plog](https://github.com/SergiusTheBest/plog)
    - [MIT](deps/plog/LICENSE)
- [tomlplusplus](https://github.com/marzer/tomlplusplus)
    - [MIT](deps/tomlplusplus/LICENSE)
- [zydis](https://github.com/zyantific/zydis)
    - [MIT](deps/zydis/LICENSE)

## Special Thanks
<table id='credit'>
<tr>
<td id='HalkazeMUGEN'>
<a href="https://github.com/HalkazeMUGEN">
    <img src="https://github.com/HalkazeMUGEN.png" width='140px;'>
</a>
<h4 align='center'><a href='https://twitter.com/i/user/1411391549502492679'>Halkaze@MUGEN</a></h4>
</td>
<td id='PL'>
<a href="https://github.com/purpuraBib">
    <img src="https://github.com/purpuraBib.png" width='140px;'>
</a>
<h4 align='center'><a href='https://twitter.com/i/user/1005782550814445568'>PL</a></h4>
</td>
<td id='Ryori514'>
<a href="https://github.com/Ryori514">
    <img src="https://github.com/Ryori514.png" width='140px;'>
</a>
<h4 align='center'><a href='https://twitter.com/i/user/845146560887324673'>リョウリ/Ryori</a></h4>
</td>
</tr>
</table>