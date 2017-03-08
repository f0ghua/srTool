
# Requirement 20161214

1. 程序启动，自动载入三个头文件XXXHeaderForGeneration.txt，读取HEADER段和INFO段的内容并显示于界面；
2. 用户点击"Load Full Content S19 File"，载入文件full.s19，读取INFO段， BLOCK段的信息；原HEADER段和INFO段内容及界面均不更新，保持原txt文件中读取的内容；
3. 用户点击"Save Binaries", 将HEADER段，INFO段，BLOCK段合并生成相应的BIN文件和HEX文件.
4.1 用户通过界面修改HEADER/INFO段信息，然后点击"Save Headers"保存到XXXHeaderForGeneration.txt；
4.2 用户点击"Save Binaries", 将HEADER段，INFO段，BLOCK段合并生成相应的BIN文件和HEX文件.

# Requirement 20161010

文件格式有4种
- txt文件:    自定义,保存字段头(Header)信息
- s19文件:    输入,包含完整的Info信息和文件内容,通过地址索引
https://en.wikipedia.org/wiki/SREC_(file_format)
- bin文件:    输入/输出
- hex文件:    输出
https://en.wikipedia.org/wiki/Intel_HEX
http://www.dlwrr.com/electronics/tools/hexview/hexview.html

## 程序逻辑

### 输入文件:
- full.s19
> 包含除Header以外的所有信息,由地址索引
```
0x00000000  CAL2_DATAINFO, length = 0x20
…   
0x0000001F  
0x00000020  CAL2_BLOCK, length = 0x7FD8   
…   
0x00007FF7  
0x00007FF8  CAL2_PSI, length = 0x8               
…   
0x00007FFF  
0x00008000  CAL1_DATAINFO, length = 0x20
…   
0x0000801F  
0x00008020  CAL1_BLOCK, length = 0x3FD8   
…   
0x0000BFF7  
0x0000BFF8  CAL1_PSI, length = 0x8               
…   
0x0000BFFF  
0x00010000  APPL_SWINFO, length = 0x200
…   
0x000101FF  
0x00010200  APPL_BLOCK, length = 0xCFFF8 
…   
0x000DFFF7  
0x000DFFF8  APPL_PSI, length = 0x8               
…   
0x000DFFFF  
```

- AppHeaderForGeneration.txt
> 包含APP Header的信息

- Cal1HeaderForGeneration.txt
> 包含CAL1 Header的信息

- Cal2HeaderForGeneration.txt
> 包含CAL2 Header的信息

### 输出文件

- 3个hex文件，文件名为"Hex Part Number"转换成10进制
> Hex1 = APP_SWINFO + APP_BLOCK
> Hex2 = 01 01 + APP_SWINFO + APP_BLOCK

- 3个bin文件，文件名为"Hex Part Number"转换成10进制
> Bin = 03 01 + App Header + 01 01 + APP_SWINFO + APP_BLOCK

### 处理流程

- 基本功能
1. 载入文件XXXHeaderForGeneration.txt,读取HEADER段和INFO段的内容并显示;
2. 修改HEADER/INFO信息并保存(注意HEADER,INFO相同字段的同步);
3. 载入文件full.s19, 读取INFO段, BLOCK段的信息;根据INFO段信息更新界面显示的HEADER段和INFO段内容;
4. 将HEADER段, INFO段, BLOCK段合并生成相应的BIN文件和HEX文件(HEX文件INFO段需要padding).

- 附加功能
1. 读取bin文件,分析出HEADER段和INFO段信息,并保存至TXT文件,文件名为AppHeaderInfo.txt/Cal1HeaderInfo.txt/Cal2HeaderInfo.txt.
2. APP SW Location Info需要支持编辑最大4个
3. 生成的Hex文件支持两种格式(前置0101或者没有)，可配置
4.生成bin和hex文件时，生成log文件(可配置); 把时间信息，生成文件信息写到一个txt文档，这个txt文件名称最后加时间戳.

# Requirement 20160909

- 文件命名用Hex Part Number
- 生成3个binary
- txt文件命名APP.txt, CAL1.txt, CAL2.txt
- 支持命令行操作
- 从binay读取txt
    + load binary file, 
