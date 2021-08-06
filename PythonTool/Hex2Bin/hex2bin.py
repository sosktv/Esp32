# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
# import re
import numpy as np


def num_data(file_path):
    fr = open(file_path, 'r+')
    lines = fr.readlines()
    num1 = []
    for line in lines:
        if line.__len__() == 24:
            num1.append(line.strip())
    return num1


def segmentation(nums):
    af_nums = []
    for gr_num in nums:
        af_nums.append(gr_num.split(' ', 8))
    return af_nums


def string_to_hex(string_in):
    gaps = []
    labels = string_in
    for label in labels:
        for label_men in label:
            gap = bytes(label_men, 'UTF-8')
            gap = int(gap, 16)
            gaps.append(gap)
    return gaps


def file_write(contact):
    np.savetxt("test3.txt", contact, fmt='%s', delimiter='\t')


if __name__ == '__main__':
    np.set_printoptions(threshold=np.inf)
    num = num_data('HEX2BIN.txt')
    final_Gaps = []
    after_labels = segmentation(num)
    final_Gaps = string_to_hex(after_labels)
    arr = np.array(final_Gaps)
    # arr = arr.reshape(432, 8)
    arr = arr.reshape(int(arr.size/8), 8)
    np.set_printoptions(formatter={'int': bin})
    print(np.vectorize(np.binary_repr)(arr, width=8))
    file_write(np.vectorize(np.binary_repr)(arr, width=8))


