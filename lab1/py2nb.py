#! /usr/bin/python3
import os
for py in os.listdir("py"):
    if py.endswith(".py"):
        nb = py.replace(".py", ".ipynb")
        print(f"{py} to {nb}")
        os.system(f"ipynb-py-convert py/{py} nb/{nb}")
