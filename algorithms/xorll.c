#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
 
struct _xorll {
	void* value;
	struct _xorll* np;
};
typedef struct _xorll xorll;
 
typedef void (*xorll_callback)(const xorll*, const xorll*);
 
xorll* new_node(xorll* prev, xorll* cur, void* value) {
	xorll* next = (xorll*) malloc(sizeof(xorll));
 
	next->value = value;
	next->np = cur;
 
	if (cur) {
		cur->np = prev == cur ? cur : (xorll*)((uintptr_t)prev ^ (uintptr_t)next);
	}
	return next;
}
 
void traverse(xorll* start, xorll_callback callback) {
	xorll* save, *cur = start, *prev = start;
 
	while (cur) {
		callback(prev, cur);
 
		if (cur->np == cur) {
			break;
		}
		save = cur;
		cur = cur == prev ? cur->np : (xorll*)((uintptr_t)prev ^ (uintptr_t)cur->np);
		prev = save;
	}
}
 
void xorll_string_printer(const xorll* prev, const xorll* cur) {
	printf("- %s\n", (char*)cur->value);
	printf("(prev=%p cur=%p next=%p)\n", prev, cur, (xorll*)((uintptr_t)prev ^ (uintptr_t)cur->np));
}
 
void freellist(xorll* start) {
	xorll* save, *cur = start, *prev = start;
 
	while (cur) {
		if (cur->np == cur) {
			free(cur);
			break;
		}
		save = cur;
		cur = cur == prev ? cur->np : (xorll*)((uintptr_t)prev ^ (uintptr_t)cur->np);
		prev = save;
		free(save);
	}
}
 
int main(int argc, char** argv) {
	xorll* end, *head;
 
	end = head = new_node(NULL, NULL, "a");
	end = new_node(end->np, end, "b");
	end = new_node(end->np, end, "c");
	end = new_node(end->np, end, "d");
 
	traverse(head, xorll_string_printer);
	freellist(head);
 
	return 0;
}
