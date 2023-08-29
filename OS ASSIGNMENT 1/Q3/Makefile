# The name of the script
SCRIPT = calculator.sh

# The default target
all: $(SCRIPT)

# Make the script executable
$(SCRIPT):
	chmod +x $(SCRIPT)

# Run the script
run: $(SCRIPT)
	./$(SCRIPT)

# Clean up generated files
clean:
	rm -rf Result
