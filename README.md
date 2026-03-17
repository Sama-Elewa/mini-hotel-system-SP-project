### Ignoring local changes to input/output files

The repo includes empty `input.txt` and `output.txt` files so that using `freopen` works to make it easier to test your code. 
By default, Git will track changes to these files. 
If you don’t want Git to show them as modified every time you run the program, you can tell Git to ignore local edits by running:

```bash
git update-index --skip-worktree input.txt
git update-index --skip-worktree output.txt
```

### First time building the project

 You will likely face issues because of IntelliSense. 
 So for your first time building the `main.cpp` file, open a new terminal in your project and run this:

 ```bash
g++ main.cpp -o main -std=c++17 -pthread -I. -lws2_32 -lmswsock
```
