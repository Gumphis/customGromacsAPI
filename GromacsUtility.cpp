#include "GromacsUtility.h"
#include <cstring>

tsResult GetWord(char* word, FILE* file, char* phrases, char* skipPhrases, char* overlookPhrases, int length)
{
  int index = 0;
  int lindex = 0;
  char c = 0;
  bool readonly = false;
  
  int phlen = strlen(phrases);
  int sklen = strlen(skipPhrases);
  int ollen = strlen(overlookPhrases);
  
  if(!word) readonly = true;
  if(!file) return TS_ERROR;
  
  bool ac = false;
  if(sklen > 0)
  {
    bool ab;
    bool ab2 = false;
    do
    {
      ab = false;
      c = fgetc(file);
      lindex++;
      for(int i = 0; i < sklen; ++i) if(c == skipPhrases[i]) ab = true;
      if(length > 0 && lindex >= length) ab2 = true;
    } while(c != EOF && ab && !ab2);
    if(!ab)
    {
      bool append = true;
      for(int i = 0; i < ollen; ++i) if(c == overlookPhrases[i]) append = false;
      if(append) word[index++] = c;
    }
    for(int i = 0; i < phlen; ++i) if(c == phrases[i]) ac = true;
  }
  
  bool finished = false;
  if(c != EOF && ((length > 0 && lindex < length) || length == 0))
  {
    if(!ac)
    {
      bool ab = false;
      do
      {
	c = fgetc(file);
	if(!readonly)
	{
	  bool append = true;
	  for(int i = 0; i < ollen; ++i) if(c == overlookPhrases[i]) append = false;
	  if(append) word[index++] = c;
	}
	lindex++;
	if(length > 0 && lindex >= length) {index++;ab = true;}
	for(int i = 0; i < phlen; ++i) if(c == phrases[i]) ab = true;
      } while(c != EOF && !ab);
    }
  }
  else finished = true;
  
  
  if(length > 0 && lindex < length) for(int i = lindex; i < length; ++i) c = fgetc(file);

  if(!readonly)
  {
      if(!finished && index > 0) word[index-1] = '\0'; else word[index] = '\0';
  }
 
  if(c == '\n') return TS_LINESKIP;
  if(c == EOF) return TS_EOF;
  return TS_OK;
}

int GetInt(FILE* file, char* phrases, char* skipPhrases, char* overlookPhrases, int length, tsResult* r)
{
  char word[256];
  if(r != NULL)
    (*r) = GetWord(word, file, phrases, skipPhrases, overlookPhrases, length);
  else
    GetWord(word, file, phrases, skipPhrases, overlookPhrases, length);
  return atoi(word);
}

double GetDouble(FILE* file, char* phrases, char* skipPhrases, char* overlookPhrases, int length, tsResult* r)
{
  char word[256];
  if(r != NULL)
    (*r) = GetWord(word, file, phrases, skipPhrases, overlookPhrases, length);
  else
    GetWord(word, file, phrases, skipPhrases, overlookPhrases, length);
  return atof(word);  
}

float GetFloat(FILE* file, char* phrases, char* skipPhrases, char* overlookPhrases, int length, tsResult* r)
{
  char word[256];
  if(r != NULL)
    (*r) = GetWord(word, file, phrases, skipPhrases, overlookPhrases, length);
  else
    GetWord(word, file, phrases, skipPhrases, overlookPhrases, length);
  return (float)(atof(word));
}

char GetCharacter(FILE* file) {return fgetc(file);}

tsResult GetWord(char* word, const char* inWord, int* iStart, char* phrases, char* skipPhrases, char* overlookPhrases, int length)
{
  int inWordCounter=0;
  int index = 0;
  int lindex = 0;
  char c = 0;
  bool readonly = false;
  
  int phlen = strlen(phrases);
  int sklen = strlen(skipPhrases);
  int ollen = strlen(overlookPhrases);
  
  if(!word) readonly = true;
  if(!inWord) return TS_ERROR;
  if(iStart) inWordCounter = *iStart;
  
  bool ac = false;
  if(sklen > 0)
  {
    bool ab;
    bool ab2 = false;
    do
    {
      ab = false;
      c = inWord[inWordCounter++];
      lindex++;
      for(int i = 0; i < sklen; ++i) if(c == skipPhrases[i]) ab = true;
      if(length > 0 && lindex >= length) ab2 = true;
    } while(c != '\0' && ab && !ab2);
    if(!ab)
    {
      bool append = true;
      for(int i = 0; i < ollen; ++i) if(c == overlookPhrases[i]) append = false;
      if(append) word[index++] = c;
    }
    for(int i = 0; i < phlen; ++i) if(c == phrases[i]) ac = true;
  }
  
  bool finished = false;
  if(c != '\0' && ((length > 0 && lindex < length) || length == 0))
  {
    if(!ac)
    {
      bool ab = false;
      do
      {
	c = inWord[inWordCounter++];
	if(!readonly)
	{
	  bool append = true;
	  for(int i = 0; i < ollen; ++i) if(c == overlookPhrases[i]) append = false;
	  if(append) word[index++] = c;
	}
	lindex++;
	if(length > 0 && lindex >= length) {index++;ab = true;}
	for(int i = 0; i < phlen; ++i) if(c == phrases[i]) ab = true;
      } while(c != '\0' && !ab);
    }
  }
  else finished = true;
  
  
  if(length > 0 && lindex < length) for(int i = lindex; i < length; ++i) c = inWord[inWordCounter++];

  if(!readonly)
  {
    if(!finished && index > 0) word[index-1] = '\0'; else word[index] = '\0';
  }
 
  if(iStart) *iStart=inWordCounter;
  if(c == '\n') return TS_LINESKIP;
  if(c == '\0') return TS_EOF;
  return TS_OK;
}

int GetInt(const char* inWord, int* iStart, char* phrases, char* skipPhrases, char* overlookPhrases, int length, tsResult* r)
{
  char word[256];
  if(r != NULL)
    (*r) = GetWord(word, inWord, iStart, phrases, skipPhrases, overlookPhrases, length);
  else
    GetWord(word, inWord, iStart, phrases, skipPhrases, overlookPhrases, length);
  return atoi(word);
}

double GetDouble(const char* inWord, int* iStart, char* phrases, char* skipPhrases, char* overlookPhrases, int length, tsResult* r)
{
  char word[256];
  if(r != NULL)
    (*r) = GetWord(word, inWord, iStart, phrases, skipPhrases, overlookPhrases, length);
  else
    GetWord(word, inWord, iStart, phrases, skipPhrases, overlookPhrases, length);
  return atof(word);  
}

float GetFloat(const char* inWord, int* iStart, char* phrases, char* skipPhrases, char* overlookPhrases, int length, tsResult* r)
{
  char word[256];
  if(r != NULL)
    (*r) = GetWord(word, inWord, iStart, phrases, skipPhrases, overlookPhrases, length);
  else
    GetWord(word, inWord, iStart, phrases, skipPhrases, overlookPhrases, length);
  return (float)(atof(word));
}

char GetCharacter(const char* inWord, int* iStart) {return inWord[*iStart++];}