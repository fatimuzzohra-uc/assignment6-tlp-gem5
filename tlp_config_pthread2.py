import m5
from m5.objects import *

system = System()

# System clock and voltage
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = "1GHz"
system.clk_domain.voltage_domain = VoltageDomain()

# Timing memory system
system.mem_mode = "timing"
system.mem_ranges = [AddrRange("512MB")]

# One X86 Minor CPU for the baseline
system.cpu = X86MinorCPU()

# Memory bus
system.membus = SystemXBar()

# Connect CPU to memory bus
system.cpu.icache_port = system.membus.cpu_side_ports
system.cpu.dcache_port = system.membus.cpu_side_ports

# X86 interrupt connections
system.cpu.createInterruptController()
system.cpu.interrupts[0].pio = system.membus.mem_side_ports
system.cpu.interrupts[0].int_requestor = system.membus.cpu_side_ports
system.cpu.interrupts[0].int_responder = system.membus.mem_side_ports

# System port
system.system_port = system.membus.cpu_side_ports

# Memory controller
system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports

# DAXPY workload
binary = "/home/fatim/gem5/assignment6_tlp/daxpy_pthread"

system.workload = SEWorkload.init_compatible(binary)

process = Process()
process.cmd = [binary, "2"]

system.cpu.workload = process
system.cpu.createThreads()

# Root
root = Root(full_system=False, system=system)

m5.instantiate()

print("Beginning DAXPY baseline simulation...")

exit_event = m5.simulate()

print(
    "Exiting @ tick {} because {}".format(
        m5.curTick(), exit_event.getCause()
    )
)
