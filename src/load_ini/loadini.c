// loadini.c: implements the loadini functions for loading settings from a settings file.
//
// Author: tommojphillips
// GitHub: https:\\github.com\tommojphillips

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>

#include "loadini.h"

#define LOADINI_MAX_LINE_SIZE 128
#define LOADINI_DELIM "="

static void ltrim(char** str) {
	if (*str == NULL)
		return;
	while (**str == ' ' || **str == '\t' || **str == '\n') {
		(*str)++;
	}
}
static void rtrim(char** str) {
	if (*str == NULL)
		return;
	int len = strlen(*str);
	if (len == 0)
		return;
	char* end = *str + len - 1;
	while (end > *str && (*end == ' ' || *end == '\t' || *end == '\n')) {
		end--;
	}
	*(end + 1) = '\0';
}

static void set_str_value(char** setting, const char* value, uint32_t len) {
	*setting = (char*)malloc(len + 1);
	if (*setting != NULL) {
		strcpy_s(*setting, len, value);
	}
}
static void set_bool_value(bool* setting, const char* value) {
	if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0) {
		*setting = true;
	}
	else if (strcmp(value, "false") == 0 || strcmp(value, "0") == 0) {
		*setting = false;
	}
}
static void set_int_value(int* setting, const char* value) {
	if (strlen(value) > 2 && value[0] == '0' && (value[1] == 'x' || value[1] == 'X')) {
		*setting = strtol(value, NULL, 16);
	}
	else {
		*setting = strtol(value, NULL, 10);
	}
}
static void set_byte_value(uint8_t* setting, const char* value) {
	if (strlen(value) > 2 && value[0] == '0' && (value[1] == 'x' || value[1] == 'X')) {
		*setting = strtol(value, NULL, 16) % 256;
	}
	else {
		*setting = strtol(value, NULL, 10) % 256;
	}
}
static void set_float_value(float* setting, const char* value) {
	*setting = (float)atof(value);
}
static void set_double_value(double* setting, const char* value) {
	*setting = (double)atof(value);
}
static void set_flag_value(int* setting, const char* value) {

	int flag = 0;
	if (strlen(value) > 2 && value[0] == '0' && (value[1] == 'x' || value[1] == 'X')) {
		flag = strtol(value, NULL, 16);
	}
	else {
		flag = strtol(value, NULL, 10);
	}
	*setting |= flag;
}

static int parse_next_line(char* buf, const LOADINI_SETTING_MAP* settings_map, LOADINI_VAR_MAP* var_map, const uint32_t count) {
	uint32_t i = 0;
	uint32_t value_len = 0;
	char* key_ptr = NULL;
	char* value_ptr = NULL;
	char* next_token = NULL;

	char* line_ptr = buf;
	ltrim(&line_ptr);

	// line-format:	key=value
	//				key = value
	//				key = 'value'

	// comment check
	if (line_ptr[0] == ';')	return LOADINI_ERROR_SUCCESS;

	// get the key
	key_ptr = strtok_s(line_ptr, LOADINI_DELIM, &next_token);
	if (key_ptr == NULL) return LOADINI_ERROR_INVALID_KEY;
	ltrim(&key_ptr);
	rtrim(&key_ptr);

	// get the value
	value_ptr = strtok_s(NULL, LOADINI_DELIM, &next_token);
	if (value_ptr == NULL) return LOADINI_ERROR_INVALID_DATA;
	value_len = strlen(value_ptr);
	ltrim(&value_ptr);
	rtrim(&value_ptr);

	// quotes check
	if (value_ptr[0] == '\"' || value_ptr[0] == '\'') {
		value_ptr++;
		value_len = strlen(value_ptr);
	}
	if (value_ptr[value_len - 1] == '\"' || value_ptr[value_len - 1] == '\'') {
		value_ptr[value_len - 1] = '\0';
		value_len--;
	}

	// cmp key and set values.
	for (i = 0; i < count; ++i) {

		if (strcmp(key_ptr, settings_map[i].key) != 0) continue;

		switch (settings_map[i].type) {

			case LOADINI_SETTING_TYPE_BOOL:
				set_bool_value((bool*)var_map[i].var, value_ptr);
				break;
			case LOADINI_SETTING_TYPE_STR:
				set_str_value((char**)var_map[i].var, value_ptr, value_len);
				break;
			case LOADINI_SETTING_TYPE_CHAR:
				set_byte_value((uint8_t*)var_map[i].var, value_ptr);
				break;
			case LOADINI_SETTING_TYPE_INT:
				set_int_value((int*)var_map[i].var, value_ptr);
				break;
			case LOADINI_SETTING_TYPE_FLOAT:
				set_float_value((float*)var_map[i].var, value_ptr);
				break;
			case LOADINI_SETTING_TYPE_DOUBLE:
				set_double_value((double*)var_map[i].var, value_ptr);
				break;
		}
		break;
	}

	if (i == count) {
		return LOADINI_ERROR_INVALID_KEY;
	}

	return LOADINI_ERROR_SUCCESS;
}

int loadini_load_from_stream(FILE* stream, const LOADINI_SETTING_MAP* settings_map, LOADINI_VAR_MAP* var_map, const uint32_t count) {

	char buf[LOADINI_MAX_LINE_SIZE] = { 0 };
	while (fgets(buf, LOADINI_MAX_LINE_SIZE, stream) != NULL) {		
		parse_next_line(buf, settings_map, var_map, count);
	}

	return LOADINI_ERROR_SUCCESS;
}
void loadini_save_to_stream(FILE* stream, const LOADINI_SETTING_MAP* settings_map, LOADINI_VAR_MAP* var_map, const uint32_t count) {
	
	for (uint32_t i = 0; i < count; ++i) {
		switch (settings_map[i].type) {

			case LOADINI_SETTING_TYPE_BOOL:
				fprintf(stream, "%s = '%d'\n", settings_map[i].key, *(bool*)var_map[i].var);
				break;
			case LOADINI_SETTING_TYPE_CHAR:
				fprintf(stream, "%s = '%u'\n", settings_map[i].key, *(uint8_t*)var_map[i].var);
				break;
			case LOADINI_SETTING_TYPE_STR:
				fprintf(stream, "%s = '%s'\n", settings_map[i].key, *(char**)var_map[i].var);
				break;
			case LOADINI_SETTING_TYPE_INT:
				fprintf(stream, "%s = '%d'\n", settings_map[i].key, *(int*)var_map[i].var);
				break;
			case LOADINI_SETTING_TYPE_FLOAT:
				fprintf(stream, "%s = '%f'\n", settings_map[i].key, *(float*)var_map[i].var);
				break;
			case LOADINI_SETTING_TYPE_DOUBLE:
				fprintf(stream, "%s = '%lf'\n", settings_map[i].key, *(double*)var_map[i].var);
				break;
		}
	}
}
