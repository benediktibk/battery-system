#!/usr/bin/env python3

import getopt
import sys
import setproctitle
import yaml
from yaml.loader import SafeLoader
import subprocess


def printError(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


def printUsage():
    print("checks zephyr version")
    print("")
    print("available and optional parameter of this tool:")
    print("    --help                            prints out this help")
    print("    --manifest=                       manifest file")
    print("    --zephyr=                         zephyr directory")
    print("")
    print("example usages:")
    print("     zephyrVersionCheck --manifest=manifest/manifest.yml --zephyr=zephyr")


if __name__ == "__main__":
    setproctitle.setproctitle("zephyrVersionCheck")

    try:
        options, arguments = getopt.getopt(sys.argv[1:], "",
                                           [
            "help",
            "manifest=",
            "zephyr=",
        ])
    except getopt.GetoptError:
        printUsage()
        sys.exit(1)

    manifest = ""
    manifestSelected = False
    zephyr = ""
    zephyrSelected = False

    for option, argument in options:
        if option == "--help":
            printUsage()
            sys.exit()
        elif option == "--manifest":
            manifest = argument
            manifestSelected = True
        elif option == "--zephyr":
            zephyr = argument
            zephyrSelected = True
        else:
            printError("invalid option " + option)
            printUsage()
            sys.exit(1)

    if not manifestSelected:
        printError("the manifest file must be selected")
        sys.exit(1)

    if not zephyrSelected:
        printError("the zephyr directory must be selected")
        sys.exit(1)

    with open(manifest) as manifestFile:
        manifestContent = yaml.load(manifestFile, Loader=SafeLoader)
        projects = manifestContent["manifest"]["projects"]
        targetZephyrRevision = None

        for project in projects:
            if project["name"] == "zephyr":
                targetZephyrRevision = project["revision"]

        if targetZephyrRevision is None:
            printError("zephyr project not in manifest file")
            sys.exit(1)

        command = "git rev-parse HEAD"
        print(f"executing '{command}'")
        gitRevParseOutput = subprocess.check_output(command, cwd=zephyr, shell=True)
        currentZephyrRevision = gitRevParseOutput.decode("utf-8")[:-1]
        print(f"current zephyr revision is {currentZephyrRevision}")
        print(f"target zephyr revision is {targetZephyrRevision}")

        if currentZephyrRevision == targetZephyrRevision:
            print("zephyr revision is correct")
            sys.exit(0)
        else:
            printError("zephyr revision is incorrect")
            sys.exit(1)
