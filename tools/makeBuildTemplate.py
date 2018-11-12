#!/usr/bin/python3
# -*- coding: utf-8 -*-

from argparse import ArgumentParser
import datetime
import os.path

"""
BUILD-Nummer hochzählen, Builddatum erzeugen
"""
__author__ = 'Dirk Marciniak'
__copyright__ = 'Copyright 2018'
__license__ = 'GPL'
__version__ = '0.1'

BUILDCONFIGNAME = "CurrBuildDef.hpp"
BUILDCOUNTERFILE = "currBuildNum"


def main():
    """
    Die Funktion
    """
    source_file = None
    # parse argumente
    parser = ArgumentParser(fromfile_prefix_chars='@',
                            description='increment build number and set build date',
                            epilog="from Dirk Marciniak\n");
    parser.add_argument("--srcdir", type=str, help="source directory")
    parser.add_argument("--build", type=str, help="type of build")
    args = parser.parse_args()
    if args.srcdir:
        source_dir = args.srcdir
        if os.path.isfile( source_dir + "/" + BUILDCONFIGNAME):
            print("configfile " + BUILDCONFIGNAME + " found...")
            source_file = source_dir + "/" + BUILDCONFIGNAME
        else:
            print("error: config file not found in given path!")
            exit(-1)
    else:
        print("error: not an argument for source directory given...")
        exit(-1)
    if args.build:
        build_type = args.build
    else:
        build_type = ""
    #
    # hier erst mal das aktuelle Datum festlegen
    #
    today_str = datetime.datetime.now().strftime("  constexpr char SPX_BUILDTIME[]{\"%Y-%m-%d %H:%M:%S\"};");
    print("build type: {}".format(build_type))
    print("today date: " + datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
    #
    # jetzt buildnummer einlesen
    #
    build_num_file = open(BUILDCOUNTERFILE, "r")
    build_num = int(build_num_file.readline().rstrip())
    build_num_file.close()
    #
    # Nummer ausgelesen, formatieren
    #
    print("buildcount: {:08} -> {:08}".format(build_num, build_num+1))
    build_str = "  constexpr char SPX_BUILDCOUNT[]{}\"{:08}\"{};".format("{", build_num+1, "}");
    #
    # buildtyp formatieren
    #
    build_type_str = "  constexpr char SPX_BUILDTYPE[]{}\"{}\"{};".format("{", build_type, "}");
    #
    # buildnummer schreiben
    #
    build_num_file = open(BUILDCOUNTERFILE, "w")
    build_num_file.write("{:08}\n".format(build_num+1))
    build_num_file.close()
    #
    # wenn bis hier alles gelaufen ist
    #
    config_file = open(source_file, "w")
    config_file.write("#ifndef CURRBUILDDEF_HPP\n")
    config_file.write("#define CURRBUILDDEF_HPP\n")
    config_file.write("\n")
    # namespace einbringen
    config_file.write("namespace spx\n")
    config_file.write("{\n")
    config_file.write(today_str + "\n")
    config_file.write(build_str + "\n")
    config_file.write(build_type_str + "\n")
    # ende namespace
    config_file.write("}\n")
    config_file.write("\n")
    config_file.write("#endif\n")
    config_file.close()
    #
    # ende gut
    #
    exit(0)

if __name__ == '__main__':
    main()
