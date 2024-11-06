#!/usr/bin/env python

import argparse
import inspect
import platform
import os
import shutil
import subprocess
import sys
import textwrap

_PROJECT_NAME = "evelike"
_DEFAULT_INSTALL_DIR_NAME = "{}_install".format(_PROJECT_NAME)
_DEFAULT_BUILD_DIR_NAME = "build"
_EXIT_SUCCESS = 0
_EXIT_FAILURE = 1
_DBG_DIR_NAME = "Debug"
_REL_DIR_NAME = "Release"
_HELP_TEXT_WIDTH = 68

# Quite: nothing except problems.
_QUIET = 0
# Feedback: show that its working.
_FEEDBACK = 1
# Diagnostic: allow inspection of process
_DIAGNOSTIC = 2


class LineWrapRawTextHelpFormatter(argparse.RawDescriptionHelpFormatter):
    def _split_lines(self, text, width):
        text = self._whitespace_matcher.sub(' ', text).strip()
        return textwrap.wrap(text, _HELP_TEXT_WIDTH)


def _get_project_dir():
    """Return the absolute path to the project's location on the user's computer, e.g. the git repository.

    This is needed to be able to locate the project's CMake files etc. It works by knowing that this file `build.py`
    forms part of the project files.
    """
    this_file = os.path.abspath(inspect.getsourcefile(lambda: 0))
    this_file_dir = os.path.dirname(this_file)
    return this_file_dir


def _get_default_build_dir():
    """Return the absolute path of the build directory, into which will be placed the outputs of the build process.

    This will typically contain two child directories, `release` and `debug` and the optional local installation
    directory.
    """
    pwd = os.getcwd()
    return os.path.abspath(os.path.join(pwd, _DEFAULT_BUILD_DIR_NAME))


def cmake_cmd(project_dir, build_dir, dep, debug, fresh):
    """Returns the cmake generate command line as list."""
    src = os.path.join(project_dir, "external", dep)
    build = os.path.join(build_dir, dep)
    cmd = ["cmake",
           "-S{}".format(src),
           "-B{}".format(build),
           "-DCMAKE_BUILD_TYPE={}".format("Debug" if debug else "Release")]
    if fresh:
        cmd.insert(1, "--fresh")
    return cmd


def build_cmd(build_dir, dep, debug, fresh):
    """Returns the cmake build command line as list."""
    build = os.path.join(build_dir, dep)
    cmd = ["cmake",
           "--build",
           "{}".format(build),
           "--parallel", "4",
           "--config", "{}".format("Debug" if debug else "Release")]
    if fresh:
        cmd.insert(3, "--clean-first")
    return cmd


def install_cmd(build_dir, dep, debug, verbose):
    """Returns the cmake build command line as list."""
    build = os.path.join(build_dir, dep)
    cmd = ["cmake",
           "--install",
           "{}".format(build),
          # "--prefix", "{}".format(build_dir)
           ]
    if not debug:
        cmd.insert(3, "--strip")
    if verbose:
        cmd.insert(3, "--verbose")
    return cmd


def _build_and_install_all(project_dir,
                           build_dir,
                           debug_only,
                           release_only,
                           fresh,
                           run_tests,
                           verbose_lvl,
                           windows_from_linux):
    """Use `CMake` to build and install the project, in both Debug and Release configurations.

    This uses Python to run variants on the following commands:
    ```
    cmake -Sproject_dir -Bbuild_dir/release -DCMAKE_BUILD_TYPE=Release
    cmake --build build_dir/release --target install --config Release
    ```

    :param project_dir:     The directory where the project is located (the git repository).
    :param build_dir:       The directory where the CMake will put the generated project files and folders.
    :param verbose_lvl:     Integer indicating how verbose we should be during processing. 0 means only print errors.
    """
    build_dir_dbg = os.path.join(build_dir, _DBG_DIR_NAME)
    build_dir_rel = os.path.join(build_dir, _REL_DIR_NAME)

    has_debug = not release_only
    has_release = not debug_only

    if verbose_lvl > _QUIET:
        sys.stdout.write("Project dir:   {}\n".format(project_dir))
        sys.stdout.write("Build dir:     {}\n".format(build_dir))
        sys.stdout.write("Debug only:    {}\n".format(debug_only))
        sys.stdout.write("Release only:  {}\n".format(release_only))
        sys.stdout.write("Fresh:         {}\n".format(fresh))
        sys.stdout.write("Run tests:     {}\n".format(run_tests))
        sys.stdout.write("Verbose lvl:   {}\n".format(verbose_lvl))
        sys.stdout.write("Win32 on Linux {}\n".format(windows_from_linux))
        sys.stdout.write("\n")

    # Remove the release and debug directories.
    if fresh:
        if debug_only:
            shutil.rmtree(build_dir_dbg, ignore_errors=True)
        elif release_only:
            shutil.rmtree(build_dir_rel, ignore_errors=True)
        else:
            shutil.rmtree(build_dir_dbg, ignore_errors=True)
            shutil.rmtree(build_dir_rel, ignore_errors=True)

    # The CMake generate commands. These will use whatever generator the user's
    # CMake has by default.
    generate_rel = ["cmake",
                    "-S{}".format(project_dir),
                    "-B{}".format(build_dir_rel),
                    "-DCMAKE_BUILD_TYPE=Release"]

    generate_dbg = ["cmake",
                    "-S{}".format(project_dir),
                    "-B{}".format(build_dir_dbg),
                    "-DCMAKE_BUILD_TYPE=Debug",
                    "-DCMAKE_DEBUG_POSTFIX=_d"]

    # The CMake build commands:
    build_dbg = ["cmake", "--build", build_dir_dbg, "--parallel", "4", "--config", "Debug"]
    build_rel = ["cmake", "--build", build_dir_rel, "--parallel", "4", "--config", "Release"]

    # Regenerate CMake and the build.
    if fresh:
        generate_rel.insert(1, "--fresh")
        generate_dbg.insert(1, "--fresh")
        build_dbg.insert(3, "--clean-first")
        build_rel.insert(3, "--clean-first")

    # Are we cross compiling for Windows on Linux?
    if windows_from_linux:
        cross_compile_file = os.path.join(project_dir, "TC-mingw.cmake")
        sys.stdout.write("cross_compile_file {}\n".format(cross_compile_file))
        generate_rel.insert(1, "-DCMAKE_TOOLCHAIN_FILE={}".format(cross_compile_file))
        generate_dbg.insert(1, "-DCMAKE_TOOLCHAIN_FILE={}".format(cross_compile_file))
        build_dbg.append("-DCMAKE_TOOLCHAIN_FILE={}".format(cross_compile_file))
        build_rel.append("-DCMAKE_TOOLCHAIN_FILE={}".format(cross_compile_file))

    # The CMake test commands:
    test_dbg = ["ctest", "--output-on-failure", "-C", "Debug"]
    test_rel = ["ctest", "--output-on-failure", "-C", "Release"]
    if verbose_lvl == _FEEDBACK:
        test_dbg.append("--verbose")
        test_rel.append("--verbose")
    if verbose_lvl >= _DIAGNOSTIC:
        test_dbg.append("--extra-verbose")
        test_rel.append("--extra-verbose")

    # Run the following command lines in this order. CTest has no command line
    # way of having it's directory set.
    commands = dict()

    # So we can separate project building from dependency building we have
    # two booleans, `build_and_install_deps` and `build_project`
    if has_debug:
        commands["Generating CMake debug project"] = [generate_dbg, None]
        commands["Building debug version"] = [build_dbg, None]
        if run_tests:
            commands["Testing debug version"] = [test_dbg, build_dir_dbg]

    if has_release:
        commands["Generating CMake release project"] = [generate_rel, None]
        commands["Building release version"] = [build_rel, None]
        if run_tests:
            commands["Testing release version"] = [test_rel, build_dir_rel]

    # Run each command.
    for explanation, entry in commands.items():
        command = entry[0]
        working_dir = entry[1]
        try:
            if verbose_lvl > _QUIET:
                # Write the explanation of the command.
                sys.stdout.write("{} ...".format(explanation))
                sys.stdout.flush()
            if verbose_lvl > _FEEDBACK:
                # Write the command.
                sys.stdout.write("\n{}\n".format(' '.join(command)))
                sys.stdout.flush()
            if verbose_lvl == _QUIET:
                # Capture output and only show it if there is an exception.
                subprocess.run(command, check=True, cwd=working_dir,
                               stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            else:
                # In non-quiet mode we see the output as it occurs. This
                # gives nice realtime feedback.
                subprocess.run(command, check=True, cwd=working_dir)
                sys.stdout.write("OK.\n")
        except subprocess.CalledProcessError as e:
            cmd_as_string = " ".join(command)
            sys.stderr.write(
                "Exception when calling:\n`{}`\n".format(cmd_as_string))
            if e.output:
                # There will be no captured output from a verbose run.
                sys.stderr.write("\nError:\n{}\n".format(
                    str(e.output, "utf-8")))
            sys.stdout.write("\nFailed.\n")
            sys.exit(_EXIT_FAILURE)
    if verbose_lvl > _QUIET:
        sys.stdout.write("Succeeded.\n")


def main():
    """Run the build application."""
    default_build_dir = _get_default_build_dir()
    project_dir = _get_project_dir()
    help = ('specify the intermediate directory where `%(prog)s` '
            'will be built. By default this will be `{bd}`. The '
            '`{dbg}` and `{rel}` directories will be created '
            'here.').format(bd=default_build_dir,
                            dbg=_DBG_DIR_NAME,
                            rel=_REL_DIR_NAME)
    description = 'Builds `{proj}` and its dependencies. '.format(proj=_PROJECT_NAME,
                                                                  bld=_DEFAULT_INSTALL_DIR_NAME)

    on_linux = "Linux" == platform.system()

    parser = argparse.ArgumentParser(prog=_PROJECT_NAME,
                                     formatter_class=LineWrapRawTextHelpFormatter,
                                     description=description)

    parser.add_argument('-b', '--build-dir',
                        metavar="DIR",
                        help=help,
                        default=default_build_dir)

    parser.add_argument('-f', '--fresh',
                        action='store_true',
                        help='do clean CMake and build.')

    parser.add_argument('-n', '--no-tests',
                        action='store_true',
                        help='do not run tests.')

    # Add Windows cross-compile option to Linux:
    if on_linux:
        parser.add_argument('-w', '--windows-cross-compile',
                            action='store_true',
                            help='cross-compile for Windows from Linux (disables tests).')

    # We do not permit 'debug-only' and 'release-only' at the same time:
    targets_group = parser.add_mutually_exclusive_group()
    targets_group.add_argument('-d', '--debug-only',
                               action='store_true',
                               help='only Debug targets.')
    targets_group.add_argument('-r', '--release-only',
                               action='store_true',
                               help='only Release targets.')

    # We do not permit 'quiet' and 'verbose' at the same time.
    verbosity_group = parser.add_mutually_exclusive_group()
    verbosity_group.add_argument('-q', '--quiet',
                                 action='store_true',
                                 help='print no output except in the event of an error.')
    verbosity_group.add_argument('-v', '--verbose',
                                 action="count", default=0,
                                 help='print output from the generate and build steps.')

    args = parser.parse_args()

    # Determine how much to print. Default is just the simple progress messages generated by this script.
    if args.quiet:
        verbose_lvl = 0
    else:
        # Full will include output from CMake and the user's build system e.g. Make or Visual Studio.
        verbose_lvl = 1 + args.verbose

    # Are we on Linux and want cross compile to Windows?
    windows_from_linux = on_linux and args.windows_cross_compile

    # Disable testing when cross compiling (we cant run them)
    run_tests = False if windows_from_linux or args.no_tests else True

    # Perform generate, build and install steps:
    _build_and_install_all(project_dir=project_dir,
                           build_dir=args.build_dir,
                           debug_only=args.debug_only,
                           release_only=args.release_only,
                           fresh=args.fresh,
                           run_tests=run_tests,
                           verbose_lvl=verbose_lvl,
                           windows_from_linux=windows_from_linux
                        )

    sys.exit(_EXIT_SUCCESS)


if __name__ == "__main__":
    main()
