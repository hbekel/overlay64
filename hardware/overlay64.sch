EESchema Schematic File Version 2
LIBS:overlay64-rescue
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
Title "Overlay64 -- Video Overlay Module"
Date "2016-02-02"
Rev "1"
Comp "Henning Bekel"
Comment1 "http://www.henning-bekel.de/overlay64"
Comment2 "Licensed under CERN OHL v.1.2"
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L LM1881 U1
U 1 1 56A1EDB5
P 6350 2800
F 0 "U1" H 6100 3050 60  0000 C CNN
F 1 "LM1881" H 6500 2550 60  0000 C CNN
F 2 "Housings_DIP:DIP-8_W7.62mm" H 6300 2650 60  0001 C CNN
F 3 "" H 6300 2650 60  0000 C CNN
	1    6350 2800
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG01
U 1 1 56A1EF0B
P 10500 6250
F 0 "#FLG01" H 10500 6345 50  0001 C CNN
F 1 "PWR_FLAG" H 10500 6430 50  0000 C CNN
F 2 "" H 10500 6250 60  0000 C CNN
F 3 "" H 10500 6250 60  0000 C CNN
	1    10500 6250
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR02
U 1 1 56A1EF40
P 10500 6250
F 0 "#PWR02" H 10500 6100 50  0001 C CNN
F 1 "VCC" H 10500 6400 50  0000 C CNN
F 2 "" H 10500 6250 60  0000 C CNN
F 3 "" H 10500 6250 60  0000 C CNN
	1    10500 6250
	-1   0    0    1   
$EndComp
$Comp
L PWR_FLAG #FLG03
U 1 1 56A1EF56
P 10950 6250
F 0 "#FLG03" H 10950 6345 50  0001 C CNN
F 1 "PWR_FLAG" H 10950 6430 50  0000 C CNN
F 2 "" H 10950 6250 60  0000 C CNN
F 3 "" H 10950 6250 60  0000 C CNN
	1    10950 6250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 56A1EF6C
P 10950 6250
F 0 "#PWR04" H 10950 6000 50  0001 C CNN
F 1 "GND" H 10950 6100 50  0000 C CNN
F 2 "" H 10950 6250 60  0000 C CNN
F 3 "" H 10950 6250 60  0000 C CNN
	1    10950 6250
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR05
U 1 1 56A1F13D
P 6350 3350
F 0 "#PWR05" H 6350 3200 50  0001 C CNN
F 1 "VCC" H 6350 3500 50  0000 C CNN
F 2 "" H 6350 3350 60  0000 C CNN
F 3 "" H 6350 3350 60  0000 C CNN
	1    6350 3350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR06
U 1 1 56A1F155
P 6350 3650
F 0 "#PWR06" H 6350 3400 50  0001 C CNN
F 1 "GND" H 6350 3500 50  0000 C CNN
F 2 "" H 6350 3650 60  0000 C CNN
F 3 "" H 6350 3650 60  0000 C CNN
	1    6350 3650
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 56A1F16D
P 2650 1250
F 0 "C1" V 2500 1200 50  0000 L CNN
F 1 "100nF" V 2800 1150 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 2688 1100 30  0001 C CNN
F 3 "" H 2650 1250 60  0000 C CNN
	1    2650 1250
	0    1    1    0   
$EndComp
NoConn ~ 6850 2700
$Comp
L C C5
U 1 1 56A1F270
P 7450 3000
F 0 "C5" H 7475 3100 50  0000 L CNN
F 1 "100nF" H 7475 2900 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 7488 2850 30  0001 C CNN
F 3 "" H 7450 3000 60  0000 C CNN
	1    7450 3000
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR07
U 1 1 56A1F3BB
P 7600 2800
F 0 "#PWR07" H 7600 2550 50  0001 C CNN
F 1 "GND" H 7600 2650 50  0000 C CNN
F 2 "" H 7600 2800 60  0000 C CNN
F 3 "" H 7600 2800 60  0000 C CNN
	1    7600 2800
	0    -1   -1   0   
$EndComp
$Comp
L R R5
U 1 1 56A1F3DF
P 7450 2600
F 0 "R5" V 7530 2600 50  0000 C CNN
F 1 "680k" V 7450 2600 50  0000 C CNN
F 2 "Discret:R3" V 7380 2600 30  0001 C CNN
F 3 "" H 7450 2600 30  0000 C CNN
	1    7450 2600
	0    -1   -1   0   
$EndComp
$Comp
L Crystal_Small Y1
U 1 1 56A1F7F7
P 1500 2400
F 0 "Y1" V 1350 2450 50  0000 C CNN
F 1 "20MHz" V 1500 2600 50  0000 C CNN
F 2 "Crystals:Crystal_HC49-U_Vertical" H 1500 2400 60  0001 C CNN
F 3 "" H 1500 2400 60  0000 C CNN
	1    1500 2400
	0    1    1    0   
$EndComp
$Comp
L VCC #PWR08
U 1 1 56A1FF3A
P 2800 1250
F 0 "#PWR08" H 2800 1100 50  0001 C CNN
F 1 "VCC" H 2800 1400 50  0000 C CNN
F 2 "" H 2800 1250 60  0000 C CNN
F 3 "" H 2800 1250 60  0000 C CNN
	1    2800 1250
	1    0    0    -1  
$EndComp
$Comp
L C C4
U 1 1 56A22434
P 6100 2150
F 0 "C4" H 6125 2250 50  0000 L CNN
F 1 "100nF" H 6125 2050 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 6138 2000 30  0001 C CNN
F 3 "" H 6100 2150 60  0000 C CNN
	1    6100 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6850 2800 7300 2800
Wire Wire Line
	7300 3000 7300 2600
Wire Wire Line
	7600 3000 7600 2600
Connection ~ 7300 2800
Connection ~ 7600 2800
$Comp
L POT-RESCUE-overlay64 RV1
U 1 1 56A8F98B
P 6350 1800
F 0 "RV1" H 6350 1700 50  0000 C CNN
F 1 "1k" H 6350 1800 50  0000 C CNN
F 2 "Potentiometers:Potentiometer_Bourns_3339P_Angular_ScrewUp" H 6350 1800 60  0001 C CNN
F 3 "" H 6350 1800 60  0000 C CNN
	1    6350 1800
	-1   0    0    -1  
$EndComp
NoConn ~ 6600 1800
Text Label 6350 1400 2    60   ~ 0
MOSI
$Comp
L C_Small C3
U 1 1 56A9757F
P 1400 2500
F 0 "C3" V 1350 2550 50  0000 L CNN
F 1 "18pF" V 1500 2450 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 1400 2500 60  0001 C CNN
F 3 "" H 1400 2500 60  0000 C CNN
	1    1400 2500
	0    -1   1    0   
$EndComp
$Comp
L C_Small C2
U 1 1 56A976A7
P 1400 2300
F 0 "C2" V 1450 2350 50  0000 L CNN
F 1 "18pF" V 1300 2250 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 1400 2300 60  0001 C CNN
F 3 "" H 1400 2300 60  0000 C CNN
	1    1400 2300
	0    -1   1    0   
$EndComp
$Comp
L GND #PWR09
U 1 1 56A976F1
P 1300 2300
F 0 "#PWR09" H 1300 2050 50  0001 C CNN
F 1 "GND" H 1300 2150 50  0000 C CNN
F 2 "" H 1300 2300 60  0000 C CNN
F 3 "" H 1300 2300 60  0000 C CNN
	1    1300 2300
	0    1    -1   0   
$EndComp
$Comp
L GND #PWR010
U 1 1 56A97733
P 1300 2500
F 0 "#PWR010" H 1300 2250 50  0001 C CNN
F 1 "GND" H 1300 2350 50  0000 C CNN
F 2 "" H 1300 2500 60  0000 C CNN
F 3 "" H 1300 2500 60  0000 C CNN
	1    1300 2500
	0    1    -1   0   
$EndComp
Wire Wire Line
	1500 2300 2000 2300
Wire Wire Line
	1500 2500 2000 2500
Wire Wire Line
	5750 2450 6100 2450
Wire Wire Line
	6100 2450 6100 2300
Wire Wire Line
	6100 2000 7000 2000
$Comp
L C C6
U 1 1 56A91B47
P 6350 3500
F 0 "C6" H 6375 3600 50  0000 L CNN
F 1 "100nF" H 6375 3400 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 6388 3350 30  0001 C CNN
F 3 "" H 6350 3500 60  0000 C CNN
	1    6350 3500
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P4
U 1 1 56A9E70B
P 7200 2000
F 0 "P4" H 7200 2200 50  0000 C CNN
F 1 "MAIN" V 7300 2000 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x03" H 7200 2000 60  0001 C CNN
F 3 "" H 7200 2000 60  0000 C CNN
	1    7200 2000
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR011
U 1 1 56A9E7D0
P 7000 1900
F 0 "#PWR011" H 7000 1750 50  0001 C CNN
F 1 "VCC" H 7000 2050 50  0000 C CNN
F 2 "" H 7000 1900 60  0000 C CNN
F 3 "" H 7000 1900 60  0000 C CNN
	1    7000 1900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR012
U 1 1 56A9E875
P 7000 2100
F 0 "#PWR012" H 7000 1850 50  0001 C CNN
F 1 "GND" H 7000 1950 50  0000 C CNN
F 2 "" H 7000 2100 60  0000 C CNN
F 3 "" H 7000 2100 60  0000 C CNN
	1    7000 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 2800 5750 2800
Wire Wire Line
	5750 2800 5750 2450
Text Label 4000 5200 0    60   ~ 0
C1
Text Label 5400 6800 2    60   ~ 0
C0
Text Label 6700 2000 0    60   ~ 0
LUMA
Wire Wire Line
	6100 1800 6100 2000
Text Label 4000 5100 0    60   ~ 0
C0
Text Label 5400 6700 2    60   ~ 0
C1
$Comp
L CONN_01X01 P5
U 1 1 57445960
P 9350 6100
F 0 "P5" H 9350 6200 50  0000 C CNN
F 1 "HOLE" V 9450 6100 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_4mm" H 9350 6100 60  0001 C CNN
F 3 "" H 9350 6100 60  0000 C CNN
	1    9350 6100
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X01 P6
U 1 1 57445B56
P 9600 6100
F 0 "P6" H 9600 6200 50  0000 C CNN
F 1 "HOLE" V 9700 6100 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_4mm" H 9600 6100 60  0001 C CNN
F 3 "" H 9600 6100 60  0000 C CNN
	1    9600 6100
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X01 P7
U 1 1 57445BAB
P 9850 6100
F 0 "P7" H 9850 6200 50  0000 C CNN
F 1 "HOLE" V 9950 6100 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_4mm" H 9850 6100 60  0001 C CNN
F 3 "" H 9850 6100 60  0000 C CNN
	1    9850 6100
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X01 P8
U 1 1 57445C03
P 10050 6100
F 0 "P8" H 10050 6200 50  0000 C CNN
F 1 "HOLE" V 10150 6100 50  0000 C CNN
F 2 "Mounting_Holes:MountingHole_4mm" H 10050 6100 60  0001 C CNN
F 3 "" H 10050 6100 60  0000 C CNN
	1    10050 6100
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR013
U 1 1 57445C4B
P 9350 6300
F 0 "#PWR013" H 9350 6050 50  0001 C CNN
F 1 "GND" H 9350 6150 50  0000 C CNN
F 2 "" H 9350 6300 60  0000 C CNN
F 3 "" H 9350 6300 60  0000 C CNN
	1    9350 6300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR014
U 1 1 57445CA2
P 9600 6300
F 0 "#PWR014" H 9600 6050 50  0001 C CNN
F 1 "GND" H 9600 6150 50  0000 C CNN
F 2 "" H 9600 6300 60  0000 C CNN
F 3 "" H 9600 6300 60  0000 C CNN
	1    9600 6300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR015
U 1 1 57445CF2
P 9850 6300
F 0 "#PWR015" H 9850 6050 50  0001 C CNN
F 1 "GND" H 9850 6150 50  0000 C CNN
F 2 "" H 9850 6300 60  0000 C CNN
F 3 "" H 9850 6300 60  0000 C CNN
	1    9850 6300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR016
U 1 1 57445D42
P 10050 6300
F 0 "#PWR016" H 10050 6050 50  0001 C CNN
F 1 "GND" H 10050 6150 50  0000 C CNN
F 2 "" H 10050 6300 60  0000 C CNN
F 3 "" H 10050 6300 60  0000 C CNN
	1    10050 6300
	1    0    0    -1  
$EndComp
$Comp
L ATMEGA1284-P IC1
U 1 1 5799B0E7
P 3000 3500
F 0 "IC1" H 2150 5380 50  0000 L BNN
F 1 "ATMEGA1284" H 3400 1550 50  0000 L BNN
F 2 "Housings_DIP:DIP-40_W15.24mm" H 3000 3500 50  0001 C CIN
F 3 "" H 3000 3500 50  0000 C CNN
	1    3000 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2000 2500 2000 2600
Wire Wire Line
	2000 2300 2000 2200
$Comp
L VCC #PWR017
U 1 1 5799D81B
P 3000 1500
F 0 "#PWR017" H 3000 1350 50  0001 C CNN
F 1 "VCC" H 3000 1650 50  0000 C CNN
F 2 "" H 3000 1500 60  0000 C CNN
F 3 "" H 3000 1500 60  0000 C CNN
	1    3000 1500
	1    0    0    -1  
$EndComp
NoConn ~ 2000 3000
$Comp
L GND #PWR018
U 1 1 5799D944
P 2800 5500
F 0 "#PWR018" H 2800 5250 50  0001 C CNN
F 1 "GND" H 2800 5350 50  0000 C CNN
F 2 "" H 2800 5500 50  0000 C CNN
F 3 "" H 2800 5500 50  0000 C CNN
	1    2800 5500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR019
U 1 1 5799DAD2
P 3000 5500
F 0 "#PWR019" H 3000 5250 50  0001 C CNN
F 1 "GND" H 3000 5350 50  0000 C CNN
F 2 "" H 3000 5500 50  0000 C CNN
F 3 "" H 3000 5500 50  0000 C CNN
	1    3000 5500
	1    0    0    -1  
$EndComp
Text Label 5650 2700 2    60   ~ 0
~HSYNC
Text Label 5650 2900 2    60   ~ 0
~VSYNC
Wire Wire Line
	5650 2900 5850 2900
Wire Wire Line
	5650 2700 5850 2700
Text Label 4000 2700 0    60   ~ 0
~HSYNC
Text Label 4000 4800 0    60   ~ 0
~VSYNC
Text Label 4000 4500 0    60   ~ 0
BOOT
Wire Wire Line
	4000 4600 4350 4600
Wire Wire Line
	4000 4700 4350 4700
$Comp
L R R1
U 1 1 5799F6F4
P 4500 4600
F 0 "R1" V 4400 4600 50  0000 C CNN
F 1 "68" V 4500 4600 50  0000 C CNN
F 2 "Discret:R3" V 4430 4600 50  0001 C CNN
F 3 "" H 4500 4600 50  0000 C CNN
	1    4500 4600
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 5799F7C1
P 4500 4700
F 0 "R2" V 4600 4700 50  0000 C CNN
F 1 "68" V 4500 4700 50  0000 C CNN
F 2 "Discret:R3" V 4430 4700 50  0001 C CNN
F 3 "" H 4500 4700 50  0000 C CNN
	1    4500 4700
	0    1    1    0   
$EndComp
$Comp
L R R3
U 1 1 5799FAC1
P 4800 4450
F 0 "R3" V 4880 4450 50  0000 C CNN
F 1 "1k5" V 4800 4450 50  0000 C CNN
F 2 "Discret:R3" V 4730 4450 50  0001 C CNN
F 3 "" H 4800 4450 50  0000 C CNN
	1    4800 4450
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR020
U 1 1 5799FB47
P 4800 4300
F 0 "#PWR020" H 4800 4150 50  0001 C CNN
F 1 "VCC" H 4800 4450 50  0000 C CNN
F 2 "" H 4800 4300 50  0000 C CNN
F 3 "" H 4800 4300 50  0000 C CNN
	1    4800 4300
	1    0    0    -1  
$EndComp
$Comp
L ZENER D1
U 1 1 5799FCCF
P 4800 4950
F 0 "D1" H 4800 5050 50  0000 C CNN
F 1 "3.6V" H 4800 4850 50  0000 C CNN
F 2 "custom:D3" H 4800 4950 50  0001 C CNN
F 3 "" H 4800 4950 50  0000 C CNN
	1    4800 4950
	0    1    1    0   
$EndComp
$Comp
L ZENER D2
U 1 1 5799FFFE
P 5100 4950
F 0 "D2" H 5100 5050 50  0000 C CNN
F 1 "3.6V" H 5100 4850 50  0000 C CNN
F 2 "custom:D3" H 5100 4950 50  0001 C CNN
F 3 "" H 5100 4950 50  0000 C CNN
	1    5100 4950
	0    1    1    0   
$EndComp
$Comp
L GND #PWR021
U 1 1 579A0108
P 4950 5150
F 0 "#PWR021" H 4950 4900 50  0001 C CNN
F 1 "GND" H 4950 5000 50  0000 C CNN
F 2 "" H 4950 5150 50  0000 C CNN
F 3 "" H 4950 5150 50  0000 C CNN
	1    4950 5150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 4750 4800 4600
Connection ~ 4800 4600
Wire Wire Line
	5100 4750 5100 4700
Connection ~ 5100 4700
Wire Wire Line
	4800 5150 5100 5150
Connection ~ 4950 5150
$Comp
L CONN_01X05 P1
U 1 1 579A13A4
P 5700 4700
F 0 "P1" H 5700 5000 50  0000 C CNN
F 1 "USB" V 5800 4700 50  0000 C CNN
F 2 "custom:USB-MINI-B-TH" H 5700 4700 50  0001 C CNN
F 3 "" H 5700 4700 50  0000 C CNN
	1    5700 4700
	1    0    0    -1  
$EndComp
NoConn ~ 5500 4500
Text Label 5400 4600 2    60   ~ 0
D-
Text Label 5400 4700 2    60   ~ 0
D+
NoConn ~ 5500 4800
$Comp
L GND #PWR022
U 1 1 579A190A
P 5500 4900
F 0 "#PWR022" H 5500 4650 50  0001 C CNN
F 1 "GND" H 5500 4750 50  0000 C CNN
F 2 "" H 5500 4900 50  0000 C CNN
F 3 "" H 5500 4900 50  0000 C CNN
	1    5500 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 4600 5500 4600
Wire Wire Line
	4650 4700 5500 4700
Text Label 4000 3200 0    60   ~ 0
MOSI
NoConn ~ 4000 3100
Text Label 6850 2900 0    60   ~ 0
~BURST
Text Label 4000 2900 0    60   ~ 0
~BURST
$Comp
L CONN_01X16 P2
U 1 1 57A0D9B5
P 7250 4950
F 0 "P2" H 7250 5800 50  0000 C CNN
F 1 "INPUT" V 7350 4950 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x16" H 7250 4950 50  0001 C CNN
F 3 "" H 7250 4950 50  0000 C CNN
	1    7250 4950
	1    0    0    -1  
$EndComp
Text Label 7050 5700 2    60   ~ 0
IN0
Text Label 7050 5600 2    60   ~ 0
IN1
Text Label 7050 5500 2    60   ~ 0
IN2
Text Label 7050 5400 2    60   ~ 0
IN3
Text Label 7050 5300 2    60   ~ 0
IN4
Text Label 7050 5200 2    60   ~ 0
IN5
Text Label 7050 5100 2    60   ~ 0
IN6
Text Label 7050 5000 2    60   ~ 0
IN7
Text Label 7050 4900 2    60   ~ 0
IN8
Text Label 7050 4800 2    60   ~ 0
IN9
Text Label 7050 4700 2    60   ~ 0
IN10
Text Label 7050 4600 2    60   ~ 0
IN11
Text Label 7050 4500 2    60   ~ 0
IN12
Text Label 7050 4400 2    60   ~ 0
IN13
Text Label 7050 4300 2    60   ~ 0
IN14
Text Label 7050 4200 2    60   ~ 0
IN15
Text Label 4000 1800 0    60   ~ 0
IN0
Text Label 4000 1900 0    60   ~ 0
IN1
Text Label 4000 2000 0    60   ~ 0
IN2
Text Label 4000 2100 0    60   ~ 0
IN3
Text Label 4000 2200 0    60   ~ 0
IN4
Text Label 4000 2300 0    60   ~ 0
IN5
Text Label 4000 2400 0    60   ~ 0
IN6
Text Label 4000 2500 0    60   ~ 0
IN7
Text Label 4000 4300 0    60   ~ 0
IN8
Text Label 4000 4200 0    60   ~ 0
IN9
Text Label 4000 4100 0    60   ~ 0
IN10
Text Label 4000 4000 0    60   ~ 0
IN11
Text Label 4000 3900 0    60   ~ 0
IN12
Text Label 4000 3800 0    60   ~ 0
IN13
Text Label 4000 3700 0    60   ~ 0
IN14
Text Label 4000 3600 0    60   ~ 0
IN15
Text Label 4000 2800 0    60   ~ 0
C2
Text Label 4000 3000 0    60   ~ 0
C3
Text Label 4000 3300 0    60   ~ 0
C4
Text Label 4000 3400 0    60   ~ 0
C5
Text Label 4000 4900 0    60   ~ 0
C6
Text Label 4000 5000 0    60   ~ 0
C7
$Comp
L CONN_01X08 P3
U 1 1 57A0F745
P 5600 6450
F 0 "P3" H 5600 6900 50  0000 C CNN
F 1 "CONTROL" V 5700 6450 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x08" H 5600 6450 50  0001 C CNN
F 3 "" H 5600 6450 50  0000 C CNN
	1    5600 6450
	1    0    0    -1  
$EndComp
Text Label 5400 6600 2    60   ~ 0
C2
Text Label 5400 6500 2    60   ~ 0
C3
Text Label 5400 6400 2    60   ~ 0
C4
Text Label 5400 6300 2    60   ~ 0
C5
Text Label 5400 6200 2    60   ~ 0
C6
Text Label 5400 6100 2    60   ~ 0
C7
$Comp
L GND #PWR023
U 1 1 57A1101A
P 2500 1400
F 0 "#PWR023" H 2500 1150 50  0001 C CNN
F 1 "GND" H 2500 1250 50  0000 C CNN
F 2 "" H 2500 1400 50  0000 C CNN
F 3 "" H 2500 1400 50  0000 C CNN
	1    2500 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 1400 2500 1250
Wire Wire Line
	2800 1500 2800 1250
$Comp
L CONN_01X16 P9
U 1 1 57AD94B2
P 7900 4950
F 0 "P9" H 7900 5800 50  0000 C CNN
F 1 "BYPASS" V 8000 4950 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x16" H 7900 4950 50  0001 C CNN
F 3 "" H 7900 4950 50  0000 C CNN
	1    7900 4950
	-1   0    0    1   
$EndComp
Text Label 8100 5700 0    60   ~ 0
IN0
Text Label 8100 5600 0    60   ~ 0
IN1
Text Label 8100 5500 0    60   ~ 0
IN2
Text Label 8100 5400 0    60   ~ 0
IN3
Text Label 8100 5300 0    60   ~ 0
IN4
Text Label 8100 5200 0    60   ~ 0
IN5
Text Label 8100 5100 0    60   ~ 0
IN6
Text Label 8100 5000 0    60   ~ 0
IN7
Text Label 8100 4900 0    60   ~ 0
IN8
Text Label 8100 4800 0    60   ~ 0
IN9
Text Label 8100 4700 0    60   ~ 0
IN10
Text Label 8100 4600 0    60   ~ 0
IN11
Text Label 8100 4500 0    60   ~ 0
IN12
Text Label 8100 4400 0    60   ~ 0
IN13
Text Label 8100 4300 0    60   ~ 0
IN14
Text Label 8100 4200 0    60   ~ 0
IN15
$Comp
L GND #PWR024
U 1 1 57B5BBC8
P 1100 4200
F 0 "#PWR024" H 1100 3950 50  0001 C CNN
F 1 "GND" H 1100 4050 50  0000 C CNN
F 2 "" H 1100 4200 50  0000 C CNN
F 3 "" H 1100 4200 50  0000 C CNN
	1    1100 4200
	1    0    0    -1  
$EndComp
Text Label 1400 4200 0    60   ~ 0
BOOT
NoConn ~ 2000 1800
Wire Wire Line
	6350 1650 6350 1400
$Comp
L SW_PUSH_SMALL_H SW1
U 1 1 5832A426
P 1250 4200
F 0 "SW1" H 1330 4310 50  0000 C CNN
F 1 "SW_BOOT" H 1610 4140 50  0000 C CNN
F 2 "custom:Button" H 1250 4400 50  0001 C CNN
F 3 "" H 1250 4400 50  0000 C CNN
	1    1250 4200
	1    0    0    -1  
$EndComp
Text Notes 600  7700 0    60   ~ 0
Overlay64 video overlay module\nCopyright 2016 Henning Bekel\n\nThis documentation describes Open Hardware and is \nlicensed under the CERN OHL v. 1.2.\n\nYou may redistribute and modify this documentation\nunder the terms of the CERN OHL v.1.2. (http://ohwr.org/cernohl).\nThis documentation is distributed WITHOUT ANY EXPRESS OR \nIMPLIED WARRANTY, INCLUDING OF MERCHANTABILITY, \nSATISFACTORY QUALITY AND FITNESS FOR A PARTICULAR \nPURPOSE. Please see the CERN OHL v.1.2 for applicable conditions
$EndSCHEMATC
