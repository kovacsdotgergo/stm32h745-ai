#!/usr/bin/env python3
# todo when the binaries are correct, try again
import subprocess

st_cube_programmer = "/opt/st/stm32cubeclt_1.15.0/STM32CubeProgrammer/bin/STM32_Programmer_CLI"
m4_elf = "src/h745/Makefile/CM4/build/stm32h745-ai_CM4.elf"
m7_elf = "src/h745/Makefile/CM7/build/stm32h745-ai_CM7.elf"

commands = [f"{st_cube_programmer} -c port=SWD mode=UR reset=HWrst -halt",
            f"{st_cube_programmer} -c port=SWD mode=UR reset=HWrst -write {m4_elf}",
            f"{st_cube_programmer} -c port=SWD mode=UR reset=HWrst-write {m7_elf} -hardRst"]

for cmd in commands:
    subprocess.run(cmd, shell=True, check=True)