#!/usr/bin/env python3

import os, subprocess

project_root_path = os.path.normpath(os.path.dirname(os.path.dirname(__file__)))

# TODO: set these to be arguments with these default values and somehow solve that they are global
goal = "print" # TODO: maybe rename to 'action'
input_files = ["Source/TransformFunctions/TransformFunctions.c"]
CMSIS_DSP_PATH = os.path.join(project_root_path, os.pardir, os.pardir, "lib", "CMSIS-DSP")
CMSIS_PATH = os.path.join(project_root_path, "src", "h745", "Drivers", "CMSIS")


def find_dependencies(input_file) -> list[str]:
    """Returns a list of the paths of the dependencies"""
    # call gcc to list dependencies
    dep_cmd = [
        "arm-none-eabi-gcc",
        "-mcpu=cortex-m7",
        "-mthumb",
        "-mfpu=fpv5-d16",
        "-mfloat-abi=hard",  # for predefined macros
        f"-I{CMSIS_DSP_PATH}/Include",
        f"-I{CMSIS_DSP_PATH}/PrivateInclude",
        f"-I{CMSIS_PATH}/Include",
        f"{input_file}",
        "-MM",
    ]  # could call cpp, but MM sets -E which is the same, only preprocessing
    compl = subprocess.run(dep_cmd, check=True, capture_output=True)
    out = compl.stdout  # byte array
    # strip the object file target rule
    target_rule_idx = out.find(b":")
    out = out[target_rule_idx + 1 :]
    out = out.replace(b"\\\n ", b"")  # filter out newlines
    out = out.strip()
    out = out.decode()
    out = out.split()  # split on whitespaces
    return out


def get_all_dependencies(input_file: str, dependencies: set[str]):
    if input_file not in dependencies:
        dependencies.add(input_file)
        # discovering the deps of headers is redundant
        _, ext = os.path.splitext(input_file)
        if ext != ".h":
            deps = find_dependencies(input_file)
            for dep in deps:
                get_all_dependencies(dep, dependencies)


def main():
    used_files = [os.path.join(CMSIS_DSP_PATH, file) for file in input_files]
    required_files = set()
    for used_file in used_files:
        get_all_dependencies(used_file, required_files)

    required_files = [os.path.relpath(file, CMSIS_DSP_PATH) for file in required_files]
    required_files = [file for file in required_files if file[:2] != ".."]

    match goal:
        case "print":
            print(" ".join(required_files))
        case "copy":
            dest = os.path.join(
                project_root_path, "src", "h745", "Drivers", "CMSIS-DSP"
            )
            # the --parents option preserves the subfolder structure, also this is why cwd is required
            copy_cmd = ["cp", "--parents", *required_files, dest]
            subprocess.run(copy_cmd, check=True, cwd=CMSIS_DSP_PATH)
        case _:
            raise ValueError("Unknown value of goal")


main()
