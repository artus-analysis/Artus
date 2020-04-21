#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os
import re
import copy
import Artus.HarryPlotter.utility.roottools as roottools
from Artus.Utility.tfilecontextmanager import TFileContextManager


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Print content of ROOT file", parents=[logger.loggingParser])

    parser.add_argument("root_file", help="Input ROOT file")
    parser.add_argument("-e", "--elements", nargs="+", help="Regexes for elements for which the code should be executed.", default=[])
    parser.add_argument("-c", "--codes", nargs="+", help="Codes to be executed for matching elements. \"element\" is replaced by the matching element.", default=[])
    parser.add_argument("-t", "--tree", "--trees", nargs="*", help="trees", default=[])
    parser.add_argument("--verbosity", type=int, help="trees", default=0)
    parser.add_argument('-m', '--minimal', action='store_true', default=False, help='minimal output')

    args = parser.parse_args()
    logger.initLogger(args)

    if len(args.tree) > 0:
        lookup_trees = [t.split('*') for t in args.tree]

    with TFileContextManager(args.root_file, "READ") as root_file:
        elements = roottools.RootTools.walk_root_directory(root_file)
        for index, (key, path) in enumerate(elements):

            printout = True if args.verbosity > 0 else False

            class_name = key.GetClassName()
            if not args.minimal:
                log.info("%s (%s)" % (path, class_name))

            if not printout and len(args.tree) != 0:
                for t in lookup_trees:
                    if len(t) == 1:
                        if t[0] in path:
                            printout = True
                            break
                    else:
                        wc = True
                        path_copy = copy.deepcopy(path)
                        for t2 in t:
                            if t2 not in path_copy:
                                wc = False
                                break
                            elif t2 != '':
                                path_copy = path_copy.split(t2)[1]

                        if wc:
                            printout = True

            if printout:
                if args.minimal:
                    log.info("%s (%s)" % (path, class_name))
                roottools.RootTools.check_type(
                    root_file_names=args.root_file,
                    path_to_objects=path,
                    print_quantities=True
                )
            for regex, code in zip(args.elements, args.codes):
                if re.match(regex, path):
                    root_object = root_file.Get(path)
                    result = eval(code.replace("element", "root_object"))
                    if result:
                        log.info(code.replace("element", path))
                        log.info(result)
                    if index < len(elements)-1:
                        log.info("\n" + (100*"-") + "\n")
            if log.isEnabledFor(logging.DEBUG):
                root_object = root_file.Get(path)
                log.debug("")
                root_object.Print("")
                if index < len(elements)-1:
                    log.debug("\n" + (100*"=") + "\n")
                """
                if (class_name == "TTree") or (class_name == "TNtuple"):
                    log.debug("%s (%s, entries=%d)" % (path, class_name, root_object.GetEntries()))
                elif class_name.startswith("TH") or class_name.startswith("TProfile"):
                    log.debug("%s (%s, integral=%f)" % (path, class_name, root_object.Integral()))
                elif class_name.startswith("TGraph"):
                    log.debug("%s (%s, points=%d)" % (path, class_name, root_object.GetN()))
                """
