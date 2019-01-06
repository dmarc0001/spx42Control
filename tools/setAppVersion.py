#!/usr/bin/python3
# -*- coding: utf-8 -*-

from argparse import ArgumentParser
import datetime
import os.path
import re
from xml.dom import minidom

"""
Versionsnummer hochzählen für UPDATE
"""
__author__ = 'Dirk Marciniak'
__copyright__ = 'Copyright 2019'
__license__ = 'GPL'
__version__ = '0.1'

INSTALLDIR = "installer"
PACKAGESDIR = "packages"
CONFIGDIR = "config"
PACKAGELIST = ['spx42Control', 'spx42ControlMac']


def get_version_list(version: str):
    """aus String Liste mit Versionsnummern"""
    if re.match('^\\d+\\.\d+\\.\\d+$', version) is not None:
        # das funktioniert dann warscheinlich
        temp_list = str.split(version, '.')
        temp_list[0] = int(temp_list[0])
        temp_list[1] = int(temp_list[1])
        temp_list[2] = int(temp_list[2])
        return temp_list
    return None


def get_node_text(nodes):
    """gib den Text der TEXT Node zurück"""
    rc = []
    for node in nodes:
        if node.nodeType == node.TEXT_NODE:
            rc.append(node.data)
    return ''.join(rc)


def set_node_text(nodes, text):
    """setzte den NodeText"""
    for node in nodes:
        if node.nodeType == node.TEXT_NODE:
            node.data = text
            return


def main():
    """
    Die Funktion
    """
    #
    # Voreinstellungen
    #
    force_set_version = False
    version_list = [0, 0, 0]
    #
    # parse argumente
    #
    parser = ArgumentParser(fromfile_prefix_chars='@',
                            description='set new version number for package(s)',
                            epilog="from Dirk Marciniak\n");
    parser.add_argument("--prjdir", type=str, help="project directory")
    parser.add_argument("--setversion", type=str, help="set new version number")
    args = parser.parse_args()
    if args.prjdir:
        project_dir = args.prjdir
        if os.path.isdir(project_dir):
            print("set project dir to {}".format(project_dir))
        else:
            print("error: not given project path!")
            exit(-1)
    if args.setversion:
        version_list = get_version_list(args.setversion)
        if get_version_list(args.setversion) is not None:
            # das funktioniert dann warscheinlich
            # version erzwingen
            force_set_version = True
            print("force version {}.{}.{} ".format(version_list[0], version_list[1], version_list[2]))
    if not force_set_version:
        print("increment version numbers...")
    #
    # erst mal die config datei
    #
    config_file = os.path.join(project_dir, INSTALLDIR, CONFIGDIR, 'config.xml')
    if not os.path.isfile(config_file):
        print("error: config file not found: {}".format(config_file))
        exit(-1)
    #
    # CONFIG Datei bearbeiten
    #
    print("compute config file: {}".format(config_file))
    # parse xml datei
    config_dom = minidom.parse(config_file)
    # den oder die element(e) holen
    version_node = config_dom.getElementsByTagName("Version")[0]
    #
    version_string = get_node_text(version_node.childNodes)
    print("config version: {}".format(version_string))
    if not force_set_version:
        version_list = get_version_list(version_string)
        version_list[2] = version_list[2] + 1
    print("new version: {}.{}.{}".format(version_list[0], version_list[1], version_list[2]))
    set_node_text(version_node.childNodes, "{}.{}.{}".format(version_list[0], version_list[1], version_list[2]))
    xml_file_handle = open(config_file, "wt")
    config_dom.writexml(xml_file_handle)
    xml_file_handle.close()
    print("compute config file: {} OK".format(config_file))
    #
    # Packages bearbeiten
    #
    for package_file in PACKAGELIST:
        current_file = os.path.join(project_dir, INSTALLDIR, PACKAGESDIR, package_file, 'meta', 'package.xml')
        if not os.path.isfile(current_file):
            print("error: package file not found: {}".format(current_file))
            exit(-1)
        print("compute package {} file {}".format(package_file, current_file))
        #
        # parse xml datei
        #
        config_dom = minidom.parse(current_file)
        #
        # VERSION bearbeiten
        #
        version_node = config_dom.getElementsByTagName("Version")[0]
        version_string = get_node_text(version_node.childNodes)
        print("version: {}".format(version_string))
        if not force_set_version:
            version_list = get_version_list(version_string)
            version_list[2] = version_list[2] + 1
        print("new version: {}.{}.{}".format(version_list[0], version_list[1], version_list[2]))
        set_node_text(version_node.childNodes, "{}.{}.{}".format(version_list[0], version_list[1], version_list[2]))
        #
        # Release Datum setzten
        #
        release_node = config_dom.getElementsByTagName("ReleaseDate")[0]
        release_string = get_node_text(release_node.childNodes)
        print("old release date: {}".format(release_string))
        release_string = '{0:%Y-%m-%d}'.format(datetime.datetime.now())
        print("new release date: {}".format(release_string))
        set_node_text(release_node.childNodes, release_string)
        #
        # in Datei zurück schreiben
        #
        xml_file_handle = open(current_file, "wt")  # oder "wt"
        config_dom.writexml( xml_file_handle )
        xml_file_handle.close()
        print("compute package {} file {} - OK".format(package_file, current_file))


if __name__ == '__main__':
    main()
