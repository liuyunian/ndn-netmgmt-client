# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
import sys
reload(sys)
sys.setdefaultencoding('utf-8')

def options(opt):
    opt.load(['compiler_c', 'compiler_cxx', 'qt5'])

def configure(conf):
    conf.load(['compiler_c', 'compiler_cxx', 'qt5'])
    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'], uselib_store='NDN_CXX', mandatory=True)
    conf.check_cfg(path='pcap-config', package='libpcap', args=['--libs', '--cflags'], uselib_store='PCAP', mandatory=True)

def build(bld): # 创建一个任务生成器，用来生成下面的任务
    bld.program(
        features = 'qt5 cxx',
        target='client', # 生成的可执行文件名
        source=bld.path.ant_glob(['src/client/*.cpp', 'src/client/ui/*.cpp', 'src/client/ui/*.ui']),
        includes = ". ./src/client ./src/client/ui",
        use='NDN_CXX QT5CORE QT5GUI QT5OPENGL QT5SVG QT5BASE QT5XML', # 使用的库名
    )

    bld.program(
        features='cxx',
        target='server',
        source=bld.path.ant_glob(['src/server/*.cpp']),
        use='NDN_CXX PCAP',
    )
