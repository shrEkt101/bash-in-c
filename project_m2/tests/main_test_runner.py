import sys 
import time
import os
import datetime
import statistics
import multiprocessing

current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(current_dir + "/milestone1tests/")
sys.path.append(current_dir + "/milestone2tests/")

# Milestone 1 tests
import tests_compile, tests_commands, tests_echo, tests_launch
# Milestone 2 tests
import tests_variables

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

def run_milestone2_tests(comment_file_path, student_dir): 
  tests_variables.test_variables_suite(comment_file_path, student_dir)



def run_tests(comment_file_path, student_dir):
  _helper_cd_to_student(student_dir)
  ret = tests_compile.test_compile_suite(comment_file_path, student_dir)
  if ret:
    run_milestone1_tests(comment_file_path, student_dir)
    run_milestone2_tests(comment_file_path, student_dir)
  _helper_return_to_original_dir()  
  return 0 


def summarize_student_grade(student_dir, comment_file):
  comment_file = open(student_dir + f"/FEEDBACK.txt", "r")
  c = comment_file.read()
  count_pass = c.count("PASS")
  count_fail = c.count("FAIL")
  count_recomp = c.count("RECOMPILE")
  count_unexp = c.count("UNEXPECTED")
  count_starter = c.count("STARTER CODE")
  count_expected_ok = c.count("EXPECTED_OK")
  count_expected_not_ok = c.count("EXPECTED_NOT_OK")
  comment_file.close()


  total = count_fail + count_pass + count_unexp + (count_expected_ok + count_expected_not_ok) * 0.1
  points = count_pass + count_expected_ok * 0.1 
  if count_recomp != 0:
    total -= 1
    points = int(0.8 * points)
  failed_compilation = "Code compiles with starter NiceMakefile (allowing warnings):FAIL"
  if failed_compilation in c or count_starter > 0:
    points = 0
  return float(points), total



def test_plan():
  f = []
  for (dirpath, dirnames, filenames) in os.walk(student_dir):
    f.extend(filenames)
    break
  
  # Set up grade structure
  comment_file_path = student_dir+f"/FEEDBACK.txt"
  comment_file = open(comment_file_path, "w")
  comment_file.write(datetime.datetime.now().strftime("%d-%B-%Y %H:%M:%S") + "")
  comment_file.write(f"---Student: {student_dir.split('/')[-2]}\n")
  comment_file.close()
  student_point = {"total": 0}
  points = 0
  run_tests(comment_file_path, student_dir)
  points, total = summarize_student_grade(student_dir, comment_file)
  comment_file = open(comment_file_path, "a")
  comment_file.write(datetime.datetime.now().strftime("%d-%B-%Y %H:%M:%S") + f"--- Test Suites Passed {points}/{total}\n")
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
