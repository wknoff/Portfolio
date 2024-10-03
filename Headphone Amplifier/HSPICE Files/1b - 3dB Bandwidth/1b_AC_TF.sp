**** ECE 222 Course Project ****
*
* AC Analysis Test Deck - Small-Signal Gain and Bandwidth ****
* This test deck runs AC analysis to measure 
* the small-signal frequency response,
* and to find gain and bandwidth of the DUT.
* 
***************************************************

** Parameter **
.PARAM Rs = 100
.PARAM RL = 32

** Power Supply **
Vdd1 vdd 0 9V
Vss1 vss 0 -9V

** Signal Source **
Vsig sig 0 DC 0V AC 1V
Rsig sig in Rs

** Audio Amplifier **
XX1 in out vdd vss DUT

** Load **
Rload out 0 RL

** Analysis **

.AC DEC 10 1 100K

.OPTION POST TNOM=27

** Measurements **

.MEAS AC gain_mid MAX VM(out)
.MEAS AC gain_dB MAX VDB(out)

.MEAS AC bw_f1 WHEN VDB(out)='gain_dB-3' RISE=1
.MEAS AC bw_f2 WHEN VDB(out)='gain_dB-3' FALL=1
.MEAS bw_3dB PARAM='bw_f2-bw_f1'

* Include Your Design Netlist
.INC DUT.hsp

.end
