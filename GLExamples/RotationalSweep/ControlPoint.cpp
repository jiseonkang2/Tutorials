#include "ControlPoint.h"

// 연결리스트 초기화
void CP_init(CPLIST* list)
{
	list->count = 0;
	list->head = NULL;
}

// index에 노드 추가
bool CP_insert(CPLIST* list, GLfloat* pos, int index)
{
	if (index < 1 || index >(list->count) + 1) {
		//printf("ERR: CP_insert\n");
		return false;
	}

	CPOINT* new_cp = (CPOINT*)malloc(sizeof(CPOINT));
	new_cp->pos[0] = pos[0];
	new_cp->pos[1] = pos[1];
	new_cp->pos[2] = pos[2];

	if (index == 1) {
		new_cp->next = list->head;
		list->head = new_cp;
	}
	else {
		CPOINT* temp = list->head;
		for (int i = 1; i < index - 1; i++)
			temp = temp->next;
		new_cp->next = temp->next;
		temp->next = new_cp;
	}
	list->count++;

	return true;
}

// index의 노드 삭제
bool CP_delete(CPLIST* list, int index)
{
	if (index < 1 || index >(list->count)) {
		//printf("ERR: CP_delete\n");
		return false;
	}

	CPOINT* temp = list->head;

	if (index == 1) {
		list->head = temp->next;
		free(temp);
	}
	else {
		for (int i = 1; i < index - 1; i++)
			temp = temp->next;
		CPOINT* temp2 = temp->next;
		temp->next = temp2->next;
		free(temp2);
	}
	list->count--;

	return true;
}

void CP_deleteall(CPLIST* list)
{
	if (list->count == 0)
		return;

	CPOINT* temp = list->head;
	CPOINT* temp2;
	for (int i = 1; i < list->count; i++) {
		temp2 = temp->next;
		free(temp);
		temp = temp2;
	}

	CP_init(list);
}

// pos와 동일한 값 찾기
int CP_search(CPLIST* list, GLfloat* pos, GLfloat e)
{
	CPOINT* temp = list->head;
	int i = 1;

	while (temp != NULL) {
		if (pos[0] >= temp->pos[0] - e && pos[0] <= temp->pos[0] + e &&
			pos[1] >= temp->pos[1] - e && pos[1] <= temp->pos[1] + e &&
			pos[2] >= temp->pos[2] - e && pos[2] <= temp->pos[2] + e)
			break;
		i++;
		temp = temp->next;
	}

	if (i > list->count)
		return 0;
	else
		return i;
}

GLfloat* CP_get(CPLIST* list, int index)
{
	if (index < 1 || index >(list->count)) {
		//printf("ERR: CP_get\n");
		return NULL;
	}

	CPOINT* temp = list->head;

	for (int i = 1; i < index; i++)
		temp = temp->next;

	return temp->pos;
}

void CP_getArray(CPLIST* list, GLfloat a[30][3])
{
	int i;
	for (i = 0; i < list->count; i++)
	{
		float* p = CP_get(list, i + 1);
		a[i][0] = p[0];
		a[i][1] = p[1];
		a[i][2] = p[2];
	}

	for (int k = 0; k < 3; k++, i++)
	{
		float* p = CP_get(list, k + 1);
		a[i][0] = p[0];
		a[i][1] = p[1];
		a[i][2] = p[2];
	}
}

// index의 노드 값(pos) 수정
void CP_modify(CPLIST* list, GLfloat* pos, int index)
{
	if (index < 1 || index >(list->count)) {
		printf("ERR: CP_modify\n");
		return;
	}

	CPOINT* temp = list->head;

	for (int i = 1; i < index; i++)
		temp = temp->next;

	temp->pos[0] = pos[0];
	temp->pos[1] = pos[1];
	temp->pos[2] = pos[2];
}

void CP_print(CPLIST* list)
{
	CPOINT* temp = list->head;

	if (list->count == 0)
		return;

	printf("CP(%d): ", list->count);
	while (temp != NULL) {
		printf("(%.2f,%.2f,%.2f) ", temp->pos[0], temp->pos[1], temp->pos[2]);
		temp = temp->next;
	}
	printf("\n");
}