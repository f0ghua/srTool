# Description

This tool is used to convert Motorola S-record hex file to a binary file which with customer defined application header.

## Difference between hex file and binary file

The hex file(Motorola S-record) use ASCII to display the binary value, e.g. binary 0x3F displays as ASCII char '3' and 'F', each byte need 2 bytes char; while binary file only use the actually value 0x3F, which eat just 1 byte space.


# Reference

- https://en.wikipedia.org/wiki/SREC_(file_format)
- [crc16 generate code](https://github.com/ErichStyger/mcuoneclipse/tree/master/Examples/KDS/FRDM-KL25Z/FRDM-KL25Z_CRC/Sources)
- [srecord parser](https://github.com/Oukache/SRecParser/blob/master/SRecParser/srec_parser.cpp)
- [s19tobin](https://github.com/rururutan/s19tobin)
- [bin2s19](https://github.com/DougBarry/Bin2S19/blob/master/bin2s19.cpp)
- [SRecordizer](http://srecordizer.codeplex.com/SourceControl/latest#SRecordizer/Objects/S19.cs)