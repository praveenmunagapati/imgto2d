import re

content = open('main.cpp', 'r', encoding='utf-8').read()

helper = '''
inline float get_pixel_float(const cv::Mat& image, int y, int x) {
    if (image.type() == CV_32F) return image.at<float>(y, x);
    if (image.type() == CV_8U) return (float)image.at<uchar>(y, x);
    return 0.0f;
}
'''

# Add helper at the top of the file, after includes
content = re.sub(r'(#include <vector>\n)', r'\1' + helper, content)

# Replace image.at<float>(y, x) with get_pixel_float(image, y, x)
content = re.sub(r'image\.at<float>\(([^,]+),\s*([^)]+)\)', r'get_pixel_float(image, \1, \2)', content)

with open('main.cpp', 'w', encoding='utf-8') as f:
    f.write(content)

print("Replaced image.at<float> with helper.")
