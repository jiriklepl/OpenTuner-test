# OpenTuner test repository

## How to recreate the transcript

```sh
# ./run.sh [MAX_TESTS [MAX_SECONDS]]
./run.sh 300 60 2>&1 | tee transcript.log
```

```sh
# ./run.sh [MAX_TESTS [MAX_SECONDS]]
# uncomment autotuner.py:45
# comment autotuner.py:46
./run.sh 300 60 2>&1 | tee transcript2.log
```
