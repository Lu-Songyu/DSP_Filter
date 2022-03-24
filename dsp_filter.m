%read in audio
Fs = 48000;
[x, ~] = audioread("speech.wav")
xa = abs(x);
%normalize audio
xmax = max(xa);
x_norm = x/xmax;
max(abs(x));

%add noise signal v
v = generate_sigs(960000, 2400, Fs);
w = v' + x_norm;
figure(1);
spectrogram(w, 1024,24, 'yaxis')
figure(2)
spectrogram(x_norm, 1024,24, 'yaxis')
% Look from y-axis, we can see 0.1 is a delta function as 2400 represents
% 0.1 of 24000

%Remove noise v with a 14th order notch filter with notch at 2.4 kHz(low-pass)
n = [-7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7]
wc = (2400/48000)*2*pi;
h = zeros(1,15);
%h = sin(wc*n)/(pi*n)
for i = 1:15
        h(i) = sin(wc*n(i))/(pi*n(i));
end
h = h*2;
h(8) = 0.1-1;
h
freqz(h)

y_fir = fir_filter(w, h)
spectrogram(y_fir, 1024,24, 'yaxis')
% line is lighter

%Remove noise v with a 2nd order IIR notch filter with a notch at 2.4 kHz
a = [1, -2*0.99*cos(0.1*pi), 0.99*0.99]
b = [1, -2*cos(0.1*pi), 1]
y_iir = iir_filter(w,b,a)
freqz(b,a)
spectrogram(y_iir', 1024,24, 'yaxis')


function g = generate_sigs(L, f, Fs)
    g = zeros(1,L);
    for i = 1:L
        g(i) = sin(2*pi*i*f/Fs);
    end
end


function r = fir_filter(x,a)
    x = [zeros(1,15) x'];
    r = zeros(1, length(x));
    for j = 15:length(r)
        r(j) = a(1)*x(j)+a(2)*x(j-1)+a(3)*x(j-2)+a(4)*x(j-3)+a(5)*x(j-4)+a(6)*x(j-5)+a(7)*x(j-6)+a(8)*x(j-7)+a(9)*x(j-8)+a(10)*x(j-9)+a(11)*x(j-10)+a(12)*x(j-11)+a(13)*x(j-12)+a(14)*x(j-13)+a(15)*x(j-14);
        
    end
end 


function r = iir_filter(x,a,b)
    x = [zeros(1,3) x'];
    r = zeros(1, length(x));
    for j = 5:length(r)
        N = a(1)*x(j)+a(2)*x(j-1)+a(3)*x(j-2);
        D = -b(2)*r(j-1)-b(3)*r(j-2);
        r(j) = (N+D)/b(1);
    end
end 