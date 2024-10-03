**** ECE 222 Course Project ****
*
* AC Analysis Test Deck - Small-Signal Input Impedance
*
* This test deck runs an AC analysis to measure 
* small-signal input impedance Zin=(Rin||Cin) of DUT.
*
***************************************************

** Parameters **
.PARAM RL = 32

** Power Supply **
Vdd1 Vdd 0 12V
Vss1 Vss 0 -12V


** Test Signal **
Vx in 0 DC 0V AC 1V

** Audio Amplifier**
XX2 in out Vdd Vss DUT

Rload out 0 RL

** AC Analysis **
.AC DEC 10 10 1Meg

.PROBE AC IR(Vx) II(Vx)
.PROBE AC zin=PAR(`1/IM(Vx)')
.PROBE AC rin=PAR(`-1/IR(Vx)')
.PROBE AC cin=PAR(`-II(Vx)/(2*acos(-1)*hertz)')

** Measure Rin at Several Frequencies **
.MEAS AC zin_1k FIND PAR(`zin') AT 1K
.MEAS AC zin_5k FIND PAR(`zin') AT 5K
.MEAS AC zin_10k FIND PAR(`zin') AT 15K
.MEAS AC zin_15k FIND PAR(`zin') AT 15K
.MEAS AC zin_20k FIND PAR(`zin') AT 20K

.OPTION POST PROBE TNOM=27 RUNLVL=6 METHOD=TRAP MAXORD=2

* Include Your Design Netlist
.INC DUT1.hsp

***********************************************
.END
