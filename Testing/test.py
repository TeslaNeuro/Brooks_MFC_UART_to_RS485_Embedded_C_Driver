import serial

s = serial.Serial("COM7",
                    baudrate=19200,
                    bytesize=serial.EIGHTBITS,
                    parity=serial.PARITY_ODD,
                    stopbits=serial.STOPBITS_ONE,
                    timeout=0.05)

# COMMAND #11 for Serial No: 82719007 Nitrogen Working
#s.write(bytes.fromhex("FFFFFFFFFF8280000000000B06E32DF1E70C37EC"))

# response #1: ffffffffff8680000000000b0e0020fe0a320505010308017e38238b
# response #2: ffffffffff8680000000000b0e0000fe0a320505010308017e3823ab
#0000fe0a320505010308017e3823
#0a327
#e3823
#0a327e3823

# COMMAND #11 for Serial No: 82717030 Nitrogen Working``
#s.write(bytes.fromhex("FFFFFFFFFF8280000000000B06e32df1df0cf013"))

# 82717030 packed 6 ascii e32df1df0cf0
# response #1: ffffffffff8680000000000b0e0020fe0a320505010308017e3772d5
# response #2: ffffffffff8680000000000b0e0000fe0a320505010308017e3772f5
# response #3: ffffffffff8680000000000b0e0000fe0a320505010308017e3772f5
#0000fe0a320505010308017e3772
#0a32
#7e3772
#0a327e3772 long address 
#0a6466ef7b
# COMMAND #13 Read tag name still needs long address for some reason
#s.write(bytes.fromhex("FFFFFFFFFF820a327e37720D008C"))

# COMMAND #1 Reading FLOW for Serial No: 82717030 Nitrogen Working
#s.write(bytes.fromhex("FFFFFFFFFF820a327e3772010080"))

# Response from slave:  ffffffffff860a327e37720107000011 25 04 83 bb 8b
# Response from slave when flow is 0 or no flow:  ffffffffff860a327e37720107000011 00 00 00 00 92


# COMMAND #2 Read current
#s.write(bytes.fromhex("FFFFFFFFFF820a327e3772020083"))

# Response from slave:  ffffffffff860a327e3772020a000040800000000000004d

# COMMAND #236 Setpoint set to some value (Example below uses 20.0f)
# Message: 0xFF 0xFF 0xFF 0xFF 0xFF 0x82 0x0A 0x32 0x7E 0x37 0x72 0xEC 0x05 0x39 0x41 0xA0 0x00 0x00 0xB0 
#s.write(bytes.fromhex("FFFFFFFFFF820a327e3772EC053941A00000B0"))

# Response from slave:  ffffffffff860a327e3772ec0c00503941a0000011402666679b


# Set setpoint to 0 Packed bytes: 00 00 00 00
#s.write(bytes.fromhex("FFFFFFFFFF820a327e3772EC05110000000079"))
# ff ff ff ff ff 86 0a 32 7e 37 72 ec 0c 00 40 39 00 00 00 00 11 00 00 00 00 0d

# Set setpoint to 1 Packed bytes: 3F 80 00 00 
#s.write(bytes.fromhex("FFFFFFFFFF820a327e3772EC05113f800000c6"))

# ff ff ff ff ff 86 0a 32 7e 37 72 ec 0c 00 40 39 40 f6 27 63 11 3f 80 00 00 40

# COMMAND #245 Read Alarm Enable Setting Read the alarm enable settings. 
# These alarm settings can be used to mask specific alarm sources.
#s.write(bytes.fromhex("FFFFFFFFFF820a327e3772F50074"))

# 1st Response from slave:  ff ff ff ff ff 86 0a 32 7e 37 72 f5 06 00 20 1f 7f 00 3f 09
# Respone from slave: ff ff ff ff ff 86 0a 32 7e 37 72 f5 06 00 00 1f 7f 00 3f 29

# COMMAND #246 Write Alarm Enable Setting the alarm enable settings. 
# FF FF FF FF FF 82 0a 32 7e 37 72 F6 04 1f 7f 03 3f 2f
# FFFFFFFFFF820a327e3772F6041f7f003f2c
#s.write(bytes.fromhex("FFFFFFFFFF820a327e3772F6041f7f033f2f"))

# Response from slave: ff ff ff ff ff 86 0a 32 7e 37 72 f6 06 00 40 1f 7f 03 3f 69

# COMMAND #247 Read the high/low flow alarm settings as a percent of device full scale. 
# This command can be used to read the actual flow alarm limits.
#s.write(bytes.fromhex("FFFFFFFFFF820a327e3772F70076"))

# 1st Response from slave : ff ff ff ff ff 86 0a 32 7e 37 72 f7 0a 00 20 00 00 00 00 42 c8 00 00 d2
# Response from slave : ff ff ff ff ff 86 0a 32 7e 37 72 f7 0a 00 00 00 00 00 00 42 c8 00 00 f2

# ff ff ff ff ff 86 0a 32 7e 37 72 f7 0a 00 00 00 00 00 00 42 c8 00 00 f2

# Command #248 Write High/Low Flow Alarm Sets the high/low flow alarm settings in percent of device full scale. 
# This command can be used to configure the flow alarm limits.
#const uint8_t command[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82, 0x0a, 0x32, 0x7e, 0x37, 0x72, 0xf8, 0x08, 0x39, 0xc0, 0xa0, 0x00, 0x00, 0x42, 0xc8, 0x00, 0x00};  

# Worked with FFFFFFFFFF820A327E3772F8080000000042C80000FB       0 % Lower limit 100 % Higher limit
#s.write(bytes.fromhex("FFFFFFFFFF820A327E3772F8080000000042C80000FB"))
# Response from slave:  ff ff ff ff ff 86 0a 32 7e 37 72 f8 0a 00 60 00 00 00 00 42 c8 00 00 9d

# Worked with FFFFFFFFFF820A327E3772F8083F80000042C8000044       1 % Lower limit 100 % Higher limit
#s.write(bytes.fromhex("FFFFFFFFFF820A327E3772F8083F80000042C8000044"))
# Response from slave:  ffffffffff860a327e3772f80a00403f80000042c8000002

# COMMAND #11 for Serial No: 40300411 Hydrogen New Model Working

#s.write(bytes.fromhex("FFFFFFFFFF8280000000000B06d30cf0c34c71DE")) 
# response #1: ffffffffff8680000000000b0e0020fe0a6405050101080166ef7b48
# response #2: ffffffffff8680000000000b0e0000fe0a6405050101080166ef7b68
#0000fe0a6405050101080166ef7b
#0a64
#66ef7b
#0a6466ef7b long address

# COMMAND #1 Read flow
# s.write(bytes.fromhex("FFFFFFFFFF820a6466EF7B01001f"))
# Response from slave:  ffffffffff860a6466ef7b01070000abc427bf33d8

# COMMAND #193 Read Operational Settings
#s.write(bytes.fromhex("FFFFFFFFFF820a6466EF7BC100DF"))
# First Response from slave:  ffffffffff860a6466ef7b c1 06 00 20 01 02 ab 20 75
# Response from slave:  ffffffffff860a6466ef7b c1 06 00 00 01 02 ab 20 55

# COMMAND #196 Select Flow Unit
#s.write(bytes.fromhex("FFFFFFFFFF820a6466EF7BC4020211CB"))
# Response from slave:  ff ff ff ff ff 86 0a 64 66 ef 7b c4 04 00 40 02 11 89

# COMMAND #236 Set setpoint to a value
# s.write(bytes.fromhex("FFFFFFFFFF820A6466EF7BEC05AB000000005C"))
#s.write(bytes.fromhex("FFFFFFFFFF820A6466EF7BEC051100000000E6"))

# Response from slave:  ffffffffff860a6466ef7bec0c00403900000000110000000092 0 flow
# Response from slave:  ffffffffff860a6466ef7bec0c00403900000000110000000092 0 flow

# FF FF FF FF FF 82 0a 64 66 EF 7B EC 05 39 00 00 00 00 CE
# FF FF FF FF FF 82 0A 64 66 EF 7B EC 05 11 00 00 00 00 E6 
# FF FF FF FF FF 82 0A 64 66 EF 7B EC 05 AB 00 00 00 00 5C 

# COMMAND #245 Read Alarm Enable Setting Read the alarm enable settings. 
# These alarm settings can be used to mask specific alarm sources.
# s.write(bytes.fromhex("FFFFFFFFFF820a6466EF7BF500EB"))
# Response from slave: ff ff ff ff ff 86 0a 64 66 ef 7b f5 06 00 20 2b 40 00 04 a6
# Response from slave:  ff ff ff ff ff 86 0a 64 66 ef 7b f5 06 00 00 2b 40 03 04 85

# COMMAND #246 Write Alarm Enable Setting the alarm enable settings. 
# FF FF FF FF FF 82 0a 64 66 ef 7b F6 04 2b 40 03 04 
# FFFFFFFFFF820a6466EF7B01001fxxbbcc
# FFFFFFFFFF820a6466ef7bF6042b400304
#s.write(bytes.fromhex("FFFFFFFFFF820a6466ef7bF6042b40030480"))
# Response from slave: ff ff ff ff ff 86 0a 64 66 ef 7b f6 06 00 00 2b 40 03 04 86

# COMMAND #247 Read the high/low flow alarm settings as a percent of device full scale. 
# This command can be used to read the actual flow alarm limits.
# FF FF FF FF FF 82 80 8A 64 66 EF 7B F7 00 E9 
#s.write(bytes.fromhex("FFFFFFFFFF820a6466EF7BF700E9"))

# First ever response: ff ff ff ff ff 86 0a 64 66 ef 7b f7 0a 00 00 bf 80 00 00 42 dc 00 00 46
# Second Response: ff ff ff ff ff 86 0a 64 66 ef 7b f7 0a 00 00 c0 a0 00 00 42 c8 00 00 0d

# COMMAND #248 Write High / Low Flow alarm limits
# ff ff ff ff ff 86 0a 32 7e 37 72 ec 0c 00 50 39 41 a0 00 00 11 40 26 66 67 9b
# ff ff ff ff ff 86 0a 64 66 ef 7b f8 0a 00 00 c0 a0 00 00 42 c8 00 00 02
# ff ff ff ff ff 86 0a 64 66 ef 7b f6 06 00 00 2b 40 03 04 86

# 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26
# ff ff ff ff ff 86 0a 64 66 ef 7b f7 0a 00 00 c0 a0 00 00 42 c8 00 00 0d
# ff ff ff ff ff 86 0a 64 66 ef 7b c4 04 00 40 02 11 89
# ff ff ff ff ff 86 0a 64 66 ef 7b 01 07 00 00 ab c4 27 bf 33 d8

# FF FF FF FF FF 82 8a 64 66 EF 7B F8 08 00 00 00 00 42 C8 00 00 E4     // 0% Low Limit 100% High Limit
# FF FF FF FF FF 82 0a 64 66 EF 7B F8 08 bf 80 00 00 42 C8 00 00        // -1% Low Limit 100% High Limit
# FF FF FF FF FF 82 0a 64 66 EF 7B F8 08 c0 a0 00 00 42 C8 00 00 04     // -5% Low Limit 100% High Limit
#s.write(bytes.fromhex("FFFFFFFFFF820a6466EF7BF808c0a0000042C8000004")) 

# Response from slave: ff ff ff ff ff 86 0a 64 66 ef 7b f8 0a 00 00 c0 a0 00 00 42 c8 00 00 02

# COMMAND #11 for Serial No: 82711003 Hydrogen BAD SO NO RESPONSE
#s.write(bytes.fromhex("FFFFFFFFFF8280000000000B06d30cf0c34c71DE"))

response = s.read(128)
print("Response from slave: ", response.hex())

#const unsigned char command[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x06, 0xe3, 0x2d, 0xf1, 0xe7, 0x0c, 0x37};
#const unsigned char command[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x06, 0xE3, 0x2D, 0xF1, 0xDF, 0x0C, 0xF0};
#const unsigned char command[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x06, 0xd3, 0x0c, 0xf0, 0xc3, 0x4c, 0x70};  
#const unsigned char command[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x06, 0xE3, 0x2D, 0xf1, 0xc7, 0x0c, 0x37};
