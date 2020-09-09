# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
import sys
reload(sys)
sys.setdefaultencoding('utf-8')

def options(opt):
    opt.load(['compiler_c', 'compiler_cxx', 'qt5'])

def configure(conf):
    conf.load(['compiler_c', 'compiler_cxx', 'qt5'])
    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'], uselib_store='NDN_CXX', mandatory=True)

def build(bld): # 创建一个任务生成器，用来生成下面的任务
    bld.program(
        features = 'qt5 cxx',
        target='client', # 生成的可执行文件名
        source=bld.path.ant_glob(['src/*.cpp', 'src/ui/*.cpp', 'src/ui/*.ui']),
        includes = ". ./src ./src/ui",
        use='NDN_CXX QT5CORE QT5OPENGL QT5XML', # 使用的库名
    )
