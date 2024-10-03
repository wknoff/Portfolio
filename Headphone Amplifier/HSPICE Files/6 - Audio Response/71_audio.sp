**** ECE 222 Course Project ****
*
* Transient Analysis Test Deck - Audio Test
*
* This test deck runs an AC analysis to test DUT1
* with an stereo audio input generated from a WAV file.
* Due to convengence issue, 
* only the voltage-gain stage (DUT1) is tested here. 
* So set RL=1kohm.
*
***************************************************

** Parameters **
.PARAM RS = 1k
.PARAM RL = 1k
.PARAM vin_rms = 0.3
.PARAM vin_peak = 'vin_rms*sqrt(2)'

.PARAM FULLSCALE = 1
.PARAM EGAIN = 'vin_peak/FULLSCALE'
.PARAM WavSampleRate = 44100
.PARAM tstep = 1/WavSampleRate
.PARAM tlength = 18.5

.parameter wav_left=str('left.txt')
.parameter wav_right=str('right.txt')

** Power Supply **
Vdd1 vdd 0 12V
Vss1 vss 0 -12V

** Read stereo input from two text files
Vsig1 sig1 0 PWL PWLFILE=str(wav_left)  TD=1n
Vsig2 sig2 0 PWL PWLFILE=str(wav_right) TD=1n

** Adjust input signal amplitude 
E1 sig10 0 sig1 0 EGAIN
E2 sig20 0 sig2 0 EGAIN

Rsig1 sig10 in1 RS
Rsig2 sig20 in2 RS

** Stereo channels of the DUT amplifier
XX1 in1 out1 vdd vss DUT
XX2 in2 out2 vdd vss DUT

** AC coupling at outputs
C1 out1 load1 1u
C2 out2 load2 1u

RL1 load1 0 RL
RL2 load2 0 RL

** Transient analysis
*.OP
.TRAN tstep tlength

** Record output signals
** The signal amplitude is adjusted back to the same level as original WAV
.STIM TRAN PWL FILENAME=recording_left vsrc[0]=PAR('V(load1)/EGAIN')
.STIM TRAN PWL FILENAME=recording_right vsrc[0]=PAR('V(load2)/EGAIN')

.OPTION POST PROBE TNOM=27 NUMDGT=10 RUNLVL=6 METHOD=TRAP MAXORD=2

.INC DUT1.hsp

.end
