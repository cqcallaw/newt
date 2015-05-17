TEST_PATH = ../tests/

#ref: http://stackoverflow.com/a/2706067/577298
TEST_FILES = $(wildcard $(TEST_PATH)*.nwt)
TESTS = $(patsubst $(TEST_PATH)%.nwt,%,$(TEST_FILES))

test: newt $(TESTS)

$(TEST_PATH)output:
	mkdir -p $(TEST_PATH)output

%: $(TEST_PATH)%.nwt $(TEST_PATH)output
	-@echo ' '
	./newt $< >$(TEST_PATH)output/$@ 2>&1
	diff $(TEST_PATH)reference/$@ $(TEST_PATH)output/$@

test-clean:
	rm -rf $(TEST_PATH)output/*
