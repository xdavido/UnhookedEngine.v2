#include "Globals.h"
#include "JsonParser.h"



JsonParser::JsonParser()
{
	rootValue = nullptr;
}

JsonParser::~JsonParser()
{
}

JSON_Value* JsonParser::InitJsonObject()
{
	JSON_Value* object = json_value_init_object();
	rootValue = object;

	return object;
}


JSON_Value* JsonParser::FileToValue(const char* fileName)
{

	JSON_Value* user_data = json_parse_file(fileName);
	if (user_data == NULL) {
		user_data = json_value_init_object();
		json_serialize_to_file(user_data, fileName);
	}

	JSON_Value* config = json_parse_file(fileName);
	rootValue = config;

	return config;
}

JSON_Object* JsonParser::ValueToObject(JSON_Value* value) const
{
	JSON_Object* object = json_value_get_object(value);

	if (object == NULL)
	{
		LOG("Unable to retrieve the referenced object");
		return false;
	}
	return object;
}

const char* JsonParser::ValueToString(const char* name) const
{
	return json_object_get_string(ValueToObject(rootValue), name);
}

double JsonParser::JsonValToNumber(const char* name) const
{
	return json_object_get_number(ValueToObject(rootValue), name);
}

bool JsonParser::JsonValToBool(const char* name) const
{
	return json_object_get_boolean(ValueToObject(rootValue), name);
}

JsonParser JsonParser::GetChild(JSON_Value* parent, const char* name)
{
	JsonParser child;

	child.rootValue = json_object_get_value(ValueToObject(parent), name);

	return child;
}

bool JsonParser::ExistChild(JSON_Value* parent, const char* name)
{
	bool ret = true;

	ret = json_object_get_value(ValueToObject(parent), name);

	return ret;
}

JSON_Status JsonParser::SerializeFile(JSON_Value* value, const char* fileName) const
{
	return json_serialize_to_file_pretty(value, fileName);
}
JSON_Value* JsonParser::GetRootValue() const
{
	return rootValue;
}

JSON_Object* JsonParser::SetNewJsonNode(JSON_Object* parent, const char* nodeName) const
{
	json_object_set_value(parent, nodeName, json_value_init_object());;

	return json_object_get_object(ValueToObject(rootValue), nodeName);
}

JSON_Status JsonParser::SetNewJsonString(JSON_Object* node, const char* name, const char* string) const
{
	return json_object_set_string(node, name, string);
}

JSON_Status JsonParser::SetNewJsonNumber(JSON_Object* node, const char* name, double number) const
{
	return json_object_set_number(node, name, number);
}

JSON_Status JsonParser::SetNewJsonBool(JSON_Object* node, const char* name, bool boolean) const
{
	return json_object_set_boolean(node, name, boolean);
}

JsonParser JsonParser::SetChild(JSON_Value* parent, const char* name)
{
	json_object_set_value(ValueToObject(parent), name, json_value_init_object());

	return GetChild(parent, name);
}