# this is just an example, could go in some example dir
# it is not generic and will probably not run
# artus -c
# works
import Artus.Configuration.Base as base


def config():
    conf = base.BaseConfig('mc', '2012', 'z')
    conf["InputFiles"] = "/storage/a/berger/zpj/kappa539_MC12_madgraph_tags/*.root"  # or any other kappa skim
    conf = base.expand(conf, ['all', 'zcuts'])
    base.pipelinediff(conf)
    return conf

