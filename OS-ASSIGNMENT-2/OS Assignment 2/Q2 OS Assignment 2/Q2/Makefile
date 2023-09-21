# List of C source files
SRCS = wordcount.c process1.c process2.c process3.c parent_program.c

# List of C executable targets
EXES = wordcount process1 process2 process3 parent_program

# Target to compile all C executables and run the Python script
all: $(EXES) run

# Compile C source files
%: %.c
	sudo gcc -o $@ $< -lrt

run:
	# Execute the C programs in the background with sudo
	
	sudo ./parent_program
	
	@sort -n -r -o execution_time.txt execution_time.txt 
	

	
	wait

	# Execute the Python script and display its output
	python3 scheduler_benchmark.py

clean:
	rm -f $(EXES)
