"""
Functional tests for the Plazza pizzeria simulation.
"""

import subprocess
import time
import sys
import fcntl
import os
from test_cases import TEST_DATA, TEST_CONFIG


def set_non_blocking(file_obj):
    """
    Set a file object to non-blocking mode.
    """
    flags = fcntl.fcntl(file_obj, fcntl.F_GETFL)
    fcntl.fcntl(file_obj, fcntl.F_SETFL, flags | os.O_NONBLOCK)


def read_available_output(process):
    """
    Read all currently available output from the process without blocking.
    """
    try:
        return process.stdout.read() or ""
    except (IOError, TypeError):
        return ""


def wait_for_output(process, expected, timeout=10):
    """
    Wait for specific strings to appear in the output.
    """
    start_time = time.time()
    accumulated_output = ""
    while time.time() - start_time < timeout:
        if process.poll() is not None:
            accumulated_output += read_available_output(process)
            break

        chunk = read_available_output(process)
        if chunk:
            accumulated_output += chunk
            if all(exp in accumulated_output for exp in expected):
                return True, accumulated_output
        time.sleep(0.1)

    found_all = all(exp in accumulated_output for exp in expected)
    return found_all, accumulated_output


def run_test_case(test_name, config, steps):
    """
    Run a multi-step test case.
    """
    print(f"Running test: {test_name}...")
    multiplier, cooks, regen = config
    success = True
    try:
        with subprocess.Popen(
            ["./plazza", str(multiplier), str(cooks), str(regen)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=0
        ) as process:
            set_non_blocking(process.stdout)

            for step in steps:
                if process.poll() is not None:
                    print(f"FAILED: Process died prematurely in {test_name}")
                    success = False
                    break

                if "send" in step:
                    try:
                        process.stdin.write(step["send"] + "\n")
                        process.stdin.flush()
                    except BrokenPipeError:
                        print(f"FAILED: Broken pipe in {test_name} (process died?)")
                        success = False
                        break
                if "sleep" in step:
                    time.sleep(step["sleep"])

                expected_list = step.get("expect", [])
                ok, output = wait_for_output(process, expected_list, step.get("timeout", 5))
                if not ok:
                    print(f"FAILED Step: {step.get('send', 'sleep')}. "
                          f"Expected {expected_list} not found.")
                    print(f"Output was:\n{output}")
                    success = False
                    break

                if "wait_for" in step:
                    ok, output = wait_for_output(process, step["wait_for"],
                                                 step.get("wait_timeout", 10))
                    if not ok:
                        print(f"FAILED Wait: {step['wait_for']} not found.")
                        print(f"Output was:\n{output}")
                        success = False
                        break

            process.terminate()
            try:
                process.wait(timeout=2)
            except subprocess.TimeoutExpired:
                process.kill()
            return success
    except Exception as err:  # pylint: disable=broad-except
        print(f"Error in {test_name}: {err}")
        return False


def main():
    """
    Entry point for functional tests.
    """
    overall_success = True

    for test in TEST_DATA:
        if "steps" in test:
            overall_success &= run_test_case(
                test["name"],
                test.get("config", TEST_CONFIG),
                test["steps"]
            )
        else:
            steps = [
                {"send": test["command"], "expect": test["expected"]}
            ]
            overall_success &= run_test_case(
                test["name"],
                test.get("config", TEST_CONFIG),
                steps
            )

    if not overall_success:
        sys.exit(1)


if __name__ == "__main__":
    main()
