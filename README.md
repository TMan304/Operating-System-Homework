# Operating-System-Homework
### 在寫module時要放進c_cpp_properties.json
``` json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**",
                "/usr/include",
                "/usr/local/include",
                "/lib/modules/6.8.0-106-generic/build/include",
                "/lib/modules/6.8.0-106-generic/build/arch/x86/include",
                "/lib/modules/6.8.0-106-generic/build/arch/x86/include/generated",
                "/lib/gcc/x86_64-linux-gnu/13/include"
            ],
            "browse": {
                "limitSymbolsToIncludedHeaders": true,
                "databaseFilename": ""
            },
            "defines": [
                "__GNUC__",
                "__KERNEL__",
                "MODULE"
            ],
            "intelliSenseMode": "linux-gcc-x64",
            "compilerPath": "/usr/bin/gcc",
            "cStandard": "c99",
            "cppStandard": "gnu++17"
        }
    ],
    "version": 4
}
```