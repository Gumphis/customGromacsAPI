/* 
 * File:   tsUtility.h
 * Author: kulkem
 *
 * Created on February 9, 2017, 2:35 PM
 */

#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>

#define TS_SAFE_DELETE(x) if((x)) {delete (x); (x) = NULL;}
#define TS_SAFE_DELETE_ARRAY(x) if((x)) {delete[] (x); (x) = NULL;}

namespace gmx
{

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

tsResult GetWord(char* word, FILE* file, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0);
double GetDouble(FILE* file, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0, tsResult* r = NULL);
float GetFloat(FILE* file, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0, tsResult* r = NULL);
int GetInt(FILE* file, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0, tsResult* r = NULL);
char GetCharacter(FILE* file);

tsResult SkipLine(FILE* file);
tsResult GetWordWithLength(char* word, int number, FILE* file);
tsResult GetLine(char* word, FILE* file);
tsResult SkipCharacters(int number, FILE* file);
float GetFloatWithLength(int number, FILE* file);
int GetIntWithLength(int number, FILE* file);
double GetDoubleWithLength(int number, FILE* file);

tsResult GetWord(char* word, const char* inWord, int* iStart = NULL, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0);
double GetDouble(const char* inWord, int* iStart = NULL, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0, tsResult* r = NULL);
float GetFloat(const char* inWord, int* iStart = NULL, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0, tsResult* r = NULL);
int GetInt(const char* inWord, int* iStart = NULL, char* phrases = " \n\t", char* skipPhrases = " \n\t", char* overlookPhrases = "", int length = 0, tsResult* r = NULL);
char GetCharacter(const char* inWord, int* iStart = NULL);

tsResult GetWordWithLength(char* word, int number, const char* inWord, int* iStart = NULL);
tsResult SkipCharacters(int number, const char* inWord, int* iStart = NULL);
float GetFloatWithLength(int number, const char* inWord, int* iStart = NULL);
int GetIntWithLength(int number, const char* inWord, int* iStart = NULL);
double GetDoubleWithLength(int number, const char* inWord, int* iStart = NULL);

} //gmx namespace
