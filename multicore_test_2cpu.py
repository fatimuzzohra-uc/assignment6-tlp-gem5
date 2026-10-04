import m5
from m5.objects import *

NUM_CPUS = 2

system = System(
    cpu=[X86MinorCPU() for _ in range(NUM_CPUS)],
    mem_mode="timing",
    mem_ranges=[AddrRange("512MB")]
)

system.clk_domain = SrcClockDomain()
system.clk_domain.clock = "1GHz"
system.clk_domain.voltage_domain = VoltageDomain()

system.membus = SystemXBar()

for cpu in system.cpu:
    cpu.icache_port = system.membus.cpu_side_ports
    cpu.dcache_port = system.membus.cpu_side_ports

    cpu.createInterruptController()
    cpu.interrupts[0].pio = system.membus.mem_side_ports
    cpu.interrupts[0].int_requestor = system.membus.cpu_side_ports
    cpu.interrupts[0].int_responder = system.membus.mem_side_ports

system.system_port = system.membus.cpu_side_ports

system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports

binary = "/home/fatim/gem5/assignment6_tlp/multicore_test"

system.workload = SEWorkload.init_compatible(binary)

for i, cpu in enumerate(system.cpu):
    process = Process(
        pid=100 + i,
        cmd=[binary, str(i)]
    )

    cpu.workload = process
    cpu.createThreads()

root = Root(full_system=False, system=system)

m5.instantiate()

print("Beginning 2-CPU sanity test...")

exit_event = m5.simulate()

print(
    "Exiting @ tick {} because {}".format(
        m5.curTick(),
        exit_event.getCause()
    )
)
