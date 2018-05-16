
#ifndef _EDT_ConfigDataSet_H_
#define _EDT_ConfigDataSet_H_

#ifdef __cplusplus

extern "C" {

#endif

#ifdef __linux__
#define CONST const
#endif

/***************************************/
/* data types */
/***************************************/

/* A ConfigData item holds the data from a single line of the .cfg file */
struct _ConfigData {

	char *Name;
	char *Value;
	char *Comment;

} ;

typedef struct _ConfigData ConfigData;

/* The ConfigDataSet contains all the data values in one .cfg file */
struct _ConfigDataSet {
	
	char *Name;

	int nItems;
	int nItemsAllocated;

	unsigned char bIgnoreCase;

	ConfigData *pItems;

	char *comment;

	int index;
	ConfigData *pCurrent;

	char separator;

};

typedef struct _ConfigDataSet ConfigDataSet;

/***************************************/
/* access functions */
/***************************************/

void cfgitem_set_value(ConfigData *pItem, CONST char *Value);
void cfgitem_set_name(ConfigData *pItem, CONST char *Name);
void cfgitem_set_comment(ConfigData *pItem, CONST char *Comment);
void cfgitem_init(ConfigData *pItem, CONST char *Name, CONST char *Value, CONST char *Comment);
void cfgitem_set_all_values(ConfigData *pItem, CONST char *Name, CONST char *Value, CONST char *Comment);
void cfgitem_destroy(ConfigData *pItem);


void cfgset_init(ConfigDataSet * pSet);

void cfgset_reset(ConfigDataSet * pSet);

void cfgset_destroy(ConfigDataSet *pSet);

void cfgset_add_item(ConfigDataSet * pSet, CONST char * Name, CONST char *Value, CONST char *Comment);
int cfgset_lookup_index(ConfigDataSet * pSet, CONST char *Name);
ConfigData * cfgset_lookup_item(ConfigDataSet * pSet, CONST char *Name);
void cfgset_delete_item(ConfigDataSet * pSet, int n);
void cfgset_delete_named_item(ConfigDataSet * pSet, CONST char *Name);
void cfgset_set_values(ConfigDataSet * pSet, int nItem, CONST char *Name, 
					   CONST char *Value, CONST char *Comment);

int cfgset_get_int(ConfigDataSet * pSet, CONST char  *Name, int *pValue);
int cfgset_get_float(ConfigDataSet * pSet, CONST char  *Name, float *pValue);
int cfgset_get_double(ConfigDataSet * pSet, CONST char  *Name, double *pValue);
char *cfgset_lookup(ConfigDataSet * pSet, CONST char *Name);

int cfgset_set_string(ConfigDataSet * pSet, CONST char  *Name, CONST char *Value);
int cfgset_set_int(ConfigDataSet * pSet, CONST char  *Name, int iValue);
int cfgset_set_float(ConfigDataSet * pSet, CONST char  *Name, float dValue);
int cfgset_set_double(ConfigDataSet * pSet, CONST char  *Name, double dValue);


/* File access functions */

int cfgset_read_line(ConfigDataSet * pSet, char *line);

int cfgset_load_file(ConfigDataSet * pSet, FILE *f);

int cfgset_write_line(ConfigDataSet * pSet, FILE *f, int line);

int cfgset_save_file(ConfigDataSet * pSet, FILE *f);

int cfgset_load(ConfigDataSet * pSet, CONST char *Name);

int cfgset_save(ConfigDataSet * pSet, CONST char *Name);


#ifdef __cplusplus

}

#endif


#endif
