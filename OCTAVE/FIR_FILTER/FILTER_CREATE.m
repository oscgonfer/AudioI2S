clc
clear all
close all

pkg load signal

fs = 44100;
fftSize = 512;
factorX = 2;
BIT_LENGTH = 24;
FULL_SCALE_DBSPL = 120; %% FULL SCALE dBSPL (AOP = 116dB SPL)
FULL_SCALE_DBFS = 20*log10((2^(BIT_LENGTH)));
RMS_HANN = 0.61177;
CONST_FACTOR = 64;

%% SIGNAL
x = [0.5:1/factorX:fftSize/factorX];
%y = sin(x)+sin(3*x)+cos(15*x)+sin(2*x)+sin(10*x);
%y = [173739,570132,68184,-549128,-331984,534496,240747,-605196,-1063808,-31868,406796,-343584,-216136,-675108,174295,-429304,-213172,-592904,656552,317164,-771536,161543,451843,1111804,120423,810575,-639336,-302380,49327,142252,-199304,-944840,-438692,193411,-105072,45656,-60644,111147,1424511,144248,-762136,-5716,744387,97159,-1400176,-313216,-385700,-456812,84143,-734688,250044,333804,-440668,-173736,820323,468956,-748908,-669864,162664,-414268,38879,431939,-145952,-62608,295452,350628,571063,404808,-696612,279504,-31680,-459676,518852,-553732,208675,-103144,-111076,766344,-247988,-417004,-54192,543308,-560964,-850944,788160,12248,-97872,393759,-136388,486608,59736,-314340,246592,424996,-386316,365556,-165676,-300972,-163004,329719,692520,191528,527320,-769524,-346940,1072579,399520,-386320,237343,156847,-502072,103771,-344896,144908,18492,99351,-774988,182340,110360,842536,-261976,-252364,928275,-33704,644720,-706672,37460,-172440,-319136,530799,-302496,-183320,133619,3236,149551,-147724,-213900,648623,-192032,-278736,-121156,204644,194496,-474896,308464,873323,-506508,-51112,206775,-257984,806727,-264848,18092,108216,-185216,38280,558396,334203,96812,28972,300059,867636,536007,-718068,-50492,496939,46583,-84592,-315968,-128864,-413760,-506332,-450700,2080,490395,-424652,-472420,578979,-18180,13795,623232,-521020,84256,-104036,144319,-31420,-378404,-226496,447219,-139948,416128,630908,288219,-471108,419111,744799,-333320,87844,13636,511215,-1141716,171687,761543,414148,-846424,-484876,816899,-74208,422275,-413528,-1170420,202455,-3704,-786920,-181584,-322076,-148440,87896,109999,409824,-556044,-187720,218415,303544,-64972,-278208,-643712,-397520,-82924,133867,351360,352555,-302740,275683,816408,349143,685391,-311936,17472,395767,55711,-469484,-294032,260540,-352916,-501996,337459,123144,-599912,-633060,217392,26060,535028,-511200,-822252,-239804,879383,-225048,54131,-115976,753347,-400404,-421132,608132,392328,600219,-513540,-207728,121379,647651,30795,-788564,-11244,-127204,-21976,-38816,-907328,-256988,368680,305008,-352800,12620,220816,286183,-204112,312048,231584,-35804,-445432,-435540,887391,415483,-893908,-383324,223620,-176292,498756,-118996,-221064,17656,251736,571199,45456,-699976,796579,93531,43803,-305572,586815,405535,217712,609587,-732784,-58504,575672,-10040,-406356,-956968,21356,176591,152204,-253440,92184,253675,237063,-323680,665968,360884,166559,-1128204,679784,751156,-473528,-602040,-519564,1592696,-593408,-464604,-250532,84156,-200744,-101888,403035,-578508,-171376,-247284,213971,-121316,-806904,744768,999456,-293048,151884,-77948,-27748,131055,-494584,689660,-266912,134576,106144,-582712,-233680,310859,613259,-71352,-697744,-38980,-34724,58808,-831576,359100,200228,-38216,-592128,120115,726123,-500340,143280,-510992,-3416,527195,402443,-493104,-253956,140359,187595,-333180,-281280,-273024,276448,17355,143863,358752,144572,847484,-728024,501327,474007,40695,-681684,-23740,-65616,-506696,-554088,988164,515119,-237052,257779,388235,420203,-320652,122679,599692,-157192,-997988,-93380,-169892,391551,-59284,158504,-88148,-280016,605760,215320,-528292,333371,490735,-15348,-86656,-51616,280435,-32496,295991,-282748,-12068,-406372,-549344,613787,-317240,-120192,102351,488560,-66968,-244692,-74476,115840,-5140,328428,-211964,241155,-867916,638216,-156700,-232024,151300,339872,8900,-418088,-412000,175803,655712,-190228,-165128,444887,713251,-209244,-677008,-575772,547328,-702552,-141936,-9884,-753620,16635,990739,205008,-498732,991679,530108,456223,-216444,-166420,-454268,368427,418092,-551672,378132,546044,454343,-377452,-13248,499091,-818256,-16540,-214140,-586760,-189836,248523,-527220,-718064,-213320,289739,786147,-956336,-727468,489079,961568,-177000,-628656,662263,836356];
y=[-6660,-8668,-13296,-15332,-16712,-19852,-21616,-23388,-21348,-23048,-23380,-21808,-21448,-19112,-17372,-15400,-12388,-9636,-7764,-3072,1043,3379,6660,9288,12531,15612,17636,19127,20080,21783,23055,22803,22384,20187,21351,18968,17123,14871,12824,10427,7180,3375,-256,-3236,-5924,-8440,-13864,-15988,-16416,-19644,-22004,-21592,-22756,-23716,-24056,-21864,-20016,-20252,-18824,-15068,-12452,-9864,-7216,-3412,575,3279,7232,9671,13475,15116,17687,19656,20923,22963,23031,23188,23795,24500,21291,18400,17668,15991,13412,10031,7691,3304,1004,-1860,-5768,-9720,-12136,-14876,-17640,-17744,-22112,-22288,-23132,-23856,-21940,-21380,-21324,-19508,-17884,-15396,-12940,-10564,-6180,-3252,-900,2276,6056,9020,12847,15580,17251,19619,19699,22172,22988,23776,23207,21916,22000,19919,17324,15668,13751,10172,6864,3843,920,-1568,-5176,-10300,-12392,-14296,-17000,-20160,-19296,-22624,-23032,-23756,-22572,-22380,-22080,-19796,-17864,-16828,-14244,-10524,-7220,-5136,-908,1588,5432,9100,11503,14807,16399,18432,19348,21687,22435,22467,22223,20712,20859,19939,17712,14599,13147,9180,6612,2844,3,-2600,-6528,-9480,-12480,-14688,-17644,-19020,-21536,-22752,-22696,-24216,-23732,-22624,-22092,-20120,-19240,-16732,-13800,-12036,-8520,-4820,-1764,1543,5708,8687,9603,14795,18068,19624,20028,22144,23543,23123,22827,22912,21947,20423,17331,16355,12739,11671,8667,5280,1027,-1976,-3956,-7484,-11080,-13996,-17776,-18764,-19356,-21660,-21780,-22592,-22984,-22220,-21580,-20284,-17416,-14832,-14044,-10968,-7272,-4520,-1112,1983,5531,9311,11640,14548,16851,19395,21628,22423,23663,23811,23448,23395,21551,19924,18739,18180,13352,11023,9048,4155,1087,-1196,-4848,-8716,-10220,-14608,-17148,-18568,-20696,-21912,-22000,-22992,-23944,-23912,-22536,-19896,-18216,-16988,-13704,-10324,-9580,-5584,-664,2447,4163,7084,10904,14724,16027,18720,19839,22436,22892,22888,22455,23259,21468,19731,18467,15400,14095,11916,8264,5804,1943,-2948,-4380,-9100,-12660,-15224,-16800,-18164,-19700,-22940,-23824,-22432,-22408,-23164,-20988,-20760,-18308,-16084,-14196,-12588,-8856,-5456,-2732,1132,3411,8063,10799,13151,15695,17720,19544,21871,20640,22455,22856,22340,20827,19564,19132,16387,13892,12435,8327,5815,347,-1308,-4116,-7228,-10764,-14744,-15696,-18912,-19672,-22184,-23512,-23628,-23236,-22108,-22616,-21824,-20132,-16696,-15168,-13348,-8572,-4712,-4692,-176,4775,6808,9375,12943,15028,18215,20488,21731,22367,22907,21984,23091,20688,19863,17892,17507,14931,11503,8868,6247,3051,-1472,-3412,-7672,-11652,-14092,-17680,-17800,-19800,-23152,-23868,-23580,-23500,-23124,-23136,-21632,-19316,-17036,-15556,-12948,-9460,-6084,-2832,-936,3735,7412,7940,11296,16760,17588,18971,20940,22372,21851,23380,23276,19863,19940,18960,15499,13632,11196,8184,4316,1579,-1644,-5344,-7116,-10416,-14036,-16684,-19456,-20576,-22228,-22604,-24004,-24940,-23992,-23180,-21576,-19936,-17964,-15660,-13408,-10676,-7276,-4272,-144,2683,5620,8564,13159,14224,17163,19371,20236,21379,22228,21312,21903,21208,18447,18220,16572,14027,10291,7823,5371,3167,-1972,-4068,-7336,-11136,-13492,-16196,-18984,-21384,-20876,-22620,-24368,-24304,-24180,-22660,-21812,-21716,-18440,-15236,-13320,-10072,-6296,-4488,-1036,3012,6592,8160,11852,13131,16319];

%% FILTER (fft domain)
timefiltert = [1:1:length(x)];
[freq_ampl_Atable,ampl_Atable]=aweightingfn(fs,fftSize-1);
ampl_Atable_24bit = 10.^(ampl_Atable/20);
Equalisation = csvread("Filter_Equalization.csv");

freq_equalization_filter_aux = real(Equalisation)';
freq_equalization_filter = freq_equalization_filter_aux(2:end);
ampl_equalization_filter_aux = imag(Equalisation)';
ampl_equalization_filter = ampl_equalization_filter_aux(2:end);

vector_fir = [0:1:length(ampl_Atable_24bit)-1]./(length(ampl_Atable_24bit)-1);
vectorfft = vector_fir*fs/2;

ampl_equalization_filter_resampled= interp1(freq_equalization_filter,ampl_equalization_filter,vectorfft,'EXTRAP');
filter_desired = ampl_Atable_24bit./ampl_equalization_filter_resampled;

FIR_COEFS = fir2(200,vector_fir,filter_desired);
[h, w] = freqz (FIR_COEFS);
freq_FIR_COEF = w/pi;
ampl_FIR_COEF = abs (h);

FIR_COEFS_FLIPPED = fliplr(FIR_COEFS);

filename = "../../lib/FilterCoefficients/fir_coeffs.cpp";
fid = fopen (filename, "w");
fputs (fid, disp("//This file is automatically generated by the .m file in the OCTAVE folder"));
fputs (fid, disp("#pragma once"));
fputs (fid, disp(""));
fputs (fid, disp("#define ARM_MATH_CM0PLUS"));
fputs (fid, disp(""));
fputs (fid, disp("#include <arm_math.h>"));
fputs (fid, disp("#include <fir_coeffs.h>"));
fputs (fid, disp(""));
fputs (fid, disp("float32_t firCoeffs[FIR_COEFFS_LEN] = {"));
for i = 1:1:length(FIR_COEFS_FLIPPED)
  fputs (fid, num2str(FIR_COEFS_FLIPPED(i)));
  fputs( fid, disp(","));
end 
fputs(fid, disp("};"));
fclose (fid);

filename = "../../lib/FilterCoefficients/fir_coeffs.h";
fid = fopen (filename, "w");
fputs (fid, disp("//This file is automatically generated by the .m file in the OCTAVE folder"));
fputs (fid, disp("#ifndef __FIR_COEFFS__"));
fputs (fid, disp("#define __FIR_COEFFS__"));
fputs (fid, disp(""));
fputs (fid, ("#define FIR_COEFFS_LEN  "));
fputs (fid, disp(num2str(length(FIR_COEFS_FLIPPED))));
fputs (fid, disp(""));
fputs (fid, disp("const uint16_t FILTERSIZE = FIR_COEFFS_LEN;"));
fputs (fid, disp("const uint32_t FILTERBLOCKSIZE = 15;"));
fputs (fid, disp("extern float32_t firCoeffs[FIR_COEFFS_LEN];"));
fputs (fid, disp(""));
fputs (fid, disp("#endif"));
fclose (fid);

%% FILTER (time domain)
filter_desired_tot = zeros (1,length(filter_desired)*2);
filter_desired_tot(1:end/2) = filter_desired(1:end);
filter_desired_tot(2:end/2) = 1/2*filter_desired_tot(2:end/2);
for i = 0:1:(length(filter_desired_tot)/2)
    filter_desired_tot(length(filter_desired_tot)/2+i) = filter_desired_tot(length(filter_desired_tot)/2-i+1);
end

filter_y = ifft(filter_desired_tot);
filter_y_mir = zeros(1,length(filter_y));
filter_y_mir(1:end/2) = filter_y(end/2+1:end);
filter_y_mir(end/2+1:end) = filter_y(1:end/2);
filter_y_mir(2:end-1) = 2*filter_y_mir(2:end-1);

window = blackman(length(filter_y_mir));
windowed_filter_y =(filter_y_mir) .*window';
windowed_filter_y_real = real(windowed_filter_y);
windowed_filter_y_imag = imag(windowed_filter_y);
window_filter_tab = zeros(length(windowed_filter_y),2);
window_filter_tab(:,1) = windowed_filter_y_real(:)';
window_filter_tab(:,2) = windowed_filter_y_imag(:)';

%% FFT of raw signal
ffty = fft(y);
p1 = abs(ffty)./length(ffty);
p2 = p1(1:length(ffty)/2);
p2 (2:end) = 2 * p2 (2:end);

%% APPLY FILTER
% On frequency domain
p2_filtered = filter_desired.* p2;

% On time domain (complex)
y_filter_time = conv(y,windowed_filter_y,'same');

% On time domain (real + imag)
y_filter_time_R = conv(y,windowed_filter_y_real,'same');
y_filter_time_I = conv(y,windowed_filter_y_imag,'same');
y_filter_time_RI = sign(y_filter_time_R(:)).*sqrt(y_filter_time_R(:).^2+y_filter_time_I(:).^2);

% On time domain (FIR)
y_filter_desiredIR = conv(y,FIR_COEFS,'same');

%% FFT of filtered signal
% On time domain (complex)
fftyfilter_cor = fft(y_filter_time);
p1yfilter_cor = abs(fftyfilter_cor)./length(fftyfilter_cor);
p2yfilter_cor = p1yfilter_cor(1:length(fftyfilter_cor)/2);
p2yfilter_cor (2:end) = 2 * p2yfilter_cor (2:end);

% On time domain (real + imag)
fftyfilter_cor_RI = fft(y_filter_time_RI);
p1yfilter_cor_RI = abs(fftyfilter_cor_RI)./length(fftyfilter_cor_RI);
p2yfilter_cor_RI = p1yfilter_cor_RI(1:length(fftyfilter_cor_RI)/2);
p2yfilter_cor_RI (2:end) = 2 * p2yfilter_cor_RI (2:end);

% On time domain (FIR)
fftyfilter_cor_FIR = fft(y_filter_desiredIR);
p1yfilter_cor_FIR = abs(fftyfilter_cor_FIR)./length(fftyfilter_cor_FIR);
p2yfilter_cor_FIR = p1yfilter_cor_FIR(1:length(fftyfilter_cor_FIR)/2);
p2yfilter_cor_FIR (2:end) = 2 * p2yfilter_cor_FIR (2:end);

%% RMS check
%Not filtered
rms_y = rms_test_in_house(y);
rms_y_DB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*rms_y)); 

rms_yf = rms_test_in_house(ffty)/sqrt(length(ffty));
rms_yf_ADB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*rms_yf)); 

rms_p2 = rms_test_in_house(p2)*sqrt(length(p2))/sqrt(2);
rms_p2_DB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*rms_p2)); 

%Filtered
rms_y_conv = rms_test_in_house(y_filter_time);
rms_y_conv_DB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*rms_y_conv)); 

rms_y_conv_RI = rms_test_in_house(y_filter_time_RI);
rms_y_conv_RI_ADB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*rms_y_conv_RI)); 

rms_y_FIR = rms_test_in_house(y_filter_desiredIR);
rms_y_FIR_ADB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*rms_y_FIR)); 

rms_p2_fft = rms_test_in_house(p2_filtered) *sqrt(length(p2_filtered))/sqrt(2);
rms_p2_fft_ADB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*rms_p2_fft)); 

rms_p2_conv = rms_test_in_house(p2yfilter_cor) *sqrt(length(p2yfilter_cor))/sqrt(2);
rms_p2_conv_ADB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*rms_p2_conv)); 

rms_p2_conv_RI = rms_test_in_house(p2yfilter_cor_RI)*sqrt(length(p2yfilter_cor_RI))/sqrt(2);
rms_p2_conv_RI_ADB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*rms_p2_conv_RI)); 

rms_p2_conv_FIR = rms_test_in_house(p2yfilter_cor_FIR)*sqrt(length(p2yfilter_cor_FIR))/sqrt(2);
rms_p2_conv_FIR_ADB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*rms_p2_conv_FIR)); 

fdisp(stdout,"**********---------**********");
fdisp(stdout,"");
fdisp(stdout,"NON FILTERED SIGNAL RMS");
fdisp(stdout,"");
fdisp(stdout,"RMS from Time domain");
fdisp(stdout,rms_y);
fdisp(stdout,rms_y_DB);
fdisp(stdout,"RMS from P2 domain");
fdisp(stdout,rms_p2);
fdisp(stdout,rms_p2_DB);
fdisp(stdout,"");
fdisp(stdout,"**********---------**********");
fdisp(stdout,"");
fdisp(stdout,"FILTERED SIGNAL RMS");
fdisp(stdout,"RMS from Time domain Convoluted with Ideal Filter by RI method");
fdisp(stdout,rms_y_conv_RI);
fdisp(stdout,rms_y_conv_RI_ADB);
fdisp(stdout,"RMS from Time domain Convoluted with FIR Filter");
fdisp(stdout,rms_y_FIR);
fdisp(stdout,rms_y_FIR_ADB);
fdisp(stdout,"");
fdisp(stdout,"**********---------**********");
fdisp(stdout,"");
fdisp(stdout,"FFT SIGNAL RMS");
fdisp(stdout,"RMS from FFT domain by Filter Frequency response multiplication");
fdisp(stdout,rms_p2_fft);
fdisp(stdout,rms_p2_fft_ADB);
fdisp(stdout,"RMS from FFT domain - Signal Convoluted with Ideal Filter");
fdisp(stdout,rms_p2_conv); 
fdisp(stdout,rms_p2_conv_ADB); 
fdisp(stdout,"RMS from FFT domain - Signal Convoluted with Ideal Filter by RI method");
fdisp(stdout,rms_p2_conv_RI);
fdisp(stdout,rms_p2_conv_RI_ADB);
fdisp(stdout,"RMS from FFT domain - Signal Convoluted with FIR Filter");
fdisp(stdout,rms_p2_conv_FIR);
fdisp(stdout,rms_p2_conv_FIR_ADB);

%% PLOTs
figure
plot (vector_fir, filter_desired, ";target response;", w/pi, abs (h), ";filter response;");
xlabel('Frequency (-)')
ylabel('Filter (-)')
title('Frequency domain filter')
grid

figure
axisFIR_COEFS=[-length(FIR_COEFS)/2+1.5:1:length(FIR_COEFS)/2+0.5];
timefiltert_offset = timefiltert-(timefiltert(end))/2;
plot(axisFIR_COEFS,FIR_COEFS,'g',timefiltert_offset,filter_y_mir,'k',timefiltert_offset,windowed_filter_y,'b')
xlabel('Time (s)')
ylabel('Filter (-)')
title('Ideal Time domain filter')
legend('FIR Reduced','Raw','Windowed')
grid

figure
axisFIR_COEFS_fs = w/pi * fs/2;
semilogx(axisFIR_COEFS_fs,abs(h),'b',freq_ampl_Atable,filter_desired,'r',freq_ampl_Atable,ampl_Atable_24bit,'--k',freq_ampl_Atable,ampl_equalization_filter_resampled,'g');
xlabel('Frequency (-)')
ylabel('Filter (-)')
title('Frequency domain filter')
legend('Obtained','Desired','A-weighting','Equalization')
grid

figure
axisFIR_COEFS_fs = w/pi * fs/2;
semilogx(axisFIR_COEFS_fs,abs(h),'b',freq_ampl_Atable,filter_desired,'r');
xlabel('Frequency (-)')
ylabel('Filter (-)')
title('Frequency domain filter')
legend('Obtained','Desired')
grid

figure
subplot(2,1,1)
plot(timefiltert,filter_y_mir,'k',timefiltert,windowed_filter_y,'b')
xlabel('Time (s)')
ylabel('Filter (-)')
title('Ideal Time domain filter')
legend('Raw','Windowed')
grid

subplot(2,1,2)
plot(x,y,'b',x,y_filter_time,'r',x,y_filter_time_RI,'--g')
xlabel('Time (s)')
ylabel('Signal (-)')
title('Sample Signal')
legend('Signal', 'Filtered Complex','Filtered RI')
grid

figure
subplot(2,1,1)
plot(freq_ampl_Atable,filter_desired)
xlabel('Frequency (-)')
ylabel('Filter (-)')
title('Frequency domain filter')
grid

subplot(2,1,2)
plot(freq_ampl_Atable,p2,'b',freq_ampl_Atable,p2_filtered,'r')
xlabel('Frequency (-)')
ylabel('Signal frequency response (-)')
legend('Signal', 'Filtered')
grid

figure
plot(freq_ampl_Atable,p2,'b',freq_ampl_Atable,p2_filtered,'r',freq_ampl_Atable,p2yfilter_cor,'k',freq_ampl_Atable,p2yfilter_cor_RI,'--g',freq_ampl_Atable,p2yfilter_cor_FIR,'--m')
xlabel('Frequency (-)')
ylabel('Signal frequency response (-)')
legend('Signal','FFT Filtered', 'Time Filtered Complex','Time Filtered RI','Time Filtered FIR')
grid
