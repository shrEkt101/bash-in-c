"""
Helpers used by test files. 
"""

import multiprocessing
from sys import stdin
from time import sleep 
import datetime
import random 
import subprocess
import os

TESTS_TIMEOUT_M1 = 2
PAUSE_TIMEOUT = TESTS_TIMEOUT_M1 / 10

TESTS_TIMEOUT_M2 = 0.5
debug = False

def enable_debug():
  print("Debug mode enabled")
  global debug
  debug = True

def logger(*message):
  if debug:
    print(*message)

def start(executable_file):
    return subprocess.Popen(
        executable_file,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

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

def remove_folder(path):
  command = "rm -rf {}".format(path)
  os.system(command)

def remove_file(path):
  command = "rm {}".format(path)
  os.system(command)

def start_test(comment_file_path, name):
  comment_file = open(comment_file_path, "a")
  current_time(comment_file)
  comment_file.write(name + ":")
  comment_file.close()

def start_with_timeout(test_function, comment_file_path, student_dir="", timeout=1, timeoutFeedback="NOT OK (TIMEOUT)\n"):
  p = multiprocessing.Process(target=test_function, args=(comment_file_path, student_dir,))
  p.start()
  sleep(timeout)
  if p.is_alive():
    comment_file = open(comment_file_path, "a")
    comment_file.write(timeoutFeedback)
    comment_file.close()
    p.terminate()
    p.join()


def finish_process(comment_file_path, status="NOT OK\n", proc=None):
  comment_file = open(comment_file_path, "a")
  comment_file.write(status + "\n")
  comment_file.close()

  if proc:
    try:
      proc.kill()
    except:
      pass


def finish(comment_file_path, status="NOT OK"):
  comment_file = open(comment_file_path, "a")
  comment_file.write(status + "\n")  
  comment_file.close()


def write_no_stdout_flush(process, message):
    process.stdin.write(f"{message.strip()}\n".encode("utf-8"))
    process.stdin.flush()

def write(process, message):
    process.stdin.write(f"{message.strip()}\n".encode("utf-8"))
    process.stdin.flush()
    process.stdout.flush()

def current_time(comment_file):
  comment_file.write(datetime.datetime.now().strftime("  %d-%B-%Y %H:%M:%S") + "----")

def read_stderr(process):
    return process.stderr.readline().decode("utf-8").strip()

def read_stdout(process):
    return process.stdout.readline().decode("utf-8").strip()

def has_memory_leaks(p) -> bool:
  """Given a process, terminate the process and 
  verify if there were memory leaks"""
  write(p, "exit")
  sleep(0.1)    # Give the process some time to terminate peacefully  
  rsl = p.poll()
  if rsl == None:
    # Didn't terminate.... give it a bit more time 
    sleep(0.5)
    rsl = p.poll()

  if rsl == None:
    # Still didn't terminate, problem with exit. Report a problem. 
    return True 
  
  return bool(p.poll())   # 1 if there are memory leaks, 0 if no memory leaks 


def remove_extra_spaces(s):
  prev_space = False 
  new_s = ""
  for char in s:
    if char != " " or (char == " " and not prev_space):
      new_s += char 
    
    prev_space = char == " "

  return new_s


def generate_random_message():
  length = random.randint(10, 30)
  message = ""
  characters = ["a", "b", "c", "d", "e", " "]
  for i in range(length):
    message += random.choice(characters)
  return message

