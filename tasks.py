import os

from invoke import Collection, task

ROOT_DIR = os.path.dirname(__file__)
PROJECT_DIR = os.path.join(".")
BUILD_DIR = os.path.join("./build")
EXE="firmware.elf"
FLASH_SCRIPT="flash.jlink"
DEBUG_SCRIPT="debug.jlink"

@task
def debug(c):
    """Debug with J-Link Edu Mini"""
    with c.cd("build"):
        c.run("JLinkGDBServer -select USB=801031598 -device RP2040_M0_1 -endian little -if SWD -speed 4000 \
                                -ir -noLocalhostOnly -nologtofile",pty=True)
        # c.run("arm-none-eabi-gdb {}".format(EXE),pty=True)
        # c.run('kill $(pgrep JLinkGDBServer)',pty=True)


@task
def clean(c):
    """Clean all binaries build from project"""
    c.run("rm -rf build/*".format(EXE)) 
    
@task
def test(c):
    """Run all unit tests"""

@task
def cmake(c):
    """Generate Makefile from CMakefile"""
    with c.cd(BUILD_DIR):
        c.run("cmake ..")

@task(pre=[cmake])
def build(c):
    """Build the project"""
    with c.cd(BUILD_DIR):
        c.run("make -j4")

@task(pre=[build])
def flash(c):
    """Get ELF file and flash to device"""
    with c.cd(BUILD_DIR):
       c.run("JLinkExe ../{}".format(FLASH_SCRIPT))
