import concurrent.futures
import pprint
import time
import os
import datetime
import statistics
import multiprocessing
import sys 
sys.path.append("milestone1tests/")
from milestone1tests import tests_compile, tests_commands, tests_echo, tests_launch


current_dir = os.path.dirname(os.path.abspath(__file__))
student_submissions_path = os.path.dirname(os.path.abspath(__file__))+ "/../"

def _helper_cd_to_student(student_dir):
  cwd = os.getcwd()  
  os.chdir(student_dir)
  cwd = os.getcwd()  

def _helper_return_to_original_dir():
  os.chdir(current_dir) 
  cwd = os.getcwd()  



def run_milestone1_tests(comment_file_path, student_dir):
  tests_launch.test_launch_suite(comment_file_path, student_dir)
  tests_commands.test_commands_suite(comment_file_path, student_dir)
  tests_echo.test_echo_suite(comment_file_path, student_dir)


def run_tests(comment_file_path, student_dir):
  _helper_cd_to_student(student_dir)
  ret = tests_compile.test_compile_suite(comment_file_path, student_dir)
  if ret:
    run_milestone1_tests(comment_file_path, student_dir)

  _helper_return_to_original_dir()  
  return 0 


def summarize_tests(comment_file_path, student_dir):
  comment_file = open(comment_file_path, "r")
  c = comment_file.read()
  count_pass = c.count("PASS")
  count_fail = c.count("FAIL")
  comment_file.close()

  return float(count_pass), float(count_pass + count_fail)


def test_plan():
  f = []
  for (dirpath, dirnames, filenames) in os.walk(student_dir):
    f.extend(filenames)
    break
  
  # Set up grade structure
  feedback_file = "studentTester"
  comment_file_path = student_dir + f"/FEEDBACK{feedback_file}.txt"
  comment_file = open(student_dir+f"/FEEDBACK{feedback_file}.txt", "w")
  comment_file.write(datetime.datetime.now().strftime("%d-%B-%Y %H:%M:%S"))
  comment_file.write(f"---Student: you\n")
  comment_file.close()
  student_point = {"total": 0}
  points = 0
  run_tests(comment_file_path, student_dir)

  points, total = summarize_tests(comment_file_path, student_dir)
  comment_file = open(comment_file_path, "a")
  comment_file.write(datetime.datetime.now().strftime("%d-%B-%Y %H:%M:%S") + f"--- Suites Passed {points}/{total}\n")
  comment_file.write(datetime.datetime.now().strftime("%d-%B-%Y %H:%M:%S") + " FINISHED\n")
  comment_file.close()
  return student_point


def begin_testing(entire_class=False, student=None, student_path=None):
  global student_dir 

  if not entire_class and student:
    print("Student: {} {}".format(student, student_submissions_path+student)) 

    student_dir = student_submissions_path+student
    test_plan()

  if entire_class:
    all_students = [x[0] for x in os.walk(student_submissions_path)]
    f = []
    for (dirpath, dirnames, filenames) in os.walk(student_submissions_path):
      f.extend(dirnames)
      break
    all_students = f
    count = 0

    for s in all_students:
      print(f"\n\nTesting student: {s.split('/')[-1]} / {count}")
      begin_testing(entire_class=False, student=s.split("/")[-1])
      count += 1


if __name__ == "__main__":
  start = datetime.datetime.now()
  begin_testing(entire_class=False, student="src")

