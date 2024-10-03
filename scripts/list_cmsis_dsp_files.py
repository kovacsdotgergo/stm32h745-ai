#!/usr/bin/env python3
# Prints a list of the required dependencies from the project if the used files are listed
# The CMSIS-DSP library is written in a way that the few main files include all other source files, so discovering all dependencies recursively wouldn't be neccessary, but this solution could be useful later, when not applying for this problem

import os, subprocess

project_root_path = os.path.normpath(os.path.dirname(os.path.dirname(__file__)))

# TODO: set these to be arguments with these default values and somehow solve that they are global
goal = "print"  # TODO: maybe rename to 'action'
input_files = ["Source/TransformFunctions/TransformFunctions.c", "Source/CommonTables/CommonTables.c"]
CMSIS_DSP_PATH = os.path.join(
    project_root_path, os.pardir, os.pardir, "lib", "CMSIS-DSP"
)
CMSIS_PATH = os.path.join(project_root_path, "src", "h745", "Drivers", "CMSIS")
include_dirs = [
    f"{CMSIS_DSP_PATH}/Include",
    f"{CMSIS_DSP_PATH}/PrivateInclude",
    f"{CMSIS_PATH}/Include",
]
source_dirs = [f"{CMSIS_DSP_PATH}/Source"]


def find_dependencies(input_file) -> list[str]:
    """Returns a list of the paths of the dependencies"""
    # call gcc to list dependencies
    dep_cmd = [
        "arm-none-eabi-gcc",
        "-mcpu=cortex-m7",
        "-mthumb",
        "-mfpu=fpv5-d16",
        "-mfloat-abi=hard",  # for predefined macros
        *["-I" + include_dir for include_dir in include_dirs],
        f"{input_file}",  # TODO: isn't it always string?
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


def get_related_sources(input_file: str):
    """Finds related source files for the input header, e.g. source files with the same name as headers"""
    # For the current context I used these commands to check the feasibility of this function (e.g. if the assertion of using the same named headers and source files holds up):
    # h_files=$(find Source Include -type f -name "*.h" | xargs -n 1 basename | sed 's/\.h$//' | sort)
    # c_files=$(find Source Include -type f -name "*.c" | xargs -n 1 basename | sed 's/\.c$//' | sort)
    # to get the files that are either only sources or headers
    # comm -12 <(echo "$c_files") <(echo "$h_files")
    # to get the files that have headers and sources with the same name
    # comm -3 <(echo "$c_files") <(echo "$h_files")
    basename = os.path.basename(input_file)
    filename, ext = os.path.splitext(basename)
    if ext != ".h":
        return []

    def snake_to_camel(snake_str):
        words = snake_str.split('_')
        return "".join([x.capitalize() for x in words])

    source_candidates = [filename + ".c", snake_to_camel(filename + ".c")]
    ret = []
    project_source_files = {}
    for source_dir in source_dirs:
        for dirpath, _, files in os.walk(source_dir):
            project_source_files.update(
                [
                    (file, os.path.join(dirpath, file))
                    for file in files
                    if file.endswith(".c")
                ]
            )
    for source_candidate in source_candidates:
        if source_candidate in project_source_files:
            ret.append(project_source_files[source_candidate])

    return ret


def get_all_dependencies(input_file: str, dependencies: set[str]):
    if input_file not in dependencies:
        dependencies.add(input_file)
        _, ext = os.path.splitext(input_file)
        if ext == ".h":
            # discovering the deps of headers is redundant, but for related sources the discovery should continue
            deps = get_related_sources(input_file)
            for dep in deps:
                get_all_dependencies(dep, dependencies)
        else:
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


if __name__ == "__main__":
    main()
