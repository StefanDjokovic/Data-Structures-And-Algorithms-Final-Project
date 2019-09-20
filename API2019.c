#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LENGTH 50       //maximum name length of both relations and entities
#define HASH_SIZE 50000 //hashsize
#define N 37            //parameter for hashvalue
#define COMP 50         //number of bits used of long long
#define MAX_N_REP 20    //maximum number of relation type allowed (5 in the exam)
#define N_COLS 10       //number of columns for the fromIdEnt matrix
#define N_ROWS 20       //number of rows for the fromIdEnt matrix

struct Score;
struct NodeReport;
struct Report;
struct RelOut;

//structure for the entity
typedef struct Ent {
  char name[LENGTH];
  int idEnt;                               //progressive number of the entity
  struct RelOut * relOut;                  //list of all outbound relations
  unsigned long long int *** fromIdEnt;    //3D martix of all inbound relations
  struct NodeReport ** node;               //array of pointers to node of the report
  struct Ent * next;                       //next elements inside the same hash cell
} Ent;

typedef struct RelOut {
  int toIdEnt;                      //progressive number of the entity to which it is connected
  struct Report * rel;              //pointer to the report structure of that relation
  struct RelOut * next;
} RelOut;

//the structure for report
typedef struct Report {
  char relName[LENGTH];
  int idRel;                        //progressive number of the relation
  struct Score * zero;              //keeps track of the relations that were eliminated
  struct Score * one;               //pointer to the first cell of Score of that relationship
  struct Score * highest;           //pointer to the Score list of elements with maximum number of inbounds
  struct Report * next;
} Report;

typedef struct Score {
  int val;                          //number of inbounds for the node in the list
  struct NodeReport * list;         //list of the elements on that level
  struct Score * prev;              //lower val elements
  struct Score * next;              //higher val elements
} Score;

typedef struct NodeReport {
  int inIdEnt;                      //progressive value of the entity
  struct Score * head;              //pointer to the Score head
  struct NodeReport * prev;         //previous element of the list
  struct NodeReport * next;         //next element of the list
} NodeReport;

Ent **idEntKeeper;                  //array that returns the unique entity given its idEnt
Report * headReport;                //first element in the report list



//AddEnt function
void AddEnt(char * ent1, Ent ** entries, int *unusedIdEnt);

//AddRel funcion
void AddRel (char * ent1, char * ent2, char * rel, Ent ** entries, int * unusedIdRel);
//function that searches for that relation type, and creates it if it does not exist in report
Report * FindRelAndAdd(char * rel, int * unusedIdRel);
//first time that element recieves that relation
NodeReport * AddNodeReport(Report * rep, Ent * ent2);
//moves the relationship to a higher Score
void IncreaseScore(Report * rep, NodeReport * node);

//DelRel function
void DelRel(char * ent1, char * ent2, char * rel, Ent ** entries);
//decreasing the position of an element in a report
void DecreaseScore(Report * currRep, NodeReport * node);

//deletes an entity and its relations
void DelEnt(char * ent1, Ent ** entries);
//total deletion of an element
void DeleteNodeReport (NodeReport * node, int nRep);
//makes delrel of the outer connections
void DecreaseScoreByRelOut(RelOut * node, int ent1Id, Ent ** entries);

//prints the result
void ReportPrint();

//various self-explanatory functions
int GetHashValueEnt(char * ent1);
void AddNodeToList(NodeReport * node, NodeReport * pointerNode);
void DeleteNodeFromList(NodeReport * node);
Ent * FindEnt(char * ent1, Ent ** entries);



int main() {
  //the possible commands
  char addent[] = "addent";
  char addrel[] = "addrel";
  char report[] = "report";
  char delent[] = "delent";
  char delrel[] = "delrel";
  char end[] = "end";
  char command[7];
  char ent1[LENGTH];
  char ent2[LENGTH];
  char rel[LENGTH];

  idEntKeeper = malloc(sizeof(Ent) * HASH_SIZE);
  headReport = NULL;

  int unusedIdEnt = 0;                //current unused Id for Ent
  int unusedIdRel = 0;                //current unused Id for Rel

  Ent * entries[HASH_SIZE];
  for (int i = 0; i < HASH_SIZE; i++)
		entries[i] = NULL;


	scanf("%s", command);
  while (strcmp(command, end) != 0) {	//END
    if (strncmp(command, addrel, sizeof(char)*4) == 0) {	//ADDREL
      scanf("%s", ent1);
      scanf("%s", ent2);
      scanf("%s", rel);

      AddRel(ent1, ent2, rel, entries, &unusedIdRel);
    }
    else if (strncmp(command, addent, sizeof(char)*4) == 0) {	//ADDENT
      scanf("%s", ent1);

      AddEnt(ent1, entries, &unusedIdEnt);
    }
    else if (strncmp(command, delrel, sizeof(char)*4) == 0) {	//DELREL
      scanf("%s", ent1);
      scanf("%s", ent2);
      scanf("%s", rel);

      DelRel(ent1, ent2, rel, entries);
    }
    else if (strncmp(command, delent, sizeof(char)*4) == 0) {	//DELENT
      scanf("%s", ent1);

      DelEnt(ent1, entries);
    }
    else if (strncmp(command, report, sizeof(char)) == 0) {	//REPORT
      ReportPrint();
    }
    scanf("%s", command);
  }

	return 0;
}

//AddEnt function
void AddEnt(char * ent1, Ent ** entries, int *unusedIdEnt) {
  int hashvalue = GetHashValueEnt(ent1);
  Ent *pointer = entries[hashvalue];

  //placing the pointer to the right spot or returning if the entity already exists
  while (pointer != NULL && pointer->next != NULL) {
    if (strcmp(ent1, pointer->name) == 0)
      return;
    else
      pointer = pointer->next;
  }
  if (pointer != NULL && strcmp(ent1, pointer->name) == 0)
    return;

  //building the new entity
  Ent *newEnt = malloc(sizeof(Ent));
  strcpy(newEnt->name, ent1);
  newEnt->idEnt = *unusedIdEnt;
  idEntKeeper[*unusedIdEnt] = newEnt;
  (*unusedIdEnt)++;
  newEnt->next = NULL;
  newEnt->relOut = NULL;
  newEnt->fromIdEnt = malloc(MAX_N_REP * sizeof(long long int**));
  newEnt->node = malloc(MAX_N_REP * sizeof(NodeReport *));
  for (int i = 0; i < MAX_N_REP; i++) {
    newEnt->fromIdEnt[i] = NULL;
    newEnt->node[i] = NULL;
  }

  //placing the entity in the hash
  if (entries[hashvalue] == NULL)
    entries[hashvalue] = newEnt;
  else {
    pointer->next = newEnt;
  }

  return;
}

//AddRel funcion
void AddRel (char * ent1, char * ent2, char * rel, Ent ** entries, int * unusedIdRel) {
  int new = 0;            //keps track if the new inbound relation is new to ent2
  Ent * current1;
  Ent * current2;
  current1 = FindEnt(ent1, entries);
  current2 = FindEnt(ent2, entries);
  if (current1 != NULL && current2 != NULL) {
    Report * rep = FindRelAndAdd(rel, unusedIdRel);

    int repId = rep->idRel;
    int current1Id = current1->idEnt;
    unsigned long long int ** currin;

    //creating the inbound structure if it does not already exist
    if (current2->fromIdEnt[repId] == NULL) {
      currin = malloc(sizeof(unsigned long long int *)*20);
      for (int i = 0; i < N_ROWS; i++)
        currin[i] = NULL;
      current2->fromIdEnt[repId] = currin;
      new = 1;
    }
    currin = current2->fromIdEnt[repId];
    //adding raw of the matrix if it does not already exist
    if (currin[(current1Id) / (COMP*N_COLS)] == NULL) {
      unsigned long long int * arr = malloc(sizeof(unsigned long long int)* 10);
      for (int i = 0; i < N_COLS; i++)
        arr[i] = (unsigned long long int)0;
      currin[(current1Id) / (COMP * N_COLS)] = arr;
    }
    unsigned long long int * currarr = currin[(current1Id) / (COMP*N_COLS)];
    //checking if the relationship already exists, else adding it
    if ((currarr[(current1Id%(COMP*N_COLS))/COMP]>>((current1Id)%COMP))%2 == 1) {
      return;
    }
    else {
      currarr[(current1Id%(COMP*N_COLS))/COMP] += ((unsigned long long int)1 << (current1Id%COMP) );
      if (new == 1)
        current2->node[repId] = AddNodeReport(rep, current2);
      else
        IncreaseScore(rep, current2->node[repId]);

      //placing RelOut, always head insertion
      RelOut * currout = malloc(sizeof(RelOut));
      currout->toIdEnt = current2->idEnt;
      currout->rel = rep;
      currout->next = current1->relOut;
      current1->relOut = currout;
    }
  }
  return;
}


Report * FindRelAndAdd(char * rel, int * unusedIdRel) {
  Report * rep = headReport;
  Report * repPrev = rep;

  while (rep != NULL && strcmp(rep->relName, rel) < 0) {
    repPrev = rep;
    rep = rep->next;
  }

  if (rep == NULL || strcmp(rep->relName, rel) > 0) {  //if that type of relationship has never appeard before
    Report * currRep = malloc(sizeof(Report));
    strcpy(currRep->relName, rel);
    currRep->idRel = *unusedIdRel;
    currRep->zero = NULL;
    (*unusedIdRel)++;

    Score * newScore = malloc(sizeof(Score));
    newScore->val = 1;
    newScore->list = NULL;
    newScore->prev = NULL;
    newScore->next = NULL;
    currRep->one = newScore;
    currRep->highest = NULL;

    if (repPrev == NULL || repPrev == rep) {
      currRep->next = rep;
      headReport = currRep;
    }
    else {
      currRep->next = rep;
      repPrev->next = currRep;
    }
    rep = currRep;
  }

  return rep;
}

//first time that ent2 recieves that relation type
NodeReport * AddNodeReport(Report * rep, Ent * ent2) {
  NodeReport * newNode = malloc(sizeof(NodeReport));
  NodeReport * curr;
  newNode->inIdEnt = ent2->idEnt;
  newNode->head = rep->one;
  curr = rep->one->list;
  if (curr == NULL || strcmp(ent2->name, idEntKeeper[curr->inIdEnt]->name) < 0) { //insertion in head
    newNode->prev = NULL;
    newNode->next = curr;
    rep->one->list = newNode;
    if (curr != NULL)
      curr->prev = newNode;

    if (rep->highest == NULL)
        rep->highest = rep->one;

    return newNode;
  }

  while (curr->next != NULL && strcmp(ent2->name, idEntKeeper[curr->inIdEnt]->name) >= 0) {
    curr = curr->next;
  }
  if (strcmp(ent2->name, idEntKeeper[curr->inIdEnt]->name) >= 0) {
    curr->next = newNode;
    newNode->prev = curr;
    newNode->next = NULL;
  }
  else {
    curr->prev->next = newNode;
    newNode->prev = curr->prev;
    curr->prev = newNode;
    newNode->next = curr;
  }

  if (rep->highest == NULL)
    rep->highest = rep->one;

  return newNode;
}

//moves the relationship to a higher Score
void IncreaseScore(Report * rep, NodeReport * node) {
  NodeReport * pointerNode;
  Score * currScore = node->head;
  if (currScore->next == NULL) { //if there is no higher Score
    Score * newScore;
    newScore = malloc(sizeof(Score));
    newScore->val = currScore->val + 1;
    newScore->prev = currScore;
    newScore->next = NULL;
    newScore->list = NULL;
    currScore->next = newScore;
  }
  //deleting node from the current list
  DeleteNodeFromList(node);

  currScore = currScore->next;
  node->head = currScore;

  AddNodeToList(node, currScore->list);

  if (rep->highest == NULL || currScore->val > rep->highest->val)
    rep->highest = currScore;
}

//DelRel function
void DelRel(char * ent1, char * ent2, char * rel, Ent ** entries) {
  Ent * current1;
  Ent * current2;
  current1 = FindEnt(ent1, entries);
  current2 = FindEnt(ent2, entries);
  if (current1 != NULL && current2 != NULL) { //if the entities still exist
    Report * rep = headReport;
    while (rep != NULL && strcmp(rep->relName, rel) < 0) {
      rep = rep->next;
    }
    if (rep == NULL || strcmp(rep->relName, rel) > 0) {  //if that kind of relationship does not exist
      return;
    }

    int repId = rep->idRel;
    int current1Id = current1->idEnt;
    unsigned long long int ** currin;
    if (current2->fromIdEnt == NULL)
      return;
    if (current2->fromIdEnt[repId] == NULL) {
      return;
    }
    currin = current2->fromIdEnt[repId];
    if (currin[(current1Id) / (COMP*N_COLS)] == NULL) {
      return;
    }
    unsigned long long int * currarr = currin[(current1Id) / (COMP*N_COLS)];
    if ((currarr[(current1Id%(COMP*N_COLS))/COMP]>>(current1Id%COMP))%2 == 1) {
      currarr[(current1Id%(COMP*N_COLS))/COMP] -= ((unsigned long long int)1 << (current1Id%COMP));
      DecreaseScore(rep, current2->node[repId]);
      return;
    }
  }

  return;
}

//decreasing the position of an element in a report
void DecreaseScore(Report * currRep, NodeReport * node) {
  Score * currScore = node->head;
  NodeReport * pointerNode;

  //deleting the element from the current list
  DeleteNodeFromList(node);

  if (currScore->prev == NULL) { //will move it to the zero Score
    Score * zeroScore = currRep->zero;
    if (currRep->zero == NULL) { //if zero does not already exist
      zeroScore = malloc(sizeof(Score));
      zeroScore->val = 0;
      zeroScore->list = NULL;
      zeroScore->prev = NULL;
      zeroScore->next = currRep->one;
      currRep->zero = zeroScore;
    }
    node->head = currRep->zero;
    node->prev = NULL;
    node->next = zeroScore->list;
    if (zeroScore->list != NULL) {
      zeroScore->list->prev = node;
    }
    zeroScore->list = node;
  }
  else {
    currScore = currScore->prev;
    node->head = currScore;
    AddNodeToList(node, currScore->list);
  }

  if (currRep->highest != NULL && currRep->highest->list == NULL) {
    if (node->head != currRep->zero)
      currRep->highest = node->head;
    else
      currRep->highest = NULL;
  }
}

//deletes and entity and its connections
void DelEnt(char * ent1, Ent ** entries) {
  Ent * current = FindEnt(ent1, entries);
  if (current != NULL) {
    if (current->fromIdEnt != NULL) {
      for (int i = 0; i < MAX_N_REP; i++) {
        if (current->fromIdEnt[i] != NULL) {
          for (int j = 0; j < N_ROWS; j++) {
            if (current->fromIdEnt[i][j] != NULL) {
              free(current->fromIdEnt[i][j]);
              current->fromIdEnt[i][j] = NULL;
            }
          }
          free(current->fromIdEnt[i]);
          current->fromIdEnt[i] = NULL;
        }
      }
      free(current->fromIdEnt);
      current->fromIdEnt = NULL;
    }
    //deleting rel_ins
    if (current->node != NULL) {
      for (int i = 0; i < MAX_N_REP; i++) {
        if (current->node[i] != NULL) {
          DeleteNodeReport(current->node[i], i);
        }

      }
      free(current->node);
      current->node = NULL;
    }
    //deleting rel_outs
    DecreaseScoreByRelOut(current->relOut, current->idEnt, entries);
    int hashvalue = GetHashValueEnt(ent1);
    if (entries[hashvalue] == current) {
      entries[hashvalue] = current->next;
    }
    else {
      Ent * lookent = entries[hashvalue];
      while (lookent->next != current)
        lookent = lookent->next;
      lookent->next = current->next;
    }
    idEntKeeper[current->idEnt] = NULL;

    free(current);
    current = NULL;
  }
  return;
}

//total deletion of an element
void DeleteNodeReport (NodeReport * node, int nRep) {
  Report * rep;
  Score * currScore = node->head;

  DeleteNodeFromList(node);

  rep = headReport;
  while (rep->idRel != nRep)
    rep = rep->next;

  if (rep->highest != NULL && rep->highest->list == NULL)
    while (rep->highest != NULL && rep->highest->list == NULL)
      rep->highest = rep->highest->prev;

  free(node);
  node = NULL;
}

void DecreaseScoreByRelOut(RelOut * nodeRelOut, int ent1Id, Ent ** entries) {
  RelOut * tempRelOut = nodeRelOut;
  while (tempRelOut != NULL) {
    if (idEntKeeper[tempRelOut->toIdEnt] != NULL)
      if (idEntKeeper[tempRelOut->toIdEnt]->name != NULL)
        DelRel(idEntKeeper[ent1Id]->name, idEntKeeper[tempRelOut->toIdEnt]->name, tempRelOut->rel->relName, entries);
    tempRelOut = nodeRelOut->next;
    free(nodeRelOut);
    nodeRelOut = NULL;
    nodeRelOut = tempRelOut;
  }
}

//prints the result
void ReportPrint() {
  int atLeastOne = 0;
  Report * currRep = headReport;
  NodeReport * pointerNode;
  while (currRep != NULL) {
    if (currRep->highest != NULL && currRep->highest->val != 0) {
      atLeastOne = 1;
      fputs(currRep->relName, stdout);
      printf(" ");
      pointerNode = currRep->highest->list;
      while (pointerNode != NULL) {
        fputs(idEntKeeper[pointerNode->inIdEnt]->name, stdout);
        printf(" ");
        pointerNode = pointerNode->next;
      }
      printf("%d; ", currRep->highest->val);
    }
    currRep = currRep->next;
  }
  if (atLeastOne == 0) {
    printf("none");
  }
  printf("\n");
}

int GetHashValueEnt(char * ent1) {
	unsigned int hashval = 0;
	char * i;

	for (i = ent1; *i != '\0'; i++)
		hashval = N * hashval + *i;

	return (hashval % HASH_SIZE);
}
//Insertion in sorted list
void AddNodeToList(NodeReport * node, NodeReport * pointerNode) {
  Ent * ent2 = idEntKeeper[node->inIdEnt];
  if (pointerNode == NULL || strcmp(ent2->name, idEntKeeper[pointerNode->inIdEnt]->name) < 0) { //head insertion
    if (pointerNode != NULL)
      pointerNode->prev = node;
    node->next = pointerNode;
    node->head->list = node;
    node->prev = NULL;
  }
  else {
    while (pointerNode->next != NULL && strcmp(ent2->name, idEntKeeper[pointerNode->inIdEnt]->name) >= 0)
      pointerNode = pointerNode->next;
    if (strcmp(ent2->name, idEntKeeper[pointerNode->inIdEnt]->name) >= 0) {
      node->prev = pointerNode;
      node->next = NULL;
      pointerNode->next = node;
    }
    else {
      pointerNode->prev->next = node;
      node->prev = pointerNode->prev;
      pointerNode->prev = node;
      node->next = pointerNode;
    }
  }
}

void DeleteNodeFromList(NodeReport * node) {
  if (node->prev == NULL)
    node->head->list = node->next;
  else
    node->prev->next = node->next;

  if (node->next != NULL)
    node->next->prev = node->prev;
}

//searches the entity in the hash, returns it if it exists, else NULL
Ent * FindEnt(char * ent1, Ent ** entries) {
  int hashvalue = GetHashValueEnt(ent1);
  Ent * current = entries[hashvalue];
  while (current != NULL) {
    if (strcmp(current->name, ent1) == 0)
      return current;
    else
      current = current->next;
  }
  return NULL;
}
