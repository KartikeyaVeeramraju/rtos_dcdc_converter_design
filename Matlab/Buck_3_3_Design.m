close all
clear all
clc

commandwindow;
%Worst case
%V max
%I max
%d min


%Design Specs
Vin_min = 8;
Vin_max = 15;
Power_rating_max = 10;
Power_rating_min = 5;

Vout = 3.3;

Iout_max = Power_rating_max/Vout;
Iout_min = Power_rating_min/Vout;

Iout_mean = (Iout_min+Iout_max)/2;

f = 500e+3;
ripple = 0.01;
L_over_size = 1.15;



%Switch Specs
%MFR No: DMN3023L-7
R_DS_on =  32e-3;
t_rise = (14)*1e-9;
t_fall = (8)*1e-9;

%Diode Specs
RD_ON = 0.054;
Vf = 0.95;
Qrr = 0;

%inductor Resistance
R_L = 21.9e-3;
%P_ind =19;





Rmax = Vout/Iout_min;
Rmin = Vout/Iout_max;

D_max = Vout/Vin_min;
D_min = Vout/Vin_max;
Dnom = (D_max+D_min)/2;

L_crit = ((1-D_min)*Rmax)/(2*f);
L_design = 0.8*3.3e-6; % oversizing over the critical inductance
%C_filter = (1-D_max)/(8*(L_design)*(f^2)*ripple);


%max V, max I
iL_max_Vmax_Imax = Vout*((1/Rmin)+((1-D_min)/(2*L_design*f)));
iL_min_Vmax_Imax = Vout*((1/Rmin)-((1-D_min)/(2*L_design*f)));
delta4 = (iL_max_Vmax_Imax-iL_min_Vmax_Imax);
per_ripp_4 = (delta4/((iL_max_Vmax_Imax + iL_min_Vmax_Imax)*0.5))*100;
I_ind_rms = Iout_max*sqrt(1+(1/12)*(per_ripp_4/100)^2);

%max V, min I
iL_max_Vmax_Imin = Vout*((1/Rmax)+((1-D_min)/(2*L_design*f)));
iL_min_Vmax_Imin = Vout*((1/Rmax)-((1-D_min)/(2*L_design*f)));
delta3 = (iL_max_Vmax_Imin-iL_min_Vmax_Imin);
per_ripp_3 = (delta3/((iL_max_Vmax_Imin + iL_min_Vmax_Imin)*0.5))*100;

%min V, max I
iL_max_Vmin_Imax = Vout*((1/Rmin)+((1-D_max)/(2*L_design*f)));
iL_min_Vmin_Imax = Vout*((1/Rmin)-((1-D_max)/(2*L_design*f)));
delta2 = (iL_max_Vmin_Imax-iL_min_Vmin_Imax);
per_ripp_2 = (delta2/((iL_max_Vmin_Imax + iL_min_Vmin_Imax)*0.5))*100;

%min V, min I 
iL_max_Vmin_Imin = Vout*((1/Rmax)+((1-D_max)/(2*L_design*f)));
iL_min_Vmin_Imin = Vout*((1/Rmax)-((1-D_max)/(2*L_design*f)));
delta1 = (iL_max_Vmin_Imin-iL_min_Vmin_Imin);
per_ripp_1 = (delta1/((iL_max_Vmin_Imin + iL_min_Vmin_Imin)*0.5))*100;

%Output Capacitor Sizing
I_cout_rms = max([delta1 delta2 delta3 delta4])/sqrt(12);
delta_V = ripple*Vout;
C_filter = max([delta1 delta2 delta3 delta4])/(8*f*delta_V);
DF = 0.16; %Dissipation factor 
ESR = (1/(2*pi*f*C_filter))*DF;

%Input capacitor sizing
Delta_Vin = ripple*Vout;
C_in = (Dnom*(1-Dnom)*Iout_mean)/(f*Delta_Vin);

%capacitor ESR loss

P_cap = I_cout_rms^2*ESR;
Vrip_esr_1_cap = I_cout_rms*ESR;
Worst_case_rip_1_cap = Vrip_esr_1_cap+Vout*ripple;

%If 2 caps in parallel
N_caps = 3;
%C = GRM21BR61D106ME15L actually a 10uF
C_selected = 5.2e-6; %derated value after accounting dc bias and frequency effects
ESR_Sel = (1/(2*pi*f))*DF;
ESR_Sel = 5e-3;
delta_I_new = max([delta1 delta2 delta3 delta4]);
delta_V_2_parallel = (delta_I_new/(8*f*(N_caps*C_selected)));
Vrip_esr_2_caps = (I_cout_rms/N_caps)*ESR_Sel; %each esr sees half the inital rms current
worst_case_ripple_N_caps = (Vrip_esr_2_caps + delta_V_2_parallel)/Vout;

I_ave_switch = 0.5*D_max*(iL_max_Vmin_Imax + iL_min_Vmin_Imax);
I_peak_switch = iL_max_Vmax_Imax;

I_ave_diode =  0.5*(1-D_min)*(iL_max_Vmax_Imax + iL_min_Vmax_Imax);
I_peak_diode = iL_max_Vmax_Imax;

%Input capacitor sizing
I_C_in_rms = Iout_max*sqrt(D_max*(1-D_max));


%Switch Loss calculations

%RMS current calculations
I_Q_rms = sqrt(D_min)*Iout_max;
I_D_rms = sqrt(1-D_min)*Iout_max;
I_D_ave = (1-D_min)*Iout_max;

%Inductor Losses
P_ind_loss = I_ind_rms^2*R_L;

%Overcurrent protection sensor loss
I_limit = 5;
R_sense_max = 10e-3; %max allowable resistance for loss limiting
P_sense = R_sense_max*I_ind_rms^2;
Target_Voltage_max = I_limit*R_sense_max;
initial_error = 1.5; %percent assumed errors in current limit resistor and current source reference
error_budget = 5;
error_allowable = error_budget-initial_error;
Vos = 125e-6;   %Offset voltage; Vs = 3.3; delay = 50 us
Vos_error = (Vos/Target_Voltage_max)*100;

R_sense_min = 750e-6;%V_sense_min/I_limit;
V_sense_min = R_sense_min*I_limit;%Vos/(error_allowable/100);
P_sense_min = R_sense_min*I_ind_rms^2;
NAF = 500e-6; %Noise Adjustment Factor see page 12 in TI note: http://www.ti.com/lit/ds/symlink/ina300.pdf
%I_limit = 20e-6;
R_limit = (V_sense_min+NAF)/I_limit;

%Switching Losses
%Calculate the worst case blocking voltage of each switch and also the
%maximum current at which the MOSFET turns ON and turns OFF and use the
%expressions to determine switching loss

P_Q_on = 0.5*f*Vin_max*(iL_min_Vmax_Imax)*t_rise;
P_Q_off = 0.5*f*Vin_max*(iL_max_Vmax_Imax)*t_fall;

%for a conventional buck, the reverse recovery charge of the diode plays an
%important role in the switching loss. it can be calculated by,
%0.5*f*Qrr*Voff

P_D_off = 0.5*Vin_max*f*Qrr;

P_Q_sync_on = 0.5*f*0*(iL_max_Vmax_Imax)*t_rise;
P_Q_sync_off = 0.5*f*0*(iL_min_Vmax_Imax)*t_fall;



%Conduction Losses
%Calculate the RMS currents in both the switches and calculate the RMS
%current by rds_on
P_Q_cond = (I_Q_rms^2)*R_DS_on;

N_low = 1; %Number of freewheeling mosfets in parllel

Irms_Per_FET = I_D_rms/N_low;

P_Q_sync_cond_per_fet = (Irms_Per_FET^2)*R_DS_on;

P_Q_sync_cond = P_Q_sync_cond_per_fet*N_low;

P_D_cond = (I_D_rms^2)*RD_ON + I_D_ave*Vf;

%eff_buck = (Vout*Iout_max*100)/((Vout*Iout_max)+(P_Q_on + P_Q_off + P_Q_cond + P_D_cond+P_D_off+P_ind_loss));
%eff_sync_buck = (Vout*Iout_max*100)/((Vout*Iout_max)+(P_Q_on + P_Q_off + P_Q_cond + P_Q_sync_on + P_Q_sync_off + P_Q_sync_cond+P_ind_loss));
eff_buck = (Vout*Iout_max*100)/((Vout*Iout_max)+(P_Q_on + P_Q_off + P_Q_cond + P_D_cond+P_D_off+P_ind_loss+P_cap+P_sense_min));
eff_sync_buck = (Power_rating_max*100)/((Power_rating_max)+(P_Q_on + P_Q_off + P_Q_cond + P_Q_sync_on + P_Q_sync_off + P_Q_sync_cond+P_ind_loss+P_sense_min+P_cap));

fprintf("*****CCM Buck Converter Parameters*******\n");
fprintf("Specifications:\n")
fprintf("Vin = %d V - %d V\n", Vin_min,Vin_max);
fprintf("Vout = %d V\n", Vout);
fprintf("Frequency = %4.1e KHz\n",f);
fprintf("Voltage Ripple (percent) = %0.2f\n",ripple*100);
fprintf("Duty Range = %0.3f - %0.3f\n\n", D_min, D_max);

fprintf("*******Design parameters***********\n");

fprintf("Case 1: Vin = %d, I out = %2.1f\n", Vin_min, Iout_min);
fprintf("I L min = %0.3f\tI L max = %0.3f\n",iL_min_Vmin_Imin,iL_max_Vmin_Imin);
fprintf("Ripple Current = %0.3f\tPercent Ripple = %0.3f\n\n", delta1, per_ripp_1);

fprintf("Case 2: Vin = %d, I out = %2.1f\n", Vin_min, Iout_max);
fprintf("I L min = %0.3f\tI L max = %0.3f\n",iL_min_Vmin_Imax,iL_max_Vmin_Imax);
fprintf("Ripple Current = %0.3f\tPercent Ripple = %0.3f\n\n", delta2, per_ripp_2);

fprintf("Case 3: Vin = %d, I out = %2.1f\n", Vin_max, Iout_min);
fprintf("I L min = %0.3f\tI L max = %0.3f\n",iL_min_Vmax_Imin,iL_max_Vmax_Imin);
fprintf("Ripple Current = %0.3f\tPercent Ripple = %0.3f\n\n", delta3, per_ripp_3);

fprintf("Case 4: Vin = %d, I out = %2.1f\n", Vin_max, Iout_max);
fprintf("I L min = %0.3f\tI L max = %0.3f\n",iL_min_Vmax_Imax,iL_max_Vmax_Imax);
fprintf("Ripple Current = %0.3f\tPercent Ripple = %0.3f\n\n", delta4, per_ripp_4);

fprintf("\n*****Passives********\n")
fprintf("Critical Inductance = %0.3e H\n",L_crit);
fprintf("Design Inductance (%0.2fx oversized) = %0.3e H\n",L_over_size,L_design);
fprintf("Inductor RMS current = %0.3e A\n",I_ind_rms);
fprintf("Peak Inductor Current = %0.3f A\n",iL_max_Vmax_Imax);
fprintf("Input Capacitence = %0.3e F\n",C_in);

fprintf("Filter Capacitance = %0.3e F\n",C_filter);
fprintf("Worst case ripple with single capacitence = %0.3f \n",(Worst_case_rip_1_cap)*100);
fprintf("Worst case ripple with %i capacitors in parallel = %0.3f \n",N_caps,(worst_case_ripple_N_caps)*100);

fprintf("\n*******Switch Ratings****\n")

fprintf("\nMOSFET\n");
fprintf("Peak Switch Current = %0.3f A\n",I_peak_switch);
fprintf("Max Switch RMS current = %0.3f A\n",I_Q_rms);
fprintf("Switch Rating (5x oversize) = %d V, %0.3f A\n",Vin_max,I_peak_switch*5);

fprintf("\nDIODE\n");
fprintf("Peak Diode Current = %0.3f A\n",I_peak_diode);
fprintf("Max Diode Average Current = %0.3f A\n",I_ave_diode);
fprintf("Max Diode RMS current = %0.3f A\n",I_D_rms);
fprintf("Diode Rating(2x oversize) = %d V, %0.3f A\n",Vin_max,I_peak_diode*2);

fprintf("\n****Loss Estimates*****\n\n");
fprintf("Efficiency calculated for V in max I lload max case \n")

fprintf("Inductor Loss = %0.3f W\n",P_ind_loss);
fprintf("Current Sense Resistor (%f ohm) Loss = %0.3f W\n",R_sense_min,P_sense_min);
fprintf("Capacitor ESR Loss = %0.3f W\n",P_cap);
fprintf("Conventional buck\n");
fprintf("MOSFET Switching Losses = %0.3f Watts\n",(P_Q_on + P_Q_off));
fprintf("MOSFET Conduction Losses = %0.3f Watts\n",P_Q_cond);
fprintf("Net MOSFET Losses = %0.3f Watts\n",(P_Q_on + P_Q_off+P_Q_cond));
fprintf("Diode Switching Losses = %0.3f Watts\n",P_D_off);
fprintf("Diode Conduction Losses = %0.3f Watts\n",P_D_cond);
fprintf("Net Diode Losses = %0.3f Watts\n",P_D_cond+P_D_off);
fprintf("Total Losses = %0.3f\n",(P_Q_on + P_Q_off+P_Q_cond+P_D_cond+P_D_off+P_ind_loss));
fprintf("Efficiency (percent) = %0.3f\n",eff_buck);
fprintf("\nSynchronous buck\n");
fprintf("Top MOSFET Switching Losses = %0.3f Watts\n",(P_Q_on + P_Q_off));
fprintf("Top MOSFET Conduction Losses = %0.3f Watts\n",P_Q_cond);
fprintf("Net Top MOSFET losses= %0.3f Watts\n",(P_Q_on + P_Q_off+P_Q_cond));
fprintf("Freewheeling MOSFET Switching Losses = %0.3f Watts\n",(P_Q_sync_on + P_Q_sync_off));
fprintf("Freewheeling MOSFET Conduction Losses = %0.3f Watts\n",P_Q_sync_cond);
fprintf("Net Freewheeling MOSFET Losses = %0.3f Watts\n",(P_Q_sync_on + P_Q_sync_off+P_Q_sync_cond));
fprintf("Total Losses = %0.3f\n",(P_Q_on + P_Q_off + P_Q_cond + P_Q_sync_on + P_Q_sync_off + P_Q_sync_cond+P_ind_loss));
fprintf("Efficiency (percent) = %0.3f\n",eff_sync_buck);

fprintf('\n***Overcurrent Protection Details.***\n');
fprintf('Overcurrent Trigger Voltage = %0.3f V\n',Target_Voltage_max);
fprintf('Overcurrent Limit = %i A\n',I_limit);
fprintf('Maximum shunt resistance = %0.3f ohm\n',R_sense_max);
fprintf('Minimum shunt resistance = %0.5f ohm\n',R_sense_min);
fprintf('Minimum shunt loss = %0.3f W\n',P_sense_min);
fprintf('Limit Resistance for minimim sense resistance: %0.3f ohm\n',R_limit);