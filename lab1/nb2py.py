#! /usr/bin/python3
import os
for nb in os.listdir("nb"):
    if nb.endswith(".ipynb"):
        py = nb.replace(".ipynb", ".py")
        print(f"{nb} ------ to ------ {py}")
        os.system(f"ipynb-py-convert nb/{nb} py/{py}")
