"""
Helpers used by test files. 
"""

import multiprocessing 
from time import sleep 
import datetime
import random 
import subprocess


TESTS_TIMEOUT = 2
PAUSE_TIMEOUT = TESTS_TIMEOUT / 10


def start_test(comment_file_path, name):
  comment_file = open(comment_file_path, "a")
  current_time(comment_file)
  comment_file.write(name + ":")
  comment_file.close()

def start_suite(comment_file_path, name):
  global count_not_ok 
  comment_file = open(comment_file_path, "r")
  contents = comment_file.read()
  count_not_ok = contents.count("NOT OK")

  comment_file = open(comment_file_path, "a")
  comment_file.write(name)
  comment_file.write(":\n")
  comment_file.close()

def end_suite(comment_file_path):
  global count_not_ok 

  comment_file = open(comment_file_path, "r")
  contents = comment_file.read()
  comment_file.close()
  local_not_ok = contents.count("NOT OK")

  result = "FAIL\n"
  if local_not_ok == count_not_ok:
    result = "PASS\n"

  comment_file = open(comment_file_path, "a")
  comment_file.write("Result:")
  comment_file.write(result)
  comment_file.close()

def start_with_timeout(test_function, comment_file_path, student_dir="", timeout=TESTS_TIMEOUT, timeoutFeedback="NOT OK\n"):
  p = multiprocessing.Process(target=test_function, args=(comment_file_path, student_dir,))
  p.start()
  sleep(timeout)
  if p.is_alive():
    comment_file = open(comment_file_path, "a")
    comment_file.write(timeoutFeedback)
    comment_file.close()
    p.terminate()
    p.join()

def finish(comment_file_path, status="NOT OK\n", proc=None):
  comment_file = open(comment_file_path, "a")
  comment_file.write(status + "\n")  
  comment_file.close()

  if proc:
    try:
      proc.kill()
    except:
      pass

def start(executable_file):
    return subprocess.Popen(
        executable_file,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

def current_time(comment_file):
  comment_file.write(datetime.datetime.now().strftime("  %d-%B-%Y %H:%M:%S") + "----")

def read_stderr(process):
    return process.stderr.readline().decode("utf-8").strip()

def read_stdout(process):
    return process.stdout.readline().decode("utf-8").strip()

def write(process, message):
    process.stdin.write(f"{message.strip()}\n".encode("utf-8"))
    process.stdin.flush()

def remove_extra_spaces(s):
  prev_space = False 
  new_s = ""
  for char in s:
    if char != " " or (char == " " and not prev_space):
      new_s += char 
    
    prev_space = char == " "

  return new_s

