// list.h

#include <stdio.h>

#define ListPush(List, Count, Element) {\
  if (List) {\
    List = realloc(List, sizeof(Element) * (Count + 1));\
    Assert(List);\
    ++Count;\
  }\
  else { \
    List = malloc(sizeof(Element) * 1);\
    Count = 1;\
  }\
  List[Count - 1] = Element;\
}

#define ListFree(List, Count) {\
  if (List) {\
    free(List);\
    Count = 0;\
  }\
}

#define BufferPush(Buffer, Count, Element) ListPush(Buffer, Count, Element)
#define BufferFree(Buffer, Count) ListFree(Buffer, Count)
