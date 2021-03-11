// Proof of concept of a method to get current CPU frequency and converting CPU time to CPU cycles.
// SPDX-License-Identifier: Apache-2.0

#include <cstdint>
#include <fstream>
#include <iostream>

#include <sched.h>

[[gnu::noinline]] long inner_work(long s, long i)
{
    return 17 * s + 3 * i - 1;
}

[[gnu::noinline]] long work(long n)
{
    long s = 13;
    for (long i = 0; i < n; ++i)
        s = inner_work(s, i);
    return s;
}

inline double ProcessCPUUsage()
{
    timespec ts;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    return ts.tv_sec + (static_cast<double>(ts.tv_nsec) * 1e-9);
}

double get_cpu_freq()
{
    // Get the number of the CPU the current thread is running on. This value can change during
    // benchmark run.
    const auto cpu = sched_getcpu();

    // Use "scaling_cur_freq" in kHz - the frequency scheduler thinks the CPU runs at.
    // Maybe "cpuinfo_cur_freq" is better, but was not available for my CPU.
    // https://www.kernel.org/doc/Documentation/cpu-freq/user-guide.txt
    const auto path =
        "/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/cpufreq/scaling_cur_freq";

    auto f = std::ifstream{path};
    int64_t freq = -1;
    f >> freq;

    return freq * 1e-6;
}

int main(int argc, const char* argv[])
{
    double cpu_time;
    const auto start_time = ProcessCPUUsage();

    const auto min_time = (argc >= 2) ? std::atof(argv[1]) : 1.0;
    std::cout << "min time:  " << min_time << " s\n";

    volatile long sink;
    do
    {
        sink = work(1000000000);
        cpu_time = ProcessCPUUsage() - start_time;
    } while (cpu_time < min_time);

    const auto freq = get_cpu_freq();
    const auto cycles = static_cast<int64_t>(cpu_time * freq * 1e9);

    std::cout << "CPU time:  " << cpu_time << " s\n";
    std::cout << "CPU freq:  " << freq << " GHz\n";
    std::cout << "cycles:    " << cycles << "\n";

    return 0;
}
