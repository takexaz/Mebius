Mebius
============================
(多分)世界初のwinmugen用ModLoader, FunctionHooker

## Requirements
* [Microsoft Visual C++ 再頒布可能パッケージ(x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)
* [MebiusLoader](https://github.com/takexaz/MebiusLoader)

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
* [MIT](LICENSE)

## Credits
- [plog](https://github.com/SergiusTheBest/plog)
    - [MIT](https://github.com/SergiusTheBest/plog/blob/199734337bec45e72956c4e82f848a716cbf7ac3/LICENSE)
- [tomlplusplus](https://github.com/marzer/tomlplusplus)
    - [MIT](https://github.com/marzer/tomlplusplus/blob/d8fa9a1fddc90254cac2366dde23f0b613bc1280/LICENSE)
- [zydis](https://github.com/zyantific/zydis)
    - [MIT](https://github.com/zyantific/zydis/blob/ba1e3cf3a64f668790164c005d1ee52f3d77cf76/LICENSE)

## Special Thanks
<table id='credit'>
    <tr>
        <td id='HalkazeMUGEN'>
            <a href="https://github.com/HalkazeMUGEN">
                <img src="https://github.com/HalkazeMUGEN.png" width='140px'>
            </a>
            <h4 align='center'><a href='https://twitter.com/i/user/1411391549502492679'>Halkaze</a></h4>
        </td>
        <td id='Minor2CCh'>
            <a href="https://github.com/Minor2CCh">
                <img src="https://github.com/Minor2CCh.png" width='140px'>
            </a>
            <h4 align='center'><a href='https://twitter.com/i/user/1185504247565058049'>マイナー</a></h4>
        </td>
        <td id='PL'>
            <a href="https://github.com/purpuraBib">
                <img src="https://github.com/purpuraBib.png" width='140px'>
            </a>
            <h4 align='center'><a href='https://twitter.com/i/user/1005782550814445568'>PL</a></h4>
        </td>
        <td id='Ryori514'>
            <a href="https://github.com/Ryori514">
                <img src="https://github.com/Ryori514.png" width='140px'>
            </a>
            <h4 align='center'><a href='https://twitter.com/i/user/845146560887324673'>リョウリ/Ryori</a></h4>
        </td>
    </tr>
</table>
