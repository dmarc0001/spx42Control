#!/usr/bin/python3
# -*- coding: utf-8 -*-

from argparse import ArgumentParser
from os import path, chdir, remove
import subprocess

"""
StyleSheets machen
Zielnamensmuster:

spx42Control_[Light|Dark]_[Win|Mac|Lin].css

Quellen

styleWinSizes.less => Größen für Windows
styleMacSizes.less => Größen für Mac

spxControlWinLight.less => Helles Thema
spxControlMacDark.less => dunkles Thema

"""
__author__ = 'Dirk Marciniak'
__copyright__ = 'Copyright 2019'
__license__ = 'GPL'
__version__ = '0.1'

RES_DIR = 'src/res'
TOOL_DIR = 'tools'
TEMPLATE_DIR = 'template'
SIZE_TEMPLATEFILE = "style{}Sizes.less"
TEME_TEMPLATEFILE = "spx42Control{}.less"
DEST_STYLE_TEMPLATEFILE = "spx42Control{}{}.css"
LESSCOMPILER = 'lessc'
PLATFORMLIST = ['Mac', 'Win']
THEMELIST = ['Dark', 'Light']



def main():
    """
    Die Funktion
    """
    #
    # parse argumente
    #
    parser = ArgumentParser(fromfile_prefix_chars='@',
                            description='create styles for app from templates',
                            epilog="from Dirk Marciniak\n");
    parser.add_argument("--prjdir", type=str, help="project directory")
    args = parser.parse_args()
    if args.prjdir:
        project_dir = args.prjdir
        if path.isdir(project_dir):
            print("set project dir to {}".format(project_dir))
        else:
            print("error: not given project path!")
            exit(-1)
    chdir(project_dir)
    #
    # plattformen iterieren, danach themen
    #
    for platform in PLATFORMLIST:
        for theme in THEMELIST:
            print("platform: {}, theme: {}".format(platform, theme))
            sizefile = path.join(TEMPLATE_DIR, SIZE_TEMPLATEFILE.format(platform))
            source_template_file = path.join(TEMPLATE_DIR, TEME_TEMPLATEFILE.format(theme))
            final_template_file = path.join(TEMPLATE_DIR, "styletemplate.less")
            stylesheet = path.join(RES_DIR, DEST_STYLE_TEMPLATEFILE.format(platform, theme))
            # print("quelle aus {} und {}".format(sizefile, source_template_file))
            # print("zieldatei {}".format(stylesheet))
            #
            # erzeuge temporäre source
            #
            print("oeffne groessendatei {}...".format(sizefile))
            size_file = open(sizefile, 'r')
            print("oeffne styledatei {}...".format(source_template_file))
            style_file = open(source_template_file, 'r')
            print("oeffne/erzeuge finale temporaere less datei...")
            s_file = open(final_template_file, 'w')
            print("schreibe datei...")
            for line in size_file:
                s_file.write(line)
            size_file.close()
            for line in style_file:
                s_file.write(line)
            style_file.close()
            s_file.close()
            print("schreibe datei...OK")
            print("compiliere less datei  - {}".format(final_template_file))
            print("compiliere less datei zu stylesheet - {}".format(stylesheet))
            command = []
            command.append(LESSCOMPILER)
            command.append(final_template_file)
            command.append(stylesheet)
            print("compiliere less datei zu stylesheet...")
            subprocess.run(command, shell=True, check=True)
            print("compiliere less datei zu stylesheet...OK")
            remove(final_template_file)
            print("-")


if __name__ == '__main__':
    main()
