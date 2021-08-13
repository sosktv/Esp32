# -*- coding: utf-8 -*-
import wave
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt

if __name__ == '__main__':
    sample_rate = 16000
    time = 10

    # 产生10秒16kHz的100Hz - 1kHz的频率扫描波
    t = np.arange(0, time, 1.0 / sample_rate)
    wave_data = signal.chirp(t, f0=10000, f1=10000, t1=10, method='linear') * 10000
    wave_data = wave_data.astype(np.short)
    plt.plot(t, wave_data)
    plt.title("Linear Chirp, f(0)=10000, f(10)=10000")
    plt.xlabel('t (sec)')
    plt.show()
    # 打开WAV文档
    f = wave.open(r"C:\Users\soskt\Desktop\sourceSound\SourceSound\Intro.wav", "wb")

    # 配置声道数、量化位数和取样频率
    f.setnchannels(1)
    f.setsampwidth(2)
    f.setframerate(sample_rate)
    # 将wav_data转换为二进制数据写入文件
    f.writeframes(wave_data.tobytes())
    f.close()

    t = np.linspace(0, 10, 1500)
    w = signal.chirp(t, f0=6, f1=1, t1=10, method='linear')
    plt.plot(t, w)
    plt.title("Linear Chirp, f(0)=6, f(10)=1")
    plt.xlabel('t (sec)')
    plt.show()
