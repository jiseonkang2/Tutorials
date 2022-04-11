#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

typedef struct _CPOINT
{
	GLfloat pos[3];
	struct _CPOINT* next;
} CPOINT;

typedef struct _CPLIST
{
	int count;
	CPOINT* head;
} CPLIST;

void		CP_init(CPLIST* list);
bool		CP_insert(CPLIST* list, GLfloat* pos, int index);
bool		CP_delete(CPLIST* list, int index);
void		CP_deleteall(CPLIST* list);
int			CP_search(CPLIST* list, GLfloat* pos, GLfloat e);
GLfloat*	CP_get(CPLIST* list, int index);
void		CP_getArray(CPLIST* list, GLfloat a[30][3]);
void		CP_modify(CPLIST* list, GLfloat* pos, int index);
void		CP_print(CPLIST* list);
