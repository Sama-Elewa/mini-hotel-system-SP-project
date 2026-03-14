### Ignoring local changes to input/output files

The repo includes empty `input.txt` and `output.txt` files so that using `freopen` works to make it easier to test your code. 
By default, Git will track changes to these files. 
If you don’t want Git to show them as modified every time you run the program, you can tell Git to ignore local edits by running:

```bash
git update-index --skip-worktree input.txt
git update-index --skip-worktree output.txt
