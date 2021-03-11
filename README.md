# CPU frequency and cycles

Proof of concept of a method to get current CPU frequency and converting CPU time to CPU cycles.

## Usage

Compare the CPU cycles computed by the `cycles` executable with the one reported by `perf`.

```bash
perf stat -e cycles,task-clock --no-big-num ./cycles 10
```
