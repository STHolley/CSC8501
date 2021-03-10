Title of work: 
 CSC8501 Maze Generation
 Creation period:
 September 2020-October 2020 (4 weeks total)

-------------------------------------------------- ---------------------------
 Please fill in the development environment of the program work
 What was the development environment used for your program?
-------------------------------------------------- ---------------------------
-OS name and version that have been confirmed to work
  Used OS
  　Windows 10 64 bit

-Version of the development environment used for development
  Used Development Environment
  　Microsoft Visual Studio Community 2019 Version 16.7.6

-------------------------------------------------- ---------------------------
 Why did you decide to make that program?
 Why did you create this program?
-------------------------------------------------- ---------------------------
This project was developed as a solution for the module CSC8501 for Newcastle University. 

-------------------------------------------------- ---------------------------
 What did you pay attention to when creating the program?
 What were you careful about?
-------------------------------------------------- ---------------------------
The main focus on the program was execution speed/time and memory management. 
The source code relies on the ommitance of IF statements to reduce overall execution time with regards to machine code compilation and execution around the use of IF statements.

-------------------------------------------------- ---------------------------
 What was the difficulty in creating the program?
 What did you find difficult?
-------------------------------------------------- ---------------------------
Since the solution required a maze to be generated, with multiple 'enterances' and 'exits', I needed to adapt Prim's Algorithm (Maze generation) to account for this.
To do so, I used a 'boring' technique to break down walls where a solution was not found, to make way for a possible solution. This solution provides a relatively cost efficient method for opening and expanding a maze to give multiple exits, without detracting anything from the maze quality provided by Prim's Algorithm.
