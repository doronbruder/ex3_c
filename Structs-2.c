#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "RBTree.h"
#include "Structs.h"


/**
 * check
 * @param str
 * @return
 */
unsigned int stringLength(const char * str)
{
    unsigned int count = 0;
    while ((char) str[count] != '\0')
    {
        count++;
    }
    return count;
}


/**
 * CompFunc for strings (assumes strings end with "\0")
 * @param a - char* pointer
 * @param b - char* pointer
 * @return equal to 0 iff a == b. lower than 0 if a < b. Greater than 0 iff b < a. (lexicographic
 * order)
 */
int stringCompare(const void *a, const void *b)
{
    return (a == NULL || b == NULL) ? false : strcmp(a,b);
}


/**
 * ForEach function that concatenates the given word and \n to pConcatenated. pConcatenated is
 * already allocated with enough space.
 * @param word - char* to add to pConcatenated
 * @param pConcatenated - char*
 * @return 0 on failure, other on success
 */
int concatenate(const void *word, void *pConcatenated)
{
    if (word == NULL || pConcatenated == NULL)
    {
        return false;
    }
    char * concatenated = (char *) pConcatenated;
    const char * strToAdd = (char *) word;
    unsigned int concatenatedLen = 0, strToAddLen = 0;
    while (concatenated[concatenatedLen] != '\0')
    {
        concatenatedLen++;
    }
    while (strToAdd[strToAddLen] != '\0')
    {
        concatenated[concatenatedLen + strToAddLen] = strToAdd[strToAddLen];
        strToAddLen++;
    }
    return true;
}


/**
 * FreeFunc for strings
 */
void freeString(void *s)
{
    free(s);
}


double compareVectors(const Vector * v1, const Vector * v2)
{
    unsigned int minimalLength = v1->len > v2->len ? v2->len : v1->len;
    for (unsigned int i = 0; i < minimalLength; ++i)
    {
        if (v1->vector[i] != v2->vector[i])
        {
            return (v1->vector[i] - v2->vector[i]);
        }
    }
    return v1->len - v2->len;
}



/**
 * CompFunc for Vectors, compares element by element, the vector that has the first larger
 * element is considered larger. If vectors are of different lengths and identify for the length
 * of the shorter vector, the shorter vector is considered smaller.
 * @param a - first vector
 * @param b - second vector
 * @return equal to 0 iff a == b. lower than 0 if a < b. Greater than 0 iff b < a.
 */
int vectorCompare1By1(const void *a, const void *b)
{
    double result = compareVectors((Vector *) a, (Vector *) b);
    if (-1 < result && result < 1)
    {
        return result > 0 ? 1 : (result < 0 ? -1 : 0);
    }
    return (int) result;
}


/**
 * FreeFunc for vectors
 */
void freeVector(void *pVector)
{
    free(((Vector *) pVector)->vector);
    free(pVector);
}


long double normCalculator(Vector * v)
{
    long double sum = 0;
    for (int i = 0; i < v->len; ++i)
    {
        sum += v->vector[i] * v->vector[i];
    }
    return sum;
}


void deepcopyVector(const Vector *pVector, Vector *pMaxVector)
{
    pMaxVector->len = pVector->len;
    free(pMaxVector->vector);
    pMaxVector->vector = realloc(pVector->vector, pVector->len * sizeof(double));
}


/**
 * copy pVector to pMaxVector if : 1. The norm of pVector is greater then the norm of pMaxVector.
 * 								   2. pMaxVector->vector == NULL.
 * @param pVector pointer to Vector
 * @param pMaxVector pointer to Vector that will hold a copy of the data of pVector.
 * @return 1 on success, 0 on failure (if pVector == NULL || pMaxVector==NULL: failure).
 */
int copyIfNormIsLarger(const void *pVector, void *pMaxVector)
{
    if (pVector == NULL || pMaxVector==NULL)
    {
        return false;
    }
    long double pVectorNorm = normCalculator((Vector *) pVector);
    long double pMaxVectorNorm = normCalculator((Vector *) pMaxVector);
    if (pVectorNorm > pMaxVectorNorm)
    {
        deepcopyVector(pVector, pMaxVector);
    }
    return true;
}


/**
 * This function allocates memory it does not free.
 * @param tree a pointer to a tree of Vectors
 * @return pointer to a *copy* of the vector that has the largest norm (L2 Norm), NULL on failure.
 */
Vector * findMaxNormVectorInTree(RBTree *tree)
{
    if (tree == NULL || tree->root == NULL)
    {
        return NULL;
    }
    Vector *resVector = NULL;
    resVector = (Vector *) malloc(sizeof(Vector));
    if (resVector == NULL)
    {
        return NULL;
    }
    resVector->len = 0;
    forEachRBTree(tree, copyIfNormIsLarger, resVector);
    return resVector;
}
