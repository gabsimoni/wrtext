# WRText Coding Style Rules

## Language
This project uses C99. Do not use features that are not present in this version of the language.

## Tabs and indenting
Always use tabs to indent, never spaces. 
A tab is four spaces long. 

## Column limit
The project uses a limit of 99 columns for readability.

## Identifiers names
Use `snake_case` for variables and function names. 
Use `UPPER_CASE` for macros.

Shorten variable names for local variables in a small scopes.

## Header includes
Include system headers after user headers.

## .clang-format
The rest of the coding style is defined in `.clang-format`, most of it is based on the GNU style.
You can see what the style consists of by researching online or looking at the example snippet below

## Checking the style
You can check if the source files follow the coding style by running `make check-style` in the root directory.
If the code does not comply with the coding style it will be highlighted by errors or warnings in the terminal.

## Auto-formatting
You can automatically format your code to follow the coding style by running `make auto-style` in the root directory. This will automatically edit files to make them comply with the coding style.

## Example
	#include "something.h"
  
	#include <stdio.h>
	  
	/*
		Returns the number of occurrences of the argument character in the argument string
	*/
	int
	count_occurences(char test, char *str)
	{
		int count = 0;
		  
		for(char *c = str; *c != '\0'; c++) {
			if(*c == test)
				count++;
		}

		return count;
	}

	/*
		Main function that uses the count_occurrences function with some hard-coded values
		and prints its result
	*/
	int
	main()
	{
		char *str = "Tomato";
		char target = 'o';

		printf("There are %d \'%c\' in the word \"%s\".\n", count_occurences(target, str), target,
		str);

		  

		return 0;
	}