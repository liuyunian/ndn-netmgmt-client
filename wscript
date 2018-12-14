# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
import sys
reload(sys)
sys.setdefaultencoding('utf-8')


def options(opt):
    opt.load('compiler_cxx')

def configure(conf):
    conf.load('compiler_cxx')
    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'], uselib_store='NDN_CXX', mandatory=True)

def build(bld):
    bld.program(
        target='client',
        source=['src/client/client.cpp','src/client/ndn_client.cpp'],
        use='NDN_CXX',
    )

    bld.program(
        features='cxx',
        target='server',
        source=['src/server/server.cpp','src/server/ndn_producer.cpp'],
        use='NDN_CXX',
    )
