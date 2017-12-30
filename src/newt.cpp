/*
 Copyright (C) 2015 The newt Authors.

 This file is part of newt.

 newt is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 newt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with newt.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <string.h>
#include <memory>
#include <fstream>

#include "builtins.h"

#include "driver.h"

using namespace std;

void print_help() {
	cout << "Usage: newt [options] file" << endl;
	cout << "Options:" << endl;
	cout << "  --help           : Display this information" << endl;
	cout
			<< "  --include-paths  : Specify a pipe-separated list of include paths"
			<< endl;
	cout << "Debug Options:" << endl;
	cout
			<< "  --debug          : Print debug information during script execution"
			<< endl;
	cout
			<< "  --trace-scanning : Print scanning information during script execution"
			<< endl;
	cout
			<< "  --trace-parsing  : Print parsing information during script execution"
			<< endl;
	cout
			<< "  --trace-import   : Print import information during script execution"
			<< endl;
}

int get_exit_code(bool debug, int exit_code) {
	if (debug) {
		//return "success" so the test infrastructure doesn't barf
		return EXIT_SUCCESS;
	} else {
		return exit_code;
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cerr << "Input script must be specified." << endl;
		return EXIT_FAILURE;
	}

	if (strcmp(argv[1], "--help") == 0) {
		print_help();
		return EXIT_SUCCESS;
	}

	bool debug = false;
	TRACE trace = NO_TRACE;
	volatile_shared_ptr<string_list> import_paths = make_shared<string_list>();
	import_paths->push_back(make_shared<const string>("."));
	int i = 1;
	for (; i < argc - 1; i++) {
		if (strcmp(argv[i], "--debug") == 0) {
			debug = true;
		}

		if (strcmp(argv[i], "--trace-scanning") == 0) {
			trace = TRACE(trace | SCANNING);
		}

		if (strcmp(argv[i], "--trace-parsing") == 0) {
			trace = TRACE(trace | PARSING);
		}

		if (strcmp(argv[i], "--trace-import") == 0) {
			trace = TRACE(trace | IMPORT);
		}

		if (strcmp(argv[i], "--help") == 0) {
			print_help();
			return 0;
		}
		if (strcmp(argv[i], "--include-paths") == 0) {
			auto as_string = string(argv[++i]);
			auto commandline_include_paths = Unique(Tokenize(as_string, "|"));
			import_paths->insert(import_paths->end(),
					commandline_include_paths->begin(),
					commandline_include_paths->end());
		}
	}

	if (i >= argc) {
		cerr << "Input script must be specified." << endl;
		return EXIT_FAILURE;
	}

#ifndef NEWT_DEFAULT_IMPORT_PATH
#define NEWT_DEFAULT_IMPORT_PATH /usr/local/lib/newt
#endif
// ref: https://stackoverflow.com/a/5256500/577298
#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)
	import_paths->push_back(
			make_shared<const string>(STRINGIZE(NEWT_DEFAULT_IMPORT_PATH)));

	if ((trace & IMPORT) == IMPORT) {
		for (auto & import_path : *import_paths) {
			cout << "Import path: " << *import_path << endl;
		}
	}

	auto filename = make_shared<string>(argv[argc - 1]);
	if (debug) {
		cout << "Parsing file " << *filename << "..." << endl;
	}

	Driver driver(import_paths, trace);
	int builtin_parse_result = driver.parse_string(
			*Builtins::get_builtin_definition());
	if (builtin_parse_result == 0) {
		auto builtin_statements = driver.GetStatementBlock();
		auto builtin_context = make_shared<ExecutionContext>(
				Modifier::Type::MUTABLE);
		auto builtin_preprocess_result = builtin_statements->Preprocess(
				builtin_context, const_shared_ptr<TypeSpecifier>());

		auto builtin_errors = builtin_preprocess_result.GetErrors();
		if (ErrorList::IsTerminator(builtin_errors)) {
			auto builtin_execute_result = builtin_statements->Execute(
					builtin_context);

			builtin_errors = builtin_execute_result.GetErrors();
			if (ErrorList::IsTerminator(builtin_errors)) {
				int parse_result = driver.parse(filename);
				if (parse_result != 0 || driver.GetErrorCount() != 0) {
					if (debug) {
						cout << "Parsed file " << *filename << "." << endl;
					}

					cerr << driver.GetErrorCount() << " error";
					if (driver.GetErrorCount() > 1)
						cout << "s";
					cout << " found; giving up." << endl;

					return get_exit_code(debug, EXIT_FAILURE);
				}

				int exit_code = EXIT_SUCCESS;

				if (parse_result == 0) {
					auto main_statement_block = driver.GetStatementBlock();
					//auto builtin_context = Builtins::GetBuiltinContext();
					auto root_context = make_shared<ExecutionContext>(
							Modifier::Type::MUTABLE, LifeTime::ROOT);
					root_context->LinkToParent(builtin_context);
					auto semantic_errors =
							main_statement_block->Preprocess(root_context,
									TypeTable::GetNilTypeSpecifier()).GetErrors();

					if (ErrorList::IsTerminator(semantic_errors)) {
						if (debug) {
							cout << "Parsed file " << *filename << "." << endl;
						}

						auto execution_result = main_statement_block->Execute(
								root_context);
						auto execution_errors = execution_result.GetErrors();

						bool has_execution_errors = false;
						while (!ErrorList::IsTerminator(execution_errors)) {
							has_execution_errors = true;
							cerr << execution_errors->GetData()->ToString()
									<< endl;
							execution_errors = execution_errors->GetNext();
						}

						if (debug) {
							cout << "Root Symbol Table:" << endl;
							cout << "----------------" << endl;
							root_context->print(cout,
									*root_context->GetTypeTable(), Indent(0));
							cout << endl;
							cout << "Root Type Table:" << endl;
							cout << "----------------" << endl;
							root_context->GetTypeTable()->print(cout,
									Indent(0));
						}

						auto execution_exit_code =
								execution_result.GetExitCode();
						if (execution_exit_code
								!= ExecutionResult::GetDefaultExitCode()) {
							exit_code = *execution_exit_code;
						}

						// cleanup any open file handles
						auto file_handle_map = Builtins::get_file_handle_map();
						for (file_handle_map::iterator it =
								file_handle_map->begin();
								it != file_handle_map->end(); ++it) {
							auto stream = it->second;
							stream->close();
						}

						return get_exit_code(debug,
								has_execution_errors ? EXIT_FAILURE : exit_code);
					} else {
						//reverse linked list of errors, which comes to us in reverse order
						semantic_errors = ErrorList::Reverse(semantic_errors);

						int semantic_error_count = 0;
						ErrorListRef error = semantic_errors;
						while (!ErrorList::IsTerminator(error)) {
							semantic_error_count++;
							cerr << *(error->GetData()) << endl;
							error = error->GetNext();
						}

						if (debug) {
							cout << "Parsed file " << *filename << "." << endl;
						}

						if (semantic_error_count == 1) {
							cout << "1 error found; giving up." << endl;
						} else {
							cout << semantic_error_count
									<< " errors found; giving up." << endl;
						}

						return get_exit_code(debug, EXIT_FAILURE);
					}
				}
			}
		}

		if (!ErrorList::IsTerminator(builtin_errors)) {
			while (!ErrorList::IsTerminator(builtin_errors)) {
				cerr << builtin_errors->GetData()->ToString() << endl;
				builtin_errors = builtin_errors->GetNext();
			}

			return get_exit_code(debug, EXIT_FAILURE);
		}
	}
}
