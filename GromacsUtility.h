/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GromacsUtility.h
 * Author: kulkem
 *
 * Created on February 9, 2017, 2:35 PM
 */

#ifndef GROMACSUTILITY_H
#define GROMACSUTILITY_H

#include <cstdio>
#include <cstdlib>
#include <iostream>

enum tsResult
{
  TS_OK,
  TS_WARNING,
  TS_ERROR,
  TS_BADHEADER,
  TS_BADFORMAT,
  TS_CHECKSUMERROR,
  TS_EOF,
  TS_NOT_FOUND,
  TS_INCONSISTENCY,
  TS_LINESKIP
};

#define SAFE_DELETE_ARRAY(x) if((x)) {delete[] (x); (x) = NULL;}

tsResult GetWord(char* word, FILE* file, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0);
double GetDouble(FILE* file, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0, tsResult* r = NULL);
float GetFloat(FILE* file, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0, tsResult* r = NULL);
int GetInt(FILE* file, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0, tsResult* r = NULL);
char GetCharacter(FILE* file);

inline tsResult SkipLine(FILE* file) { return GetWord(NULL, file, "\n", "", "", 0); }
inline tsResult GetWordWithLength(char* word, int number, FILE* file) { return GetWord(word, file, " \n\t", " \n\t", "", number); }
inline tsResult GetLine(char* word, FILE* file) { return GetWord(word, file, "\n", "", "", 0); }
inline tsResult SkipCharacters(int number, FILE* file) { return GetWord(NULL, file, "", "", "", number); }
inline float GetFloatWithLength(int number, FILE* file) { return GetFloat(file, " \n\t", " \n\t", "", number); }
inline int GetIntWithLength(int number, FILE* file) { return GetInt(file, " \n\t", " \n\t", "", number); }
inline double GetDoubleWithLength(int number, FILE* file) { return GetDouble(file, " \n\t", " \n\t", "", number); }

tsResult GetWord(char* word, const char* inWord, int* iStart = NULL, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0);
double GetDouble(const char* inWord, int* iStart = NULL, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0, tsResult* r = NULL);
float GetFloat(const char* inWord, int* iStart = NULL, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0, tsResult* r = NULL);
int GetInt(const char* inWord, int* iStart = NULL, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0, tsResult* r = NULL);
char GetCharacter(const char* inWord, int* iStart = NULL);

inline tsResult GetWordWithLength(char* word, int number, const char* inWord, int* iStart = NULL) { return GetWord(word, inWord, iStart, " \n\t", " \n\t", "", number); }
inline tsResult SkipCharacters(int number, const char* inWord, int* iStart = NULL) { return GetWord(NULL, inWord, iStart, "", "", "", number); }
inline float GetFloatWithLength(int number, const char* inWord, int* iStart = NULL) { return GetFloat(inWord, iStart, " \n\t", " \n\t", "", number); }
inline int GetIntWithLength(int number, const char* inWord, int* iStart = NULL) { return GetInt(inWord, iStart, " \n\t", " \n\t", "", number); }
inline double GetDoubleWithLength(int number, const char* inWord, int* iStart = NULL) { return GetDouble(inWord, iStart, " \n\t", " \n\t", "", number); }

#endif /* GROMACSUTILITY_H */

