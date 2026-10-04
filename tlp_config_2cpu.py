import m5
from m5.objects import *

NUM_CPUS = 2

system = System()

# System clock and voltage
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = "1GHz"
system.clk_domain.voltage_domain = VoltageDomain()

# Timing memory system
system.mem_mode = "timing"
system.mem_ranges = [AddrRange("512MB")]

# Create multiple MinorCPU cores
system.cpu = [
    X86MinorCPU(cpu_id=i)
    for i in range(NUM_CPUS)
]

# Shared memory bus
system.membus = SystemXBar()

# Connect every CPU to the shared memory bus
for cpu in system.cpu:
    cpu.icache_port = system.membus.cpu_side_ports
    cpu.dcache_port = system.membus.cpu_side_ports

    # X86 interrupt connections
    cpu.createInterruptController()
    cpu.interrupts[0].pio = system.membus.mem_side_ports
    cpu.interrupts[0].int_requestor = system.membus.cpu_side_ports
    cpu.interrupts[0].int_responder = system.membus.mem_side_ports

# System port
system.system_port = system.membus.cpu_side_ports

# Memory controller
system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports

# DAXPY workload
binary = "/home/fatim/gem5/assignment6_tlp/daxpy_tlp"

system.workload = SEWorkload.init_compatible(binary)

# Assign one partition of DAXPY to each CPU
for i, cpu in enumerate(system.cpu):
    process = Process(pid=100 + i)
    process.cmd = [
        binary,
        str(i),
        str(NUM_CPUS)
    ]

    cpu.workload = process
    cpu.createThreads()

# Root
root = Root(full_system=False, system=system)

m5.instantiate()

print("Beginning 2-CPU DAXPY TLP simulation...")

exit_event = m5.simulate()

print(
    "Exiting @ tick {} because {}".format(
        m5.curTick(),
        exit_event.getCause()
    )
)
