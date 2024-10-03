**** ECE 222 Course Project ****
*
* Transient Analysis Test Deck - Total Harmonic Distortion (THD)
* 
* This test deck runs transient analysis to measure 
* the total harmonic distortion (THD) of DUT
*
***************************************************

** Parameter **
.PARAM Rs = 1k
.PARAM RL = 32
.PARAM vin_rms = 0.3
.PARAM vin_peak = 'vin_rms*sqrt(2)'
.PARAM vin_freq = 1k
.PARAM fft_f0 = 'vin_freq'

** Power Supply **
Vdd1 Vdd 0 12V
Vss1 Vss 0 -12V

** Transient Signal Source **
Vsig sig 0 DC 0 SIN(0 vin_peak vin_freq 1ns)
Rsig sig Vin Rs

** Noise Signal Source **
*Vsig1 Vin 0 0V AC 1

** DUT Amplifier **
XX1 Vin Vout Vdd Vss DUT

** Load and Filter **
Rload Vout 0 RL

** Transient Analysis**
*.OP
.TRAN 1u 18m

** FFT to Measure THD and SNR **
** FFT frequency increment dF=1/(STOP-START)
** Set FFT window size so that it has an integer number of periods
** Make sure there are enough transient data points within the window >NP
*.FOUR fft_freq V(Vout) 
.FFT V(Vout) START=2m STOP=18m NP=8192 WINDOW=HAMM FREQ=fft_f0 
.MEAS FFT THD_db THD VM(Vout) nbharm = 40
.MEAS FFT SNDR_db SNDR V(Vout)

** Noise Analysis **
*.ac dec 10 10 100k
*.noise V(Vout) Vsig1
*.print noise onoise
*.print noise inoise

** Simulation Options **
.OPTION POST TNOM=27 RUNLVL=6 METHOD=TRAP MAXORD=2

** Include DUT
.INC DUT.hsp

***********************************************
.end
