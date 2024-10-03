**** ECE 222 Course Project ****
**** Transient Analysis Test Deck ****
*
**** This test deck runs transient analysis to measure 
**** the output voltage amplitude Vout_peak and 
**** output power Pout_peak 
******************************************************

** Parameter **
.PARAM Rs = 1k
.PARAM RL = 32
.PARAM vin_rms = 0.3
.PARAM vin_peak = 'vin_rms*sqrt(2)'
.PARAM vin_freq = 1k

** Power Supply **
** Increased from 9V to 12V solve convergence 
Vdd1 Vdd 0 11V
Vss1 Vss 0 -11V

** Input Signal Source **
Vsig sig 0 DC 0 SIN(0 vin_peak vin_freq 1n)
Rsig sig in Rs

** Load and Filter **
Rload out 0 RL

** Audio Amplifier **
XX1 in out Vdd Vss DUT

** Analysis **
*.OP
.TRAN 1u 2m

.OPTION POST TNOM=27 RUNLVL=6 METHOD=TRAP MAXORD=2

** Measurements **
* Measure the output voltage peak-to-peak amplitude in the last period
* to avoid error due to offset and DC shift.
.MEAS TRAN vout_peak MAX V(out) FROM=1m TO=2m
.MEAS TRAN vout_pp PP V(out) FROM=1m TO=2m
*.MEAS Pout_rms PARAM='0.5*vout_peak**2/RL'
.MEAS Pout_rms PARAM='1/8*vout_pp**2/RL'
* Note that Idd and Iss have DC and half-sinusoidal parts.
* The latter is due to the class-AB operation 
* of the output stages of op-amp and buffer chips.
.MEAS TRAN idd_avg AVG I(Vdd1) FROM=1m TO=2m
.MEAS TRAN iss_avg AVG I(Vss1) FROM=1m TO=2m
.MEAS TRAN idd_peak MIN I(Vdd1) FROM=1m TO=2m
.MEAS TRAN iss_peak MAX I(Vss1) FROM=1m TO=2m

* Include Your Design Netlist
.INC DUT.hsp

***********************************************
.end
