#pragma once

#include "parson.h"


class JsonParser
{
public:
	JsonParser();
	~JsonParser();
	JSON_Value* InitJsonObject();

	JSON_Value* FileToValue(const char* fileName);

	JSON_Object* ValueToObject(JSON_Value* value) const;

	const char* ValueToString(const char* name) const;

	double JsonValToNumber(const char* name) const;

	bool JsonValToBool(const char* name) const;

	JsonParser GetChild(JSON_Value* parent, const char* name);

	bool ExistChild(JSON_Value* parent, const char* name);

	JSON_Status SerializeFile(JSON_Value* value, const char* fileName) const;

	JSON_Value* GetRootValue() const;

	JSON_Object* SetNewJsonNode(JSON_Object* parent, const char* nodeName) const;

	JSON_Status SetNewJsonString(JSON_Object* node, const char* name, const char* string) const;

	JSON_Status SetNewJsonNumber(JSON_Object* node, const char* name, double number) const;

	JSON_Status SetNewJsonBool(JSON_Object* node, const char* name, bool boolean) const;

	JsonParser SetChild(JSON_Value* parent, const char* name);


private:
	JSON_Value* rootValue;


};