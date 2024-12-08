import numpy as np
import cv2


def create_3d_histogram(image, x_start, x_end, y_start, y_end, bin_number):
    histogram = np.zeros((bin_number, bin_number, bin_number), dtype=float)
    step = 256 // bin_number
    for i in range(x_start, x_end):
        for j in range(y_start, y_end):
            ind1 = int(image[i][j][0] // step)  # R value
            ind2 = int(image[i][j][1] // step)  # G value
            ind3 = int(image[i][j][2] // step)  # B value
            histogram[ind1][ind2][ind3] += 1
    return l1_normalization(histogram, True)


def create_per_channel_histogram(image, x_start, x_end, y_start, y_end, bin_number):
    list_of_histograms = []
    histogram_red = np.zeros(bin_number, dtype=float)
    histogram_green = np.zeros(bin_number, dtype=float)
    histogram_blue = np.zeros(bin_number, dtype=float)
    step = 256 // bin_number
    for i in range(x_start, x_end):
        for j in range(y_start, y_end):
            histogram_red[int(image[i][j][0] // step)] += 1
            histogram_green[int(image[i][j][1] // step)] += 1
            histogram_blue[int(image[i][j][2] // step)] += 1
    list_of_histograms.append(histogram_red)
    list_of_histograms.append(histogram_green)
    list_of_histograms.append(histogram_blue)
    list_of_histograms = l1_normalization(list_of_histograms, False)
    return list_of_histograms


def l1_normalization(histogram, is_three_d):
    if is_three_d:
        tot = 0
        for i in range(len(histogram)):
            for j in range(len(histogram[0])):
                for k in range(len(histogram[0][0])):
                    tot += histogram[i][j][k]
        if tot == 0:
            return histogram
        for i in range(len(histogram)):
            for j in range(len(histogram[0])):
                for k in range(len(histogram[0][0])):
                    histogram[i][j][k] /= tot
        return histogram
    normals = []
    normals.append(np.linalg.norm(histogram[0], 1))
    normals.append(np.linalg.norm(histogram[1], 1))
    normals.append(np.linalg.norm(histogram[2], 1))
    for i in range(len(histogram)):
        for j in range(len(histogram[0])):
            histogram[i][j] /= normals[i]
    return histogram


def rgb_to_hsv(image):
    result_image = np.zeros((len(image), len(image[0]), 3), dtype=float)
    for i in range(len(image)):
        for j in range(len(image[0])):
            R, G, B = image[i][j][0]/255, image[i][j][1]/255, image[i][j][2]/255
            c_max = max(R, G, B)
            c_min = min(R, G, B)
            delta_c = c_max - c_min
            # Hue calculation
            if delta_c == 0:
                result_image[i][j][0] = 0
            elif c_max == R:
                result_image[i][j][0] = ((((G - B) / delta_c) % 6) / 6) * 255
            elif c_max == G:
                result_image[i][j][0] = ((((B - R) / delta_c) + 2) / 6) * 255
            else:
                result_image[i][j][0] = ((((R - G) / delta_c) + 4) / 6) * 255
            # Saturation calculation
            if c_max == 0:
                result_image[i][j][1] = 0
            else:
                result_image[i][j][1] = 255 * delta_c / c_max
            # Value calculation
            result_image[i][j][2] = c_max * 255
    return result_image


def create_grid_histograms_three_d(image, bin_number, grid):
    list_of_histograms, x_list, count = [], [], 0
    while count <= len(image):
        x_list.append(count)
        count += len(image) // int(np.sqrt(grid))
    for i in range(len(x_list) - 1):
        for j in range(len(x_list) - 1):
            list_of_histograms.append(create_3d_histogram(image, x_list[i],
                                                          x_list[i + 1], x_list[j],
                                                          x_list[j + 1], bin_number))
    return list_of_histograms


def create_grid_histograms_per_channel(image, bin_number, grid):
    list_of_histograms, x_list, count = [], [], 0
    while count <= len(image):
        x_list.append(count)
        count += len(image) // int(np.sqrt(grid))
    for i in range(len(x_list) - 1):
        for j in range(len(x_list) - 1):
            list_of_histograms.append(create_per_channel_histogram(image, x_list[i],
                                                                   x_list[i + 1], x_list[j],
                                                                   x_list[j + 1], bin_number))
    return list_of_histograms


def per_channel_accuracy(histogram_1_list, histogram_2_list):
    histogram_1_array = np.array(histogram_1_list)
    histogram_2_array = np.array(histogram_2_list)
    min_values = np.minimum(histogram_1_array, histogram_2_array)
    return np.sum(np.sum(min_values, axis=0) / 3)


def three_d_accuracy(histogram_1, histogram_2):
    histogram_1_array = np.array(histogram_1)
    histogram_2_array = np.array(histogram_2)
    return np.sum(np.minimum(histogram_1_array, histogram_2_array))


def calculate(type_of_histogram, bin_number, grid, query, with_hsv):
    image_paths = open("dataset/InstanceNames.txt").read().splitlines()
    original_histograms, query_histograms, length = [], [], len(image_paths)
    if type_of_histogram == 0 and with_hsv == 0:
        for i in range(length):
            query_image = cv2.imread("dataset/query_{}/{}".format(query, image_paths[i]))
            query_histogram = create_grid_histograms_three_d(query_image, bin_number, grid)
            query_histograms.append(query_histogram)
            original_image = cv2.imread("dataset/support_96/{}".format(image_paths[i]))
            original_histogram = create_grid_histograms_three_d(original_image, bin_number, grid)
            original_histograms.append(original_histogram)
    elif type_of_histogram == 1 and with_hsv == 0:
        for i in range(length):
            query_image = cv2.imread("dataset/query_{}/{}".format(query, image_paths[i]))
            query_histogram = create_grid_histograms_per_channel(query_image, bin_number, grid)
            query_histograms.append(query_histogram)
            original_image = cv2.imread("dataset/support_96/{}".format(image_paths[i]))
            original_histogram = create_grid_histograms_per_channel(original_image, bin_number, grid)
            original_histograms.append(original_histogram)
    elif type_of_histogram == 0 and with_hsv == 1:
        for i in range(length):
            query_image = cv2.imread("dataset/query_{}/{}".format(query, image_paths[i]))
            query_image = rgb_to_hsv(query_image)
            query_histogram = create_grid_histograms_three_d(query_image, bin_number, grid)
            query_histograms.append(query_histogram)
            original_image = cv2.imread("dataset/support_96/{}".format(image_paths[i]))
            original_image = rgb_to_hsv(original_image)
            original_histogram = create_grid_histograms_three_d(original_image, bin_number, grid)
            original_histograms.append(original_histogram)
    elif type_of_histogram == 1 and with_hsv == 1:
        for i in range(length):
            query_image = cv2.imread("dataset/query_{}/{}".format(query, image_paths[i]))
            query_image = rgb_to_hsv(query_image)
            query_histogram = create_grid_histograms_per_channel(query_image, bin_number, grid)
            query_histograms.append(query_histogram)
            original_image = cv2.imread("dataset/support_96/{}".format(image_paths[i]))
            original_image = rgb_to_hsv(original_image)
            original_histogram = create_grid_histograms_per_channel(original_image, bin_number, grid)
            original_histograms.append(original_histogram)
    index, count = 0, 0
    if type_of_histogram == 0:
        for i in range(length):
            accuracy = 0
            for j in range(length):
                temp_accuracy = 0
                for k in range(len(original_histograms[j])):
                    temp_accuracy += three_d_accuracy(original_histograms[j][k], query_histograms[i][k]) / grid
                if temp_accuracy > accuracy:
                    accuracy = temp_accuracy
                    index = j
            if image_paths[i] == image_paths[index]:
                count += 1
        print("Accuracy is {}%".format(count / 2))
    elif type_of_histogram == 1:
        for i in range(length):
            accuracy = 0
            for j in range(length):
                temp_accuracy = 0
                for k in range(len(original_histograms[j])):
                    temp_accuracy += per_channel_accuracy(original_histograms[j][k], query_histograms[i][k]) / grid
                if temp_accuracy > accuracy:
                    accuracy = temp_accuracy
                    index = j
            if image_paths[i] == image_paths[index]:
                count += 1
        print("Accuracy is {}%".format(count / 2))


if __name__ == '__main__':
    type = int(input("Choose the type of histogram. 0 for 3D, 1 for per channel: "))
    bin = 256 // int(input("Choose the quantization interval: "))
    grid = int(input("Choose the grid to be used. 2 means 2x2, 4 means 4x4 etc. : ")) **2
    q = int(input("Choose the query number. Either 1, 2 or 3: "))
    hsv = int(input("Choose the color space. 0 for RGB and 1 for HSV: "))
    calculate(type,bin,grid,q,hsv)

