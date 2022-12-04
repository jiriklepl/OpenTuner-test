#!/usr/bin/env python
#
# Optimize blocksize of apps/mmm_block.cpp
#
# This is an extremely simplified version meant only for tutorials
#
import math

import opentuner
from opentuner import ConfigurationManipulator
from opentuner import IntegerParameter
from opentuner import EnumParameter
from opentuner import MeasurementInterface
from opentuner import Result


class BlockSizeTuner(MeasurementInterface):

  def manipulator(self):
    """
    Define the search space by creating a
    ConfigurationManipulator
    """
    manipulator = ConfigurationManipulator()
    manipulator.add_parameter(IntegerParameter('blockSize', 1, 1260))
    manipulator.add_parameter(EnumParameter('compiler', ['gcc', 'clang']))
    manipulator.add_parameter(IntegerParameter('optLevel', 0, 3))

    return manipulator

  def run(self, desired_result, input, limit):
    """
    Compile and run a given configuration then
    return performance
    """
    cfg = desired_result.configuration.data

    gcc_cmd = cfg['compiler'] + ' kernels/c_kernel.c'
    gcc_cmd += ' -DBLOCK_SIZE=' + str(cfg['blockSize'])
    gcc_cmd += ' -O' + str(cfg['optLevel'])
    gcc_cmd += ' -o ./tmp.bin'

    compile_result = self.call_program(gcc_cmd)
    if not compile_result['returncode'] == 0:
        return Result(state='ERROR', time=math.inf)

    run_cmd = './tmp.bin'

    run_result = self.call_program(run_cmd)
    if not run_result['returncode'] == 0:
        return Result(state='ERROR', time=math.inf)

    return Result(time=run_result['time'])

  def save_final_config(self, configuration):
    """called at the end of tuning"""
    print("Optimal block size written to mmm_final_config.json:" + str(configuration.data))
    self.manipulator().save_to_file(configuration.data, 'mmm_final_config.json')

if __name__ == '__main__':
  argparser = opentuner.default_argparser()
  BlockSizeTuner.main(argparser.parse_args())
