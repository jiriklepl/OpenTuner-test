#!/usr/bin/env python
#
# Optimize blocksize of apps/mmm_block.cpp
#
# This is an extremely simplified version meant only for tutorials
#
import math
import sys

import opentuner
from opentuner.search.manipulator import ConfigurationManipulator
from opentuner.search.manipulator import IntegerParameter
from opentuner.search.manipulator import BooleanParameter
from opentuner import MeasurementInterface
from opentuner import Result


class BlockSizeTuner(MeasurementInterface):
  def manipulator(self):
    """
    Define the search space by creating a
    ConfigurationManipulator
    """
    manipulator = ConfigurationManipulator()
    
    # search space: 64*64*64 * 2 * 2 = 2^20 = 1_048_576
    # -> we don't want to search all of it, we need something more clever

    manipulator.add_parameter(IntegerParameter('I_BLOCK_SIZE', 1, 64))
    manipulator.add_parameter(IntegerParameter('J_BLOCK_SIZE', 1, 64))
    manipulator.add_parameter(IntegerParameter('K_BLOCK_SIZE', 1, 64))

    manipulator.add_parameter(BooleanParameter('TRANSPOSE_A'))
    manipulator.add_parameter(BooleanParameter('TRANSPOSE_B'))

    return manipulator

  def run(self, desired_result, input, limit):
    """
    Compile and run a given configuration then
    return performance
    """
    cfg = desired_result.configuration.data

    gcc_cmd = 'cc -O3 -march=native -D NDEBUG' + ' kernels/c_kernel.c'

    gcc_cmd += ' -D I_BLOCK_SIZE=' + str(cfg['I_BLOCK_SIZE'])
    gcc_cmd += ' -D J_BLOCK_SIZE=' + str(cfg['J_BLOCK_SIZE'])
    gcc_cmd += ' -D K_BLOCK_SIZE=' + str(cfg['K_BLOCK_SIZE'])

    if cfg['TRANSPOSE_A']: gcc_cmd += ' -D TRANSPOSE_A'
    if cfg['TRANSPOSE_B']: gcc_cmd += ' -D TRANSPOSE_B'

    gcc_cmd += ' -o ./tmp.bin'

    compile_result = self.call_program(gcc_cmd)
    if not compile_result['returncode'] == 0:
        return Result(state='ERROR', time=math.inf)

    run_cmd = './tmp.bin'

    run_result = self.call_program(run_cmd)
    if not run_result['returncode'] == 0:
        print(run_result['stdout'].decode('utf-8')[:-1] + " on " + str(cfg), file=sys.stderr)
        return Result(state='ERROR', time=math.inf)

    time = float(run_result['stdout'].split()[0])
    print("Success: time=" + str(time) + " on  " + str(cfg), file=sys.stderr)
    return Result(time=time)

  def save_final_config(self, configuration):
    """called at the end of tuning"""
    print("Optimal configuration written to mmm_final_config.json:" + str(configuration.data))
    self.manipulator().save_to_file(configuration.data, 'mmm_final_config.json')

if __name__ == '__main__':
  argparser = opentuner.default_argparser()
  BlockSizeTuner.main(argparser.parse_args())
