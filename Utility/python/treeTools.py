# -*- coding: utf-8 -*-


__all__ = [
    "TreeExtender", "TreeMerger", "TreeIOHelper",
    "TreePathIOError", "TreePathFormatError"
]


# python imports
import os
import gc
import re
from array import array
import ROOT


class TreeExtender(object):
    """
    Helper class to add new branches to an existing tree. *treePath* defines to tree to extend. See
    :py:class:`TreeIOHelper` for more info on tree paths. If *copyTreePath* is not ``None``, the
    file that contains the tree to extend is copied first. *force* is fowarded to
    :py:func:`TreeIOHelper.copyTree`. *writeEvery* describes the number of entries to be processed
    between each *Write* event.

    New branches are filled within the iterator of this class. Their values may depend on the values
    of existing branches. Therefore, it is not performant to unpack all of them. You can configure
    the branches to unpack when calling :py:func:`addBranch` with the *unpackBranches* argument.

    .. code-block:: python

       with TreeExtender("/source/file.root/myTree", "/target/file.root") as extender:
           extender.addBranch("myNewBranch", nLeaves=1, unpackBranches=["branchXYZ"])

           for entry in extender:
               entry.myNewBranch[0] = entry.branchXYZ * 2

    In the example above, *entry* is a wrapper around the arrays associated to the branches. It
    provides getters to both new and existing, unpacked branches returning the associated arrays.

    Prior to each iteration, the values of new branches are set to a default value. You may change
    this value be changing the class member *DEFAULT_VALUE* which defaults to ``-1e5``.
    """

    DEFAULT_VALUE = -1e5

    def __init__(self, treePath, copyTreePath=None, force=False, writeEvery=-1):
        super(TreeExtender, self).__init__()

        self.treeIOHelper = TreeIOHelper()

        if copyTreePath is not None:
            treePath = self.treeIOHelper.copyTree(treePath, copyTreePath, force=force)

        filePath = self.treeIOHelper.splitTreePath(treePath)[0]

        self.tree = self.treeIOHelper.getTree(treePath, mode="UPDATE")
        self.file = self.treeIOHelper._files[filePath]

        # store data for new branches in tuples (name, nLeaves)
        # and names of branches to unpack
        self.branchData     = []
        self.unpackBranches = set()

        self.writeEvery = writeEvery

        self.isTuple  = isinstance(self.tree, ROOT.TNtuple)
        self.extended = False

    def __del__(self):
        del(self.tree)
        del(self.file)
        del(self.treeIOHelper)

    def __enter__(self):
        return self

    def __exit__(self, cls, err, traceback):
        return False

    def addBranch(self, name, nLeaves=1, unpackBranches=None):
        """
        Adds a new branch *name* with *nLeaves* when *extend* is invoked, i.e. when the internal
        iterator is requested. *unpackBranches* should be a list of names of branches that will be
        unpacked during the iterations. ``"*"`` will unpack all branches.
        """
        if self.isTuple:
            nLeaves = 1

        self.branchData.append((name, nLeaves))

        # parse unpackBranches
        if unpackBranches is None:
            unpackBranches = set()
        elif unpackBranches == "*":
            unpackBranches = set(b.GetName() for b in self.tree.GetListOfBranches())
        if not hasattr(unpackBranches, "__iter__"):
            unpackBranches = [unpackBranches]
        if not isinstance(unpackBranches, set):
            unpackBranches = set(unpackBranches)
        unpackBranches.add(name)

        self.unpackBranches.update(unpackBranches)

    def __iter__(self):
        if self.extended:
            return
        self.extended = True

        # store arrays of new branches and branches to unpack mapped to their names
        arrays = {}

        # in each iteration, a wrapper to the current entry is yielded that provides
        # access to leaves via attributes
        class Entry(object): pass

        # factory functions to return branch name dependent getters
        def getter(name):
            def getter(self):
                return arrays[name]
            return getter

        # loop through branch data and create new branches
        # store new branches to call their fill methods after each iteration
        # store new arrays and ranges according to their lengths for a reset before each iteration
        newBranches = []
        newArrays   = []
        for name, nLeaves in self.branchData:
            arr = array("f", nLeaves * [0.])
            branch = self.tree.Branch(name, arr, name + "/F")
            arrays[name] = arr
            newBranches.append(branch)
            newArrays.append((arr, range(nLeaves)))
            setattr(Entry, name, property(getter(name)))

        # also store the arrays of the already existing branches
        for name in self.unpackBranches:
            if name in arrays:
                continue
            branch = self.tree.GetBranch(name)
            nLeaves = branch.GetNleaves()
            arr = array("f", nLeaves * [0.])
            self.tree.SetBranchAddress(name, arr)
            arrays[name] = arr
            setattr(Entry, name, property(getter(name)))

        # create the entry wrapper
        entry = Entry()

        # loop through the tree and fill all branches after yielding is finished
        for i in range(self.tree.GetEntries()):
            # unpack a new entry
            self.tree.GetEntry(i)

            # reset values of new branches to a default value
            for arr, leafRange in newArrays:
                for j in leafRange:
                    arr[j] = self.DEFAULT_VALUE

            # yield
            yield entry

            # fill new branches
            for branch in newBranches:
                branch.Fill()

            if self.writeEvery > 0 and not (i + 1) % self.writeEvery:
                self.tree.Write()

        # write the tree
        self.file.cd()
        self.tree.Write()

        gc.collect()


class TreeMerger(object):
    """
    Helper class to merge trees. You can see this class as a programatic approach to ROOT's *hadd*
    tool. The target tree is given by *treePath*. See :py:class:`TreeIOHelper` for more info on tree
    paths. Unless *force* is ``True``, a :py:class:`TreePathIOError` is raised in case the
    target file already exists.

    .. code-block:: python

        with TreeMerger("/target/file.root/myMergedTree") as merger:
            merger.addTree("/source/file1.root")
            merger.addTree("/source/file2.root")

    The above example makes use of python's *context guards*. The actual merging process starts when
    the guard is closed. Internally, the :py:func:`merge` method is invoked. An equivalent example
    is:

    .. code-block:: python

        merger = TreeMerger("/target/file.root/myMergedTree")
        merger.addTree("/source/file1.root")
        merger.addTree("/source/file2.root")
        merger.merge()
    """

    def __init__(self, treePath, force=False):
        super(TreeMerger, self).__init__()

        self.treeIOHelper = TreeIOHelper()

        filePath, treeName = self.treeIOHelper.splitTreePath(treePath)

        if os.path.exists(filePath) and not force:
            raise TreePathIOError(treePath, "file already exists")

        self.filePath = filePath
        self.treeName = treeName

        self.treePaths = []

        self.merged = False

    def __del__(self):
        del(self.treeIOHelper)

    def __enter__(self):
        return self

    def __exit__(self, cls, err, traceback):
        self.merge()
        return False

    def addTree(self, treePath):
        """
        Add a tree for merging defined by *treePath*.
        """
        self.treePaths.append(treePath)

    def merge(self):
        """
        Starts the merging process and writes the target file.
        """
        if self.merged:
            return
        self.merged = True

        if not len(self.treePaths):
            return

        # read in all trees and store them in a tlist
        tlist = ROOT.TList()
        for i, treePath in enumerate(self.treePaths):
            tree = self.treeIOHelper.getTree(treePath)
            if i == 0 and self.treeName is None:
                self.treeName = tree.GetName()
            tlist.Add(tree)

        # create the target root file
        tfile = ROOT.TFile(self.filePath, "RECREATE")
        tfile.cd()

        # create the merged tree
        mergedTree = ROOT.TTree.MergeTrees(tlist)
        mergedTree.SetName(self.treeName)

        # write it and close the file
        mergedTree.Write()
        tfile.Close()

        gc.collect()


class TreeIOHelper(object):
    """
    Helper class providing easy IO handling when working with trees.
    """

    TREEPATH_CRE = re.compile("^(.*\.root)(|%s(.+))$" % os.sep)

    def __init__(self):
        super(TreeIOHelper, self).__init__()

        # temporary opened files mapped to their paths
        self._files = {}

    def __del__(self):
        # close temporary files
        for tfile in self._files.values():
            tfile.Close()

    @classmethod
    def splitTreePath(cls, treePath, expand=True):
        """
        Splits a *treePath* into a file path and a tree name and returns them in a tuple. The valid
        format is */path/to/root/file.root[/treeName]* which is the same format as excepted by
        ROOT's `TChain <https://root.cern.ch/root/html/TChain.html>`_. When no tree name is given,
        *(filePath, None)* is returned. When the format is invalid, a
        :py:class:`TreePathFormatError` is raised. When *expand* is ``True``, environment and user
        variables in the file path are expanded. Examples:

        .. code-block:: python

            splitTreePath("/tmp/file.root")
            # -> ("/tmp/file.root", None)

            splitTreePath("/tmp/file.root/myTree")
            # -> ("/tmp/file.root", "myTree")

            splitTreePath("/tmp/file.root/")
            # -> TreePathFormatError

            splitTreePath("/tmp/file.txt/myTree")
            # -> TreePathFormatError
        """
        m = cls.TREEPATH_CRE.match(treePath)
        if not m:
            raise TreePathFormatError(treePath)

        filePath, treeName = m.group(1), m.group(3)

        if expand:
            filePath = os.path.expandvars(os.path.expanduser(filePath))

        return filePath, treeName

    @classmethod
    def joinTreePath(cls, filePath, treeName):
        """
        Joins a *filePath* and a *treeName* with *os.sep* to a valid tree path. When the format is
        invalid, a :py:class:`TreePathFormatError` is raised. For more info on the format, see
        :py:func:`splitTreePath`.
        """
        treePath = os.path.join(filePath, treeName)
        if not cls.TREEPATH_CRE.match(treePath):
            raise TreePathFormatError(treePath)

        return treePath

    def getTree(self, treePath, getFirst=True, mode="READ"):
        """
        Returns a tree based on *treePath* using :py:func:`splitTreePath`. When *treePath* does not
        contain a tree name and *getFirst* is ``True``, the first tree of the file is returned.
        *mode* is passed to the `TFile <https://root.cern.ch/root/html/TFile.html>`_ constructor.

        .. code-block:: python

            getTree("/tmp/file.root/myTree")
            # -> <ROOT.TTree object ("myTree") at 0x2cb6400>
        """
        if isinstance(treePath, ROOT.TTree):
            return treePath

        filePath, treeName = self.splitTreePath(treePath)

        if not os.path.exists(filePath):
            raise TreePathIOError(treePath, "no such file")

        tfile = self._files.setdefault(filePath, ROOT.TFile(filePath, mode))

        if treeName is None:
            if getFirst:
                for tkey in tfile.GetListOfKeys():
                    tobj = tfile.Get(tkey.GetName())
                    if isinstance(tobj, ROOT.TTree):
                        return tobj
            raise TreePathIOError(treePath, "no tree found")
        else:
            tree = tfile.Get(treeName)
            if not isinstance(tree, ROOT.TTree):
                raise TreePathIOError(treePath, "no tree found")
            return tree

    def copyTree(self, treePath, copyTreePath, force=False):
        """
        Copies a tree given by *treePath* into an other file identified by *copyTreePath* and
        returns the final *copyTreePath*. When it does not contain a tree name, the one given in
        *treePath* is used. Unless *force* is ``True``, a :py:class:`TreePathIOError` is raised when
        the target file already exists.
        """

        tree = self.getTree(treePath)

        copyFilePath, copyTreeName = self.splitTreePath(copyTreePath)
        if os.path.exists(copyFilePath) and not force:
            raise TreePathIOError(copyTreePath, "file already exists")

        if copyTreeName is None:
            copyTreeName = tree.GetName()

        tfile = ROOT.TFile(copyFilePath, "RECREATE")
        tfile.cd()

        # '-1' and 'fast' disable unpacking the baskests
        # see http://root.cern.ch/root/html/TTree.html#TTree:CloneTree
        treeCopy = tree.CloneTree(-1, "fast")
        treeCopy.SetName(copyTreeName)
        treeCopy.Write()
        tfile.Close()

        return self.joinTreePath(copyFilePath, copyTreeName)

    def getFirstTreeName(self, treePath):
        """
        Returns the name of the first tree found in a file defined by *treePath*. If *treePath*
        already contains a tree name, this name is returned. A :py:class:`TreePathIOError` is
        raised, when the file does not contain a tree at all.
        """
        filePath, treeName = self.splitTreePath(treePath)
        if treeName is not None:
            return treeName

        if not os.path.exists(filePath):
            raise TreePathIOError(treePath, "no such file")

        isCached = filePath in self._files
        tfile    = self._files.get(filePath, ROOT.TFile(filePath))

        for tkey in tfile.GetListOfKeys():
            name = tkey.GetName()
            tobj = tfile.Get(name)
            if isinstance(tobj, ROOT.TTree):
                treeName = name
                break

        if treeName is None:
            raise TreePathIOError(treePath, "no tree found")

        if not isCached:
            tfile.Close()

        return treeName


class TreePathIOError(Exception):
    """
    Raised when a *treePath* could not be resolved to an existing tree.
    """

    def __init__(self, treePath, msg):
        """
        __init__()
        """
        msg += ": " + str(treePath)
        super(TreePathIOError, self).__init__(msg)


class TreePathFormatError(Exception):
    """
    Raised when the format of a *treePath* is invalid. The valid format is
    */path/to/root/file.root[/treeName]*.
    """

    def __init__(self, treePath):
        """
        __init__()
        """
        msg = "invalid format: " + str(treePath)
        super(TreePathFormatError, self).__init__(msg)


if __name__ == "__main__":
    pass
    # test TreeExtender
    # src = "/user/rieger/dnnTuples/test.root"
    # dst = "/user/rieger/dnnTuples/foo.root"
    # with TreeExtender(src) as te:
    #     te.addBranch("someNewBranch", unpackBranches=["Reco__matched_b_t__Px", "Reco__matched_b_t__Py"])
    #     for entry in te:
    #         entry.someNewBranch[0] = entry.Reco__matched_b_t__Px + entry.Reco__matched_b_t__Py


    # test TreeMerger
    # with TreeMerger("/user/rieger/merged.root") as tm:
    #     tm.addTree("/user/rieger/foo.root")
    #     tm.addTree("/user/rieger/bar.root")
