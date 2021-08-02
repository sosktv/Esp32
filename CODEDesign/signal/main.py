import numpy as np
import matplotlib.pyplot as plt

sample_rate = 16000
fc = 10000

T = 1.0 / sample_rate  # 周期
x = np.arange(0, 1.0, T)
y = 0.9 * np.sin(2 * np.pi * fc * x)
file = wave.open(r'hello.wav', 'wb')
file.setnchannels(1)  # 设置通道数
file.setsampwidth(2)  # 设置采样宽
file.setframerate(16000)  # 设置采样
file.setcomptype('NONE', 'not compressed')  # 设置采样格式  无压缩
plt.plot(y[:16])  # 画出一个周期的波形图
plt.show()
y = y * 32768
y_data = y.astype(np.int16).tobytes()  # 将类型转为字节
file.writeframes(y_data)
file.close()
ys = y[:fft_size]
yf = np.abs(np.fft.fft(ys) / fft_size)
plt.subplot(211)
plt.plot(x[:fft_size], ys)
plt.subplot(212)
plt.plot(x[:fft_size], yf)
plt.show()
