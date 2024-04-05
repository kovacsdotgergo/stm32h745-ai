#!/usr/bin/env python3
import subprocess, os, sys, argparse
from termcolor import colored

ERR_COLOR = 'red'
INFO_COLOR = 'blue'
BASE_FOLDER = os.path.normpath(os.path.dirname(os.path.dirname(__file__)))
GENERATED_SRC_DIR = os.path.join(BASE_FOLDER, 'src', 'h745_generated')
OWN_SRC_DIR = os.path.join(BASE_FOLDER, 'src', 'h745')

parser = argparse.ArgumentParser(
       description=f'''Merge the generated changes from the file in 
              {GENERATED_SRC_DIR} into the file in the same 
              relative location in {OWN_SRC_DIR}''')
parser.add_argument('file',
                    help=f'file path relative to {GENERATED_SRC_DIR}')
parser.add_argument('-t', '--tmp', action='store_true',
                    help='write to temporary output in a tmp file in '
                         f'{BASE_FOLDER} instead of overwriting own file')
args = parser.parse_args()
file = args.file
tmp_out = args.tmp

generated_file = os.path.join(GENERATED_SRC_DIR, file)
own_file = os.path.join(OWN_SRC_DIR, file)
# overwriting own file
if tmp_out:
    out_path = os.path.join(BASE_FOLDER, 'tmp')
else:
    out_path = own_file

if not os.path.isfile(generated_file):
    print(colored("Generated source doesn't exist", ERR_COLOR))
    sys.exit(1)
if not os.path.isfile(own_file):
    print(colored("Own target file doesn't exist", ERR_COLOR))
    sys.exit(1)

# diff format in git merge style for vscode editor support
conflict_format = "<<<<<<< own file\n%<=======\n%>\n>>>>>>> generated file\n"
incoming_format = "<<<<<<< own file\n=======\n%>\n>>>>>>> generated file\n"
cmd = ("diff "
       "--old-group-format='%<' "
       f"--new-group-format='{incoming_format}' "
       f"--changed-group-format='{conflict_format}' "
       f"{own_file} {generated_file} > {out_path}")

compl = subprocess.run(cmd, shell=True)

DIFF_ERR_RET = 2
if compl.returncode == DIFF_ERR_RET:
    print(colored(f'Diff returned error code {compl.returncode}',
                  ERR_COLOR))