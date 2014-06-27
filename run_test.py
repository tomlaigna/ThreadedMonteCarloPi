import os, sys, multiprocessing, StringIO
from subprocess import Popen, PIPE

def runTest(threads, numbers):
  process = Popen("ThreadedMonteCarloPi.exe", stdout=PIPE, stdin=PIPE)
  process.stdin.write(str(threads) + '\n' + str(numbers) + '\n')
  (output, err) = process.communicate()
  exit_code = process.wait()
#  print (output)
  for line in StringIO.StringIO(output):
    if "Execution time:" in line:
      exec_time = line.split(" ")[2]
      return "exec time: " + exec_time + " milliseconds"

if len(sys.argv) > 1:
  N = int(sys.argv[1])
else:
  N = 1000000

total_cores = multiprocessing.cpu_count()
best_time = 0
best_threads = 0

print "Running test with total of " + str(N) + " random numbers"

for cores in range (1, 4 * (total_cores + 1)):
  print runTest(cores, N / cores) + " with " + str(cores) + " cores"
