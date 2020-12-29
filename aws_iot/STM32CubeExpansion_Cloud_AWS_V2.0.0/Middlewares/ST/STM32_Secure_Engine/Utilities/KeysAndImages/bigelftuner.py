# Copyright(c) 2020 STMicroelectronics International N.V.
# All rights reserved.
#
# This software component is licensed by ST under Ultimate Liberty license
# SLA0044, the "License"; You may not use this file except in compliance with
# the License. You may obtain a copy of the License at:
#                             www.st.com/SLA0044
#
import argparse
import lief

def do_processing(args):
  # Retrieve the SBSFU entry point.
  print("processing: " + args.sbsfu)
  sbsfu_elf = lief.parse(args.sbsfu)
  sbsfu_header = sbsfu_elf.header
  sbsfu_entry = sbsfu_header.entrypoint
  print ("sbsfu entry point is: " + hex(sbsfu_entry))

  # Retrieve the application (big elf) entry point.
  print("processing: " + args.appli)
  appli_elf = lief.parse(args.appli)
  appli_header = appli_elf.header
  appli_entry = appli_header.entrypoint
  print ("initial appli entry point is: " + hex(appli_entry))

  # Set the application's entry point to sbsfu's entry point.
  # Thus the CubeIDE debugger starts SBSFU
  # before setting the breakpoint in the application's main() function.
  appli_header.entrypoint = sbsfu_entry
  print ("changing appli entry point to: " + hex(sbsfu_entry))
  appli_elf.write(args.appli)


def args():
  parser = argparse.ArgumentParser()
  # the first parameter is the sbsfu elf file
  parser.add_argument("sbsfu", help="sbsfu elf file")
  # the second parameter is the elf file concatenating the application elf file,
  # the header binary and the SBSFU elf file
  parser.add_argument("appli", help="application big elf file")

  args = parser.parse_args()
  do_processing(args)


if __name__ == '__main__':
    args()