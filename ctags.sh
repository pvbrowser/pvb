#!/bin/bash
ctags -R --language-force=c++ --exclude=language_bindings --exclude=processviewserver_string.h
echo generated:
ls -al tags
