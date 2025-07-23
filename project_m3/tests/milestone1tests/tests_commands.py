from subprocess import CalledProcessError, STDOUT, check_output, TimeoutExpired, Popen, PIPE 
import os
import shutil
import pty
import datetime
import sys
from tests_helpers import * 


def _test_1unknown_command(comment_file_path, student_dir, timeout=TESTS_TIMEOUT_M1):
  start_test(comment_file_path, "Unknown command should display corresponding message")
  try:
    p = Popen(['./mysh'], stdout=PIPE, stderr=PIPE, stdin=PIPE)
    write_no_stdout_flush(p, "destroy")
    output = read_stderr(p)
    if "ERROR: Unknown command: destroy" in output and "input line too long" not in output:
      finish_process(comment_file_path, "OK", p)
    else:
      finish_process(comment_file_path, "NOT OK", p)
  except Exception as e:
    logger(e)
    finish_process(comment_file_path, "NOT OK", p)

def _test_10unknown_commands(comment_file_path, student_dir):
  start_test(comment_file_path, "Multiple unknown commands")
  try:
    process = start("./mysh")
    for i in range(10):
        write_no_stdout_flush(process, "nonexistantcommand" + str(i))
        if "ERROR: Unknown command" not in read_stderr(process):
          finish_process(comment_file_path, "NOT OK", process)
          return
    else:
      write_no_stdout_flush(process, "exit")
      finish_process(comment_file_path, "OK", process)
  except Exception as e:
    logger(e)
    finish_process(comment_file_path, "NOT OK", process)

def _test_short_line(comment_file_path, student_dir, timeout=TESTS_TIMEOUT_M1):
  start_test(comment_file_path, "A not-too-long and legal command input is valid")
  try:
    p = Popen(['./mysh'], stdout=PIPE, stderr=PIPE, stdin=PIPE)
    s = "echo " + "o" * 100
    write_no_stdout_flush(p, s)
    decoded = read_stdout(p)
    if "ERROR: " in decoded:
      finish_process(comment_file_path, "NOT OK", p)
    else:
      finish_process(comment_file_path, "OK", p)
  except Exception as e:
    logger(e)
    finish_process(comment_file_path, "NOT OK", p)

def _test_long_line(comment_file_path, student_dir, timeout=TESTS_TIMEOUT_M1):
  start_test(comment_file_path, "Long command input is invalid")
  try:
    p = Popen(['./mysh'], stdout=PIPE, stderr=PIPE, stdin=PIPE)
    s = "echo " + "o" * 135
    write_no_stdout_flush(p, s)
    decoded = read_stderr(p)
    if "ERROR: input line too long" in decoded and "Unrecognized command" not in decoded:
      finish_process(comment_file_path, "OK", p)
    else:
      finish_process(comment_file_path, "NOT OK", p)
  except Exception as e:
    logger(e)
    finish_process(comment_file_path, "NOT OK", p)

def _test_long_priority(comment_file_path, student_dir, timeout=TESTS_TIMEOUT_M1):
  start_test(comment_file_path, "Long command message takes priority")
  try:
    p = Popen(['./mysh'], stdout=PIPE, stderr=PIPE, stdin=PIPE)
    s = "a" * 140
    write_no_stdout_flush(p, s)
    decoded = read_stderr(p)
    if "ERROR: input line too long" in decoded and "Unrecognized command" not in decoded:
      finish_process(comment_file_path, "OK", p)
    else:
      finish_process(comment_file_path, "NOT OK", p)
  except Exception as e:
    logger(e)
    finish_process(comment_file_path, "NOT OK", p)


def test_commands_suite(comment_file_path, student_dir):
  start_suite(comment_file_path, "Unknown Command Message")
  start_with_timeout(_test_1unknown_command, comment_file_path)
  start_with_timeout(_test_10unknown_commands, comment_file_path)
  end_suite(comment_file_path)

  start_suite(comment_file_path, "Long Command Message")
  start_with_timeout(_test_short_line, comment_file_path)
  start_with_timeout(_test_long_line, comment_file_path)
  start_with_timeout(_test_long_priority, comment_file_path)
  end_suite(comment_file_path)

