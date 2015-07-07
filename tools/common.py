##Author: Xuepeng Fan, xuepeng_fan@163.com, 2015.07.07

import os

cur_dir = os.path.dirname(os.path.realpath(__file__))
base_dir = os.path.dirname(cur_dir)
output_dir = os.path.join(base_dir, 'tools_output')

if not os.path.exists(output_dir):
    os.mkdir(output_dir)
