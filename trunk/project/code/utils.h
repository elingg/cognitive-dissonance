/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    utils.h
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
** DESCRIPTION:
**  Useful utility functions.
*****************************************************************************/

#pragma once

#include <vector>
#include <sstream>

// toString
// convert a basic data type to a string
template<class T>
std::string toString(const T& v)
{
    std::stringstream s;
    s << v;
    return s.str();
}

// getTrainingFiles
// Given a directory, looks for all *.jpg files within one directory level
// below the root and labels each with the name of the subdirectory, e.g.
// all files in data/mug/*.jpg would be labeled as "mug" given the root
// directory "data".
//
typedef struct _TTrainingFile {
    std::string filename;
    std::string label;
} TTrainingFile;

typedef struct _TTrainingFileList {
    std::vector<TTrainingFile> files;
    std::vector<std::string> classes;
} TTrainingFileList;
    
TTrainingFileList getTrainingFiles(const char *root, const char *ext);

// showProgress
// Shows % complete on standard output
void showProgress(int index, int maxIndex);
