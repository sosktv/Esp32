%%  ??? ???? ????
clear 
close all
clc
 
%%  signal
% ???
A=1;                %amplify
f=10000;               %Hz
w=2*pi*f;           %rad/s
p=0;                %rad
%??
T=1;                %s        %????
fs=20*f;            %Hz       %????
d=1/fs;             %s        %????
 
 
t=-T/2:d:T/2;       %????t
s1=A*sin(w*t+p);    %????
s2=A*cos(w*t+p);
s3=s1+s2;

figure(1)
plot(t,s1);
xlabel('??/s');
ylabel('??');

figure(2)
plot(t,s2);
xlabel('??/s');
ylabel('??');

figure(3)
plot(t,s3);
xlabel('??/s');
ylabel('??');

disp(s3);

