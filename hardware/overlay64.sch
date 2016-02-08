EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:lm1881
LIBS:overlay64-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Simple Video Overlay Driver"
Date "2016-02-02"
Rev "1"
Comp "Henning Bekel"
Comment1 "http://www.henning-bekel.de"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L LM1881 U1
U 1 1 56A1EDB5
P 5800 4350
F 0 "U1" H 5550 4600 60  0000 C CNN
F 1 "LM1881" H 5950 4100 60  0000 C CNN
F 2 "Housings_DIP:DIP-8_W7.62mm" H 5750 4200 60  0001 C CNN
F 3 "" H 5750 4200 60  0000 C CNN
	1    5800 4350
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG1
U 1 1 56A1EF0B
P 6250 7500
F 0 "#FLG1" H 6250 7595 50  0001 C CNN
F 1 "PWR_FLAG" H 6250 7680 50  0000 C CNN
F 2 "" H 6250 7500 60  0000 C CNN
F 3 "" H 6250 7500 60  0000 C CNN
	1    6250 7500
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR12
U 1 1 56A1EF40
P 6250 7500
F 0 "#PWR12" H 6250 7350 50  0001 C CNN
F 1 "VCC" H 6250 7650 50  0000 C CNN
F 2 "" H 6250 7500 60  0000 C CNN
F 3 "" H 6250 7500 60  0000 C CNN
	1    6250 7500
	-1   0    0    1   
$EndComp
$Comp
L PWR_FLAG #FLG2
U 1 1 56A1EF56
P 6700 7500
F 0 "#FLG2" H 6700 7595 50  0001 C CNN
F 1 "PWR_FLAG" H 6700 7680 50  0000 C CNN
F 2 "" H 6700 7500 60  0000 C CNN
F 3 "" H 6700 7500 60  0000 C CNN
	1    6700 7500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR15
U 1 1 56A1EF6C
P 6700 7500
F 0 "#PWR15" H 6700 7250 50  0001 C CNN
F 1 "GND" H 6700 7350 50  0000 C CNN
F 2 "" H 6700 7500 60  0000 C CNN
F 3 "" H 6700 7500 60  0000 C CNN
	1    6700 7500
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR9
U 1 1 56A1F13D
P 5600 4850
F 0 "#PWR9" H 5600 4700 50  0001 C CNN
F 1 "VCC" H 5600 5000 50  0000 C CNN
F 2 "" H 5600 4850 60  0000 C CNN
F 3 "" H 5600 4850 60  0000 C CNN
	1    5600 4850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR10
U 1 1 56A1F155
P 5600 5150
F 0 "#PWR10" H 5600 4900 50  0001 C CNN
F 1 "GND" H 5600 5000 50  0000 C CNN
F 2 "" H 5600 5150 60  0000 C CNN
F 3 "" H 5600 5150 60  0000 C CNN
	1    5600 5150
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 56A1F16D
P 2050 2650
F 0 "C3" H 2075 2750 50  0000 L CNN
F 1 "100nF" H 2075 2550 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 2088 2500 30  0001 C CNN
F 3 "" H 2050 2650 60  0000 C CNN
	1    2050 2650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR2
U 1 1 56A1F1BC
P 2050 2800
F 0 "#PWR2" H 2050 2550 50  0001 C CNN
F 1 "GND" H 2050 2650 50  0000 C CNN
F 2 "" H 2050 2800 60  0000 C CNN
F 3 "" H 2050 2800 60  0000 C CNN
	1    2050 2800
	1    0    0    -1  
$EndComp
NoConn ~ 6300 4450
NoConn ~ 6300 4250
$Comp
L C C6
U 1 1 56A1F270
P 6700 4550
F 0 "C6" H 6725 4650 50  0000 L CNN
F 1 "100nF" H 6725 4450 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 6738 4400 30  0001 C CNN
F 3 "" H 6700 4550 60  0000 C CNN
	1    6700 4550
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR16
U 1 1 56A1F3BB
P 6850 4350
F 0 "#PWR16" H 6850 4100 50  0001 C CNN
F 1 "GND" H 6850 4200 50  0000 C CNN
F 2 "" H 6850 4350 60  0000 C CNN
F 3 "" H 6850 4350 60  0000 C CNN
	1    6850 4350
	0    -1   -1   0   
$EndComp
$Comp
L R R1
U 1 1 56A1F3DF
P 6700 4150
F 0 "R1" V 6780 4150 50  0000 C CNN
F 1 "680k" V 6700 4150 50  0000 C CNN
F 2 "Discret:R1" V 6630 4150 30  0001 C CNN
F 3 "" H 6700 4150 30  0000 C CNN
	1    6700 4150
	0    -1   -1   0   
$EndComp
$Comp
L Crystal_Small Y1
U 1 1 56A1F7F7
P 4650 3150
F 0 "Y1" V 4500 3200 50  0000 C CNN
F 1 "20MHz" V 4650 3350 50  0000 C CNN
F 2 "Crystals:Crystal_HC49-U_Vertical" H 4650 3150 60  0001 C CNN
F 3 "" H 4650 3150 60  0000 C CNN
	1    4650 3150
	0    -1   1    0   
$EndComp
$Comp
L VCC #PWR1
U 1 1 56A1FF3A
P 2000 2500
F 0 "#PWR1" H 2000 2350 50  0001 C CNN
F 1 "VCC" H 2000 2650 50  0000 C CNN
F 2 "" H 2000 2500 60  0000 C CNN
F 3 "" H 2000 2500 60  0000 C CNN
	1    2000 2500
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR3
U 1 1 56A20155
P 2450 4700
F 0 "#PWR3" H 2450 4450 50  0001 C CNN
F 1 "GND" H 2450 4550 50  0000 C CNN
F 2 "" H 2450 4700 60  0000 C CNN
F 3 "" H 2450 4700 60  0000 C CNN
	1    2450 4700
	0    1    1    0   
$EndComp
$Comp
L C C5
U 1 1 56A22434
P 5550 3700
F 0 "C5" H 5575 3800 50  0000 L CNN
F 1 "100nF" H 5575 3600 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 5588 3550 30  0001 C CNN
F 3 "" H 5550 3700 60  0000 C CNN
	1    5550 3700
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR4
U 1 1 56A60DBD
P 2450 4800
F 0 "#PWR4" H 2450 4550 50  0001 C CNN
F 1 "GND" H 2450 4650 50  0000 C CNN
F 2 "" H 2450 4800 60  0000 C CNN
F 3 "" H 2450 4800 60  0000 C CNN
	1    2450 4800
	0    1    1    0   
$EndComp
Wire Wire Line
	6300 4350 6550 4350
Wire Wire Line
	6550 4550 6550 4150
Wire Wire Line
	6850 4550 6850 4150
Connection ~ 6550 4350
Connection ~ 6850 4350
$Comp
L VCC #PWR11
U 1 1 56A78618
P 5800 2600
F 0 "#PWR11" H 5800 2450 50  0001 C CNN
F 1 "VCC" H 5800 2750 50  0000 C CNN
F 2 "" H 5800 2600 60  0000 C CNN
F 3 "" H 5800 2600 60  0000 C CNN
	1    5800 2600
	1    0    0    -1  
$EndComp
$Comp
L POT RV1
U 1 1 56A8F98B
P 5800 3350
F 0 "RV1" H 5800 3250 50  0000 C CNN
F 1 "1k" H 5800 3350 50  0000 C CNN
F 2 "Potentiometers:Potentiometer_Bourns_3339P_Angular_ScrewUp" H 5800 3350 60  0001 C CNN
F 3 "" H 5800 3350 60  0000 C CNN
	1    5800 3350
	1    0    0    -1  
$EndComp
NoConn ~ 6050 3350
Text Label 4350 3950 0    60   ~ 0
~RESET
Text Label 4350 2800 0    60   ~ 0
MOSI
Text Label 4350 3000 0    60   ~ 0
SCK
Text Label 3150 1750 2    60   ~ 0
MISO
Text Label 3150 1850 2    60   ~ 0
SCK
Text Label 3150 1950 2    60   ~ 0
~RESET
Text Label 3650 1850 0    60   ~ 0
MOSI
$Comp
L VCC #PWR5
U 1 1 56A92B5A
P 3750 1750
F 0 "#PWR5" H 3750 1600 50  0001 C CNN
F 1 "VCC" H 3750 1900 50  0000 C CNN
F 2 "" H 3750 1750 60  0000 C CNN
F 3 "" H 3750 1750 60  0000 C CNN
	1    3750 1750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR6
U 1 1 56A92BBD
P 3750 1950
F 0 "#PWR6" H 3750 1700 50  0001 C CNN
F 1 "GND" H 3750 1800 50  0000 C CNN
F 2 "" H 3750 1950 60  0000 C CNN
F 3 "" H 3750 1950 60  0000 C CNN
	1    3750 1950
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X03 P1
U 1 1 56A92D65
P 3400 1850
F 0 "P1" H 3400 2050 50  0000 C CNN
F 1 "ISP" H 3400 1650 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03" H 3400 650 60  0001 C CNN
F 3 "" H 3400 650 60  0000 C CNN
	1    3400 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 1950 3750 1950
Wire Wire Line
	3650 1750 3750 1750
$Comp
L ATMEGA328P-P IC1
U 1 1 56A94A6B
P 3350 3600
F 0 "IC1" H 2600 4850 40  0000 L BNN
F 1 "ATMEGA328P-P" H 3750 2200 40  0000 L BNN
F 2 "Housings_DIP:DIP-28_W7.62mm" H 3350 3600 30  0001 C CIN
F 3 "" H 3350 3600 60  0000 C CNN
	1    3350 3600
	1    0    0    -1  
$EndComp
Text Label 4350 2900 0    60   ~ 0
MISO
$Comp
L C_Small C2
U 1 1 56A9757F
P 4750 3250
F 0 "C2" V 4700 3300 50  0000 L CNN
F 1 "18pF" V 4850 3200 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 4750 3250 60  0001 C CNN
F 3 "" H 4750 3250 60  0000 C CNN
	1    4750 3250
	0    1    1    0   
$EndComp
$Comp
L C_Small C1
U 1 1 56A976A7
P 4750 3050
F 0 "C1" V 4800 3100 50  0000 L CNN
F 1 "18pF" V 4650 3000 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 4750 3050 60  0001 C CNN
F 3 "" H 4750 3050 60  0000 C CNN
	1    4750 3050
	0    1    1    0   
$EndComp
$Comp
L GND #PWR7
U 1 1 56A976F1
P 4850 3050
F 0 "#PWR7" H 4850 2800 50  0001 C CNN
F 1 "GND" H 4850 2900 50  0000 C CNN
F 2 "" H 4850 3050 60  0000 C CNN
F 3 "" H 4850 3050 60  0000 C CNN
	1    4850 3050
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR8
U 1 1 56A97733
P 4850 3250
F 0 "#PWR8" H 4850 3000 50  0001 C CNN
F 1 "GND" H 4850 3100 50  0000 C CNN
F 2 "" H 4850 3250 60  0000 C CNN
F 3 "" H 4850 3250 60  0000 C CNN
	1    4850 3250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4350 3100 4350 3050
Wire Wire Line
	4350 3050 4650 3050
Wire Wire Line
	4650 3250 4350 3250
Wire Wire Line
	4350 3250 4350 3200
NoConn ~ 4350 2700
Wire Wire Line
	2350 2800 2450 2800
Connection ~ 2350 2500
Connection ~ 2350 2800
Wire Wire Line
	2000 2500 2450 2500
Connection ~ 2050 2500
Wire Wire Line
	2350 2800 2350 2500
NoConn ~ 2450 3100
Wire Wire Line
	5200 4000 5550 4000
Wire Wire Line
	5550 4000 5550 3850
Wire Wire Line
	5550 3550 6450 3550
$Comp
L C C4
U 1 1 56A91B47
P 5600 5000
F 0 "C4" H 5625 5100 50  0000 L CNN
F 1 "100nF" H 5625 4900 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 5638 4850 30  0001 C CNN
F 3 "" H 5600 5000 60  0000 C CNN
	1    5600 5000
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P2
U 1 1 56A9E70B
P 6650 3550
F 0 "P2" H 6650 3750 50  0000 C CNN
F 1 "MAIN" V 6750 3550 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 6650 3550 60  0001 C CNN
F 3 "" H 6650 3550 60  0000 C CNN
	1    6650 3550
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR13
U 1 1 56A9E7D0
P 6450 3450
F 0 "#PWR13" H 6450 3300 50  0001 C CNN
F 1 "VCC" H 6450 3600 50  0000 C CNN
F 2 "" H 6450 3450 60  0000 C CNN
F 3 "" H 6450 3450 60  0000 C CNN
	1    6450 3450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR14
U 1 1 56A9E875
P 6450 3650
F 0 "#PWR14" H 6450 3400 50  0001 C CNN
F 1 "GND" H 6450 3500 50  0000 C CNN
F 2 "" H 6450 3650 60  0000 C CNN
F 3 "" H 6450 3650 60  0000 C CNN
	1    6450 3650
	1    0    0    -1  
$EndComp
Text Label 4350 3350 0    60   ~ 0
IN0
Text Label 4350 3450 0    60   ~ 0
IN1
Text Label 4350 3550 0    60   ~ 0
IN2
Text Label 4350 3650 0    60   ~ 0
IN3
Text Label 4350 3750 0    60   ~ 0
IN4
Text Label 4350 3850 0    60   ~ 0
IN5
Text Label 4350 4100 0    60   ~ 0
IN6
Text Label 4350 4200 0    60   ~ 0
IN7
Text Label 4350 4500 0    60   ~ 0
IN8
Text Label 4350 4600 0    60   ~ 0
IN9
Text Label 4350 2500 0    60   ~ 0
IN10
Text Label 4350 2600 0    60   ~ 0
IN11
Wire Wire Line
	5300 4350 5200 4350
Wire Wire Line
	5200 4350 5200 4000
Wire Wire Line
	5300 4250 4750 4250
Wire Wire Line
	4750 4250 4750 4300
Wire Wire Line
	4750 4300 4350 4300
Wire Wire Line
	4350 4400 4750 4400
Wire Wire Line
	4750 4400 4750 4450
Wire Wire Line
	4750 4450 5300 4450
Text Label 7750 3100 2    60   ~ 0
IN0
Text Label 7750 3200 2    60   ~ 0
IN1
Text Label 7750 3300 2    60   ~ 0
IN2
Text Label 7750 3400 2    60   ~ 0
IN3
Text Label 7750 3500 2    60   ~ 0
IN4
Text Label 7750 3600 2    60   ~ 0
IN5
Text Label 7750 3700 2    60   ~ 0
IN6
Text Label 7750 3800 2    60   ~ 0
IN7
Text Label 7750 3900 2    60   ~ 0
IN8
Text Label 7750 4000 2    60   ~ 0
IN9
Text Label 7750 4100 2    60   ~ 0
IN10
Text Label 7750 4200 2    60   ~ 0
IN11
$Comp
L R R2
U 1 1 56AA9AFF
P 5350 2800
F 0 "R2" V 5430 2800 50  0000 C CNN
F 1 "4k7" V 5350 2800 50  0000 C CNN
F 2 "Discret:R1" V 5280 2800 50  0001 C CNN
F 3 "" H 5350 2800 50  0000 C CNN
	1    5350 2800
	0    1    1    0   
$EndComp
Wire Wire Line
	4350 2800 5200 2800
Text Label 4350 4800 0    60   ~ 0
~OR
Text Label 7750 4300 2    60   ~ 0
~OE
Text Label 6150 3550 0    60   ~ 0
LUMA
$Comp
L CONN_02X14 P3
U 1 1 56B0DCD2
P 8000 3750
F 0 "P3" H 8000 4500 50  0000 C CNN
F 1 "INPUT/BYPASS" V 8000 3750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x14" H 8000 2600 60  0001 C CNN
F 3 "" H 8000 2600 60  0000 C CNN
	1    8000 3750
	1    0    0    -1  
$EndComp
Text Label 8250 4300 0    60   ~ 0
~OE
Text Label 8250 3100 0    60   ~ 0
IN0
Text Label 8250 3200 0    60   ~ 0
IN1
Text Label 8250 3300 0    60   ~ 0
IN2
Text Label 8250 3400 0    60   ~ 0
IN3
Text Label 8250 3500 0    60   ~ 0
IN4
Text Label 8250 3600 0    60   ~ 0
IN5
Text Label 8250 3700 0    60   ~ 0
IN6
Text Label 8250 3800 0    60   ~ 0
IN7
Text Label 8250 3900 0    60   ~ 0
IN8
Text Label 8250 4000 0    60   ~ 0
IN9
Text Label 8250 4100 0    60   ~ 0
IN10
Text Label 8250 4200 0    60   ~ 0
IN11
$Comp
L JUMPER JP1
U 1 1 56B2539D
P 5500 3100
F 0 "JP1" H 5500 3250 50  0000 C CNN
F 1 "BACKDROP" H 5500 3020 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02" H 5500 3100 50  0001 C CNN
F 3 "" H 5500 3100 50  0000 C CNN
	1    5500 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 3000 5800 3200
Connection ~ 5800 3100
Wire Wire Line
	5550 3350 5550 3550
Wire Wire Line
	5200 2800 5200 3100
$Comp
L BC547 Q1
U 1 1 56B5F1A8
P 5700 2800
F 0 "Q1" H 5900 2875 50  0000 L CNN
F 1 "BC547B" H 5900 2800 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-92_Inline_Narrow_Oval" H 5900 2725 50  0001 L CIN
F 3 "" H 5700 2800 50  0000 L CNN
	1    5700 2800
	1    0    0    -1  
$EndComp
Text Label 4350 4700 0    60   ~ 0
~OE
Text Label 8250 4400 0    60   ~ 0
~OR
Text Label 7750 4400 2    60   ~ 0
~OR
$EndSCHEMATC
