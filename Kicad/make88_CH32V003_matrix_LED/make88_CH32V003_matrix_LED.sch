EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU_Microchip_ATtiny:ATtiny202-SS U1
U 1 1 642E07F6
P 4850 3250
F 0 "U1" H 4321 3296 50  0000 R CNN
F 1 "ATtiny202-SS" H 4321 3205 50  0000 R CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 4850 3250 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny202-402-AVR-MCU-with-Core-Independent-Peripherals_and-picoPower-40001969A.pdf" H 4850 3250 50  0001 C CNN
	1    4850 3250
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery_Cell BT1
U 1 1 642E14EA
P 3500 3700
F 0 "BT1" H 3618 3796 50  0000 L CNN
F 1 "Battery_Cell" H 3618 3705 50  0000 L CNN
F 2 "Battery:BatteryHolder_ComfortableElectronic_CH273-2450_1x2450" V 3500 3760 50  0001 C CNN
F 3 "~" V 3500 3760 50  0001 C CNN
	1    3500 3700
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 JUPDI1
U 1 1 642E21E5
P 1450 4100
F 0 "JUPDI1" H 1368 3775 50  0000 C CNN
F 1 "Conn_01x03" H 1368 3866 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1450 4100 50  0001 C CNN
F 3 "~" H 1450 4100 50  0001 C CNN
	1    1450 4100
	-1   0    0    1   
$EndComp
Wire Wire Line
	1650 4000 2000 4000
Wire Wire Line
	4850 2500 4850 2650
Wire Wire Line
	4850 4200 4850 3850
Wire Wire Line
	3500 3800 3500 4200
Connection ~ 3500 4200
Wire Wire Line
	3500 4200 4000 4200
$Comp
L Device:R R1
U 1 1 642E4B78
P 2650 4100
F 0 "R1" V 2443 4100 50  0000 C CNN
F 1 "4.7k" V 2534 4100 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 2580 4100 50  0001 C CNN
F 3 "~" H 2650 4100 50  0001 C CNN
	1    2650 4100
	0    1    1    0   
$EndComp
Text Notes 5250 4500 0    50   ~ 0
UPDI 6pin
Wire Wire Line
	6000 2950 5450 2950
Wire Wire Line
	6000 2950 6000 3650
Wire Wire Line
	1650 4100 2500 4100
Wire Wire Line
	2800 4100 6000 4100
Text Notes 4050 6800 0    50   ~ 0
Pin     Digital     Analog     Function\n1        -               -               VCC\n2        0              A6            TXD\n3        1              A7            RXD/LED_BUILTIN\n4        2              A1            SDA/MOSI\n5        3              A2            SCL/MISO\n6        5              A0            SS\n7        4              A3            SCK\n8        -               -              GND
$Comp
L Connector_Generic:Conn_01x04 JI2C1
U 1 1 642E8A0F
P 8950 2150
F 0 "JI2C1" H 9030 2142 50  0000 L CNN
F 1 "Conn_01x04" H 9030 2051 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 8950 2150 50  0001 C CNN
F 3 "~" H 8950 2150 50  0001 C CNN
	1    8950 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 3050 7200 3050
Wire Wire Line
	5450 3150 7350 3150
Connection ~ 4850 4200
Wire Wire Line
	5450 3250 7050 3250
Text Notes 9600 4050 0    50   ~ 0
SPI I/F\n1   VDD\n2   CS(EN)\n3   MOSI\n4   MISO\n5   CLOCK\n6   GND\n
Wire Wire Line
	8750 3650 6000 3650
Connection ~ 6000 3650
Wire Wire Line
	6000 3650 6000 4100
Wire Wire Line
	7200 3050 7200 3750
Wire Wire Line
	7200 3750 8750 3750
Connection ~ 7200 3050
$Comp
L Connector_Generic:Conn_01x06 JSPI1
U 1 1 642EDD6D
P 8950 3750
F 0 "JSPI1" H 9030 3742 50  0000 L CNN
F 1 "Conn_01x06" H 9030 3651 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 8950 3750 50  0001 C CNN
F 3 "~" H 8950 3750 50  0001 C CNN
	1    8950 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 3150 7350 3850
Wire Wire Line
	7350 3850 8750 3850
Connection ~ 7350 3150
Wire Wire Line
	7050 3250 7050 3950
Wire Wire Line
	7050 3950 8750 3950
Text Notes 9650 2650 0    50   ~ 0
I2C I/F\n1   VDD\n2   SDA\n3   SCL\n4   GND\n
$Comp
L Connector_Generic:Conn_01x03 J1WIRED1
U 1 1 642EFC02
P 8950 5150
F 0 "J1WIRED1" H 9030 5192 50  0000 L CNN
F 1 "Conn_01x03" H 9030 5101 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 8950 5150 50  0001 C CNN
F 3 "~" H 8950 5150 50  0001 C CNN
	1    8950 5150
	1    0    0    -1  
$EndComp
Text Notes 9650 5400 0    50   ~ 0
1WIRE I/F\n1   VDD\n2   DATA\n3   GND\n
Wire Wire Line
	7200 3750 7200 5150
Wire Wire Line
	7200 5150 8750 5150
Connection ~ 7200 3750
Wire Wire Line
	7350 2250 7350 3150
Wire Wire Line
	7350 2250 8750 2250
Wire Wire Line
	7200 2150 7200 3050
Wire Wire Line
	7200 2150 8750 2150
Wire Wire Line
	8750 3550 8500 3550
Wire Wire Line
	8500 3550 8500 2050
Connection ~ 4850 2500
Wire Wire Line
	8750 2050 8500 2050
Connection ~ 8500 2050
Wire Wire Line
	8500 2050 8500 1500
Wire Wire Line
	8750 5050 8500 5050
Wire Wire Line
	8500 5050 8500 3550
Connection ~ 8500 3550
Wire Wire Line
	4850 5250 4850 5100
Wire Wire Line
	4850 5250 6500 5250
Wire Wire Line
	8350 5250 8350 4050
Wire Wire Line
	8350 4050 8750 4050
Connection ~ 8350 5250
Wire Wire Line
	8350 5250 8750 5250
Wire Wire Line
	8350 4050 8350 2350
Connection ~ 8350 4050
Wire Wire Line
	8350 2350 8750 2350
$Comp
L Connector_Generic:Conn_01x03 JPSSEL1
U 1 1 642F911B
P 3600 2100
F 0 "JPSSEL1" V 3564 1912 50  0000 R CNN
F 1 "Conn_01x03" V 3473 1912 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 3600 2100 50  0001 C CNN
F 3 "~" H 3600 2100 50  0001 C CNN
	1    3600 2100
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C1
U 1 1 642FC39F
P 4000 3000
F 0 "C1" H 4115 3046 50  0000 L CNN
F 1 "C" H 4115 2955 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 4038 2850 50  0001 C CNN
F 3 "~" H 4000 3000 50  0001 C CNN
	1    4000 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 2850 4000 2500
Wire Wire Line
	4000 2500 4850 2500
Connection ~ 4000 4200
Wire Wire Line
	4000 4200 4700 4200
Wire Wire Line
	4000 3150 4000 4200
Wire Wire Line
	4850 1500 4850 2500
Wire Wire Line
	8500 1500 4850 1500
Wire Wire Line
	3500 2300 2000 2300
Wire Wire Line
	2000 2300 2000 4000
Wire Wire Line
	3600 2300 3600 2500
Wire Wire Line
	3600 2500 4000 2500
Connection ~ 4000 2500
$Comp
L Connector_Generic:Conn_01x03 JDCDC1
U 1 1 64314509
P 3100 3000
F 0 "JDCDC1" H 3018 2675 50  0000 C CNN
F 1 "Conn_01x03" H 3018 2766 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 3100 3000 50  0001 C CNN
F 3 "~" H 3100 3000 50  0001 C CNN
	1    3100 3000
	-1   0    0    1   
$EndComp
Wire Wire Line
	3500 3500 3500 2900
Wire Wire Line
	3500 2900 3300 2900
Wire Wire Line
	3300 3000 3700 3000
Wire Wire Line
	3700 3000 3700 2300
Wire Wire Line
	3300 3100 3300 4200
Wire Wire Line
	1650 4200 3300 4200
Connection ~ 3300 4200
Wire Wire Line
	3300 4200 3500 4200
Wire Wire Line
	3350 5100 3500 5100
Connection ~ 4850 5100
Wire Wire Line
	4850 5100 4850 4200
Wire Wire Line
	5600 3450 5450 3450
Wire Wire Line
	3350 5000 5750 5000
Wire Wire Line
	5750 5000 5750 3350
Wire Wire Line
	5750 3350 5450 3350
Text Label 3500 5000 0    50   ~ 0
ADINPUT1
$Comp
L power:GND #PWR0101
U 1 1 6431D2B8
P 4700 4200
F 0 "#PWR0101" H 4700 3950 50  0001 C CNN
F 1 "GND" H 4705 4027 50  0000 C CNN
F 2 "" H 4700 4200 50  0001 C CNN
F 3 "" H 4700 4200 50  0001 C CNN
	1    4700 4200
	1    0    0    -1  
$EndComp
Connection ~ 4700 4200
Wire Wire Line
	4700 4200 4850 4200
Text Label 4500 2500 0    50   ~ 0
VDD5V
$Comp
L Switch:SW_Push SW1
U 1 1 643156AD
P 4300 5500
F 0 "SW1" H 4300 5785 50  0000 C CNN
F 1 "SW_Push" H 4300 5694 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4300 5700 50  0001 C CNN
F 3 "~" H 4300 5700 50  0001 C CNN
	1    4300 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 5500 5600 5500
Wire Wire Line
	5600 3450 5600 5500
$Comp
L Device:C C2
U 1 1 64319ECB
P 6000 5750
F 0 "C2" H 6115 5796 50  0000 L CNN
F 1 ".1u" H 6115 5705 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 6038 5600 50  0001 C CNN
F 3 "~" H 6000 5750 50  0001 C CNN
	1    6000 5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 5600 6000 5500
Wire Wire Line
	6000 5500 5600 5500
Connection ~ 5600 5500
Wire Wire Line
	6000 5900 6000 6000
Wire Wire Line
	6000 5500 6000 5000
Connection ~ 6000 5500
$Comp
L Device:R R2
U 1 1 6431FFB4
P 6000 4850
F 0 "R2" H 6070 4896 50  0000 L CNN
F 1 "100K" H 6070 4805 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 5930 4850 50  0001 C CNN
F 3 "~" H 6000 4850 50  0001 C CNN
	1    6000 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 4700 6000 4500
Text Label 6000 4550 0    50   ~ 0
VDD5V
Wire Wire Line
	6000 6000 6500 6000
Wire Wire Line
	6500 6000 6500 5250
Connection ~ 6500 5250
Wire Wire Line
	6500 5250 8350 5250
Wire Wire Line
	3500 5500 3500 5100
Connection ~ 3500 5100
Wire Wire Line
	3500 5100 4850 5100
Wire Wire Line
	3500 5500 4100 5500
$Comp
L Connector_Generic:Conn_01x02 JADCIN1
U 1 1 6433A351
P 3150 5100
F 0 "JADCIN1" H 3068 4775 50  0000 C CNN
F 1 "Conn_01x02" H 3068 4866 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 3150 5100 50  0001 C CNN
F 3 "~" H 3150 5100 50  0001 C CNN
	1    3150 5100
	-1   0    0    1   
$EndComp
$EndSCHEMATC
