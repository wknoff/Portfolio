**** ECE 222 Course Project ****
*
* AC Analysis Test Deck - Small-Signal Output Impedance
*
* This test deck runs an AC analysis to measure 
* small-signal output impedance Zout=(Rout||Cout) of DUT.
*
***************************************************

** Parameters **
.PARAM Rs = 1K

** Power Supply **
Vdd1 Vdd 0 12V
Vss1 Vss 0 -12V


** Test Signal **
Vx out 0 DC 0V AC 1V

** Audio Amplifier**
XX2 in out Vdd Vss DUT

Rsig in 0 Rs

** AC Analysis **
.AC DEC 10 10 1Meg

.PROBE AC IR(Vx) II(Vx)
.PROBE AC zout=PAR(`1/IM(Vx)')
.PROBE AC rout=PAR(`-1/IR(Vx)')
.PROBE AC cout=PAR(`-II(Vx)/(2*acos(-1)*hertz)')

** Measure Rout at Several Frequencies **
.MEAS AC zout_1k FIND PAR(`zout') AT 1K
.MEAS AC zout_5k FIND PAR(`zout') AT 5K
.MEAS AC zout_10k FIND PAR(`zout') AT 15K
.MEAS AC zout_15k FIND PAR(`zout') AT 15K
.MEAS AC zout_20k FIND PAR(`zout') AT 20K

.OPTION POST PROBE TNOM=27 RUNLVL=6 METHOD=TRAP MAXORD=2

* Include Your Design Netlist
.INC DUT1.hsp

***********************************************
.END
