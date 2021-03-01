a. Create a .sh360rc file. The first line store the prompt, and the rest of lines store each path per line.

b. First open .sh360rc file to get the prompt and path, then reading input from user and store into a char* array. Extract the argument from user to find if there are binary files exist.

c. When user enters "OR" with space, program will eliminate the "OR" with the space and go into function "or_funct" for further proccess. If user does not follow the format, the program will print out message to indicate the user to try again. e.g. missing "->" character... Finally within a new child open a file that writes in all the data from stdout.

d. When user enter "PP" with space, program will eliminate the "OR" with the space and go into function "pp_funct" for further proccess. User enter one or two "->" will function in different way. For two "->", the program would create two pipes to make a connection and redirect the data among three children.